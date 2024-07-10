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

    virtual NodeType getNodeType() const = 0;

    virtual std::string string() const = 0;
    friend std::ostream &operator<<(std::ostream &os, const ASTNode &t) {
        os << t.string();
        return os;
    };
};

class NilNode : public ASTNode {
    NodeType getNodeType() const { return NodeType::NIL; };
    std::string string() { return "{node: nil, value: nil}"; }
};

class ValueNode : public ASTNode {
private:
    ValueType valType_;

public:
    StoreValueSP value;

    ValueNode(StoreValueSP);

    NodeType getNodeType() const { return NodeType::VALUE; };
    std::string string() const;

    // Identifiers are also considered strings internally, call this to mark it as such.
    void setAsIdentifier() { valType_ = ValueType::IDENTIFIER; }
    ValueType getValueType() const { return valType_; }
};

using ValueNodeSP = std::shared_ptr<ValueNode>;

class CommandNode : public ASTNode {
private:
    CommandType cmdType_;
    std::vector<ValueNodeSP> args_;

public:
    CommandNode()
        : cmdType_(CommandType::UNKNOWN)
        , args_(std::vector<ValueNodeSP>()) {};
    CommandNode(const std::string &c)
        : cmdType_(mapGet(mapToCmd, c, CommandType::UNKNOWN))
        , args_(std::vector<ValueNodeSP>()) {};
    CommandNode(CommandType c)
        : cmdType_(c)
        , args_(std::vector<ValueNodeSP>()) {};

    NodeType getNodeType() const { return NodeType::COMMAND; };
    std::string string() const;

    CommandType getCmdType() const { return cmdType_; }
    void addArg(ValueNodeSP &a) { args_.push_back(a); };
    std::vector<ValueNodeSP> &getArgs() { return args_; };
};

using ASTNodeSP = std::shared_ptr<ASTNode>;
using CommandNodeSP = std::shared_ptr<CommandNode>;
