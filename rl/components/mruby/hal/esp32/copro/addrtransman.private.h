/**
 * @file addrtransman.private.h
 * @author Go Suzuki (puyogo.suzuki@gmail.com)
 * @brief A private header file for the address translator between the main processor and the coprocessor.
 * @version 0.1
 * @date 2025-05-10
 * 
 * @copyright Copyright (c) 2024-2025 Go Suzuki and Programming Systems Group in Institute of Science Tokyo.
 * 
 */

#pragma once
#include "addrtransman.h"
typedef struct mrbcopro_addrtrans_entry_t {
  RObjectPtrCopro copro;
  void * main;
  mrbc_vtype ty;
} mrbcopro_addrtrans_entry_t;
mrbcopro_addrtrans_entry_t * mrbcopro_addrtrans_get(mrbcopro_addrtransman_t * addrtransman, RObjectPtrCopro copro, void * main);
int mrbcopro_addrtrans_register(struct VM * vm, mrbcopro_addrtransman_t * addrtransman, RObjectPtrCopro copro, mrbc_value * v);