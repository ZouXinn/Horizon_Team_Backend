#pragma once
#include "StmtAST.h"
#include <string>
class ContinueStmtAST :
	public StmtAST
{
public:
	string tag = "continue";
};

