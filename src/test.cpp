#include "parser.h"
#include "lexer.h"
#include "lexerUtils.h"

#include <iostream>

//extern std::unique_ptr<llvm::Module> naruto::sModule;
void intTest() {
  naruto::ASTInt a;

}
int main(void)
{
	std::string input = "../code_samples/fib.uwu";
	std::string expr = "sayonara finonacci: nth-1 no jutsu + fibonacci: nth-2 no jutsu chan~";
	std::vector<naruto::Lex> lexes;
	lexes = naruto::naruto_lexize_file(input);
	//naruto::naruto_lexize(expr, lexes);
	int i = 0;
	for(auto item : lexes)
	{
		std::cout << i << std::endl;
		printLex(item);
		i++;
	}

	//naruto::ASTRetExpr expression;
	//naruto::ASTFnDecl fndecl;
	//fndecl.parse(lexes, 0);
	//fndecl.print();
	//int end = expression.parse(lexes, 0);
	//std::cout << end << std::endl;
	//expression.print();
	naruto::ASTRoot file;
	file.parse(lexes, 0);
	file.print();
	std::cout << std::endl;
	return 0;
}
