#include <iostream>
#include <algorithm>
#include <stack>
#include <unordered_map>
#include <cstdlib>

#include "parser.h"
#include "lexerUtils.h"

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
#define IS_EOF(stream, start) (start >=	(int)stream.size())
#define ASSERT_BOUNDS(stream, start) if(start >= (int)stream.size()) return stream.size(); else if(start == -1) return -1;
#define RESET(obj) delete obj; obj = nullptr;
namespace naruto
{	
	llvm::LLVMContext sContext;
	llvm::IRBuilder<> sBuilder(sContext);
	std::unique_ptr<llvm::Module> sModule;
	std::map<std::string, llvm::AllocaInst*> sLocals;
	
	//this only works with objects with virtual funcs,
	//sorry. also this is super hacky and totally dangerous, do not attempt
	template <class T>
	bool is_of_type(void * obj)
	{
		T temp;
		long type_vtable = *reinterpret_cast<long*>(&temp);
		long obj_vtable = *reinterpret_cast<long*>(obj);
		return type_vtable == obj_vtable;
	}
	
	std::vector<ASTVarDecl*> get_vardecls(ASTNode * node, ASTNode * toMatch)
	{
		if(is_of_type<ASTState>(node)) {
			std::cout << "We are a state" << std::endl;
			ASTNode * scope = ((ASTState*)node)->getState();
			if(is_of_type<ASTSelState>(scope))
			{
				std::cout << "We are sel" << std::endl;
				std::vector<ASTVarDecl*> vdecs;
				bool correct_body = false;
				for(auto s : ((ASTSelState*)scope)->getIfBody())
				{
					if(toMatch == s)
						correct_body = true;
					ASTNode * state = s->getState();
					if(is_of_type<ASTVarDecl>(state)) 
					{
						vdecs.push_back((ASTVarDecl*)state);
					}
				}
				if(correct_body)
					return vdecs;
				vdecs = std::vector<ASTVarDecl*>();
				for(auto s : ((ASTSelState*)scope)->getElseBody())
				{
					if(toMatch == s)
						correct_body = true;
					ASTNode * state = s->getState();
					if(is_of_type<ASTVarDecl>(state)) 
					{
						vdecs.push_back((ASTVarDecl*)state);
					}
				}
				if(correct_body)
					return vdecs;
			}
			else if(is_of_type<ASTWhileState>(scope))
			{
				std::cout << "We are while" << std::endl;
				std::vector<ASTVarDecl*> vdecs;
				bool correct_body = false;
				for(auto s : ((ASTWhileState*)scope)->getState())
				{
					if(toMatch == s)
						correct_body = true;
					ASTNode * state = s->getState();
					if(is_of_type<ASTVarDecl>(state)) 
					{
						vdecs.push_back((ASTVarDecl*)state);
					}
				}
				if(correct_body)
					return vdecs;
			}
			else if(is_of_type<ASTLambdaThread>(scope))
			{
				std::cout << "We are thread" << std::endl;
				std::vector<ASTVarDecl*> vdecs;
				bool correct_body = false;
				for(auto s : ((ASTLambdaThread*)scope)->getState())
				{
					if(toMatch == s)
						correct_body = true;
					ASTNode * state = s->getState();
					if(is_of_type<ASTVarDecl>(state)) 
					{
						vdecs.push_back((ASTVarDecl*)state);
					}
				}
				if(correct_body)
					return vdecs;
			}
		}
		else if(is_of_type<ASTFnDecl>(node))
		{
			std::cout << "We are a func" << std::endl;
			std::vector<ASTVarDecl*> vdecs;
			bool correct_body = false;
			for(auto s : ((ASTFnDecl*)node)->getBody())
			{
				if(toMatch == s)
					correct_body = true;
				ASTNode * state = s->getState();
				if(is_of_type<ASTVarDecl>(state)) 
				{
					vdecs.push_back((ASTVarDecl*)state);
				}
			}
			if(correct_body)
				return vdecs;
		}
		else if(is_of_type<ASTRoot>(node))
		{
			std::cout << "We are the root" << std::endl;
			std::vector<ASTVarDecl*> vdecs;
			bool correct_body = false;
			for(auto s : ((ASTRoot*)node)->getGlobals())
			{
				vdecs.push_back(s);
			}
			if(correct_body)
				return vdecs;
		}
		return std::vector<ASTVarDecl*>();
	}
	
