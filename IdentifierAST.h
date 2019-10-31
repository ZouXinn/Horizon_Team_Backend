#pragma once
#include "AST.h"
#include <string>
class IdentifierAST :
	public AST
{
public:
	string identifier;
	IdentifierAST(string identifier)
	{
		this->identifier = identifier;
	};


	string codegenStr()
	{
		return identifier;
	};
};

