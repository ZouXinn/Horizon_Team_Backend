#pragma once
#include "AST.h"
class IntAST :
	public AST
{
public:
	int integer;
	IntAST(int integer)
	{
		this->integer = integer;
	};

	Value* codegen()
	{
		return ConstantInt::get(TheContext, APInt(32, integer));
	};
};

