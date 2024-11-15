#include "handler.h"

#include "error_msgs.h"
#include "file_io_macros.h"
#include "terminal_colors.h"

#include <iostream>

static constexpr bool DEBUG = false;

/**
 * Proccesses a query and hands it off to the store to execute.
 * Returns whether to keep running the program.
 */
bool Handler::handleQuery(std::string &query) {
    std::vector<std::shared_ptr<Token>> &tokens = lexer_.tokenize(query);
    if (DEBUG)
        for (const auto &t : tokens)
            std::cout << "\t" << *t << std::endl;

    std::vector<ASTNodeSP> &nodes = parser_.parse(tokens);
    for (const auto &n : nodes) {
        if (DEBUG) std::cout << "\t" << *n << std::endl;

        CommandASTNodeSP cmd = std::dynamic_pointer_cast<CommandASTNode>(n);
        if (!cmd) throw RuntimeErr(WRONG_CMD_FMT);

        // Check what kind of command this is
        SystemCmdASTNodeSP sysCmd = std::dynamic_pointer_cast<SystemCmdASTNode>(cmd);
        if (sysCmd) {
            if (!sysCmd->validate()) throw RuntimeErr(WRONG_CMD_FMT);
            sysCmd->execute();
            break;
        } else {
            StoreCmdASTNodeSP storeCmd = std::dynamic_pointer_cast<StoreCmdASTNode>(cmd);

            if (!storeCmd->validate()) throw RuntimeErr(WRONG_CMD_FMT);
            storeCmd->execute(store_);
            break;
        }
    }
    return true;
}
