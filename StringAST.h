#pragma once
#include "AST.h"
#include <string>
class StringAST :
	public AST
{
public:
	string theString;
	StringAST(string theString)
	{
		this->theString = theString;
	};
};

