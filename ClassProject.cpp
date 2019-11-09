#include "ClassProject.h"

LLVMContext TheContext;
std::unique_ptr<Module> TheModule;

std::map<std::string, AllocaInst*> NamedValues;
map<string, int*> testMap;
IRBuilder<> Builder(TheContext);

std::unique_ptr<KaleidoscopeJIT> TheJIT;

AllocaInst* CreateEntryBlockAlloca(Function* TheFunction,
	const std::string& VarName, zx::Type type) {
	if (type == zx::Type::INT) {
		IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
			TheFunction->getEntryBlock().begin());
		return std::move(TmpB.CreateAlloca(llvm::Type::getInt32Ty(TheContext), nullptr,
			VarName.c_str()));
	}
}

AllocaInst* CreateEntryBlockAlloca(Function* TheFunction,
	const std::string& VarName, llvm::Type* type) {
	IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
		TheFunction->getEntryBlock().begin());
	return TmpB.CreateAlloca(type, nullptr, VarName.c_str());
}

int testInt = 0;

void InitializeModule() {
	// Open a new module.
	TheModule = std::make_unique<Module>("my cool jit", TheContext);

	/*TheModule->setDataLayout(TheJIT->getTargetMachine().createDataLayout());*/
}

Function* currentFun = nullptr;
FunctionType* currentFunType = nullptr;
BasicBlock* BB = nullptr;

std::unique_ptr<legacy::FunctionPassManager> TheFPM;

void InitializeModuleAndPassManager(void) {
	// Open a new module.
	TheModule = std::make_unique<Module>("my cool jit", TheContext);

	// Create a new pass manager attached to it.
	TheFPM = std::make_unique<legacy::FunctionPassManager>(TheModule.get());

	// Do simple "peephole" optimizations and bit-twiddling optzns.
	TheFPM->add(createInstructionCombiningPass());
	// Reassociate expressions.
	TheFPM->add(createReassociatePass());
	// Eliminate Common SubExpressions.
	TheFPM->add(createGVNPass());
	// Simplify the control flow graph (deleting unreachable blocks, etc).
	TheFPM->add(createCFGSimplificationPass());

	TheFPM->doInitialization();

	////init read and write function

	//万花筒示例
	//std::vector<Type*> Doubles(1,Type::getDoubleTy(TheContext));
	//FunctionType* FT = FunctionType::get(Type::getDoubleTy(TheContext), Doubles, false);
	//Function* F = Function::Create(FT, Function::ExternalLinkage, Name, TheModule.get());
	//万花筒示例结束

	std::vector<Type*> WTINT(1, Type::getInt32Ty(TheContext));
	std::vector<Type*> WTDB(1, Type::getDoubleTy(TheContext));
	FunctionType* readIntFT = FunctionType::get(Type::getInt32Ty(TheContext), false);
	FunctionType* readDoubleFT = FunctionType::get(Type::getDoubleTy(TheContext), false);
	FunctionType* writeIntFT = FunctionType::get(Type::getVoidTy(TheContext), WTINT, false);
	FunctionType* writeDoubleFT = FunctionType::get(Type::getVoidTy(TheContext), WTDB, false);
	
	Function::Create(readIntFT, Function::ExternalLinkage, "readInt", TheModule.get());
	Function::Create(readDoubleFT, Function::ExternalLinkage, "readDouble", TheModule.get());
	Function::Create(writeIntFT, Function::ExternalLinkage, "writeInt", TheModule.get());
	Function::Create(writeDoubleFT, Function::ExternalLinkage, "writeDouble", TheModule.get());

	//

}


BasicBlock* updateBB()
{

	/*InitializeModule();*/
	InitializeModuleAndPassManager();

	currentFunType = FunctionType::get(llvm::Type::getVoidTy(TheContext),
		{ llvm::Type::getInt32Ty(TheContext) }, false);
	currentFun = Function::Create(currentFunType, Function::ExternalLinkage, "VarDec", *TheModule);
	BB = BasicBlock::Create(TheContext, "EntryBlock", currentFun);
	Builder.SetInsertPoint(BB);
	return BB;
}


#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif
///read
extern "C" DLLEXPORT int readInt() {
	int inputInt;
	fscanf(stdin, "%d", &inputInt);//是否需要取消& ？
	return inputInt;
}
extern "C" DLLEXPORT double readDouble() {
	double inputDouble;
	fscanf(stdin, "%f", &inputDouble);
	return inputDouble;
}

///write
extern "C" DLLEXPORT void writeInt(int i) {
	fprintf(stdout, "%d\n", i);
}
extern "C" DLLEXPORT void writeDouble(double d) {
	fprintf(stdout, "%f\n", d);
}

