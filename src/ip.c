#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>
#include "ip.h"
#include "ip_unittest.h"

ipv4_t read_ipv4(const char *ipv4_string)
{
    char string_buffer[INET_ADDRSTRLEN] = { 0 };
    uint32_t inet_address;
    uint8_t p = 0;
    ipv4_t ip;

    while ((p < INET_ADDRSTRLEN) && (ipv4_string[p] != '/') && (ipv4_string[p] != '\0'))
    {
        p++;
    }
    
    strncpy(string_buffer, ipv4_string, p);
    
    if (inet_pton(AF_INET, string_buffer, &inet_address) != 1)
    {
        return empty_ipv4();
    }

    ip.ip = reverseBytesIPv4(inet_address);

    if (ipv4_string[p] == '/')
    {
        p++;
        ip.ps = read_prefix_size(ipv4_string, &p, 32);
        if (ip.ps == 0)
        {
            return empty_ipv4();
        }
    }
    else
    {
        ip.ps = 32;
    }
    return ip;
}

ipv6_t read_ipv6(const char *ipv6_string)
{
    char string_buffer[INET6_ADDRSTRLEN] = { 0 };
    uint16_t ip_buffer[8];
    uint8_t p = 0;
    ipv6_t ip;

    while ((p < INET6_ADDRSTRLEN) && (ipv6_string[p] != '/') && (ipv6_string[p] != '\0'))
    {
        p++;
    }

    strncpy(string_buffer, ipv6_string, p);
    
    if (inet_pton(AF_INET6, string_buffer, &ip_buffer) != 1)
    {
        return empty_ipv6();
    }

    reverseBytesIPv6(ip_buffer, ip.ip);

    if (ipv6_string[p] == '/')
    {
        p++;
        ip.ps = read_prefix_size(ipv6_string, &p, 128);
        if (ip.ps == 0)
        {
            return empty_ipv6();
        }
    }
    else
    {
        ip.ps = 128;
    }
    return ip;
}

void ipv4tostring(char *string_buffer, ipv4_t ip)
{
    uint32_t ip_reversed = reverseBytesIPv4(ip.ip);

    inet_ntop(AF_INET, &ip_reversed, string_buffer, INET_ADDRSTRLEN);
    append_prefix_size(string_buffer, ip.ps, 32);
}

void ipv6tostring(char *string_buffer, ipv6_t ip)
{
    uint16_t ip_reversed[8];

    reverseBytesIPv6(ip.ip, ip_reversed);
    inet_ntop(AF_INET6, ip_reversed, string_buffer, INET6_ADDRSTRLEN);
    append_prefix_size(string_buffer, ip.ps, 128);
}

uint8_t read_prefix_size(const char* ip_string, uint8_t *string_index, uint8_t max_value)
{
    /*
    We don't use the stdlib functions atoi() or strtoul(), because the former 
    does not raise any errors, and the latter is too forgiving with whitespace.
    Neither of these functions will error out on non-numeric characters, 
    which makes them unsuitable.
    */
    char c;
    uint8_t prefix_size = 0;

    while ((c = ip_string[(*string_index)++]) != '\0')
    {
        if (c >= '0' && c <= '9')
        {
            prefix_size *= 10;
            prefix_size += (c - '0');
        }
        else
        {
            prefix_size = 0;
            fprintf(stderr, "IP string %s: invalid character '%c' (%d) in prefix size\n", ip_string, c, c);
        }
    }
    if (prefix_size > max_value)
    {
        prefix_size = 0;
    }
    return prefix_size;
}

uint32_t reverseBytesIPv4(uint32_t i) {
    return (i >> 24) | ((i >> 8) & 0x0000FF00) | ((i << 8) & 0x00FF0000) | (i << 24);
}

ipv4_t empty_ipv4()
{
    ipv4_t ip;
    ip.ip = 0;
    ip.ps = 0;
    return ip;
}

ipv6_t empty_ipv6()
{
    ipv6_t ip;
    for (uint8_t i = 0; i < 8; i++)
    {
        ip.ip[i] = 0;
    }
    ip.ps = 0;
    return ip;
}

uint16_t switchHiLoBytes(uint16_t i)
{
    return (i >> 8) | (uint16_t)(i << 8);
}

void reverseBytesIPv6(const uint16_t *src, uint16_t *dest)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        dest[i] = switchHiLoBytes(src[i]);
    }
}

void append_prefix_size(char *string_buffer, uint8_t prefix_size, uint8_t default_ps)
{
    int string_index = 0;

    while (string_buffer[string_index] != '\0')
    {
        string_index++;
    }
    if (prefix_size < default_ps)
    {
        string_index += sprintf(string_buffer + string_index, "/%d", prefix_size);
    }
    string_buffer[string_index] = '\0';
}
