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

static std::vector<std::string> operators = {
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

static std::unordered_map<int32_t, std::string> tokenToString = {
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

static std::unordered_map<std::string, int32_t> stringToToken;

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
    std::unordered_map<std::string, int32_t> stringToToken);