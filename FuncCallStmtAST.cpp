#include "FuncCallStmtAST.h"


FuncCallStmtAST::FuncCallStmtAST(IdentifierAST* identifier, RealParaListAST* realParaListAST)
{
	this->identifier = identifier;
	this->realParaListAST = realParaListAST;
}

FuncCallStmtAST::~FuncCallStmtAST()
{
	if (this->realParaListAST != nullptr)
	{
		delete this->realParaListAST;
	}
	if (this->identifier != nullptr)
	{
		delete this->identifier;
	}
}