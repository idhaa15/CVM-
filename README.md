================================================================
  CVM++ — Stack-Based Virtual Machine & Custom Compiler
================================================================
  Language : C++17
  Platform : macOS / Linux
  Compiler : clang++ or g++
  Build     : GNU Make
================================================================


----------------------------------------------------------------
  REQUIREMENTS
----------------------------------------------------------------

  macOS / Linux:
  - clang++ (Apple clang 17+) or g++ (GCC 11+)
  - make

  Windows:
  - See "WINDOWS SETUP" section below


----------------------------------------------------------------
  WINDOWS SETUP
----------------------------------------------------------------

  CVM++ is written in standard C++17 and runs on Windows.
  Choose one of the following options:

  ── Option 1: WSL (Recommended) ──────────────────────────────

  WSL (Windows Subsystem for Linux) lets you run CVM++ exactly
  like on macOS/Linux.

  1. Open PowerShell as Administrator and run:
       wsl --install

  2. Restart your computer when prompted.

  3. Open the Ubuntu app from the Start menu and run:
       sudo apt update
       sudo apt install build-essential

  4. Navigate to the project folder and build:
       make
       ./cvm counter.cvm

  ── Option 2: MinGW-w64 (Native Windows g++) ─────────────────

  1. Download and install MinGW-w64:
       https://www.mingw-w64.org

  2. Open Command Prompt, navigate to the project folder,
     and compile manually (make is not required):

       g++ -std=c++17 -Wall src/main.cpp src/lexer.cpp ^
           src/parser.cpp src/compiler.cpp src/vm.cpp -o cvm.exe

  3. Run:
       cvm.exe counter.cvm
       cvm.exe --debug calculator.cvm

  ── Option 3: Visual Studio ──────────────────────────────────

  1. Open Visual Studio (2019 or later).
  2. Create a new Empty C++ Project.
  3. Add all files from the src/ folder to the project.
  4. Set C++ Language Standard to C++17 in Project Settings.
  5. Build and run.

  ── Option 4: Online Compiler (Zero Setup) ───────────────────

  Paste all source files into https://replit.com
  No installation required — runs entirely in the browser.


----------------------------------------------------------------
  BUILD INSTRUCTIONS (macOS / Linux / WSL)
----------------------------------------------------------------

  1. Open a terminal in the project root folder (cvm++)

  2. Run:
       make

  3. This produces the executable:
       ./cvm

  To clean the build:
       make clean


----------------------------------------------------------------
  RUNNING A SCRIPT FILE
----------------------------------------------------------------

  Syntax:
       ./cvm <filename.cvm>

  Examples:
       ./cvm calculator.cvm
       ./cvm counter.cvm
       ./cvm factorial.cvm

  - Where the script contains an 'input' statement, the
    program will pause and wait for you to type an integer
    and press Enter.

  - Output is printed to the terminal via 'print' statements.


----------------------------------------------------------------
  DEBUG MODE
----------------------------------------------------------------

  Syntax:
       ./cvm --debug <filename.cvm>

  Example:
       ./cvm --debug counter.cvm

  Debug mode prints three sections before execution:

    === AST ===
    (the Abstract Syntax Tree — the parsed structure of
     your source code)

    === BYTECODE ===
    (the compiled instructions with human-readable opcode
     names and operand values)

    === OUTPUT ===
    (the actual program output)

  This is useful for understanding exactly how your source
  code is compiled and what bytecode the VM executes.


----------------------------------------------------------------
  REPL (Interactive Mode)
----------------------------------------------------------------

  Launch with no arguments:
       ./cvm

  You will see:
       CVM++ REPL  (type 'exit' to quit)
       >>

  Type CVM++ statements and press Enter. All previous lines
  are remembered within the session.

  Example session:
       >> let x = 10 + 5;
       >> print x;
       15
       >> let y = x * 2;
       >> print y;
       30
       >> exit

  Note: For multi-statement programs, the file runner mode
  is recommended over the REPL.


----------------------------------------------------------------
  SAMPLE SCRIPTS
----------------------------------------------------------------

  calculator.cvm
    Takes two integers as input.
    Prints their sum, difference, and product.
    Run: ./cvm calculator.cvm

  counter.cvm
    Counts from 1 to 5 using a while loop.
    No input required.
    Run: ./cvm counter.cvm

  countdown.cvm
    Takes integer n as input. Counts down from n to 1.
    Run: ./cvm countdown.cvm

  truth_machine.cvm
    Input 0 → prints 0 once and exits.
    Input 1 → prints 1 forever (Ctrl+C to stop).
    Run: ./cvm truth_machine.cvm

  powers.cvm
    Takes an integer n as input.
    Prints n squared and n cubed.
    Run: ./cvm powers.cvm

  factorial.cvm
    Takes an integer n as input. Prints n! (factorial).
    Run: ./cvm factorial.cvm

  absolute_value.cvm
    Takes an integer as input. Prints its absolute value.
    Run: ./cvm absolute_value.cvm

  even_or_odd.cvm
    Takes an integer as input. Prints 1 if even, 0 if odd.
    Run: ./cvm even_or_odd.cvm

  max_of_two_numbers.cvm
    Takes two integers as input. Prints the larger one.
    Run: ./cvm max_of_two_numbers.cvm

  multiplication_table.cvm
    Takes integer n as input.
    Prints the multiplication table of n (1x to 10x).
    Run: ./cvm multiplication_table.cvm

  sum_of_first_N_numbers.cvm
    Takes integer n as input.
    Prints the sum of all integers from 1 to n.
    Run: ./cvm sum_of_first_N_numbers.cvm


----------------------------------------------------------------
  CVM++ LANGUAGE QUICK REFERENCE
----------------------------------------------------------------

  Variables:
       let x = 42;
       let y = x + 10;

  Input (reads integer from keyboard):
       let n = input;

  Output:
       print x;

  If / Else:
       if x < 10 {
           print x;
       } else {
           print 0;
       }

  While loop:
       let i = 0;
       while i < 5 {
           print i;
           let i = i + 1;
       }

  Supported operators:  +  -  *  /  ==  !=  <  >

  Supported types:      Integer,  Boolean (true / false)

  All statements end with a semicolon (;).
  Blocks are enclosed in curly braces { }.


----------------------------------------------------------------
  PROJECT STRUCTURE
----------------------------------------------------------------

  cvm++/
  ├── src/
  │   ├── lexer.h / lexer.cpp       Tokenizer
  │   ├── parser.h / parser.cpp     Parser + AST
  │   ├── compiler.h / compiler.cpp Bytecode Compiler
  │   ├── vm.h / vm.cpp             Virtual Machine
  │   └── main.cpp                  Entry point
  ├── Makefile
  ├── README.txt                    This file
  ├── absolute_value.cvm
  ├── calculator.cvm
  ├── countdown.cvm
  ├── counter.cvm
  ├── even_or_odd.cvm
  ├── factorial.cvm
  ├── max_of_two_numbers.cvm
  ├── multiplication_table.cvm
  ├── powers.cvm
  ├── sum_of_first_N_numbers.cvm
  └── truth_machine.cvm


----------------------------------------------------------------
  PIPELINE OVERVIEW
----------------------------------------------------------------

  Source Code (.cvm)
       |
       v
    Lexer         converts characters -> Tokens
       |
       v
    Parser        arranges Tokens -> Abstract Syntax Tree
       |
       v
    Compiler      flattens AST -> Bytecode (uint8_t array)
       |
       v
    VM            executes Bytecode -> program output


================================================================
