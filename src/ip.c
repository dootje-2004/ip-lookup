#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "ip.h"

/**
 * @brief Enum to keep track of the read status while processing an IPV4 string.
 */
typedef enum
{
    IP_STRING_READING = 0x0,
    IP_STRING_DONE = 0x1,
    IP_STRING_ERROR = 0x2
} ip_string_status;

/**
 * @brief Struct for parsing an IPv4 string.
 */
typedef struct
{
    const char* ipv4_string;
    ipv4_t ipv4;
    uint8_t string_index;
    int16_t group_value;
    uint8_t group_index;
    uint8_t group_string_index;
} ipv4_read_object;

typedef struct
{
    const char* ipv6_string;
    ipv6_t ipv6;
    uint8_t string_index;
    uint8_t group_index;
    uint8_t group_string_index;
    int8_t double_colon_marker_position;
    ipv4_t ip4;
} ipv6_read_object;

/**
 * @brief Checks an IPv4 group for leading zeroes. Prints a message to stderr if there are.
 * 
 * @param ipo                IPv4 bookkeeping struct.
 * @return ip_string_status  IP_STRING_READING if ok, IP_STRING_ERROR if there are leading zeroes.
 */
ip_string_status check_leading_zeroes(const ipv4_read_object ipo)
{
    ip_string_status result = IP_STRING_READING;

    if ((ipo.group_value == 0) && (ipo.group_string_index > 0))
    {
        fprintf(stderr, "IPv4 string %s, group %d: leading zeroes are not allowed\n", ipo.ipv4_string, ipo.group_index);
        result = IP_STRING_ERROR;
    }
    return result;
}

/**
 * @brief Checks if an IPv4 group value is in the range 0-255. Prints a message to stderr if it is not.
 * 
 * @param ipo                IPv4 bookkeeping struct.
 * @return ip_string_status  IP_STRING_READING if ok, IP_STRING_ERROR if not.
 */
ip_string_status check_group_value(const ipv4_read_object ipo)
{
    ip_string_status result = IP_STRING_READING;

    if (ipo.group_value == -1)
    {
        fprintf(stderr, "IPv4 string %s: group %d is empty\n", ipo.ipv4_string, ipo.group_index);
        result = IP_STRING_ERROR;
    }
    else if (ipo.group_value > 255)
    {
        fprintf(stderr, "IPv4 string %s: group %d has a value greater than 255: %d\n", ipo.ipv4_string, ipo.group_index, ipo.group_value);
        result = IP_STRING_ERROR;
    }
    return result;
}

/**
 * @brief Checks if an IP string has the required number of groups. Prints a message to stderr is it has not.
 * 
 * @param ip_string          IP string.
 * @param group_count        Actual number of groups.
 * @param target_count       Required number of groups.
 * @return ip_string_status  IP_STRING_READING if ok, IP_STRING_ERROR if not.
 */
ip_string_status check_group_count(const char* ip_string, const uint8_t group_count, const uint8_t target_count)
{
    ip_string_status result = IP_STRING_DONE;

    if (group_count != target_count)
    {
        fprintf(stderr, "IP string %s has %d groups instead of %d\n", ip_string, group_count, target_count);
        result = IP_STRING_ERROR;
    }
    return result;
}

/**
 * @brief Checks if an IPv4 prefix size is in the range 1-32. Prints a message to stderr if it is not.
 * 
 * @param ipo                IPv4 bookkeeping struct.
 * @return ip_string_status  IP_STRING_DONE if ok, IP_STRING_ERROR if not.
 */
ip_string_status check_prefix_size(const char* ip_string, const uint8_t ps_actual, const uint8_t ps_max)
{
    ip_string_status result = IP_STRING_DONE;

    if (ps_actual == 0)
    {
        fprintf(stderr, "IPv4 string %s has missing or zero prefix size\n", ip_string);
        result = IP_STRING_ERROR;
    }
    else if (ps_actual > ps_max)
    {
        fprintf(stderr, "IPv4 string %s has a prefix size of more than %d bits: %d\n", ip_string, ps_actual, ps_max);
        result = IP_STRING_ERROR;
    }
    return result;
}

/**
 * @brief Reads the prefix size at the tail of an IP string.
 * 
 * @param ip_string          IP string; can be IPv4 or IPv6.
 * @param string_index       Current position in ip_string.
 * @param prefix             Holds the numerical prefix size.
 * @return ip_string_status  IP_STRING_DONE if reading succeeds, IP_STRING_ERROR if it doesn't.
 */
ip_string_status read_prefix_size(const char* ip_string, uint8_t *string_index, uint8_t* prefix_size)
{
    char c;
    ip_string_status result = IP_STRING_DONE;

    *prefix_size = 0;
    while ((c = ip_string[(*string_index)++]) != '\0')
    {
        if (c >= '0' && c <= '9')
        {
            *prefix_size *= 10;
            *prefix_size += (c - '0');
        }
        else
        {
            *prefix_size = 0;
            fprintf(stderr, "IP string %s: invalid character '%c' (%d) in prefix size\n", ip_string, c, c);
            result |= IP_STRING_ERROR;
        }
    }
    return result;
}

