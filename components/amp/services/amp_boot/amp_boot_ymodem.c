
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "amp_config.h"
#include "amp_defines.h"
#include "aos_system.h"
#include "amp_boot_recovery.h"
#include "amp_boot_uart.h"

#define MOD_STR "AMP_YMODEM"
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

#define UART_RECV_TIMEOUT 4000000

#define YMODEM_STATE_INIT      0
#define YMODEM_STATE_WAIT_HEAD 1
#define YMODEM_STATE_WAIT_DATA 2
#define YMODEM_STATE_WAIT_END  3
#define YMODEM_STATE_WAIT_NEXT 4

#define YMODEM_MAX_CHAR_NUM    64
#define YMODEM_ERR_NAK_NUM     5

static unsigned int ymodem_flash_addr     = 0;
static unsigned int ymodem_flash_size     = 0;
static unsigned int ymodem_max_write_size = 40 * 1024 * 1024;

#define uart_send_byte amp_boot_uart_send_byte
#define uart_recv_byte amp_boot_uart_recv_byte

typedef void (*ymodem_write_t)(unsigned char *, int);

static ymodem_write_t ymodem_write = NULL;

extern void aos_boot_delay(uint32_t ms);

typedef struct {
    uint16_t crc;
} CRC16_Context;

static uint16_t UpdateCRC16(uint16_t crcIn, uint8_t byte)
{
    uint32_t crc = crcIn;
    uint32_t in = byte | 0x100;

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

static void CRC16_Init( CRC16_Context *inContext )
{
    inContext->crc = 0;
}

static void CRC16_Update( CRC16_Context *inContext, const void *inSrc, size_t inLen )
{
    const uint8_t *src = (const uint8_t *) inSrc;
    const uint8_t *srcEnd = src + inLen;
    while ( src < srcEnd ) {
        inContext->crc = UpdateCRC16(inContext->crc, *src++);
    }
}

static void CRC16_Final( CRC16_Context *inContext, uint16_t *outResult )
{
    inContext->crc = UpdateCRC16(inContext->crc, 0);
    inContext->crc = UpdateCRC16(inContext->crc, 0);
    *outResult = inContext->crc & 0xffffu;
}

static unsigned short crc16_computer(void *addr, size_t len)
{
    unsigned short crc = 0;
    CRC16_Context crc_context;
    CRC16_Init(&crc_context);
    CRC16_Update(&crc_context, addr, len);
    CRC16_Final(&crc_context, &crc);
    return crc;
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

    amp_debug(MOD_STR, "ymodem_data_head_parse\n");
    buf_len = ((YMODEM_SOH == data_type) ? SOH_DATA_LEN : STX_DATA_LEN) + 4;
    buffer  = aos_malloc(buf_len);
    memset(buffer, 0, buf_len);
    /* SOH HEAD */
    value = ymodem_recv_bytes(buffer, buf_len, UART_RECV_TIMEOUT);
    if( (buf_len != value)  || (0 != buffer[0]) || (0xFF != buffer[1]) ) {
        amp_debug(MOD_STR, "header error: %d %02x %02x\n", buf_len, buffer[0], buffer[1]);
        goto err_exit;
    }

    amp_debug(MOD_STR, "ymodem_recv_bytes head done, buf_len:%d\n", buf_len);
    /* check CRC */
    crc = crc16_computer(&buffer[2], buf_len-4);
    if (((crc >> 8) != buffer[buf_len - 2]) || ((crc & 0xFF) != buffer[buf_len - 1])) {
        amp_debug(MOD_STR, "header crc error\n");
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
            ymodem_flash_size = value;
            if(value > ymodem_max_write_size) {
                ret = YMODEM_FILE_TOOBIG;
                goto err_exit;
            }
            break;
        }
    }

    for (i = 2; i < buf_len - 4; i ++) {
        if (buffer[i] != 0) {
            ret = YMODEM_OK;
        }
    }
    amp_debug(MOD_STR, "get file size:%d\n", ymodem_flash_size);

err_exit:
    aos_free(buffer);
    return ret;
}

