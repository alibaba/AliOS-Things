/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#include <stdio.h>
#include "Cloud_CoAPExport.h"

int Cloud_CoAPDeserialize_Header(Cloud_CoAPMessage *msg, unsigned char *buf)
{
    msg->header.version   = ((buf[0] >> 6) & 0x03);
    msg->header.type      = ((buf[0] >> 4) & 0x03);
    msg->header.tokenlen  = (buf[0] & 0x0F);
    msg->header.code      =  buf[1];
    msg->header.msgid     =  buf[2] << 8;
    msg->header.msgid    +=  buf[3];

    return 4;
}

int Cloud_CoAPDeserialize_Token(Cloud_CoAPMessage *msg, unsigned char *buf)
{
    memcpy(msg->token, buf, msg->header.tokenlen > COAP_MSG_MAX_TOKEN_LEN ? COAP_MSG_MAX_TOKEN_LEN : msg->header.tokenlen);
    return msg->header.tokenlen;
}

#define COAP_OPT(o,e,step) if ((e) < step) {           \
        COAP_ERR("cannot advance opt past end");           \
        return -1;                                         \
    } else {                                             \
        (e) -= step;                                       \
        (o) = ((o)) + step;                                \
    }

/*
 * Used to prevent access to *option when pointing to after end of buffer
 * after doing a COAP_OPT()
 */
#define COAP_OPT_CHECK(o,e,step) do {                  \
        COAP_OPT(o,e,step);                                \
        if ((e) < 1)                                       \
            return -1;                                       \
    } while (0)

static int Cloud_CoAPDeserialize_Option(Cloud_CoAPMsgOption *option, unsigned char *buf, int left, unsigned short *predeltas)
{
    unsigned char  *ptr      = buf;
    unsigned short optdelta  = 0;
    unsigned short optlen    = 0;
    unsigned short predelta  = 0;

    optdelta  = (*ptr & 0xF0) >> 4;
    optlen    = (*ptr & 0x0F);
    COAP_OPT_CHECK(ptr, left, 1);
    predelta = *predeltas;
    if (13 == optdelta) {
        predelta += 13 + *ptr;
        COAP_OPT_CHECK(ptr, left, 1);

    } else if (14 == optdelta) {
        predelta += 269;
        predelta += (*ptr << 8);
        predelta +=  *(ptr + 1);
        COAP_OPT_CHECK(ptr, left, 2);
    } else {
        predelta += optdelta;
    }
    option->num = predelta;

    if (13 == optlen) {
        optlen = 13 + *ptr;
        COAP_OPT_CHECK(ptr, left, 1);
    } else if (14 == optlen) {
        optlen = (*ptr << 8) + (*(ptr + 1));
        if (optlen + 269 < 269) {
            return -1;
        }
        optlen += 269;
        COAP_OPT_CHECK(ptr, left, 2);

    }
    option->len = optlen;

    option->val = ptr;
    *predeltas = option->num;

    return (ptr - buf + option->len);
}

int Cloud_CoAPDeserialize_Options(Cloud_CoAPMessage *msg, unsigned char *buf, int buflen)
{
    int  index = 0;
    unsigned char  *ptr      = buf;
    int            len       = 0;
    int            left      = buflen;
    unsigned short optdeltas = 0;

    msg->optnum = 0;
    while (left > 0 && (0xFF != *ptr) && index < COAP_MSG_MAX_OPTION_NUM) {
        len = Cloud_CoAPDeserialize_Option(&msg->options[index], ptr, left, &optdeltas);
        if (len < 0) {
            return len;
        }
        msg->optnum+= 1;
        ptr += len;
        left -= len;
        index ++;
    }

    return (int)(ptr - buf);
}

int Cloud_CoAPDeserialize_Payload(Cloud_CoAPMessage *msg, unsigned char *buf, int buflen)
{
    unsigned char *ptr = buf;

    if (0xFF == *ptr) {
        ptr ++;
    } else {
        return 0;
    }
    msg->payloadlen = buflen - 1;
    msg->payload = (unsigned char *)ptr;

    return buflen;
}

int Cloud_CoAPDeserialize_Message(Cloud_CoAPMessage *msg, unsigned char *buf, int buflen)
{
    int count  = 0;
    int remlen = buflen;
    unsigned char *ptr = buf;

    if (NULL == buf || NULL == msg) {
        return COAP_ERROR_INVALID_PARAM;
    }

    if (buflen < 4) {
        return COAP_ERROR_INVALID_LENGTH;
    }

    /* Deserialize CoAP header. */
    count = Cloud_CoAPDeserialize_Header(msg, ptr);
    if (count > remlen) {
        return COAP_ERROR_INVALID_LENGTH;
    }
    ptr += count;
    remlen -= count;

    /* Deserialize the token, if any. */
    count = Cloud_CoAPDeserialize_Token(msg, ptr);
    if (count > remlen) {
        return COAP_ERROR_INVALID_LENGTH;
    }
    ptr += count;
    remlen -= count;

    count = Cloud_CoAPDeserialize_Options(msg, ptr, remlen);
    if (count > remlen || count < 0) {
        return COAP_ERROR_INVALID_LENGTH;
    }
    ptr += count;
    remlen -= count;

    Cloud_CoAPDeserialize_Payload(msg, ptr, remlen);

    return COAP_SUCCESS;
}
