#pragma once
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "llvm/IR/Function.h"

namespace ast {
/// ExprAST - Base class for all expression nodes.
class ExprAST {
  public:
    virtual ~ExprAST() = default;
};

/// StatementAST - Base class for all statement nodes. `varName = expr;`
class StatementAST : public ExprAST {
  public:
    std::string varName;
    ExprAST* expr;
    StatementAST(std::string varName, ExprAST* expr)
        : varName(std::move(varName)), expr(expr) {}
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
  public:
    float Val;
    NumberExprAST(float Val) : Val(Val) {}
};

class BooleanExprAST : public ExprAST {
  public:
    bool Val;
    BooleanExprAST(bool Val) : Val(Val) {}
};

class StringExprAST : public ExprAST {
  public:
    std::string Val;
    StringExprAST(std::string Val) : Val(std::move(Val)) {}
};

class VectorCreateExprAST : public ExprAST {
  public:
    std::string dtype;
    std::vector<unsigned int> dims;
    VectorCreateExprAST(std::string dtype, std::vector<unsigned int> dims)
        : dtype(std::move(dtype)), dims(std::move(dims)) {}
};

class VectorSetExprAST : public ExprAST {
  public:
    std::string vectorName;
    int index;
    std::string(value);
    VectorSetExprAST(std::string vectorName, int index, std::string value)
        : vectorName(std::move(vectorName)),
          index(index),
          value(std::move(value)) {}

    VectorSetExprAST(std::string vectorName, int index, float value)
        : vectorName(std::move(vectorName)),
          index(index),
          value(std::to_string(value)) {}
    VectorSetExprAST(std::string vectorName, int index, bool value)
        : vectorName(std::move(vectorName)),
          index(index),
          value(value ? "true" : "false") {}
};

class VectorGetExprAST : public ExprAST {
  public:
    std::string vectorName;
    int index;
    VectorGetExprAST(std::string vectorName, int index)
        : vectorName(std::move(vectorName)), index(index) {}
};

/// VariableNameExprAST - Expression class for referencing a variable, like "a".
class VariableNameExprAST : public ExprAST {
  public:
    std::string Name;
    VariableNameExprAST(std::string Name) : Name(std::move(Name)) {}
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
/// of arguments the function takes). `void funcname(arg1, arg2);`
class PrototypeAST {
  public:
    std::string Name;
    std::vector<std::string> Args;

    PrototypeAST(std::string Name, std::vector<std::string> Args)
        : Name(std::move(Name)), Args(std::move(Args)) {}
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST {
  public:
    std::unique_ptr<PrototypeAST> Proto;
    std::vector<std::unique_ptr<ExprAST>> Body;
    std::unique_ptr<ExprAST> Ret;

    FunctionAST(
        std::unique_ptr<PrototypeAST> Proto,
        std::vector<std::unique_ptr<ExprAST>> Body,
        std::unique_ptr<ExprAST> Ret = nullptr)
        : Proto(std::move(Proto)), Body(std::move(Body)), Ret(std::move(Ret)) {}
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

/// LoopExprAST - Expression class for for/in.
class LoopExprAST : public ExprAST {
  public:
    std::string VarName;
    std::unique_ptr<ExprAST> Start, End, Step, Body;
    LoopExprAST(
        std::string VarName,
        std::unique_ptr<ExprAST> Start,
        std::unique_ptr<ExprAST> End,
        std::unique_ptr<ExprAST> Step,
        std::unique_ptr<ExprAST> Body)
        : VarName(std::move(VarName)),
          Start(std::move(Start)),
          End(std::move(End)),
          Step(std::move(Step)),
          Body(std::move(Body)) {}
};

/// VarDeclExprAST - Expression class for declaring variable and assigning
/// values to them.
class VarDeclExprAST : public ExprAST {
  public:
    std::vector<std::string> VarNames;
    std::vector<std::string> VarTypes;
    std::vector<std::unique_ptr<ExprAST>> VarValues;

    VarDeclExprAST(
        std::vector<std::string> VarNames,
        std::vector<std::string> VarTypes,
        std::vector<std::unique_ptr<ExprAST>> VarValues)
        : VarNames(std::move(VarNames)),
          VarTypes(std::move(VarTypes)),
          VarValues(std::move(VarValues)) {}
};

/// importAST: `import "./somedir/somepath.deeplang;"
class ImportExprAST : public ExprAST {
  public:
    std::string importPath;
    ImportExprAST(std::string importPath) : importPath(std::move(importPath)) {}
};

class StructExprAST : public ExprAST {
  public:
    std::string structName;
    std::vector<std::string> fieldTypes;
    std::vector<std::string> fieldNames;
    StructExprAST(
        std::string structName,
        std::vector<std::string> fieldTypes,
        std::vector<std::string> fieldNames)
        : structName(std::move(structName)),
          fieldTypes(std::move(fieldTypes)),
          fieldNames(std::move(fieldNames)) {}
};

class StructMethodExprAST : public ExprAST {
  public:
    std::string structName;
    std::unique_ptr<ast::FunctionAST> method;
    StructMethodExprAST(
        std::string structName,
        std::unique_ptr<ast::FunctionAST> method)
        : structName(std::move(structName)), method(std::move(method)) {}
};

} // namespace ast
