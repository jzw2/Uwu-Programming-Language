#include <vector>
#include "llvm/IR/Value.h"

#include "lexer.h"

namespace naruto
{
	extern int stream_pos;
	class ASTNode
	{	
	public:
		virtual int parse(std::vector<Lex> &stream, int start) = 0;
		virtual llvm::Value * generate() = 0;
	};

	class ASTBinOp : public ASTNode
	{
		std::string op;
	public:
		virtual int parse(std::vector<Lex> &stream, int start) override;
		virtual llvm::Value * generate() override;
	};

	class ASTIden : public ASTNode
	{
		std::string iden;
	public:
		virtual int parse(std::vector<Lex> &stream, int start) override;
		virtual llvm::Value * generate() override;
	};

	class ASTInt : public ASTNode
	{
		long val;
	public:
		virtual int parse(std::vector<Lex> &stream, int start) override;
		virtual llvm::Value * generate() override;
	};

	class ASTFloat : public ASTNode
	{
		double val;
	public:
		virtual int parse(std::vector<Lex> &stream, int start) override;
		virtual llvm::Value * generate() override;
	};

	class ASTExpr;
	
	class ASTFnCall : public ASTNode
	{
		ASTIden * iden;
		std::vector<ASTExpr *> params;
	public:
		virtual int parse(std::vector<Lex> &stream, int start) override;
		virtual llvm::Value * generate() override;
	};

	class ASTExpr : ASTNode //ASTIden, ASTInt, ASTFloat, ASTFnCall???
	{
		ASTExpr * lhs;
		ASTBinOp * op;
		ASTExpr * rhs;
		
		ASTExpr * expr;
		ASTIden * iden;
		ASTFloat * flt;
		ASTInt * int_v;
	public:
		virtual int parse(std::vector<Lex> &stream, int start) override;
		virtual llvm::Value * generate() override;
	};

	class ASTRetExpr : public ASTNode
	{
		ASTExpr * expr;
	public:
		virtual int parse(std::vector<Lex> &stream, int start) override;
		virtual llvm::Value * generate() override;
	};

	class ASTVarDecl : ASTNode
	{
		ASTIden * name;
		ASTExpr * val;
	public:
		virtual int parse(std::vector<Lex> &stream, int start) override;
		virtual llvm::Value * generate() override;
	};

	class ASTState;

	class ASTSelState : ASTNode
	{
		ASTExpr * expr;
		std::vector<ASTState*> if_body;
		std::vector<ASTState*> else_body;
		ASTSelState * elif;
	public:
		virtual int parse(std::vector<Lex> &stream, int start) override;
		virtual llvm::Value * generate() override;
	};

	class ASTWhileState : ASTNode
	{
		ASTExpr * expr;
		std::vector<ASTState*> state;
	public:
		virtual int parse(std::vector<Lex> &stream, int start) override;
		virtual llvm::Value * generate() override;
	};

	class ASTState : ASTNode
	{
		ASTWhileState ws;
		ASTSelState ss;
		ASTExpr * expr;
		ASTRetExpr * retexpr;
		ASTVarDecl * vdc;
	public:
		virtual int parse(std::vector<Lex> &stream, int start) override;
		virtual llvm::Value * generate() override;
	};

	class ASTFnDecl : ASTNode
	{
		ASTIden * name;
		std::vector<ASTIden*> params;
		std::vector<ASTState*> body;
	public:
		virtual int parse(std::vector<Lex> &stream, int start) override;
		virtual llvm::Value * generate() override;
	};
}
