#pragma once

#include <list>
#include <string>
#include <unordered_map>

enum class CommandType {
    SET,
    GET,
    DELETE,
    UPDATE,
    UNKNOWN,
};

static const std::unordered_map<std::string, CommandType> mapToCmd = {
    { "SET", CommandType::SET },
    { "S", CommandType::SET },
    { "GET", CommandType::GET },
    { "G", CommandType::GET },
    { "DELETE", CommandType::DELETE },
    { "DEL", CommandType::DELETE },
    { "D", CommandType::DELETE },
    { "UPDATE", CommandType::UPDATE },
    { "U", CommandType::UPDATE },
};

enum class ValueType {
    INT,
    FLOAT,
    STRING,
    LIST,
};

// https://stackoverflow.com/a/54596
struct SyntaxNode {
    // https://stackoverflow.com/q/461203
    virtual ~SyntaxNode() = default;

    virtual std::string string() const { return "{node_type: default, value: nil}"; };

    friend std::ostream &operator<<(std::ostream &os, const SyntaxNode &n) {
        os << n.string();
        return os;
    }
};

struct CommandNode : SyntaxNode {
    CommandType cmd;
    std::list<SyntaxNode> args;
    CommandNode()
        : cmd(CommandType::UNKNOWN) {};
    CommandNode(CommandType c)
        : cmd(c) {};

    std::string string() const override {
        std::string s = "{node_type: Command, args: [";
        for (const auto &a : args) {
            s += a.string() + ", ";
        }
        s.pop_back();
        s.pop_back();
        s += "]}";
        return s;
    }
};

struct IntegerNode : SyntaxNode {
    int value;
    IntegerNode(int v)
        : value(v) {};

    std::string string() const override {
        return "{node_type: Integer, value: " + std::to_string(value) + "}";
    }
};

struct FloatNode : SyntaxNode {
    float value;
    FloatNode(float v)
        : value(v) {};

    std::string string() const override {
        return "{node_type: Float, value: " + std::to_string(value) + "}";
    }
};

struct StringNode : SyntaxNode {
    std::string value;
    StringNode(std::string v)
        : value(v) {};

    std::string string() const override {
        return "{node_type: String, value: " + value + "}";
    }
};

struct ListNode : SyntaxNode {
    std::list<SyntaxNode> value;
    ListNode(std::list<SyntaxNode> &v)
        : value(v) {};

    std::string string() const override {
        std::string s = "{node_type: List, value: [";
        for (const auto &v : value) {
            s += v.string() + ", ";
        }
        s.pop_back();
        s.pop_back();
        s += "]}";
        return s;
    }
};
