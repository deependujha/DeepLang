include_directories("../include/lexer") # add include directory to the executable
add_executable(deeplang main.cc)
target_link_libraries(${EXECUTABLE_NAME} ${LEXER_LIBRARY_NAME})
