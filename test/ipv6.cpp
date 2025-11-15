#include <gtest/gtest.h>

extern "C"
{
    #include "ip.h"
}

TEST(IPv6Suite, ValidIPv6ConstantFullForm)
{
    ipv6_t ip = read_ipv6("2001:0db8:85a3:0000:0000:8a2e:0370:7334");
    EXPECT_EQ(ip.ip[0], 0x2001);
    EXPECT_EQ(ip.ip[1], 0x0db8);
    EXPECT_EQ(ip.ip[2], 0x85a3);
    EXPECT_EQ(ip.ip[3], 0);
    EXPECT_EQ(ip.ip[4], 0);
    EXPECT_EQ(ip.ip[5], 0x8a2e);
    EXPECT_EQ(ip.ip[6], 0x0370);
    EXPECT_EQ(ip.ip[7], 0x7334);
    EXPECT_EQ(ip.ps, 128);
}

TEST(IPv6Suite, ValidIPv6VariableFullForm)
{
    char s[] = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    ipv6_t ip = read_ipv6(s);
    EXPECT_EQ(ip.ip[0], 0x2001);
    EXPECT_EQ(ip.ip[1], 0x0db8);
    EXPECT_EQ(ip.ip[2], 0x85a3);
    EXPECT_EQ(ip.ip[3], 0);
    EXPECT_EQ(ip.ip[4], 0);
    EXPECT_EQ(ip.ip[5], 0x8a2e);
    EXPECT_EQ(ip.ip[6], 0x0370);
    EXPECT_EQ(ip.ip[7], 0x7334);
    EXPECT_EQ(ip.ps, 128);
}

TEST(IPv6Suite, ValidIPv6FullFormUpperCase)
{
    ipv6_t ip = read_ipv6("2001:0DB8:85A3:0000:0000:8A2E:0370:7334");
    EXPECT_EQ(ip.ip[0], 0x2001);
    EXPECT_EQ(ip.ip[1], 0x0db8);
    EXPECT_EQ(ip.ip[2], 0x85a3);
    EXPECT_EQ(ip.ip[3], 0);
    EXPECT_EQ(ip.ip[4], 0);
    EXPECT_EQ(ip.ip[5], 0x8a2e);
    EXPECT_EQ(ip.ip[6], 0x0370);
    EXPECT_EQ(ip.ip[7], 0x7334);
    EXPECT_EQ(ip.ps, 128);
}

TEST(IPv6Suite, ValidIPv6NoLeadingZeroes)
{
    ipv6_t ip = read_ipv6("2001:db8:85a3:0:0:8a2e:370:7334");
    EXPECT_EQ(ip.ip[0], 0x2001);
    EXPECT_EQ(ip.ip[1], 0x0db8);
    EXPECT_EQ(ip.ip[2], 0x85a3);
    EXPECT_EQ(ip.ip[3], 0);
    EXPECT_EQ(ip.ip[4], 0);
    EXPECT_EQ(ip.ip[5], 0x8a2e);
    EXPECT_EQ(ip.ip[6], 0x0370);
    EXPECT_EQ(ip.ip[7], 0x7334);
    EXPECT_EQ(ip.ps, 128);
}

