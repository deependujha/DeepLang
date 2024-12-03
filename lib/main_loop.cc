#include "main_loop.h"
#include "parser/parser.h"

void MainLoop::run() {
    fprintf(stderr, "deeplang> ");
    this->prs->getNextToken();
    while (true) {
        fprintf(stderr, "deeplang> ");
        switch (this->prs->bt->getTokenEnum()) {
            // case tok_eof:
            //     return;
            case lexer::SEMICOLON: // ignore top-level semicolons.
                prs->getNextToken();
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
