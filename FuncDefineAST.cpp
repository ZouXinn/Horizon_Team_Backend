#include "FuncDefineAST.h"


FuncDefineAST::FuncDefineAST(TypeSpecifyAST* typeSpecifyAST, FormalParaListAST* formalParaListAST, StmtsAST* stmtsAST, IdentifierAST* funcNameIdentifier)
{
	this->typeSpecifyAST = typeSpecifyAST;
	this->formalParaListAST = formalParaListAST;
	this->stmtsAST = stmtsAST;
	this->funcNameIdentifier = funcNameIdentifier;
}
FuncDefineAST::~FuncDefineAST()
{
	if (this->formalParaListAST != nullptr)
	{
		delete this->formalParaListAST;
	}
	if (this->typeSpecifyAST != nullptr)
	{
		delete this->typeSpecifyAST;
	}
	if (this->stmtsAST != nullptr)
	{
		delete this->stmtsAST;
	}
	if (this->funcNameIdentifier != nullptr)
	{
		delete this->funcNameIdentifier;
	}
}

Value* FuncDefineAST::codegen() {

	//
	//FWH
	//

	/*currentFun = Builder.GetInsertBlock()->getParent();
	Value* Val = stmtsAST->codegen();
	cout << "\nFuncDefineAST\n";
	verifyFunction(*currentFun);
	currentFun->print(errs());
	return Val;
	return nullptr;*/

	//
	//LBQ
	//
	Type* type = typeSpecifyAST->codegenType();
	string Name = funcNameIdentifier->codegenStr();


	std::vector<Type*> params;
	for (int i = this->formalParaListAST->formalParaItemASTs->size() - 1; i >= 0; i--) {
		Type* a = this->formalParaListAST->formalParaItemASTs->at(i)->codegenType();
		if (a->isDoubleTy()) {
			params.push_back(Type::getDoubleTy(TheContext));
		}
		if (a->isIntegerTy()) {
			params.push_back(Type::getInt32Ty(TheContext));
		}
	}

	FunctionType* FT;
	if (type->isDoubleTy()) {
		FT = FunctionType::get(Type::getDoubleTy(TheContext), params, false);
	}
	if (type->isIntegerTy()) {
		FT = FunctionType::get((Type*)Type::getInt32Ty(TheContext), params, false);
	}

	currentFun = Function::Create(FT, Function::ExternalLinkage, Name, TheModule.get());
	/*currentFun = Function::Create(FT, Function::ExternalLinkage, Name, TheModule.get());*/


	unsigned Idx = 0;
	/*for (auto& Arg : F->args()) {*/
	for (auto& Arg : currentFun->args()) {
		int i = this->formalParaListAST->formalParaItemASTs->size() - 1 - Arg.getArgNo();
		string name = this->formalParaListAST->formalParaItemASTs->at(i)->codegenName();
		Arg.setName(name);
	}

	// Create a new basic block to start insertion into.
	BasicBlock* BB = BasicBlock::Create(TheContext, "entry", currentFun);
	/*BasicBlock* BB = BasicBlock::Create(TheContext, "entry", currentFun);*/
	Builder.SetInsertPoint(BB);


	if (Value* RetVal = stmtsAST->codegen()) {

		Builder.CreateRet(RetVal);

		currentFun = Builder.GetInsertBlock()->getParent();

		verifyFunction(*currentFun);
		currentFun->print(errs());
		currentFun = NULL;
		return nullptr;
	}

	//// Error reading body, remove function.
	currentFun->eraseFromParent();
	return nullptr;
}