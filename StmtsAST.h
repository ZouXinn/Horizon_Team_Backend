#pragma once
#include "AST.h"
#include "StmtAST.h"
#include <vector>

class StmtsAST :
	public AST
{
public:
	vector<StmtAST*>* stmtASTs;

	StmtsAST();
	virtual ~StmtsAST();

	void addStmtAST(StmtAST* ast);

	Value* codegen();
};

