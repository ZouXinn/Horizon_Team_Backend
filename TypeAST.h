#pragma once
#include "AST.h"
#include "Enums_Structs.h"
using namespace zx;
class TypeAST :
	public AST
{
public:
	zx::Type type;
	TypeAST(zx::Type type)
	{
		this->type = type;
	}

	llvm::Type* codegenType()
	{
		switch (type)
		{
		case zx::Type::INT:
			/*return llvm::Type::getInt32Ty(TheContext);*/
			return llvm::IntegerType::get(TheContext, 32);
		case zx::Type::REAL:
			return llvm::Type::getDoubleTy(TheContext);
		case zx::Type::VOID:
			return llvm::Type::getVoidTy(TheContext);
		case zx::Type::CHAR:
			return llvm::Type::getInt8Ty(TheContext);
		case zx::Type::STRING:
			return llvm::Type::getInt8PtrTy(TheContext);
		default:
			break;
		}
		return nullptr;
	}
};

