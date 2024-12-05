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

llvm::Function* CodeGen::codegen(const ast::IfExprAST& fnAst) {
    llvm::Function* function = this->Builder->GetInsertBlock()->getParent();

    // Create a merge block for after the if/elif/else ladder
    llvm::BasicBlock* mergeBlock =
        llvm::BasicBlock::Create(*this->TheContext, "merge", function);

    // Track the block where we are currently emitting
    llvm::BasicBlock* currentBlock = this->Builder->GetInsertBlock();

    // Create blocks for conditions and corresponding 'then' bodies
    std::vector<llvm::BasicBlock*> condBlocks;
    std::vector<llvm::BasicBlock*> thenBlocks;

    for (size_t i = 0; i < fnAst.cond.size(); ++i) {
        condBlocks.push_back(
            llvm::BasicBlock::Create(*this->TheContext, "cond", function));
        thenBlocks.push_back(
            llvm::BasicBlock::Create(*this->TheContext, "then", function));
    }

    // Create the 'else' block if present
    llvm::BasicBlock* elseBlock = nullptr;
    if (fnAst.cond.size() < fnAst.then.size()) {
        elseBlock =
            llvm::BasicBlock::Create(*this->TheContext, "else", function);
    }

    // Generate code for each condition
    for (size_t i = 0; i < fnAst.cond.size(); ++i) {
        // Emit code for condition
        this->Builder->SetInsertPoint(currentBlock);
        llvm::Value* condition = this->codegen(*fnAst.cond[i]);
        if (!condition) {
            return nullptr;
        }

        // Create a conditional branch to 'then' or the next condition/else
        llvm::BasicBlock* nextBlock = (i + 1 < fnAst.cond.size())
            ? condBlocks[i + 1]
            : (elseBlock ? elseBlock : mergeBlock);
        this->Builder->CreateCondBr(condition, thenBlocks[i], nextBlock);

        // Emit code for the 'then' block
        this->Builder->SetInsertPoint(thenBlocks[i]);
        if (!this->codegen(*fnAst.then[i])) {
            return nullptr;
        }
        this->Builder->CreateBr(mergeBlock);

        // Update current block
        currentBlock = condBlocks[i];
    }

    // Emit the 'else' block if present
    if (elseBlock) {
        this->Builder->SetInsertPoint(elseBlock);
        if (!this->codegen(*fnAst.then.back())) {
            return nullptr;
        }
        this->Builder->CreateBr(mergeBlock);
    }

    // Set insert point to the merge block for further code
    this->Builder->SetInsertPoint(mergeBlock);

    // Validate the generated code, checking for consistency.
    llvm::verifyFunction(*function);
    return function;

    return nullptr; // Return void for now (extend if you want an expression
                    // result)
}

} // namespace codegen