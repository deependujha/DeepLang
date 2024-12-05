#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include "ast/ast.h"
#include "lexer/token.h"
#include "parser/parser.h"

namespace parser {

std::unique_ptr<ast::ExprAST> Parser::ParseIfExpr() {
    typedef std::unique_ptr<ast::ExprAST> exprType;
    std::vector<exprType> cond;
    std::vector<exprType> then;

    this->getNextToken(); // eat the if.

    while (true) {
        // condition.
        auto Cond = ParseExpression();
        if (!Cond) {
            return nullptr;
        }
        cond.push_back(std::move(Cond));

        if (!(this->bt->getTokenEnum() == lexer::OPERATOR &&
              this->bt->getValue() == "{")) {
            return LogError("expected '{'");
        }
        this->getNextToken(); // eat the {

        auto Then = this->ParseExpression();
        if (!Then) {
            return nullptr;
        }
        then.push_back(std::move(Then));

        if (!(this->bt->getTokenEnum() == lexer::OPERATOR &&
              this->bt->getValue() == ";")) {
            this->bt->printLn();
            return LogError("expected ';'");
        }
        this->getNextToken();
        if (!(this->bt->getTokenEnum() == lexer::OPERATOR &&
              this->bt->getValue() == "}")) {
            this->bt->printLn();
            return LogError("expected '}'");
        }
        this->getNextToken();

        if (this->bt->getTokenEnum() != lexer::ELIF) {
            break;
        }

        this->getNextToken(); // eat elif
    }

    if (this->bt->getTokenEnum() != lexer::ELSE) {
        return std::make_unique<ast::IfExprAST>(
            std::move(cond), std::move(then));
    }

    this->getNextToken();
    if (!(this->bt->getTokenEnum() == lexer::OPERATOR &&
          this->bt->getValue() == "{")) {
        return LogError("expected '{'");
    }
    this->getNextToken();
    auto Else = this->ParseExpression();
    if (!Else) {
        return nullptr;
    }
    then.push_back(std::move(Else)); // last else block goes in then expr
    if (!(this->bt->getTokenEnum() == lexer::OPERATOR &&
          this->bt->getValue() == ";")) {
        this->bt->printLn();
        return LogError("expected ';'");
    }
    this->getNextToken();
    if (!(this->bt->getTokenEnum() == lexer::OPERATOR &&
          this->bt->getValue() == "}")) {
        this->bt->printLn();
        return LogError("expected '}'");
    }
    return std::make_unique<ast::IfExprAST>(std::move(cond), std::move(then));
}

} // namespace parser