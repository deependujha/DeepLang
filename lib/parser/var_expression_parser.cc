#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include "ast/ast.h"
#include "lexer/token.h"
#include "parser/parser.h"

namespace parser {
std::unique_ptr<ast::ExprAST> Parser::ParseVarExpr() {
    this->getNextToken(); // eat the var(let).

    std::vector<std::pair<std::string, std::unique_ptr<ast::ExprAST>>> VarNames;

    // At least one variable name is required.
    if (lexer::VARIABLE != this->bt->getTokenEnum()) {
        return LogError("expected identifier after var");
    }

    while (true) {
        std::string Name = this->bt->getValue();
        this->getNextToken(); // eat identifier.

        // Read the optional initializer.
        std::unique_ptr<ast::ExprAST> Init;
        if (this->bt->getValue() == "=") {
            this->getNextToken(); // eat the '='.

            Init = this->ParseExpression();
            if (!Init) {
                return nullptr;
            }
        }

        VarNames.emplace_back(Name, std::move(Init));

        // End of var list, exit loop.
        if (this->bt->getValue() != ",") {
            break;
        }
        this->getNextToken(); // eat the ','.

        if (lexer::VARIABLE != this->bt->getTokenEnum()) {
            return LogError("expected identifier list after var");
        }
    }

    // return std::make_unique<ast::VarDeclExprAST>(std::move(VarNames));
    return nullptr;
}
} // namespace parser