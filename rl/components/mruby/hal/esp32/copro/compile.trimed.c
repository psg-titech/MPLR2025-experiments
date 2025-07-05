/*! @file
  @brief
  mruby bytecode executor.

  <pre>
  Copyright (C) 2024-2025 Go Suzuki and Institute of Science Tokyo Programming Systems Group.

  This is derived from mrubyc/mrubyc/src/vm.c
  The original copyright is below.
  Copyright (C) 2015- Kyushu Institute of Technology.
  Copyright (C) 2015- Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  Fetch mruby VM bytecodes, decode and execute.

  </pre>
*/

/***** Feature test switches ************************************************/
/***** System headers *******************************************************/
//@cond
#include "../../src/vm_config.h"
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32C6
#include "ulp_lp_core.h"
#else
#include "ulp_riscv.h"
#endif
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/misc.h"
//@endcond

/***** Local headers ********************************************************/
#include "../../../src/mrubyc.h"
#include "../../../src/opcode.h"
#include "../../../src/c_object.h"
#include "riscv_writer.h"
#include "value.copro.h"
#include "gc.copro.h"
#include "dbg.h"
#include "inst_skip.h"
#include "compile.h"
#include "compile.private.h"
#include "stack.h"
#include "mymem.h"

/***** Constat values *******************************************************/
#define CALL_MAXARGS 15		// 15 is CALL_MAXARGS in mruby
#define INST_BUF_SIZE 64 // 64 compressed instructions.

/***** Macros ***************************************************************/
#if defined(_MSVC_VER)
#define alloca(size) _malloca(size)
#endif

/***** Typedefs *************************************************************/
/***** Function prototypes **************************************************/
/***** Local variables ******************************************************/
/***** Global variables *****************************************************/
/***** Signal catching functions ********************************************/
/***** Local functions ******************************************************/

#define DBG_ALLOCATION_PROFILE 0
#if DBG_ALLOCATION_PROFILE
static int allocated_code = 0;
static void * mrbcopro_gc_alloc_code(size_t size, int special) {
  void * ret = mrbcopro_gc_alloc(size, special);
  allocated_code += size;
  dbg_mrbc_prof_printf("GC allocated = %d(delta: %d)\n", allocated_code, size);
  return ret;
}
static int allocated_prof = 0;
static void * mrbcopro_alloc_prof(struct VM * vm, size_t size) {
  void * ret = mrbcopro_alloc(vm, size);
  allocated_prof += size;
  dbg_mrbc_prof_printf("PROF allocated = %d(delta: %d)\n", allocated_prof, size);
  return ret;
}
#else
static void * mrbcopro_gc_alloc_code(size_t size, int special) {
  return mrbcopro_gc_alloc(size, special);
}
static void * mrbcopro_alloc_prof(struct VM * vm, size_t size) {
  return mrbcopro_alloc(vm, size);
}
#endif

/// @brief Create profiling structure.
/// @param vm The Virtual Machine
/// @return If the allocation is succeeded, returns non-null, otherwise null.
static mrbc_profile_basic_block * new_profiling(struct VM * vm, size_t regLen) {
  mrbc_profile_basic_block * ret = mrbcopro_alloc_prof(vm, sizeof(mrbc_profile_basic_block) + ((sizeof(uint16_t)*2) * regLen));
  if(ret == NULL) return NULL;
  memset(ret, 0, sizeof(mrbc_profile_basic_block) + ((sizeof(uint16_t)*2)  * regLen));
#if MRBC_COPRO_ENABLE_CODE_HEADER
  struct code_header * ch = mrbcopro_gc_alloc_code(sizeof(struct code_header), 0);
  if(ch == NULL)
    goto FREE_NULL;
  ret->allocatedHeader = ch;
  ch->tt = MRBC_COPRO_TT_CODE;
  ch->jmp_code = 0;
#endif
  ret->beginning_inst = vm->inst;
  ret->regLen = regLen;
  return ret;
#if MRBC_COPRO_ENABLE_CODE_HEADER
FREE_NULL:
#endif
  mrbcopro_free(vm, ret);
  return NULL;
}

/// @brief Search profiled basic block versions within the current function.
/// @param prof
/// @return If not found, returns NULL.
static mrbc_profile_basic_block * search_basic_block_within_function(mrbc_profile_profiler * prof) {
  mrbc_vm * vm = prof->vm;
  mrbc_profile_basic_block * bb = prof->stack.functions_current->blocks;
  while (bb)
  {
    if (bb->beginning_inst == vm->inst)
    {
      for (int i = 0; i < bb->regLen; ++i) {
        mrbc_copro_vtype prof_head_enterTypeProfile = mrbc_prof_get_entrance_type(bb, i);
        if ((prof_head_enterTypeProfile != mrbcopro_objman_type2coprotype(vm, &(prof->objMan), &(vm->cur_regs[i])))
              && (prof_head_enterTypeProfile != MRBC_COPRO_TT_EMPTY))
          goto continue_label;
      }
      return bb;
    }
  continue_label:
    bb = bb->next;
  }
  return NULL;
}

static int get_temporary_register(mrbc_profile_profiler * prof) {
  mrbc_profile_function_header * fh = prof->stack.functions_current;
  uint32_t freeMachineRegisters = fh->freeMachineRegisters & RISCV_NORMAL_CALLER_SAVE_REGISTERS;
  if(freeMachineRegisters == 0) return 0;
  return __builtin_ctz(freeMachineRegisters);
}

// placeholder for stack allocated variables.
static int try_allocate_register(mrbc_profile_profiler * prof, int regNum) {
  return 0;
}

int gen_load_immediate(mrbc_profile_profiler * prof, int reg, int value);
/// @brief Get allocation, if it is a constant, load immediates are written.
/// @param prof profiler
/// @param virtualRegNum virual register number
/// @return physical register number, otherwise -1.
int get_allocation(mrbc_profile_profiler * prof, int virtualRegNum) {
  char v = prof->regsinfo[virtualRegNum];
  int physnum = mrbc_function_header_allocation(profiler_currentfunction(prof), virtualRegNum);
  if(v == 0 || (v&1) == 1) { // no translation, aliased, or constant
    if(v == 1) { // constant
      if(gen_load_immediate(prof, physnum, prof->vm->cur_regs[virtualRegNum].i)) return -1;
      prof->regsinfo[virtualRegNum] = 0;
    }
    return physnum;
  } else
    return mrbc_function_header_allocation(profiler_currentfunction(prof), v >> 2);
}

#define RISCV_JAL_IMM_MASK RISCV_JTYPE(0, 0, 0xFFFFFFFF)
static void write_jmp(uint32_t * caller, void * callee, int rd) {
  if(caller != NULL && callee != NULL) {
    *caller = RISCV_JUMP_AND_LINK(rd, (size_t)callee - (size_t)caller);
    dbg_mrbc_prof_print_inst("%p: %p", caller, *caller);
  }
}

mrbc_send_inst_bufs_return_t send_inst_bufs(mrbc_profile_profiler * prof, mrbc_profile_basic_block * relavant_basic_block) {
  mrbc_send_inst_bufs_return_t ret = { NULL };
  if(prof->giveup == 1) {
    prof->giveup = 0;
    ret.buffer = (void *)1;
    goto RETURN;
  }
  size_t actualSize = mrbcopro_vector_bytes2(prof->buf);
#if MRBC_COPRO_ENABLE_CODE_HEADER
  if(actualSize <= sizeof(uint32_t)) {
    current_basic_block->allocatedHeader->jmp_to = 0;
    current_basic_block->allocatedHeader->jmp_code = ((uint32_t *)shrinked)[0];
    return &(current_basic_block->allocatedHeader->jmp_code);
  }
#endif
  void * copro_allocated = mrbcopro_gc_alloc_code(sizeof(CLASS_IDENTIFIER_TYPE) + actualSize, 0);
  if(copro_allocated == NULL) {
    mrbc_raise(prof->vm, NULL, "GC of Coprocessor is full.");
    return ret;
  }
  *((CLASS_IDENTIFIER_TYPE *)copro_allocated) = MRBC_COPRO_TT_CODE;
  copro_allocated += sizeof(CLASS_IDENTIFIER_TYPE);
  memcpy(copro_allocated, prof->buf.head, actualSize);

  prof->buf.cur = prof->buf.head;
#if MRBC_COPRO_ENABLE_CODE_HEADER
  relavant_basic_block->allocatedHeader->jmp_to = (uint16_t)(size_t)copro_allocated;
  relavant_basic_block->allocatedHeader->jmp_code = RISCV_JUMP_AND_LINK(0, (size_t)copro_allocated - (size_t)(&(relavant_basic_block->allocatedHeader->jmp_code)));
  // mrbc_printf("ALLOCATION main:%d copro:%d, %d\n", sizeof(mrbc_profile_basic_block) + ((sizeof(mrbc_vtype)*2 + sizeof(uint8_t)) * relavant_basic_block->regLen), sizeof(struct code_header), sizeof(uint16_t) + actualSize);
#else
  relavant_basic_block->allocatedCode = copro_allocated;
  write_jmp(prof->stack.previous_jmp, copro_allocated, prof->stack.previous_jmp_rd);
#endif
  ret.buffer = copro_allocated;
#if MRBC_PROF_DBG_ENABLE
  ret.length = actualSize;
#endif
  RETURN: return ret;
}

int gen_load_immediate(mrbc_profile_profiler * prof, int reg, int value) {
  uint32_t buf[2];
  int len = 0;
  if ((value >> 11) == 0 || (value >> 11) == -1) {
    if((value >> 5) == 0 || (value >> 5) == -1) {
      uint16_t * buf2 = (uint16_t *)buf;
      *buf2 = RISCV_C_LI(reg, value);
      dbg_mrbc_prof_print_inst_readable("c.li x%d, %d", reg, value);
      len = 2;
    } else {
      buf[0] = RISCV_ADD_IMM(reg, 0, value);
      dbg_mrbc_prof_print_inst_readable("addi x%d, x0, %d", reg, value);
      len = 4;
    }
  } else {
    if((value >> 11) & 1) {
      int hv = ((value >> 12) + 1) << 12;
      buf[0] = RISCV_LOAD_UPPER_IMM(reg, hv);
      dbg_mrbc_prof_print_inst_readable("lui x%d, %d", reg, hv);
      value = value - hv;
    } else {
      buf[0] = RISCV_LOAD_UPPER_IMM(reg, value);
      dbg_mrbc_prof_print_inst_readable("lui x%d, %d", reg, value & ~(0xFFF));
      value = value & 0xFFF;
      if(value == 0) { len = 4; goto end;}
    }
    // This branch is rarely used. (and the condition is a bit wrong)
    // if((value >> 5) == 0 || (value >> 5) == -1) {
    //   uint16_t * buf2 = (uint16_t *)&buf[1];
    //   *buf2 = RISCV_C_ADDI(reg, value);
    //   dbg_mrbc_prof_print_inst_readable("c.addi x%d, %d", reg, value);
    //   return 6;
    // } else {
      buf[1] = RISCV_ADD_IMM(reg, reg, value);
      dbg_mrbc_prof_print_inst_readable("addi x%d, x%d, %d", reg, reg, value);
      len = 8;
    // }
  }
  end:
  return mrbcopro_vector_append(prof->vm, &(prof->buf), len, (char *)buf);
}

/// @brief Generates move instruction
/// @param prof the profiler
/// @param dstVirtualReg a Ruby's register number of the destination
/// @param srcVirtualReg a Ruby's register number of the source
/// @return 1 if Out-Of-Memory, 0 if success
static int gen_move(mrbc_profile_profiler * prof, int dstVirtualReg, int srcVirtualReg) {
  if(try_allocate_register(prof, dstVirtualReg)) return 1;
  mrbc_profile_function_header * fh = profiler_currentfunction(prof);
  // do not translation because these have already been checked.
  int rd = mrbc_function_header_allocation(fh, dstVirtualReg), rs1 = mrbc_function_header_allocation(fh, srcVirtualReg);
  dbg_mrbc_prof_print_inst_readable("c.mv x%d, x%d", rd, rs1);
  prof->regsinfo[dstVirtualReg] = 0;
  prof->regsinfo[srcVirtualReg] = 0;
  return mrbcopro_vector_append16(prof->vm, &(prof->buf), RISCV_C_MOVE(rd, rs1));
}

/// @brief prof->regsInfo[dst] = val. If it has already aliased, the register aliasing is applied.
/// @param prof profiler
/// @param dst  destination virtual register.
/// @param val  the value assigned.
/// @return     1 if fail.
int override_register_information(mrbc_profile_profiler * prof, int dst, char val) {
  char * regsInfo = prof->regsinfo;
  if(regsInfo[dst] > 1) {
    int reg2 = regsInfo[dst] >> 2;
    if(regsInfo[dst] & 1) {// aliased!
      if(gen_move(prof, reg2, dst)) return 1;
    } else if((regsInfo[dst] ^ val) == 1) {
      if(gen_move(prof, dst, reg2)) return 1;
    } else
      regsInfo[reg2] = 0;
  }
  regsInfo[dst] = val;
  return 0;
}

static int write_constant(mrbc_profile_profiler * prof, int virtualReg, int physicalReg) {
  mrbc_value * v = &(prof->vm->cur_regs[virtualReg]);
  int vval = v->i;
  switch(v->tt) {
    case MRBC_TT_TRUE: vval = 1; break;
    case MRBC_TT_FALSE:
    case MRBC_TT_NIL:
    vval = 0; break;
    case MRBC_TT_SYMBOL:
    vval = v->sym_id;
    default: break;
  }
  return gen_load_immediate(prof, physicalReg, vval);
}

struct read_and_override_register_information_ret_t
read_and_override_register_information(mrbc_profile_profiler * prof, int reg, char val) {
  char * regsInfo = prof->regsinfo;
  struct read_and_override_register_information_ret_t ret = {};
  char regInfo = prof->regsinfo[reg];
  ret.dst = mrbc_function_header_allocation(profiler_currentfunction(prof), reg);
  ret.src = ret.dst;
  if(regInfo == 0) return ret;
  else if(regInfo & 1) {
    if(regInfo >> 1 == 0) {
      // do not perform constant propagation
      if(write_constant(prof, reg, ret.dst)) return (struct read_and_override_register_information_ret_t){.src = 0, .dst = 0};
    } else // aliased!
      if(override_register_information(prof, reg, val)) return (struct read_and_override_register_information_ret_t){.src = 0, .dst = 0};
  } else
    ret.src = mrbc_function_header_allocation(profiler_currentfunction(prof), regInfo >> 2);
  regsInfo[reg] = val;
  return ret;
}

static int apply_register_information(mrbc_profile_profiler * prof, int idx) {
  mrbc_profile_function_header * fh = profiler_currentfunction(prof);
  char * regInfo = prof->regsinfo;
  if(regInfo[idx] == 0) return 0;
  if(regInfo[idx] == 1)
    write_constant(prof, idx, mrbc_function_header_allocation(fh, idx));
  else if((regInfo[idx]&1) == 0) // translation.
    if(gen_move(prof, idx, regInfo[idx] >> 2)) return 1;
  return 0;
}

void set_prof_exit_types(mrbc_profile_profiler * prof, int len) {
  mrbc_profile_basic_block * bb = profiler_current(prof);
  mrbc_vm * vm = prof->vm;
  for(int i = 0; i < len; ++i)
    mrbc_prof_get_type(bb, i) = mrbcopro_objman_type2coprotype(vm, &(prof->objMan), &(vm->cur_regs[i]));
}

void set_prof_entrance_types(mrbc_profile_profiler * prof, int len) {
  mrbc_profile_basic_block * bb = profiler_current(prof);
  mrbc_vm * vm = prof->vm;
  for(int i = 0; i < len; ++i)
    mrbc_prof_get_entrance_type(bb, i) = mrbcopro_objman_type2coprotype(vm, &(prof->objMan), &(vm->cur_regs[i]));
}

int try_search_or_new_profiling(mrbc_profile_profiler * prof) {
  mrbc_profile_basic_block * bb = profiler_current(prof);
  mrbc_profile_basic_block * ret = search_basic_block_within_function(prof);
  memset(prof->regsinfo, 0, sizeof(char) * prof->regsinfo_length);
  if(ret) {
    profiler_current(prof) = ret;
    return 0;
  }
  ret = new_profiling(prof->vm, bb->regLen);
  if(ret == NULL) return 1;
  profiler_append_and_set_current(prof, ret);
  return 2;
}

