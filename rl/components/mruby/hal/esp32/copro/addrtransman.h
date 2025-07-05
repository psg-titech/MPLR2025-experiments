/**
 * @file addrtransman.h
 * @author Go Suzuki (puyogo.suzuki@gmail.com)
 * @brief A header file for the address translator between the main processor and the coprocessor.
 * @version 0.1
 * @date 2025-05-10
 * 
 * @copyright Copyright (c) 2024-2025 Go Suzuki and Programming Systems Group in Institute of Science Tokyo.
 * 
 */

#pragma once
#include "value.h"
#include "value.copro.h"
#include "vector_t.h"

typedef struct addrtransman_t {
  mrbcopro_vector_t translation_table;
} mrbcopro_addrtransman_t;

int mrbcopro_addrtrans_register_typeinfo(struct VM * vm, mrbcopro_addrtransman_t * addrtransman, void * v, mrbc_vtype ty);
RObjectPtrCopro mrbcopro_addrtrans_translate_to_copro(mrbcopro_addrtransman_t * addrtransman, void * main);
mrbc_vtype mrbcopro_addrtrans_retrive_type(mrbcopro_addrtransman_t * addrtransman, void * addr);