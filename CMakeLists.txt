# CMakeLists.txt
cmake_minimum_required(VERSION 3.21)
project(deeplang VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD          17) # set c++ standard to 17
set(CMAKE_CXX_STANDARD_REQUIRED ON) # without cpp 17, compiler will give error
set(CMAKE_CXX_EXTENSIONS        OFF) # it's fine to use c++ 17 without extensions (like, bits/stdc++.h)
option(CMAKE_EXPORT_COMPILE_COMMANDS "Generate compile_commands.json" ON) # for clang-tidy

option(BUILD_TESTS "Build tests" OFF) # for googletest

set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

set(LEXER_LIBRARY_NAME lexer)
set(AST_LIBRARY_NAME ast)
set(PARSER_LIBRARY_NAME parser)
set(CODEGEN_LIBRARY_NAME codegen)
set(MAIN_LOOP_LIBRARY_NAME main_loop)
set(UTILS_LIBRARY_NAME utils)
set(EXECUTABLE_NAME deeplang)

find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
llvm_map_components_to_libnames(LLVM_LIBS core support irreader)


add_subdirectory(lib)
add_subdirectory(src)


if(BUILD_TESTS)
    add_subdirectory(third_party/googletest)
    enable_testing()
    add_subdirectory(tests)
endif()
