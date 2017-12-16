/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SAL_IPADDR_H_
#define _SAL_IPADDR_H_

#ifdef __cplusplus
extern "C" {
#endif

/* If your port already typedef's in_addr_t, define IN_ADDR_T_DEFINED
   to prevent this code from redefining it. */
#if !defined(in_addr_t) && !defined(IN_ADDR_T_DEFINED)
typedef u32_t in_addr_t;
#endif

struct in_addr {
    in_addr_t s_addr;
};

struct in6_addr {
    union {
        u32_t u32_addr[4];
        u8_t  u8_addr[16];
    } un;
#define s6_addr  un.u8_addr
};

enum sal_ip_addr_type {
    /** IPv4 */
    IPADDR_TYPE_V4 =   0U,
    /** IPv6 */
    IPADDR_TYPE_V6 =   6U,
    /** IPv4+IPv6 ("dual-stack") */
    IPADDR_TYPE_ANY = 46U
};

typedef struct ip4_addr {
    u32_t addr;
} ip4_addr_t;

typedef struct ip6_addr {
    u32_t addr[4];
} ip6_addr_t;

typedef struct _ip_addr {
    union {
        ip6_addr_t ip6;
        ip4_addr_t ip4;
    } u_addr;
    /** @ref sal_ip_addr_type */
    u8_t type;
} ip_addr_t;

/** 255.255.255.255 */
#define IPADDR_NONE         ((u32_t)0xffffffffUL)
/** 127.0.0.1 */
#define IPADDR_LOOPBACK     ((u32_t)0x7f000001UL)
/** 0.0.0.0 */
#define IPADDR_ANY          ((u32_t)0x00000000UL)
/** 255.255.255.255 */
#define IPADDR_BROADCAST    ((u32_t)0xffffffffUL)

/** 255.255.255.255 */
#define IPADDR_NONE         ((u32_t)0xffffffffUL)
/** 127.0.0.1 */
#define IPADDR_LOOPBACK     ((u32_t)0x7f000001UL)
/** 0.0.0.0 */
#define IPADDR_ANY          ((u32_t)0x00000000UL)
/** 255.255.255.255 */
#define IPADDR_BROADCAST    ((u32_t)0xffffffffUL)

/** 255.255.255.255 */
#define INADDR_NONE         IPADDR_NONE
/** 127.0.0.1 */
#define INADDR_LOOPBACK     IPADDR_LOOPBACK
/** 0.0.0.0 */
#define INADDR_ANY          IPADDR_ANY
/** 255.255.255.255 */
#define INADDR_BROADCAST    IPADDR_BROADCAST


in_addr_t ipaddr_addr(const char *cp);
int ip4addr_aton(const char *cp, ip4_addr_t *addr);
char *ip4addr_ntoa(const ip4_addr_t *addr);

#define inet_addr(cp) ipaddr_addr(cp)
#define inet_aton(cp,addr) ip4addr_aton(cp,(ip4_addr_t*)addr)
#define inet_ntoa(addr) ip4addr_ntoa((const ip4_addr_t*)&(addr))


#ifdef __cplusplus
}
#endif

#endif
