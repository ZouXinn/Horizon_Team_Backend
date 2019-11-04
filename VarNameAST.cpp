#include "VarNameAST.h"


VarNameAST::VarNameAST(IdentifierAST* identifier)// type = 0
{
	this->type = 0;
	this->identifier = identifier;
}

VarNameAST::VarNameAST(VarNameAST* left, ExpAST* intExp)//type = 1
{
	this->type = 1;
	this->left = left;
	this->intExp = intExp;
}

VarNameAST::VarNameAST(VarNameAST* left, VarNameAST* right, int type)// type = 2   3
{
	this->type = type;
	this->left = left;
	this->right = right;
}

VarNameAST::VarNameAST(ExpAST* exp)
{
	this->type = 4;
	this->exp = exp;
}

void VarNameAST::setType(zx::Type varType)
{
	this->varType = varType;
}

void VarNameAST::setFinalToType(zx::Type finalToType)
{
	this->finalToType = finalToType;
}

void VarNameAST::setPointerNum(int pointerNum)
{
	this->pointerNum = pointerNum;
}

void VarNameAST::setStructName(string name)
{
	this->structName = name;
}

VarNameAST::~VarNameAST()
{
	if (this->left != nullptr)
	{
		delete this->left;
	}
	if (this->right != nullptr)
	{
		delete this->right;
	}
	if (this->intExp != nullptr)
	{
		delete this->intExp;
	}
	if (this->identifier != nullptr)
	{
		delete this->identifier;
	}
	if (this->exp != nullptr)
	{
		delete this->exp;
	}
}

string VarNameAST::codegenStr()
{
	switch (type)
	{
	case 0:
		return identifier->codegenStr();
	case 1:
		return left->identifier->codegenStr();
	default:
		break;
	}
	return NULL;
}

Value* VarNameAST::codegen()
{
	Value* expVal = nullptr;
	Value* LVarVal = nullptr;
	Value* RVarVal = nullptr;
	AllocaInst* allo = nullptr;
	Value* Val = nullptr;
	vector<unsigned int> zeroV;
	ArrayRef<unsigned int> idx;
	switch (type)
	{
	case 0:
		return NamedValues[identifier->codegenStr()];//此处返回的是AllocaInst*
	case 1://varName[exp]
		expVal = intExp->codegen();
		if (AllocaInst::classof(expVal)) {
			expVal = Builder.CreateLoad(expVal);
		}
		allo = NamedValues[this->codegenStr()]; // allo是varName的内存空间，需要找到按下标访问的方法
		//或者不用allo,用load之后的allo?
		
		/*
		//CreateExtractValue
		zeroV.push_back(0);
		idx = ArrayRef<unsigned int>(zeroV);

		Val = Builder.CreateLoad(allo);
		//通过按下标访问的方法访问
		Val = Builder.CreateExtractValue(Val , idx);
		//end
		*/

		//Builder.CreateInsertValue()
		//Builder.CreateExtractValue()

		//CreateExtractElement
		Val = Builder.CreateLoad(allo);
		Val = Builder.CreateExtractElement(Val, expVal);

		//end


		//Builder.CreateExtractValue()
		//Builder.CreateExtractElement()
		//Val = Builder.CreateExtractValue(allo, idx);
		//Val = Builder.CreateLoad(allo);
		return Val;//是否也该返回AllocaInst*类型？
		//allo.
	case 2:

	case 3:

	case 4:

	default:
		break;
	}
	return nullptr;
}

Value* VarNameAST::expCodegen()
{
	if (this->exp != nullptr)
	{
		return exp->codegen();
	}
	return nullptr;
}