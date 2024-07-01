#include "handler.h"

#include <iostream>

int main(int argc, char const *argv[]) {
    std::string input;
    Handler handler;

    std::cout << "Welcome to KeplerKV!" << std::endl;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "\\q") break;

        handler.handleQuery(input);
    }

    return EXIT_SUCCESS;
}
