#include "parser.h"

#include "syntax_tree.h"

#include <stdexcept>

Parser::Parser() { nodes = std::vector<SyntaxNode>(2); }

std::vector<SyntaxNode> &Parser::parse(std::vector<Token> &tokens) {
    nodes.clear();
    if (tokens.empty()) return nodes;

    tt_ = tokens.begin();
    tend_ = tokens.end();
    while (tt_ != tend_) {
        Token &t = *tt_;

        switch (t.type) {
            case TokenType::COMMAND: nodes.push_back(parseCmd_(t)); break;
            case TokenType::DELIMITER: tt_++; break;
            case TokenType::LIST_START:
                tt_++;
                nodes.push_back(parseList_());
                break;
            case TokenType::NUMBER:
            case TokenType::STRING: nodes.push_back(parsePrimitive_(t)); break;
            case TokenType::UNKNOWN:
                throw std::runtime_error("Error: unknown token \'" + t.value + "\'");
                break;
            default: tt_++; break;
        }
    }

    return nodes;
}
