#include "ValAST.h"


ValAST::ValAST(NumberAST* numberAST)
{
	this->numberAST = numberAST;
}

ValAST::ValAST(StringAST* stringAST)
{
	this->stringAST = stringAST;
}

ValAST::ValAST(CharAST* charAST)
{
	this->charAST = charAST;
}

ValAST::~ValAST()
{
	if (this->numberAST != nullptr)
	{
		delete this->numberAST;
	}
	if (this->stringAST != nullptr)
	{
		delete this->stringAST;
	}
	if (this->charAST != nullptr)
	{
		delete this->charAST;
	}
}

Value* ValAST::codegen() {
	if (numberAST != NULL) {
		return numberAST->codegen();
	}
	if (stringAST != NULL) {

	}
	if (charAST != NULL) {

	}
	return nullptr;
}