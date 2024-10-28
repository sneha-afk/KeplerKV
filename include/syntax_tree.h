#pragma once

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

class NilASTNode : public ASTNode {
    inline NodeType getNodeType() const { return NodeType::NIL; };
    std::string string() { return "{node: nil, value: nil}"; }
};

class ValueASTNode : public ASTNode {
public:
    virtual StoreValueSP evaluate() const = 0;
};

using ValueASTNodeSP = std::shared_ptr<ValueASTNode>;

class IntASTNode : public ValueASTNode {
public:
    IntASTNode()
        : value_(0) {};
    IntASTNode(int i)
        : value_(i) {};

    inline NodeType getNodeType() const { return NodeType::INT; }
    std::string string() const override;
    StoreValueSP evaluate() const override;

private:
    int value_;
};

class FloatASTNode : public ValueASTNode {
public:
    FloatASTNode()
        : value_(0) {};
    FloatASTNode(float f)
        : value_(f) {};

    inline NodeType getNodeType() const { return NodeType::FLOAT; }
    std::string string() const override;
    StoreValueSP evaluate() const override;

private:
    float value_;
};

class StringASTNode : public ValueASTNode {
public:
    StringASTNode()
        : value_("") {};
    StringASTNode(std::string s)
        : value_(s) {};

    inline NodeType getNodeType() const { return NodeType::STRING; }
    std::string string() const override;
    StoreValueSP evaluate() const override;

protected:
    std::string value_;
};

class IdentifierASTNode : public StringASTNode {
public:
    IdentifierASTNode()
        : StringASTNode() {};
    IdentifierASTNode(std::string s)
        : StringASTNode(s) {};

    inline NodeType getNodeType() const { return NodeType::IDENTIFIER; }
    std::string string() const override;
    StoreValueSP evaluate() const override;
};

class ListValueASTNode : public ValueASTNode {
public:
    ListValueASTNode()
        : value_(std::vector<ValueASTNodeSP>()) {};
    ListValueASTNode(std::vector<ValueASTNodeSP> &l)
        : value_(l) {};

    inline void addNode(ValueASTNodeSP e) { value_.push_back(std::move(e)); }

    inline NodeType getNodeType() const { return NodeType::LIST; }
    std::string string() const override;
    StoreValueSP evaluate() const override;

private:
    std::vector<ValueASTNodeSP> value_;
};

class CommandASTNode : public ASTNode {
public:
    CommandASTNode()
        : cmdType_(CommandType::UNKNOWN)
        , args_(std::vector<ValueASTNodeSP>()) {};
    CommandASTNode(const std::string &c)
        : cmdType_(mapGet(mapToCmd, c, CommandType::UNKNOWN))
        , args_(std::vector<ValueASTNodeSP>()) {};
    CommandASTNode(const CommandType &c)
        : cmdType_(c)
        , args_(std::vector<ValueASTNodeSP>()) {};

    inline NodeType getNodeType() const { return NodeType::COMMAND; }
    std::string string() const override;

    inline CommandType getCmdType() const { return cmdType_; }
    inline void addArg(ValueASTNodeSP &a) { args_.push_back(std::move(a)); }
    inline std::vector<ValueASTNodeSP> &getArgs() { return args_; }
    inline std::size_t numArgs() const { return args_.size(); }

    // Validates the syntax and semantics of a command.
    // Not all commands have syntax to validate, so default returns true.
    virtual bool validate() const { return true; }

protected:
    const CommandType cmdType_;
    std::vector<ValueASTNodeSP> args_;
};

// Store commands interact with the store itself.
class StoreCmdASTNode : public CommandASTNode {
public:
    StoreCmdASTNode()
        : CommandASTNode() { }
    StoreCmdASTNode(const std::string &c)
        : CommandASTNode(c) { }
    StoreCmdASTNode(const CommandType &c)
        : CommandASTNode(c) { }

    // execute() assumes the node has been validated.
    // Executing non-validated nodes can have undefined behavior.
    // Error-handling is the caller's responsibility.
    virtual void execute(Store &s) const = 0;
};

// System commands do not interact with the store.
class SystemCmdASTNode : public CommandASTNode {
public:
    SystemCmdASTNode()
        : CommandASTNode() { }
    SystemCmdASTNode(const std::string &c)
        : CommandASTNode(c) { }
    SystemCmdASTNode(const CommandType &c)
        : CommandASTNode(c) { }

    // execute() assumes the node has been validated.
    // Executing non-validated nodes can have undefined behavior.
    // Error-handling is the caller's responsibility.
    virtual void execute() const = 0;
};

class SetCmdASTNode : public StoreCmdASTNode {
public:
    SetCmdASTNode()
        : StoreCmdASTNode(CommandType::SET) { }
    virtual bool validate() const override;
    virtual void execute(Store &s) const override;
};

class GetCmdASTNode : public StoreCmdASTNode {
public:
    GetCmdASTNode()
        : StoreCmdASTNode(CommandType::GET) { }
    virtual bool validate() const override;
    virtual void execute(Store &s) const override;
};

class ListCmdASTNode : public StoreCmdASTNode {
public:
    ListCmdASTNode()
        : StoreCmdASTNode(CommandType::LIST) { }
    virtual void execute(Store &s) const override;
};

using CommandASTNodeSP = std::shared_ptr<CommandASTNode>;
using IntASTNodeSP = std::shared_ptr<IntASTNode>;
using FloatASTNodeSP = std::shared_ptr<FloatASTNode>;
using StringASTNodeSP = std::shared_ptr<StringASTNode>;
using IdentifierASTNodeSP = std::shared_ptr<IdentifierASTNode>;
using ListValueASTNodeSP = std::shared_ptr<ListValueASTNode>;
