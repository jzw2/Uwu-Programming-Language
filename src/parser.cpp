#include <iostream>

#include "parser.h"

#define ASSERT_BOUNDS(stream, start) if(start >= stream.size() || start == -1) return -1;

namespace naruto
{
	int stream_pos;

	int ASTBinOp::parse(std::vector<Lex> &stream, int start)
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
	
	int ASTIden::parse(std::vector<Lex> &stream, int start)
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
	
	int ASTInt::parse(std::vector<Lex> &stream, int start)
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

	int ASTFloat::parse(std::vector<Lex> &stream, int start)
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
std::string type_to_string(int code)
{
	switch(code)
	{	
	case naruto::TokenCodes::identifier:
		return "Identifier";
	case naruto::TokenCodes::int_val:
		return "Int Val";
	case naruto::TokenCodes::float_val:
		return "Float Val";
	case naruto::TokenCodes::no_jutsu:
		return "No Jutsu";
	case naruto::TokenCodes::sayonara:
		return "Sayonara";
	case naruto::TokenCodes::chan:
		return "Chan";
	case naruto::TokenCodes::nani:
		return "Nani";
	case naruto::TokenCodes::baka:
		return "Baka";
	case naruto::TokenCodes::suki:
		return "Suki";
	case naruto::TokenCodes::wa:
		return "Wa";
	case naruto::TokenCodes::desu:
		return "Desu";
	case naruto::TokenCodes::bin_op:
		return "Binary Operator";
	case naruto::TokenCodes::paren_open:
		return "(";
	case naruto::TokenCodes::paren_close:
		return ")";
	case naruto::TokenCodes::newline:
		return "Newline";
	}
}


	int ASTFnCall::parse(std::vector<Lex> &stream, int start)
	{	
		ASSERT_BOUNDS(stream, start);
		int new_start = start;
		if(stream[start].code == TokenCodes::identifier)
		{
			iden = new ASTIden;
			start = iden->parse(stream, start);
			std::cout << "Starting fncall: " << start << std::endl;
			while(start != -1 && stream[start].code != TokenCodes::no_jutsu)
			{
				ASTExpr * expr = new ASTExpr;
				start = expr->parse(stream, start);
				std::cout << "Found param: "; expr->print();
		
		std::cout << std::endl << "------------" << std::endl << 
		"Obj type: " << type_to_string(stream[start].code) << std::endl << 
		"Obj info: '" << stream[start].info << "'" << std::endl << 
		"Int val: " << stream[start].int_val << std::endl << 
		"Flt val: " << stream[start].float_val << std::endl;

				params.push_back(expr);
			}
			if(start == -1)
			{
				delete iden; iden = nullptr;
				for(auto p : params)
				{
					delete p;
				}
				params.clear();
				return start;
			}
			else
			{
				return start + 1;
			}
		}
		return -1;
	}

	int ASTExpr::parse_rhs(std::vector<Lex> &stream, int start)
	{
		ASSERT_BOUNDS(stream, start);
		if(stream[start].code == TokenCodes::bin_op)
		{
			op = new ASTBinOp;
			start = op->parse(stream, start);
			
			if(start == -1)
			{
				delete op; op = nullptr;
				return start;
			}

			rhs = new ASTExpr;
			start = rhs->parse(stream, start);
			if(start == -1)
			{
				delete op; op = nullptr;
				delete rhs; rhs = nullptr;
			}
			return start;
		}
		else
		{
			return -1;
		}
	}

	int ASTExpr::parse(std::vector<Lex> &stream, int start)
	{
		ASSERT_BOUNDS(stream, start);
		int pos_start;
		std::cout << start << std::endl;
		if(stream[start].code == TokenCodes::paren_open)
		{
			lhs = new ASTExpr;
			start = lhs->parse(stream, start + 1);
			if(start == -1)
			{
				delete lhs;
				lhs = nullptr;
				return -1;
			}
			else
			{
				pos_start = parse_rhs(stream, start);
				
				if(pos_start == -1)
					return start + 1;
				else
					return pos_start + 1;
			}
		}
		else if(stream[start].code == TokenCodes::identifier)
		{
			std::cout << "Trying fncall!" << std::endl;
			call = new ASTFnCall;
			pos_start = call->parse(stream, start);
			if(pos_start == -1)
			{
				std::cout << "fncall failed!" << std::endl;
				delete call;
				call = nullptr;
				
				iden = new ASTIden;
			    start = iden->parse(stream, start);

				pos_start = parse_rhs(stream, start);
				if(pos_start == -1)
				{
					return start;
				}
				else
				{
					return pos_start;
				}
			}
			else
			{
			std::cout << std::endl << "Trying the rhs fn call" << std::endl << 
			"------------" << std::endl << 
		"Obj type: " << type_to_string(stream[start].code) << std::endl << 
		"Obj info: '" << stream[start].info << "'" << std::endl << 
		"Int val: " << stream[start].int_val << std::endl << 
		"Flt val: " << stream[start].float_val << std::endl;


				pos_start = parse_rhs(stream, start);				
	
				if(pos_start == -1)
					return start;
				else
					return pos_start;
			}
		}
		else if(stream[start].code == TokenCodes::float_val)
		{
			flt = new ASTFloat;
			start = flt->parse(stream, start);

			pos_start = parse_rhs(stream, start);
			if(pos_start == -1)
				return start;
			else
				return pos_start;
		}
		else if(stream[start].code == TokenCodes::int_val)
		{
			int_v = new ASTInt;
			start = int_v->parse(stream, start);

			pos_start = parse_rhs(stream, start);
			if(pos_start == -1)
				return start;
			else
				return pos_start;
		}
		else
		{
			return -1;
		}
	}

	int ASTRetExpr::parse(std::vector<Lex> &stream, int start)
	{
		return 0;
	}

	int ASTVarDecl::parse(std::vector<Lex> &stream, int start)
	{
		return 0;
	}

	int ASTSelState::parse(std::vector<Lex> &stream, int start)
	{
		return 0;
	}

	int ASTWhileState::parse(std::vector<Lex> &stream, int start)
	{
		return 0;
	}
	
	int ASTState::parse(std::vector<Lex> &stream, int start)
	{
		return 0;
	}

	int ASTFnDecl::parse(std::vector<Lex> &stream, int start)
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
