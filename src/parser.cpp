#include "parser.h"

#include "syntax_tree.h"
#include "util.h"

#include <stdexcept>

Parser::Parser() { nodes = std::vector<SyntaxNode>(2); }

std::vector<SyntaxNode> &Parser::parse(std::vector<Token> &tokens) {
    nodes.clear();
    if (tokens.empty()) return nodes;

    tt_ = tokens.begin();
    tend_ = tokens.end();

    // First token must be a command
    if ((*tt_).type != TokenType::COMMAND)
        throw std::runtime_error("Error: invalid command \'" + (*tt_).value + "\'");

    while (tt_ != tend_) {
        Token &t = *tt_;
        nodes.push_back(parseCommand_(t));
    }

    return nodes;
}

SyntaxNode Parser::parseCommand_(Token &t) {
    CommandType cmdType = mapGet(mapToCmd, t.value, CommandType::UNKNOWN);
    if (cmdType == CommandType::UNKNOWN)
        throw std::runtime_error("Error: invalid command \'" + (*tt_).value + "\'");

    CommandNode cmd = CommandNode(cmdType);
    std::list<SyntaxNode> &args = cmd.args;

    while (tt_ != tend_ && (*tt_).type != TokenType::END) {
        Token &t = *tt_;

        switch (t.type) {
            case TokenType::END: break;
            case TokenType::COMMAND:
                throw std::runtime_error("Error: nested commands not supported (yet?)");
            case TokenType::DELIMITER: tt_++; break;
            case TokenType::LIST_START: args.push_back(parseList_()); break;
            case TokenType::NUMBER:
            case TokenType::STRING: args.push_back(parsePrimitive_(t)); break;
            case TokenType::UNKNOWN:
                throw std::runtime_error("Error: unknown token \'" + t.value + "\'");
            default: tt_++; break;
        }
    }
    return cmd;
}

SyntaxNode Parser::parsePrimitive_(Token &t) { return SyntaxNode(); };

SyntaxNode Parser::parseList_() {
    // Assuming called this method upon seeing '['
    tt_++;
    return SyntaxNode();
};
