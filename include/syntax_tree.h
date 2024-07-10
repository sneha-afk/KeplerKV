#pragma once

#include "store_value.h"
#include "util.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

enum class NodeType { COMMAND, VALUE, NIL };

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
    IDENTIFIER,
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

// https://en.wikipedia.org/wiki/Abstract_syntax_tree
// https://stackoverflow.com/a/54596
class ASTNode {
public:
    // https://stackoverflow.com/q/461203
    virtual ~ASTNode() = default;

    virtual NodeType getType() const = 0;

    virtual std::string string() const = 0;
    friend std::ostream &operator<<(std::ostream &os, const ASTNode &t) {
        os << t.string();
        return os;
    };
};

class NilNode : public ASTNode {
    NodeType getType() const { return NodeType::NIL; };
    std::string string() { return "{node: nil, value: nil}"; }
};

class ValueNode : public ASTNode {
public:
    ValueType valType;
    StoreValue storeValue;

    NodeType getType() const { return NodeType::VALUE; };

    ValueNode(int v)
        : valType(ValueType::INT)
        , storeValue(v) {};
    ValueNode(float v)
        : valType(ValueType::FLOAT)
        , storeValue(v) {};
    ValueNode(const std::string &v)
        : valType(ValueType::STRING)
        , storeValue(v) {};
    ValueNode(const std::vector<StoreValueSP> &v)
        : valType(ValueType::LIST)
        , storeValue(v) {};
    ValueNode(StoreValueSP);

    // Uses default values for each type
    ValueNode(ValueType t)
        : valType(t) {
        switch (t) {
            case ValueType::INT: storeValue = 0; break;
            case ValueType::FLOAT: storeValue = float(0); break;
            case ValueType::STRING: storeValue = std::string(); break;
            case ValueType::LIST: storeValue = std::vector<StoreValueSP>();
            default: break;
        }
    };

    // Identifiers are also considered strings internally, call this to mark it as such.
    void setAsIdentifier() { valType = ValueType::IDENTIFIER; }

    // Constructor for identifiers
    ValueNode(const std::string &v, bool isIdent)
        : storeValue(v) {
        if (isIdent)
            valType = ValueType::IDENTIFIER;
        else
            valType = ValueType::STRING;
    }

    std::string string() const;
};

using ValueNodeSP = std::shared_ptr<ValueNode>;

class CommandNode : public ASTNode {
public:
    CommandType cmdType;
    std::vector<ValueNodeSP> args;

    CommandNode()
        : cmdType(CommandType::UNKNOWN)
        , args(std::vector<ValueNodeSP>()) {};
    CommandNode(const std::string &c)
        : cmdType(mapGet(mapToCmd, c, CommandType::UNKNOWN))
        , args(std::vector<ValueNodeSP>()) {};
    CommandNode(CommandType c)
        : cmdType(c)
        , args(std::vector<ValueNodeSP>()) {};

    NodeType getType() const { return NodeType::COMMAND; };
    std::string string() const;

    void addArg(ValueNodeSP &a) { args.push_back(a); };
};

using ASTNodeSP = std::shared_ptr<ASTNode>;
using CommandNodeSP = std::shared_ptr<CommandNode>;
