#include "parser.h"

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
#include "llvm/ADT/APInt.h"

//all the code gen functions
namespace naruto {
  

llvm::Value * ASTBinOp::generate()
{
  return nullptr;
}
	
llvm::Value * ASTIden::generate()
{
  return nullptr;
}
	
llvm::Value * ASTInt::generate()
{
  return llvm::ConstantInt::get(sContext, llvm::APInt(64, (uint64_t) val));
}

llvm::Value * ASTFloat::generate()
{
  return llvm::ConstantFP::get(sContext, llvm::APFloat(val));
}

llvm::Value * ASTFnCall::generate()
{
  return nullptr;
}

llvm::Value * ASTExpr::generate()
{
  return nullptr;
}

llvm::Value * ASTRetExpr::generate()
{
  return nullptr;
}

llvm::Value * ASTVarDecl::generate()
{
  return nullptr;
}

llvm::Value * ASTSelState::generate()
{
  return nullptr;
}

llvm::Value * ASTWhileState::generate()
{
  return nullptr;
}
	
llvm::Value * ASTState::generate()
{
  return nullptr;
}

llvm::Value * ASTFnDecl::generate()
{
  return nullptr;
}
}
