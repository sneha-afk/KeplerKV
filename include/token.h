#pragma once

#include <memory>
#include <ostream>
#include <string>

#define WHITESPACE   ' '
#define BACKSLASH    '\\'
#define SINGLE_QUOTE '\''
#define DOUBLE_QUOTE '\"'
#define UNDERSCORE   '_'
#define COMMA        ','
#define SEMICOLON    ';'

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
    TokenType type;
    std::string value;

    Token()
        : type(TokenType::UNKNOWN)
        , value("") {};
    Token(TokenType type, std::string value)
        : type(type)
        , value(value) {};
    Token(TokenType type, char c)
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
