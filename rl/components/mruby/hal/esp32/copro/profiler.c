#include "profiler.h"
#include "value.copro.h"
#include <stdio.h>
#include "mymem.h"

struct profiler_search_result_t search_basic_block_by_ra(mrbc_profile_profiler * prof, void * addr) {
  struct profiler_search_result_t ret = {NULL, NULL};
  struct profiler_search_result_t cur = {prof->functions, NULL};
  size_t minDiff = 1024 * 1024 * 1024; // 1GiB
  addr = MRBC_COPRO_PTR_TO_NATIVE_PTR(addr);
  while(cur.function != NULL) {
    cur.basicblock = cur.function->blocks;
    for(;cur.basicblock != NULL;cur.basicblock = cur.basicblock->next) {
      void * code = (void *)cur.basicblock->allocatedCode;
      struct profile_assertion_block * a = cur.basicblock->asserts;
      head:
      size_t diff = (size_t)addr - (size_t)code;
      if(minDiff > diff) {
        minDiff = diff;
        ret = cur;
      }
      if((uint32_t)a > 2) {
        code = a->allocatedCode;
        a = a->next;
        goto head;
      }
    }
    cur.function = cur.function->next;
  }
  return ret;
}

void profile_dtor(struct VM * vm, mrbc_profile_profiler * p) {
  mrbc_profile_function_header * fh = p->functions;
  while(fh) {
    for(mrbc_profile_basic_block * bb = fh->blocks, * ne = NULL; bb != NULL; bb = ne) {
      for(mrbc_profile_assertion_block * ab = bb->asserts, * ne2 = NULL; (size_t)ab > 2; ab = ne2) {
        ne2 = ab->next;
        mrbcopro_free(vm, ab);
      }
      ne = bb->next;
      mrbcopro_free(vm, bb);
    }
    mrbc_profile_function_header * ne = fh->next;
    mrbcopro_free(vm, fh);
    fh = ne;
  }
  mrbcopro_vector_free(vm, &(p->buf));
  mrbcopro_globalman_clear(vm, &(p->globalMan));
  mrbcopro_objman_clear(vm, &(p->objMan));
  if(p->regsinfo != NULL) mrbcopro_free(vm, p->regsinfo);
  mrbcopro_free(vm, p);
}