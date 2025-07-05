/**
 * @file addrtransman.c
 * @author Go Suzuki (puyogo.suzuki@gmail.com)
 * @brief An implementation for the address translator between the main processor and the coprocessor.
 * @version 0.1
 * @date 2025-05-10
 * 
 * @copyright Copyright (c) 2024-2025 Go Suzuki and Programming Systems Group in Institute of Science Tokyo.
 * 
 */

#include "addrtransman.h"
#include "addrtransman.private.h"
#include "dbg.h"

mrbcopro_addrtrans_entry_t * mrbcopro_addrtrans_get(mrbcopro_addrtransman_t * addrtransman, RObjectPtrCopro copro, void * main) {
  mrbcopro_addrtrans_entry_t * last = (mrbcopro_addrtrans_entry_t *)(addrtransman->translation_table.cur);
  for(mrbcopro_addrtrans_entry_t * v = (mrbcopro_addrtrans_entry_t *)(addrtransman->translation_table.head);
      v < last; ++v) {
    if(v->copro == copro || v->main == main) return v;
  }
  return NULL;
}

int mrbcopro_addrtrans_register_typeinfo(struct VM * vm, mrbcopro_addrtransman_t * addrtransman, void * v, mrbc_vtype ty) {
  mrbcopro_addrtrans_entry_t * e = mrbcopro_addrtrans_get(addrtransman, -1, v);
  if(e) {
    e->ty = ty;
    return 0;
  }
  
  mrbcopro_addrtrans_entry_t e_new = {.copro = 0, .main = v, .ty = ty};
  dbg_mrbc_prof_printf("[OBJMAN] Translation Registered type=%d, main=%x, copro=0", e_new.ty, e_new.main);
  return mrbcopro_vector_append(vm, &(addrtransman->translation_table), sizeof(e_new), (char *)&e_new);
}

int mrbcopro_addrtrans_register(struct VM * vm, mrbcopro_addrtransman_t * addrtransman, RObjectPtrCopro copro, mrbc_value * v) {
  mrbcopro_addrtrans_entry_t * e = mrbcopro_addrtrans_get(addrtransman, copro, v->instance);
  if(e) goto already;
  mrbcopro_addrtrans_entry_t e_new = {.copro = copro, .main = v->instance, .ty = v->tt};
  dbg_mrbc_prof_printf("[OBJMAN] Translation Registered type=%d, main=%x, copro=%x", e_new.ty, e_new.main, e_new.copro);
  return mrbcopro_vector_append(vm, &(addrtransman->translation_table), sizeof(e_new), (char *)&e_new);
already:
  e->ty = v->tt;
  e->main = v->instance;
  e->copro = copro;
  dbg_mrbc_prof_printf("[OBJMAN] Translation Updated type=%d, main=%x, copro=%x", e->ty, e->main, e->copro);
  return 0;
}

RObjectPtrCopro mrbcopro_addrtrans_translate_to_copro(mrbcopro_addrtransman_t * addrtransman, void * main) {
  mrbcopro_addrtrans_entry_t * e = mrbcopro_addrtrans_get(addrtransman, 0, main);
  if(e)
    return e->copro;
  return 0;
}
mrbc_vtype mrbcopro_addrtrans_retrive_type(mrbcopro_addrtransman_t * addrtransman, void * addr) {
  mrbcopro_addrtrans_entry_t * e = mrbcopro_addrtrans_get(addrtransman, 0, addr);
  if(e)
    return e->ty;
  return MRBC_TT_EMPTY;
}
