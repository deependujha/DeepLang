add_subdirectory(utils)
add_subdirectory(lexer)
add_subdirectory(ast)
add_subdirectory(parser)
add_subdirectory(codegen)

set(main_loop
    main_loop.cc
)

add_library(${MAIN_LOOP_LIBRARY_NAME} ${main_loop})

target_include_directories(${MAIN_LOOP_LIBRARY_NAME}
    PRIVATE
    ${PROJECT_INCLUDE_DIR}
    ${PROJECT_INCLUDE_DIR}/ast
)

target_link_libraries(${MAIN_LOOP_LIBRARY_NAME}
    ${PARSER_LIBRARY_NAME}
    ${CODEGEN_LIBRARY_NAME}
    ${AST_LIBRARY_NAME}
)
