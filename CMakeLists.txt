cmake_minimum_required(VERSION 3.28.3)
project(connect4 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the executable
add_executable(connect4 main.cpp)

# Include directories
target_include_directories(connect4 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Set compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(connect4 PRIVATE -Wall -Wextra -O3)
elseif(MSVC)
    target_compile_options(connect4 PRIVATE /W4 /O2)
endif()
