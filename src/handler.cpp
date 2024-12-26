#include "handler.h"

#include "error_msgs.h"
#include "file_io_macros.h"
#include "terminal_colors.h"

#include <iostream>

static constexpr bool DEBUG = false;

bool inTransaction_ = false;

/**
 * Processes a query and hands it off to the store to execute.
 * Returns whether to keep running the program.
 */
bool Handler::handleQuery(std::string &query) {
    std::vector<std::shared_ptr<Token>> &tokens = lexer_.tokenize(query);
    if (DEBUG)
        for (const auto &t : tokens)
            std::cout << "\t" << *t << std::endl;

    std::vector<CommandASTNodeSP> &nodes = parser_.parse(tokens);
    for (const CommandASTNodeSP &cmd : nodes) {
        if (DEBUG) std::cout << "\t" << *cmd << std::endl;

        // Validate the command
        if (!cmd || !cmd->validate()) throw RuntimeErr(WRONG_CMD_FMT);

        // Check what kind of command this is
        if (SystemCmdASTNodeSP sysCmd = std::dynamic_pointer_cast<SystemCmdASTNode>(cmd)) {
            sysCmd->execute();
        } else if (StoreCmdASTNodeSP storeCmd = std::dynamic_pointer_cast<StoreCmdASTNode>(cmd)) {
            switch (storeCmd->getCmdType()) {
                case CommandType::BEGIN:
                    inTransaction_ = true;
                    storeCmd->execute(store_);
                    break;
                case CommandType::COMMIT:
                    inTransaction_ = false;
                    while (0 < wal_.size()) {
                        StoreCmdASTNodeSP frontCmd = wal_.front();
                        wal_.pop_front();
                        frontCmd->execute(store_);
                    }
                    storeCmd->execute(store_);
                    break;
                case CommandType::ROLLBACK:
                    inTransaction_ = false;
                    wal_.clear();
                    storeCmd->execute(store_);
                    break;
                default:
                    if (inTransaction_) {
                        wal_.push_back(storeCmd);
                        std::cout << T_BYLLW << "LOGGED" << T_RESET << std::endl;
                    } else {
                        storeCmd->execute(store_);
                    }
                    break;
            }

            break;
        }
    }
    return true;
}
