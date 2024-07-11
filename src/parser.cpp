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
            tt_++;
            continue;
        }
        nodes.push_back(parseCommand_(t));
    }

    return nodes;
}

ASTNodeSP Parser::parseCommand_(TokenSP &cmdTok) {
    CommandType cmdType = mapGet(mapToCmd, cmdTok->value, CommandType::UNKNOWN);
    if (cmdType == CommandType::UNKNOWN)
        throw std::runtime_error("Error: invalid command \'" + cmdTok->value + "\'");
    tt_++;

    CommandNodeSP cmd = std::make_shared<CommandNode>(cmdType);
    while (tt_ != tend_ && (*tt_)->type != TokenType::END) {
        TokenSP &t = *tt_;
        switch (t->type) {
            case TokenType::END:
            case TokenType::LIST_END: break;
            case TokenType::DELIMITER: tt_++; break;
            case TokenType::COMMAND:
                throw std::runtime_error("Error: nested commands not supported (yet?)");
            case TokenType::UNKNOWN:
                throw std::runtime_error("Error: unknown token \'" + t->value + "\'");
            default:
                StoreValueSP val = parseValue_(t);
                if (val != nullptr) {
                    ValueNodeSP a = std::make_shared<ValueNode>(val);
                    if (t->type == TokenType::IDENTIIFER) a->setAsIdentifier();
                    cmd->addArg(a);
                };

                // Wary of where parseValue() ended up
                if (tt_ != tend_) tt_++;
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
    while (tt_ != tend_ && (*tt_)->type == TokenType::LIST_START)
        tt_++;

    std::vector<StoreValueSP> lst = std::vector<StoreValueSP>();

    while (tt_ != tend_ && (*tt_)->type != TokenType::END
           && (*tt_)->type != TokenType::LIST_END) {
        TokenSP &t = *tt_;
        switch (t->type) {
            case TokenType::END:
            case TokenType::LIST_END: break;
            case TokenType::DELIMITER: tt_++; break;
            case TokenType::COMMAND:
                throw std::runtime_error("Error: commands not supported within lists");
            case TokenType::UNKNOWN:
                throw std::runtime_error("Error: unknown token \'" + t->value + "\'");
            default:
                StoreValueSP val = parseValue_(t);
                if (val != nullptr) lst.push_back(val);

                if (tt_ != tend_) tt_++;
                break;
        }
    }
    return std::make_shared<StoreValue>(lst);
};
