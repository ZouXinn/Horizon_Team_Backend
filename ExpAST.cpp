#include "ExpAST.h"
#include "VarNameAST.h"

ExpAST::ExpAST(ExpAST* left, OperatorAST* operatorAST)//单目
{
	this->left = left;
	this->operatorAST= operatorAST;
	this->type = 0;
}

ExpAST::ExpAST(ExpAST* left, ExpAST* right, OperatorAST* operatorAST)//双目
{
	this->left = left;
	this->right = right;
	this->operatorAST = operatorAST;
	this->type = 1;
}

void ExpAST::setStructName(string name)
{
	this->structName = name;
}

ExpAST::ExpAST(ValAST* valAST)//<val>
{
	this->valAST = valAST;
	this->type = 2;
}

ExpAST::ExpAST(VarNameAST* varNameAST)//<var_name>
{
	this->varNameAST = varNameAST;
	this->type = 3;
}

ExpAST::ExpAST(FuncCallStmtAST* funcCallStmtAST)
{
	this->funcCallStmtAST = funcCallStmtAST;
	this->type = 4;
}

void ExpAST::setType(zx::Type expType)
{
	this->expType = expType;
}

void ExpAST::setFinalToType(zx::Type type)
{
	this->finalToType = type;
}

void ExpAST::setPointerNum(int num)
{
	this->pointerNum = num;
}

ExpAST::~ExpAST()
{
	if (this->left != nullptr)
	{
		this->left = left;
	}
	if (this->right != nullptr)
	{
		this->right = right;
	}
	if (this->valAST != nullptr)
	{
		this->valAST = valAST;
	}
	if (this->varNameAST != nullptr)
	{
		this->varNameAST = varNameAST;
	}
	if (this->operatorAST != nullptr)
	{
		delete this->operatorAST;
	}
	if (this->funcCallStmtAST != nullptr)
	{
		delete this->funcCallStmtAST;
	}
}

