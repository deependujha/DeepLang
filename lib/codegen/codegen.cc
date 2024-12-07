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

    add_new_namedValue();
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
        std::cout << "binary expr\n";
        e = BinaryExpr;
    } else if (typeid(exprAst) == typeid(ast::NumberExprAST)) {
        e = NumberExpr;
        std::cout << "num expr\n";
    } else if (typeid(exprAst) == typeid(ast::VariableExprAST)) {
        e = VariableExpr;
        std::cout << "vari expr\n";
    } else if (typeid(exprAst) == typeid(ast::CallExprAST)) {
        e = CallExpr;
        std::cout << "call expr\n";
    } else if (typeid(exprAst) == typeid(ast::IfExprAST)) {
        e = IfExpr;
        std::cout << "if expr\n";
    } else if (typeid(exprAst) == typeid(ast::LoopExprAST)) {
        e = LoopExpr;
        std::cout << "loop expr\n";
    } else if (typeid(exprAst) == typeid(ast::VarExprAST)) {
        e = VarExpr;
        std::cout << "VarExpr expr\n";
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
    // print all the keys of namedValues
    llvm::AllocaInst* meow = nullptr;
    for (auto& e : this->NamedValues.back()) {
        std::cout << "namedvalue: " << e.first << " " << e.second << "\n";
    }
    std::string myname = varAst.Name;
    std::cout << "variable expression codegen" << myname << "\n";

    if (this->NamedValues.back().find(myname) !=
        this->NamedValues.back().end()) {
        meow = this->NamedValues.back()[myname];
    } else {
        std::cerr << "Variable " << myname << " not found in NamedValues.\n";
        return nullptr; // Or handle the error appropriately
    }
    if (!meow) {
        std::string errMsg = "Unknown variable name: " + varAst.Name;
        // print all the keys of namedValues
        for (auto& e : this->NamedValues.back()) {
            std::cout << "namedvalue: " << e.first << " " << e.second << "\n";
        }
        return LogErrorV(errMsg.c_str());
    }
    // Load the value.
    return Builder->CreateLoad(
        meow->getAllocatedType(), meow, varAst.Name.c_str());
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

llvm::Value* CodeGen::codegen(const ast::CallExprAST& calAst) {
    // Look up the name in the global module table.
    llvm::Function* CalleeF = TheModule->getFunction(calAst.Callee);
    if (!CalleeF) {
        return LogErrorV("Unknown function referenced");
    }

    // If argument mismatch error.
    if (CalleeF->arg_size() != calAst.Args.size()) {
        return this->LogErrorV("Incorrect # arguments passed");
    }

    std::vector<llvm::Value*> ArgsV;
    for (unsigned i = 0, e = calAst.Args.size(); i != e; ++i) {
        ArgsV.push_back(this->codegen(*calAst.Args[i]));
        if (!ArgsV.back()) {
            return nullptr;
        }
    }

    return this->Builder->CreateCall(CalleeF, ArgsV, "calltmp");
}
llvm::Function* CodeGen::codegen(const ast::PrototypeAST& protoAst) {
    // Make the function type:  double(double,double) etc.
    std::vector<llvm::Type*> Doubles(
        protoAst.Args.size(), llvm::Type::getDoubleTy(*TheContext));
    llvm::FunctionType* FT = llvm::FunctionType::get(
        llvm::Type::getDoubleTy(*TheContext), Doubles, false);

    llvm::Function* F = llvm::Function::Create(
        FT,
        llvm::Function::ExternalLinkage,
        protoAst.Name,
        this->TheModule.get());

    // Set names for all arguments.
    unsigned Idx = 0;
    for (auto& Arg : F->args()) {
        Arg.setName(protoAst.Args[Idx++]);
    }

    return F;
}
llvm::Function* CodeGen::codegen(const ast::FunctionAST& fnAst) {
    // First, check for an existing function from a previous 'extern'
    // declaration.
    llvm::Function* TheFunction =
        this->TheModule->getFunction(fnAst.Proto->getName());

    if (!TheFunction) {
        TheFunction = this->codegen(*fnAst.Proto);
    }

    if (!TheFunction) {
        return nullptr;
    }

    // Create a new basic block to start insertion into.
    llvm::BasicBlock* BB =
        llvm::BasicBlock::Create(*this->TheContext, "entry", TheFunction);
    this->Builder->SetInsertPoint(BB);

    // Record the function arguments in the NamedValues map.
    this->add_new_namedValue();
    std::cout << "in function codegen\n";
    for (auto& Arg : TheFunction->args()) {
        // Create an alloca for this variable.
        llvm::AllocaInst* Alloca = this->CreateEntryBlockAlloca(
            TheFunction, std::string(Arg.getName()));

        // Store the initial value into the alloca.
        Builder->CreateStore(&Arg, Alloca);

        // Add arguments to variable symbol table.
        this->NamedValues.back()[std::string(Arg.getName())] = Alloca;
    }

    // Generate code for each body expression.
    for (size_t i = 0; i < fnAst.Body.size(); ++i) {
        llvm::Value* BodyVal = this->codegen(*fnAst.Body[i]);

        // Optionally, handle side effects here. For now, we just ensure the
        // value is generated.
        if (!BodyVal) {
            std::cout << "error generating code for body of the function\n";
            return nullptr; // Error generating code for body expression.
        }
    }

    // Generate code for the return expression.
    llvm::Value* RetVal = nullptr;
    if (fnAst.Ret) {
        RetVal = this->codegen(*fnAst.Ret);
    } else {
        RetVal = llvm::Constant::getNullValue(llvm::Type::getInt32Ty(
            *this->TheContext)); // Default return for void.
    }

    if (!RetVal) {
        return nullptr; // Error generating return value.
    }

    // if (llvm::Value* RetVal = this->codegen(*fnAst.Body)) {
    //     // Finish off the function.
    //     this->Builder->CreateRet(RetVal);

    //     // Validate the generated code, checking for consistency.
    //     llvm::verifyFunction(*TheFunction);

    //     return TheFunction;
    // }

    Builder->CreateRet(RetVal);

    // Validate the generated code, checking for consistency.
    llvm::verifyFunction(*TheFunction);

    // Error reading body, remove function.
    // TheFunction->eraseFromParent();
    return nullptr;
}

} // namespace codegen