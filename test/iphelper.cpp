#include <gtest/gtest.h>
#include <arpa/inet.h>

extern "C"
{
#include "ip.h"
#include "ip_unittest.h"
}

TEST(HelperSuite, DiscoverAF_INET6_AllZeroes)
{
    int result;
    uint16_t ip[8];
    result = inet_pton(AF_INET6, "::", ip);
    EXPECT_EQ(result, 1);
    EXPECT_EQ(ip[0], 0);
    EXPECT_EQ(ip[1], 0);
    EXPECT_EQ(ip[2], 0);
    EXPECT_EQ(ip[3], 0);
    EXPECT_EQ(ip[4], 0);
    EXPECT_EQ(ip[5], 0);
    EXPECT_EQ(ip[6], 0);
    EXPECT_EQ(ip[7], 0);
}

TEST(HelperSuite, DiscoverAF_INET6_Loopback)
{
    int result;
    uint16_t ip[8];
    result = inet_pton(AF_INET6, "::1", ip);
    EXPECT_EQ(result, 1);
    EXPECT_EQ(ip[0], 0);
    EXPECT_EQ(ip[1], 0);
    EXPECT_EQ(ip[2], 0);
    EXPECT_EQ(ip[3], 0);
    EXPECT_EQ(ip[4], 0);
    EXPECT_EQ(ip[5], 0);
    EXPECT_EQ(ip[6], 0);
    EXPECT_EQ(ip[7], 256);
}

TEST(HelperSuite, DiscoverAF_INET6_AllOnes)
{
    int result;
    uint16_t ip[8];
    result = inet_pton(AF_INET6, "1:1:1:1:1:1:1:1", ip);
    EXPECT_EQ(result, 1);
    EXPECT_EQ(ip[0], 256);
    EXPECT_EQ(ip[1], 256);
    EXPECT_EQ(ip[2], 256);
    EXPECT_EQ(ip[3], 256);
    EXPECT_EQ(ip[4], 256);
    EXPECT_EQ(ip[5], 256);
    EXPECT_EQ(ip[6], 256);
    EXPECT_EQ(ip[7], 256);
}

TEST(HelperSuite, DiscoverAF_INET6_12345678)
{
    int result;
    uint16_t ip[8];
    result = inet_pton(AF_INET6, "1:2:3:4:5:6:7:8", ip);
    EXPECT_EQ(result, 1);
    EXPECT_EQ(ip[0], 256);
    EXPECT_EQ(ip[1], 512);
    EXPECT_EQ(ip[2], 768);
    EXPECT_EQ(ip[3], 1024);
    EXPECT_EQ(ip[4], 1280);
    EXPECT_EQ(ip[5], 1536);
    EXPECT_EQ(ip[6], 1792);
    EXPECT_EQ(ip[7], 2048);
}

TEST(HelperSuite, DiscoverAF_INET6_WithPrefix)
{
    int result;
    uint16_t ip[8];
    result = inet_pton(AF_INET6, "1:2:3:4:5:6:7:8/120", ip);
    EXPECT_EQ(result, 0);
}

TEST(HelperSuite, SwitchHiLoBytes)
{
    EXPECT_EQ(switchHiLoBytes(1), 256);
    EXPECT_EQ(switchHiLoBytes(2), 512);
}

TEST(HelperSuite, ReverseBytesIPv6)
{
    uint16_t in[8] = { 1,1,1,1,1,1,1,1 };
    uint16_t out[8];
    reverseBytesIPv6(in, out);
    EXPECT_EQ(out[0], 256);
    EXPECT_EQ(out[1], 256);
    EXPECT_EQ(out[2], 256);
    EXPECT_EQ(out[3], 256);
    EXPECT_EQ(out[4], 256);
    EXPECT_EQ(out[5], 256);
    EXPECT_EQ(out[6], 256);
    EXPECT_EQ(out[7], 256);
}
