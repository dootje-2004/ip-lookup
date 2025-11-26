/**
 * @file btree.h
 * @author Aldo Verlinde (aldo.verlinde@gmail.com)
 * @brief Library header for binary tree holding IP addresses.
 * @version 0.1
 * @date 2025-11-25
 */
#ifndef BTREE_H_
#define BTREE_H_

#include "ip.h"

/**
 * @brief Node in a binary tree.
 * Its value (0 or 1) is not stored, but follows from its parent:
 * if the parent node points via .child[0], this node represents 0,
 * and similarly for .child[1].
 *
 * A leaf node has both child nodes pointing back to itself.
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
bnode_t *createNode();

/**
 * @brief Adds an IPv4 string to a binary tree.
 *
 * @return int 0 if successfully added, 1 if the address
 * (or an encompassing range) already figures in the tree,
 * 2 if the address could not be added.
 */
void insertIPv4(bnode_t *, const char *);

/**
 * @brief Adds an IPv6 string to a binary tree.
 *
 * @return int 0 if successfully added, 1 if the address
 * (or an encompassing range) already figures in the tree,
 * 2 if the address could not be added.
 */
void insertIPv6(bnode_t *, const char *);

/**
 * @brief Prints all IP addresses in an IPv4 tree to stdout.
 *
 * @return uint32_t  The number of addresses in the tree.
 */
uint32_t dumpIPv4Tree(bnode_t *);

/**
 * @brief Returns the number of addresses in an IPv4 tree.
 *
 * @return uint32_t  Number of addresses.
 */
uint32_t countIPv4Tree(bnode_t *);

/**
 * @brief Returns a pointer to the root of an IPv4 tree,
 * filled with the addresses read from a text file.
 * If there is an error opening the text file, an empty
 * node (with child[0] == child[1] == NULL) is returned.
 *
 * @return bnode_t*  Pointer to the root of the IPv4 tree.
 */
bnode_t *createIPv4TreeFromFile(const char *);

/**
 * @brief Checks if an IPv4 address occurs in an IPv4 tree.
 *
 * @return uint8_t  1 if found, 0 if not.
 */
uint8_t findIPv4(bnode_t *, const char *);

/**
 * @brief Prints all IP addresses in an IPv6 tree to stdout.
 *
 * @return uint32_t  The number of addresses in the tree.
 */
uint32_t dumpIPv6Tree(bnode_t *);

void bitshiftLeft(uint16_t *, const uint8_t);

/**
 * @brief Returns the number of addresses in an IPv6 tree.
 *
 * @return uint32_t  Number of addresses.
 */
uint32_t countIPv6Tree(bnode_t *);

/**
 * @brief Returns a pointer to the root of an IPv6 tree,
 * filled with the addresses read from a text file.
 * If there is an error opening the text file, an empty
 * node (with child[0] == child[1] == NULL) is returned.
 *
 * @return bnode_t*  Pointer to the root of the IPv6 tree.
 */
bnode_t *createIPv6TreeFromFile(const char *);

/**
 * @brief Checks if an IPv6 address occurs in an IPv6 tree.
 *
 * @return uint8_t  1 if found, 0 if not.
 */
uint8_t findIPv6(bnode_t *, const char *);

#endif
