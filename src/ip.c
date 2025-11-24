#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "ip.h"

/**
 * @brief Enum to keep track of the status while processing an IPV4 string.
 */
typedef enum ip_string_status
{
    IP_STRING_READING = 0x0,
    IP_STRING_DONE = 0x1,
    IP_STRING_ERROR = 0x2
} ip_string_status;

/**
 * @brief Checks an IPv4 group for leading zeroes. Prints a message to stderr if there are.
 * 
 * @param ipv4_string        IPv4 string.
 * @param group_index        Group index in the IPv4 string.
 * @param group_value        Numerical value of group[group_index].
 * @param group_string_index String position in group[group_index].
 * @return ip_string_status  IP_STRING_READING if ok, IP_STRING_ERROR if there are leading zeroes.
 */
ip_string_status check_leading_zeroes(const char* ipv4_string, uint8_t group_index, int16_t group_value, uint8_t group_string_index)
{
    ip_string_status result = IP_STRING_READING;

    if ((group_value == 0) && (group_string_index > 0))
    {
        fprintf(stderr, "IPv4 string %s, group %d: leading zeroes are not allowed\n", ipv4_string, group_index);
        result = IP_STRING_ERROR;
    }
    return result;
}

/**
 * @brief Checks if an IPv4 group value is in the range 0-255. Prints a message to stderr if it is not.
 * 
 * @param ipv4_string        IPv4 string.
 * @param group_index        Group index in the IPv4 string.
 * @param group_value        Numerical value of group[group_index].
 * @return ip_string_status  IP_STRING_READING if ok, IP_STRING_ERROR if not.
 */
ip_string_status check_group_value(const char* ipv4_string, uint8_t group_index, int16_t group_value)
{
    ip_string_status result = IP_STRING_READING;

    if (group_value == -1)
    {
        fprintf(stderr, "IPv4 string %s: group %d is empty\n", ipv4_string, group_index);
        result = IP_STRING_ERROR;
    }
    else if (group_value > 255)
    {
        fprintf(stderr, "IPv4 string %s: group %d has a value greater than 255: %d\n", ipv4_string, group_index, group_value);
        result = IP_STRING_ERROR;
    }
    return result;
}

/**
 * @brief Checks if an IPv4 string has indeed four groups. Prints a message to stderr is it has not.
 * 
 * @param ipv4_string        IPv4 string.
 * @param group_index        Group index in the IPv4 string.
 * @return ip_string_status  IP_STRING_READING if ok, IP_STRING_ERROR if not.
 */
ip_string_status check_group_count(const char* ipv4_string, uint8_t group_index)
{
    ip_string_status result = IP_STRING_DONE;

    if (group_index != 3)
    {
        fprintf(stderr, "IPv4 string %s has %d groups instead of 4\n", ipv4_string, group_index);
        result = IP_STRING_ERROR;
    }
    return result;
}

/**
 * @brief Checks if an IPv4 prefix size is in the range 1-32. Prints a message to stderr if it is not.
 * 
 * @param ipv4_string        IPv4 string.
 * @param ipv4               ipv4_t variable.
 * @return ip_string_status  IP_STRING_DONE if ok, IP_STRING_ERROR if not.
 */
ip_string_status check_prefix_size(const char* ipv4_string, ipv4_t ipv4)
{
    ip_string_status result = IP_STRING_DONE;

    if (ipv4.ps == 0)
    {
        fprintf(stderr, "IPv4 string %s has missing or zero prefix size\n", ipv4_string);
        result = IP_STRING_ERROR;
    }
    if (ipv4.ps > 32)
    {
        fprintf(stderr, "IPv4 string %s has a prefix size of more than 32 bits: %d\n", ipv4_string, ipv4.ps);
        result = IP_STRING_ERROR;
    }
    return result;
}

/**
 * @brief Reads the prefix size at the tail of an IPv4 string.
 * 
 * @param ipv4               IPv4 variable to update (called by reference).
 * @param ipv4_string        IPv4 string.
 * @param string_index       Current position in ipv4_string.
 * @return ip_string_status  IP_STRING_DONE if reading succeeds, IP_STRING_ERROR if it doesn't.
 */
