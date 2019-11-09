#include "RealParaItemAST.h"
#include "ExpAST.h"


RealParaItemAST::RealParaItemAST(ExpAST* expAST)
{
	this->expAST = expAST;
}

RealParaItemAST::~RealParaItemAST()
{
	if (this->expAST != nullptr)
	{
		delete this->expAST;
	}
}

Value* RealParaItemAST::codegen() {
	Value* val = expAST->codegen();
	/*if (AllocaInst::classof(val)) {
		val = Builder.CreateLoad(val);
	}*/
	return val;
}

