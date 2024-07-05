#pragma once

#include "token.h"

#include <string>
#include <vector>

class Lexer {
public:
    Lexer();
    std::vector<Token> tokens;
    std::vector<Token> &tokenize(std::string &);

private:
    std::string input_;
    std::string::iterator it_;
    std::string::iterator iend_;

    Token lexCommand_();
    Token lexIdentifier_();
    Token lexNumber_();
    Token lexString_();
};
