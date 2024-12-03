set(ast
    ast.cc
)

add_library(${AST_LIBRARY_NAME} ${ast})
target_include_directories(${AST_LIBRARY_NAME} PRIVATE ${PROJECT_INCLUDE_DIR}/ast)
