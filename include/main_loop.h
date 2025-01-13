#pragma once
#include "codegen/codegen.h"
#include "lexer/token.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
#include "parser/parser.h"

/**
 * @brief Brief description of the function.
 *
 * @param param1 Description of the first parameter.
 * @param param2 Description of the second parameter.
 * @return Description of the return value.
 */
class MainLoop {
  private:
    parser::Parser* prs;
    codegen::CodeGen* cg;
    void HandleDefinition();
    void HandleTopLevelExpression();

  public:
    MainLoop() : cg(new codegen::CodeGen()) {
        // this->cg->InitializeModuleAndManagers();

        lexer::TokenOperatorUtils* tou = new lexer::TokenOperatorUtils();
        this->prs = new parser::Parser(tou);
    }
    void run();
};
