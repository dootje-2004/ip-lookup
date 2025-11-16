#include "ip.h"

/**
 * @brief Node in a binary tree.
 * Its value (0 or 1) is not stored, but follows from its parent:
 * if the parent node points via .child[0], this node represents 0,
 * and similarly for .child[1].
 */
typedef struct node
{
    struct node *child[2];
} bnode_t;

/**
 * @brief Returns a pointer to an empty bnode.
 * 
 * @return bnode_t
 */
bnode_t* createNode();

/**
 * @brief Adds an IPv4 string to a binary tree.
 * 
 * @return int 0 if successfully added, 1 if the address
 * (or an encompassing range) already figures in the tree,
 * 2 if the address could not be added.
 */
int insertIPv4(bnode_t**, const char*);

/**
 * @brief Adds an IPv6 string to a binary tree.
 * 
 * @return int 0 if successfully added, 1 if the address
 * (or an encompassing range) already figures in the tree,
 * 2 if the address could not be added.
 */
int insertIPv6(bnode_t*, const char*);

/**
 * @brief Prints all IP addresses in a binary tree to stdout.
 * 
 * @return uint32_t  The number of addresses in the tree.
 */
uint32_t dumpIPv4Tree(bnode_t*);
