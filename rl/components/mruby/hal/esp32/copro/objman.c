/**
 * @file objman.c
 * @author Go Suzuki (puyogo.suzuki@gmail.com)
 * @brief An implementation of the object management of mruby/copro, running on the main processor.
 * @version 0.1
 * @date 2025-06-03
 * 
 * @copyright Copyright (c) 2024-2025 Go Suzuki and Programming Systems Group in Institute of Science Tokyo.
 * 
 */
#include "inst_skip.h"
#include <stdint.h>
#include "objman.h"
#include "vector_t.h"
#include "string.h"
#include "value.copro.h"
#include "value.h"
#include "c_string.h"
#include "c_array.h"
#include "c_object.h"
#include "gc.copro.private.h"
#include "addrtransman.private.h"
#include "dbg.h"
#include "symbol.h"
#include "mymem.h"

int mrbcopro_objman_new(struct VM * vm, mrbcopro_objman_t * objman) {
  objman->next_objid = MRBC_COPRO_TT_OBJECT_START + 1;
  mrbcopro_vector_new(vm, &(objman->decref_buf), sizeof(void *) * 16);
  mrbcopro_vector_new(vm, &(objman->addrtrans.translation_table), sizeof(void *) * 16);
  mrbcopro_objinfo * new_objinfo = (mrbcopro_objinfo *)mrbcopro_alloc(vm, sizeof(mrbcopro_objinfo));
  memset(new_objinfo, 0, sizeof(mrbcopro_objinfo));
  new_objinfo->cls = (mrbc_class *)&mrbc_class_Object;
  new_objinfo->copro_objid = MRBC_COPRO_TT_OBJECT_START;
  objman->obj_infos = new_objinfo;
  return 0;
}

struct collector_payload {
  struct VM * vm;
  mrbcopro_vector_t vec;
  mrbc_irep * irep;
};

int collector(const uint8_t * inst_buffer, struct collector_payload * payload) {
  switch(inst_buffer[0])  {
    case OP_GETIV: case OP_SETIV:
    mrbc_sym iv_sym = mrbc_irep_symbol_id(payload->irep, inst_buffer[2]);
    char const * iv_sym_str = symid_to_str(iv_sym);
    iv_sym = str_to_symid(iv_sym_str+1); // skip "@"
    for(mrbc_sym * s = (mrbc_sym *)payload->vec.head,
                 * right = (mrbc_sym *)payload->vec.cur; s < right; ++s)
      if(*s == iv_sym) return 0; // found!
    mrbcopro_vector_append16(payload->vm, &(payload->vec), (uint16_t)iv_sym);
    break;
  }
  return 0;
}

void recursive_irep(mrbc_irep * irep, struct collector_payload * payload) {
  inst_analyze(irep, (inst_analyzer)collector, payload);
  for(int n = 0; n < irep->rlen; ++n) {
    struct collector_payload payload2 = *payload;
    payload2.irep = mrbc_irep_child_irep(irep, n);
    recursive_irep(payload2.irep, &payload2);
  }
}

void c_object_getiv(struct VM *vm, mrbc_value v[], int argc);
mrbcopro_objinfo * get_obj_info(struct VM * vm, mrbcopro_objman_t * objman, mrbc_class * cls);
mrbcopro_objinfo * create_hashtbl(struct VM * vm, mrbcopro_objman_t * objman, mrbc_class * cls) {
  dbg_mrbc_prof_printf("[OBJMAN] created for %s (tt:%d)", symid_to_str(cls->sym_id), objman->next_objid);
  struct collector_payload payload;
  #define LEN (sizeof(mrbc_sym) * 8)
  if(cls->super != NULL) {
    mrbcopro_objinfo * sup = mrbcopro_objman_get_obj_info(vm, objman, cls->super);
    mrbcopro_vector_new(vm, &(payload.vec), LEN + ( sizeof(mrbc_sym) * sup->len));
    memcpy(payload.vec.head, sup->sym, sizeof(mrbc_sym) * sup->len);
  } else
    mrbcopro_vector_new(vm, &(payload.vec), LEN);
  #undef LEN
  for(mrbc_method * meth = cls->method_link; meth != NULL; ) {
    if(meth->c_func == 0) { // irep
      payload.irep = meth->irep;
      recursive_irep(meth->irep, &payload);
    } else if(meth->func == c_object_getiv) {// getter attribute
      mrbc_sym iv_sym = meth->sym_id;
      for(mrbc_sym * s = (mrbc_sym *)payload.vec.head,
                   * right = (mrbc_sym *)payload.vec.cur; s < right; ++s)
        if(*s == iv_sym) goto skip;
      mrbcopro_vector_append16(vm, &(payload.vec), iv_sym);
    }
    skip: meth = meth->next;
  }
  int len = ((size_t)payload.vec.cur - (size_t)payload.vec.head) / sizeof(mrbc_sym);
  mrbcopro_objinfo * new_objinfo = (mrbcopro_objinfo *)mrbcopro_alloc(vm, sizeof(mrbcopro_objinfo) + (len * sizeof(mrbc_sym)));
  new_objinfo->cls = cls;
  new_objinfo->copro_objid = objman->next_objid;
  objman->next_objid++;
  new_objinfo->next = objman->obj_infos;
  new_objinfo->len = len;
  memcpy(new_objinfo->sym, payload.vec.head, sizeof(mrbc_sym) * len);
  mrbcopro_vector_free(vm, &(payload.vec));
  objman->obj_infos = new_objinfo;
  return new_objinfo;
}

