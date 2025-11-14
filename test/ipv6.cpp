#include <gtest/gtest.h>

extern "C"
{
    #include "ip.h"
}

TEST(IPv6Suite, ValidIPv6ConstantFullForm)
{
    ipv6_t ip = read_ipv6("2001:0db8:85a3:0000:0000:8a2e:0370:7334");
    EXPECT_EQ(ip.group[0], 8193);
    EXPECT_EQ(ip.group[1], 3512);
    EXPECT_EQ(ip.group[2], 34211);
    EXPECT_EQ(ip.group[3], 0);
    EXPECT_EQ(ip.group[4], 0);
    EXPECT_EQ(ip.group[5], 35374);
    EXPECT_EQ(ip.group[6], 880);
    EXPECT_EQ(ip.group[7], 29492);
    EXPECT_EQ(ip.mask, 128);
}
