# Makefile
CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Wpedantic

SRC := src/main.cpp src/analyzer.cpp src/text_parser.cpp
HDR := src/analyzer.hpp src/text_parser.hpp src/dyn_array.hpp src/linked_list.hpp src/hash_table.hpp src/utils.hpp
BIN := analyzer

$(BIN): $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC)

clean:
	rm -f $(BIN)