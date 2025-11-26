#include <gtest/gtest.h>
#include <arpa/inet.h>

extern "C"
{
#include "ip.h"
#include "ip_unittest.h"
}

TEST(IPHelperSuite, DiscoverAF_INET6_AllZeroes)
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

TEST(IPHelperSuite, DiscoverAF_INET6_Loopback)
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

TEST(IPHelperSuite, DiscoverAF_INET6_AllOnes)
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

TEST(IPHelperSuite, DiscoverAF_INET6_12345678)
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

TEST(IPHelperSuite, DiscoverAF_INET6_WithPrefix)
{
    int result;
    uint16_t ip[8];
    result = inet_pton(AF_INET6, "1:2:3:4:5:6:7:8/120", ip);
    EXPECT_EQ(result, 0);
}

TEST(IPHelperSuite, EmptyIPv4)
{
    ipv4_t ip = empty_ipv4();
    EXPECT_EQ(ip.ip, 0);
    EXPECT_EQ(ip.ps, 0);
}

TEST(IPHelperSuite, EmptyIPv6)
{
    ipv6_t ip = empty_ipv6();
    EXPECT_EQ(ip.ip[0], 0);
    EXPECT_EQ(ip.ip[1], 0);
    EXPECT_EQ(ip.ip[2], 0);
    EXPECT_EQ(ip.ip[3], 0);
    EXPECT_EQ(ip.ip[4], 0);
    EXPECT_EQ(ip.ip[5], 0);
    EXPECT_EQ(ip.ip[6], 0);
    EXPECT_EQ(ip.ip[7], 0);
    EXPECT_EQ(ip.ps, 0);
}

TEST(IPHelperSuite, ReverseBytes)
{
    EXPECT_EQ(reverseBytes(1), 256);
    EXPECT_EQ(reverseBytes(2), 512);
    EXPECT_EQ(reverseBytes(3), 768);
    EXPECT_EQ(reverseBytes(0xabcd), 0xcdab);
    EXPECT_EQ(reverseBytes(0xefcd), 0xcdef);
}

TEST(IPHelperSuite, ReverseBytesIPv4)
{
    EXPECT_EQ(reverseBytesIPv4(1),0x01000000);
    EXPECT_EQ(reverseBytesIPv4(0x01234567),0x67452301);
    EXPECT_EQ(reverseBytesIPv4(0x67452301),0x01234567);
}

TEST(IPHelperSuite, ReverseBytesIPv6AllOnes)
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

TEST(IPHelperSuite, ReverseBytesIPv6AllDifferent)
{
    uint16_t in[8] = { 0x102,0x304,0x506,0x708,0x90a,0xb0c,0xd0e,0xf00 };
    uint16_t out[8];
    reverseBytesIPv6(in, out);
    EXPECT_EQ(out[0], 0x201);
    EXPECT_EQ(out[1], 0x403);
    EXPECT_EQ(out[2], 0x605);
    EXPECT_EQ(out[3], 0x807);
    EXPECT_EQ(out[4], 0xa09);
    EXPECT_EQ(out[5], 0xc0b);
    EXPECT_EQ(out[6], 0xe0d);
    EXPECT_EQ(out[7], 0xf);
}

TEST(IPHelperSuite, ReadPrefixSizeIPv4)
{
    char s[] = "1.2.3.4/24";
    uint8_t i = 8;
    uint8_t ps = read_prefix_size(s, &i, 32);
    EXPECT_EQ(ps, 24);
}

TEST(IPHelperSuite, ReadPrefixSizeIPv6)
{
    char s[] = "1:2:3:4:5:6:7:8/121";
    uint8_t i = 16;
    uint8_t ps = read_prefix_size(s, &i, 128);
    EXPECT_EQ(ps, 121);
}

TEST(IPHelperSuite, AppendPrefixSize)
{
    char s[IPSTRLENV4];
    strncpy(s, "12.24.36.48", 12);
    append_prefix_size(s, 24, 32);
    EXPECT_STREQ(s, "12.24.36.48/24");
}

TEST(IPHelperSuite, AppendPrefixSizeDefaultValue)
{
    char s[IPSTRLENV4];
    strncpy(s, "12.24.36.48", 12);
    append_prefix_size(s, 32, 32);
    EXPECT_STREQ(s, "12.24.36.48");
}

TEST(IPHelperSuite, AppendPrefixSizeToNonIPString)
{
    char s[IPSTRLENV6];
    strncpy(s, "non-ip-string", 14);
    append_prefix_size(s, 100, 128);
    EXPECT_STREQ(s, "non-ip-string/100");
}
