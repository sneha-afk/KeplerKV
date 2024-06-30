#pragma once

#include <string>
#include <vector>

enum class TokenType {
    COMMAND,
    IDENTIIFER,
    PRIMITIVE,
    LIST_START,
    LIST_END,
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
    Token lexList_();
};
