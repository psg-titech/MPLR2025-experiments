#include <alloca.h>
#include <stdlib.h>
#include <string.h>
#include "../../../src/vm.h"
#include "alloc.h"
#include "stack.h"
#include "dbg.h"
#include "symbol.h"
#include "mymem.h"
#include "value.copro.h"
#include "riscv_writer.h"
#include "opcode.h"
#include "inst_skip.h"
#include "freertos/FreeRTOS.h"

void profiler_append_and_set_current(mrbc_profile_profiler * prof, mrbc_profile_basic_block * v) {
  mrbc_profile_function_header * fh = prof->stack.functions_current;
  v->next = fh->blocks;
  fh->blocks = v;
  dbg_mrbc_prof_printf("registered: function=%p basicblock=%p", fh, v);
  prof->stack.current = v;
}

int profiler_push_stack(mrbc_profile_profiler * prof) {
  mrbc_profile_profiler_stack * s = (mrbc_profile_profiler_stack *)mrbcopro_alloc(prof->vm, sizeof(mrbc_profile_profiler_stack));
  if(s == NULL) return 1;
  if(prof->stack.prev != NULL) prof->stack.prev->next = s;
  memcpy(s, &(prof->stack), sizeof(mrbc_profile_profiler_stack));
  memset(&(prof->stack), 0, sizeof(mrbc_profile_profiler_stack));
  if(prof->stacktop == &prof->stack) prof->stacktop = s;
  prof->stack.prev = s;
  s->next = &prof->stack;
  return 0;
}

int profiler_pop_stack(mrbc_profile_profiler * prof) {
  mrbc_profile_profiler_stack * prev = prof->stack.prev;
  if(prev == NULL) return 1;
  memcpy(&prof->stack, prev, sizeof(mrbc_profile_profiler_stack));
  if(prof->stack.prev != NULL) prof->stack.prev->next = &prof->stack;
  if(prof->stacktop == prev) prof->stacktop = &prof->stack;
  prof->stack.next = NULL;
  mrbcopro_free(prof->vm, prev);
  return 0;
}

#if MRBC_PROF_DBG_ENABLE
static void dbg_mrbc_prof_print_callinfo(mrbc_callinfo * ci) {
  dbg_mrbc_prof_printf("--- CALLINFO(addr = %p) ---", ci);
  dbg_mrbc_prof_printf("cur_irep = %p", ci->cur_irep);
  dbg_mrbc_prof_printf("inst = %p", ci->inst);
  dbg_mrbc_prof_printf("cur_regs = %p", ci->cur_regs);
  if(ci->target_class != NULL)
    dbg_mrbc_prof_printf("target_class = %p (name:%s)", ci->target_class, symid_to_str(ci->target_class->sym_id));
  else
    dbg_mrbc_prof_printf("target_class = %p", ci->target_class);
  if(ci->own_class != NULL)
    dbg_mrbc_prof_printf("own_class = %p (name:%s)", ci->own_class, symid_to_str(ci->own_class->sym_id));
  else
    dbg_mrbc_prof_printf("own_class = %p", ci->own_class);
  dbg_mrbc_prof_printf("karg_keep = %p", ci->karg_keep);
  if(ci->method_id != 0)
    dbg_mrbc_prof_printf("method_id = %p (name:%s)", ci->method_id, symid_to_str(ci->method_id));
  else
    dbg_mrbc_prof_printf("method_id = %p", ci->method_id);
  dbg_mrbc_prof_printf("reg_offset = %d", ci->reg_offset);
  dbg_mrbc_prof_printf("n_args = %d", ci->n_args);
  dbg_mrbc_prof_printf("is_called_super = %d", ci->is_called_super);
  dbg_mrbc_prof_print("-----------------------------------");
}
static void dbg_mrbc_prof_print_registerfile(mrbc_value * l, mrbc_value * r) {
  dbg_mrbc_prof_print("=register file=");
  for(; l <= r; ++l)
    dbg_mrbc_prof_printf("%p | %p (type: %d %s, rc: %d)", (size_t)l, l->i, l->tt, dbg_mrbc_to_string(l->tt), l->tt <= MRBC_TT_INC_DEC_THRESHOLD ? 0 : l->obj->ref_count);
}
#else
#define dbg_mrbc_prof_print_callinfo(ci) {}
#define dbg_mrbc_prof_print_registerfile(l, r) {}
#endif

