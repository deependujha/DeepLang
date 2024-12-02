#include <exception>
#include <iostream>
#include "token.h"
#include "utils.h"

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
            "elif",
            "22.5",
            "10",
            "0",
            "0.1",
            "99",
            "99.0",
            "32.",
            "32.1e"};
        bool isFloat = false;
        for (auto& word : words) {
            isFloat = isStrFloat(word);
            BaseToken bs = tokenizeWord(word, tou, isFloat);
            bs.printLn();
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
