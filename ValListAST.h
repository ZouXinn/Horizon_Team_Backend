#pragma once
#include "AST.h"
#include "ValAST.h"
#include <vector>
class ValListAST :
	public AST
{
public:
	vector<ValAST*>* vals = nullptr;

	bool isInt = false;
	bool isReal = false;

	ValListAST(ValAST* val);
	void addVal(ValAST* val);

	virtual ~ValListAST();
};

