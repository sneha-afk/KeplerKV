#pragma once

#include "lexer.h"
#include "parser.h"

#include <string>

class Handler {
public:
    Handler() {
        lexer_ = Lexer();
        parser_ = Parser();
    };

    Handler(Lexer &l, Parser &p)
        : lexer_(l)
        , parser_(p) {};

    void handleQuery(std::string &);

private:
    Lexer lexer_;
    Parser parser_;
};
