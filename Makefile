# =============================================================================
#  Student points analyzer - GNU make build (Unix / WSL / MSYS2)
#  Usage:  make            build into bin/students
#          make run        build and start the interactive menu
#          make data       generate the small test files
#          make bench      build and benchmark students1000.txt
#          make clean      remove build artefacts
#          make distclean  also remove generated data and result files
# =============================================================================

CXX       ?= g++
CXXSTD    := -std=c++17
WARN      := -Wall -Wextra -pedantic
OPT       := -O2
CXXFLAGS  ?= $(CXXSTD) $(OPT) $(WARN) -Iinclude
LDFLAGS   ?=

SRC_DIR   := src
OBJ_DIR   := build
BIN_DIR   := bin
TARGET    := $(BIN_DIR)/students

SOURCES   := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS   := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))
DEPS      := $(OBJECTS:.o=.d)

.PHONY: all run data bench clean distclean

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@
	@echo "Built $@"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

data: $(TARGET)
	./$(TARGET) --generate 1000
	./$(TARGET) --generate 10000
	./$(TARGET) --generate 100000

bench: $(TARGET)
	./$(TARGET) --bench students1000.txt

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

distclean: clean
	rm -f students[0-9]*.txt out_*.txt manual_*.txt benchmark_results.md

-include $(DEPS)
