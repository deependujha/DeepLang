#include "main_loop.h"
#include <exception>
#include <iostream>
#include "parser/parser.h"

void MainLoop::run() {
    while (true) {
        std::cout << "\n===========================================\n";
        std::cout << "deeplang> ";
        this->prs->getNextToken();
        switch (this->prs->bt->getTokenEnum()) {
            case (lexer::OPERATOR): // ignore top-level semicolons.
                if (this->prs->bt->getValue() != ";") {
                    std::cout << "expected semicolon\n";
                }
                break;
            case lexer::FN:
                this->HandleDefinition();
                break;
            default:
                this->HandleTopLevelExpression();
                break;
        }
    }
}

void MainLoop::HandleDefinition() {
    if (auto FnAST = prs->ParseDefinition()) {
        this->cg->printIR(*FnAST);
    } else {
        // Skip token for error recovery.
        prs->getNextToken();
    }
}

void MainLoop::HandleTopLevelExpression() {
    // Evaluate a top-level expression into an anonymous function.
    if (auto FnAST = prs->ParseTopLevelExpr()) {
        this->cg->printIR(*FnAST, true);
    } else {
        // Skip token for error recovery.
        prs->getNextToken();
    }
}
