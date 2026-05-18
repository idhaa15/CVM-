#pragma once
#include "lexer.h"
#include <vector>
#include <memory>
#include <string>

// ── AST Node types ──────────────────────────────────────────
struct ASTNode { virtual ~ASTNode() = default; };

struct NumberNode : ASTNode {
    int value;
    NumberNode(int v) : value(v) {}
};

struct BoolNode : ASTNode {
    bool value;
    BoolNode(bool v) : value(v) {}
};

struct VarNode : ASTNode {
    std::string name;
    VarNode(const std::string& n) : name(n) {}
};

struct BinOpNode : ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> left, right;
    BinOpNode(const std::string& op,
              std::unique_ptr<ASTNode> l,
              std::unique_ptr<ASTNode> r)
        : op(op), left(std::move(l)), right(std::move(r)) {}
};

struct AssignNode : ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> value;
    AssignNode(const std::string& n, std::unique_ptr<ASTNode> v)
        : name(n), value(std::move(v)) {}
};

struct PrintNode : ASTNode {
    std::unique_ptr<ASTNode> expr;
    PrintNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
};

struct InputNode : ASTNode {
    std::string name;
    InputNode(const std::string& n) : name(n) {}
};

struct IfNode : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> thenBlock;
    std::vector<std::unique_ptr<ASTNode>> elseBlock;
};

struct WhileNode : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> body;
};

// ── Parser ───────────────────────────────────────────────────
class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<ASTNode>> parse();

private:
    std::vector<Token> tokens;
    size_t pos;

    Token& current();
    Token& peek(int offset = 1);
    Token consume();
    Token expect(TokenType type, const std::string& errMsg);

    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseLet();
    std::unique_ptr<ASTNode> parsePrint();
    std::unique_ptr<ASTNode> parseInput();
    std::unique_ptr<ASTNode> parseIf();
    std::unique_ptr<ASTNode> parseWhile();
    std::vector<std::unique_ptr<ASTNode>> parseBlock();

    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseComparison();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
    std::unique_ptr<ASTNode> parsePrimary();
};