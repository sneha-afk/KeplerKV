#include "handler.h"

#include <iostream>

void Handler::handleQuery(std::string &query) {
    std::vector<std::shared_ptr<Token>> &tokens = lexer_.tokenize(query);
    for (const auto &t : tokens) {
        std::cout << "\t" << *t << std::endl;
    }

    std::vector<std::shared_ptr<SyntaxNode>> &nodes = parser_.parse(tokens);
    for (const auto &n : nodes) {
        std::cout << "\t" << *n << std::endl;
    }
}
