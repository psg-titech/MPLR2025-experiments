/**
 * @file gc.c
 * @author Go Suzuki (puyogo.suzuki@gmail.com)
 * @brief An implementation of the garbage collection of mruby/copro.
 * @version 0.1
 * @date 2025-04-24
 * 
 * @copyright Copyright (c) 2024-2025 Go Suzuki and Programming Systems Group in Institute of Science Tokyo.
 * 
 */

#include "gc.h"
uint32_t mrbc_gc_space[MRBC_GC_MAX];
#if GCTEST
#include <stdio.h>
#endif

struct heapInfo {
  RObjectPtr region;
  RObjectPtr ctrl;
};

struct heapInfo heaps[5] = {
  {(RObjectPtr)MRBC_GC_B8_ALIGN_REGION_START, (RObjectPtr)MRBC_GC_B8_ALIGN_START},
  {(RObjectPtr)MRBC_GC_B16_ALIGN_REGION_START, (RObjectPtr)MRBC_GC_B16_ALIGN_START},
  {(RObjectPtr)MRBC_GC_B32_ALIGN_REGION_START, (RObjectPtr)MRBC_GC_B32_ALIGN_START},
  {(RObjectPtr)MRBC_GC_BIGGER_REGION_START, (RObjectPtr)MRBC_GC_BIGGER_START},
  {(RObjectPtr)0, (RObjectPtr)MRBC_GC_MAX}
};

#if GCTEST
void mrbc_gc_init(void) {
  if(heaps[4].region != 0) return;
  for(int i = 0; i < 5; ++i) {
    heaps[i].ctrl = (RObjectPtr)(&mrbc_gc_space[(size_t)heaps[i].ctrl]);
    heaps[i].region = (RObjectPtr)(&mrbc_gc_space[(size_t)heaps[i].region]);
  }
}
#else
// THIS IS DONE BY THE MAIN PROCESSOR.
#endif

struct headInfo {
  uint32_t * ptr; // ctrl pointer.
  uint16_t idx;
};

static struct headInfo mrbc_get_head(RObjectPtr p) {
  size_t pp = (size_t)p;
  // Out of bounds Check.
  for(size_t i = 0, bs = 3; i < 4; ++i, bs++) {
    size_t region = (size_t)(heaps[i].region);
    size_t ctrl1 = (size_t)(heaps[i+1].ctrl);
    if(pp < region) goto fail; // check lower bound
    if(pp >= ctrl1) continue;  // check upper bound
    size_t pp_minus_region = pp - region;
    size_t bbs = (i == 3 ? MRBC_GC_BIGGER_ALIGN_LOG : bs);
    if(((pp_minus_region >> bbs) << bbs) != pp_minus_region) goto fail; // check alignment
    struct headInfo ret = {(uint32_t *)(size_t)(heaps[i].ctrl), pp_minus_region >> bbs};
    size_t ctrl = (size_t)(heaps[i].ctrl);
    uint32_t * u = (uint32_t *)(ctrl + (((region - ctrl) / (i == 3 ? 4 : 3)) * 2));
    uint32_t uu = u[ret.idx / 32];
    if(i == 3) { // start bit
      uint32_t * s = (uint32_t *)(ctrl + (((region - ctrl) / 4) * 3));
      uu &= s[ret.idx / 32];
    }
    if((uu >> (ret.idx % 32) & 1) == 0) // check used bit.
      goto fail;
    return ret;
  }
  fail:
  return (struct headInfo){NULL, 0};
}

void mrbc_gc_sweep(void) {
  for (int i = 0; i < 3; ++i) {
    size_t maxCtrl = ((size_t)heaps[i].region - (size_t)heaps[i].ctrl) / (i == 3 ? 4 : 3);
    uint32_t * right = (uint32_t *)((size_t)heaps[i].ctrl + maxCtrl);
    uint32_t * used = (uint32_t *)((size_t)heaps[i].ctrl + maxCtrl*2);
    for(uint32_t * marked = (uint32_t *)(heaps[i].ctrl); (uint32_t)marked < (uint32_t)right; ++marked, ++used) {
      *used = *marked;
      *marked = 0;
    }
  }
  {
    uint32_t * marked = &mrbc_gc_space[MRBC_GC_BIGGER_START];
    uint32_t * frozen = &marked[MRBC_GC_BIGGER_COUNT / 32];
    uint32_t * used = &frozen[MRBC_GC_BIGGER_COUNT / 32];
    uint32_t * start = &used[MRBC_GC_BIGGER_COUNT / 32];
    int state = 0;
    uint32_t bit = (uint32_t)1 << 31;
    for (size_t i = 0; i < MRBC_GC_BIGGER_COUNT; marked++, frozen++, used++, start++) {
      uint32_t  s = *start,
            new_s = (*marked|*frozen)&s,
            new_u = 0;
      for(int j = 0; j < 32 && i < MRBC_GC_BIGGER_COUNT; ++j, ++i, s >>= 1) {
        new_u = new_u >> 1;
        if(s&1) state = (new_s>>j)&1;
        if(state) new_u |= bit;
      }
      *used = new_u & *used;
      *start = new_s;
      *marked = 0;
    }
  }
}

