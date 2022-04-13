
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "aos/kernel.h"
#include "ymodem_adapt.h"

#define YMODEM_OK          0
#define YMODEM_ERR         (-1)
#define YMODEM_FILE_TOOBIG (-2)

#define YMODEM_SOH 0x01
#define YMODEM_STX 0x02
#define YMODEM_EOT 0x04
#define YMODEM_ACK 0x06
#define YMODEM_NAK 0x15
#define YMODEM_CAN 0x18
#define YMODEM_CCHAR 'C'
#define SOH_DATA_LEN 128
#define STX_DATA_LEN 1024

#define UART_RECV_TIMEOUT 400000

#define YMODEM_STATE_INIT      0
#define YMODEM_STATE_WAIT_HEAD 1
#define YMODEM_STATE_WAIT_DATA 2
#define YMODEM_STATE_WAIT_END  3
#define YMODEM_STATE_WAIT_NEXT 4

#define YMODEM_MAX_CHAR_NUM    64
#define YMODEM_ERR_NAK_NUM     5

typedef struct {
    unsigned short crc;
} CRC16_CTX;

unsigned short update(unsigned short crcIn, unsigned char byte)
{
    unsigned int crc = crcIn;
    unsigned int in = byte | 0x100;
    do {
        crc <<= 1;
        in <<= 1;
        if (in & 0x100) {
            ++crc;
        }
        if (crc & 0x10000) {
            crc ^= 0x1021;
        }
    } while (!(in & 0x10000));
    return crc & 0xffffu;
}

void crc16_init(CRC16_CTX *inContext)
{
    inContext->crc = 0;
}

void crc16_update(CRC16_CTX *inContext, const unsigned char *inSrc, long inLen)
{
    const unsigned char *src = (const unsigned char *) inSrc;
    const unsigned char *srcEnd = src + inLen;
    while ( src < srcEnd ) {
        inContext->crc = update(inContext->crc, *src++);
    }
}

void crc16_final(CRC16_CTX *inContext, unsigned short *outResult)
{
    inContext->crc = update(inContext->crc, 0);
    inContext->crc = update(inContext->crc, 0);
    *outResult = inContext->crc & 0xffffu;
}

unsigned short crc16_computer(unsigned char *buf, long len)
{
    unsigned short patch_crc = 0;
    CRC16_CTX crc_ctx;
    crc16_init(&crc_ctx);
    crc16_update(&crc_ctx, buf, len);
    crc16_final(&crc_ctx, &patch_crc);
    return patch_crc;
}

unsigned int ymodem_str2int(char *buf, unsigned int buf_len)
{
    int type  = 10;
    int value = 0;
    int i     = 0;

    if((buf[0] == '0') && (buf[1] == 'x' || buf[1] == 'X')) {
        type = 16;
        buf += 2;
    }
    for (i = 0; i < buf_len; buf++, i++) {
        if(*buf == 0) {
            return value;
        }
        if (*buf >= '0' && *buf <= '9') {
            value = value * type + *buf - '0';
        } else {
            if(10 == type) {
                return value;
            }
            if (*buf >= 'A' && *buf <= 'F') {
                value = value * 16 + *buf - 'A' + 10;
            }
            else if (*buf >= 'a' && *buf <= 'f') {
                value = value * 16 + *buf - 'a' + 10;
            } else {
                return value;
            }
        }
    }
    return value;
}

unsigned int ymodem_recv_bytes(unsigned char *buffer, unsigned int nbytes, unsigned int timeout)
{
    int ret = 0;
    unsigned char c = 0;
    unsigned int  i = 0;
    unsigned int  t = 0;

    while ((i < nbytes) && (t < timeout)) {
        ret = uart_recv_byte(&c);
        if (1 == ret) {
            buffer[i] = c;
            i++;
        }
        t++;
    }
    return i;
}

int ymodem_data_head_parse(unsigned char data_type)
{
    int    i   = 0;
    int    ret = YMODEM_ERR;
    int    lp  = 0;
    unsigned int buf_len = 0;
    unsigned char *buffer = NULL;
    unsigned short crc    = 0;
    unsigned int   value  = 0;

    buf_len = ((YMODEM_SOH == data_type) ? SOH_DATA_LEN : STX_DATA_LEN) + 4;
    buffer  = aos_malloc(buf_len);
    memset(buffer, 0, buf_len);
    /* SOH HEAD */
    value = ymodem_recv_bytes(buffer, buf_len, UART_RECV_TIMEOUT);
    if( (buf_len != value)  || (0 != buffer[0]) || (0xFF != buffer[1]) ) {
        goto err_exit;
    }

    /* check CRC */
    crc = crc16_computer(&buffer[2], buf_len-4);
    if (((crc >> 8) != buffer[buf_len - 2]) || ((crc & 0xFF) != buffer[buf_len - 1])) {
        goto err_exit;
    }

    /* parse file name && file length */
    for(i = 2; i < buf_len - 2; i++) {
        if((0 == buffer[i]) && (0 == lp)) {
            lp = i + 1;
            continue;
        }

        if((0 == buffer[i]) && (0 != lp)) {
            /* from buffer[lp] to buffer[i] is file length ascii */
            value = ymodem_str2int((char *)&buffer[lp], i - lp);
            if (0 == value) {
                goto err_exit;
            }
            printf("\r\nfile name : %s\r\n", &buffer[2]);
            printf("\r\nfile size : %d\r\n", value);
            break;
        }
    }

    for (i = 2; i < buf_len - 4; i ++) {
        if (buffer[i] != 0) {
            ret = YMODEM_OK;
        }
    }

err_exit:
    aos_free(buffer);
    return ret;
}

