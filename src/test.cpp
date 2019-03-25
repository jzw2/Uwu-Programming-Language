#include "parser.h"
#include "lexer.h"
#include "lexerUtils.h"

#include <iostream>

int main(void)
{
	std::string input = "fibonacci : nth no jutsu";
	std::vector<naruto::Lex> lexes;
	naruto::naruto_lexize(input, lexes);
	for(auto item : lexes)
	{
		printLex(item);
	}
	naruto::ASTExpr expression;
	std::cout << "fn end: " << naruto::ASTFnCall::get_end_fn_call(lexes, 0) << std::endl;
	expression.parse(lexes, 0);
	expression.print();
	return 0;
}
