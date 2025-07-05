/*! @file
  @brief
  mruby/c value definitions

  by 2024-2025 Go Suzuki (Science Tokyo), BSD 3-Clause License.

  Some parts of this file are derived from mrubyc/mrubyc/src/value.h (https://github.com/mrubyc/mrubyc)
  Original Copyright:
  <pre>
  Copyright (C) 2015- Kyushu Institute of Technology.
  Copyright (C) 2015- Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.
  </pre>
*/

#pragma once

#include <assert.h>
#include "../../src/value.h"
#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stdint.h>
#include <stddef.h>
#endif

#define MRBC_COPRO_TT_UNKNOWN (uint16_t)-10
//================================================================
/*! value type in mrbc_value.
*/
typedef enum {
  /* (note) Must be same order as mrbc_class_tbl[], mrbc_delfunc[]. */

  /* internal use */
  // MRBC_TT_JMPUW		= -5,  // use in OP_JMPUW...
  // MRBC_TT_BREAK		= -4,
  // MRBC_TT_RETURN_BLK	= -3,
  // MRBC_TT_RETURN	= -2,
  // MRBC_TT_HANDLE	= -1,

  /* primitive */
  // (note) true/false threshold. see op_jmpif

  MRBC_COPRO_TT_EMPTY     = 0,
  // MRBC_TT_NIL       = 1,        //!< NilClass
  // MRBC_TT_FALSE     = 2,        //!< FalseClass
  // (note) true/false threshold. see op_jmpif

  MRBC_COPRO_TT_BOOL = 3,
  // MRBC_TT_TRUE      = 3,        //!< TrueClass
  MRBC_COPRO_TT_INTEGER = 4,		//!< Integer
  MRBC_COPRO_TT_FIXNUM  = 4,
  // MRBC_TT_FLOAT     = 5,        //!< Float
  MRBC_COPRO_TT_SYMBOL  = 6,		//!< Symbol
  MRBC_COPRO_TT_CLASS   = 7,        //!< Class
  MRBC_COPRO_TT_MODULE  = 8,        //!< Module
  // (note) inc/dec ref threshold.

  // /* non-primitive */
  // MRBC_TT_OBJECT    = 9,        //!< General instance
  // MRBC_TT_PROC      = 10,       //!< Proc
  MRBC_COPRO_TT_ARRAY	    = 11,	//!< Array
  MRBC_COPRO_TT_STRING    = 12,	//!< String
  MRBC_COPRO_TT_RANGE     = 13,       //!< Range
  MRBC_COPRO_TT_HASH      = 14,       //!< Hash
  // MRBC_TT_EXCEPTION = 15,       //!< Exception
  MRBC_COPRO_TT_CODE = 16,

  // BELOWS ARE RInstanceCopro.
  MRBC_COPRO_TT_GLOBALOBJ = 17,
  MRBC_COPRO_TT_OBJECT_START = 18,

} mrbc_copro_vtype;

struct RObjectCopro;

#define CLASS_IDENTIFIER_SIZE 16
#define CLASS_IDENTIFIER_TYPE uint16_t
// #if CONFIG_IDF_TARGET_ESP32S3
// typedef uint16_t RObjectPtrCopro;
// #else
typedef uint32_t RObjectPtrCopro;
// #endif

typedef struct RStringCopro{
  mrbc_copro_vtype tt : CLASS_IDENTIFIER_SIZE;
  uint16_t size;
  uint8_t str[];
} mrbc_copro_rstring;

typedef struct RKeyValueCopro{
  mrbc_copro_vtype tt : CLASS_IDENTIFIER_SIZE;
  RObjectPtrCopro next;
  RObjectPtrCopro value;
  struct RStringCopro key;
} mrbc_copro_keyvalue;

typedef struct RInstanceCopro {
  mrbc_copro_vtype tt : CLASS_IDENTIFIER_SIZE;
  // struct RKeyValue * data;
  uint16_t size;
  RObjectPtrCopro data[];
} mrbc_copro_instance;

