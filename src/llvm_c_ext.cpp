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

#include <llvm/Target/TargetMachine.h>

extern "C"
{
#include <llvm-c/Core.h>

#include "llvm_c_ext.h"
#include "str.h"
#include "type.h"
}

// ========================================================================== //
// LLVMArchType
// ========================================================================== //

static llvm::Triple::ArchType
llvm_from_arch_type(LLVMArchType arch_type)
{
  switch (arch_type) {
    case LLVMARMArchType: {
      return llvm::Triple::ArchType::arm;
    }
    case LLVMARMEBArchType: {
      return llvm::Triple::ArchType::armeb;
    }
    case LLVMAarch64ArchType: {
      return llvm::Triple::ArchType::aarch64;
    }
    case LLVMAvrArchType: {
      return llvm::Triple::ArchType::avr;
    }
    case LLVMMIPSArchType: {
      return llvm::Triple::ArchType::mips;
    }
    case LLVMMIPSelArchType: {
      return llvm::Triple::ArchType::mipsel;
    }
    case LLVMMIPS64ArchType: {
      return llvm::Triple::ArchType::mips64;
    }
    case LLVMRiscv32ArchType: {
      return llvm::Triple::ArchType::riscv32;
    }
    case LLVMRiscv64ArchType: {
      return llvm::Triple::ArchType::riscv64;
    }
    case LLVMx86ArchType: {
      return llvm::Triple::ArchType::x86;
    }
    case LLVMx86_64ArchType: {
      return llvm::Triple::ArchType::x86_64;
    }
    case LLVMWasm64ArchType: {
      return llvm::Triple::ArchType::wasm64;
    }
    case LLVMUnknownArchType:
    default: {
      return llvm::Triple::ArchType::UnknownArch;
    }
  }
}

// -------------------------------------------------------------------------- //

static LLVMArchType
llvm_to_arch_type(llvm::Triple::ArchType arch_type)
{
  switch (arch_type) {
    case llvm::Triple::arm: {
      return LLVMARMArchType;
    }
    case llvm::Triple::armeb: {
      return LLVMARMEBArchType;
    }
    case llvm::Triple::aarch64: {
      return LLVMAarch64ArchType;
    }
    case llvm::Triple::avr: {
      return LLVMAvrArchType;
    }
    case llvm::Triple::mips: {
      return LLVMMIPSArchType;
    }
    case llvm::Triple::mipsel: {
      return LLVMMIPSelArchType;
    }
    case llvm::Triple::mips64: {
      return LLVMMIPS64ArchType;
    }
    case llvm::Triple::riscv32: {
      return LLVMRiscv32ArchType;
    }
    case llvm::Triple::riscv64: {
      return LLVMRiscv64ArchType;
    }
    case llvm::Triple::x86: {
      return LLVMx86ArchType;
    }
    case llvm::Triple::x86_64: {
      return LLVMx86_64ArchType;
    }
    case llvm::Triple::wasm64: {
      return LLVMWasm64ArchType;
    }
    case llvm::Triple::UnknownArch:
    default: {
      return LLVMUnknownArchType;
    }
  }
}

// ========================================================================== //
// LLVMVendorType
// ========================================================================== //

static llvm::Triple::VendorType
llvm_from_vendor_type(LLVMVendorType vendor_type)
{
  switch (vendor_type) {
    case LLVMUnknownVendorType: {
      return llvm::Triple::VendorType::UnknownVendor;
    }
    case LLVMAppleVendorType: {
      return llvm::Triple::VendorType::Apple;
    }
    case LLVMPCVendorType: {
      return llvm::Triple::VendorType::PC;
    }
    case LLVMSCEIVendorType: {
      return llvm::Triple::VendorType::SCEI;
    }
    case LLVMBGPVendorType: {
      return llvm::Triple::VendorType::BGP;
    }
    case LLVMBGQVendorType: {
      return llvm::Triple::VendorType::BGQ;
    }
    case LLVMFreescaleVendorType: {
      return llvm::Triple::VendorType::Freescale;
    }
    case LLVMIBMVendorType: {
      return llvm::Triple::VendorType::IBM;
    }
    case LLVMImaginationTechnologiesVendorType: {
      return llvm::Triple::VendorType::ImaginationTechnologies;
    }
    case LLVMMipsTechnologiesVendorType: {
      return llvm::Triple::VendorType::MipsTechnologies;
    }
    case LLVMNVIDIAVendorType: {
      return llvm::Triple::VendorType::NVIDIA;
    }
    case LLVMCSRVendorType: {
      return llvm::Triple::VendorType::CSR;
    }
    case LLVMMyriadVendorType: {
      return llvm::Triple::VendorType::Myriad;
    }
    case LLVMAMDVendorType: {
      return llvm::Triple::VendorType::AMD;
    }
    case LLVMMESAVendorType: {
      return llvm::Triple::VendorType::Mesa;
    }
    case LLVMSUSEVendorType: {
      return llvm::Triple::VendorType::SUSE;
    }
    case LLVMOpenEmbeddedVendorType: {
      return llvm::Triple::VendorType::OpenEmbedded;
    }
  }
}

