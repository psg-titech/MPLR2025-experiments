#include "compile.inlined.h"
#include "profiler.h"
#include "riscv_writer.h"
#include "compile.private.h"
#include "objman.h"
#include "symbol.h"
#include "stack.h"
#include "profiler.h"

int gen_compiled_object_not(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) {
  struct VM * vm = prof->vm;
  struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a);
  mrbcopro_vector_append32(vm, &(prof->buf), RISCV_LESS_THAN_UNSIGNED_IMM(regA.dst, regA.src, 1));
  dbg_mrbc_prof_print_inst_readable("sltiu x%d, x%d, 1", regA.dst, regA.src, 1);
  return 0;
}
int gen_compiled_object_nil(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) {
  struct VM * vm = prof->vm;
  int writeval = recv[0].tt > MRBC_TT_NIL;
  override_register_information(prof, a, 0);
  int regA = mrbc_function_header_allocation(profiler_current(prof), a);
  mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_LI(regA, writeval));
  dbg_mrbc_prof_print_inst_readable("c.li x%d, %d", regA, writeval);
  return 0;
}
int gen_ineffect(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) { 
  return 0;
}

extern RObjectPtrCopro ulp_mrbc_gc_alloc(size_t len);
extern void ulp_object_new(void);
int gen_compiled_object_new(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) {
  mrbc_func_t cfunc = NULL;
  { // unfortunately...
    mrbc_method method;
    if( !mrbc_find_method(&method, recv[0].cls, MRBC_SYM(initialize)))
      cfunc = (mrbc_func_t)1;
    else
      cfunc = method.c_func ? method.copro_func : NULL;
  }
  // LIMITATION: currently, the object size is limited to 2^16 bytes and the object id is limited to 2^16.
  // LIMITATION: cfunc must be a coprocessor function if not null. (TODO)
  /*
    {register saving}
    x10 <- ALLOCATION_SIZE << 16 | id
    call object_new
    call initialize
    {register restoring}
    jmp
  */
  struct VM * vm = prof->vm;
  struct register_saving_analysis_result_t anresult = register_saving(prof, a, narg);
  mrbcopro_objinfo * oi = mrbcopro_objman_get_obj_info(vm, &(prof->objMan), recv[0].cls);
  gen_load_immediate(prof, 10, (oi->len << 16) | oi->copro_objid);
  size_t call_place = mrbcopro_vector_bytes2(prof->buf);
  mrbcopro_vector_append_without_fill(vm, &(prof->buf), cfunc == (mrbc_func_t)1 ? sizeof(uint32_t) : (sizeof(uint32_t) * 2));
  register_restoring(prof, anresult, a);
  size_t jmp_place = mrbcopro_vector_bytes2(prof->buf);
  mrbcopro_vector_append_without_fill(vm, &(prof->buf), CODE_BYTES_WRITE_JMP_TO_FALLBACK); // jump to next
  mrbc_send_inst_bufs_return_t sib = send_inst_bufs(prof, profiler_current(prof));
  if(sib.buffer == NULL) return 1;

  uint32_t * writer = (uint32_t *)((size_t)sib.buffer + call_place);
  writer[0] = RISCV_JUMP_AND_LINK(1, (size_t)ulp_object_new - (size_t)writer);
  if(cfunc != (mrbc_func_t)1) {
    writer++;
    if(cfunc == NULL)
      write_jmp_to_fallback(writer);
    else
      *writer = RISCV_JUMP_AND_LINK(1, (size_t)cfunc - (size_t)writer);
  }
  uint32_t * jmp = (uint32_t *)((size_t)sib.buffer + jmp_place);
  write_jmp_to_fallback(jmp);
#if MRBC_COPRO_ENABLE_CODE_HEADER
  dbg_dump_riscv_code2(sizeof(uint32_t), &(bb->allocatedHeader->jmp_code));
#endif
  dbg_dump_riscv_code2(sib.length, sib.buffer);

  struct profile_basic_block * b = profiler_current(prof);
  b->failInstPtr = jmp;
  b->lastInstMRubyPtr = prof->lastInst;
  b->asserts = (struct profile_assertion_block *)2;
  b->returnPos = MRBC_COPRO_NATIVE_PTR_TO_PTR2(&writer[1]);

  prof->stack.previous_jmp = jmp;
  prof->stack.previous_jmp_rd = 0;

  if(cfunc == NULL) {
    set_prof_exit_types(prof, a);
    mrbc_prof_get_type(b, a) = oi->copro_objid;
    if(profiler_push_stack(prof)) return 1;
    prof->stack.previous_jmp = (uint32_t *)(writer);
    prof->stack.previous_jmp_rd = 1;
    return 0;
  }
  return 2;
}

