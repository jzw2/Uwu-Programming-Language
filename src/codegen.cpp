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

#include <iostream>
//all the code gen functions
namespace naruto {

	llvm::Value * ASTIden::generate()
	{
		//i think thi will work
		llvm::Value *v = sLocals[iden];
	
		if (v == nullptr) 
		{
			std::cerr << "It's not like a wanted to be instatiated or anything, baka!" << iden << std::endl;
		}
		
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
		if (func == nullptr) 
		{
			if (iden->getIden() == "puts") 
			{
				//pro hack
				
				std::vector<llvm::Type *> putsArgs;
				putsArgs.push_back(sBuilder.getInt8Ty()->getPointerTo());
				llvm::FunctionType *putsType = llvm::FunctionType::get(sBuilder.getInt32Ty(), putsArgs, false);
				func = llvm::Function::Create(putsType, llvm::Function::ExternalLinkage, "puts", sModule.get());
			} 
			else if (iden->getIden() == "printf") 
			{
				//pro hack
				
				std::vector<llvm::Type *> putsArgs;
				putsArgs.push_back(sBuilder.getInt8Ty()->getPointerTo());
				llvm::FunctionType *putsType = llvm::FunctionType::get(sBuilder.getInt32Ty(), putsArgs, true);
				func = llvm::Function::Create(putsType, llvm::Function::ExternalLinkage, "printf", sModule.get());
			} 
		}
		std::vector<llvm::Value*> args;
		for (auto param : params) 
		{
			args.push_back(param->generate());
		}
		return sBuilder.CreateCall(func, args, "calling the function");
	}
	
	llvm::Value * ASTExpr::generate()
	{
		//ok this one is going to be hard
		if (op) 
		{
			auto left = lhs->generate();
			auto right = rhs->generate();
			if (op->getOp() == "+") 
			{
				return sBuilder.CreateAdd(left, right, "adding");
			} 
			else if (op->getOp() == "*") 
			{
				return sBuilder.CreateMul(left, right, "multiplying");
			} 
			else if (op->getOp() == "-") 
			{
				return sBuilder.CreateSub(left, right, "subtracting");
			} 
			else if (op->getOp() == "/") 
			{
				return sBuilder.CreateSDiv(left, right, "diving");
			} 
			else if (op->getOp() == "==") 
			{
				auto leftType = left->getType();
				auto rightType = right->getType();
				return	sBuilder.CreateICmpEQ(left, right, "camping");
			}
			else 
			{
				return nullptr; //uh oh
			}
		} 
		else if (lhs) 
		{
			return lhs->generate();
		} 
		else if (iden) 
		{
			return iden->generate();
		} 
		else if (int_v) 
		{
			return int_v->generate();
		} 
		else if (flt) 
		{
			return flt->generate();
		} 
		else if (call) 
		{
			return call->generate();
		} 
		else if (str) 
		{
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
	
		if (val == nullptr) 
		{
			std::cerr << name << "-dono was null! Kowai!" << std::endl;
		}
		// Store the initial value into the alloca.
		if (sLocals.count(name->getIden())) 
		{	
			alloc = sLocals[name->getIden()];
			return sBuilder.CreateStore(val->generate(), alloc);
		} 
		else 
		{
			alloc = sBuilder.CreateAlloca(llvm::Type::getInt64Ty(sContext), val->generate());
			sLocals[name->getIden()] = alloc;
		}
	
		// Add arguments to variable symbol table.
		return alloc;
	}
	
	llvm::Value * ASTSelState::generate()
	{
		auto condition = expr->generate();
		llvm::Function* func = sBuilder.GetInsertBlock()->getParent();
	
		llvm::BasicBlock* then_block = llvm::BasicBlock::Create(sContext, "then", func);
		llvm::BasicBlock* else_block = llvm::BasicBlock::Create(sContext, "else");
		llvm::BasicBlock* merge_block = llvm::BasicBlock::Create(sContext, "merge");
	
		sBuilder.CreateCondBr(condition, then_block, else_block);
		sBuilder.SetInsertPoint(then_block);
	
		for (auto& then_state : if_body) 
		{
			then_state->generate();
		}

		sBuilder.CreateBr(merge_block);
		then_block = sBuilder.GetInsertBlock();
	
		func->getBasicBlockList().push_back(else_block);
		sBuilder.SetInsertPoint(else_block); 
		if (elif) 
		{
			elif->generate();
		} 
		else 
		{
			for (auto& else_statement : else_body) 
			{
				else_statement->generate();
			}
		}
		sBuilder.CreateBr(merge_block);
		else_block = sBuilder.GetInsertBlock();
	
	
		func->getBasicBlockList().push_back(merge_block);
		sBuilder.SetInsertPoint(merge_block);
	
		return nullptr;
	}
	
	llvm::Value * ASTWhileState::generate()
	{
		return nullptr;
	}
		
	llvm::Value * ASTState::generate()
	{
		if (retexpr) 
		{
			return retexpr->generate();
		} 
		else if (vdc)
		{
			return vdc->generate();
		} 
		else if (expr) 
		{
			return expr->generate();
		} 
		else if (ws) 
		{
			return ws->generate();
		} 
		else if (ss) 
		{
			return ss->generate();
		} 
		else if (thread) 
		{
			return thread->generate();
		}
		return nullptr;
	}
	
	static llvm::AllocaInst* CreateEntryBlockAlloca(llvm::Function* func, const std::string& var_name) 
	{
		llvm::IRBuilder<> idk(&func->getEntryBlock(), func->getEntryBlock().begin());
		return idk.CreateAlloca(llvm::Type::getInt64Ty(sContext), 0, var_name.c_str());
	}
	
	llvm::Value * ASTFnDecl::generate()
	{
		//idk what type its going to be so for now evertyhin is an into
		std::vector<llvm::Type *> types(params.size(), llvm::Type::getInt64Ty(sContext)); 		
		llvm::FunctionType *func_type = llvm::FunctionType::get(llvm::Type::getInt64Ty(sContext), types, false);
	
		//created the function
		llvm::Function* func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, name->getIden(), sModule.get());
	
	
		//creatirg fntuctiuon bady
		llvm::BasicBlock *block = llvm::BasicBlock::Create(sContext, "entry point", func);
		//insert
		sBuilder.SetInsertPoint(block);
		//SETUP the names ofy the function
		int index = 0;
		for (auto &arg : func->args()) 
		{
			// Create an alloca for this variable.
			llvm::AllocaInst *alloc = CreateEntryBlockAlloca(func, arg.getName());
	
			// Store the initial value into the alloca.
			sBuilder.CreateStore(&arg, alloc);
	
			// Add arguments to variable symbol table.
			arg.setName(params[index]->getIden());
			sLocals[arg.getName()] = alloc;
		}
		for (auto state : body) 
		{
			state->generate();
		}
	
		return func;
	}

	llvm::Value* ASTRoot::generate() 
	{
		for (auto& func : funcs) 
		{
			func->generate();
		}
		return nullptr;
	}

	llvm::Value * ASTString::generate()
	{
		return sBuilder.CreateGlobalStringPtr(str);
		//return llvm::ConstantDataArray::getString(sContext, llvm::StringRef(str));
	}

	llvm::Value * ASTLambdaThread::generate() 
	{
		return nullptr;
	}

	llvm::Value * ASTBinOp::generate()
	{
		return nullptr;
	}
}
