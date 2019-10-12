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

#ifndef LN_TARGET_H
#define LN_TARGET_H

#include "llvm_c_ext.h"
#include "common.h"

// ========================================================================== //
// TargetErr
// ========================================================================== //

typedef enum TargetErr
{
  /* No error */
  kTargetNoErr,
  /* Invalid target */
  kTargetInvTarget
} TargetErr;

// ========================================================================== //
// Target
// ========================================================================== //

/* Target machine */
typedef struct Target
{
  /* Triple */
  LLVMTripleRef triple;
  /* Machine */
  LLVMTargetMachineRef machine;
  /* Data layout */
  LLVMTargetDataRef data_layout;
} Target;

// -------------------------------------------------------------------------- //

TargetErr
make_target(const Str* target_name, Target* p_target);

// -------------------------------------------------------------------------- //

void
release_target(Target* target);

// -------------------------------------------------------------------------- //

u64
target_get_type_sizeof(const Target* target, Type* type);

// -------------------------------------------------------------------------- //

u64
target_get_type_alignof(const Target* target, Type* type);

#endif // LN_TARGET_H