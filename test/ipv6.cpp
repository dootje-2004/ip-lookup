#include <gtest/gtest.h>

extern "C"
{
    #include "ip.h"
}

TEST(IPv6Suite, ValidIPv6ConstantFullForm)
{
    ipv6_t ip = read_ipv6("2001:0db8:85a3:0000:0000:8a2e:0370:7334");
    EXPECT_EQ(ip.ip[0], 0x2001);
    EXPECT_EQ(ip.ps, 128);
}
