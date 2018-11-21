/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __USB_HOST_RNDIS_COMMON_H
#define __USB_HOST_RNDIS_COMMON_H

#ifdef __cplusplus
 extern "C" {
#endif

/** Maxsize in bytes of an Ethernet frame according to the Ethernet standard. */
#define ETHERNET_FRAME_SIZE_MAX               1500

/* RNDIS Message Values */
#define RNDIS_PACKET_MSG                0x00000001
#define RNDIS_INITIALIZE_MSG            0x00000002
#define RNDIS_HALT_MSG                  0x00000003
#define RNDIS_QUERY_MSG                 0x00000004
#define RNDIS_SET_MSG                   0x00000005
#define RNDIS_RESET_MSG                 0x00000006
#define RNDIS_INDICATE_STATUS_MSG       0x00000007
#define RNDIS_KEEPALIVE_MSG             0x00000008

/* RNDIS Message Status Value */
#define RNDIS_STATUS_SUCCESS            0x00000000
#define RNDIS_STATUS_FAILURE            0xC0000001
#define RNDIS_STATUS_INVALID_DATA       0xC0010015
#define RNDIS_STATUS_NOT_SUPPORTED      0xC00000BB
#define RNDIS_STATUS_MEDIA_CONNECT      0x4001000B
#define RNDIS_STATUS_MEDIA_DISCONNECT   0x4001000C

/* Implemented RNDIS Version */
#define RNDIS_VERSION_MAJOR 0x00000001
#define RNDIS_VERSION_MINOR 0x00000000

/* RNDIS Packet Filter */
#define RNDIS_PACKET_DIRECTED           0x00000001
#define RNDIS_PACKET_MULTICAST          0x00000002
#define RNDIS_PACKET_ALL_MULTICAST      0x00000004
#define RNDIS_PACKET_BROADCAST          0x00000008
#define RNDIS_PACKET_SOURCE_ROUTING     0x00000010
#define RNDIS_PACKET_PROMISCUOUS        0x00000020
#define RNDIS_PACKET_SMT                0x00000040
#define RNDIS_PACKET_ALL_LOCAL          0x00000080
#define RNDIS_PACKET_GROUP              0x00001000
#define RNDIS_PACKET_ALL_FUNCTIONAL     0x00002000
#define RNDIS_PACKET_FUNCTIONAL         0x00004000
#define RNDIS_PACKET_MAC_FRAME          0x00008000

/* RNDIS OID Values */
#define OID_GEN_SUPPORTED_LIST                  0x00010101
#define OID_GEN_HARDWARE_STATUS                 0x00010102
#define OID_GEN_MEDIA_SUPPORTED                 0x00010103
#define OID_GEN_MEDIA_IN_USE                    0x00010104
#define OID_GEN_MAXIMUM_LOOKAHEAD               0x00010105
#define OID_GEN_MAXIMUM_FRAME_SIZE              0x00010106
#define OID_GEN_LINK_SPEED                      0x00010107
#define OID_GEN_TRANSMIT_BUFFER_SPACE           0x00010108
#define OID_GEN_RECEIVE_BUFFER_SPACE            0x00010109
#define OID_GEN_TRANSMIT_BLOCK_SIZE             0x0001010A
#define OID_GEN_RECEIVE_BLOCK_SIZE              0x0001010B
#define OID_GEN_VENDOR_ID                       0x0001010C
#define OID_GEN_VENDOR_DESCRIPTION              0x0001010D
#define OID_GEN_CURRENT_PACKET_FILTER           0x0001010E
#define OID_GEN_CURRENT_LOOKAHEAD               0x0001010F
#define OID_GEN_DRIVER_VERSION                  0x00010110
#define OID_GEN_MAXIMUM_TOTAL_SIZE              0x00010111
#define OID_GEN_PROTOCOL_OPTIONS                0x00010112
#define OID_GEN_MAC_OPTIONS                     0x00010113
#define OID_GEN_MEDIA_CONNECT_STATUS            0x00010114
#define OID_GEN_MAXIMUM_SEND_PACKETS            0x00010115
#define OID_GEN_VENDOR_DRIVER_VERSION           0x00010116

#define OID_802_3_PERMANENT_ADDRESS             0x01010101
#define OID_802_3_CURRENT_ADDRESS               0x01010102
#define OID_802_3_MULTICAST_LIST                0x01010103
#define OID_802_3_MAXIMUM_LIST_SIZE             0x01010104
#define OID_802_3_MAC_OPTIONS                   0x01010105




#ifdef __cplusplus
}
#endif

#endif /* __USB_HOST_RNDIS_COMMON_H */
