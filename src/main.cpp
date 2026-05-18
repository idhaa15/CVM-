#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.h"
#include "parser.h"
#include "compiler.h"
#include "vm.h"

// ── Debug: print AST ─────────────────────────────────────────
void printAST(ASTNode* node, int indent = 0) {
    std::string pad(indent * 2, ' ');
    if (auto* n = dynamic_cast<NumberNode*>(node))
        std::cout << pad << "Number(" << n->value << ")\n";
    else if (auto* n = dynamic_cast<BoolNode*>(node))
        std::cout << pad << "Bool(" << (n->value ? "true" : "false") << ")\n";
    else if (auto* n = dynamic_cast<VarNode*>(node))
        std::cout << pad << "Var(" << n->name << ")\n";
    else if (auto* n = dynamic_cast<BinOpNode*>(node)) {
        std::cout << pad << "BinOp(" << n->op << ")\n";
        printAST(n->left.get(),  indent + 1);
        printAST(n->right.get(), indent + 1);
    }
    else if (auto* n = dynamic_cast<AssignNode*>(node)) {
        std::cout << pad << "Assign(" << n->name << ")\n";
        printAST(n->value.get(), indent + 1);
    }
    else if (auto* n = dynamic_cast<PrintNode*>(node)) {
        std::cout << pad << "Print\n";
        printAST(n->expr.get(), indent + 1);
    }
    else if (auto* n = dynamic_cast<InputNode*>(node))
        std::cout << pad << "Input(" << n->name << ")\n";
    else if (auto* n = dynamic_cast<IfNode*>(node)) {
        std::cout << pad << "If\n";
        std::cout << pad << "  condition:\n";
        printAST(n->condition.get(), indent + 2);
        std::cout << pad << "  then:\n";
        for (auto& s : n->thenBlock) printAST(s.get(), indent + 2);
        if (!n->elseBlock.empty()) {
            std::cout << pad << "  else:\n";
            for (auto& s : n->elseBlock) printAST(s.get(), indent + 2);
        }
    }
    else if (auto* n = dynamic_cast<WhileNode*>(node)) {
        std::cout << pad << "While\n";
        std::cout << pad << "  condition:\n";
        printAST(n->condition.get(), indent + 2);
        std::cout << pad << "  body:\n";
        for (auto& s : n->body) printAST(s.get(), indent + 2);
    }
}

// ── Debug: print bytecode ────────────────────────────────────
void printBytecode(const std::vector<uint8_t>& bc) {
    const char* names[] = {
        "PUSH","POP","ADD","SUB","MUL","DIV",
        "EQ","NEQ","LT","GT",
        "STORE","LOAD","PRINT","INPUT",
        "JUMP","JUMP_IF_FALSE","HALT"
    };
    size_t i = 0;
    while (i < bc.size()) {
        uint8_t op = bc[i];
        std::cout << i << ": " << names[op];
        i++;
        // opcodes that are followed by a 4-byte int operand
        if (op == 0  || // PUSH
            op == 10 || // STORE
            op == 11 || // LOAD
            op == 13 || // INPUT
            op == 14 || // JUMP
            op == 15) { // JUMP_IF_FALSE
            int val = bc[i] | (bc[i+1]<<8) | (bc[i+2]<<16) | (bc[i+3]<<24);
            std::cout << " " << val;
            i += 4;
        }
        std::cout << "\n";
    }
}

// ── Run pipeline ─────────────────────────────────────────────
void run(const std::string& source, bool debug = false) {
    try {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        auto ast = parser.parse();

        if (debug) {
            std::cout << "\n=== AST ===\n";
            for (auto& node : ast) printAST(node.get());
        }

        Compiler compiler;
        compiler.compile(ast);

        if (debug) {
            std::cout << "\n=== BYTECODE ===\n";
            printBytecode(compiler.bytecode);
            std::cout << "\n=== OUTPUT ===\n";
        }

        VM vm;
        vm.run(compiler.bytecode, compiler.varCount);

    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

// ── REPL ─────────────────────────────────────────────────────
void repl(bool debug) {
    std::cout << "CVM++ REPL  (type 'exit' to quit)\n";
    std::string accumulated;
    std::string line;

    while (true) {
        std::cout << ">> ";
        if (!std::getline(std::cin, line)) break;
        if (line == "exit") break;
        if (line.empty()) continue;
        accumulated += line + "\n";
        run(accumulated, debug);
    }
}

// ── Entry point ──────────────────────────────────────────────
int main(int argc, char* argv[]) {
    bool debug = false;
    std::string filename;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--debug") debug = true;
        else filename = arg;
    }

    if (!filename.empty()) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Could not open file: " << filename << "\n";
            return 1;
        }
        std::stringstream buf;
        buf << file.rdbuf();
        run(buf.str(), debug);
    } else {
        repl(debug);
    }

    return 0;
}