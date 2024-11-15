#pragma once

#include "lexer.h"
#include "parser.h"
#include "store.h"

#include <functional>
#include <string>

class Handler;
using HandlerFunctionPtr
    = std::function<void(Handler *, std::vector<ValueASTNodeSP> &, const std::size_t)>;

class Handler {
public:
    Handler()
        : lexer_(Lexer())
        , parser_(Parser())
        , store_(Store()) {};

    Handler(Lexer &l, Parser &p, Store &s)
        : lexer_(l)
        , parser_(p)
        , store_(s) {};

    Handler(Store &s)
        : lexer_(Lexer())
        , parser_(Parser())
        , store_(s) {};

    bool handleQuery(std::string &);

private:
    Lexer lexer_;
    Parser parser_;
    Store store_;

    static std::unordered_map<CommandType, HandlerFunctionPtr> cmdToFunc_;
};
