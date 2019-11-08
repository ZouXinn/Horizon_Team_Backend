#pragma once
#include "AST.h"
//#include "ExpAST.h"
class ExpAST;
class RealParaItemAST:public AST
{
public:
	ExpAST* expAST = nullptr;

	RealParaItemAST(ExpAST* expAST);
	virtual ~RealParaItemAST();
};

