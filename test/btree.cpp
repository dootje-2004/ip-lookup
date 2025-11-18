#include <gtest/gtest.h>

extern "C"
{
    #include "btree.h"
}

TEST(BTreeSuite, NewEmptyTree)
{
    bnode_t* tree = createNode();
    EXPECT_TRUE(tree->child[0] == NULL);
    EXPECT_TRUE(tree->child[1] == NULL);
}

TEST(BTreeSuite, AddIPv4AllZeroes)
{
    char s[] = "0.0.0.0";
    bnode_t* tree = createNode();
    bnode_t* p = tree;
    uint8_t d = 32;
    
    insertIPv4(&tree, s);
    while(d)
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
    bnode_t* tree = createNode();
    bnode_t* p = tree;
    uint8_t d = 32;

    insertIPv4(&tree, s);
    while(d)
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
    bnode_t* tree = createNode();
    bnode_t* p = tree;
    uint8_t d = 31;

    insertIPv4(&tree, s0);
    insertIPv4(&tree, s1);
    EXPECT_FALSE(p->child[0] == NULL);
    EXPECT_FALSE(p->child[1] == NULL);
    p = tree->child[0];
    while(d)
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
    while(d)
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
    bnode_t* tree = createNode();
    bnode_t* p = tree;
    uint8_t d = 24;

    insertIPv4(&tree, s);
    while(d)
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
    bnode_t* tree = createNode();

    insertIPv4(&tree, s0);
    insertIPv4(&tree, s1);
    insertIPv4(&tree, s2);

    testing::internal::CaptureStdout();
    std::cout << dumpIPv4Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "0.0.0.0\n1.2.3.4\n255.255.255.255\n3");
}

TEST(BTreeSuite, DumpIPv4TreeRepeatedIPs)
{
    char s0[] = "1.2.3.4";
    char s1[] = "10.20.30.40";
    bnode_t* tree = createNode();

    insertIPv4(&tree, s0);
    insertIPv4(&tree, s1);
    insertIPv4(&tree, s0);
    insertIPv4(&tree, s1);

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
    bnode_t* tree = createNode();

    insertIPv4(&tree, s0);
    insertIPv4(&tree, s1);
    insertIPv4(&tree, s2);

    testing::internal::CaptureStdout();
    std::cout << dumpIPv4Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "0.0.0.0\n1.2.3.0/28\n255.255.0.0/16\n3");
}

TEST(BTreeSuite, DumpIPv4TreeWithOverlappingRange)
{
    char s0[] = "1.2.3.4";
    char s1[] = "1.2.3.4/28";
    bnode_t* tree = createNode();

    insertIPv4(&tree, s0);
    insertIPv4(&tree, s1);

    testing::internal::CaptureStdout();
    std::cout << dumpIPv4Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "1.2.3.0/28\n1");
}

TEST(BTreeSuite, DumpIPv4TreeWithOverlappingRangeInReverseOrder)
{
    char s0[] = "1.2.3.4/28";
    char s1[] = "1.2.3.4";
    bnode_t* tree = createNode();

    insertIPv4(&tree, s0);
    insertIPv4(&tree, s1);

    testing::internal::CaptureStdout();
    std::cout << dumpIPv4Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "1.2.3.0/28\n1");
}

TEST(BTreeSuite, CountIPv4TreeSingleAddress)
{
    char s0[] = "1.2.3.4";
    bnode_t* tree = createNode();

    insertIPv4(&tree, s0);

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
    bnode_t* tree = createNode();

    insertIPv4(&tree, s0);
    insertIPv4(&tree, s1);
    insertIPv4(&tree, s2);
    insertIPv4(&tree, s3);

    testing::internal::CaptureStdout();
    std::cout << countIPv4Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "4");
}

TEST(BTreeSuite, CountIPv4TreeOverlappingRanges)
{
    char s0[] = "1.2.3.4/28";
    char s1[] = "1.2.3.4/24";
    bnode_t* tree = createNode();

    insertIPv4(&tree, s0);
    insertIPv4(&tree, s1);

    testing::internal::CaptureStdout();
    std::cout << countIPv4Tree(tree);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "1");
}

TEST(BTreeSuite, CreateTreeFromFileWithOneAddress)
{
    bnode_t* tree = createIPv4TreeFromFile("../test/data/ipv4single.txt");
    EXPECT_EQ(countIPv4Tree(tree), 1);
}

TEST(BTreeSuite, CreateTreeFromFileWithSeveralAddresses)
{
    bnode_t* tree = createIPv4TreeFromFile("../test/data/ipv4list_tiny.txt");
    EXPECT_EQ(countIPv4Tree(tree), 10);
}