#define MRBC_PROF_STACK_DBG_PRINT_ENABLE 0
#if MRBC_PROF_DBG_ENABLE && MRBC_PROF_STACK_DBG_PRINT_ENABLE
void dbg_mrbc_prof_print_state(mrbc_profile_profiler * prof) {
  mrbc_vm * vm = prof->vm;
  mrbc_value * VM_REGFILE_RIGHT = &vm->regs[vm->regs_size];
  dbg_mrbc_prof_print("\n\nDUMP FOR DEBUGGING PURPOSE.");
  mrbc_profile_profiler_stack * st = &prof->stack;
  mrbc_callinfo * ci = vm->callinfo_tail;
  for(int i = 0; ci != NULL || st != NULL; i++) {
    dbg_mrbc_prof_printf("{%d}",i);
    if(ci != NULL) {
      dbg_mrbc_prof_print_callinfo(ci);
      ci = ci->prev;
    }
    if(st != NULL) {
      dbg_mrbc_prof_printf("st->functions_current = %p", st->functions_current);
      dbg_mrbc_prof_printf("st->current = %p", st->current);
      dbg_mrbc_prof_printf("st->functions_current->cur_irep = %p", st->functions_current->irep);
      if(st->current->lastInstMRubyPtr != NULL)
        dbg_mrbc_prof_printf("st->current->lastinst = %p", st->current->lastInstMRubyPtr + inst_skipper_inst_len[*st->current->lastInstMRubyPtr] + sizeof(uint8_t));
      // dbg_mrbc_prof_printf("st->functions_current->own_class = %p", st->functions_current->own_class);
      st = st->prev;
    }
  }
  dbg_mrbc_prof_print("=vm dump=");
  dbg_mrbc_prof_printf("vm->regs=%p", vm->regs);
  dbg_mrbc_prof_printf("vm->cur_regs=%p", vm->cur_regs);
  dbg_mrbc_prof_printf("last reg addr (incl.)=%p", VM_REGFILE_RIGHT - 1);
  dbg_mrbc_prof_printf("vm->inst=%p", vm->inst);
  dbg_mrbc_prof_printf("vm->top_irep=%p", vm->top_irep);
  dbg_mrbc_prof_printf("vm->cur_irep=%p", vm->cur_irep);

  dbg_mrbc_prof_print_registerfile(prof->callTop->cur_regs, &(vm->cur_regs[vm->cur_irep->nregs-1]));
  dbg_mrbc_prof_print("END OF DEBUG DUMP.\n\n");
}
#else
#define dbg_mrbc_prof_print_state(prof) {}
#endif

uint32_t analyze_register_saving(mrbc_profile_function_header * fh, int a, int narg) {
  uint32_t ret = 0;
  int first_loop_max = a + narg + 1;
  uint32_t DONTSAVE = RISCV_CALLEE_SAVE_REGISTERS | /* x0 */ 1;
  for(int i = 0; i < first_loop_max; ++i) {
    int allocatedReg = mrbc_function_header_allocation(fh, i);
    int allocatedReg_bit = 1 << allocatedReg;
    if(i < a){
      if((DONTSAVE & allocatedReg_bit) != 0) continue;
    } else if(allocatedReg <= RISCV_ARGS_REGISTER(i-a-1) || allocatedReg > RISCV_ARGS_REGISTER(narg)) continue;
    ret |= allocatedReg_bit;
  }
  return ret;
}

int estimate_stack_move_size(int save_register_size /* < 2^5 */) {
  int lowerbit = save_register_size & 0x3;
  if(lowerbit != 0)
    save_register_size = (save_register_size - lowerbit) + 4;
  return save_register_size * 4;
}

const int __get_a_method_and_narg_method_ids[] =
{ MRBC_SYMID_PLUS, MRBC_SYMID_PLUS, MRBC_SYMID_MINUS, MRBC_SYMID_MINUS, MRBC_SYMID_MUL, MRBC_SYMID_DIV,
  MRBC_SYMID_EQ_EQ, MRBC_SYMID_LT, MRBC_SYMID_LT_EQ, MRBC_SYMID_GT, MRBC_SYMID_GT_EQ };

