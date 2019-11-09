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
		CondV = Builder.CreateICmpNE((Constant*)CondV, ConstantInt::get(TheContext, APInt(32, 0)), "ifcond");

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
		//PHINode* PN = Builder.CreatePHI(Type::getDoubleTy(TheContext), 2, "iftmp");
		//PN->addIncoming(ThenV, ThenBB);
		////PN->addIncoming(ElseV, ElseBB);
		//PN->print(errs());
		return ConstantInt::get(IntegerType::get(TheContext, 32), APInt(32, 0));
	}
	else {//if(){}else{}
		Value* CondV = expAST->codegen();

		if (!CondV)return nullptr;

		CondV = Builder.CreateICmpNE((Constant*)CondV, ConstantInt::get(TheContext, APInt(32, 0)), "ifcond");


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

		return ConstantInt::get(IntegerType::get(TheContext, 32), APInt(32, 0));

		/*return nullptr;*/
	}
}