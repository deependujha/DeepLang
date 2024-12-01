#include <iostream>
#include "token.h"

using namespace lexer;

int main() {
    SUPPORTED_TOKENS t = SUPPORTED_TOKENS::LET;
    std::cout << tokenToString[t] << "\n";

    for (auto& e : tokenToString) {
        std::cout << e.first << " " << e.second << '\n';
    }
    std::cout << stringToToken["let"] << '\n';
    std::cout << (stringToToken["let"] == LET) << "\n";

    std::cout << "---------------" << '\n';

    std::string word = "loop";
    BaseToken bs = tokenizeWord(word, stringToToken);
    std::cout << "==" << '\n';
    std::cout << word
              << " -> BaseToken(token=" << tokenToString[int32_t(bs.getToken())]
              << ", value = " << bs.getValue()
              << "; value_float = " << bs.getValueFloat() << '\n';
    return 0;
}
