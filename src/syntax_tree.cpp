#include "syntax_tree.h"

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

std::string FloatASTNode::string() const {
    return "{node: Value, type: Float, value: " + std::to_string(value_) + "}";
}

std::string StringASTNode::string() const {
    return "{node: Value, type: String, value: " + value_ + "}";
}

std::string IdentifierASTNode::string() const {
    return "{node: Value, type: Identifier, value: " + value_ + "}";
}

std::string ListASTNode::string() const {
    std::string s = "{node: Value, type: String, value: [";
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
