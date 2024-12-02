#include <exception>
#include <iostream>
#include "token.h"

using namespace lexer;

int main() {
    try {
        TokenOperatorUtils* tou = new lexer::TokenOperatorUtils();

        // SUPPORTED_TOKENS t = SUPPORTED_TOKENS::LET;
        // std::cout << tou->tokenToString[t] << "\n";

        // for (auto& e : tou->tokenToString) {
        //     std::cout << e.first << " " << e.second << '\n';
        // }
        // std::cout << tou->stringToToken["let"] << '\n';
        // std::cout << (tou->stringToToken["let"] == LET) << "\n";

        // std::cout << "---------------" << '\n';

        std::vector<std::string> words = {
            "loop",
            "let",
            "for",
            "selfGrowth",
            "earnMoney",
            "=",
            "if",
            "else",
            "elseif",
            "elif"};
        for (auto& word : words) {
            BaseToken bs = tokenizeWord(word, tou);
            std::cout << word << " -> BaseToken(token="
                      << tou->tokenToString[int32_t(bs.getToken())]
                      << ", value = '" << bs.getValue()
                      << "'; value_float = " << bs.getValueFloat() << '\n';
        }
        delete tou;
    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << '\n';
        return 1; // Return non-zero to indicate an error
    } catch (...) {
        std::cerr << "Caught an unknown exception\n";
        return 1;
    }

    return 0;
}
