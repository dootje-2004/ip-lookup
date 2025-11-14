#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "ip.h"

ipv4_t read_ipv4(const char *ip)
{
    ipv4_t ipv4;    // IPv4 address
    char c;         // ip character being evaluated
    uint8_t p = 0;  // position in ip string
    int16_t d = -1; // decimal group value
    uint8_t g = 1;  // group index
    uint8_t b = 0;  // position in group substring
    
    ipv4.ps = 32;
    
    while (1)
    {
        c = ip[p++];
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
                    fprintf(stderr, "IPv4 string %s, block %d: leading zeroes are not allowed\n", ip, g);
                    goto error_handler;
                }
                if (d == -1)
                {
                    d = 0;
                }
                d = d * 10 + (c - '0');
                if (d > 255) {
                    fprintf(stderr, "IPv4 string %s: block %d has a value greater than 255: %d\n", ip, g, d);
                    goto error_handler;
                }
                b++;
                break;
            case '.':
                if (d == -1)
                {
                    fprintf(stderr, "IPv4 string %s: block %d is empty\n", ip, g);
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
                    fprintf(stderr, "IPv4 string %s: block %d is empty\n", ip, g);
                    goto error_handler;
                }
                ipv4.ip = (ipv4.ip << 8) + d;
                goto final_check;
            case '/':
                if (d == -1)
                {
                    fprintf(stderr, "IPv4 string %s: block %d is empty\n", ip, g);
                    goto error_handler;
                }
                ipv4.ip = (ipv4.ip << 8) + d;
                d = 0;
                while ((c = ip[p++]) != '\0')
                {
                    if (c >= '0' && c <= '9')
                    {
                        d = 10 * d + (c - '0');
                    }
                    else
                    {
                        fprintf(stderr, "IPv4 %s: invalid character '%c' (%d) in ps\n", ip, c, c);
                        goto error_handler;
                    }
                }
                if (d == 0)
                {
                    fprintf(stderr, "IPv4 string %s has a zero-bit ps\n", ip);
                    goto error_handler;
                }
                if (d > 32)
                {
                    fprintf(stderr, "IPv4 string %s has a ps of more than 32 bits: %d\n", ip, d);
                    goto error_handler;
                }
                ipv4.ps = d;
                goto final_check;
            default:
                fprintf(stderr, "IPv4 string %s has an invalid character '%c' (%d) at position %d\n", ip, c, c, p-1);
                goto error_handler;
        }
    }

final_check:
    if (g != 4) {
        fprintf(stderr, "IPv4 string %s has %d blocks instead of 4\n", ip, g);
        goto error_handler;
    }
    return ipv4;

error_handler:
    ipv4.ip = 0;
    ipv4.ps = 0;
    return ipv4;
}

ipv6_t read_ipv6(const char *ip)
{
    ipv6_t ipv6;    // IPv6 address
    char c;         // ip character being evaluated
    uint8_t p = 0;  // position in ip string
    uint8_t g = 0;  // group index
    uint8_t b = 0;  // position in group substring
    
    for (g = 0; g < 8; g++)
    {
        ipv6.ip[g] = 0;
    }
    ipv6.ps = 128;

    g = 0;
    while (1)
    {
        c = ip[p++];
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
                b = 0;
                g++;
                break;
            case '/':
            case '.':
            case '\0':
                goto final_check;
            default:
                fprintf(stderr, "IPv6 string %s has an invalid character '%c' (%d) at position %d\n", ip, c, c, p-1);
                goto error_handler;
        }
    }

final_check:
    // TODO: Check number of groups, expand ::
    return ipv6;

error_handler:
    for (g = 0; g < 8; g++)
    {
        ipv6.ip[g] = 0;
    }
    ipv6.ps = 0;
    return ipv6;
}
