#include <iostream>
#include <algorithm>
#include <stack>
#include "parser.h"
#include "lexerUtils.h"

#define IS_EOF(stream, start) (start >=  stream.size())
#define ASSERT_BOUNDS(stream, start) if(start >= stream.size()) return stream.size(); else if(start == -1) return -1;
#define RESET(obj) delete obj; obj = nullptr;
namespace naruto
{
	int stream_pos;
	std::vector<std::vector<std::string> > precedence;

	bool init_precedence() {
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
		if(stream[start].code == TokenCodes::bin_op)
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
		if(stream[start].code == TokenCodes::identifier)
		{
			iden = stream[start].info;
			return start + 1;
		}
		else
		{
			return -1;
		}
	}
	
	int ASTInt::parse(stream_t &stream, int start)
	{
		ASSERT_BOUNDS(stream, start);
		if(stream[start].code == TokenCodes::int_val)
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
		if(stream[start].code == TokenCodes::float_val)
		{
			val = stream[start].float_val;
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
			if(start >= stream.size())
				return -1;
			if(stream[start].code == TokenCodes::no_jutsu)
			{
				return start+1;
			}
			else if(stream[start].isColon())
			{
				for(; start < stream.size(); start++)
				{
					if(stream[start].isIden())
					{
						start++;
						if(start >= stream.size())
							return -1;
						if(stream[start].isColon())
						{
							start = get_end_fn_call(stream, start-1);
						}
					}
					else if(stream[start].code == TokenCodes::no_jutsu)
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
		return -1;
	}	

	int ASTExpr::find_end_expression(stream_t &stream, int start)
	{
		return -1;
	}
	
	bool ASTExpr::is_end_expression(stream_t &stream, int start)
	{
		int fn_end = ASTFnCall::get_end_fn_call(stream, start);
		if(fn_end != -1) {
			if(stream[fn_end].isKeyword() |
			stream[fn_end].isParenOpen() |
			stream[fn_end].isIden() |
			stream[fn_end].isDelim())
				return true;
		}
		else
		{
			if(stream[start].isIden() | stream[start].isParenClose())
			{
				start++;
				if(start > stream.size())
					return true;;
				if(stream[start].isKeyword() |
				stream[start].isParenOpen() |
				stream[start].isIden() |
				stream[start].isDelim())
					return true;
			}
		}
		return false;
	}
	
	int skip_paren(stream_t &stream, int start)
	{
		int paren_pos = start;
		int stack = 1;
		while(stack)
		{
			//if we reach the EOF before we find the end parens
			//then parsing definitely failed
			if(IS_EOF(stream, paren_pos))
				return -1;
			if(stream[paren_pos].isParenOpen()) stack++;
			if(stream[paren_pos].isParenClose()) stack--;
			paren_pos++;
		}
		return paren_pos;
	}

	//this function is for parsing the things that are to the right and left of the highest
	//precedence opator
	int ASTExpr::parse_operand(stream_t &stream, int start)
	{
		//first try checking for a function call. 
		if(ASTFnCall::is_fn_call(stream, start))
		{
			call = new ASTFnCall();
			call->parse(stream, start);
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
			return iden->parse(stream, start);
		}
		else if(stream[start].isVal())
		{
			if(stream[start].code == TokenCodes::int_val)
			{
				int_v = new ASTInt();
				return int_v->parse(stream, start);
			}
			else
			{
				flt = new ASTFloat();
				return flt->parse(stream, start);
			}
		}
		return -1;
	}	

	int ASTExpr::parse_lvl(stream_t &stream, int start, int end, std::vector<std::vector<std::string> > delim, int level)
	{
		if(level >= delim.size()) level = delim.size() - 1;
		//find the highest level operator
		int next_pos = start;
		while(true)
		{
			//std::cout << "Next Pos: " << next_pos << " | End: " << end << std::endl;
			if(next_pos == end) break;
			bool found = 0;
			//std::cout << "Start compare for iter" << std::endl;
			std::cout << "Level: " << level << " | Max lvl: " << delim.size() << std::endl;
			for(auto o : delim[level])
			{
				//std::cout << "Comparing '" << o << "' to '" << stream[next_pos].info << "'" << std::endl;
				if(stream[next_pos].info == o) found = 1;
			}

			int one_after_start = start;
			if(stream[one_after_start].isParenOpen())
			{
				int paren_pos = skip_paren(stream, start);
				if(paren_pos != -1)
					one_after_start = paren_pos;
				else
					return -1;
			}
			else if(ASTFnCall::is_fn_call(stream, one_after_start))
			{
				int fn_end = ASTFnCall::get_end_fn_call(stream, start);
				one_after_start = fn_end;
			}
			else 
			{
				one_after_start++;
			}
			std::cout << "OAS: " << one_after_start << " | End: " << end << std::endl;

			//if we find one on this level, then we should parse to the right and left of it
			if(found)
			{
				std::cout << "Found an operator: '" << stream[next_pos].info << "'" << std::endl;
				
				lhs = new ASTExpr();
				lhs->parse_lvl(stream, start, next_pos, precedence, level+1);
				op = new ASTBinOp();
				op->parse(stream, next_pos);
				rhs = new ASTExpr();
				rhs->parse_lvl(stream, next_pos+1, end, precedence, level);
				return 0;
			}
			else if(one_after_start >= end || IS_EOF(stream, one_after_start))
			{
				lhs = new ASTExpr();
				lhs->parse_operand(stream, start);
				return 0;
				/*op = new ASTBinOp();
				op->parse(stream, next_pos);
				rhs = new ASTExpr();
				rhs->parse_lvl(stream, end+1, stream.size(), delim, level);*/
			}

			//get the next position accordingly
			if(stream[next_pos].isParenOpen())
			{
				int paren_pos = skip_paren(stream, start);
				if(paren_pos != -1)
					next_pos = paren_pos;
				else
					return -1;
			}
			else if(ASTFnCall::is_fn_call(stream, start))
			{
				int fn_end = ASTFnCall::get_end_fn_call(stream, start);
				next_pos = fn_end;
			}
			else
			{
				next_pos++;
			}
		}
		std::cout << "didn't find anything, s: " << start << " | e: " << end << std::endl;
		parse_lvl(stream, start, end, precedence, level+1);
		return 0;
	}

	int ASTExpr::parse(stream_t &stream, int start)
	{
		return parse_lvl(stream, start, stream.size(), precedence, 0);
	}

	int ASTRetExpr::parse(stream_t &stream, int start)
	{
		return 0;
	}

	int ASTVarDecl::parse(stream_t &stream, int start)
	{
		return 0;
	}

	int ASTSelState::parse(stream_t &stream, int start)
	{
		return 0;
	}

	int ASTWhileState::parse(stream_t &stream, int start)
	{
		return 0;
	}
	
	int ASTState::parse(stream_t &stream, int start)
	{
		return 0;
	}

	int ASTFnDecl::parse(stream_t &stream, int start)
	{
		return 0;
	}

	//all the code gen functions
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
		return nullptr;
	}

	llvm::Value * ASTFloat::generate()
	{
		return nullptr;
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

	//all the print functions
	void ASTBinOp::print()
	{
		std::cout << " " << op << " ";
	}
	
	void ASTIden::print()
	{
		std::cout << " " << iden << " ";
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
		std::cout << " (EXPR ";
		if(lhs) lhs->print();
		if(op) op->print();
		if(rhs) rhs->print();

		if(iden) iden->print();
		if(flt) flt->print();
		if(int_v) int_v->print();
		if(call) call->print();
		std::cout << " ENDEXPR)";
	}

	void ASTRetExpr::print()
	{
		
	}

	void ASTVarDecl::print()
	{
		
	}

	void ASTSelState::print()
	{
		
	}

	void ASTWhileState::print()
	{
		
	}
	
	void ASTState::print()
	{
		
	}

	void ASTFnDecl::print()
	{
		
	}
}
