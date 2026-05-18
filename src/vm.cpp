#include "vm.h"
#include "compiler.h"  // for OpCode enum
#include <iostream>
#include <stdexcept>

int VM::readInt(const std::vector<uint8_t>& bc, size_t& ip) {
    int val = bc[ip]
            | (bc[ip+1] << 8)
            | (bc[ip+2] << 16)
            | (bc[ip+3] << 24);
    ip += 4;
    return val;
}

void VM::push(int val) { stack.push_back(val); }
int  VM::pop()  {
    if (stack.empty()) throw std::runtime_error("Stack underflow");
    int val = stack.back();
    stack.pop_back();
    return val;
}

void VM::run(const std::vector<uint8_t>& bytecode, int numVars) {
    vars.assign(numVars, 0);
    size_t ip = 0;

    while (ip < bytecode.size()) {
        OpCode op = (OpCode)bytecode[ip++];

        switch (op) {

            case OpCode::PUSH: {
                int val = readInt(bytecode, ip);
                push(val);
                break;
            }

            case OpCode::POP:
                pop();
                break;

            case OpCode::ADD: { int b = pop(), a = pop(); push(a + b); break; }
            case OpCode::SUB: { int b = pop(), a = pop(); push(a - b); break; }
            case OpCode::MUL: { int b = pop(), a = pop(); push(a * b); break; }
            case OpCode::DIV: {
                int b = pop(), a = pop();
                if (b == 0) throw std::runtime_error("Division by zero");
                push(a / b);
                break;
            }

            case OpCode::EQ:  { int b = pop(), a = pop(); push(a == b ? 1 : 0); break; }
            case OpCode::NEQ: { int b = pop(), a = pop(); push(a != b ? 1 : 0); break; }
            case OpCode::LT:  { int b = pop(), a = pop(); push(a <  b ? 1 : 0); break; }
            case OpCode::GT:  { int b = pop(), a = pop(); push(a >  b ? 1 : 0); break; }

            case OpCode::STORE: {
                int slot = readInt(bytecode, ip);
                vars[slot] = pop();
                break;
            }

            case OpCode::LOAD: {
                int slot = readInt(bytecode, ip);
                push(vars[slot]);
                break;
            }

            case OpCode::PRINT:
                std::cout << pop() << "\n";
                break;

            case OpCode::INPUT: {
                int slot = readInt(bytecode, ip);
                int val;
                std::cin >> val;
                vars[slot] = val;
                break;
            }

            case OpCode::JUMP: {
                int dest = readInt(bytecode, ip);
                ip = dest;
                break;
            }

            case OpCode::JUMP_IF_FALSE: {
                int dest = readInt(bytecode, ip);
                if (pop() == 0) ip = dest;
                break;
            }

            case OpCode::HALT:
                return;

            default:
                throw std::runtime_error("Unknown opcode");
        }
    }
}