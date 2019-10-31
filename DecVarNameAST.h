#pragma once
#include "AST.h"
#include<string>
#include "IdentifierAST.h"
#include "IntAST.h"
class DecVarNameAST:public AST
{
public:
	IdentifierAST* identifier;
	bool LSRS = false;//true 表示 a[]
	IntAST* intAST = nullptr;//非nullptr表示   a[integer]
	DecVarNameAST(IdentifierAST* identifier);
	DecVarNameAST(IdentifierAST* identifier, bool LSRS);
	DecVarNameAST(IdentifierAST* identifier, bool LSRS, IntAST* intAST);
	virtual ~DecVarNameAST();
	string codegenStr();
};

