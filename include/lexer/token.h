#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>

namespace lexer {
enum SUPPORTED_TOKENS : int8_t {
    // identifier
    LET,

    // datatypes
    VARIABLE,
    STRING,
    FLOAT,
    BOOLEAN,

    // operator
    OPERATOR,

    // conditionals
    IF,
    ELIF,
    ELSE,

    // loop
    LOOP,

    // struct & methods
    MY_TYPE,
    METHOD,

    // logical operators
    AND,
    OR,

    // function
    FN,
    RETURN,

};

class TokenOperatorUtils {
  public:
    std::unordered_map<int32_t, std::string> tokenToString;
    std::unordered_map<std::string, int32_t> stringToToken;
    std::set<std::string> operators;

    TokenOperatorUtils();

    void _initialize();
};

class BaseToken {
  protected:
    SUPPORTED_TOKENS token;
    std::string value;
    float value_float{};

  public:
    BaseToken(
        SUPPORTED_TOKENS token,
        std::string value = "",
        float value_float = 0.0f);

    SUPPORTED_TOKENS getToken();
    std::string getValue();
    float getValueFloat();
};

} // namespace lexer

lexer::BaseToken tokenizeWord(
    const std::string& word,
    lexer::TokenOperatorUtils* tou);
