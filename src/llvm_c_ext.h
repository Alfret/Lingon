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

#ifndef LN_LLVM_LN_H
#define LN_LLVM_LN_H

#include <llvm-c/Types.h>
#include <llvm-c/Core.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Target.h>

#include "type.h"

/**
 * This header contains enum and structs for abstracting larger parts of the
 * LLVM C++ API than what is available in the C API.
 *
 * This means that the naming convention are made to be similar to the existing
 * LLVM API, thus breaking away from the Lingon style.
 *
 */

// ========================================================================== //
// LLVMArchType
// ========================================================================== //

/* Architecture type */
typedef enum LLVMArchType
{
  LLVMUnknownArchType,
  LLVMARMArchType,
  LLVMARMEBArchType,
  LLVMAarch64ArchType,
  LLVMAvrArchType,
  LLVMMIPSArchType,
  LLVMMIPSelArchType,
  LLVMMIPS64ArchType,
  LLVMRiscv32ArchType,
  LLVMRiscv64ArchType,
  LLVMx86ArchType,
  LLVMx86_64ArchType,
  LLVMWasm64ArchType
} LLVMArchType;

// ========================================================================== //
// LLVMVendorType
// ========================================================================== //

typedef enum LLVMVendorType
{
  LLVMUnknownVendorType,
  LLVMAppleVendorType,
  LLVMPCVendorType,
  LLVMSCEIVendorType,
  LLVMBGPVendorType,
  LLVMBGQVendorType,
  LLVMFreescaleVendorType,
  LLVMIBMVendorType,
  LLVMImaginationTechnologiesVendorType,
  LLVMMipsTechnologiesVendorType,
  LLVMNVIDIAVendorType,
  LLVMCSRVendorType,
  LLVMMyriadVendorType,
  LLVMAMDVendorType,
  LLVMMESAVendorType,
  LLVMSUSEVendorType,
  LLVMOpenEmbeddedVendorType,
  LLVMLastVendorType = LLVMOpenEmbeddedVendorType
} LLVMVendorType;

// ========================================================================== //
// LLVMOSType
// ========================================================================== //

typedef enum LLVMOSType
{
  LLVMUnknownOSType,
  LLVMDarwinOSType,
  LLVMFreeBSDOSType,
  LLVMFuchsisaOSType,
  LLVMIOSOSType,
  LLVMLinuxOSType,
  LLVMMacOSOSType,
  LLVMOpenBSDOSType,
  LLVMWin32OSType,
  LLVMTvOSOSType,
  LLVMWatchOSOSType
} LLVMOSType;

// ========================================================================== //
// LLVMObjectFormatType
// ========================================================================== //

typedef enum LLVMObjectFormatType
{
  LLVMUnknownObjectFormatType,
  LLVMCOFFObjectFormatType,
  LLVMELFObjectFormatType,
  LLVMMachOObjectFormatType,
  LLVMWasmObjectFormatType,
  LLVMXCOFFObjectFormatType,
} LLVMObjectFormatType;

// ========================================================================== //
// LLVMTriple
// ========================================================================== //

/* LLVM triple */
typedef struct LLVMOpaqueTriple* LLVMTripleRef;

// -------------------------------------------------------------------------- //

LLVMTripleRef
LLVMGetTripleFromTargetTriple(const char* TargetTriple);

// -------------------------------------------------------------------------- //

LLVMTripleRef
LLVMGetTripleFromArchVendorOs(LLVMArchType ArchType,
                              LLVMVendorType VendorType,
                              LLVMOSType OSType);

// -------------------------------------------------------------------------- //

LLVMTripleRef
LLVMGetDefaultTriple();

// -------------------------------------------------------------------------- //

void
LLVMDisposeTriple(LLVMTripleRef Triple);

// -------------------------------------------------------------------------- //

LLVMTargetMachineRef
LLVMCreateTargetMachineFromTriple(LLVMTargetRef T,
                                  LLVMTripleRef Triple,
                                  const char* CPU,
                                  const char* Features,
                                  LLVMCodeGenOptLevel Level,
                                  LLVMRelocMode Reloc,
                                  LLVMCodeModel CodeModel);

// -------------------------------------------------------------------------- //

LLVMBool
LLVMTripleGetTarget(LLVMTripleRef Triple,
                    LLVMTargetRef* T,
                    char** ErrorMessage);

// -------------------------------------------------------------------------- //

LLVMArchType
LLVMTripleGetArch(LLVMTripleRef Triple);

// -------------------------------------------------------------------------- //

const char*
LLVMTripleGetTriple(LLVMTripleRef Triple);

// -------------------------------------------------------------------------- //

bool
LLVMTripleIsArch64Bit(LLVMTripleRef Triple);

// -------------------------------------------------------------------------- //

bool
LLVMTripleIsArch32Bit(LLVMTripleRef Triple);

// -------------------------------------------------------------------------- //

bool
LLVMTripleIsArch16Bit(LLVMTripleRef Triple);

#endif // LN_LLVM_LN_H