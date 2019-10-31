#pragma once
#include "AST.h"
#include <string>
#include "VarNameAST.h"
#include "IdentifierAST.h"
#include "IntAST.h"
#include "ExpAST.h"
#include "Enums_Structs.h"

class VarNameAST:public AST
{
public:
	int type;//0:identifier  1:<var_name> [ <exp> ]   2:<var_name> -> <var_name>  3:<var_name> . <var_name>  4:<var_name> @ * <exp>
	IdentifierAST* identifier = nullptr;
	ExpAST* intExp = nullptr;
	VarNameAST* left = nullptr;// type = 1 时用left
	VarNameAST* right = nullptr;
	ExpAST* exp = nullptr;

	//静态语义检查 start
	zx::Type varType = zx::Type::VOID;
	zx::Type finalToType = zx::Type::VOID;//如果是指针，则表示最终指向的type
	int pointerNum = 0;//如果是指针，表示指针个数
	string structName = "";//如果类型是struct或者finalToType是struct，则表示struct的名字
	bool isArray = false;
	void setType(zx::Type varType);
	void setFinalToType(zx::Type finalToType);
	void setPointerNum(int pointerNum);
	void setStructName(string name);

	//静态语义检查 end

	VarNameAST(IdentifierAST* identifier);// type = 0
	VarNameAST(VarNameAST* left, ExpAST* intExp);//type = 1
	VarNameAST(VarNameAST* left, VarNameAST* right, int type);// type = 2   3
	VarNameAST(ExpAST* exp);// type = 4
	virtual ~VarNameAST();

	Value* codegen();
	string codegenStr();
};

