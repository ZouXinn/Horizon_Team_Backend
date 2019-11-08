#pragma once
#include "DecExpAST.h"
#include <string>
#include "VarListAST.h"
#include "IdentifierAST.h"

class StructDecAST :
	public DecExpAST
{
public:
	IdentifierAST* structNameIdentifier;
	VarListAST* varList;


	StructDecAST(IdentifierAST* structNameIdentifier,VarListAST* varList);
	virtual ~StructDecAST();
};

