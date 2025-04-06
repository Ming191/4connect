cmake_minimum_required(VERSION 3.29)
project(4connect)

# Use C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable target.
# List all your source files here.
add_executable(4connect
        main.cpp
        bitboard.h
        solver.h
        lookup_table.h
)
