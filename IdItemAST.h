#pragma once
#include "AST.h"
#include "DecVarNameAST.h"
#include "ExpListAST.h"


struct ID {
	bool isArray = false;
	vector<Value*> valueVector;
	vector<Value*> notConstantVector;
	vector<int> notConstantIndex;
	int elementNum = 0;
	ID(bool isArray, vector<Value*> valueVector) {
		this->isArray = isArray;
		this->valueVector = valueVector;
	}
	ID(bool isArray) {
		this->isArray = isArray;
	}
};

class IdItemAST:public AST
{
public:
	DecVarNameAST* decVarNameAST;
	//以下两个变量必有其一为空
	ExpAST* exp = nullptr;//单个值
	ExpListAST* exps = nullptr;//多个值

	IdItemAST(DecVarNameAST* decVarNameAST);
	IdItemAST(DecVarNameAST* decVarNameAST, ExpAST* exp);
	IdItemAST(DecVarNameAST* decVarNameAST, ExpListAST* exps);
	virtual ~IdItemAST();

	string codegenStr();
	vector<string> codegenStrVec();
	/*map<string, Value*>* codegenMap();*/
	pair<string, ID*> codegenPair();
};

