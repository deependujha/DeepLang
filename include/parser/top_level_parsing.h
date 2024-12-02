#pragma once
#include <memory>
#include <utility>
#include "ast.h"
#include "token.h"

namespace top_level_parsing {
class Parsing {
  private:
    float NumVal;
    std::string IdentifierStr;
    std::string CurTok;

  public:
    void getNextToken();

    /// LogError* - These are little helper functions for error handling.
    std::unique_ptr<ast::ExprAST> LogError(const char* Str);

    std::unique_ptr<ast::PrototypeAST> LogErrorP(const char* Str);

    std::unique_ptr<ast::ExprAST> ParseExpression();

    /// numberexpr ::= number
    std::unique_ptr<ast::ExprAST> ParseNumberExpr();
    /// parenexpr ::= "(" expression ")"
    std::unique_ptr<ast::ExprAST> ParseParenExpr();

    /// identifierexpr
    ///   ::= identifier
    ///   ::= identifier "(" expression* ")"
    std::unique_ptr<ast::ExprAST> ParseIdentifierExpr();

    /// primary
    ///   ::= identifierexpr
    ///   ::= numberexpr
    ///   ::= parenexpr
    std::unique_ptr<ast::ExprAST> ParsePrimary();

    /// binoprhs
    ///   ::= ('+' primary)*
    std::unique_ptr<ast::ExprAST> ParseBinOpRHS(
        int ExprPrec,
        std::unique_ptr<ast::ExprAST> LHS);

    /// expression
    ///   ::= primary binoprhs

    /// prototype
    ///   ::= id "(" id* ")"
    std::unique_ptr<ast::PrototypeAST> ParsePrototype();

    /// definition ::= 'def' prototype expression
    std::unique_ptr<ast::FunctionAST> ParseDefinition();

    /// toplevelexpr ::= expression
    std::unique_ptr<ast::FunctionAST> ParseTopLevelExpr();
};

} // namespace top_level_parsing
