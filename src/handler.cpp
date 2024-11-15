#include "handler.h"

#include "error_msgs.h"
#include "file_io_macros.h"
#include "terminal_colors.h"

#include <iostream>

static constexpr bool DEBUG = false;

std::unordered_map<CommandType, HandlerFunctionPtr> Handler::cmdToFunc_ = {
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

        CommandASTNodeSP cmd = std::dynamic_pointer_cast<CommandASTNode>(n);
        if (!cmd) throw RuntimeErr(WRONG_CMD_FMT);

        // Check what kind of command this is
        SystemCmdASTNodeSP sysCmd = std::dynamic_pointer_cast<SystemCmdASTNode>(cmd);
        if (sysCmd) {
            switch (sysCmd->getCmdType()) {
                case CommandType::STATS: handleStats_(); break;
                default:
                    if (!sysCmd->validate()) throw RuntimeErr(WRONG_CMD_FMT);
                    sysCmd->execute();
                    break;
            }
        } else {
            StoreCmdASTNodeSP storeCmd = std::dynamic_pointer_cast<StoreCmdASTNode>(cmd);

            if (!storeCmd->validate()) throw RuntimeErr(WRONG_CMD_FMT);
            storeCmd->execute(store_);
            break;
        }
    }
    return true;
}

void Handler::print_item_(const std::string &id, const StoreValueSP val) {
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

        switch (pair.second->getValueType()) {
            case ValueType::INT:
                numInts++;
                memInts += memCurr;
                break;
            case ValueType::FLOAT:
                numFloats++;
                memFloats += memCurr;
                break;
            case ValueType::STRING:
                numStrs++;
                memStrs += memCurr;
                break;
            case ValueType::LIST:
                numLists++;
                memLists += memCurr;
                break;
            case ValueType::IDENTIIFER:
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

void Handler::handleResolve_(std::vector<ValueASTNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 1) throw MIN_ONE_ARG_K("RESOLVE");

    for (const auto &arg : args) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        if (!idNode) throw RuntimeErr(NOT_IDENT);
        const std::string &ident = idNode->getValue();

        StoreValueSP value = store_.resolve(ident, true);
        if (value)
            print_item_(ident, value);
        else
            std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
    }
}

std::string Handler::getFilename_(const ValueASTNodeSP node) {
    std::string &filename = DEFAULT_SAVE_FILE;

    StringValueSP fnNode = std::dynamic_pointer_cast<StringValue>(node);
    if (!fnNode) throw RuntimeErr(INVALID_FNAME);
    filename = fnNode->getValue();

    return fnNode->getValueType() == ValueType::IDENTIIFER ? filename : removeQuotations(filename);
}

void Handler::handleSave_(std::vector<ValueASTNodeSP> &args, const std::size_t numArgs) {
    std::string &filename = DEFAULT_SAVE_FILE;
    if (numArgs) filename = getFilename_(args[0]);

    store_.saveToFile(filename + ".kep");
    std::cout << T_BGREEN << "SAVED" << T_RESET << std::endl;
}

void Handler::handleLoad_(std::vector<ValueASTNodeSP> &args, const std::size_t numArgs) {
    std::string &filename = DEFAULT_SAVE_FILE;
    if (numArgs) filename = getFilename_(args[0]);

    store_.loadFromFile(filename + ".kep");
    std::cout << T_BGREEN << "LOADED" << T_RESET << std::endl;
}

void Handler::handleRename_(std::vector<ValueASTNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 2) throw MIN_TWO_ARG_KK("RENAME");

    for (std::size_t i = 0; i < numArgs; i += 2) {
        if (!(i + 1 < numArgs)) throw MIN_TWO_ARG_KK("RENAME");

        IdentifierValueSP oldNode
            = std::dynamic_pointer_cast<IdentifierValue>((args[i])->evaluate());
        IdentifierValueSP newNode
            = std::dynamic_pointer_cast<IdentifierValue>((args[i + 1])->evaluate());
        if (!oldNode || !newNode) throw RuntimeErr(NOT_IDENT);

        const std::string &oldName = oldNode->getValue();
        const std::string &newName = newNode->getValue();

        // Make the user confirm overwrites
        if (store_.contains(newName)) {
            std::cout << T_BYLLW << "Warning: key \'" << newName
                      << "\' already exists. Do you want to overwrite it? (y/n)" << T_RESET
                      << "\n> ";
            std::string confirm;
            std::getline(std::cin, confirm);
            if (confirm.size() && confirm[0] != 'y') {
                std::cout << T_BYLLW << "No changes made to the store." << T_RESET << std::endl;
                return;
            }
        }

        store_.rename(oldName, newName);
        std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
    }
}

