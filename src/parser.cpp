#include "parser.h"

#include "error_msgs.h"
#include "util.h"

std::vector<ASTNodeSP> &Parser::parse(std::vector<TokenSP> &tokens) {
    nodes.clear();
    if (tokens.empty()) return nodes;

    tt_ = tokens.begin();
    tend_ = tokens.end();

    // First token must be a command
    if ((*tt_)->type != TokenType::COMMAND) throw INVALID_CMD((*tt_)->value);

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

CommandNodeSP Parser::parseCommand_(TokenSP &cmdTok) {
    CommandType cmdType = mapGet(mapToCmd, cmdTok->value, CommandType::UNKNOWN);
    if (cmdType == CommandType::UNKNOWN) throw INVALID_CMD(cmdTok->value);
    tt_++;

    CommandNodeSP cmd = std::make_shared<CommandASTNode>(cmdType);
    while (tt_ != tend_ && (*tt_)->type != TokenType::END) {
        TokenSP &t = *tt_;
        switch (t->type) {
            case TokenType::END:
            case TokenType::LIST_END:
            case TokenType::DELIMITER: tt_++; break;
            case TokenType::COMMAND: throw RuntimeErr(NESTED_CMD);
            case TokenType::UNKNOWN: throw UNKNOWN_TOKEN(t->value);
            default:
                ASTNodeSP val = parseValue_(t);
                if (val != nullptr) cmd->addArg(val);

                // Wary of where parseValue() ended up
                if (tt_ != tend_) tt_++;
                break;
        }
    }
    return cmd;
}

ValueASTNodeSP Parser::parseValue_(TokenSP &t) {
    std::string &tValue = t->value;
    switch (t->type) {
        case TokenType::NUMBER:
            if (strContains(tValue, '.'))
                return std::make_shared<FloatASTNode>(std::stof(tValue));
            else
                return std::make_shared<IntASTNode>(std::stoi(tValue));
        case TokenType::IDENTIIFER: return std::make_shared<IdentifierASTNode>(tValue);
        case TokenType::STRING: return std::make_shared<StringASTNode>(tValue);
        case TokenType::LIST_START: tt_++; return parseList_();
        default: break;
    }
    return nullptr;
}

ValueASTNodeSP Parser::parseList_() {
    std::shared_ptr<ListASTNode> lstNode = std::make_shared<ListASTNode>();

    while (tt_ != tend_ && (*tt_)->type != TokenType::END && (*tt_)->type != TokenType::LIST_END) {
        TokenSP &t = *tt_;
        switch (t->type) {
            case TokenType::END:
            case TokenType::LIST_END: break;
            case TokenType::DELIMITER: tt_++; break;
            case TokenType::COMMAND: throw RuntimeErr(CMD_IN_LIST);
            case TokenType::UNKNOWN: throw UNKNOWN_TOKEN(t->value);
            default:
                ValueASTNodeSP val = parseValue_(t);
                if (val != nullptr) lstNode->addNode(val);

                if (tt_ != tend_) tt_++;
                break;
        }
    }
    return lstNode;
}
