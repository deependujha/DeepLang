set(parser
    parser.cc
    parser_ast.cc
    conditional_parser.cc
)

add_library(${PARSER_LIBRARY_NAME} ${parser})

target_include_directories(${PARSER_LIBRARY_NAME}
    PRIVATE
    ${PROJECT_INCLUDE_DIR}
)

target_link_libraries(${PARSER_LIBRARY_NAME}
    ${AST_LIBRARY_NAME}
    ${LEXER_LIBRARY_NAME}
    ${UTILS_LIBRARY_NAME}
)
