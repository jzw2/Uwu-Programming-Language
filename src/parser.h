#pragma once

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
	typedef std::vector<Lex> stream_t;


	extern llvm::LLVMContext sContext;
	extern llvm::IRBuilder<> sBuilder;
	extern std::unique_ptr<llvm::Module> sModule;
	extern std::map<std::string, llvm::AllocaInst*> sLocals;
	class ASTNode
	{	
		ASTNode * parent;
	public:
		ASTNode() : parent(nullptr) {} 
		ASTNode(ASTNode * par) : parent(par) {}
		ASTNode(const ASTNode & other) { parent = other.parent; }
		ASTNode & operator=(const ASTNode & rhs) { parent = rhs.parent; 
			return *this; }
		
		virtual ~ASTNode() = default;
		virtual int parse(stream_t &stream, int start) { return -1; }
		virtual llvm::Value * generate() { return nullptr; }
		virtual void print() {}
		
		ASTNode * getParent() { return parent; }
		void setParent(ASTNode * par) { parent = par; }
	};
	
	//this is for much later, for the clone
	std::vector<std::string> get_outofcontext_vars(ASTNode * subroot);

	class ASTBinOp : public ASTNode
	{
		std::string op;
	public:
		ASTBinOp() : op(std::string()) {}
	ASTBinOp(std::string o) : op(o) {}

		ASTBinOp(const ASTBinOp & other) : ASTNode((ASTNode)other) { 
			op = other.op; }
		ASTBinOp & operator=(const ASTBinOp & rhs) { ASTNode::operator=(rhs); 
			op = rhs.op; return *this; }
		
		virtual ~ASTBinOp() override = default;
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;

		std::string getOp() {return op;}
	};

	class ASTIden : public ASTNode
	{
		std::string iden;
		std::string type;
	public:
		ASTIden() : iden(std::string()), type(std::string()) {}
		ASTIden(const ASTIden & other) : ASTNode((ASTNode)other) { iden = other.iden; type = other.type; }
		ASTIden(std::string s) {
			iden = s;
			type = "int"; //deafult i	is int
		}
		ASTIden & operator=(const ASTIden & rhs) { ASTNode::operator=(rhs); iden = rhs.iden; type = rhs.type; return *this; }
		
		virtual ~ASTIden() override = default;
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
		
		std::string getIden() {return iden;}
		bool isFloat() {return type == "float";}
		bool isInt() {return type == "int";}
		bool isString() {return type == "string";}
		void setIden(std::string s) {iden = s;}
	};

	class ASTInt : public ASTNode
	{
		long val;
	public:
		ASTInt() : val(0) {}
		ASTInt(const ASTInt & rhs) : ASTNode((ASTNode)rhs) { val = rhs.val; }
		ASTInt & operator=(const ASTInt & rhs) { ASTNode::operator=(rhs); 
			val = rhs.val; return *this; }
		
	ASTInt(long v) : val(v) {}
		virtual ~ASTInt() override = default;
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
		
		void setVal(long v) {val = v;}
	};

	class ASTFloat : public ASTNode
	{
		double val;
	public:
		ASTFloat() : val(0) {}
		ASTFloat(const ASTFloat & other) : ASTNode((ASTNode)other) { 
			val = other.val; }
		ASTFloat & operator=(const ASTFloat & rhs) { ASTNode::operator=(rhs); 
			val = rhs.val; return *this; }
		
		virtual ~ASTFloat() override = default;
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
	};

	class ASTString : public ASTNode
	{
		std::string str;
	public:
		ASTString() : str(EMP_STR) {}
		ASTString(const ASTString & rhs) : ASTNode((ASTNode)rhs) { str = rhs.str; }
		ASTString &operator=(const ASTString & rhs) { ASTNode::operator=(rhs); 
			str = rhs.str; return *this; }
		virtual ~ASTString() override = default;
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
		ASTFnCall() : iden(nullptr), params(std::vector<ASTExpr *>()) {}
		ASTFnCall(const ASTFnCall & other) : ASTNode((ASTNode)other) { 
			iden = other.iden ? new ASTIden(*(other.iden)) : iden; 
			params = other.params; }
		ASTFnCall & operator=(const ASTFnCall & rhs) { ASTNode::operator=(rhs); 
			iden = rhs.iden ? new ASTIden(*(rhs.iden)) : iden; 
			params = rhs.params; 
			return *this; }
		virtual ~ASTFnCall() override; //{ for(ASTExpr * p : params) delete p; delete iden; };
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
		
		static int get_end_fn_call(stream_t &stream, int start);
		static bool is_fn_call(stream_t &stream, int start);
	};

	class ASTExpr : public ASTNode //ASTIden, ASTInt, ASTFloat, ASTFnCall???
	{
		ASTExpr * lhs;
		ASTBinOp * op;
		ASTExpr * rhs;
		
		//ASTExpr * expr; //this is the lhs
		ASTIden * iden;
		ASTFloat * flt;
		ASTInt * int_v;
		ASTFnCall * call;
		ASTString * str;
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
		static ASTExpr* make_binop(ASTExpr *a, std::string o, ASTExpr *b) {
			auto ret = new ASTExpr();
			ret->op = new ASTBinOp(o);
			ret->lhs = a;
			ret->rhs = b;
			return ret;
		}

		static ASTExpr* make_var(std::string s) {
			auto ret = new ASTExpr();
			ret->iden = new ASTIden(s);
			return ret;
		}

		static ASTExpr* make_plus_plus(std::string s) {
			auto left = make_var(s);
			auto right = new ASTExpr(1);

			return make_binop(left, "+", right);
		}
		ASTExpr() : lhs(nullptr), 
			op(nullptr), 
			rhs(nullptr), 
			iden(nullptr), 
			flt(nullptr), 
			int_v(nullptr), 
			str(nullptr), 
			call(nullptr) {}
		ASTExpr(const ASTExpr & other) : ASTNode((ASTNode)other) { 
			lhs = other.lhs ? new ASTExpr(*(other.lhs)) : other.lhs;
			op = other.op ? new ASTBinOp(*(other.op)) : other.op;
			rhs = other.rhs ? new ASTExpr(*(other.rhs)) : other.rhs;
			iden = other.iden ? new ASTIden(*(other.iden)) : other.iden;
			flt = other.flt ? new ASTFloat(*(other.flt)) : other.flt;
			int_v = other.int_v ? new ASTInt(*(other.int_v)) : other.int_v;
			call = other.call ? new ASTFnCall(*(other.call)) : other.call;
			str = other.str ? new ASTString(*(other.str)) : other.str; }
		ASTExpr & operator=(const ASTExpr & other) { ASTNode::operator=(other);
			lhs = other.lhs ? new ASTExpr(*(other.lhs)) : other.lhs;
			op = other.op ? new ASTBinOp(*(other.op)) : other.op;
			rhs = other.rhs ? new ASTExpr(*(other.rhs)) : other.rhs;
			iden = other.iden ? new ASTIden(*(other.iden)) : other.iden;
			flt = other.flt ? new ASTFloat(*(other.flt)) : other.flt;
			int_v = other.int_v ? new ASTInt(*(other.int_v)) : other.int_v;
			call = other.call ? new ASTFnCall(*(other.call)) : other.call;
			str = other.str ? new ASTString(*(other.str)) : other.str; 
			return *this; }

	ASTExpr(long val) : lhs(nullptr), 
			op(nullptr), 
			rhs(nullptr), 
			iden(nullptr), 
			flt(nullptr), 
			int_v(nullptr), 
			str(nullptr), 
			call(nullptr) {
										 int_v = new ASTInt(val);
			}
		virtual ~ASTExpr() override {delete lhs; delete op; delete iden; delete flt; 
			delete int_v; delete call; delete str; }
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;

		void setInt_V(ASTInt *i) {int_v = i;}
		
	};

	class ASTRetExpr : public ASTNode
	{
		ASTExpr * expr;
	public:
		ASTRetExpr() : expr(nullptr) {}
		ASTRetExpr(const ASTRetExpr & other) : ASTNode((ASTNode)other) 
			{ expr = other.expr ? new ASTExpr(*(other.expr)) : other.expr; }
		ASTRetExpr & operator=(const ASTRetExpr & rhs) { ASTNode::operator=(rhs); 
			expr = rhs.expr ? new ASTExpr(*(rhs.expr)) : rhs.expr; return *this; }
		
		virtual ~ASTRetExpr() override { delete expr; }
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
	
		void setExpr(ASTExpr *e) {expr = e;}
	};

	class ASTVarDecl : public ASTNode
	{
		ASTIden * name;
		ASTExpr * val;
	public:
		ASTVarDecl() : name(nullptr),
		val(nullptr) {}
		ASTVarDecl(const ASTVarDecl & other) : ASTNode((ASTNode)other) { 
			val = other.val ? new ASTExpr(*(other.val)) : other.val;
			name = other.name ? new ASTIden(*(other.name)) : other.name; }
		ASTVarDecl & operator=(const ASTVarDecl & rhs) { ASTNode::operator=(rhs); 
			val = rhs.val ? new ASTExpr(*(rhs.val)) : rhs.val;
			name = rhs.name ? new ASTIden(*(rhs.name)) : rhs.name; 
			return *this; }
		ASTVarDecl(std::string n, long v) {
																			 name = new ASTIden(n);
																			 val = new ASTExpr(v);

		}
		ASTVarDecl(std::string n, ASTExpr* v) {
																			 name = new ASTIden(n);
																			 val = v;

		}
		virtual ~ASTVarDecl() override { delete name; delete val; }
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
	
		std::string getIden() { return name->getIden(); }
	};

	class ASTState;

	class ASTSelState : public ASTNode
	{
		ASTExpr * expr;
		std::vector<ASTState*> if_body;
		std::vector<ASTState*> else_body;
		ASTSelState * elif;
	public:
		ASTSelState() : expr(nullptr), 
			if_body(std::vector<ASTState*>()), 
			else_body(std::vector<ASTState*>()), 
			elif(nullptr) {}
		ASTSelState(const ASTSelState & other) : ASTNode((ASTNode)other) { 
			expr = other.expr ? new ASTExpr(*(other.expr)) : other.expr;
			if_body = other.if_body;
			else_body = other.else_body;
			elif = other.elif ? new ASTSelState(*(other.elif)) : other.elif; }
		ASTSelState & operator=(const ASTSelState & rhs) { 
			ASTNode::operator=((ASTNode)rhs); 
			expr = rhs.expr ? new ASTExpr(*(rhs.expr)) : rhs.expr;
			if_body = rhs.if_body;
			else_body = rhs.else_body;
			elif = rhs.elif ? new ASTSelState(*(rhs.elif)) : rhs.elif; 
			return *this; }

		virtual ~ASTSelState() override;
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
	
		std::vector<ASTState *> getIfBody() { return if_body; }
		std::vector<ASTState *> getElseBody() { return else_body; }
	};

	class ASTWhileState : public ASTNode
	{
		ASTExpr * expr;
		std::vector<ASTState*> state;
	public:
		ASTWhileState() : expr(nullptr),
			state(std::vector<ASTState*>()) {}
		ASTWhileState(const ASTWhileState & other) : ASTNode((ASTNode)other) { 
			expr = other.expr ? new ASTExpr(*(other.expr)) : other.expr; 
			state = other.state; }
		ASTWhileState(ASTExpr * ex, std::vector<ASTState*> st) : expr(ex),
			state(st) {};
		ASTWhileState & operator=(const ASTWhileState & rhs) { 
			ASTNode::operator=(rhs); 
			expr = rhs.expr ? new ASTExpr(*(rhs.expr)) : rhs.expr; 
			state = rhs.state; return *this; }
		virtual ~ASTWhileState() override;
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
		
		std::vector<ASTState *> getState() { return state; }
	};
	
	class ASTLambdaThread : public ASTNode
	{
		ASTExpr * expr;
		std::vector<ASTState*> state;
	public:
		ASTLambdaThread() : expr(nullptr),
			state(std::vector<ASTState*>()) {}
		ASTLambdaThread(const ASTLambdaThread & other) : ASTNode((ASTNode)other) { 
			expr = other.expr ? new ASTExpr(*(other.expr)) : other.expr; 
			state = other.state; }
		ASTLambdaThread(ASTExpr * ex, std::vector<ASTState*> st) : expr(ex),
			state(st) {};
		ASTLambdaThread & operator=(const ASTLambdaThread & rhs) { 
			ASTNode::operator=(rhs); 
			expr = rhs.expr ? new ASTExpr(*(rhs.expr)) : rhs.expr; 
			state = rhs.state; return *this; }
		virtual ~ASTLambdaThread() override;
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
		
		std::vector<ASTState *> getState() { return state; }
	};

	class ASTState : public ASTNode
	{
		ASTWhileState * ws;
		ASTSelState * ss;
		ASTExpr * expr;
		ASTRetExpr * retexpr;
		ASTVarDecl * vdc;
		ASTLambdaThread * thread;
	public:
		ASTState() : ws(nullptr),
			ss(nullptr),
			expr(nullptr),
			retexpr(nullptr),
			thread(nullptr),
			vdc(nullptr) {} 
		ASTState(const ASTState & other) : ASTNode((ASTNode)other) { 
			ws = other.ws ? new ASTWhileState(*(other.ws)) : other.ws;
			ss = other.ss ? new ASTSelState(*(other.ss)) : other.ss;
			expr = other.expr ? new ASTExpr(*(other.expr)) : other.expr;
			retexpr = other.retexpr ? new ASTRetExpr(*(other.retexpr)) : other.retexpr;
			vdc = other.vdc ? new ASTVarDecl(*(other.vdc)) : other.vdc;
			thread = other.thread ? new ASTLambdaThread(*(other.thread)) : other.thread; }
		ASTState & operator=(const ASTState & rhs) { 
			ASTNode::operator=((ASTNode)rhs); 
			ws = rhs.ws ? new ASTWhileState(*(rhs.ws)) : rhs.ws;
			ss = rhs.ss ? new ASTSelState(*(rhs.ss)) : rhs.ss;
			expr = rhs.expr ? new ASTExpr(*(rhs.expr)) : rhs.expr;
			retexpr = rhs.retexpr ? new ASTRetExpr(*(rhs.retexpr)) : rhs.retexpr;
			vdc = rhs.vdc ? new ASTVarDecl(*(rhs.vdc)) : rhs.vdc;
			thread = rhs.thread ? new ASTLambdaThread(*(rhs.thread)) : rhs.thread; 
			return *this; }
		
	ASTState(ASTExpr* e) : ws(nullptr),
			ss(nullptr),
			expr(nullptr),
			retexpr(nullptr),
			thread(nullptr),
			vdc(nullptr) {
										expr = e;
			}
	ASTState(ASTVarDecl* e) : ws(nullptr),
			ss(nullptr),
			expr(nullptr),
			retexpr(nullptr),
			thread(nullptr),
			vdc(nullptr) {
										vdc = e;
			}
		virtual ~ASTState() override { delete ws; 
			delete ss; 
			delete expr; 
			delete retexpr;
			delete vdc; 
			delete thread; }
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
		
		void setRetExpr(ASTRetExpr *r) {retexpr = r;}
		ASTNode * getState() { if(ws) return ws;
			if(ss) return ss; 
			if(expr) return expr; 
			if(retexpr) return retexpr; 
			if(thread) return thread; 
			if(vdc) return vdc; 
			return nullptr; }
		ASTVarDecl *getVdc() {
			return vdc;
			}
	};

	class ASTFnDecl : public ASTNode
	{
		ASTIden * name;
		std::vector<ASTIden*> params;
		std::vector<ASTState*> body;
	public:
		ASTFnDecl() : name(nullptr),
			params(std::vector<ASTIden*>()),
			body(std::vector<ASTState*>()) {}
		ASTFnDecl(const ASTFnDecl & other) : ASTNode((ASTNode)other) { name = other.name ? new ASTIden(*(other.name)) : other.name;
			params = other.params;
			body = other.body; }

		ASTFnDecl(std::string func_name, std::vector<ASTState*> statements) {
			name = new ASTIden();
			name->setIden(func_name);
			body = statements;
		}
		ASTFnDecl operator=(const ASTFnDecl & rhs) { ASTNode::operator=((ASTNode)rhs); name = rhs.name ? new ASTIden(*(rhs.name)) : rhs.name;
			params = rhs.params;
			body = rhs.body;
			return *this; }
		
		virtual ~ASTFnDecl() override { delete name; 
			for(auto p : params) delete p; 
			for(auto b : body) delete b; }
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
		
		void setName(ASTIden *n) {name = n;}
		std::vector<ASTIden*>& getParams() {return params;}
		std::vector<ASTState*>& getBody() {return body;}
	};
	

	class ASTRoot : public ASTNode
	{
		std::vector<ASTVarDecl*> globals;
		std::vector<ASTFnDecl*> funcs;
	public:
		ASTRoot() : globals(std::vector<ASTVarDecl*>()), 
			funcs(std::vector<ASTFnDecl*>()) {}
		ASTRoot(const ASTRoot & rhs) : ASTNode((ASTNode)rhs) { 
			globals = rhs.globals;
			funcs = rhs.funcs; }
		ASTRoot & operator=(const ASTRoot & rhs) { ASTNode::operator=((ASTNode)rhs); 
			globals = rhs.globals;
			funcs = rhs.funcs; }
		virtual ~ASTRoot() override { 
			for(auto g : globals) delete g; 
			for(auto f : funcs) delete f; }
		virtual int parse(stream_t &stream, int start) override;
		virtual llvm::Value * generate() override;
		virtual void print() override;
		
		std::vector<ASTFnDecl*> getFuncs() { return funcs; }
		std::vector<ASTVarDecl*> getGlobals() { return globals; }
	};
	
	class CloneCall : public ASTExpr 
	{
		std::string function_name;
		llvm::Value *param;
	public:
		CloneCall(std::string s, llvm::Value* p) { function_name = s; param = p;}

		virtual llvm::Value * generate() override ;
	};

}

