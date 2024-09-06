#pragma once

#include "store_value.h"
#include "util.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

// clang-format off
enum class NodeType { COMMAND, NIL, INT, FLOAT, STRING, LIST, IDENTIFIER };

enum class CommandType {
    SET,        GET,            DELETE,
    UPDATE,     UNKNOWN,        QUIT,
    LIST,       RESOLVE,        SAVE,
    LOAD,       CLEAR,          RENAME,
    INCR,       DECR,           APPEND,
    PREPEND,    STATS,          SEARCH,
};
// clang-format on

static const std::unordered_map<std::string, CommandType> mapToCmd = { { "SET", CommandType::SET },
    { "S", CommandType::SET }, { "GET", CommandType::GET }, { "G", CommandType::GET },
    { "DELETE", CommandType::DELETE }, { "DEL", CommandType::DELETE }, { "D", CommandType::DELETE },
    { "UPDATE", CommandType::UPDATE }, { "U", CommandType::UPDATE }, { "Q", CommandType::QUIT },
    { "QUIT", CommandType::QUIT }, { "L", CommandType::LIST }, { "LS", CommandType::LIST },
    { "LIST", CommandType::LIST }, { "R", CommandType::RESOLVE }, { "RES", CommandType::RESOLVE },
    { "RESOLVE", CommandType::RESOLVE }, { "SAVE", CommandType::SAVE },
    { "LOAD", CommandType::LOAD }, { "CLEAR", CommandType::CLEAR },
    { "RENAME", CommandType::RENAME }, { "RN", CommandType::RENAME }, { "INCR", CommandType::INCR },
    { "DECR", CommandType::DECR }, { "APPEND", CommandType::APPEND },
    { "PREPEND", CommandType::PREPEND }, { "STATS", CommandType::STATS },
    { "SEARCH", CommandType::SEARCH } };

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

using ASTNodeSP = std::shared_ptr<ASTNode>;

class NilNode : public ASTNode {
    NodeType getNodeType() const { return NodeType::NIL; };
    std::string string() { return "{node: nil, value: nil}"; }
};

class CommandASTNode : public ASTNode {
public:
    CommandASTNode()
        : cmdType_(CommandType::UNKNOWN)
        , args_(std::vector<ValueNodeSP>()) {};
    CommandASTNode(const std::string &c)
        : cmdType_(mapGet(mapToCmd, c, CommandType::UNKNOWN))
        , args_(std::vector<ValueNodeSP>()) {};
    CommandASTNode(CommandType c)
        : cmdType_(c)
        , args_(std::vector<ValueNodeSP>()) {};

    NodeType getNodeType() const { return NodeType::COMMAND; };
    std::string string() const override;

    CommandType getCmdType() const { return cmdType_; }
    void addArg(ValueNodeSP &a) { args_.push_back(a); };
    std::vector<ValueNodeSP> &getArgs() { return args_; };

private:
    CommandType cmdType_;
    std::vector<ValueNodeSP> args_;
};

using CommandNodeSP = std::shared_ptr<CommandASTNode>;

class ValueASTNode : public ASTNode {
public:
    virtual StoreValueSP evaluate() const = 0;
};

using ValueNodeSP = std::shared_ptr<ValueASTNode>;

class IntASTNode : public ValueASTNode {
public:
    IntASTNode(int i)
        : value_(i) {};

    NodeType getNodeType() const { return NodeType::INT; }
    std::string string() const override;

private:
    int value_;
};

class FloatASTNode : public ValueASTNode {
public:
    FloatASTNode(float f)
        : value_(f) {};

    NodeType getNodeType() const { return NodeType::FLOAT; }
    std::string string() const override;

private:
    float value_;
};

class StringASTNode : public ValueASTNode {
public:
    StringASTNode(std::string s)
        : value_(s) {};

    NodeType getNodeType() const { return NodeType::STRING; }
    std::string string() const override;

private:
    std::string value_;
};

class IdentifierASTNode : public ValueASTNode {
public:
    IdentifierASTNode(std::string s)
        : value_(s) {};

    NodeType getNodeType() const { return NodeType::IDENTIFIER; }
    std::string string() const override;

private:
    std::string value_;
};

class ListASTNode : public ValueASTNode {
public:
    ListASTNode(std::vector<ValueNodeSP> &l)
        : value_(l) {};

    void addElem(ValueNodeSP e) { value_.push_back(std::move(e)); }

    NodeType getNodeType() const { return NodeType::LIST; }
    std::string string() const override;

private:
    std::vector<ValueNodeSP> value_;
};