mrbcopro_objinfo * mrbcopro_objman_get_obj_info(struct VM * vm, mrbcopro_objman_t * objman, mrbc_class * cls) {
  while(cls->flag_alias) cls = cls->aliased;
  for(mrbcopro_objinfo * cur = objman->obj_infos; cur != NULL; cur = cur->next)
    if(cur->cls == cls) return cur;
  return create_hashtbl(vm, objman, cls);
}
mrbcopro_objinfo * mrbcopro_objman_get_obj_info2(mrbcopro_objman_t * objman, mrbc_copro_vtype ty) {
  for(mrbcopro_objinfo * cur = objman->obj_infos; cur != NULL; cur = cur->next)
    if(cur->copro_objid == ty) return cur;
  return NULL;
}
int mrbcopro_objman_get_fieldoffset(struct VM * vm, mrbcopro_objinfo * obj, mrbc_sym sym) {
  for(int ret = 0; ret < obj->len; ++ret) {
    if(obj->sym[ret] == sym) return ret*sizeof(RObjectPtrCopro) + CLASS_IDENTIFIER_SIZE/8 + sizeof(uint16_t);
  }
  return 0;
}

void mrbcopro_objman_clear(struct VM * vm, mrbcopro_objman_t * objman) {
  for(mrbcopro_objinfo * oi = objman->obj_infos, * ne = NULL;
    oi != NULL; oi = ne) {
    ne = oi->next;
    mrbcopro_free(vm, oi);
  }
  mrbcopro_vector_free(vm, &(objman->decref_buf));
  mrbcopro_vector_free(vm, &(objman->addrtrans.translation_table));
  objman->next_objid = MRBC_COPRO_TT_OBJECT_START;
}

RObjectPtrCopro mrbcopro_objman_to2(struct VM * vm, mrbcopro_objman_t * objman, mrbc_value * src, int * success) {
  *success = 0;
  switch(src->tt) {
    case MRBC_TT_EMPTY: return 0;
    case MRBC_TT_NIL: *success = 1; return 0;
    case MRBC_TT_FALSE: *success = 1; return FALSEOBJECT_COPRO;
    case MRBC_TT_TRUE: *success = 1; return TRUEOBJECT_COPRO;
    case MRBC_TT_INTEGER: *success = 1; return (src->i << 1) + 1;
    case MRBC_TT_FLOAT: assert(0); return 0; 
    case MRBC_TT_SYMBOL: assert(0); return 0; // TODO
    case MRBC_TT_PROC: assert(0); return 0;
    case MRBC_TT_RANGE: assert(0); return 0;
    case MRBC_TT_HASH: assert(0); return 0;
    case MRBC_TT_EXCEPTION: assert(0); return 0;
    case MRBC_TT_STRING:
    case MRBC_TT_ARRAY:
    case MRBC_TT_OBJECT:
    case MRBC_TT_CLASS:
    case MRBC_TT_MODULE:
      *success = (mrbcopro_addrtrans_register_typeinfo(vm, &(objman->addrtrans), src->instance, src->tt) == 0);
      RObjectPtrCopro ret = mrbcopro_addrtrans_translate_to_copro(&(objman->addrtrans), src->instance);
      return ret != 0 ? MRBC_COPRO_PTR_TO_NATIVE_PTR(ret) : (RObjectPtrCopro)(src->instance);
    default: assert(0); return 0;
  }
}

