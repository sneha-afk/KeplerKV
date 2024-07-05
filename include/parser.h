#pragma once

#include "syntax_tree.h"
#include "token.h"

#include <vector>

class Parser {
public:
    Parser();
    std::vector<SyntaxNode> nodes;
    std::vector<SyntaxNode> &parse(std::vector<Token> &);

private:
    std::vector<Token>::iterator tt_;
    std::vector<Token>::iterator tend_;

    SyntaxNode parseCommand_(Token &);
    SyntaxNode parsePrimitive_(Token &);
    SyntaxNode parseList_();
};
