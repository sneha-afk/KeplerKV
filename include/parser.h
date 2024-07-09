#pragma once

#include "syntax_tree.h"
#include "token.h"

class Parser {
public:
    Parser() { nodes = std::vector<SyntaxNodeSP>(); };

    std::vector<SyntaxNodeSP> nodes;
    std::vector<SyntaxNodeSP> &parse(std::vector<TokenSP> &);

private:
    std::vector<TokenSP>::iterator tt_;
    std::vector<TokenSP>::iterator tend_;

    SyntaxNodeSP parseCommand_(TokenSP &);
    SyntaxNodeSP parsePrimitive_(TokenSP &);
    SyntaxNodeSP parseList_();
};
