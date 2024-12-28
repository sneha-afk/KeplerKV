#include "parser.h"

#include "command_ast_nodes.h"
#include "error_msgs.h"

std::vector<CommandSP> &Parser::parse(std::vector<TokenSP> &tokens) {
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

CommandSP Parser::parseCommand_(const TokenSP &cmdTok) {
    CommandType cmdType = mapGet(mapToCmd, cmdTok->value, CommandType::UNKNOWN);
    if (cmdType == CommandType::UNKNOWN) throw INVALID_CMD(cmdTok->value);
    tt_++;

    CommandSP cmd;
    switch (cmdType) {
        case CommandType::QUIT: cmd = std::make_shared<QuitCommand>(); break;
        case CommandType::CLEAR: cmd = std::make_shared<ClearCommand>(); break;
        case CommandType::SET: cmd = std::make_shared<SetCommand>(); break;
        case CommandType::GET: cmd = std::make_shared<GetCommand>(); break;
        case CommandType::LIST: cmd = std::make_shared<ListCommand>(); break;
        case CommandType::DELETE: cmd = std::make_shared<DeleteCommand>(); break;
        case CommandType::UPDATE: cmd = std::make_shared<UpdateCommand>(); break;
        case CommandType::RESOLVE: cmd = std::make_shared<ResolveCommand>(); break;
        case CommandType::SAVE: cmd = std::make_shared<SaveCommand>(); break;
        case CommandType::LOAD: cmd = std::make_shared<LoadCommand>(); break;
        case CommandType::RENAME: cmd = std::make_shared<RenameCommand>(); break;
        case CommandType::INCR: cmd = std::make_shared<IncrementCommand>(); break;
        case CommandType::DECR: cmd = std::make_shared<DecrementCommand>(); break;
        case CommandType::APPEND: cmd = std::make_shared<AppendCommand>(); break;
        case CommandType::PREPEND: cmd = std::make_shared<PrependCommand>(); break;
        case CommandType::SEARCH: cmd = std::make_shared<SearchCommand>(); break;
        case CommandType::STATS: cmd = std::make_shared<StatsCommand>(); break;
        case CommandType::BEGIN: cmd = std::make_shared<BeginCommand>(); break;
        case CommandType::COMMIT: cmd = std::make_shared<CommitCommand>(); break;
        case CommandType::ROLLBACK: cmd = std::make_shared<RollbackCommand>(); break;
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
                ValueSP val = parseValue_(t);
                if (val != nullptr) cmd->addArg(val);

                // Wary of where parseValue() ended up
                if (tt_ != tend_) tt_++;
                break;
        }
    }
    return cmd;
}

ValueSP Parser::parseValue_(const TokenSP &t) {
    std::string &tValue = t->value;
    switch (t->type) {
        case TokenType::NUMBER:
            if (strContains(tValue, '.')) {
                try {
                    return std::make_shared<FloatNode>(std::stof(tValue));
                } catch (Exception &e) {
                    throw RuntimeErr(WRONG_F_FMT);
                }
            } else {
                try {
                    return std::make_shared<IntNode>(std::stoi(tValue));
                } catch (Exception &e) {
                    throw RuntimeErr(WRONG_I_FMT);
                }
            }
        case TokenType::IDENTIFIER: return std::make_shared<IdentifierNode>(tValue);
        case TokenType::STRING: return std::make_shared<StringNode>(tValue);
        case TokenType::LIST_START: tt_++; return parseList_();
        default: throw UNKNOWN_TOKEN(t->value); break;
    }
    return nullptr;
}

ValueSP Parser::parseList_() {
    std::shared_ptr<ListNode> lstNode = std::make_shared<ListNode>();

    while (tt_ != tend_ && (*tt_)->type != TokenType::END && (*tt_)->type != TokenType::LIST_END) {
        TokenSP &t = *tt_;
        switch (t->type) {
            case TokenType::END:
            case TokenType::LIST_END: break;
            case TokenType::DELIMITER: tt_++; break;
            case TokenType::COMMAND: throw RuntimeErr(CMD_IN_LIST);
            case TokenType::UNKNOWN: throw UNKNOWN_TOKEN(t->value);
            default:
                ValueSP val = parseValue_(t);
                if (val != nullptr) lstNode->addNode(val);

                if (tt_ != tend_) tt_++;
                break;
        }
    }
    return lstNode;
}
