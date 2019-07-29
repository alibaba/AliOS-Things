/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef MESH_COMPRESS6_H
#define MESH_COMPRESS6_H

#include "utilities/message.h"
#include "utilities/memory.h"
#include "hal/interface_context.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FIXED_IID  "\x00\x00\x00\xff\xfe\x00"

enum {
    IP_VERSION_6       = 0x60000000,
    VERSION_MASK       = 0xf0000000,
    TRAFFIC_CLASS_MASK = 0x0ff00000,
    TC_ECN_MASK        = 0x0c000000,
    TC_DSCP_MASK       = 0x03f00000,
    FLOW_LABEL_MASK    = 0x000fffff,
};

enum {
    IPHC_DISPATCH    = 0x3,
    NHC_UDP_DISPATCH = 0x1e,
};

enum {
    TC_FL_BOTH_APEENDED         = 0x0,
    DCSP_ELEDED_ECN_FL_APPENDED = 0x1,
    TC_APENDED_FL_ELIDED        = 0x2,
    TC_FL_BOTH_ELIDED           = 0x3,
};

enum {
    NEXT_HEADER_APPENDED = 0x0,
    NEXT_HEADER_ELIDED   = 0x1,
};

enum {
    HOP_LIM_APPENDED = 0x0,
    HOP_LIM_1        = 0x1,
    HOP_LIM_64       = 0x2,
    HOP_LIM_255      = 0x3,
};

enum {
    STATELESS_COMPRESS = 0x0,
    STATEFULL_COMPRESS = 0x1,
};

enum {
    UNICAST_DESTINATION   = 0x0,
    MULTICAST_DESTINATION = 0x1,
};

enum {
    UCAST_ADDR_128BIT = 0x0,
    UCAST_ADDR_64BIT  = 0x1,
    UCAST_ADDR_16BIT  = 0x2,
    UCAST_ADDR_ELIDED = 0x3,
};

enum {
    MCAST_ADDR_128BIT = 0x0,
    MCAST_ADDR_48BIT  = 0x1,
    MCAST_ADDR_32BIT  = 0x2,
    MCAST_ADDR_8BIT   = 0x3,
};

enum {
    CHKSUM_APPENDED = 0x0,
    CHKSUM_ELIDED   = 0x1,
};

enum {
    NO_PORT_COMPRESSED   = 0x0,
    DST_PORT_COMPRESSED  = 0x1,
    SRC_PORT_COMPRESSED  = 0x2,
    BOTH_PORT_COMPRESSED = 0x3,
};

enum {
    MIN_LOWPAN_FRM_SIZE = 6,
};

typedef struct iphc_header_s {
    unsigned int DAM  : 2;
    unsigned int DAC  : 1;
    unsigned int M    : 1;
    unsigned int SAM  : 2;
    unsigned int SAC  : 1;
    unsigned int CID  : 1;

    unsigned int HLIM : 2;
    unsigned int NH   : 1;
    unsigned int TF   : 2;
    unsigned int DP   : 3;
} __attribute__((packed)) iphc_header_t;

typedef struct nhc_header_s {
    unsigned int P  : 2;
    unsigned int C  : 1;
    unsigned int DP : 5;
} __attribute__((packed)) nhc_header_t;

ur_error_t lp_header_compress(const uint8_t *header, uint8_t *buffer,
                              uint16_t *ip_hdr_len, uint16_t *lowpan_hdr_len);
ur_error_t lp_header_decompress(uint8_t *header, uint16_t *header_size,
                                uint16_t *lowpan_header_size,
                                ur_addr_t *src, ur_addr_t *dest);
#ifdef __cplusplus
}
#endif

#endif /* MESH_COMPRESS6_H */
