#pragma once
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "ast/ast.h"
#include "codegen/KaleidoscopeJIT.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"

namespace codegen {
class CodeGen {
  private:
    std::unique_ptr<llvm::LLVMContext> TheContext;
    std::unique_ptr<llvm::Module> TheModule;
    std::unique_ptr<llvm::IRBuilder<>> Builder;
    std::vector<std::map<std::string, std::shared_ptr<llvm::AllocaInst>>>
        NamedValues;

    std::unique_ptr<llvm::FunctionPassManager> TheFPM;
    std::unique_ptr<llvm::LoopAnalysisManager> TheLAM;
    std::unique_ptr<llvm::FunctionAnalysisManager> TheFAM;
    std::unique_ptr<llvm::CGSCCAnalysisManager> TheCGAM;
    std::unique_ptr<llvm::ModuleAnalysisManager> TheMAM;
    std::unique_ptr<llvm::PassInstrumentationCallbacks> ThePIC;
    std::unique_ptr<llvm::StandardInstrumentations> TheSI;
    std::map<std::string, std::unique_ptr<ast::PrototypeAST>> FunctionProtos;
    llvm::ExitOnError ExitOnErr;

  public:
    std::unique_ptr<llvm::orc::KaleidoscopeJIT> TheJIT;
    void InitializeModuleAndManagers();

    CodeGen() {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        llvm::InitializeNativeTargetAsmParser();
        this->NamedValues.emplace_back();
        this->TheJIT = ExitOnErr(llvm::orc::KaleidoscopeJIT::Create());
        this->InitializeModuleAndManagers();
    }
    virtual ~CodeGen() = default;
    llvm::Value* LogErrorV(const char* Str);

    virtual llvm::Value* codegen(const ast::ExprAST& exprAst);
    llvm::Value* codegen(const ast::NumberExprAST& numAst);
    llvm::Value* codegen(const ast::VariableNameExprAST& varAst);
    llvm::Value* codegen(const ast::BinaryExprAST& binAst);
    llvm::Value* codegen(const ast::CallExprAST& calAst);
    llvm::Function* codegen(const ast::PrototypeAST& protoAst);
    llvm::Function* codegen(const ast::FunctionAST& fnAst);
    llvm::Value* codegen(const ast::IfExprAST& fnAst);
    llvm::Value* codegen(const ast::LoopExprAST& fnAst);
    llvm::Value* codegen(const ast::VarDeclExprAST& varAst);

    void printIR(const ast::FunctionAST& expAst, bool anonymous = false);

    /// CreateEntryBlockAlloca - Create an alloca instruction in the entry block
    /// of
    /// the function.  This is used for mutable variables etc.
    std::unique_ptr<llvm::AllocaInst> CreateEntryBlockAlloca(
        llvm::Function* TheFunction,
        const std::string& VarName) {
        llvm::IRBuilder<> TmpB(
            &TheFunction->getEntryBlock(),
            TheFunction->getEntryBlock().begin());

        auto allocaInst = TmpB.CreateAlloca(
            llvm::Type::getDoubleTy(*this->TheContext), nullptr, VarName);
        // Return the unique_ptr managing the AllocaInst.
        return std::unique_ptr<llvm::AllocaInst>(allocaInst);
    }

    void executeTopLevelExpression() {
        std::cout
            << "===== going to execute top level expression with JIT (JET)\n";
        // Create a ResourceTracker to track JIT'd memory allocated to our
        // anonymous expression -- that way we can free it after executing.
        auto RT = TheJIT->getMainJITDylib().createResourceTracker();

        auto TSM = llvm::orc::ThreadSafeModule(
            std::move(TheModule), std::move(TheContext));
        ExitOnErr(TheJIT->addModule(std::move(TSM), RT));
        // InitializeModuleAndManagers();

        // Search the JIT for the __anon_expr symbol.
        auto ExprSymbol = ExitOnErr(TheJIT->lookup("__anon_expr"));

        // Get the symbol's address and cast it to the right type (takes no
        // arguments, returns a double) so we can call it as a native function.
        double (*FP)() = ExprSymbol.getAddress().toPtr<double (*)()>();
        fprintf(stderr, "Evaluated to %f\n", FP());

        // Delete the anonymous expression module from the JIT.
        ExitOnErr(RT->remove());
    }
};
} // namespace codegen