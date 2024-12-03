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
    if (prs->ParseDefinition()) {
        fprintf(stderr, "Parsed a function definition.\n");
    } else {
        // Skip token for error recovery.
        prs->getNextToken();
    }
}

void MainLoop::HandleTopLevelExpression() {
    // Evaluate a top-level expression into an anonymous function.
    if (prs->ParseTopLevelExpr()) {
        fprintf(stderr, "Parsed a top-level expr\n");
    } else {
        // Skip token for error recovery.
        prs->getNextToken();
    }
}
