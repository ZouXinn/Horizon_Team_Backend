#include "IdItemAST.h"

IdItemAST::IdItemAST(DecVarNameAST* decVarNameAST)
{
	this->decVarNameAST = decVarNameAST;
}

IdItemAST::IdItemAST(DecVarNameAST* decVarNameAST, ExpAST* exp)
{
	this->decVarNameAST = decVarNameAST;
	this->exp = exp;
}

IdItemAST::IdItemAST(DecVarNameAST* decVarNameAST, ExpListAST* exps)
{
	this->decVarNameAST = decVarNameAST;
	this->exps = exps;
}

IdItemAST::~IdItemAST()
{
	delete this->decVarNameAST;
	if (this->exp != nullptr)
	{
		delete this->exp;
	}
	if (this->exps != nullptr)
	{
		delete this->exps;
	}
}

string IdItemAST::codegenStr()
{
	return string();
}

vector<string> IdItemAST::codegenStrVec()
{
	return vector<string>();
}

//map<string, Value*>* IdItemAST::codegenMap()
//{
//	return nullptr;
//}

pair<string, Value*> IdItemAST::codegenPair() {
	pair<string, Value*> a;
	if (!decVarNameAST->LSRS) {
		a.first = decVarNameAST->codegenStr();
	}
	if (exp != NULL) {
		a.second = exp->codegen();
	}
	if (exps != NULL) {

	}
	return a;
}