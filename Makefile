# Makefile for DPLL SAT Solver
# default g++ -std=c++17 DPLL.cpp -o DPLL ./DPLL test.cnf
# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O3

# Target executable name
TARGET = mySAT
PYTHON = python3
BATCH_SCRIPT = batch.py
BATCH_OUTPUT = batch_results.csv
TEST_DIR = test
DLIS ?= 1
WATCHED_LITERALS ?= 1

# Source files
SOURCES = DPLL.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target: build the executable
all: $(TARGET)
	./$(TARGET) test_BMS_k3_n100_m429_0.cnf

# Build the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET) $(BATCH_OUTPUT)

veryclean: clean # 这个地方想要clean log？
	rm -f TARGET

# Batch test: run batch.py over test/**/*.cnf and write monitor stats to CSV.
batch: $(TARGET) $(BATCH_SCRIPT)
	$(PYTHON) $(BATCH_SCRIPT) --solver ./$(TARGET) --test-dir $(TEST_DIR) --output $(BATCH_OUTPUT) --DLIS $(DLIS) --watched-literals $(WATCHED_LITERALS)

# Test with all CNF files under test/
test: batch

# Help target
help:
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all    - Build the DPLL SAT solver executable (default)"
	@echo "  batch  - Run all test/**/*.cnf and write $(BATCH_OUTPUT)"
	@echo "  clean  - Remove build artifacts, executable, and batch output"
	@echo "  test   - Alias for batch"
	@echo "  help   - Display this help message"
	@echo ""
	@echo "Run the solver with: ./mysAT <cnf_file> [--DLIS 0|1] [--watched-literals 0|1]"
	@echo "Run batch with DLIS and watched-literals enabled: make batch DLIS=1 WATCHED_LITERALS=1"

.PHONY: all clean batch test help
