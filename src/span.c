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

/* Make pos from offset */
bool
make_pos_off(const Str* str, u32 off, Pos* p_pos)
{
  u32 line, col;
  bool success = str_off_to_line_col(str, off, &line, &col);
  if (!success) {
    *p_pos = (Pos){ .off = 0, .line = 0, .col = 0 };
    return false;
  }
  *p_pos = make_pos(off, line, col);
  return true;
}

// -------------------------------------------------------------------------- //

/* Make pos from line and column */
bool
make_pos_line_col(const Str* str, u32 line, u32 col, Pos* p_pos)
{
  u32 off;
  bool success = str_line_col_to_off(str, line, col, &off);
  if (!success) {
    *p_pos = (Pos){ .off = 0, .line = 0, .col = 0 };
    return false;
  }
  *p_pos = make_pos(off, line, col);
  return true;
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

// -------------------------------------------------------------------------- //

bool
pos_eq(const Pos* pos0, const Pos* pos1)
{
  return pos0->off == pos1->off && pos0->line == pos1->line &&
         pos0->col == pos1->col;
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

bool
make_span_off(const Str* str, u32 off_beg, u32 off_end, Span* p_span)
{
  *p_span = (Span){};

  Pos beg, end;
  bool success = make_pos_off(str, off_beg, &beg);
  if (!success) {
    return false;
  }
  success = make_pos_off(str, off_end, &end);
  if (!success) {
    return false;
  }
  *p_span = make_span(beg, end);
  return true;
}

// -------------------------------------------------------------------------- //

bool
make_span_line_col(const Str* str,
                   u32 line_beg,
                   u32 col_beg,
                   u32 line_end,
                   u32 col_end,
                   Span* p_span)
{
  *p_span = (Span){};

  Pos beg, end;
  bool success = make_pos_line_col(str, line_beg, col_beg, &beg);
  if (!success) {
    return false;
  }
  success = make_pos_line_col(str, line_end, col_end, &end);
  if (!success) {
    return false;
  }
  *p_span = make_span(beg, end);
  return true;
}

// -------------------------------------------------------------------------- //

StrSlice
span_slice(const Span* pos, const Str* src)
{
  return make_str_slice(src, pos->beg.off, pos->end.off - pos->beg.off);
}

// -------------------------------------------------------------------------- //

StrSlice
span_line(const Span* span, const Str* str)
{
  if (span->beg.line != span->end.line) {
    return str_slice_null();
  }

  // Find line start
  s64 line_beg = span->beg.off;
  while (line_beg > 0) {
    if (str->buf[line_beg] == '\n') {
      line_beg++;
      break;
    }
    line_beg--;
  }
  if (line_beg == 0 && str->buf[line_beg] == '\n') {
    line_beg++;
  }

  // Find line end
  s64 line_end = span->end.off;
  while (line_end < str->size) {
    if (str->buf[line_end] == '\n') {
      break;
    }
    line_end++;
  }

  return make_str_slice(str, line_beg, line_end - line_beg);
}

// -------------------------------------------------------------------------- //

StrSlice
span_line_before(Span span, const Str* src, u32 n)
{
  if (span.beg.line == 0) {
    return str_slice_null();
  }

  // Find end of line
  s64 line_end = span.beg.off;
  while (line_end > 0) {
    if (src->buf[line_end] == '\n' && n-- == 0) {
      break;
    }
    line_end--;
  }
  if (line_end <= 0) {
    return make_str_slice(src, 0, 0);
  }

  // Find beginning of line
  s64 line_start = line_end - 1;
  while (line_start > 0) {
    if (src->buf[line_start] == '\n') {
      line_start++;
      break;
    }
    line_start--;
  }
  if (line_start < 0) {
    return str_slice_null();
  }

  return make_str_slice(src, line_start, line_end - line_start);
}

// -------------------------------------------------------------------------- //

StrSlice
span_line_after(Span span, const Str* src, u32 n)
{
  // Find end of line
  s64 line_beg = span.end.off;
  while (line_beg < src->size) {
    if (src->buf[line_beg] == '\n' && n-- == 0) {
      line_beg++;
      break;
    }
    line_beg++;
  }
  if (line_beg >= src->size) {
    return str_slice_null();
  }

  // Find end of next line
  s64 line_end = line_beg + 1;
  while (line_end < src->size) {
    if (src->buf[line_end] == '\n') {
      break;
    }
    line_end++;
  }

  return make_str_slice(src, line_beg, line_end - line_beg);
}

// -------------------------------------------------------------------------- //

bool
span_eq(const Span* span0, const Span* span1)
{
  return pos_eq(&span0->beg, &span1->beg) && pos_eq(&span0->end, &span1->end);
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