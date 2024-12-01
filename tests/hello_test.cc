#include <gtest/gtest.h>
#include "llvm/ADT/APFloat.h"

class SimpleTest : public testing::Test {};

// Demonstrate some basic assertions.
TEST_F(SimpleTest, BasicAssertions) {
    llvm::fltSemantics;
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}

// Tests factorial of positive numbers.
TEST_F(SimpleTest, HandlesPositiveInput) {
    EXPECT_EQ(1, 1);
    EXPECT_EQ(2, 2);
    EXPECT_EQ(6, 6);
    EXPECT_EQ(40320, 40320);
}
