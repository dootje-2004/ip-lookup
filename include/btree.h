#include "ip.h"

/**
 * @brief Node in a binary tree.
 * Its value (0 or 1) is not stored, but follows from its parent:
 * if the parent node points via .node0, this node represents 0,
 * and similarly for .node1.
 */
typedef struct node
{
    struct node *node0;
    struct node *node1;
} bnode_t;

/**
 * @brief Adds an IPv4 variable to a binary tree.
 * 
 * @return int 0 if successfully added, 1 if the address
 * (or an encompassing range) already figures in the tree,
 * 2 if the address could not be added.
 */
int addIPv4(bnode_t*, ipv4_t);

/**
 * @brief Adds an IPv6 variable to a binary tree.
 * 
 * @return int 0 if successfully added, 1 if the address
 * (or an encompassing range) already figures in the tree,
 * 2 if the address could not be added.
 */
int addIPv6(bnode_t*, ipv6_t);
