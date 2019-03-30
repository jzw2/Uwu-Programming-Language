#include "parser.h"
#include "lexer.h"
#include "lexerUtils.h"

#include <iostream>

int main(void)
{
	std::string input = "../code_samples/fib.uwu";
	std::vector<naruto::Lex> lexes;
	//naruto::sModule = std::make_unique<llvm::Module>("sugma", naruto::sContext);
	lexes = naruto::naruto_lexize_file(input);

  
	for(auto item : lexes)
	{
		printLex(item);
	}


	naruto::ASTExpr expression;
	//expression.parse(lexes, 0);
	//expression.print();
	std::cout << std::endl;
	return 0;
}
