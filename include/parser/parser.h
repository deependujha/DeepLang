#pragma once
#include <map>
#include <utility>
#include "ast.h"

namespace parser {
class OperatorPrecedence {
  private:
    std::map<char, int> BinopPrecedence = {
        {'<', 10},
        {'+', 20},
        {'-', 20},
        {'*', 40}, // highest.
    };

  public:
    /// GetTokPrecedence - Get the precedence of the pending binary operator
    /// token.
    int GetTokPrecedence(char op);
};

/// top ::= definition | external | expression | ';'
static void MainLoop() {
    while (true) {
        fprintf(stderr, "ready> ");
    }
}
} // namespace parser
