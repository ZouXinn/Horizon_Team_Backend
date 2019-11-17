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
string getHighestStr(string str) {

	
	int i = str.length();
	for (; i >= 0; i--) {
		if (str[i] == '_' && i > 0 && str[i - 1] == '_') {
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
			return ts;
		}
	}
	string pts = substr + to_string(1) + ".addr";
	if (Params.count(pts) == 1) {
		return pts;
	}
	string ts = substr + to_string(num);
	if (GV.count(ts) == 1) {
		return ts;
	}
	else {
		return "";
	}
}



vector<string> split(string str) {//根据.分开
	vector<string> ret;
	string ts = "";
	int finishedNum = 0;
	for (int i = 0; i < str.length(); i++) {
		if (str[i] != '.') {
			ts += str[i];
		}
		else {
			ret.push_back(ts);
			ts = "";
		}
	}
	if (ts != "") {
		ret.push_back(ts);
	}
	return ret;
}

//
//冯文翰于11月17日9：38新增
//
string getHighestStr(string str, int type) {
	if (type < 0 || type > 3) {
		return "";
	}
	int i = str.length();
	vector<string> strs = split(str);
	if (strs.size() != 2) {
		return "";
	}
	int oriLevel = 0;
	for (int i = 0; i < strs[1].length(); i++) {
		oriLevel *= 10;
		oriLevel += strs[1][i] - '0';
	}
	for (int i = oriLevel; i >= 0; i--) {
		string tName = strs[0] + '.' + to_string(i) + '.' + to_string(type);
		if (NamedValues.count(tName) == 1) {
			return tName;
		}
	}
	string tPName = strs[0] + '.' + to_string(1) + ".addr";
	if (Params.count(tPName) == 1) {
		return tPName;
	}
	string tGName = strs[0] + '.' + to_string(0) + '.' + to_string(type);
	if (GV.count(tGName) == 1) {
		return tGName;
	}
	return "";
}


AllocaInst* getHighestValue(string str, int type) {//str ---> name.level
	if (type < 0 || type > 3) {
		return nullptr;
	}
	int i = str.length();
	vector<string> strs = split(str);
	if (strs.size() != 2) {
		return nullptr;
	}
	int oriLevel = 0;
	for (int i = 0; i < strs[1].length(); i++) {
		oriLevel *= 10;
		oriLevel += strs[1][i] - '0';
	}
	for (int i = oriLevel; i >= 0; i--) {
		string tName = strs[0] + '.' + to_string(i) + '.' + to_string(type);
		if (NamedValues.count(tName) == 1) {
			return NamedValues[tName];
		}
	}
	string tPName = strs[0] + '.' + to_string(1) + ".addr";
	if (Params.count(tPName) == 1) {
		return (AllocaInst*)Params[tPName];
	}
	string tGName = strs[0] + '.' + to_string(0) + '.' + to_string(type);
	if (GV.count(tGName) == 1) {
		return (AllocaInst*)GV[tGName];
	}
	return nullptr;
}
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
	std::vector<Type*> WTSTR(1, Type::getInt8PtrTy(TheContext));
	FunctionType* readIntFT = FunctionType::get(Type::getInt32Ty(TheContext), false);
	FunctionType* readDoubleFT = FunctionType::get(Type::getDoubleTy(TheContext), false);
	FunctionType* writeIntFT = FunctionType::get(Type::getVoidTy(TheContext), WTINT, false);
	FunctionType* writeDoubleFT = FunctionType::get(Type::getVoidTy(TheContext), WTDB, false);
	FunctionType* writeStringFT = FunctionType::get(Type::getVoidTy(TheContext), WTSTR, false);
	
	Function::Create(readIntFT, Function::ExternalLinkage, "readInt", TheModule.get());
	Function::Create(readDoubleFT, Function::ExternalLinkage, "readDouble", TheModule.get());
	Function::Create(writeIntFT, Function::ExternalLinkage, "writeInt", TheModule.get());
	Function::Create(writeDoubleFT, Function::ExternalLinkage, "writeDouble", TheModule.get());
	Function::Create(writeStringFT, Function::ExternalLinkage, "writeString", TheModule.get());
	


	//
	std::vector<Type*> WTCHAR(1, Type::getInt8Ty(TheContext));
	FunctionType* writeCharFT = FunctionType::get(Type::getVoidTy(TheContext), WTCHAR, false);
	Function::Create(writeCharFT, Function::ExternalLinkage, "writeChar", TheModule.get());
	

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
extern "C" DLLEXPORT void writeString(const char* cp) {
	fprintf(stdout, "%s\n",cp);
}

