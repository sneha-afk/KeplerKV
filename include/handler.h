#pragma once

#include "lexer.h"
#include "parser.h"
#include "store.h"

#include <string>

class Handler {
public:
    Handler() {
        lexer_ = Lexer();
        parser_ = Parser();
        store_ = Store();
    };

    Handler(Lexer &l, Parser &p, Store &s)
        : lexer_(l)
        , parser_(p)
        , store_(s) {};

    void handleQuery(std::string &);

private:
    Lexer lexer_;
    Parser parser_;
    Store store_;
};
