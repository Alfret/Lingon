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
// Unicode
// ========================================================================== //

u32
unicode_width(u32 code_point)
{
  return alfUTF8CodepointWidth(code_point);
}

// -------------------------------------------------------------------------- //

bool
unicode_encode(char* buf, u32 off, u32 code_point, u32* p_width)
{
  return alfUTF8Encode((AlfChar8*)buf, off, code_point, p_width);
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
  memcpy(buf, str->buf, str->size);
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

// -------------------------------------------------------------------------- //

bool
str_line_col_to_off(const Str* str, u32 line, u32 col, u32* p_off)
{
  *p_off = 0;

  u32 _line = 0, _col = 0;
  for (u32 i = 0; i < str->size; i++) {
    if (_line == line && _col == col) {
      *p_off = i;
      return true;
    }

    _col++;
    if (str->buf[i] == '\n') {
      _line++;
      _col = 0;
    }
  }
  return false;
}

// -------------------------------------------------------------------------- //

bool
str_off_to_line_col(const Str* str, u32 off, u32* p_line, u32* p_col)
{
  *p_line = 0;
  *p_col = 0;

  u32 line = 0, col = 0;
  for (u32 i = 0; i < str->size; i++) {
    if (off == i) {
      *p_line = line;
      *p_col = col;
      return true;
    }

    col++;
    if (str->buf[i] == '\n') {
      line++;
      col = 0;
    }
  }
  return false;
}

// ========================================================================== //
// StrSlice
// ========================================================================== //

StrSlice
make_str_slice(const Str* str, u32 off, u32 count)
{
  if (!str) {
    return str_slice_null();
  }
  return (StrSlice){ .ptr = str->buf + off, .count = count };
}

// -------------------------------------------------------------------------- //

StrSlice
str_slice_null()
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

// -------------------------------------------------------------------------- //

bool
str_slice_is_null(const StrSlice* slice)
{
  return slice->ptr == NULL;
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

// ========================================================================== //
// StrList
// ========================================================================== //

/* Make empty str list */
StrList
make_str_list(u32 cap)
{
  u32 _cap = cap ? cap : 10;
  Str* buf = alloc(sizeof(Str) * _cap, kLnMinAlign);
  if (!buf) {
    return (StrList){ .buf = 0, .len = 0, .cap = 0 };
  }
  return (StrList){ .buf = buf, .len = 0, .cap = _cap };
}

// -------------------------------------------------------------------------- //

/* Release str list */
void
release_str_list(StrList* list)
{
  release(list->buf);
}

// -------------------------------------------------------------------------- //

/* Append to str list */
void
str_list_append(StrList* list, const Str* str)
{
  if (list->len >= list->cap) {
    str_list_reserve(list, list->cap * 2);
  }
  memcpy(list->buf + list->len++, str, sizeof(Str));
}

// -------------------------------------------------------------------------- //

/* Remove from str list */
Str
str_list_remove(StrList* list, u32 index)
{
  assrt(index < list->len, make_str("Index out of bounds (%u)"), index);
  Str str = list->buf[index];
  memmove(list->buf + index,
          list->buf + index + 1,
          sizeof(Str) * (list->len - index - 1));
  return str;
}

// -------------------------------------------------------------------------- //

/* Get object from str list */
const Str*
str_list_get(const StrList* list, u32 index)
{
  assrt(index < list->len, make_str("Index out of bounds (%u)"), index);
  return &list->buf[index];
}

// -------------------------------------------------------------------------- //

/* Reserve capacity in str list */
void
str_list_reserve(StrList* list, u32 cap)
{
  if (list->cap > cap) {
    return;
  }
  Str* buf = alloc(sizeof(Str) * cap, kLnMinAlign);
  memcpy(buf, list->buf, sizeof(Str) * list->len);
  release(list->buf);
  list->buf = buf;
  list->cap = cap;
}