#pragma once
#include "AST.h"
enum Op { ADD, MINUS, STAR, DIVIDE, MODE, MORE, LESS, MOREOREQ, LESSOREQ, EQ, NEQ, AND, OR, NOT, PLUSPLUS,
	MINUSMINUS,DOT,POINTERSIGN};
class OperatorAST :
	public AST
{
public:
	Op op;
	OperatorAST(Op op);
};

