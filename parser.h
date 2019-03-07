#include <vector>

#include "lexer.h"

namespace naruto
{
	class AST
	{
		struct ASTNode
		{
			std::vector<ASTNode> nodes;
		}
		ASTNode root;
	public:
		void build_tree(std::vector<Lex> stream);
	};
}
