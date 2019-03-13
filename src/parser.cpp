#include "parser.h"

namespace naruto
{
	int stream_pos;

	int ASTBinOp::parse(std::vector<Lex> &stream, int start)
	{
		return 0;
	}
	
	int ASTIden::parse(std::vector<Lex> &stream, int start)
	{
		return 0;
	}
	
	int ASTInt::parse(std::vector<Lex> &stream, int start)
	{
		return 0;
	}	

	int ASTFloat::parse(std::vector<Lex> &stream, int start)
	{
		return 0;
	}

	int ASTFnCall::parse(std::vector<Lex> &stream, int start)
	{
		return 0;
	}

	int ASTExpr::parse(std::vector<Lex> &stream, int start)
	{
		return 0;
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
}
