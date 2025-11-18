#include <gtest/gtest.h>

extern "C"
{
#include "btree.h"
}

TEST(BTreeSuite, NewEmptyTree)
{
    bnode_t *tree = createNode();
    EXPECT_TRUE(tree->child[0] == NULL);
    EXPECT_TRUE(tree->child[1] == NULL);
}

TEST(BTreeSuite, AddIPv4AllZeroes)
{
    char s[] = "0.0.0.0";
    bnode_t *tree = createNode();
    bnode_t *p = tree;
    uint8_t d = 32;

    insertIPv4(tree, s);
    while (d)
    {
        EXPECT_FALSE(p->child[0] == NULL);
        EXPECT_TRUE(p->child[1] == NULL);
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
    bnode_t *p = tree;
    uint8_t d = 32;

    insertIPv4(tree, s);
    while (d)
    {
        EXPECT_TRUE(p->child[0] == NULL);
        EXPECT_FALSE(p->child[1] == NULL);
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
    bnode_t *p = tree;
    uint8_t d = 31;

    insertIPv4(tree, s0);
    insertIPv4(tree, s1);
    EXPECT_FALSE(p->child[0] == NULL);
    EXPECT_FALSE(p->child[1] == NULL);
    p = tree->child[0];
    while (d)
    {
        EXPECT_FALSE(p->child[0] == NULL);
        EXPECT_TRUE(p->child[1] == NULL);
        p = p->child[0];
        d--;
    }
    EXPECT_TRUE(p->child[0] == p);
    EXPECT_TRUE(p->child[1] == p);

    p = tree->child[1];
    d = 31;
    while (d)
    {
        EXPECT_TRUE(p->child[0] == NULL);
        EXPECT_FALSE(p->child[1] == NULL);
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
    bnode_t *p = tree;
    uint8_t d = 24;

    insertIPv4(tree, s);
    while (d)
    {
        EXPECT_TRUE(p->child[0] == NULL);
        EXPECT_FALSE(p->child[1] == NULL);
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

TEST(BTreeSuite, CreateTreeFromTinyFile)
{
    bnode_t *tree = createIPv4TreeFromFile("../test/data/ipv4single.txt");
    EXPECT_EQ(countIPv4Tree(tree), 1);
}

TEST(BTreeSuite, CreateTreeFromSmallFile)
{
    bnode_t *tree = createIPv4TreeFromFile("../test/data/ipv4list.txt");
    EXPECT_EQ(countIPv4Tree(tree), 10);
}

TEST(BTreeSuite, CreateTreeFromLargeFile)
{
    bnode_t *tree = createIPv4TreeFromFile("../test/data/inbound.txt");
    EXPECT_GT(countIPv4Tree(tree), 1000000);
}

TEST(BTreeSuite, FindIPv4InSmallFile)
{
    bnode_t *tree = createIPv4TreeFromFile("../test/data/ipv4list.txt");
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
    bnode_t *tree = createIPv4TreeFromFile("../test/data/ipv4range.txt");
    EXPECT_EQ(findIPv4(tree, "1.2.3.0"), 1);
    EXPECT_EQ(findIPv4(tree, "1.2.3.15"), 1);
    EXPECT_EQ(findIPv4(tree, "1.2.3.16"), 0);
    EXPECT_EQ(findIPv4(tree, "2.3.1.0"), 1);
    EXPECT_EQ(findIPv4(tree, "2.3.15.0"), 1);
    EXPECT_EQ(findIPv4(tree, "2.3.16.0"), 0);
}

TEST(BTreeSuite, FindIPv4InRangeWithMask24)
{
    bnode_t *tree = createIPv4TreeFromFile("../test/data/ipv4range.txt");
    EXPECT_EQ(findIPv4(tree, "3.4.5.0"), 1);
    EXPECT_EQ(findIPv4(tree, "3.4.5.127"), 1);
    EXPECT_EQ(findIPv4(tree, "3.4.5.255"), 1);
    EXPECT_EQ(findIPv4(tree, "3.4.4.0"), 0);
    EXPECT_EQ(findIPv4(tree, "3.4.6.0"), 0);
}

TEST(BTreeSuite, FindIPv4InRangeWithMask31)
{
    bnode_t *tree = createIPv4TreeFromFile("../test/data/ipv4range.txt");
    EXPECT_EQ(findIPv4(tree, "6.7.8.7"), 0);
    EXPECT_EQ(findIPv4(tree, "6.7.8.8"), 1);
    EXPECT_EQ(findIPv4(tree, "6.7.8.9"), 1);
    EXPECT_EQ(findIPv4(tree, "6.7.8.10"), 0);
}

TEST(BTreeSuite, FindIPv4Range)
{
    bnode_t *tree = createIPv4TreeFromFile("../test/data/ipv4range.txt");
    EXPECT_EQ(findIPv4(tree, "1.2.3.4/28"), 1);
    EXPECT_EQ(findIPv4(tree, "1.2.3.4/29"), 1);
    EXPECT_EQ(findIPv4(tree, "1.2.3.4/27"), 0);
}
