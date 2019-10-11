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

#ifndef LN_CONSOLE_H
#define LN_CONSOLE_H

#include <stdarg.h>

#include "common.h"
#include "str.h"

// ========================================================================== //
// Con
// ========================================================================== //

#define con_col16(val) "\033[" #val "m"

// -------------------------------------------------------------------------- //

#define con_col256(val) "\033[38:5:" #val "m"

// -------------------------------------------------------------------------- //

/* Reset color */
#define con_col_reset() "\033[0m"

// -------------------------------------------------------------------------- //

#define con_col_ice con_col256(81)

// -------------------------------------------------------------------------- //

/* Print line */
void
con_cprintln(const char* fmt, ...);

// -------------------------------------------------------------------------- //

/* Print line */
void
con_cprintln_v(const char* fmt, va_list args);

// -------------------------------------------------------------------------- //

/* Print line */
void
con_println(const Str* fmt, ...);

// -------------------------------------------------------------------------- //

/* Print line */
void
con_println_v(const Str* fmt, va_list args);

// ========================================================================== //
// ProgBar
// ========================================================================== //

typedef struct ProgBar
{
  Str fmt;
  u32 cur;
  u32 min;
  u32 max;
  u32 width;
} ProgBar;

// -------------------------------------------------------------------------- //

ProgBar
make_prog_bar(u32 min, u32 max, u32 width);

// -------------------------------------------------------------------------- //

void
release_prog_bar(ProgBar* bar);

// -------------------------------------------------------------------------- //

void
prog_bar_set_fmt(ProgBar* bar, const Str* fmt);

// -------------------------------------------------------------------------- //

bool
prog_bar_inc(ProgBar* bar);

// -------------------------------------------------------------------------- //

void
prog_bar_draw(const ProgBar* bar, const Str* label);

// -------------------------------------------------------------------------- //

/* Draw progress bar with formatted label  */
void
prog_bar_draw_fmt(const ProgBar* bar, const Str* fmt, ...);

// -------------------------------------------------------------------------- //

/* Draw progress bar with formatted label  */
void
prog_bar_draw_fmt_v(const ProgBar* bar, const Str* fmt, va_list args);

// -------------------------------------------------------------------------- //

/* Finish drawing progress bar */
void
prog_bar_draw_finish(const ProgBar* bar);

#endif // LN_CONSOLE_H