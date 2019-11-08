#pragma once
#include "AST.h"
#include "SingleVarDecAST.h"
#include <vector>
class VarListAST:public AST
{
public:
	vector<SingleVarDecAST*>* singleVarDecASTs;


	VarListAST(SingleVarDecAST* ast);
	virtual ~VarListAST();

	void addSingleVarDecAST(SingleVarDecAST* ast);
};

