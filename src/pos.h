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

/* Calculate line and column */
Pos
make_pos_calc(const Str* str, u32 off);

// ========================================================================== //
// RangePos
// ========================================================================== //

/* Position (Range) */
typedef struct RangePos
{
  /* Begin pos */
  Pos beg;
  /* End pos */
  Pos end;
} RangePos;

/* Make range pos from begin and end pos */
RangePos
make_range_pos(Pos beg, Pos end);

// -------------------------------------------------------------------------- //

/* Extract str slice from str and range pos */
StrSlice
range_pos_slice(const RangePos* pos, const Str* src);

// -------------------------------------------------------------------------- //

/* Join two range pos into one */
RangePos
range_pos_join(const RangePos* pos0, const RangePos* pos1);

#endif // LN_POS_H