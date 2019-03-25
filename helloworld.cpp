#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/raw_os_ostream.h"

#include <vector>
#include <iostream>

using namespace llvm;
static LLVMContext sContext;
static IRBuilder<> sBuilder(sContext);
static std::unique_ptr<Module> sModule;

int main() {
  sModule = llvm::make_unique<Module>("sugma", sContext);

  FunctionType *funcType = FunctionType::get(sBuilder.getInt32Ty(), false);	
  Function *mainFunc = Function::Create(funcType, Function::ExternalLinkage, "main", sModule.get());

  BasicBlock *bb = BasicBlock::Create(sContext, "entry", mainFunc);
  sBuilder.SetInsertPoint(bb);
  std::vector<Value*> args;
  Value *helloWorld = sBuilder.CreateGlobalStringPtr("sugma\n");

  std::vector<Type *> putsArgs;
	putsArgs.push_back(sBuilder.getInt8Ty()->getPointerTo());
	ArrayRef<Type*> argsRef(putsArgs);
	FunctionType *putsType = FunctionType::get(sBuilder.getInt32Ty(), argsRef, false);
  Function *f = Function::Create(putsType, Function::ExternalLinkage, "puts", sModule.get());
  if (f == nullptr) {
    std::cout << "bad" << std::endl;
  }
  args.push_back(helloWorld);

  Value* ret = sBuilder.CreateCall(f, args);
  sBuilder.CreateRet(ConstantInt::get(sContext, APInt(32, 0)));

  raw_os_ostream file_stream(std::cout);
  sModule->print(file_stream, nullptr);

}
