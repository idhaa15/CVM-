#include "lexer.h"
#include <stdexcept>

Lexer::Lexer(const std::string& source) : src(source), pos(0) {}

char Lexer::current() { return pos < src.size() ? src[pos] : '\0'; }
char Lexer::peek()    { return pos + 1 < src.size() ? src[pos + 1] : '\0'; }
void Lexer::advance() { pos++; }

void Lexer::skipWhitespace() {
    while (current() == ' ' || current() == '\n' ||
           current() == '\r' || current() == '\t')
        advance();
}

Token Lexer::readNumber() {
    std::string num;
    while (isdigit(current())) { num += current(); advance(); }
    return { TokenType::NUMBER, num };
}

Token Lexer::readIdentifierOrKeyword() {
    std::string word;
    while (isalnum(current()) || current() == '_') { word += current(); advance(); }

    if (word == "let")   return { TokenType::LET,        word };
    if (word == "if")    return { TokenType::IF,         word };
    if (word == "else")  return { TokenType::ELSE,       word };
    if (word == "while") return { TokenType::WHILE,      word };
    if (word == "print") return { TokenType::PRINT,      word };
    if (word == "input") return { TokenType::INPUT,      word };
    if (word == "true")  return { TokenType::BOOL_TRUE,  word };
    if (word == "false") return { TokenType::BOOL_FALSE, word };

    return { TokenType::IDENTIFIER, word };
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (true) {
        skipWhitespace();
        char c = current();

        if (c == '\0') { tokens.push_back({ TokenType::EOF_T, "" }); break; }

        if (isdigit(c))  { tokens.push_back(readNumber());              continue; }
        if (isalpha(c))  { tokens.push_back(readIdentifierOrKeyword()); continue; }

        switch (c) {
            case '+': tokens.push_back({ TokenType::PLUS,      "+" }); advance(); break;
            case '-': tokens.push_back({ TokenType::MINUS,     "-" }); advance(); break;
            case '*': tokens.push_back({ TokenType::STAR,      "*" }); advance(); break;
            case '/': tokens.push_back({ TokenType::SLASH,     "/" }); advance(); break;
            case '(': tokens.push_back({ TokenType::LPAREN,    "(" }); advance(); break;
            case ')': tokens.push_back({ TokenType::RPAREN,    ")" }); advance(); break;
            case '{': tokens.push_back({ TokenType::LBRACE,    "{" }); advance(); break;
            case '}': tokens.push_back({ TokenType::RBRACE,    "}" }); advance(); break;
            case ';': tokens.push_back({ TokenType::SEMICOLON, ";" }); advance(); break;
            case '=':
                if (peek() == '=') { tokens.push_back({ TokenType::EQEQ, "==" }); advance(); advance(); }
                else               { tokens.push_back({ TokenType::EQ,   "="  }); advance(); }
                break;
            case '!':
                if (peek() == '=') { tokens.push_back({ TokenType::NEQ, "!=" }); advance(); advance(); }
                else throw std::runtime_error("Unexpected character: !");
                break;
            case '<': tokens.push_back({ TokenType::LT, "<" }); advance(); break;
            case '>': tokens.push_back({ TokenType::GT, ">" }); advance(); break;
            default:
                throw std::runtime_error(std::string("Unexpected character: ") + c);
        }
    }

    return tokens;
}