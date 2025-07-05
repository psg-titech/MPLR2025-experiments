#pragma once
#include "profiler.h"
#include "../../../src/vm.h"

/// @brief Converts a value in the interpreter to the coprocessor's.
/// @param v value
uint32_t mrbcopro_interpreter_value_to_coprocessor_value(struct VM * vm, mrbc_value * v, mrbcopro_objman_t * objman);

/// @brief Append the given mrbc_profile_basic_block and set as the current.
/// @param prof
/// @param v
void profiler_append_and_set_current(mrbc_profile_profiler * prof, mrbc_profile_basic_block * v);
/// @brief Push a new mrbc_profile_profiler_stack.
/// @param prof prof->stack is pushed.
/// @return 0 for success, 1 for out of memory.
int profiler_push_stack(mrbc_profile_profiler * prof);

/// @brief Pop a mrbc_profile_profiler_stack.
/// @param prof prof->stack is popped.
/// @return 0 for success, 1 for empty (nothing is changed).
int profiler_pop_stack(mrbc_profile_profiler * prof);
/// @brief Analyze the register saving.
/// @param fh function header
/// @param a "a" in send a, b, n
/// @param narg "n" in send a, b, n
/// @return bit field representing the saved registers.
uint32_t analyze_register_saving(mrbc_profile_function_header * fh, int a, int narg);
/// @brief Estimate the stack move size to satisfy alignment constraints.
/// @param save_register_size The number of saved registers (< 2^5 (=32))
/// @return The evaluated stack move size.
int estimate_stack_move_size(int save_register_size /* < 2^5 */);
/// @brief Migrate to the main processor from the coprocessor.
/// @param prof profiler
/// @param regs Registers value to be set. regs[32] represents used registers.
int main_to_copro(mrbc_profile_profiler * prof, uint32_t regs[33]);
/// @brief Migrate to the coprocessor from the main processor.
/// @param prof profiler
/// @param x11 may contain the type id of the return value.
int copro_to_main(mrbc_profile_profiler * prof, mrbc_copro_vtype * x11);