#include "FormalParaListAST.h"


FormalParaListAST::FormalParaListAST()
{
	this->formalParaItemASTs = new vector<FormalParaItemAST*>();
}

FormalParaListAST::~FormalParaListAST()
{
	for (int i = 0; i < this->formalParaItemASTs->size(); i++)
	{
		delete this->formalParaItemASTs->at(i);
	}
	delete this->formalParaItemASTs;
}

void FormalParaListAST::addFormalParaItemAST(FormalParaItemAST* ast)
{
	this->formalParaItemASTs->push_back(ast);
}