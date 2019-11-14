#include "WhileStmtAST.h"

WhileStmtAST::WhileStmtAST(ExpAST* expAST, StmtsAST* stmtsAST)
{
	this->expAST = expAST;
	this->stmtsAST = stmtsAST;
}

WhileStmtAST::~WhileStmtAST()
{
	if (this->expAST != nullptr)
	{
		delete this->expAST;
	}
	if (this->stmtsAST != nullptr)
	{
		delete this->stmtsAST;
	}
}

Value* WhileStmtAST::codegen()
{
	cout << "WhileStmtAST\n";

	Value* Val;


	BasicBlock* PreheaderBB = Builder.GetInsertBlock();
	BasicBlock* LoopBB = BasicBlock::Create(TheContext, "loop", currentFun);
	BasicBlock* ExpBB = BasicBlock::Create(TheContext, "ExpBB", currentFun);
	BasicBlock* AfterBB = BasicBlock::Create(TheContext, "afterloop", currentFun);

	//如果Prehead语句块中已经包含终结符了，就不需要再生成Br
	if (Builder.GetInsertBlock()->getTerminator() == NULL) Val = Builder.CreateBr(ExpBB);

	Builder.SetInsertPoint(ExpBB);
	Value* a = expAST->codegen();
	if (AllocaInst::classof(a)||GlobalVariable::classof(a)) {
		a = Builder.CreateLoad(a);
	}
	Value* b = Builder.CreateICmpNE(a, ConstantInt::get(TheContext, APInt(32, 0)));
	Builder.CreateCondBr(b, LoopBB, AfterBB);


	Builder.SetInsertPoint(LoopBB);
	Value* c = stmtsAST->codegen();
	//如果Loop语句块中已经包含终结符了，就不需要再生成Br
	if (Builder.GetInsertBlock()->getTerminator() == NULL) Builder.CreateBr(ExpBB);

	Builder.SetInsertPoint(AfterBB);


	return ConstantInt::get(IntegerType::get(TheContext, 32), APInt(32, 0));
}