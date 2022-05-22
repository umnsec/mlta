#ifndef _ANALYZER_GLOBAL_H
#define _ANALYZER_GLOBAL_H

#include <llvm/IR/DebugInfo.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Instructions.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/SmallPtrSet.h>
#include <llvm/ADT/StringExtras.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include "llvm/Support/CommandLine.h"
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Common.h"


// 
// typedefs
//
typedef std::vector< std::pair<llvm::Module*, llvm::StringRef> > ModuleList;
// Mapping module to its file name.
typedef std::unordered_map<llvm::Module*, llvm::StringRef> ModuleNameMap;
// The set of all functions.
typedef llvm::SmallPtrSet<llvm::Function*, 8> FuncSet;
typedef llvm::SmallPtrSet<llvm::CallInst*, 8> CallInstSet;
typedef DenseMap<Function*, CallInstSet> CallerMap;
typedef DenseMap<CallInst *, FuncSet> CalleeMap;

struct GlobalContext {

	GlobalContext() {}

	// Statistics 
	unsigned NumFunctions = 0;
	unsigned NumFirstLayerTypeCalls = 0;
	unsigned NumSecondLayerTypeCalls = 0;
	unsigned NumSecondLayerTargets = 0;
	unsigned NumValidIndirectCalls = 0;
	unsigned NumIndirectCallTargets = 0;
	unsigned NumFirstLayerTargets = 0;

	// Global variables
	DenseMap<size_t, GlobalVariable *>Globals;
	
	// Map global function GUID (uint64_t) to its actual function with body.
	map<uint64_t, Function*> GlobalFuncMap;

	// Functions whose addresses are taken.
	FuncSet AddressTakenFuncs;

	// Map a callsite to all potential callee functions.
	CalleeMap Callees;

	// Map a function to all potential caller instructions.
#ifdef MAP_CALLER_TO_CALLEE
	CallerMap Callers;
#endif

	// Map function signature to functions
	DenseMap<size_t, FuncSet>sigFuncsMap;

	// Indirect call instructions.
	std::vector<CallInst *>IndirectCallInsts;

	// Modules.
	ModuleList Modules;
	ModuleNameMap ModuleMaps;
	std::set<std::string> InvolvedModules;

};

class IterativeModulePass {
protected:
	const char * ID;
public:
	IterativeModulePass(GlobalContext *Ctx_, const char *ID_)
		: ID(ID_) { }

	// Run on each module before iterative pass.
	virtual bool doInitialization(llvm::Module *M)
		{ return true; }

	// Run on each module after iterative pass.
	virtual bool doFinalization(llvm::Module *M)
		{ return true; }

	// Iterative pass.
	virtual bool doModulePass(llvm::Module *M)
		{ return false; }

	virtual void run(ModuleList &modules);
};

#endif
