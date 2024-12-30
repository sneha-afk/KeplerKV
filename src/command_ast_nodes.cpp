#include "command_ast_nodes.h"

#include "environment_interface.h"
#include "error_msgs.h"
#include "file_io_macros.h"
#include "syntax_tree.h"
#include "terminal_colors.h"

#define PRINT_ITEM(id, valStr) T_BBLUE + id + T_RESET + " | " + valStr

#define OK_MSG        PRINT_GREEN("OK")
#define NOT_FOUND_MSG PRINT_YELLOW("NOT FOUND")

// Filenames may have been passed in deliminated as strings
std::string getFilename_(const ValueSP node) {
    std::string &filename = DEFAULT_SAVE_FILE;

    StringValueSP fnNode = std::dynamic_pointer_cast<StringValue>(node);
    if (!fnNode) throw RuntimeErr(INVALID_FNAME);
    filename = fnNode->getValue();

    return fnNode->getValueType() == ValueType::IDENTIFIER ? filename : removeQuotations(filename);
}

void QuitCommand::execute(EnvironmentInterface &e) const {
    e.printToConsole(PRINT_BLUE("Farewell!"));
    e.exitSuccess();
};

void ClearCommand::execute(EnvironmentInterface &e) const { e.printToConsole(CLEAR_SCREEN); };

void BeginCommand::execute(EnvironmentInterface &e) const {
    e.setTransacState(true);
    e.printToConsole(PRINT_YELLOW("TRANSAC BEGIN"));
}

void CommitCommand::execute(EnvironmentInterface &e) const {
    e.executeAllWAL();
    e.setTransacState(false);
    e.printToConsole(PRINT_YELLOW("TRANSAC COMMITTED"));
}

void RollbackCommand::execute(EnvironmentInterface &e) const {
    e.clearWAL();
    e.setTransacState(false);
    e.printToConsole(PRINT_YELLOW("TRANSAC ROLLBACK"));
}

bool SetCommand::validate() const {
    if (numArgs() < 2) return false;

    for (std::size_t i = 0; i < numArgs(); i += 2) {
        if (!args_[i]) continue;

        // First node must be an identifier
        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(args_[i]->evaluate());
        if (!idNode) return false;

        // Identifier must follow a value
        if (!(i + 1 < numArgs()) || !args_[i + 1]) return false;
    }
    return true;
}

void SetCommand::execute(EnvironmentInterface &e, Store &s) const {
    for (std::size_t i = 0; i < numArgs(); i += 2) {
        if (!args_[i]) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(args_[i]->evaluate());
        const std::string &ident = idNode->getValue();

        s.set(ident, (args_[i + 1])->evaluate());
        e.printToConsole(OK_MSG);
    }
}

bool GetCommand::validate() const {
    if (numArgs() < 1) return false;

    for (const ValueSP &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        if (!idNode) return false;
    }
    return true;
}

void GetCommand::execute(EnvironmentInterface &e, Store &s) const {
    for (const ValueSP &arg : args_) {
        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        const std::string &ident = idNode->getValue();

        StoreValueSP value = s.get(ident);
        if (value) {

            e.printToConsole(PRINT_ITEM(ident, value->string()));
        } else {
            e.printToConsole(NOT_FOUND_MSG);
        }
    }
}

void ListCommand::execute(EnvironmentInterface &e, Store &s) const {
    if (s.size() < 1) e.printToConsole(PRINT_YELLOW("(empty)"));
    for (const auto &item : s)

        e.printToConsole(PRINT_ITEM(item.first, item.second->string()));
}

bool DeleteCommand::validate() const {
    if (numArgs() < 1) return false;

    for (const ValueSP &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        if (!idNode) return false;
    }
    return true;
}

void DeleteCommand::execute(EnvironmentInterface &e, Store &s) const {
    for (const ValueSP &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        const std::string &ident = idNode->getValue();

        bool deleted = s.del(ident);
        if (deleted) {
            e.printToConsole(OK_MSG);
        } else {
            e.printToConsole(NOT_FOUND_MSG);
        }
    }
}

bool UpdateCommand::validate() const {
    if (numArgs() < 2) return false;

    for (std::size_t i = 0; i < numArgs(); i += 2) {
        if (!args_[i]) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(args_[i]->evaluate());
        if (!idNode) return false;

        if (!(i + 1 < numArgs()) || !args_[i + 1]) return false;
    }
    return true;
}

