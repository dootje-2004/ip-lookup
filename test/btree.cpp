#include <gtest/gtest.h>

extern "C"
{
#include "btree.h"
}

TEST(BTreeSuite, NewEmptyTree)
{
    const bnode_t *tree = createNode();
    EXPECT_TRUE(tree->child[0] == nullptr);
    EXPECT_TRUE(tree->child[1] == nullptr);
}

TEST(BTreeSuite, AddIPv4AllZeroes)
{
    char s[] = "0.0.0.0";
    bnode_t *tree = createNode();
    const bnode_t *p = tree;
    uint8_t d = 32;

    insertIPv4(tree, s);
    while (d)
    {
        EXPECT_FALSE(p->child[0] == nullptr);
        EXPECT_TRUE(p->child[1] == nullptr);
        p = p->child[0];
        d--;
    }
    EXPECT_TRUE(p->child[0] == p);
    EXPECT_TRUE(p->child[1] == p);
}

TEST(BTreeSuite, AddIPv4AllOnes)
{
    char s[] = "255.255.255.255";
    bnode_t *tree = createNode();
    const bnode_t *p = tree;
    uint8_t d = 32;

    insertIPv4(tree, s);
    while (d)
    {
        EXPECT_TRUE(p->child[0] == nullptr);
        EXPECT_FALSE(p->child[1] == nullptr);
        p = p->child[1];
        d--;
    }
    EXPECT_TRUE(p->child[0] == p);
    EXPECT_TRUE(p->child[1] == p);
}

TEST(BTreeSuite, AddIPv4AllZeroesAndAllOnes)
{
    char s0[] = "0.0.0.0";
    char s1[] = "255.255.255.255";
    bnode_t *tree = createNode();
    const bnode_t *p = tree;
    uint8_t d = 31;

    insertIPv4(tree, s0);
    insertIPv4(tree, s1);
    EXPECT_FALSE(p->child[0] == nullptr);
    EXPECT_FALSE(p->child[1] == nullptr);
    p = tree->child[0];
    while (d)
    {
        EXPECT_FALSE(p->child[0] == nullptr);
        EXPECT_TRUE(p->child[1] == nullptr);
        p = p->child[0];
        d--;
    }
    EXPECT_TRUE(p->child[0] == p);
    EXPECT_TRUE(p->child[1] == p);

    p = tree->child[1];
    d = 31;
    while (d)
    {
        EXPECT_TRUE(p->child[0] == nullptr);
        EXPECT_FALSE(p->child[1] == nullptr);
        p = p->child[1];
        d--;
    }
    EXPECT_TRUE(p->child[0] == p);
    EXPECT_TRUE(p->child[1] == p);
}

TEST(BTreeSuite, AddIPv4AllOnesWithPrefix)
{
    char s[] = "255.255.255.0/24";
    bnode_t *tree = createNode();
    const bnode_t *p = tree;
    uint8_t d = 24;

    insertIPv4(tree, s);
    while (d)
    {
        EXPECT_TRUE(p->child[0] == nullptr);
        EXPECT_FALSE(p->child[1] == nullptr);
        p = p->child[1];
        d--;
    }
    EXPECT_TRUE(p->child[0] == p);
    EXPECT_TRUE(p->child[1] == p);
}

TEST(BTreeSuite, DumpIPv4Tree)
{
    char s0[] = "0.0.0.0";
    char s1[] = "255.255.255.255";
    char s2[] = "1.2.3.4";
    bnode_t *tree = createNode();

    insertIPv4(tree, s0);
    insertIPv4(tree, s1);
    insertIPv4(tree, s2);

    testing::internal::CaptureStdout();
    std::cout << dumpIPv4Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "0.0.0.0\n1.2.3.4\n255.255.255.255\n3");
}

