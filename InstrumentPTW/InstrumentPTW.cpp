#include "InstrumentPTW.h"

bool InstrumentPTW::runOnModule(Module &M) {
	////errs() << "hello from InstrumentPTW::runOnModule\n";

	vector<Instruction *> workSet;

	for (auto & F : M)
		for (auto & BB : F) {
			for (auto & I : BB) { 
				if (CallInst * CI = dyn_cast<CallInst>(&I)) {

					Function * calledF = CI->getCalledFunction();
					if (calledF == nullptr)
						workSet.push_back(CI);

				} else if (InvokeInst * II = dyn_cast<InvokeInst>(&I)) {

					Function * calledF = II->getCalledFunction();
					if (calledF == nullptr)
						workSet.push_back(II);

				}
			}
		}

	IRBuilder<> builder(M.getContext());
	for (auto I : workSet) {

		CallSite CS(I);

		Value * calledV = CS.getCalledValue();

		if (isa<InlineAsm>(calledV)) continue;

		builder.SetInsertPoint(I);

		InlineAsm *Asm = InlineAsm::get(
				FunctionType::get(builder.getVoidTy(), {calledV->getType()}, false),
				"ptwriteq $0", "r,~{dirflag},~{fpsr},~{flags}", 
				true);
		builder.CreateCall(Asm, calledV);
	}

	return true;
}

char InstrumentPTW::ID = 0;
static RegisterPass<InstrumentPTW> X("InstrumentPTW", "InstrumentPTW pass", false, false);

static void register_pass(const PassManagerBuilder &PMB,
		legacy::PassManagerBase &PM) {
	PM.add(new InstrumentPTW());
}

static RegisterStandardPasses RegisterPass(
		PassManagerBuilder::EP_OptimizerLast, register_pass);
static RegisterStandardPasses RegisterPass1(
		PassManagerBuilder::EP_EnabledOnOptLevel0, register_pass);
