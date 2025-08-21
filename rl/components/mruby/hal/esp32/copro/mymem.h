/**
 * @file globalman.h
 * @author Go Suzuki (puyogo.suzuki@gmail.com)
 * @brief malloc/free for profiling.
 * @version 0.1
 * @date 2025-05-17
 * 
 * @copyright Copyright (c) 2025 Go Suzuki and Programming Systems Group in Institute of Science Tokyo.
 * 
 */

#include "vm.h"
#include "../../../src/mrubyc.h"

#define MRBC_DBG_ALLOCATION_PROFILE 0
#if MRBC_DBG_ALLOCATION_PROFILE
void * mrbcopro_alloc(struct VM * vm, size_t size);
void * mrbcopro_realloc(struct VM * vm, void * ptr, size_t size);
void mrbcopro_free(struct VM * vm, void * ptr);
void mrbcopro_mem_reset(void);
#else
#include "../../../src/alloc.h"
static inline void * mrbcopro_alloc(struct VM * vm, size_t size) { return mrbc_alloc(vm, size); }
static inline void * mrbcopro_realloc(struct VM * vm, void * ptr, size_t size) { return mrbc_realloc(vm, ptr, size); }

static inline void mrbcopro_free(struct VM * vm, void * ptr) { mrbc_free(vm, ptr); }
static inline void mrbcopro_mem_reset(void) {}
#endif