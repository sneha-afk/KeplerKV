#include "syntax_tree.h"

#include "error_msgs.h"
#include "file_io_macros.h"

#include <iostream>

// Filenames may have been passed in deliminated as strings
std::string getFilename_(const ValueASTNodeSP node) {
    std::string &filename = DEFAULT_SAVE_FILE;

    StringValueSP fnNode = std::dynamic_pointer_cast<StringValue>(node);
    if (!fnNode) throw RuntimeErr(INVALID_FNAME);
    filename = fnNode->getValue();

    return fnNode->getValueType() == ValueType::IDENTIIFER ? filename : removeQuotations(filename);
}

std::string CommandASTNode::string() const {
    std::string s = "{node: Command, cmd: " + std::to_string((int) cmdType_) + ", args: [";
    for (const auto &a : args_) {
        if (!a) continue;
        s += a->string() + ", ";
    }
    if (!args_.empty()) {
        s.pop_back();
        s.pop_back();
    }
    s += "]}";
    return s;
}

std::string IntASTNode::string() const {
    return "{node: Value, type: Int, value: " + std::to_string(value_) + "}";
}

StoreValueSP IntASTNode::evaluate() const { return std::make_shared<IntValue>(value_); }

std::string FloatASTNode::string() const {
    return "{node: Value, type: Float, value: " + std::to_string(value_) + "}";
}

StoreValueSP FloatASTNode::evaluate() const { return std::make_shared<FloatValue>(value_); }

std::string StringASTNode::string() const {
    return "{node: Value, type: String, value: " + value_ + "}";
}

StoreValueSP StringASTNode::evaluate() const {
    return std::make_shared<StringValue>(std::move(value_));
}

std::string IdentifierASTNode::string() const {
    return "{node: Value, type: Identifier, value: " + value_ + "}";
}

StoreValueSP IdentifierASTNode::evaluate() const {
    return std::make_shared<IdentifierValue>(std::move(value_));
}

std::string ListValueASTNode::string() const {
    std::string s = "{node: Value, type: List, value: [";
    for (const auto &v : value_) {
        if (!v) continue;
        s += v->string() + ", ";
    }
    if (!value_.empty()) {
        s.pop_back();
        s.pop_back();
    }
    s += "]}";
    return s;
}

StoreValueSP ListValueASTNode::evaluate() const {
    std::shared_ptr<ListValue> listVal = std::make_shared<ListValue>();
    for (const auto &node : value_)
        listVal->append(node->evaluate());
    return listVal;
}

void QuitCmdASTNode::execute() const {
    std::cout << T_BBLUE << "Farewell!" << T_RESET << std::endl;
    exit(EXIT_SUCCESS);
};

void ClearCmdASTNode::execute() const { std::cout << "\033[H\033[2J" << std::endl; };

bool SetCmdASTNode::validate() const {
    if (numArgs() < 2) return false;

    for (std::size_t i = 0; i < numArgs(); i += 2) {
        if (!args_[i]) continue;

        // First node must be an identifier
        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(args_[i]->evaluate());
        if (!idNode) return false;

        // Identiifer must follow a value
        if (!(i + 1 < numArgs()) || !args_[i + 1]) return false;
    }
    return true;
}

void SetCmdASTNode::execute(Store &s) const {
    for (std::size_t i = 0; i < numArgs(); i += 2) {
        if (!args_[i]) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(args_[i]->evaluate());
        const std::string &ident = idNode->getValue();

        s.set(ident, (args_[i + 1])->evaluate());
        std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
    }
}

bool GetCmdASTNode::validate() const {
    if (numArgs() < 1) return false;

    for (const auto &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        if (!idNode) return false;
    }
    return true;
}

void GetCmdASTNode::execute(Store &s) const {
    for (const auto &arg : args_) {
        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        const std::string &ident = idNode->getValue();

        StoreValueSP value = s.get(ident);
        if (value)
            printItem(ident, value);
        else
            std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
    }
}

void ListCmdASTNode::execute(Store &s) const {
    if (s.size() < 1) std::cout << T_BYLLW << "(empty)" << T_RESET << std::endl;
    for (const auto &item : s)
        printItem(item.first, item.second);
}

bool DeleteCmdASTNode::validate() const {
    if (numArgs() < 1) return false;

    for (const auto &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        if (!idNode) return false;
    }
    return true;
}

void DeleteCmdASTNode::execute(Store &s) const {
    for (const auto &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        const std::string &ident = idNode->getValue();

        bool deleted = s.del(ident);
        if (deleted)
            std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
        else
            std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
    }
}

bool UpdateCmdASTNode::validate() const {
    if (numArgs() < 2) return false;

    for (std::size_t i = 0; i < numArgs(); i += 2) {
        if (!args_[i]) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(args_[i]->evaluate());
        if (!idNode) return false;

        if (!(i + 1 < numArgs()) || !args_[i + 1]) return false;
    }
    return true;
}

void UpdateCmdASTNode::execute(Store &s) const {
    for (std::size_t i = 0; i < numArgs(); i += 2) {
        if (!args_[i]) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(args_[i]->evaluate());
        const std::string &ident = idNode->getValue();

        bool updated = s.update(ident, (args_[i + 1])->evaluate());
        if (updated)
            std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
        else
            std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
    }
}

bool ResolveCmdASTNode::validate() const {
    if (numArgs() < 1) return false;

    for (const auto &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        if (!idNode) return false;
    }
    return true;
}

void ResolveCmdASTNode::execute(Store &s) const {
    for (const auto &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        const std::string &ident = idNode->getValue();

        StoreValueSP value = s.resolve(ident, true);
        if (value)
            printItem(ident, value);
        else
            std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
    }
}

