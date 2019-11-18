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
	//
	//冯文翰于11月19日0：24修改
	//
	//this->isArray = true;
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
		return identifier->codegenStr()+"."+to_string(level);
	case 1:
		return left->identifier->codegenStr()+ "." + to_string(level);
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
	int type = -1;

	//
	//冯文翰于11月18日17：42更改
	//
	if (this->varType == zx::Type::INT) {
		//if (this->isArray) {
		if (this->type == 1) {
			type = 2;
		}
		else {
			type = 0;
		}
	}
	else if (this->varType == zx::Type::REAL) {
		//if (this->isArray) {
		if (this->type == 1) {
			type = 3;
		}
		else {
			type = 1;
		}
	}
	/*if (this->isArray) {
		type = 1;
	}
	else
	{
		type = 0;
	}*/
	switch (this->type)
	{
	case 0:
		//return NamedValues[identifier->codegenStr()];//此处返回的是AllocaInst*
		return getHighestValue(this->codegenStr(), type);
	case 1://varName[exp]
		expVal = intExp->codegen();
		if (AllocaInst::classof(expVal) || GlobalValue::classof(expVal)) {
			expVal = Builder.CreateLoad(expVal);
		}
		//allo = NamedValues[this->codegenStr()]; // allo是varName的内存空间，需要找到按下标访问的方法
		allo = getHighestValue(this->codegenStr(), type);
		Val = Builder.CreateLoad(allo);
		Val = Builder.CreateExtractElement(Val, expVal);
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

vector<Value*> VarNameAST::codegenAlloAndExpValue() {//第一个是varName的allo，第二个是exp的
	vector<Value*> ret;
	if (type != 1) {
		return ret;
	}
	else {
		//ret.push_back(NamedValues[this->codegenStr()]);
		int type = -1;
		if (this->varType == zx::Type::INT) {
			//if (this->isArray) {
			if (this->type == 1) {
				type = 2;
			}
			else {
				type = 0;
			}
		}
		else if (this->varType == zx::Type::REAL) {
			//if (this->isArray) {
			if (this->type == 1) {
				type = 3;
			}
			else {
				type = 1;
			}
		}
		ret.push_back(getHighestValue(this->codegenStr(), type));
		ret.push_back(intExp->codegen());
		return ret;
	}
}

Value* VarNameAST::expCodegen()
{
	if (this->exp != nullptr)
	{
		return exp->codegen();
	}
	return nullptr;
}