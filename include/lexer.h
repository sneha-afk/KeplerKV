#pragma once

#include <vector>
#include <string>

enum class TokenType { COMMAND, IDENTIIFER, VALUE };

class Token {
public:
    TokenType type;
    std::string value;
};

class Lexer {
public:
    std::vector<Token> tokenize(std::string &);
};
