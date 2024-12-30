#pragma once

#include "environment.h"
#include "lexer.h"
#include "parser.h"

#include <string>

class Handler {
public:
    // Dummy handler
    Handler() {};

    Handler(Store *s_ptr, Environment *e_ptr)
        : lexer_(Lexer())
        , parser_(Parser())
        , store_(s_ptr)
        , env_(e_ptr) {};

    void handleQuery(std::string &);

private:
    Lexer lexer_;
    Parser parser_;
    Store *store_;
    Environment *env_;
};
