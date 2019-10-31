#pragma once
#include "AST.h"
#include "IntAST.h"
#include "RealAST.h"

class NumberAST:public AST
{
public:
	int type;// 0 int    1 real
	IntAST* integer = nullptr;
	RealAST* real_number = nullptr;

	NumberAST(IntAST* integer);//type= 0
	NumberAST(RealAST* real_number);//type =1
	virtual ~NumberAST();

	Value* codegen();
};

