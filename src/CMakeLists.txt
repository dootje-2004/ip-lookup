add_library(iplib ip.c)
add_library(btreelib btree.c)

enable_coverage(iplib btreelib)
