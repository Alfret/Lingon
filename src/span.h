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

#ifndef LN_POS_H
#define LN_POS_H

#include "str.h"

// ========================================================================== //
// Pos
// ========================================================================== //

/* Position (Point) */
typedef struct Pos
{
  /* Byte offset */
  u32 off;
  /* Line */
  u32 line;
  /* Column */
  u32 col;
} Pos;

/* Make pos from given values */
Pos
make_pos(u32 off, u32 line, u32 col);

// -------------------------------------------------------------------------- //

/* Make pos from offset */
bool
make_pos_off(const Str* str, u32 off, Pos* p_pos);

// -------------------------------------------------------------------------- //

/* Make pos from line and column */
bool
make_pos_line_col(const Str* str, u32 line, u32 col, Pos* p_pos);

// -------------------------------------------------------------------------- //

/* True if 'pos' is before 'other' */
bool
pos_is_before(const Pos* pos, const Pos* other);

// -------------------------------------------------------------------------- //

/* True if 'pos' is after 'other' */
bool
pos_is_after(const Pos* pos, const Pos* other);

// -------------------------------------------------------------------------- //

bool
pos_eq(const Pos* pos0, const Pos* pos1);

// ========================================================================== //
// RangePos
// ========================================================================== //

/* Span (Range) */
typedef struct Span
{
  /* Begin pos */
  Pos beg;
  /* End pos */
  Pos end;
} Span;

/* Make range pos from begin and end pos */
Span
make_span(Pos beg, Pos end);

// -------------------------------------------------------------------------- //

bool
make_span_off(const Str* str, u32 off_beg, u32 off_end, Span* p_span);

// -------------------------------------------------------------------------- //

bool
make_span_line_col(const Str* str,
                   u32 line_beg,
                   u32 col_beg,
                   u32 line_end,
                   u32 col_end,
                   Span* p_span);

// -------------------------------------------------------------------------- //

/* Extract str slice from str and range pos */
StrSlice
span_slice(const Span* pos, const Str* src);

// -------------------------------------------------------------------------- //

StrSlice
span_line(const Span* span, const Str* str);

// -------------------------------------------------------------------------- //

/* Return the n:th line before the one represented by the slice */
StrSlice
span_line_before(Span span, const Str* src, u32 n);

// -------------------------------------------------------------------------- //

/* Return the n:th line after the one represented by the slice */
StrSlice
span_line_after(Span span, const Str* src, u32 n);

// -------------------------------------------------------------------------- //

bool
span_eq(const Span* span0, const Span* span1);

// -------------------------------------------------------------------------- //

/* Join two range pos into one */
Span
span_join(const Span* pos0, const Span* pos1);

#endif // LN_POS_H