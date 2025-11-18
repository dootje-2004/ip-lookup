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
    uint8_t d;  // tree depth.
    uint8_t b;  // byte value.
    bnode_t* p; // node pointer;

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
    if ( p->child[0] != p)
    {
        deleteSubtree(p->child[0]);
        deleteSubtree(p->child[1]);
        p->child[0] = p;
        p->child[1] = p;
    }
}

int insertIPv6(bnode_t* root,  const char* s)
{
    // TODO
    read_ipv6(s);
    root->child[0] = NULL;
    root->child[1] = NULL;
    return 0;
}

void printIPv4(FILE *stream, ipv4_t ipv4)
{
    char s[19];
    ip4string(s, ipv4);
    fprintf(stream, "%s\n", s);
}

uint32_t walkIPv4Recursive(bnode_t* node, uint8_t depth, uint32_t ip, uint8_t printIPs)
{
    uint32_t ctr = 0;   // address counter
    ipv4_t ipv4;        // helper variable

    if (node == NULL)
    {
        return 0;
    }

    if (node != node->child[0])
    {
        ctr += walkIPv4Recursive(node->child[0], depth + 1, ip << 1, printIPs);
        ctr += walkIPv4Recursive(node->child[1], depth + 1, (ip << 1) + 1, printIPs);
    }

    if (ctr == 0)
    {
        ipv4.ip = ip << (32 - depth);
        ipv4.ps = depth;
        if (printIPs)
        {
            printIPv4(stdout, ipv4);
        }
        ctr = 1;
    }
    return ctr;
}

uint32_t dumpIPv4Tree(bnode_t* node)
{
    return walkIPv4Recursive(node, 0, 0, 1);
}

uint32_t countIPv4Tree(bnode_t* node)
{
    return walkIPv4Recursive(node, 0, 0, 0);
}

bnode_t* createIPv4TreeFromFile(const char* filename)
{
    const uint8_t MAX_IP_LEN = 44;
    bnode_t* root = createNode();
    FILE* fp = fopen(filename, "r");
    char buf[MAX_IP_LEN];
    char c;         // helper variable
    uint8_t p = 0;  // string index

    if (fp == NULL)
    {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    while ((c = getc(fp)) != EOF)
    {
        if ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t') || (c == EOF))
        {
            buf[p++] = '\0';
            insertIPv4(&root, buf);
            for (p = 0; p < MAX_IP_LEN; p++)
            {
                buf[p] = '\0';
            }
            p = 0;
        }
        else
        {
            buf[p++] = c;
        }
    }
    
    fclose(fp);
    return root;
}
