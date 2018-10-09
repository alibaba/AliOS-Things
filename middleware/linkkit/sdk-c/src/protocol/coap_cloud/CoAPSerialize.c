/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#include <stdio.h>
#include "Cloud_CoAPSerialize.h"
#include "Cloud_CoAPExport.h"

int Cloud_CoAPSerialize_Header(Cloud_CoAPMessage *msg, unsigned char *buf, unsigned short buflen)
{
    if(4 > buflen){
        return 0;
    }
    buf[0] = (((msg->header.version & 0x3) << 6) | ((msg->header.type & 0x3) << 4))
                | (msg->header.tokenlen & 0x0F);

    buf[1] = msg->header.code;
    buf[2] = (msg->header.msgid & 0xFF00) >> 8;
    buf[3] = (msg->header.msgid & 0x00FF);

    return 4;
}

int Cloud_CoAPSerialize_Token(Cloud_CoAPMessage *msg, unsigned char * buf, unsigned short buflen)
{
    int i = 0;

    if(buflen < msg->header.tokenlen){
        return 0;
    }
    for (i = 0; i < msg->header.tokenlen; i++){
        buf[i] = msg->token[i];
    }
    return msg->header.tokenlen;
}

static unsigned short Cloud_CoAPSerialize_Option(Cloud_CoAPMsgOption *option, unsigned char *buf)
{
    unsigned char *ptr   = buf;

    if(269 <= option->num){
        *ptr = ((14 & 0x0F) << 4);
    }
    else if(13 <= option->num){
        *ptr = ((13 & 0x0F) << 4);
    }
    else{
        *ptr = option->num << 4;
    }

    if (269 <= option->len){
        *ptr |=  (14 & 0x0F);
    }
    else if(13 <= option->len){
        *ptr |= (13 & 0x0F);
    }
    else{
        *ptr |= (option->len & 0x0F);
    }
    ptr ++;

    if (269 <= option->num){
        *ptr     = (unsigned char)(((option->num - 269) & 0xFF00) >> 8);
        *(ptr+1) = (unsigned char)(((option->num - 269) & 0x00FF));
        ptr += 2;
    }
    else if(13 <= option->num){
        *ptr    = (unsigned char)(option->num - 13);
        ptr ++;
    }


    if (269 <= option->len){
        *ptr     = (unsigned char)(((option->len - 269) & 0xFF00) >> 8);
        *(ptr+1) = (unsigned char)(((option->len - 269) & 0x00FF));
        ptr += 2;
    }
    else if(13 <= option->len){
        *ptr = (unsigned char)(option->len - 13);
        ptr ++;
    }


    memcpy(ptr, option->val, option->len);
    ptr += option->len;

    return (int)(ptr - buf);
}

unsigned short Cloud_CoAPSerialize_Options(Cloud_CoAPMessage *msg,  unsigned char * buf, unsigned short buflen)
{
    int i      = 0;
    unsigned short count  = 0;

    for (i = 0; i < msg->optnum; i++)
    {
        unsigned short len = 0;
        len = Cloud_CoAPSerialize_Option(&msg->options[i], &buf[count]);
        if (0 < len){
            count += len;
        }
        else{
            return 0;
        }
    }

    return count;
}

static unsigned short Cloud_CoAPSerialize_OptionLen(Cloud_CoAPMsgOption *option)
{
    unsigned short  len  = 1;

    if(269 <= option->num){
        len += 2;
    }
    else if(13 <= option->num){
        len += 1;
    }
    else{
    }

    if (269 <= option->len){
        len += 2;
    }
    else if(13 <= option->len){
        len += 1;
    }
    else{
    }

    len += option->len;
    return len;
}


unsigned short Cloud_CoAPSerialize_OptionsLen(Cloud_CoAPMessage *msg)
{
    int i      = 0;
    unsigned short count  = 0;

    for (i = 0; i < msg->optnum; i++)
    {
        unsigned short len = 0;
        len = Cloud_CoAPSerialize_OptionLen(&msg->options[i]);
        if (0 < len){
            count += len;
        }
        else{
            return 0;
        }
    }

    return count;
}


int Cloud_CoAPSerialize_Payload(Cloud_CoAPMessage *msg, unsigned char *buf, int buflen)
{
    if(msg->payloadlen + 1 > buflen){
        return -1;
    }
    if(msg->payloadlen > 0 && NULL != msg->payload)
    {
        *buf = 0xFF;
        buf ++;
        memcpy(buf, msg->payload, msg->payloadlen);
        return msg->payloadlen + 1;
    }
    else{
        return 0;
    }
}


unsigned short Cloud_CoAPSerialize_MessageLength(Cloud_CoAPMessage *msg)
{
    unsigned short msglen = 4;

    msglen += msg->header.tokenlen;
    msglen += Cloud_CoAPSerialize_OptionsLen(msg);

    if(0 < msg->payloadlen){
        msglen += msg->payloadlen;
        msglen += 1; /*CoAP payload marker*/
    }

    return msglen;
}

int Cloud_CoAPSerialize_Message(Cloud_CoAPMessage *msg, unsigned char *buf, unsigned short buflen)
{
    unsigned char *ptr   = buf;
    unsigned short count = 0;
    unsigned short remlen  = buflen;

    if(NULL == buf || NULL == msg){
        return COAP_ERROR_INVALID_PARAM;
    }

    count = Cloud_CoAPSerialize_Header(msg, ptr, remlen);
    ptr += count;
    remlen -= count;

    count = Cloud_CoAPSerialize_Token(msg, ptr, remlen);
    ptr += count;
    remlen -= count;


    count = Cloud_CoAPSerialize_Options(msg, ptr, remlen);
    ptr += count;
    remlen -= count;

    count = Cloud_CoAPSerialize_Payload(msg, ptr, remlen);
    ptr += count;
    remlen -= count;

    return (buflen-remlen);
}