static void * generate_osr_entry(mrbc_profile_profiler * prof) {
  uint32_t regs[33] = {0};
  int new_sp = main_to_copro(prof, regs);
  if(new_sp == 0)
  {
    mrbc_raise(prof->vm, NULL, "Stack (main -> copro) fail."); return NULL;
  }
  regs[32] &= RISCV_CALLEE_SAVE_REGISTERS;
  regs[32] |= 6;
  mrbc_profile_basic_block * bb = profiler_current(prof);
  mrbc_profile_function_header * fh = profiler_currentfunction(prof);
  void * head = (void *)prof->buf.cur;
  struct VM * vm = prof->vm;
  {
    mrbc_value * cur_regs = (*(prof->lastInst) == OP_ENTER) ? vm->callinfo_tail->cur_regs : vm->cur_regs;
    size_t right = (size_t)((*(prof->lastInst) == OP_ENTER) ? vm->cur_regs : &(vm->cur_regs[bb->regLen]));
    for(int i = 0; (size_t)(&(cur_regs[i])) < right; ++i) {
      uint32_t alloc_reg = mrbc_function_header_allocation(fh, i); // do not translate
      if(alloc_reg == 0 || cur_regs[i].tt == MRBC_TT_EMPTY) continue;
      regs[32] |= 1 << alloc_reg;
      uint32_t val = mrbcopro_interpreter_value_to_coprocessor_value(vm, &(cur_regs[i]), &(prof->objMan));
      dbg_mrbc_prof_printf("Register %d x%d: %s %p -> %p", i, alloc_reg, dbg_mrbc_to_string(cur_regs[i].tt), cur_regs[i].i, val);
      regs[alloc_reg] = val;
    }
    cur_regs = vm->cur_regs;
    if(*(prof->lastInst) == OP_ENTER) { // arguments.
      for(int i = 0; i < vm->callinfo_tail->n_args+1; ++i) {
        uint32_t alloc_reg = RISCV_ARGS_REGISTER(i);
        if(cur_regs[i].tt == MRBC_TT_EMPTY) continue;
        regs[32] |= 1 << alloc_reg;
        uint32_t val = mrbcopro_interpreter_value_to_coprocessor_value(vm, &(vm->cur_regs[i]), &(prof->objMan));
        dbg_mrbc_prof_printf("Register %d x%d: %s %p -> %p", i, alloc_reg, dbg_mrbc_to_string(vm->cur_regs[i].tt), vm->cur_regs[i].i, val);
        regs[alloc_reg] = val;
      }
    }
  }
  regs[2] = MRBC_COPRO_NATIVE_PTR_TO_PTR(new_sp);
  for(int i = 1; i < 32; ++i) { 
    if((regs[32] & (1 << i)) == 0) continue;
    gen_load_immediate(prof, i, regs[i]);
  }
  size_t size = (size_t)prof->buf.cur - (size_t)head;
  void * ret = mrbcopro_gc_alloc(size + sizeof(uint32_t), 3); // do not profile.
  memcpy(ret, head, size);
  uint32_t * writeto = (uint32_t *)((size_t)ret + size);
  uint32_t * frm = MRBC_COPRO_NATIVE_PTR_TO_PTR(writeto);
#if MRBC_COPRO_ENABLE_CODE_HEADER
  *writeto = RISCV_JUMP_AND_LINK(0, (size_t)(&(bb->allocatedHeader->jmp_code)) - (size_t)frm);
#else
  *writeto = RISCV_JUMP_AND_LINK(0, (size_t)(bb->allocatedCode) - (size_t)frm);
#endif
  dbg_mrbc_prof_print("register_set:");
  dbg_dump_riscv_code2(size + sizeof(uint32_t), ret);
  prof->buf.cur = head;
  return (void *)MRBC_COPRO_PTR_TO_NATIVE_PTR(ret);
}

int write_send_epilogue(mrbc_profile_profiler * prof, mrbc_copro_vtype ty);
extern uint32_t * ulp_mrbc_sp_bottom;
// volatile extern int ulp_stop;
extern void * ulp_entrypoint;
volatile extern uint16_t ulp_stopreason;
volatile extern uint16_t ulp_ack;
volatile extern void * ulp_required_object;
#define TRANSLATION_TABLE_SIZE 4
volatile extern struct {
  void * from;
  void * to;
} ulp_translation_table[TRANSLATION_TABLE_SIZE];
static int execute_on_ulp(mrbc_profile_profiler * prof) {
  mrbc_vm * vm = prof->vm;
  reenter:
  void * entry = generate_osr_entry(prof);
  if(entry == NULL) {
    mrbc_raise(prof->vm, NULL, "OSR Entry fail."); return 1;
  }
  ulp_entrypoint = entry;
  mrbcopro_globalman_send(vm, &(prof->globalMan), &(prof->objMan));
  memset((void *)ulp_translation_table, 0, sizeof(ulp_translation_table));
  dbg_entering();
  ulp_stopreason = 0;
  ulp_ack = 0;
  esp_sleep_enable_ulp_wakeup();
#if CONFIG_IDF_TARGET_ESP32C6
  ulp_lp_core_cfg_t cfg  = {
    .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    .lp_timer_sleep_duration_us = 0
  };
  ulp_lp_core_run(&cfg);
#else
  ulp_riscv_run();
#endif
  while(1) {
    esp_light_sleep_start();
    while(ulp_stopreason == 10){}
    dbg_mrbc_prof_printf("Wake up. stopreason=%d", ulp_stopreason);
    switch(ulp_stopreason) {
      case 0: ulp_ack=1;  goto break_while;
      case 1: { // data transfer required.
        static int transtable_itr = 0;
        RObjectPtrCopro c = mrbcopro_objman_to(vm, &(prof->objMan), (void *)ulp_required_object);
        if(c == (RObjectPtrCopro)NULL) {
          dbg_mrbc_prof_printf("CRITICAL FAILURE %x", (size_t)ulp_required_object);
          while(1) vTaskDelay(100);
        }
        ulp_translation_table[transtable_itr].from = (void *)ulp_required_object;
        ulp_translation_table[transtable_itr].to = (void *)c;
        dbg_mrbc_prof_printf("TRANSLATION[%d] REGISTERD: %x -> %x", transtable_itr, ulp_required_object, c);
        transtable_itr = (transtable_itr + 1) % TRANSLATION_TABLE_SIZE;
        break;
      }
      case 2: // data dump
        dbg_mrbc_prof_printf("data dump: %x", (size_t)ulp_required_object);
        break;
    }
#if MRBC_PROF_DBG_ENABLE // to flash out debug output and for human.
    vTaskDelay(10);
#endif
    ulp_ack=1;
  }
  break_while:
  // uint32_t * sp = (uint32_t *)MRBC_COPRO_PTR_TO_NATIVE_PTR((size_t)ulp_mrbc_sp_bottom);
  // for(int i = 0; i < 32; ++i) {
  //   dbg_mrbc_prof_printf("Stack [%d]: %p %p", i, *sp, sp[1]);
  //   sp += 2;
  // }
  // sp = (uint32_t *)MRBC_COPRO_PTR_TO_NATIVE_PTR((size_t)ulp_mrbc_sp_bottom);

  mrbc_copro_vtype x11;
  mrbcopro_objman_receive(vm, &(prof->objMan));
  mrbcopro_globalman_receive(vm, &(prof->globalMan), &(prof->objMan));
  if(copro_to_main(prof, &x11))
  {
    mrbc_raise(prof->vm, NULL, "Stack (copro -> main) fail."); return 1;
  }
  mrbcopro_objman_cleanup(vm, &(prof->objMan));

  prof->lastInst = vm->inst;
  uint8_t op = *(vm->inst++);
  FETCH_BS();
  int setregLen = 0;
  switch(op) {
    case OP_JMPIF:
      if( vm->cur_regs[a].tt > MRBC_TT_FALSE )
        vm->inst += (int16_t)b;
      prof->stack.previous_jmp = profiler_current(prof)->failInstPtr;
      prof->stack.previous_jmp_rd = 0;
      break;
    case OP_JMPNOT:
      if( vm->cur_regs[a].tt <= MRBC_TT_FALSE )
        vm->inst += (int16_t)b;
      prof->stack.previous_jmp = profiler_current(prof)->failInstPtr;
      prof->stack.previous_jmp_rd = 0;
      break;
    case OP_GETUPVAR: case OP_GETGV: case OP_GETIV:
      write_send_epilogue(prof, x11);
      vm->inst += inst_skipper_inst_len[op] - 3;
      break;
    case OP_SSEND...OP_SENDB: case OP_ADD...OP_GE:
    case OP_GETIDX: case OP_SETIDX:
      write_send_epilogue(prof, x11);
      vm->inst += inst_skipper_inst_len[op] - 3;
      setregLen = a+1;
      break;
    // OP_GETMCNST, OP_GETCONST, OP_STRING must not lead to deoptimization!!
    default: dbg_mrbc_prof_printf("inst=%d", op); mrbc_raise(vm, NULL, "Something wrong."); return 1;
  }
  int ret = try_search_or_new_profiling(prof);
  if(ret == 1) return 1;
  if(ret == 0) {
    write_jmp(prof->stack.previous_jmp, profiler_current(prof)->allocatedCode, 0);
    goto reenter;
  }
  set_prof_entrance_types(prof, setregLen);
  return 0;
}

int write_jmp_and_execute_on_ulp(mrbc_profile_profiler * prof) {
  if(profiler_current(prof)->allocatedCode == NULL) return 0;
  write_jmp(prof->stack.previous_jmp, profiler_current(prof)->allocatedCode, prof->stack.previous_jmp_rd);
  prof->stack.previous_jmp = NULL;
  return execute_on_ulp(prof);
}

#define ASSERTION_CODE_SIZE 0x10
static void write_type_assertion(uint8_t writer[ASSERTION_CODE_SIZE], mrbc_copro_vtype assert_type)
{
  // 0x0: c.addi x11, -assert_type
  // 0x2: c.bnez x11, 0x6 -> jump to 0x8
  // 0x4: jal ###
  // 0x8: c.addi x11, assert_type
  // 0xa: jal ###
  // 0x10:
  uint16_t * w16 = (uint16_t *)writer;
  w16[0] = RISCV_C_ADDI(11, (uint32_t)(-(int32_t)assert_type));
  w16[1] = RISCV_C_BNEZ(11, 6);
  write_jmp_to_fallback((uint32_t *)(&w16[2]));
  w16[4] = RISCV_C_ADDI(11, assert_type);
  write_jmp_to_fallback((uint32_t *)&w16[5]);
  dbg_dump_riscv_code2(ASSERTION_CODE_SIZE, (void *)writer);
}

int write_send_epilogue(mrbc_profile_profiler * prof, mrbc_copro_vtype ty) {
  uint8_t * buf;
  if(profiler_current(prof)->failInstPtr == NULL) return 0;
  if((int)(profiler_current(prof)->asserts) == 2) {
    return 0;
  } else if((int)(profiler_current(prof)->asserts) == 1) {
    buf = (uint8_t *)profiler_current(prof)->failInstPtr;
    profiler_current(prof)->asserts = NULL;
  } else {
    struct profile_assertion_block * ab = (struct profile_assertion_block *)mrbcopro_alloc_prof(prof->vm, sizeof(struct profile_assertion_block));
    if(ab == NULL) return 1;
    struct code_body * copro_allocated = mrbcopro_gc_alloc_code(sizeof(uint16_t) + ASSERTION_CODE_SIZE, 0);
    if(copro_allocated == NULL) {
      mrbc_raise(prof->vm, NULL, "GC of Coprocessor is full.");
      return 1;
    }
    copro_allocated->tt = MRBC_COPRO_TT_CODE;
    ab->allocatedCode = copro_allocated;
    buf = (uint8_t *)copro_allocated->data;
    ab->next = profiler_current(prof)->asserts;
    profiler_current(prof)->asserts = ab;
    write_jmp(profiler_current(prof)->failInstPtr, copro_allocated->data, 0);
  }
  write_type_assertion(buf, ty);
  profiler_current(prof)->failInstPtr = (uint32_t * )&buf[0xA];
  prof->stack.previous_jmp_rd = 0;
  prof->stack.previous_jmp = (uint32_t *)&buf[0x4];
  return 0;
}

static int gen_lw_and_typecheck(mrbc_profile_profiler * prof, int fromReg, int offset, int toVMReg) {
  struct VM * vm = prof->vm;
  try_allocate_register(prof, toVMReg);
  mrbc_profile_basic_block * bb = profiler_current(prof);
  mrbc_profile_function_header * fh = profiler_currentfunction(prof);
  int toReg = mrbc_function_header_allocation(fh, toVMReg);
  override_register_information(prof, toVMReg, 0);
  if(fromReg == 0)
    // load_immediate to x11.
    gen_load_immediate(prof, 11, offset);
  else if(offset == 0) {
    if(fromReg != 11)
    {
      mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_MOVE(11, fromReg));
      dbg_mrbc_prof_print_inst_readable("c.mov x11, x%d", fromReg);
    }
  } else {
    mrbcopro_vector_append32(vm, &(prof->buf), RISCV_ADD_IMM(11, fromReg, offset));
    dbg_mrbc_prof_print_inst_readable("addi x11, x%d, %d", fromReg, offset);
  }
  for(int i = 0; i < bb->regLen; ++i) 
    apply_register_information(prof, i);
  set_prof_exit_types(prof, bb->regLen);
  int size = mrbcopro_vector_bytes2(prof->buf);
  mrbcopro_vector_extend(vm, &(prof->buf), ASSERTION_CODE_SIZE + sizeof(uint32_t) + sizeof(uint16_t));
  prof->buf.cur = (char *)((size_t)(prof->buf.cur) + ASSERTION_CODE_SIZE + sizeof(uint32_t));
  mrbc_copro_vtype desired_type = mrbc_prof_get_type(profiler_current(prof), toVMReg);
  mrbc_prof_get_type(profiler_current(prof), toVMReg) = MRBC_COPRO_TT_UNKNOWN;
  mrbc_send_inst_bufs_return_t ret = send_inst_bufs(prof, bb);
  if((size_t)ret.buffer <= 1) {
    prof->stack.previous_jmp = NULL;
    prof->stack.previous_jmp_rd = 0;
    return 1;
  }
  uint32_t * callsite = (uint32_t *)((int)ret.buffer + size);
  *callsite = RISCV_JUMP_AND_LINK(1, (int)ulp_read_barrier - (int)callsite);
  dbg_mrbc_prof_print_inst_readable("jal ra, ulp_read_barrier", fromReg, offset);
  callsite++;
  uint16_t * callsite2 = (uint16_t *)callsite;
  *callsite2 = RISCV_C_MOVE(toReg, 3);
  callsite2++;
  bb->asserts = (struct profile_assertion_block *)1;
  bb->failInstPtr = (uint32_t *)callsite2; 
  write_send_epilogue(prof, desired_type);
#if MRBC_COPRO_ENABLE_CODE_HEADER
  dbg_dump_riscv_code2(sizeof(uint32_t), &(bb->allocatedHeader->jmp_code));
