#include "AssignStmtAST.h"


AssignStmtAST::AssignStmtAST(VarNameAST* varNameAST, OperatorAST* operatorAST)//type = 0 
{
	this->type = 0;
	this->varNameAST = varNameAST;
	this->operatorAST = operatorAST;
}

AssignStmtAST::AssignStmtAST(VarNameAST* varNameAST, ExpAST* expAST)//type = 1
{
	this->type = 1;
	this->varNameAST = varNameAST;
	this->expAST = expAST;
}
Value* AssignStmtAST::codegen()
{
	Value* varNameVal = nullptr;
	Value* expVal = nullptr;
	Value* Val = nullptr;
	Value* Val2 = nullptr;
	switch (type)
	{
	case 0:
		varNameVal = varNameAST->codegen();
		switch (operatorAST->op)
		{
		case Op::PLUSPLUS://++
			Val = Builder.CreateLoad(varNameVal);
			Val = Builder.CreateAdd(Val, ConstantInt::get(IntegerType::get(TheContext, 32), APInt(32, 1)));
			Val = Builder.CreateStore(Val, varNameVal);
			return Val;
		case Op::MINUSMINUS://--
			Val = Builder.CreateLoad(varNameVal);
			Val = Builder.CreateSub(Val, ConstantInt::get(IntegerType::get(TheContext, 32), APInt(32, 1)));
			Val = Builder.CreateStore(Val, varNameVal);
			return Val;
		default:
			return nullptr;
		}
	case 1:
		if (varNameAST->type == 1) {//varName[exp] = ...
			vector<Value*> values = varNameAST->codegenAlloAndExpValue();
			if (values.size() == 2) {//注意类型转换！！
				Value* allo = values[0];
				Value* intExpVal = values[1];
				expVal = expAST->codegen();
				Val = Builder.CreateLoad(allo);
				//Val = Builder.CreateInsertElement(Val, expVal, intExpVal);
				/*cout << endl;
				Val->print(errs());
				cout << endl;
				expVal->print(errs());
				cout << endl;
				intExpVal->print(errs());
				cout << endl;*/
				

				if (AllocaInst::classof(intExpVal)) {
					intExpVal = Builder.CreateLoad(intExpVal);
				}
				if (AllocaInst::classof(expVal)) {
					expVal = Builder.CreateLoad(expVal);
				}
				//类型转换
				if (Val->getType()->isVectorTy() && Val->getType()->getVectorElementType()->isDoubleTy() && expVal->getType()->isIntegerTy()) {
					expVal = Builder.CreateSIToFP(expVal, Type::getDoubleTy(TheContext));
				}

				Val = Builder.CreateInsertElement(Val, expVal, intExpVal);
				//Val = Builder.CreateInsertElement(allo, expVal, intExpVal);
				//store
				Val = Builder.CreateStore(Val, allo);
			}
		}
		else {
			varNameVal = varNameAST->codegen();
			expVal = expAST->codegen();
			//如果varName是 AllocaInst*,而expVal不是AllocaInst*，代表应该
			//Val = Builder.CreateStore(expVal, varNameVal);
			if (AllocaInst::classof(varNameVal) && !AllocaInst::classof(expVal)) {
				//类型转换?
				if (((AllocaInst*)varNameVal)->getAllocatedType()->isDoubleTy() && expVal->getType()->isIntegerTy()) {
					expVal = Builder.CreateSIToFP(expVal, Type::getDoubleTy(TheContext));
				}
				Val = Builder.CreateStore(expVal, varNameVal);

			}

			//如果varName是 AllocaInst*,而expVal也是AllocaInst*，代表应该
			//Value* tempVal = Builder.CreateLoad(expVal);
			//Val = Builder.CreateStore(tempVal, varNameVal);
			else if (AllocaInst::classof(varNameVal) && AllocaInst::classof(expVal)) {
				Value* tempVal = Builder.CreateLoad(expVal);
				if (((AllocaInst*)varNameVal)->getAllocatedType()->isDoubleTy() && tempVal->getType()->isIntegerTy()) {
					tempVal = Builder.CreateSIToFP(tempVal, Type::getDoubleTy(TheContext));
				}
				//类型转换?
				Val = Builder.CreateStore(tempVal, varNameVal);
			}

			//Global
			if (GlobalVariable::classof(varNameVal)) {
				if (((GlobalVariable*)varNameVal)->getValueType()->isDoubleTy() && expVal->getType()->isIntegerTy()) {
					expVal = Builder.CreateSIToFP(expVal, Type::getDoubleTy(TheContext));
				}
				Builder.CreateStore(expVal, varNameVal);
			}
			else if (Argument::classof(varNameVal)) {
				if (((Argument*)varNameVal)->getType()->isDoubleTy()) {
					if (expVal->getType()->isIntegerTy()) {
						expVal = Builder.CreateSIToFP(expVal, Type::getDoubleTy(TheContext));
					}
					varNameVal = Builder.CreateAdd(expVal, ConstantFP::get(TheContext, APFloat(0.0)));
					//Params[varNameVal->getName()] = varNameVal;
				}
				else {
					varNameVal = Builder.CreateAdd(expVal, varNameVal);
					expVal->print(errs()); cout << endl;
					varNameVal->print(errs()); cout << endl;
					//Params[varNameVal->getName()] = varNameVal;
				}
				//Builder.Create
				//Builder.CreateStore(expVal, varNameVal);
			}



			//if (varNameVal->getType() == varNameVal->getType()->getPointerTo()) {
			//	Val = Builder.CreateStore(expVal, varNameVal);
			//	Val->print(errs());
			//	cout << "\n";
			//}
			//if (isa<IntegerType>(expVal->getType()) && (varNameVal->getType() == llvm::Type::getDoublePtrTy(TheContext))) {

			//	expVal = Builder.CreateSIToFP(expVal, Type::getDoubleTy(TheContext));
			//	Val = Builder.CreateStore(expVal, varNameVal);
			//	Val->print(errs());
			//	/*cout << "\nAlu\n";
			//	a->getType()->getPointerTo()->print(errs());*/
			//}
		}
		
		return Val;
	default:
		break;
	}
	return nullptr;
}

AssignStmtAST::~AssignStmtAST()
{
	if (this->expAST != nullptr)
	{
		delete this->expAST;
	}
	if (this->varNameAST != nullptr)
	{
		delete this->varNameAST;
	}
	if (this->operatorAST != nullptr)
	{
		delete this->operatorAST;
	}
}