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

// Forward declaration to type-alias
struct SyntaxNode;
using SyntaxNodeSP = std::shared_ptr<SyntaxNode>;

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
    std::string string() const override;
};

struct CommandNode : SyntaxNode {
    CommandType cmdType;
    std::vector<SyntaxNodeSP> args;

    CommandNode()
        : cmdType(CommandType::UNKNOWN)
        , SyntaxNode(NodeType::COMMAND) {
        args = std::vector<SyntaxNodeSP>();
        args.reserve(2);
    };
    CommandNode(CommandType c)
        : cmdType(c)
        , SyntaxNode(NodeType::COMMAND) {
        args = std::vector<SyntaxNodeSP>();
        args.reserve(2);
    };

    std::string string() const override;
};

// Used to identify all value types
struct ValueNode : SyntaxNode {
    ValueType valType;

    ValueNode(ValueType t)
        : SyntaxNode(NodeType::VALUE)
        , valType(t) {};

    std::string string() const override;
};

struct IntegerNode : ValueNode {
    int value;

    IntegerNode(int v)
        : value(v)
        , ValueNode(ValueType::INT) {};

    std::string string() const override;
};

struct FloatNode : ValueNode {
    float value;

    FloatNode(float v)
        : value(v)
        , ValueNode(ValueType::FLOAT) {};

    std::string string() const override;
};

struct StringNode : ValueNode {
    std::string value;

    StringNode(std::string v)
        : value(v)
        , ValueNode(ValueType::STRING) {};

    std::string string() const override;
};

struct IdentifierNode : StringNode {
    IdentifierNode(std::string v)
        : StringNode(v) {
        type = NodeType::IDENTIFIER;
    };

    std::string string() const override;
};

struct ListNode : ValueNode {
    std::vector<SyntaxNodeSP> value;

    ListNode()
        : ValueNode(ValueType::LIST) {
        value = std::vector<SyntaxNodeSP>();
    };
    ListNode(std::vector<SyntaxNodeSP> &v)
        : value(v)
        , ValueNode(ValueType::LIST) {};

    std::string string() const override;
};
