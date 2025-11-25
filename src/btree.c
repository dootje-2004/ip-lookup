#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "btree.h"
#include "ip.h"

bnode_t *createNode()
{
    bnode_t *newNode = (bnode_t *)malloc(sizeof(bnode_t));
    newNode->child[0] = newNode->child[1] = NULL;
    return newNode;
}

void deleteSubtree(bnode_t *node)
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

void insertIPv4(bnode_t *root, const char *s)
{
    uint8_t byte;
    bnode_t *node_ptr;

    ipv4_t ip = read_ipv4(s);
    if (ip.ps == 0)
    {
        return;
    }

    node_ptr = root;
    for (uint8_t depth = 0; depth < ip.ps; depth++)
    {
        if (node_ptr == node_ptr->child[0])
        {
            return;
        }
        byte = ip.ip >> 31;
        if (node_ptr->child[byte] == NULL)
        {
            bnode_t *childNode = createNode();
            node_ptr->child[byte] = childNode;
        }
        node_ptr = node_ptr->child[byte];
        ip.ip <<= 1;
    }
    if (node_ptr->child[0] != node_ptr)
    {
        deleteSubtree(node_ptr->child[0]);
        deleteSubtree(node_ptr->child[1]);
        node_ptr->child[0] = node_ptr;
        node_ptr->child[1] = node_ptr;
    }
}

void insertIPv6(bnode_t *root, const char *s)
{
    uint8_t byte;
    bnode_t *node_ptr;
    uint8_t group_index = 0;

    ipv6_t ip = read_ipv6(s);
    if (ip.ps == 0)
    {
        return;
    }

    node_ptr = root;
    for (uint8_t depth = 0; depth < ip.ps; depth++)
    {
        if (node_ptr == node_ptr->child[0])
        {
            return;
        }
        byte = ip.ip[group_index] >> 15;
        if (node_ptr->child[byte] == NULL)
        {
            bnode_t *childNode = createNode();
            node_ptr->child[byte] = childNode;
        }
        node_ptr = node_ptr->child[byte];
        ip.ip[group_index] <<= 1;
        if ((depth % 16) == 15)
        {
            group_index++;
        }
    }
    if (node_ptr->child[0] != node_ptr)
    {
        deleteSubtree(node_ptr->child[0]);
        deleteSubtree(node_ptr->child[1]);
        node_ptr->child[0] = node_ptr;
        node_ptr->child[1] = node_ptr;
    }
}

void printIPv4(FILE *stream, ipv4_t ipv4)
{
    char s[19];
    ipv4tostring(s, ipv4);
    fprintf(stream, "%s\n", s);
}

uint32_t walkIPv4Recursive(bnode_t **node, uint8_t depth, const uint32_t ip, const uint8_t printIPs)
{
    uint32_t counter = 0;
    ipv4_t ipv4;
    
    if (*node == NULL)
    {
        return 0;
    }
    
    if (*node != (*node)->child[0])
    {
        counter += walkIPv4Recursive(&((*node)->child[0]), depth + 1, ip << 1, printIPs);
        counter += walkIPv4Recursive(&((*node)->child[1]), depth + 1, (ip << 1) + 1, printIPs);
    }
    
    if (counter == 0)
    {
        ipv4.ip = depth > 0 ? ip << (32 - depth) : 0;
        ipv4.ps = depth;
        if (printIPs)
        {
            printIPv4(stdout, ipv4);
        }
        counter = 1;
    }
    return counter;
}

uint32_t dumpIPv4Tree(bnode_t *node)
{
    return walkIPv4Recursive(&node, 0, 0, 1);
}

uint32_t countIPv4Tree(bnode_t *node)
{
    return walkIPv4Recursive(&node, 0, 0, 0);
}

bnode_t *createIPv4TreeFromFile(const char *filename)
{
    const uint8_t MAX_IP_LEN = 44;
    bnode_t *root = createNode();
    FILE *fp = fopen(filename, "r");
    char buffer[MAX_IP_LEN];
    int c;
    uint8_t buffer_index = 0;

    if (fp == NULL)
    {
        fprintf(stderr, "Error opening file %s\n", filename);
        return root;
    }

    while ((c = getc(fp)) != EOF)
    {
        if ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t') || (c == EOF))
        {
            buffer[buffer_index++] = '\0';
            insertIPv4(root, buffer);
            for (buffer_index = 0; buffer_index < MAX_IP_LEN; buffer_index++)
            {
                buffer[buffer_index] = '\0';
            }
            buffer_index = 0;
        }
        else
        {
            buffer[buffer_index++] = (char)c;
        }
    }

    fclose(fp);
    return root;
}

