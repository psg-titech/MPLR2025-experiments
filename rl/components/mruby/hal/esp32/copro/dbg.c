#include "dbg.h"
#if MRBC_PROF_DBG_ENABLE
const char * dbg_mrbc_type_string[21] = {
  "MRBC_TT_JMPUW",  "MRBC_TT_BREAK", "MRBC_TT_BREAK_BLK", "MRBC_TT_RETURN", "MRBC_TT_HANDLE",
  "MRBC_TT_EMPTY", "MRBC_TT_NIL", "MRBC_TT_FALSE", "MRBC_TT_TRUE", "MRBC_TT_INTEGER",
  "MRBC_TT_FLOAT", "MRBC_TT_SYMBOL", "MRBC_TT_CLASS", "MRBC_TT_MODULE", "MRBC_TT_OBJECT", "MRBC_TT_PROC",
  "MRBC_TT_ARRAY", "MRBC_TT_STRING", "MRBC_TT_RANGE", "MRBC_TT_HASH", "MRBC_TT_EXCEPTION"
};

void dbg_dump_riscv_code(size_t len, void * buf) {
  for(size_t i = 0; i < len;) {
    uint8_t * v = (uint8_t *)((size_t)buf + i);
    if(((*v) & 3) == 3) {
      dbg_mrbc_prof_print_inst("%p", *((uint32_t *)v));
      i += sizeof(uint32_t);
    } else  {
      dbg_mrbc_prof_print_inst("%p", (uint32_t)(*((uint16_t *)v)));
      i += sizeof(uint16_t);
    }
  }
}

void dbg_dump_riscv_code2(size_t len, void * buf) {
  for(size_t i = 0; i < len;) {
    uint8_t * v = (uint8_t *)((size_t)buf + i);
    if(((*v) & 3) == 3) {
      dbg_mrbc_prof_print_inst("%p: %p", (uint32_t)v, *((uint32_t *)v));
      i += sizeof(uint32_t);
    } else  {
      dbg_mrbc_prof_print_inst("%p: %p", (uint32_t)v, (uint32_t)(*((uint16_t *)v)));
      i += sizeof(uint16_t);
    }
  }
}
#endif