#endif
  dbg_dump_riscv_code2(ret.length, ret.buffer);
  bb->lastInstMRubyPtr = prof->lastInst;
  int ret2 = try_search_or_new_profiling(prof);
  if(ret2 == 0) return write_jmp_and_execute_on_ulp(prof);
  if(ret2 == 1) return 1;
  set_prof_entrance_types(prof, bb->regLen);
  return 0;
}
static void gen_sw(mrbc_profile_profiler * prof, int valReg, int dstAddrReg, int offset, mrbc_copro_vtype the_type) {
  int delta = 0, tmpReg = 0;
  if(the_type == MRBC_COPRO_TT_INTEGER)
    delta = 1;
  else if(the_type == MRBC_COPRO_TT_BOOL)
    delta = 2;
  if(delta > 0) {
    int prevValReg = valReg;
    if(dstAddrReg == 11) {
      valReg = get_temporary_register(prof);
      dbg_mrbc_prof_printf("get_temporary_register returns: %d", valReg);
      if(valReg == 0) {
        mrbcopro_vector_append16(prof->vm, &(prof->buf),RISCV_C_ADDI16SP(-16));
        dbg_mrbc_prof_print_inst_readable("c.addi16sp x2, -16", 0);
        mrbcopro_vector_append16(prof->vm, &(prof->buf),RISCV_C_SWSP(10, 0));
        dbg_mrbc_prof_print_inst_readable("c.swsp x10, 0", 0);
        valReg = 10; // even if prevValReg == 10, it does not matter.
        tmpReg = 1;
      }
    } else
      valReg = 11;
    mrbcopro_vector_append32(prof->vm, &(prof->buf), RISCV_SHIFT_LEFT_IMM(valReg, prevValReg, delta));
    dbg_mrbc_prof_print_inst_readable("slli x%d, x%d, %d", valReg, prevValReg, delta);
    mrbcopro_vector_append16(prof->vm, &(prof->buf), RISCV_C_ADDI(valReg, delta));// actually, pow(2, delta-1)
    dbg_mrbc_prof_print_inst_readable("c.addi x%d, %d", valReg, delta);
  }
  // if(the_type == MRBC_TT_INTEGER)
  // {
  //   mrbcopro_vector_append32(prof->vm, &(prof->buf), RISCV_SHIFT_LEFT_IMM(11, fromReg, 1));
  //   dbg_mrbc_prof_print_inst_readable("slli x11, x%d, 1", fromReg);
  //   mrbcopro_vector_append16(prof->vm, &(prof->buf), RISCV_C_ADDI(11, 1));
  //   dbg_mrbc_prof_print_inst_readable("c.addi x11, 1", 0);
  //   fromReg = 11;
  // } else if(the_type == MRBC_TT_TRUE || the_type == MRBC_TT_FALSE) {
  //   mrbcopro_vector_append32(prof->vm, &(prof->buf), RISCV_SHIFT_LEFT_IMM(11, fromReg, 2));
  //   dbg_mrbc_prof_print_inst_readable("slli x11, x%d, 2", fromReg);
  //   mrbcopro_vector_append16(prof->vm, &(prof->buf), RISCV_C_ADDI(11, 2));
  //   dbg_mrbc_prof_print_inst_readable("c.addi x11, 2", 0);
  //   fromReg = 11;
  // }
// #if CONFIG_IDF_TARGET_ESP32S3
//   mrbcopro_vector_append32(prof->vm, &(prof->buf), RISCV_STORE_HALF(fromReg, toReg, offset));
//   dbg_mrbc_prof_print_inst_readable("sh x%d, x%d(%d)", fromReg, toReg, offset);
// #else
  mrbcopro_vector_append32(prof->vm, &(prof->buf), RISCV_STORE_WORD(dstAddrReg, valReg, offset));
  dbg_mrbc_prof_print_inst_readable("sw x%d, x%d(%d)", valReg, dstAddrReg, offset);
// #endif
  if(tmpReg) {
    mrbcopro_vector_append16(prof->vm, &(prof->buf),RISCV_C_LWSP(10, 0));
    dbg_mrbc_prof_print_inst_readable("c.lwsp x10, 0", 0);
    mrbcopro_vector_append16(prof->vm, &(prof->buf),RISCV_C_ADDI16SP(16));
    dbg_mrbc_prof_print_inst_readable("c.addi16sp x2, 16", 0);
  }
}

struct register_saving_result_t {
  mrbc_send_inst_bufs_return_t sibret;
  uint32_t * callsite; // The address of the callsite. It should be filled with "jal x1, foo".
  uint32_t * epilogue_jmp; // The epilogue of the callsite. It should be filled with "jal x0, foo" or type assertion.
};

struct register_saving_analysis_result_t
register_saving(mrbc_profile_profiler * prof, int a, int narg) {
  struct register_saving_analysis_result_t analysis; 
  analysis.saved_registers = analyze_register_saving(profiler_currentfunction(prof), a, narg);
  analysis.spMoved = estimate_stack_move_size(__builtin_popcount(analysis.saved_registers));
  // apply aliasing information
  for(int i = 1; i < a; ++i)
    apply_register_information(prof, i);
  struct VM * vm = prof->vm; 
  uint8_t located[32] = { 0 };
  if(analysis.saved_registers == 0) goto arguments_move;
  
  mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_ADDI16SP(-analysis.spMoved));
  dbg_mrbc_prof_print_inst_readable("c.addi16sp %d", -analysis.spMoved);
  for(int loc_cur = 0, i = 1; i < 32; ++i) {
    if((analysis.saved_registers & (1 << i)) == 0) continue;
    located[i] = loc_cur;
    mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_SWSP(i, loc_cur));
    dbg_mrbc_prof_print_inst_readable("c.swsp x%d, %d", i, loc_cur);
    loc_cur += 4;
  }

  arguments_move:
  for(int i = 0, maxRegNum = a + narg, regNum = a; regNum <= maxRegNum; ++regNum, ++i) { // narg must be less than 8
    int dst = RISCV_ARGS_REGISTER(i);
    if(prof->regsinfo[regNum] == 1) { // insted, write a constant.
      write_constant(prof, regNum, dst);
      continue;
    }
    int allocatedReg = get_allocation(prof, regNum);
    if(allocatedReg == 0) continue;
    if(allocatedReg < dst || allocatedReg > (10+narg)) {
      mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_MOVE(dst, allocatedReg));
      dbg_mrbc_prof_print_inst_readable("c.move x%d, x%d", dst, allocatedReg);
    } else if(allocatedReg != dst) {
      mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_LWSP(dst, located[allocatedReg]));
      dbg_mrbc_prof_print_inst_readable("c.lwsp x%d, %d", dst, located[allocatedReg]);
    }
  }
  return analysis;
}

int register_restoring(mrbc_profile_profiler * prof, struct register_saving_analysis_result_t analysis, int a) {
  mrbc_profile_function_header * fh = profiler_currentfunction(prof);
  try_allocate_register(prof, a);
  int allocReg = mrbc_function_header_allocation(fh, a);
  struct VM * vm = prof->vm; 
  mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_MOVE(allocReg, 10));
  dbg_mrbc_prof_print_inst_readable("c.mov x%d, x10(a0)", allocReg);

  if(analysis.saved_registers == 0) goto jmp;
  for(int loc_cur = 0, i = 1; i < 32; ++i) {
    if((analysis.saved_registers & (1 << i)) == 0) continue;
    mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_LWSP(i, loc_cur));
    dbg_mrbc_prof_print_inst_readable("c.lwsp x%d, %d", i, loc_cur);
    loc_cur += 4;
  }
  mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_ADDI16SP(analysis.spMoved));
  dbg_mrbc_prof_print_inst_readable("c.addi16sp %d", analysis.spMoved);

  jmp:
  return 0;
}

// defined in c_object.c
void c_object_getiv(struct VM *vm, mrbc_value v[], int argc);
void c_object_setiv(struct VM *vm, mrbc_value v[], int argc);
static int gen_getiv(struct VM *vm, mrbc_sym sym_id, mrbc_profile_profiler * prof, int a, mrbc_class * cls) {
  int offset = mrbcopro_objman_get_fieldoffset(vm, mrbcopro_objman_get_obj_info(vm, &(prof->objMan), cls), sym_id);
  mrbc_profile_function_header * fh = profiler_currentfunction(prof);
  struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a, 0);
  return gen_lw_and_typecheck(prof, regA.src, offset, a);
}
static int gen_setiv(struct VM *vm, mrbc_sym _sym_id, mrbc_profile_profiler * prof, int a, mrbc_class * cls) {
  static const int NAMEBUFSIZ = 16;
  char namebuf_auto[NAMEBUFSIZ];
  char const * name = mrbc_irep_symbol_cstr(vm->cur_irep, _sym_id);
  int len = strlen(name);
  char * namebuf;
  if( NAMEBUFSIZ < len ) {
    namebuf = mrbcopro_alloc(vm, len);
    if( !namebuf ) return 1;
  } else {
    namebuf = namebuf_auto;
  }
  memcpy( namebuf, name, len-1 );
  namebuf[len-1] = '\0';	// delete '='
  mrbc_sym sym_id = mrbc_str_to_symid(namebuf);
  if( NAMEBUFSIZ < len ) {
    mrbcopro_free(vm, namebuf);
  }
  int offset = mrbcopro_objman_get_fieldoffset(vm, mrbcopro_objman_get_obj_info(vm, &(prof->objMan), cls), sym_id);
  gen_sw(prof, get_allocation(prof, a), mrbc_function_header_allocation(profiler_currentfunction(prof), 0), offset, mrbcopro_objman_type2coprotype(vm, &(prof->objMan), &(vm->cur_regs[a])));
  return 0;
}

void gen_native_call(mrbc_profile_profiler * prof, int a, int narg, void * function_addr, int no_typecheck, uint32_t ** callsite) {
  struct VM * vm = prof->vm;
  struct register_saving_analysis_result_t anresult = register_saving(prof, a, narg);
  size_t calladdr =  mrbcopro_vector_bytes2(prof->buf);
  mrbcopro_vector_append_without_fill(vm, &(prof->buf), sizeof(uint32_t)); // jal ra, method.copro_func
  register_restoring(prof, anresult, a);
  size_t epilogueaddr = mrbcopro_vector_bytes2(prof->buf);
  mrbcopro_vector_append_without_fill(vm, &(prof->buf), no_typecheck ? CODE_BYTES_WRITE_JMP_TO_FALLBACK : ASSERTION_CODE_SIZE);
  mrbc_send_inst_bufs_return_t sib = send_inst_bufs(prof, profiler_current(prof));  
  prof->stack.previous_jmp_rd = 0;
  if((size_t)sib.buffer <= 1) {
    prof->stack.previous_jmp = NULL;
    return;
  }
  // Write a jal instruction.
  uint32_t * writer = (uint32_t *)((size_t)sib.buffer + calladdr);
  *callsite = writer;
  *(writer) = RISCV_JUMP_AND_LINK(1, (size_t)function_addr - (size_t)writer);
  writer = (uint32_t *)((size_t)sib.buffer + epilogueaddr);
  write_jmp_to_fallback(writer);
#if MRBC_COPRO_ENABLE_CODE_HEADER
  dbg_dump_riscv_code2(sizeof(uint32_t), &(bb->allocatedHeader->jmp_code));
#endif
  dbg_dump_riscv_code2(sib.length, sib.buffer);
  struct profile_basic_block * b = profiler_current(prof);
  if(no_typecheck) {
    b->asserts = (struct profile_assertion_block *)2;
  } else {
    b->failInstPtr = writer;
    b->returnPos = MRBC_COPRO_NATIVE_PTR_TO_PTR2(&((uint32_t *)((size_t)sib.buffer + calladdr))[1]);
    b->asserts = (struct profile_assertion_block *)1;
    set_prof_exit_types(prof, a);
    mrbc_prof_get_type(b, a) = MRBC_COPRO_TT_UNKNOWN;
    profiler_current(prof)->lastInstMRubyPtr = prof->lastInst;
  }
  prof->stack.previous_jmp = writer;
}

//================================================================
/*! Method call by method name's id

  @param  vm		pointer to VM.
  @param  sym_id	method name symbol id
  @param  a		operand a
  @param  c		bit: 0-3=narg, 4-7=karg, 8=have block param flag.
  @param  prof  Current profiling
  @param  ra_is_r0 Copies R0 to RA if true.
*/
static void send_by_name( struct VM *vm, mrbc_sym sym_id, int a, int c, mrbc_profile_profiler * prof)
{
  int narg = c & 0x0f;
  int karg = (c >> 4) & 0x0f;
  mrbc_value *recv = vm->cur_regs + a;

  // If it's packed in an array, expand it.
  // TODO: array-packed arguments
  if( narg == CALL_MAXARGS ) {
    mrbc_value argv = recv[1];
    narg = mrbc_array_size(&argv);
    for( int i = 0; i < narg; i++ ) {
      mrbc_incref( &argv.array->data[i] );
    }

    memmove( recv + narg + 1, recv + 2, sizeof(mrbc_value) * (karg * 2 + 1) );
    memcpy( recv + 1, argv.array->data, sizeof(mrbc_value) * narg );

    mrbc_decref(&argv);
  }

  // Convert keyword argument to hash.
  // TODO: keyword arguents
  if( karg ) {
    narg++;
    if( karg != CALL_MAXARGS ) {
      mrbc_value h = mrbc_hash_new( vm, karg );
      if( !h.hash ) return;	// ENOMEM

      mrbc_value *r1 = recv + narg;
      memcpy( h.hash->data, r1, sizeof(mrbc_value) * karg * 2 );
      h.hash->n_stored = karg * 2;

      mrbc_value block = r1[karg * 2];
      memset( r1 + 2, 0, sizeof(mrbc_value) * (karg * 2 - 1) );
      *r1++ = h;
      *r1 = block;
    }
  }

  // is not have block
  if( (c >> 8) == 0 ) {
    mrbc_decref( recv + narg + 1 );
    mrbc_set_nil( recv + narg + 1 );
  }

  // find a method
  mrbc_class *cls = find_class_by_object(recv);
  mrbc_method method;
  if( mrbc_find_method( &method, cls, sym_id ) != 0 ) goto CALL_METHOD;
  // method missing?
  if( mrbc_find_method( &method, cls, MRBC_SYM(method_missing) ) == 0 ) {
    // TODO: NoMethodError
    mrbc_raisef(vm, MRBC_CLASS(NoMethodError),
      "undefined local variable or method '%s' for %s",
      mrbc_symid_to_str(sym_id), mrbc_symid_to_str(cls->sym_id));

    if( vm->callinfo_tail != 0 ) {
      vm->exception.exception->method_id = vm->callinfo_tail->method_id;
    }
    return;
  }
  // prepare to call 'method_missing' method.
  for( int i = narg+1; i != 0; i-- ) {	// shift arguments
    recv[i+1] = recv[i];
  }
  recv[1] = mrbc_symbol_value(sym_id);
  sym_id = MRBC_SYM(method_missing);
  narg++;

CALL_METHOD:
  if( !method.c_func ) goto CALL_RUBY_METHOD;

  int ret = 1;
  if(prof) {
    profiler_current(prof)->asserts = (struct profile_assertion_block *)2; // do not insert type assertions.
    // must be executed before.
    if(MRBC_COPRO_IS_IN_COPRO(method.copro_func)) {
      uint32_t * _;
      gen_native_call(prof, a, narg, (void *)((size_t)method.copro_func & ~1), (size_t)method.copro_func & 1, &_);
    } else if(method.copro_func != NULL) {
      ret = ((inline_code_gen_t)method.copro_func)(prof, &method, a, recv, narg);
      if(ret == 1) return; // TODO: Error Handling
    }
    method.func(vm, recv, narg);

    if( mrbc_israised(vm) && vm->exception.exception->method_id == 0 ) {
      vm->exception.exception->method_id = sym_id;
    }

    if(method.copro_func == NULL){ // AFTER
      if(method.func == c_object_getiv)
        gen_getiv(vm, sym_id, prof, a, cls);
      else if(method.func == c_object_setiv)
        gen_setiv(vm, sym_id, prof, a, cls);
      else {
        dbg_mrbc_prof_print("NO method found!");
        // TODO: Can't compile!
      }
    } else if(MRBC_COPRO_IS_IN_COPRO(method.copro_func)) {
      if((size_t)method.copro_func & 1)
        set_prof_exit_types(prof, a+1);
      L_try_search_or_new_prof:
      ret = try_search_or_new_profiling(prof);
      if(ret == 0) {write_jmp_and_execute_on_ulp(prof); return;}
      if (ret == 1) return;
    } else if(ret == 2) goto L_try_search_or_new_prof;
  } else {
    method.func(vm, recv, narg);

    if( mrbc_israised(vm) && vm->exception.exception->method_id == 0 ) {
      vm->exception.exception->method_id = sym_id;
    }
  }

  if(sym_id == MRBC_SYM(call) || sym_id == MRBC_SYM(new)) return;
  for( int i = 1; i <= narg+1; i++ ) {
    mrbc_decref_empty( recv + i );
  }
  
  if(prof) {
    if(ret == 0) write_jmp_and_execute_on_ulp(prof);
    else set_prof_entrance_types(prof, a+1);
  }
  return;

CALL_RUBY_METHOD:
  {
    uint32_t * callsite = NULL;
    gen_native_call(prof, a, narg, NULL, 0, &callsite);

    // call Ruby method.
    mrbc_callinfo *callinfo = mrbc_push_callinfo(vm, sym_id, a, narg);
    callinfo->own_class = method.cls;

    vm->cur_irep = method.irep;
    vm->inst = vm->cur_irep->inst;
    vm->cur_regs = recv;

    if(profiler_push_stack(prof)) return; // TODO: Error Handling
    if(callsite != NULL) {
      prof->stack.previous_jmp = callsite;
      prof->stack.previous_jmp_rd = 1;
    }
  }
}


