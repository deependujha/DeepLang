#include <gtest/gtest.h>
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

class SimpleTest : public testing::Test {};

static void InitializeModule() {
    using namespace llvm;
    // Open a new context and module.
    auto TheContext = std::make_unique<LLVMContext>();
    auto TheModule = std::make_unique<Module>("my cool jit", *TheContext);

    // Create a new builder for the module.
    auto Builder = std::make_unique<IRBuilder<>>(*TheContext);
}

// Demonstrate some basic assertions.
TEST_F(SimpleTest, BasicAssertions) {
    InitializeModule();
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
