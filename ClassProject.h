#include "KaleidoscopeJIT.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/DIBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <map>

#include "Enums_Structs.h"

#include "llvm/IR/ValueMap.h"
#include "llvm/IR/DerivedTypes.h"

using namespace std;

using namespace llvm;
using namespace llvm::orc;


extern LLVMContext TheContext;

extern IRBuilder<> Builder;

extern std::unique_ptr<Module> TheModule;

extern std::map<std::string, AllocaInst*> NamedValues;

extern std::map<std::string, AllocaInst*> GlobalValues;

extern std::map<std::string, Value*> Params;

extern std::map<std::string, GlobalVariable*> GV;

extern std::unique_ptr<KaleidoscopeJIT> TheJIT;

extern AllocaInst* CreateEntryBlockAlloca(Function* TheFunction,
	const std::string& VarName, zx::Type type);

extern AllocaInst* CreateEntryBlockAlloca(Function* TheFunction,
	const std::string& VarName, llvm::Type* type);

extern Function* currentFun;
extern FunctionType* currentFunType;
extern BasicBlock* BB;

extern std::unique_ptr<legacy::FunctionPassManager> TheFPM;
extern void InitializeModuleAndPassManager(void);

extern void InitializeModule();

extern BasicBlock* updateBB();

extern AllocaInst* getHighestValue(string str);
extern string getHighestStr(string str);

struct IdItem
{
	Value* decVarName;
	Value* val;
};

struct FunIndex {
	string FN = "";
	string PR = "";
	friend bool operator < (const struct FunIndex& FI1, const struct FunIndex& FI2) {
		if (FI1.FN != FI2.FN) {
			return FI1.FN < FI2.FN;
		}
		else {
			return FI1.PR < FI2.PR;
		}
	}
};


extern map<FunIndex, string> FuncNames;

extern std::map <string, Function*> Funcs;
static int currentType;

extern int testInt;
extern map<string, int*> testMap;

//11.12�� 18��55���ĺ�����
extern BasicBlock* currentRetBB;
extern Value* currentRetValue;
extern PHINode* currentRetPN;

//debug part
//DIBuilder* DBuilder = nullptr;
//struct DebugInfo {
//	DICompileUnit* TheCU = nullptr;
//	DIType* DblTy = nullptr;
//	DIType* IntTy = nullptr;
//
//	DIType* getDoubleTy() {
//		if (DblTy)
//			return DblTy;
//
//		DblTy = DBuilder->createBasicType("real", 64, dwarf::DW_ATE_float);
//		return DblTy;
//	}
//	DIType* getIntegerTy() {//unfinished
//		if (IntTy)
//			return IntTy;
//		//IntTy = DBuilder->createBasicType("int", 32, dwarf::DW_ATE_i)
//		return IntTy;
//	}
//};
//
//extern DebugInfo DbgInfo;

//extern int strIndex;
//void CreateWriteStr(string str);