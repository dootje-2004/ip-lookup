/**
 * @file ip_unittest.h
 * @author Aldo Verlinde (aldo.verlinde@gmail.com)
 * @brief Header file for internal helper functions in ip.c; allows white-box testing.
 * @version 0.1
 * @date 2025-11-25
 */
#ifndef IP_UNITTEST_H_
#define IP_UNITTEST_H_

#include <stdint.h>

/**
 * @brief Returns an empty IPv4 struct.
 * 
 * @return ipv4_t  Initialized to all zeroes.
 */
ipv4_t empty_ipv4();

/**
 * @brief Returns an empty IPv6 struct.
 * 
 * @return ipv6_t  Initialized to all zeroes.
 */
ipv6_t empty_ipv6();

/**
 * @brief Swaps the two bytes in a 16-bit integer.
 * 
 * @param i          Input.
 * @return uint16_t  Integer with bytes swapped.
 */
uint16_t switchHiLoBytes(uint16_t i);

/**
 * @brief Converts an AF_INET address to an integer with
 * ordered bits (most significant first) and vice versa.
 * 
 * @param i          Input.
 * @return uint32_t  Integer with bytes in reverse order.
 */
uint32_t reverseBytesIPv4(uint32_t i);

/**
 * @brief Converts an AF_INET6 address to an integer array
 * with ordered bits (most significant first) and vice versa.
 * 
 * @param src   Input array of 8 unsigned 16-bit integers.
 * @param dest  Output array of 8 unsigned 16-bit integers.
 */
void reverseBytesIPv6(const uint16_t *src, uint16_t *dest);

/**
 * @brief Returns the prefix size at the tail of an IPv4 or IPv6 string.
 * 
 * @param ip_string     IP string; can be both IPv4 or IPv6.
 * @param string_index  Position in ip_string to start reading.
 * @param max_value     Maximum allowed return value; 32 for IPv4, 128 for IPv6.
 * @return uint8_t      Prefix size.
 */
uint8_t read_prefix_size(const char *ip_string, uint8_t *string_index, uint8_t max_value);

/**
 * @brief Appends the prefix size to an IP string.
 * 
 * @param string_buffer  Null-terminated IP string.
 * @param prefix_size    The value to append.
 * @param default_ps     If prefix_size equals the default, nothing is appended.
 * 
 * The string buffer needs to be large enough to hold the result. It is recommended
 * to create a buffer of size IPSTRLENV4 for IPv4, or IPSTRLENV6 for IPv6.
 * DO NOT USE the values INET_ADDRSTRLEN or INET6_ADDRSTRLEN, since these don't
 * take the range prefix into account.
 */
void append_prefix_size(char *string_buffer, uint8_t prefix_size, uint8_t default_ps);

#endif
