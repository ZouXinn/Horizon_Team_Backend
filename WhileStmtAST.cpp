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
	/*if (isa<ConstantInt>(expAST->codegen())) {
		cout << "\nDeer.xl\n";
	}*/

	Value* Val;


	BasicBlock* PreheaderBB = Builder.GetInsertBlock();
	BasicBlock* LoopBB = BasicBlock::Create(TheContext, "loop", currentFun);
	BasicBlock* ExpBB = BasicBlock::Create(TheContext, "ExpBB", currentFun);
	BasicBlock* AfterBB = BasicBlock::Create(TheContext, "afterloop", currentFun);

	Val = Builder.CreateBr(ExpBB);

	Builder.SetInsertPoint(ExpBB);
	Value* a = expAST->codegen();
	Value* b = Builder.CreateICmpNE(a, ConstantInt::get(TheContext, APInt(32, 0)));
	Builder.CreateCondBr(b, LoopBB, AfterBB);


	Builder.SetInsertPoint(LoopBB);
	Value* c = stmtsAST->codegen();
	Builder.CreateBr(ExpBB);

	/*PHINode *Variable = Builder.CreatePHI(IntegerType::get(TheContext,32),2,Va)*/




	/*PreheaderBB->print(errs());
	cout << "\n";
	ExpBB->print(errs());
	cout << "\n";
	LoopBB->print(errs());
	cout << "\n";
	AfterBB->print(errs());
	cout << "\n";*/



	Builder.SetInsertPoint(AfterBB);


	return ConstantInt::get(IntegerType::get(TheContext, 32), APInt(32, 0));
}