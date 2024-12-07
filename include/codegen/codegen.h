#pragma once
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "ast/ast.h"
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
class CodeGen {
  private:
    std::unique_ptr<llvm::LLVMContext> TheContext;
    std::unique_ptr<llvm::Module> TheModule;
    std::unique_ptr<llvm::IRBuilder<>> Builder;
    std::map<std::string, llvm::AllocaInst*> mp;
    std::vector<std::map<std::string, llvm::AllocaInst*>> NamedValues;

    void initializeModule();

  public:
    CodeGen() {
        this->initializeModule();
    }
    virtual ~CodeGen() = default;
    llvm::Value* LogErrorV(const char* Str);
    void add_new_namedValue() {
        this->NamedValues.emplace_back(mp); // global variable
    }

    virtual llvm::Value* codegen(const ast::ExprAST& exprAst);
    llvm::Value* codegen(const ast::NumberExprAST& numAst);
    llvm::Value* codegen(const ast::VariableExprAST& varAst);
    llvm::Value* codegen(const ast::BinaryExprAST& binAst);
    llvm::Value* codegen(const ast::CallExprAST& calAst);
    llvm::Function* codegen(const ast::PrototypeAST& protoAst);
    llvm::Function* codegen(const ast::FunctionAST& fnAst);
    llvm::Value* codegen(const ast::IfExprAST& fnAst);
    llvm::Value* codegen(const ast::LoopExprAST& fnAst);
    llvm::Value* codegen(const ast::VarExprAST& varAst);

    void printIR(const ast::FunctionAST& expAst, bool anonymous = false);

    /// CreateEntryBlockAlloca - Create an alloca instruction in the entry block
    /// of
    /// the function.  This is used for mutable variables etc.
    llvm::AllocaInst* CreateEntryBlockAlloca(
        llvm::Function* TheFunction,
        const std::string& VarName) {
        llvm::IRBuilder<> TmpB(
            &TheFunction->getEntryBlock(),
            TheFunction->getEntryBlock().begin());
        return TmpB.CreateAlloca(
            llvm::Type::getDoubleTy(*TheContext), nullptr, VarName);
    }
};
} // namespace codegen