uint8_t findIPv4(bnode_t *root, const char *ipv4_string)
{
    ipv4_t ipv4 = read_ipv4(ipv4_string);
    if (ipv4.ps == 0)
    {
        return 0;
    }

    bnode_t **tree_ptr = &root;
    while ((*tree_ptr != NULL) && (*tree_ptr != (*tree_ptr)->child[0]) && (ipv4.ps > 0))
    {
        *tree_ptr = (*tree_ptr)->child[ipv4.ip >> 31];
        ipv4.ip <<= 1;
        ipv4.ps--;
    }
    return !((*tree_ptr == NULL) || (*tree_ptr != (*tree_ptr)->child[0]));
}

void bitshiftLeft(uint16_t *lst, const uint8_t shift)
{
    uint8_t groupShift = shift / 16;
    uint8_t remainingShift = shift % 16;
    uint8_t i;

    for (i = 0; i < (8 - groupShift); i++)
    {
        lst[i] = lst[i + groupShift];
    }
    for (i = 8 - groupShift; i < 8; i++)
    {
        lst[i] = 0;
    }
    for (i = 0; i < 8; i++)
    {
        lst[i] = (uint16_t)(lst[i] << remainingShift);
        if (i == 7)
        {
            break;
        }
        lst[i] += lst[i+1] >> (16 - remainingShift);
    }
}

void printIPv6(FILE *stream, ipv6_t ipv6)
{
    char s[44];
    ipv6tostring(s, ipv6);
    fprintf(stream, "%s\n", s);
}

uint32_t walkIPv6Recursive(bnode_t **node, const uint8_t depth, const uint16_t *ip, const uint8_t printIPs)
{
    uint32_t counter = 0;
    ipv6_t ipv6;
    uint16_t ipShift0[8];
    uint16_t ipShift1[8];
    uint16_t ipCopy[8];
    uint8_t i;

    if (*node == NULL)
    {
        return 0;
    }
    
    if (*node != (*node)->child[0])
    {
        for (i = 0; i < 8; i++)
        {
            ipShift0[i] = ip[i];
            ipShift1[i] = ip[i];
        }
        bitshiftLeft(ipShift0, 1);
        bitshiftLeft(ipShift1, 1);
        ipShift1[7] += 1;
        counter += walkIPv6Recursive(&((*node)->child[0]), depth + 1, ipShift0, printIPs);
        counter += walkIPv6Recursive(&((*node)->child[1]), depth + 1, ipShift1, printIPs);
    }
    
    if (counter == 0)
    {
        for (i = 0; i < 8; i++)
        {
            ipCopy[i] = ip[i];
        }
        bitshiftLeft(ipCopy, 128 - depth);
        for (i = 0; i < 8; i++)
        {
            ipv6.ip[i] = ipCopy[i];
        }
        ipv6.ps = depth;
        if (printIPs)
        {
            printIPv6(stdout, ipv6);
        }
        counter = 1;
    }
    return counter;
}

uint32_t dumpIPv6Tree(bnode_t *node)
{
    uint16_t ip[8];
    for (uint8_t i = 0; i < 8; i++)
    {
        ip[i] = 0;
    }
    return walkIPv6Recursive(&node, 0, ip, 1);
}

uint32_t countIPv6Tree(bnode_t *node)
{
    uint16_t ip[8];
    for (uint8_t i = 0; i < 8; i++)
    {
        ip[i] = 0;
    }
    return walkIPv6Recursive(&node, 0, ip, 0);
}

bnode_t *createIPv6TreeFromFile(const char *filename)
{
    const uint8_t MAX_IP_LEN = 44;
    bnode_t *root = createNode();
    FILE *fp = fopen(filename, "r");
    char buffer[MAX_IP_LEN];
    int c;
    uint8_t buffer_index = 0;

    if (fp == NULL)
    {
        fprintf(stderr, "Error opening file %s\n", filename);
        return root;
    }

    while ((c = getc(fp)) != EOF)
    {
        if ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t') || (c == EOF))
        {
            buffer[buffer_index++] = '\0';
            insertIPv6(root, buffer);
            for (buffer_index = 0; buffer_index < MAX_IP_LEN; buffer_index++)
            {
                buffer[buffer_index] = '\0';
            }
            buffer_index = 0;
        }
        else
        {
            buffer[buffer_index++] = (char)c;
        }
    }

    fclose(fp);
    return root;
}

uint8_t findIPv6(bnode_t *root, const char *ipv6_string)
{
    ipv6_t ipv6 = read_ipv6(ipv6_string);

    if (ipv6.ps == 0)
    {
        return 0;
    }

    bnode_t **tree_ptr = &root;
    while ((*tree_ptr != NULL) && (*tree_ptr != (*tree_ptr)->child[0]) && (ipv6.ps > 0))
    {
        *tree_ptr = (*tree_ptr)->child[ipv6.ip[0] >> 15];
        bitshiftLeft(ipv6.ip, 1);
        ipv6.ps--;
    }
    return !((*tree_ptr == NULL) || (*tree_ptr != (*tree_ptr)->child[0]));
}
