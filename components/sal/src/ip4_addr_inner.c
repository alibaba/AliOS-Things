/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "sal_sockets_internal.h"

/* Here for now until needed in other places in lwIP */
#ifndef isprint
    #define in_range(c, lo, up)  ((u8_t)c >= lo && (u8_t)c <= up)
    #define isprint(c)           in_range(c, 0x20, 0x7f)
    #define isdigit(c)           in_range(c, '0', '9')
    #define isxdigit(c)          (isdigit(c) || in_range(c, 'a', 'f') || in_range(c, 'A', 'F'))
    #define islower(c)           in_range(c, 'a', 'z')
    #define isspace(c)           (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v')
#endif

void ip4_sockaddr_to_ipstr_port(const struct sockaddr *name, char *ip)
{
    struct sockaddr_in *saddr;
    union {
        uint32_t ip_u32;
        uint8_t ip_u8[4];
    } ip_u;

    if (!name || !ip) {
        return;
    }

    saddr = (struct sockaddr_in *)name;
    memset(ip, 0, 16);

    /* Convert network order ip_addr to ip str (dot number fomrat) */
    ip_u.ip_u32 = (uint32_t)(saddr->sin_addr.s_addr);
    SAL_SNPRINTF(ip, SAL_SOCKET_IP4_ADDR_LEN, "%d.%d.%d.%d",
                 ip_u.ip_u8[0], ip_u.ip_u8[1], ip_u.ip_u8[2], ip_u.ip_u8[3]);
    ip[SAL_SOCKET_IP4_ADDR_LEN - 1] = '\0';
}

/*  Caller to ensure a valid ip string */
int ipstr_to_u32(char *ipstr, uint32_t *ip32)
{
    uint8_t *q = (uint8_t *)ip32, n = 0, stridx = 0, dotnum = 0;
    char *p = ipstr;

    if (!ipstr || !ip32) {
        return -1;
    }

    for (n = 0, stridx = 0, dotnum = 0;
         *p != '\0' && stridx < 15 && dotnum < 4;
         stridx++, p++) {
        if (*p == '.') {
            q[dotnum] = n; /* saved in network order */
            n = 0;
            dotnum++;
            continue;
        }
        if (*p < '0' || *p > '9') {
            return -1;
        }
        n = n * 10 + *p - '0';
    }

    if (dotnum >= 4 || stridx > 15) {
        return -1;
    } else {
        q[dotnum] = n;    /* the last number */
    }

    return 0;
}
