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
    // Emit the start code first, without 'variable' in scope.
    llvm::Value* StartVal = this->codegen(*loopAst.Start);
    if (!StartVal) {
        return nullptr;
    }

    // Make the new basic block for the loop header, inserting after current
    // block.
    llvm::Function* TheFunction = this->Builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* PreheaderBB = this->Builder->GetInsertBlock();
    llvm::BasicBlock* LoopBB =
        llvm::BasicBlock::Create(*TheContext, "loop", TheFunction);

    // Insert an explicit fall through from the current block to the LoopBB.
    this->Builder->CreateBr(LoopBB);

    // Start insertion in LoopBB.
    this->Builder->SetInsertPoint(LoopBB);

    // Start the PHI node with an entry for Start.
    llvm::PHINode* Variable = Builder->CreatePHI(
        llvm::Type::getDoubleTy(*TheContext), 2, loopAst.VarName);
    Variable->addIncoming(StartVal, PreheaderBB);

    // Within the loop, the variable is defined equal to the PHI node.  If it
    // shadows an existing variable, we have to restore it, so save it now.
    llvm::Value* OldVal = this->NamedValues[loopAst.VarName];
    this->NamedValues[loopAst.VarName] = Variable;

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

    llvm::Value* NextVar = Builder->CreateFAdd(Variable, StepVal, "nextvar");

    // Compute the end condition.
    llvm::Value* EndCond = this->codegen(*loopAst.End);
    if (!EndCond) {
        return nullptr;
    }

    // Convert condition to a bool by comparing non-equal to 0.0.
    EndCond = Builder->CreateFCmpONE(
        EndCond,
        llvm::ConstantFP::get(*TheContext, llvm::APFloat(0.0)),
        "loopcond");

    // Create the "after loop" block and insert it.
    llvm::BasicBlock* LoopEndBB = this->Builder->GetInsertBlock();
    llvm::BasicBlock* AfterBB =
        llvm::BasicBlock::Create(*TheContext, "afterloop", TheFunction);

    // Insert the conditional branch into the end of LoopEndBB.
    this->Builder->CreateCondBr(EndCond, LoopBB, AfterBB);

    // Any new code will be inserted in AfterBB.
    this->Builder->SetInsertPoint(AfterBB);

    // Add a new entry to the PHI node for the backedge.
    Variable->addIncoming(NextVar, LoopEndBB);

    // Restore the unshadowed variable.
    if (OldVal) {
        NamedValues[loopAst.VarName] = OldVal;
    } else {
        NamedValues.erase(loopAst.VarName);
    }

    // for expr always returns 0.0.
    return llvm::Constant::getNullValue(llvm::Type::getDoubleTy(*TheContext));
}

} // namespace codegen