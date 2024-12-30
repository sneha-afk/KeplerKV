#pragma once

#include "syntax_tree.h"
#include "token.h"

class Parser {
public:
    std::vector<CommandSP> nodes;

    Parser()
        : nodes(std::vector<CommandSP>()) {};

    std::vector<CommandSP> &parse(std::vector<TokenSP> &);

private:
    TokenSP curr_();
    TokenSP peek_();
    TokenSP peekNext_();

    std::vector<TokenSP>::iterator tt_;
    std::vector<TokenSP>::iterator tend_;

    CommandSP parseCommand_();
    ValueSP parseValue_();
    ValueSP parseList_();
};
