#include <assert.h>
#include <stdio.h>

#include <iostream>
#include <map>
#include <vector>
#include <set>

#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
//#include "llvm/IR/TypeBuilder.h"  //NOTE: If I include this line, compilation will fail.
#include "llvm/IRReader/IRReader.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;
using namespace std;

// This is legacy pass manager; must provide clang flag '-flegacy-pass-manager'
class LegacyIRDumper : public ModulePass {

public:
	static char ID;

	LegacyIRDumper() : ModulePass(ID) {}

	virtual bool runOnModule(Module &M);
};

// FIXME: the following does not work with the new pass manager.
// Refer to https://llvm.org/docs/NewPassManager.html
class IRDumper : public PassInfoMixin<IRDumper> {

public:
	virtual PreservedAnalyses run(Module &M, ModuleAnalysisManager &);
};

