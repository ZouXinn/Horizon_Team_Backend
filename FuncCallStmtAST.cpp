#include "FuncCallStmtAST.h"


FuncCallStmtAST::FuncCallStmtAST(IdentifierAST* identifier, RealParaListAST* realParaListAST)
{
	this->identifier = identifier;
	this->realParaListAST = realParaListAST;
}

FuncCallStmtAST::~FuncCallStmtAST()
{
	if (this->realParaListAST != nullptr)
	{
		delete this->realParaListAST;
	}
	if (this->identifier != nullptr)
	{
		delete this->identifier;
	}
}

Value* FuncCallStmtAST::codegen()
{
	string a = identifier->codegenStr();
 
	std::vector<Type*> params;
	Function* func = Function::Create(FunctionType::get((Type*)Type::getInt32Ty(TheContext), params, false), Function::ExternalLinkage, "fun0", TheModule.get());
	Value* Val = Builder.CreateCall(func, ArrayRef<Value*>());

	return nullptr;
}
