#pragma once
#include "AST.h"
#include "IdItemAST.h"
#include <vector>


class IdListAST:public AST
{
public:
	vector<IdItemAST*>* idItemASTs;

	IdListAST();
	IdListAST(IdItemAST* item);
	virtual ~IdListAST();
	void addIdItem(IdItemAST* item);

	virtual map<string, ID*> codegenMap();
	map<string, Value*> Map;
};