/**
 * @brief Prints a message that an invalid character was encountered at the current reading position.
 * 
 * @param ip_string   IP string.
 * @param index       String position.
 */
void invalid_character_message(const char* ip_string, const uint8_t index)
{
    char c = ip_string[index];
    fprintf(stderr, "IP string %s has an invalid character '%c' (%d) at position %d\n", ip_string, c, c, index);
}

/**
 * @brief Wrapper function for read_ipv4 that allows ignoring the prefix.
 * Used when reading the IPv4 part of a mapped IPv4-to-IPv6 address.
 *
 * @param ipv4_string   String representing an IPv4 address.
 * @param skip_prefix   0: check prefix as IPv4; > 0: skip reading the prefix and set it to 32.
 * @return ipv4_t       IPv4 struct.
 */
ipv4_t read_ipv4_internal(const char *ipv4_string, const uint8_t skip_prefix)
{
    char c;
    ip_string_status ip_read_status = IP_STRING_READING;
    
    ipv4_read_object ipo;
    ipo.ipv4_string = ipv4_string;
    ipo.ipv4.ip = 0;
    ipo.ipv4.ps = 32;
    ipo.string_index = 0;
    ipo.group_value = -1;
    ipo.group_index = 0;
    ipo.group_string_index = 0;

    while (ip_read_status == IP_STRING_READING)
    {
        c = ipo.ipv4_string[ipo.string_index++];
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
                ip_read_status |= check_leading_zeroes(ipo);
                if (ip_read_status & IP_STRING_ERROR)
                {
                    break;
                }
                if (ipo.group_value == -1)
                {
                    ipo.group_value = 0;
                }
                ipo.group_value = ipo.group_value * 10 + (c - '0');
                ip_read_status |= check_group_value(ipo);
                if (ip_read_status & IP_STRING_ERROR)
                {
                    break;
                }
                ipo.group_string_index++;
                break;
            case '.':
                ip_read_status |= check_group_value(ipo);
                if (ip_read_status & IP_STRING_ERROR)
                {
                    break;
                }
                ipo.ipv4.ip = (ipo.ipv4.ip << 8) + ipo.group_value;
                ipo.group_value = -1;
                ipo.group_index++;
                ipo.group_string_index = 0;
                break;
            case '\0':
                ip_read_status |= check_group_value(ipo);
                if (ip_read_status & IP_STRING_ERROR)
                {
                    break;
                }
                ipo.ipv4.ip = (ipo.ipv4.ip << 8) + ipo.group_value;
                ip_read_status = IP_STRING_DONE;
                break;
            case '/':
                ip_read_status |= check_group_value(ipo);
                if (ip_read_status & IP_STRING_ERROR)
                {
                    break;
                }
                ipo.ipv4.ip = (ipo.ipv4.ip << 8) + ipo.group_value;
                if (skip_prefix != 0)
                {
                    ip_read_status = IP_STRING_DONE;
                    break;
                }
                ip_read_status |= read_prefix_size(ipo.ipv4_string, &(ipo.string_index), &(ipo.ipv4.ps));
                break;
            default:
                invalid_character_message(ipo.ipv4_string, ipo.string_index - 1);
                ip_read_status |= IP_STRING_ERROR;
                break;
        }
    }

    ip_read_status |= check_group_count(ipo.ipv4_string, ipo.group_index + 1, 4);
    ip_read_status |= check_prefix_size(ipo.ipv4_string, ipo.ipv4.ps, 32);
    if (ip_read_status & IP_STRING_ERROR)
    {
        ipo.ipv4.ip = 0;
        ipo.ipv4.ps = 0;
    }
    return ipo.ipv4;
}

ipv4_t read_ipv4(const char *ipv4_string)
{
    return read_ipv4_internal(ipv4_string, 0);
}

ip_string_status check_if_ipv6_group_empty(const ipv6_read_object ipo)
{
    ip_string_status result = IP_STRING_READING;

    if ((ipo.group_string_index == 0) && (ipo.double_colon_marker_position != ipo.group_index))
    {
        fprintf(stderr, "IPv6 string %s ends with empty group\n", ipo.ipv6_string);
        result = IP_STRING_ERROR;
    }
    return result;
}

ip_string_status handle_group_termination(ipv6_read_object* ipo)
{
    ip_string_status result = IP_STRING_READING;

    if (ipo->ipv6_string[ipo->string_index] == ':')
    {
        if (ipo->double_colon_marker_position == -1)
        {
            ipo->string_index += 1;
            ipo->double_colon_marker_position = ipo->group_index;
        }
        else
        {
            fprintf(stderr, "IPv6 string %s has multiple double colons\n", ipo->ipv6_string);
            result |= IP_STRING_ERROR;
        }
    }
    else
    {
        result |= check_if_ipv6_group_empty(*ipo);
    }
    return result;
}

