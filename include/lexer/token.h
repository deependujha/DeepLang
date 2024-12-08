#pragma once
#include <set>
#include <string>
#include <unordered_map>

namespace lexer {
enum SUPPORTED_TOKENS : int8_t {
    // identifier
    LET,

    // datatypes
    VARIABLE,
    STRING,
    FLOAT,
    BOOLEAN,
    VECTOR,

    // operator
    OPERATOR,

    // conditionals
    IF,
    ELIF,
    ELSE,

    // loop
    LOOP,

    // struct & methods
    STRUCT,
    METHOD,

    // logical operators
    AND,
    OR,

    // function
    FN,
    RETURN,

    // import
    IMPORT,

    // end-of-file
    END_OF_FILE,

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
    SUPPORTED_TOKENS getTokenEnum();

    std::string getValue();

    float getValueFloat();

    void print();

    void printLn();
};

} // namespace lexer

lexer::BaseToken tokenizeWord(
    const std::string& word,
    lexer::TokenOperatorUtils* tou,
    bool isFloat = false);
