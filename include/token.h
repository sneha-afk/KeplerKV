#pragma once

#include <memory>
#include <ostream>
#include <string>

enum class TokenType {
    COMMAND,
    IDENTIFIER,
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

    std::string string() const {
        std::string s = "{token: ";
        switch (type) {
            case TokenType::COMMAND: s += "CMD"; break;
            case TokenType::IDENTIFIER: s += "IDENTIFIER"; break;
            case TokenType::NUMBER: s += "NUMBER"; break;
            case TokenType::STRING: s += "STRING"; break;
            case TokenType::LIST_START: s += "LIST_START"; break;
            case TokenType::LIST_END: s += "LIST_END"; break;
            case TokenType::DELIMITER: s += "DELIMITER"; break;
            case TokenType::END: s += "END"; break;
            case TokenType::UNKNOWN:
            default: s += "UNKNOWN"; break;
        }
        s += ", value: `" + value + "`}";
        return s;
    }

    friend std::ostream &operator<<(std::ostream &os, const Token &t) {
        os << t.string();
        return os;
    };
};

using TokenSP = std::shared_ptr<Token>;
