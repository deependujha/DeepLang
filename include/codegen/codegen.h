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
    std::map<std::string, llvm::Value*> NamedValues;

    void initializeModule();

  public:
    CodeGen() {
        this->initializeModule();
    }
    virtual ~CodeGen() = default;
    llvm::Value* LogErrorV(const char* Str);

    virtual llvm::Value* codegen(const ast::ExprAST& exprAst);
    llvm::Value* codegen(const ast::NumberExprAST& numAst);
    llvm::Value* codegen(const ast::VariableExprAST& varAst);
    llvm::Value* codegen(const ast::BinaryExprAST& binAst);
    llvm::Value* codegen(const ast::CallExprAST& calAst);
    llvm::Function* codegen(const ast::PrototypeAST& protoAst);
    llvm::Function* codegen(const ast::FunctionAST& fnAst);
    llvm::Value* codegen(const ast::IfExprAST& fnAst);
    llvm::Value* codegen(const ast::LoopExprAST& fnAst);

    void printIR(const ast::FunctionAST& expAst, bool anonymous = false);
};
} // namespace codegen