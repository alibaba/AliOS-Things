/*!
 * @filename umesh_types.h
 * This is uMesh data structures header file
 *
 * @copyright  Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UMESH_TYPES_H
#define UMESH_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#include "aos/list.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup aos_umesh uMesh
 *  AliOS Things mesh networks
 *
 *  @{
 */

/**
 * mesh error number
 */
typedef enum ur_error_s {
    UR_ERROR_NONE          = 0,   /** < success */
    UR_ERROR_FAIL          = 1,   /** < fail */
    UR_ERROR_BUSY          = 2,   /** < error busy */
    UR_ERROR_DROP          = 3,   /** < error drop */
    UR_ERROR_MEM           = 4,   /** < error mem */
    UR_ERROR_ROUTE         = 5,   /** < error route */
    UR_ERROR_PARSE         = 6,   /** < error parse */
    UR_ERROR_ADDRESS_QUERY = 7,   /** < error address query */
    UR_ERROR_BUFFER        = 8,   /** < error buffer */
    UR_ERROR_INVALID_PARM  = 9,   /** < error invalid paramater */
} ur_error_t;

/**
 * network address size
 */
enum {
    SHORT_ADDR_SIZE = 2,    /** < short address size */
    EXT_ADDR_SIZE   = 8,    /** < extension address size */
    EXT_NETID_SIZE  = 6,    /** < extension network id size */
};

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
 * MAC address
 */
typedef struct mac_address_s {
    union {
        uint64_t value;               /** < MAC address in uint64_t */
        uint16_t short_addr;          /** < MAC address in uint16_t */
        uint8_t addr[EXT_ADDR_SIZE];  /** < MAC address in uint8_t  */
    };
    uint8_t len;                      /** <MAC address length */
} mac_address_t;

/**
 * uMesh extension network ID
 */
typedef struct umesh_extnetid_s {
    uint8_t netid[EXT_NETID_SIZE];   /** < extension network ID */
    uint8_t len;                     /** < extension network ID length */
} umesh_extnetid_t;

struct pbuf;
typedef ur_error_t (* adapter_input_t)(struct pbuf *buf);
typedef ur_error_t (* adapter_interface_up_t)(void);
typedef ur_error_t (* adapter_interface_down_t)(void);
typedef ur_error_t (* adapter_interface_update_t)(void);

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

/**
 * node modes
 */
typedef enum node_mode_s {
    MODE_NONE     = 0x00,   /** < this is only for testing */
    MODE_MOBILE   = 0x01,   /** < mobile node mode */
    MODE_LOW_MASK = 0x0f,   /** < mode low mask */
    MODE_RX_ON    = 0x10,   /** < normal node mode */
    MODE_SUPER    = 0x20,   /** < super node mode */
    MODE_LEADER   = 0x40,   /** < leader node mode */
    MODE_HI_MASK  = 0xf0,   /** < mode high mask */
} node_mode_t;

/**
 * mesh events code
 */
#define CODE_MESH_STARTED        1    /** < mesh started event */
#define CODE_MESH_ATTACHED       2    /** < mesh attached event */
#define CODE_MESH_DETACHED       3    /** < mesh detached event */
#define CODE_MESH_CONNECTED      4    /** < mesh connected event */
#define CODE_MESH_DISCONNECTED   5    /** < mesh disconnected event */
#define CODE_MESH_DATA_RECV      6    /** < mesh data recv event */
#define CODE_MESH_PSCHED_UP      7    /** < mesh lowpower parent schedule up event */
#define CODE_MESH_ASCHED_UP      8    /** < mesh lowpower schedule up event */
#define CODE_MESH_SCHED_DOWN     9    /** < mesh lowpower schedule down event */

/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* UMESH_TYPES_H */
