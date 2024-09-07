#pragma once

#include <memory>
#include <ostream>
#include <string>

enum class TokenType {
    COMMAND,
    IDENTIIFER,
    NUMBER,
    STRING,
    LIST_START,
    LIST_END,
    DELIMITER,
    END,
    UNKNOWN
};

struct Token {
    const TokenType type;
    std::string value;

    Token()
        : type(TokenType::UNKNOWN)
        , value("") {};
    Token(const TokenType &type)
        : type(type)
        , value("") {};
    Token(const TokenType &type, const std::string &s)
        : type(type)
        , value(s) {};
    Token(const TokenType &type, std::string &&s)
        : type(type)
        , value(std::move(s)) {};
    Token(const TokenType &type, char c)
        : type(type)
        , value(std::string(1, c)) {};

    friend std::ostream &operator<<(std::ostream &os, const Token &t) {
        os << "{token: ";
        switch (t.type) {
            case TokenType::COMMAND: os << "CMD"; break;
            case TokenType::IDENTIIFER: os << "IDENTIFIER"; break;
            case TokenType::NUMBER: os << "NUMBER"; break;
            case TokenType::STRING: os << "STRING"; break;
            case TokenType::LIST_START: os << "LIST_START"; break;
            case TokenType::LIST_END: os << "LIST_END"; break;
            case TokenType::DELIMITER: os << "DELIMITER"; break;
            case TokenType::END: os << "END"; break;
            case TokenType::UNKNOWN:
            default: os << "UNKNOWN"; break;
        }
        os << ", value: `" << t.value << "`}";
        return os;
    };
};

using TokenSP = std::shared_ptr<Token>;
