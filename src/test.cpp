#include "parser.h"
#include "lexer.h"
#include "lexerUtils.h"

#include <iostream>

//extern std::unique_ptr<llvm::Module> naruto::sModule;
int main(void)
{
	std::string input = "finonacci : ( nth - 1 ) no jutsu + fibonacci : nth - 2 no jutsu";
	std::vector<naruto::Lex> lexes;
  naruto::sModule = std::make_unique<llvm::Module>("sugma", naruto::sContext);
	naruto::naruto_lexize(input, lexes);
	for(auto item : lexes)
	{
		//printLex(item);
	}
	naruto::ASTExpr expression;
	expression.parse(lexes, 0);
	expression.print();
	std::cout << std::endl;
	return 0;
}
