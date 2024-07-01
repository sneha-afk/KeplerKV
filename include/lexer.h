#pragma once

#include <ostream>
#include <string>
#include <vector>

#define WHITESPACE   ' '
#define BACKSLASH    '\\'
#define SINGLE_QUOTE '\''
#define DOUBLE_QUOTE '\"'
#define UNDERSCORE   '_'
#define COMMA        ','

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

class Token {
public:
    TokenType type;
    std::string value;

    Token(TokenType type, std::string value)
        : type(type)
        , value(value) {};
    Token(TokenType type, char c)
        : type(type)
        , value(std::string(1, c)) {};

    friend std::ostream &operator<<(std::ostream &os, const Token &t);
};

class Lexer {
public:
    std::vector<Token> tokenize(std::string &);

private:
    std::string input_;
    std::string::iterator it_;
    std::string::iterator inp_end_;

    Token lexCommand_();
    Token lexIdentifier_();
    Token lexNumber_();
    Token lexString_();
};
