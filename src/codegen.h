/*


#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include <llvm-c/Core.h>
#include <llvm-c/Target.h>
#include <llvm-c/Initialization.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Support.h>
#include <llvm-c/Analysis.h>

int
main()
{
  // Init
  LLVMInitializeAllTargets();
  LLVMInitializeAllTargetInfos();
  LLVMInitializeAllTargetMCs();
  LLVMInitializeAllAsmPrinters();
  LLVMInitializeAllAsmParsers();

  // Context
  LLVMModuleRef module = LLVMModuleCreateWithName("my_module");
  LLVMBuilderRef builder = LLVMCreateBuilder();

  const char* targetTriple = LLVMGetDefaultTargetTriple();
  LLVMTargetRef target;
  char* error = NULL;
  bool success = !LLVMGetTargetFromTriple(targetTriple, &target, &error);
  if (!success) {
    printf("Failed to create target (%s)\n", error);
    return -1;
  }
  LLVMTargetMachineRef targetMachine =
    LLVMCreateTargetMachine(target,
                            targetTriple,
                            "generic",
                            "",
                            LLVMCodeGenLevelDefault,
                            LLVMRelocDefault,
                            LLVMCodeModelDefault);

  // Function
  LLVMTypeRef paramTypes[] = { LLVMInt32Type(), LLVMInt32Type() };
  LLVMTypeRef funType = LLVMFunctionType(LLVMInt32Type(), paramTypes, 2, 0);
  LLVMValueRef fun = LLVMAddFunction(module, "mul", funType);

  // Code
  LLVMBasicBlockRef entryBlock = LLVMAppendBasicBlock(fun, "entry");
  LLVMPositionBuilderAtEnd(builder, entryBlock);

  LLVMValueRef tmp0 =
    LLVMBuildMul(builder, LLVMGetParam(fun, 0), LLVMGetParam(fun, 1), "tmp0");
  LLVMBuildRet(builder, tmp0);

  // Verify
  error = NULL;
  success = !LLVMVerifyModule(module, LLVMAbortProcessAction, &error);
  if (!success) {
    printf("Module is not valid\n");
    return -2;
  }

  // Emit
  error = NULL;
  success = !LLVMTargetMachineEmitToFile(
    targetMachine, module, "my_mod.o", LLVMObjectFile, &error);
  if (!success) {
    printf("Failed to write object file\n");
    return -3;
  }

  // Done
  int i = 0;
  return 0;
}




 */