RObjectPtrCopro mrbcopro_objman_to4(struct VM * vm, mrbcopro_objman_t * objman, mrbc_value * src)  {
  int succ = 1;
  RObjectPtrCopro ret = 0;
  switch(src->tt) {
    case MRBC_TT_STRING: {
      int str_len = mrbc_string_size(src);
      mrbc_copro_rstring * rs = (mrbc_copro_rstring *)mrbcopro_gc_alloc(sizeof(mrbc_copro_instance) + sizeof(char) * str_len, 0);
      rs->size = str_len;
      rs->tt = MRBC_COPRO_TT_STRING;
      memcpy(rs->str, src->string->data, sizeof(char) * str_len);
      return (RObjectPtrCopro)MRBC_COPRO_PTR_TO_NATIVE_PTR(rs);
    }
    case MRBC_TT_OBJECT: {
      mrbcopro_objinfo * oi = mrbcopro_objman_get_obj_info(vm, objman, src->instance->cls);
      if(oi == NULL) return 0;
      mrbc_copro_instance * co = (mrbc_copro_instance *)mrbcopro_gc_alloc(sizeof(mrbc_copro_instance) + sizeof(RObjectPtrCopro) * oi->len, 0);
      co->size = oi->len;
      co->tt = oi->copro_objid;
      for(int i = 0; i < oi->len; ++i) {
        mrbc_value val = mrbc_instance_getiv(src, oi->sym[i]);
        co->data[i] = mrbcopro_objman_to2(vm, objman, &val, &succ);
        mrbc_decref(&val);
        assert(succ);
      }
      return (RObjectPtrCopro)MRBC_COPRO_PTR_TO_NATIVE_PTR(co);
    }
    case MRBC_TT_ARRAY: {
      int si = mrbc_array_size(src);
      mrbc_copro_array * ary = (mrbc_copro_array *)mrbcopro_gc_alloc(sizeof(mrbc_copro_array) + sizeof(RObjectPtrCopro) * si, 0);
      ary->tt = MRBC_COPRO_TT_ARRAY;
      ary->length = si;
      for(int i = 0; i < si; ++i) {
        mrbc_value val = mrbc_array_get(src, i);
        ary->data[i] = mrbcopro_objman_to2(vm, objman, &val, &succ);
        assert(succ);
      }
      return (RObjectPtrCopro)MRBC_COPRO_PTR_TO_NATIVE_PTR(ary);
    }
    default: return mrbcopro_objman_to2(vm, objman, src, &succ);
  }
  return ret;
}

RObjectPtrCopro mrbcopro_objman_to3(struct VM * vm, mrbcopro_objman_t * objman, mrbc_value * src)  {
  if(src->tt >= MRBC_TT_OBJECT) {
    mrbcopro_addrtrans_entry_t * e =  mrbcopro_addrtrans_get(&(objman->addrtrans), -1, src->instance);
    if(e != NULL && e->copro != 0) return e->copro;
  }
  RObjectPtrCopro ret = mrbcopro_objman_to4(vm, objman, src);
  if(src->tt >= MRBC_TT_OBJECT && ret != NULL) {
    mrbcopro_addrtrans_register(vm, &(objman->addrtrans), ret, src);
  }
  return ret;
}

RObjectPtrCopro mrbcopro_objman_to(struct VM * vm, mrbcopro_objman_t * objman, void * src) {
  mrbcopro_addrtrans_entry_t * e =  mrbcopro_addrtrans_get(&(objman->addrtrans), -1, src);
  if(e == NULL) return 0;
  if(e->copro != 0) return e->copro;
  mrbc_value v;
  v.tt = e->ty;
  v.instance = e->main;
  RObjectPtrCopro ret = mrbcopro_objman_to4(vm, objman, &v);
  e->copro = ret;
  return ret;
}


