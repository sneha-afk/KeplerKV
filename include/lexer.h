#pragma once

#include "token.h"

#include <memory>
#include <string>
#include <vector>

class Lexer {
public:
    Lexer() { tokens = std::vector<std::shared_ptr<Token>>(2); };

    std::vector<std::shared_ptr<Token>> tokens;
    std::vector<std::shared_ptr<Token>> &tokenize(std::string &);

private:
    std::string input_;
    std::string::iterator it_;
    std::string::iterator iend_;

    std::shared_ptr<Token> lexCommand_();
    std::shared_ptr<Token> lexIdentifier_();
    std::shared_ptr<Token> lexNumber_();
    std::shared_ptr<Token> lexString_();
};
