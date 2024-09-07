#include "lexer.h"

#include <cctype>

#define WHITESPACE   ' '
#define BACKSLASH    '\\'
#define SINGLE_QUOTE '\''
#define DOUBLE_QUOTE '\"'
#define UNDERSCORE   '_'
#define COMMA        ','
#define SEMICOLON    ';'

std::vector<TokenSP> &Lexer::tokenize(std::string &query) {
    tokens.clear();
    if (query.empty()) return tokens;

    it_ = query.begin();
    iend_ = query.end();
    while (it_ != iend_) {
        const char &c = *it_;
        switch (c) {
            case WHITESPACE: it_++; break;
            case COMMA:
                tokens.push_back(std::make_shared<Token>(TokenType::DELIMITER, c));
                it_++;
                break;
            case SEMICOLON:
                tokens.push_back(std::make_shared<Token>(TokenType::END, c));
                it_++;
                break;
            case BACKSLASH: tokens.push_back(lexCommand_()); break;
            case UNDERSCORE: tokens.push_back(lexIdentifier_()); break;
            case SINGLE_QUOTE:
            case DOUBLE_QUOTE: tokens.push_back(lexString_()); break;
            case '[':
                tokens.push_back(std::make_shared<Token>(TokenType::LIST_START, c));
                it_++;
                break;
            case ']':
                tokens.push_back(std::make_shared<Token>(TokenType::LIST_END, c));
                it_++;
                break;
            default:
                if (isalpha(c)) {
                    tokens.push_back(lexIdentifier_());
                    break;
                }

                if (isdigit(c) || c == '-' || c == '+' || c == '.') {
                    tokens.push_back(lexNumber_());
                    break;
                }

                tokens.push_back(std::make_shared<Token>(TokenType::UNKNOWN, c));
                it_++;
                break;
        }
    }

    tokens.push_back(std::make_shared<Token>(TokenType::END, SEMICOLON));
    return tokens;
}

TokenSP Lexer::lexCommand_() {
    std::string s;
    while (it_ != iend_ && *it_ != WHITESPACE && *it_ != SEMICOLON) {
        // Represent as uppercase internally
        if (*it_ != '\\') s.push_back(toupper(*it_));
        it_++;
    }
    return std::make_shared<Token>(TokenType::COMMAND, s);
}

TokenSP Lexer::lexIdentifier_() {
    std::string s;
    while (it_ != iend_ && (isalnum(*it_) || *it_ == UNDERSCORE)) {
        s.push_back(*it_);
        it_++;
    }
    return std::make_shared<Token>(TokenType::IDENTIIFER, s);
}

// Numbers have digits, at most one sign, and at most one decimal
TokenSP Lexer::lexNumber_() {
    std::string s;

    // Flags for symbols that can only appear once
    bool signF = false;
    bool decimalF = false;
    while (it_ != iend_ && (isdigit(*it_) || *it_ == '-' || *it_ == '+' || *it_ == '.')) {
        const char &c = *it_;
        if (c == '-' || c == '+') {
            // Not supporting arithmetic expressions for now
            if (signF || !s.empty())
                return std::make_shared<Token>(TokenType::UNKNOWN, s);
            else
                signF = true;
        }

        if (c == '.') {
            if (decimalF)
                return std::make_shared<Token>(TokenType::UNKNOWN, s);
            else
                decimalF = true;
        }

        s.push_back(c);
        it_++;
    }
    return std::make_shared<Token>(TokenType::NUMBER, s);
}

// Valid strings should be denoted with quotations around the string
TokenSP Lexer::lexString_() {
    std::string s;

    const char &quote = *it_;
    s.push_back(*it_++); // Start quote
    while (it_ != iend_ && *it_ != quote) {
        s.push_back(*it_);
        it_++;
    }
    if (it_ != iend_) s.push_back(*it_++); // End quote

    if (*(s.end() - 1) != quote) return std::make_shared<Token>(TokenType::UNKNOWN, s);
    return std::make_shared<Token>(TokenType::STRING, s);
}
