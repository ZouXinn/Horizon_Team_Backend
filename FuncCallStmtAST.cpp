#include "FuncCallStmtAST.h"


FuncCallStmtAST::FuncCallStmtAST(IdentifierAST* identifier, RealParaListAST* realParaListAST)
{
	this->identifier = identifier;
	this->realParaListAST = realParaListAST;
}

FuncCallStmtAST::~FuncCallStmtAST()
{
	if (this->realParaListAST != nullptr)
	{
		delete this->realParaListAST;
	}
	if (this->identifier != nullptr)
	{
		delete this->identifier;
	}
}

Value* FuncCallStmtAST::codegen()
{
	/*string a = identifier->codegenStr();
	
	std::vector<Type*> params;
	
	Function* func = Function::Create(FunctionType::get((Type*)Type::getInt32Ty(TheContext), params, false), Function::ExternalLinkage, a, TheModule.get());
	Value* Val = Builder.CreateCall(func, ArrayRef<Value*>());*/
	//Function::Create()

	//zx start
	string funcName = identifier->codegenStr();
	Function* func = nullptr;
	Value* Val = nullptr;
	if (funcName == "read" || funcName == "write") {
		vector<Value*> rparamsVec = this->realParaListAST->codegenVec();
		Value* rparam = rparamsVec[0];
		if (funcName == "read") {
			if (AllocaInst::classof(rparam)) {
				if (((AllocaInst*)rparam)->getAllocatedType()->isIntegerTy()) {//readInt
					func = TheModule->getFunction("readInt");
					vector<Value*> RDI;
					Val = Builder.CreateCall(func, RDI, "readtmp");
					if (AllocaInst::classof(rparam)) {
						//cout << "rparam is Allo" << endl;
					}
					//rparam = Val
					Builder.CreateStore(Val, rparam);//此处rparam是AllocaInst*，若不是，则在静态语义分析时就报错了，如果是数组？不支持？--
				}
				else if (((AllocaInst*)rparam)->getAllocatedType()->isDoubleTy()) {//readDouble
					func = TheModule->getFunction("readDouble");
					vector<Value*> RDD;
					Val = Builder.CreateCall(func, RDD, "readtmp");
					//rparam = Val
					Builder.CreateStore(Val, rparam);
				}
			}
			else if (GlobalVariable::classof(rparam)) {
				if (((GlobalVariable*)rparam)->getValueType()->isIntegerTy()) {
					func = TheModule->getFunction("readInt");
					vector<Value*> RDI;
					Val = Builder.CreateCall(func, RDI, "readtmp");
					Builder.CreateStore(Val, rparam);
				}
				else if (((GlobalVariable*)rparam)->getValueType()->isDoubleTy()) {
					func = TheModule->getFunction("readDouble");
					vector<Value*> RDI;
					Val = Builder.CreateCall(func, RDI, "readtmp");
					Builder.CreateStore(Val, rparam);
				}
			}
		}
		else {
			if (GlobalVariable::classof(rparam)) {
				if (((GlobalVariable*)rparam)->getValueType()->isIntegerTy()) {
					func = TheModule->getFunction("writeInt");
					rparamsVec[0] = Builder.CreateLoad(rparam);
					Builder.CreateCall(func, rparamsVec);
				}
				else if (((GlobalVariable*)rparam)->getValueType()->isDoubleTy()) {
					func = TheModule->getFunction("writeDouble");
					rparamsVec[0] = Builder.CreateLoad(rparam);
					Builder.CreateCall(func, rparamsVec);
				}
			}
			else {
				if (rparam->getType()->isIntegerTy() || AllocaInst::classof(rparam) && ((AllocaInst*)rparam)->getAllocatedType()->isIntegerTy()) {//writeInt
					func = TheModule->getFunction("writeInt");
					if (AllocaInst::classof(rparam)) {
						rparamsVec[0] = Builder.CreateLoad(rparam);
					}
					Builder.CreateCall(func, rparamsVec);
				}
				else if (rparam->getType()->isDoubleTy() || AllocaInst::classof(rparam) && ((AllocaInst*)rparam)->getAllocatedType()->isDoubleTy()) {//writeDouble
					func = TheModule->getFunction("writeDouble");
					if (AllocaInst::classof(rparam)) {
						rparamsVec[0] = Builder.CreateLoad(rparam);
					}
					Builder.CreateCall(func, rparamsVec);
				}
			}
		}
	}
	else {//其他函数正常调用
		func = TheModule->getFunction(funcName);
		//ArrayRef<Value*> rparams = this->realParaListAST->codegenArr();
		vector<Value*> rparamsVec = this->realParaListAST->codegenVec();
		for (int i = 0; i < rparamsVec.size(); i++) {
			if (AllocaInst::classof(rparamsVec[i])) {
				rparamsVec[i] = Builder.CreateLoad(rparamsVec[i]);
			}
		}
		Val = Builder.CreateCall(func, rparamsVec, "calltmp");
	}
	return Val;
}
