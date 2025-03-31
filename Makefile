# Makefile for Connect Four

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
TARGET = connect4
SRCS = main.cpp
HEADERS = bitboard.hpp solver.hpp

all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run