BasicBlock* currentRetBB;
Value* currentRetValue;
PHINode* currentRetPN;

Function* CreateInterprterFunction()
{

	string Name = "interpreterFunction";


	std::vector<Type*> params;
	FunctionType* FT;

	FT = FunctionType::get((Type*)Type::getVoidTy(TheContext), params, false);

	Function* interFunc = Function::Create(FT, Function::ExternalLinkage, Name, TheModule.get());

	// Create a new basic block to start insertion into.
	BasicBlock* BB = BasicBlock::Create(TheContext, "entry", interFunc);
	Builder.SetInsertPoint(BB);

	verifyFunction(*interFunc);

	/*Function* func = nullptr;
	Value* Val = nullptr;
	map<string, AllocaInst*>::iterator iter;
	for (auto iter = NamedValues.begin(); iter != NamedValues.end(); iter++) {
		Val = Builder.CreateLoad(iter->second);
		if (Val->getType()->isIntegerTy()) {
			func = TheModule->getFunction("writeInt");
		}
		else if (Val->getType()->isDoubleTy())
		{
			func = TheModule->getFunction("writeDouble");
		}


		Builder.CreateCall(func, Val);
	}*/

	Builder.CreateFAdd(ConstantFP::get(TheContext, APFloat(1.0)), ConstantFP::get(TheContext, APFloat(1.0)));

	Builder.CreateRetVoid();

	return interFunc;
}


int strIndex = 0;

void CreateWriteStr(string str) {
	llvm::ArrayRef<llvm::Constant*> V;
	vector<Constant*> constVector;
	for (int i = 0; i < str.length(); i++) {
		constVector.push_back(ConstantInt::get(TheContext, APInt(8, str[i])));
	}
	constVector.push_back(ConstantInt::get(TheContext, APInt(8, 0)));
	V = llvm::ArrayRef<llvm::Constant*>(constVector);

	string strName = "str__" + to_string(strIndex);
	Value* idx = ConstantInt::get(TheContext, APInt(32, 0));
	ArrayRef<llvm::Value*> idxs = ArrayRef<llvm::Value*>(idx);

	//创建Array
	ArrayType* arrType = ArrayType::get(Type::getInt8Ty(TheContext), str.length() + 1);
	AllocaInst* strArr = CreateEntryBlockAlloca(currentFun, strName, arrType);
	//给Array赋初值
	Constant* constantArr = ConstantArray::get(arrType, V);
	Value* arrVal = Builder.CreateStore(constantArr, strArr);

	//获取ElementPtr
	arrVal->print(errs());

	Type* type = arrType->getElementType();
	type->print(errs());

	GetElementPtrInst* i = GetElementPtrInst::Create(type, strArr, idxs);



	strIndex++;
}

extern "C" DLLEXPORT void writeChar(int8_t x) {
	fputc((char)x, stdout);
}


void writeChar(string s) {
	Function* func = TheModule->getFunction("writeChar");
	for (int i = 0; i < s.size(); i++) {
		int8_t a = s[i];
		Value* Val = ConstantInt::get(IntegerType::get(TheContext, 8), APInt(8, a));
		Builder.CreateCall(func, Val);
	}
}
