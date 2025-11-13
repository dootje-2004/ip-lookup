#include <gtest/gtest.h>

extern "C"
{
    #include "ip.h"
}

TEST(IPv4Suite, ValidIPv4)
{
    char s[] = "1.2.3.4";
    ipv4_t ip = read_ipv4(s);
    EXPECT_EQ(ip.ip, 16909060);
    EXPECT_EQ(ip.umask, 0);
}
