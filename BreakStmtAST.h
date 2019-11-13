#pragma once
#include "StmtAST.h"
#include <string>
class BreakStmtAST :
	public StmtAST
{
public:
	string tag = "break";

	Value* codegen() {
		


		return nullptr;
	}
};

