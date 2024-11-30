#include <gtest/gtest.h>
#include "lexer/token.h"

TEST(SumTest, TestSum) {
    EXPECT_EQ(sum(1, 2), 3);
    EXPECT_EQ(sum(10, 20), 30);
}

TEST(MultiplyTest, TestMultiply) {
    EXPECT_EQ(multiply(2, 3), 6);
    EXPECT_EQ(multiply(10, 20), 200);
}