TEST(IPv6Suite, InvalidIPv6TooFewGroups)
{
    ipv6_t ip = read_ipv6("2001:db8:85a3:0:0:8a2e:370");
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

TEST(IPv6Suite, InvalidIPv6TooManyGroups)
{
    ipv6_t ip = read_ipv6("2001:db8:85a3:0:0:8a2e:370:7334:8fb9");
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

TEST(IPv6Suite, InvalidIPv6EmptyFirstGroup)
{
    ipv6_t ip = read_ipv6(":db8:85a3:0:0:8a2e:370:7334");
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

TEST(IPv6Suite, InvalidIPv6EmptyLastGroup)
{
    ipv6_t ip = read_ipv6("2001:db8:85a3:0:0:8a2e:370:");
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

TEST(IPv6Suite, ValidIPv6WithDoubleColonGroups3And4)
{
    ipv6_t ip = read_ipv6("2001:db8:85a3::8a2e:370:7334");
    EXPECT_EQ(ip.ip[0], 0x2001);
    EXPECT_EQ(ip.ip[1], 0x0db8);
    EXPECT_EQ(ip.ip[2], 0x85a3);
    EXPECT_EQ(ip.ip[3], 0);
    EXPECT_EQ(ip.ip[4], 0);
    EXPECT_EQ(ip.ip[5], 0x8a2e);
    EXPECT_EQ(ip.ip[6], 0x0370);
    EXPECT_EQ(ip.ip[7], 0x7334);
    EXPECT_EQ(ip.ps, 128);
}

TEST(IPv6Suite, ValidIPv6WithDoubleColonGroups0And1)
{
    ipv6_t ip = read_ipv6("::2001:db8:85a3:8a2e:370:7334");
    EXPECT_EQ(ip.ip[0], 0);
    EXPECT_EQ(ip.ip[1], 0);
    EXPECT_EQ(ip.ip[2], 0x2001);
    EXPECT_EQ(ip.ip[3], 0x0db8);
    EXPECT_EQ(ip.ip[4], 0x85a3);
    EXPECT_EQ(ip.ip[5], 0x8a2e);
    EXPECT_EQ(ip.ip[6], 0x0370);
    EXPECT_EQ(ip.ip[7], 0x7334);
    EXPECT_EQ(ip.ps, 128);
}

TEST(IPv6Suite, ValidIPv6WithDoubleColonGroups6And7)
{
    ipv6_t ip = read_ipv6("2001:db8:85a3:8a2e:370:7334::");
    EXPECT_EQ(ip.ip[0], 0x2001);
    EXPECT_EQ(ip.ip[1], 0x0db8);
    EXPECT_EQ(ip.ip[2], 0x85a3);
    EXPECT_EQ(ip.ip[3], 0x8a2e);
    EXPECT_EQ(ip.ip[4], 0x0370);
    EXPECT_EQ(ip.ip[5], 0x7334);
    EXPECT_EQ(ip.ip[6], 0);
    EXPECT_EQ(ip.ip[7], 0);
    EXPECT_EQ(ip.ps, 128);
}

TEST(IPv6Suite, InvalidIPv6WithRepeatedDoubleColon)
{
    ipv6_t ip = read_ipv6("2001:db8:85a3::8a2e::");
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

TEST(IPv6Suite, ValidIPv6WithPrefixSize)
{
    ipv6_t ip = read_ipv6("2001:0db8:85a3:0000:0000:8a2e:0370:7334/12");
    EXPECT_EQ(ip.ip[0], 0x2001);
    EXPECT_EQ(ip.ip[1], 0x0db8);
    EXPECT_EQ(ip.ip[2], 0x85a3);
    EXPECT_EQ(ip.ip[3], 0);
    EXPECT_EQ(ip.ip[4], 0);
    EXPECT_EQ(ip.ip[5], 0x8a2e);
    EXPECT_EQ(ip.ip[6], 0x0370);
    EXPECT_EQ(ip.ip[7], 0x7334);
    EXPECT_EQ(ip.ps, 12);
}

TEST(IPv6Suite, InvalidIPv6MissingPrefixSize)
{
    ipv6_t ip = read_ipv6("2001:0db8:85a3:0000:0000:8a2e:0370:7334/");
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

TEST(IPv6Suite, InvalidIPv6ZeroPrefixSize)
{
    ipv6_t ip = read_ipv6("2001:0db8:85a3:0000:0000:8a2e:0370:7334/0");
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

TEST(IPv6Suite, InvalidIPv6PrefixSizeTooLarge)
{
    ipv6_t ip = read_ipv6("2001:0db8:85a3:0000:0000:8a2e:0370:7334/200");
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

TEST(IPv6Suite, InvalidIPv6EmptyGroupBeforePrefix)
{
    ipv6_t ip = read_ipv6("2001:0db8:85a3:0000:0000:8a2e:0370:/20");
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

TEST(IPv6Suite, InvalidIPv6InvalidCharacter)
{
    ipv6_t ip = read_ipv6("2001:0dg8:85a3:0000:0000:8a2e:0370:7334");
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

TEST(IPv6Suite, InvalidIPv6InvalidCharacterInPrefix)
{
    ipv6_t ip = read_ipv6("2001:0db8:85a3:0000:0000:8a2e:0370:7334/6c");
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
