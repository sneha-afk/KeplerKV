#include "handler.h"

#include <iostream>

void Handler::handleQuery(std::string &query) {
    std::vector<Token> tokens = lexer_.tokenize(query);

    for (const auto &t : tokens) {
        std::cout << "\t" << t << std::endl;
    }
}