int ymodem_data_parse(unsigned char data_type, unsigned char *addr)
{
    int ret = YMODEM_ERR;
    unsigned int buf_len = 0;
    unsigned short crc    = 0;
    unsigned int   value  = 0;
    unsigned char *buffer = NULL;
    static unsigned int data_len = 0;

    buf_len = ((YMODEM_SOH == data_type) ? SOH_DATA_LEN : STX_DATA_LEN) + 4;
    buffer = (unsigned char *)aos_malloc(buf_len);
    if (!buffer) {
        return ret;
    }
    memset(buffer, 0, buf_len);

    if(NULL == addr) {
        goto err_exit;
    }
    /* SOH HEAD */
    value = ymodem_recv_bytes(buffer, buf_len, UART_RECV_TIMEOUT * (buf_len / SOH_DATA_LEN));
    if ((buf_len != value) || (0xFF != buffer[0] + buffer[1])) {
        goto err_exit;
    }

    /* check CRC */
    crc = crc16_computer(&buffer[2], buf_len - 4U);
    if (((crc >> 8) != buffer[buf_len - 2]) || ((crc & 0xFF) != buffer[buf_len - 1])) {
        goto err_exit;
    }

    //test
    memcpy((addr + data_len), &buffer[2], buf_len - 4U);
    data_len += buf_len - 4;

    ret = YMODEM_OK;

err_exit :
    aos_free(buffer);
    return ret;
}

int ymodem_recv_file(unsigned char *addr)
{
    int i        = 0;
    int ret      = YMODEM_OK;
    int state    = 0;
    int end_flag = 0;
    unsigned char c     = 0;
    unsigned int  bytes = 0;

    /* send C */
    while (1) {
        if(state != YMODEM_STATE_INIT) {
            bytes = ymodem_recv_bytes(&c, 1, 5000);
        }

        switch (state)
        {
        case YMODEM_STATE_INIT: /* send 'C' */
            if(i % 500 == 0) {
                uart_send_byte(YMODEM_CCHAR);
            }
            state = YMODEM_STATE_WAIT_HEAD;
            break;

        case YMODEM_STATE_WAIT_HEAD: /* wait SOH */
            if(1 != bytes) {
                i ++;
                state = 0;
                break;
            }
            if(( YMODEM_SOH == c ) || ( YMODEM_STX == c )) {
                ret = ymodem_data_head_parse(c);
                if (ret == YMODEM_OK) {
                    uart_send_byte(YMODEM_ACK);
                    aos_msleep(100);
                    uart_send_byte(YMODEM_CCHAR);
                    state = YMODEM_STATE_WAIT_DATA;
                    break;
                } else {
                    /* end */
                    uart_send_byte(YMODEM_ACK);
                    aos_msleep(200);
                    if(end_flag == 1) {
                        ret = YMODEM_OK;
                    } else {
                        for(i = 0; i < YMODEM_ERR_NAK_NUM; i++) {
                            uart_send_byte(YMODEM_NAK);
                        }
                    }
                    return ret;
                }
            } else if (3 == c) {  /* ctrl+c abort ymodem */
                printf("Abort\n");
                return YMODEM_ERR;
            }
            break;
        case YMODEM_STATE_WAIT_DATA: /* receive data */
            if(1 == bytes) {
                if( (YMODEM_SOH == c) || (YMODEM_STX == c) ) {
                    ret = ymodem_data_parse(c, addr);
                    if (ret == YMODEM_OK) {
                        uart_send_byte(YMODEM_ACK);
                    }
                } else if( YMODEM_EOT == c ) {
                    uart_send_byte(YMODEM_NAK);
                    state = YMODEM_STATE_WAIT_END;
                }
            }
            break;
        case YMODEM_STATE_WAIT_END: /* receive end eot */
            if ((1 == bytes) && (YMODEM_EOT == c)) {
                uart_send_byte(YMODEM_ACK);
                i     = 0;
                state = YMODEM_STATE_INIT;
                end_flag = 1;
            }
            break;
        default:
            state = YMODEM_STATE_INIT;
            break;
        }
    }

    return YMODEM_OK;
}