Value* ExpAST::codegen() {
	Value* LVar;
	Value* RVar;
	Value* Val;
	AllocaInst* allo;
	ConstantFP* LVarV;
	bool BothIsInterger;
	if (left != NULL) {
		LVar = left->codegen();
	}
	if (right != NULL) {
		RVar = right->codegen();
	}
	switch (type)
	{
	case 0:
		switch (operatorAST->op)
		{
		case Op::NOT:
			Val = Builder.CreateNot(LVar);
			//Val->getType()->print(errs());  //i32
			//cout << endl;
			Val->print(errs());
			std::cout << "\n";
			return Val;
		case Op::MINUS:
			Val = Builder.CreateNeg(LVar);
			//Val->getType()->print(errs()); //i32
			//cout << endl;
			Val->print(errs());
			std::cout << "\n";
			return Val;
		case Op::STAR:
			/*Val = Builder.CreateNot(LVar, "Not tmp");
			Val->print(errs());
			std::cout << "\n";
			return Val;*/
			break;
		}
		break;
	case 1:
		switch (operatorAST->op)
		{
		case Op::ADD:
			if (IntegerType::classof(LVar->getType()) && IntegerType::classof(RVar->getType())) {
				//可以加上第三个参数作为IR语句中的临时变量名
				Val = Builder.CreateAdd(LVar, RVar);
				Val->print(errs());
				std::cout << "\n";
				return Val;
			}
			//这里似乎不用加判断也可生成正常IR代码
			if (IntegerType::classof(LVar->getType())) {
				LVar = Builder.CreateSIToFP(LVar, Type::getDoubleTy(TheContext));
				LVar->print(errs());
				cout << "\n";
			}
			if (IntegerType::classof(RVar->getType())) {
				RVar = Builder.CreateSIToFP(RVar, Type::getDoubleTy(TheContext));
				RVar->print(errs());
				cout << "\n";
			}
			Val = Builder.CreateFAdd(LVar, RVar);
			Val->print(errs());
			std::cout << "\n";
			return Val;
		case Op::MINUS:
			if (IntegerType::classof(LVar->getType()) && IntegerType::classof(RVar->getType())) {
				//可以加上第三个参数作为IR语句中的临时变量名
				Val = Builder.CreateSub(LVar, RVar);
				Val->print(errs());
				cout << "\n";
				return Val;
			}
			//这里似乎不用加判断也可生成正常IR代码
			if (IntegerType::classof(LVar->getType())) {
				LVar = Builder.CreateSIToFP(LVar, Type::getDoubleTy(TheContext));
				LVar->print(errs());
				cout << "\n";
			}
			if (IntegerType::classof(RVar->getType())) {
				RVar = Builder.CreateSIToFP(RVar, Type::getDoubleTy(TheContext));
				RVar->print(errs());
				cout << "\n";
			}
			Val = Builder.CreateFSub(LVar, RVar);
			Val->print(errs());
			cout << "\n";
			return Val;
		case Op::STAR:
			if (IntegerType::classof(LVar->getType()) && IntegerType::classof(RVar->getType())) {
				//可以加上第三个参数作为IR语句中的临时变量名
				Val = Builder.CreateMul(LVar, RVar);
				Val->print(errs());
				cout << "\n";
				return Val;
			}
			//这里似乎不用加判断也可生成正常IR代码
			if (IntegerType::classof(LVar->getType())) {
				LVar = Builder.CreateSIToFP(LVar, Type::getDoubleTy(TheContext));
				LVar->print(errs());
				cout << "\n";
			}
			if (IntegerType::classof(RVar->getType())) {
				RVar = Builder.CreateSIToFP(RVar, Type::getDoubleTy(TheContext));
				RVar->print(errs());
				cout << "\n";
			}


			//对于乘法和除法，Val输出的errs()有时候不是十进制数而是16位16进制数
			//由IEEE浮点数标准换算（即系统级程序设计上写的方法），可以算出该16进制数换算后就是正确结果
			//具体换算已经写成了IEEEConvert项目
			Val = Builder.CreateFMul(LVar, RVar, "multmp");
			Val->print(errs());
			cout << "\n";
			return Val;
		case Op::DIVIDE://如果两个都是int类型，也要转换吗？

			BothIsInterger = true;

			//这里似乎不用加判断也可生成正常IR代码
			if (IntegerType::classof(LVar->getType())) {
				LVar = Builder.CreateSIToFP(LVar, Type::getDoubleTy(TheContext));
				LVar->print(errs());
				cout << "\n";
			}
			if (IntegerType::classof(RVar->getType())) {
				RVar = Builder.CreateSIToFP(RVar, Type::getDoubleTy(TheContext));
				RVar->print(errs());
				cout << "\n";
			}

			Val = Builder.CreateFDiv(LVar, RVar);
			Val->print(errs());
			cout << "\n";

			//对于乘法和除法，Val输出的errs()有时候不是十进制数而是16位16进制数
			//由IEEE浮点数标准换算（即系统级程序设计上写的方法），可以算出该16进制数换算后就是正确结果
			//具体换算已经写成了IEEEConvert项目
			if (IntegerType::classof(LVar->getType()) && IntegerType::classof(RVar->getType())) {
				Val = Builder.CreateFPToSI(Val, IntegerType::get(TheContext, 32));
				Val->print(errs());
				cout << "\n";
			}
			return Val;
		case Op::MORE:
			if (IntegerType::classof(LVar->getType()) && IntegerType::classof(RVar->getType())) {
				//可以加上第三个参数作为IR语句中的临时变量名
				Val = Builder.CreateICmpSGT(LVar, RVar, "GreaterTmp");
				Val = Builder.CreateZExt(Val, IntegerType::get(TheContext, 32));
				Val->print(errs());
				cout << "\n";
				return Val;
			}
			if (IntegerType::classof(LVar->getType())) {
				LVar = Builder.CreateSIToFP(LVar, Type::getDoubleTy(TheContext));
				LVar->print(errs());
				cout << "\n";
			}
			if (IntegerType::classof(RVar->getType())) {
				RVar = Builder.CreateSIToFP(RVar, Type::getDoubleTy(TheContext));
				RVar->print(errs());
				cout << "\n";
			}
			Val = Builder.CreateICmpSGT(LVar, RVar);
			Val = Builder.CreateZExt(Val, IntegerType::get(TheContext, 32));
			Val->print(errs());
			cout << "\n";
			return Val;
		case Op::MOREOREQ:
			if (IntegerType::classof(LVar->getType()) && IntegerType::classof(RVar->getType())) {
				//可以加上第三个参数作为IR语句中的临时变量名
				//Val = Builder.CreateMul(LVar, RVar);
				Val = Builder.CreateICmpSGE(LVar, RVar);
				Val = Builder.CreateZExt(Val, IntegerType::get(TheContext, 32));
				Val->print(errs());
				cout << "\n";
				return Val;
			}
			if (IntegerType::classof(LVar->getType())) {
				LVar = Builder.CreateSIToFP(LVar, Type::getDoubleTy(TheContext));
				LVar->print(errs());
				cout << "\n";
			}
			if (IntegerType::classof(RVar->getType())) {
				RVar = Builder.CreateSIToFP(RVar, Type::getDoubleTy(TheContext));
				RVar->print(errs());
				cout << "\n";
			}
			Val = Builder.CreateFCmpOGE(LVar, RVar);
			Val = Builder.CreateZExt(Val, IntegerType::get(TheContext, 32));
			return Val;
		case Op::LESS:
			if (IntegerType::classof(LVar->getType()) && IntegerType::classof(RVar->getType())) {
				//可以加上第三个参数作为IR语句中的临时变量名
				//Val = Builder.CreateMul(LVar, RVar);
				Val = Builder.CreateICmpSLT(LVar, RVar);
				Val = Builder.CreateZExt(Val, IntegerType::get(TheContext, 32));
				Val->print(errs());
				cout << "\n";
				return Val;
			}
			if (IntegerType::classof(LVar->getType())) {
				LVar = Builder.CreateSIToFP(LVar, Type::getDoubleTy(TheContext));
				LVar->print(errs());
				cout << "\n";
			}
			if (IntegerType::classof(RVar->getType())) {
				RVar = Builder.CreateSIToFP(RVar, Type::getDoubleTy(TheContext));
				RVar->print(errs());
				cout << "\n";
			}
			Val = Builder.CreateFCmpOLT(LVar, RVar);
			Val = Builder.CreateZExt(Val, IntegerType::get(TheContext, 32));
			return Val;
		case Op::LESSOREQ:
			if (IntegerType::classof(LVar->getType()) && IntegerType::classof(RVar->getType())) {
				//可以加上第三个参数作为IR语句中的临时变量名
				//Val = Builder.CreateMul(LVar, RVar);
				Val = Builder.CreateICmpSLE(LVar, RVar);
				Val = Builder.CreateZExt(Val, IntegerType::get(TheContext, 32));
				Val->print(errs());
				cout << "\n";
				return Val;
			}
			if (IntegerType::classof(LVar->getType())) {
				LVar = Builder.CreateSIToFP(LVar, Type::getDoubleTy(TheContext));
				LVar->print(errs());
				cout << "\n";
			}
			if (IntegerType::classof(RVar->getType())) {
				RVar = Builder.CreateSIToFP(RVar, Type::getDoubleTy(TheContext));
				RVar->print(errs());
				cout << "\n";
			}
			Val = Builder.CreateFCmpOLE(LVar, RVar);
			Val = Builder.CreateZExt(Val, IntegerType::get(TheContext, 32));
			return Val;
		case Op::EQ:
			if (IntegerType::classof(LVar->getType()) && IntegerType::classof(RVar->getType())) {
				//可以加上第三个参数作为IR语句中的临时变量名
				//Val = Builder.CreateMul(LVar, RVar);
				Val = Builder.CreateICmpEQ(LVar, RVar);
				Val = Builder.CreateZExt(Val, IntegerType::get(TheContext, 32));
				Val->print(errs());
				cout << "\n";
				return Val;
			}
			if (IntegerType::classof(LVar->getType())) {
				LVar = Builder.CreateSIToFP(LVar, Type::getDoubleTy(TheContext));
				LVar->print(errs());
				cout << "\n";
			}
			if (IntegerType::classof(RVar->getType())) {
				RVar = Builder.CreateSIToFP(RVar, Type::getDoubleTy(TheContext));
				RVar->print(errs());
				cout << "\n";
			}
			Val = Builder.CreateFCmpOEQ(LVar, RVar);
			Val = Builder.CreateZExt(Val, IntegerType::get(TheContext, 32));
			return Val;
		case Op::NEQ:
			if (IntegerType::classof(LVar->getType()) && IntegerType::classof(RVar->getType())) {
				//可以加上第三个参数作为IR语句中的临时变量名
				//Val = Builder.CreateMul(LVar, RVar);
				Val = Builder.CreateICmpNE(LVar, RVar);
				Val = Builder.CreateZExt(Val, IntegerType::get(TheContext, 32));
				Val->print(errs());
				cout << "\n";
				return Val;
			}
			if (IntegerType::classof(LVar->getType())) {
				LVar = Builder.CreateSIToFP(LVar, Type::getDoubleTy(TheContext));
				LVar->print(errs());
				cout << "\n";
			}
			if (IntegerType::classof(RVar->getType())) {
				RVar = Builder.CreateSIToFP(RVar, Type::getDoubleTy(TheContext));
				RVar->print(errs());
				cout << "\n";
			}
			Val = Builder.CreateFCmpONE(LVar, RVar);
			Val = Builder.CreateZExt(Val, IntegerType::get(TheContext, 32));
			return Val;
		case Op::AND://CreateAnd难道是按位与吗？
			Val = Builder.CreateAnd(LVar, RVar);
			//cout << endl;
			//Val->getType()->print(errs());//    Val->getType() == i32 (1位无符号整数)
			Val->print(errs());
			cout << "\n";
			return Val;
		case Op::OR:
			Val = Builder.CreateOr(LVar, RVar);
			Val->print(errs());
			cout << "\n";
			return Val;
		default:
			break;
		}
		break;
	case 2:
		return valAST->codegen();
	case 3:
		/*return NamedValues[varNameAST->codegenStr()];*/
		allo = NamedValues[varNameAST->codegenStr()];
		Val = Builder.CreateLoad(allo);
		Val->print(errs());
		cout << "\n";
		return Val;
	default:
		break;
	}
	return nullptr;
}

string ExpAST::codegenStr() {
	switch (type)
	{
	case 3:
		return varNameAST->codegenStr();

	default:
		break;
	}
}