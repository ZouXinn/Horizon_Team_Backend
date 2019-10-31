#pragma once
#include "AST.h"
//#include "VarNameAST.h"
#include "ValAST.h"
#include "OperatorAST.h"
#include "FuncCallStmtAST.h"
#include "Enums_Structs.h"
class VarNameAST;
class ExpAST :
	public AST
{
public:
	int type;//0 单目运算符   1双目运算符  2  <val>   3 <var_name>  4 <func_call_stmt>     (<exp>)不管
	ExpAST* left = nullptr;//单目运算符只用left
	ExpAST* right = nullptr;

	zx::Type expType = zx::Type::VOID;//此exp的变量类型，默认为VOID
	int pointerNum = 0;//指针数
	zx::Type finalToType = zx::Type::VOID;//最终指向的类型
	string structName = "";//如果是结构体或者FinalToType为结构体，则structName表示结构体的名称
	////////////结构体的考虑？

	ValAST* valAST = nullptr;
	VarNameAST* varNameAST = nullptr;
	OperatorAST* operatorAST = nullptr;
	FuncCallStmtAST* funcCallStmtAST = nullptr;

	ExpAST(ExpAST* left, OperatorAST* operatorAST);//单目
	ExpAST(ExpAST* left, ExpAST* right, OperatorAST* operatorAST);//双目
	ExpAST(ValAST* valAST);//<val>
	ExpAST(VarNameAST* varNameAST);//<var_name>
	ExpAST(FuncCallStmtAST* funcCallStmtAST);//<func_call_stmt>
	void setType(zx::Type expType);
	void setFinalToType(zx::Type type);
	void setPointerNum(int num);
	void setStructName(string name);
	virtual ~ExpAST();

	Value* codegen();
	string codegenStr();
};

