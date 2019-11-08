#pragma once
#include "AST.h"
#include "RealParaItemAST.h"
#include <vector>
class RealParaListAST:public AST
{
public:
	vector<RealParaItemAST*>* realParaItemASTs;

	RealParaListAST();
	RealParaListAST(RealParaItemAST* ast);
	virtual ~RealParaListAST();

	void addRealParaListAST(RealParaItemAST* ast);

};