// -------------------------------------------------------------------------- //

static LLVMVendorType
llvm_to_vendor_type(llvm::Triple::VendorType vendor_type)
{
  switch (vendor_type) {
    case llvm::Triple::UnknownVendor: {
      return LLVMUnknownVendorType;
    }
    case llvm::Triple::Apple: {
      return LLVMAppleVendorType;
    }
    case llvm::Triple::PC: {
      return LLVMPCVendorType;
    }
    case llvm::Triple::SCEI: {
      return LLVMSCEIVendorType;
    }
    case llvm::Triple::BGP: {
      return LLVMBGPVendorType;
    }
    case llvm::Triple::BGQ: {
      return LLVMBGQVendorType;
    }
    case llvm::Triple::Freescale: {
      return LLVMFreescaleVendorType;
    }
    case llvm::Triple::IBM: {
      return LLVMIBMVendorType;
    }
    case llvm::Triple::ImaginationTechnologies: {
      return LLVMImaginationTechnologiesVendorType;
    }
    case llvm::Triple::MipsTechnologies: {
      return LLVMMipsTechnologiesVendorType;
    }
    case llvm::Triple::NVIDIA: {
      return LLVMNVIDIAVendorType;
    }
    case llvm::Triple::CSR: {
      return LLVMCSRVendorType;
    }
    case llvm::Triple::Myriad: {
      return LLVMMyriadVendorType;
    }
    case llvm::Triple::AMD: {
      return LLVMAMDVendorType;
    }
    case llvm::Triple::Mesa: {
      return LLVMMESAVendorType;
    }
    case llvm::Triple::SUSE: {
      return LLVMSUSEVendorType;
    }
    case llvm::Triple::OpenEmbedded: {
      return LLVMOpenEmbeddedVendorType;
    }
  }
}

// ========================================================================== //
// LLVMOSType
// ========================================================================== //

static llvm::Triple::OSType
llvm_from_os_type(LLVMOSType os_type)
{
  switch (os_type) {
    case LLVMUnknownOSType: {
      return llvm::Triple::OSType::UnknownOS;
    }
    case LLVMDarwinOSType: {
      return llvm::Triple::OSType::Darwin;
    }
    case LLVMFreeBSDOSType: {
      return llvm::Triple::OSType::FreeBSD;
    }
    case LLVMFuchsisaOSType: {
      return llvm::Triple::OSType::Fuchsia;
    }
    case LLVMIOSOSType: {
      return llvm::Triple::OSType::IOS;
    }
    case LLVMLinuxOSType: {
      return llvm::Triple::OSType::Linux;
    }
    case LLVMMacOSOSType: {
      return llvm::Triple::OSType::MacOSX;
    }
    case LLVMOpenBSDOSType: {
      return llvm::Triple::OSType::OpenBSD;
    }
    case LLVMWin32OSType: {
      return llvm::Triple::OSType::Win32;
    }
    case LLVMTvOSOSType: {
      return llvm::Triple::OSType::TvOS;
    }
    case LLVMWatchOSOSType: {
      return llvm::Triple::OSType::WatchOS;
    }
  }
}

// -------------------------------------------------------------------------- //

static LLVMOSType
llvm_to_os_type(llvm::Triple::OSType osType)
{
  switch (osType) {
    case llvm::Triple::Darwin: {
      return LLVMDarwinOSType;
    }
    case llvm::Triple::FreeBSD: {
      return LLVMFreeBSDOSType;
    }
    case llvm::Triple::Fuchsia: {
      return LLVMFuchsisaOSType;
    }
    case llvm::Triple::IOS: {
      return LLVMIOSOSType;
    }
    case llvm::Triple::Linux: {
      return LLVMLinuxOSType;
    }
    case llvm::Triple::MacOSX: {
      return LLVMMacOSOSType;
    }
    case llvm::Triple::OpenBSD: {
      return LLVMOpenBSDOSType;
    }
    case llvm::Triple::Win32: {
      return LLVMWin32OSType;
    }
    case llvm::Triple::TvOS: {
      return LLVMTvOSOSType;
    }
    case llvm::Triple::WatchOS: {
      return LLVMWatchOSOSType;
    }
    case llvm::Triple::UnknownOS:
    default: {
      return LLVMUnknownOSType;
    }
  }
}

