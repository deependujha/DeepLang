#pragma once
#include <map>
#include <memory>
#include "ast/ast.h"
#include "lexer/token.h"

namespace parser {
class OperatorPrecedence {
  private:
    std::map<std::string, int> BinopPrecedence = {
        {"=", 2}, // assignment operator
        {"<", 10}, // less than
        {">", 10}, // greater than
        {"+", 20}, // addition
        {"-", 20}, // difference (subtract)
        {"*", 40}, // multiply
        {"/", 40}, // divide
        {"%", 40}, // modulo (remainder)
        {"**", 50}, // power
    };

  public:
    /// GetTokPrecedence - Get the precedence of the pending binary operator
    /// token.
    int GetTokPrecedence(std::string op);
};

class Parser {
  private:
    /// gettok - Return the next token from standard input.
    lexer::TokenOperatorUtils* tou;
    lexer::BaseToken* getTok();
    OperatorPrecedence* op;
    char LastChar = ' ';

  public:
    Parser(lexer::TokenOperatorUtils* tou)
        : tou(tou), op(new OperatorPrecedence()) {}
    lexer::BaseToken* bt{};

    void getNextToken();

    /// LogError* - These are little helper functions for error handling.
    std::unique_ptr<ast::ExprAST> LogError(const char* Str);

    std::unique_ptr<ast::PrototypeAST> LogErrorP(const char* Str);

    std::unique_ptr<ast::ExprAST> ParseExpression();

    /// parses float, string, bool
    std::unique_ptr<ast::ExprAST> ParseValueExpr();

    /// parenexpr ::= "(" expression ")"
    std::unique_ptr<ast::ExprAST> ParseParenExpr();

    /// parse import statement
    std::unique_ptr<ast::ExprAST> ParseImportExpr();

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

    /// ifexpr ::= 'if' expression 'then' expression 'else' expression
    std::unique_ptr<ast::ExprAST> ParseIfExpr();

    /// forexpr ::= 'for' identifier '=' expr ',' expr (',' expr)? 'in'
    /// expression
    std::unique_ptr<ast::ExprAST> ParseLoopExpr();

    /// varexpr ::= 'var' identifier ('=' expression)?
    //                    (',' identifier ('=' expression)?)* 'in' expression
    std::unique_ptr<ast::ExprAST> ParseVarExpr();

    /// struct & method parser
    std::unique_ptr<ast::ExprAST> ParseStructExpr();
    std::unique_ptr<ast::ExprAST> ParseStructMethodExpr();
};

} // namespace parser
