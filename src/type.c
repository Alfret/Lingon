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

static Type* s_type_void;

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
  LN_TYPE_LIST_ADD(s_type_void, kTypeVoid);
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

#define LN_TYPE_TO_STR_BASIC(knd, name)                                        \
  do {                                                                         \
    if (type->kind == knd) {                                                   \
      return make_str_copy(name);                                              \
    }                                                                          \
  } while (0)

Str
type_to_str(Type* type)
{
  LN_TYPE_TO_STR_BASIC(kTypeVoid, "void");
  LN_TYPE_TO_STR_BASIC(kTypeChar, "char");
  LN_TYPE_TO_STR_BASIC(kTypeBool, "bool");
  LN_TYPE_TO_STR_BASIC(kTypeU8, "u8");
  LN_TYPE_TO_STR_BASIC(kTypeS8, "s8");
  LN_TYPE_TO_STR_BASIC(kTypeU16, "u16");
  LN_TYPE_TO_STR_BASIC(kTypeS16, "s16");
  LN_TYPE_TO_STR_BASIC(kTypeU32, "u32");
  LN_TYPE_TO_STR_BASIC(kTypeS32, "s32");
  LN_TYPE_TO_STR_BASIC(kTypeU64, "u64");
  LN_TYPE_TO_STR_BASIC(kTypeS64, "s64");
  LN_TYPE_TO_STR_BASIC(kTypeF32, "f32");
  LN_TYPE_TO_STR_BASIC(kTypeF64, "f64");

  if (type->kind == kTypeArray) {
    Str elem_str = type_to_str(type->array.type);
    Str array_str;
    if (type->array.len == kTypeArrayUnknownLen) {
      array_str = str_format(make_str("[%s]"), str_cstr(&elem_str));
    } else {
      array_str =
        str_format(make_str("[%s; %u]"), str_cstr(&elem_str), type->array.len);
    }
    release_str(&elem_str);
    return array_str;
  } else if (type->kind == kTypePtr) {
    Str ptee_str = type_to_str(type->pointer.type);
    Str ptr_str = str_format(make_str("%s*"), str_cstr(&ptee_str));
    release_str(&ptee_str);
    return ptr_str;
  } else if (type->kind == kTypeStruct) {
    LN_NOT_IMPL();
  } else if (type->kind == kTypeEnum) {
    LN_NOT_IMPL();
  } else if (type->kind == kTypeTrait) {
    LN_NOT_IMPL();
  }

  return make_str_copy("Unknown");
}

#undef LN_TYPE_TO_STR_BASIC

// -------------------------------------------------------------------------- //

void
types_cleanup()
{
  assrt(s_type_list.buf != NULL,
        make_str("Cannot cleanup types without first initializing them"));
  for (u32 i = 0; i < s_type_list.len; i++) {
    Type* type = type_list_get(&s_type_list, i);
    if (type->kind == kTypeStruct) {
      // TODO(Filip Björklund): Free field type list
    }
  }
  release_type_list(&s_type_list);
}

// -------------------------------------------------------------------------- //

bool
type_is_primitive(Type* type)
{
  return type->kind == kTypeVoid || type->kind == kTypeChar ||
         type->kind == kTypeBool || type->kind == kTypeU8 ||
         type->kind == kTypeS8 || type->kind == kTypeU16 ||
         type->kind == kTypeS16 || type->kind == kTypeU32 ||
         type->kind == kTypeS32 || type->kind == kTypeU64 ||
         type->kind == kTypeS64 || type->kind == kTypeF32 ||
         type->kind == kTypeF64;
}

// -------------------------------------------------------------------------- //

#define LN_TYPE_FROM_NAME_CASE(val, typ)                                       \
  if (str_slice_eq_str(name, &make_str(val))) {                                \
    return typ;                                                                \
  }

Type*
get_type_from_name(const StrSlice* name)
{
  LN_TYPE_FROM_NAME_CASE("void", get_type_void());
  LN_TYPE_FROM_NAME_CASE("char", get_type_char());
  LN_TYPE_FROM_NAME_CASE("bool", get_type_bool());
  LN_TYPE_FROM_NAME_CASE("u8", get_type_u8());
  LN_TYPE_FROM_NAME_CASE("s8", get_type_s8());
  LN_TYPE_FROM_NAME_CASE("u16", get_type_u16());
  LN_TYPE_FROM_NAME_CASE("s16", get_type_s16());
  LN_TYPE_FROM_NAME_CASE("u32", get_type_u32());
  LN_TYPE_FROM_NAME_CASE("s32", get_type_s32());
  LN_TYPE_FROM_NAME_CASE("u64", get_type_u64());
  LN_TYPE_FROM_NAME_CASE("s64", get_type_s64());
  LN_TYPE_FROM_NAME_CASE("f32", get_type_f32());
  LN_TYPE_FROM_NAME_CASE("f64", get_type_f64());
  return NULL;
}

#undef LN_TYPE_FROM_NAME_CASE

// -------------------------------------------------------------------------- //

Type*
get_type_array(Type* elem_type, u64 len)
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
get_type_ptr(Type* pointee_type)
{
  for (u32 i = 0; i < s_type_list.len; i++) {
    Type* other = type_list_get(&s_type_list, i);
    if (other->kind == kTypePtr && other->pointer.type == pointee_type) {
      return other;
    }
  }

  Type type = (Type){ .kind = kTypePtr, .pointer = { .type = pointee_type } };
  return type_list_append(&s_type_list, &type);
}

// -------------------------------------------------------------------------- //

Type*
get_type_void()
{
  return s_type_void;
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