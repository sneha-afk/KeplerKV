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

    std::vector<ASTNodeSP> &nodes = parser_.parse(tokens);
    for (const auto &n : nodes) {
        std::cout << "\t" << *n << std::endl;

        // https://stackoverflow.com/a/14545746
        CommandNodeSP cmd = std::dynamic_pointer_cast<CommandNode>(n);
        if (cmd == nullptr) throw WRONG_FMT;

        std::vector<ValueNodeSP> &args = cmd->getArgs();
        const std::size_t numArgs = args.size();
        switch (cmd->getCmdType()) {
            case CommandType::SET:
                if (numArgs < 2)
                    throw std::runtime_error(
                        "Error: SET requires at least two arguments (key value)");

                for (int i = 0; i < numArgs; i += 2) {
                    if (!args[i]) continue;

                    ValueNodeSP identNode = args[i];
                    if (identNode->getValueType() != ValueType::IDENTIFIER)
                        throw NOT_IDENT;
                    std::string ident = identNode->value->getString();

                    if (!(i + 1 < numArgs) || !args[i + 1])
                        throw std::runtime_error(
                            "Error: expected value after identifier");

                    ValueNodeSP valueNode = args[i + 1];

                    // todo: send set() instructions to store
                    store_.set(ident, valueNode->value);
                    std::cout << *(store_.get(ident)) << std::endl;
                }
                break;
            case CommandType::GET:
                if (numArgs < 1)
                    throw std::runtime_error(
                        "Error: GET requires at least one argument (key)");

                for (int i = 0; i < numArgs; i++) {
                    if (!args[i]) continue;

                    ValueNodeSP identNode = args[i];
                    if (identNode->getValueType() != ValueType::IDENTIFIER)
                        throw NOT_IDENT;
                    std::string ident = identNode->value->getString();

                    // todo: send get() instructions to store
                }
                break;
            case CommandType::DELETE:
                if (numArgs < 1)
                    throw std::runtime_error(
                        "Error: DELETE requires at least one argument (key)");

                for (int i = 0; i < numArgs; i++) {
                    if (!args[i]) continue;

                    ValueNodeSP identNode = args[i];
                    if (identNode->getValueType() != ValueType::IDENTIFIER)
                        throw NOT_IDENT;
                    std::string ident = identNode->value->getString();

                    // todo: send del() instructions to store
                }
                break;
            case CommandType::UPDATE:
                if (numArgs < 2)
                    throw std::runtime_error(
                        "Error: UPDATE requires at least two arguments (key value)");

                for (int i = 0; i < numArgs; i += 2) {
                    if (!args[i]) continue;

                    ValueNodeSP identNode = args[i];
                    if (identNode->getValueType() != ValueType::IDENTIFIER)
                        throw NOT_IDENT;
                    std::string ident = identNode->value->getString();

                    if (!(i + 1 < numArgs) || !args[i + 1])
                        throw std::runtime_error(
                            "Error: expected value after identifier");

                    ValueNodeSP value = args[i + 1];

                    // todo: send update() instructions to store
                }
                break;
            default: break;
        }
    }
}
