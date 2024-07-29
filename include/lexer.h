#pragma once

#include "token.h"

#include <string>
#include <vector>

class Lexer {
public:
    std::vector<TokenSP> tokens;

    Lexer() { tokens = std::vector<TokenSP>(); };

    std::vector<TokenSP> &tokenize(std::string &);

private:
    std::string input_;
    std::string::iterator it_;
    std::string::iterator iend_;

    TokenSP lexCommand_();
    TokenSP lexIdentifier_();
    TokenSP lexNumber_();
    TokenSP lexString_();
};
