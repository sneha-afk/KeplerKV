#include "environment.h"
#include "handler.h"
#include "terminal_colors.h"

#include <iostream>
#include <stdexcept>

int main() {
    std::string input;
    Store store = Store();
    Environment env = Environment(store);
    Handler handler = Handler(store, env);

    std::cout << T_BBLUE << "Welcome to KeplerKV! Type \\q to quit!" << T_RESET << std::endl;
    while (env.isRunning()) {
        std::cout << "> ";
        std::getline(std::cin, input);

        try {
            handler.handleQuery(input);
        } catch (std::exception &e) {
            std::cerr << T_BRED << e.what() << T_RESET << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
