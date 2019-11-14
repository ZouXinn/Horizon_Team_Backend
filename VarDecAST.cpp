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
	//cout << "VarDecAST" << "\n";

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

			int elementNum = d->elementNum;
			vector<llvm::Value*> valueVector = d->valueVector;//已经排好了顺序的
			vector<llvm::Value*> noneConstVector = d->notConstantVector;
			vector<int> noneConstIndex = d->notConstantIndex;

			llvm::ArrayRef<llvm::Constant*> V;// = llvm::ArrayRef<llvm::Constant*>;
			if (type->isIntegerTy()) {//是整数
				elementType = IntegerType::get(TheContext, 32);
			}
			else if (type->isDoubleTy()) {//是real
				elementType = llvm::Type::getDoubleTy(TheContext);
			}
			//else if()

			//获得数组类型
			arrType = ArrayType::get(elementType, elementNum);


			VectorType* vectorType = VectorType::get(elementType, elementNum);

			//Builder.createG
			
			//初值 start  -----> 获得 llvm::ArrayRef<llvm::Constant*> 类型

			//是否赋初值？
			vector<llvm::Constant*> constVector;
			if (valueVector.empty()) {//如果没赋初值
				for (int i = 0; i < elementNum; i++) {
					Constant* defaultVal;
					if (type->isIntegerTy()) {//如果是int
						defaultVal = ConstantInt::get(TheContext, APInt(32, 0));
					}
					else if (type->isDoubleTy()) {//如果是real
						defaultVal = ConstantFP::get(TheContext, APFloat(0.0));
					}
					//else if()
					constVector.push_back(defaultVal);
				}
			}
			else {//如果赋了初值
				for (int i = 0; i < valueVector.size(); i++) {
					Value* eleVal = valueVector[i];
					if (ConstantInt::classof(eleVal) && !GlobalVariable::classof(eleVal)) {
						eleVal = ConstantInt::get(TheContext, APInt(32, cast<ConstantInt>(eleVal)->getSExtValue()));
						if (e->isDoubleTy()) {
							eleVal = Builder.CreateSIToFP(eleVal, llvm::Type::getDoubleTy(TheContext));
						}

					}
					else if (ConstantFP::classof(eleVal)&& !GlobalVariable::classof(eleVal)) {
						eleVal = ConstantFP::get(TheContext, APFloat(cast<ConstantFP>(eleVal)->getValueAPF()));
						if (e->isIntegerTy()) {
							eleVal = Builder.CreateFPToSI(eleVal, IntegerType::get(TheContext, 32));
						}
					}
					//else if()
					constVector.push_back((Constant*)eleVal);
				}
			}
			
			V = llvm::ArrayRef<llvm::Constant*>(constVector);
			//初值 end

			//llvm::Array
			//llvm::ConstantArray::get(arrType,V);

			//Constant* arrConstant = llvm::ConstantArray::get(arrType, V);


			Constant* constantVector = llvm::ConstantVector::get(V);
			
			//申请内存？
			//AllocaInst* c = CreateEntryBlockAlloca(currentFun, iter->first, arrType);
			
			if (this->level == 0) {
				//GlobalValues[iter->first] = c;
				GlobalVariable* gv = new GlobalVariable(*TheModule, vectorType, false, GlobalValue::PrivateLinkage, constantVector, iter->first);
				gv->print(errs());
				cout << endl;
				GV[iter->first] = gv;
				if (noneConstIndex.size() > 0) {//不会出现
					Value* vectorValue = Builder.CreateLoad(gv);
					//vectorValue = Builder.CreateLoad(gv);
					vectorValue->print(errs()); cout << endl;
					for (int i = 0; i < noneConstIndex.size(); i++) {
						Value* noneConstValue = noneConstVector[i];
						if (AllocaInst::classof(noneConstValue) || GlobalVariable::classof(noneConstValue)) {
							noneConstValue = Builder.CreateLoad(noneConstValue);
						}
						int index = noneConstIndex[i];
						if (noneConstValue->getType()->isIntegerTy() && elementType->isDoubleTy()) {//如果需要类型转换，则进行类型转换
							noneConstValue = Builder.CreateSIToFP(noneConstValue, Type::getDoubleTy(TheContext));
						}
						vectorValue = Builder.CreateInsertElement(vectorValue, noneConstValue, (uint64_t)index);
						vectorValue->print(errs()); cout << endl;
					}
					Builder.CreateStore(vectorValue, gv);
				}
			}
			else {
				AllocaInst* c = CreateEntryBlockAlloca(currentFun, iter->first, vectorType);
				NamedValues[iter->first] = c;
				//在内存中装载相应的值
				//Value* g = Builder.CreateStore(arrConstant, c);
				Value* g = Builder.CreateStore(constantVector, c);
				if (noneConstIndex.size() > 0) {
					Value* vectorValue = Builder.CreateLoad(c);
					for (int i = 0; i < noneConstIndex.size(); i++) {
						Value* noneConstValue = noneConstVector[i];
						if (AllocaInst::classof(noneConstValue)||GlobalVariable::classof(noneConstValue)) {
							noneConstValue = Builder.CreateLoad(noneConstValue);
						}
						int index = noneConstIndex[i];
						if (noneConstValue->getType()->isIntegerTy() && elementType->isDoubleTy()) {//如果需要类型转换，则进行类型转换
							noneConstValue = Builder.CreateSIToFP(noneConstValue, Type::getDoubleTy(TheContext));
						}
						vectorValue = Builder.CreateInsertElement(vectorValue, noneConstValue, (uint64_t)index);
					}
					Builder.CreateStore(vectorValue, c);
				}
			}
			
			
			
		}
		else {//不是数组
			if (d->valueVector.size() == 0) {//没有为变量赋值
				Value* defaultVal = nullptr;
				if (type->isIntegerTy()) {//如果是int a;
					defaultVal = ConstantInt::get(TheContext, APInt(32, 0));
					
				}
				else if (type->isDoubleTy()) {//real a;
					defaultVal = ConstantFP::get(TheContext, APFloat(0.0));
				}
				//else if(type->is)

				
				if (this->level == 0) {
					//GlobalVariable* gv = new GlobalVariable(type, false, GlobalValue::LinkageTypes::ExternalLinkage, (Constant*)defaultVal,iter->first);
					GlobalVariable* gv = new GlobalVariable(*TheModule, type, false, GlobalValue::PrivateLinkage, (Constant*)defaultVal,iter->first);
					//GlobalValues[iter->first] = c;
					//CreateGlabol

					gv->print(errs());
					cout << endl;
					GV[iter->first] = gv;
				}
				else {
					AllocaInst* c = CreateEntryBlockAlloca(currentFun, iter->first, e);
					NamedValues[iter->first] = c;
					Value* g = Builder.CreateStore(defaultVal, c);
					g->print(errs());
				}

				/*Value* g = Builder.CreateStore(defaultVal, c);
				g->print(errs());*/
			}
			else {
				Value* Val = (d->valueVector)[0];
				if (ConstantInt::classof(Val)&&!GlobalVariable::classof(Val)) {//如果初值是int
					Val = ConstantInt::get(TheContext, APInt(32, cast<ConstantInt>(Val)->getSExtValue()));
					if (e->isDoubleTy()) {//申请的变量是real
						Val = Builder.CreateSIToFP(Val, llvm::Type::getDoubleTy(TheContext));
					}

				}
				
				if (ConstantFP::classof(Val) && !GlobalVariable::classof(Val)) {//如果初值是real
					Val = ConstantFP::get(TheContext, APFloat(cast<ConstantFP>(Val)->getValueAPF()));
					if (e->isIntegerTy()) {//申请的变量是int   ： 应该不会走这一步
						Val = Builder.CreateFPToSI(Val, IntegerType::get(TheContext, 32));
					}
				}

				
				if (this->level == 0) {
					if (GlobalVariable::classof(Val)||!Constant::classof(Val)) {//不会出现
						//GlobalVariable* gv = nullptr;
						//Constant* c = ((GlobalVariable*)Val)->getInitializer();
						//gv = new GlobalVariable(*TheModule, type, false, GlobalValue::PrivateLinkage, c, iter->first);
						//gv->print(errs()); cout << endl;
						////Value* g = Builder.CreateStore(Val, gv);
						////g->print(errs()); cout << endl;
						//GV[iter->first] = gv;
						//throw Exception(DynamicSemaEx, this->row, "全局变量只能用常量初始化!");
					}
					else {
						GlobalVariable* gv = new GlobalVariable(*TheModule, type, false, GlobalValue::PrivateLinkage, (Constant*)Val, iter->first);
						gv->print(errs());
						cout << endl;
						GV[iter->first] = gv;
					}
					
					//GlobalValues[iter->first] = c;
				}
				else {
					AllocaInst* c = CreateEntryBlockAlloca(currentFun, iter->first, e);
					NamedValues[iter->first] = c;
					if (AllocaInst::classof(Val)) {//如果是 int a = b
						Value* RVar = Builder.CreateLoad(Val);
						//RVar->print(errs());
						//cout << "\n";
						Value* LVar = Builder.CreateStore(RVar, c);
						//LVar->print(errs());
						//cout << "\n";
					}
					else {//如果是 int a = 1
						Value* g = Builder.CreateStore(Val, c);
						/*Value* g = Builder.CreateStore(iter->second, c);*/
						//g->print(errs());
						//cout << "\n";
					}
				}
			}
		}
		iter++;
	}
	return nullptr;
}

Value* VarDecAST::codegenGlobal() 
{
	return nullptr;
}