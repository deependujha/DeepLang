#include "token.h"
#include <map>
#include <string>
#include <utility>
#include <vector>

// class BaseToken {
//   protected:
//     std::string token;
//     std::string value;
//     float value_float;

//   public:
//     BaseToken(
//         std::string token = "NIL",
//         std::string value = "",
//         float value_float = 0.0f)
//         : token(std::move(token)),
//           value(std::move(value)),
//           value_float(value_float) {}

//     std::string getToken() {
//         return token;
//     }
//     std::string getValue() {
//         return value;
//     }
//     float getValueFloat() {
//         return value_float;
//     }
// };

BaseToken::BaseToken(std::string token, std::string value, float value_float)
    : token(std::move(token)),
      value(std::move(value)),
      value_float(value_float) {}

std::string BaseToken::getToken() {
    return token;
}

std::string BaseToken::getValue() {
    return value;
}

float BaseToken::getValueFloat() {
    return value_float;
}

class NumberToken : public BaseToken {
  public:
    NumberToken(
        std::string token = "NIL",
        std::string value = "",
        float value_float = 0.0f)
        : BaseToken(token, value, value_float) {}
};
class StringToken : public BaseToken {
  public:
    StringToken(
        std::string token = "NIL",
        std::string value = "",
        float value_float = 0.0f)
        : BaseToken(token, value, value_float) {}
};
class BooleanToken : public BaseToken {
  public:
    BooleanToken(
        std::string token = "NIL",
        std::string value = "",
        float value_float = 0.0f)
        : BaseToken(token, value, value_float) {}
};
class IdentifierToken : public BaseToken {
  public:
    IdentifierToken(
        std::string token = "NIL",
        std::string value = "",
        float value_float = 0.0f)
        : BaseToken(token, value, value_float) {}
};
class KeywordToken : public BaseToken {
  public:
    KeywordToken(
        std::string token = "NIL",
        std::string value = "",
        float value_float = 0.0f)
        : BaseToken(token, value, value_float) {}
};
class OperatorToken : public BaseToken {
  public:
    OperatorToken(
        std::string token = "NIL",
        std::string value = "",
        float value_float = 0.0f)
        : BaseToken(token, value, value_float) {}
};

void supportedKeywords() {
    std::vector<std::string> keywords = {
        // identifier
        "let",

        // conditionals
        "if",
        "elif",
        "else",

        // loop
        "loop",

        // struct & methods
        "mytype",
        "method",

        // logical operators
        "and",
        "or",

        // function
        "fn",
        "return",
    };
    std::map<std::string, BaseToken> mp = {
        {"let", KeywordToken("let", "let", 0.0f)}};
}

void supportedOperator() {
    std::vector<std::string> operators = {
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
