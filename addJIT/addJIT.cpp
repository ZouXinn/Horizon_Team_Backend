#include "../KaleidoscopeJIT.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
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
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "../ClassProject.h"
static std::string IdentifierStr; // Filled in if tok_identifier
static double NumVal;             // Filled in if tok_number
using namespace llvm;
using namespace llvm::orc;
static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<Module> TheModule;
static std::unique_ptr<legacy::FunctionPassManager> TheFPM;
static std::unique_ptr<KaleidoscopeJIT> TheJIT;
static std::map<char, int> BinopPrecedence;
//static std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;
//ΪJIT

void Init() {
	InitializeNativeTarget();
	InitializeNativeTargetAsmPrinter();
	InitializeNativeTargetAsmParser();

	// Install standard binary operators.
	// 1 is lowest precedence.
	BinopPrecedence['<'] = 10;
	BinopPrecedence['+'] = 20;
	BinopPrecedence['-'] = 20;
	BinopPrecedence['*'] = 40; // highest.
	TheJIT = std::make_unique<KaleidoscopeJIT>();
	InitializeModuleAndPassManager();
}
void addJIT() {

	// JIT the module containing the anonymous expression, keeping a handle so
	 // we can free it later.
	auto H = TheJIT->addModule(std::move(TheModule));
	InitializeModuleAndPassManager();

	// Search the JIT for the __anon_expr symbol.
	auto ExprSymbol = TheJIT->findSymbol("__anon_expr");
	assert(ExprSymbol && "Function not found");

	// Get the symbol's address and cast it to the right type (takes no
	// arguments, returns a double) so we can call it as a native function.
	(intptr_t)ExprSymbol.getAddress();
	double (*FP)() = (double (*)())(intptr_t)ExprSymbol.getAddress();
	fprintf(stderr, "Evaluated to %f\n", FP());

	// Delete the anonymous expression module from the JIT.
	TheJIT->removeModule(H);

	Init();
	TheJIT->addModule(std::move(TheModule));
	InitializeModuleAndPassManager();
}





