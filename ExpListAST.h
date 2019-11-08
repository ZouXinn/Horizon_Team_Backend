#pragma once
#include "AST.h"
#include "ExpAST.h"
#include <vector>
class ExpListAST :
	public AST
{
private:
	void setTypes(ExpAST* exp);
public:
	vector<ExpAST*>* exps = nullptr;

	zx::Type expType = zx::Type::VOID;//此exp的变量类型，默认为VOID
	int pointerNum = 0;//指针数
	zx::Type finalToType = zx::Type::VOID;//最终指向的类型
	string structName = "";//如果是结构体或者FinalToType为结构体，则structName是结构体名称

	ExpListAST(ExpAST* exp);
	void addExp(ExpAST* exp);
	bool check(ExpAST* exp);//检查exp是否和list类型相符
	virtual ~ExpListAST();
};

