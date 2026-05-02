# Makefile for DPLL SAT Solver
# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2

# Target executable name
TARGET = mysAT
PYTHON = python3
BATCH_SCRIPT = batch.py
BATCH_OUTPUT = batch_results.csv
TEST_DIR = test

# Source files
SOURCES = DPLL.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target: build the executable
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET) $(BATCH_OUTPUT)

# Batch test: run batch.py over test/**/*.cnf and write monitor stats to CSV.
batch: $(TARGET) $(BATCH_SCRIPT)
	$(PYTHON) $(BATCH_SCRIPT) --solver ./$(TARGET) --test-dir $(TEST_DIR) --output $(BATCH_OUTPUT)

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
	@echo "Run the solver with: ./mysAT <cnf_file>"

.PHONY: all clean batch test help
