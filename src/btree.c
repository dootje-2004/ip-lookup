#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "btree.h"
#include "ip.h"

bnode_t* createNode()
{
    bnode_t* newNode = (bnode_t*)malloc(sizeof(bnode_t));
    newNode->child[0] = newNode->child[1] = NULL;
    return newNode;
}

void deleteSubtree(bnode_t* node)
{
    if (node == NULL)
    {
        return;
    }
    if (node != node->child[0])
    {
        deleteSubtree(node->child[0]);
        deleteSubtree(node->child[1]);
    }
    free(node);
}

void insertIPv4(bnode_t** root, const char* s)
{
    uint8_t d;  // Tree depth.
    uint8_t b;  // Byte value.
    bnode_t* p; // Node pointer;

    ipv4_t ip = read_ipv4(s);
    p = *root;
    for (d = 0; d < ip.ps; d++)
    {
        if (p == p->child[0])
        {
            return;
        }
        b = ip.ip >> 31;
        if (p->child[b] == NULL)
        {
            bnode_t* childNode = createNode();
            p->child[b] = childNode;
        }
        p = p->child[b];
        ip.ip <<=1;
    }
    deleteSubtree(p->child[0]);
    deleteSubtree(p->child[1]);
    p->child[0] = p;
    p->child[1] = p;
}

int insertIPv6(bnode_t* root,  const char* s)
{
    // TODO
    read_ipv6(s);
    (*root).child[0] = NULL;
    (*root).child[1] = NULL;
    return 0;
}

void printIPv4(FILE *stream, ipv4_t ipv4)
{
    char s[19];
    ip4string(s, ipv4);
    fprintf(stream, "%s\n", s);
}

uint32_t dumpIPv4Recursive(bnode_t* node, uint8_t depth, uint32_t ip)
{
    uint32_t ctr = 0;   // Node counter
    ipv4_t ipv4;        // Helper variable

    if (node == NULL)
    {
        return 0;
    }

    if (node != node->child[0])
    {
        ctr += dumpIPv4Recursive(node->child[0], depth + 1, ip << 1);
        ctr += dumpIPv4Recursive(node->child[1], depth + 1, (ip << 1) + 1);
    }

    if (ctr == 0)
    {
        ipv4.ip = ip << (32 - depth);
        ipv4.ps = depth;
        printIPv4(stdout, ipv4);
        ctr = 1;
    }
    return ctr;
}

uint32_t dumpIPv4Tree(bnode_t* node)
{
    return dumpIPv4Recursive(node, 0, 0);
}
