#pragma once
#include "AST.h"
class CharAST :
	public AST
{
public:
	char Achar;
	CharAST(char Achar)
	{
		this->Achar = Achar;
	};
};

