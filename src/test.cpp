#include "parser.h"
#include "lexer.h"
#include "lexerUtils.h"
#include "llvm/Support/raw_os_ostream.h"

#include <iostream>

extern llvm::LLVMContext naruto::sContext;
extern std::unique_ptr<llvm::Module> naruto::sModule;
void test1() {
  // naruto::ASTInt six;
  // six.setVal(69);

  // naruto::ASTExpr six_expr;
  // six_expr.setInt_V(&six);

  // naruto::ASTRetExpr expr;
  // expr.setExpr(&six_expr);

  // naruto::ASTState state;
  // state.setRetExpr(&expr);

  // naruto::ASTIden name;
  // name.setIden("testfunc");

  // naruto::ASTFnDecl func;
  // func.setName(&name);

  // auto params = func.getParams(); //params should be void
  
  // auto body = func.getBody();
  // body.push_back(&state);

  // func.generate();

  naruto::sModule = llvm::make_unique<llvm::Module>("module", naruto::sContext);
  std::vector<naruto::Lex> stream = naruto::naruto_lexize_file("test.uwu");
  naruto::ASTFnDecl f;
  f.parse(stream, 0);
  f.generate();

  llvm::raw_os_ostream file_stream(std::cout);
  naruto::sModule->print(file_stream, nullptr);
}

void test2() {
  naruto::sModule = llvm::make_unique<llvm::Module>("module", naruto::sContext);
  std::vector<naruto::Lex> stream = naruto::naruto_lexize_file("test2.uwu");
  naruto::ASTFnDecl f;
  f.parse(stream, 0);
  f.generate();

  llvm::raw_os_ostream file_stream(std::cout);
  naruto::sModule->print(file_stream, nullptr);
  
}


int main(void)
{
  //naruto::sModule = llvm::make_unique<llvm::Module>("module", naruto::sContext);
  //test1();
  test2();
	// std::string input = "../code_samples/factorial.uwu";
	// //std::string input = "test.uwu";
	// std::string expr = "temp wa temp*((n/10)*namae + 1) desu~";
	// std::vector<naruto::Lex> lexes;
	// lexes = naruto::naruto_lexize_file(input);
	// //naruto::naruto_lexize(expr, lexes);
	// int i = 0;
	// for(auto item : lexes)
	// {
	// 	std::cout << i << std::endl;
	// 	printLex(item);
	// 	i++;
	// }

	// /*naruto::ASTState state;
	// state.parse(lexes, 0);
	// state.print();*/
	// naruto::ASTRoot file;
	// file.parse(lexes, 0);
	// file.print();
	// std::cout << std::endl;
	return 0;
}
