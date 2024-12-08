#include "lexer/token.h"
#include "parser/parser.h"

namespace parser {
/// parses float, string, bool
std::unique_ptr<ast::ExprAST> Parser::ParseValueExpr() {
    std::unique_ptr<ast::ExprAST> parsedAst = nullptr;

    switch (this->bt->getTokenEnum()) {
        case lexer::FLOAT: {
            parsedAst =
                std::make_unique<ast::NumberExprAST>(this->bt->getValueFloat());
            break;
        }
        case lexer::STRING: {
            parsedAst =
                std::make_unique<ast::StringExprAST>(this->bt->getValue());
            break;
        }
        case lexer::BOOLEAN: {
            parsedAst = std::make_unique<ast::BooleanExprAST>(
                this->bt->getValue() == "true");
            break;
        }
        default:
            throw std::runtime_error("unknown token when expecting a value");
            break;
    }

    this->getNextToken(); // consume the number

    return std::move(parsedAst);
}
} // namespace parser