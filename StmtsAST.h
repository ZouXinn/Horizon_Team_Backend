#pragma once
#include "AST.h"
#include "StmtAST.h"
#include <vector>

class StmtsAST :
	public AST
{
public:
	vector<StmtAST*>* stmtASTs;
	int level = 0;

	StmtsAST();
	virtual ~StmtsAST();

	void addStmtAST(StmtAST* ast);

	Value* codegen();
	void setLevel(int lv) {
		this->level = lv;
	}
};

