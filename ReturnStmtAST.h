#pragma once
#include "StmtAST.h"
#include "ExpAST.h"
#include <string>

class ReturnStmtAST :
	public StmtAST
{
public:
	string tag = "return";
	ExpAST* expAST=nullptr;
	
	ReturnStmtAST();
	ReturnStmtAST(ExpAST* expAST);
	virtual ~ReturnStmtAST();
};

