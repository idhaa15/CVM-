#include "parser.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

Token& Parser::current()          { return tokens[pos]; }
Token& Parser::peek(int offset)   { return tokens[pos + offset]; }
Token  Parser::consume()          { return tokens[pos++]; }

Token Parser::expect(TokenType type, const std::string& errMsg) {
    if (current().type != type)
        throw std::runtime_error(errMsg + " got: " + current().value);
    return consume();
}

// ── Top-level parse ──────────────────────────────────────────
std::vector<std::unique_ptr<ASTNode>> Parser::parse() {
    std::vector<std::unique_ptr<ASTNode>> stmts;
    while (current().type != TokenType::EOF_T)
        stmts.push_back(parseStatement());
    return stmts;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (current().type == TokenType::LET)   return parseLet();
    if (current().type == TokenType::PRINT) return parsePrint();
    if (current().type == TokenType::IF)    return parseIf();
    if (current().type == TokenType::WHILE) return parseWhile();
    throw std::runtime_error("Unknown statement: " + current().value);
}

// ── Statements ───────────────────────────────────────────────
std::unique_ptr<ASTNode> Parser::parseLet() {
    consume(); // eat 'let'
    std::string name = expect(TokenType::IDENTIFIER, "Expected variable name").value;
    expect(TokenType::EQ, "Expected '=' after variable name");

    // handle: let x = input;
    if (current().type == TokenType::INPUT) {
        consume(); // eat 'input'
        expect(TokenType::SEMICOLON, "Expected ';' after input");
        return std::make_unique<InputNode>(name);
    }

    auto val = parseExpression();
    expect(TokenType::SEMICOLON, "Expected ';' after assignment");
    return std::make_unique<AssignNode>(name, std::move(val));
}

std::unique_ptr<ASTNode> Parser::parsePrint() {
    consume(); // eat 'print'
    auto expr = parseExpression();
    expect(TokenType::SEMICOLON, "Expected ';' after print");
    return std::make_unique<PrintNode>(std::move(expr));
}


std::unique_ptr<ASTNode> Parser::parseIf() {
    consume(); // eat 'if'
    auto node = std::make_unique<IfNode>();
    node->condition = parseExpression();
    node->thenBlock = parseBlock();
    if (current().type == TokenType::ELSE) {
        consume(); // eat 'else'
        node->elseBlock = parseBlock();
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::parseWhile() {
    consume(); // eat 'while'
    auto node = std::make_unique<WhileNode>();
    node->condition = parseExpression();
    node->body = parseBlock();
    return node;
}

std::vector<std::unique_ptr<ASTNode>> Parser::parseBlock() {
    expect(TokenType::LBRACE, "Expected '{'");
    std::vector<std::unique_ptr<ASTNode>> stmts;
    while (current().type != TokenType::RBRACE &&
           current().type != TokenType::EOF_T)
        stmts.push_back(parseStatement());
    expect(TokenType::RBRACE, "Expected '}'");
    return stmts;
}

// ── Expressions (precedence climbing) ────────────────────────
std::unique_ptr<ASTNode> Parser::parseExpression() {
    return parseComparison();
}

std::unique_ptr<ASTNode> Parser::parseComparison() {
    auto left = parseTerm();
    while (current().type == TokenType::LT   ||
           current().type == TokenType::GT   ||
           current().type == TokenType::EQEQ ||
           current().type == TokenType::NEQ) {
        std::string op = consume().value;
        auto right = parseTerm();
        left = std::make_unique<BinOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseTerm() {
    auto left = parseFactor();
    while (current().type == TokenType::PLUS ||
           current().type == TokenType::MINUS) {
        std::string op = consume().value;
        auto right = parseFactor();
        left = std::make_unique<BinOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseFactor() {
    auto left = parsePrimary();
    while (current().type == TokenType::STAR ||
           current().type == TokenType::SLASH) {
        std::string op = consume().value;
        auto right = parsePrimary();
        left = std::make_unique<BinOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    TokenType t = current().type;

    if (t == TokenType::NUMBER) {
        int val = std::stoi(consume().value);
        return std::make_unique<NumberNode>(val);
    }
    if (t == TokenType::BOOL_TRUE) {
        consume();
        return std::make_unique<BoolNode>(true);
    }
    if (t == TokenType::BOOL_FALSE) {
        consume();
        return std::make_unique<BoolNode>(false);
    }
    if (t == TokenType::IDENTIFIER) {
        return std::make_unique<VarNode>(consume().value);
    }
    if (t == TokenType::LPAREN) {
        consume();
        auto expr = parseExpression();
        expect(TokenType::RPAREN, "Expected ')'");
        return expr;
    }
    throw std::runtime_error("Unexpected token in expression: " + current().value);
}