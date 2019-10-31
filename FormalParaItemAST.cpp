#include "FormalParaItemAST.h"


FormalParaItemAST::FormalParaItemAST(TypeSpecifyAST* typeSpecifyAST, DecVarNameAST* decVarNameAST)
{
	this->typeSpecifyAST = typeSpecifyAST;
	this->decVarNameAST = decVarNameAST;
}

FormalParaItemAST::~FormalParaItemAST()
{
	delete this->typeSpecifyAST;
	delete this->decVarNameAST;
}

llvm::Type* FormalParaItemAST::codegenType() {
	Type* type = typeSpecifyAST->codegenType();
	if (type->isIntegerTy()) {
		return IntegerType::get(TheContext, 32);
	}
	if (type->isDoubleTy()) {
		return llvm::Type::getDoubleTy(TheContext);
	}
	return nullptr;
}
string FormalParaItemAST::codegenName() {
	return decVarNameAST->codegenStr();
}