#pragma once
#include "StmtAST.h"
#include "ExpAST.h"
#include "StmtsAST.h"


class WhileStmtAST :
	public StmtAST
{
public:
	ExpAST* expAST;
	StmtsAST* stmtsAST;

	WhileStmtAST(ExpAST* expAST, StmtsAST* stmtsAST);
	virtual ~WhileStmtAST();

	Value* codegen();
};

