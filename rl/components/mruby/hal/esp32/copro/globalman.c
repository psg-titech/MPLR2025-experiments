/**
 * @file globalman.c
 * @author Go Suzuki (puyogo.suzuki@gmail.com)
 * @brief An implementation of the Global Manager of mruby/copro. This manages global objects and upvalues outside of Copro primitives.
 * @version 0.1
 * @date 2025-05-10
 * 
 * @copyright Copyright (c) 2025 Go Suzuki and Programming Systems Group in Institute of Science Tokyo.
 * 
 */

#include "globalman.h"
#include "gc.copro.h"
#include "value.copro.h"
#include "dbg.h"
struct globalman_entry_t {
  mrbc_value * key;
  RObjectPtrCopro * value;
};

int mrbcopro_globalman_new(struct VM * vm, struct mrbcopro_globalman_t * gman) {
  gman->current = NULL;
  gman->current_buffer_last = NULL;
  return mrbcopro_vector_new(vm, &(gman->dic), sizeof(struct globalman_entry_t) * 4);
}

static RObjectPtrCopro * add(struct VM * vm, struct mrbcopro_globalman_t * gman) {
  RObjectPtrCopro * ret = (void *)gman->current;
  if(ret >= gman->current_buffer_last) 
  {
    struct RObjectCopro * copro = mrbcopro_gc_alloc(32, 0); // 32 byte allignment.
    if(copro == NULL) return NULL;
    copro->tt = MRBC_COPRO_TT_GLOBALOBJ;
    mrbc_copro_instance * ins = (mrbc_copro_instance *)copro;
    uint8_t size = (32 - sizeof(struct RObjectCopro)) / sizeof(RObjectPtrCopro);
    ins->size = size;
    gman->current = ins->data;
    gman->current_buffer_last = &(ins->data[size]);
    ret = ins->data;
  }
  (gman->current)++;
  return ret;
}

RObjectPtrCopro * mrbcopro_globalman_get(struct VM * vm, struct mrbcopro_globalman_t * gman, mrbc_value * key_addr) {
  struct globalman_entry_t * last = (struct globalman_entry_t *)(gman->dic.cur);
  for(struct globalman_entry_t * cur = (struct globalman_entry_t *)(gman->dic.head);
      cur < last; ++cur) {
    if(cur->key == key_addr) return cur->value;
  }
  RObjectPtrCopro * new_value = add(vm, gman);
  if(new_value == NULL) return NULL;
  struct globalman_entry_t v = {.key = key_addr, .value = new_value};
  dbg_mrbc_prof_printf("[GLOBALMAN] a new entry(key = %x, value = %x) is added.", key_addr,  new_value);
  if(mrbcopro_vector_append(vm, &(gman->dic), sizeof(v), (char *)(&v))) return NULL;
  return new_value;
}

void mrbcopro_globalman_clear(struct VM * vm, struct mrbcopro_globalman_t * gman) {
  mrbcopro_vector_free(vm, &(gman->dic));
}

int mrbcopro_globalman_send(struct VM * vm, struct mrbcopro_globalman_t * gman, mrbcopro_objman_t * objman){
  int count = 0;
  struct globalman_entry_t * last = (struct globalman_entry_t *)(gman->dic.cur);
  for(struct globalman_entry_t * cur = (struct globalman_entry_t *)(gman->dic.head);
      cur < last; ++cur) {
    mrbc_value * src = cur->key;
    dbg_mrbc_prof_printf("[GLOBALMAN] sent copro:%x <--> main:%x(type:%d)", src, cur->value, src->tt);
    switch(src->tt) {
      case MRBC_TT_INTEGER:
        *(cur->value) = (RObjectPtrCopro)((src->i << 1) + 1);
        break;
      case MRBC_TT_FALSE:
        *(cur->value) = (RObjectPtrCopro)FALSEOBJECT_COPRO;
        break;
      case MRBC_TT_TRUE:
        *(cur->value) = (RObjectPtrCopro)TRUEOBJECT_COPRO;
        break;
      default:
        RObjectPtrCopro translated = mrbcopro_addrtrans_translate_to_copro(&(objman->addrtrans), src->instance);
        if(translated != 0)
          *(cur->value) = translated;
        else {
          if(mrbcopro_addrtrans_register_typeinfo(vm, &(objman->addrtrans), src->instance, src->tt)) return 1;
          *(cur->value) = (RObjectPtrCopro)(src->instance);
        }
        break;
    }
    count++;
  }
  dbg_mrbc_prof_printf("[GLOBALMAN] Object Sent: count=%d", count);
  return 0;
}

int mrbcopro_globalman_receive(struct VM * vm, struct mrbcopro_globalman_t * gman, mrbcopro_objman_t * objman){
  int count = 0;
  struct globalman_entry_t * last = (struct globalman_entry_t *)(gman->dic.cur);
  for(struct globalman_entry_t * cur = (struct globalman_entry_t *)(gman->dic.head);
      cur < last; ++cur) {
    mrbc_value * dst = cur->key;
    RObjectPtrCopro src = *(cur->value);
    if(mrbcopro_objman_translate_from_copro(vm, objman, dst, src)) return 1;
    count++;
  }
  dbg_mrbc_prof_printf("[GLOBALMAN] Object Received: count=%d", count);
  return 0;
}