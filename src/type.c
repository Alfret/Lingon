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

#include <stdlib.h>
#include <string.h>

#include "type.h"
#include "str.h"

// ========================================================================== //
// TypeList
// ========================================================================== //

typedef struct TypeList
{
  Type* buf;
  u32 len;
  u32 cap;
} TypeList;

// -------------------------------------------------------------------------- //

static TypeList
make_type_list()
{
  u32 cap = 16;
  Type* buf = alloc(sizeof(Type) * cap, kLnMinAlign);
  return (TypeList){ .buf = buf, .len = 0, .cap = cap };
}

// -------------------------------------------------------------------------- //

static void
release_type_list(TypeList* list)
{
  release(list->buf);
}

// -------------------------------------------------------------------------- //

static void
type_list_reserve(TypeList* list, u32 cap)
{
  if (list->cap > cap) {
    return;
  }
  Type* buf = alloc(sizeof(Type) * cap, kLnMinAlign);
  memcpy(buf, list->buf, sizeof(Type) * list->len);
  release(list->buf);
  list->buf = buf;
  list->cap = cap;
}

// -------------------------------------------------------------------------- //

static Type*
type_list_get(const TypeList* list, u32 index)
{
  assrt(index < list->len, make_str("TypeList index out of bounds"));
  return &list->buf[index];
}
// -------------------------------------------------------------------------- //

static Type*
type_list_append(TypeList* list, const Type* type)
{
  if (list->len >= list->cap) {
    type_list_reserve(list, list->cap * 2);
  }
  memcpy(list->buf + list->len++, type, sizeof(Type));
  return type_list_get(list, list->len - 1);
}

// ========================================================================== //
// Types
// ========================================================================== //

static TypeList s_type_list;

// -------------------------------------------------------------------------- //

static Type* s_type_char;
static Type* s_type_bool;

static Type* s_type_u8;
static Type* s_type_s8;
static Type* s_type_u16;
static Type* s_type_s16;
static Type* s_type_u32;
static Type* s_type_s32;
static Type* s_type_u64;
static Type* s_type_s64;

static Type* s_type_f32;
static Type* s_type_f64;

// -------------------------------------------------------------------------- //

#define LN_TYPE_LIST_ADD(store, type_kind)                                     \
  type = (Type){ .kind = type_kind };                                          \
  store = type_list_append(&s_type_list, &type);

void
types_init()
{
  assrt(s_type_list.buf == NULL,
        make_str("Types can only be initialized once"));
  s_type_list = make_type_list();

  Type type;
  LN_TYPE_LIST_ADD(s_type_char, kTypeChar);
  LN_TYPE_LIST_ADD(s_type_bool, kTypeBool);
  LN_TYPE_LIST_ADD(s_type_u8, kTypeU8);
  LN_TYPE_LIST_ADD(s_type_s8, kTypeS16);
  LN_TYPE_LIST_ADD(s_type_u16, kTypeU16);
  LN_TYPE_LIST_ADD(s_type_s16, kTypeS16);
  LN_TYPE_LIST_ADD(s_type_u32, kTypeU32);
  LN_TYPE_LIST_ADD(s_type_s32, kTypeS32);
  LN_TYPE_LIST_ADD(s_type_u64, kTypeU64);
  LN_TYPE_LIST_ADD(s_type_s64, kTypeS64);
  LN_TYPE_LIST_ADD(s_type_f32, kTypeF32);
  LN_TYPE_LIST_ADD(s_type_f64, kTypeF64);
}

#undef LN_TYPE_LIST_ADD

// -------------------------------------------------------------------------- //

void
types_cleanup()
{
  assrt(s_type_list.buf != NULL,
        make_str("Cannot cleanup types without first initializing them"));
  release_type_list(&s_type_list);
}

// -------------------------------------------------------------------------- //

Type*
get_array_type(Type* elem_type, u32 len)
{
  for (u32 i = 0; i < s_type_list.len; i++) {
    Type* other = type_list_get(&s_type_list, i);
    if (other->kind == kTypeArray && other->array.type == elem_type &&
        other->array.len == len) {
      return other;
    }
  }

  Type type =
    (Type){ .kind = kTypeArray, .array = { .type = elem_type, .len = len } };
  return type_list_append(&s_type_list, &type);
}

// -------------------------------------------------------------------------- //

Type*
get_pointer_type(Type* pointee_type)
{
  for (u32 i = 0; i < s_type_list.len; i++) {
    Type* other = type_list_get(&s_type_list, i);
    if (other->kind == kTypePointer && other->pointer.type == pointee_type) {
      return other;
    }
  }

  Type type =
    (Type){ .kind = kTypePointer, .pointer = { .type = pointee_type } };
  return type_list_append(&s_type_list, &type);
}

// -------------------------------------------------------------------------- //

Type*
get_type_char()
{
  return s_type_char;
}

// -------------------------------------------------------------------------- //

Type*
get_type_bool()
{
  return s_type_bool;
}

// -------------------------------------------------------------------------- //

Type*
get_type_u8()
{
  return s_type_u8;
}

// -------------------------------------------------------------------------- //

Type*
get_type_s8()
{
  return s_type_s8;
}

// -------------------------------------------------------------------------- //

Type*
get_type_u16()
{
  return s_type_u16;
}

// -------------------------------------------------------------------------- //

Type*
get_type_s16()
{
  return s_type_s16;
}

// -------------------------------------------------------------------------- //

Type*
get_type_u32()
{
  return s_type_u32;
}

// -------------------------------------------------------------------------- //

Type*
get_type_s32()
{
  return s_type_s32;
}

// -------------------------------------------------------------------------- //

Type*
get_type_u64()
{
  return s_type_u64;
}

// -------------------------------------------------------------------------- //

Type*
get_type_s64()
{
  return s_type_s64;
}

// -------------------------------------------------------------------------- //

Type*
get_type_f32()
{
  return s_type_f32;
}

// -------------------------------------------------------------------------- //

Type*
get_type_f64()
{
  return s_type_f64;
}