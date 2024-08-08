#include "handler.h"

#include "error_msgs.h"
#include "terminal_colors.h"

#include <iostream>

static constexpr bool DEBUG = false;

std::unordered_map<CommandType, HandlerFunctionPtr> Handler::CommandToFunction
    = { { CommandType::SET, &Handler::handleSet_ },
          { CommandType::GET, &Handler::handleGet_ },
          { CommandType::DELETE, &Handler::handleDelete_ },
          { CommandType::UPDATE, &Handler::handleUpdate_ },
          { CommandType::RESOLVE, &Handler::handleResolve_ },
          { CommandType::SAVE, &Handler::handleSave_ },
          { CommandType::LOAD, &Handler::handleLoad_ } };

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

        // https://stackoverflow.com/a/14545746
        CommandNodeSP cmd = std::dynamic_pointer_cast<CommandNode>(n);
        if (cmd == nullptr) throw RuntimeErr(WRONG_FMT);

        // Retrieve appropriate function pointer and run
        auto func = CommandToFunction.find(cmd->getCmdType());
        if (func != CommandToFunction.end()) {
            std::vector<ValueNodeSP> &args = cmd->getArgs();
            const size_t numArgs = args.size();
            func->second(this, args, numArgs);
            return true;
        }

        // If not in the map, then it is one of these
        switch (cmd->getCmdType()) {
            case CommandType::QUIT:
                std::cout << T_BBLUE << "Farewell!" << T_RESET << std::endl;
                return false;
            case CommandType::CLEAR: std::cout << "\033[H\033[2J" << std::endl; break;
            case CommandType::LIST:
                for (const auto &item : store_)
                    print_item_(item.first, item.second);
                break;
            default: break;
        }
    }
    return true;
}

void Handler::print_item_(const std::string &id, StoreValueSP val) {
    std::cout << T_BBLUE << id << T_RESET << " | " << *val << std::endl;
}

void Handler::handleSet_(std::vector<ValueNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 2) throw MIN_TWO_ARG("SET");

    for (std::size_t i = 0; i < numArgs; i += 2) {
        if (!args[i]) continue;

        ValueNodeSP identNode = args[i];
        if (identNode->getValueType() != ValueType::IDENTIFIER)
            throw RuntimeErr(NOT_IDENT);
        std::string ident = identNode->value->getString();

        if (!(i + 1 < numArgs) || !args[i + 1]) throw RuntimeErr(VAL_AFTER_IDENT);

        ValueNodeSP valueNode = args[i + 1];

        store_.set(ident, valueNode->value);
        std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
    }
}

void Handler::handleGet_(std::vector<ValueNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 1) throw MIN_ONE_ARG("GET");

    for (std::size_t i = 0; i < numArgs; i++) {
        if (!args[i]) continue;

        ValueNodeSP identNode = args[i];
        if (identNode->getValueType() != ValueType::IDENTIFIER)
            throw RuntimeErr(NOT_IDENT);
        std::string ident = identNode->value->getString();

        StoreValueSP value = store_.get(ident);
        if (value)
            print_item_(ident, value);
        else
            std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
    }
}

void Handler::handleDelete_(std::vector<ValueNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 1) throw MIN_ONE_ARG("DELETE");

    for (std::size_t i = 0; i < numArgs; i++) {
        if (!args[i]) continue;

        ValueNodeSP identNode = args[i];
        if (identNode->getValueType() != ValueType::IDENTIFIER)
            throw RuntimeErr(NOT_IDENT);
        std::string ident = identNode->value->getString();

        bool deleted = store_.del(ident);
        if (deleted)
            std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
        else
            std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
    }
}

void Handler::handleUpdate_(std::vector<ValueNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 2) throw MIN_TWO_ARG("UPDATE");

    for (std::size_t i = 0; i < numArgs; i += 2) {
        if (!args[i]) continue;

        ValueNodeSP identNode = args[i];
        if (identNode->getValueType() != ValueType::IDENTIFIER)
            throw RuntimeErr(NOT_IDENT);
        std::string ident = identNode->value->getString();

        if (!(i + 1 < numArgs) || !args[i + 1]) throw RuntimeErr(VAL_AFTER_IDENT);

        ValueNodeSP valueNode = args[i + 1];

        bool updated = store_.update(ident, valueNode->value);
        if (updated)
            std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
        else
            std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
    }
}

void Handler::handleResolve_(std::vector<ValueNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 1) throw MIN_ONE_ARG("RESOLVE");

    for (std::size_t i = 0; i < numArgs; i++) {
        if (!args[i]) continue;

        ValueNodeSP identNode = args[i];
        if (identNode->getValueType() != ValueType::IDENTIFIER)
            throw RuntimeErr(NOT_IDENT);
        std::string ident = identNode->value->getString();

        StoreValueSP value = store_.resolve(ident);
        if (value)
            print_item_(ident, value);
        else
            std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
    }
}

std::string &Handler::getFilename_(ValueNodeSP fnameNode) {
    std::string &filename = DEFAULT_SAVE_FILE;
    if (fnameNode->getValueType() == ValueType::IDENTIFIER)
        filename = fnameNode->value->getString();
    else if (fnameNode->getValueType() == ValueType::STRING) {
        filename = fnameNode->value->getString();

        // Remove the quotation marks
        size_t first_quote = filename.find_first_of('"');
        if (first_quote != std::string::npos) {
            filename.erase(filename.find_last_of('"'), 1);
        } else {
            first_quote = filename.find_first_of('\'');
            filename.erase(filename.find_last_of('\''), 1);
        }
        filename.erase(first_quote, 1);
    } else
        throw RuntimeErr(INVALID_FNAME);

    return filename;
}

void Handler::handleSave_(std::vector<ValueNodeSP> &args, const std::size_t numArgs) {
    std::string &filename = DEFAULT_SAVE_FILE;
    if (numArgs) filename = getFilename_(args[0]);

    store_.saveToFile(filename + ".kep");
    std::cout << T_BGREEN << "SAVED" << T_RESET << std::endl;
}

void Handler::handleLoad_(std::vector<ValueNodeSP> &args, const std::size_t numArgs) {
    std::string &filename = DEFAULT_SAVE_FILE;
    if (numArgs) filename = getFilename_(args[0]);

    store_.loadFromFile(filename + ".kep");
    std::cout << T_BGREEN << "LOADED" << T_RESET << std::endl;
}
