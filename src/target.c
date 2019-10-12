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

#include <stdlib.h>

#include "target.h"
#include "str.h"
#include "llvm_util.h"

// ========================================================================== //
// Target
// ========================================================================== //

static LLVMTripleRef
target_match_triple(const Str* target_name)
{
  if (target_name->len == 0) { // Native
    return LLVMGetDefaultTriple();
  } else if (str_eq(target_name, &make_str("x86-win32"))) {
    return LLVMGetTripleFromArchVendorOs(
      LLVMx86ArchType, LLVMUnknownVendorType, LLVMWin32OSType);
  } else if (str_eq(target_name, &make_str("x86_64-win32"))) {
    return LLVMGetTripleFromArchVendorOs(
      LLVMx86_64ArchType, LLVMUnknownVendorType, LLVMWin32OSType);
  } else if (str_eq(target_name, &make_str("aarch64-ios"))) {
    return LLVMGetTripleFromArchVendorOs(
      LLVMAarch64ArchType, LLVMAppleVendorType, LLVMIOSOSType);
  }
  return NULL;
}

// -------------------------------------------------------------------------- //

TargetErr
make_target(const Str* target_name, Target* p_target)
{
  // Match name with triple
  LLVMTripleRef triple = target_match_triple(target_name);

  // Get target
  char* error;
  LLVMTargetRef llvm_target;
  bool success = !LLVMTripleGetTarget(triple, &llvm_target, &error);
  if (!success) {
    return kTargetInvTarget;
  }

  // Create target machine
  LLVMTargetMachineRef target_machine =
    LLVMCreateTargetMachineFromTriple(llvm_target,
                                      triple,
                                      "generic",
                                      "",
                                      LLVMCodeGenLevelDefault,
                                      LLVMRelocDefault,
                                      LLVMCodeModelDefault);

  // Create target data
  LLVMTargetDataRef target_layout = LLVMCreateTargetDataLayout(target_machine);

  // Setup machine
  Target target;
  target.triple = triple;
  target.machine = target_machine;
  target.data_layout = target_layout;
  *p_target = target;
  return kTargetNoErr;
}

// -------------------------------------------------------------------------- //

void
release_target(Target* target)
{
  LLVMDisposeTargetData(target->data_layout);
  LLVMDisposeTargetMachine(target->machine);
  LLVMDisposeTriple(target->triple);
}

// -------------------------------------------------------------------------- //

u64
target_get_type_sizeof(const Target* target, Type* type)
{
  LLVMTypeRef llvm_type = to_llvm_type(type);
  return LLVMABISizeOfType(target->data_layout, llvm_type);
}

// -------------------------------------------------------------------------- //

u64
target_get_type_alignof(const Target* target, Type* type)
{
  LLVMTypeRef llvm_type = to_llvm_type(type);
  return LLVMABIAlignmentOfType(target->data_layout, llvm_type);
}