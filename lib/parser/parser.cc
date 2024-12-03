#include "parser/parser.h"
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

lexer::BaseToken* Parser::getNextToken() {
    this->bt = this->getTok();
    return this->bt;
}

lexer::BaseToken* Parser::getTok() {
    char LastChar = ' ';
    float NumVal = 0;
    std::string IdentifierStr;

    // Skip any whitespace.
    while (isSpace(LastChar)) {
        LastChar = char(getchar());
    }

    if (LastChar == ';') {
        return new lexer::BaseToken(this->tou, lexer::SEMICOLON);
    }

    if (isAlpha(LastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
        IdentifierStr = std::to_string(LastChar);
        while (isAlphaNumeric((LastChar = char(getchar())))) {
            IdentifierStr += std::to_string(LastChar);
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

    if (isNumeric(LastChar)) { // Number: [0-9.]+
        std::string NumStr;
        do {
            NumStr += std::to_string(LastChar);
            LastChar = char(getchar());
        } while (isdigit(LastChar) || LastChar == '.');

        NumVal = strtof(NumStr.c_str(), nullptr);
        return new lexer::BaseToken(
            this->tou, lexer::FLOAT, IdentifierStr, NumVal);
    }

    if (LastChar == '#') {
        // Comment until end of line.
        do {
            LastChar = char(getchar());
        } while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

        if (LastChar != EOF) {
            return this->getTok();
        }
    }

    // Check for end of file.  Don't eat the EOF.
    if (LastChar == EOF) {
        return new lexer::BaseToken(this->tou, lexer::END_OF_FILE);
    }

    // Otherwise, just return the character as its ascii value.
    // int ThisChar = LastChar;
    // LastChar = char(getchar());
    return new lexer::BaseToken(
        this->tou, lexer::OPERATOR, std::string{LastChar});
}

} // namespace parser