//================================================================
/*! Find ensure catch handler
*/
static const mrbc_irep_catch_handler *find_catch_handler_ensure( const struct VM *vm )
{
  const mrbc_irep *irep = vm->cur_irep;
  int cnt = irep->clen;
  if( cnt == 0 ) return NULL;

  const mrbc_irep_catch_handler *catch_table =
    (const mrbc_irep_catch_handler *)(irep->inst + irep->ilen);
  uint32_t inst = vm->inst - irep->inst;

  for( cnt--; cnt >= 0 ; cnt-- ) {
    const mrbc_irep_catch_handler *handler = catch_table + cnt;
    // Catch type and range check
    if( (handler->type == 1) &&		// 1=CATCH_FILTER_ENSURE
	(bin_to_uint32(handler->begin) < inst) &&
	(inst <= bin_to_uint32(handler->end)) ) {
      return handler;
    }
  }

  return NULL;
}

/***** opecode functions ****************************************************/
#if defined(MRBC_SUPPORT_OP_EXT)
#define EXT , int ext
#else
#define EXT
#endif
//================================================================
/*! OP_NOP

  No operation
*/
static inline void op_nop( mrbc_vm *vm, mrbc_value *regs EXT ) { }

//================================================================
static int registers_move( mrbc_profile_profiler * prof, int dst, int src) {
  if (prof->regsinfo[src] == 1) // if constant, dst will be a constant.
    return override_register_information(prof, dst, 1);
  return override_register_information(prof, src, (dst << 2) + 3)
  || override_register_information(prof, dst, (src << 2) + 2);
} 

/*! OP_MOVE

  R[a] = R[b]
*/
#define OP_MOVE_DEBUG_OUT 0
static inline void op_move( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BB();

  mrbc_incref(&regs[b]);
  mrbc_decref(&regs[a]);
  regs[a] = regs[b];
#if OP_MOVE_DEBUG_OUT
  int relate1 = prof->regsinfo[a] >> 2, relate2 = prof->regsinfo[b] >> 2;
  mrbc_profile_function_header * fh = profiler_currentfunction(prof);
  dbg_mrbc_prof_printf("R%d(x%d %d %d) <- R%d(x%d %d %d) BEFORE", a, mrbc_function_header_allocation(fh, a), relate1, prof->regsinfo[a] & 3,  b, mrbc_function_header_allocation(fh, b), relate2, prof->regsinfo[b] & 3);
  if(relate1) dbg_mrbc_prof_printf("relates...R%d(%d) BEFORE", relate1, prof->regsinfo[relate1] >> 2, prof->regsinfo[relate1] & 3);
  if(relate2) dbg_mrbc_prof_printf("relates...R%d(%d) BEFORE", relate2, prof->regsinfo[relate2] >> 2, prof->regsinfo[relate2] & 3);
#endif
  registers_move(prof, a, b);
#if OP_MOVE_DEBUG_OUT
  dbg_mrbc_prof_printf("R%d(%d %d) <- R%d(%d %d) AFTER", a, prof->regsinfo[a] >> 2, prof->regsinfo[a] & 3,  b, prof->regsinfo[b] >> 2, prof->regsinfo[b] & 3);
  if(relate1) dbg_mrbc_prof_printf("relates...R%d(%d) AFTER", relate1, prof->regsinfo[relate1] >> 2, prof->regsinfo[relate1] & 3);
  if(relate2) dbg_mrbc_prof_printf("relates...R%d(%d) AFTER", relate2, prof->regsinfo[relate2] >> 2, prof->regsinfo[relate2] & 3);
#endif
}

#define write_immediate(a, b, c) _write_immediate(a, b)
static int _write_immediate(mrbc_profile_profiler * prof, int virtualA) {
  return override_register_information(prof, virtualA, 1);
}

static int gen_read_const_object(mrbc_vm * vm, mrbc_value * regs, mrbc_profile_profiler * prof, mrbc_value * v, int a) {
  switch(mrbc_type(regs[a])) {
    case MRBC_TT_INTEGER:
    case MRBC_TT_CLASS:
    case MRBC_TT_MODULE:
    case MRBC_TT_SYMBOL:
    case MRBC_TT_FLOAT:
      return write_immediate(prof, a, regs[a].i);
    case MRBC_TT_TRUE:
      return write_immediate(prof, a, 1);
    case MRBC_TT_FALSE:
      return write_immediate(prof, a, 0);
    default: break;
  }
  // Floating and Int64 are not supported.
  if(v == NULL) return 1; // TODO: implement for OP_STRING

  RObjectPtrCopro * lwaddr = mrbcopro_globalman_get(vm, &(prof->globalMan), v);
  if(lwaddr == 0) return 1;
  if(gen_load_immediate(prof, 11, (int)MRBC_COPRO_NATIVE_PTR_TO_PTR(lwaddr))) return 1;
  mrbc_profile_basic_block * bb = profiler_current(prof);
  mrbc_profile_function_header * fh = profiler_currentfunction(prof);
  int toReg = mrbc_function_header_allocation(fh, a);
  override_register_information(prof, a, 0);
  for(int i = 0; i < bb->regLen; ++i) 
    apply_register_information(prof, i);
  set_prof_exit_types(prof, bb->regLen);
  int size = mrbcopro_vector_bytes2(prof->buf);
  mrbcopro_vector_append_without_fill(vm, &(prof->buf), CODE_BYTES_WRITE_JMP_TO_FALLBACK + sizeof(uint32_t) + sizeof(uint16_t));
  mrbc_send_inst_bufs_return_t ret = send_inst_bufs(prof, bb);
  prof->stack.previous_jmp_rd = 0;
  if((size_t)ret.buffer <= 1)
    prof->stack.previous_jmp = NULL;
  else {
    uint32_t * callsite = (uint32_t *)((int)ret.buffer + size);
    *callsite = RISCV_JUMP_AND_LINK(1, (int)ulp_read_barrier - (int)callsite);
    dbg_mrbc_prof_print_inst_readable("jal ra, ulp_read_barrier", fromReg);
    callsite++;
    uint16_t * callsite2 = (uint16_t *)callsite;
    *callsite2 = RISCV_C_MOVE(toReg, 3);
    dbg_mrbc_prof_print_inst_readable("c.mv %d, x3", toReg);
    callsite2++;
    bb->asserts = (struct profile_assertion_block *)2;
    bb->failInstPtr = (uint32_t *)0; // this does not happen deoptimization. 
    write_jmp_to_fallback((uint32_t *)callsite2);
#if MRBC_COPRO_ENABLE_CODE_HEADER
    dbg_dump_riscv_code2(sizeof(uint32_t), &(bb->allocatedHeader->jmp_code));
#endif
    dbg_dump_riscv_code2(ret.length, ret.buffer);
    prof->stack.previous_jmp = (uint32_t *)callsite2;
    bb->lastInstMRubyPtr = prof->lastInst;
  }
  int ret2 = try_search_or_new_profiling(prof);
  if(ret2 == 0) return write_jmp_and_execute_on_ulp(prof);
  if(ret2 == 1) return 1;
  set_prof_entrance_types(prof, bb->regLen);
  return 0;
}

//================================================================
/*! OP_LOADL

  R[a] = Pool[b]
*/
static inline void op_loadl( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BB();

  mrbc_decref(&regs[a]);
  regs[a] = mrbc_irep_pool_value(vm, b);

  gen_read_const_object(vm, regs, prof, NULL, a);
}

//================================================================
/*! OP_LOADI

  R[a] = mrb_int(b)
*/
static inline void op_loadi( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BB();

  mrbc_decref(&regs[a]);
  mrbc_set_integer(&regs[a], b);
  write_immediate(prof, a, b);
}


//================================================================
/*! OP_LOADINEG

  R[a] = mrb_int(-b)
*/
static inline void op_loadineg( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BB();

  mrbc_decref(&regs[a]);
  mrbc_set_integer(&regs[a], -(mrbc_int_t)b);
  write_immediate(prof, a, -(int32_t)b);
}


//================================================================
/*! OP_LOADI_n (n=-1,0,1..7)

  R[a] = mrb_int(n)
*/
static inline void op_loadi_n( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  // get n
  int opcode = vm->inst[-1];
  int n = opcode - OP_LOADI_0;

  FETCH_B();

  mrbc_decref(&regs[a]);
  mrbc_set_integer(&regs[a], n);
  write_immediate(prof, a, (uint32_t)n);
}

//================================================================
/*! OP_LOADI16

  R[a] = mrb_int(b)
*/
static inline void op_loadi16( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BS();

  mrbc_decref(&regs[a]);
  int16_t signed_b = (int16_t)b;
  mrbc_set_integer(&regs[a], signed_b);
  write_immediate(prof, a, (int32_t)signed_b);
}


//================================================================
/*! OP_LOADI32

  R[a] = mrb_int((b<<16)+c)
*/
static inline void op_loadi32( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BSS();

  mrbc_decref(&regs[a]);
  mrbc_set_integer(&regs[a], (((int32_t)b<<16)+(int32_t)c));
  write_immediate(prof, a, (((int32_t)b<<16)+(int32_t)c));
}


//================================================================
/*! OP_LOADSYM

  R[a] = Syms[b]
*/
static inline void op_loadsym( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BB();

  mrbc_decref(&regs[a]);
  mrbc_set_symbol(&regs[a], mrbc_irep_symbol_id(vm->cur_irep, b));
  write_immediate(prof, a, regs[a].sym_id);
}


//================================================================
/*! OP_LOADNIL

  R[a] = nil
*/
static inline void op_loadnil( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_B();

  mrbc_decref(&regs[a]);
  mrbc_set_nil(&regs[a]);

  write_immediate(prof, a, 0);
}


//================================================================
/*! OP_LOADSELF

  R[a] = self
*/
static inline void op_loadself( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_B();

  mrbc_decref(&regs[a]);
  regs[a] = *mrbc_get_self( vm, regs );
  mrbc_incref( &regs[a] );
  if(regs[0].tt == MRBC_TT_PROC) {
    // TODO: Support PROC
  } else
    registers_move(prof, a, 0);
}


//================================================================
/*! OP_LOADT

  R[a] = true
*/
static inline void op_loadt( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_B();

  mrbc_decref(&regs[a]);
  mrbc_set_true(&regs[a]);

  write_immediate(prof, a, 1);
}


//================================================================
/*! OP_LOADF

  R[a] = false
*/
static inline void op_loadf( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_B();

  mrbc_decref(&regs[a]);
  mrbc_set_false(&regs[a]);

  write_immediate(prof, a, 0);
}

static int gen_read_global_object(mrbc_vm * vm, mrbc_value *regs,  mrbc_profile_profiler * prof, mrbc_value * v, int a) {
  RObjectPtrCopro * lwaddr = mrbcopro_globalman_get(vm, &(prof->globalMan), v);
  if(lwaddr == 0) return 1;
  if(gen_load_immediate(prof, 11, (int)MRBC_COPRO_NATIVE_PTR_TO_PTR(lwaddr))) return 1;
  return gen_lw_and_typecheck(prof, 11, 0, a);
}

static int gen_write_global_object(mrbc_vm * vm, mrbc_value *regs,  mrbc_profile_profiler * prof, mrbc_value * v, int a) {
  RObjectPtrCopro * swaddr = mrbcopro_globalman_get(vm, &(prof->globalMan), v);
  if(swaddr == 0) return 1;
  if(gen_load_immediate(prof, 11, (int)MRBC_COPRO_NATIVE_PTR_TO_PTR(swaddr))) return 1;
  gen_sw(prof, get_allocation(prof, a), 11, 0, mrbc_type(regs[a]));
  return 0;
}

//================================================================
/*! OP_GETGV

  R[a] = getglobal(Syms[b])
*/
static inline void op_getgv( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BB();

  mrbc_decref(&regs[a]);
  mrbc_value *v = mrbc_get_global( mrbc_irep_symbol_id(vm->cur_irep, b) );
  if( v == NULL ) {
    mrbc_set_nil(&regs[a]);
  } else {
    mrbc_incref(v);
    regs[a] = *v;
  }
  // TODO: Error Handling.
  gen_read_global_object(vm, regs, prof, v, a);
}


//================================================================
/*! OP_SETGV

  setglobal(Syms[b], R[a])
*/
static inline void op_setgv( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BB();

  mrbc_incref(&regs[a]);
  mrbc_set_global( mrbc_irep_symbol_id(vm->cur_irep, b), &regs[a] );
  
  // TODO: Error Handling.
  gen_write_global_object(vm, regs, prof, mrbc_get_global(mrbc_irep_symbol_id(vm->cur_irep, b)), a);
}

//================================================================
/*! OP_GETIV

  R[a] = ivget(Syms[b])
*/
static inline void op_getiv( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BB();

  const char *sym_name = mrbc_irep_symbol_cstr(vm->cur_irep, b);
  mrbc_sym sym_id = mrbc_str_to_symid(sym_name+1);   // skip '@'
  if( sym_id < 0 ) {
    mrbc_raise(vm, MRBC_CLASS(Exception), "Overflow MAX_SYMBOLS_COUNT");
    return;
  }
  mrbc_value *self = mrbc_get_self( vm, regs );
  if( self->tt != MRBC_TT_OBJECT ) {
    mrbc_raise(vm, MRBC_CLASS(NotImplementedError), 0);
    return;
  }
  mrbc_decref(&regs[a]);
  regs[a] = mrbc_instance_getiv(self, sym_id);

  mrbcopro_objinfo * oi = mrbcopro_objman_get_obj_info(vm, &(prof->objMan), self->instance->cls);
  if(oi == NULL) {
    dbg_mrbc_prof_print("[FATAL]oi is null!");
    return; // TODO: Error Handling
  }
  int offset = mrbcopro_objman_get_fieldoffset(vm, oi, sym_id);
  if(offset == 0) {
    dbg_mrbc_prof_printf("[FATAL]offset is zero! (object is %s, symbol is %s)", symid_to_str(oi->cls->sym_id), symid_to_str(sym_id));
    return; // TODO: Error Handling
  }
  gen_lw_and_typecheck(prof, mrbc_function_header_allocation(profiler_currentfunction(prof), 0), offset, a);
}


//================================================================
/*! OP_SETIV

  ivset(Syms[b],R[a])
*/
static inline void op_setiv( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BB();

  const char *sym_name = mrbc_irep_symbol_cstr(vm->cur_irep, b);
  mrbc_sym sym_id = mrbc_str_to_symid(sym_name+1);   // skip '@'
  if( sym_id < 0 ) {
    mrbc_raise(vm, MRBC_CLASS(Exception), "Overflow MAX_SYMBOLS_COUNT");
    return;
  }
  mrbc_value *self = mrbc_get_self( vm, regs );
  if( self->tt != MRBC_TT_OBJECT ) {
    mrbc_raise(vm, MRBC_CLASS(NotImplementedError), 0);
    return;
  }

  mrbc_instance_setiv(self, sym_id, &regs[a]);
  mrbcopro_objinfo * oi = mrbcopro_objman_get_obj_info(vm, &(prof->objMan), self->instance->cls);
  if(oi == NULL) return; // TODO: Error Handling
  int offset = mrbcopro_objman_get_fieldoffset(vm, oi, sym_id);
  if(offset == 0) return; // TODO: Error Handling
  gen_sw(prof, get_allocation(prof, a), mrbc_function_header_allocation(profiler_currentfunction(prof), 0), offset, mrbcopro_objman_type2coprotype(vm, &(prof->objMan), &regs[a]));
}


