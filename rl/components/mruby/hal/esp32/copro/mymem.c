#include "mymem.h"
#include "dbg.h"

#if MRBC_DBG_ALLOCATION_PROFILE

uint32_t allocated_on_main_memory = 0;

void * mrbcopro_alloc(struct VM * vm, size_t size) {
  void * ret = mrbc_alloc(vm, size);
  if(ret) {
    allocated_on_main_memory += *((uint32_t *)((size_t)ret - sizeof(uint32_t))) & ~0x3;
    dbg_mrbc_prof_printf("[MEM] Allocated: %d, Used: %d", size, allocated_on_main_memory);
  } else 
    dbg_mrbc_prof_print("[MEM] Allocation Failed.");
  return ret;
}
void * mrbcopro_realloc(struct VM * vm, void * ptr, size_t size) {
  uint32_t prev_size = *((uint32_t *)((size_t)ptr - sizeof(uint32_t))) & ~0x3;
  void * ret = mrbc_realloc(vm, ptr, size);
  if(ret) {
    uint32_t new_size = *((uint32_t *)((size_t)ret - sizeof(uint32_t))) & ~0x3;
    allocated_on_main_memory += new_size - prev_size;
    dbg_mrbc_prof_printf("[MEM] ReAllocated: %d -> %d, Used: %d", prev_size, size, allocated_on_main_memory);
  } else 
    dbg_mrbc_prof_print("[MEM] ReAllocation Failed.");
  return ret;

}

void mrbcopro_free(struct VM * vm, void * ptr) {
#if !defined(MRBC_ALLOC_LIBC)
  uint32_t size = *((uint32_t *)((size_t)ptr - sizeof(uint32_t))) & ~0x3;
  mrbc_free(vm, ptr);
  allocated_on_main_memory -= size;
  dbg_mrbc_prof_printf("[MEM] Freed: %d, Used: %d", size, allocated_on_main_memory);
#endif
}

void mrbcopro_mem_reset(void) {
  allocated_on_main_memory = 0;
  dbg_mrbc_prof_print("[MEM] Reset");
}
#endif
