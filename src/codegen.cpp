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
#include "llvm/IR/Instructions.h"

//all the code gen functions
namespace naruto {
  

llvm::Value * ASTBinOp::generate()
{
  //nothing because it is all contained within the expression parser
  return nullptr;
}
	
llvm::Value * ASTIden::generate()
{
  //this does not look right at all
  llvm::Value *v = sLocals[iden];

  
  return sBuilder.CreateLoad(v, iden.c_str());
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
  llvm::Function* func = sModule->getFunction(iden->getIden());
  std::vector<llvm::Value*> args;
  for (auto param : params) {
    args.push_back(param->generate());
  }
  return sBuilder.CreateCall(func, args, "calling the function");
}

llvm::Value * ASTExpr::generate()
{

  //ok this one is going to be hard
  if (op) {
    auto left = lhs->generate();
    auto right = rhs->generate();
    if (op->getOp() == "+") {
      return sBuilder.CreateAdd(left, right, "adding");
    } else if (op->getOp() == "*") {
      return sBuilder.CreateMul(left, right, "multiplying");
    } else if (op->getOp() == "-") {
      return sBuilder.CreateSub(left, right, "subtracting");
    } else if (op->getOp() == "/") {
      return sBuilder.CreateSDiv(left, right, "diving");
    } else if (op->getOp() == "==") {
      return  sBuilder.CreateICmpEQ(left, right, "camping");
    }else {
      return nullptr; //uh oh
    }
  } else if (lhs) {
    return lhs->generate();
  } else if (iden) {
    return iden->generate();
  } else if (int_v) {
    return int_v->generate();
  } else if (flt) {
    return flt->generate();
  } else if (call) {
    return call->generate();
  }

  return nullptr;
}

llvm::Value * ASTRetExpr::generate()
{
  
  return sBuilder.CreateRet(expr->generate());
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

  if (retexpr) {
    return retexpr->generate();
  }
  return nullptr;
}

  static llvm::AllocaInst* CreateEntryBlockAlloca(llvm::Function* func, const std::string& var_name) {
    llvm::IRBuilder<> idk(&func->getEntryBlock(), func->getEntryBlock().begin());
    return idk.CreateAlloca(llvm::Type::getDoubleTy(sContext), 0, var_name.c_str());
  }

llvm::Value * ASTFnDecl::generate()
{
  std::vector<llvm::Type *> types(params.size(), llvm::Type::getInt64Ty(sContext)); //idk what type its going to be so for now evertyhin is an into
  llvm::FunctionType *func_type = llvm::FunctionType::get(llvm::Type::getInt64Ty(sContext), types, false);

  //created the function
  llvm::Function* func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, name->getIden(), sModule.get());

  //setup the names ofy the function
  // auto iter = func->args().begin(); //tbh idkw tahte the arg is for thi so you nheanh
  // for (int i = 0; i < params.size(); i++,iter++) {
  //   auto& arg = *iter;
  //   arg.setName(params[i]->getIden()); //
  //   llvm::AllocaInst* alloc = CreateEntryBlockAlloca(func, arg.getName());

  //   sBuilder.CreateStore(&arg, alloc);
  //   sLocals[arg.getName()] = alloc;
  // }

  //creatirg fntuctiuon bady
  llvm::BasicBlock *block = llvm::BasicBlock::Create(sContext, "plec that i need to endetr", func);
  //insert
  sBuilder.SetInsertPoint(block);
  for (auto state : body) {
    state->generate();
  }

  return func;
}
  llvm::Value* ASTRoot::generate() {
    return nullptr;
  }
}
