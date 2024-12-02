set(lexical_analysis
    token.cc
    utils.cc
)

add_library(${LEXER_LIBRARY_NAME} ${lexical_analysis})