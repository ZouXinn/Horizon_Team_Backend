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
		varNameVal = varNameAST->codegen();
		expVal = expAST->codegen();
		//如果varName是 AllocaInst*,而expVal不是AllocaInst*，代表应该
		//Val = Builder.CreateStore(expVal, varNameVal);
		if (AllocaInst::classof(varNameVal) && !AllocaInst::classof(expVal)) {
			//类型转换?
			Val = Builder.CreateStore(expVal, varNameVal);
		}
		
		//如果varName是 AllocaInst*,而expVal也是AllocaInst*，代表应该
		//Value* tempVal = Builder.CreateLoad(expVal);
		//Val = Builder.CreateStore(tempVal, varNameVal);
		if (AllocaInst::classof(varNameVal) && AllocaInst::classof(expVal)) {
			Value* tempVal = Builder.CreateLoad(expVal);
			//类型转换?
			Val = Builder.CreateStore(tempVal, varNameVal);
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