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
// Str
// ========================================================================== //

/* Produce a string from string literal. Result must not be released */
#define LN_CSTR(str)                                                           \
  (Str) { .buf = (u8*)str, .size = cstr_size(str), .len = cstr_len(str) }

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

Str
make_str(const char* str);

// -------------------------------------------------------------------------- //

Str
str_copy(const Str* str);

// -------------------------------------------------------------------------- //

Str
str_null();

// -------------------------------------------------------------------------- //

Str
str_format(const Str* fmt, ...);

// -------------------------------------------------------------------------- //

Str
str_format_v(const Str* fmt, va_list args);

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

// ========================================================================== //
// StrSlice
// ========================================================================== //

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
make_str_slice_null();

// -------------------------------------------------------------------------- //

Str
str_slice_to_string(const StrSlice* slice);

// -------------------------------------------------------------------------- //

bool
str_slice_eq(const StrSlice* slice0, const StrSlice* slice1);

// -------------------------------------------------------------------------- //

bool
str_slice_eq_str(const StrSlice* slice, const Str* str);

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

#endif // LN_STR_H