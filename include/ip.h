#ifndef IP_H_
#define IP_H_

#include <stdint.h>

enum ip_t {
    IPv4, IPv6
};

typedef struct {
    uint32_t ip;
    uint8_t mask;
} ipv4_t;

typedef struct {
    uint64_t group[8];
    uint8_t mask;
} ipv6_t;

ipv4_t read_ipv4(const char*);

// ipv6_t read_ipv6(char*);

#endif
