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

// ========================================================================== //
// TypeKind
// ========================================================================== //

typedef enum TypeKind
{
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
  kTypePointer,
  kTypeStruct,
  kTypeEnum,
  kTypeTrait,
} TypeKind;

// ========================================================================== //
// Type
// ========================================================================== //

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
      u32 len;
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

Type*
get_array_type(Type* elem_type, u32 len);

// -------------------------------------------------------------------------- //

Type*
get_pointer_type(Type* pointee_type);

// -------------------------------------------------------------------------- //

Type*
get_type_char();

// -------------------------------------------------------------------------- //

Type*
get_type_bool();

// -------------------------------------------------------------------------- //

Type*
get_type_u8();

// -------------------------------------------------------------------------- //

Type*
get_type_s8();

// -------------------------------------------------------------------------- //

Type*
get_type_u16();

// -------------------------------------------------------------------------- //

Type*
get_type_s16();

// -------------------------------------------------------------------------- //

Type*
get_type_u32();

// -------------------------------------------------------------------------- //

Type*
get_type_s32();

// -------------------------------------------------------------------------- //

Type*
get_type_u64();

// -------------------------------------------------------------------------- //

Type*
get_type_s64();

// -------------------------------------------------------------------------- //

Type*
get_type_f32();

// -------------------------------------------------------------------------- //

Type*
get_type_f64();

#endif // LN_TYPE_H