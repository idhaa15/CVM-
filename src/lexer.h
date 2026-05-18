#pragma once
#include <string>
#include <vector>

enum class TokenType {
    // Literals
    NUMBER, BOOL_TRUE, BOOL_FALSE, IDENTIFIER,
    // Operators
    PLUS, MINUS, STAR, SLASH,
    EQEQ, NEQ, LT, GT,
    // Assignment
    EQ,
    // Delimiters
    LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON,
    // Keywords
    LET, IF, ELSE, WHILE, PRINT, INPUT,
    // Special
    EOF_T
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string src;
    size_t pos;

    char current();
    char peek();
    void advance();
    void skipWhitespace();
    Token readNumber();
    Token readIdentifierOrKeyword();
};