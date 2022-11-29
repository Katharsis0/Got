cmake_minimum_required(VERSION 3.23)
project(CLI)

set(CMAKE_CXX_STANDARD 14)

add_executable(
        main.cpp CommandParser.cpp CommandParser.h json.hpp Huffman.cpp)
