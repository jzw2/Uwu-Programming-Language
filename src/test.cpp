#include "parser.h"
#include "lexer.h"
#include "lexerUtils.h"

#include <iostream>

int main(void)
{
	std::string input = "a * b + c * d";
	std::vector<naruto::Lex> lexes;
	naruto::naruto_lexize(input, lexes);
	for(auto item : lexes)
	{
		printLex(item);
	}
	naruto::ASTExpr expression;
	expression.parse(lexes, 0);
	expression.print();
	return 0;
}
