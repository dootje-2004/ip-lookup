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
    EXPECT_TRUE(p->child[0] == NULL);
    EXPECT_TRUE(p->child[1] == NULL);
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
    EXPECT_TRUE(p->child[0] == NULL);
    EXPECT_TRUE(p->child[1] == NULL);
}
