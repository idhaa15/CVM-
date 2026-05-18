#include "compiler.h"
#include <stdexcept>

void Compiler::emitByte(uint8_t b) {
    bytecode.push_back(b);
}

void Compiler::emitInt(int value) {
    // store int as 4 bytes, little-endian
    bytecode.push_back((value >>  0) & 0xFF);
    bytecode.push_back((value >>  8) & 0xFF);
    bytecode.push_back((value >> 16) & 0xFF);
    bytecode.push_back((value >> 24) & 0xFF);
}

int Compiler::resolveVar(const std::string& name) {
    if (varMap.find(name) == varMap.end())
        varMap[name] = varCount++;
    return varMap[name];
}

// emit a jump instruction with a placeholder offset, return position to patch
int Compiler::emitJump(OpCode op) {
    emitByte((uint8_t)op);
    int pos = bytecode.size();
    emitInt(0); // placeholder
    return pos;
}

// go back and write the real jump destination
void Compiler::patchJump(int patchPos) {
    int dest = bytecode.size();
    bytecode[patchPos + 0] = (dest >>  0) & 0xFF;
    bytecode[patchPos + 1] = (dest >>  8) & 0xFF;
    bytecode[patchPos + 2] = (dest >> 16) & 0xFF;
    bytecode[patchPos + 3] = (dest >> 24) & 0xFF;
}

void Compiler::compile(const std::vector<std::unique_ptr<ASTNode>>& stmts) {
    for (auto& stmt : stmts)
        compileNode(stmt.get());
    emitByte((uint8_t)OpCode::HALT);
}

void Compiler::compileNode(ASTNode* node) {

    if (auto* n = dynamic_cast<NumberNode*>(node)) {
        emitByte((uint8_t)OpCode::PUSH);
        emitInt(n->value);
        return;
    }

    if (auto* n = dynamic_cast<BoolNode*>(node)) {
        emitByte((uint8_t)OpCode::PUSH);
        emitInt(n->value ? 1 : 0);
        return;
    }

    if (auto* n = dynamic_cast<VarNode*>(node)) {
        int slot = resolveVar(n->name);
        emitByte((uint8_t)OpCode::LOAD);
        emitInt(slot);
        return;
    }

    if (auto* n = dynamic_cast<BinOpNode*>(node)) {
        compileNode(n->left.get());
        compileNode(n->right.get());
        if      (n->op == "+")  emitByte((uint8_t)OpCode::ADD);
        else if (n->op == "-")  emitByte((uint8_t)OpCode::SUB);
        else if (n->op == "*")  emitByte((uint8_t)OpCode::MUL);
        else if (n->op == "/")  emitByte((uint8_t)OpCode::DIV);
        else if (n->op == "==") emitByte((uint8_t)OpCode::EQ);
        else if (n->op == "!=") emitByte((uint8_t)OpCode::NEQ);
        else if (n->op == "<")  emitByte((uint8_t)OpCode::LT);
        else if (n->op == ">")  emitByte((uint8_t)OpCode::GT);
        return;
    }

    if (auto* n = dynamic_cast<AssignNode*>(node)) {
        compileNode(n->value.get());
        int slot = resolveVar(n->name);
        emitByte((uint8_t)OpCode::STORE);
        emitInt(slot);
        return;
    }

    if (auto* n = dynamic_cast<PrintNode*>(node)) {
        compileNode(n->expr.get());
        emitByte((uint8_t)OpCode::PRINT);
        return;
    }

    if (auto* n = dynamic_cast<InputNode*>(node)) {
        int slot = resolveVar(n->name);
        emitByte((uint8_t)OpCode::INPUT);
        emitInt(slot);
        return;
    }

    if (auto* n = dynamic_cast<IfNode*>(node)) {
        // compile condition
        compileNode(n->condition.get());
        // emit jump-if-false over then block
        int jumpToElse = emitJump(OpCode::JUMP_IF_FALSE);

        for (auto& s : n->thenBlock) compileNode(s.get());

        if (!n->elseBlock.empty()) {
            int jumpOverElse = emitJump(OpCode::JUMP);
            patchJump(jumpToElse);
            for (auto& s : n->elseBlock) compileNode(s.get());
            patchJump(jumpOverElse);
        } else {
            patchJump(jumpToElse);
        }
        return;
    }

    if (auto* n = dynamic_cast<WhileNode*>(node)) {
        int loopStart = bytecode.size();
        compileNode(n->condition.get());
        int jumpOut = emitJump(OpCode::JUMP_IF_FALSE);
        for (auto& s : n->body) compileNode(s.get());
        // jump back to condition
        emitByte((uint8_t)OpCode::JUMP);
        emitInt(loopStart);
        patchJump(jumpOut);
        return;
    }

    throw std::runtime_error("Unknown AST node in compiler");
}