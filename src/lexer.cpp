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
            case WHITESPACE:
                it_++;
                continue;
                break;
            case BACKSLASH: tokens.push_back(lexCommand_()); break;
            case UNDERSCORE: tokens.push_back(lexIdentifier_()); break;
            case SINGLE_QUOTE:
            case DOUBLE_QUOTE: tokens.push_back(lexString_()); break;
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

                if (isdigit(c) || c == '-' || c == '+') {
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

Token Lexer::lexCommand_() {
    std::string s;
    while (it_ != inp_end_ && *it_ != WHITESPACE) {
        s.push_back(toupper(*it_));
        it_++;
    }
    return Token(TokenType::COMMAND, s);
}

Token Lexer::lexIdentifier_() {
    std::string s;
    while (it_ != inp_end_ && (isalpha(*it_) || *it_ == UNDERSCORE)) {
        s.push_back(*it_);
        it_++;
    }
    return Token(TokenType::IDENTIIFER, s);
}

Token Lexer::lexNumber_() {
    std::string s;
    while (it_ != inp_end_ && (isdigit(*it_) || *it_ == '-' || *it_ == '+')) {
        s.push_back(*it_);
        it_++;
    }
    return Token(TokenType::NUMBER, s);
}

Token Lexer::lexString_() {
    std::string s;

    const char &toMatch = *it_;
    s.push_back(*it_++); // Start quote
    while (it_ != inp_end_ && *it_ != toMatch) {
        s.push_back(*it_);
        it_++;
    }
    s.push_back(*it_++); // End quote

    if (*(s.end() - 1) != toMatch) {
        return Token(TokenType::UNKNOWN, s);
    }
    return Token(TokenType::STRING, s);
}

std::ostream &operator<<(std::ostream &os, const Token &t) {
    os << "{type: ";
    switch (t.type) {
        case TokenType::COMMAND: os << "CMD"; break;
        case TokenType::IDENTIIFER: os << "IDENTIFIER"; break;
        case TokenType::NUMBER: os << "NUMBER"; break;
        case TokenType::STRING: os << "STRING"; break;
        case TokenType::LIST_START: os << "LIST_START"; break;
        case TokenType::LIST_END: os << "LIST_END"; break;
        case TokenType::END: os << "END"; break;
        case TokenType::UNKNOWN:
        default: os << "UNKNOWN"; break;
    }
    os << ", value: `" << t.value << "`}";
    return os;
}
