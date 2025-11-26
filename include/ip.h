/**
 * @file ip.h
 * @author Aldo Verlinde (aldo.verlinde@gmail.com)
 * @brief IP library public header file.
 * @version 0.1
 * @date 2025-11-14
 */
#ifndef IP_H_
#define IP_H_

#include <stdint.h>

/**
 * @brief arpa/inet.h defines INET_ADDRSTRLEN and INET6_ADDRSTRLEN,
 * but those do not account for IP range notation.
 */
#define IPSTRLENV4 20
#define IPSTRLENV6 50

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
typedef struct
{
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
 * As with ipv4_t, as prefix size of 0 signals an invalid string.
 */
typedef struct
{
    uint16_t ip[8];
    uint8_t ps;
} ipv6_t;

/**
 * @brief Converts a string to ipv4_t.
 *
 * @param ipv4_string  Null-terminated input string.
 * @return ipv4_t
 *
 * Whitespace is not tolerated.
 * Use .ps == 0 to check for invalid input.
 */
ipv4_t read_ipv4(const char *ipv4_string);

/**
 * @brief Converts a null-terminated string to ipv6_t.
 *
 * @param ipv6_string  Null-terminated input string.
 * @return ipv6_t
 *
 * This function converts strings that are logically correct.
 * They may still be syntactically incorrect, though. E.g.,
 * having leading zeroes in one group but not in another is
 * perfectly fine. The same goes for zero-valued groups adjacent
 * to '::'. Mixing uppercase and lowercase hexadecimals is also
 * allowed.
 * Whitespace is not tolerated.
 * Use .ps == 0 to check for invalid input.
 */
ipv6_t read_ipv6(const char *ipv6_string);

/**
 * @brief Converts an ipv4_t type to a human-readable string.
 * 
 * @param string_buffer  Holds the resulting null-terminated string.
 * @param ip             Input.
 */
void ipv4tostring(char *string_buffer, ipv4_t ip);

/**
 * @brief Converts an ipv6_t type to a human-readable string.
 * 
 * @param string_buffer  Holds the resulting null-terminated string.
 * @param ip             Input.
 */
void ipv6tostring(char *string_buffer, ipv6_t ip);

#endif
