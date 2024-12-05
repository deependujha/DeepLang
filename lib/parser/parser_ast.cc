#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include "ast/ast.h"
#include "lexer/token.h"
#include "parser/parser.h"

namespace parser {
/// LogError* - These are little helper functions for error handling.
std::unique_ptr<ast::ExprAST> Parser::LogError(const char* Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}
std::unique_ptr<ast::PrototypeAST> Parser::LogErrorP(const char* Str) {
    LogError(Str);
    return nullptr;
}

std::unique_ptr<ast::ExprAST> Parser::ParseExpression() {
    auto LHS = this->ParsePrimary();
    if (!LHS) {
        return nullptr;
    }

    return this->ParseBinOpRHS(0, std::move(LHS));
}

/// numberexpr ::= number
std::unique_ptr<ast::ExprAST> Parser::ParseNumberExpr() {
    auto Result =
        std::make_unique<ast::NumberExprAST>(this->bt->getValueFloat());
    this->getNextToken(); // consume the number
    return std::move(Result);
}
/// parenexpr ::= "(" expression ")"
std::unique_ptr<ast::ExprAST> Parser::ParseParenExpr() {
    this->getNextToken(); // eat (.
    auto V = this->ParseExpression();
    if (!V) {
        return nullptr;
    }

    if (this->bt->getTokenEnum() != lexer::OPERATOR ||
        this->bt->getValue() != ")") {
        return LogError("expected ')'");
    }
    this->getNextToken(); // eat ).
    return V;
}

/// identifierexpr
///   ::= identifier
///   ::= identifier '(' expression* ')'
std::unique_ptr<ast::ExprAST> Parser::ParseIdentifierExpr() {
    std::string IdName = this->bt->getValue();

    this->getNextToken(); // eat identifier.

    if (this->bt->getValue() != "(") { // Simple variable ref.
        return std::make_unique<ast::VariableExprAST>(IdName);
    }

    // Call.
    this->getNextToken(); // eat (
    std::vector<std::unique_ptr<ast::ExprAST>> Args;
    if (this->bt->getValue() != ")") {
        while (true) {
            if (auto Arg = ParseExpression()) {
                Args.push_back(std::move(Arg));
            } else {
                return nullptr;
            }

            if (this->bt->getTokenEnum() == lexer::OPERATOR &&
                this->bt->getValue() == ")") {
                break;
            }

            if (this->bt->getTokenEnum() != lexer::OPERATOR ||
                this->bt->getValue() != ",") {
                this->bt->printLn();
                return LogError("Expected ')' or ',' in argument list");
            }
            this->getNextToken();
        }
    }

    // Eat the ')'.
    getNextToken();

    return std::make_unique<ast::CallExprAST>(IdName, std::move(Args));
}

/// primary
///   ::= identifierexpr
///   ::= numberexpr
///   ::= parenexpr
std::unique_ptr<ast::ExprAST> Parser::ParsePrimary() {
    switch (this->bt->getTokenEnum()) {
        default:
            return LogError("unknown token when expecting an expression");
        case lexer::VARIABLE:
            return this->ParseIdentifierExpr();
        case lexer::FLOAT:
            return this->ParseNumberExpr();
        case lexer::OPERATOR:
            return this->ParseParenExpr();
        case lexer::IF:
            return this->ParseIfExpr();
    }
}

/// binoprhs
///   ::= ('+' primary)*
std::unique_ptr<ast::ExprAST> Parser::ParseBinOpRHS(
    int ExprPrec,
    std::unique_ptr<ast::ExprAST> LHS) {
    // If this is a binop, find its precedence.
    while (true) {
        int TokPrec = this->op->GetTokPrecedence(this->bt->getValue());

        // If this is a binop that binds at least as tightly as the current
        // binop, consume it, otherwise we are done.
        if (TokPrec < ExprPrec) {
            return LHS;
        }

        // Okay, we know this is a binop.
        std::string BinOp = this->bt->getValue();
        this->getNextToken(); // eat binop

        // Parse the primary expression after the binary operator.
        auto RHS = this->ParsePrimary();
        if (!RHS) {
            return nullptr;
        }

        // If BinOp binds less tightly with RHS than the operator after RHS, let
        // the pending operator take RHS as its LHS.
        int NextPrec = this->op->GetTokPrecedence(this->bt->getValue());
        if (TokPrec < NextPrec) {
            RHS = this->ParseBinOpRHS(TokPrec + 1, std::move(RHS));
            if (!RHS) {
                return nullptr;
            }
        }

        // Merge LHS/RHS.
        LHS = std::make_unique<ast::BinaryExprAST>(
            BinOp, std::move(LHS), std::move(RHS));
    }
}

/// prototype
///   ::= id '(' id* ')'
std::unique_ptr<ast::PrototypeAST> Parser::ParsePrototype() {
    if (this->bt->getTokenEnum() != lexer::VARIABLE) {
        return LogErrorP("Expected function name in prototype");
    }

    std::string FnName = this->bt->getValue();
    this->getNextToken();

    if (this->bt->getTokenEnum() != lexer::OPERATOR ||
        this->bt->getValue() != "(") {
        return LogErrorP("Expected '(' in prototype");
    }

    std::vector<std::string> ArgNames;
    this->getNextToken();
    while (this->bt->getTokenEnum() == lexer::VARIABLE) {
        ArgNames.push_back(this->bt->getValue());
        this->getNextToken();
        if (this->bt->getTokenEnum() == lexer::OPERATOR &&
            this->bt->getValue() == ",") {
            this->getNextToken();
        }
    }
    if (this->bt->getTokenEnum() != lexer::OPERATOR ||
        this->bt->getValue() != ")") {
        return LogErrorP("Expected ')' in prototype");
    }

    // success.
    this->getNextToken(); // eat ')'.

    return std::make_unique<ast::PrototypeAST>(FnName, std::move(ArgNames));
}

/// definition ::= 'def' prototype expression
std::unique_ptr<ast::FunctionAST> Parser::ParseDefinition() {
    this->getNextToken(); // eat fn.
    auto Proto = this->ParsePrototype();
    if (!Proto) {
        return nullptr;
    }
    if (auto E = this->ParseExpression()) {
        return std::make_unique<ast::FunctionAST>(
            std::move(Proto), std::move(E));
    }
    return nullptr;
}

/// toplevelexpr ::= expression
std::unique_ptr<ast::FunctionAST> Parser::ParseTopLevelExpr() {
    if (auto E = this->ParseExpression()) {
        // Make an anonymous proto.
        auto Proto = std::make_unique<ast::PrototypeAST>(
            "__anon_expr", std::vector<std::string>());
        return std::make_unique<ast::FunctionAST>(
            std::move(Proto), std::move(E));
    }
    return nullptr;
}

std::unique_ptr<ast::ExprAST> Parser::ParseIfExpr() {
    typedef std::unique_ptr<ast::ExprAST> exprType;
    std::vector<exprType> cond;
    std::vector<exprType> then;

    this->getNextToken(); // eat the if.

    bool inIfElseIfLadder = true;

    while (inIfElseIfLadder) {
        // condition.
        auto Cond = ParseExpression();
        if (!Cond) {
            return nullptr;
        }
        cond.push_back(std::move(Cond));

        if (this->getTok()->getTokenEnum() != lexer::OPERATOR ||
            this->getTok()->getValue() != "{") {
            return LogError("expected '{'");
        }
        this->getNextToken(); // eat the {

        auto Then = this->ParseExpression();
        if (!Then) {
            return nullptr;
        }
        then.push_back(std::move(Then));

        if (this->getTok()->getTokenEnum() != lexer::ELIF) {
            inIfElseIfLadder = false;
        }

        this->getNextToken(); // eat elif
    }

    if (this->getTok()->getTokenEnum() != lexer::ELSE) {
        return std::make_unique<ast::IfExprAST>(
            std::move(cond), std::move(then));
    }

    auto Else = this->ParseExpression();
    if (!Else) {
        return nullptr;
    }
    then.push_back(std::move(Else)); // last else block goes in then expr

    return std::make_unique<ast::IfExprAST>(std::move(cond), std::move(then));
}

} // namespace parser
