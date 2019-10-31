#include "VarDecAST.h"



VarDecAST::VarDecAST(TypeSpecifyAST* typeSpecifyAST, IdListAST* idListAST)
{
	this->typeSpecifyAST = typeSpecifyAST;
	this->idListAST = idListAST;
}

VarDecAST::~VarDecAST()
{
	delete this->typeSpecifyAST;
	delete idListAST;
}
Value* VarDecAST::codegen() {
	llvm::Type* a = typeSpecifyAST->codegenType();
	map<string, Value*> b = idListAST->codegenMap();
	cout << "VarDecAST" << "\n";

	map< string, Value* >::iterator iter;
	iter = b.begin();
	while (iter != b.end()) {

		/*InitializeModule();
		FunctionType* FibFTy = FunctionType::get(llvm::Type::getVoidTy(TheContext),
			{ llvm::Type::getInt32Ty(TheContext) }, false);
		Function* VarDec =
			Function::Create(FibFTy, Function::ExternalLinkage, "VarDec", *TheModule);
		BasicBlock* BB = BasicBlock::Create(TheContext, "EntryBlock", VarDec);
		Builder.SetInsertPoint(BB);*/

		Value* d = iter->second;

		llvm::Type* e = NULL;

		if (a->isIntegerTy()) {
			e = IntegerType::get(TheContext, 32);
		}
		if (a->isDoubleTy()) {
			e = llvm::Type::getDoubleTy(TheContext);
		}

		if (ConstantInt::classof(iter->second)) {
			d = ConstantInt::get(TheContext, APInt(32, cast<ConstantInt>(iter->second)->getSExtValue()));
			if (e->isDoubleTy()) {
				d = Builder.CreateSIToFP(d, llvm::Type::getDoubleTy(TheContext));
			}

		}

		if (ConstantFP::classof(iter->second)) {
			d = ConstantFP::get(TheContext, APFloat(cast<ConstantFP>(iter->second)->getValueAPF()));
			if (e->isIntegerTy()) {
				d = Builder.CreateFPToSI(d, IntegerType::get(TheContext, 32));
			}
		}



		AllocaInst* c = CreateEntryBlockAlloca(currentFun, iter->first, e);
		NamedValues[iter->first] = c;

		if (AllocaInst::classof(iter->second)) {
			Value* RVar = Builder.CreateLoad(iter->second);
			RVar->print(errs());
			cout << "\n";
			Value* LVar = Builder.CreateStore(RVar, c);
			LVar->print(errs());
			cout << "\n";
		}
		else {
			Value* g = Builder.CreateStore(d, c);
			/*Value* g = Builder.CreateStore(iter->second, c);*/
			g->print(errs());
			cout << "\n";

		}

		iter++;
	}


	return nullptr;
}