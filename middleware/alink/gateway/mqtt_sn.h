/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __AOS_MQTT_SN_H__
#define __AOS_MQTT_SN_H__

#include <stdint.h>

#define MQTT_SN_PORT 1883

enum {
    ADVERTISE = 0x0,
    SEARCHGW,
    GWINFO,
    reserved1,
    CONNECT,
    CONNACK,
    WILLTOPICREQ,
    WILLTOPIC,
    WILLMSGREQ,
    WILLMSG,
    REGISTER,
    REGACK,
    PUBLISH,
    PUBACK,
    PUBCOMP,
    PUBREC,
    PUBREL,
    reserved2,
    SUBSCRIBE,
    SUBACK,
    UNSUBSCRIBE,
    UNSUBACK,
    PINGREQ,
    PINGRESP,
    DISCONNECT,
    reserved3,
    WILLTOPICUPD,
    WILLTOPICRESP,
    WILLMSGUPD,
    WILLMSGRESP,
    ENCAP = 0xfe,
};

#define FLAG_DUP    0x80
#define FLAG_QoS    0x60
#define FLAG_RETAIN 0x10
#define FLAG_WILL   0x08
#define FLAG_CLEAN  0x04
#define FLAG_TOPIC  0x03

#define QoS_00 0x00
#define QoS_01 0x20
#define QoS_10 0x40
#define QoS_11 0x60

#define TOPIC_NORMAL 0x0
#define TOPIC_PRE    0x1
#define TOPIC_SHORT  0x2
#define TOPIC_RSV    0x3
typedef struct {
    uint8_t Flags;
    uint8_t TopicId[2];
    uint8_t MsgId[2];
    uint8_t payload[];
} __attribute__((packed)) pub_body_t;

typedef struct {
    uint8_t Flags;
    uint8_t MsgId[2];
    union {
        char TopicName[1];
        uint8_t TopicId[2];
    };
} __attribute__((packed)) sub_body_t;

typedef struct {
    uint8_t Flags;
    uint8_t ProtocolId;
    uint8_t Duration[2];
    char ClientId[];
} __attribute__((packed)) conn_body_t;

typedef struct {
    uint8_t ReturnCode;
    uint8_t payload[];
} __attribute__((packed)) conn_ack_t;

typedef struct {
    uint8_t GwId;
    uint8_t Duration[2];
    uint8_t payload[];
} __attribute__((packed)) adv_body_t;

static inline int msn_parse_header(uint8_t *buf, int l, uint8_t **pbuf)
{
    int len = *buf ++;
    if (len == 1) {
        len = *buf ++;
        len <<= 8;
        len += *buf ++;
    } else {
        len = (uint8_t)len - 1;
    }

    if (pbuf) {
        *pbuf = buf;
    }

    if (l != len &&
        l != len + 3) {
        return -1;
    }

    return len;
}

static inline int msn_header_len(uint8_t msg_type)
{
    switch (msg_type) {
        case PUBLISH:
            return sizeof(pub_body_t);
        case CONNECT:
            return sizeof(conn_body_t);
        case CONNACK:
            return sizeof(conn_ack_t);
        case ADVERTISE:
            return sizeof(adv_body_t);
    }
    return 0;
}

static inline void *msn_alloc(uint8_t msg_type, int payload_len, void **pbuf, int *plen)
{
    int header_len = 2 + msn_header_len(msg_type);
    int len;
    uint8_t *buf;

    len = header_len + payload_len;
    buf = aos_malloc(len);
    buf[0] = header_len + payload_len + 1;
    buf[1] = msg_type;

    *pbuf = buf;
    *plen = len;

    return buf + 2;
}
#endif
