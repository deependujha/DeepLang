#include "parser.h"

int parser::OperatorPrecedence::GetTokPrecedence(char op) {
    int TokPrec = BinopPrecedence[op];
    if (TokPrec <= 0) {
        return -1;
    }
    return TokPrec;
}
