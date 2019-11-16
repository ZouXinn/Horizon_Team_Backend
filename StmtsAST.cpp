#include "StmtsAST.h"

StmtsAST::StmtsAST()
{
	this->stmtASTs = new vector<StmtAST*>();
}

StmtsAST::~StmtsAST()
{
	if (this->stmtASTs != nullptr)
	{
		for (int i = 0; i < this->stmtASTs->size(); i++)
		{
			delete this->stmtASTs->at(i);
		}
		delete this->stmtASTs;
	}
}

void StmtsAST::addStmtAST(StmtAST* ast)
{
	this->stmtASTs->push_back(ast);
}

Value* StmtsAST::codegen()
{
	//
	//FWH
	//

	Value* Val = nullptr;
	if (this->stmtASTs != nullptr)
	{
		for (int i = this->stmtASTs->size() - 1; i >= 0; i--)
		{
			
		

			Function* func = nullptr;
			Value* Val = nullptr;
			Function* strFunc = nullptr;
			Value* strVal = nullptr;
			map<string, AllocaInst*>::iterator iter;
			string s = "\nLine "+to_string(this->stmtASTs->at(i)->row)+":\n\n";

			writeChar(s);
			string tmp;
			for (auto iter = NamedValues.begin(); iter != NamedValues.end(); iter++) {
				//tmp = "__" + to_string(this->stmtASTs->at(i)->level);
				///*if (!iter->first.find("__" + to_string(this->stmtASTs->at(i)->level))) {
				//	Continue;
				//}*/
				////string s = iter->first.substr(iter->first.size() - tmp.size());
				//if ((tmp.size()> iter->first.size()) || (!(iter->first.substr(iter->first.size() - tmp.size())._Equal(tmp)))) {
				//	Continue;
				//}
				//
				writeChar(iter->first.substr(0,iter->first.size()-tmp.size()) + ":");

				Val = Builder.CreateLoad(iter->second);
				if (Val->getType()->isIntegerTy()) {
					func = TheModule->getFunction("writeInt");
				}
				else if (Val->getType()->isDoubleTy())
				{
					func = TheModule->getFunction("writeDouble");
				}
				Builder.CreateCall(func, Val);

				writeChar("\n");

				

			}

			Val = this->stmtASTs->at(i)->codegen();


			//冯文翰于11月12日23：41修改
			//如果这个stmt是return，那么直接返回
			if (this->stmtASTs->at(i)->son == 4) {
				return ConstantInt::get(IntegerType::get(TheContext, 32), APInt(32, 0));
			}

		}
	}

	//冯文翰11月15日22:04修改
	

	return ConstantInt::get(IntegerType::get(TheContext, 32), APInt(32, 0));
	//return nullptr;
	for (auto iter = NamedValues.begin(); iter != NamedValues.end(); iter++) {

	}

	//
	//LBQ
	//

	/*for (int i = this->stmtASTs->size() - 1; i >= 0; i--)
	{
		this->stmtASTs->at(i)->codegen();
	}
	return (Value*)stmtASTs;*/
}