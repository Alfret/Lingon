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
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "err.h"
#include "con.h"

// ========================================================================== //
// Util
// ========================================================================== //

static u32
err_util_int_width(u32 val)
{
  return (u32)floor(log10(val)) + 1;
}

// ========================================================================== //
// Err
// ========================================================================== //

// ========================================================================== //
// ErrList
// ========================================================================== //

/* Make err list */
ErrList
make_err_list()
{
  u32 cap = 10;
  Err* buf = alloc(sizeof(Err) * cap, kLnMinAlign);
  if (!buf) {
    return (ErrList){ .buf = 0, .len = 0, .cap = 0 };
  }
  return (ErrList){ .buf = buf, .len = 0, .cap = cap };
}

// -------------------------------------------------------------------------- //

/* Release err list */
void
release_err_list(ErrList* list)
{
  release(list->buf);
}

// -------------------------------------------------------------------------- //

/* Append to err list */
void
err_list_append(ErrList* list, const Err* err)
{
  if (list->len >= list->cap) {
    err_list_reserve(list, list->cap * 2);
  }
  memcpy(list->buf + list->len++, err, sizeof(Err));
}

// -------------------------------------------------------------------------- //

/* Remove from err list */
Err
err_list_remove(ErrList* list, u32 index)
{
  assrt(index < list->len, make_str("Index out of bounds (%u)"), index);
  Err err = list->buf[index];
  memmove(list->buf + index,
          list->buf + index + 1,
          sizeof(Err) * (list->len - index - 1));
  return err;
}

// -------------------------------------------------------------------------- //

/* Get object from err list */
const Err*
err_list_get(ErrList* list, u32 index)
{
  assrt(index < list->len, make_str("Index out of bounds (%u)"), index);
  return &list->buf[index];
}

// -------------------------------------------------------------------------- //

/* Reserve capacity in err list */
void
err_list_reserve(ErrList* list, u32 cap)
{
  if (list->cap > cap) {
    return;
  }
  Err* buf = alloc(sizeof(Err) * cap, kLnMinAlign);
  memcpy(buf, list->buf, sizeof(Err) * list->len);
  release(list->buf);
  list->buf = buf;
  list->cap = cap;
}

// ========================================================================== //
// ErrNum
// ========================================================================== //

