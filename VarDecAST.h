#pragma once
#include "DecExpAST.h"
#include <string>
#include "Enums_Structs.h"
#include "IdListAST.h"
#include "TypeSpecifyAST.h"

class VarDecAST:public DecExpAST
{
public:
	TypeSpecifyAST* typeSpecifyAST;
	IdListAST* idListAST;
	int level = 0;
	VarDecAST(TypeSpecifyAST* typeSpecifyAST, IdListAST* idListAST);
	virtual ~VarDecAST();

	Value* codegen();

	void setLevel(int lv) {
		this->level = lv;
	}
};