void Handler::handleIncr_(std::vector<ValueASTNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 1) throw MIN_ONE_ARG_K("INCR");

    for (const auto &arg : args) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        if (!idNode) throw RuntimeErr(NOT_IDENT);
        const std::string &ident = idNode->getValue();

        StoreValueSP value = store_.resolve(ident);
        if (value == nullptr) {
            std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
            continue;
        }

        NumericTypeSP number = std::dynamic_pointer_cast<NumericType>(value);
        if (!number) {
            std::cout << T_BRED << NOT_NUMERIC << T_RESET << std::endl;
            continue;
        }

        number->incr();
        std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
    }
}

void Handler::handleDecr_(std::vector<ValueASTNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 1) throw MIN_ONE_ARG_K("DECR");

    for (const auto &arg : args) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        if (!idNode) throw RuntimeErr(NOT_IDENT);
        const std::string &ident = idNode->getValue();

        StoreValueSP value = store_.resolve(ident);
        if (value == nullptr) {
            std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
            continue;
        }

        NumericTypeSP number = std::dynamic_pointer_cast<NumericType>(value);
        if (!number) {
            std::cout << T_BRED << NOT_NUMERIC << T_RESET << std::endl;
            continue;
        }

        number->decr();
        std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
    }
}

void Handler::handleAppend_(std::vector<ValueASTNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 2) throw MIN_TWO_ARG_KV("APPEND");

    // First must be an identifier to a list
    IdentifierValueSP identNode = std::dynamic_pointer_cast<IdentifierValue>(args[0]->evaluate());
    if (!identNode) throw RuntimeErr(NOT_IDENT);
    StoreValueSP listObj = store_.resolve(identNode->getValue());
    if (!listObj) {
        std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
        return;
    }

    ListValueSP list = std::dynamic_pointer_cast<ListValue>(listObj);
    if (!list) {
        std::cout << T_BYLLW << NOT_LIST << T_RESET << std::endl;
        return;
    }

    for (std::size_t i = 1; i < numArgs; i++) {
        if (!args[i]) continue;
        list->append((args[i])->evaluate());
        std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
    }
}

void Handler::handlePrepend_(std::vector<ValueASTNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 2) throw MIN_TWO_ARG_KV("PREPEND");

    IdentifierValueSP identNode = std::dynamic_pointer_cast<IdentifierValue>(args[0]->evaluate());
    if (!identNode) throw RuntimeErr(NOT_IDENT);
    StoreValueSP listObj = store_.resolve(identNode->getValue());
    if (!listObj) {
        std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
        return;
    }

    ListValueSP list = std::dynamic_pointer_cast<ListValue>(listObj);
    if (!list) {
        std::cout << T_BYLLW << NOT_LIST << T_RESET << std::endl;
        return;
    }

    for (std::size_t i = 1; i < numArgs; i++) {
        if (!args[i]) continue;
        list->prepend((args[i])->evaluate());
        std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
    }
}

void Handler::handleSearch_(std::vector<ValueASTNodeSP> &args, const std::size_t numArgs) {
    if (numArgs < 1) throw RuntimeErr("Error: SEARCH requires at least one argument (pattern)");

    for (const auto &arg : args) {
        if (!arg) continue;

        StringValueSP node = std::dynamic_pointer_cast<StringValue>(arg->evaluate());
        if (!node) throw RuntimeErr(WRONG_CMD_FMT);

        const std::string &pattern = node->getValueType() == ValueType::IDENTIIFER
                                         ? node->getValue()
                                         : removeQuotations(node->getValue());
        std::vector<std::string> keys = store_.search(pattern);

        std::cout << T_BYLLW << pattern << " (" << keys.size() << ")" << T_RESET << std::endl;
        for (const auto &key : keys)
            std::cout << "  " << key << std::endl;
    }
}
