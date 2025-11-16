#include <stdlib.h>
#include <stddef.h>
#include "btree.h"
#include "ip.h"

bnode_t* createNode()
{
    bnode_t* newNode = (bnode_t*)malloc(sizeof(bnode_t));
    newNode->child[0] = newNode->child[1] = NULL;
    return newNode;
}

int insertIPv4(bnode_t** root, const char* s)
{
    uint8_t d;  // Tree depth.
    uint8_t b;  // Byte value.
    bnode_t* p; // Node pointer;

    ipv4_t ip = read_ipv4(s);
    p = *root;
    for (d = 0; d < ip.ps; d++)
    {
        b = ip.ip >> 31;
        if (p->child[b] == NULL)
        {
            bnode_t* childNode = createNode();
            p->child[b] = childNode;
        }
        p = p->child[b];
        ip.ip <<=1;
    }
    return 0;
}

int insertIPv6(bnode_t* root,  const char* s)
{
    // TODO
    read_ipv6(s);
    (*root).child[0] = NULL;
    (*root).child[1] = NULL;
    return 0;
}

uint32_t dumpIPv4Tree(bnode_t* root)
{
    // TODO
    return 0;
}
