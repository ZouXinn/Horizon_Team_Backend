#pragma once
#include "TypeSpecifyAST.h"
#include "IdentifierAST.h"
#include "TypeAST.h"

class DirectTypeSpecifyAST :
	public TypeSpecifyAST
{
public:
	TypeAST* type = nullptr;
	IdentifierAST* structNameIdentifier = nullptr;//如果是结构体，则为结构体名称

	DirectTypeSpecifyAST(TypeAST* type);
	DirectTypeSpecifyAST(IdentifierAST* structNameIdentifier);

	virtual ~DirectTypeSpecifyAST();

	Value* codegen();
	llvm::Type* codegenType();
};

