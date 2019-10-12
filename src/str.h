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

#ifndef LN_STR_H
#define LN_STR_H

#include "common.h"

// ========================================================================== //
// CStr
// ========================================================================== //

/* c-str byte size */
u32
cstr_size(const char* str);

// -------------------------------------------------------------------------- //

/* Returns c-str length */
u32
cstr_len(const char* str);

// -------------------------------------------------------------------------- //

/* c-str equality */
bool
cstr_eq(const char* str0, const char* str1);

// ========================================================================== //
// Unicode
// ========================================================================== //

/* Gets unicode code-point width */
u32
unicode_width(u32 code_point);

// -------------------------------------------------------------------------- //

/* Encode code-point. Returns encoded width in 'p_width' */
bool
unicode_encode(char* buf, u32 off, u32 code_point, u32* p_width);

// ========================================================================== //
// Str
// ========================================================================== //

/* String */
typedef struct Str
{
  /* Str buffer */
  u8* buf;
  /* Size in bytes */
  u32 size;
  /* Len in characters */
  u32 len;
} Str;

// -------------------------------------------------------------------------- //

#define make_str(str)                                                          \
  (Str) { .buf = (u8*)str, .size = cstr_size(str), .len = cstr_len(str) }

// -------------------------------------------------------------------------- //

Str
make_str_copy(const char* str);

// -------------------------------------------------------------------------- //

Str
str_copy(const Str* str);

// -------------------------------------------------------------------------- //

Str
str_null();

// -------------------------------------------------------------------------- //

Str
str_format(Str fmt, ...);

// -------------------------------------------------------------------------- //

Str
str_format_v(Str fmt, va_list args);

// -------------------------------------------------------------------------- //

void
release_str(Str* str);

// -------------------------------------------------------------------------- //

u8*
str_buf(Str* str);

// -------------------------------------------------------------------------- //

const char*
str_cstr(const Str* str);

// -------------------------------------------------------------------------- //

bool
str_eq(const Str* s0, const Str* s1);

// -------------------------------------------------------------------------- //

char*
str_write_cp(char buf[5], u32 cp);

// -------------------------------------------------------------------------- //

/* Calculate offset from line and column */
bool
str_line_col_to_off(const Str* str, u32 line, u32 col, u32* p_off);

// -------------------------------------------------------------------------- //

/* Calculate line and column from offset */
bool
str_off_to_line_col(const Str* str, u32 off, u32* p_line, u32* p_col);

// ========================================================================== //
// StrSlice
// ========================================================================== //

#define str_sl "%.*s"

#define str_slice_print(slice) (slice)->count, (char*)(slice)->ptr

// -------------------------------------------------------------------------- //

/* String slice */
typedef struct StrSlice
{
  /* Offset Str buf */
  u8* ptr;
  /* Slice byte count */
  u32 count;
} StrSlice;

// -------------------------------------------------------------------------- //

StrSlice
make_str_slice(const Str* str, u32 off, u32 count);

// -------------------------------------------------------------------------- //

StrSlice
str_slice_null();

// -------------------------------------------------------------------------- //

Str
str_slice_to_string(const StrSlice* slice);

// -------------------------------------------------------------------------- //

bool
str_slice_eq(const StrSlice* slice0, const StrSlice* slice1);

// -------------------------------------------------------------------------- //

bool
str_slice_eq_str(const StrSlice* slice, const Str* str);

// -------------------------------------------------------------------------- //

bool
str_slice_is_null(const StrSlice* slice);

// ========================================================================== //
// StrIter
// ========================================================================== //

#define kInvalidCodepoint 0xFFFFFFFF

// -------------------------------------------------------------------------- //

/* String iterator */
typedef struct StrIter
{
  /* Iterated string */
  const Str* str;
  /* Offset in bytes */
  u32 off;
  /* Offset in indices */
  u32 idx;
} StrIter;

// -------------------------------------------------------------------------- //

StrIter
make_str_iter(const Str* str);

// -------------------------------------------------------------------------- //

u32
str_iter_next(StrIter* iter);

// -------------------------------------------------------------------------- //

u32
str_iter_peek(const StrIter* iter);

// ========================================================================== //
// StrList
// ========================================================================== //

/* Str list */
typedef struct StrList
{
  Str* buf;
  u32 len;
  u32 cap;
} StrList;

// -------------------------------------------------------------------------- //

/* Make empty str list */
StrList
make_str_list(u32 cap);

// -------------------------------------------------------------------------- //

/* Release str list */
void
release_str_list(StrList* list);

// -------------------------------------------------------------------------- //

/* Append to str list */
void
str_list_append(StrList* list, const Str* str);

// -------------------------------------------------------------------------- //

/* Remove from str list */
Str
str_list_remove(StrList* list, u32 index);

// -------------------------------------------------------------------------- //

/* Get object from str list */
const Str*
str_list_get(const StrList* list, u32 index);

// -------------------------------------------------------------------------- //

/* Reserve capacity in str list */
void
str_list_reserve(StrList* list, u32 cap);

#endif // LN_STR_H