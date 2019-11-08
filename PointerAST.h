#pragma once
#include "AST.h"
class PointerAST:public AST
{
public:
	int starNum;
	PointerAST(int starNum);
	void addStarNum();
};

