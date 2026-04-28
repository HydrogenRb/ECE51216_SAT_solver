# Makefile for DPLL SAT Solver
# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2

# Target executable name
TARGET = mysAT

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
	rm -f $(OBJECTS) $(TARGET)

# Test with provided CNF files
test: $(TARGET)
	@echo "Testing with uf20-01.cnf..."
	./$(TARGET) uf20-01.cnf
	@echo "\n================================\n"
	@echo "Testing with uf20-02.cnf..."
	./$(TARGET) uf20-02.cnf
	@echo "\n================================\n"
	@echo "Testing with uf20-03.cnf..."
	./$(TARGET) uf20-03.cnf

# Help target
help:
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all   - Build the DPLL SAT solver executable (default)"
	@echo "  clean - Remove build artifacts and executable"
	@echo "  test  - Build and run tests on CNF files"
	@echo "  help  - Display this help message"
	@echo ""
	@echo "Run the solver with: ./mysAT <cnf_file>"

.PHONY: all clean test help
