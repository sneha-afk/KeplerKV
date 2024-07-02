#pragma once

#include "syntax_tree.h"
#include <vector>

class Parser {
public:
    std::vector<SyntaxNode> nodes;
    std::vector<SyntaxNode> &parse(std::vector<Token> &);

private:
    std::vector<Token>::iterator tt_;
    std::vector<Token>::iterator tend_;

    SyntaxNode parseCmd_(Token &);
    SyntaxNode parsePrimitive_(Token &);
    SyntaxNode parseList_(Token &);
};
