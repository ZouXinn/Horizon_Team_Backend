#pragma once
#include "AST.h"
#include "FormalParaItemAST.h"
#include <vector>

class FormalParaListAST:public AST
{
public:
	vector<FormalParaItemAST*>* formalParaItemASTs;

	FormalParaListAST();
	virtual ~FormalParaListAST();
	void addFormalParaItemAST(FormalParaItemAST* ast);
};

