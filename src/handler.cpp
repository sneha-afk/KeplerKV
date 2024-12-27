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
void Handler::handleQuery(std::string &query) {
    std::vector<std::shared_ptr<Token>> &tokens = lexer_.tokenize(query);
    if (DEBUG)
        for (const auto &t : tokens)
            std::cout << "\t" << *t << std::endl;

    std::vector<CommandSP> &nodes = parser_.parse(tokens);
    for (const CommandSP &cmd : nodes) {
        if (DEBUG) std::cout << "\t" << *cmd << std::endl;

        // Validate the command
        if (!cmd || !cmd->validate()) throw RuntimeErr(WRONG_CMD_FMT);

        // Check what kind of command this is
        if (SystemCommandSP sysCmd = std::dynamic_pointer_cast<SystemCommand>(cmd)) {
            sysCmd->execute(env_);
        } else if (StoreCommandSP storeCmd = std::dynamic_pointer_cast<StoreCommand>(cmd)) {
            if (env_.inTransaction()) {
                env_.addCommand(storeCmd);
                env_.printToConsole(T_BYLLW "LOGGED" T_RESET);
            } else {
                storeCmd->execute(store_);
            }
        }
    }

    env_.setRunning(true);
}
