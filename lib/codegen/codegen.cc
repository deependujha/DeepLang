#include "codegen/codegen.h"
#include <iostream>
#include <string>
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
void CodeGen::initializeModule() {
    // Open a new context and module.
    this->TheContext = std::make_unique<llvm::LLVMContext>();
    this->TheModule = std::make_unique<llvm::Module>("deeplang", *TheContext);

    // Create a new builder for the module.
    this->Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);

    this->add_new_namedValue();
}

llvm::Value* CodeGen::LogErrorV(const char* Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}

llvm::Value* CodeGen::codegen(const ast::ExprAST& exprAst) {
    enum ExprAstType : int8_t {
        NumberExpr,
        VariableExpr,
        BinaryExpr,
        CallExpr,
        IfExpr,
        LoopExpr,
        VarExpr,
        Invalid,
    };

    ExprAstType e = Invalid;
    if (typeid(exprAst) == typeid(ast::BinaryExprAST)) {
        e = BinaryExpr;
    } else if (typeid(exprAst) == typeid(ast::NumberExprAST)) {
        e = NumberExpr;
    } else if (typeid(exprAst) == typeid(ast::VariableExprAST)) {
        e = VariableExpr;
    } else if (typeid(exprAst) == typeid(ast::CallExprAST)) {
        e = CallExpr;
    } else if (typeid(exprAst) == typeid(ast::IfExprAST)) {
        e = IfExpr;
    } else if (typeid(exprAst) == typeid(ast::LoopExprAST)) {
        e = LoopExpr;
    } else if (typeid(exprAst) == typeid(ast::VarExprAST)) {
        e = VarExpr;
    }

    switch (e) {
        default:
            return nullptr;
        case NumberExpr: {
            // Dynamic cast to derived class
            const auto* numExpr =
                dynamic_cast<const ast::NumberExprAST*>(&exprAst);
            if (numExpr) {
                return codegen(*numExpr);
            }
            break;
        }
        case BinaryExpr: {
            const auto* binExpr =
                dynamic_cast<const ast::BinaryExprAST*>(&exprAst);
            if (binExpr) {
                return codegen(*binExpr);
            }
            break;
        }
        case VariableExpr: {
            const auto* varExpr =
                dynamic_cast<const ast::VariableExprAST*>(&exprAst);
            if (varExpr) {
                return codegen(*varExpr);
            }
            break;
        }
        case CallExpr: {
            const auto* callExpr =
                dynamic_cast<const ast::CallExprAST*>(&exprAst);
            if (callExpr) {
                return codegen(*callExpr);
            }
            break;
        }
        case IfExpr: {
            const auto* conditionalExpr =
                dynamic_cast<const ast::IfExprAST*>(&exprAst);
            if (conditionalExpr) {
                return codegen(*conditionalExpr);
            }
            break;
        }
        case LoopExpr: {
            const auto* loopExpr =
                dynamic_cast<const ast::LoopExprAST*>(&exprAst);
            if (loopExpr) {
                return codegen(*loopExpr);
            }
            break;
        }
        case VarExpr: {
            const auto* varExpr =
                dynamic_cast<const ast::VarExprAST*>(&exprAst);
            if (varExpr) {
                return codegen(*varExpr);
            }
            break;
        }
    }

    return nullptr;
}

void CodeGen::printIR(const ast::FunctionAST& expAst, bool anonymous) {
    if (llvm::Function* FnIR = this->codegen(expAst)) {
        if (anonymous) {
            fprintf(stderr, "Read top-level expression:\n");
        } else {
            fprintf(stderr, "Read function definition:\n");
        }
        FnIR->print(llvm::errs());
        fprintf(stderr, "\n");

        if (anonymous) { // Remove the anonymous expression.
            FnIR->eraseFromParent();
        }
    }
}

llvm::Value* CodeGen::codegen(const ast::NumberExprAST& numAst) {
    return llvm::ConstantFP::get(*this->TheContext, llvm::APFloat(numAst.Val));
}

llvm::Value* CodeGen::codegen(const ast::VariableExprAST& varAst) {
    llvm::AllocaInst* ptrInst = nullptr;

    for (auto it = this->NamedValues.rbegin(); it != this->NamedValues.rend();
         ++it) {
        auto& nv = *it;
        // std::cout << "------ new namedvalue -------\n";
        // for (auto& e : nv) {
        //     std::cout << "namedvalue: " << e.first << " " << e.second <<
        //     "\n";
        // }
        if (nv.find(varAst.Name) != nv.end()) {
            ptrInst = nv[varAst.Name];
            break;
        }
    }

    if (!ptrInst) {
        std::string errMsg = "Unknown variable name: " + varAst.Name;
        return LogErrorV(errMsg.c_str());
    }
    // Load the value.
    return this->Builder->CreateLoad(
        ptrInst->getAllocatedType(), ptrInst, varAst.Name.c_str());
}

llvm::Value* CodeGen::codegen(const ast::BinaryExprAST& binAst) {
    llvm::Value* L = nullptr;
    if (binAst.Op != "=") {
        L = this->codegen(*binAst.LHS);
    }
    llvm::Value* R = this->codegen(*binAst.RHS);
    if (!L || !R) {
        return nullptr;
    }
    if (binAst.Op == "=") {
        // This assume we're building without RTTI because LLVM builds that way
        // by
        // default. If you build LLVM with RTTI this can be changed to a
        // dynamic_cast for automatic error checking.
        ast::VariableExprAST* LHSE =
            static_cast<ast::VariableExprAST*>(binAst.LHS.get());
        if (!LHSE) {
            return LogErrorV("destination of '=' must be a variable");
        }
        // Look up the name.
        llvm::Value* Variable = this->NamedValues.back()[LHSE->Name];
        if (!Variable) {
            return LogErrorV("Unknown variable name");
        }

        Builder->CreateStore(R, Variable);
        return R;
    } else if (binAst.Op == "+") {
        return Builder->CreateFAdd(L, R, "addtmp");
    } else if (binAst.Op == "-") {
        return Builder->CreateFSub(L, R, "subtmp");
    } else if (binAst.Op == "*") {
        return Builder->CreateFMul(L, R, "multmp");
    } else if (binAst.Op == "/") {
        return Builder->CreateFDiv(L, R, "divtmp");
    } else if (binAst.Op == "%") {
        return Builder->CreateFRem(L, R, "remtmp");
    } else if (binAst.Op == "<") {
        L = Builder->CreateFCmpULT(L, R, "cmptmp");
        // Convert bool 0/1 to double 0.0 or 1.0
        return Builder->CreateUIToFP(
            L, llvm::Type::getDoubleTy(*this->TheContext), "booltmp");
    } else if (binAst.Op == ">") {
        L = Builder->CreateFCmpUGT(L, R, "cmptmp");
        // Convert bool 0/1 to double 0.0 or 1.0
        return Builder->CreateUIToFP(
            L, llvm::Type::getDoubleTy(*this->TheContext), "booltmp");
    } else {
        return LogErrorV("invalid binary operator");
    }
}

} // namespace codegen