//================================================================
/*! OP_GETCONST

  R[a] = constget(Syms[b])
*/
static inline void op_getconst( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BB();

  mrbc_sym sym_id = mrbc_irep_symbol_id(vm->cur_irep, b);
  mrbc_class *crit_cls;
  mrbc_value *ret;
  
  if( vm->callinfo_tail && vm->callinfo_tail->own_class ) {
    crit_cls = vm->callinfo_tail->own_class;
  } else {
    crit_cls = find_class_by_object( mrbc_get_self(vm, regs) );
  }

  // search in my class, then search nested outer class.
  mrbc_class *cls = crit_cls;
  while( 1 ) {
    ret = mrbc_get_class_const(cls, sym_id);
    if( ret ) goto DONE;

    if( !mrbc_is_nested_symid(cls->sym_id) ) break;

    mrbc_sym outer_id;
    mrbc_separate_nested_symid( cls->sym_id, &outer_id, 0 );
    cls = mrbc_get_const( outer_id )->cls;
  }

  // search in super class.
  cls = crit_cls->super;
  while( cls ) {
    ret = mrbc_get_class_const(cls, sym_id);
    if( ret ) goto DONE;
    cls = cls->super;
  }

  // is top level constant definition?
  ret = mrbc_get_const(sym_id);
  if( ret == NULL ) {
    mrbc_raisef( vm, MRBC_CLASS(NameError),
		 "uninitialized constant %s", mrbc_symid_to_str(sym_id));
    return;
  }

 DONE:
  mrbc_incref(ret);
  mrbc_decref(&regs[a]);
  regs[a] = *ret;

  gen_read_const_object(vm, regs, prof, ret, a);
}


//================================================================
/*! OP_SETCONST

  constset(Syms[b],R[a])
*/
static inline void op_setconst( mrbc_vm *vm, mrbc_value *regs EXT )
{
  FETCH_BB();

  mrbc_sym sym_id = mrbc_irep_symbol_id(vm->cur_irep, b);

  mrbc_incref(&regs[a]);
  if( mrbc_type(regs[0]) == MRBC_TT_CLASS || mrbc_type(regs[0]) == MRBC_TT_MODULE ) {
    mrbc_set_class_const(regs[0].cls, sym_id, &regs[a]);
  } else {
    mrbc_set_const(sym_id, &regs[a]);
  }
}


//================================================================
/*! OP_GETMCNST

  R[a] = R[a]::Syms[b]
*/
static inline void op_getmcnst( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BB();

  mrbc_sym sym_id = mrbc_irep_symbol_id(vm->cur_irep, b);
  mrbc_class *cls = regs[a].cls;
  mrbc_value *ret;
  
  // ::CONST case
  if( cls->sym_id == MRBC_SYM(Object) ) {
    ret = mrbc_get_const(sym_id);
    if( ret == NULL ) {
      mrbc_raisef( vm, MRBC_CLASS(NameError), "uninitialized constant %s::%s",
                   "", mrbc_symid_to_str( sym_id ));
      return;
    }
    goto DONE;
  }

  while( !(ret = mrbc_get_class_const(cls, sym_id)) ) {
    cls = cls->super;
    if( !cls ) {
      mrbc_raisef( vm, MRBC_CLASS(NameError), "uninitialized constant %s::%s",
	mrbc_symid_to_str( regs[a].cls->sym_id ), mrbc_symid_to_str( sym_id ));
      return;
    }
  }

DONE:
  mrbc_incref(ret);
  mrbc_decref(&regs[a]);
  regs[a] = *ret;

  gen_read_const_object(vm, regs, prof, ret, a);
}


//================================================================
/*! OP_GETUPVAR

  R[a] = uvget(b,c)

  b: target offset of regs.
  c: nested block level.
*/
static inline void op_getupvar( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BBB();

  assert( mrbc_type(regs[0]) == MRBC_TT_PROC );
  mrbc_callinfo *callinfo = regs[0].proc->callinfo;

  mrbc_profile_profiler_stack * st = prof->stacktop->prev;
  for( int i = 0; i < c; i++ ) {
    assert( callinfo );
    mrbc_value *reg0 = callinfo->cur_regs + callinfo->reg_offset;

    if( mrbc_type(*reg0) != MRBC_TT_PROC ) break;	// What to do?
    if(st != NULL) st = st->prev;
    callinfo = reg0->proc->callinfo;
  }

  mrbc_value *p_val;
  if( callinfo == 0 ) {
    p_val = vm->regs + b;
  } else {
    p_val = callinfo->cur_regs + callinfo->reg_offset + b;
  }
  mrbc_incref( p_val );

  mrbc_decref( &regs[a] );
  regs[a] = *p_val;

  if(st == NULL) // it's outside of Copro#run.
    // TODO: Error Handling.
    gen_read_global_object(vm, regs, prof, p_val, a);
  else
  {
    // TODO: Support Closures.
  }
}


//================================================================
/*! OP_SETUPVAR

  uvset(b,c,R[a])
*/
static inline void op_setupvar( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BBB();

  assert( regs[0].tt == MRBC_TT_PROC );
  mrbc_callinfo *callinfo = regs[0].proc->callinfo;

  mrbc_profile_profiler_stack * st = prof->stacktop->prev;
  for( int i = 0; i < c; i++ ) {
    assert( callinfo );
    mrbc_value *reg0 = callinfo->cur_regs + callinfo->reg_offset;
    assert( reg0->tt == MRBC_TT_PROC );
    if(st != NULL) st = st->prev;
    callinfo = reg0->proc->callinfo;
  }

  mrbc_value *p_val;
  if( callinfo == 0 ) {
    p_val = vm->regs + b;
  } else {
    p_val = callinfo->cur_regs + callinfo->reg_offset + b;
  }
  mrbc_decref( p_val );

  mrbc_incref( &regs[a] );
  *p_val = regs[a];

  if(st == NULL) // it's outside of Copro#run.
  {
    // TODO: Error Handling.
    gen_write_global_object(vm, regs, prof, p_val, a);
  }
  else
  {
    // TODO: Support Closures.
  }
}


//================================================================
/*! OP_GETIDX

  R[a] = R[a][R[a+1]]
*/
static inline void op_getidx( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_B();

  send_by_name( vm, MRBC_SYMID_BL_BR, a, 1, prof);
}


//================================================================
/*! OP_SETIDX

  R[a][R[a+1]] = R[a+2]
*/
static inline void op_setidx( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_B();

  send_by_name( vm, MRBC_SYMID_BL_BR_EQ, a, 2, prof);
}

typedef uint32_t * (*func_jmp_writer_t)(void * buffer, mrbc_profile_basic_block * current_pc, mrbc_profile_basic_block * const next_pc, void * payload);
/// @brief Write jump instructions
/// @param vm  The VM
/// @param prof  The prof
/// @param prepare_size  Code Size for jump
/// @param alive_reg_number In general, this is a+1, I believe.
/// @param jmpwriter_payload Given to the jmpwriter
/// @param jmpwriter Jmp code writing function.
/// @return 0 if success, otherwise 1.
static inline int write_jmp_instructions(mrbc_vm *vm,  mrbc_profile_profiler * prof,
  size_t prepare_size, int alive_reg_number, void * jmpwriter_payload, func_jmp_writer_t jmpwriter) {
  // The current profling struct.
  mrbc_profile_basic_block * ppc = profiler_current(prof);
  for(int i = 0; i < prof->regsinfo_length; ++i)
    apply_register_information(prof, i);
  set_prof_exit_types(prof, ppc->regLen);
  // The next profiled profiling struct. If found, not null.
  int ret = try_search_or_new_profiling(prof);
  if(ret == 1) return 1;
  mrbcopro_vector_extend(vm, &(prof->buf), prepare_size);
  size_t towrite = mrbcopro_vector_bytes2(prof->buf);
  prof->buf.cur = (char *)((size_t)prof->buf.cur + prepare_size);
  mrbc_send_inst_bufs_return_t sibret = send_inst_bufs(prof, ppc);
  prof->stack.previous_jmp_rd = 0;
  if((size_t)sibret.buffer <= 1)
    prof->stack.previous_jmp = NULL;
  else {
    prof->stack.previous_jmp = jmpwriter((void *)((size_t)sibret.buffer +  towrite), ppc, profiler_current(prof), jmpwriter_payload);
#if MRBC_COPRO_ENABLE_CODE_HEADER
    dbg_dump_riscv_code2(sizeof(uint16_t), &(ppc->allocatedHeader->jmp_code));
#endif
    dbg_dump_riscv_code2(sibret.length, sibret.buffer);
  }
  if(ret == 0) return write_jmp_and_execute_on_ulp(prof);
  set_prof_entrance_types(prof, alive_reg_number);
  return 0;
}

#define JMP_WRITER_SIZE sizeof(uint32_t)
uint32_t * jmp_writer(uint32_t * buf, mrbc_profile_basic_block * current_pc, mrbc_profile_basic_block * next_pc, void * payload) {
#if MRBC_COPRO_ENABLE_CODE_HEADER
  *buf = RISCV_JUMP_AND_LINK(0, (size_t)(&(next_pc->allocatedHeader->jmp_code)) - (size_t)buf);
  dbg_mrbc_prof_printf("jal 0, %d", (size_t)(&(next_pc->allocatedHeader->jmp_code)) - (size_t)buf);
#else
  current_pc->failInstPtr = buf;
  write_jmp_to_fallback(buf);
#endif
  return buf;
}

//================================================================
/*! OP_JMP

  pc+=a
*/
static inline void op_jmp( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_S();
  vm->inst += (int16_t)a;
  if((int16_t)a < 0) { // If backward, do not create the new basic block.
    dbg_mrbc_prof_print("OP_JMP");
    write_jmp_instructions(vm, prof, JMP_WRITER_SIZE, vm->cur_irep->nlocals+1, NULL, (func_jmp_writer_t)jmp_writer);
  }
}

#if MRBC_COPRO_ENABLE_CODE_HEADER
#define MRBC_COPRO_BNE_BEQ_WRITER_SIZE (CODE_BYTES_WRITE_JMP_TO_FALLBACK + sizeof(uint32_t))
#else
#define MRBC_COPRO_BNE_BEQ_WRITER_SIZE (CODE_BYTES_WRITE_JMP_TO_FALLBACK + (sizeof(uint32_t) * 2))
#endif

struct branch_write_state_t {
  int reg_num;
  int op;
};

uint32_t * branch_writer(uint32_t * buf, mrbc_profile_basic_block * current_pc, mrbc_profile_basic_block * next_pc, struct branch_write_state_t * payload) {
  uint32_t * ret;
#if MRBC_COPRO_ENABLE_CODE_HEADER
  size_t jmp = (size_t)(&(next_pc->allocatedHeader->jmp_code)) - (size_t)(buf);
#else
  size_t jmp = sizeof(uint32_t) * 2;
#endif
  *buf = RISCV_BTYPE(0x63, payload->reg_num, jmp, 0, payload->op);
  dbg_mrbc_prof_print_inst_readable("%s x%d, x0, %d", payload->op == 0 ? "beq" : "bne", payload->reg_num, jmp);
#if !MRBC_COPRO_ENABLE_CODE_HEADER
  buf++;
  ret = buf;
  *buf = RISCV_JUMP_AND_LINK(0, 0);
#endif
  buf++;
  current_pc->failInstPtr = buf;
  write_jmp_to_fallback(buf);
  return ret;
}

//================================================================
/*! OP_JMPIF

  if R[a] pc+=b
*/
static inline void op_jmpif( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BS();
  if( regs[a].tt > MRBC_TT_FALSE ) {
    vm->inst += (int16_t)b;
  }

  // If type is always NIL, Empty, Go through.
  if(regs[a].tt < MRBC_TT_FALSE)
    return;
  // If type is always non-boolean, Jump.
  if(regs[a].tt > MRBC_TT_TRUE && b > 0)
    return;

  profiler_current(prof)->lastInstMRubyPtr = prof->lastInst;

  dbg_mrbc_prof_print("OP_JMPIF");
  struct branch_write_state_t st =
    {get_allocation(prof, a), regs[a].tt <= MRBC_TT_FALSE};
  write_jmp_instructions(vm, prof, MRBC_COPRO_BNE_BEQ_WRITER_SIZE,
    a+1, (void *)(&st), (func_jmp_writer_t)(branch_writer));
}

//================================================================
/*! OP_JMPNOT

  if !R[a] pc+=b
*/
static inline void op_jmpnot( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BS();
  if( regs[a].tt <= MRBC_TT_FALSE ) {
    vm->inst += (int16_t)b;
  }
  // If type is always NIL, Empty, Jump.
  if(regs[a].tt < MRBC_TT_FALSE && b > 0)
    return;

  // If type is always non-boolean, Go Through.
  if(regs[a].tt > MRBC_TT_TRUE)
    return;

  profiler_current(prof)->lastInstMRubyPtr = prof->lastInst;

  dbg_mrbc_prof_print("OP_JMPNOT");
  struct branch_write_state_t st =
    {get_allocation(prof, a), regs[a].tt <= MRBC_TT_FALSE};
  write_jmp_instructions(vm, prof, MRBC_COPRO_BNE_BEQ_WRITER_SIZE,
    a+1, (void *)(&st), (func_jmp_writer_t)(branch_writer));
}

//================================================================
/*! OP_JMPNIL

  if R[a]==nil pc+=b
*/
static inline void op_jmpnil( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BS();

  if( regs[a].tt == MRBC_TT_NIL ) {
    vm->inst += (int16_t)b;
  }
  // If forwarding, Do Not Anything, because NIL is specialized.
  if((int16_t)b < 0 && regs[a].tt == MRBC_TT_NIL) {
    dbg_mrbc_prof_print("OP_JMPNIL");
    write_jmp_instructions(vm, prof, JMP_WRITER_SIZE, a+1, NULL, (func_jmp_writer_t)jmp_writer);
  }
}


//================================================================
/*! OP_JMPUW

  unwind_and_jump_to(a)
*/
static inline void op_jmpuw( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_S();

  const uint8_t *jump_inst = vm->inst + (int16_t)a;

  // check catch handler (ensure)
  const mrbc_irep_catch_handler *handler = find_catch_handler_ensure(vm);
  if( !handler ) {
    vm->inst = jump_inst;
    if((int16_t)a < 0) { // If backward, do not create the new basic block.
      dbg_mrbc_prof_print("OP_JMPUW(nohandler)");
      write_jmp_instructions(vm, prof, JMP_WRITER_SIZE, vm->cur_irep->nlocals+1, NULL, (func_jmp_writer_t)jmp_writer);
    }
    return;
  }
}

//================================================================
/*! OP_SSEND

  R[a] = self.send(Syms[b],R[a+1]..,R[a+n+1]:R[a+n+2]..) (c=n|k<<4)
*/
static inline void op_ssend( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BBB();

  mrbc_decref( &regs[a] );
  regs[a] = *mrbc_get_self( vm, regs );
  mrbc_incref( &regs[a] );

  // MOVE
  gen_move(prof, a, 0);
  send_by_name( vm, mrbc_irep_symbol_id(vm->cur_irep, b), a, c, prof);
}



//================================================================
/*! OP_SSENDB

  R[a] = self.send(Syms[b],R[a+1]..,R[a+n+1]:R[a+n+2]..,&R[a+n+2k+1])
*/
static inline void op_ssendb( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BBB();

  mrbc_decref( &regs[a] );
  regs[a] = *mrbc_get_self( vm, regs );
  mrbc_incref( &regs[a] );

  // MOVE
  gen_move(prof, a, 0);
  send_by_name( vm, mrbc_irep_symbol_id(vm->cur_irep, b), a, c | 0x100, prof);
}



//================================================================
/*! OP_SEND

  R[a] = R[a].send(Syms[b],R[a+1]..,R[a+n+1]:R[a+n+2]..) (c=n|k<<4)
*/
static inline void op_send( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BBB();

  send_by_name( vm, mrbc_irep_symbol_id(vm->cur_irep, b), a, c, prof);
}


//================================================================
/*! OP_SENDB

  R[a] = R[a].send(Syms[b],R[a+1]..,R[a+n+1]:R[a+n+2]..,&R[a+n+2k+1])
*/
static inline void op_sendb( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BBB();

  send_by_name( vm, mrbc_irep_symbol_id(vm->cur_irep, b), a, c | 0x100, prof);
}


