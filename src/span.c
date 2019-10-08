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

#include "span.h"

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

// -------------------------------------------------------------------------- //

bool
pos_is_before(const Pos* pos, const Pos* other)
{
  return pos->off < other->off;
}

// -------------------------------------------------------------------------- //

/* True if 'pos' is after 'other' */
bool
pos_is_after(const Pos* pos, const Pos* other)
{
  return pos->off > other->off;
}

// ========================================================================== //
// RangePos
// ========================================================================== //

Span
make_span(Pos beg, Pos end)
{
  return (Span){ .beg = beg, .end = end };
}

// -------------------------------------------------------------------------- //

StrSlice
span_slice(const Span* pos, const Str* src)
{
  return make_str_slice(src, pos->beg.off, pos->end.off - pos->beg.off);
}

// -------------------------------------------------------------------------- //

Span
span_join(const Span* pos0, const Span* pos1)
{
  const Pos beg =
    (pos_is_before(&pos0->beg, &pos1->beg) ? pos0->beg : pos1->beg);
  const Pos end =
    (pos_is_after(&pos0->end, &pos1->end) ? pos0->end : pos1->end);
  return make_span(beg, end);
}