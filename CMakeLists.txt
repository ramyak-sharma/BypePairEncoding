cmake_minimum_required(VERSION 3.10)

project(BytePairEncoding VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(bpe_encoder main.cpp)

target_compile_options(bpe_encoder PRIVATE -Wall -Wextra -Wpedantic)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIRECTORY})