// ========================================================================== //
// LLVMObjectFormatType
// ========================================================================== //

static LLVMObjectFormatType
llvm_to_object_format_type(llvm::Triple::ObjectFormatType objectFormatType)
{
  switch (objectFormatType) {
    case llvm::Triple::UnknownObjectFormat: {
      return LLVMUnknownObjectFormatType;
    }
    case llvm::Triple::COFF: {
      return LLVMCOFFObjectFormatType;
    }
    case llvm::Triple::ELF: {
      return LLVMELFObjectFormatType;
    }
    case llvm::Triple::MachO: {
      return LLVMMachOObjectFormatType;
    }
    case llvm::Triple::Wasm: {
      return LLVMWasmObjectFormatType;
    }
  }
}

// ========================================================================== //
// LLVMTriple
// ========================================================================== //

typedef struct LLVMOpaqueTriple
{
  /* Triple */
  llvm::Triple handle;

  /* Triple string */
  std::string str;
} LLVMOpaqueTriple;

// -------------------------------------------------------------------------- //

LLVMTripleRef
LLVMGetTripleFromTargetTriple(const char* targetTriple)
{
  LLVMTripleRef triple = new LLVMOpaqueTriple();
  triple->handle = llvm::Triple(targetTriple);
  triple->str = triple->handle.getTriple();
  return triple;
}

// -------------------------------------------------------------------------- //

LLVMTripleRef
LLVMGetTripleFromArchVendorOs(LLVMArchType ArchType,
                              LLVMVendorType VendorType,
                              LLVMOSType OSType)
{
  llvm::StringRef arch_str =
    llvm::Triple::getArchTypeName(llvm_from_arch_type(ArchType));
  llvm::StringRef vendor_str =
    llvm::Triple::getVendorTypeName(llvm_from_vendor_type(VendorType));
  llvm::StringRef os_str =
    llvm::Triple::getOSTypeName(llvm_from_os_type(OSType));

  LLVMTripleRef triple = new LLVMOpaqueTriple();
  triple->handle = llvm::Triple(arch_str, vendor_str, os_str);
  triple->str = triple->handle.getTriple();
  return triple;
}

// -------------------------------------------------------------------------- //

LLVMTripleRef
LLVMGetDefaultTriple()
{
  char* triple_str = LLVMGetDefaultTargetTriple();
  LLVMTripleRef triple_ref = LLVMGetTripleFromTargetTriple(triple_str);
  LLVMDisposeMessage(triple_str);
  return triple_ref;
}

// -------------------------------------------------------------------------- //

void
LLVMDisposeTriple(LLVMTripleRef Triple)
{
  delete Triple;
}

// -------------------------------------------------------------------------- //

LLVMTargetMachineRef
LLVMCreateTargetMachineFromTriple(LLVMTargetRef T,
                                  LLVMTripleRef Triple,
                                  const char* CPU,
                                  const char* Features,
                                  LLVMCodeGenOptLevel Level,
                                  LLVMRelocMode Reloc,
                                  LLVMCodeModel CodeModel)
{
  return LLVMCreateTargetMachine(
    T, LLVMTripleGetTriple(Triple), CPU, Features, Level, Reloc, CodeModel);
}

// -------------------------------------------------------------------------- //

LLVMBool
LLVMTripleGetTarget(LLVMTripleRef Triple, LLVMTargetRef* T, char** ErrorMessage)
{
  const char* triple_str = Triple->handle.getTriple().c_str();
  return LLVMGetTargetFromTriple(triple_str, T, ErrorMessage);
}
// -------------------------------------------------------------------------- //

LLVMArchType
LLVMTripleGetArch(LLVMTripleRef triple)
{
  return llvm_to_arch_type(triple->handle.getArch());
}

// -------------------------------------------------------------------------- //

const char*
LLVMTripleGetTriple(LLVMTripleRef Triple)
{
  return Triple->str.c_str();
}

// -------------------------------------------------------------------------- //

bool
LLVMTripleIsArch64Bit(LLVMTripleRef Triple)
{
  return Triple->handle.isArch64Bit();
}

// -------------------------------------------------------------------------- //

bool
LLVMTripleIsArch32Bit(LLVMTripleRef Triple)
{
  return Triple->handle.isArch32Bit();
}

// -------------------------------------------------------------------------- //

bool
LLVMTripleIsArch16Bit(LLVMTripleRef Triple)
{
  return Triple->handle.isArch16Bit();
}