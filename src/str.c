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
#include <stdio.h>

#include "str.h"
#include "alf_unicode.h"

// ========================================================================== //
// CStr
// ========================================================================== //

u32
cstr_size(const char* str)
{
  return strlen(str);
}

// -------------------------------------------------------------------------- //

u32
cstr_len(const char* str)
{
  return (u32)alfUTF8StringLength(str);
}

// -------------------------------------------------------------------------- //

bool
cstr_eq(const char* str0, const char* str1)
{
  return strcmp(str0, str1) == 0;
}

// ========================================================================== //
// Str
// ========================================================================== //

#define kStrFmtBufSize 2048

// -------------------------------------------------------------------------- //

static char s_str_fmt_buf[kStrFmtBufSize];

// -------------------------------------------------------------------------- //

Str
make_str_copy(const char* str)
{
  u32 size = cstr_size(str);
  u8* buf = alloc(size + 1, kLnMinAlign);
  if (!buf) {
    return (Str){ .buf = NULL, .size = 0, .len = 0 };
  }
  buf[size] = 0;
  memcpy(buf, str, size);
  return (Str){ .buf = buf, .size = size, .len = cstr_len(str) };
}

// -------------------------------------------------------------------------- //

Str
str_copy(const Str* str)
{
  u8* buf = alloc(str->size + 1, kLnMinAlign);
  if (!buf) {
    return (Str){ .buf = NULL, .size = 0, .len = 0 };
  }
  buf[str->size] = 0;
  memcpy(buf, str, str->size);
  return (Str){ .buf = buf, .size = str->size, .len = str->len };
}

// -------------------------------------------------------------------------- //

Str
str_null()
{
  return (Str){ .buf = NULL, .size = 0, .len = 0 };
}

// -------------------------------------------------------------------------- //

Str
str_format(Str fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  Str str = str_format_v(fmt, args);
  va_end(args);
  return str;
}

// -------------------------------------------------------------------------- //

Str
str_format_v(Str fmt, va_list args)
{
  int written = vsnprintf(s_str_fmt_buf, kStrFmtBufSize, str_cstr(&fmt), args);
  assrt(written != -1, make_str("Str format buffer too small"));
  return make_str_copy(s_str_fmt_buf);
}

// -------------------------------------------------------------------------- //

void
release_str(Str* str)
{
  release(str->buf);
}

// -------------------------------------------------------------------------- //

u8*
str_buf(Str* str)
{
  return str->buf;
}

// -------------------------------------------------------------------------- //

const char*
str_cstr(const Str* str)
{
  return (const char*)str->buf;
}

// -------------------------------------------------------------------------- //

bool
str_eq(const Str* s0, const Str* s1)
{
  if (!s0->buf || !s1->buf) {
    return false;
  }
  if (s0->size != s1->size) {
    return false;
  }
  return memcmp(s0->buf, s1->buf, s1->size) == 0;
}

// -------------------------------------------------------------------------- //

char*
str_write_cp(char buf[5], u32 cp)
{
  u32 numBytes;
  if (!alfUTF8Encode(buf, 0, cp, &numBytes)) {
    buf[0] = 0;
    return buf;
  }
  buf[numBytes] = 0;
  return buf;
}

// ========================================================================== //
// StrSlice
// ========================================================================== //

StrSlice
make_str_slice(const Str* str, u32 off, u32 count)
{
  if (!str) {
    return make_str_slice_null();
  }
  return (StrSlice){ .ptr = str->buf + off, .count = count };
}

// -------------------------------------------------------------------------- //

StrSlice
make_str_slice_null()
{
  return (StrSlice){ .ptr = NULL, .count = 0 };
}

// -------------------------------------------------------------------------- //

Str
str_slice_to_string(const StrSlice* slice)
{
  u32 size = slice->count;
  u8* buf = alloc(size + 1, kLnMinAlign);
  if (!buf) {
    return (Str){ .buf = NULL, .size = 0, .len = 0 };
  }
  buf[size] = 0;
  memcpy(buf, slice->ptr, size);
  return (Str){ .buf = buf, .size = size, .len = cstr_len((char*)buf) };
}

// -------------------------------------------------------------------------- //

bool
str_slice_eq(const StrSlice* slice0, const StrSlice* slice1)
{
  if (!slice0 || !slice1) {
    return slice0 == slice1;
  }
  if (slice0->count != slice1->count) {
    return false;
  }
  return memcmp(slice0->ptr, slice1->ptr, slice0->count) == 0;
}

// -------------------------------------------------------------------------- //

bool
str_slice_eq_str(const StrSlice* slice, const Str* str)
{
  if (!slice || !str) {
    return (!slice && !str);
  }
  if (slice->count != str->size) {
    return false;
  }
  return memcmp(slice->ptr, str->buf, slice->count) == 0;
}

// ========================================================================== //
// StrIter
// ========================================================================== //

StrIter
make_str_iter(const Str* str)
{
  return (StrIter){ .str = str, .off = 0, .idx = 0 };
}

// -------------------------------------------------------------------------- //

u32
str_iter_next(StrIter* iter)
{
  if (iter->off >= iter->str->size) {
    return kInvalidCodepoint;
  }
  u32 cp;
  u32 width;
  AlfBool success =
    alfUTF8Decode((AlfChar8*)iter->str->buf, iter->off, &cp, &width);
  if (!success) {
    return kInvalidCodepoint;
  }
  iter->off += width;
  iter->idx++;
  return cp;
}

// -------------------------------------------------------------------------- //

u32
str_iter_peek(const StrIter* iter)
{
  if (iter->off >= iter->str->size) {
    return kInvalidCodepoint;
  }
  u32 cp;
  u32 width;
  AlfBool success =
    alfUTF8Decode((AlfChar8*)iter->str->buf, iter->off, &cp, &width);
  if (!success) {
    return kInvalidCodepoint;
  }
  return cp;
}