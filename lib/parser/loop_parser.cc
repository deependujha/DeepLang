#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include "ast/ast.h"
#include "lexer/token.h"
#include "parser/parser.h"

namespace parser {
std::unique_ptr<ast::ExprAST> Parser::ParseLoopExpr() {
    this->getNextToken(); // eat the for.

    if (this->bt->getValue() != "(") {
        return LogError("expected '('");
    }
    this->getNextToken();

    if (this->bt->getTokenEnum() != lexer::VARIABLE) {
        return LogError("expected 'identifier' after `for(`");
    }

    std::string IdName = this->bt->getValue();
    this->getNextToken(); // eat identifier.

    if (this->bt->getValue() != "=") {
        return LogError("parsing loop: expected '='");
    }
    this->getNextToken(); // eat '='.

    auto Start = this->ParseExpression();
    if (!Start) {
        return nullptr;
    }
    if (this->bt->getValue() != ";") {
        return LogError("parsing loop: expected ';' after for start value");
    }

    this->getNextToken();

    auto End = this->ParseExpression();
    if (!End) {
        return nullptr;
    }

    // The step value is optional.
    std::unique_ptr<ast::ExprAST> Step;
    if (this->bt->getValue() == ";") {
        this->getNextToken();
        Step = this->ParseExpression();
        if (!Step) {
            return nullptr;
        }
    }

    if (this->bt->getValue() != "{") {
        return LogError("parsing loop: expected '{' for body of loop");
    }
    this->getNextToken(); // eat '{'.

    auto Body = this->ParseExpression();
    if (!Body) {
        return nullptr;
    }

    return std::make_unique<ast::ForExprAST>(
        IdName,
        std::move(Start),
        std::move(End),
        std::move(Step),
        std::move(Body));
}
} // namespace parser