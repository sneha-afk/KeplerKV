#include "handler.h"

#include <iostream>

static const std::runtime_error WRONG_FMT
    = std::runtime_error("Error: incorrect command format");
static const std::runtime_error NOT_IDENT
    = std::runtime_error("Error: expected identifier");

void Handler::handleQuery(std::string &query) {
    std::vector<std::shared_ptr<Token>> &tokens = lexer_.tokenize(query);
    for (const auto &t : tokens) {
        std::cout << "\t" << *t << std::endl;
    }

    std::vector<std::shared_ptr<SyntaxNode>> &nodes = parser_.parse(tokens);
    for (const auto &n : nodes) {
        std::cout << "\t" << *n << std::endl;

        // https://stackoverflow.com/a/14545746
        std::shared_ptr<CommandNode> cmd = std::dynamic_pointer_cast<CommandNode>(n);
        if (cmd == nullptr) throw WRONG_FMT;

        std::vector<std::shared_ptr<SyntaxNode>> &args = cmd->args;
        const std::size_t numArgs = args.size();
        switch (cmd->cmdType) {
            case CommandType::SET:
                if (numArgs < 2)
                    throw std::runtime_error(
                        "Error: SET requires at least two arguments (key value)");

                for (int i = 0; i < numArgs; i += 2) {
                    if (!args[i]) continue;
                    std::shared_ptr<IdentifierNode> ident
                        = std::dynamic_pointer_cast<IdentifierNode>(args[i]);
                    if (ident == nullptr) throw NOT_IDENT;

                    std::shared_ptr<ValueNode> value
                        = std::dynamic_pointer_cast<ValueNode>(args[i + 1]);
                    if (value == nullptr) throw WRONG_FMT;

                    // todo: send set() instructions to store
                }
                break;
            case CommandType::GET:
                if (numArgs < 1)
                    throw std::runtime_error(
                        "Error: GET requires at least one argument (key)");

                for (int i = 0; i < numArgs; i++) {
                    std::shared_ptr<IdentifierNode> ident
                        = std::dynamic_pointer_cast<IdentifierNode>(args[i]);
                    if (ident == nullptr) throw NOT_IDENT;

                    // todo: send get() instructions to store
                }
                break;
            case CommandType::DELETE:
                if (numArgs < 1)
                    throw std::runtime_error(
                        "Error: DELETE requires at least one argument (key)");

                for (int i = 0; i < numArgs; i++) {
                    std::shared_ptr<IdentifierNode> ident
                        = std::dynamic_pointer_cast<IdentifierNode>(args[i]);
                    if (ident == nullptr) throw NOT_IDENT;

                    // todo: send del() instructions to store
                }
                break;
            case CommandType::UPDATE:
                if (numArgs < 2)
                    throw std::runtime_error(
                        "Error: UPDATE requires at least two arguments (key value)");

                for (int i = 0; i < numArgs; i += 2) {
                    std::shared_ptr<IdentifierNode> ident
                        = std::dynamic_pointer_cast<IdentifierNode>(args[i]);
                    if (ident == nullptr) throw NOT_IDENT;

                    std::shared_ptr<ValueNode> value
                        = std::dynamic_pointer_cast<ValueNode>(args[i + 1]);
                    if (value == nullptr) throw WRONG_FMT;

                    // todo: send update() instructions to store
                }
                break;
            default: break;
        }
    }
}
