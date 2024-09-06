#include "handler.h"

#include "error_msgs.h"
#include "terminal_colors.h"

#include <iostream>

static constexpr bool DEBUG = false;

std::unordered_map<CommandType, HandlerFunctionPtr> Handler::cmdToFunc_ = {
    { CommandType::SET, &Handler::handleSet_ }, { CommandType::GET, &Handler::handleGet_ },
    { CommandType::DELETE, &Handler::handleDelete_ },
    { CommandType::UPDATE, &Handler::handleUpdate_ },
    { CommandType::RESOLVE, &Handler::handleResolve_ },
    { CommandType::SAVE, &Handler::handleSave_ }, { CommandType::LOAD, &Handler::handleLoad_ },
    { CommandType::RENAME, &Handler::handleRename_ }, { CommandType::INCR, &Handler::handleIncr_ },
    { CommandType::DECR, &Handler::handleDecr_ }, { CommandType::APPEND, &Handler::handleAppend_ },
    { CommandType::PREPEND, &Handler::handlePrepend_ },
    { CommandType::SEARCH, &Handler::handleSearch_ }
};

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
        auto func = cmdToFunc_.find(cmd->getCmdType());
        if (func != cmdToFunc_.end()) {
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
                if (store_.size() < 1) std::cout << T_BYLLW << "(empty)" << T_RESET << std::endl;

                for (const auto &item : store_)
                    print_item_(item.first, item.second);
                break;
            case CommandType::STATS: handleStats_(); break;
            default: break;
        }
    }
    return true;
}

void Handler::print_item_(const std::string &id, StoreValueSP val) {
    std::cout << T_BBLUE << id << T_RESET << " | " << *val << std::endl;
}

void Handler::handleStats_() {
    std::cout << T_BYLLW << "KeplerKV Statistics" << T_RESET << std::endl;

    int totalNum = store_.size(), numInts = 0, numFloats = 0, numStrs = 0, numLists = 0,
        numAliases = 0;
    std::size_t totalMem = 0, memInts = 0, memFloats = 0, memStrs = 0, memLists = 0, memAliases = 0,
                memCurr;
    for (const auto &pair : store_) {
        totalMem += sizeof(pair.first);
        memCurr = pair.second ? pair.second->size() : 0;
        if (!memCurr) continue;
        totalMem += memCurr;

        switch (pair.second->getType()) {
            case StoreValueType::INT:
                numInts++;
                memInts += memCurr;
                break;
            case StoreValueType::FLOAT:
                numFloats++;
                memFloats += memCurr;
                break;
            case StoreValueType::STRING:
                numStrs++;
                memStrs += memCurr;
                break;
            case StoreValueType::LIST:
                numLists++;
                memLists += memCurr;
                break;
            case StoreValueType::IDENTIIFER:
                numAliases++;
                memAliases += memCurr;
                break;
            default: break;
        }
    }

    std::cout << T_BYLLW << "Total keys: " << T_RESET << totalNum << std::endl;

    std::cout << T_BYLLW << "Key Distribution by Type: " << T_RESET << std::endl;
    std::cout << "\tIntegers: " << numInts << std::endl;
    std::cout << "\tFloats: " << numFloats << std::endl;
    std::cout << "\tStrings: " << numStrs << std::endl;
    std::cout << "\tLists: " << numLists << std::endl;
    std::cout << "\tAliases: " << numAliases << std::endl;

    std::cout << T_BYLLW << "Usage (including keys) in bytes: " << T_RESET << totalMem << std::endl;
    std::cout << "\tIntegers: " << memInts << std::endl;
    std::cout << "\tFloats: " << memFloats << std::endl;
    std::cout << "\tStrings: " << memStrs << std::endl;
    std::cout << "\tLists: " << memLists << std::endl;
    std::cout << "\tAliases: " << memAliases << std::endl;
}

void Handler::handleSet_(std::vector<ValueNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 2) throw MIN_TWO_ARG_KV("SET");

    for (std::size_t i = 0; i < numArgs; i += 2) {
        if (!args[i]) continue;

        ValueNodeSP identNode = args[i];
        if (identNode->getValueType() != ValueType::IDENTIFIER) throw RuntimeErr(NOT_IDENT);
        const std::string &ident = identNode->value->getString();

        if (!(i + 1 < numArgs) || !args[i + 1]) throw RuntimeErr(VAL_AFTER_IDENT);

        ValueNodeSP valueNode = args[i + 1];

        store_.set(ident, valueNode->value);
        std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
    }
}

void Handler::handleGet_(std::vector<ValueNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 1) throw MIN_ONE_ARG_K("GET");

    for (std::size_t i = 0; i < numArgs; i++) {
        if (!args[i]) continue;

        ValueNodeSP identNode = args[i];
        if (identNode->getValueType() != ValueType::IDENTIFIER) throw RuntimeErr(NOT_IDENT);
        const std::string &ident = identNode->value->getString();

        StoreValueSP value = store_.get(ident);
        if (value)
            print_item_(ident, value);
        else
            std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
    }
}

void Handler::handleDelete_(std::vector<ValueNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 1) throw MIN_ONE_ARG_K("DELETE");

    for (std::size_t i = 0; i < numArgs; i++) {
        if (!args[i]) continue;

        ValueNodeSP identNode = args[i];
        if (identNode->getValueType() != ValueType::IDENTIFIER) throw RuntimeErr(NOT_IDENT);
        const std::string &ident = identNode->value->getString();

        bool deleted = store_.del(ident);
        if (deleted)
            std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
        else
            std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
    }
}