int ymodem_data_parse(unsigned char data_type)
{
    int ret = YMODEM_ERR;
    unsigned int buf_len = 0;
    unsigned short crc    = 0;
    unsigned int   value  = 0;
    unsigned char *buffer = NULL;

    buf_len = ((YMODEM_SOH == data_type) ? SOH_DATA_LEN : STX_DATA_LEN) + 4;
    buffer = aos_malloc(buf_len);
    memset(buffer, 0, buf_len);

    amp_debug(MOD_STR, "ymodem_data_parse\n");
    /* SOH HEAD */
    value = ymodem_recv_bytes(buffer, buf_len, UART_RECV_TIMEOUT);
    if ((buf_len != value) || (0xFF != buffer[0] + buffer[1])) {
        amp_error(MOD_STR, "ymodem_data_parse crc error:%02x %02x\n", buffer[buf_len - 2], buffer[buf_len - 1] );
        goto err_exit;
    }

    /* check CRC */
    crc = crc16_computer(&buffer[2], buf_len - 4);
    if (((crc >> 8) != buffer[buf_len - 2]) || ((crc & 0xFF) != buffer[buf_len - 1])) {
        goto err_exit;
    }

    /* write data fo flash */
    amp_debug(MOD_STR, "write data, buf_len:%d\n", buf_len - 4);
    if (ymodem_write != NULL) {
        ymodem_write(&buffer[2], buf_len - 4);
    }
    // ymodem_write_data_to_flash(&buffer[2], *addr, buf_len - 4);
    // *addr += buf_len - 4;
    ret = YMODEM_OK;

err_exit :
    aos_free(buffer);
    return ret;
}

int ymodem_recv_file(void)
{
    int i        = 0;
    int ret      = YMODEM_OK;
    int state    = 0;
    int end_flag = 0;
    unsigned char c     = 0;
    unsigned int  bytes = 0;

    /* send C */
    while (1) {
        aos_boot_delay(1);
        if(state != YMODEM_STATE_INIT) {
            bytes = ymodem_recv_bytes(&c, 1, 50000);
        }
        //aos_printf("ymodem_recv_file bytes = %d, i = %d, state = %d\n", bytes, i, state);
        //amp_debug_send_str("ymodem_recv_file \n");
        switch (state)
        {
        case YMODEM_STATE_INIT: /* send 'C' */
            if(i % 20 == 0) {
                uart_send_byte(YMODEM_CCHAR);
            }
            state = YMODEM_STATE_WAIT_HEAD;
            break;

        case YMODEM_STATE_WAIT_HEAD: /* wait SOH */
            if(1 != bytes) {
                i ++;
                state = YMODEM_STATE_INIT;
                break;
            }
            if(( YMODEM_SOH == c ) || ( YMODEM_STX == c )) {
                ret = ymodem_data_head_parse(c);
                if (ret == YMODEM_OK) {
                    uart_send_byte(YMODEM_ACK);
                    aos_boot_delay(100);
                    uart_send_byte(YMODEM_CCHAR);
                    state = YMODEM_STATE_WAIT_DATA;
                    break;
                } else {
                    /* end */
                    uart_send_byte(YMODEM_ACK);
                    aos_boot_delay(200);
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
                amp_debug(MOD_STR, "Abort\n");
                return YMODEM_ERR;
            }
            break;

        case YMODEM_STATE_WAIT_DATA: /* receive data */
            if(1 == bytes) {
                if( (YMODEM_SOH == c) || (YMODEM_STX == c) ) {
                    ret = ymodem_data_parse(c);
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

int ymodem_upgrade(void (*func)(unsigned char *, int))
{
    int    i = 0;
    int  ret = 0;
    int flag = 0;
    unsigned char c   = 0;
    char buf[YMODEM_MAX_CHAR_NUM];

    ymodem_write = func;

    amp_debug(MOD_STR, "Please start ymodem ... (press ctrl+c to cancel)\n");

    ret = ymodem_recv_file();
    if (ret != YMODEM_OK) {
        for(i = 0; i < 5000; i++ ) {
            if(uart_recv_byte(&c)) {
                break;
            }
        }
    }

    if(ret == YMODEM_OK) {
        amp_debug(MOD_STR, "Recv App Bin OK\n");
    } else if(ret == YMODEM_FILE_TOOBIG) {
        amp_debug(MOD_STR, "file too big len:0x%08x !!!\n", ymodem_flash_size);
    } else {
        amp_debug(MOD_STR, "Ymodem recv file Err:%d !!!\n", ret);
    }
    return ret;
}