typedef struct RArrayCopro {
  mrbc_copro_vtype tt : CLASS_IDENTIFIER_SIZE;
  uint16_t length;
  RObjectPtrCopro data[];
} mrbc_copro_array;

//================================================================
/*!@brief
  Value object.
  RObject consists of this struct RObject and a instace for the type.
  The instance follows struct RObject without any padding.
*/
struct RObjectCopro {
  mrbc_copro_vtype tt : CLASS_IDENTIFIER_SIZE;
//   union {
//     mrbc_int_t i;		// MRBC_TT_INTEGER, SYMBOL
// #if MRBC_USE_FLOAT
//     mrbc_float_t d;		// MRBC_TT_FLOAT
// #endif
//     // struct RClass *cls;		// MRBC_TT_CLASS
//     struct RInstanceCopro instance;	// MRBC_TT_OBJECT
//     // struct RProc *proc;		// MRBC_TT_PROC
//     struct RArrayCopro array;	// MRBC_TT_ARRAY
//     struct RStringCopro string;	// MRBC_TT_STRING
//     // TODO.
//     // struct RRange *range;	// MRBC_TT_RANGE
//     // struct RHash *hash;		// MRBC_TT_HASH
//     // struct RException *exception; // MRBC_TT_EXCEPTION
//     struct RKeyValueCopro kv;
//   };
};
// struct RObject * TrueObject;
// struct RObject * FalseObject;
#define NILOBJECT_COPRO (0x0)
#define FALSEOBJECT_COPRO (0x2)
#define TRUEOBJECT_COPRO (0x6)
#define STRINGEMPTYOBJECT_COPRO (0x4)

#define IS_PTR(v) (((size_t)(v) & 0x1) == 0)

/***** Macros ***************************************************************/

// getters
/**
  @def mrbc_type(o)
  get the type (#mrbc_vtype) from mrbc_value.

  @def mrbc_integer(o)
  get int value from mrbc_value.

  @def mrbc_float(o)
  get float(double) value from mrbc_value.

  @def mrbc_symbol(o)
  get symbol value (#mrbc_sym) from mrbc_value.
*/
#define mrbc_copro_type(o)		((o).tt)
#define mrbc_copro_integer(o)		(o & 1 == 1 ? (signed)o >> 1 : (o).i)
// #define mrbc_float(o)		((o).d)
#define mrbc_copro_symbol(o)		((o).i)

// for Numeric values.
/**
  @def MRBC_ISNUMERIC(val)
  Check the val is numeric.

  @def MRBC_TO_INT(val)
  Convert mrbc_value to C-lang int.
*/
#define MRBC_COPRO_ISNUMERIC(val) ((val) & 1 || ((val).tt == MRBC_TT_INTEGER))
#define MRBC_COPRO_TO_INT(val) \
  (val) & 1 ? (signed)(val) >> 1 : \
  ((val).tt == MRBC_TT_INTEGER ? (val).i : 0)

#if CONFIG_IDF_TARGET_ESP32S3
#define MRBC_COPRO_PTR_TO_NATIVE_PTR(p) ((void *)((size_t)(p) + 0x50000000))
#define MRBC_COPRO_NATIVE_PTR_TO_PTR(p) (RObjectPtrCopro)((size_t)(p) - 0x50000000)
#define MRBC_COPRO_NATIVE_PTR_TO_PTR2(p) (void *)((size_t)(p) - 0x50000000)
#define MRBC_COPRO_IS_IN_COPRO(p) ((size_t)(p) < 8*1024)
#else
#define MRBC_COPRO_PTR_TO_NATIVE_PTR(p) (p)
#define MRBC_COPRO_NATIVE_PTR_TO_PTR(p) (p)
#define MRBC_COPRO_NATIVE_PTR_TO_PTR2(p) (p)
#define MRBC_COPRO_IS_IN_COPRO(p) (((size_t)(p) >= 0x50000000) &&((size_t)(p) < 0x50004000))
#endif
#ifdef __cplusplus
}
#endif