//================================================================
/*! OP_SUPER

  R[a] = super(R[a+1],... ,R[a+b+1])
*/
static inline void op_super( mrbc_vm *vm, mrbc_value *regs EXT )
{
  FETCH_BB();

  int narg = b & 0x0f;
  int karg = (b >> 4) & 0x0f;
  mrbc_value *recv = regs + a;	// new regs[0]

  // set self to new regs[0]
  mrbc_value *self = mrbc_get_self(vm, regs);
  assert( self->tt != MRBC_TT_PROC );

  mrbc_incref( self );
  mrbc_decref( recv );
  *recv = *self;

  // If it's packed in an array, expand it.
  if( narg == CALL_MAXARGS ) {
    /* (note)
       on mrbc ver 3.1
         b = 15  in initialize method.
	 b = 255 in other method.
    */

    assert( recv[1].tt == MRBC_TT_ARRAY );

    mrbc_value argary = recv[1];
    mrbc_value proc = recv[2];
    recv[1].tt = MRBC_TT_EMPTY;
    recv[2].tt = MRBC_TT_EMPTY;

    int argc = mrbc_array_size(&argary);
    for( int i = 0; i < argc; i++ ) {
      mrbc_decref( &recv[i+1] );
      recv[i+1] = argary.array->data[i];
    }
    mrbc_array_delete_handle(&argary);

    mrbc_decref( &recv[argc+1] );
    recv[argc+1] = proc;
    narg = argc;
  }

  // Convert keyword argument to hash.
  if( karg && karg != CALL_MAXARGS ) {
    narg++;
    mrbc_value h = mrbc_hash_new( vm, karg );
    if( !h.hash ) return;	// ENOMEM

    mrbc_value *r1 = recv + narg;
    memcpy( h.hash->data, r1, sizeof(mrbc_value) * karg * 2 );
    h.hash->n_stored = karg * 2;

    mrbc_value block = r1[karg * 2];
    memset( r1 + 2, 0, sizeof(mrbc_value) * (karg * 2 - 1) );
    *r1++ = h;
    *r1 = block;
  }

  // find super class
  mrbc_callinfo *callinfo = vm->callinfo_tail;
  mrbc_class *cls = callinfo->own_class;
  mrbc_method method;

  assert( cls );
  cls = cls->super;
  assert( cls );
  if( mrbc_find_method( &method, cls, callinfo->method_id ) == 0 ) {
    mrbc_raisef( vm, MRBC_CLASS(NoMethodError),
	"no superclass method '%s' for %s",
	mrbc_symid_to_str(callinfo->method_id),
	mrbc_symid_to_str(callinfo->own_class->sym_id));
    return;
  }

  // call C function and return.
  if( method.c_func ) {
    method.func(vm, recv, narg);
    for( int i = 1; i <= narg+1; i++ ) {
      mrbc_decref_empty( recv + i );
    }
    return;
  }

  // call Ruby method.
  callinfo = mrbc_push_callinfo(vm, callinfo->method_id, a, narg);
  callinfo->own_class = method.cls;
  callinfo->is_called_super = 1;

  vm->cur_irep = method.irep;
  vm->inst = vm->cur_irep->inst;
  vm->cur_regs = recv;
}

int search_function(mrbc_vm *vm, mrbc_profile_profiler * prof, int regLen)
{
  mrbc_profile_function_header * fh = prof->functions;
  const mrbc_irep * vm_cur_irep = vm->cur_irep;
  const mrbc_class * vm_own_class = vm->callinfo_tail->own_class;
  while(fh) {
    if(fh->irep == vm_cur_irep && fh->own_class == vm_own_class) {
      prof->stack.functions_current = fh;
      return 1;
    }
    fh = fh->next;
  }
  fh = mrbcopro_alloc_prof(vm, sizeof(mrbc_profile_function_header) + (sizeof(uint8_t) * regLen));
  dbg_mrbc_prof_printf("[Profile] [NEW FUNCTION] vm->callinfo_tail=%p, cur_irep=%p, own_class=%p", vm->callinfo_tail, vm_cur_irep, vm_own_class);
  memset(fh, 0, sizeof(mrbc_profile_function_header) + (sizeof(uint8_t) * regLen));
  fh->next = prof->functions;
  fh->irep = vm_cur_irep;
  fh->regLen = regLen;
  fh->own_class = vm_own_class;
  prof->functions = fh;
  prof->stack.functions_current = fh;
  return 0;
}

struct has_send_payload_t {
  uint8_t senda;
  uint8_t has_readbarrier;
};
int has_send(const uint8_t * inst, struct has_send_payload_t * payload) {
  uint8_t i = *inst;
  if((uint32_t)(i - OP_SSEND) <= (OP_SENDB - OP_SSEND)) {
    uint8_t p = payload->senda;
    uint8_t newp = inst[1];
    payload->senda = p > newp ? p : newp;
  }
  if((i == OP_GETUPVAR) || (i == OP_GETGV) || (i == OP_GETIV) || (i == OP_GETCONST) || (i == OP_GETMCNST) || (i == OP_STRING)) // OP_LOADI does not likely to load objects.
    payload->has_readbarrier = 1;
  return 0;
}

#if MRBC_PROF_DBG_ENABLE
int alloc_register(mrbc_profile_function_header * fh, int i, uint32_t mask, char const * msg) {
#else
int alloc_register(mrbc_profile_function_header * fh, int i, uint32_t mask) {
#endif
  uint32_t freeMachineRegisters = fh->freeMachineRegisters;
  uint32_t caller_free = mask & freeMachineRegisters;
  if(caller_free == 0) return 0;
  int newreg = __builtin_ctz(caller_free);
  fh->freeMachineRegisters = freeMachineRegisters ^ (1 << newreg);
  mrbc_function_header_allocation(fh, i) = newreg;
  dbg_mrbc_prof_printf("REGISTER ALLOCATION: R%d -> x%d(%s) %x", i, newreg, msg, fh->freeMachineRegisters);
  return newreg;
}

struct plan_result_t {
  int callee_save;
};
static struct plan_result_t plan_register_alloc(mrbc_profile_profiler * prof, int senda, int args) {
  mrbc_profile_function_header * fh = prof->stack.functions_current;
  fh->freeMachineRegisters = (uint32_t)-1 ^ 0x81F;
  // first attempts to senda.
  const int CALLER_REGISTERS = __builtin_popcount(RISCV_NORMAL_CALLER_SAVE_REGISTERS & ~(1 << 11));
  const int CALLEE_REGISTERS = __builtin_popcount(RISCV_CALLEE_SAVE_REGISTERS);
  int callee_save = senda > CALLEE_REGISTERS ? CALLEE_REGISTERS : senda;
  int caller_save = prof->vm->cur_irep->nregs - callee_save;
  if(caller_save > CALLER_REGISTERS) {
    caller_save = CALLER_REGISTERS;
    callee_save = prof->vm->cur_irep->nregs - caller_save;
    // TODO: stack alloc.
  }
  int i = 0;
  if(callee_save == 0) { // special case for no callee saved used.
#if MRBC_PROF_DBG_ENABLE
    alloc_register(fh, 0, 1 << RISCV_ARGS_REGISTER(0), "caller_saved");
    if(args >= 2)
      alloc_register(fh, 1, 1 << RISCV_ARGS_REGISTER(args), "caller_saved");
    i=1;
    for(i = 2; i < args; ++i)
      alloc_register(fh, i, 1 << RISCV_ARGS_REGISTER(i), "caller_saved");
#else
    alloc_register(fh, 0, 1 << RISCV_ARGS_REGISTER(0));
    if(args >= 2)
      alloc_register(fh, 1, 1 << RISCV_ARGS_REGISTER(args));
    i = 1;
    for(i = 2; i < args; ++i)
      alloc_register(fh, i, 1 << RISCV_ARGS_REGISTER(i));
#endif
  }
  int right = callee_save + caller_save;
#if MRBC_PROF_DBG_ENABLE
  const char str_callee_saved[] = "callee-saved";
  const char str_caller_saved[] = "caller-saved";
  char const * dbgstr = str_callee_saved;
#endif
  int mask = RISCV_CALLEE_SAVE_REGISTERS;
  int r = callee_save;
  while(1) {
    if(i >= r) {
      if(r == right) break;
      r = right;
      mask = RISCV_NORMAL_CALLER_SAVE_REGISTERS & ~(1 << 11);
#if MRBC_PROF_DBG_ENABLE
      dbgstr = str_caller_saved;
#endif
      continue;
    }
#if MRBC_PROF_DBG_ENABLE
    alloc_register(fh, i, mask, dbgstr);
#else
    alloc_register(fh, i, mask);
#endif
    ++i;
  }
  return (struct plan_result_t){callee_save};
}

static void move_argments(struct VM * vm, mrbc_profile_function_header * fh, mrbcopro_vector_t * prof_buf, int argNo) {
  int allocated = mrbc_function_header_allocation(fh, argNo);
  int src = RISCV_ARGS_REGISTER(argNo);
  if(allocated == src) return;
  mrbcopro_vector_append16(vm, prof_buf, RISCV_C_MOVE(allocated, src));
  dbg_mrbc_prof_print_inst_readable("c.move x%d, x%d", allocated, src);
}

//================================================================
/*! OP_ENTER

  arg setup according to flags (23=m5:o5:r1:m5:k5:d1:b1)

  flags: 0mmm_mmoo_ooor_mmmm_mkkk_kkdb
*/
static inline void op_enter( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  {
    if(prof->regsinfo_length < vm->cur_irep->nregs) {
      char * ch = mrbcopro_realloc(vm, prof->regsinfo, sizeof(char) * vm->cur_irep->nregs);
      if(ch == NULL) return; // TODO: OoM Error
      prof->regsinfo = ch;
      prof->regsinfo_length = vm->cur_irep->nregs;
    }
    memset(prof->regsinfo, 0, sizeof(char) * vm->cur_irep->nregs);
    int found = search_function(vm, prof, vm->cur_irep->nregs);
    if(found) {
      mrbc_profile_basic_block * bb2 = search_basic_block_within_function(prof);
      if(bb2) {
        profiler_current(prof) = bb2;
        dbg_mrbc_prof_print("Gotcha! (INST: OP_ENTER).");
        if(bb2->allocatedCode != NULL) {
          write_jmp_and_execute_on_ulp(prof); // TODO: Error handling
          return;
        }
      }
    }
    dbg_mrbc_prof_print("New profiling(INST: OP_ENTER).");
    int argLen = vm->callinfo_tail->n_args + 1;
    mrbc_profile_basic_block * bb = new_profiling(vm, vm->cur_irep->nregs);
    if(bb == NULL) return; // TODO: Error handling
    // bb->functionHeader = prof->stack->functions_current;
    profiler_append_and_set_current(prof, bb);
    set_prof_entrance_types(prof, argLen);
    int spmoves;
    mrbc_profile_function_header * fh;
    if(found) {
      spmoves = profiler_currentfunction(prof)->spMoved;
      fh = profiler_currentfunction(prof);
      goto skip_analyze;
    }

    struct has_send_payload_t callee_saved_usage = {0};
    inst_analyze(vm->cur_irep, (inst_analyzer)has_send, &callee_saved_usage);
    struct plan_result_t plan = plan_register_alloc(prof, callee_saved_usage.senda, argLen);
    callee_saved_usage.has_readbarrier = *((uint16_t *)&(callee_saved_usage)) != 0; // to symplify, ra is saved if there are no send and the used registers are more than callee saved registers.
    fh = profiler_currentfunction(prof);
    spmoves = estimate_stack_move_size(plan.callee_save + callee_saved_usage.has_readbarrier);
    fh->spMoved = spmoves;
    skip_analyze:
    if(spmoves == 0) {
      //move arguments. be careful, must be moved along decreasing register number.
      for(int i = argLen-1; i >= 0; --i)
        move_argments(vm, fh, &(prof->buf), i);
      goto end_func_prologue;
    }
    mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_ADDI16SP(-spmoves));
    dbg_mrbc_prof_print_inst_readable("c.addi16sp %d", -spmoves);
    //save callee used.
    for(int i = 1, used_callee = (mrbc_function_header_allocation_retrived_registers_on_callee_regiseter_saving(fh) >> 1) + 1;
        i < 32; ++i, used_callee >>= 1) {
      if((used_callee & 1) == 0) continue; // not allocated.
      spmoves -= 4;
      mrbcopro_vector_append16(prof->vm, &(prof->buf), RISCV_C_SWSP(i, spmoves));
      dbg_mrbc_prof_print_inst_readable("c.swsp x%d, %d", i, spmoves);
    }
    fh->spRemaining = spmoves;
    //move arguments. be careful, must be moved along increasing register number.
    for(int i = 0; i < argLen; ++i)
      move_argments(vm, fh, &(prof->buf), i);
    goto end_func_prologue;
    end_func_prologue:
  }
#define FLAG_REST	0x1000
#define FLAG_M2		0x0f80
#define FLAG_KW		0x007c
#define FLAG_DICT	0x0002
#define FLAG_BLOCK	0x0001

  FETCH_W();

  // Check the number of registers to use.
  int reg_use_max = regs - vm->regs + vm->cur_irep->nregs;
  if( reg_use_max >= vm->regs_size ) {
    mrbc_raise( vm, MRBC_CLASS(Exception), "MAX_REGS_SIZE overflow");
    return;
  }

  // Check m2 parameter.
  if( a & FLAG_M2 ) {
    mrbc_raise( vm, MRBC_CLASS(NotImplementedError), "not support m2 or keyword argument");
    return;
  }

  int m1 = (a >> 18) & 0x1f;	// num of required parameters 1
  int o  = (a >> 13) & 0x1f;	// num of optional parameters
  int argc = vm->callinfo_tail->n_args;

  if( argc < m1 && mrbc_type(regs[0]) != MRBC_TT_PROC ) {
    mrbc_raise( vm, MRBC_CLASS(ArgumentError), "wrong number of arguments");
    return;
  }

  // save proc (or nil) object.
  mrbc_value proc = regs[argc+1];
  regs[argc+1].tt = MRBC_TT_EMPTY;

  // support yield [...] pattern, to expand array.
  if( mrbc_type(regs[0]) == MRBC_TT_PROC &&
      mrbc_type(regs[1]) == MRBC_TT_ARRAY &&
      argc == 1 && m1 > 1 ) {
    mrbc_value argary = regs[1];
    int argary_size = mrbc_array_size(&argary);

    argc = argary_size > m1 ? argary_size : m1;

    for( int i = argc; i > 0; i-- ) {
      if( i != 1 ) mrbc_decref( &regs[i] );
      if( argary_size >= i ) {
        regs[i] = argary.array->data[i-1];
        mrbc_incref(&regs[i]);
      } else {
        mrbc_set_nil( &regs[i] );
      }
    }
    mrbc_decref(&argary);
  }

  // dictionary, keyword or rest parameter exists.
  if( a & (FLAG_DICT|FLAG_KW|FLAG_REST) ) {
    mrbc_value dict;
    if( a & (FLAG_DICT|FLAG_KW) ) {
      if( (argc - m1) > 0 && mrbc_type(regs[argc]) == MRBC_TT_HASH ) {
	dict = regs[argc];
	regs[argc--].tt = MRBC_TT_EMPTY;
      } else {
	dict = mrbc_hash_new( vm, 0 );
      }
    }

    mrbc_value rest;
    if( a & FLAG_REST ) {
      int rest_size = argc - m1 - o;
      if( rest_size < 0 ) rest_size = 0;
      rest = mrbc_array_new(vm, rest_size);
      if( !rest.array ) return;	// ENOMEM

      int rest_reg = m1 + o + 1;
      for( int i = 0; i < rest_size; i++ ) {
	mrbc_array_push( &rest, &regs[rest_reg] );
	regs[rest_reg++].tt = MRBC_TT_EMPTY;
      }
    }

    // reorder arguments.
    int i;
    for( i = argc; i < m1; ) {
      mrbc_decref( &regs[++i] );
      mrbc_set_nil( &regs[i] );
    }
    i = m1 + o;
    if( a & FLAG_REST ) {
      mrbc_decref(&regs[++i]);
      regs[i] = rest;
    }
    if( a & (FLAG_DICT|FLAG_KW) ) {
      mrbc_decref(&regs[++i]);
      regs[i] = dict;
      if( a & FLAG_KW ) {
	vm->callinfo_tail->karg_keep = mrbc_hash_dup(vm, &dict).hash;
      }
    }
    mrbc_decref(&regs[i+1]);
    regs[i+1] = proc;
    vm->callinfo_tail->n_args = i;

  } else {
    // reorder arguments.
    int i;
    for( i = argc; i < m1; ) {
      mrbc_decref( &regs[++i] );
      mrbc_set_nil( &regs[i] );
    }
    i = m1 + o;
    mrbc_decref(&regs[i+1]);
    regs[i+1] = proc;
    vm->callinfo_tail->n_args = i;
  }

  // prepare for get default arguments.
  int jmp_ofs = argc - m1;
  if( jmp_ofs > 0 ) {
    if( jmp_ofs > o ) {
      jmp_ofs = o;

      if( !(a & FLAG_REST) && mrbc_type(regs[0]) != MRBC_TT_PROC ) {
	mrbc_raise( vm, MRBC_CLASS(ArgumentError), "wrong number of arguments");
	return;
      }
    }
    vm->inst += jmp_ofs * 3;	// 3 = bytecode size of OP_JMP
  }

#undef FLAG_REST
#undef FLAG_M2
#undef FLAG_KW
#undef FLAG_DICT
#undef FLAG_BLOCK
}

