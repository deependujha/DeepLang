include(GoogleTest)

enable_testing()

set(
    TEST_CODE
    hello_test.cc
    sum_test.cc
    )

include_directories("${CMAKE_SOURCE_DIR}/include")

add_executable(TEST_CODE ${TEST_CODE})
target_link_libraries(TEST_CODE GTest::gtest_main ${LLVM_LIBS} ${LEXER_LIBRARY_NAME})

gtest_discover_tests(TEST_CODE)
