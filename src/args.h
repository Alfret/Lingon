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

#ifndef LN_ARGS_H
#define LN_ARGS_H

#include "common.h"
#include "str.h"

// ========================================================================== //
// Args
// ========================================================================== //

typedef struct Args
{
  /* Output file name */
  Str output;
  /* Input file names */
  StrList input;
  /* Target. Empty for native */
  Str target;
  /* Show help */
  bool help;
  /* Verbose output */
  bool verbose;
  /* Lsp mode */
  bool lsp;
  /* Lsp data */
  struct
  {
    Str type;
    Str host;
    Str port;
  } lsp_data;
  /* Debug: Dump tokens */
  bool dbg_dump_tokens;
  /* Debug: Dump ast */
  bool dbg_dump_ast;
} Args;

// -------------------------------------------------------------------------- //

void
make_args(int argc, char** argv, Args* p_args);

// -------------------------------------------------------------------------- //

void
release_args(Args* p_args);

#endif // LN_ARGS_H