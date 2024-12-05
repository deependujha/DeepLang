#pragma once
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace ast {
/// ExprAST - Base class for all expression nodes.
class ExprAST {
  public:
    virtual ~ExprAST() = default;
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
  public:
    float Val;
    NumberExprAST(float Val) : Val(Val) {}
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
  public:
    std::string Name;
    VariableExprAST(std::string Name) : Name(std::move(Name)) {}
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
  public:
    std::string Op;
    std::unique_ptr<ExprAST> LHS, RHS;
    BinaryExprAST(
        std::string Op,
        std::unique_ptr<ExprAST> LHS,
        std::unique_ptr<ExprAST> RHS)
        : Op(std::move(Op)), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
  public:
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;

    CallExprAST(std::string Callee, std::vector<std::unique_ptr<ExprAST>> Args)
        : Callee(std::move(Callee)), Args(std::move(Args)) {}
};

/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST {
  public:
    std::string Name;
    std::vector<std::string> Args;

    PrototypeAST(std::string Name, std::vector<std::string> Args)
        : Name(std::move(Name)), Args(std::move(Args)) {}

    const std::string& getName() const {
        return Name;
    }
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST {
  public:
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;

    FunctionAST(
        std::unique_ptr<PrototypeAST> Proto,
        std::unique_ptr<ExprAST> Body)
        : Proto(std::move(Proto)), Body(std::move(Body)) {}
};

/// IfExprAST - Expression class for if/then/else.
class IfExprAST : public ExprAST {
    typedef std::unique_ptr<ExprAST> exprType;
    // vector of condition and execute
    // (cond, then), (cond, then), (then) => (if then), (elif then), (else)
  public:
    std::vector<exprType> cond;
    std::vector<exprType> then;
    IfExprAST(std::vector<exprType> cond, std::vector<exprType> then)
        : cond(std::move(cond)), then(std::move(then)) {}
};

} // namespace ast