static const inline_code_gen_t method_copro_Object[] = {
  gen_compiled_object_not, // c_object_not,
  0, // c_object_neq,
  0, // c_object_compare,
  0, // c_object_equal2,
  0, // c_object_equal3,
  0, // c_object_attr_accessor,
  0, // c_object_attr_reader,
  0, // c_object_block_given,
  0, // c_object_class,
  0, // c_object_constants,
  0, // c_object_dup,
  0, // c_object_include,
  0, // c_object_include,
#if MRBC_USE_STRING
  0, // c_object_to_s,
#endif
#if defined(MRBC_DEBUG)
  0, // c_object_instance_methods,
#endif
#if defined(MRBC_DEBUG)
  0, // c_object_instance_variables,
#endif
  0, // c_object_kind_of,
  0, // c_object_kind_of,
#if defined(MRBC_DEBUG)
#if !defined(MRBC_ALLOC_LIBC)
  0, // c_object_memory_statistics,
#endif
#endif
  gen_compiled_object_new, // c_object_new,
  gen_compiled_object_nil, // c_object_nil,
#if defined(MRBC_DEBUG)
  0, // c_object_object_id,
#endif
#if !defined(MRBC_NO_STDIO)
  0, // c_object_p,
#endif
#if !defined(MRBC_NO_STDIO)
  0, // c_object_print,
#endif
#if MRBC_USE_STRING
#if !defined(MRBC_NO_STDIO)
  0, // c_object_printf,
#endif
#endif
  gen_ineffect, // c_ineffect,
  gen_ineffect, // c_ineffect,
  gen_ineffect, // c_ineffect,
#if !defined(MRBC_NO_STDIO)
  0, // c_object_puts,
#endif
  0, // c_object_raise,
#if MRBC_USE_STRING
  0, // c_object_sprintf,
#endif
#if MRBC_USE_STRING
  0, // c_object_to_s,
#endif
};

int gen_compiled_array_size(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) {
  struct VM * vm = prof->vm;
  struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a);
  mrbcopro_vector_append32(vm, &(prof->buf), RISCV_LOAD_HALF(regA.dst, regA.src, 2));
  dbg_mrbc_prof_print_inst_readable("lh x%d, 2(x%d)", regA.dst, regA.src, 2);
  return 0;
}

