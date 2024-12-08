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

/// parenexpr ::= "(" expression ")"
std::unique_ptr<ast::ExprAST> Parser::ParseParenExpr() {
    if (this->bt->getTokenEnum() != lexer::OPERATOR ||
        this->bt->getValue() != "(") {
        this->bt->printLn();
        std::cout << "not a paren expression\n";
        return nullptr;
    }
    this->getNextToken(); // eat (.
    auto V = this->ParseExpression();
    if (!V) {
        return nullptr;
    }

    if (this->bt->getTokenEnum() != lexer::OPERATOR ||
        this->bt->getValue() != ")") {
        this->bt->printLn();
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
        return std::make_unique<ast::VariableNameExprAST>(IdName);
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
    if (this->bt->getTokenEnum() == lexer::FLOAT ||
        this->bt->getTokenEnum() == lexer::STRING ||
        this->bt->getTokenEnum() == lexer::BOOLEAN) {
        return this->ParseValueExpr();
    }

    else if (this->bt->getTokenEnum() == lexer::OPERATOR) {
        return this->ParseParenExpr();
    }

    else if (this->bt->getTokenEnum() == lexer::IF) {
        return this->ParseIfExpr();
    }

    else if (this->bt->getTokenEnum() == lexer::LOOP) {
        return this->ParseLoopExpr();
    }

    else if (this->bt->getTokenEnum() == lexer::VARIABLE) {
        return this->ParseIdentifierExpr();
    }

    else if (this->bt->getTokenEnum() == lexer::LET) {
        return this->ParseVarExpr();
    }

    else {
        this->bt->printLn();
        return LogError("unknown token when expecting an expression");
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
    if (this->bt->getValue() != "{") {
        std::cout << "expected '{'\n";
        return nullptr;
    }
    this->getNextToken();
    std::vector<std::unique_ptr<ast::ExprAST>> Body;
    std::unique_ptr<ast::ExprAST> Ret = nullptr;
    while (true) {
        if (this->bt->getTokenEnum() == lexer::RETURN) {
            this->getNextToken();
            if (this->bt->getValue() != ";") {
                auto E = this->ParseExpression();
                Ret = std::move(E);
            }

            // eat ';' and '}' and return
            if (this->bt->getValue() != ";") {
                std::cout << "expected ';'\n";
                return nullptr;
            }
            this->getNextToken();
            if (this->bt->getValue() != "}") {
                std::cout << "expected '}'\n";
                return nullptr;
            }
            this->getNextToken();
            return std::make_unique<ast::FunctionAST>(
                std::move(Proto), std::move(Body), std::move(Ret));
        }
        auto E = this->ParseExpression();
        Body.push_back(std::move(E));

        // eat ';'
        if (this->bt->getValue() != ";") {
            std::cout << "expected ';'\n";
            return nullptr;
        }
        this->getNextToken();
    }
    return nullptr;
}

/// toplevelexpr ::= expression
std::unique_ptr<ast::FunctionAST> Parser::ParseTopLevelExpr() {
    if (auto E = this->ParseExpression()) {
        std::vector<std::unique_ptr<ast::ExprAST>> e;
        // e.push_back(std::move(E));
        // Make an anonymous proto.
        auto Proto = std::make_unique<ast::PrototypeAST>(
            "__anon_expr", std::vector<std::string>());
        return std::make_unique<ast::FunctionAST>(
            std::move(Proto), std::move(e), std::move(E));
    }
    return nullptr;
}

std::unique_ptr<ast::ExprAST> Parser::ParseImportExpr() {
    if (this->bt->getTokenEnum() != lexer::IMPORT) {
        return LogError("expected import");
    }
    this->getNextToken(); // eat import
    if (this->bt->getTokenEnum() != lexer::STRING) {
        return LogError("expected path(string) after import keyword");
    }
    std::string importPath = this->bt->getValue();

    return std::make_unique<ast::ImportExprAST>(importPath);
}

} // namespace parser
