#pragma once

#include <string>
#include <vector>

#include "token.h"

class Lexer {
public:
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
