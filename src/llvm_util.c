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

#include <llvm-c/Core.h>

#include "llvm_util.h"
#include "str.h"
#include "type.h"

// ========================================================================== //
// LLVMUtil
// ========================================================================== //

LLVMTypeRef
to_llvm_type(Type* type)
{
  switch (type->kind) {
    case kTypeChar: {
      return LLVMInt8Type();
    }
    case kTypeBool: {
      return LLVMInt8Type();
    }
    case kTypeU8: {
      return LLVMInt8Type();
    }
    case kTypeS8: {
      return LLVMInt8Type();
    }
    case kTypeU16: {
      return LLVMInt16Type();
    }
    case kTypeS16: {
      return LLVMInt16Type();
    }
    case kTypeU32: {
      return LLVMInt32Type();
    }
    case kTypeS32: {
      return LLVMInt32Type();
    }
    case kTypeU64: {
      return LLVMInt64Type();
    }
    case kTypeS64: {
      return LLVMInt64Type();
    }
    case kTypeF32: {
      return LLVMFloatType();
    }
    case kTypeF64: {
      return LLVMDoubleType();
    }
    case kTypeArray: {
      return LLVMArrayType(to_llvm_type(type->array.type), type->array.len);
    }
    case kTypePointer: {
      return LLVMPointerType(to_llvm_type(type->pointer.type), 0);
    }
    case kTypeStruct: {
      panic(make_str("Not supported yet"));
      return LLVMInt32Type();
    }
    case kTypeEnum: {
      panic(make_str("Not supported yet"));
      return LLVMInt32Type();
    }
    case kTypeTrait: {
      panic(make_str("Not supported yet"));
      return LLVMInt32Type();
    }
    default: {
      panic(make_str("Invalid"));
    }
  }
}