#if GCTEST
#define IS_PTR(p) 1
#endif
void mrbc_gc_mark(RObjectPtr p) {
  if(!IS_PTR(p)) return;
  struct headInfo hi = mrbc_get_head(p);
  if(hi.ptr == NULL) return; // Not placed on the managed space!
  hi.ptr[hi.idx / 32] |= 1 << (hi.idx % 32);
#if !GCTEST
  struct RObject * pp = (struct RObject *)(size_t)p;
  switch(pp->tt) {
    case MRBC_TT_INTEGER:
    // case MRBC_TT_FIXNUM:
    case MRBC_TT_SYMBOL:
    case MRBC_TT_STRING:
    case MRBC_TT_CODE:
    break;
    // case MRBC_TT_KEYVALUE:
    //   struct RKeyValue * kv = (struct RKeyValue *)&(pp[1]);
    //   mrbc_gc_mark(kv->value);
    //   mrbc_gc_mark(kv->next);
    //   break;
    case MRBC_TT_ARRAY: // same as default.
    default:
      struct RInstance * instance = (struct RInstance *)pp;
      for(int i = 0; i < instance->size; ++i)
        mrbc_gc_mark(instance->data[i]);
      break;
  }
#endif
}

void mrbc_gc_mark_frozen_objects(void) {
  for (int i = 0; i < 4; ++i) {
    size_t maxCtrl = ((size_t)heaps[i].region - (size_t)heaps[i].ctrl) / (i == 3 ? 4 : 3);
    uint32_t * marked = (uint32_t *)(heaps[i].ctrl);
    uint32_t * frozen =  (uint32_t *)((size_t)marked + maxCtrl);
    uint32_t * right = (uint32_t *)((size_t)frozen + maxCtrl);
    uint32_t shift = i == 3 ? 8 : (2 << i);
    for(uint32_t * region = (uint32_t *)heaps[i].region;
        (uint32_t)frozen < (uint32_t)right; ++marked, ++frozen) {
      uint32_t tomark = *frozen;// & (~(*marked));
      if(tomark == 0)
        region += shift*(sizeof(uint32_t));
      else
        for(int i = 0; i < 32; ++i, region += shift) {
          if((tomark & (1 << i)) == 0) continue;
          if((uint32_t)region >= (uint32_t)(&(mrbc_gc_space[MRBC_GC_MAX]))) return;
          mrbc_gc_mark((RObjectPtr)region);
        }
    }
  }
}

