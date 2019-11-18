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
			string s = "\nLine "+to_string(this->stmtASTs->at(i)->row)+":\n";
			map<string, Value*> res;

			writeChar(s);
			string name;
			int type;
			for (auto iter = NamedValues.begin(); iter != NamedValues.end(); iter++) {

				int pos1 = iter->first.find(".");
				int pos2 = iter->first.find_last_of(".");
				int ori_level = level;
				name = iter->first.substr(0, pos1)+"."+to_string(ori_level);
				type = atoi(iter->first.substr(pos2+1, iter->first.size() - pos2).c_str());
				
				name = getHighestStr(name,type);
				if (name.empty()) {
					continue;
				}
				if (!name._Equal(iter->first)) {
					continue;
				}
				Val = Builder.CreateLoad(iter->second);
				res[iter->first.substr(0,pos1)] = Val;

				/*cout << "NameValues:\n";
				cout << iter->first;
				cout << "\n";*/


			}

			for (auto iter = Params.begin(); iter != Params.end(); iter++) {

				int pos1 = iter->first.find(".");
				int pos2 = iter->first.find_last_of(".");
				if ((res.count(iter->first.substr(0, pos1)) == 0) && (pos1!=pos2)) {
					/*Val = iter->second;*/
					Val = Builder.CreateLoad((AllocaInst*)iter->second);
					if (Val != NULL) {
						res[iter->first.substr(0, pos1)] = Val;
						/*cout << "Params:\n";
						cout << iter->first;
						cout << "\n";*/
					}
				}


			}

			for (auto iter = GV.begin(); iter != GV.end(); iter++) {

				int pos1 = iter->first.find(".");
				if (res.count(iter->first.substr(0, pos1)) == 0) {
					Val = Builder.CreateLoad(iter->second);
					
					res[iter->first.substr(0, pos1)] = Val;
					//cout << "GB:\n";
					//cout << iter->first;
					//cout << "\n";
				}
				

			}
			
			for (auto iter = res.begin(); iter != res.end(); iter++) {
				writeChar(iter->first+":");
				Val = iter->second;
				if (Val->getType()->isIntegerTy()) {
					func = TheModule->getFunction("writeInt");
					Builder.CreateCall(func, Val);
				}
				else if (Val->getType()->isDoubleTy())
				{
					func = TheModule->getFunction("writeDouble");
					Builder.CreateCall(func, Val);
				}
				else if (Val->getType()->isVectorTy()){
					//cout << "yxl\n";
					int eleCount = ((ConstantVector*)Val)->getType()->getElementCount().Min;
					/*cout << iter->first<<"\n";
					cout << eleCount << "\n";
					cout << "line " << this->stmtASTs->at(i)->row;*/
					Value* eleVal;
					writeChar("{");
					for (int i = 0; i < eleCount; i++) {
						eleVal = Builder.CreateExtractElement(Val, ConstantInt::get(IntegerType::get(TheContext,32),APInt(32,i)));
						if (eleVal->getType()->isIntegerTy()) {
							func = TheModule->getFunction("writeIntNotBr");
						}
						else if (eleVal->getType()->isDoubleTy())
						{
							func = TheModule->getFunction("writeDoubleNotBr");
						}
						Builder.CreateCall(func, eleVal);
						if (i != eleCount-1) {
							writeChar(",");
						}
					}
					writeChar("}\n");
					
				}
				
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