void SaveCmdASTNode::execute(Store &s) const {
    std::string &filename = DEFAULT_SAVE_FILE;
    if (numArgs()) filename = getFilename_(args_[0]);

    s.saveToFile(filename + ".kep");
    std::cout << T_BGREEN << "SAVED" << T_RESET << std::endl;
}

void LoadCmdASTNode::execute(Store &s) const {
    std::string &filename = DEFAULT_SAVE_FILE;
    if (numArgs()) filename = getFilename_(args_[0]);

    s.loadFromFile(filename + ".kep");
    std::cout << T_BGREEN << "LOADED" << T_RESET << std::endl;
}

bool RenameCmdASTNode::validate() const {
    if (numArgs() < 2) return false;

    for (std::size_t i = 0; i < numArgs(); i += 2) {
        if (!args_[i]) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(args_[i]->evaluate());
        if (!idNode) return false;

        if (!(i + 1 < numArgs()) || !args_[i + 1]) return false;
    }
    return true;
}

void RenameCmdASTNode::execute(Store &s) const {
    for (std::size_t i = 0; i < numArgs(); i += 2) {
        if (!args_[i]) continue;

        IdentifierValueSP oldNode
            = std::dynamic_pointer_cast<IdentifierValue>((args_[i])->evaluate());
        IdentifierValueSP newNode
            = std::dynamic_pointer_cast<IdentifierValue>((args_[i + 1])->evaluate());

        const std::string &oldName = oldNode->getValue();
        const std::string &newName = newNode->getValue();

        // Make the user confirm overwrites
        if (s.contains(newName)) {
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

        s.rename(oldName, newName);
        std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
    }
}

bool IncrCmdASTNode::validate() const {
    if (numArgs() < 1) return false;

    for (const auto &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        if (!idNode) return false;
    }
    return true;
}

void IncrCmdASTNode::execute(Store &s) const {
    for (const auto &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        const std::string &ident = idNode->getValue();

        StoreValueSP value = s.resolve(ident);
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

bool DecrCmdASTNode::validate() const {
    if (numArgs() < 1) return false;

    for (const auto &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        if (!idNode) return false;
    }
    return true;
}

void DecrCmdASTNode::execute(Store &s) const {
    for (const auto &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        const std::string &ident = idNode->getValue();

        StoreValueSP value = s.resolve(ident);
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

bool AppendCmdASTNode::validate() const {
    if (numArgs() < 2) return false;

    // First must be an identifier to a list
    IdentifierValueSP identNode = std::dynamic_pointer_cast<IdentifierValue>(args_[0]->evaluate());
    if (!identNode) return false;
    return true;
}

void AppendCmdASTNode::execute(Store &s) const {
    IdentifierValueSP identNode = std::dynamic_pointer_cast<IdentifierValue>(args_[0]->evaluate());
    StoreValueSP listObj = s.resolve(identNode->getValue());
    if (!listObj) {
        std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
        return;
    }

    ListValueSP list = std::dynamic_pointer_cast<ListValue>(listObj);
    if (!list) {
        std::cout << T_BYLLW << NOT_LIST << T_RESET << std::endl;
        return;
    }

    for (std::size_t i = 1; i < numArgs(); i++) {
        if (!args_[i]) continue;
        list->append((args_[i])->evaluate());
        std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
    }
}

bool PrependCmdASTNode::validate() const {
    if (numArgs() < 2) return false;

    // First must be an identifier to a list
    IdentifierValueSP identNode = std::dynamic_pointer_cast<IdentifierValue>(args_[0]->evaluate());
    if (!identNode) return false;
    return true;
}

void PrependCmdASTNode::execute(Store &s) const {
    IdentifierValueSP identNode = std::dynamic_pointer_cast<IdentifierValue>(args_[0]->evaluate());
    StoreValueSP listObj = s.resolve(identNode->getValue());
    if (!listObj) {
        std::cout << T_BYLLW << "NOT FOUND" << T_RESET << std::endl;
        return;
    }

    ListValueSP list = std::dynamic_pointer_cast<ListValue>(listObj);
    if (!list) {
        std::cout << T_BYLLW << NOT_LIST << T_RESET << std::endl;
        return;
    }

    for (std::size_t i = 1; i < numArgs(); i++) {
        if (!args_[i]) continue;
        list->prepend((args_[i])->evaluate());
        std::cout << T_BGREEN << "OK" << T_RESET << std::endl;
    }
}

bool SearchCmdASTNode::validate() const {
    if (numArgs() < 1) return false;

    for (const auto &arg : args_) {
        if (!arg) continue;

        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(arg->evaluate());
        if (!idNode) return false;
    }
    return true;
}

void SearchCmdASTNode::execute(Store &s) const {
    for (const auto &arg : args_) {
        if (!arg) continue;

        StringValueSP node = std::dynamic_pointer_cast<StringValue>(arg->evaluate());

        const std::string &pattern = node->getValueType() == ValueType::IDENTIIFER
                                         ? node->getValue()
                                         : removeQuotations(node->getValue());
        std::vector<std::string> keys = s.search(pattern);

        std::cout << T_BYLLW << pattern << " (" << keys.size() << ")" << T_RESET << std::endl;
        for (const auto &key : keys)
            std::cout << "  " << key << std::endl;
    }
}

void StatsCmdASTNode::execute(Store &s) const {
    std::cout << T_BYLLW << "KeplerKV Statistics" << T_RESET << std::endl;

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
