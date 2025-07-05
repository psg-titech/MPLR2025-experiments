#pragma once
/*! @file
  @brief
  mruby/c value definitions

  by 2024 Go Suzuki (Tokyo Tech), BSD 3-Clause License.

  Some parts of this file are derived from mrubyc/mrubyc/src/value.h (https://github.com/mrubyc/mrubyc)
  Original Copyright:
  <pre>
  Copyright (C) 2015- Kyushu Institute of Technology.
  Copyright (C) 2015- Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.
  </pre>
*/


#include <stdint.h>
#include <assert.h>
#ifdef __cplusplus
extern "C" {
#endif
// mrbc types
#if defined(MRBC_INT16)
typedef int16_t mrbc_int_t;
typedef uint16_t mrbc_uint_t;
#elif defined(MRBC_INT64)
typedef int64_t mrbc_int_t;
typedef uint64_t mrbc_uint_t;
#else
typedef int32_t mrbc_int_t;
typedef uint32_t mrbc_uint_t;
#endif
typedef mrbc_int_t mrb_int;

#if MRBC_USE_FLOAT == 1
typedef float mrbc_float_t;
#elif MRBC_USE_FLOAT == 2
typedef double mrbc_float_t;
#endif
#if MRBC_USE_FLOAT != 0
typedef mrbc_float_t mrb_float;
#endif

typedef int16_t mrbc_sym;	//!< mruby/c symbol ID
// typedef void (*mrbc_func_t)(struct VM *vm, struct RObject *v, int argc);

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

  // MRBC_TT_EMPTY     = 0,
  MRBC_TT_NIL       = 1,        //!< NilClass
  // MRBC_TT_FALSE     = 2,        //!< FalseClass
  // (note) true/false threshold. see op_jmpif

  MRBC_TT_BOOL = 3,
  // MRBC_TT_TRUE      = 3,        //!< TrueClass
  MRBC_TT_INTEGER = 4,		//!< Integer
  MRBC_TT_FIXNUM  = 4,
  // MRBC_TT_INTEGER   = 4,        //!< Integer
  // MRBC_TT_FIXNUM    = 4,
  // MRBC_TT_FLOAT     = 5,        //!< Float
  MRBC_TT_SYMBOL  = 6,		//!< Symbol
  // MRBC_TT_SYMBOL    = 6,        //!< Symbol
  // MRBC_TT_CLASS     = 7,        //!< Class
  // MRBC_TT_MODULE    = 8,        //!< Module
  // (note) inc/dec ref threshold.

  // /* non-primitive */
  // MRBC_TT_OBJECT    = 9,        //!< General instance
  // MRBC_TT_PROC      = 10,       //!< Proc
  MRBC_TT_ARRAY	    = 11,	//!< Array
  // MRBC_TT_ARRAY     = 11,       //!< Array
  MRBC_TT_STRING    = 12,	//!< String
  // MRBC_TT_STRING    = 12,       //!< String
  // MRBC_TT_RANGE     = 13,       //!< Range
  // MRBC_TT_HASH      = 14,       //!< Hash
  // MRBC_TT_EXCEPTION = 15,       //!< Exception
  MRBC_TT_CODE = 16,
  // BELOWS ARE RInstanceCopro.
  MRBC_TT_GLOBALOBJ = 17,
  MRBC_TT_OBJECT_BEGIN = 18
} mrbc_vtype;

//================================================================
/*! error code for internal use. (BETA TEST)
*/
typedef enum {
  E_NOMEMORY_ERROR = 1,
  E_RUNTIME_ERROR,
  E_TYPE_ERROR,
  E_ARGUMENT_ERROR,
  E_INDEX_ERROR,
  E_RANGE_ERROR,
  E_NAME_ERROR,
  E_NOMETHOD_ERROR,
  E_SCRIPT_ERROR,
  E_SYNTAX_ERROR,
  E_LOCALJUMP_ERROR,
  E_REGEXP_ERROR,
  E_NOTIMP_ERROR,
  E_FLOATDOMAIN_ERROR,
  E_KEY_ERROR,

  // Internal Error
  E_BYTECODE_ERROR,
} mrbc_error_code;

struct RObject;

#define CLASS_IDENTIFIER_SIZE 16
#define CLASS_IDENTIFIER_TYPE uint16_t
#if CONFIG_IDF_TARGET_ESP32S3
typedef uint16_t RObjectPtr;
#else
typedef uint32_t RObjectPtr;
#endif

typedef struct RString{
  mrbc_vtype tt : CLASS_IDENTIFIER_SIZE;
  uint16_t size;
  uint8_t str[];
} mrbc_rstring;

typedef struct RKeyValue{
  mrbc_vtype tt : CLASS_IDENTIFIER_SIZE;
  RObjectPtr next;
  RObjectPtr value;
  struct RString key;
} mrbc_keyvalue;

typedef struct RInstance {
  // struct RKeyValue * data;
  mrbc_vtype tt : CLASS_IDENTIFIER_SIZE;
  uint16_t size;
  RObjectPtr data[];
} mrbc_instance;

typedef struct RArray {
  mrbc_vtype tt : CLASS_IDENTIFIER_SIZE;
  uint16_t length;
  RObjectPtr data[];
} mrbc_array;

//================================================================
/*!@brief
  Value object.
  RObject consists of this struct RObject and a instace for the type.
  The instance follows struct RObject without any padding.
*/
struct RObject {
  mrbc_vtype tt : CLASS_IDENTIFIER_SIZE;
//   union {
//     mrbc_int_t i;		// MRBC_TT_INTEGER, SYMBOL
// #if MRBC_USE_FLOAT
//     mrbc_float_t d;		// MRBC_TT_FLOAT
// #endif
//     // struct RClass *cls;		// MRBC_TT_CLASS
//     struct RInstance instance;	// MRBC_TT_OBJECT
//     // struct RProc *proc;		// MRBC_TT_PROC
//     struct RArray array;	// MRBC_TT_ARRAY
//     struct RString string;	// MRBC_TT_STRING
//     // TODO.
//     // struct RRange *range;	// MRBC_TT_RANGE
//     // struct RHash *hash;		// MRBC_TT_HASH
//     // struct RException *exception; // MRBC_TT_EXCEPTION
//     struct RKeyValue kv;
//   };
};

// struct RObject * TrueObject;
// struct RObject * FalseObject;

#define NILOBJECT_COPRO (0x0)
#define FALSEOBJECT_COPRO (0x2)
#define TRUEOBJECT_COPRO (0x6)
#define STRINGEMPTYOBJECT_COPRO (0x4)

#define IS_PTR(v) ((v & 0x1) == 0)

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
#define mrbc_type(o)		(o).tt
#define mrbc_integer(o)		(o & 1 == 1 ? (signed)o >> 1 : (o).i)
// #define mrbc_float(o)		((o).d)
#define mrbc_symbol(o)		((o).i)

// for Numeric values.
/**
  @def MRBC_ISNUMERIC(val)
  Check the val is numeric.

  @def MRBC_TO_INT(val)
  Convert mrbc_value to C-lang int.
*/
#define MRBC_ISNUMERIC(val) ((val) & 1 || ((val).tt == MRBC_TT_INTEGER))
#define MRBC_TO_INT(val) \
  (val) & 1 ? (signed)(val) >> 1 : \
  ((val).tt == MRBC_TT_INTEGER ? (val).i : 0)

#ifdef __cplusplus
}
#endif