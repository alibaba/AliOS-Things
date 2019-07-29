/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_IP_H
#define UR_IP_H

#include <stdint.h>

#include "hal/interface_context.h"

enum {
    UR_IP6_MTU = 1500,
};

enum {
    UR_IPPROTO_TCP    = 6,
    UR_IPPROTO_UDP    = 17,
    UR_IPPROTO_ICMPV6 = 58,
};

enum {
#if LWIP_IPV6
    UR_ICMP6_TYPE_EREQ = 128,
    UR_ICMP6_TYPE_EREP = 129,
#else
    UR_ICMP6_TYPE_EREQ = 8,
    UR_ICMP6_TYPE_EREP = 0,
#endif
};

typedef struct ur_ip6_header_s {
    uint32_t v_tc_fl;
    uint16_t len;
    uint8_t next_header;
    uint8_t hop_lim;
    ur_ip6_addr_t src;
    ur_ip6_addr_t dest;
} __attribute__((packed)) ur_ip6_header_t;

typedef struct ur_ip4_header_s {
    uint8_t v_hl;
    uint8_t tos;
    uint16_t len;
    uint16_t id;
    uint16_t offset;
    uint8_t ttl;
    uint8_t proto;
    uint16_t chksum;
    ur_ip4_addr_t src;
    ur_ip4_addr_t dest;
} __attribute__((packed)) ur_ip4_header_t;

typedef struct ur_icmp6_header_s {
    uint8_t type;
    uint8_t code;
    uint16_t chksum;
    uint32_t data;
} __attribute__((packed)) ur_icmp6_header_t;

typedef struct ur_udp_header_s {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t chksum;
} __attribute__((packed)) ur_udp_header_t;

ur_error_t string_to_ip6_addr(const char *buf, ur_ip6_addr_t *target);
#endif  /* UR_IP_H */
