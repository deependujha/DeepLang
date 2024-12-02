#include "token.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace lexer;

BaseToken::BaseToken(
    SUPPORTED_TOKENS token,
    std::string value,
    float value_float) {
    if ((token != SUPPORTED_TOKENS::STRING &&
         token != SUPPORTED_TOKENS::OPERATOR) &&
        !value.empty()) {
        throw std::runtime_error("non-string token passes string value");
    }
    if (token != SUPPORTED_TOKENS::FLOAT && value_float != 0.0f) {
        throw std::runtime_error("non-float token passes float value");
    }

    this->token = token;
    this->value = value;
    this->value_float = value_float;
}

SUPPORTED_TOKENS BaseToken::getToken() {
    return token;
}

std::string BaseToken::getValue() {
    return value;
}

float BaseToken::getValueFloat() {
    return value_float;
}

BaseToken tokenizeWord(
    const std::string& word,
    lexer::TokenOperatorUtils* tou) {
    if (tou->stringToToken.find(word) == tou->stringToToken.end()) {
        return BaseToken(STRING, word, 0.0f);
    }
    int32_t tk = tou->stringToToken[word];
    return BaseToken(SUPPORTED_TOKENS(tk), "", 0.0f);
}

TokenOperatorUtils::TokenOperatorUtils() {
    this->_initialize();
}

void TokenOperatorUtils::_initialize() {
    this->tokenToString = {
        {LET, "let"},
        {VARIABLE, "variable"},
        {STRING, "string"},
        {FLOAT, "float"},
        {BOOLEAN, "bool"},
        {OPERATOR, "operator"},
        {IF, "if"},
        {ELIF, "elif"},
        {ELSE, "else"},
        {LOOP, "loop"},
        {MY_TYPE, "my_type"},
        {METHOD, "method"},
        {AND, "and"},
        {OR, "or"},
        {FN, "fn"},
        {RETURN, "return"},
    };
    for (auto& e : this->tokenToString) {
        this->stringToToken[e.second] = e.first;
    }
    operators = {
        "+",
        "-",
        "*",
        "/",
        "//",
        "**",
        "=",
        "==",
        "->",
        ">",
        "<",
        ">=",
        "<=",
    };
}