TEST(BTreeSuite, DumpIPv4TreeRepeatedIPs)
{
    char s0[] = "1.2.3.4";
    char s1[] = "10.20.30.40";
    bnode_t *tree = createNode();

    insertIPv4(tree, s0);
    insertIPv4(tree, s1);
    insertIPv4(tree, s0);
    insertIPv4(tree, s1);

    testing::internal::CaptureStdout();
    std::cout << dumpIPv4Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "1.2.3.4\n10.20.30.40\n2");
}

TEST(BTreeSuite, DumpIPv4TreeWithPrefix)
{
    char s0[] = "0.0.0.0";
    char s1[] = "255.255.255.255/16";
    char s2[] = "1.2.3.4/28";
    bnode_t *tree = createNode();

    insertIPv4(tree, s0);
    insertIPv4(tree, s1);
    insertIPv4(tree, s2);

    testing::internal::CaptureStdout();
    std::cout << dumpIPv4Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "0.0.0.0\n1.2.3.0/28\n255.255.0.0/16\n3");
}

TEST(BTreeSuite, DumpIPv4TreeWithOverlappingRange)
{
    char s0[] = "1.2.3.4";
    char s1[] = "1.2.3.4/28";
    bnode_t *tree = createNode();

    insertIPv4(tree, s0);
    insertIPv4(tree, s1);

    testing::internal::CaptureStdout();
    std::cout << dumpIPv4Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "1.2.3.0/28\n1");
}

TEST(BTreeSuite, DumpIPv4TreeWithOverlappingRangeInReverseOrder)
{
    char s0[] = "1.2.3.4/28";
    char s1[] = "1.2.3.4";
    bnode_t *tree = createNode();

    insertIPv4(tree, s0);
    insertIPv4(tree, s1);

    testing::internal::CaptureStdout();
    std::cout << dumpIPv4Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "1.2.3.0/28\n1");
}

TEST(BTreeSuite, CountIPv4TreeSingleAddress)
{
    char s0[] = "1.2.3.4";
    bnode_t *tree = createNode();

    insertIPv4(tree, s0);

    testing::internal::CaptureStdout();
    std::cout << countIPv4Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "1");
}

TEST(BTreeSuite, CountIPv4TreeMultipleAddresses)
{
    char s0[] = "1.2.3.4";
    char s1[] = "10.20.30.40";
    char s2[] = "91.92.93.94";
    char s3[] = "11.22.33.44";
    bnode_t *tree = createNode();

    insertIPv4(tree, s0);
    insertIPv4(tree, s1);
    insertIPv4(tree, s2);
    insertIPv4(tree, s3);

    testing::internal::CaptureStdout();
    std::cout << countIPv4Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "4");
}

TEST(BTreeSuite, CountIPv4TreeOverlappingRanges)
{
    char s0[] = "1.2.3.4/28";
    char s1[] = "1.2.3.4/24";
    bnode_t *tree = createNode();

    insertIPv4(tree, s0);
    insertIPv4(tree, s1);

    testing::internal::CaptureStdout();
    std::cout << countIPv4Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "1");
}

TEST(BTreeSuite, CreateIPv4TreeFromTinyFile)
{
    bnode_t *tree = createIPv4TreeFromFile("/home/aldo/git/ip-lookup/test/data/ipv4single.txt");
    EXPECT_EQ(countIPv4Tree(tree), 1);
}

TEST(BTreeSuite, CreateIPv4TreeFromSmallFile)
{
    bnode_t *tree = createIPv4TreeFromFile("/home/aldo/git/ip-lookup/test/data/ipv4list.txt");
    EXPECT_EQ(countIPv4Tree(tree), 10);
}

TEST(BTreeSuite, CreateIPv4TreeFromLargeFile)
{
    bnode_t *tree = createIPv4TreeFromFile("/home/aldo/git/ip-lookup/test/data/inbound_v4.txt");
    EXPECT_GT(countIPv4Tree(tree), 1000000);
}

