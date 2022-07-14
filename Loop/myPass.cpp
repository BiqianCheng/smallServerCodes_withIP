#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/CallSite.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Support/raw_ostream.h>
#include "llvm/Analysis/LoopPass.h"
#include <llvm/IR/BasicBlock.h>
#include <glog/logging.h>
#include <iostream>
#include <set>

struct Config {
    std::vector<std::string> summarized;
    std::vector<std::string> inlined;
    std::vector<std::string> recursive_summarized;

    // enterBlocks (numbers of enterBlocks could be larger than number of headerBlocks)
    std::map<llvm::BasicBlock*, int> enterBlocks;
    std::map<llvm::BasicBlock*, int> headerBlocks;
    int i = 0;

    // enterBlocks must have equal number of headerBlocks
    std::vector<llvm::BasicBlock*> exitingBlocks;
    std::vector<llvm::BasicBlock*> exitBlocks;
};


class LoopMaxIterPass : public llvm::LoopPass {
public:
    static char ID;
    Config& config;
    

    LoopMaxIterPass(Config& config);
    ~LoopMaxIterPass() {}

    void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
    bool runOnLoop(llvm::Loop *L, llvm::LPPassManager &LPM) override;

    void FindExits(llvm::Loop *L);
};


char LoopMaxIterPass::ID = 0;

LoopMaxIterPass::LoopMaxIterPass(Config& config) : llvm::LoopPass(ID), config(config) {
}

bool LoopMaxIterPass::runOnLoop(llvm::Loop *L, llvm::LPPassManager &LPM) {
    llvm::Function* F = (*(L->block_begin()))->getParent();
    if (std::find(config.summarized.begin(), config.summarized.end(), 
            F->getName().str()) == config.summarized.end()) {
        return false;
    }

    LOG(INFO) << "runOnLoop : ";
    llvm::LoopInfo &LI = getAnalysis<llvm::LoopInfoWrapperPass>().getLoopInfo();
    // LI.print(errs());
    L->dump();
    auto* h = L->getHeader();
    llvm::SmallVector<llvm::BasicBlock *, 10> LoopLatches;
    L->getLoopLatches(LoopLatches);
    std::set<llvm::BasicBlock*> Latches;
    for (auto* p : LoopLatches) Latches.insert(p);

    for (auto *pred : predecessors(h)) {
        if (Latches.count(pred)) {
            config.enterBlocks[pred] = config.i;
        }
    }
    config.headerBlocks[h] = config.i;
    config.i++;

    FindExits(L);

    return false;
}

void LoopMaxIterPass::FindExits(llvm::Loop *L) {
    llvm::SmallVector<llvm::BasicBlock *, 10> LoopExitBlocks, LoopExitingBlocks;
    L->getExitBlocks(LoopExitBlocks);
    L->getExitingBlocks(LoopExitingBlocks);
    
    // create set for exits
    std::set<llvm::BasicBlock*> ExitBlocks;
    for (auto* p : LoopExitBlocks)
        ExitBlocks.insert(p); 

    for (auto* e : LoopExitingBlocks) {
        for (auto *p : successors(e)) {
            if (ExitBlocks.count(p)) { // this is the exit
                config.exitingBlocks.push_back(e);
                config.exitBlocks.push_back(p);
            }
        }
    }
}

void LoopMaxIterPass::getAnalysisUsage(llvm::AnalysisUsage &AU) const {
    AU.addRequired<llvm::LoopInfoWrapperPass>();
}