mrbc_value mrbcopro_objman_from_copro_shallow_without_checking(struct VM * vm, mrbcopro_objman_t * objman, RObjectPtrCopro src) {
  mrbcopro_addrtrans_entry_t * e =  mrbcopro_addrtrans_get(&(objman->addrtrans), src, (void *)-1);
  if(e) return (mrbc_value){.tt = e->ty, .instance=(struct RInstance *)e->main};
  struct RObjectCopro * obj = (struct RObjectCopro *)src;
  switch(obj->tt) {
    case MRBC_COPRO_TT_CODE: return (mrbc_value){.tt = MRBC_TT_NIL};
    case MRBC_COPRO_TT_STRING: {
      mrbc_copro_rstring * str = (mrbc_copro_rstring *)obj;
      return mrbc_string_new_alloc(vm, str->str, str->size);
    }
    case MRBC_COPRO_TT_ARRAY: {
      mrbc_copro_array * ary = (mrbc_copro_array *)obj;
      return mrbc_array_new(vm, ary->length);
    }
    case MRBC_COPRO_TT_SYMBOL:
    default:
      if(obj->tt >= MRBC_COPRO_TT_OBJECT_START) {
        dbg_mrbc_prof_printf("[OBJMAN] Catch addr:%x tt:%d", obj, obj->tt);
        mrbc_copro_instance * ins = (mrbc_copro_instance *)obj;
        mrbcopro_objinfo * oi = mrbcopro_objman_get_obj_info2(objman, ins->tt);
        return mrbc_instance_new(vm, oi->cls, 0);
      } else
        return (mrbc_value){}; // TODO
  }
}

mrbc_value mrbcopro_objman_from_copro_shallow(struct VM * vm, mrbcopro_objman_t * objman, RObjectPtrCopro src) {
  if(src & 1) // this is an integer.
    return (mrbc_value){.tt = MRBC_TT_INTEGER, .i = (int)src >> 1};
  else if (src == 0) 
    return (mrbc_value){.tt = MRBC_TT_NIL};
  else if ((src >> 3) == 0) // this is a boolean.
    return (mrbc_value){.tt = src >> 2 ? MRBC_TT_TRUE : MRBC_TT_FALSE};
  else if (MRBC_COPRO_IS_IN_COPRO(src)){ // is an object
    return mrbcopro_objman_from_copro_shallow_without_checking(vm, objman, src);
  } else {
    return (mrbc_value){.tt = mrbcopro_addrtrans_retrive_type(&(objman->addrtrans), (void *)src), .obj = (struct RBasic *)src};
  }
  return (mrbc_value){};
}

int mrbcopro_objman_safe_decref(struct VM * vm, mrbcopro_objman_t * objman, mrbc_value * v) {
  if( v->tt <= MRBC_TT_INC_DEC_THRESHOLD ) return 0;
  for(size_t * s = (size_t *)(objman->decref_buf.head), * r = (size_t *)(objman->decref_buf.cur);s < r; ++s)
    if(*s == (size_t)(v->obj)) return 0;
  if(v->obj->ref_count == 1) {
    dbg_mrbc_prof_printf("safe_decref %x", v);
    // TODO: check duplication.
    return mrbcopro_vector_append(vm, &(objman->decref_buf), sizeof(mrbc_value), (char *)v);
  }
  mrbc_decref(v);
  return 0;
}

void mrbcopro_objman_cleanup(struct VM * vm, mrbcopro_objman_t * objman) {
  for(mrbc_value * v = (mrbc_value *)(objman->decref_buf.head),
                 * right = (mrbc_value *)(objman->decref_buf.cur); v < right; ++v)
    mrbc_decref(v);
  objman->decref_buf.cur = objman->decref_buf.head;
  objman->addrtrans.translation_table.cur = objman->addrtrans.translation_table.head;
}

int mrbcopro_objman_translate_from_copro(struct VM * vm, mrbcopro_objman_t * objman, mrbc_value * dst, RObjectPtrCopro src) {
  if(mrbcopro_objman_safe_decref(vm, objman, dst)) return 1;
  if(src & 1) { // this is an integer.
    dst->tt = MRBC_TT_INTEGER;
    dst->i = (int)src >> 1;
  } else if (src == 0) { 
    dst->tt = MRBC_TT_NIL;
    dst->i = 0;
  } else if ((src >> 3) == 0) {// this is a boolean.
    dst->tt = src >> 2 ? MRBC_TT_TRUE : MRBC_TT_FALSE;
    dst->i = 0;
  } else {
    mrbcopro_addrtrans_entry_t * e =  mrbcopro_addrtrans_get(&(objman->addrtrans), src, (void *)src);
    if(e == NULL ) return 2;
    dst->tt = e->ty;
    dst->instance = (struct RInstance *)e->main;
    mrbc_incref(dst);
  }
  return 0;
}

