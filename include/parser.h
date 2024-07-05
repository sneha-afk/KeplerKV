#pragma once

#include "syntax_tree.h"
#include "token.h"

#include <vector>

class Parser {
public:
    Parser() { nodes = std::vector<std::shared_ptr<SyntaxNode>>(2); };

    std::vector<std::shared_ptr<SyntaxNode>> nodes;
    std::vector<std::shared_ptr<SyntaxNode>> &parse(
        std::vector<std::shared_ptr<Token>> &);

private:
    std::vector<std::shared_ptr<Token>>::iterator tt_;
    std::vector<std::shared_ptr<Token>>::iterator tend_;

    std::shared_ptr<SyntaxNode> parseCommand_(std::shared_ptr<Token> &);
    std::shared_ptr<SyntaxNode> parsePrimitive_(std::shared_ptr<Token> &);
    std::shared_ptr<SyntaxNode> parseList_();
};