void UpdateCommand::execute(EnvironmentInterface &e, Store &s) const {
    for (std::size_t i = 0; i < numArgs(); i += 2) {
        if (!args_[i]) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(args_[i]->evaluate());
        const std::string &ident = idNode->getValue();

        bool updated = s.update(ident, (args_[i + 1])->evaluate());
        if (updated) {
            e.printToConsole(OK_MSG);
        } else {
            e.printToConsole(NOT_FOUND_MSG);
        }
    }
}

bool ResolveCommand::validate() const {
    if (numArgs() < 1) return false;

    for (const ValueSP &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        if (!idNode) return false;
    }
    return true;
}

void ResolveCommand::execute(EnvironmentInterface &e, Store &s) const {
    for (const ValueSP &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        const std::string &ident = idNode->getValue();

        StoreValueSP value = s.resolve(ident, true);
        if (value) {

            e.printToConsole(PRINT_ITEM(ident, value->string()));
        } else {
            e.printToConsole(NOT_FOUND_MSG);
        }
    }
}

void SaveCommand::execute(EnvironmentInterface &e, Store &s) const {
    std::string &filename = DEFAULT_SAVE_FILE;
    if (numArgs()) filename = getFilename_(args_[0]);

    s.saveToFile(filename + ".kep");
    e.printToConsole(PRINT_GREEN("SAVED"));
}

void LoadCommand::execute(EnvironmentInterface &e, Store &s) const {
    std::string &filename = DEFAULT_SAVE_FILE;
    if (numArgs()) filename = getFilename_(args_[0]);

    s.loadFromFile(filename + ".kep");
    e.printToConsole(PRINT_GREEN("LOADED"));
}

bool RenameCommand::validate() const {
    if (numArgs() < 2) return false;

    for (std::size_t i = 0; i < numArgs(); i += 2) {
        if (!args_[i]) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(args_[i]->evaluate());
        if (!idNode) return false;

        if (!(i + 1 < numArgs()) || !args_[i + 1]) return false;
    }
    return true;
}

void RenameCommand::execute(EnvironmentInterface &e, Store &s) const {
    for (std::size_t i = 0; i < numArgs(); i += 2) {
        if (!args_[i]) continue;

        IdentifierValueSP oldNode
            = std::dynamic_pointer_cast<IdentifierValue>((args_[i])->evaluate());
        IdentifierValueSP newNode
            = std::dynamic_pointer_cast<IdentifierValue>((args_[i + 1])->evaluate());

        const std::string &oldName = oldNode->getValue();
        const std::string &newName = newNode->getValue();

        // Make the user confirm overwrites
        if (s.contains(newName) && !hasOption(CommandOption::YES)) {
            e.printToConsole(T_BYLLW "Warning: key \'" + newName
                                 + "\' already exists. Do you want to overwrite it? (y/n)" T_RESET,
                true);

            if (hasOption(CommandOption::NO)) {
                e.printToConsole(PRINT_YELLOW("No changes made to the store."));
                return;
            }

            std::string confirm;
            std::getline(std::cin, confirm);
            if (confirm.size() && confirm[0] != 'y') {
                e.printToConsole(PRINT_YELLOW("No changes made to the store."));
                return;
            }
        }

        s.rename(oldName, newName);
        e.printToConsole(OK_MSG);
    }
}

bool IncrementCommand::validate() const {
    if (numArgs() < 1) return false;

    for (const ValueSP &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        if (!idNode) return false;
    }
    return true;
}

void IncrementCommand::execute(EnvironmentInterface &e, Store &s) const {
    for (const ValueSP &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        const std::string &ident = idNode->getValue();

        StoreValueSP value = s.resolve(ident);
        if (value == nullptr) {
            e.printToConsole(NOT_FOUND_MSG);
            continue;
        }

        NumericTypeSP number = std::dynamic_pointer_cast<NumericType>(value);
        if (!number) {
            e.printToConsole(PRINT_RED(NOT_NUMERIC));
            continue;
        }

        number->incr();
        e.printToConsole(OK_MSG);
    }
}

bool DecrementCommand::validate() const {
    if (numArgs() < 1) return false;

    for (const ValueSP &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        if (!idNode) return false;
    }
    return true;
}

void DecrementCommand::execute(EnvironmentInterface &e, Store &s) const {
    for (const ValueSP &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        const std::string &ident = idNode->getValue();

        StoreValueSP value = s.resolve(ident);
        if (value == nullptr) {
            e.printToConsole(NOT_FOUND_MSG);
            continue;
        }

        NumericTypeSP number = std::dynamic_pointer_cast<NumericType>(value);
        if (!number) {
            e.printToConsole(PRINT_RED(NOT_NUMERIC));
            continue;
        }

        number->decr();
        e.printToConsole(OK_MSG);
    }
}