int gen_compiled_array_new(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) {
  if(narg != 1 || recv[1].tt != MRBC_TT_INTEGER) { // suppports only  Array.new(length_of_array)
    dbg_mrbc_prof_print("Only Array.new(length) is supported!");
    return 1;
  }
  /*
    {register saving}
    x10 <- x11 << len | MRBC_COPRO_TT_ARRAY
    call object_new
    {register restoring}
    jmp
  */
  struct VM * vm = prof->vm;
  struct register_saving_analysis_result_t anresult = register_saving(prof, a, narg);
  mrbcopro_vector_append32(prof->vm, &(prof->buf), RISCV_SHIFT_LEFT_IMM(10, 11, 16));
  mrbcopro_vector_append16(prof->vm, &(prof->buf), RISCV_C_ADDI(10, MRBC_COPRO_TT_ARRAY));
  size_t call_place = mrbcopro_vector_bytes2(prof->buf);
  mrbcopro_vector_append_without_fill(vm, &(prof->buf), sizeof(uint32_t));
  register_restoring(prof, anresult, a);
  size_t jmp_place = mrbcopro_vector_bytes2(prof->buf);
  mrbcopro_vector_append_without_fill(vm, &(prof->buf), CODE_BYTES_WRITE_JMP_TO_FALLBACK); // jump to next
  mrbc_send_inst_bufs_return_t sib = send_inst_bufs(prof, profiler_current(prof));
  if(sib.buffer == NULL) return 1;

  uint32_t * writer = (uint32_t *)((size_t)sib.buffer + call_place);
  *writer = RISCV_JUMP_AND_LINK(1, (size_t)ulp_object_new - (size_t)writer);
  uint32_t * jmp = (uint32_t *)((size_t)sib.buffer + jmp_place);
  write_jmp_to_fallback(jmp);
#if MRBC_COPRO_ENABLE_CODE_HEADER
  dbg_dump_riscv_code2(sizeof(uint32_t), &(bb->allocatedHeader->jmp_code));
#endif
  dbg_dump_riscv_code2(sib.length, sib.buffer);

  struct profile_basic_block * b = profiler_current(prof);
  b->failInstPtr = jmp;
  b->lastInstMRubyPtr = prof->lastInst;
  b->asserts = (struct profile_assertion_block *)2;

  prof->stack.previous_jmp = jmp;
  prof->stack.previous_jmp_rd = 0;
  return 2;
}

extern void ulp_array_set(void);
int gen_compiled_array_set(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) {
  // do not type check the return value.
  struct VM * vm = prof->vm;
  uint32_t * jmp = NULL;
  struct register_saving_analysis_result_t anresult = register_saving(prof, a, narg);
  // conversion
  int delta = 0;
  if(recv[2].tt == MRBC_TT_INTEGER)
    delta = 1;
  else if(recv[2].tt == MRBC_TT_TRUE || recv[2].tt == MRBC_TT_FALSE)
    delta = 2;
  if(delta > 0) {
    mrbcopro_vector_append16(prof->vm, &(prof->buf), RISCV_C_SHIFT_LEFT_IMM(12, delta));
    dbg_mrbc_prof_print_inst_readable("c.slli x12, %d", delta);
    mrbcopro_vector_append16(prof->vm, &(prof->buf), RISCV_C_ADDI(12, delta));// actually, pow(2, delta-1)
    dbg_mrbc_prof_print_inst_readable("c.addi x12, %d", delta);
  }

  size_t call_place = mrbcopro_vector_bytes2(prof->buf);
  mrbcopro_vector_append_without_fill(vm, &(prof->buf), sizeof(uint32_t));
  register_restoring(prof, anresult, a);
  if(delta > 0) {
    mrbcopro_vector_append16(prof->vm, &(prof->buf), RISCV_C_SHIFT_RIGHT_ARITHMETIC_IMM(10, delta));
    dbg_mrbc_prof_print_inst_readable("c.srli x10, %d", delta);
  }
  size_t jmp_place = mrbcopro_vector_bytes2(prof->buf);
  mrbcopro_vector_append_without_fill(vm, &(prof->buf), CODE_BYTES_WRITE_JMP_TO_FALLBACK); // jump to next
  mrbc_send_inst_bufs_return_t sib = send_inst_bufs(prof, profiler_current(prof));
  if(sib.buffer == NULL) return 1;
  if(sib.buffer == (void *)1) return 0;

  uint32_t * writer = (uint32_t *)((size_t)sib.buffer + call_place);
  writer[0] = RISCV_JUMP_AND_LINK(1, (size_t)ulp_array_set - (size_t)writer);
  jmp = (uint32_t *)((size_t)sib.buffer + jmp_place);
  write_jmp_to_fallback(jmp);
#if MRBC_COPRO_ENABLE_CODE_HEADER
  dbg_dump_riscv_code2(sizeof(uint32_t), &(bb->allocatedHeader->jmp_code));
#endif
  dbg_dump_riscv_code2(sib.length, sib.buffer);

  struct profile_basic_block * b = profiler_current(prof);
  b->failInstPtr = jmp;
  b->lastInstMRubyPtr = prof->lastInst;
  b->asserts = (struct profile_assertion_block *)2;

  prof->stack.previous_jmp = jmp;
  prof->stack.previous_jmp_rd = 0;
  return 2;
}


