#pragma once
#include "AST.h"
#include "TypeSpecifyAST.h"
#include "DecVarNameAST.h"
#include "ValListAST.h"
#include "ValAST.h"


class SingleVarDecAST:public AST
{
public:
	TypeSpecifyAST* typeSpecifyAST;
	DecVarNameAST* decVarNameAST;

	ValAST* valAST = nullptr;//Ä¬ÈÏÃ»ÓÐ
	ValListAST* valListAST = nullptr;

	SingleVarDecAST(TypeSpecifyAST* typeSpecifyAST, DecVarNameAST* decVarNameAST);
	SingleVarDecAST(TypeSpecifyAST* typeSpecifyAST, DecVarNameAST* decVarNameAST, ValAST* valAST);
	SingleVarDecAST(TypeSpecifyAST* typeSpecifyAST, DecVarNameAST* decVarNameAST, ValListAST* valListAST);
	virtual ~SingleVarDecAST();
};

