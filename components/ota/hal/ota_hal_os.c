/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include "aos/kernel.h"
#include "ota_log.h"
#include "ota_hal_os.h"

#ifdef OTA_LINUX
#include <unistd.h>
#include <pthread.h>
#include <sys/reboot.h>
#else
#include "aos/kernel.h"
#endif

/*Memory realloc*/
void *ota_realloc(void *ptr, int size)
{
#if !defined OTA_LINUX
    return aos_realloc(ptr, size);
#else
    return realloc(ptr, size);
#endif
}

/*Memory calloc*/
void *ota_calloc(int n, int size)
{
#if !defined OTA_LINUX
    return aos_calloc(n, size);
#else
    return calloc(n, size);
#endif
}

/*Reboot*/
void ota_reboot(void)
{
#if !defined OTA_LINUX
    aos_reboot();
#else
    reboot(0x1234567);
#endif
}

/*Memory malloc*/
void *ota_malloc(int size)
{
#if !defined OTA_LINUX
    return aos_malloc(size);
#else
    return malloc(size);
#endif
}

/*Memory free*/
void ota_free(void *ptr)
{
#if !defined OTA_LINUX
    aos_free(ptr);
#else
    free(ptr);
#endif
}

/*Sleep ms*/
void ota_msleep(int ms)
{
#if !defined OTA_LINUX
    aos_msleep(ms);
#else
    usleep(1000 * ms);
#endif
}

int ota_thread_create(void **thread_handle, void *(*work_routine)(void *), void *arg, void *pm, int stack_size)
{
    return 0;
}

void ota_thread_destroy(void *ptread)
{
    aos_task_exit(0);
}

/*Strings*/
int ota_to_capital(char *value, int len)
{
    int i = 0;
    int ret = -1;
    if ((value != NULL) && (len > 0)) {
        ret = 0;
        for (; i < len; i++) {
            if (*(value + i) >= 'a' && *(value + i) <= 'z') {
                *(value + i) -= 'a' - 'A';
            }
        }
    }
    return ret;
}

int ota_hex2str(char *dest_buf, const unsigned char *src_buf, unsigned int dest_len, unsigned int src_len)
{
    int i = 0;
    int ret = -1;
    if ((dest_buf != NULL) && (src_buf != NULL) && (dest_len > 2 * src_len)) {
        ret = 0;
        memset(dest_buf, 0x00, dest_len);
        for (i = 0; i < src_len; i++) {
            ota_snprintf(dest_buf + i * 2, 2 + 1, "%02X", src_buf[i]);
        }
    }
    return ret;
}

int ota_str2hex(const char *src, char *dest, unsigned int dest_len)
{
    int i, n = 0;
    int ret = -1;
    if ((src != NULL) && (dest != NULL) && (strlen(src) % 2 == 0) && (dest_len >= strlen(src) / 2)) {
        ret = 0;
        for (i = 0; src[i]; i += 2) {
            if (src[i] >= 'A' && src[i] <= 'F') {
                dest[n] = src[i] - 'A' + 10;
            } else {
                dest[n] = src[i] - '0';
            }
            if (src[i + 1] >= 'A' && src[i + 1] <= 'F') {
                dest[n] = (dest[n] << 4) | (src[i + 1] - 'A' + 10);
            } else {
               dest[n] = (dest[n] << 4) | (src[i + 1] - '0');
            }
            ++n;
        }
    }
    return ret;
}

/*CRC16*/
static unsigned short update_crc16(unsigned short crcIn, unsigned char byte)
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

void ota_crc16_init(ota_crc16_ctx *inCtx)
{
    inCtx->crc = 0;
}

void ota_crc16_update(ota_crc16_ctx *inCtx, const void *inSrc, unsigned int inLen)
{
    const unsigned char *src = (const unsigned char *) inSrc;
    const unsigned char *srcEnd = src + inLen;
    while (src < srcEnd) {
        inCtx->crc = update_crc16(inCtx->crc, *src++);
    }
}

void ota_crc16_final(ota_crc16_ctx *inCtx, unsigned short *outResult)
{
    inCtx->crc = update_crc16(inCtx->crc, 0);
    inCtx->crc = update_crc16(inCtx->crc, 0);
    *outResult = inCtx->crc & 0xffffu;
}

unsigned short ota_get_data_crc16(const unsigned char *buf, unsigned int len)
{
    ota_crc16_ctx ctx;
    unsigned short crc16 = 0xffff;
    if ((buf != NULL) && (len > 0)) {
        ota_crc16_init(&ctx);
        ota_crc16_update(&ctx, buf, len);
        ota_crc16_final(&ctx, &crc16);
    }
    return crc16;
}

/*base64*/
static const unsigned char base64_dec_map[128] = {
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127,  62, 127, 127, 127,  63,  52,  53,
     54,  55,  56,  57,  58,  59,  60,  61, 127, 127,
    127,  64, 127, 127, 127,   0,   1,   2,   3,   4,
      5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
     15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
     25, 127, 127, 127, 127, 127, 127,  26,  27,  28,
     29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
     39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
     49,  50,  51, 127, 127, 127, 127, 127
};

int ota_base64_decode(const unsigned char *src, unsigned int slen, unsigned char *dst, unsigned int *dlen)
{
    unsigned int i, n;
    unsigned int j, x;
    unsigned char *p;

    for (i = n = j = 0; i < slen; i++) {
        if ((slen - i) >= 2 &&
            src[i] == '\r' && src[i + 1] == '\n')
            continue;

        if (src[i] == '\n')
            continue;

        if (src[i] == '=' && ++j > 2)
            return -1;

        if (src[i] > 127 || base64_dec_map[src[i]] == 127)
            return -1;

        if (base64_dec_map[src[i]] < 64 && j != 0)
            return -1;

        n++;
    }

    if (n == 0)
        return 0;

    n = ((n * 6) + 7) >> 3;
    n -= j;

    if (dst == 0 || *dlen < n) {
        *dlen = n;
        return -2;
    }

    for (j = 3, n = x = 0, p = dst; i > 0; i--, src++) {
        if (*src == '\r' || *src == '\n')
            continue;

        j -= (base64_dec_map[*src] == 64);
        x = (x << 6) | (base64_dec_map[*src] & 0x3F);

        if (++n == 4) {
            n = 0;
            if (j > 0)
                *p++ = (unsigned char)(x >> 16);
            if (j > 1)
                *p++ = (unsigned char)(x >> 8);
            if (j > 2)
                *p++ = (unsigned char)(x);
        }
    }

    *dlen = p - dst;
    return 0;
}