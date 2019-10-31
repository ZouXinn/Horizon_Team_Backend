#include "KaleidoscopeJIT.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
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


static std::unique_ptr<KaleidoscopeJIT> TheJIT;

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

struct IdItem
{
	Value* decVarName;
	Value* val;
};

static int currentType;

extern int testInt;
extern map<string, int*> testMap;


