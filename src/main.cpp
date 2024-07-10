#include "handler.h"
#include "terminal_colors.h"

#include <iostream>
#include <stdexcept>

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
            std::cerr << T_BRED << e.what() << T_RESET << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
