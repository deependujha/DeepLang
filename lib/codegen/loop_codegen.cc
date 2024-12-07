#include "codegen/codegen.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

namespace codegen {
llvm::Value* CodeGen::codegen(const ast::LoopExprAST& loopAst) {
    llvm::Function* TheFunction = Builder->GetInsertBlock()->getParent();

    // Create an alloca for the variable in the entry block.
    llvm::AllocaInst* Alloca =
        this->CreateEntryBlockAlloca(TheFunction, loopAst.VarName);

    // Emit the start code first, without 'variable' in scope.
    llvm::Value* StartVal = this->codegen(*loopAst.Start);
    if (!StartVal) {
        return nullptr;
    }

    // Store the value into the alloca.
    this->Builder->CreateStore(StartVal, Alloca);

    // Make the new basic block for the loop header, inserting after current
    // block.
    llvm::BasicBlock* LoopBB =
        llvm::BasicBlock::Create(*TheContext, "loop", TheFunction);

    // Insert an explicit fall through from the current block to the LoopBB.
    this->Builder->CreateBr(LoopBB);

    // Start insertion in LoopBB.
    this->Builder->SetInsertPoint(LoopBB);

    // Within the loop, the variable is defined equal to the PHI node.  If it
    // shadows an existing variable, we have to restore it, so save it now.
    llvm::AllocaInst* OldVal = this->NamedValues.back()[loopAst.VarName];
    this->NamedValues.back()[loopAst.VarName] = Alloca;

    // Emit the body of the loop.  This, like any other expr, can change the
    // current BB.  Note that we ignore the value computed by the body, but
    // don't allow an error.
    if (!this->codegen(*loopAst.Body)) {
        return nullptr;
    }

    // Emit the step value.
    llvm::Value* StepVal = nullptr;
    if (loopAst.Step) {
        StepVal = this->codegen(*loopAst.Step);
        if (!StepVal) {
            return nullptr;
        }
    } else {
        // If not specified, use 1.0.
        StepVal = llvm::ConstantFP::get(*TheContext, llvm::APFloat(1.0));
    }

    // Compute the end condition.
    llvm::Value* EndCond = this->codegen(*loopAst.End);
    if (!EndCond) {
        return nullptr;
    }

    // Reload, increment, and restore the alloca.  This handles the case where
    // the body of the loop mutates the variable.
    llvm::Value* CurVar = this->Builder->CreateLoad(
        Alloca->getAllocatedType(), Alloca, loopAst.VarName.c_str());
    llvm::Value* NextVar =
        this->Builder->CreateFAdd(CurVar, StepVal, "nextvar");
    this->Builder->CreateStore(NextVar, Alloca);

    // Convert condition to a bool by comparing non-equal to 0.0.
    EndCond = Builder->CreateFCmpONE(
        EndCond,
        llvm::ConstantFP::get(*TheContext, llvm::APFloat(0.0)),
        "loopcond");

    // Create the "after loop" block and insert it.
    llvm::BasicBlock* AfterBB =
        llvm::BasicBlock::Create(*TheContext, "afterloop", TheFunction);

    // Insert the conditional branch into the end of LoopEndBB.
    this->Builder->CreateCondBr(EndCond, LoopBB, AfterBB);

    // Any new code will be inserted in AfterBB.
    this->Builder->SetInsertPoint(AfterBB);

    // Restore the unshadowed variable.
    if (OldVal) {
        this->NamedValues.back()[loopAst.VarName] = OldVal;
    } else {
        this->NamedValues.back().erase(loopAst.VarName);
    }

    // for expr always returns 0.0.
    return llvm::Constant::getNullValue(llvm::Type::getDoubleTy(*TheContext));
}

} // namespace codegen