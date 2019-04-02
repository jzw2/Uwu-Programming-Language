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

llvm::Value * ASTIden::generate()
{
  //i think thi will work
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
  if (func == nullptr) {
    if (iden->getIden() == "puts") {
      //pro hack
      
      std::vector<llvm::Type *> putsArgs;
      putsArgs.push_back(sBuilder.getInt8Ty()->getPointerTo());
      llvm::FunctionType *putsType = llvm::FunctionType::get(sBuilder.getInt32Ty(), putsArgs, false);
      func = llvm::Function::Create(putsType, llvm::Function::ExternalLinkage, "puts", sModule.get());
    }
  }
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
  } else if (str) {
    return str->generate();
  }

  return nullptr;
}

llvm::Value * ASTRetExpr::generate()
{
  
  return sBuilder.CreateRet(expr->generate());
}
  //not really declaration, rather it is assignment
  //if it has not been used before, create it 
  llvm::Value * ASTVarDecl::generate() 
{
  llvm::AllocaInst *alloc;

  // Store the initial value into the alloca.
  if (sLocals.count(name->getIden())) {
    
    alloc = sLocals[name->getIden()];
    return sBuilder.CreateStore(val->generate(), alloc);
  } else {
    alloc = sBuilder.CreateAlloca(llvm::Type::getInt64Ty(sContext), val->generate());
    sLocals[name->getIden()] = alloc;
  }

  // Add arguments to variable symbol table.
  return alloc;
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
  } else if (vdc){
    return vdc->generate();
  } else if (expr) {
    return expr->generate();
  } else if (ws) {
    return ws->generate();
  } else if (ss) {
    return ss->generate();
  } else if (thread) {
    return thread->generate();
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


  //creatirg fntuctiuon bady
  llvm::BasicBlock *block = llvm::BasicBlock::Create(sContext, "entry point", func);
  //insert
  sBuilder.SetInsertPoint(block);
  //SETUP the names ofy the function
  int index = 0;
  for (auto &arg : func->args()) {
    // Create an alloca for this variable.
    llvm::AllocaInst *alloc = CreateEntryBlockAlloca(func, arg.getName());

    // Store the initial value into the alloca.
    sBuilder.CreateStore(&arg, alloc);

    // Add arguments to variable symbol table.
    arg.setName(params[index]->getIden());
    sLocals[arg.getName()] = alloc;
  }
  for (auto state : body) {
    state->generate();
  }

  return func;
}
  llvm::Value* ASTRoot::generate() {
    return nullptr;
  }
	llvm::Value * ASTString::generate()
	{
		return llvm::ConstantDataArray::getString(sContext, llvm::StringRef(str));
	}

  llvm::Value * ASTLambdaThread::generate() {

		return nullptr;
	}

  llvm::Value * ASTBinOp::generate()
  {
      return nullptr;

	}


}
