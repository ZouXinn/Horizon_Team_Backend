#include "IdItemAST.h"

IdItemAST::IdItemAST(DecVarNameAST* decVarNameAST)
{
	this->decVarNameAST = decVarNameAST;
}

IdItemAST::IdItemAST(DecVarNameAST* decVarNameAST, ExpAST* exp)
{
	this->decVarNameAST = decVarNameAST;
	this->exp = exp;
}

IdItemAST::IdItemAST(DecVarNameAST* decVarNameAST, ExpListAST* exps)
{
	this->decVarNameAST = decVarNameAST;
	this->exps = exps;
}

IdItemAST::~IdItemAST()
{
	delete this->decVarNameAST;
	if (this->exp != nullptr)
	{
		delete this->exp;
	}
	if (this->exps != nullptr)
	{
		delete this->exps;
	}
}

string IdItemAST::codegenStr()
{
	return string();
}

vector<string> IdItemAST::codegenStrVec()
{
	return vector<string>();
}

//map<string, Value*>* IdItemAST::codegenMap()
//{
//	return nullptr;
//}

pair<string, ID*> IdItemAST::codegenPair() {//不是数组
	pair<string, ID*> pair;
	pair.first = decVarNameAST->codegenStr();
	if (!decVarNameAST->LSRS) {//如果不是数组
		if (exp != NULL) {
			vector<Value*> valVector;
			valVector.push_back(exp->codegen());
			ID* id = new ID(false, valVector);
			pair.second = id;
			return pair;
		}
		else {//未赋初值

		}
	}
	else {//是数组
		vector<Value*> valVector;
		ID* id = new ID(true);
		if (decVarNameAST->intAST == nullptr) {//如果没有显式指定长度
			if (this->exps != nullptr) {
				for (int i = this->exps->exps->size() - 1; i >= 0; i--) {
					ExpAST* exp = this->exps->exps->at(i);
					valVector.push_back(exp->codegen());
					
				}
				id->elementNum = this->exps->exps->size();
			}
			else {
				throw Exception(OtherEx, 0, "IdItemAST::codegenPairVec() 调用错误!");
			}
		}
		else {//显式指定了长度
			int arrayLength = decVarNameAST->intAST->integer;//数组长度
			id->elementNum = arrayLength;
			if (this->exps != nullptr) {//如果赋了初值 则把没补满的补满
				for (int i = this->exps->exps->size() - 1; i >= 0; i--) {
					ExpAST* exp = this->exps->exps->at(i);
					valVector.push_back(exp->codegen());
				}
				int rest = arrayLength - this->exps->exps->size();
				if (this->exps->expType == zx::Type::INT) {
					for (int i = 0; i < rest; i++) {
						valVector.push_back(ConstantInt::get(TheContext, APInt(32, 0)));
					}
				}
				else if (this->exps->expType == zx::Type::REAL) {
					for (int i = 0; i < rest; i++) {
						valVector.push_back(ConstantFP::get(TheContext, APFloat(0.0)));
					}
				}
				else if (this->exps->expType == zx::Type::STRING) {//有待解决

				}
				else if (this->exps->expType == zx::Type::POINTER) {//有待解决

				}
				else if (this->exps->expType == zx::Type::STRUCT) {//有待解决

				}
				else if (this->exps->expType == zx::Type::CHAR) {//有待解决
					/*for (int i = 0; i < rest; i++) {
						valVector.push_back(ConstantFP::get(TheContext, AP);
					}*/
				}
			}
			else {//如果没有赋初值   全部自己赋初值

			}

		}
		id->valueVector = valVector;
		return pair;
	}
}
