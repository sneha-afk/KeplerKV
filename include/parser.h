#pragma once

#include "syntax_tree.h"
#include "token.h"

class Parser {
public:
    std::vector<ASTNodeSP> nodes;

    Parser() { nodes = std::vector<ASTNodeSP>(); };

    std::vector<ASTNodeSP> &parse(std::vector<TokenSP> &);

private:
    std::vector<TokenSP>::iterator tt_;
    std::vector<TokenSP>::iterator tend_;

    ASTNodeSP parseCommand_(TokenSP &);
    StoreValueSP parseValue_(TokenSP &);
    StoreValueSP parseList_();
};
