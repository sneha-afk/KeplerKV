#pragma once

#include "environment.h"
#include "lexer.h"
#include "parser.h"

#include <string>

class Handler {
public:
    Handler()
        : lexer_(Lexer())
        , parser_(Parser())
        , store_(Store())
        , env_(Environment(store_)) {};

    Handler(Store &s)
        : lexer_(Lexer())
        , parser_(Parser())
        , store_(s)
        , env_(Environment(s)) {};

    Handler(Store &s, Environment &e)
        : lexer_(Lexer())
        , parser_(Parser())
        , store_(s)
        , env_(e) {};

    void handleQuery(std::string &);

private:
    Lexer lexer_;
    Parser parser_;
    Store store_;
    Environment env_;
};
