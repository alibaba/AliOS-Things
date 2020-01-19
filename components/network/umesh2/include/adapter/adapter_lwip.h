/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _UMESH_ADAPTER_LWIP_H_
#define _UMESH_ADAPTER_LWIP_H_

#include <aos/list.h>
#include "define.h"
#include "state.h"
/**
 * IP address size
 */
enum {
    UR_IP6_ADDR_SIZE   = 16,  /** < IPv6 address size */
    MESH_IP4_ADDR_SIZE = 4,   /** < IPv4 address size */
};

/**
 * mesh media type enum
 */
typedef enum media_type_s {
    MEDIA_TYPE_DFL  = 0,    /** < default media type */
    MEDIA_TYPE_WIFI = 1,    /** < media type Wi-Fi */
    MEDIA_TYPE_BLE  = 2,    /** < media type BLE  */
    MEDIA_TYPE_15_4 = 3,    /** < media type 802.15.4 */
} media_type_t;

#ifndef NULL
#define NULL (void *)0
#endif

/**
 * IPv6 address
 */
typedef struct ur_ip6_addr_s {
    union {
        uint8_t m8[UR_IP6_ADDR_SIZE];                        /** < IPv6 address in uint8_t */
        uint16_t m16[UR_IP6_ADDR_SIZE / sizeof(uint16_t)];   /** < IPv6 address in uint16_t */
        uint32_t m32[UR_IP6_ADDR_SIZE / sizeof(uint32_t)];   /** < IPv6 address in uint32_t */
    };
} __attribute__((packed)) ur_ip6_addr_t;

/**
 * IPv6 address prefix
 */
typedef struct ur_ip6_prefix_s {
    ur_ip6_addr_t prefix;                      /** < IPv6 address prefix */
    uint8_t length;                            /** < prefix length */
} __attribute__((packed)) ur_ip6_prefix_t;

/**
 * IP packet header length
 */
enum {
    UR_IP6_HLEN   = 40,    /** < IPv6 header length */
    MESH_IP4_HLEN = 20,    /** < IPv4 header length */
    UR_UDP_HLEN   = 8,     /** < UDP header length  */
};

/**
 * MAC address
 */
typedef struct mac_address_s {
    union {
        uint64_t value;               /** < MAC address in uint64_t */
        uint16_t short_addr;          /** < MAC address in uint16_t */
        uint8_t addr[IEEE80211_MAC_ADDR_LEN];  /** < MAC address in uint8_t  */
    };
    uint8_t len;                      /** <MAC address length */
} mac_address_t;

struct pbuf;
typedef int (* adapter_input_t)(struct pbuf *buf);
typedef int (* adapter_interface_up_t)(void);
typedef int (* adapter_interface_down_t)(void);
typedef int (* adapter_interface_update_t)(void);

/**
 * adapter callbacks
 */
typedef struct ur_adapter_callback_s {
    slist_t next;                                   /** < next pointer */
    adapter_input_t input;                          /** < adapter input */
    adapter_interface_up_t interface_up;            /** < interface up */
    adapter_interface_down_t interface_down;        /** < interface down */
    adapter_interface_update_t interface_update;    /** < interface update */
} ur_adapter_callback_t;


int umesh_adapter_interface_init(umesh_state_t *state);
#endif /* UMESH_TYPES_PRIV_H */
