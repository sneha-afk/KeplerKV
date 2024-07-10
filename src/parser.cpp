#include "parser.h"

#include "syntax_tree.h"
#include "util.h"

#include <stdexcept>

std::vector<ASTNodeSP> &Parser::parse(std::vector<TokenSP> &tokens) {
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

ASTNodeSP Parser::parseCommand_(TokenSP &cmdTok) {
    CommandType cmdType = mapGet(mapToCmd, cmdTok->value, CommandType::UNKNOWN);
    if (cmdType == CommandType::UNKNOWN)
        throw std::runtime_error("Error: invalid command \'" + cmdTok->value + "\'");

    CommandNodeSP cmd = std::make_shared<CommandNode>(cmdType);
    std::vector<ValueNodeSP> &args = cmd->args;
    tt_++;

    while (tt_ != tend_ && (*tt_)->type != TokenType::END) {
        TokenSP &t = *tt_;
        switch (t->type) {
            case TokenType::END: break;
            case TokenType::COMMAND:
                throw std::runtime_error("Error: nested commands not supported (yet?)");
            case TokenType::DELIMITER: tt_++; break;
            case TokenType::UNKNOWN:
                throw std::runtime_error("Error: unknown token \'" + t->value + "\'");
            case TokenType::NUMBER:
            case TokenType::STRING:
            case TokenType::LIST_START:
            default:
                ValueNodeSP a = std::make_shared<ValueNode>(parseValue_(t));
                if (t->type == TokenType::IDENTIIFER) a->setAsIdentifier();
                args.push_back(a);
                tt_++;
                break;
        }
    }
    return cmd;
}

StoreValueSP Parser::parseValue_(TokenSP &t) {
    std::string &tValue = t->value;
    switch (t->type) {
        case TokenType::NUMBER:
            if (strContains(tValue, '.'))
                return std::make_shared<StoreValue>(std::stof(tValue));
            else
                return std::make_shared<StoreValue>(std::stoi(tValue));
        case TokenType::IDENTIIFER:;
        case TokenType::STRING: return std::make_shared<StoreValue>(tValue);
        case TokenType::LIST_START: return parseList_();
        default: break;
    }
    return nullptr;
};

StoreValueSP Parser::parseList_() {
    // Assuming called this method upon seeing '['
    tt_++;

    std::vector<StoreValueSP> lst = std::vector<StoreValueSP>();

    while (tt_ != tend_ && (*tt_)->type != TokenType::LIST_END) {
        TokenSP &t = *tt_;
        switch (t->type) {
            case TokenType::LIST_END: break;
            case TokenType::COMMAND:
                throw std::runtime_error("Error: commands not supported within lists");
            case TokenType::DELIMITER: tt_++; break;
            case TokenType::UNKNOWN:
                throw std::runtime_error("Error: unknown token \'" + t->value + "\'");
            case TokenType::END: break;
            case TokenType::LIST_START:
            case TokenType::NUMBER:
            case TokenType::IDENTIIFER:
            case TokenType::STRING:
            default:
                lst.push_back(parseValue_(t));
                tt_++;
                break;
        }
    }
    return std::make_shared<StoreValue>(lst);
};
