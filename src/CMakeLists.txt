add_library(iplib ip.c)
add_library(btreelib btree.c)

enable_coverage(iplib btreelib)

add_executable(server server.c)
add_executable(client client.c)