extern void ulp_array_get(void);
static const inline_code_gen_t method_copro_Array[] = {
  0, //c_array_and,
  0, //c_array_add,
  0, //c_array_push,
  (inline_code_gen_t)ulp_array_get, //c_array_get,
  gen_compiled_array_set, //c_array_set,
  (inline_code_gen_t)ulp_array_get, //c_array_get,
  0, //c_array_clear,
  gen_compiled_array_size, //c_array_size,
  0, //c_array_delete_at,
  0, //c_array_dup,
  0, //c_array_empty,
  0, //c_array_first,
  0, //c_array_include,
#if MRBC_USE_STRING
  0, //c_array_inspect,
#endif
#if MRBC_USE_STRING
  0, //c_array_join,
#endif
  0, //c_array_last,
  gen_compiled_array_size, //c_array_size,
  0, //c_array_max,
  0, //c_array_min,
  0, //c_array_minmax,
  gen_compiled_array_new, //c_array_new,
  0, //c_array_pop,
  0, //c_array_push,
  0, //c_array_shift,
  gen_compiled_array_size, //c_array_size,
#if MRBC_USE_STRING
  0, //c_array_inspect,
#endif
  0, //c_array_uniq,
  0, //c_array_uniq_self,
  0, //c_array_unshift,
  0  //c_array_or,
};


int gen_compiled_integer_mod(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) {
  if(narg != 1) return 1;
  // The mruby compiler seems to do constant foldings because methods of Integer cannot be overloaded.
  // if((prof->regsinfo[a] + prof->regsinfo[a+1]) == 2) {
  //   prof->regsinfo[a+1] = 0;
  //   return 0;
  // }
  struct VM * vm = prof->vm;
  struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a);
  int regA1 = get_allocation(prof, a+1);
  mrbcopro_vector_append32(vm, &(prof->buf), RISCV_REM(regA.dst, regA.src, regA1));
  dbg_mrbc_prof_print_inst_readable("rem x%d, x%d, x%d", regA.dst, regA.src, regA1);
  override_register_information(prof, a+1, 0);
  return 0;
}

int gen_compiled_integer_and(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) {
  if(narg != 1) return 1;
  struct VM * vm = prof->vm;
  struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a);
  if(prof->regsinfo[a+1] == 1 && ((((int)(mrbc_integer(recv[1])) >> 11) == -1) || (mrbc_integer(recv[1]) < (1 << 11)))) {
    prof->regsinfo[a+1] = 0;
    mrbcopro_vector_append32(vm, &(prof->buf), RISCV_AND_IMM(regA.dst, regA.src, mrbc_integer(recv[1])));
    dbg_mrbc_prof_print_inst_readable("andi x%d, x%d, %d", regA.dst, regA.src, mrbc_integer(recv[1]));
  } else {
    int regA1 = get_allocation(prof, a+1);
    mrbcopro_vector_append32(vm, &(prof->buf), RISCV_AND(regA.dst, regA.src, regA1));
    dbg_mrbc_prof_print_inst_readable("and x%d, x%d, x%d", regA.dst, regA.src, regA1);
    override_register_information(prof, a+1, 0);
  }
  return 0;
}
int gen_compiled_integer_neg(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) {
  if(narg != 0) return 1;
  struct VM * vm = prof->vm;
  struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a);
  mrbcopro_vector_append32(vm, &(prof->buf), RISCV_SUB(regA.dst, 0, regA.src));
  dbg_mrbc_prof_print_inst_readable("sub x%d, zero, x%d", regA.dst, 0, regA.src);
  return 0;
}

