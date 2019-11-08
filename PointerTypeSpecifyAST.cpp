#include "PointerTypeSpecifyAST.h"

PointerTypeSpecifyAST::PointerTypeSpecifyAST(DirectTypeSpecifyAST* directTypeSpecifyAST, PointerAST* pointerAST)
{
	this->directTypeSpecifyAST = directTypeSpecifyAST;
	this->pointerAST = pointerAST;
}

PointerTypeSpecifyAST::~PointerTypeSpecifyAST()
{
	delete this->directTypeSpecifyAST;
	delete this->pointerAST;
}

