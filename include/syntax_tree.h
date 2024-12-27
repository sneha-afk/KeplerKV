#pragma once

#include "environment_interface.h"
#include "store.h"
#include "store_value.h"
#include "util.h"

#include <memory>
#include <string>
#include <unordered_map>
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
    BEGIN,      COMMIT,         ROLLBACK,
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
    { "SEARCH", CommandType::SEARCH }, { "BEGIN", CommandType::BEGIN },
    { "COMMIT", CommandType::COMMIT }, { "ROLLBACK", CommandType::ROLLBACK } };

class ASTNode {
public:
    virtual ~ASTNode() = default;

    virtual inline NodeType getNodeType() const = 0;

    virtual std::string string() const = 0;
    friend std::ostream &operator<<(std::ostream &os, const ASTNode &t) {
        os << t.string();
        return os;
    };
};

using ASTNodeSP = std::shared_ptr<ASTNode>;

class Nil : public ASTNode {
    inline NodeType getNodeType() const override { return NodeType::NIL; };
    std::string string() const override { return "{node: nil, value: nil}"; }
};

class Value : public ASTNode {
public:
    virtual StoreValueSP evaluate() const = 0;
};

using ValueSP = std::shared_ptr<Value>;

class IntNode : public Value {
public:
    IntNode()
        : value_(0) {};
    IntNode(int i)
        : value_(i) {};

    inline NodeType getNodeType() const override { return NodeType::INT; }
    std::string string() const override;
    StoreValueSP evaluate() const override;

private:
    int value_;
};

class FloatNode : public Value {
public:
    FloatNode()
        : value_(0) {};
    FloatNode(float f)
        : value_(f) {};

    inline NodeType getNodeType() const override { return NodeType::FLOAT; }
    std::string string() const override;
    StoreValueSP evaluate() const override;

private:
    float value_;
};

class StringNode : public Value {
public:
    StringNode()
        : value_("") {};
    StringNode(std::string s)
        : value_(s) {};

    inline NodeType getNodeType() const override { return NodeType::STRING; }
    std::string string() const override;
    StoreValueSP evaluate() const override;

protected:
    std::string value_;
};

class IdentifierNode : public StringNode {
public:
    IdentifierNode()
        : StringNode() {};
    IdentifierNode(std::string s)
        : StringNode(s) {};

    inline NodeType getNodeType() const override { return NodeType::IDENTIFIER; }
    std::string string() const override;
    StoreValueSP evaluate() const override;
};

class ListNode : public Value {
public:
    ListNode()
        : value_(std::vector<ValueSP>()) {};
    ListNode(std::vector<ValueSP> &l)
        : value_(l) {};

    inline void addNode(ValueSP e) { value_.push_back(std::move(e)); }

    inline NodeType getNodeType() const override { return NodeType::LIST; }
    std::string string() const override;
    StoreValueSP evaluate() const override;

private:
    std::vector<ValueSP> value_;
};

class Command : public ASTNode {
public:
    Command()
        : cmdType_(CommandType::UNKNOWN)
        , args_(std::vector<ValueSP>()) {};
    Command(const std::string &c)
        : cmdType_(mapGet(mapToCmd, c, CommandType::UNKNOWN))
        , args_(std::vector<ValueSP>()) {};
    Command(const CommandType &c)
        : cmdType_(c)
        , args_(std::vector<ValueSP>()) {};

    inline NodeType getNodeType() const override { return NodeType::COMMAND; }
    std::string string() const override;

    inline CommandType getCmdType() const { return cmdType_; }
    inline void addArg(ValueSP &a) { args_.push_back(std::move(a)); }
    inline std::vector<ValueSP> &getArgs() { return args_; }
    inline std::size_t numArgs() const { return args_.size(); }

    // Validates the syntax and semantics of a command.
    // Not all commands have syntax to validate, so default returns true.
    virtual bool validate() const { return true; }

protected:
    const CommandType cmdType_;
    std::vector<ValueSP> args_;
};

// Store commands interact with the store itself.
class StoreCommand : public Command {
public:
    StoreCommand()
        : Command() { }
    StoreCommand(const std::string &c)
        : Command(c) { }
    StoreCommand(const CommandType &c)
        : Command(c) { }

    // execute() assumes the node has been validated.
    // Executing non-validated nodes can have undefined behavior.
    // Error-handling is the caller's responsibility.
    virtual void execute(Store &) const = 0;
};

// System commands do not interact with the store.
class EnvironmentInterface; // Forward declaration
class SystemCommand : public Command {
public:
    SystemCommand()
        : Command() { }
    SystemCommand(const std::string &c)
        : Command(c) { }
    SystemCommand(const CommandType &c)
        : Command(c) { }

    // execute() assumes the node has been validated.
    // Executing non-validated nodes can have undefined behavior.
    // Error-handling is the caller's responsibility.
    virtual void execute(EnvironmentInterface &) const = 0;
};

using CommandSP = std::shared_ptr<Command>;
using StoreCommandSP = std::shared_ptr<StoreCommand>;
using SystemCommandSP = std::shared_ptr<SystemCommand>;
using IntNodeSP = std::shared_ptr<IntNode>;
using FloatNodeSP = std::shared_ptr<FloatNode>;
using StringNodeSP = std::shared_ptr<StringNode>;
using IdentifierNodeSP = std::shared_ptr<IdentifierNode>;
using ListNodeSP = std::shared_ptr<ListNode>;
