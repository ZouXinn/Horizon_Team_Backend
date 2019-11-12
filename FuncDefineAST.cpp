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
	else if (type->isIntegerTy()) {
		FT = FunctionType::get((Type*)Type::getInt32Ty(TheContext), params, false);
	}
	else if (type->isVoidTy()) {
		FT = FunctionType::get((Type*)Type::getVoidTy(TheContext), params, false);
	}

	currentFun = Function::Create(FT, Function::ExternalLinkage, Name, TheModule.get());
	/*currentFun = Function::Create(FT, Function::ExternalLinkage, Name, TheModule.get());*/


	unsigned Idx = 0;
	/*for (auto& Arg : F->args()) {*/


	for (auto& Arg : currentFun->args()) {
		int i = this->formalParaListAST->formalParaItemASTs->size() - 1 - Arg.getArgNo();
		string name = this->formalParaListAST->formalParaItemASTs->at(i)->codegenName();
		Arg.setName(name);
		//AllocaInst* Alloca = CreateEntryBlockAlloca(currentFun, Arg.getName());  // toy4   1077 row
		Params[name] = &Arg;
	}

	// Create a new basic block to start insertion into.
	BasicBlock* BB = BasicBlock::Create(TheContext, "entry", currentFun);
	/*BasicBlock* BB = BasicBlock::Create(TheContext, "entry", currentFun);*/
	Builder.SetInsertPoint(BB);

	//Args Handle  zx
	for (auto& Arg : currentFun->args()) {
		//Builder.CreateAddrSpaceCast()
		int i = this->formalParaListAST->formalParaItemASTs->size() - 1 - Arg.getArgNo();
		string name = this->formalParaListAST->formalParaItemASTs->at(i)->codegenName();
		//Arg.setName(name);
		//AllocaInst* Alloca = CreateEntryBlockAlloca(currentFun, Arg.getName());  // toy4   1077 row
		string rname = name + ".addr";
		AllocaInst* Alloca = CreateEntryBlockAlloca(currentFun, rname,Arg.getType());
		Builder.CreateStore(Params[name], Alloca, false);
		Params[rname] = Alloca;
	}



	//liu start
	if (Value* RetVal = stmtsAST->codegen()) {

		//Builder.CreateRet(RetVal);
		Type* retType = FT->getReturnType();
		if (retType->isIntegerTy()) {
			Builder.CreateRet(ConstantInt::get(IntegerType::get(TheContext, 32), APInt(32, 0)));
		}
		else if (retType->isDoubleTy()) {
			Builder.CreateRet(ConstantFP::get(TheContext, APFloat(0.0)));
		}
		else if (retType->isVoidTy()) {
			Builder.CreateRetVoid();
		}
		currentFun = Builder.GetInsertBlock()->getParent();

		verifyFunction(*currentFun);
		currentFun->print(errs());
		currentFun = NULL;
		return nullptr;
	}
	//liu end

	//zx start
	/*stmtsAST->codegen();
	currentFun = Builder.GetInsertBlock()->getParent();

	verifyFunction(*currentFun);
	currentFun->print(errs());
	currentFun = NULL;
	return nullptr;*/

	//zx end


	//// Error reading body, remove function.
	currentFun->eraseFromParent();
	return nullptr;
}