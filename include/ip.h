/**
 * @file ip.h
 * @author Aldo Verlinde (aldo.verlinde@gmail.com)
 * @brief IP library header file.
 * @version 0.1
 * @date 2025-11-14
 */
#ifndef IP_H_
#define IP_H_

#include <stdint.h>

/**
 * @brief IPv4 address type.
 * 
 * .ip is the numerical 32-bit address value.
 * .ps is the prefix size, 32 for a fixed IP or
 * less than 32 for a network range.
 * 
 * A prefix size of 0 is used to denote an invalid IPv4
 * string: x.y.z.t/0 is equivalent to the entire IPv4
 * address space and has no practical purpose.
 */
typedef struct {
    uint32_t ip;
    uint8_t ps;
} ipv4_t;

/**
 * @brief IPv6 address type.
 * 
 * .ip is an array of 8 16-bit numbers that together
 * form the 128-bit IP address.
 * .ps is the prefix size, 128 for a fixed IP or
 * less than that for a network range.
 * 
 * As with ipv4_t, as prefix size of 0 denotes an invalid IPv6 string.
 */
typedef struct {
    uint16_t ip[8];
    uint8_t ps;
} ipv6_t;

/**
 * @brief Converts a null-terminated string to ipv4_t.
 * Whitespace is not tolerated.
 * 
 * @return ipv4_t
 * Use .ps == 0 to check for invalid input.
 */
ipv4_t read_ipv4(const char*);

/**
 * @brief Converts a null-terminated string to ipv6_t.
 * Whitespace is not tolerated.
 * 
 * @return ipv6_t
 * Use .ps == 0 to check for invalid input.
 */
ipv6_t read_ipv6(const char*);

#endif
