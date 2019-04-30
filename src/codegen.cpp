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

std::map<std::string, llvm::Function*> funcScope;

namespace naruto {

	llvm::Value * ASTIden::generate()
	{
		//i think this will work

		std::string func_name = sBuilder.GetInsertBlock()->getParent()->getName();
		llvm::Value *v = sLocals[func_name + iden];
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
			{//pro hack
				std::vector<llvm::Type *> putsArgs;
				putsArgs.push_back(sBuilder.getInt8Ty()->getPointerTo());
				llvm::FunctionType *putsType = llvm::FunctionType::get(sBuilder.getInt32Ty(), putsArgs, true);
				func = llvm::Function::Create(putsType, llvm::Function::ExternalLinkage, "printf", sModule.get());
			} 
			else if (iden->getIden() == "clone") {


				std::vector<llvm::Type *> anon_func_types_vec;
				anon_func_types_vec.push_back(llvm::Type::getInt8Ty(sContext)->getPointerTo());
				llvm::FunctionType *anon_func_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(sContext), anon_func_types_vec, false);


				std::vector<llvm::Type *> clone_types_vec;
				clone_types_vec.push_back(anon_func_type);
				clone_types_vec.push_back(sBuilder.getInt8Ty()->getPointerTo());
				clone_types_vec.push_back(sBuilder.getInt32Ty());
				clone_types_vec.push_back(sBuilder.getInt8Ty()->getPointerTo());

				llvm::FunctionType *clone_type = llvm::FunctionType::get(sBuilder.getInt32Ty(), clone_types_vec, true);
				auto clone_func = llvm::Function::Create(clone_type, llvm::Function::ExternalLinkage, "clone", sModule.get());
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
			else if (op->getOp() == ">")
			{
				auto leftType = left->getType();
				auto rightType = right->getType();
				return	sBuilder.CreateICmpSGT(left, right, "comparing greater than");
			}
			else if (op->getOp() == "<")
			{
				auto leftType = left->getType();
				auto rightType = right->getType();
				return	sBuilder.CreateICmpSLT(left, right, "comparing less than than");
			}
			else 
			{
				std::cerr << "unable to regecognize operator " << op->getOp() << std::endl;
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
		std::string func_name = sBuilder.GetInsertBlock()->getParent()->getName();
		std::string full_name = func_name + name->getIden();

		if (sLocals.count(full_name)) 
		{	
			alloc = sLocals[full_name];
			return sBuilder.CreateStore(val->generate(), alloc);
		} 
		else 
		{
			auto generated_val = val->generate();
			alloc = sBuilder.CreateAlloca(llvm::Type::getInt64Ty(sContext));
			//llvm::ConstantInt::get(sContext, llvm::APInt(64, (uint64_t) val));
			sBuilder.CreateStore(generated_val, alloc);
			sLocals[full_name] = alloc;
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
		auto last_instr = sBuilder.GetInsertBlock()->getTerminator();
		if (last_instr == nullptr || std::string("ret") != last_instr->getOpcodeName())
		{
			sBuilder.CreateBr(merge_block);
		}
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
		auto last_instr2 = sBuilder.GetInsertBlock()->getTerminator();
		if (last_instr2 == nullptr || std::string("ret") != last_instr2->getOpcodeName())
		{
			sBuilder.CreateBr(merge_block);
		}
		else_block = sBuilder.GetInsertBlock();


		func->getBasicBlockList().push_back(merge_block);
		sBuilder.SetInsertPoint(merge_block);

		return nullptr;
	}

	llvm::Value * ASTWhileState::generate()
	{
		llvm::Function* func = sBuilder.GetInsertBlock()->getParent();

		llvm::BasicBlock* check_block = llvm::BasicBlock::Create(sContext, "while check", func);
		llvm::BasicBlock* body_block = llvm::BasicBlock::Create(sContext, "while body", func);
		llvm::BasicBlock* merge_block = llvm::BasicBlock::Create(sContext, "merge", func);

		sBuilder.CreateBr(check_block);

		sBuilder.SetInsertPoint(check_block);
		auto condition = expr->generate();
		sBuilder.CreateCondBr(condition, body_block, merge_block);


		sBuilder.SetInsertPoint(body_block);
		for (auto& statement :	state) {
			statement->generate();
		}
		sBuilder.CreateBr(check_block);


		sBuilder.SetInsertPoint(merge_block);
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
			std::string func_name = func->getName();
			std::string full_name = func_name + std::string(arg.getName());
			sLocals[full_name] = alloc;
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
		auto old_block = sBuilder.GetInsertBlock();

		std::string secret_func_name = "xx___secrete_FuncTion__";
		std::vector<llvm::Type *> anon_func_types_vec;
		anon_func_types_vec.push_back(llvm::Type::getInt8Ty(sContext)->getPointerTo());
		llvm::FunctionType *anon_func_type = llvm::FunctionType::get(llvm::Type::getInt64Ty(sContext), anon_func_types_vec, false);
		
		//doing locking of varialbes and barirers stuff
		auto var_names = get_outofcontext_vars(this);
		auto vars = sBuilder.CreateAlloca(llvm::Type::getInt64Ty(sContext)->getPointerTo(), llvm::ConstantInt::get(sContext, llvm::APInt(64, var_names.size())));
		auto locks = sBuilder.CreateAlloca(llvm::Type::getInt64Ty(sContext)->getPointerTo(), llvm::ConstantInt::get(sContext, llvm::APInt(64, var_names.size())));

		auto barrier_lock = sBuilder.CreateAlloca(llvm::Type::getInt64Ty(sContext)->getPointerTo(), llvm::ConstantInt::get(sContext, llvm::APInt(64, 1)));
		auto barrier_count = sBuilder.CreateAlloca(llvm::Type::getInt64Ty(sContext)->getPointerTo(), llvm::ConstantInt::get(sContext, llvm::APInt(64, 1)));
		
		//created the function
		llvm::Function* secret_func = llvm::Function::Create(anon_func_type, llvm::Function::ExternalLinkage, secret_func_name, sModule.get());
		auto fn_param = ((llvm::Value*)secret_func->args_begin)

		//creatirg fntuctiuon bady
		llvm::BasicBlock *block = llvm::BasicBlock::Create(sContext, "entry point", secret_func);
		sBuilder.SetInsertPoint(block);
		for (auto &s : this->state) 
		{
			s->generate(var_names, fn_param);
		}

		sBuilder.CreateRet(llvm::ConstantInt::get(sContext, llvm::APInt(64, 0)));

		sBuilder.SetInsertPoint(old_block);

		std::string secret_var = "__secret_variable";

		long zero = 0;
		ASTVarDecl* secret_init = new ASTVarDecl(secret_var, zero);
		secret_init->generate();

		//ARY, always repeat yourself
		auto params = sBuilder.CreateAlloca(llvm::Type::getInt8Ty(sContext)->getPointerTo()->getPointerTo(), llvm::ConstantInt::get(sContext, llvm::APInt(8, 1)));
		
		auto params0 = sBuilder.CreateGEP(params, llvm::ConstantInt::get(sContext, llvm::APInt(8, 0)));
		auto params1 = sBuilder.CreateGEP(params, llvm::ConstantInt::get(sContext, llvm::APInt(8, 1)));
		auto params2 = sBuilder.CreateGEP(params, llvm::ConstantInt::get(sContext, llvm::APInt(8, 2)));
		auto params3 = sBuilder.CreateGEP(params, llvm::ConstantInt::get(sContext, llvm::APInt(8, 3)));

		auto load0 = sBuilder.CreateStore(vars, params0);
		auto load1 = sBuilder.CreateStore(locks, params1);
		auto load2 = sBuilder.CreateStore(barrier_lock, params2);
		auto load3 = sBuilder.CreateStore(barrier_count, params3);



		auto left = ASTExpr::make_var(secret_var);
		auto right = expr;
		auto cond = ASTExpr::make_binop(left, "<", right);

		std::vector<ASTState*> loop_statements;

		CloneCall* cc = new CloneCall(secret_func_name, params);
		cc->setParent(this);
		auto clone_state = new ASTState(cc);
		loop_statements.push_back(clone_state); //change this later

		auto inc = ASTExpr::make_plus_plus(secret_var);
		ASTVarDecl *dec = new ASTVarDecl(secret_var, inc);

		auto state2 = new ASTState(dec);

		loop_statements.push_back(state2);

		ASTWhileState while_statement(cond, loop_statements);
		while_statement.generate();
	}

	llvm::Value * ASTBinOp::generate()
	{
		//literally nothig
		return nullptr;
	}

	llvm::Value* CloneCall::generate() 
	{
		llvm::Function* clone_func = sModule->getFunction("clone");
		llvm::Function* secret_func = sModule->getFunction(function_name);
		if (clone_func == nullptr) 
		{

			std::vector<llvm::Type *> anon_func_types_vec;
			anon_func_types_vec.push_back(llvm::Type::getInt8Ty(sContext)->getPointerTo());
			llvm::FunctionType *anon_func_type = llvm::FunctionType::get(llvm::Type::getInt64Ty(sContext), anon_func_types_vec, false);
			

			std::vector<llvm::Type *> clone_types_vec;
			clone_types_vec.push_back(anon_func_type->getPointerTo());
			clone_types_vec.push_back(sBuilder.getInt8Ty()->getPointerTo());
			clone_types_vec.push_back(sBuilder.getInt32Ty());
			clone_types_vec.push_back(sBuilder.getInt8Ty()->getPointerTo());

			llvm::FunctionType *clone_type = llvm::FunctionType::get(sBuilder.getInt32Ty(), clone_types_vec, true);
			clone_func = llvm::Function::Create(clone_type, llvm::Function::ExternalLinkage, "clone", sModule.get());
		}
		std::vector<llvm::Value*> clone_arguments;

		clone_arguments.push_back(secret_func);

		std::vector<llvm::Value*> calloc_arguments;
		calloc_arguments.push_back(llvm::ConstantInt::get(sContext, llvm::APInt(64, (uint64_t) 1024 * 8)));
		calloc_arguments.push_back(llvm::ConstantInt::get(sContext, llvm::APInt(64, 1)));

		llvm::Function* calloc_func = sModule->getFunction("calloc");
		if (calloc_func == nullptr) 
		{
			std::vector<llvm::Type *> calloc_types;
			calloc_types.push_back(sBuilder.getInt64Ty());
			calloc_types.push_back(sBuilder.getInt64Ty());
			llvm::FunctionType *calloc_type = llvm::FunctionType::get(sBuilder.getInt8Ty()->getPointerTo(), calloc_types, true);
			calloc_func = llvm::Function::Create(calloc_type, llvm::Function::ExternalLinkage, "calloc", sModule.get());
		}
		auto calloc_ptr = sBuilder.CreateCall(calloc_func, calloc_arguments, "clalling calloc");			
		auto index = llvm::ConstantInt::get(sContext, llvm::APInt(64, 1024 * 8 - 1));
		auto stack_top = sBuilder.CreateGEP(calloc_ptr, index);
		//create the gep arguments
		clone_arguments.push_back(stack_top);
		clone_arguments.push_back(llvm::ConstantInt::get(sContext, llvm::APInt(32, 256)));
		clone_arguments.push_back(llvm::Constant::getNullValue(sBuilder.getInt8Ty()->getPointerTo()));

		return sBuilder.CreateCall(clone_func, clone_arguments, "clalling clone");
	}

	llvm::Value * ASTIden::generate(std::vector<std::string> var_names, llvm::Value * var_vals)
	{
		//i think this will work
		int name_idx = -1;
		for(int i = 0; i < var_names.size(); i++)
		{	
			if(var_names[i] == name->getIden())
			{
				name_idx = i;
				break;
			}
		}

		if(name_idx != -1) 
		{
			auto vars_address = sBuilder.CreateGEP(var_vals, 0);
			//auto locks_address = sBuilder.CreateGEP(var_vals, 1);
			
			auto vars = sBuilder.CreateLoad(vars_address);
			//auto locks = sBuilder.CreateLoad(locks_address);
		
			auto var_address = sBuilder.CreateGEP(vars, name_idx);
			//auto lock_address = sBuilder.CreateGEP(locks name_idx);
			
			auto var = sBuilder.CreateLoad(var_address);
			//auto lock = sBuilder.CreateLoad(lock_address);

			return var;
		}
		else
		{
			std::string func_name = sBuilder.GetInsertBlock()->getParent()->getName();
			llvm::Value *v = sLocals[func_name + iden];
			
			if (v == nullptr) 
			{
				std::cerr << "It's not like a wanted to be instatiated or anything, baka!" << iden << std::endl;
			}

			return sBuilder.CreateLoad(v, iden.c_str());
		}
	}

	llvm::Value * ASTInt::generate(std::vector<std::string> var_names, llvm::Value * var_vals)
	{
		return llvm::ConstantInt::get(sContext, llvm::APInt(64, (uint64_t) val));
	}

	llvm::Value * ASTFloat::generate(std::vector<std::string> var_names, llvm::Value * var_vals)
	{
		return llvm::ConstantFP::get(sContext, llvm::APFloat(val));
	}

	llvm::Value * ASTFnCall::generate(std::vector<std::string> var_names, llvm::Value * var_vals)
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
			{//pro hack
				std::vector<llvm::Type *> putsArgs;
				putsArgs.push_back(sBuilder.getInt8Ty()->getPointerTo());
				llvm::FunctionType *putsType = llvm::FunctionType::get(sBuilder.getInt32Ty(), putsArgs, true);
				func = llvm::Function::Create(putsType, llvm::Function::ExternalLinkage, "printf", sModule.get());
			} 
			else if (iden->getIden() == "clone") {


				std::vector<llvm::Type *> anon_func_types_vec;
				anon_func_types_vec.push_back(llvm::Type::getInt8Ty(sContext)->getPointerTo());
				llvm::FunctionType *anon_func_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(sContext), anon_func_types_vec, false);


				std::vector<llvm::Type *> clone_types_vec;
				clone_types_vec.push_back(anon_func_type);
				clone_types_vec.push_back(sBuilder.getInt8Ty()->getPointerTo());
				clone_types_vec.push_back(sBuilder.getInt32Ty());
				clone_types_vec.push_back(sBuilder.getInt8Ty()->getPointerTo());

				llvm::FunctionType *clone_type = llvm::FunctionType::get(sBuilder.getInt32Ty(), clone_types_vec, true);
				auto clone_func = llvm::Function::Create(clone_type, llvm::Function::ExternalLinkage, "clone", sModule.get());
			}
		}
		std::vector<llvm::Value*> args;
		for (auto param : params) 
		{
			args.push_back(param->generate(var_names, var_vals));
		}
		return sBuilder.CreateCall(func, args, "calling the function");
	}


	//this is all the generation funcs that want to use predefined ref from the array in var_vals
	//used soley for the generation of code in cloned func (from a shadow clone statement)
	llvm::Value * ASTExpr::generate(std::vector<std::string> var_names, llvm::Value * var_vals)
	{
		//ok this one is going to be hard
		if (op) 
		{
			auto left = lhs->generate(var_names, var_vals);
			auto right = rhs->generate(var_names, var_vals);
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
			else if (op->getOp() == ">")
			{
				auto leftType = left->getType();
				auto rightType = right->getType();
				return	sBuilder.CreateICmpSGT(left, right, "comparing greater than");
			}
			else if (op->getOp() == "<")
			{
				auto leftType = left->getType();
				auto rightType = right->getType();
				return	sBuilder.CreateICmpSLT(left, right, "comparing less than than");
			}
			else 
			{
				std::cerr << "unable to regecognize operator " << op->getOp() << std::endl;
				return nullptr; //uh oh
			}
		} 
		else if (lhs) 
		{
			return lhs->generate(var_names, var_vals);
		} 
		else if (iden) 
		{
			return iden->generate(var_names, var_vals);
		} 
		else if (int_v) 
		{
			return int_v->generate(var_names, var_vals);
		} 
		else if (flt) 
		{
			return flt->generate(var_names, var_vals);
		} 
		else if (call) 
		{
			return call->generate(var_names, var_vals);
		} 
		else if (str) 
		{
			return str->generate(var_names, var_vals);
		}

		return nullptr;
	}

	llvm::Value * ASTRetExpr::generate(std::vector<std::string> var_names, llvm::Value * var_vals)
	{

		return sBuilder.CreateRet(expr->generate(var_names, var_vals));
	}

	//not really declaration, rather it is assignment
	//if it has not been used before, create it 
	llvm::Value * ASTVarDecl::generate(std::vector<std::string> var_names, llvm::Value * var_vals) 
	{
		//i think this will work
	
		llvm::AllocaInst *alloc;

		if (val == nullptr) 
		{
			std::cerr << name << "-dono was null! Kowai!" << std::endl;
		}

		for(int i = 0; i < var_names.size(); i++)
		{
			
			should_lock ||= val->isInTree();
		}

		// Store the initial value into the alloca.
		std::string func_name = sBuilder.GetInsertBlock()->getParent()->getName();
		std::string full_name = func_name + name->getIden();

		if (sLocals.count(full_name)) 
		{	
			alloc = sLocals[full_name];
			
			auto locks_address = sBuilder.CreateGEP(var_vals, 1);
			auto locks = sBuilder.CreateLoad(locks_address);
			auto lock_address = sBuilder.CreateGEP(locks name_idx);
			auto lock = sBuilder.CreateLoad(lock_address);

			if(should_lock) 
			
			auto tmp = sBuilder.CreateStore(val->generate(var_names, var_vals), alloc);
			
			if(should_lock)

			return tmp;
		} 
		else 
		{
			auto generated_val = val->generate(var_names, var_vals);
			alloc = sBuilder.CreateAlloca(llvm::Type::getInt64Ty(sContext));
			sBuilder.CreateStore(generated_val, alloc);
			sLocals[full_name] = alloc;
		}

		// Add arguments to variable symbol table.
		return alloc;
	}

	llvm::Value * ASTSelState::generate(std::vector<std::string> var_names, llvm::Value * var_vals)
	{
		auto condition = expr->generate(var_names, var_vals);
		llvm::Function* func = sBuilder.GetInsertBlock()->getParent();

		llvm::BasicBlock* then_block = llvm::BasicBlock::Create(sContext, "then", func);
		llvm::BasicBlock* else_block = llvm::BasicBlock::Create(sContext, "else");
		llvm::BasicBlock* merge_block = llvm::BasicBlock::Create(sContext, "merge");

		sBuilder.CreateCondBr(condition, then_block, else_block);
		sBuilder.SetInsertPoint(then_block);

		for (auto& then_state : if_body) 
		{
			then_state->generate(var_names, var_vals);
		}
		auto last_instr = sBuilder.GetInsertBlock()->getTerminator();
		if (last_instr == nullptr || std::string("ret") != last_instr->getOpcodeName())
		{
			sBuilder.CreateBr(merge_block);
		}
		then_block = sBuilder.GetInsertBlock();
		func->getBasicBlockList().push_back(else_block);
		sBuilder.SetInsertPoint(else_block); 
		if (elif) 
		{
			elif->generate(var_names, var_vals);
		} 
		else 
		{
			for (auto& else_statement : else_body) 
			{
				else_statement->generate(var_names, var_vals);
			}
		}
		auto last_instr2 = sBuilder.GetInsertBlock()->getTerminator();
		if (last_instr2 == nullptr || std::string("ret") != last_instr2->getOpcodeName())
		{
			sBuilder.CreateBr(merge_block);
		}
		else_block = sBuilder.GetInsertBlock();


		func->getBasicBlockList().push_back(merge_block);
		sBuilder.SetInsertPoint(merge_block);

		return nullptr;
	}

	llvm::Value * ASTWhileState::generate(std::vector<std::string> var_names, llvm::Value * var_vals)
	{
		llvm::Function* func = sBuilder.GetInsertBlock()->getParent();

		llvm::BasicBlock* check_block = llvm::BasicBlock::Create(sContext, "while check", func);
		llvm::BasicBlock* body_block = llvm::BasicBlock::Create(sContext, "while body", func);
		llvm::BasicBlock* merge_block = llvm::BasicBlock::Create(sContext, "merge", func);

		sBuilder.CreateBr(check_block);

		sBuilder.SetInsertPoint(check_block);
		auto condition = expr->generate(var_names, var_vals);
		sBuilder.CreateCondBr(condition, body_block, merge_block);


		sBuilder.SetInsertPoint(body_block);
		for (auto& statement :	state) {
			statement->generate(var_names, var_vals);
		}
		sBuilder.CreateBr(check_block);


		sBuilder.SetInsertPoint(merge_block);
		return nullptr;
	}

	llvm::Value * ASTState::generate(std::vector<std::string> var_names, llvm::Value * var_vals)
	{
		if (retexpr) 
		{
			return retexpr->generate(var_names, var_vals);
		} 
		else if (vdc)
		{
			return vdc->generate(var_names, var_vals);
		} 
		else if (expr) 
		{
			return expr->generate(var_names, var_vals);
		} 
		else if (ws) 
		{
			return ws->generate(var_names, var_vals);
		} 
		else if (ss) 
		{
			return ss->generate(var_names, var_vals);
		} 
		else if (thread) 
		{
			return thread->generate(var_names, var_vals);
		}
		return nullptr;
	}

	static llvm::AllocaInst* CreateEntryBlockAlloca(llvm::Function* func, const std::string& var_name) 
	{
		llvm::IRBuilder<> idk(&func->getEntryBlock(), func->getEntryBlock().begin());
		return idk.CreateAlloca(llvm::Type::getInt64Ty(sContext), 0, var_name.c_str());
	}

	llvm::Value * ASTFnDecl::generate(std::vector<std::string> var_names, llvm::Value * var_vals)
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
			std::string func_name = func->getName();
			std::string full_name = func_name + std::string(arg.getName());
			sLocals[full_name] = alloc;
		}
		for (auto state : body) 
		{
			state->generate(var_names, var_vals);
		}

		return func;
	}

	llvm::Value* ASTRoot::generate(std::vector<std::string> var_names, llvm::Value * var_vals) 
	{
		for (auto& func : funcs) 
		{
			func->generate(var_names, var_vals);
		}
		return nullptr;
	}

	llvm::Value * ASTString::generate(std::vector<std::string> var_names, llvm::Value * var_vals)
	{
		return sBuilder.CreateGlobalStringPtr(str);
		//return llvm::ConstantDataArray::getString(sContext, llvm::StringRef(str));
	}

	llvm::Value * ASTLambdaThread::generate(std::vector<std::string> var_names, llvm::Value * var_vals) 
	{
		auto old_block = sBuilder.GetInsertBlock();

		std::string secret_func_name = "xx___secrete_FuncTion__";
		std::vector<llvm::Type *> anon_func_types_vec;
		anon_func_types_vec.push_back(llvm::Type::getInt8Ty(sContext)->getPointerTo());
		llvm::FunctionType *anon_func_type = llvm::FunctionType::get(llvm::Type::getInt64Ty(sContext), anon_func_types_vec, false);
		
		
		//doing locking of varialbes and barirers stuff
		auto var_names = get_outofcontext_vars(getParent());
		auto vars = sBuilder.CreateAlloca(llvm::Type::getInt8Ty(sContext)->getPointerTo(), llvm::ConstantInt::get(sContext, llvm::APInt(8, var_names.size())));
		auto locks = sBuilder.CreateAlloca(llvm::Type::getInt8Ty(sContext)->getPointerTo(), llvm::ConstantInt::get(sContext, llvm::APInt(8, var_names.size())));

		auto barrier_lock = sBuilder.CreateAlloca(llvm::Type::getInt8Ty(sContext)->getPointerTo(), llvm::ConstantInt::get(sContext, llvm::APInt(8, 1)));
		auto barrier_count = sBuilder.CreateAlloca(llvm::Type::getInt8Ty(sContext)->getPointerTo(), llvm::ConstantInt::get(sContext, llvm::APInt(8, 1)));

		auto params = sBuilder.CreateAlloca(llvm::Type::getInt8Ty(sContext)->getPointerTo()->getPointerTo(), llvm::ConstantInt::get(sContext, llvm::APInt(8, 1)));
		
		//created the function
		llvm::Function* secret_func = llvm::Function::Create(anon_func_type, llvm::Function::ExternalLinkage, secret_func_name, sModule.get());
		auto fn_param = ((llvm::Value*)secret_func->args_begin)
		//creatirg fntuctiuon bady
		llvm::BasicBlock *block = llvm::BasicBlock::Create(sContext, "entry point", secret_func);
		sBuilder.SetInsertPoint(block);
		for (auto &s : this->state) 
		{
			s->generate(var_names, fn_param);
		}
		sBuilder.CreateRet(llvm::ConstantInt::get(sContext, llvm::APInt(64, 0)));

		sBuilder.SetInsertPoint(old_block);

		std::string secret_var = "__secret_variable";

		long zero = 0;
		ASTVarDecl* secret_init = new ASTVarDecl(secret_var, zero);
		secret_init->generate(var_names, var_vals);

		//ARY, always repeat yourself
		auto params0 = sBuilder.CreateGEP(params, llvm::ConstantInt::get(sContext, llvm::APInt(8, 0)));
		auto params1 = sBuilder.CreateGEP(params, llvm::ConstantInt::get(sContext, llvm::APInt(8, 1)));
		auto params2 = sBuilder.CreateGEP(params, llvm::ConstantInt::get(sContext, llvm::APInt(8, 2)));
		auto params3 = sBuilder.CreateGEP(params, llvm::ConstantInt::get(sContext, llvm::APInt(8, 3)));

		auto load0 = sBuilder.CreateStore(vars, params0);
		auto load1 = sBuilder.CreateStore(locks, params1);
		auto load2 = sBuilder.CreateStore(barrier_lock, params2);
		auto load3 = sBuilder.CreateStore(barrier_count, params3);



		auto left = ASTExpr::make_var(secret_var);
		auto right = expr;
		auto cond = ASTExpr::make_binop(left, "<", right);

		std::vector<ASTState*> loop_statements;

		CloneCall* cc = new CloneCall(secret_func_name, params);
		cc->setParent(this);
		auto clone_state = new ASTState(cc);
		loop_statements.push_back(clone_state); //change this later

		auto inc = ASTExpr::make_plus_plus(secret_var);
		ASTVarDecl *dec = new ASTVarDecl(secret_var, inc);

		auto state2 = new ASTState(dec);

		loop_statements.push_back(state2);

		ASTWhileState while_statement(cond, loop_statements);
		while_statement.generate();
	}

	llvm::Value * ASTBinOp::generate(std::vector<std::string> var_names, llvm::Value * var_vals)
	{
		//literally nothig
		return nullptr;
	}

	llvm::Value* CloneCall::generate(std::vector<std::string> var_names, llvm::Value * var_vals) 
	{ 

		llvm::Function* clone_func = sModule->getFunction("clone");
		llvm::Function* secret_func = sModule->getFunction(function_name);
		if (clone_func == nullptr) 
		{

			std::vector<llvm::Type *> anon_func_types_vec;
			anon_func_types_vec.push_back(llvm::Type::getInt8Ty(sContext)->getPointerTo());
			llvm::FunctionType *anon_func_type = llvm::FunctionType::get(llvm::Type::getInt64Ty(sContext), anon_func_types_vec, false);
			

			std::vector<llvm::Type *> clone_types_vec;
			clone_types_vec.push_back(anon_func_type->getPointerTo());
			clone_types_vec.push_back(sBuilder.getInt8Ty()->getPointerTo());
			clone_types_vec.push_back(sBuilder.getInt32Ty());
			clone_types_vec.push_back(sBuilder.getInt8Ty()->getPointerTo());

			llvm::FunctionType *clone_type = llvm::FunctionType::get(sBuilder.getInt32Ty(), clone_types_vec, true);
			clone_func = llvm::Function::Create(clone_type, llvm::Function::ExternalLinkage, "clone", sModule.get());
		}
		std::vector<llvm::Value*> clone_arguments;

		clone_arguments.push_back(secret_func);

		std::vector<llvm::Value*> calloc_arguments;
		calloc_arguments.push_back(llvm::ConstantInt::get(sContext, llvm::APInt(64, (uint64_t) 1024 * 8)));
		calloc_arguments.push_back(llvm::ConstantInt::get(sContext, llvm::APInt(64, 1)));

		llvm::Function* calloc_func = sModule->getFunction("calloc");
		if (calloc_func == nullptr) 
		{
			std::vector<llvm::Type *> calloc_types;
			calloc_types.push_back(sBuilder.getInt64Ty());
			calloc_types.push_back(sBuilder.getInt64Ty());
			llvm::FunctionType *calloc_type = llvm::FunctionType::get(sBuilder.getInt8Ty()->getPointerTo(), calloc_types, true);
			calloc_func = llvm::Function::Create(calloc_type, llvm::Function::ExternalLinkage, "calloc", sModule.get());
		}
		auto calloc_ptr = sBuilder.CreateCall(calloc_func, calloc_arguments, "clalling calloc");			
		auto index = llvm::ConstantInt::get(sContext, llvm::APInt(64, 1024 * 8 - 1));
		auto stack_top = sBuilder.CreateGEP(calloc_ptr, index);
		//create the gep arguments
		clone_arguments.push_back(stack_top);
		clone_arguments.push_back(llvm::ConstantInt::get(sContext, llvm::APInt(32, 256)));
		clone_arguments.push_back(llvm::Constant::getNullValue(sBuilder.getInt8Ty()->getPointerTo()));

		return sBuilder.CreateCall(clone_func, clone_arguments, "clalling clone");
	}


}