//================================================================
/*! op_return, op_return_blk subroutine.
*/
static inline void op_return__sub( mrbc_vm *vm, mrbc_value *regs, int a, mrbc_profile_profiler * prof )
{
  if(prof) {
    if( vm->callinfo_tail->method_id != MRBC_SYM(initialize) ){ // return value except for initialize methods.
      if(prof->regsinfo[a] == 1) // constant
        gen_load_immediate(prof, 10, regs[a].i);
      else {
        int retReg = get_allocation(prof, a);
        mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_MOVE(10, retReg));
        dbg_mrbc_prof_print_inst_readable("c.move a0, x%d", retReg);
      }
      // type writing.
      gen_load_immediate(prof, 11, mrbcopro_objman_type2coprotype(vm, &(prof->objMan), &(regs[a])));
    }
    mrbc_profile_function_header * fh = profiler_currentfunction(prof);
    if(fh->spMoved) {
      uint32_t retrived_registers = (mrbc_function_header_allocation_retrived_registers_on_callee_regiseter_saving(fh) | 2);
      uint32_t j = fh->spMoved - sizeof(uint32_t);
      for(int i = 1; i < 32; ++i) {
        if(!(retrived_registers & (1 << i))) continue;
        mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_LWSP(i, j));
        dbg_mrbc_prof_print_inst_readable("c.lwsp x%d, %d", i, j);
        j -= sizeof(uint32_t);
      }
      mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_ADDI16SP(fh->spMoved));
      dbg_mrbc_prof_print_inst_readable("c.addi16sp %d", fh->spMoved);
    }
    mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_JR(1));
    dbg_mrbc_prof_print_inst_readable("c.jr ra", 0);
    memset(prof->regsinfo, 0, prof->regsinfo_length);
  }
  // If have a ensure, jump to it.
  if( vm->cur_irep->clen ) {
    const mrbc_irep_catch_handler *handler = find_catch_handler_ensure(vm);
    if( handler ) {
      assert( vm->exception.tt == MRBC_TT_NIL );

      // Save the return value in the last+1 register.
      regs[ vm->cur_irep->nregs ] = regs[a];
      regs[a].tt = MRBC_TT_EMPTY;

      vm->exception.tt = MRBC_TT_RETURN;
      vm->inst = vm->cur_irep->inst + bin_to_uint32(handler->target);
      return;
    }
  }

  // return without anything if top level.
  if( vm->callinfo_tail == NULL ) {
    if (vm->flag_permanence == 1) {
      mrbc_incref(&regs[a]);
    } else {
      mrbc_decref(&regs[0]);
      regs[0] = regs[a];
      regs[a].tt = MRBC_TT_EMPTY;
    }
    vm->flag_preemption = 1;
    vm->flag_stop = 1;
    return;
  }

  // not in initialize method, set return value.
  if( vm->callinfo_tail->method_id != MRBC_SYM(initialize) ) goto SET_RETURN;

  // not called by op_super, ignore return value.
  if( !vm->callinfo_tail->is_called_super ) goto RETURN;

  // set the return value
 SET_RETURN:
  mrbc_decref(&regs[0]);
  regs[0] = regs[a];
  regs[a].tt = MRBC_TT_EMPTY;

 RETURN:
  mrbc_pop_callinfo(vm);
  {
    mrbc_send_inst_bufs_return_t sib = send_inst_bufs(prof, profiler_current(prof));
    if(sib.buffer > (void *)1)
      dbg_dump_riscv_code2(sib.length, sib.buffer);
    if(profiler_pop_stack(prof)) return; // This is end of Copro#run.
    write_send_epilogue(prof, mrbcopro_objman_type2coprotype(vm, &(prof->objMan), regs));
    int ret2 = try_search_or_new_profiling(prof);
    if(ret2 == 0) write_jmp_and_execute_on_ulp(prof);
    else set_prof_entrance_types(prof, ((size_t)regs - (size_t)vm->cur_regs) / sizeof(mrbc_value) + 1);
  }
}


//================================================================
/*! OP_RETURN

  return R[a] (normal)
*/
static inline void op_return( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_B();

  op_return__sub( vm, regs, a, prof );
}


//================================================================
/*! OP_RETURN_BLK

  return R[a] (in-block return)
*/
static inline void op_return_blk( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_B();

  if( mrbc_type(regs[0]) != MRBC_TT_PROC ) {
    op_return__sub( vm, regs, a, prof );
    return;
  }

}

//================================================================
/*! OP_ADD

  R[a] = R[a]+R[a+1]
*/
static inline void op_add( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_B();

  // in case of Integer + Integer
  if( regs[a].tt == MRBC_TT_INTEGER && regs[a+1].tt == MRBC_TT_INTEGER ) {
    struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a, 0);
    
    regs[a].i += regs[a+1].i;
    
    int regA1 = get_allocation(prof, a+1);
    if(regA.src == regA.dst) {
      dbg_mrbc_prof_print_inst_readable("c.add x%d, x%d", regA.dst, regA1);
      mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_ADD(regA.dst, regA1));
    } else {
      dbg_mrbc_prof_print_inst_readable("add x%d, x%d, x%d", regA.dst, regA.src, regA1);
      mrbcopro_vector_append32(vm, &(prof->buf), RISCV_ADD(regA.dst, regA.src, regA1));
    }
    override_register_information(prof, a+1, 0);
    return;
  }

#if MRBC_USE_FLOAT // Floating points are not supported on the coprceossor.
  // in case of Integer + Float
  if( regs[a].tt == MRBC_TT_INTEGER && regs[a+1].tt == MRBC_TT_FLOAT ) {
    mrbc_set_float( &regs[a], regs[a].i + regs[a+1].d );
    return;
  }

  // in case of Float + Integer
  if( regs[a].tt == MRBC_TT_FLOAT && regs[a+1].tt == MRBC_TT_INTEGER ) {
    regs[a].d += regs[a+1].i;
    return;
  }

  // in case of Float + Float
  if( regs[a].tt == MRBC_TT_FLOAT && regs[a+1].tt == MRBC_TT_FLOAT ) {
    regs[a].d += regs[a+1].d;
    return;
  }
#endif

  // other case
  send_by_name( vm, MRBC_SYM(PLUS), a, 1, prof);
}


//================================================================
/*! OP_ADDI

  R[a] = R[a]+mrb_int(b)
*/
static inline void op_addi( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BB();
  
  if( regs[a].tt == MRBC_TT_INTEGER ) {
    struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a, 0);

    regs[a].i += b;
    
    if(b <= 0x1F && regA.src == regA.dst) {
      dbg_mrbc_prof_print_inst_readable("c.addi x%d, %d", regA.dst, b);
      mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_ADDI(regA.dst, b));
    } else {
      dbg_mrbc_prof_print_inst_readable("addi x%d, x%d, %d", regA.dst, regA.src, b);
      mrbcopro_vector_append32(vm, &(prof->buf), RISCV_ADD_IMM(regA.dst, regA.src, b));
    }
    return;
  }

#if MRBC_USE_FLOAT // NOT SUPPORTED ON THE LP COPROCESSOR.
  if( regs[a].tt == MRBC_TT_FLOAT ) {
    regs[a].d += b;
    return;
  }
#endif

  regs[a+1] = mrbc_integer_value(b);

  override_register_information(prof, a+1, 1);
  
  send_by_name(vm, MRBC_SYM(PLUS), a, 1, prof);
}


//================================================================
/*! OP_SUB

  R[a] = R[a]-R[a+1]
*/
static inline void op_sub( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_B();

  // in case of Integer - Integer
  if( regs[a].tt == MRBC_TT_INTEGER && regs[a+1].tt == MRBC_TT_INTEGER ) {
    struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a, 0);

    regs[a].i -= regs[a+1].i;
    
    int regA1 = get_allocation(prof, a+1);
    if(regA.src == regA.dst &&
      RISCV_THESE_REGISTERS_ARE_COMPRESSED_AVAILABLE(regA.dst, regA1)) {
      dbg_mrbc_prof_print_inst_readable("c.sub x%d, x%d", regA.dst, regA1);
      mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_SUB(regA.dst, regA1));
    } else {
      dbg_mrbc_prof_print_inst_readable("sub x%d, x%d, x%d", regA.dst, regA.src, regA1);
      mrbcopro_vector_append32(vm, &(prof->buf), RISCV_SUB(regA.dst, regA.src, regA1));
    }
    override_register_information(prof, a+1, 0);
    return;
  }

#if MRBC_USE_FLOAT
  // in case of Integer - Float
  if( regs[a].tt == MRBC_TT_INTEGER && regs[a+1].tt == MRBC_TT_FLOAT ) {
    mrbc_set_float( &regs[a], regs[a].i - regs[a+1].d );
    return;
  }

  // in case of Float - Integer
  if( regs[a].tt == MRBC_TT_FLOAT && regs[a+1].tt == MRBC_TT_INTEGER ) {
    regs[a].d -= regs[a+1].i;
    return;
  }

  // in case of Float - Float
  if( regs[a].tt == MRBC_TT_FLOAT && regs[a+1].tt == MRBC_TT_FLOAT ) {
    regs[a].d -= regs[a+1].d;
    return;
  }
#endif

  // other case
  send_by_name( vm, MRBC_SYM(MINUS), a, 1, prof);
}


//================================================================
/*! OP_SUBI

  R[a] = R[a]-mrb_int(b)
*/
static inline void op_subi( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BB();

  if( regs[a].tt == MRBC_TT_INTEGER ) {
    struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a, 0);

    regs[a].i -= b;

    if(b <= 0x20 && regA.src == regA.dst) {
      dbg_mrbc_prof_print_inst_readable("c.addi x%d, %d", regA.dst, -b);
      mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_ADDI(regA.dst, -b));
    } else {
      dbg_mrbc_prof_print_inst_readable("addi x%d, x%d, %d", regA.dst, regA.src, -b);
      mrbcopro_vector_append32(vm, &(prof->buf), RISCV_ADD_IMM(regA.dst, regA.src, -b));
    }
    return;
  }
#if MRBC_USE_FLOAT // NOT SUPPORTED ON THE LP COPROCESSOR.
  if( regs[a].tt == MRBC_TT_FLOAT ) {
    regs[a].d -= b;
    return;
  }
#endif

  regs[a+1] = mrbc_integer_value(b);

  override_register_information(prof, b, 1);

  send_by_name(vm, MRBC_SYM(MINUS), a, 1, prof);
}


//================================================================
/*! OP_MUL

  R[a] = R[a]*R[a+1]
*/
static inline void op_mul( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_B();

  // in case of Integer * Integer
  if( regs[a].tt == MRBC_TT_INTEGER && regs[a+1].tt == MRBC_TT_INTEGER ) {
    struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a, 0);
    
    regs[a].i *= regs[a+1].i;

    int regA1 = get_allocation(prof, a+1);
    dbg_mrbc_prof_print_inst_readable("mul x%d, x%d, x%d", regA.dst, regA.src, regA1);
    mrbcopro_vector_append32(vm, &(prof->buf), RISCV_MUL(regA.dst, regA.src, regA1));

    override_register_information(prof, a+1, 0);
    return;
  }

#if MRBC_USE_FLOAT
  // in case of Integer * Float
  if( regs[a].tt == MRBC_TT_INTEGER && regs[a+1].tt == MRBC_TT_FLOAT ) {
    mrbc_set_float( &regs[a], regs[a].i * regs[a+1].d );
    return;
  }

  // in case of Float * Integer
  if( regs[a].tt == MRBC_TT_FLOAT && regs[a+1].tt == MRBC_TT_INTEGER ) {
    regs[a].d *= regs[a+1].i;
    return;
  }

  // in case of Float * Float
  if( regs[a].tt == MRBC_TT_FLOAT && regs[a+1].tt == MRBC_TT_FLOAT ) {
    regs[a].d *= regs[a+1].d;
    return;
  }
#endif

  // other case
  send_by_name( vm, MRBC_SYM(MUL), a, 1, prof);
}


//================================================================
/*! OP_DIV

  R[a] = R[a]/R[a+1]
*/
static inline void op_div( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_B();

  // in case of Integer / Integer
  if( regs[a].tt == MRBC_TT_INTEGER && regs[a+1].tt == MRBC_TT_INTEGER ) {
    struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a, 0);
    
    mrbc_int_t v0 = regs[a].i;
    mrbc_int_t v1 = regs[a+1].i;

    if( v1 == 0 ) {
      mrbc_raise(vm, MRBC_CLASS(ZeroDivisionError), 0 );
      return;
    }

    mrbc_int_t ret = v0 / v1;
    mrbc_int_t mod = v0 % v1;

    if( (mod != 0) && ((v0 ^ v1) < 0) ) ret -= 1;

    regs[a].i = ret;

    int regA1 = get_allocation(prof, a + 1);
    dbg_mrbc_prof_print_inst_readable("div x%d, x%d, x%d", regA.dst, regA.src, regA1);
    mrbcopro_vector_append32(vm, &(prof->buf), RISCV_DIV(regA.dst, regA.src, regA1));
    override_register_information(prof, a+1, 0);
    return;
  }

#if MRBC_USE_FLOAT
  // in case of Integer / Float
  if( regs[a].tt == MRBC_TT_INTEGER && regs[a+1].tt == MRBC_TT_FLOAT ) {
    mrbc_set_float( &regs[a], regs[a].i / regs[a+1].d );
    return;
  }

  // in case of Float / Integer
  if( regs[a].tt == MRBC_TT_FLOAT && regs[a+1].tt == MRBC_TT_INTEGER ) {
    regs[a].d /= regs[a+1].i;
    return;
  }

  // in case of Float / Float
  if( regs[a].tt == MRBC_TT_FLOAT && regs[a+1].tt == MRBC_TT_FLOAT ) {
    regs[a].d /= regs[a+1].d;
    return;
  }
#endif
  // other case
  send_by_name( vm, MRBC_SYM(DIV), a, 1, prof);
}


//================================================================
/*! OP_EQ

  R[a] = R[a]==R[a+1]
*/
static inline void op_eq( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_B();

  if (regs[a].tt == MRBC_TT_OBJECT) {
    send_by_name(vm, MRBC_SYM(EQ_EQ), a, 1, prof);
    return;
  }

  int result = mrbc_compare(&regs[a], &regs[a+1]);

  mrbc_decref(&regs[a]);
  regs[a].tt = result ? MRBC_TT_FALSE : MRBC_TT_TRUE;

  // TODO: Super-instruction/Op fusion?
  struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a, 0);
  int rs2 = get_allocation(prof, a + 1);
  if(regA.src == regA.dst && RISCV_THESE_REGISTERS_ARE_COMPRESSED_AVAILABLE(regA.dst, rs2)) {
    dbg_mrbc_prof_print_inst_readable("c.xor x%d, x%d", regA.dst, rs2);
    mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_XOR(regA.dst, rs2));
  } else {
    dbg_mrbc_prof_print_inst_readable("xor x%d, x%d, x%d", regA.dst, regA.src, rs2);
    mrbcopro_vector_append32(vm, &(prof->buf),  RISCV_XOR(regA.dst, regA.src, rs2));
  }
  dbg_mrbc_prof_print_inst_readable("sltiu x%d, x%d, 1", regA.dst, regA.dst);
  mrbcopro_vector_append32(vm, &(prof->buf), RISCV_LESS_THAN_UNSIGNED_IMM(regA.dst, regA.dst, 1));

  override_register_information(prof, a+1, 0);
}


