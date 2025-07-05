/**
 * @file objman.c
 * @author Go Suzuki (puyogo.suzuki@gmail.com)
 * @brief A header file of the object management of mruby/copro, running on the main processor.
 * @version 0.1
 * @date 2025-05-10
 * 
 * @copyright Copyright (c) 2024-2025 Go Suzuki and Programming Systems Group in Institute of Science Tokyo.
 * 
 */

#pragma once
#include "../../src/class.h"
#include "../../src/alloc.h"
#include "../../src/value.h"
#include "vector_t.h"
#include "opcode.h"
#include "vm.h"
#include "addrtransman.h"

typedef struct mrbcopro_objinfo {
  struct mrbcopro_objinfo * next;
  mrbc_class * cls;
  uint16_t copro_objid;
  uint16_t len;
  mrbc_sym sym[];
} mrbcopro_objinfo;

typedef struct mrbcopro_objman {
  mrbcopro_objinfo * obj_infos;
  uint16_t next_objid;
  mrbcopro_addrtransman_t addrtrans;
  mrbcopro_vector_t decref_buf;
} mrbcopro_objman_t;

int mrbcopro_objman_new(struct VM * vm, mrbcopro_objman_t * objman);
mrbcopro_objinfo * mrbcopro_objman_get_obj_info(struct VM * vm, mrbcopro_objman_t * objman, mrbc_class * cls);
mrbcopro_objinfo * mrbcopro_objman_get_obj_info2(mrbcopro_objman_t * objman, mrbc_copro_vtype ty);
RObjectPtrCopro mrbcopro_objman_to(struct VM * vm, mrbcopro_objman_t * objman, void * src);
RObjectPtrCopro mrbcopro_objman_to3(struct VM * vm, mrbcopro_objman_t * objman, mrbc_value * src);
int mrbcopro_objman_get_fieldoffset(struct VM * vm, mrbcopro_objinfo * obj, mrbc_sym sym);
void mrbcopro_objman_clear(struct VM * vm, mrbcopro_objman_t * objman);
int mrbcopro_objman_safe_decref(struct VM * vm, mrbcopro_objman_t * objman, mrbc_value * v);
void mrbcopro_objman_cleanup(struct VM * vm, mrbcopro_objman_t * objman);
int mrbcopro_objman_receive(struct VM * vm, mrbcopro_objman_t * objman);
int mrbcopro_objman_translate_from_copro(struct VM * vm, mrbcopro_objman_t * objman, mrbc_value * dst, RObjectPtrCopro src);
mrbc_copro_vtype mrbcopro_objman_type2coprotype(struct VM * vm, mrbcopro_objman_t * objman, mrbc_value * v);

static inline mrbc_vtype mrbcopro_objman_coprotype2type(mrbc_copro_vtype ty) {
  return ty < MRBC_COPRO_TT_OBJECT_START ? (mrbc_copro_vtype)ty : MRBC_TT_OBJECT;
}