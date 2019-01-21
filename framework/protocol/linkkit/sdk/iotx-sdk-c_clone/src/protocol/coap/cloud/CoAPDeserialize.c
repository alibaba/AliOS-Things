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
    memcpy(msg->token, buf, msg->header.tokenlen);
    return msg->header.tokenlen;
}

static int Cloud_CoAPDeserialize_Option(Cloud_CoAPMsgOption *option, unsigned char *buf, unsigned short *predeltas)
{
    unsigned char  *ptr      = buf;
    unsigned short optdelta  = 0;
    unsigned short optlen    = 0;
    unsigned short predelta  = 0;

    optdelta  = (*ptr & 0xF0) >> 4;
    optlen    = (*ptr & 0x0F);
    ptr++;

    predelta = *predeltas;
    if (13 == optdelta) {
        predelta += 13 + *ptr;
        ptr ++;

    } else if (14 == optdelta) {
        predelta += 269;
        predelta += (*ptr << 8);
        predelta +=  *(ptr + 1);
        ptr += 2;
    } else {
        predelta += optdelta;
    }
    option->num = predelta;

    if (13 == optlen) {
        optlen = 13 + *ptr;
        ptr ++;
    } else if (14 == optlen) {
        optlen = 269;
        optlen += (*ptr << 8);
        optlen += *(ptr + 1);
        ptr += 2;
    }
    option->len = optlen;

    option->val = ptr;
    *predeltas = option->num;

    return (ptr - buf + option->len);
}

int Cloud_CoAPDeserialize_Options(Cloud_CoAPMessage *msg, unsigned char *buf, int buflen)
{
    int  index = 0;
    int  count = 0;
    unsigned char  *ptr      = buf;
    unsigned short len       = 0;
    unsigned short optdeltas = 0;

    msg->optnum = 0;
    while ((count < buflen) && (0xFF != *ptr)) {
        len = Cloud_CoAPDeserialize_Option(&msg->options[index], ptr, &optdeltas);
        msg->optnum += 1;
        ptr += len;
        index ++;
        count += len;
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
    ptr += count;
    remlen -= count;

    /* Deserialize the token, if any. */
    count = Cloud_CoAPDeserialize_Token(msg, ptr);
    ptr += count;
    remlen -= count;

    count = Cloud_CoAPDeserialize_Options(msg, ptr, remlen);
    ptr += count;
    remlen -= count;

    Cloud_CoAPDeserialize_Payload(msg, ptr, remlen);

    return COAP_SUCCESS;
}