#define LN_ERR_NUM_STR_CASE(e)                                                 \
  case e: {                                                                    \
    return make_str(#e);                                                       \
  }

Str
err_num_to_str(ErrNum num)
{
  switch (num) {
    LN_ERR_NUM_STR_CASE(kErrNumNone)
    LN_ERR_NUM_STR_CASE(kErrNumUnexpTok)
    default: {
      panic(make_str("Invalid ErrNum (%u)"), num);
    }
  }
}

// ========================================================================== //
// ErrBuilder
// ========================================================================== //

ErrBuilder
make_err_builder(const Src* src)
{
  assrt(src != NULL, make_str("ErrBuilder must be created with valid source"));
  return (ErrBuilder){ .err_num = kErrNumNone,
                       .src = src,
                       .err_desc = NULL,
                       .err_msg = NULL,
                       .err_sugg = NULL,
                       .span = NULL,
                       .line_before = 0,
                       .line_after = 0,
                       .pad_line_before = 0,
                       .pad_line_after = 0 };
}

// -------------------------------------------------------------------------- //

void
err_builder_set_err_num(ErrBuilder* builder, ErrNum num)
{
  builder->err_num = num;
}

// -------------------------------------------------------------------------- //

void
err_builder_set_desc(ErrBuilder* builder, const Str* desc)
{
  builder->err_desc = desc;
}
// -------------------------------------------------------------------------- //

void
err_builder_set_msg(ErrBuilder* builder, const Str* msg)
{
  builder->err_msg = msg;
}

// -------------------------------------------------------------------------- //

void
err_builder_set_sugg(ErrBuilder* builder, const Str* sugg)
{
  builder->err_sugg = sugg;
}

// -------------------------------------------------------------------------- //

void
err_builder_set_span(ErrBuilder* builder, const Span* span)
{
  builder->span = span;
}

// -------------------------------------------------------------------------- //

void
err_builder_set_lines_before(ErrBuilder* builder, u32 lines)
{
  builder->line_before = lines;
}
// -------------------------------------------------------------------------- //

void
err_builder_set_lines_after(ErrBuilder* builder, u32 lines)
{
  builder->line_after = lines;
}

// -------------------------------------------------------------------------- //

void
err_builder_set_pad_lines_before(ErrBuilder* builder, u32 lines)
{
  builder->pad_line_before = lines;
}
// -------------------------------------------------------------------------- //

void
err_builder_set_pad_lines_after(ErrBuilder* builder, u32 lines)
{
  builder->pad_line_after = lines;
}

// -------------------------------------------------------------------------- //

void
err_builder_emit(const ErrBuilder* builder)
{
  assrt(builder->span != NULL, make_str("ErrBuilder span must be set"));

  // Retrieve target line
  StrSlice trgt_line = span_line(builder->span, &builder->src->src);
  assrt(!str_slice_is_null(&trgt_line),
        make_str("Failed to get target line slice"));

  // Print error
  if (builder->err_desc) {
    if (builder->err_num != kErrNumNone) {
      printf(con_col_err "error" con_col_reset "[%04u]: %s\n",
             builder->err_num,
             str_cstr(builder->err_desc));
    } else {
      printf(con_col_err "error" con_col_reset ": %s\n",
             str_cstr(builder->err_desc));
    }
  } else {
    if (builder->err_num != kErrNumNone) {
      printf(con_col_err "error" con_col_reset "[%04u]:\n", builder->err_num);
    } else {
      printf(con_col_err "error" con_col_reset ":\n");
    }
  }

  // Padding
  Str pad0 = err_pad_str(' ', builder->span->beg.col);
  Str pad1 = err_pad_str('-', builder->span->end.col - builder->span->beg.col);

  // Number of lines to include
  u32 lines_before = LN_CLAMP(builder->line_before, 0, builder->span->beg.line);
  u32 lines_after = builder->line_after;

  // Calculates max line-number width
  u32 line_num_width =
    err_util_int_width(builder->span->beg.line + 1 + builder->line_after);

  // Lines before
  {
    u32 pad_line_before = builder->pad_line_before;
    if (lines_before == 0) {
      pad_line_before = LN_CLAMP(pad_line_before, 1, 99);
    }
    for (u32 i = 0; i < pad_line_before; i++) {
      printf("%*s |\n", line_num_width, "");
    }

    for (u32 i = lines_before; i > 0; i--) {
      StrSlice line_ =
        span_line_before(*builder->span, &builder->src->src, i - 1);
      if (!str_slice_is_null(&line_)) {
        printf("%-.*u | %.*s\n",
               line_num_width,
               builder->span->beg.line - i + 1,
               str_slice_print(&line_));
      }
    }
  }

  // Error itself
  printf("%-*u | %.*s\n"
         "%*s | %s%s %s\n",
         line_num_width,
         builder->span->beg.line + 1,
         str_slice_print(&trgt_line),
         line_num_width,
         "",
         str_cstr(&pad0),
         str_cstr(&pad1),
         str_cstr(builder->err_msg));

  // Lines after
  {
    u32 actual_lines_after = 0;
    for (u32 i = 0; i < lines_after; i++) {
      StrSlice line_ = span_line_after(*builder->span, &builder->src->src, i);
      if (!str_slice_is_null(&line_)) {
        printf("%-.*u | %.*s\n",
               line_num_width,
               builder->span->beg.line + 2 + i,
               str_slice_print(&line_));
        actual_lines_after++;
      }
    }

    u32 pad_line_after = builder->pad_line_after;
    if (actual_lines_after == 0) {
      pad_line_after = LN_CLAMP(pad_line_after, 1, 99);
    }
    for (u32 i = 0; i < pad_line_after; i++) {
      printf("%*s |\n", line_num_width, "");
    }
  }

  // Final info
  printf("Suggestion: %s\n", str_cstr(builder->err_sugg));

  release_str(&pad0);
  release_str(&pad1);
}

// ========================================================================== //
// Err
// ========================================================================== //

Str
err_pad_str(u32 code_point, u32 count)
{
  u32 buf_size = unicode_width(code_point) * count;
  char* buf = alloc(buf_size + 1, kLnMinAlign);
  if (!buf) {
    return str_null();
  }
  buf[buf_size] = 0;
  u32 off = 0;
  for (u32 i = 0; i < count; i++) {
    u32 width = 0;
    unicode_encode(buf, off, code_point, &width);
    off += width;
  }
  return make_str(buf);
}