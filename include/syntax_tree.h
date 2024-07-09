#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

enum class NodeType { COMMAND, IDENTIFIER, VALUE, NIL };

enum class CommandType {
    SET,
    GET,
    DELETE,
    UPDATE,
    UNKNOWN,
};

enum class ValueNodeType {
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

struct ValueNode;
using ValueNodeSP = std::shared_ptr<ValueNode>;
using ValueType = std::variant<int, float, std::string, std::vector<ValueNodeSP>>;

// https://stackoverflow.com/a/54596
class SyntaxNode {
public:
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

class CommandNode : SyntaxNode {
public:
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
class ValueNode : SyntaxNode {
public:
    ValueNodeType valType;
    ValueType value;

    ValueNode(ValueNodeType t, ValueType v)
        : SyntaxNode(NodeType::VALUE)
        , valType(t)
        , value(v) {};
    ValueNode(NodeType n, ValueNodeType t, ValueType v)
        : SyntaxNode(n)
        , valType(t)
        , value(v) {};

    std::string string() const override;
};

class IntegerNode : ValueNode {
public:
    IntegerNode(int v)
        : ValueNode(ValueNodeType::INT, v) {};

    const int &getValue() const { return std::get<int>(value); }
    std::string string() const override;
};

class FloatNode : ValueNode {
public:
    FloatNode(float v)
        : ValueNode(ValueNodeType::FLOAT, v) {};

    const float &getValue() const { return std::get<float>(value); }
    std::string string() const override;
};

class StringNode : ValueNode {
public:
    StringNode(std::string v)
        : ValueNode(ValueNodeType::STRING, v) {};

    const std::string &getValue() const { return std::get<std::string>(value); }
    std::string string() const override;
};

class IdentifierNode : ValueNode {
public:
    IdentifierNode(std::string v)
        : ValueNode(NodeType::IDENTIFIER, ValueNodeType::STRING, v) {};

    const std::string &getIdent() const { return std::get<std::string>(value); }
    const std::string &getValue() const { return getValue(); }
    std::string string() const override;
};

class ListNode : ValueNode {
public:
    ListNode()
        : ValueNode(ValueNodeType::LIST, std::vector<ValueNodeSP>()) {};
    ListNode(std::vector<ValueNodeSP> &v)
        : ValueNode(ValueNodeType::LIST, v) {};

    const std::vector<ValueNodeSP> &getValue() const {
        return std::get<std::vector<ValueNodeSP>>(value);
    }
    std::string string() const override;
};

using CommandNodeSP = std::shared_ptr<CommandNode>;
using IntegerNodeSP = std::shared_ptr<IntegerNode>;
using FloatNodeSP = std::shared_ptr<FloatNode>;
using StringNodeSP = std::shared_ptr<StringNode>;
using IdentifierNodeSP = std::shared_ptr<IdentifierNode>;
using ListNodeSP = std::shared_ptr<ListNode>;
