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

#ifndef LN_COMMON_H
#define LN_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdarg.h>

typedef struct Str Str;

// ========================================================================== //
// Macros
// ========================================================================== //

#define LN_MIN(a, b) a < b ? a : b
#define LN_MAX(a, b) a > b ? a : b

#define LN_UNUSED(var) (void)var

#define LN_CHECK_LEAK()                                                        \
  do {                                                                         \
    assrt(                                                                     \
      mem_usage() == 0, make_str("Leaking memory (%u bytes)"), mem_usage());   \
  } while (0)

#define LN_CONST(name, value)                                                  \
  enum                                                                         \
  {                                                                            \
    name = value                                                               \
  };

#define LN_CLAMP(a, min, max) (a) < min ? min : ((a) > max ? max : (a))

#define kLnMinAlign sizeof(void*)

// ========================================================================== //
// Types
// ========================================================================== //

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;

#if defined(__linux__)
#define LN_NORET __attribute__((noreturn))
#else
#define LN_NORET
#endif

// ========================================================================== //
// Assert
// ========================================================================== //

void
assrt(bool cond, Str fmt, ...);

// -------------------------------------------------------------------------- //

void
assrt_v(bool cond, Str fmt, va_list args);

// -------------------------------------------------------------------------- //

LN_NORET void
panic(Str fmt, ...);

// -------------------------------------------------------------------------- //

LN_NORET void
panic_v(Str fmt, va_list args);

// ========================================================================== //
// Mem
// ========================================================================== //

void*
alloc(u64 size, u64 align);

// -------------------------------------------------------------------------- //

void
release(void* mem);

// -------------------------------------------------------------------------- //

u64
mem_usage();

#endif // LN_COMMON_H