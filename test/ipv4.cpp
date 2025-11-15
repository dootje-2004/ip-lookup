#include <gtest/gtest.h>

extern "C"
{
    #include "ip.h"
}

TEST(IPv4Suite, ValidIPv4Constant)
{
    ipv4_t ip = read_ipv4("1.2.3.4");
    EXPECT_EQ(ip.ip, 16909060);
    EXPECT_EQ(ip.ps, 32);
}

TEST(IPv4Suite, ValidIPv4Variable)
{
    char s[] = "2.3.4.5";
    ipv4_t ip = read_ipv4(s);
    EXPECT_EQ(ip.ip, 33752069);
    EXPECT_EQ(ip.ps, 32);
}

TEST(IPv4Suite, ValidIPv4WithMask)
{
    ipv4_t ip = read_ipv4("100.200.100.200/24");
    EXPECT_EQ(ip.ip, 1690854600);
    EXPECT_EQ(ip.ps, 24);
}

TEST(IPv4Suite, InvalidIPv4GroupTooLarge)
{
    ipv4_t ip = read_ipv4("100.200.100.300/24");
    EXPECT_EQ(ip.ip, 0);
    EXPECT_EQ(ip.ps, 0);
}

TEST(IPv4Suite, InvalidIPv4TooFewGroups)
{
    ipv4_t ip = read_ipv4("100.200.100");
    EXPECT_EQ(ip.ip, 0);
    EXPECT_EQ(ip.ps, 0);
}

TEST(IPv4Suite, InvalidIPv4TooManyGroups)
{
    ipv4_t ip = read_ipv4("100.200.100.200.100");
    EXPECT_EQ(ip.ip, 0);
    EXPECT_EQ(ip.ps, 0);
}

TEST(IPv4Suite, InvalidIPv4InvalidMask)
{
    ipv4_t ip = read_ipv4("100.200.100.200/40");
    EXPECT_EQ(ip.ip, 0);
    EXPECT_EQ(ip.ps, 0);
}

TEST(IPv4Suite, InvalidIPv4MissingMaskValue)
{
    ipv4_t ip = read_ipv4("100.200.100.200/");
    EXPECT_EQ(ip.ip, 0);
    EXPECT_EQ(ip.ps, 0);
}

TEST(IPv4Suite, InvalidIPv4MaskIs0)
{
    ipv4_t ip = read_ipv4("100.200.100.200/0");
    EXPECT_EQ(ip.ip, 0);
    EXPECT_EQ(ip.ps, 0);
}

TEST(IPv4Suite, InvalidIPv4EmptyGroup0)
{
    ipv4_t ip = read_ipv4(".200.100.200");
    EXPECT_EQ(ip.ip, 0);
    EXPECT_EQ(ip.ps, 0);
}

TEST(IPv4Suite, InvalidIPv4EmptyGroup1)
{
    ipv4_t ip = read_ipv4("100..100.200");
    EXPECT_EQ(ip.ip, 0);
    EXPECT_EQ(ip.ps, 0);
}

TEST(IPv4Suite, InvalidIPv4EmptyGroup2)
{
    ipv4_t ip = read_ipv4("100.200..200");
    EXPECT_EQ(ip.ip, 0);
    EXPECT_EQ(ip.ps, 0);
}

TEST(IPv4Suite, InvalidIPv4EmptyGroup3)
{
    ipv4_t ip = read_ipv4("100.200.100.");
    EXPECT_EQ(ip.ip, 0);
    EXPECT_EQ(ip.ps, 0);
}

TEST(IPv4Suite, InvalidIPv4InvalidCharacter)
{
    ipv4_t ip = read_ipv4("a.200.100.200");
    EXPECT_EQ(ip.ip, 0);
    EXPECT_EQ(ip.ps, 0);
}

TEST(IPv4Suite, InvalidIPv4AnotherInvalidCharacter)
{
    ipv4_t ip = read_ipv4("100.200.-100.200");
    EXPECT_EQ(ip.ip, 0);
    EXPECT_EQ(ip.ps, 0);
}

TEST(IPv4Suite, InvalidIPv4InvalidSeparator)
{
    ipv4_t ip = read_ipv4("100:200:100:200");
    EXPECT_EQ(ip.ip, 0);
    EXPECT_EQ(ip.ps, 0);
}

TEST(IPv4Suite, InvalidIPv4LeadingZeroes)
{
    ipv4_t ip = read_ipv4("100.0200.100.200");
    EXPECT_EQ(ip.ip, 0);
    EXPECT_EQ(ip.ps, 0);
}

TEST(IPv4Suite, ValidIPv4With0Group)
{
    ipv4_t ip = read_ipv4("100.0.1.200");
    EXPECT_EQ(ip.ip, 1677722056);
    EXPECT_EQ(ip.ps, 32);
}

TEST(IPv4Suite, InvalidIPv4InvalidCharacterInMask)
{
    ipv4_t ip = read_ipv4("100.200.100.200/4-");
    EXPECT_EQ(ip.ip, 0);
    EXPECT_EQ(ip.ps, 0);
}

TEST(IPv4Suite, IPv4WithAllZeroes)
{
    ipv4_t ip = read_ipv4("0.0.0.0");
    EXPECT_EQ(ip.ip, 0);
    EXPECT_EQ(ip.ps, 32);
}
