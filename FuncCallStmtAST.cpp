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
	/*string a = identifier->codegenStr();
	
	std::vector<Type*> params;
	
	Function* func = Function::Create(FunctionType::get((Type*)Type::getInt32Ty(TheContext), params, false), Function::ExternalLinkage, a, TheModule.get());
	Value* Val = Builder.CreateCall(func, ArrayRef<Value*>());*/
	//Function::Create()

	//zx start
	string funcName = identifier->codegenStr();
	if (funcName == "read" || funcName == "write") {

	}
	else {//其他函数正常调用
		Function* func = TheModule->getFunction(funcName);
		//ArrayRef<Value*> rparams = this->realParaListAST->codegenArr();
		vector<Value*> rparamsVec = this->realParaListAST->codegenVec();
		Value* Val = Builder.CreateCall(func, rparamsVec, "calltmp");
	}
	return nullptr;
}
