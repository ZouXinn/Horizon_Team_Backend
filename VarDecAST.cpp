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
	llvm::Type* type = typeSpecifyAST->codegenType();
	map<string, ID*> b = idListAST->codegenMap();
	cout << "VarDecAST" << "\n";

	map< string, ID* >::iterator iter;
	iter = b.begin();
	while (iter != b.end()) {

		/*InitializeModule();
		FunctionType* FibFTy = FunctionType::get(llvm::Type::getVoidTy(TheContext),
			{ llvm::Type::getInt32Ty(TheContext) }, false);
		Function* VarDec =
			Function::Create(FibFTy, Function::ExternalLinkage, "VarDec", *TheModule);
		BasicBlock* BB = BasicBlock::Create(TheContext, "EntryBlock", VarDec);
		Builder.SetInsertPoint(BB);*/

		ID* d = iter->second;
		llvm::Type* e = NULL;

		if (type->isIntegerTy()) {//是整数
			e = IntegerType::get(TheContext, 32);
		}
		else if (type->isDoubleTy()) {//是real
			e = llvm::Type::getDoubleTy(TheContext);
		}
		//else if()

		if (d->isArray) {//是数组
			llvm::ArrayType* arrType = nullptr;
			llvm::Type* elementType = nullptr;
			if (type->isIntegerTy()) {//是整数
				elementType = IntegerType::get(TheContext, 32);
			}
			else if (type->isDoubleTy()) {//是real
				elementType = llvm::Type::getDoubleTy(TheContext);
			}
			//else if()

			int elementNum = d->elementNum;
			arrType = ArrayType::get(elementType, elementNum);
			
		}
		else {//不是数组
			if (d->valueVector.size() == 0) {//没有为变量赋值
				if (type->isIntegerTy()) {

				}
				else if (type->isDoubleTy()) {

				}
				//else if(type->is)
			}
			else {
				Value* Val = (d->valueVector)[0];
				if (ConstantInt::classof(Val)) {
					Val = ConstantInt::get(TheContext, APInt(32, cast<ConstantInt>(Val)->getSExtValue()));
					if (e->isDoubleTy()) {
						Val = Builder.CreateSIToFP(Val, llvm::Type::getDoubleTy(TheContext));
					}

				}
				
				if (ConstantFP::classof(Val)) {
					Val = ConstantFP::get(TheContext, APFloat(cast<ConstantFP>(Val)->getValueAPF()));
					if (e->isIntegerTy()) {
						Val = Builder.CreateFPToSI(Val, IntegerType::get(TheContext, 32));
					}
				}

				AllocaInst* c = CreateEntryBlockAlloca(currentFun, iter->first, e);
				NamedValues[iter->first] = c;

				if (AllocaInst::classof(Val)) {
					Value* RVar = Builder.CreateLoad(Val);
					RVar->print(errs());
					cout << "\n";
					Value* LVar = Builder.CreateStore(RVar, c);
					LVar->print(errs());
					cout << "\n";
				}
				else {
					Value* g = Builder.CreateStore(Val, c);
					/*Value* g = Builder.CreateStore(iter->second, c);*/
					g->print(errs());
					cout << "\n";
				}
			}
		}
		iter++;
	}
	return nullptr;
}