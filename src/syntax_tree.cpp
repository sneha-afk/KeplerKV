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
    return "{node: Integer, value: " + std::to_string(value) + "}";
}

std::string FloatNode::string() const {
    return "{node: Float, value: " + std::to_string(value) + "}";
}

std::string StringNode::string() const { return "{node: String, value: " + value + "}"; }

std::string IdentifierNode::string() const {
    return "{node: Identifier, value: " + value + "}";
}

std::string ListNode::string() const {
    std::string s = "{node: List, value: [";
    for (const auto &v : value) {
        if (!v) continue;
        s += v->string() + ", ";
    }
    if (!value.empty()) {
        s.pop_back();
        s.pop_back();
    }
    s += "]}";
    return s;
}