// LIMITATION: does not support negative values.
int gen_compiled_integer_lshift(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) {
  if(narg != 1) return 1;
  struct VM * vm = prof->vm;
  struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a);
  if(prof->regsinfo[a+1] == 1) {
    prof->regsinfo[a+1] = 0;
    // do not think when R(a+1) >= BIT_WIDTH
    // if(regA.dst == regA.src) {
    //   mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_SHIFT_LEFT_IMM(regA.src, mrbc_integer(recv[1])));
    //   dbg_mrbc_prof_print_inst_readable("c.slli x%d, %d", regA.src, mrbc_integer(recv[1]));
    // } else {
      mrbcopro_vector_append32(vm, &(prof->buf), RISCV_SHIFT_LEFT_IMM(regA.dst, regA.src, mrbc_integer(recv[1])));
      dbg_mrbc_prof_print_inst_readable("slli x%d, x%d, %d", regA.dst, regA.src, mrbc_integer(recv[1]));
    // }
  } else {
    int regA1 = get_allocation(prof, a+1);
    mrbcopro_vector_append32(vm, &(prof->buf), RISCV_SHIFT_LEFT(regA.dst, regA.src, regA1));
    dbg_mrbc_prof_print_inst_readable("sll x%d, x%d, x%d", regA.dst, regA.src, regA1);
    override_register_information(prof, a+1, 0);
  }
  return 0;
}

// LIMITATION: does not support negative values.
int gen_compiled_integer_rshift(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) {
  if(narg != 1) return 1;
  struct VM * vm = prof->vm;
  struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a);
  if(prof->regsinfo[a+1] == 1) {
    prof->regsinfo[a+1] = 0;
    // do not think when R(a+1) >= BIT_WIDTH
    mrbcopro_vector_append32(vm, &(prof->buf), RISCV_SHIFT_RIGHT_ARITH_IMM(regA.dst, regA.src, mrbc_integer(recv[1])));
    dbg_mrbc_prof_print_inst_readable("srai x%d, x%d, %d", regA.dst, regA.src, mrbc_integer(recv[1]));
  } else {
    int regA1 = get_allocation(prof, a+1);
    mrbcopro_vector_append32(vm, &(prof->buf), RISCV_SHIFT_RIGHT_ARITH(regA.dst, regA.src, regA1));
    dbg_mrbc_prof_print_inst_readable("srai x%d, x%d, x%d", regA.dst, regA.src, regA1);
    override_register_information(prof, a+1, 0);
  }
  return 0;
}

