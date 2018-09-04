/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#include <stdio.h>
#include "CoAPExport.h"

int CoAPDeserialize_Header(CoAPMessage *msg, unsigned char *buf)
{
    msg->header.version   = ((buf[0] >> 6) & 0x03);
    msg->header.type      = ((buf[0] >> 4) & 0x03);
    msg->header.tokenlen  = (buf[0] & 0x0F);
    msg->header.code      =  buf[1];
    msg->header.msgid     =  buf[2] << 8;
    msg->header.msgid    +=  buf[3];

    return 4;
}

int CoAPDeserialize_Token(CoAPMessage *msg, unsigned char *buf)
{
    memcpy(msg->token, buf, msg->header.tokenlen);
    return msg->header.tokenlen;
}

static int CoAPDeserialize_Option(CoAPMsgOption *option, unsigned char *buf, unsigned short *predeltas)
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

int CoAPDeserialize_Options(CoAPMessage *msg, unsigned char *buf, int buflen)
{
    int  index = 0;
    int  count = 0;
    unsigned char  *ptr      = buf;
    unsigned short len       = 0;
    unsigned short optdeltas = 0;

    msg->optnum = 0;
    while ((count < buflen) && (0xFF != *ptr)) {
        len = CoAPDeserialize_Option(&msg->options[index], ptr, &optdeltas);
        msg->optnum += 1;
        ptr += len;
        index ++;
        count += len;
    }

    return (int)(ptr - buf);
}

int CoAPDeserialize_Payload(CoAPMessage *msg, unsigned char *buf, int buflen)
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

int CoAPDeserialize_Message(CoAPMessage *msg, unsigned char *buf, int buflen)
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
    count = CoAPDeserialize_Header(msg, ptr);
    ptr += count;
    remlen -= count;

    /* Deserialize the token, if any. */
    count = CoAPDeserialize_Token(msg, ptr);
    ptr += count;
    remlen -= count;

    count = CoAPDeserialize_Options(msg, ptr, remlen);
    ptr += count;
    remlen -= count;

    CoAPDeserialize_Payload(msg, ptr, remlen);

    return COAP_SUCCESS;
}