struct get_a_method_and_narg_return_t {
  mrbc_sym method_id; uint8_t narg; uint8_t a;
};
/// @brief Analyzes the OP_SEND, etc.
/// @param inst The instruction
/// @param cur_irep The mrbc_irep of inst
/// @return returns "a", "b", and "n" of send a, b, n
struct get_a_method_and_narg_return_t
get_a_method_and_narg(const uint8_t * inst, const mrbc_irep * cur_irep) {
  int _narg, a;
  struct get_a_method_and_narg_return_t ret;
  switch(inst[0]) {
    case OP_GETIDX: ret.method_id = MRBC_SYMID_BL_BR; _narg = 1; break;
    case OP_SETIDX: ret.method_id = MRBC_SYMID_BL_BR_EQ; _narg = 2; break;
    case OP_SSEND...OP_SENDB: ret.method_id = mrbc_irep_symbol_id(cur_irep, inst[2]); _narg = inst[3]; break;
    // case OP_SUPER: break; // TODO: TBD
    case OP_ADD...OP_GE: ret.method_id = __get_a_method_and_narg_method_ids[*inst - OP_ADD]; _narg = 1; break;
    default: a = 0; _narg = 0; goto end;
  }
  a = inst[1];
end:
  ret.narg = _narg & 0xF;
  ret.a = a;
  return ret;
}

uint32_t mrbcopro_interpreter_value_to_coprocessor_value(struct VM * vm, mrbc_value * v, mrbcopro_objman_t * objman) {
  switch(v->tt) {
    case MRBC_TT_TRUE: return 1;
    case MRBC_TT_FALSE: return 0;
    case MRBC_TT_NIL: return 0;
    case MRBC_TT_OBJECT:
    case MRBC_TT_ARRAY:
    case MRBC_TT_STRING:
      return mrbcopro_objman_to3(vm, objman, v);
    default: return (uint32_t)(v->i);
  }
}

static uint32_t cpy_stack_interpreter_to_registers(struct VM * vm, int regLen, mrbc_value * curregs, uint8_t * allocations, uint32_t dst[32], mrbcopro_objman_t * objman) {
  uint32_t regsused = 0;
  for(int r = 0; r < regLen; r++) {
    uint8_t ri = allocations[r];
    regsused |= 1 << ri;
    if(ri == 0) continue;
    uint32_t val = mrbcopro_interpreter_value_to_coprocessor_value(vm, &(curregs[r]), objman);
    dst[ri] = val;
    dbg_mrbc_prof_printf("Register %d x%d: %s %p->%p", r, ri, dbg_mrbc_to_string(curregs[r].tt), curregs[r].i, val);
  }
  return regsused;
}

extern void * ulp_mrbc_sp_bottom;
extern void * ulp___stack_top; // the beggining of the stack.

struct callinfo_main_to_copro_t {
  mrbc_value * curregs;
};

