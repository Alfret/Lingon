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

#include <stdio.h>

#include "con.h"

// ========================================================================== //
// Con
// ========================================================================== //

void
con_cprintln(const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  con_cprintln_v(fmt, args);
  va_end(args);
}

// -------------------------------------------------------------------------- //

void
con_cprintln_v(const char* fmt, va_list args)
{
  con_println_v(&make_str(fmt), args);
}

// -------------------------------------------------------------------------- //

void
con_println(const Str* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  con_println_v(fmt, args);
  va_end(args);
}

// -------------------------------------------------------------------------- //

void
con_println_v(const Str* fmt, va_list args)
{
  Str msg = str_format_v(*fmt, args);
  printf("%s\n", str_cstr(&msg));
  release_str(&msg);
}

// ========================================================================== //
// ProgBar
// ========================================================================== //

ProgBar
make_prog_bar(u32 min, u32 max, u32 width)
{
  return (ProgBar){ .fmt = make_str_copy("[=> ]"),
                    .cur = 0,
                    .min = min,
                    .max = max,
                    .width = width };
}

// -------------------------------------------------------------------------- //

void
release_prog_bar(ProgBar* bar)
{
  release_str(&bar->fmt);
}

// -------------------------------------------------------------------------- //

void
prog_bar_set_fmt(ProgBar* bar, const Str* fmt)
{
  release_str(&bar->fmt);
  bar->fmt = str_copy(fmt);
  int y = 0;
  LN_UNUSED(y);
}

// -------------------------------------------------------------------------- //

bool
prog_bar_inc(ProgBar* bar)
{
  return ++bar->cur == bar->max;
}

// -------------------------------------------------------------------------- //

#include <stdlib.h>
#include <string.h>

void
prog_bar_draw(const ProgBar* bar, const Str* label)
{
  u32 len = bar->max - bar->min;
  f32 prog = (f32)bar->cur / len;
  u32 count = (u32)((f32)(bar->width - 2) * prog);

  u32 buf_size = bar->width + label->size;
  char* buf = alloc(buf_size + 1, kLnMinAlign);
  memcpy(buf, label->buf, label->size);
  buf[label->size] = bar->fmt.buf[0];
  buf[buf_size - 1] = bar->fmt.buf[4];
  buf[buf_size] = 0;

  for (u32 i = 0; i < bar->width - 2; i++) {
    u32 off = i + label->size + 1;
    if (i == count) {
      buf[off] = bar->fmt.buf[2];
    } else if (i > count) {
      buf[off] = ' ';
    } else {
      buf[off] = bar->fmt.buf[1];
    }
  }

  printf("\033[2K\r%s", buf);
  fflush(stdout);
  release(buf);
}

// -------------------------------------------------------------------------- //

/* Draw progress bar with formatted label  */
void
prog_bar_draw_fmt(const ProgBar* bar, const Str* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  prog_bar_draw_fmt_v(bar, fmt, args);
  va_end(args);
}

// -------------------------------------------------------------------------- //

/* Draw progress bar with formatted label  */
void
prog_bar_draw_fmt_v(const ProgBar* bar, const Str* fmt, va_list args)
{
  Str label = str_format_v(*fmt, args);
  prog_bar_draw(bar, &label);
  release_str(&label);
}

// -------------------------------------------------------------------------- //

/* Finish drawing progress bar */
void
prog_bar_draw_finish(const ProgBar* bar)
{
  LN_UNUSED(bar);
  printf("\n");
}