#include "syntax_tree.h"

std::string NilNode::string() const { return "{node: nil, value: nil}"; }

std::string ValueNode::string() const { return "{node: ValueDefault, value: N/A}"; }

std::string CommandNode::string() const {
    std::string s = "{node: Command, cmd: " + std::to_string((int) cmdType) + ", args: [";
    for (const auto &a : args) {
        if (!a) continue;
        s += a->string() + ", ";
    }
    if (!args.empty()) {
        s.pop_back();
        s.pop_back();
    }
    s += "]}";
    return s;
}

std::string IntegerNode::string() const {
    return "{node: Integer, value: " + std::to_string(getValue()) + "}";
}

std::string FloatNode::string() const {
    return "{node: Float, value: " + std::to_string(getValue()) + "}";
}

std::string StringNode::string() const {
    return "{node: String, value: " + getValue() + "}";
}

std::string IdentifierNode::string() const {
    return "{node: Identifier, value: " + getIdent() + "}";
}

std::string ListNode::string() const {
    std::string s = "{node: List, value: [";

    const std::vector<ValueNodeSP> &values = getValue();
    for (const auto &v : values) {
        if (!v) continue;
        s += v->string() + ", ";
    }
    if (!values.empty()) {
        s.pop_back();
        s.pop_back();
    }
    s += "]}";
    return s;
}
