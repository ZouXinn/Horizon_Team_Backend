#pragma once
#include "StmtAST.h"
#include "RealParaListAST.h"
#include "IdentifierAST.h"
#include "TypeSpecifyAST.h"

class FuncCallStmtAST :
	public StmtAST
{
public:
	IdentifierAST* identifier;
	RealParaListAST* realParaListAST;

	TypeSpecifyAST* retTypeSpecify = nullptr;
	FuncCallStmtAST(IdentifierAST* identifier, RealParaListAST* realParaListAST);
	virtual ~FuncCallStmtAST();
};

