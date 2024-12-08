#pragma once
#include "codegen/codegen.h"
#include "lexer/token.h"
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
    codegen::CodeGen* cg = new codegen::CodeGen();
    void HandleDefinition();
    void HandleTopLevelExpression();

  public:
    MainLoop() {
        lexer::TokenOperatorUtils* tou = new lexer::TokenOperatorUtils();
        this->prs = new parser::Parser(tou);
    }
    void run();
};
