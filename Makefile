CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
SRC := Project-Dames/Project-Dames
CORE := $(SRC)/posicio.cpp $(SRC)/fitxa.cpp $(SRC)/tauler.cpp $(SRC)/cuamoviments.cpp
HEADERS := $(wildcard $(SRC)/*.hpp)
.PHONY: all test clean
all: build/checkers build/tauler_inicial.txt
build:
	mkdir -p build
build/checkers: cli/main.cpp $(CORE) $(HEADERS) | build
	$(CXX) $(CXXFLAGS) -I$(SRC) cli/main.cpp $(CORE) -o $@
build/checkers_tests: tests/test_engine.cpp $(CORE) $(HEADERS) | build
	$(CXX) $(CXXFLAGS) -I$(SRC) tests/test_engine.cpp $(CORE) -o $@
build/tauler_inicial.txt: $(SRC)/tauler_inicial.txt | build
	cp $< $@
test: build/checkers_tests
	./build/checkers_tests
clean:
	rm -rf build
