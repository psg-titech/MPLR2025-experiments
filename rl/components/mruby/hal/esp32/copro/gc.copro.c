/**
 * @file gc.copro.c
 * @author Go Suzuki (puyogo.suzuki@gmail.com)
 * @brief An implementation of the garbage collection of mruby/copro, running on the main processor.
 * @version 0.1
 * @date 2025-05-10
 * 
 * @copyright Copyright (c) 2024-2025 Go Suzuki and Programming Systems Group in Institute of Science Tokyo.
 * 
 */

#include "gc.copro.private.h"
#include "value.copro.h"
#include "string.h"
#include <stdio.h>
#define MRBC_GC_B8_ALIGN_REGION_START MRBC_COPRO_GC_B8_ALIGN_REGION_START
#define MRBC_GC_B8_ALIGN_START MRBC_COPRO_GC_B8_ALIGN_START
#define MRBC_GC_B16_ALIGN_REGION_START MRBC_COPRO_GC_B16_ALIGN_REGION_START
#define MRBC_GC_B16_ALIGN_START MRBC_COPRO_GC_B16_ALIGN_START
#define MRBC_GC_B32_ALIGN_REGION_START MRBC_COPRO_GC_B32_ALIGN_REGION_START
#define MRBC_GC_B32_ALIGN_START MRBC_COPRO_GC_B32_ALIGN_START
#define MRBC_GC_BIGGER_REGION_START MRBC_COPRO_GC_BIGGER_REGION_START
#define MRBC_GC_BIGGER_START MRBC_COPRO_GC_BIGGER_START
#define MRBC_GC_BIGGER_ALIGN MRBC_COPRO_GC_BIGGER_ALIGN
#define MRBC_GC_MAX MRBC_COPRO_GC_MAX
#define MRBC_GC_BIGGER_COUNT MRBC_COPRO_GC_BIGGER_COUNT
#define MRBC_GC_BIGGER_ALIGN_LOG MRBC_COPRO_GC_BIGGER_ALIGN_LOG // 4 * 8 (=32) bytes align
#define MRBC_GC_BIGGER_MARK_START MRBC_COPRO_GC_BIGGER_MARK_START
#define MRBC_GC_BIGGER_FROZEN_START MRBC_COPRO_GC_BIGGER_FROZEN_START
#define MRBC_GC_BIGGER_USED_START MRBC_COPRO_GC_BIGGER_USED_START
#define MRBC_GC_BIGGER_START_START MRBC_COPRO_GC_BIGGER_START_START
#define mrbc_gc_space ulp_mrbc_gc_space

extern mrbcopro_heap_info ulp_heaps[5];
// not shared!
mrbcopro_heap_info heaps[5] = {
  {(void *)(uint32_t)MRBC_GC_B8_ALIGN_REGION_START, (void *)(uint32_t)MRBC_GC_B8_ALIGN_START},
  {(void *)(uint32_t)MRBC_GC_B16_ALIGN_REGION_START, (void *)(uint32_t)MRBC_GC_B16_ALIGN_START},
  {(void *)(uint32_t)MRBC_GC_B32_ALIGN_REGION_START, (void *)(uint32_t)MRBC_GC_B32_ALIGN_START},
  {(void *)(uint32_t)MRBC_GC_BIGGER_REGION_START, (void *)(uint32_t)MRBC_GC_BIGGER_START},
  {0, (void *)(uint32_t)(MRBC_GC_MAX)}
};

void mrbcopro_gc_init(void) {
  if(heaps[4].region != 0) return;
  for(int i = 0; i < 5; ++i) {
    void * ctrl = &mrbc_gc_space[(uint32_t)heaps[i].ctrl],
         * rgon = &mrbc_gc_space[(uint32_t)heaps[i].region];
    heaps[i].ctrl = ctrl;
    heaps[i].region = rgon;
    ulp_heaps[i].ctrl = MRBC_COPRO_PTR_TO_NATIVE_PTR(ctrl);
    ulp_heaps[i].region = MRBC_COPRO_PTR_TO_NATIVE_PTR(rgon);
  }
}

static mrbcopro_gc_head_info mrbcopro_gc_get_head(void * p) {
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
    mrbcopro_gc_head_info ret = {i, pp_minus_region >> bbs};
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
  return (mrbcopro_gc_head_info){-1, 0};
}

