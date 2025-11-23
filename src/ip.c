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
 * @param ipv4_string   String representing an IPv4 address.
 * @param skip_prefix   0: check prefix as IPv4; > 0: skip reading and set prefix to 32.
 * @return ipv4_t       IPv4 struct.
 */
ipv4_t read_ipv4_internal(const char *ipv4_string, uint8_t skip_prefix)
{
    ipv4_t ipv4;
    char c;
    uint8_t string_index = 0;
    int16_t group_value = -1;
    uint8_t group_index = 0;
    uint8_t group_string_index = 0;

    ipv4.ps = 32;

    while (1)
    {
        c = ipv4_string[string_index++];
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
            if ((group_value == 0) && (group_string_index > 0))
            {
                fprintf(stderr, "IPv4 string %s, group %d: leading zeroes are not allowed\n", ipv4_string, group_index);
                goto error_handler;
            }
            if (group_value == -1)
            {
                group_value = 0;
            }
            group_value = group_value * 10 + (c - '0');
            if (group_value > 255)
            {
                fprintf(stderr, "IPv4 string %s: group %d has a value greater than 255: %d\n", ipv4_string, group_index, group_value);
                goto error_handler;
            }
            group_string_index++;
            break;
        case '.':
            if (group_value == -1)
            {
                fprintf(stderr, "IPv4 string %s: group %d is empty\n", ipv4_string, group_index);
                goto error_handler;
            }
            ipv4.ip = (ipv4.ip << 8) + group_value;
            group_value = -1;
            group_index++;
            group_string_index = 0;
            break;
        case '\0':
            if (group_value == -1)
            {
                fprintf(stderr, "IPv4 string %s: group %d is empty\n", ipv4_string, group_index);
                goto error_handler;
            }
            ipv4.ip = (ipv4.ip << 8) + group_value;
            goto final_check;
        case '/':
            if (group_value == -1)
            {
                fprintf(stderr, "IPv4 string %s: group %d is empty\n", ipv4_string, group_index);
                goto error_handler;
            }
            ipv4.ip = (ipv4.ip << 8) + group_value;
            if (skip_prefix != 0)
            {
                goto final_check;
            }
            ipv4.ps = 0;
            while ((c = ipv4_string[string_index++]) != '\0')
            {
                if (c >= '0' && c <= '9')
                {
                    ipv4.ps = 10 * ipv4.ps + (c - '0');
                }
                else
                {
                    fprintf(stderr, "IPv4 %s: invalid character '%c' (%d) in prefix size\n", ipv4_string, c, c);
                    goto error_handler;
                }
            }
            goto final_check;
        default:
            fprintf(stderr, "IPv4 string %s has an invalid character '%c' (%d) at position %d\n", ipv4_string, c, c, string_index - 1);
            goto error_handler;
        }
    }

final_check:
    if (group_index != 3)
    {
        fprintf(stderr, "IPv4 string %s has %d groups instead of 4\n", ipv4_string, group_index);
        goto error_handler;
    }
    if (ipv4.ps == 0)
    {
        fprintf(stderr, "IPv4 string %s has missing or zero prefix size\n", ipv4_string);
        goto error_handler;
    }
    if (ipv4.ps > 32)
    {
        fprintf(stderr, "IPv4 string %s has a prefix size of more than 32 bits: %d\n", ipv4_string, ipv4.ps);
        goto error_handler;
    }
    return ipv4;

error_handler:
    ipv4.ip = 0;
    ipv4.ps = 0;
    return ipv4;
}

ipv4_t read_ipv4(const char *ipv4_string)
{
    return read_ipv4_internal(ipv4_string, 0);
}

