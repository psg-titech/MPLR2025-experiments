/**
 * @file gc.copro.private.h
 * @author Go Suzuki (puyogo.suzuki@gmail.com)
 * @brief Garbage Collector for the ULP coprocessor, running on the main processor.
 * @version 0.1
 * @date 2025-04-25
 * 
 * @copyright Copyright (c) 2024- Go Suzuki and Programming Systems Group in Institute of Science Tokyo.
 * 
 */
#pragma once
#include "gc.copro.h"


typedef struct mrbcopro_heap_info {
  void * region;
  void * ctrl;
} mrbcopro_heap_info;
extern mrbcopro_heap_info heaps[5];

typedef struct mrbcopro_gc_head_info{
  int region_idx; // region index
  uint16_t idx;   // index in region
} mrbcopro_gc_head_info;

void mrbcopro_gc_sweep(void);