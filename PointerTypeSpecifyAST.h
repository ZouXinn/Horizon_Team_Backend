#pragma once
#include "TypeSpecifyAST.h"
#include "DirectTypeSpecifyAST.h"
#include "PointerAST.h"

class PointerTypeSpecifyAST :
	public TypeSpecifyAST
{
public:
	DirectTypeSpecifyAST* directTypeSpecifyAST;
	PointerAST* pointerAST;

	PointerTypeSpecifyAST(DirectTypeSpecifyAST* directTypeSpecifyAST, PointerAST* pointerAST);
	virtual ~PointerTypeSpecifyAST();
	
};

