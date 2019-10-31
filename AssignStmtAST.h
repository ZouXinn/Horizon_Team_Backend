#pragma once
#include "StmtAST.h"
#include "VarNameAST.h"
#include "ExpAST.h"
#include "OperatorAST.h"

class AssignStmtAST :
	public StmtAST
{
public:
	VarNameAST* varNameAST;

	int type;//0:<var_name> ++  <var_name> --    1:<var_name> = <exp> ;  

	OperatorAST* operatorAST = nullptr;
	ExpAST* expAST = nullptr;

	AssignStmtAST(VarNameAST* varNameAST, OperatorAST* operatorAST);//type = 0    operatorASTÎª++»ò--
	AssignStmtAST(VarNameAST* varNameAST, ExpAST* expAST);//type = 1
	
	virtual ~AssignStmtAST();
	Value* codegen();
};

