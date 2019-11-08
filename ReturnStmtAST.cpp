#include "ReturnStmtAST.h"


ReturnStmtAST::ReturnStmtAST()
{

}

ReturnStmtAST::ReturnStmtAST(ExpAST* expAST)
{
	this->expAST = expAST;
}

ReturnStmtAST::~ReturnStmtAST()
{
	if (this->expAST != nullptr)
	{
		delete this->expAST;
	}
}