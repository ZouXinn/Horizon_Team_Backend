#include "NumberAST.h"



NumberAST::NumberAST(IntAST* integer)
{
	this->type = 0;
	this->integer = integer;
}

NumberAST::NumberAST(RealAST* real_number)
{
	this->type = 1;
	this->real_number = real_number;
}

NumberAST::~NumberAST()
{
	if (this->integer != nullptr)
	{
		delete this->integer;
	}
	if (this->real_number != nullptr)
	{
		delete this->real_number;
	}
}

Value* NumberAST::codegen()
{
	if (integer != NULL) {
		return integer->codegen();
	}
	if (real_number != NULL) {
		return real_number->codegen();
	}
	return nullptr;
}