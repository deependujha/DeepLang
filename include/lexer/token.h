#pragma once
#include <set>
#include <string>
#include <unordered_map>
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
    int32_t token;
    std::string value;
    float value_float{};
    lexer::TokenOperatorUtils* tou;

  public:
    BaseToken(
        lexer::TokenOperatorUtils* tou,
        int32_t token,
        std::string value = "",
        float value_float = 0.0f);

    // methods
    std::string getToken();

    std::string getValue();

    float getValueFloat();

    void print();

    void printLn();
};

} // namespace lexer

lexer::BaseToken tokenizeWord(
    const std::string& word,
    lexer::TokenOperatorUtils* tou);
