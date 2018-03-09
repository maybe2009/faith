#include "gtest/gtest.h"

TEST(case0, test0) {
    EXPECT_STREQ("123", "345");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
