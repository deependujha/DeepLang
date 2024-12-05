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

// llvm::Function* CodeGen::codegen(const ast::IfExprAST& ifAst) {
//     llvm::Function* function = this->Builder->GetInsertBlock()->getParent();

//     // Create a merge block for after the if/elif/else ladder
//     llvm::BasicBlock* mergeBlock =
//         llvm::BasicBlock::Create(*this->TheContext, "merge", function);

//     // Track the block where we are currently emitting
//     llvm::BasicBlock* currentBlock = this->Builder->GetInsertBlock();

//     // Create blocks for conditions and corresponding 'then' bodies
//     std::vector<llvm::BasicBlock*> condBlocks;
//     std::vector<llvm::BasicBlock*> thenBlocks;

//     for (size_t i = 0; i < ifAst.cond.size(); ++i) {
//         condBlocks.push_back(llvm::BasicBlock::Create(
//             *this->TheContext, "cond" + std::to_string(i), function));
//         thenBlocks.push_back(llvm::BasicBlock::Create(
//             *this->TheContext, "then" + std::to_string(i), function));
//     }

//     // Create the 'else' block if present
//     llvm::BasicBlock* elseBlock = nullptr;
//     if (ifAst.cond.size() < ifAst.then.size()) {
//         elseBlock =
//             llvm::BasicBlock::Create(*this->TheContext, "else", function);
//     }

//     // Generate code for each condition
//     for (size_t i = 0; i < ifAst.cond.size(); ++i) {
//         // Emit code for condition
//         this->Builder->SetInsertPoint(currentBlock);
//         llvm::Value* condition = this->codegen(*ifAst.cond[i]);
//         if (!condition) {
//             return nullptr;
//         }

//         // Create a conditional branch to 'then' or the next condition/else
//         llvm::BasicBlock* nextBlock = (i + 1 < ifAst.cond.size())
//             ? condBlocks[i + 1]
//             : (elseBlock ? elseBlock : mergeBlock);
//         this->Builder->CreateCondBr(condition, thenBlocks[i], nextBlock);

//         // Emit code for the 'then' block
//         this->Builder->SetInsertPoint(thenBlocks[i]);
//         if (!this->codegen(*ifAst.then[i])) {
//             return nullptr;
//         }
//         this->Builder->CreateBr(mergeBlock);

//         // Update current block
//         currentBlock = condBlocks[i];
//     }

//     // Emit the 'else' block if present
//     if (elseBlock) {
//         this->Builder->SetInsertPoint(elseBlock);
//         if (!this->codegen(*ifAst.then.back())) {
//             return nullptr;
//         }
//         this->Builder->CreateBr(mergeBlock);
//     }

//     // Set insert point to the merge block for further code
//     this->Builder->SetInsertPoint(mergeBlock);

//     // Validate the generated code, checking for consistency.
//     llvm::verifyFunction(*function);
//     return function;

//     return nullptr; // Return void for now (extend if you want an expression
//                     // result)
// }

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