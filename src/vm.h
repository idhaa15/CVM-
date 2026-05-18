#pragma once
#include <vector>
#include <cstdint>

class VM {
public:
    void run(const std::vector<uint8_t>& bytecode, int numVars);

private:
    std::vector<int> stack;
    std::vector<int> vars;

    int  readInt(const std::vector<uint8_t>& bc, size_t& ip);
    void push(int val);
    int  pop();
};