	std::vector<std::string> get_outofcontext_vars(ASTNode * subroot)
	{
		std::cout << "gov called" << std::endl;
		std::unordered_map<std::string, ASTVarDecl*> mvdecs;
		std::vector<std::string> vdecs;
		
		//then get the var decls of the greater scope
		ASTNode * last = subroot;
		subroot = subroot->getParent();
		while(subroot)
		{
			for(auto v : get_vardecls(subroot, last))
			{
				std::cout << "get_vardecls: " << v << std::endl;
				if(mvdecs.find(v->getIden()) == mvdecs.end())
				{
					mvdecs[v->getIden()] = v;
					vdecs.push_back(v->getIden());
				}
			}
			last = subroot;
			subroot = subroot->getParent();
		}
		return vdecs;
	}

	int skip_paren(stream_t &stream, int start)
	{
		int paren_pos = start;
		int stack = 0;
		do
		{
			//if we reach the EOF before we find the end parens
			//then parsing definitely failed
			if(IS_EOF(stream, paren_pos))
				return -1;
			if(stream[paren_pos].isParenOpen()) stack++;
			if(stream[paren_pos].isParenClose()) stack--;
			paren_pos++;
		}
		while(stack);
		return paren_pos;
	}

	int stream_pos;
	std::vector<std::vector<std::string> > precedence;

	bool init_precedence() 
	{
		//precedence = std::vector<std::vector<std::string> >();
		precedence.push_back(std::vector<std::string>());
		precedence[0].push_back(std::string("&&"));
		precedence[0].push_back(std::string("||"));
		precedence.push_back(std::vector<std::string>());
		precedence[1].push_back(std::string("<"));
		precedence[1].push_back(std::string(">"));
		precedence[1].push_back(std::string(">="));
		precedence[1].push_back(std::string("<="));
		precedence[1].push_back(std::string("=="));
		precedence[1].push_back(std::string("!="));
		precedence.push_back(std::vector<std::string>());
		precedence[2].push_back(std::string("+"));
		precedence[2].push_back(std::string("-"));
		precedence.push_back(std::vector<std::string>());
		precedence[3].push_back(std::string("*"));
		precedence[3].push_back(std::string("/"));
		precedence[3].push_back(std::string("%"));
		precedence[3].push_back(std::string(">>"));
		precedence[3].push_back(std::string("<<"));
		return true;
	}

	bool init_result = init_precedence();

	int ASTBinOp::parse(stream_t &stream, int start)
	{
		ASSERT_BOUNDS(stream, start);
		if(stream[start].isOp())
		{
			op = stream[start].info;
			return start + 1;
		}
		else
		{
			return -1;
		}
	}
	