#if CONFIG_IDF_TARGET_ESP32S3
#define ULP_STACK_TOP ((size_t)0x50000000 + 8176)
#else
#define ULP_STACK_TOP ((size_t)(&ulp___stack_top) - 192)
#endif
int main_to_copro(mrbc_profile_profiler * prof, uint32_t regs[33]) {
  struct VM * vm = prof->vm;
  int is_func_head = *(prof->lastInst) == OP_ENTER;
  int copro_stack = (int)ULP_STACK_TOP;
  int calls = -1;
  for(mrbc_profile_profiler_stack * ps = prof->stacktop; ps != NULL; ps = ps->next, calls++) {}
  dbg_mrbc_prof_printf("miain_to_copro %d %p", calls, copro_stack);
  if(calls <= 0) return  MRBC_COPRO_NATIVE_PTR_TO_PTR(ULP_STACK_TOP);
  struct callinfo_main_to_copro_t * callinfos = calls > 8 ?
          (struct callinfo_main_to_copro_t *)mrbcopro_alloc(vm, sizeof(struct callinfo_main_to_copro_t) * calls) :
          (struct callinfo_main_to_copro_t *)alloca(sizeof(struct callinfo_main_to_copro_t) * calls);
  {
    mrbc_callinfo * ci = vm->callinfo_tail;
    for(int c = calls - 1; c >= 0; ci = ci->prev, c--) {
      callinfos[c].curregs = ci->cur_regs;
    }
  }
  
  mrbc_profile_profiler_stack * ps = prof->stacktop;
  uint32_t regs_used = 0;
  for(int c = 0; c < calls; ++c) {
    struct callinfo_main_to_copro_t * callinfo = &callinfos[c];
      // regs[2] = (uint32_t)((size_t)(&(callinfo.pstack->callsite_jmp[1])));
    struct get_a_method_and_narg_return_t a_and_narg = get_a_method_and_narg(ps->current->lastInstMRubyPtr, ps->functions_current->irep);
    dbg_mrbc_prof_printf("A AND NARG: %d, %d, method_id: %s", a_and_narg.a, a_and_narg.narg, symid_to_str(a_and_narg.method_id));
    regs_used |= cpy_stack_interpreter_to_registers(vm, a_and_narg.a, callinfo->curregs, mrbc_function_header_allocation_head(ps->functions_current), regs, &(prof->objMan));
    regs[1] = (uint32_t)ps->current->returnPos;
    int caller_save = analyze_register_saving(ps->functions_current, a_and_narg.a, a_and_narg.narg);
    int caller_save_amount = __builtin_popcount(caller_save);
    if(caller_save_amount > 0) caller_save_amount = estimate_stack_move_size(caller_save_amount);
    copro_stack = copro_stack - caller_save_amount;
    for(int i = 1; i < 32; ++i) if((caller_save & (1 << i)) != 0) {
      dbg_mrbc_prof_printf("[caller saved] stack addr: %p <- x%d", &((uint32_t *)copro_stack)[i], i);
      ((uint32_t *)copro_stack)[i] = regs[i];
    }

    mrbc_profile_profiler_stack * psnext = ps->next;
    if(psnext != NULL && psnext->functions_current->spMoved != 0
      && (!is_func_head || c != (calls-1))) {
      uint32_t retrived_registers = mrbc_function_header_allocation_retrived_registers_on_callee_regiseter_saving(ps->functions_current);
      *((uint32_t *)(copro_stack - 4)) = regs[1]; // RA
      dbg_mrbc_prof_printf("[callee saved] stack addr: %p <- ra %p", copro_stack - 4, regs[1]);
      for(int i = 1, j = -2; i < 32; ++i) {
        if(!(retrived_registers & (1 << i))) continue;
        // regs[i] = copro_stack[j--];
        dbg_mrbc_prof_printf("[callee saved] stack addr: %p <- x%d %p", copro_stack + (j*4), i, regs[i]);
        *((uint32_t *)(copro_stack + (j*4))) = regs[i];
        j--;
      }
      copro_stack -= psnext->functions_current->spMoved;
    }
    ps = psnext;
  }
  regs[32] = regs_used;
  if(calls > 8) mrbcopro_free(vm, callinfos);
  return MRBC_COPRO_NATIVE_PTR_TO_PTR(copro_stack);
}

/// @brief Restores callee-saved registers
/// @param cur Stack (source)
/// @param fh Function Header(currently.)
/// @param hardware_registers Hardware registers (destination)
/// @return the new stack iterator
uint32_t * restore_callee_saved(uint32_t * cur, mrbc_profile_function_header * fh, uint32_t hardware_registers[32]) {
// This function returns cur. Thus, the first argument should be cur because of the code size.
  if(fh->spMoved == 0) return cur;
  cur = (uint32_t *)((size_t)cur + fh->spMoved);
  uint32_t saved_regs = mrbc_function_header_allocation_retrived_registers_on_callee_regiseter_saving(fh) | 2;
  int j = -1;
  for(int i = 1; i < 32; ++i) { // zero register is not stored.
    if((saved_regs & (1 << i)) == 0) continue;
    hardware_registers[i] = cur[j];
    dbg_mrbc_prof_printf("[restore_callee_saved] restore callee saved: %p -> x%d value: %p", &cur[j], i, cur[j]);
    j--;
  }
  return cur;
}

/// @brief Restores callee-saved registers
/// @param cur Stack (soruce)
/// @param fh Function Header(currently.)
/// @param a_and_narg "a" and "n" in send a, b, n
/// @param hardware_registers Hardware registers (destination)
/// @return the new stack iterator
uint32_t * restore_caller_saved(uint32_t * cur, mrbc_profile_function_header * fh, struct get_a_method_and_narg_return_t a_and_narg, uint32_t hardware_registers[32]) {
  uint32_t caller_save = analyze_register_saving(fh, a_and_narg.a, a_and_narg.narg);
  if(caller_save == 0) return cur;
  for(int loc_cur = -1, i = 1; i < 32; ++i) {
    if((caller_save & (1 << i)) == 0) continue;
    hardware_registers[i] = cur[loc_cur];
    dbg_mrbc_prof_printf("[restore_caller_saved] restore caller saved: %p -> x%d", &cur[loc_cur], i);
    loc_cur--;
  }
  return cur - estimate_stack_move_size(__builtin_popcount(caller_save));
}

