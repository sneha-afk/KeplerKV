#include "handler.h"

#include "error_msgs.h"
#include "file_io_macros.h"
#include "terminal_colors.h"

#include <iostream>

static constexpr bool DEBUG = false;

/**
 * Processes a query and hands it off to the store to execute.
 * Returns whether to keep running the program.
 */
void Handler::handleQuery(std::string &query) {
    std::vector<std::shared_ptr<Token>> &tokens = lexer_.tokenize(query);
    if (DEBUG)
        for (const TokenSP &t : tokens)
            env_->printToConsole("\t" + t->string());

    std::vector<CommandSP> &nodes = parser_.parse(tokens);
    for (const CommandSP &cmd : nodes) {
        if (DEBUG) env_->printToConsole("\t" + cmd->string());

        // Validate the command
        if (!cmd || !cmd->validate()) throw RuntimeErr(WRONG_CMD_FMT);

        // Check what kind of command this is
        if (SystemCommandSP sysCmd = std::dynamic_pointer_cast<SystemCommand>(cmd)) {
            sysCmd->execute(*env_);
        } else if (StoreCommandSP storeCmd = std::dynamic_pointer_cast<StoreCommand>(cmd)) {
            if (!storeCmd->ignoresTransactions() && env_->inTransaction()) {
                env_->addCommand(storeCmd);
                env_->printToConsole(PRINT_YELLOW("LOGGED"));
            } else {
                storeCmd->execute(*env_, *store_);
            }
        }
    }

    env_->setRunning(true);
}
