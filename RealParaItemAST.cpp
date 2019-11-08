#include "RealParaItemAST.h"



RealParaItemAST::RealParaItemAST(ExpAST* expAST)
{
	this->expAST = expAST;
}

RealParaItemAST::~RealParaItemAST()
{
	if (this->expAST != nullptr)
	{
		delete this->expAST;
	}
}