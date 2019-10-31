#include "IfStmtAST.h"


IfStmtAST::IfStmtAST(ExpAST* expAST, StmtsAST* thenStmts)
{
	this->expAST = expAST;
	this->thenStmts = thenStmts;
}

IfStmtAST::IfStmtAST(ExpAST* expAST, StmtsAST* thenStmts, StmtsAST* elseStmts):IfStmtAST(expAST,thenStmts)
{
	this->elseStmts = elseStmts;
}

IfStmtAST::~IfStmtAST()
{
	if (this->expAST != nullptr)
	{
		delete this->expAST;
	}
	if (this->thenStmts != nullptr)
	{
		delete this->thenStmts;
	}
	if (this->elseStmts != nullptr)
	{
		delete this->elseStmts;
	}
}

Value* IfStmtAST::codegen()
{
	//return nullptr;

	//if(){}
	if (elseStmts == nullptr) {
		Value* CondV = expAST->codegen();
		if (!CondV)return nullptr;
		CondV = Builder.CreateFCmpONE(CondV, ConstantFP::get(TheContext, APFloat(0.0)), "ifcond");

		Function* TheFunciton = Builder.GetInsertBlock()->getParent();

		BasicBlock* ThenBB = BasicBlock::Create(TheContext, "then", TheFunciton);
		//BasicBlock* ElseBB = BasicBlock::Create(TheContext, "else");
		BasicBlock* MergeBB = BasicBlock::Create(TheContext, "ifcont");

		Builder.CreateCondBr(CondV, ThenBB, MergeBB);

		Builder.SetInsertPoint(ThenBB);
		Value* ThenV = thenStmts->codegen();
		if (!ThenV)return nullptr;
		Builder.CreateBr(MergeBB);
		ThenBB = Builder.GetInsertBlock();

		/*TheFunciton->getBasicBlockList().push_back(ElseBB);
		Builder.SetInsertPoint(ElseBB);
		Value* ElseV = elseStmts->codegen();
		if (!ElseV)return nullptr;
		Builder.CreateBr(MergeBB);
		ElseBB = Builder.GetInsertBlock();*/

		TheFunciton->getBasicBlockList().push_back(MergeBB);
		Builder.SetInsertPoint(MergeBB);
		PHINode* PN = Builder.CreatePHI(Type::getDoubleTy(TheContext), 2, "iftmp");
		PN->addIncoming(ThenV, ThenBB);
		//PN->addIncoming(ElseV, ElseBB);
		PN->print(errs());
		return PN;
	}
	else {//if(){}else{}
		Value* CondV = expAST->codegen();
		//Value* compare = ConstantFP::get(TheContext, APFloat(0.0));
		if (!CondV)return nullptr;
		//CondV = cast<ConstantFP>(CondV);

		/*zx::Type type = expAST->expType;
		if (type == zx::Type::INT) {
			CondV = Builder.CreateFCmpONE((Constant*)CondV, ConstantInt::get(TheContext, APInt(32, 0)), "ifcond");
		}*/
		//CondV = (ConstantInt*)CondV;
		//CondV = cast<Value>(CondV);
		//CondV = cast<ConstantFP*>(CondV);

		CondV = Builder.CreateICmpNE((Constant*)CondV, ConstantInt::get(TheContext, APInt(32, 0)), "ifcond");


		/*	APInt a = ((ConstantInt*)CondV)->getValue();
			int b = a.getSExtValue();
			float c = (float)b;
			Value* CondV2 = ConstantFP::get(TheContext, APFloat(c));
			CondV2 = Builder.CreateFCmpONE(CondV2, ConstantFP::get(TheContext, APFloat(0.0)), "ifcond");*/

		Function* TheFunciton = Builder.GetInsertBlock()->getParent();

		BasicBlock* ThenBB = BasicBlock::Create(TheContext, "then", TheFunciton);
		BasicBlock* ElseBB = BasicBlock::Create(TheContext, "else");
		BasicBlock* MergeBB = BasicBlock::Create(TheContext, "ifcont");

		Builder.CreateCondBr(CondV, ThenBB, ElseBB);

		Builder.SetInsertPoint(ThenBB);
		Value* ThenV = thenStmts->codegen();
		if (!ThenV)return nullptr;
		Builder.CreateBr(MergeBB);
		ThenBB = Builder.GetInsertBlock();

		TheFunciton->getBasicBlockList().push_back(ElseBB);
		Builder.SetInsertPoint(ElseBB);
		Value* ElseV = elseStmts->codegen();
		if (!ElseV)return nullptr;
		Builder.CreateBr(MergeBB);
		ElseBB = Builder.GetInsertBlock();

		TheFunciton->getBasicBlockList().push_back(MergeBB);
		Builder.SetInsertPoint(MergeBB);


		/*cout << "\n\nDeer.xl\n\n";
		TheFunciton->print(errs());*/

		/*PHINode* PN = Builder.CreatePHI(Type::getInt32Ty(TheContext), 2, "iftmp");
		PN->addIncoming(ThenV, ThenBB);
		PN->addIncoming(ElseV, ElseBB);
		PN->print(errs());*/
		/*return PN;*/

		return ConstantInt::get(IntegerType::get(TheContext, 32), APInt(32, 0));

		/*return nullptr;*/
	}

}