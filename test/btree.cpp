#include <gtest/gtest.h>

extern "C"
{
    #include "btree.h"
}

TEST(BTreeSuite, EmptyTree)
{
    EXPECT_EQ(32, 32);
}
