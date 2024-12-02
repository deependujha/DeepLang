include_directories(
    "../include/lexer"
    "../include/ast"
    "../include/parser"
) # add include directory to the executable
add_executable(${EXECUTABLE_NAME} main.cc)
target_link_libraries(${EXECUTABLE_NAME}
    ${LEXER_LIBRARY_NAME}
    ${PARSER_LIBRARY_NAME}
    ${AST_LIBRARY_NAME}
    ${LLVM_LIBS}
)
