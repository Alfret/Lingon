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

#include "file.h"
#include "src.h"

// ========================================================================== //
// Src
// ========================================================================== //

SrcErr
make_src(const Str* path, Src* p_src)
{
  Src src;
  FileErr err = file_read_str(path, &src.src);
  if (err != kFileNoErr) {
    return kSrcFileNotFound;
  }
  src.name = str_copy(path);

  *p_src = src;
  return kSrcNoErr;
}

// -------------------------------------------------------------------------- //

Src
make_src_str(const Str* name, Str src)
{
  return (Src){ .name = str_copy(name), .src = src };
}

// -------------------------------------------------------------------------- //

void
release_src(Src* src)
{
  release_str(&src->name);
  release_str(&src->src);
}