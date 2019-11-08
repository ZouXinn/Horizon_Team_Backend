#include "RealParaListAST.h"


RealParaListAST::RealParaListAST()
{
	this->realParaItemASTs = new vector<RealParaItemAST*>();
}

RealParaListAST::RealParaListAST(RealParaItemAST* ast):RealParaListAST()
{
	addRealParaListAST(ast);
}

RealParaListAST::~RealParaListAST()
{
	if (this->realParaItemASTs != nullptr)
	{
		for (int i = 0; i < this->realParaItemASTs->size(); i++)
		{
			delete this->realParaItemASTs->at(i);
		}
		delete this->realParaItemASTs;
	}
}

void RealParaListAST::addRealParaListAST(RealParaItemAST* ast)
{
	this->realParaItemASTs->push_back(ast);
}