/// @brief Copy to the interpreter stack.
/// @param vm the virtual machine
/// @param objman the object manager for address translation.
/// @param interpreter_stack_cur interpreter register file (destination)
/// @param interpreter_stack_sentinel the bound of intepreter register file
/// @param bb basic block
/// @param regLen the length of register (typically, reglen or "a" in send a, b, n)
/// @param hardware_registers hardware registers
/// @param x11 the mrbc_copro_vtype for unknown typed register.
/// @return the new iterator of the interpreter register file
static mrbc_value * cpy_to_interpreter_stack(struct VM * vm, mrbcopro_objman_t * objman, mrbc_value * interpreter_stack_cur, mrbc_value * interpreter_stack_sentinel,
  mrbc_profile_function_header * fh, mrbc_profile_basic_block * bb, int regLen, uint32_t hardware_registers[32], mrbc_copro_vtype x11) {
  mrbc_value * cur = (interpreter_stack_cur - regLen);
  if(cur < interpreter_stack_sentinel) return NULL;
  dbg_mrbc_prof_printf("[cpy_to_interpreter_stack] regLen = %d", regLen);
  for(int i = 0; i < regLen; ++i) {
    uint32_t val = hardware_registers[mrbc_function_header_allocation(fh, i)];
    mrbc_copro_vtype ty = mrbc_prof_get_type(bb, i);
    if(ty == MRBC_COPRO_TT_UNKNOWN) {
      ty = x11;
      dbg_mrbc_prof_printf("[cpy_to_interp_stack] restore to interp.: x%d -> R%d(%x) (value: %p, type: %d[UNKNOWN])", mrbc_function_header_allocation(fh, i), i, &(cur[i]), val, ty);
    } else
      dbg_mrbc_prof_printf("[cpy_to_interp_stack] restore to interp.: x%d -> R%d(%x) (value: %p, type: %d)", mrbc_function_header_allocation(fh, i), i, &(cur[i]), val, ty);
    switch(ty) {
      case MRBC_COPRO_TT_BOOL:
        mrbc_type(cur[i]) = val ? MRBC_TT_TRUE : MRBC_TT_FALSE;
        break;
      default:
        if(ty >= MRBC_COPRO_TT_OBJECT_START || ty == MRBC_COPRO_TT_ARRAY || ty == MRBC_COPRO_TT_STRING)
          mrbcopro_objman_translate_from_copro(vm, objman, &(cur[i]), val); // Error Handling.
        else {
          cur[i].tt = (mrbc_vtype)ty;
          cur[i].i = val;
          mrbc_incref(&(cur[i]));
        }
        break;
    }
  }
  return cur;
}

struct copro_to_main_thunk_t {
  struct copro_to_main_thunk_t * callee;
  mrbc_profile_function_header * function;
  mrbc_profile_basic_block * basicblock;
  mrbc_sym method_id;
  uint8_t reg_offset;
  uint8_t n_args;
  uint8_t issuper;
};

/// @brief Creates new copro_to_main_thunk_t.
struct copro_to_main_thunk_t * new_thunk_t(struct VM * vm, struct profiler_search_result_t sr, struct copro_to_main_thunk_t * callee) {
  struct copro_to_main_thunk_t * ret = (struct copro_to_main_thunk_t *)mrbcopro_alloc(vm, sizeof(struct copro_to_main_thunk_t));
  if(ret == NULL) return ret;
  ret->callee = callee;
  ret->function = sr.function;
  ret->basicblock = sr.basicblock;
  return ret;
}

