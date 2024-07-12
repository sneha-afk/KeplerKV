#include "handler.h"
#include "store.h"
#include "terminal_colors.h"

#include <iostream>
#include <stdexcept>

int main(int argc, char const *argv[]) {
    std::string input;
    bool running = true;
    Store store;
    Handler handler = Handler(store);

    std::cout << T_BBLUE << "Welcome to KeplerKV! Type \\q to quit!" << T_RESET
              << std::endl;
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
