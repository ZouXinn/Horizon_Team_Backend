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

	//处理重名函数问题 start    zx
	string FN = currentFun->getName();
	string MyPR = "";
	for (int i = 0; i < params.size(); i++) {
		Type* type = params[i];
		if (type->isIntegerTy()) {
			MyPR += 'i';
		}
		else if (type->isDoubleTy()) {
			MyPR += 'r';
		}
	}
	FunIndex funIndex;
	funIndex.FN = Name;
	funIndex.PR = MyPR;
	FuncNames[funIndex] = FN;
	Funcs[FN] = currentFun;
	//处理重名函数问题 end

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



	for (auto& Arg : currentFun->args()) {
		int i = this->formalParaListAST->formalParaItemASTs->size() - 1 - Arg.getArgNo();
		string name = this->formalParaListAST->formalParaItemASTs->at(i)->codegenName();
		string pName = name + ".addr";
		AllocaInst* Alloca = CreateEntryBlockAlloca(currentFun, pName, Arg.getType());
		Params[name] = &Arg;
		Params[pName] = Alloca;
		Builder.CreateStore(Params[name], Params[pName]);
	}
	

	//liu start
	if (Value* RetVal = stmtsAST->codegen()) {

		//Builder.CreateRet(RetVal);

		

		//冯文翰于11.12日 19：03修改
		//参考https://stackoverflow.com/questions/53632131/compiler-how-to-check-a-user-function-returns-properly
		//当stmts生成完时，检查每一个BaickBlock，如果没有终结标志就添加ret
		//对于使用者写的ret太多的情况，再stmtsAST和if,while语句中分别处理，使得遇到第一个return后便不再继续生成IR代码



		Type* retType = FT->getReturnType();
		for (BasicBlock& BB : currentFun->getBasicBlockList()) {
			Instruction* Terminator = BB.getTerminator();
			if (Terminator != nullptr) continue; /// Well-formed
			if (retType->isVoidTy()) {
				/// Make implicit return of void Function explicit.
				Builder.SetInsertPoint(&BB);
				Builder.CreateRetVoid();
			}
			else if (retType->isIntegerTy()){
				Builder.SetInsertPoint(&BB);
				Builder.CreateRet(ConstantInt::get(IntegerType::get(TheContext, 32), APInt(32, 0)));
			}
			else if (retType->isDoubleTy())
			{
				Builder.SetInsertPoint(&BB);
				Builder.CreateRet(ConstantFP::get(TheContext, APFloat(0.0)));
			}
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

