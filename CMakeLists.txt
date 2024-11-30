# CMakeLists.txt
cmake_minimum_required(VERSION 3.21)
project(deeplang VERSION 1.0.0 LANGUAGES CXX)


set(LEXER_LIBRARY_NAME lexer)
set(EXECUTABLE_NAME deeplang)

add_subdirectory(lib)
add_subdirectory(include)
add_subdirectory(src)

