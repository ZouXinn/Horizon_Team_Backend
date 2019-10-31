#pragma once
#include "AST.h"
#include "Enums_Structs.h"
class TypeSpecifyAST:public AST
{
public:
	int son = -1;//0 : <direct_type_specify>      1: <pointer_type_specify>
	/*
	Type type;
	//bool isArray;      这是具体VarAST决定
	//int arrayLength = 0;  这是具体VarAST决定
	std::string structName = "";//如果type是STRUCT，则存此struct的名称
	Type toType = VOID;//如果type是指针，则存指向的目标的类型
	

	VarDecAST(Type type, IdListAST* idList);
	VarDecAST(Type type, Type toType, IdListAST* idList);
	*/

	Value* codegen()
	{
		return nullptr;
	};

	virtual llvm::Type* codegenType()
	{
		return nullptr;
	};
};