TEST(BTreeSuite, FindIPv4InSmallFile)
{
    bnode_t *tree = createIPv4TreeFromFile("/home/aldo/git/ip-lookup/test/data/ipv4list.txt");
    EXPECT_EQ(findIPv4(tree, "1.2.3.4"), 1);
    EXPECT_EQ(findIPv4(tree, "2.3.4.5"), 1);
    EXPECT_EQ(findIPv4(tree, "3.4.5.6"), 1);
    EXPECT_EQ(findIPv4(tree, "4.5.6.7"), 1);
    EXPECT_EQ(findIPv4(tree, "5.6.7.8"), 1);
    EXPECT_EQ(findIPv4(tree, "10.20.30.40"), 1);
    EXPECT_EQ(findIPv4(tree, "20.30.40.50"), 1);
    EXPECT_EQ(findIPv4(tree, "30.40.50.60"), 1);
    EXPECT_EQ(findIPv4(tree, "40.50.60.70"), 1);
    EXPECT_EQ(findIPv4(tree, "50.60.70.80"), 1);

    EXPECT_EQ(findIPv4(tree, "1.2.3.3"), 0);
    EXPECT_EQ(findIPv4(tree, "1.2.3.5"), 0);
}

TEST(BTreeSuite, FindIPv4InRange)
{
    bnode_t *tree = createIPv4TreeFromFile("/home/aldo/git/ip-lookup/test/data/ipv4range.txt");
    EXPECT_EQ(findIPv4(tree, "1.2.3.0"), 1);
    EXPECT_EQ(findIPv4(tree, "1.2.3.15"), 1);
    EXPECT_EQ(findIPv4(tree, "1.2.3.16"), 0);
    EXPECT_EQ(findIPv4(tree, "2.3.1.0"), 1);
    EXPECT_EQ(findIPv4(tree, "2.3.15.0"), 1);
    EXPECT_EQ(findIPv4(tree, "2.3.16.0"), 0);
}

TEST(BTreeSuite, FindIPv4InRangeWithMask24)
{
    bnode_t *tree = createIPv4TreeFromFile("/home/aldo/git/ip-lookup/test/data/ipv4range.txt");
    EXPECT_EQ(findIPv4(tree, "3.4.5.0"), 1);
    EXPECT_EQ(findIPv4(tree, "3.4.5.127"), 1);
    EXPECT_EQ(findIPv4(tree, "3.4.5.255"), 1);
    EXPECT_EQ(findIPv4(tree, "3.4.4.0"), 0);
    EXPECT_EQ(findIPv4(tree, "3.4.6.0"), 0);
}

TEST(BTreeSuite, FindIPv4InRangeWithMask31)
{
    bnode_t *tree = createIPv4TreeFromFile("/home/aldo/git/ip-lookup/test/data/ipv4range.txt");
    EXPECT_EQ(findIPv4(tree, "6.7.8.7"), 0);
    EXPECT_EQ(findIPv4(tree, "6.7.8.8"), 1);
    EXPECT_EQ(findIPv4(tree, "6.7.8.9"), 1);
    EXPECT_EQ(findIPv4(tree, "6.7.8.10"), 0);
}

TEST(BTreeSuite, FindIPv4Range)
{
    bnode_t *tree = createIPv4TreeFromFile("/home/aldo/git/ip-lookup/test/data/ipv4range.txt");
    EXPECT_EQ(findIPv4(tree, "1.2.3.4/28"), 1);
    EXPECT_EQ(findIPv4(tree, "1.2.3.0"), 1);
    EXPECT_EQ(findIPv4(tree, "1.2.3.4/29"), 1);
    EXPECT_EQ(findIPv4(tree, "1.2.3.4/27"), 0);
}

TEST(BTreeSuite, FindInvalidIPv4)
{
    bnode_t *tree = createIPv4TreeFromFile("/home/aldo/git/ip-lookup/test/data/ipv4list.txt");
    EXPECT_EQ(findIPv4(tree, "1.2.3."), 0);
}

TEST(BTreeSuite, InvalidIPv4InputFile)
{
    const bnode_t *tree = createIPv4TreeFromFile("/home/aldo/git/non-existent.txt");
    EXPECT_TRUE(tree->child[0] == nullptr);
    EXPECT_TRUE(tree->child[1] == nullptr);
}

