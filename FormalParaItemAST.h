#pragma once
#include "AST.h"
#include "TypeSpecifyAST.h"
#include "DecVarNameAST.h"

class FormalParaItemAST:public AST
{
public:
	TypeSpecifyAST* typeSpecifyAST;
	DecVarNameAST* decVarNameAST;

	FormalParaItemAST(TypeSpecifyAST* typeSpecifyAST, DecVarNameAST* decVarNameAST);
	virtual ~FormalParaItemAST();

	llvm::Type* codegenType();
	string codegenName();
};

