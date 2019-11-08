#include "VarListAST.h"
VarListAST::VarListAST(SingleVarDecAST* ast)
{
	this->singleVarDecASTs = new vector<SingleVarDecAST*>();
	addSingleVarDecAST(ast);
}

VarListAST::~VarListAST()
{
	if (this->singleVarDecASTs != nullptr)
	{
		for (int i = 0; i < this->singleVarDecASTs->size(); i++)
		{
			delete this->singleVarDecASTs->at(i);
		}
		delete this->singleVarDecASTs;
	}
}

void VarListAST::addSingleVarDecAST(SingleVarDecAST* ast)
{
	if (this->singleVarDecASTs != nullptr)
	{
		this->singleVarDecASTs->push_back(ast);
	}
}