void Handler::handleUpdate_(std::vector<ValueNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 2) throw MIN_TWO_ARG_KV("UPDATE");

    for (std::size_t i = 0; i < numArgs; i += 2) {
        if (!args[i]) continue;

        ValueNodeSP identNode = args[i];
        if (identNode->getValueType() != ValueType::IDENTIFIER) throw RuntimeErr(NOT_IDENT);
        const std::string &ident = identNode->value->getString();

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
    if (numArgs < 1) throw MIN_ONE_ARG_K("RESOLVE");

    for (std::size_t i = 0; i < numArgs; i++) {
        if (!args[i]) continue;

        ValueNodeSP identNode = args[i];
        if (identNode->getValueType() != ValueType::IDENTIFIER) throw RuntimeErr(NOT_IDENT);
        const std::string &ident = identNode->value->getString();

        StoreValueSP value = store_.resolve(ident, true);
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
        filename = removeQuotations(fnameNode->value->getString());
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

void Handler::handleRename_(std::vector<ValueNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 2) throw MIN_TWO_ARG_KK("RENAME");

    for (std::size_t i = 0; i < numArgs; i += 2) {
        ValueNodeSP oldNameNode = args[i];
        ValueNodeSP newNameNode = args[i + 1];

        if (oldNameNode->getValueType() != ValueType::IDENTIFIER
            || newNameNode->getValueType() != ValueType::IDENTIFIER)
            throw RuntimeErr(NOT_IDENT);

        const std::string &oldName = oldNameNode->value->getString();
        const std::string &newName = newNameNode->value->getString();

        // Make the user confirm overwrites
        if (store_.contains(newName)) {
            std::cout << T_BYLLW << "Warning: key \'" << newName
                      << "\' already exists. Do you want to overwrite it? (y/n)" << T_RESET
                      << "\n> ";
            std::string confirm;
            std::getline(std::cin, confirm);
            if (0 < confirm.size() && (confirm[0]) != 'y') {
                std::cout << T_BYLLW << "No changes made to the store." << T_RESET << std::endl;
                return;
            }
        }

        store_.rename(oldName, newName);
        std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
    }
}

void Handler::handleIncr_(std::vector<ValueNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 1) throw MIN_ONE_ARG_K("INCR");

    for (std::size_t i = 0; i < numArgs; i++) {
        if (!args[i]) continue;

        ValueNodeSP identNode = args[i];
        if (identNode->getValueType() != ValueType::IDENTIFIER) throw RuntimeErr(NOT_IDENT);
        const std::string &ident = identNode->value->getString();

        StoreValueSP value = store_.resolve(ident);
        if (value == nullptr) {
            std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
            continue;
        }

        if (value->incr())
            std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
        else
            std::cout << T_BRED << NOT_NUMERIC << T_RESET << std::endl;
    }
}

void Handler::handleDecr_(std::vector<ValueNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 1) throw MIN_ONE_ARG_K("DECR");

    for (std::size_t i = 0; i < numArgs; i++) {
        if (!args[i]) continue;

        ValueNodeSP identNode = args[i];
        if (identNode->getValueType() != ValueType::IDENTIFIER) throw RuntimeErr(NOT_IDENT);
        const std::string &ident = identNode->value->getString();

        StoreValueSP value = store_.resolve(ident);
        if (value == nullptr) {
            std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
            continue;
        }

        if (value->decr())
            std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
        else
            std::cout << T_BRED << NOT_NUMERIC << T_RESET << std::endl;
    }
}

void Handler::handleAppend_(std::vector<ValueNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 2) throw MIN_TWO_ARG_KV("APPEND");

    // First must be an identifier to a list
    ValueNodeSP identNode = args[0];
    if (identNode->getValueType() != ValueType::IDENTIFIER) throw RuntimeErr(NOT_IDENT);
    const std::string &ident = identNode->value->getString();
    StoreValueSP list = store_.resolve(ident);
    if (list == nullptr) {
        std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
        return;
    }

    for (std::size_t i = 1; i < numArgs; i++) {
        if (!args[i]) continue;

        ValueNodeSP valueNode = args[i];
        if (list->append(valueNode->value))
            std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
        else
            std::cout << T_BRED << UNEXPECTED << T_RESET << std::endl;
    }
}

void Handler::handlePrepend_(std::vector<ValueNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 2) throw MIN_TWO_ARG_KV("PREPEND");

    // First must be an identifier to a list
    ValueNodeSP identNode = args[0];
    if (identNode->getValueType() != ValueType::IDENTIFIER) throw RuntimeErr(NOT_IDENT);
    const std::string &ident = identNode->value->getString();
    StoreValueSP list = store_.resolve(ident);
    if (list == nullptr) {
        std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
        return;
    }

    for (std::size_t i = 1; i < numArgs; i++) {
        if (!args[i]) continue;

        ValueNodeSP valueNode = args[i];
        if (list->prepend(valueNode->value))
            std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
        else
            std::cout << T_BRED << UNEXPECTED << T_RESET << std::endl;
    }
}

void Handler::handleSearch_(std::vector<ValueNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 1) throw RuntimeErr("Error: SEARCH requires at least one argument (pattern)");

    for (std::size_t i = 0; i < numArgs; i++) {
        if (!args[i]) continue;

        ValueNodeSP patNode = args[i];
        if (patNode->getValueType() != ValueType::IDENTIFIER
            && patNode->getValueType() != ValueType::STRING)
            throw RuntimeErr(WRONG_FMT);

        std::string pattern = removeQuotations(patNode->value->getString());
        std::vector<std::string> keys = store_.search(pattern);

        std::cout << T_BYLLW << pattern << " (" << keys.size() << ")" << T_RESET << std::endl;
        for (const auto &key : keys)
            std::cout << "  " << key << std::endl;
    }
}
