#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include "ast/ast.h"
#include "lexer/token.h"
#include "parser/parser.h"

namespace parser {
/// struct definition parser
/// syntax:
///     struct MyStr{float var1, string var2, bool var3};
///
std::unique_ptr<ast::ExprAST> Parser::ParseStructExpr() {
    // check if currToken is struct
    if (this->bt->getTokenEnum() != lexer::STRUCT) {
        return LogError("expected struct");
    }
    this->getNextToken(); // eat struct

    // check if currToken is identifier
    if (this->bt->getTokenEnum() != lexer::VARIABLE) {
        return LogError("expected identifier after struct");
    }
    std::string structName;
    std::vector<std::string> fieldTypes;
    std::vector<std::string> fieldNames;

    structName = this->bt->getValue();
    this->getNextToken(); // eat identifier

    // check if currToken is {
    if (this->bt->getValue() != "{") {
        return LogError("expected '{'");
    }
    this->getNextToken(); // eat {

    while (true) {
        if (this->bt->getTokenEnum() != lexer::VARIABLE) {
            return LogError("expected identifier");
        }
        if (this->bt->getValue() != "string" ||
            this->bt->getValue() != "float" || this->bt->getValue() != "bool") {
            return LogError(
                "Only string, float, bool are allowed as field types");
        }
        fieldTypes.push_back(this->bt->getValue());
        this->getNextToken(); // eat identifier

        // check if currToken is variable
        if (this->bt->getTokenEnum() != lexer::VARIABLE) {
            return LogError("expected variablename");
        }
        fieldNames.push_back(this->bt->getValue());

        this->getNextToken();

        // check if currToken is ,
        if (this->bt->getValue() == ",") {
            this->getNextToken();
            continue;
        }
        if (this->bt->getValue() == "}") {
            this->getNextToken(); // eat }
            return std::make_unique<ast::StructExprAST>(ast::StructExprAST(
                std::move(structName),
                std::move(fieldNames),
                std::move(fieldTypes)));
        }
    }
    return std::make_unique<ast::StructExprAST>(ast::StructExprAST(
        std::move(structName), std::move(fieldNames), std::move(fieldTypes)));
}

/// struct method parser
/// syntax:
///     method StructName: fn methodName(dtype arg1, dtype arg2) -> dtype {
///         ... statements ...
///         return expr;
///     }
std::unique_ptr<ast::ExprAST> Parser::ParseStructMethodExpr() {
    if (this->bt->getTokenEnum() != lexer::METHOD) {
        return LogError("expected method");
    }
    this->getNextToken(); // eat method

    if (this->bt->getTokenEnum() != lexer::VARIABLE) {
        return LogError("expected struct name after method");
    }

    std::string structName = this->bt->getValue();

    this->getNextToken(); // eat struct name

    if (this->bt->getValue() != ":") {
        return LogError("expected ':' after struct name");
    }
    this->getNextToken(); // eat ':'

    if (this->bt->getTokenEnum() != lexer::FN) {
        return LogError("expected fn after struct name");
    }
    std::unique_ptr<ast::FunctionAST> method = this->ParseDefinition();
    return std::make_unique<ast::StructMethodExprAST>(
        ast::StructMethodExprAST(std::move(structName), std::move(method)));
}
} // namespace parser