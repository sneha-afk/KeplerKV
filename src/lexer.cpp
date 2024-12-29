#include "lexer.h"

#include <cctype>

#define WHITESPACE   ' '
#define BACKSLASH    '\\'
#define SINGLE_QUOTE '\''
#define DOUBLE_QUOTE '\"'
#define UNDERSCORE   '_'
#define COMMA        ','
#define SEMICOLON    ';'
#define DASH         '-'
#define PLUS         '+'
#define PERIOD       '.'
#define NULL_CHAR    '\0'

// Returns current character (or null char if no more) and increments.
char Lexer::curr_() {
    if (it_ == iend_) return '\0';
    return *it_++;
}

// Returns current character (or null if no more), does NOT increment.
char Lexer::peek_() {
    if (it_ == iend_) return '\0';
    return *it_;
}

// Returns next character (or null if no more), does NOT increment.
char Lexer::peekNext_() {
    if ((it_ + 1) == iend_) return '\0';
    return *(it_ + 1);
}

std::vector<TokenSP> &Lexer::tokenize(std::string &query) {
    tokens.clear();
    if (query.empty()) return tokens;

    it_ = query.begin();
    iend_ = query.end();

    char c;
    while ((c = peek_()) != NULL_CHAR) {
        switch (c) {
            case NULL_CHAR:
            case WHITESPACE: it_++; break;
            case COMMA:
                tokens.push_back(std::make_shared<Token>(TokenType::DELIMITER, curr_()));
                break;
            case SEMICOLON:
                tokens.push_back(std::make_shared<Token>(TokenType::END, curr_()));
                break;
            case BACKSLASH: tokens.push_back(lexCommand_()); break;
            case UNDERSCORE: tokens.push_back(lexIdentifier_()); break;
            case SINGLE_QUOTE:
            case DOUBLE_QUOTE: tokens.push_back(lexString_()); break;
            case '[':
                tokens.push_back(std::make_shared<Token>(TokenType::LIST_START, curr_()));
                break;
            case ']':
                tokens.push_back(std::make_shared<Token>(TokenType::LIST_END, curr_()));
                break;
            default:
                // Identifiers start with underscores or letters
                if (isalpha(c)) {
                    tokens.push_back(lexIdentifier_());
                    break;
                }

                // Signed decimals and integers
                if (isdigit(c) || c == DASH || c == PLUS || c == PERIOD) {
                    tokens.push_back(lexNumber_());
                    break;
                }

                // Unknown tokens
                tokens.push_back(std::make_shared<Token>(TokenType::UNKNOWN, curr_()));
                break;
        }
    }

    tokens.push_back(std::make_shared<Token>(TokenType::END, SEMICOLON));
    return tokens;
}

TokenSP Lexer::lexCommand_() {
    std::string s;

    char c;
    while ((c = peek_()) != NULL_CHAR && c != WHITESPACE && c != SEMICOLON) {
        // Represent as uppercase internally without backslashes
        c = curr_();
        if (c == BACKSLASH) {
            continue;
        }
        s.push_back(toupper(c));
    }

    return std::make_shared<Token>(TokenType::COMMAND, std::move(s));
}

TokenSP Lexer::lexIdentifier_() {
    std::string s;

    char c;
    while ((c = peek_()) != NULL_CHAR && (isalnum(c) || c == UNDERSCORE)) {
        s.push_back(curr_());
    }
    return std::make_shared<Token>(TokenType::IDENTIFIER, std::move(s));
}

// Numbers have digits, at most one sign, and at most one decimal
TokenSP Lexer::lexNumber_() {
    std::string s;

    // Flags for symbols that can only appear once
    bool signF = false;
    bool decimalF = false;

    char c;
    while ((c = peek_()) != NULL_CHAR && (isdigit(c) || c == DASH || c == PLUS || c == PERIOD)) {
        if (c == DASH || c == PLUS) {
            // Not supporting arithmetic expressions for now
            if (signF || !s.empty())
                return std::make_shared<Token>(TokenType::UNKNOWN, std::move(s));
            else
                signF = true;
        }

        if (c == PERIOD) {
            if (decimalF)
                return std::make_shared<Token>(TokenType::UNKNOWN, std::move(s));
            else
                decimalF = true;
        }

        s.push_back(curr_());
    }
    return std::make_shared<Token>(TokenType::NUMBER, std::move(s));
}

// Valid strings should be denoted with quotations around the string
TokenSP Lexer::lexString_() {
    std::string s;

    const char &quote = *it_;
    s.push_back(*it_++); // Start quote

    char c;
    while ((c = peek_()) != NULL_CHAR && c != quote) {
        s.push_back(curr_());
    }
    if ((c = peek_()) != NULL_CHAR) s.push_back(curr_()); // End quote

    if (*(s.end() - 1) != quote) return std::make_shared<Token>(TokenType::UNKNOWN, s);
    return std::make_shared<Token>(TokenType::STRING, std::move(s));
}