int mrbcopro_objman_from_copro_update_value_without_checking(struct VM * vm, mrbcopro_objman_t * objman, RObjectPtrCopro src, mrbc_value * dst) {
  // is an object
  // CHECK ALREADY TRANSLATED.
  struct RObjectCopro * obj = (struct RObjectCopro *)(src);
  switch(dst->tt) {
    case MRBC_TT_STRING: return 0; // DO NOT TOUCH BECAUSE THEY ARE IMMUTABLE.
    case MRBC_TT_ARRAY: {
      mrbc_copro_array * ary = (mrbc_copro_array *)obj;
      mrbc_array * dary = dst->array;
      int arysize = dary->n_stored;
      if(arysize < ary->length)
        mrbc_array_resize(dst, ary->length);
      int i;
      for(i = 0; i < ary->length; ++i) // if rc of each element is 1, objman maintains the reference.
        mrbcopro_objman_translate_from_copro(vm, objman, &(dary->data[i]), ary->data[i]);
      for(;i < arysize; ++i)
        mrbcopro_objman_safe_decref(vm, objman, &(dary->data[i]));
      if(dary->n_stored != ary->length)
        mrbc_array_resize(dst, ary->length);
      return 0;
    }
    case MRBC_TT_OBJECT: {
      mrbc_copro_instance * ins = (mrbc_copro_instance *)obj;
      mrbcopro_objinfo * oi = mrbcopro_objman_get_obj_info2(objman, ins->tt);
      for(int i = 0; i < oi->len; ++i) {
        mrbc_value * v = mrbc_instance_getiv_p(dst, oi->sym[i]);
        if(v != NULL) {
          mrbcopro_objman_translate_from_copro(vm, objman, v, ins->data[i]); // prev.rc-- if prev.rc > 1, then, new.rc++
          // RESULT: prev.rc -= 1, new.rc += 1
        } else {
          mrbc_value vv = {.tt = MRBC_TT_EMPTY};
          mrbcopro_objman_translate_from_copro(vm, objman, &vv, ins->data[i]);
          if(vv.tt > MRBC_TT_NIL) {
            mrbc_instance_setiv(dst, oi->sym[i], &vv);
            mrbc_decref(&vv);
            v = &vv;
          }
        }
      }
      return 0;
    }
    default:
      return 1; // TODO
  }
}

// DO NOT USE.
// mrbc_value mrbcopro_objman_from_copro(struct VM * vm, mrbcopro_objman_t * objman, RObjectPtrCopro src) {
//   if(src & 1) // this is an integer.
//     return (mrbc_value){.tt = MRBC_TT_INTEGER, .i = (int)src >> 1};
//   else if (src == 0) 
//     return (mrbc_value){.tt = MRBC_TT_NIL};
//   else if ((src >> 3) == 0) // this is a boolean.
//     return (mrbc_value){.tt = src >> 2 ? MRBC_TT_TRUE : MRBC_TT_FALSE};
//   else { // is an object
//     // CHECK ALREADY TRANSLATED.
//     struct RObjectCopro * obj = (struct RObjectCopro *)(MRBC_COPRO_NATIVE_PTR_TO_PTR(src));
//     switch(obj->tt) {
//       case MRBC_COPRO_TT_CODE: return (mrbc_value){.tt = MRBC_TT_NIL};
//       case MRBC_COPRO_TT_STRING: {
//         mrbc_copro_rstring * str = (mrbc_copro_rstring *)obj;
//         return mrbc_string_new_alloc(vm, str->str, str->size);
//       }
//       case MRBC_COPRO_TT_ARRAY: {
//         mrbc_copro_array * ary = (mrbc_copro_array *)obj;
//         mrbc_value ret = mrbc_array_new(vm, ary->length);
//         for(int i = 0; i < ary->length; ++i) {
//           mrbc_value v = mrbcopro_objman_from_copro(vm, objman, ary->data[i]);
//           mrbc_incref(&v);
//           mrbc_array_set(&ret, i, &v);
//         }
//         return ret;
//       }
//       case MRBC_COPRO_TT_SYMBOL:
//       default:
//         if(obj->tt >= MRBC_COPRO_TT_OBJECT_START) {
//           mrbc_copro_instance * ins = (mrbc_copro_instance *)obj;
//           mrbcopro_objinfo * oi = mrbcopro_objman_get_obj_info2(objman, ins->tt);
//           mrbc_value ret = mrbc_instance_new(vm, oi->cls, 0);
//           for(int i = 0; i < oi->len; ++i) {
//             mrbc_value v = mrbcopro_objman_from_copro(vm, objman, ins->data[i]);
//             mrbc_instance_setiv(&ret, oi->sym[i], &v);
//           }
//           return ret;
//         } else
//           return (mrbc_value){}; // TODO
//     }
//   }
//   return (mrbc_value){};
// }

