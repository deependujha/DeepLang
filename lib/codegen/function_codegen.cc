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