#include "ClassProject.h"

LLVMContext TheContext;
std::unique_ptr<Module> TheModule;

std::map<std::string, AllocaInst*> NamedValues;
std::map<std::string, AllocaInst*> GlobalValues;
std::map<std::string, GlobalVariable*> GV;
std::map<std::string, Value*> Params;
std::map<FunIndex, string> FuncNames;
std::map <string, Function*> Funcs;
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

//inline bool operator < (const struct FunIndex& FI1, const struct FunIndex& FI2) {
//	if (FI1.FN != FI2.FN) {
//		return FI1.FN < FI2.FN;
//	}
//	else {
//		return FI1.PR < FI2.PR;
//	}
//}
AllocaInst* getHighestValue(string str) //返回AllocaInst*
{
	int i = str.length();
	for (; i >= 0; i--) {
		if (str[i] == '_' && i > 0 && str[i-1] == '_') {
			break;
		}
	}
	i++;
	string substr = str.substr(0, i);
	int num = 0;
	for (; i < str.length(); i++) {
		num *= 10;
		num += str[i] - '0';
	}
	for (; num >= 1; num--)
	{
		string ts = substr + to_string(num);
		if (NamedValues.count(ts) == 1) {//找到
			return NamedValues[ts];
		}
	}
	string pts = substr + to_string(1)+".addr";
	if (Params.count(pts) == 1){
		return ((AllocaInst*)Params[pts]);
	}
	string ts = substr + to_string(num);
	if (GV.count(ts) == 1) {
		//return GlobalValues[ts];
		/*if (AllocaInst::classof(GV[ts])) {
			return ((AllocaInst*)GV[ts]);
		}
		else {
			return ((AllocaInst*)GV[ts]);
		}*/
		//return AlGV[ts];
		return ((AllocaInst*)GV[ts]);
	}
	else {
		return nullptr;
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

void initOthers() {
	InitializeNativeTarget();
	InitializeNativeTargetAsmPrinter();
	InitializeNativeTargetAsmParser();
	TheJIT = std::make_unique<KaleidoscopeJIT>();
}

void InitializeModuleAndPassManager(void) {

	// Open a new module.
	TheModule = std::make_unique<Module>("my cool jit", TheContext);

	//set layout
	TheModule->setDataLayout(TheJIT->getTargetMachine().createDataLayout());


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
	initOthers();
	/*InitializeModule();*/
	InitializeModuleAndPassManager();

	/*currentFunType = FunctionType::get(llvm::Type::getVoidTy(TheContext),
		{ llvm::Type::getInt32Ty(TheContext) }, false);
	currentFun = Function::Create(currentFunType, Function::ExternalLinkage, "VarDec", *TheModule);
	BB = BasicBlock::Create(TheContext, "EntryBlock", currentFun);
	Builder.SetInsertPoint(BB);
	return BB;*/
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

//extern "C" DLLEXPORT void writeStr(string str) {
//	fprintf(stdout, "%s\n", str);
//}
BasicBlock* currentRetBB;
Value* currentRetValue;
PHINode* currentRetPN;