int mrbcopro_objman_receive(struct VM * vm, mrbcopro_objman_t * objman) {
  int ret = 0;
  char tbl_size[4] = {
    MRBC_COPRO_GC_B8_COUNT,
    MRBC_COPRO_GC_B16_COUNT,
    MRBC_COPRO_GC_B32_COUNT,
    MRBC_COPRO_GC_BIGGER_COUNT
  };
  tbl_size[3] = ((size_t)&(ulp_mrbc_gc_space[MRBC_COPRO_GC_MAX]) - (size_t)(heaps[3].region)) / 32;
  mrbc_value * tbl[4] = {0};
  for(int i = 0; i < 4; ++i)
  {
    tbl[i] = mrbcopro_alloc(vm, tbl_size[i] * sizeof(mrbc_value));
    if(tbl[i] == NULL) return 1;
    memset(tbl[i], 0, tbl_size[i]);
  }
  // first pass, create objects on the main processor!
  // and remove all marked and frozen bits for the objects.
  for(int i = 0; i < 4; ++i) {
    uint32_t tblsize32 = (i == 3 ? MRBC_COPRO_GC_BIGGER_COUNT : tbl_size[i])/32;
    uint32_t * frozen = &((uint32_t *)heaps[i].ctrl)[tblsize32];
    uint32_t * used = &frozen[tblsize32];
    uint32_t * start = i == 3 ? &used[tblsize32] : used;
    uint32_t * region = (uint32_t *)heaps[i].region;
    mrbc_value * t = tbl[i];
    uint32_t shift = i == 3 ? 8 : (2 << i);
    for(int j = 0; j < tbl_size[i]; used++, start++, frozen++) {
      uint32_t s = *start & *used, f = *frozen;
      for(int k = 0; k < 32 && j < tbl_size[i]; ++k, ++j, region += shift) {
        if((s&(1 << k)) == 0){
          t[j].tt = MRBC_TT_NIL;
          continue;
        }
        t[j] = mrbcopro_objman_from_copro_shallow_without_checking(vm, objman, (RObjectPtrCopro)region);
        if(t[j].tt > MRBC_TT_NIL) {
          mrbcopro_addrtrans_register(vm, &(objman->addrtrans), (RObjectPtrCopro)region, &(t[j]));
          mrbc_incref(&(t[j]));
          f ^= 1 << k;
        }
      }
      *frozen = f;
    }
  }
  // second pass, update objects on the tbl.
  for(int i = 0; i < 4; ++i) {
    uint32_t * region = (uint32_t *)heaps[i].region;
    mrbc_value * t = tbl[i];
    uint32_t shift = i == 3 ? 8 : (2 << i);
    for(int j = 0; j < tbl_size[i]; ++j) {
      if(t[j].tt > MRBC_TT_NIL) {
        dbg_mrbc_prof_printf("[OBJMAN] Updated %x(%d)", t[j].instance, t[j].tt);
        if(mrbcopro_objman_from_copro_update_value_without_checking(vm, objman, (RObjectPtrCopro)region, &(t[j])))
        {
          ret = 1; goto FINALIZER;
        }
      }
      region += shift;
    }
  }
  mrbcopro_gc_sweep();
FINALIZER:
  for(int i = 0; (i < 4) && (tbl[i] != 0); ++i) {
    for(int j = 0; j < tbl_size[i]; ++j)
      mrbcopro_objman_safe_decref(vm, objman, &(tbl[i][j]));
    mrbcopro_free(vm, tbl[i]);
  }
  return ret;
}


mrbc_copro_vtype mrbcopro_objman_type2coprotype(struct VM * vm, mrbcopro_objman_t * objman, mrbc_value * v) {
  if(v->tt == MRBC_TT_OBJECT) {
    dbg_mrbc_prof_printf("v->instance is %p", v->instance);
    mrbcopro_objinfo * res = mrbcopro_objman_get_obj_info(vm, objman, v->instance->cls);
    if(res == NULL)
      return MRBC_TT_NIL;
    return res->copro_objid;
  } else
    return v->tt == MRBC_TT_FALSE ? MRBC_TT_TRUE : (mrbc_copro_vtype)v->tt;
}
