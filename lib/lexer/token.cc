#include "token.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

using namespace lexer;

BaseToken::BaseToken(
    lexer::TokenOperatorUtils* tou,
    int32_t token,
    std::string value,
    float value_float) {
    if ((token != SUPPORTED_TOKENS::STRING &&
         token != SUPPORTED_TOKENS::OPERATOR &&
         token != SUPPORTED_TOKENS::VARIABLE) &&
        !value.empty()) {
        std::string errMsg = "non-string token (" +
            tou->tokenToString[SUPPORTED_TOKENS(token)] +
            ") passes string value";
        throw std::runtime_error(errMsg);
    }
    if (token != SUPPORTED_TOKENS::FLOAT && value_float != 0.0f) {
        throw std::runtime_error("non-float token passes float value");
    }

    this->token = token;
    this->value = value;
    this->value_float = value_float;
    this->tou = tou;
}

std::string BaseToken::getToken() {
    std::string s = tou->tokenToString[this->token];
    transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

lexer::SUPPORTED_TOKENS BaseToken::getTokenEnum() {
    return lexer::SUPPORTED_TOKENS(this->token);
}

std::string BaseToken::getValue() {
    return value;
}

float BaseToken::getValueFloat() {
    return value_float;
}

void BaseToken::print() {
    std::cout << "BaseToken( token=" << this->getToken() << ", value = '"
              << this->getValue()
              << "', value_float = " << this->getValueFloat() << " )";
}

void BaseToken::printLn() {
    this->print();
    std::cout << '\n';
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
        {STRUCT, "struct"},
        {METHOD, "method"},
        {AND, "and"},
        {OR, "or"},
        {FN, "fn"},
        {RETURN, "return"},
        {IMPORT, "import"},
        {END_OF_FILE, "end_of_file"}};
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

BaseToken tokenizeWord(
    const std::string& word,
    lexer::TokenOperatorUtils* tou,
    bool isFloat) {
    if (tou->stringToToken.find(word) == tou->stringToToken.end()) {
        // token can be string or float or boolean
        // strings will start with `:`
        if (word == "false" || word == "true") {
            return BaseToken(tou, BOOLEAN, word, 0.0f);
        }
        if (isFloat) {
            return BaseToken(tou, FLOAT, "", stof(word));
        }
        return BaseToken(tou, STRING, word, 0.0f);
    }
    int32_t tk = tou->stringToToken[word];
    return BaseToken(tou, SUPPORTED_TOKENS(tk), "", 0.0f);
}
