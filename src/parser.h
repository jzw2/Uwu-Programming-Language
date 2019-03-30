#include <vector>
#include "llvm/IR/Value.h"
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

#include "lexer.h"

namespace naruto
{
  extern llvm::LLVMContext sContext;
  extern llvm::IRBuilder<> sBuilder;
  extern std::unique_ptr<llvm::Module> sModule;
  extern std::map<std::string, llvm::AllocaInst*> sLocals;

	typedef std::vector<Lex> stream_t;
	extern int stream_pos;
	class ASTNode
	{	
	public:

		virtual int parse(stream_t &stream, int start) = 0;
		virtual llvm::Value * generate() = 0;
		virtual void print() = 0;
	};

	class ASTBinOp : public ASTNode
	{
		std::string op;
	public:
		ASTBinOp() : op(std::string()) {}
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
	};

	class ASTIden : public ASTNode
	{
		std::string iden;
	public:
		ASTIden() : iden(std::string()) {}
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
    std::string getIden() {return iden;}
	};

	class ASTInt : public ASTNode
	{
		long val;
	public:
		ASTInt() : val(0) {}
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
	};

	class ASTFloat : public ASTNode
	{
		double val;
	public:
		ASTFloat() : val(0) {}
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
	};

	class ASTExpr;
	
	class ASTFnCall : public ASTNode
	{
		ASTIden * iden;
		std::vector<ASTExpr *> params;
	public:
		static int get_end_fn_call(stream_t &stream, int start);
		static bool is_fn_call(stream_t &stream, int start);
		ASTFnCall() : iden(nullptr), params(std::vector<ASTExpr *>()) {}
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
	};

	class ASTExpr : ASTNode //ASTIden, ASTInt, ASTFloat, ASTFnCall???
	{
		ASTExpr * lhs;
		ASTBinOp * op;
		ASTExpr * rhs;
		
		//ASTExpr * expr; //this is the lhs
		ASTIden * iden;
		ASTFloat * flt;
		ASTInt * int_v;
		ASTFnCall * call;
		//specify a level to parse at
		int parse_lvl(stream_t &stream, 
			int start, 
			int end, 
			std::vector<std::vector<std::string> > delim, 
			int level);
		int parse_operand(stream_t &stream, int start);
	public:
		static int find_end_expression(stream_t &stream, int start);
		static bool is_end_expression(stream_t &stream, int start);
		ASTExpr() : lhs(nullptr), 
			op(nullptr), 
			rhs(nullptr), 
			iden(nullptr), 
			flt(nullptr), 
			int_v(nullptr), 
			call(nullptr) {}
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
	};

	class ASTRetExpr : public ASTNode
	{
		ASTExpr * expr;
	public:
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
	};

	class ASTVarDecl : ASTNode
	{
		ASTIden * name;
		ASTExpr * val;
	public:
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
	};

	class ASTState;

	class ASTSelState : ASTNode
	{
		ASTExpr * expr;
		std::vector<ASTState*> if_body;
		std::vector<ASTState*> else_body;
		ASTSelState * elif;
	public:
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
	};

	class ASTWhileState : ASTNode
	{
		ASTExpr * expr;
		std::vector<ASTState*> state;
	public:
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
	};

	class ASTState : ASTNode
	{
		ASTWhileState ws;
		ASTSelState ss;
		ASTExpr * expr;
		ASTRetExpr * retexpr;
		ASTVarDecl * vdc;
	public:
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
	};

	class ASTFnDecl : ASTNode
	{
		ASTIden * name;
		std::vector<ASTIden*> params;
		std::vector<ASTState*> body;
	public:
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
	};
}