int gen_compiled_integer_bitref(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) {
  if(narg > 2) return 1;
  struct VM * vm = prof->vm;
  struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a);
  if(prof->regsinfo[a+1] == 1) {
    prof->regsinfo[a+1] = 0;
    // do not think when R(a+1) >= BIT_WIDTH
    // if(regA.dst == regA.src) {
    //   mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_SHIFT_LEFT_IMM(regA.src, mrbc_integer(recv[1])));
    //   dbg_mrbc_prof_print_inst_readable("c.slli x%d, %d", regA.src, mrbc_integer(recv[1]));
    // } else {
      mrbcopro_vector_append32(vm, &(prof->buf), RISCV_SHIFT_LEFT_IMM(regA.dst, regA.src, mrbc_integer(recv[1])));
      dbg_mrbc_prof_print_inst_readable("slli x%d, x%d, %d", regA.dst, regA.src, mrbc_integer(recv[1]));
    // }
  } else {
    int regA1 = get_allocation(prof, a+1);
    mrbcopro_vector_append32(vm, &(prof->buf), RISCV_SHIFT_LEFT(regA.dst, regA.src, regA1));
    dbg_mrbc_prof_print_inst_readable("sll x%d, x%d, x%d", regA.dst, regA.src, regA1);
    override_register_information(prof, a+1, 0);
  }
  if(narg == 1) {
    mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_AND_IMM(regA.dst, 1));
    dbg_mrbc_prof_print_inst_readable("c.andi x%d, %d", regA.dst, 1);
  } else {
    if(prof->regsinfo[a+2] == 1) {
      prof->regsinfo[a+2] = 0;
      int mask = (1 << mrbc_integer(recv[2])) - 1;
      // do not think when R(a+1) >= BIT_WIDTH
      if((((mask >> 5) == -1) || (mask < (1 << 5)))) {
        mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_AND_IMM(regA.dst, mask));
        dbg_mrbc_prof_print_inst_readable("c.andi x%d, %d", regA.dst, mask);
        return 0;
      } else if((((mask >> 11) == -1) || (mask < (1 << 11)))) {
        mrbcopro_vector_append32(vm, &(prof->buf), RISCV_AND_IMM(regA.dst, regA.dst, mask));
        dbg_mrbc_prof_print_inst_readable("andi x%d, x%d, %d", regA.dst, regA.dst, mask);
        return 0;
      } else {
        gen_load_immediate(prof, 11, mask);
      }
    } else {
      int regA2 = get_allocation(prof, a+2);
      mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_LI(11, 1));
      dbg_mrbc_prof_print_inst_readable("c.li x11, 1", 0);
      mrbcopro_vector_append32(vm, &(prof->buf), RISCV_SHIFT_LEFT(11, 11, regA2));
      dbg_mrbc_prof_print_inst_readable("sll x11, x11, x%d", regA2);
      mrbcopro_vector_append16(vm, &(prof->buf), RISCV_C_ADDI(11, -1));
      dbg_mrbc_prof_print_inst_readable("c.addi x11, -1", 0);
      override_register_information(prof, a+2, 0);
    }
    mrbcopro_vector_append32(vm, &(prof->buf), RISCV_AND(regA.dst, regA.src, 11));
    dbg_mrbc_prof_print_inst_readable("and x%d, x%d, x%d", regA.dst, regA.src, 11);
  }
  return 0;
}

int gen_compiled_integer_xor(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) {
  if(narg != 1) return 1;
  struct VM * vm = prof->vm;
  struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a);
  if(prof->regsinfo[a+1] == 1 && ((((int)(mrbc_integer(recv[1])) >> 11) == -1) || (mrbc_integer(recv[1]) < (1 << 11)))) {
    prof->regsinfo[a+1] = 0;
    mrbcopro_vector_append32(vm, &(prof->buf), RISCV_XOR_IMM(regA.dst, regA.src, mrbc_integer(recv[1])));
    dbg_mrbc_prof_print_inst_readable("xori x%d, x%d, %d", regA.dst, regA.src, mrbc_integer(recv[1]));
  } else {
    int regA1 = get_allocation(prof, a+1);
    mrbcopro_vector_append32(vm, &(prof->buf), RISCV_XOR(regA.dst, regA.src, regA1));
    dbg_mrbc_prof_print_inst_readable("xor x%d, x%d, x%d", regA.dst, regA.src, regA1);
  }
  return 0;
}

int gen_compiled_integer_abs(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) {
  if(narg != 0) return 1;
  struct VM * vm = prof->vm;
  struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a);
  mrbcopro_vector_append32(vm, &(prof->buf), RISCV_SHIFT_RIGHT_ARITH_IMM(11, regA.src, 31));
  dbg_mrbc_prof_print_inst_readable("srai x11, x%d, %d", regA.src, 31);
  mrbcopro_vector_append32(vm, &(prof->buf), RISCV_XOR(regA.dst, regA.src, 11));
  dbg_mrbc_prof_print_inst_readable("xor x%d, x%d, x11", regA.dst, regA.src);
  mrbcopro_vector_append32(vm, &(prof->buf), RISCV_SUB(regA.dst, regA.dst, 11));
  dbg_mrbc_prof_print_inst_readable("sub x%d, x%d, x11", regA.dst, regA.dst);
  return 0;
}

