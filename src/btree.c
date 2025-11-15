#include <stddef.h>
#include "btree.h"
#include "ip.h"

int addIPv4(bnode_t *tree, ipv4_t ip)
{
    tree->node0 = NULL;
    tree->node1 = NULL;
    ip.ps <<=1;
    return 0;
}

int addIPv6(bnode_t *tree,  ipv6_t ip)
{
    tree->node0 = NULL;
    tree->node1 = NULL;
    ip.ps <<=1;
    return 0;
}