TEST(BTreeSuite, AddIPv6AllZeroes)
{
    char s[] = "::";
    bnode_t *tree = createNode();
    const bnode_t *p = tree;
    uint8_t d = 128;

    insertIPv6(tree, s);
    while (d)
    {
        EXPECT_FALSE(p->child[0] == nullptr);
        EXPECT_TRUE(p->child[1] == nullptr);
        p = p->child[0];
        d--;
    }
    EXPECT_TRUE(p->child[0] == p);
    EXPECT_TRUE(p->child[1] == p);
}

TEST(BTreeSuite, AddIPv6AllOnes)
{
    char s[] = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff";
    bnode_t *tree = createNode();
    const bnode_t *p = tree;
    uint8_t d = 128;

    insertIPv6(tree, s);
    while (d)
    {
        EXPECT_TRUE(p->child[0] == nullptr);
        EXPECT_FALSE(p->child[1] == nullptr);
        p = p->child[1];
        d--;
    }
    EXPECT_TRUE(p->child[0] == p);
    EXPECT_TRUE(p->child[1] == p);
}

TEST(BTreeSuite, AddIPv6AllZeroesAndAllOnes)
{
    char s0[] = "::";
    char s1[] = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff";
    bnode_t *tree = createNode();
    const bnode_t *p = tree;
    uint8_t d = 127;

    insertIPv6(tree, s0);
    insertIPv6(tree, s1);
    EXPECT_FALSE(p->child[0] == nullptr);
    EXPECT_FALSE(p->child[1] == nullptr);
    p = tree->child[0];
    while (d)
    {
        EXPECT_FALSE(p->child[0] == nullptr);
        EXPECT_TRUE(p->child[1] == nullptr);
        p = p->child[0];
        d--;
    }
    EXPECT_TRUE(p->child[0] == p);
    EXPECT_TRUE(p->child[1] == p);

    p = tree->child[1];
    d = 127;
    while (d)
    {
        EXPECT_TRUE(p->child[0] == nullptr);
        EXPECT_FALSE(p->child[1] == nullptr);
        p = p->child[1];
        d--;
    }
    EXPECT_TRUE(p->child[0] == p);
    EXPECT_TRUE(p->child[1] == p);
}

TEST(BTreeSuite, AddIPv6AllOnesWithPrefix)
{
    char s[] = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff/120";
    bnode_t *tree = createNode();
    const bnode_t *p = tree;
    uint8_t d = 120;

    insertIPv6(tree, s);
    while (d)
    {
        EXPECT_TRUE(p->child[0] == nullptr);
        EXPECT_FALSE(p->child[1] == nullptr);
        p = p->child[1];
        d--;
    }
    EXPECT_TRUE(p->child[0] == p);
    EXPECT_TRUE(p->child[1] == p);
}

TEST(BTreeSuite, BitshiftLeftBy1)
{
    uint16_t ip[8];
    ip[0] = 1;
    ip[1] = 2;
    ip[2] = 4;
    ip[3] = 8;
    ip[4] = 16;
    ip[5] = 32;
    ip[6] = 64;
    ip[7] = 128;
    bitshiftLeft(ip, 1);
    EXPECT_EQ(ip[0], 2);
    EXPECT_EQ(ip[1], 4);
    EXPECT_EQ(ip[2], 8);
    EXPECT_EQ(ip[3], 16);
    EXPECT_EQ(ip[4], 32);
    EXPECT_EQ(ip[5], 64);
    EXPECT_EQ(ip[6], 128);
    EXPECT_EQ(ip[7], 256);
}

