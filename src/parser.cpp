#include "parser.h"

#include "command_ast_nodes.h"
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

CommandASTNodeSP Parser::parseCommand_(const TokenSP &cmdTok) {
    CommandType cmdType = mapGet(mapToCmd, cmdTok->value, CommandType::UNKNOWN);
    if (cmdType == CommandType::UNKNOWN) throw INVALID_CMD(cmdTok->value);
    tt_++;

    CommandASTNodeSP cmd;
    switch (cmdType) {
        case CommandType::QUIT: cmd = std::make_shared<QuitCmdASTNode>(); break;
        case CommandType::CLEAR: cmd = std::make_shared<ClearCmdASTNode>(); break;
        case CommandType::SET: cmd = std::make_shared<SetCmdASTNode>(); break;
        case CommandType::GET: cmd = std::make_shared<GetCmdASTNode>(); break;
        case CommandType::LIST: cmd = std::make_shared<ListCmdASTNode>(); break;
        case CommandType::DELETE: cmd = std::make_shared<DeleteCmdASTNode>(); break;
        case CommandType::UPDATE: cmd = std::make_shared<UpdateCmdASTNode>(); break;
        case CommandType::RESOLVE: cmd = std::make_shared<ResolveCmdASTNode>(); break;
        case CommandType::SAVE: cmd = std::make_shared<SaveCmdASTNode>(); break;
        case CommandType::LOAD: cmd = std::make_shared<LoadCmdASTNode>(); break;
        case CommandType::RENAME: cmd = std::make_shared<RenameCmdASTNode>(); break;
        case CommandType::INCR: cmd = std::make_shared<IncrCmdASTNode>(); break;
        case CommandType::DECR: cmd = std::make_shared<DecrCmdASTNode>(); break;
        case CommandType::APPEND: cmd = std::make_shared<AppendCmdASTNode>(); break;
        case CommandType::PREPEND: cmd = std::make_shared<PrependCmdASTNode>(); break;
        case CommandType::SEARCH: cmd = std::make_shared<SearchCmdASTNode>(); break;
        case CommandType::STATS: cmd = std::make_shared<StatsCmdASTNode>(); break;
        case CommandType::BEGIN: cmd = std::make_shared<BeginCmdASTNode>(); break;
        case CommandType::COMMIT: cmd = std::make_shared<CommitCmdASTNode>(); break;
        case CommandType::ROLLBACK: cmd = std::make_shared<RollbackCmdASTNode>(); break;
        default: return nullptr; break;
    }

    while (tt_ != tend_ && (*tt_)->type != TokenType::END) {
        TokenSP &t = *tt_;
        switch (t->type) {
            case TokenType::END:
            case TokenType::LIST_END:
            case TokenType::DELIMITER: tt_++; break;
            case TokenType::COMMAND: throw RuntimeErr(NESTED_CMD);
            case TokenType::UNKNOWN: throw UNKNOWN_TOKEN(t->value);
            default:
                ValueASTNodeSP val = parseValue_(t);
                if (val != nullptr) cmd->addArg(val);

                // Wary of where parseValue() ended up
                if (tt_ != tend_) tt_++;
                break;
        }
    }
    return cmd;
}

ValueASTNodeSP Parser::parseValue_(const TokenSP &t) {
    std::string &tValue = t->value;
    switch (t->type) {
        case TokenType::NUMBER:
            if (strContains(tValue, '.')) {
                try {
                    return std::make_shared<FloatASTNode>(std::stof(tValue));
                } catch (Exception &e) {
                    throw RuntimeErr(WRONG_F_FMT);
                }
            } else {
                try {
                    return std::make_shared<IntASTNode>(std::stoi(tValue));
                } catch (Exception &e) {
                    throw RuntimeErr(WRONG_I_FMT);
                }
            }
        case TokenType::IDENTIFIER: return std::make_shared<IdentifierASTNode>(tValue);
        case TokenType::STRING: return std::make_shared<StringASTNode>(tValue);
        case TokenType::LIST_START: tt_++; return parseList_();
        default: throw UNKNOWN_TOKEN(t->value); break;
    }
    return nullptr;
}

ValueASTNodeSP Parser::parseList_() {
    std::shared_ptr<ListValueASTNode> lstNode = std::make_shared<ListValueASTNode>();

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