int gen_compiled_integer_or(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) {
  if(narg != 1) return 1;
  struct VM * vm = prof->vm;
  struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a);
  if(prof->regsinfo[a+1] == 1 && ((((int)(mrbc_integer(recv[1])) >> 11) == -1) || (mrbc_integer(recv[1]) < (1 << 11)))) {
    prof->regsinfo[a+1] = 0;
    mrbcopro_vector_append32(vm, &(prof->buf), RISCV_OR_IMM(regA.dst, regA.src, mrbc_integer(recv[1])));
    dbg_mrbc_prof_print_inst_readable("ori x%d, x%d, %d", regA.dst, regA.src, mrbc_integer(recv[1]));
  } else {
    int regA1 = get_allocation(prof, a+1);
    mrbcopro_vector_append32(vm, &(prof->buf), RISCV_OR(regA.dst, regA.src, regA1));
    dbg_mrbc_prof_print_inst_readable("or x%d, x%d, x%d", regA.dst, regA.src, regA1);
  }
  return 0;
}

int gen_compiled_integer_not(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) {
  if(narg != 1) return 1;
  struct VM * vm = prof->vm;
  struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a);
  mrbcopro_vector_append32(vm, &(prof->buf), RISCV_XOR_IMM(regA.dst, regA.src, -1));
  dbg_mrbc_prof_print_inst_readable("xor x%d, x%d, -1", regA.dst, regA.src);
  return 0;
}

static const inline_code_gen_t method_copro_Integer[] = {
  gen_compiled_integer_mod, // c_integer_mod,
  gen_compiled_integer_mod, // c_integer_and,
  0, // c_integer_power,
  gen_ineffect, // c_integer_positive,
  gen_compiled_integer_neg, // c_integer_negative,
  gen_compiled_integer_lshift, // c_integer_lshift,
  gen_compiled_integer_rshift, // c_integer_rshift,
  gen_compiled_integer_bitref, // c_integer_bitref,
  gen_compiled_integer_xor, // c_integer_xor,
  gen_compiled_integer_abs, // c_integer_abs,
#if MRBC_USE_STRING
  0, // c_integer_chr,
#endif
  0, // c_numeric_clamp,
#if MRBC_USE_STRING
  0, // c_integer_inspect,
#endif
#if MRBC_USE_FLOAT
  0, // c_integer_to_f,
#endif
  gen_ineffect,
#if MRBC_USE_STRING
  0, // c_integer_inspect,
#endif
  gen_compiled_integer_or, // c_integer_or,
  gen_compiled_integer_not, // c_integer_not,
};

int gen_compiled_string_empty(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) {
  struct VM * vm = prof->vm;
  struct read_and_override_register_information_ret_t regA = read_and_override_register_information(prof, a);
  mrbcopro_vector_append32(vm, &(prof->buf), RISCV_LOAD_HALF(regA.dst, regA.src, 2));
  dbg_mrbc_prof_print_inst_readable("lh x%d, 2(x%d)", regA.dst, regA.src, 2);
  mrbcopro_vector_append32(vm, &(prof->buf), RISCV_LESS_THAN(regA.dst, 0, regA.dst));
  dbg_mrbc_prof_print_inst_readable("slt x%d, zero, x%d", regA.dst, 0, regA.dst);
  return 0;
}

