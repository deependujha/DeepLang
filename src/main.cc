#include <exception>
#include <iostream>
#include "lexer/token.h"
#include "main_loop.h"

using namespace lexer;

int main() {
    try {
        TokenOperatorUtils* tou = new lexer::TokenOperatorUtils();

        MainLoop* mp = new MainLoop();
        mp->run();

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
