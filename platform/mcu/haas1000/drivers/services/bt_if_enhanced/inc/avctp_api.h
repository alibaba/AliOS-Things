/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef _AVCTP_API_H
#define _AVCTP_API_H

typedef uint8_t btif_avctp_event_t;

#define BTIF_AVCTP_CONNECT_EVENT             1

#define BTIF_AVCTP_CONNECT_IND_EVENT         2

#define BTIF_AVCTP_DISCONNECT_EVENT          3

#define BTIF_AVCTP_COMMAND_EVENT             4

#define BTIF_AVCTP_RESPONSE_EVENT            5

#define BTIF_AVCTP_REJECT_EVENT              6

#define BTIF_AVCTP_TX_DONE_EVENT            8

#define BTIF_AVCTP_OPERANDS_EVENT            9

#define BTIF_AVCTP_LAST_EVENT                9

#define BTIF_AVCTP_CTYPE_CONTROL               0x00
#define BTIF_AVCTP_CTYPE_STATUS                0x01
#define BTIF_AVCTP_CTYPE_SPECIFIC_INQUIRY      0x02
#define BTIF_AVCTP_CTYPE_NOTIFY                0x03
#define BTIF_AVCTP_CTYPE_GENERAL_INQUIRY       0x04
#define BTIF_AVCTP_CTYPE_BROWSING              0x80
#define BTIF_AVCTP_CTYPE_RESERVED              0x0A

typedef U8 avctp_response_t;

#define BTIF_AVCTP_RESPONSE_NOT_IMPLEMENTED    0x08
#define BTIF_AVCTP_RESPONSE_ACCEPTED           0x09
#define BTIF_AVCTP_RESPONSE_REJECTED           0x0A
#define BTIF_AVCTP_RESPONSE_IN_TRANSITION      0x0B
#define BTIF_AVCTP_RESPONSE_IMPLEMENTED_STABLE 0x0C
#define BTIF_AVCTP_RESPONSE_CHANGED            0x0D
#define BTIF_AVCTP_RESPONSE_INTERIM            0x0F
#define BTIF_AVCTP_RESPONSE_BROWSING           0x40

typedef U8 btif_avctp_opcode_t;

#define BTIF_AVCTP_OPCODE_VENDOR_DEPENDENT     0x00
#define BTIF_AVCTP_OPCODE_UNIT_INFO            0x30
#define BTIF_AVCTP_OPCODE_SUBUNIT_INFO         0x31
#define BTIF_AVCTP_OPCODE_PASS_THROUGH         0x7C

#define BTIF_AVCTP_PACKET_TYPE_SINGLE          0
#define BTIF_AVCTP_PACKET_TYPE_START           1
#define BTIF_AVCTP_PACKET_TYPE_CONTINUE        2
#define BTIF_AVCTP_PACKET_TYPE_END             3

typedef U8 btif_avctp_errorcode_t;

#define BTIF_AVRCP_ERR_INVALID_CMD         0x00
#define BTIF_AVRCP_ERR_INVALID_PARM        0x01

typedef void avctp_cmdframe_t;
typedef void avctp_rspframe_t;

typedef struct {

    /* Callback parameter object, depending on "event" */
    union {

        void *remDev;

        void *cmdFrame;

        void *rspFrame;
    } p;
} avctp_callback_parms_t;

#endif /*  */
