CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
SRC = src/main.cpp src/lexer.cpp src/parser.cpp src/compiler.cpp src/vm.cpp
OUT = cvm

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)