TEST(BTreeSuite, BitshiftLeftBy16)
{
    uint16_t ip[8];
    ip[0] = 1;
    ip[1] = 2;
    ip[2] = 4;
    ip[3] = 8;
    ip[4] = 16;
    ip[5] = 32;
    ip[6] = 64;
    ip[7] = 128;
    bitshiftLeft(ip, 16);
    EXPECT_EQ(ip[0], 2);
    EXPECT_EQ(ip[1], 4);
    EXPECT_EQ(ip[2], 8);
    EXPECT_EQ(ip[3], 16);
    EXPECT_EQ(ip[4], 32);
    EXPECT_EQ(ip[5], 64);
    EXPECT_EQ(ip[6], 128);
    EXPECT_EQ(ip[7], 0);
}

TEST(BTreeSuite, BitshiftLeftBy127)
{
    uint16_t ip[8];
    ip[0] = 1;
    ip[1] = 2;
    ip[2] = 4;
    ip[3] = 8;
    ip[4] = 16;
    ip[5] = 32;
    ip[6] = 64;
    ip[7] = 127;
    bitshiftLeft(ip, 127);
    EXPECT_EQ(ip[0], 32768);
    EXPECT_EQ(ip[1], 0);
    EXPECT_EQ(ip[2], 0);
    EXPECT_EQ(ip[3], 0);
    EXPECT_EQ(ip[4], 0);
    EXPECT_EQ(ip[5], 0);
    EXPECT_EQ(ip[6], 0);
    EXPECT_EQ(ip[7], 0);
}

TEST(BTreeSuite, BitshiftLeftBy128)
{
    uint16_t ip[8];
    ip[0] = 1;
    ip[1] = 2;
    ip[2] = 4;
    ip[3] = 8;
    ip[4] = 16;
    ip[5] = 32;
    ip[6] = 64;
    ip[7] = 128;
    bitshiftLeft(ip, 128);
    EXPECT_EQ(ip[0], 0);
    EXPECT_EQ(ip[1], 0);
    EXPECT_EQ(ip[2], 0);
    EXPECT_EQ(ip[3], 0);
    EXPECT_EQ(ip[4], 0);
    EXPECT_EQ(ip[5], 0);
    EXPECT_EQ(ip[6], 0);
    EXPECT_EQ(ip[7], 0);
}

TEST(BTreeSuite, DumpIPv6Tree)
{
    char s0[] = "::";
    char s1[] = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff";
    char s2[] = "1:2:3:4:5:6:7:8";
    bnode_t *tree = createNode();

    insertIPv6(tree, s0);
    insertIPv6(tree, s1);
    insertIPv6(tree, s2);

    testing::internal::CaptureStdout();
    std::cout << dumpIPv6Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "::\n1:2:3:4:5:6:7:8\nffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff\n3");
}

TEST(BTreeSuite, DumpIPv6TreeRepeatedIPs)
{
    char s0[] = "1:2:3:4:5:6:7:8";
    char s1[] = "10:20:30:40:50:60:70:80";
    bnode_t *tree = createNode();

    insertIPv6(tree, s0);
    insertIPv6(tree, s1);
    insertIPv6(tree, s0);
    insertIPv6(tree, s1);

    testing::internal::CaptureStdout();
    std::cout << dumpIPv6Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "1:2:3:4:5:6:7:8\n10:20:30:40:50:60:70:80\n2");
}

TEST(BTreeSuite, DumpIPv6TreeWithPrefix)
{
    char s0[] = "::";
    char s1[] = "10:20:30:40:50:60:70:80/104";
    char s2[] = "1:2:3:4:5:6:7:8/120";
    bnode_t *tree = createNode();

    insertIPv6(tree, s0);
    insertIPv6(tree, s1);
    insertIPv6(tree, s2);

    testing::internal::CaptureStdout();
    std::cout << dumpIPv6Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "::\n1:2:3:4:5:6:7:0/120\n10:20:30:40:50:60::/104\n3");
}

TEST(BTreeSuite, DumpIPv6TreeWithOverlappingRange)
{
    char s0[] = "1:2:3:4:5:6:7:8";
    char s1[] = "1:2:3:4:5:6:7:8/120";
    bnode_t *tree = createNode();

    insertIPv6(tree, s0);
    insertIPv6(tree, s1);

    testing::internal::CaptureStdout();
    std::cout << dumpIPv6Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "1:2:3:4:5:6:7:0/120\n1");
}