#if !GCTEST
extern void  * __stack_top;
#endif
void mrbc_gc_gc(void) {
#if !GCTEST
  uint32_t * nowsp = 0;
  asm volatile("c.mv %0, sp": "=r"(nowsp));
  for(; nowsp < (uint32_t *)&__stack_top; ++nowsp)
    mrbc_gc_mark(*nowsp);
  mrbc_gc_mark_frozen_objects();
  mrbc_gc_sweep();
#endif
}
RObjectPtr mrbc_gc_alloc(size_t length) {
  int retried = 0;
  RObjectPtr retVal = 0;
  retry:
  size_t len = 8; // bytes
  if(length <= 32) {
    for(int i = 0; i <= 2; i++, len *= 2) {
      if(len < length) continue;
      size_t ctrl = (size_t)heaps[i].ctrl;
      size_t rgon = (size_t)heaps[i].region;
      for(uint32_t * pj = (uint32_t *)(ctrl + ((rgon - ctrl) / 3 * 2)), j = 0;
          (size_t)pj < rgon; j+=32, ++pj) {
        uint32_t a = *pj;
        if(a == (uint32_t)(-1)) continue;
        for(int k = 0; k < 32; ++k) {
          // leading zeros can be used.
          if((a&(1<<k))!=0) continue; // used.
          *pj = a | (1 << k);
          retVal = (RObjectPtr)(rgon + len*(j+k));
          goto ret;
        }
      }
    }
  }
  len = (length + ((MRBC_GC_BIGGER_ALIGN * 4) - 1)) / (MRBC_GC_BIGGER_ALIGN * 4);
  const size_t maxCount = ((size_t)(&mrbc_gc_space[MRBC_GC_MAX]) - (size_t)(&mrbc_gc_space[MRBC_GC_BIGGER_REGION_START]))/(MRBC_GC_BIGGER_ALIGN*sizeof(uint32_t));
  for(size_t e = 0; e < maxCount;) {
    size_t ej = e % 32, ek = e / 32;
    if(((mrbc_gc_space[MRBC_GC_BIGGER_START + (MRBC_GC_BIGGER_COUNT / 32 * 2) + ek] >> ej) & 1) != 0) { // it's used.
      e++; continue;
    }
    size_t i = e;
    for(; i < maxCount; ++i) {
      size_t ij = i % 32, ik = i / 32, ikk = MRBC_GC_BIGGER_START + (MRBC_GC_BIGGER_COUNT / 32 * 2) + ik;
      if(((mrbc_gc_space[ikk] >> ij) & 1) != 0) break; // confliction...
      if(i - e + 1 >= len) { // gotcha!
        for(size_t j = e; j <= i; ++j) 
          mrbc_gc_space[MRBC_GC_BIGGER_START + (MRBC_GC_BIGGER_COUNT / 32 * 2) + (j / 32)]
            |= 1 << (j % 32);
        mrbc_gc_space[MRBC_GC_BIGGER_START + (MRBC_GC_BIGGER_COUNT / 32 * 3) + ek] |= 1 << ej;
        retVal = (RObjectPtr)((size_t)&mrbc_gc_space[MRBC_GC_BIGGER_REGION_START] + (MRBC_GC_BIGGER_ALIGN*sizeof(uint32_t)*e));
        goto ret;
      }
    }
    e = i;
  }
  if(retried) return 0;
// dogc:
  retried = 1;
  mrbc_gc_gc();
  goto retry;
ret:
  RObjectPtr right = retVal + len;
  for(int * i = (int *)retVal; (RObjectPtr)i < right; ++i)
    *i=0;
  return retVal;
}

#if GCTEST
void print_bits(uint32_t num) {
  for(int i = 0; i < 32; ++i)
    printf((num >> i)&1 ? "1" : "0");
}
void print_state(void) {
  for(int i = 0, ali = 8; i < 3; ++i, ali *= 2) {
    uint32_t * ctrl = heaps[i].ctrl;
    uint32_t * region = heaps[i].region;
    size_t N = ((size_t)region - (size_t)ctrl) / 3 / sizeof(uint32_t);
    printf("alignment = %d, address = %#llx\n", ali, region);
    printf("MARK:");
    for(int j = 0; j < N; ++j) {
      print_bits(ctrl[j]);
    }
    printf("\n");
    printf("USED:");
    for(int j = 0; j < N; ++j) {
      print_bits(ctrl[j + N*2]);
    }
    printf("\n");
  }
  {
    printf("alignment = 32(larger heap), address = %#llx\n", &mrbc_gc_space[MRBC_GC_BIGGER_REGION_START]);
    uint32_t * ctrl = &mrbc_gc_space[MRBC_GC_BIGGER_START];
    uint32_t * region = &mrbc_gc_space[MRBC_GC_BIGGER_REGION_START];
    size_t N = (size_t)((size_t)region - (size_t)ctrl) / 4 / sizeof(uint32_t);
    printf("MARK:");
    for(int j = 0; j < N; ++j) {
      print_bits(ctrl[j]);
    }
    printf("\n");
    printf("USED:");
    for(int j = 0; j < N; ++j) {
      print_bits(ctrl[j + N*2]);
    }
    printf("\n");
    printf("STRT:");
    for(int j = 0; j < N; ++j) {
      print_bits(ctrl[j + N*3]);
    }
    printf("\n");
  }
}

int main(void) {
  printf("sizeof(size_t) = %d\n", sizeof(size_t));
  mrbc_gc_init();
  while(1) {
    size_t v = 0;
    int mark = 1;
    char buf[2];
    print_state();
    printf("Action[a,m,s]:");
    scanf("%1s", buf);
    printf("\n");
    switch(buf[0]) {
      case 'a':
        printf("Allocation bytes: ");
        scanf("%zu", &v);
        printf("\n");
        printf("Return = %#zx\n", mrbc_gc_alloc(v));
        break;
      case 'm':
        printf("Mark at:");
        scanf("%zx", &v);
        printf("\nmarking %#zx...\n", v);
        mrbc_gc_mark((RObjectPtr)v);
        break;
      case 's':
        printf("Sweep\n");
        mrbc_gc_sweep();
        mark = !mark;
        break;
    }
  }
}
#endif