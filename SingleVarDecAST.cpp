#include "SingleVarDecAST.h"

SingleVarDecAST::SingleVarDecAST(TypeSpecifyAST* typeSpecifyAST, DecVarNameAST* decVarNameAST)
{
	this->typeSpecifyAST = typeSpecifyAST;
	this->decVarNameAST = decVarNameAST;
}

SingleVarDecAST::SingleVarDecAST(TypeSpecifyAST* typeSpecifyAST, DecVarNameAST* decVarNameAST, ValAST* valAST):SingleVarDecAST(typeSpecifyAST,decVarNameAST)
{
	this->valAST = valAST;
}

SingleVarDecAST::SingleVarDecAST(TypeSpecifyAST* typeSpecifyAST, DecVarNameAST* decVarNameAST, ValListAST* valListAST): SingleVarDecAST(typeSpecifyAST, decVarNameAST)
{
	this->valListAST = valListAST;
}

SingleVarDecAST::~SingleVarDecAST()
{
	if (this->typeSpecifyAST != nullptr)
	{
		delete this->typeSpecifyAST;
	}
	if (this->decVarNameAST != nullptr)
	{
		delete this->decVarNameAST;
	}
	if (this->valAST != nullptr)
	{
		delete this->valAST;
	}
}