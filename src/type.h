// MIT License
//
// Copyright (c) 2019 Filip Björklund
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef LN_TYPE_H
#define LN_TYPE_H

#include "common.h"
#include "str.h"

// ========================================================================== //
// TypeKind
// ========================================================================== //

typedef enum TypeKind
{
  kTypeVoid,

  kTypeChar,
  kTypeBool,

  kTypeU8,
  kTypeS8,
  kTypeU16,
  kTypeS16,
  kTypeU32,
  kTypeS32,
  kTypeU64,
  kTypeS64,

  kTypeF32,
  kTypeF64,

  kTypeArray,
  kTypePtr,
  kTypeStruct,
  kTypeEnum,
  kTypeTrait,
} TypeKind;

// ========================================================================== //
// Type
// ========================================================================== //

#define kTypeArrayUnknownLen 0xFFFFFFFFFFFFFFFF

// -------------------------------------------------------------------------- //

/* Type data union */
typedef struct Type
{
  /* Kind */
  TypeKind kind;
  /* Data */
  union
  {
    struct
    {
      /* Array type */
      struct Type* type;
      /* Length */
      u64 len;
    } array;
    struct
    {
      /* Pointée type */
      struct Type* type;
    } pointer;
    struct
    {
      u32 tmp;
    } strct;
    struct
    {
      u32 tmp;
    } enuum;
    struct
    {
      u32 tmp;
    } trait;
  };
} Type;

// -------------------------------------------------------------------------- //

void
types_init();

// -------------------------------------------------------------------------- //

void
types_cleanup();

// -------------------------------------------------------------------------- //

/* Gets type name. Str must be released */
Str
type_to_str(Type* type);

// -------------------------------------------------------------------------- //

/* Checks if type is a primitive type */
bool
type_is_primitive(Type* type);

// -------------------------------------------------------------------------- //

/* Returns a primitive type from the name */
Type*
get_type_from_name(const StrSlice* name);

// -------------------------------------------------------------------------- //

/* Gets an array type */
Type*
get_type_array(Type* elem_type, u64 len);

// -------------------------------------------------------------------------- //

/* Gets a pointer type */
Type*
get_type_ptr(Type* pointee_type);

// -------------------------------------------------------------------------- //

/* Gets 'void' type */
Type*
get_type_void();

// -------------------------------------------------------------------------- //

/* Gets 'char' type */
Type*
get_type_char();

// -------------------------------------------------------------------------- //

/* Gets 'bool' type */
Type*
get_type_bool();

// -------------------------------------------------------------------------- //

/* Gets 'u8' type */
Type*
get_type_u8();

// -------------------------------------------------------------------------- //

/* Gets 's8' type */
Type*
get_type_s8();

// -------------------------------------------------------------------------- //

/* Gets 'u16' type */
Type*
get_type_u16();

// -------------------------------------------------------------------------- //

/* Gets 's16' type */
Type*
get_type_s16();

// -------------------------------------------------------------------------- //

/* Gets 'u32' type */
Type*
get_type_u32();

// -------------------------------------------------------------------------- //

/* Gets 's32' type */
Type*
get_type_s32();

// -------------------------------------------------------------------------- //

/* Gets 'u64' type */
Type*
get_type_u64();

// -------------------------------------------------------------------------- //

/* Gets 's64' type */
Type*
get_type_s64();

// -------------------------------------------------------------------------- //

/* Gets 'f32' type */
Type*
get_type_f32();

// -------------------------------------------------------------------------- //

/* Gets 'f64' type */
Type*
get_type_f64();

#endif // LN_TYPE_H