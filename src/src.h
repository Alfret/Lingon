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

#ifndef LN_SRC_H
#define LN_SRC_H

// ========================================================================== //
// Src
// ========================================================================== //

/* Src err */
typedef enum SrcErr
{
  kSrcNoErr,
  kSrcFileNotFound,
} SrcErr;

// -------------------------------------------------------------------------- //

/* Src */
typedef struct Src
{
  /* Name */
  Str name;
  /* Source code text */
  Str src;
} Src;

// -------------------------------------------------------------------------- //

SrcErr
make_src(const Str* path, Src* p_src);

// -------------------------------------------------------------------------- //

/* Make source from existing source string. Ownership is taken of source, not
 * name */
Src
make_src_str(const Str* name, Str src);

// -------------------------------------------------------------------------- //

void
release_src(Src* src);

#endif // LN_SRC_H