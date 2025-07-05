/**
 * @file compile.private.h
 * @author Go Suzuki (puyogo.suzuki@gmail.com)
 * @brief A private header file for the compiler.
 * @version 0.1
 * @date 2025-05-29
 * 
 * @copyright Copyright (c) 2024-2025 Go Suzuki and Programming Systems Group in Institute of Science Tokyo.
 * 
 */
#pragma once
#include "profiler.h"

// ULP utilities.
extern void ulp_fallback(void);
extern void ulp_read_barrier(void);

int get_allocation(mrbc_profile_profiler * prof, int virtualRegNum); // defined in compile.c
int override_register_information(mrbc_profile_profiler * prof, int dst, char val); // defined in compile.c

struct read_and_override_register_information_ret_t {int src; int dst;};
/// @brief prof->regsInfo[dst] = val. If it has already aliased, the register aliasing is applied.
/// @param prof profiler
/// @param reg  destination virtual register.
/// @param val  the value assigned.
/// @return     aliased register and allocated register, otherwise, failure returns 0 and 0
struct read_and_override_register_information_ret_t
read_and_override_register_information(mrbc_profile_profiler * prof, int reg, char val);

struct register_saving_analysis_result_t {
  int saved_registers;
  int spMoved;
};
struct register_saving_analysis_result_t
register_saving(mrbc_profile_profiler * prof, int a, int narg);
int register_restoring(mrbc_profile_profiler * prof, struct register_saving_analysis_result_t analysis, int a);

typedef struct send_inst_bufs_return_t {
  void * buffer;
#if MRBC_PROF_DBG_ENABLE
  size_t length;
#endif
} mrbc_send_inst_bufs_return_t;
mrbc_send_inst_bufs_return_t send_inst_bufs(mrbc_profile_profiler * prof, mrbc_profile_basic_block * relavant_basic_block);

#define CODE_BYTES_WRITE_JMP_TO_FALLBACK (sizeof(uint32_t))
static inline void write_jmp_to_fallback(uint32_t * dest) {
  dest[0] = RISCV_JUMP_AND_LINK(3, (size_t)ulp_fallback - (size_t)dest); // x11 is a reserved register!
}

int gen_load_immediate(mrbc_profile_profiler * prof, int reg, int value);

/// @brief Copy the types vm->cur_regs to the current mrbc_prof_get_type.
/// @param len if -1, it will be profiler_current(prof)->regLen
void set_prof_exit_types(mrbc_profile_profiler * prof, int len);

/// @brief Copy the types vm->cur_regs to the current mrbc_prof_get_entrance_type.
/// @param len if 0, it will be profiler_current(prof)->regLen
void set_prof_entrance_types(mrbc_profile_profiler * prof, int len);

/// @brief Create new profiling or search already created mrbc_profile_basic_block.
/// @param vm
/// @param prof
/// @return
///   0: Existing mrbc_profile_basic_block is found.
///   1: Out of Memory or some error.
///   2: A new mrbc_profile_basic_block is created.
int try_search_or_new_profiling(mrbc_profile_profiler * prof);

int write_jmp_and_execute_on_ulp(mrbc_profile_profiler * prof);