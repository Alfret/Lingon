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

#ifndef LN_ERR_H
#define LN_ERR_H

#include "str.h"
#include "span.h"
#include "src.h"

// ========================================================================== //
// Err
// ========================================================================== //

/* Error */
typedef struct Err
{
  /* Message */
  Str* msg;
  /* Span */
  Span span;
} Err;

// ========================================================================== //
// ErrList
// ========================================================================== //

/* Error list */
typedef struct ErrList
{
  /* Buffer */
  Err* buf;
  /* Length */
  u32 len;
  /* Capacity */
  u32 cap;
} ErrList;

// -------------------------------------------------------------------------- //

/* Make err list */
ErrList
make_err_list();

// -------------------------------------------------------------------------- //

/* Release err list */
void
release_err_list(ErrList* list);

// -------------------------------------------------------------------------- //

/* Append to err list */
void
err_list_append(ErrList* list, const Err* err);

// -------------------------------------------------------------------------- //

/* Remove from err list */
Err
err_list_remove(ErrList* list, u32 index);

// -------------------------------------------------------------------------- //

/* Get object from err list */
const Err*
err_list_get(ErrList* list, u32 index);

// -------------------------------------------------------------------------- //

/* Reserve capacity in err list */
void
err_list_reserve(ErrList* list, u32 cap);

// ==========================================================================
// // ErrNum
// ==========================================================================
// //

typedef enum ErrNum
{
  kErrNumNone = 0,
  kErrNumUnexpTok,
} ErrNum;

// -------------------------------------------------------------------------- //

Str
err_num_to_str(ErrNum num);

// ========================================================================== //
// ErrBuilder
// ========================================================================== //

typedef struct ErrBuilder
{
  /* Error number */
  ErrNum err_num;
  /* Entire source */
  const Src* src;
  /* Description */
  const Str* err_desc;
  /* Message */
  const Str* err_msg;
  /* Suggestion */
  const Str* err_sugg;
  /* Span */
  const Span* span;
  /* Num of lines before */
  u32 line_before;
  /* Num of lines before */
  u32 line_after;
  /* Pad lines before */
  u32 pad_line_before;
  /* Pad lines after */
  u32 pad_line_after;
} ErrBuilder;

// -------------------------------------------------------------------------- //

ErrBuilder
make_err_builder(const Src* src);

// -------------------------------------------------------------------------- //

void
err_builder_set_err_num(ErrBuilder* builder, ErrNum num);

// -------------------------------------------------------------------------- //

void
err_builder_set_desc(ErrBuilder* builder, const Str* desc);

// -------------------------------------------------------------------------- //

void
err_builder_set_msg(ErrBuilder* builder, const Str* msg);

// -------------------------------------------------------------------------- //

void
err_builder_set_sugg(ErrBuilder* builder, const Str* sugg);

// -------------------------------------------------------------------------- //

void
err_builder_set_span(ErrBuilder* builder, const Span* span);

// -------------------------------------------------------------------------- //

void
err_builder_set_lines_before(ErrBuilder* builder, u32 lines);

// -------------------------------------------------------------------------- //

void
err_builder_set_lines_after(ErrBuilder* builder, u32 lines);

// -------------------------------------------------------------------------- //

void
err_builder_set_pad_lines_before(ErrBuilder* builder, u32 lines);
// -------------------------------------------------------------------------- //

void
err_builder_set_pad_lines_after(ErrBuilder* builder, u32 lines);

// -------------------------------------------------------------------------- //

void
err_builder_emit(const ErrBuilder* builder);

// -------------------------------------------------------------------------- //

Str
err_pad_str(u32 code_point, u32 count);

#endif // LN_ERR_H