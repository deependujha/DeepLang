#include <iostream>
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
llvm::Value* CodeGen::codegen(const ast::VarExprAST& varAst) {
    llvm::Value* InitVal = nullptr;
    std::unique_ptr<llvm::AllocaInst> Alloca = nullptr;

    llvm::Function* TheFunction = this->Builder->GetInsertBlock()->getParent();

    // Register all variables and emit their initializer.
    for (unsigned i = 0, e = varAst.VarNames.size(); i != e; ++i) {
        const std::string& VarName = varAst.VarNames[i].first;
        ast::ExprAST* Init = varAst.VarNames[i].second.get();

        // Emit the initializer before adding the variable to scope, this
        // prevents the initializer from referencing the variable itself, and
        // permits stuff like this:
        //  var a = 1 in
        //    var a = a in ...   # refers to outer 'a'.
        InitVal = this->codegen(*Init);
        if (!InitVal) {
            return LogErrorV("could not initialize the variable expression");
        }

        Alloca = this->CreateEntryBlockAlloca(TheFunction, VarName);
        this->Builder->CreateStore(InitVal, Alloca.get());

        // Remember this binding.
        this->NamedValues.back()[VarName] = std::move(Alloca);
    }

    // Return the nullptr
    return InitVal;
}
} // namespace codegen