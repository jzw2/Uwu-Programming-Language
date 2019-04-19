#include "parser.h"

namespace naruto
{
	ASTFnCall::~ASTFnCall() 
	{ 
		for(ASTExpr * p : params) 
			delete p; 
		delete iden; 
	}

	ASTSelState::~ASTSelState() 
	{
		delete expr; 
		for(auto ib : if_body) 
			delete ib; 
		for(auto eb : else_body) 
			delete eb; 
		delete elif;
	}

	ASTWhileState::~ASTWhileState()
	{ 
		delete expr; 
		for(auto s : state) 
			delete s;
	}

	ASTLambdaThread::~ASTLambdaThread()
	{ 
		delete expr; 
		for(auto s : state) 
			delete s;
	}
}
