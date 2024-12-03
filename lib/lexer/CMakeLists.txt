set(lexical_analysis
    token.cc
)

add_library(${LEXER_LIBRARY_NAME} ${lexical_analysis})

target_include_directories(${LEXER_LIBRARY_NAME} PRIVATE ${PROJECT_INCLUDE_DIR}/lexer)