//================================================================
/*! OP_LT

  R[a] = R[a]<R[a+1]
*/
static inline void op_lt( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_B();

  if (regs[a].tt == MRBC_TT_OBJECT) {
    send_by_name(vm, MRBC_SYM(LT), a, 1, prof);
    return;
  }

  int result = mrbc_compare(&regs[a], &regs[a+1]);

  mrbc_decref(&regs[a]);
  regs[a].tt = result < 0 ? MRBC_TT_TRUE : MRBC_TT_FALSE;

  // TODO: Super-instruction/Op fusion?
  struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a, 0);
  int rs2 = get_allocation(prof, a + 1);
  dbg_mrbc_prof_print_inst_readable("slt x%d, x%d, x%d", regA.dst, regA.src, rs2);
  mrbcopro_vector_append32(vm, &(prof->buf), RISCV_LESS_THAN(regA.dst, regA.src, rs2));
  override_register_information(prof, a+1, 0);
}


//================================================================
/*! OP_LE

  R[a] = R[a]<=R[a+1]
*/
static inline void op_le( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_B();

  if (regs[a].tt == MRBC_TT_OBJECT) {
    send_by_name(vm, MRBC_SYM(LT_EQ), a, 1, prof);
    return;
  }

  int result = mrbc_compare(&regs[a], &regs[a+1]);

  mrbc_decref(&regs[a]);
  regs[a].tt = result <= 0 ? MRBC_TT_TRUE : MRBC_TT_FALSE;

  // TODO: Super-instruction/Op fusion?
  struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a, 0);
  int rs2 = get_allocation(prof, a + 1);
  dbg_mrbc_prof_print_inst_readable("slt x%d, x%d, x%d", regA.dst, rs2, regA.src);
  dbg_mrbc_prof_print_inst_readable("xori x%d, x%d, 1", regA.dst, regA.dst);
  mrbcopro_vector_append32(vm, &(prof->buf), RISCV_LESS_THAN(regA.dst, rs2, regA.src));
  mrbcopro_vector_append32(vm, &(prof->buf), RISCV_XOR_IMM(regA.dst, regA.dst, 1));
  override_register_information(prof, a+1, 0);
}


//================================================================
/*! OP_GT

  R[a] = R[a]>R[a+1]
*/
static inline void op_gt( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_B();

  if (regs[a].tt == MRBC_TT_OBJECT) {
    send_by_name(vm, MRBC_SYM(GT), a, 1, prof);
    return;
  }

  int result = mrbc_compare(&regs[a], &regs[a+1]);

  mrbc_decref(&regs[a]);
  regs[a].tt = result > 0 ? MRBC_TT_TRUE : MRBC_TT_FALSE;

  // TODO: Super-instruction/Op fusion?
  struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a, 0);
  int rs2 = get_allocation(prof, a + 1);
  dbg_mrbc_prof_print_inst_readable("slt x%d, x%d, x%d", regA.dst, rs2, regA.src);
  mrbcopro_vector_append32(vm, &(prof->buf), RISCV_LESS_THAN(regA.dst, rs2, regA.src));
  override_register_information(prof, a+1, 0);
}


//================================================================
/*! OP_GE

  R[a] = R[a]>=R[a+1]
*/
static inline void op_ge( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_B();

  if (regs[a].tt == MRBC_TT_OBJECT) {
    send_by_name(vm, MRBC_SYM(GT_EQ), a, 1, prof);
    return;
  }

  int result = mrbc_compare(&regs[a], &regs[a+1]);

  mrbc_decref(&regs[a]);
  regs[a].tt = result >= 0 ? MRBC_TT_TRUE : MRBC_TT_FALSE;

  // TODO: Super-instruction/Op fusion?
  struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a, 0);
  int rs2 = get_allocation(prof, a + 1);
  dbg_mrbc_prof_print_inst_readable("slt x%d, x%d, x%d", regA.dst, regA.src, rs2);
  dbg_mrbc_prof_print_inst_readable("xori x%d, x%d, 1", regA.dst, regA.dst);
  mrbcopro_vector_append32(vm, &(prof->buf), RISCV_LESS_THAN(regA.dst, regA.src, rs2));
  mrbcopro_vector_append32(vm, &(prof->buf), RISCV_XOR_IMM(regA.dst, regA.dst, 1));
  override_register_information(prof, a+1, 0);
}


//================================================================
/*! OP_SYMBOL

  R[a] = intern(Pool[b])
*/
static inline void op_symbol( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BB();

  const char *p = (const char *)mrbc_irep_pool_ptr(vm->cur_irep, b);
  mrbc_sym sym_id = mrbc_str_to_symid( p+3 );	// 3 is TT and length
  if( sym_id < 0 ) {
    mrbc_raise(vm, MRBC_CLASS(Exception), "Overflow MAX_SYMBOLS_COUNT");
    return;
  }

  mrbc_decref(&regs[a]);
  regs[a] = mrbc_symbol_value( sym_id );

  write_immediate(prof, a, sym_id);
}


//================================================================
/*! OP_STRING

  R[a] = str_dup(Pool[b])
*/
static inline void op_string( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{
  FETCH_BB();

  mrbc_decref(&regs[a]);
  regs[a] = mrbc_irep_pool_value(vm, b);

  gen_read_const_object(vm, regs, prof, NULL, a);
}


//================================================================
/*! OP_OCLASS

  R[a] = ::Object
*/
static inline void op_oclass( mrbc_vm *vm, mrbc_value *regs, mrbc_profile_profiler * prof EXT )
{ 
  FETCH_B();

  mrbc_decref(&regs[a]);
  regs[a].tt = MRBC_TT_CLASS;
  regs[a].cls = MRBC_CLASS(Object);

  write_immediate(prof, a, (uint32_t)(regs[a].cls));
}

#if !defined(MRBC_SUPPORT_OP_EXT)
//================================================================
/*! OP_EXTn

  make 1st operand (a) 16bit
  make 2nd operand (b) 16bit
  make 2nd operand (b) 16bit
*/
static inline void op_ext( mrbc_vm *vm, mrbc_value *regs EXT )
{
  FETCH_Z();
  mrbc_raise(vm, MRBC_CLASS(Exception),
	     "Not support op_ext. Re-compile with MRBC_SUPPORT_OP_EXT");
}
#endif


//================================================================
/*! OP_STOP

  stop VM
*/
static inline void op_stop( mrbc_vm *vm, mrbc_value *regs EXT )
{
  FETCH_Z();

  vm->flag_preemption = 1;
  vm->flag_stop = 1;
  vm->inst--;
}


//================================================================
/* Unsupported opecodes
*/
static inline void op_unsupported( mrbc_vm *vm, mrbc_value *regs EXT )
{
  mrbc_raisef( vm, MRBC_CLASS(Exception),
	       "Unimplemented opcode (0x%02x) found", *(vm->inst - 1));
}
#undef EXT

#define PROFILE_RETURN(a) {retVal=a;goto RETURN;}

//================================================================
/*! Fetch a bytecode and execute

  @param  vm	A pointer to VM.
  @retval 4	program done.
  @retval 2	exception occurred.
*/
int profile( struct VM *vm, mrbc_callinfo * callTop)
{
#if defined(MRBC_SUPPORT_OP_EXT)
  int ext = 0;
#define EXT , ext
#else
#define EXT
#endif
  int retVal = 0;
  mrbc_profile_profiler * prof = (mrbc_profile_profiler *)mrbcopro_alloc(vm, sizeof(mrbc_profile_profiler));
  { // TODO: OoM Error
    memset(prof, 0, sizeof(mrbc_profile_profiler));
    prof->vm = vm;
#if MRBC_PROF_DBG_ENABLE
    prof->callTop = vm->callinfo_tail;
#endif
    mrbcopro_vector_new(vm, &(prof->buf), sizeof(uint16_t) * INST_BUF_SIZE);
    prof->stacktop = &(prof->stack);
    mrbcopro_globalman_new(vm, &(prof->globalMan));
    mrbcopro_objman_new(vm, &(prof->objMan));
  }
  while( 1 ) {
    mrbc_value *regs = vm->cur_regs;
    prof->lastInst = vm->inst;
    uint8_t op = *vm->inst++;		// Dispatch
    switch( op ) {
    case OP_NOP:        op_nop        (vm, regs EXT); break;
    case OP_MOVE:       op_move       (vm, regs, prof EXT); break;
    case OP_LOADL:      op_loadl      (vm, regs, prof EXT); break;
    case OP_LOADI:      op_loadi      (vm, regs, prof EXT); break;
    case OP_LOADINEG:   op_loadineg   (vm, regs, prof EXT); break;
    case OP_LOADI__1:   // fall through
    case OP_LOADI_0:    // fall through
    case OP_LOADI_1:    // fall through
    case OP_LOADI_2:    // fall through
    case OP_LOADI_3:    // fall through
    case OP_LOADI_4:    // fall through
    case OP_LOADI_5:    // fall through
    case OP_LOADI_6:    // fall through
    case OP_LOADI_7:    op_loadi_n    (vm, regs, prof EXT); break;
    case OP_LOADI16:    op_loadi16    (vm, regs, prof EXT); break;
    case OP_LOADI32:    op_loadi32    (vm, regs, prof EXT); break;
    case OP_LOADSYM:    op_loadsym    (vm, regs, prof EXT); break;
    case OP_LOADNIL:    op_loadnil    (vm, regs, prof EXT); break;
    case OP_LOADSELF:   op_loadself   (vm, regs, prof EXT); break;
    case OP_LOADT:      op_loadt      (vm, regs, prof EXT); break;
    case OP_LOADF:      op_loadf      (vm, regs, prof EXT); break;
    case OP_GETGV:      op_getgv      (vm, regs, prof EXT); break;
    case OP_SETGV:      op_setgv      (vm, regs, prof EXT); break;
    case OP_GETSV:      op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_SETSV:      op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_GETIV:      op_getiv      (vm, regs, prof EXT); break;
    case OP_SETIV:      op_setiv      (vm, regs, prof EXT); break;
    case OP_GETCV:      op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_SETCV:      op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_GETCONST:   op_getconst   (vm, regs, prof EXT); break;
    case OP_SETCONST:   op_setconst   (vm, regs EXT); break;
    case OP_GETMCNST:   op_getmcnst   (vm, regs, prof EXT); break;
    case OP_SETMCNST:   op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_GETUPVAR:   op_getupvar   (vm, regs, prof EXT); break;
    case OP_SETUPVAR:   op_setupvar   (vm, regs, prof EXT); break;
    case OP_GETIDX:     op_getidx     (vm, regs, prof EXT); break;
    case OP_SETIDX:     op_setidx     (vm, regs, prof EXT); break;
    case OP_JMP:        op_jmp        (vm, regs, prof EXT); break;
    case OP_JMPIF:      op_jmpif      (vm, regs, prof EXT); break;
    case OP_JMPNOT:     op_jmpnot     (vm, regs, prof EXT); break;
    case OP_JMPNIL:     op_jmpnil     (vm, regs, prof EXT); break;
    case OP_JMPUW:      op_jmpuw      (vm, regs, prof EXT); break; // give up method
    case OP_EXCEPT:     op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_RESCUE:     op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_RAISEIF:    op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_SSEND:      op_ssend      (vm, regs, prof EXT); break;
    case OP_SSENDB:     op_ssendb     (vm, regs, prof EXT); break;
    case OP_SEND:       op_send       (vm, regs, prof EXT); break;
    case OP_SENDB:      op_sendb      (vm, regs, prof EXT); break;
    case OP_CALL:       op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_SUPER:      op_super      (vm, regs EXT); break;
    case OP_ARGARY:     op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_ENTER:      op_enter      (vm, regs, prof EXT); break;
    case OP_KEY_P:      op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_KEYEND:     op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_KARG:       op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_RETURN:     op_return     (vm, regs, prof EXT); break;
    case OP_RETURN_BLK: op_return_blk (vm, regs, prof EXT); break;
    case OP_BREAK:      op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_BLKPUSH:    op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_ADD:        op_add        (vm, regs, prof EXT); break;
    case OP_ADDI:       op_addi       (vm, regs, prof EXT); break;
    case OP_SUB:        op_sub        (vm, regs, prof EXT); break;
    case OP_SUBI:       op_subi       (vm, regs, prof EXT); break;
    case OP_MUL:        op_mul        (vm, regs, prof EXT); break;
    case OP_DIV:        op_div        (vm, regs, prof EXT); break;
    case OP_EQ:         op_eq         (vm, regs, prof EXT); break;
    case OP_LT:         op_lt         (vm, regs, prof EXT); break;
    case OP_LE:         op_le         (vm, regs, prof EXT); break;
    case OP_GT:         op_gt         (vm, regs, prof EXT); break;
    case OP_GE:         op_ge         (vm, regs, prof EXT); break;
    case OP_ARRAY:      op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_ARRAY2:     op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_ARYCAT:     op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_ARYPUSH:    op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_ARYDUP:     op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_AREF:       op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_ASET:       op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_APOST:      op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_INTERN:     op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_SYMBOL:     op_symbol     (vm, regs, prof EXT); break;
    case OP_STRING:     op_string     (vm, regs, prof EXT); break;
    case OP_STRCAT:     op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_HASH:       op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_HASHADD:    op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_HASHCAT:    op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_LAMBDA:     op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_BLOCK:      op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_METHOD:     op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_RANGE_INC:  op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_RANGE_EXC:  op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_OCLASS:     op_oclass     (vm, regs, prof EXT); break;
    case OP_CLASS:      op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_MODULE:     op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_EXEC:       op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_DEF:        op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_ALIAS:      op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_UNDEF:      op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_SCLASS:     op_unsupported(vm, regs EXT); break;
    case OP_TCLASS:     op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_DEBUG:      op_unsupported(vm, regs EXT); break; // not implemented.
    case OP_ERR:        op_unsupported(vm, regs EXT); break; // not implemented.
#if defined(MRBC_SUPPORT_OP_EXT)
    case OP_EXT1:       ext = 1; continue;
    case OP_EXT2:       ext = 2; continue;
    case OP_EXT3:       ext = 3; continue;
#else
    case OP_EXT1:       // fall through
    case OP_EXT2:       // fall through
    case OP_EXT3:       op_ext        (vm, regs EXT); break;
#endif
    case OP_STOP:       op_stop       (vm, regs EXT); break;
    default:		op_unsupported(vm, regs EXT); break;
    } // end switch.
#undef EXT
#if defined(MRBC_SUPPORT_OP_EXT)
    ext = 0;
#endif

    if( !vm->flag_preemption ) {
      if(callTop == vm->callinfo_tail)
        PROFILE_RETURN(4);
      continue;	// execute next ope code.
    }
    if( !mrbc_israised(vm) )
      PROFILE_RETURN(vm->flag_stop); // normal return.

    // Handle exception
    vm->flag_preemption = 0;
    const mrbc_irep_catch_handler *handler;

    while (1)
    {
      const mrbc_irep *irep = vm->cur_irep;
      const mrbc_irep_catch_handler *catch_table =
          (const mrbc_irep_catch_handler *)(irep->inst + irep->ilen);
      uint32_t inst = vm->inst - irep->inst;
      int cnt = irep->clen;

      for (cnt--; cnt >= 0; cnt--)
      {
        handler = catch_table + cnt;
        if ((bin_to_uint32(handler->begin) < inst) &&
            (inst <= bin_to_uint32(handler->end)))
          goto JUMP_TO_HANDLER;
      }

      if (!vm->callinfo_tail)
        PROFILE_RETURN(2); // return due to exception.

      mrbc_pop_callinfo(vm);
    }

  JUMP_TO_HANDLER:
    // jump to handler (rescue or ensure).
    vm->inst = vm->cur_irep->inst + bin_to_uint32(handler->target);
    if(callTop == vm->callinfo_tail)
      PROFILE_RETURN(4);
  }
  RETURN:
    profile_dtor(vm, prof);
    return retVal;
}
