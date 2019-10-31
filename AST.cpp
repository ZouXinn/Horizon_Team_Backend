#include "AST.h"


AST::~AST()
{
	
}

void AST::setRow(int row)
{
	this->row = row;
}

Value* AST::codegen()
{
	return nullptr;
}

ArrayRef<Value*> AST::codegenArr()
{
	return nullptr;
}