	int ASTIden::parse(stream_t &stream, int start)
	{
		ASSERT_BOUNDS(stream, start);
		if(stream[start].isIden())
		{
			iden = stream[start].info;
			if(start+1 < stream.size()) 
			{
				if(stream[start+1].isSan()) 
				{
					type = "int";
					return start + 2;
				}
				else if(stream[start+1].isKun()) 
				{
					type = "float";
					return start + 2;
				}
				else if(stream[start+1].isSama()) 
				{
					type = "string";
					return start + 2;
				}
				else
				{
					return start + 1;
				}
			}
			else
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}
	
	int ASTInt::parse(stream_t &stream, int start)
	{
		ASSERT_BOUNDS(stream, start);
		if(stream[start].isIntVal())
		{
			val = stream[start].int_val;
			return start + 1;
		}
		else
		{
			return -1;
		}	
	}	

	int ASTFloat::parse(stream_t &stream, int start)
	{
		ASSERT_BOUNDS(stream, start);
		if(stream[start].isFloatVal())
		{
			val = stream[start].float_val;
			return start + 1;
		}
		else
		{
			return -1;
		}	
	}
	
	int ASTString::parse(stream_t &stream, int start)
	{
		ASSERT_BOUNDS(stream, start);
		if(stream[start].isStrVal())
		{
			str = stream[start].info.substr(1, stream[start].info.length()-2);
			return start + 1;
		}
		else
		{
			return -1;
		}	
	}
	//returns the index of the thing right after the fn call
	int ASTFnCall::get_end_fn_call(stream_t &stream, int start)
	{
		if(stream[start].isIden())
		{
			start++;
			if(start >= (int)stream.size())
			{
				return -1;
			}
			else if(stream[start].isColon())
			{
				for(; start < (int)stream.size(); start++)
				{
					if(stream[start].isIden())
					{
						if(start+1 >= (int)stream.size())
						{
							return -1;
						}
						if(stream[start+1].isColon())
						{
							start = get_end_fn_call(stream, start);
						}
					}
					if(stream[start].isNoJutsu())
					{
						return start+1;
					}
				}
				return -1;
			}
		}
		return -1;
	}
	
	bool ASTFnCall::is_fn_call(stream_t &stream, int start)
	{
		return get_end_fn_call(stream, start) != -1;
	}

	int ASTFnCall::parse(stream_t &stream, int start)
	{
		if(stream[start].isIden() && stream[start+1].isColon())
		{
			iden = new ASTIden();
			start = iden->parse(stream, start);
			iden->setParent(this);
			int next_pos = start+1;
			while(true)
			{
				if(next_pos >= (int)stream.size())
				{
					return -1;
				}
				else if(stream[next_pos].isNoJutsu())
				{
					break;
				}
				else
				{
					ASTExpr * expr = new ASTExpr();
					next_pos = expr->parse(stream, next_pos);
					expr->setParent(this);
					params.push_back(expr);
				}
			}
			return next_pos+1;
		}
		else
		{
			return -1;
		}
	}	

	int ASTExpr::find_end_expression(stream_t &stream, int start)
	{
		int next_pos = start;
		while(true)
		{
			if(next_pos >= (int)stream.size())
				return stream.size();
			if(is_end_expression(stream, next_pos))
				break;

			if(stream[next_pos].isParenOpen())
			{
				int paren_pos = skip_paren(stream, next_pos);
				if(paren_pos != -1)
					next_pos = paren_pos;
				else
					return -1;
			}
			else if(ASTFnCall::is_fn_call(stream, next_pos))
			{
				int fn_end = ASTFnCall::get_end_fn_call(stream, next_pos);
				next_pos = fn_end;
			}
			else
			{
				next_pos++;
			}
		}

		if(stream[next_pos].isParenOpen())
		{
			int paren_pos = skip_paren(stream, next_pos);
			if(paren_pos != -1)
				next_pos = paren_pos;
			else
				return -1;
		}
		else if(ASTFnCall::is_fn_call(stream, next_pos))
		{
			
			int fn_end = ASTFnCall::get_end_fn_call(stream, next_pos);
			next_pos = fn_end;
		}
		else
		{
			next_pos++;
		}
		return next_pos;
	}
	
	bool ASTExpr::isInTree(std::string query)
	{
		if(iden && iden->getIden() == query)
			return true;
		bool res = false;
		if(rhs)
			res = res || rhs->isInTree(query);
		if(lhs)
			res =  res || lhs->isInTree(query);
		return res;
	}
	
	bool ASTExpr::is_end_expression(stream_t &stream, int start)
	{
		if(start >= (int)(stream.size() - 1))
		{
			return true;
		}
		int fn_end = ASTFnCall::get_end_fn_call(stream, start);

		if(fn_end >= (int)stream.size())
		{
			return true;
		}
		else if(fn_end != -1) 
		{
			if(stream[fn_end].isVal() |
			stream[fn_end].isNoJutsu() |
			stream[fn_end].isDesu() |
			stream[fn_end].isParenOpen() |
			ASTFnCall::is_fn_call(stream, fn_end) |
			stream[fn_end].isIden() |
			stream[fn_end].isChan() |
			stream[fn_end].isDelim() |
			stream[fn_end].isStrVal())
			{
				return true;
			}
		}
		else
		{
			if(stream[start].isIden() | stream[start].isParenOpen() | stream[start].isVal())
			{
				if(stream[start].isParenOpen())
				{
					int paren_pos = skip_paren(stream, start);
					
					if(paren_pos != -1)
					{
						start = paren_pos;
					}
					else
					{
						return -1;
					}
				}
				else if(ASTFnCall::is_fn_call(stream, start))
				{	
					int fn_end = ASTFnCall::get_end_fn_call(stream, start);
					start = fn_end;
				}
				else
				{
					start++;
				}

				if(start > (int)stream.size())
				{
					return true;
				}

				if(stream[start].isNoJutsu() |
				stream[start].isChan() |
				stream[start].isParenOpen() |
				stream[start].isIden() |
				stream[start].isDesu() |
				stream[start].isDelim() |
				ASTFnCall::is_fn_call(stream, start) |
				stream[start].isVal())
				{
					return true;
				}
			}
		}
		return false;
	}	

	//this function is for parsing the things that are to the right and left of the highest
	//precedence opator
	int ASTExpr::parse_operand(stream_t &stream, int start)
	{
		//first try checking for a function call. 
		if(ASTFnCall::is_fn_call(stream, start))
		{
			call = new ASTFnCall();
			call->setParent(this);
			return call->parse(stream, start);
		}
		if(stream[start].isParenOpen())
		{
			//if its parentheses, treat it like a new expression with precedence reset
			int end = skip_paren(stream, start);
			return parse_lvl(stream, start+1, end-1, precedence, 0);
		}
		else if(stream[start].isIden())
		{
			iden = new ASTIden();
			iden->setParent(this);
			return iden->parse(stream, start);
		}
		if(stream[start].isIntVal())
		{
			int_v = new ASTInt();
			int_v->setParent(this);
			return int_v->parse(stream, start);
		}
		else if(stream[start].isFloatVal())
		{
			flt = new ASTFloat();
			flt->setParent(this);
			return flt->parse(stream, start);
		}
		else if(stream[start].isStrVal())
		{
			str = new ASTString();
			str->setParent(this);
			return str->parse(stream, start);
		}
		return -1;
	}	

	int ASTExpr::parse_lvl(stream_t &stream, int start, int end, std::vector<std::vector<std::string> > delim, int level)
	{
		//find the highest level operator
		if(level >= delim.size())
		{
			level = (int)delim.size() -1;
		}
		int next_pos = start;
		while(true)
		{

			if(next_pos >= end) 
			{
				break;
			}

			bool found = 0;
			for(auto o : delim[level])
			{
				if(stream[next_pos].info == o) found = 1;
			}

			int one_after_start = start;
			if(stream[one_after_start].isParenOpen())
			{
				int paren_pos = skip_paren(stream, one_after_start);
				if(paren_pos != -1)
					one_after_start = paren_pos;
				else
					return -1;
			}
			else if(ASTFnCall::is_fn_call(stream, one_after_start))
			{
				int fn_end = ASTFnCall::get_end_fn_call(stream, one_after_start);
				one_after_start = fn_end;
			}
			else 
			{
				one_after_start++;
			}

			//if we find one on this level, then we should parse to the right and left of it
			if(found)
			{
				lhs = new ASTExpr();
				lhs->setParent(this);
				lhs->parse_lvl(stream, start, next_pos, precedence, level+1);
				op = new ASTBinOp();
				op->setParent(this);
				op->parse(stream, next_pos);
				rhs = new ASTExpr();
				rhs->setParent(this);
				rhs->parse_lvl(stream, next_pos+1, end, precedence, level);
				return end;
			}
			else if(one_after_start >= end || IS_EOF(stream, one_after_start))
			{
				lhs = new ASTExpr();
				lhs->setParent(this);
				lhs->parse_operand(stream, start);
				return end;	
			}

			//get the next position accordingly
			if(stream[next_pos].isParenOpen())
			{
				int paren_pos = skip_paren(stream, next_pos);
				if(paren_pos != -1)
					next_pos = paren_pos;
				else
					return -1;
			}
			else if(ASTFnCall::is_fn_call(stream, next_pos))
			{
				int fn_end = ASTFnCall::get_end_fn_call(stream, next_pos);
				next_pos = fn_end;
			}
			else
			{
				next_pos++;
			}
		}
		if(level == (((int)delim.size()) - 1))
		{
			return end;
		}
		else
		{
			return parse_lvl(stream, start, end, precedence, level+1);
		}
	}

	int ASTExpr::parse(stream_t &stream, int start)
	{
		int end = find_end_expression(stream, start);
		return parse_lvl(stream, start, end, precedence, 0);
	}

	int ASTRetExpr::parse(stream_t &stream, int start)
	{
		if(stream[start].isSayonara())
		{
			start++;
			if(!stream[start].isDelim())
			{
				int end = ASTExpr::find_end_expression(stream, start);
				if(!stream[end].isChan())
				{
					return -1;
				}
				
				expr = new ASTExpr();
				expr->setParent(this);
				start = expr->parse(stream, start);
				return start+2; //+2 for chan and ~
			}
			return start;
		}
		return -1;
	}

	int ASTVarDecl::parse(stream_t &stream, int start)
	{
		if(stream[start].isIden())
		{
			name = new ASTIden();
			name->setParent(this);
			start = name->parse(stream, start);
			if(stream[start].isWa()) start++;
			else return -1;
			int end = ASTExpr::find_end_expression(stream, start);
			if(stream[end].isDesu())
			{
				val = new ASTExpr();
				val->setParent(this);
				start = val->parse(stream, start);
				start++;
				if(stream[start].isDelim())
				{
					return start+1;
				}
			}	
		}
		return -1;
	}

	int ASTSelState::parse(stream_t &stream, int start)
	{
		if(stream[start].isNani()) 
		{
			start++;
			expr = new ASTExpr();
			expr->setParent(this);
			start = expr->parse(stream, start);
			if(stream[start].isDelim())
			{
				start++;
				while(!stream[start].isBaka() && !stream[start].isIfDelim()) 
				{
					ASTState * statement = new ASTState();
					statement->setParent(this);
					start = statement->parse(stream, start);
					if_body.push_back(statement);
				}
				if(stream[start].isBaka())
				{
					start++;
					if(stream[start].isNani())
					{
						elif = new ASTSelState();
						elif->setParent(this);
						start = elif->parse(stream, start);
						return start;
					}
					else
					{
						while(!stream[start].isIfDelim()) 
						{
							ASTState * statement = new ASTState();
							statement->setParent(this);
							start = statement->parse(stream, start);
							else_body.push_back(statement);
						}
						return start +1;
					}
				}
				else if(stream[start].isIfDelim())
				{
					return start+1;
				}
			}
			else
			{
				delete expr; expr = nullptr;
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}

	int ASTWhileState::parse(stream_t &stream, int start)
	{
		if(stream[start].isDoki()) 
		{
			start++;
			expr = new ASTExpr();
			expr->setParent(this);
			start = expr->parse(stream, start);
			if(stream[start].isDelim())
			{
				start++;
				while(start < stream.size() && !stream[start].isWhileDelim())
				{
					ASTState * statement = new ASTState();
					statement->setParent(this);
					start = statement->parse(stream, start);
					state.push_back(statement);
				}
				if(start >= stream.size())
				{
					return -1;
				}
				return start+1;
			}		
		}
		else
		{
			return -1;
		}
	}
	
	int ASTLambdaThread::parse(stream_t &stream, int start)
	{
		if(stream[start].isShadowCloneJutsu()) 
		{
			start++;
			expr = new ASTExpr();
			expr->setParent(this);
			start = expr->parse(stream, start);
			if(stream[start].isDelim())
			{
				start++;
				while(start < stream.size() && !stream[start].isThreadDelim())
				{
					ASTState * statement = new ASTState();
					statement->setParent(this);
					start = statement->parse(stream, start);
					state.push_back(statement);
				}
				if(start >= stream.size())
				{
					return -1;
				}
				return start+1;
			}
		}
		else
		{
			return -1;
		}
	}

	int ASTState::parse(stream_t &stream, int start)
	{
		if(stream[start].isNani()) 
		{
			ss = new ASTSelState();
			ss->setParent(this);
			return ss->parse(stream, start);
		}
		else if(stream[start].isDoki())
		{
			ws = new ASTWhileState();
			ws->setParent(this);
			return ws->parse(stream, start);
		}
		else if(stream[start].isSayonara()) 
		{
			retexpr = new ASTRetExpr();
			retexpr->setParent(this);
			return retexpr->parse(stream, start);
		}
		else if(stream[start].isShadowCloneJutsu()) 
		{
			thread = new ASTLambdaThread();
			thread->setParent(this);
			return thread->parse(stream, start);
		}
		else if(stream[start].isIden() && 
		(	(start+1 < stream.size() && stream[start+1].isWa()) 
		|| 	(start+2 < stream.size() && stream[start+2].isWa())))
		{
			vdc = new ASTVarDecl();
			vdc->setParent(this);
			
			return vdc->parse(stream, start);
		}
		else
		{
			expr = new ASTExpr();
			expr->setParent(this);
			int end = expr->parse(stream, start);
			if(end == -1)
				std::cout << "PARSING EXPR AS STATE RETURNED -1" << std::endl;
			return end +1;
		}
	}

	int ASTFnDecl::parse(stream_t &stream, int start)
	{
		if(stream[start].isIden())
		{
			name = new ASTIden();
			name->setParent(this);
			start = name->parse(stream, start);
			if(stream[start].isColon()) start++;
			else return -1; //fail for now, this is how grammar has it
			while(!stream[start].isNoJutsu()) 
			{
				ASTIden * param = new ASTIden();
				param->setParent(this);
				start = param->parse(stream, start);
				params.push_back(param);
			}
			if(stream[start].isNoJutsu()) start++;
			else return -1;
			if(stream[start].isDelim()) start++;
			else return -1;
			while(start < (int)stream.size() && !stream[start].isFnDelim())
			{
				ASTState * statement = new ASTState();
				statement->setParent(this);
				start = statement->parse(stream, start);
				body.push_back(statement);
			}
			return start+1;
		}
		else
		{
			return -1;
		}
	}

	int ASTRoot::parse(stream_t &stream, int start)
	{
		while(start < stream.size()) 
		{
			if(start+1 < stream.size() && stream[start].isIden() && stream[start+1].isWa())
			{
				ASTVarDecl * vdc = new ASTVarDecl();
				vdc->setParent(this);
				start = vdc->parse(stream, start);
				globals.push_back(vdc);
			}
			else if(stream[start].isIden())
			{
				ASTFnDecl * func = new ASTFnDecl();
				func->setParent(this);
				start = func->parse(stream, start);
				funcs.push_back(func);
			}
			else
			{
				return -1;	
			}
		}
		return start;
	}

	//all the print functions
	void ASTBinOp::print()
	{
		std::cout << " " << op << " ";
	}
	
	void ASTString::print()
	{
		std::cout << " " << str << " ";
	}

	void ASTIden::print()
	{
		std::cout << " " << type << " : " << iden << " ";
	}
	
	void ASTInt::print()
	{
		std::cout << " " << val << " ";
	}

	void ASTFloat::print()
	{
		std::cout << " " << val << " ";	
	}

	void ASTFnCall::print()
	{
		std::cout << " (FNCALL ";
		if(iden) iden->print();
		for(auto p : params) p->print();
		std::cout << " ENDFNCALL)";
	}

	void ASTExpr::print()
	{
		std::cout << " ( ";
		if(lhs) lhs->print();
		if(op) op->print();
		if(rhs) rhs->print();

		if(iden) iden->print();
		if(flt) flt->print();
		if(int_v) int_v->print();
		if(call) call->print();
		if(str) str->print();
		std::cout << " )";
	}

	void ASTRetExpr::print()
	{
		std::cout << "RETURN";
		if(expr) expr->print();	else std::cout << "NULL";
	}

	void ASTVarDecl::print()
	{
		if(name) name->print(); else std::cout << "NULL";
		std::cout << "IS";
		if(val) val->print(); else std::cout << "NULL";
	}

	void ASTSelState::print()
	{
		std::cout << "IF ";
		if(expr) expr->print(); else std::cout << "NULL";
		std::cout << std::endl;
		for(auto s : if_body)
		{
			s->print(); std::cout << std::endl;
		}
		std::cout << "ELSE " << std::endl;
		if(elif) elif->print();
		for(auto s : else_body)
		{
			s->print(); std::cout << std::endl;
		}

		std::cout << "END IF" << std::endl;
	}

	void ASTWhileState::print()
	{
		std::cout << "WHILE ";
		if(expr) expr->print(); else std::cout << "NULL";
		std::cout << std::endl;
		for(auto s : state)
		{
			s->print(); std::cout << std::endl;
		}
		std::cout << "END WHILE ";
	}
	
	void ASTLambdaThread::print()
	{
		std::cout << "THREAD ";
		if(expr) expr->print(); else std::cout << "NULL";
		std::cout << std::endl;
		for(auto s : state)
		{
			s->print(); std::cout << std::endl;
		}
		std::cout << "END THREAD ";
	}

	void ASTState::print()
	{
		if(ws) ws->print();	
		if(ss) ss->print();	
		if(expr) expr->print();	
		if(retexpr) retexpr->print();	
		if(vdc) vdc->print();	
		if(thread) thread->print();
	}

	void ASTFnDecl::print()
	{
		std::cout << "FN ";
		if(name) name->print(); else std::cout << "NULL" << std::endl;
		for(auto p : params)
		{
			//std::cout << " " << std::endl;
			p->print();
		}
		std::cout << " : FN BODY" << std::endl;
		for(auto s : body)
		{
			s->print();
			std::cout << std::endl;
		}
		std::cout << " " << std::endl;
	}

	void ASTRoot::print()
	{
		for(auto v : globals)
		{
			v->print();
			std::cout << std::endl;
		}
		for(auto f : funcs)
		{
			f->print();
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
}
