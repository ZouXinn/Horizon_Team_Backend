#include "ReturnStmtAST.h"


ReturnStmtAST::ReturnStmtAST()
{

}

ReturnStmtAST::ReturnStmtAST(ExpAST* expAST)
{
	this->expAST = expAST;
}

ReturnStmtAST::~ReturnStmtAST()
{
	if (this->expAST != nullptr)
	{
		delete this->expAST;
	}
}

Value* ReturnStmtAST::codegen()
{
	if (Builder.GetInsertBlock()->getTerminator() != NULL) return nullptr;

	if (expAST != nullptr) {
		Value* value = expAST->codegen();
		if (AllocaInst::classof(value)) {
			value = Builder.CreateLoad(value);
		}
		//Builder.CreateCleanupRet(value);//???
		Builder.CreateRet(value);
		
	}
	else {
		Builder.CreateRetVoid();
	}
	return nullptr;
}