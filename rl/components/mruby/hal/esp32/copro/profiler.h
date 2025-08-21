/*! @file
  @brief
  Header of the profiler for the compiler.

  <pre>
  Copyright (C) 2024 - 2025 Go Suzuki and Institute of Science Tokyo Programming Systems Group.
  This file is distributed under BSD 3-Clause License.
  </pre>
 */
#pragma once
#include "dbg.h"
#include "../../src/vm.h"
#include "vector_t.h"
#include "globalman.h"
#include "objman.h"

struct profiling;
#if MRBC_COPRO_ENABLE_CODE_HEADER
typedef struct code_header {
  uint16_t tt; // must be "MRBC_COPRO_TT_CODE"
  uint16_t jmp_to;
  uint32_t jmp_code; // jmp
} mrbc_code_header;
#endif

typedef struct __attribute__((__packed__)) code_body {
  uint16_t tt; // must be "MRBC_COPRO_TT_CODE"
  uint16_t data[];
} mrbc_code_body;

struct profile_basic_block;

typedef struct profile_function_header {
  struct profile_function_header * next;
  struct profile_basic_block * blocks;
  const mrbc_irep * irep;
  mrbc_class * own_class;
  uint32_t freeMachineRegisters;
  uint8_t regLen;
  uint8_t spMoved;    // [byte]
  uint8_t spRemaining;// [byte]
  uint8_t payload[0];
  // uint8_t allocatedRegNum[regLen];
} mrbc_profile_function_header;
#define mrbc_function_header_allocation(fh, idx) ((fh)->payload)[idx]
#define mrbc_function_header_allocation_head(fh) (fh)->payload
#define mrbc_function_header_allocation_retrived_registers_on_callee_regiseter_saving(function_header) (~((function_header)->freeMachineRegisters) & RISCV_CALLEE_SAVE_REGISTERS & ~4u)

typedef struct profile_assertion_block {
  struct profile_assertion_block * next;
  struct code_body * allocatedCode;
} mrbc_profile_assertion_block;

typedef struct profile_basic_block {
  struct profile_basic_block * next;
  void const * beginning_inst;
  uint32_t * failInstPtr;
  uint8_t const * lastInstMRubyPtr;
  void * returnPos; 
  // struct profile_function_header * functionHeader;
#if MRBC_COPRO_ENABLE_CODE_HEADER
  struct code_header * allocatedHeader;
#else
  struct profile_assertion_block * asserts;
  struct code_body * allocatedCode;
#endif
  uint8_t regLen;
  uint8_t payload[0];
  // mrbc_vtype(uint16_t) enterTypeProfile[regLen];
  // mrbc_vtype(uint16_t) typeProfile[regLen];
} mrbc_profile_basic_block;

#define mrbc_prof_get_entrance_type(prof, idx) ((uint16_t *)((prof)->payload))[idx]
#define mrbc_prof_entrance_type_head(prof) ((uint16_t *)((prof)->payload))
#define mrbc_prof_get_type(prof, idx) ((uint16_t *)((prof)->payload))[(idx) + (prof)->regLen]
#define mrbc_prof_type_head(prof) ((uint16_t *)(&((prof)->payload[sizeof(uint16_t) * (prof)->regLen])))

#define mrbc_prof_is_constant(prof, idx) ((prof)->regsinfo[idx] == 1)

typedef struct profiler_stack {
  struct profiler_stack * prev;
  struct profiler_stack * next;
  struct profile_function_header * functions_current;
  struct profile_basic_block * current;
  uint32_t * previous_jmp;
  int previous_jmp_rd;
} mrbc_profile_profiler_stack;

typedef struct profiler {
  mrbc_vm * vm;
  struct profile_function_header * functions;
  mrbc_profile_profiler_stack stack;
  mrbc_profile_profiler_stack * stacktop;
  const uint8_t * lastInst;

  // 00000000: no need to translate
  // 00000001: const val
  // 0xxxxx10: Translation -> regsinfo[i] >> 2.
  // 0xxxxx11: Aliased by regsinfo[i] >> 2.
  char * regsinfo; // Virtual Register!!
  
  mrbcopro_vector_t buf;
  mrbcopro_globalman_t globalMan;
  mrbcopro_objman_t objMan;
#if MRBC_PROF_DBG_ENABLE
  mrbc_callinfo * callTop;
#endif
  uint16_t regsinfo_length; // < 256
  uint16_t giveup;
} mrbc_profile_profiler;
#define profiler_current(v) ((v)->stack.current)
#define profiler_currentfunction(v) ((v)->stack.functions_current)

struct profiler_search_result_t {
  struct profile_function_header * function;
  struct profile_basic_block * basicblock;
};
/// @brief  Searches a basic block and a function header by the given return address.
struct profiler_search_result_t search_basic_block_by_ra(mrbc_profile_profiler * prof, void * addr);

/// @brief Deconstructor for `mrbc_profile_profiler`
void profile_dtor(struct VM * vm, mrbc_profile_profiler * p);