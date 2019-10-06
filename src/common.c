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

#include <mimalloc.h>

#include "common.h"
#include "str.h"

// ========================================================================== //
// Assert
// ========================================================================== //

void
assrt(bool cond, const Str* fmt, ...)
{
  if (!cond) {
    va_list args;
    va_start(args, fmt);
    assrt_v(cond, fmt, args);
    va_end(args);
  }
}

// -------------------------------------------------------------------------- //

void
assrt_v(bool cond, const Str* fmt, va_list args)
{
  if (!cond) {
    panic_v(fmt, args);
  }
}

// -------------------------------------------------------------------------- //

void
panic(const Str* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  panic_v(fmt, args);
  va_end(args);
}

// -------------------------------------------------------------------------- //

void
panic_v(const Str* fmt, va_list args)
{
  Str msg = str_format_v(fmt, args);
  fprintf(stderr, "Program panicked: %s\n", str_cstr(&msg));
  fflush(stderr);
  quick_exit(-1);
}

// ========================================================================== //
// Mem
// ========================================================================== //

static u64 s_mem_usage;

// -------------------------------------------------------------------------- //

void*
alloc(u64 size, u64 align)
{
  void* mem = mi_malloc_aligned(size, align);
  s_mem_usage += mi_usable_size(mem);
  return mem;
}

// -------------------------------------------------------------------------- //

void
release(void* mem)
{
  s_mem_usage -= mi_usable_size(mem);
  mi_free(mem);
}

// -------------------------------------------------------------------------- //

u64
mem_usage()
{
  return s_mem_usage;
}