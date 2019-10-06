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

#include "pos.h"

// ========================================================================== //
// Pos
// ========================================================================== //

Pos
make_pos(u32 off, u32 line, u32 col)
{
  return (Pos){ .off = off, .line = line, .col = col };
}

// -------------------------------------------------------------------------- //

Pos
make_pos_calc(const Str* str, u32 off)
{
  StrIter iter = make_str_iter(str);
  u32 code_point, line = 0, col = 0;
  while ((code_point = str_iter_next(&iter)) != kInvalidCodepoint) {
    col++;
    if (code_point == '\n') {
      line++;
      col = 0;
    }
    if (iter.off == off) {
      break;
    }
  }
  return make_pos(off, line, col);
}

// ========================================================================== //
// RangePos
// ========================================================================== //

RangePos
make_range_pos(Pos beg, Pos end)
{
  return (RangePos){ .beg = beg, .end = end };
}

// -------------------------------------------------------------------------- //

StrSlice
range_pos_slice(const RangePos* pos, const Str* src)
{
  return make_str_slice(src, pos->beg.off, pos->end.off - pos->beg.off);
}

// -------------------------------------------------------------------------- //

RangePos
range_pos_join(const RangePos* pos0, const RangePos* pos1)
{
  LN_UNUSED(pos1);
  return *pos0;
}