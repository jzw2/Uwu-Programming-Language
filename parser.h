#include <vector>
#include "llvm/IR/Value.h"
#include "lexer.h"

namespace naruto
{
  //base class
	class ASTNode 
	{
  public:
    virtual Value* codegen() = 0;
    virtual ~ASTNode() = 0;
    virtual void debug_info();
	};

	class ASTIden : public ASTNode
	{
    std::string name;
  public:
    AstIden(int val);
    
    virtual Value* codegen() override;
    virtual ~ASTIden() override;
    virtual void debug_info() override;

	};

  class ASTBinExpr
  {
    char op; //changet his later
    std::unique_ptr<ASTNode> left, right;
  public:
    ASTBinExpr(char op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right);
    
    virtual Value* codegen() override;
    virtual ~ASTBinExpr() override;
    virtual void debug_info() override;
  };

  class ASTFuncCall : public ASTNode {
    std::vector<std::unique_ptr<ExprAST>> args;
    std::string func_name;

  public:
    ASTBinExpr(std::vector<std::unique_ptr<ExprAST>> args, std::string func_name);
    
    virtual Value* codegen() override;
    virtual ~ASTFuncCall() override;
    virtual void debug_info() override;
    
  };
}
