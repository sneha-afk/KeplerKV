#include "parser.h"

#include "syntax_tree.h"
#include "util.h"

#include <stdexcept>

std::vector<SyntaxNodeSP> &Parser::parse(std::vector<TokenSP> &tokens) {
    nodes.clear();
    if (tokens.empty()) return nodes;

    tt_ = tokens.begin();
    tend_ = tokens.end();

    // First token must be a command
    if ((*tt_)->type != TokenType::COMMAND)
        throw std::runtime_error("Error: invalid command \'" + (*tt_)->value + "\'");

    while (tt_ != tend_) {
        TokenSP &t = *tt_;
        if (t->type == TokenType::END || t->type != TokenType::COMMAND) {
            break;
        }
        nodes.push_back(parseCommand_(t));
    }

    return nodes;
}

SyntaxNodeSP Parser::parseCommand_(TokenSP &cmdTok) {
    CommandType cmdType = mapGet(mapToCmd, cmdTok->value, CommandType::UNKNOWN);
    if (cmdType == CommandType::UNKNOWN)
        throw std::runtime_error("Error: invalid command \'" + cmdTok->value + "\'");

    std::shared_ptr<CommandNode> cmd = std::make_shared<CommandNode>(cmdType);
    std::vector<SyntaxNodeSP> &args = cmd->args;
    tt_++;

    while (tt_ != tend_ && (*tt_)->type != TokenType::END) {
        TokenSP &t = *tt_;
        switch (t->type) {
            case TokenType::COMMAND:
                throw std::runtime_error("Error: nested commands not supported (yet?)");
            case TokenType::DELIMITER: tt_++; break;
            case TokenType::LIST_START: args.push_back(parseList_()); break;
            case TokenType::NUMBER:
            case TokenType::IDENTIIFER:
            case TokenType::STRING:
                args.push_back(parsePrimitive_(t));
                tt_++;
                break;
            case TokenType::UNKNOWN:
                throw std::runtime_error("Error: unknown token \'" + t->value + "\'");
            case TokenType::END:
            default: tt_++; break;
        }
    }
    return cmd;
}

SyntaxNodeSP Parser::parsePrimitive_(TokenSP &t) {
    std::string &tValue = t->value;
    switch (t->type) {
        case TokenType::NUMBER:
            if (strContains(tValue, '.'))
                return std::make_shared<FloatNode>(std::stof(tValue));
            else
                return std::make_shared<IntegerNode>(std::stoi(tValue));
        case TokenType::IDENTIIFER: return std::make_shared<IdentifierNode>(tValue);
        case TokenType::STRING: return std::make_shared<StringNode>(tValue);
        default: break;
    }
    return std::make_shared<NilNode>();
};

SyntaxNodeSP Parser::parseList_() {
    // Assuming called this method upon seeing '['
    tt_++;

    std::shared_ptr<ListNode> lstnode = std::make_shared<ListNode>();
    std::vector<SyntaxNodeSP> &lst = lstnode->value;

    while (tt_ != tend_ && (*tt_)->type != TokenType::LIST_END) {
        TokenSP &t = *tt_;
        switch (t->type) {
            case TokenType::COMMAND:
                throw std::runtime_error("Error: commands not supported within lists");
            case TokenType::DELIMITER: tt_++; break;
            case TokenType::LIST_START: lst.push_back(parseList_()); break;
            case TokenType::NUMBER:
            case TokenType::IDENTIIFER:
            case TokenType::STRING:
                lst.push_back(parsePrimitive_(t));
                tt_++;
                break;
            case TokenType::UNKNOWN:
                throw std::runtime_error("Error: unknown token \'" + t->value + "\'");
            case TokenType::END:
            default: tt_++; break;
        }
    }
    return lstnode;
};
