#pragma once
#include "AST.h"
#include "NumberAST.h"
#include <string>
#include "StringAST.h"
#include "CharAST.h"

class ValAST:public AST
{
public:
	NumberAST* numberAST = nullptr;
	StringAST* stringAST = nullptr;
	CharAST* charAST = nullptr;

	ValAST(NumberAST* numberAST);
	ValAST(StringAST* stringAST);
	ValAST(CharAST* charAST);
	virtual ~ValAST();

	Value* codegen();
};

