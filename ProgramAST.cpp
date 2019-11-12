#include "ProgramAST.h"

ProgramAST::ProgramAST()
{
	this->dec_exp_ASTs = new vector<DecExpAST*>();
	this->func_define_ASTs = new vector<FuncDefineAST*>();
}

ProgramAST::~ProgramAST()
{
	for (int i = 0; i < this->dec_exp_ASTs->size(); i++)
	{
		delete this->dec_exp_ASTs->at(i);
	}
	delete this->dec_exp_ASTs;
	for (int i = 0; i < this->func_define_ASTs->size(); i++)
	{
		delete this->func_define_ASTs->at(i);
	}
	delete this->func_define_ASTs;
}

void ProgramAST::addDecExpAST(DecExpAST* ast)
{
	this->dec_exp_ASTs->push_back(ast);
}

void ProgramAST::addFuncDefineAST(FuncDefineAST* ast)
{
	this->func_define_ASTs->push_back(ast);
}

Value* ProgramAST::codegen() {

	updateBB();

	
	for (int i = this->dec_exp_ASTs->size() - 1; i >= 0; i--)
	{
		this->dec_exp_ASTs->at(i)->codegen();
		//GlobalVariable()
		//Builder.CreateG
	}


	for (int i = this->func_define_ASTs->size() - 1; i >= 0; i--)
	{
		this->func_define_ASTs->at(i)->codegen();
		NamedValues.clear();
		Params.clear();
		//NamedValues = std::map<std::string, AllocaInst*>();
	}
	return nullptr;
}

void ProgramAST::RunJIT()
{
	// JIT the module containing the anonymous expression, keeping a handle so
	  // we can free it later.
	auto H = TheJIT->addModule(std::move(TheModule));
	InitializeModuleAndPassManager();

	// Search the JIT for the __anon_expr symbol.
	auto ExprSymbol = TheJIT->findSymbol("main");
	assert(ExprSymbol && "未找到main函数");//到时候应该要修改

	// Get the symbol's address and cast it to the right type (takes no
	// arguments, returns a double) so we can call it as a native function.

    /*double (*FP)() = (double (*)())(intptr_t)cantFail(ExprSymbol.getAddress());
	fprintf(stderr, "main exited with value %f\n", FP());*/
	int (*FP)() = (int (*)())(intptr_t)cantFail(ExprSymbol.getAddress());
	fprintf(stderr, "main exited with value %d\n", FP());

	// Delete the anonymous expression module from the JIT.
	TheJIT->removeModule(H);
}
