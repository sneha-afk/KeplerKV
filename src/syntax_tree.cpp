#include "syntax_tree.h"

std::string Command::string() const {
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

std::string IntNode::string() const {
    return "{node: Value, type: Int, value: " + std::to_string(value_) + "}";
}

StoreValueSP IntNode::evaluate() const { return std::make_shared<IntValue>(value_); }

std::string FloatNode::string() const {
    return "{node: Value, type: Float, value: " + std::to_string(value_) + "}";
}

StoreValueSP FloatNode::evaluate() const { return std::make_shared<FloatValue>(value_); }

std::string StringNode::string() const {
    return "{node: Value, type: String, value: " + value_ + "}";
}

StoreValueSP StringNode::evaluate() const {
    return std::make_shared<StringValue>(std::move(value_));
}

std::string IdentifierNode::string() const {
    return "{node: Value, type: Identifier, value: " + value_ + "}";
}

StoreValueSP IdentifierNode::evaluate() const {
    return std::make_shared<IdentifierValue>(std::move(value_));
}

std::string ListNode::string() const {
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

StoreValueSP ListNode::evaluate() const {
    std::shared_ptr<ListValue> listVal = std::make_shared<ListValue>();
    for (const auto &node : value_)
        listVal->append(node->evaluate());
    return listVal;
}
