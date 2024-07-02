#pragma once

#include <list>
#include <string>

enum class CommandType {
    SET,
    GET,
    DELETE,
    UPDATE,
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
};

struct CommandNode : SyntaxNode {
    CommandType cmd;
    std::list<SyntaxNode> args;
};

struct IntegerNode : SyntaxNode {
    int value;
    IntegerNode(int v)
        : value(v) {};
};

struct FloatNode : SyntaxNode {
    float value;
    FloatNode(float v)
        : value(v) {};
};

struct StringNode : SyntaxNode {
    std::string value;
    StringNode(std::string v)
        : value(v) {};
};

struct ListNode : SyntaxNode {
    std::list<SyntaxNode> value;
    ListNode(std::list<SyntaxNode> &v)
        : value(v) {};
};
