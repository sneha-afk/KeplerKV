#include "lexer.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>
#include <vector>

std::vector<Token> Lexer::tokenize(std::string &query) {
    if (query.empty()) return {};

    std::vector<Token> tokens;
    input_ = query;
    it_ = query.begin();
    inp_end_ = query.end();
    while (it_ != inp_end_) {
        const char &c = *it_;

        switch (c) {
            case ' ':
                it_++;
                continue;
                break;
            case '\\': tokens.push_back(lexCommand_()); break;
            case '_': tokens.push_back(lexIdentifier_()); break;
            case '"':
            case '\'': tokens.push_back(lexString_()); break;
            case '[':
                tokens.push_back(Token(TokenType::LIST_START, c));
                it_++;
                break;
            case ']':
                tokens.push_back(Token(TokenType::LIST_END, c));
                it_++;
                break;
            default:
                if (isalpha(c)) {
                    tokens.push_back(lexIdentifier_());
                    break;
                }

                if (isdigit(c) || c == '-') {
                    tokens.push_back(lexNumber_());
                    break;
                }

                tokens.push_back(Token(TokenType::UNKNOWN, c));
                it_++;
                break;
        }
    }

    tokens.push_back(Token(TokenType::END, ""));
    return tokens;
}