int copro_to_main(mrbc_profile_profiler * prof, mrbc_copro_vtype * x11) {
  dbg_mrbc_prof_print_state(prof);
  uint32_t * cur = (uint32_t *)MRBC_COPRO_PTR_TO_NATIVE_PTR(ulp_mrbc_sp_bottom);
  struct copro_to_main_thunk_t * thunk_cur = NULL;
  uint32_t hardware_registers[32];
  memcpy(hardware_registers, cur, sizeof(uint32_t) * 32);
  mrbc_copro_vtype _x11 = (mrbc_copro_vtype)hardware_registers[11];
  *x11 = _x11;
  dbg_mrbc_prof_printf("[copro_to_main] x11: %d", (mrbc_copro_vtype)_x11);
  cur = &cur[32];
  struct VM * vm = prof->vm;
  mrbc_value * VM_REGFILE_RIGHT = &vm->regs[vm->regs_size];
  mrbc_value * interpreter_stack_cur = VM_REGFILE_RIGHT;
  mrbc_value * interpreter_stack_sentinel = vm->cur_regs + vm->cur_irep->nregs;
  struct profile_function_header * stop = prof->stacktop->functions_current;
  for(int iter_count = 0;;) {
    uint32_t ra = hardware_registers[1] - 4;
    dbg_mrbc_prof_printf("[copro_to_main] cur: %p, ra?: %p", cur, ra);
    struct profiler_search_result_t sr = search_basic_block_by_ra(prof, (void *)ra);
    if(sr.function == NULL) return 1;
    struct copro_to_main_thunk_t * t = new_thunk_t(vm, sr, thunk_cur);
    if(t == NULL) return 1;

    dbg_mrbc_prof_printf("[copro_to_main] function: %p basicblock: %p STACK RA: %p", sr.function, sr.basicblock, sr.basicblock->allocatedCode);
    int reg_len;
    if(iter_count == 0) {
      hardware_registers[1] = hardware_registers[0];
      hardware_registers[0] = 0;
      iter_count = 1;
      reg_len = sr.basicblock->regLen;
    } else {
      struct get_a_method_and_narg_return_t a_and_narg = get_a_method_and_narg(sr.basicblock->lastInstMRubyPtr, sr.function->irep);
      dbg_mrbc_prof_printf("A AND NARG: %d, %d, method_id: %s", a_and_narg.a, a_and_narg.narg, symid_to_str(a_and_narg.method_id));
      cur = restore_caller_saved(cur, sr.function, a_and_narg, hardware_registers);
      reg_len = a_and_narg.a; // excluding "a"
      thunk_cur->method_id = a_and_narg.method_id; thunk_cur->reg_offset = a_and_narg.a; thunk_cur->n_args = a_and_narg.narg;
      thunk_cur->issuper = *(sr.basicblock->lastInstMRubyPtr) == OP_SUPER;
      if(cur == NULL) return 1;
    }

    interpreter_stack_cur = cpy_to_interpreter_stack(vm, &(prof->objMan), interpreter_stack_cur, interpreter_stack_sentinel, sr.function, sr.basicblock, reg_len, hardware_registers, _x11);
    dbg_mrbc_prof_printf("[copro_to_main] interpreter_stack current: %p sentinel: %p", interpreter_stack_cur, interpreter_stack_sentinel);
    if(interpreter_stack_cur == NULL) return 1;
  
    thunk_cur = t;
    if(sr.function == stop) break;
    cur = restore_callee_saved(cur, sr.function, hardware_registers);
    if(cur == NULL) return 1;
  }
  mrbc_profile_profiler_stack * st = prof->stacktop;
  mrbc_profile_profiler_stack * st2 = st; // st2 must not be NULL.
  int reserve = 0;
  for(struct copro_to_main_thunk_t * tt = thunk_cur;
      st != NULL && tt != NULL;
      st2 = st, st = st->next, tt = tt->callee, reserve++) {
    dbg_mrbc_prof_printf("st=%p, tt=%p, st->functions_current=%p, tt->function=%p", st, tt, st == NULL ? 0 : st->functions_current, tt == NULL ? 0 : tt->function);
    if(st->functions_current != tt->function) break;
  }

  dbg_mrbc_prof_assert(st2 != NULL);
  int popcnt = 0;
  if(st2 != &prof->stack) { 
    prof->stack.prev = st2;
    profiler_pop_stack(prof);
    popcnt = 1;
  }
  if(st != NULL)
    while(st != &(prof->stack)) {st2 = st->next; mrbcopro_free(vm, st); popcnt++; st = st2; }
  dbg_mrbc_prof_printf("[copro_to_main] reserve = %d, popcnt = %d", reserve, popcnt);
  for(int i = 0; i < popcnt; ++i) {
    dbg_mrbc_prof_print("[copro_to_main] pop call info");
    dbg_mrbc_prof_print_callinfo(vm->callinfo_tail);
    mrbc_pop_callinfo(vm);
  }

  mrbc_callinfo ** cis = reserve > 16 ?
    mrbcopro_alloc(vm, sizeof(mrbc_callinfo *) * reserve) : alloca(sizeof(mrbc_callinfo *) * reserve);
  {
    mrbc_callinfo * ci = vm->callinfo_tail;
    for(int i = 0; i < reserve; ++i, ci = ci->prev)
      cis[i] = ci;
  }
  {
    mrbc_value * start = reserve == 1 ? &(vm->cur_regs[0]) : &(cis[reserve-2]->cur_regs[0]);
    mrbc_value * v_right = &(vm->cur_regs[vm->cur_irep->nregs + 1]);
    dbg_mrbc_prof_printf("cpy from %p to %p (length: %p)",
      interpreter_stack_cur, start, (size_t)VM_REGFILE_RIGHT - (size_t)interpreter_stack_cur);
    for(mrbc_value * v_cur = start; v_cur <= v_right; ++v_cur)
      mrbcopro_objman_safe_decref(vm, &(prof->objMan), v_cur);
    memcpy(start, interpreter_stack_cur, (size_t)VM_REGFILE_RIGHT - (size_t)interpreter_stack_cur);
    memset(interpreter_stack_cur, 0, (size_t)VM_REGFILE_RIGHT - (size_t)interpreter_stack_cur); // set empty
  }
  // const mrbc_class * _target_class;
  mrbc_value * _cur_regs = NULL;
  st = prof->stacktop;
  int cii = reserve - 1;
  mrbc_callinfo * ci = cis[cii];
  dbg_mrbc_prof_print("<<REWRITTEN>>");
  _cur_regs = ci->cur_regs + ci->reg_offset;
  st->current = thunk_cur->basicblock;
  cii--;
  {
    struct copro_to_main_thunk_t * tt = thunk_cur->callee, * delay_tt = thunk_cur;
    for(;tt != NULL; delay_tt = tt, tt = tt->callee, cii--) {
      dbg_mrbc_prof_printf("_cur_regs = %p", _cur_regs);
      if(cii >= 0) {
        ci = cis[cii];
        st = st->next;
        st->current = tt->basicblock;
        dbg_mrbc_prof_print("<<RESERVED>>");
      } else {
        ci = (mrbc_callinfo *)mrbc_alloc(vm, sizeof(mrbc_callinfo)); // do not mrbcopro_alloc
        memset(ci, 0, sizeof(mrbc_callinfo));
        ci->prev = vm->callinfo_tail;
        vm->callinfo_tail = ci;
        profiler_push_stack(prof);
        prof->stack.functions_current = tt->function;
        prof->stack.current = tt->basicblock;
        dbg_mrbc_prof_print("<<NEW>>");
      }
      ci->cur_irep = delay_tt->function->irep;
      const uint8_t * _inst = delay_tt->basicblock->lastInstMRubyPtr;
      ci->inst = (uint8_t *)((size_t)_inst + inst_skipper_inst_len[*_inst] + sizeof(uint8_t));
      ci->cur_regs = _cur_regs;
      // ci->target_class = NULL; // TODO: If support alias.
      ci->own_class = tt->function->own_class;
      // ci->karg_keep = 0; // TODO: If support keyword arguments.
      ci->method_id = tt->method_id;
      ci->n_args = tt->n_args;
      ci->reg_offset = tt->reg_offset;
      ci->is_called_super = tt->issuper;

      _cur_regs += tt->reg_offset;
    }
    if(reserve > 16) mrbcopro_free(vm, cis);
    vm->cur_irep = delay_tt->function->irep;
    vm->inst = delay_tt->basicblock->lastInstMRubyPtr;
    vm->cur_regs = _cur_regs;
  }
  for(struct copro_to_main_thunk_t * t = NULL ; thunk_cur != NULL; thunk_cur = t) {
    t = thunk_cur->callee;
    mrbcopro_free(vm, thunk_cur);
  }
  dbg_mrbc_prof_print_state(prof);
  return 0;
}