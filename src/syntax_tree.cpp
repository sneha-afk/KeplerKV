#include "syntax_tree.h"

ValueNode::ValueNode(StoreValueSP s)
    : storeValue(*s) {
    if (s->isInt())
        valType = ValueType::INT;
    else if (s->isFloat())
        valType = ValueType::FLOAT;
    else if (s->isString())
        valType = ValueType::STRING;
    else if (s->isList())
        valType = ValueType::LIST;
}

std::string ValueNode::string() const {
    std::string s = "{node: Value";
    switch (valType) {
        case ValueType::INT:
            s += ", type: Integer, value: " + std::to_string(storeValue.getInt()) + "}";
            break;
        case ValueType::FLOAT:
            s += ", type: Float, value: " + std::to_string(storeValue.getFloat()) + "}";
            break;
        case ValueType::STRING:
            s += ", type: String, value: " + storeValue.getString() + "}";
            break;
        case ValueType::LIST:
            s += ", type: List, value: [";

            const std::vector<StoreValueSP> &values = storeValue.getList();
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
