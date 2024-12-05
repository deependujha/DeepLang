#include "parser/parser.h"
#include <iostream>
#include "lexer/token.h"
#include "utils/check_type.h"

namespace parser {
int parser::OperatorPrecedence::GetTokPrecedence(const std::string op) {
    int TokPrec = BinopPrecedence[op];
    if (TokPrec <= 0) {
        return -1;
    }
    return TokPrec;
}

void Parser::getNextToken() {
    this->bt = this->getTok();
}

lexer::BaseToken* Parser::getTok() {
    float NumVal = 0;
    std::string IdentifierStr;

    // Skip any whitespace.
    while (isSpace(this->LastChar) || this->LastChar == '\n' ||
           this->LastChar == '\t' || this->LastChar == '\r') {
        this->LastChar = char(getchar());
    }

    if (isAlpha(this->LastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
        IdentifierStr = std::string{this->LastChar};
        this->LastChar = char(getchar());
        while (isAlphaNumeric(this->LastChar) || this->LastChar == '_') {
            IdentifierStr += std::string{this->LastChar};
            this->LastChar = char(getchar());
        }

        if (this->tou->stringToToken.find(IdentifierStr) !=
            this->tou->stringToToken.end()) {
            lexer::SUPPORTED_TOKENS _tk = lexer::SUPPORTED_TOKENS(
                this->tou->stringToToken[IdentifierStr]);
            return new lexer::BaseToken(this->tou, _tk);
        }

        return new lexer::BaseToken(
            this->tou, lexer::VARIABLE, IdentifierStr, NumVal);
    }

    if (isNumeric(this->LastChar)) { // Number: [0-9.]+
        std::string NumStr;
        do {
            NumStr += std::string{this->LastChar};
            this->LastChar = char(getchar());
        } while (isdigit(this->LastChar) || this->LastChar == '.');

        NumVal = strtof(NumStr.c_str(), nullptr);
        return new lexer::BaseToken(
            this->tou, lexer::FLOAT, IdentifierStr, NumVal);
    }

    if (this->LastChar == '#') {
        // Comment until end of line.
        do {
            this->LastChar = char(getchar());
        } while (this->LastChar != EOF && this->LastChar != '\n' &&
                 this->LastChar != '\r');

        if (this->LastChar != EOF) {
            return this->getTok();
        }
    }

    // Check for end of file.  Don't eat the EOF.
    if (this->LastChar == EOF) {
        return new lexer::BaseToken(this->tou, lexer::END_OF_FILE);
    }

    // Otherwise, just return the character as its ascii value.
    std::string opr = std::string{this->LastChar};
    // check if opr is closing/starting symbol
    if (opr == "{" || opr == "}" || opr == "[" || opr == "]" || opr == ";" ||
        opr == ")") {
        this->LastChar = ' ';
        return new lexer::BaseToken(this->tou, lexer::OPERATOR, opr);
    }
    this->LastChar = char(getchar());
    // while char is neither alphanumeric nor ; nor ()
    while (!isAlphaNumeric(this->LastChar) && !isSpace(this->LastChar)) {
        opr += this->LastChar;
        this->LastChar = char(getchar());
    }

    return new lexer::BaseToken(this->tou, lexer::OPERATOR, opr);
}

} // namespace parser