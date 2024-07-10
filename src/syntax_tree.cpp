#include "syntax_tree.h"

ValueNode::ValueNode(StoreValueSP s)
    : value(s) {
    if (s->isInt())
        valType_ = ValueType::INT;
    else if (s->isFloat())
        valType_ = ValueType::FLOAT;
    else if (s->isString())
        valType_ = ValueType::STRING;
    else if (s->isList())
        valType_ = ValueType::LIST;
}

std::string ValueNode::string() const {
    std::string s = "{node: Value";
    switch (valType_) {
        case ValueType::INT:
            s += ", type: Integer, value: " + std::to_string(value->getInt()) + "}";
            break;
        case ValueType::FLOAT:
            s += ", type: Float, value: " + std::to_string(value->getFloat()) + "}";
            break;
        case ValueType::STRING:
            s += ", type: String, value: " + value->getString() + "}";
            break;
        case ValueType::IDENTIFIER:
            s += ", type: Identifier, value: " + value->getString() + "}";
            break;
        case ValueType::LIST:
            s += ", type: List, value: [";

            const std::vector<StoreValueSP> &values = value->getList();
            for (const auto &v : values) {
                if (!v) continue;
                s += v->string() + ", ";
            }
            if (!values.empty()) {
                s.pop_back();
                s.pop_back();
            }
            s += "]}";
            break;
    }
    return s;
}

std::string CommandNode::string() const {
    std::string s
        = "{node: Command, cmd: " + std::to_string((int) cmdType_) + ", args: [";
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
