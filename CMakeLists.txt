# CMakeLists.txt
cmake_minimum_required(VERSION 3.21)
project(deeplang VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD          17) # set c++ standard to 17
set(CMAKE_CXX_STANDARD_REQUIRED ON) # without cpp 17, compiler will give error
set(CMAKE_CXX_EXTENSIONS        OFF) # it's fine to use c++ 17 without extensions (like, bits/stdc++.h)
option(CMAKE_EXPORT_COMPILE_COMMANDS "Generate compile_commands.json" ON) # for clang-tidy

option(BUILD_TESTS "Build tests" OFF) # for googletest

set(LEXER_LIBRARY_NAME lexer)
set(EXECUTABLE_NAME deeplang)

add_subdirectory(lib)
add_subdirectory(include)
add_subdirectory(src)


if(BUILD_TESTS)
    add_subdirectory(third_party/googletest)
    enable_testing()
    add_subdirectory(tests)
endif()
