#include "syntax_tree.h"

#include <iostream>

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
