#include "handler.h"

#include <iostream>
#include <stdexcept>

// https://stackoverflow.com/q/5947742
#define TERM_RED   "\033[1;31m"
#define TERM_BLUE  "\033[0;34m"
#define TERM_RESET "\033[0m"

int main(int argc, char const *argv[]) {
    std::string input;
    Handler handler;

    std::cout << "Welcome to KeplerKV!" << std::endl;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "\\q") break;

        try {
            handler.handleQuery(input);
        } catch (std::runtime_error &e) {
            std::cerr << TERM_RED << e.what() << TERM_RESET << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