ipv6_t read_ipv6(const char *ipv6_string)
{
    ipv6_t ipv6;
    char c;
    uint8_t string_index = 0;
    uint8_t group_index = 0;
    uint8_t group_string_index = 0;
    int8_t double_colon_marker = -1;
    ipv4_t ip4;

    for (group_index = 0; group_index < 8; group_index++)
    {
        ipv6.ip[group_index] = 0;
    }
    ipv6.ps = 128;
    group_index = 0;

    while (1)
    {
        c = tolower(ipv6_string[string_index++]);
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
            ipv6.ip[group_index] = (ipv6.ip[group_index] << 4) + c - '0';
            group_string_index++;
            break;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            ipv6.ip[group_index] = (ipv6.ip[group_index] << 4) + c - 'a' + 10;
            group_string_index++;
            break;
        case ':':
            group_index++;
            if (ipv6_string[string_index] == ':')
            {
                if (double_colon_marker == -1)
                {
                    string_index++;
                    double_colon_marker = group_index;
                }
                else
                {
                    fprintf(stderr, "IPv6 string %s has multiple double colons\n", ipv6_string);
                    goto error_handler;
                }
            }
            else
            {
                if (group_string_index == 0)
                {
                    fprintf(stderr, "IPv6 string %s has empty group at position %d\n", ipv6_string, string_index - 1);
                    goto error_handler;
                }
            }
            group_string_index = 0;
            break;
        case '/':
            if ((group_string_index == 0) && (double_colon_marker != group_index))
            {
                fprintf(stderr, "IPv6 string %s ends with empty group\n", ipv6_string);
                goto error_handler;
            }
            ipv6.ps = 0;
            while ((c = ipv6_string[string_index++]) != '\0')
            {
                if (c >= '0' && c <= '9')
                {
                    ipv6.ps = 10 * ipv6.ps + (c - '0');
                }
                else
                {
                    fprintf(stderr, "IPv6 %s: invalid character '%c' (%d) in prefix size\n", ipv6_string, c, c);
                    goto error_handler;
                }
            }
            goto final_check;
        case '.':
            ip4 = read_ipv4_internal((char *)(ipv6_string + string_index - group_string_index - 1), 1);
            if (ip4.ps == 0)
            {
                goto error_handler;
            }
            ipv6.ip[group_index++] = ip4.ip >> 16;
            ipv6.ip[group_index] = ip4.ip & 0xFFFF;
            while (((c = ipv6_string[string_index++]) != '/') && (c != '\0'))
                ;
            string_index--;
            break;
        case '\0':
            if ((group_string_index == 0) && (double_colon_marker != group_index))
            {
                fprintf(stderr, "IPv6 string %s ends with empty group\n", ipv6_string);
                goto error_handler;
            }
            goto final_check;
        default:
            fprintf(stderr, "IPv6 string %s has an invalid character '%c' (%d) at position %d\n", ipv6_string, c, c, string_index - 1);
            goto error_handler;
        }
    }

final_check:
    if (double_colon_marker > -1)
    {
        for (uint8_t i = 7; i >= double_colon_marker + 7 - group_index; i--)
        {
            ipv6.ip[i] = ipv6.ip[i - (7 - group_index)];
            ipv6.ip[i - (7 - group_index)] = 0;
        }
        group_index = 7;
    }
    if (group_index != 7)
    {
        fprintf(stderr, "IPv6 string %s must have 8 groups, found %d\n", ipv6_string, group_index + 1);
        goto error_handler;
    }
    if (ipv6.ps == 0)
    {
        fprintf(stderr, "IPv6 string %s has missing or zero prefix size\n", ipv6_string);
        goto error_handler;
    }
    if (ipv6.ps > 128)
    {
        fprintf(stderr, "IPv6 string %s has a prefix size of more than 128 bits: %d\n", ipv6_string, ipv6.ps);
        goto error_handler;
    }
    return ipv6;

error_handler:
    for (group_index = 0; group_index < 8; group_index++)
    {
        ipv6.ip[group_index] = 0;
    }
    ipv6.ps = 0;
    return ipv6;
}

void ip4string(char *string_buffer, ipv4_t ip)
{
    uint8_t group_index;
    uint8_t string_index = 0;

    for (group_index = 0; group_index < 3; group_index++)
    {
        string_index += sprintf(string_buffer + string_index, "%d.", (ip.ip >> (3 - group_index) * 8) & 0xFF);
    }
    string_index += sprintf(string_buffer + string_index, "%d", ip.ip & 0xFF);
    if (ip.ps < 32)
    {
        string_index += sprintf(string_buffer + string_index, "/%d", ip.ps);
    }
    string_buffer[string_index++] = '\0';
}