TEST(BTreeSuite, DumpIPv6TreeWithOverlappingRangeInReverseOrder)
{
    char s0[] = "1:2:3:4:5:6:7:8/120";
    char s1[] = "1:2:3:4:5:6:7:8";
    bnode_t *tree = createNode();

    insertIPv6(tree, s0);
    insertIPv6(tree, s1);

    testing::internal::CaptureStdout();
    std::cout << dumpIPv6Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "1:2:3:4:5:6:7:0/120\n1");
}

TEST(BTreeSuite, CountIPv6TreeSingleAddress)
{
    char s0[] = "1:2:3:4:5:6:7:8";
    bnode_t *tree = createNode();

    insertIPv6(tree, s0);

    testing::internal::CaptureStdout();
    std::cout << countIPv6Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "1");
}

TEST(BTreeSuite, CountIPv6TreeMultipleAddresses)
{
    char s0[] = "1:2:3:4:5:6:7:8";
    char s1[] = "10:20:30:40:50:60:70:80";
    char s2[] = "a1:a2:a3:a4:a5:a6:a7:a8";
    char s3[] = "11:22:33:44:55:66:77:88";
    bnode_t *tree = createNode();

    insertIPv6(tree, s0);
    insertIPv6(tree, s1);
    insertIPv6(tree, s2);
    insertIPv6(tree, s3);

    testing::internal::CaptureStdout();
    std::cout << countIPv6Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "4");
}

TEST(BTreeSuite, CountIPv6TreeOverlappingRanges)
{
    char s0[] = "1:2:3:4:5:6:7:8/120";
    char s1[] = "1:2:3:4:5:6:7:8/96";
    bnode_t *tree = createNode();

    insertIPv6(tree, s0);
    insertIPv6(tree, s1);

    testing::internal::CaptureStdout();
    std::cout << countIPv6Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "1");
}

TEST(BTreeSuite, CreateIPv6TreeFromTinyFile)
{
    bnode_t *tree = createIPv6TreeFromFile("/home/aldo/git/ip-lookup/test/data/ipv6single.txt");
    EXPECT_EQ(countIPv6Tree(tree), 1);
}

TEST(BTreeSuite, CreateIPv6TreeFromSmallFile)
{
    bnode_t *tree = createIPv6TreeFromFile("/home/aldo/git/ip-lookup/test/data/ipv6list.txt");
    EXPECT_EQ(countIPv6Tree(tree), 10);
}

TEST(BTreeSuite, CreateIPv6TreeFromLargeFile)
{
    bnode_t *tree = createIPv6TreeFromFile("/home/aldo/git/ip-lookup/test/data/inbound_v6.txt");
    EXPECT_GT(countIPv6Tree(tree), 1800);
}

TEST(BTreeSuite, FindIPv6InSmallFile)
{
    bnode_t *tree = createIPv6TreeFromFile("/home/aldo/git/ip-lookup/test/data/ipv6list.txt");
    EXPECT_EQ(findIPv6(tree, "1:2:3:4:5:6:7:8"), 1);
    EXPECT_EQ(findIPv6(tree, "2:3:4:5:6:7:8:9"), 1);
    EXPECT_EQ(findIPv6(tree, "3:4:5:6:7:8:9:a"), 1);
    EXPECT_EQ(findIPv6(tree, "4:5:6:7:8:9:a:b"), 1);
    EXPECT_EQ(findIPv6(tree, "5:6:7:8:9:a:b:c"), 1);
    EXPECT_EQ(findIPv6(tree, "10:20:30:40:50:60:70:80"), 1);
    EXPECT_EQ(findIPv6(tree, "20:30:40:50:60:70:80:90"), 1);
    EXPECT_EQ(findIPv6(tree, "30:40:50:60:70:80:90:a0"), 1);
    EXPECT_EQ(findIPv6(tree, "40:50:60:70:80:90:a0:b0"), 1);
    EXPECT_EQ(findIPv6(tree, "50:60:70:80:90:a0:b0:c0"), 1);

    EXPECT_EQ(findIPv6(tree, "1:2:3:4:5:6:7:9"), 0);
    EXPECT_EQ(findIPv6(tree, "1:2:3:4:5:6:7:a"), 0);
}

