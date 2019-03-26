#include <iostream>
#include <algorithm>
#include <stack>
#include "parser.h"
#include "lexerUtils.h"

#define IS_EOF(stream, start) (start >=  (int)stream.size())
#define ASSERT_BOUNDS(stream, start) if(start >= (int)stream.size()) return stream.size(); else if(start == -1) return -1;
#define RESET(obj) delete obj; obj = nullptr;
namespace naruto
{
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
			if(start >= (int)stream.size())
				return -1;
			/*if(stream[start].code == TokenCodes::no_jutsu)
			{
				return start+1;
			}
			else*/ if(stream[start].isColon())
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
		/*if(stream[start].isIden() && stream[start+1].code == TokenCodes::no_jutsu)
		{
			iden = new ASTIden();
			start = iden->parse(stream, start);
			return start + 1;
		}*/
		if(stream[start].isIden() && stream[start+1].isColon())
		{
			iden = new ASTIden();
			start = iden->parse(stream, start);
			int next_pos = start+1;
			while(true)
			{
				if(next_pos >= (int)stream.size())
				{
					return -1;
				}
				else if(stream[next_pos].code == TokenCodes::no_jutsu)
				{
					break;
				}
				else
				{
					ASTExpr * expr = new ASTExpr();
					next_pos = expr->parse(stream, next_pos);
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
			else if(is_end_expression(stream, next_pos))
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
			if(stream[fn_end].isKeyword() |
			stream[fn_end].isParenOpen() |
			stream[fn_end].isIden() |
			stream[fn_end].isDelim())
			{
				return true;
			}
		}
		else
		{
			if(stream[start].isIden() | stream[start].isParenClose())
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
				if(stream[start].isKeyword() |
				stream[start].isParenOpen() |
				stream[start].isIden() |
				stream[start].isDelim())
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
		//find the highest level operator
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
				lhs->parse_lvl(stream, start, next_pos, precedence, level+1);
				op = new ASTBinOp();
				op->parse(stream, next_pos);
				rhs = new ASTExpr();
				rhs->parse_lvl(stream, next_pos+1, end, precedence, level);
				return end;
			}
			else if(one_after_start >= end || IS_EOF(stream, one_after_start))
			{
				lhs = new ASTExpr();
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
		if(level == (int)(delim.size() - 1))
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
		return parse_lvl(stream, start, find_end_expression(stream, start), precedence, 0);
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
		std::cout << " ( ";
		if(lhs) lhs->print();
		if(op) op->print();
		if(rhs) rhs->print();

		if(iden) iden->print();
		if(flt) flt->print();
		if(int_v) int_v->print();
		if(call) call->print();
		std::cout << " )";
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
