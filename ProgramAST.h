#pragma once
#include "AST.h"
#include "DecExpAST.h"
#include "FuncDefineAST.h"

class ProgramAST :
	public AST
{
public:
	vector<DecExpAST*>* dec_exp_ASTs;//声明AST   包括变量声明和结构体声明
	vector<FuncDefineAST*>* func_define_ASTs;//函数声明AST


	ProgramAST();
	virtual ~ProgramAST();
	void addDecExpAST(DecExpAST* ast);
	void addFuncDefineAST(FuncDefineAST* ast);

	Value* codegen();
};