ip_string_status read_prefix(ipv4_t* ipv4, const char* ipv4_string, uint8_t string_index)
{
    char c;
    ip_string_status result = IP_STRING_DONE;

    ipv4->ps = 0;
    while ((c = ipv4_string[string_index++]) != '\0')
    {
        if (c >= '0' && c <= '9')
        {
            ipv4->ps = 10 * ipv4->ps;
            ipv4->ps += (c - '0');
        }
        else
        {
            fprintf(stderr, "IPv4 %s: invalid character '%c' (%d) in prefix size\n", ipv4_string, c, c);
            result |= IP_STRING_ERROR;
        }
    }
    return result;
}

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
    ip_string_status ip_read_status = IP_STRING_READING;

    ipv4.ip = 0;
    ipv4.ps = 32;

    while (ip_read_status == IP_STRING_READING)
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
                ip_read_status |= check_leading_zeroes(ipv4_string, group_index, group_value, group_string_index);
                if (ip_read_status & IP_STRING_ERROR)
                {
                    break;
                }
                if (group_value == -1)
                {
                    group_value = 0;
                }
                group_value = group_value * 10 + (c - '0');
                ip_read_status |= check_group_value(ipv4_string, group_index, group_value);
                if (ip_read_status & IP_STRING_ERROR)
                {
                    break;
                }
                group_string_index++;
                break;
            case '.':
                ip_read_status |= check_group_value(ipv4_string, group_index, group_value);
                if (ip_read_status & IP_STRING_ERROR)
                {
                    break;
                }
                ipv4.ip = (ipv4.ip << 8) + group_value;
                group_value = -1;
                group_index++;
                group_string_index = 0;
                break;
            case '\0':
                ip_read_status |= check_group_value(ipv4_string, group_index, group_value);
                if (ip_read_status & IP_STRING_ERROR)
                {
                    break;
                }
                ipv4.ip = (ipv4.ip << 8) + group_value;
                ip_read_status = IP_STRING_DONE;
                break;
            case '/':
                ip_read_status |= check_group_value(ipv4_string, group_index, group_value);
                if (ip_read_status & IP_STRING_ERROR)
                {
                    break;
                }
                ipv4.ip = (ipv4.ip << 8) + group_value;
                if (skip_prefix != 0)
                {
                    ip_read_status = IP_STRING_DONE;
                    break;
                }
                ip_read_status |= read_prefix(&ipv4, ipv4_string, string_index);
                break;
            default:
                fprintf(stderr, "IPv4 string %s has an invalid character '%c' (%d) at position %d\n", ipv4_string, c, c, string_index - 1);
                ip_read_status |= IP_STRING_ERROR;
                break;
        }
    }

    ip_read_status |= check_group_count(ipv4_string, group_index);
    ip_read_status |= check_prefix_size(ipv4_string, ipv4);
    if (ip_read_status & IP_STRING_ERROR)
    {
        ipv4.ip = 0;
        ipv4.ps = 0;
    }
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
    int8_t double_colon_marker_position = -1;
    ipv4_t ip4;
    ip_string_status ip_read_status = IP_STRING_READING;

    for (group_index = 0; group_index < 8; group_index++)
    {
        ipv6.ip[group_index] = 0;
    }
    ipv6.ps = 128;
    group_index = 0;

    while (ip_read_status == IP_STRING_READING)
    {
        c = ipv6_string[string_index++];
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
                ipv6.ip[group_index] = ipv6.ip[group_index] << 4;
                ipv6.ip[group_index] += c - 'a' + 10;
                group_string_index++;
                break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
                ipv6.ip[group_index] = ipv6.ip[group_index] << 4;
                ipv6.ip[group_index] += c - 'A' + 10;
                group_string_index++;
                break;
            case ':':
                group_index++;
                if (ipv6_string[string_index] == ':')
                {
                    if (double_colon_marker_position == -1)
                    {
                        string_index++;
                        double_colon_marker_position = group_index;
                    }
                    else
                    {
                        fprintf(stderr, "IPv6 string %s has multiple double colons\n", ipv6_string);
                        ip_read_status |= IP_STRING_ERROR;
                        break;
                    }
                }
                else
                {
                    if (group_string_index == 0)
                    {
                        fprintf(stderr, "IPv6 string %s has empty group at position %d\n", ipv6_string, string_index - 1);
                        ip_read_status |= IP_STRING_ERROR;
                        break;
                    }
                }
                group_string_index = 0;
                break;
            case '/':
                if ((group_string_index == 0) && (double_colon_marker_position != group_index))
                {
                    fprintf(stderr, "IPv6 string %s ends with empty group\n", ipv6_string);
                    ip_read_status |= IP_STRING_ERROR;
                    break;
                }
                ipv6.ps = 0;
                while ((c = ipv6_string[string_index++]) != '\0')
                {
                    if (c >= '0' && c <= '9')
                    {
                        ipv6.ps = 10 * ipv6.ps;
                        ipv6.ps += (c - '0');
                    }
                    else
                    {
                        fprintf(stderr, "IPv6 %s: invalid character '%c' (%d) in prefix size\n", ipv6_string, c, c);
                        ip_read_status |= IP_STRING_ERROR;
                        break;
                    }
                }
                ip_read_status |= IP_STRING_DONE;
                break;
            case '.':
                ip4 = read_ipv4_internal(ipv6_string + string_index - group_string_index - 1, 1);
                if (ip4.ps == 0)
                {
                    ip_read_status |= IP_STRING_ERROR;
                    break;
                }
                ipv6.ip[group_index++] = ip4.ip >> 16;
                ipv6.ip[group_index] = ip4.ip & 0xFFFF;
                while (((c = ipv6_string[string_index++]) != '/') && (c != '\0'))
                    ;
                string_index--;
                break;
            case '\0':
                if ((group_string_index == 0) && (double_colon_marker_position != group_index))
                {
                    fprintf(stderr, "IPv6 string %s ends with empty group\n", ipv6_string);
                    ip_read_status |= IP_STRING_ERROR;
                    break;
                }
                ip_read_status = IP_STRING_DONE;
                break;
            default:
                fprintf(stderr, "IPv6 string %s has an invalid character '%c' (%d) at position %d\n", ipv6_string, c, c, string_index - 1);
                ip_read_status |= IP_STRING_ERROR;
                break;
        }
    }

    if (double_colon_marker_position > -1)
    {
        for (uint8_t i = 7; i >= double_colon_marker_position + 7 - group_index; i--)
        {
            ipv6.ip[i] = ipv6.ip[i - (7 - group_index)];
            ipv6.ip[i - (7 - group_index)] = 0;
        }
        group_index = 7;
    }

    if ((ip_read_status == IP_STRING_DONE) && (group_index != 7))
    {
        fprintf(stderr, "IPv6 string %s must have 8 groups, found %d\n", ipv6_string, group_index + 1);
        ip_read_status |= IP_STRING_ERROR;
    }
    if ((ip_read_status == IP_STRING_DONE) && (ipv6.ps == 0))
    {
        fprintf(stderr, "IPv6 string %s has missing or zero prefix size\n", ipv6_string);
        ip_read_status |= IP_STRING_ERROR;
    }
    if ((ip_read_status == IP_STRING_DONE) && (ipv6.ps > 128))
    {
        fprintf(stderr, "IPv6 string %s has a prefix size of more than 128 bits: %d\n", ipv6_string, ipv6.ps);
        ip_read_status |= IP_STRING_ERROR;
    }

    if (ip_read_status & IP_STRING_ERROR)
    {
        for (group_index = 0; group_index < 8; group_index++)
        {
            ipv6.ip[group_index] = 0;
        }
        ipv6.ps = 0;
    }
    return ipv6;
}

void ip4string(char *string_buffer, ipv4_t ip)
{
    uint8_t string_index = 0;

    for (uint8_t group_index = 0; group_index < 3; group_index++)
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
