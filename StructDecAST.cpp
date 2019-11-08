#include "StructDecAST.h"



StructDecAST::StructDecAST(IdentifierAST* structNameIdentifier, VarListAST* varList)
{
	this->varList = varList;
	this->structNameIdentifier = structNameIdentifier;
}

StructDecAST::~StructDecAST()
{
	if (this -> structNameIdentifier != nullptr)
	{
		delete this->structNameIdentifier;
	}
	
	/*if (this->varList != nullptr)    //ÄªÃûÆäÃî±¨´í
	{
		delete this->varList;
	}*/
}