void handle_double_colon_marker(ipv6_read_object *ipo)
{
    if (ipo->double_colon_marker_position > -1)
    {
        for (uint8_t i = 7; i >= ipo->double_colon_marker_position + 7 - ipo->group_index; i--)
        {
            ipo->ipv6.ip[i] = ipo->ipv6.ip[i - (7 - ipo->group_index)];
            ipo->ipv6.ip[i - (7 - ipo->group_index)] = 0;
        }
        ipo->group_index = 7;
    }
}

ipv6_t read_ipv6(const char *ipv6_string)
{
    char c;
    ip_string_status ip_read_status = IP_STRING_READING;
    ipv6_read_object ipo;

    ipo.ipv6_string = ipv6_string;
    for (ipo.group_index = 0; ipo.group_index < 8; ipo.group_index++)
    {
        ipo.ipv6.ip[ipo.group_index] = 0;
    }
    ipo.ipv6.ps = 128;
    ipo.string_index = 0;
    ipo.group_index = 0;
    ipo.group_string_index = 0;
    ipo.double_colon_marker_position = -1;

    while (ip_read_status == IP_STRING_READING)
    {
        c = ipo.ipv6_string[ipo.string_index++];
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
                ipo.ipv6.ip[ipo.group_index] = ipo.ipv6.ip[ipo.group_index] * 16;
                ipo.ipv6.ip[ipo.group_index] += c - '0';
                ipo.group_string_index++;
                break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                ipo.ipv6.ip[ipo.group_index] = ipo.ipv6.ip[ipo.group_index] * 16;
                ipo.ipv6.ip[ipo.group_index] += c - 'a' + 10;
                ipo.group_string_index++;
                break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
                ipo.ipv6.ip[ipo.group_index] = ipo.ipv6.ip[ipo.group_index] * 16;
                ipo.ipv6.ip[ipo.group_index] += c - 'A' + 10;
                ipo.group_string_index++;
                break;
            case ':':
                ipo.group_index++;
                ip_read_status |= handle_group_termination(&ipo);
                if (ip_read_status & IP_STRING_ERROR)
                {
                    break;
                }
                ipo.group_string_index = 0;
                break;
            case '/':
                ip_read_status |= check_if_ipv6_group_empty(ipo);
                ip_read_status |= read_prefix_size(ipo.ipv6_string, &(ipo.string_index), &(ipo.ipv6.ps));
                ip_read_status |= IP_STRING_DONE;
                break;
            case '.':
                ipo.ip4 = read_ipv4_internal(ipo.ipv6_string + ipo.string_index - ipo.group_string_index - 1, 1);
                if (ipo.ip4.ps == 0)
                {
                    ip_read_status |= IP_STRING_ERROR;
                    break;
                }
                ipo.ipv6.ip[ipo.group_index++] = ipo.ip4.ip >> 16;
                ipo.ipv6.ip[ipo.group_index] = ipo.ip4.ip & 0xFFFF;
                while (((c = ipo.ipv6_string[ipo.string_index++]) != '/') && (c != '\0'))
                    ;
                ipo.string_index--;
                break;
            case '\0':
                ip_read_status |= check_if_ipv6_group_empty(ipo);
                ip_read_status |= IP_STRING_DONE;
                break;
            default:
                invalid_character_message(ipo.ipv6_string, ipo.string_index - 1);
                ip_read_status |= IP_STRING_ERROR;
                break;
        }
    }

    handle_double_colon_marker(&ipo);

    ip_read_status |= check_group_count(ipo.ipv6_string, ipo.group_index + 1, 8);
    ip_read_status |= check_prefix_size(ipo.ipv6_string, ipo.ipv6.ps, 128);

    if (ip_read_status & IP_STRING_ERROR)
    {
        for (ipo.group_index = 0; ipo.group_index < 8; ipo.group_index++)
        {
            ipo.ipv6.ip[ipo.group_index] = 0;
        }
        ipo.ipv6.ps = 0;
    }
    return ipo.ipv6;
}

void ip4string(char *string_buffer, ipv4_t ip)
{
    uint8_t string_index = 0;

    for (uint8_t group_index = 0; group_index < 4; group_index++)
    {
        string_index += sprintf(string_buffer + string_index, "%d.", (ip.ip >> (3 - group_index) * 8) & 0xFF);
    }
    string_index--;
    if (ip.ps < 32)
    {
        string_index += sprintf(string_buffer + string_index, "/%d", ip.ps);
    }
    string_buffer[string_index++] = '\0';
}

void ip6string(char *string_buffer, ipv6_t ip)
{
    uint8_t string_index = 0;

    for (uint8_t group_index = 0; group_index < 8; group_index++)
    {
        string_index += sprintf(string_buffer + string_index, "%x:", ip.ip[group_index]);
    }
    string_index--;
    if (ip.ps < 128)
    {
        string_index += sprintf(string_buffer + string_index, "/%d", ip.ps);
    }
    string_buffer[string_index++] = '\0';
}
