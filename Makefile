CXX = clang++
CXXFLAGS = -std=c++17 -O2 -Wall -Iinclude
SRCS = src/main.cpp src/lexer.cpp src/parser.cpp src/bytecode.cpp src/value.cpp src/vm.cpp \
	src/bench_runner.cpp src/jit/profile.cpp src/jit/code_cache.cpp src/jit/template_jit.cpp \
	src/jit/native_codegen.cpp src/jit/lir.cpp src/jit/runtime_helpers.cpp
ASM_OBJ = src/jit/bridges.o

tinylang: $(SRCS) $(ASM_OBJ)
	$(CXX) $(CXXFLAGS) $(SRCS) $(ASM_OBJ) -o tinylang

$(ASM_OBJ): src/jit/bridges.S
	clang -c src/jit/bridges.S -o $(ASM_OBJ)

.PHONY: test
test: tinylang
	./tinylang bench/fib.tl --interp --bench
