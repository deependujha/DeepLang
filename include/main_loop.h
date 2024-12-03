#pragma once
#include "codegen/codegen.h"
#include "lexer/token.h"
#include "parser/parser.h"

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
