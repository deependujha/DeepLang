#include "token.h"
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
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
        throw std::invalid_argument("non-string token passes string value");
    }
    if (token != SUPPORTED_TOKENS::FLOAT && value_float != 0.0f) {
        throw std::invalid_argument("non-float token passes float value");
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
    std::unordered_map<std::string, int32_t> stringToToken) {
    std::cout << "in the function = " << word << "\n"
              << stringToToken.size() << "\n";
    if (stringToToken.find(word) == stringToToken.end()) {
        return BaseToken(VARIABLE, word, 0.0f);
    }
    std::cout << (stringToToken.find(word) == stringToToken.end()) << "\n";
    int32_t tk = stringToToken[word];
    std::cout << "tk is: " << tk << "\n";
    return BaseToken(SUPPORTED_TOKENS(tk), "", 0.0f);
}
