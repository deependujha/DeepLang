#include <iostream>
#include <string>
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

llvm::Value* CodeGen::codegen(const ast::IfExprAST& ifAst) {
    if (!(!ifAst.cond.empty() && ifAst.then.size() - ifAst.cond.size() <= 1)) {
        LogErrorV("invalid if-expression AST");
        return nullptr;
    }
    llvm::Value* CondV = this->codegen(*ifAst.cond[0]);
    if (!CondV) {
        return nullptr;
    }

    // Convert condition to a bool by comparing non-equal to 0.0.
    CondV = this->Builder->CreateFCmpONE(
        CondV,
        llvm::ConstantFP::get(*TheContext, llvm::APFloat(0.0)),
        "ifcond");

    llvm::Function* TheFunction = this->Builder->GetInsertBlock()->getParent();

    // Create blocks for the then and else cases.  Insert the 'then' block at
    // the end of the function.
    llvm::BasicBlock* ThenBB =
        llvm::BasicBlock::Create(*TheContext, "then", TheFunction);
    llvm::BasicBlock* ElseBB =
        llvm::BasicBlock::Create(*TheContext, "else", TheFunction);
    llvm::BasicBlock* MergeBB =
        llvm::BasicBlock::Create(*TheContext, "ifcont", TheFunction);

    this->Builder->CreateCondBr(CondV, ThenBB, ElseBB);

    // Emit then value.
    Builder->SetInsertPoint(ThenBB);

    llvm::Value* ThenV = this->codegen(*ifAst.then[0]);
    if (!ThenV) {
        return nullptr;
    }

    this->Builder->CreateBr(MergeBB);
    // Codegen of 'Then' can change the current block, update ThenBB for the
    // PHI.
    ThenBB = this->Builder->GetInsertBlock();

    // Emit else block.
    // TheFunction->insert(TheFunction->end(), ElseBB);
    this->Builder->SetInsertPoint(ElseBB);

    llvm::Value* ElseV = this->codegen(*ifAst.then[1]);
    if (!ElseV) {
        return nullptr;
    }

    this->Builder->CreateBr(MergeBB);
    // Codegen of 'Else' can change the current block, update ElseBB for the
    // PHI.
    ElseBB = this->Builder->GetInsertBlock();

    // Emit merge block.
    // TheFunction->insert(TheFunction->end(), MergeBB);
    this->Builder->SetInsertPoint(MergeBB);
    llvm::PHINode* PN =
        Builder->CreatePHI(llvm::Type::getDoubleTy(*TheContext), 2, "iftmp");

    PN->addIncoming(ThenV, ThenBB);
    PN->addIncoming(ElseV, ElseBB);
    return PN;
}

} // namespace codegen