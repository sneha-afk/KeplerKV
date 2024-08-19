#include "handler.h"
#include "terminal_colors.h"

#include <iostream>
#include <stdexcept>

int main() {
    std::string input;
    bool running = true;
    Handler handler = Handler();

    std::cout << T_BBLUE << "Welcome to KeplerKV! Type \\q to quit!" << T_RESET << std::endl;
    while (running) {
        std::cout << "> ";
        std::getline(std::cin, input);

        try {
            running = handler.handleQuery(input);
        } catch (std::runtime_error &e) {
            std::cerr << T_BRED << e.what() << T_RESET << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
