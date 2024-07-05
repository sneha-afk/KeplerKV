#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

enum class NodeType { COMMAND, IDENTIFIER, VALUE, NIL };

enum class CommandType {
    SET,
    GET,
    DELETE,
    UPDATE,
    UNKNOWN,
};

enum class ValueType {
    INT,
    FLOAT,
    STRING,
    LIST,
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

// https://stackoverflow.com/a/54596
struct SyntaxNode {
    NodeType type;
    SyntaxNode(NodeType t)
        : type(t) {};

    // https://stackoverflow.com/q/461203
    virtual ~SyntaxNode() = default;

    virtual std::string string() const = 0;
    friend std::ostream &operator<<(std::ostream &os, const SyntaxNode &t) {
        os << t.string();
        return os;
    };
};

struct NilNode : SyntaxNode {
    NilNode()
        : SyntaxNode(NodeType::NIL) {};
    std::string string() const override { return "{node: nil, value: nil}"; }
};

struct CommandNode : SyntaxNode {
    CommandType cmdType;
    std::vector<std::shared_ptr<SyntaxNode>> args;
    CommandNode()
        : cmdType(CommandType::UNKNOWN)
        , SyntaxNode(NodeType::COMMAND) {
        args = std::vector<std::shared_ptr<SyntaxNode>>();
        args.reserve(2);
    };
    CommandNode(CommandType c)
        : cmdType(c)
        , SyntaxNode(NodeType::COMMAND) {
        args = std::vector<std::shared_ptr<SyntaxNode>>();
        args.reserve(2);
    };

    std::string string() const override {
        std::string s
            = "{node: Command, cmd: " + std::to_string((int) cmdType) + ", args: [";
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
};

// Used to identify all value types
struct ValueNode : SyntaxNode {
    ValueType valType;
    ValueNode(ValueType t)
        : SyntaxNode(NodeType::VALUE)
        , valType(t) {};

    std::string string() const override { return "{node: ValueDefault, value: N/A}"; }
};

struct IntegerNode : ValueNode {
    int value;
    IntegerNode(int v)
        : value(v)
        , ValueNode(ValueType::INT) {};

    std::string string() const override {
        return "{node: Integer, value: " + std::to_string(value) + "}";
    }
};

struct FloatNode : ValueNode {
    float value;
    FloatNode(float v)
        : value(v)
        , ValueNode(ValueType::FLOAT) {};

    std::string string() const override {
        return "{node: Float, value: " + std::to_string(value) + "}";
    }
};

struct StringNode : ValueNode {
    std::string value;
    StringNode(std::string v)
        : value(v)
        , ValueNode(ValueType::STRING) {};

    std::string string() const override { return "{node: String, value: " + value + "}"; }
};

struct IdentifierNode : StringNode {
    IdentifierNode(std::string v)
        : StringNode(v) {
        type = NodeType::IDENTIFIER;
    };

    std::string string() const override {
        return "{node: Identifier, value: " + value + "}";
    }
};

struct ListNode : ValueNode {
    std::vector<std::shared_ptr<SyntaxNode>> value;
    ListNode()
        : ValueNode(ValueType::LIST) {
        value = std::vector<std::shared_ptr<SyntaxNode>>();
    };
    ListNode(std::vector<std::shared_ptr<SyntaxNode>> &v)
        : value(v)
        , ValueNode(ValueType::LIST) {};

    std::string string() const override {
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
};
