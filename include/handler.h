#pragma once

#include "lexer.h"
#include "parser.h"
#include "store.h"

#include <functional>
#include <string>

class Handler;
using HandlerFunctionPtr
    = std::function<void(Handler *, std::vector<ValueNodeSP> &, const std::size_t)>;

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

    Handler(Store &s)
        : store_(s) {
        lexer_ = Lexer();
        parser_ = Parser();
    };

    bool handleQuery(std::string &);

private:
    Lexer lexer_;
    Parser parser_;
    Store store_;

    static std::unordered_map<CommandType, HandlerFunctionPtr> cmdToFunc_;

    void print_item_(const std::string &, StoreValueSP);

    std::string &getFilename_(ValueNodeSP);

    void handleStats_();

    void handleSet_(std::vector<ValueNodeSP> &, const std::size_t);
    void handleGet_(std::vector<ValueNodeSP> &, const std::size_t);
    void handleDelete_(std::vector<ValueNodeSP> &, const std::size_t);
    void handleUpdate_(std::vector<ValueNodeSP> &, const std::size_t);
    void handleResolve_(std::vector<ValueNodeSP> &, const std::size_t);
    void handleSave_(std::vector<ValueNodeSP> &, const std::size_t);
    void handleLoad_(std::vector<ValueNodeSP> &, const std::size_t);
    void handleRename_(std::vector<ValueNodeSP> &, const std::size_t);
    void handleIncr_(std::vector<ValueNodeSP> &, const std::size_t);
    void handleDecr_(std::vector<ValueNodeSP> &, const std::size_t);
    void handleAppend_(std::vector<ValueNodeSP> &, const std::size_t);
    void handlePrepend_(std::vector<ValueNodeSP> &, const std::size_t);
};