extern void ulp_string_getbyte(void);
static const inline_code_gen_t method_copro_String[] = {
  0, //c_string_mul,
  0, //c_string_add,
  0, //c_string_append,
  0, //c_string_slice,
  0, //c_string_insert,
  gen_ineffect,
  0, //c_string_bytes,
  0, //c_string_chomp,
  0, //c_string_chomp_self,
  0, // c_string_clear,
  0, // c_string_downcase,
  0, // c_string_downcase_self,
  0, // c_string_dup,
  gen_compiled_string_empty, // c_string_empty,
  0, // c_string_end_with,
  (inline_code_gen_t)ulp_string_getbyte, // c_string_getbyte,
  0, // c_string_include,
  0, // c_string_index,
  0, // c_string_inspect,
  0, // c_string_to_sym,
  gen_compiled_array_size, // c_string_size,
  0, // c_string_lstrip,
  0, // c_string_lstrip_self,
  0, // c_string_new,
  0, // c_string_ord,
  0, // c_string_rstrip,
  0, // c_string_rstrip_self,
  0, // c_string_setbyte,
  gen_compiled_array_size, //c_string_size,
  0, //c_string_slice,
  0, //c_string_slice_self,
  0, //c_string_split,
  0, //c_string_start_with,
  0, //c_string_strip,
  0, //c_string_strip_self,
#if MRBC_USE_FLOAT
  0, //c_string_to_f,
#endif
  0, //c_string_to_i,
  0, //c_string_to_s,
  0, //c_string_to_sym,
  0, //c_string_tr,
  0, //c_string_tr_self,
  0, //c_string_upcase,
  0, //c_string_upcase_self,
};

// TODO: NOT TESTED YET!
void gen_write32(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) {
  struct VM * vm = prof->vm;
  mrbc_profile_function_header * fh = profiler_currentfunction(prof);
  int dst = narg == 3 ? a + 2 : a + 3;
  mrbcopro_vector_append32(vm, &(prof->buf), RISCV_STORE_WORD(mrbc_function_header_allocation(fh, dst), mrbc_function_header_allocation(fh, a+1), 0));
  dbg_mrbc_prof_printf("sw x%d, x%d(0)", mrbc_function_header_allocation(fh, a+1), mrbc_function_header_allocation(fh, dst));
}
// TODO: NOT TESTED YET!
void gen_read32(mrbc_profile_profiler * prof, mrbc_method * method, int a, mrbc_value *recv, int narg) {
  struct VM * vm = prof->vm;
  mrbc_profile_function_header * fh = profiler_currentfunction(prof);
  int dst = narg == 2 ? a + 1 : a + 2;
  mrbcopro_vector_append32(vm, &(prof->buf), RISCV_LOAD_WORD(mrbc_function_header_allocation(fh, a), mrbc_function_header_allocation(fh, dst), 0));
  dbg_mrbc_prof_printf("lw x%d, x%d(0)", mrbc_function_header_allocation(fh, a), mrbc_function_header_allocation(fh, dst));
}

void register_gen_compiled(void) {
  {
    struct RBuiltinClass * obj = (struct RBuiltinClass *)MRBC_CLASS(Object);
    if(obj->num_builtin_method == 0) goto OBJ_END;
    obj->method_copros = method_copro_Object;
  }
  OBJ_END:
  {
    struct RBuiltinClass * ary = (struct RBuiltinClass *)MRBC_CLASS(Array);
    if(ary->num_builtin_method == 0) goto ARY_END;
    ary->method_copros = method_copro_Array;
  }
  ARY_END:
  {
    struct RBuiltinClass * iNT = (struct RBuiltinClass *)MRBC_CLASS(Integer);
    if(iNT->num_builtin_method == 0) goto INT_END;
    iNT->method_copros = method_copro_Integer;
  }
  INT_END:
  {
    struct RBuiltinClass * string = (struct RBuiltinClass *)MRBC_CLASS(String);
    if(string->num_builtin_method == 0) goto STR_END;
    string->method_copros = method_copro_String;
  }
  STR_END:
  return;
}
