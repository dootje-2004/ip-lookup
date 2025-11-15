#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "ip.h"

/**
 * @brief Wrapper function for read_ipv4 that allows ignoring the prefix.
 * Used when reading the IPv4 part of a mapped IPv4-to-IPv6 address.
 * 
 * @param s             String representing an IPv4 address.
 * @param skip_prefix   0: check prefix as IPv4; > 0: skip reading and set prefix to 32.
 * @return ipv4_t       IPv4 struct.
 */
ipv4_t read_ipv4_internal(const char *s, uint8_t skip_prefix)
{
    ipv4_t ipv4;    // IPv4 address
    char c;         // character being evaluated
    uint8_t p = 0;  // position in s string
    int16_t d = -1; // decimal group value; -1 means uninitialized
    uint8_t g = 0;  // group index
    uint8_t b = 0;  // position in group substring
    
    ipv4.ps = 32;
    
    while (1)
    {
        c = s[p++];
        switch (c)
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if ((d == 0) && (b > 0))
                {
                    fprintf(stderr, "IPv4 string %s, group %d: leading zeroes are not allowed\n", s, g);
                    goto error_handler;
                }
                if (d == -1)
                {
                    d = 0;
                }
                d = d * 10 + (c - '0');
                if (d > 255) {
                    fprintf(stderr, "IPv4 string %s: group %d has a value greater than 255: %d\n", s, g, d);
                    goto error_handler;
                }
                b++;
                break;
            case '.':
                if (d == -1)
                {
                    fprintf(stderr, "IPv4 string %s: group %d is empty\n", s, g);
                    goto error_handler;
                }
                ipv4.ip = (ipv4.ip << 8) + d;
                d = -1;
                g++;
                b = 0;
                break;
            case '\0':
                if (d == -1)
                {
                    fprintf(stderr, "IPv4 string %s: group %d is empty\n", s, g);
                    goto error_handler;
                }
                ipv4.ip = (ipv4.ip << 8) + d;
                goto final_check;
            case '/':
                if (d == -1)
                {
                    fprintf(stderr, "IPv4 string %s: group %d is empty\n", s, g);
                    goto error_handler;
                }
                ipv4.ip = (ipv4.ip << 8) + d;
                if (skip_prefix != 0)
                {
                    goto final_check;
                }
                ipv4.ps = 0;
                while ((c = s[p++]) != '\0')
                {
                    if (c >= '0' && c <= '9')
                    {
                        ipv4.ps = 10 * ipv4.ps + (c - '0');
                    }
                    else
                    {
                        fprintf(stderr, "IPv4 %s: invalid character '%c' (%d) in prefix size\n", s, c, c);
                        goto error_handler;
                    }
                }
                goto final_check;
            default:
                fprintf(stderr, "IPv4 string %s has an invalid character '%c' (%d) at position %d\n", s, c, c, p-1);
                goto error_handler;
        }
    }

final_check:
    if (g != 3) {
        fprintf(stderr, "IPv4 string %s has %d groups instead of 4\n", s, g);
        goto error_handler;
    }
    if (ipv4.ps == 0)
    {
        fprintf(stderr, "IPv4 string %s has missing or zero prefix size\n", s);
        goto error_handler;
    }
    if (ipv4.ps > 32)
    {
        fprintf(stderr, "IPv4 string %s has a prefix size of more than 32 bits: %d\n", s, ipv4.ps);
        goto error_handler;
    }
    return ipv4;

error_handler:
    ipv4.ip = 0;
    ipv4.ps = 0;
    return ipv4;
}

ipv4_t read_ipv4(const char *s)
{
    return read_ipv4_internal(s, 0);
}

ipv6_t read_ipv6(const char *s)
{
    ipv6_t ipv6;    // IPv6 address
    char c;         // character being evaluated
    uint8_t p = 0;  // position in ip string
    uint8_t g = 0;  // group index
    uint8_t b = 0;  // number of group characters read
    int8_t z = -1;  // group index of "::" substring
    ipv4_t ip4;     // Helper variable for mapped IPv4

    for (g = 0; g < 8; g++)
    {
        ipv6.ip[g] = 0;
    }
    ipv6.ps = 128;
    g = 0;

    while (1)
    {
        c = tolower(s[p++]);
        switch (c)
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                ipv6.ip[g] = (ipv6.ip[g] << 4) + c - '0';
                b++;
                break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                ipv6.ip[g] = (ipv6.ip[g] << 4) + c - 'a' + 10;
                b++;
                break;
            case ':':
                g++;
                if (s[p] == ':')
                {
                    if (z == -1)
                    {
                        p++;
                        z = g;
                    }
                    else
                    {
                        fprintf(stderr, "IPv6 string %s has multiple double colons\n", s);
                        goto error_handler;
                    }
                }
                else
                {
                    if (p == 1)
                    {
                        fprintf(stderr, "IPv6 string %s must not start with a single colon\n", s);
                        goto error_handler;
                    }
                }
                b = 0;
                break;
            case '/':
                if ((b == 0) && (z != g))
                {
                    fprintf(stderr, "IPv6 string %s ends with empty group\n", s);
                    goto error_handler;
                }
                ipv6.ps = 0;
                while ((c = s[p++]) != '\0')
                {
                    if (c >= '0' && c <= '9')
                    {
                        ipv6.ps = 10 * ipv6.ps + (c - '0');
                    }
                    else
                    {
                        fprintf(stderr, "IPv6 %s: invalid character '%c' (%d) in prefix size\n", s, c, c);
                        goto error_handler;
                    }
                }
                goto final_check;
            case '.':
                ip4 = read_ipv4_internal((char*)(s+p-b-1), 1);
                if (ip4.ps == 0)
                {
                    goto error_handler;
                }
                ipv6.ip[g++] = ip4.ip >> 16;
                ipv6.ip[g] = ip4.ip & 0xFFFF;
                while (((c = s[p++]) != '/') && (c != '\0'));
                p--;
                break;
            case '\0':
                if ((b == 0) && (z != g))
                {
                    fprintf(stderr, "IPv6 string %s ends with empty group\n", s);
                    goto error_handler;
                }
                goto final_check;
            default:
                fprintf(stderr, "IPv6 string %s has an invalid character '%c' (%d) at position %d\n", s, c, c, p-1);
                goto error_handler;
        }
    }

final_check:
    if (z > -1)
    {
        for (uint8_t i = 7; i >= z+7-g; i--)
        {
            ipv6.ip[i] = ipv6.ip[i-(7-g)];
            ipv6.ip[i-(7-g)] = 0;
        }
        g = 7;
    }
    if (g != 7)
    {
        fprintf(stderr, "IPv6 string %s must have 8 groups, found %d\n", s, g+1);
        goto error_handler;
    }
    if (ipv6.ps == 0)
    {
        fprintf(stderr, "IPv6 string %s has missing or zero prefix size\n", s);
        goto error_handler;
    }
    if (ipv6.ps > 128)
    {
        fprintf(stderr, "IPv6 string %s has a prefix size of more than 128 bits: %d\n", s, ipv6.ps);
        goto error_handler;
    }
    return ipv6;

error_handler:
    for (g = 0; g < 8; g++)
    {
        ipv6.ip[g] = 0;
    }
    ipv6.ps = 0;
    return ipv6;
}