bool AppendCommand::validate() const {
    if (numArgs() < 2) return false;

    // First must be an identifier to a list
    IdentifierValueSP identNode = std::dynamic_pointer_cast<IdentifierValue>(args_[0]->evaluate());
    if (!identNode) return false;
    return true;
}

void AppendCommand::execute(EnvironmentInterface &e, Store &s) const {
    IdentifierValueSP identNode = std::dynamic_pointer_cast<IdentifierValue>(args_[0]->evaluate());
    StoreValueSP listObj = s.resolve(identNode->getValue());
    if (!listObj) {
        e.printToConsole(NOT_FOUND_MSG);
        return;
    }

    ListValueSP list = std::dynamic_pointer_cast<ListValue>(listObj);
    if (!list) {
        e.printToConsole(PRINT_YELLOW(NOT_LIST));
        return;
    }

    for (std::size_t i = 1; i < numArgs(); i++) {
        if (!args_[i]) continue;
        list->append((args_[i])->evaluate());
        e.printToConsole(OK_MSG);
    }
}

bool PrependCommand::validate() const {
    if (numArgs() < 2) return false;

    // First must be an identifier to a list
    IdentifierValueSP identNode = std::dynamic_pointer_cast<IdentifierValue>(args_[0]->evaluate());
    if (!identNode) return false;
    return true;
}

void PrependCommand::execute(EnvironmentInterface &e, Store &s) const {
    IdentifierValueSP identNode = std::dynamic_pointer_cast<IdentifierValue>(args_[0]->evaluate());
    StoreValueSP listObj = s.resolve(identNode->getValue());
    if (!listObj) {
        e.printToConsole(NOT_FOUND_MSG);
        return;
    }

    ListValueSP list = std::dynamic_pointer_cast<ListValue>(listObj);
    if (!list) {
        e.printToConsole(PRINT_YELLOW(NOT_LIST));
        return;
    }

    for (std::size_t i = 1; i < numArgs(); i++) {
        if (!args_[i]) continue;
        list->prepend((args_[i])->evaluate());
        e.printToConsole(OK_MSG);
    }
}

bool SearchCommand::validate() const {
    if (numArgs() < 1) return false;

    for (const ValueSP &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idValue = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        if (!idValue) return false;
    }
    return true;
}

void SearchCommand::execute(EnvironmentInterface &e, Store &s) const {
    for (const ValueSP &arg : args_) {
        if (!arg) continue;

        StringValueSP patternVal = std::dynamic_pointer_cast<StringValue>(arg->evaluate());

        const std::string &pattern = patternVal->getValueType() == ValueType::IDENTIFIER
                                         ? patternVal->getValue()
                                         : removeQuotations(patternVal->getValue());
        std::vector<std::string> keys = s.search(pattern);

        e.printToConsole(T_BYLLW + pattern + " (" + std::to_string(keys.size()) + ")" T_RESET);

        for (const auto &key : keys)
            e.printToConsole(" " + key);
    }
}

void StatsCommand::execute(EnvironmentInterface &e, Store &s) const {
    e.printToConsole(PRINT_YELLOW("KeplerKV Statistics"));

    int totalNum = s.size(), numInts = 0, numFloats = 0, numStrs = 0, numLists = 0, numAliases = 0;
    std::size_t totalMem = 0, memInts = 0, memFloats = 0, memStrs = 0, memLists = 0, memAliases = 0,
                memCurr;
    for (const auto &pair : s) {
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
            case ValueType::IDENTIFIER:
                numAliases++;
                memAliases += memCurr;
                break;
            default: break;
        }
    }

    e.printToConsole(PRINT_YELLOW("Total keys: ") + std::to_string(totalNum));

    e.printToConsole(PRINT_YELLOW("Key Distribution by Type: "));

    e.printToConsole("\tIntegers: " + std::to_string(numInts));
    e.printToConsole("\tFloats: " + std::to_string(numFloats));
    e.printToConsole("\tStrings: " + std::to_string(numStrs));
    e.printToConsole("\tLists: " + std::to_string(numLists));
    e.printToConsole("\tAliases: " + std::to_string(numAliases));

    e.printToConsole(PRINT_YELLOW("Usage (including keys) in bytes: ") + std::to_string(totalMem));
    e.printToConsole("\tIntegers: " + std::to_string(memInts));
    e.printToConsole("\tFloats: " + std::to_string(memFloats));
    e.printToConsole("\tStrings: " + std::to_string(memStrs));
    e.printToConsole("\tLists: " + std::to_string(memLists));
    e.printToConsole("\tAliases: " + std::to_string(memAliases));
}