TEST(BTreeSuite, FindIPv6InRange)
{
    bnode_t *tree = createIPv6TreeFromFile("/home/aldo/git/ip-lookup/test/data/ipv6range.txt");
    EXPECT_EQ(findIPv6(tree, "1:2:3:4:5:6:7:8"), 1);
    EXPECT_EQ(findIPv6(tree, "1:2:3:4:5:6:7:0"), 1);
    EXPECT_EQ(findIPv6(tree, "1:2:3:4:5:6:7:10"), 0);
    EXPECT_EQ(findIPv6(tree, "2:3:4:5:6:7:8:0"), 1);
    EXPECT_EQ(findIPv6(tree, "2:3:4:5:6:7:8:ff"), 1);
    EXPECT_EQ(findIPv6(tree, "2:3:4:5:6:7:8:100"), 0);
}

TEST(BTreeSuite, FindIPv6InRangeWithMask120)
{
    bnode_t *tree = createIPv6TreeFromFile("/home/aldo/git/ip-lookup/test/data/ipv6range.txt");
    EXPECT_EQ(findIPv6(tree, "2:3:4:5:6:7:8:0"), 1);
    EXPECT_EQ(findIPv6(tree, "2:3:4:5:6:7:8:ff"), 1);
    EXPECT_EQ(findIPv6(tree, "2:3:4:5:6:7:8:100"), 0);
    EXPECT_EQ(findIPv6(tree, "2:3:4:5:6:7:7:0"), 0);
    EXPECT_EQ(findIPv6(tree, "2:3:4:5:6:7:9:0"), 0);
}

TEST(BTreeSuite, FindIPv6InRangeWithMask127)
{
    bnode_t *tree = createIPv6TreeFromFile("/home/aldo/git/ip-lookup/test/data/ipv6range.txt");
    EXPECT_EQ(findIPv6(tree, "4:5:6:7:8:9:a:9"), 0);
    EXPECT_EQ(findIPv6(tree, "4:5:6:7:8:9:a:a"), 1);
    EXPECT_EQ(findIPv6(tree, "4:5:6:7:8:9:a:b"), 1);
    EXPECT_EQ(findIPv6(tree, "4:5:6:7:8:9:a:c"), 0);
}

TEST(BTreeSuite, FindIPv6Range)
{
    bnode_t *tree = createIPv6TreeFromFile("/home/aldo/git/ip-lookup/test/data/ipv6range.txt");
    EXPECT_EQ(findIPv6(tree, "2:3:4:5:6:7:8:9/120"), 1);
    EXPECT_EQ(findIPv6(tree, "2:3:4:5:6:7:8:0"), 1);
    EXPECT_EQ(findIPv6(tree, "2:3:4:5:6:7:8:0/121"), 1);
    EXPECT_EQ(findIPv6(tree, "2:3:4:5:6:7:8:9/119"), 0);
}

TEST(BTreeSuite, FindInvalidIPv6)
{
    bnode_t *tree = createIPv6TreeFromFile("/home/aldo/git/ip-lookup/test/data/ipv6list.txt");
    EXPECT_EQ(findIPv6(tree, "1:2:3:4:5:6:7:"), 0);
}

TEST(BTreeSuite, InvalidIPv6InputFile)
{
    const bnode_t *tree = createIPv6TreeFromFile("/home/aldo/git/non-existent.txt");
    EXPECT_TRUE(tree->child[0] == nullptr);
    EXPECT_TRUE(tree->child[1] == nullptr);
}
