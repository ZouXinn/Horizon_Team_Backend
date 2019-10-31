#pragma once
#include "AST.h"
#include "TypeSpecifyAST.h"
#include "FormalParaListAST.h"
#include "StmtsAST.h"
#include "IdentifierAST.h"

class FuncDefineAST:public AST
{
public:
	TypeSpecifyAST* typeSpecifyAST;
	FormalParaListAST* formalParaListAST;
	StmtsAST* stmtsAST;
	IdentifierAST* funcNameIdentifier;

	FuncDefineAST(TypeSpecifyAST* typeSpecifyAST, FormalParaListAST* formalParaListAST, StmtsAST* stmtsAST, IdentifierAST* funcNameIdentifier);
	~FuncDefineAST();

	Value* codegen();
};

