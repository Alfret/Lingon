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
#include <stdlib.h>

#include "file.h"

// ========================================================================== //
// File
// ========================================================================== //

FileErr
read_file_str(const Str* path, Str* p_str)
{
  return read_file_str2(str_cstr(path), p_str);
}

// -------------------------------------------------------------------------- //

FileErr
read_file_str2(const char* path, Str* p_str)
{
  *p_str = str_null();

  FILE* file = fopen(path, "r");
  if (!file) {
    return kFileNotFound;
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);

  u8* buf = alloc(size + 1, kLnMinAlign);
  if (!buf) {
    return kFileOtherErr;
  }
  fread(buf, 1, size, file);
  if (ferror(file)) {
    release(buf);
    return kFileReadErr;
  }

  *p_str = (Str){ .buf = buf, .size = size, .len = cstr_len((char*)buf) };
  return kFileNoErr;
}