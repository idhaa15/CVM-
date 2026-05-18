#pragma once
#include "parser.h"
#include <vector>
#include <unordered_map>
#include <cstdint>

enum class OpCode : uint8_t {
    PUSH,           // push constant int onto stack
    POP,            // discard top of stack
    ADD, SUB, MUL, DIV,
    EQ, NEQ, LT, GT,
    STORE,          // store top of stack into variable slot
    LOAD,           // load variable slot onto stack
    PRINT,          // pop and print top of stack
    INPUT,          // read int from stdin, store into variable slot
    JUMP,           // unconditional jump
    JUMP_IF_FALSE,  // pop, jump if 0
    HALT
};

class Compiler {
public:
    std::vector<uint8_t> bytecode;
    std::unordered_map<std::string, int> varMap;
    int varCount = 0;

    void compile(const std::vector<std::unique_ptr<ASTNode>>& stmts);

private:
    void compileNode(ASTNode* node);

    // helpers for jump patching
    int emitJump(OpCode op);
    void patchJump(int offset);
    void emitByte(uint8_t b);
    void emitInt(int value);
    int resolveVar(const std::string& name);
};