void mrbcopro_gc_sweep(void) {
  for (int i = 0; i < 3; ++i) {
    size_t maxCtrl = ((size_t)heaps[i].region - (size_t)heaps[i].ctrl) / 3;
    uint32_t * right = (uint32_t *)((size_t)heaps[i].ctrl + maxCtrl);
    uint32_t * frozen = right;
    uint32_t * used = (uint32_t *)((size_t)heaps[i].ctrl + maxCtrl*2);
    for(uint32_t * marked = (uint32_t *)(heaps[i].ctrl); (uint32_t)marked < (uint32_t)right; ++marked, ++used, ++frozen) {
      *used = *marked | *frozen;
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

void mrbcopro_gc_mark(void * p) {
  if(!IS_PTR(p)) return;
  mrbcopro_gc_head_info hi = mrbcopro_gc_get_head(p);
  if(hi.region_idx == -1) return; // Not placed on the managed space!
  uint32_t * ctrl = heaps[hi.region_idx].ctrl;
  ctrl[hi.idx / 32] |= 1 << (hi.idx % 32);

  struct RObjectCopro * pp = (struct RObjectCopro *)p;
  switch(pp->tt) {
    case MRBC_COPRO_TT_INTEGER:
    // case MRBC_TT_FIXNUM:
    case MRBC_COPRO_TT_SYMBOL:
    case MRBC_COPRO_TT_STRING:
    case MRBC_COPRO_TT_CODE:
    break;
    // case MRBC_COPRO_TT_KEYVALUE:
    //   struct RKeyValueCopro * kv = (struct RKeyValueCopro *)&(pp[1]);
    //   mrbc_copro_gc_mark(mark, (void *)(uint32_t)(MRBC_COPRO_PTR_TO_NATIVE_PTR(kv->value)));
    //   mrbc_copro_gc_mark(mark, (void *)(uint32_t)(MRBC_COPRO_PTR_TO_NATIVE_PTR(kv->next)));
    //   break;
    case MRBC_COPRO_TT_ARRAY: // same as default.
    default:
      struct RInstanceCopro * instance = (struct RInstanceCopro *)pp;
      for(int i = 0; i < instance->size; ++i)
          mrbcopro_gc_mark((void *)(MRBC_COPRO_PTR_TO_NATIVE_PTR(instance->data[i])));
      break;
  }
}

void mrbcopro_gc_gc(void) {
  mrbcopro_gc_sweep();
}

void mrbcopro_gc_clear(void) {
  for(int i = 0; i <= 3; i++) {
    memset(heaps[i].ctrl, 0, (size_t)heaps[i].region - (size_t)heaps[i].ctrl);
  }
}

void * mrbcopro_gc_alloc(size_t length, int special) {
  int retried = 0;
  void * retVal = 0;
  retry:
  size_t len = 8; // bytes
  if(length <= 32) {
    for(int i = 0; i <= 2; i++, len *= 2) {
      if(len < length) continue;
      size_t ctrl = (size_t)heaps[i].ctrl;
      size_t rgon = (size_t)heaps[i].region;
      for(uint32_t * pj = (uint32_t *)(ctrl + ((rgon - ctrl) / 3 * 2)),
                  * pfj = (uint32_t *)(ctrl + ((rgon - ctrl) / 3)), j = 0;
          (size_t)pj < rgon; j+=32, ++pj, ++pfj) {
        uint32_t a = *pj;
        if(a == (uint32_t)(-1)) continue;
        for(int k = 0; k < 32; ++k) {
          // leading zeros can be used.
          if((a&(1<<k))!=0) continue; // used.
          if(special <= 1) {
            *pj = a | (1 << k);
            if(special == 0)
              *pfj |= (1 << k); // Frozen bit!
          }
          retVal = (void *)(rgon + len*(j+k));
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
        if(special <= 1) {
          for(size_t j = e; j <= i; ++j) 
            mrbc_gc_space[MRBC_GC_BIGGER_START + (MRBC_GC_BIGGER_COUNT / 32 * 2) + (j / 32)]
              |= 1 << (j % 32);
          mrbc_gc_space[MRBC_GC_BIGGER_START + (MRBC_GC_BIGGER_COUNT / 32 * 3) + ek] |= 1 << ej;
          if(special == 0)
            mrbc_gc_space[MRBC_GC_BIGGER_START + (MRBC_GC_BIGGER_COUNT / 32) + ek] |= 1 << ej; // Frozen bit!
        }
        retVal =  (void *)((size_t)&mrbc_gc_space[MRBC_GC_BIGGER_REGION_START] + (MRBC_GC_BIGGER_ALIGN*sizeof(uint32_t)*e));
        goto ret;
      }
    }
    e = i;
  }
  if(retried) return 0;
// dogc:
  retried = 1;
  mrbcopro_gc_gc();
  goto retry;
ret:
  memset(retVal, 0, len);
  return retVal;
}