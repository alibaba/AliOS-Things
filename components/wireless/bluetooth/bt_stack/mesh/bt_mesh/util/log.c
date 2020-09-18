/* log.c - logging helpers */

/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Helper for printk parameters to convert from binary to hex.
 * We declare multiple buffers so the helper can be used multiple times
 * in a single printk call.
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/types.h>
#include <zephyr.h>
#include <misc/util.h>
//#include <hci.h>

#include "include/common/log.h"

#if /*defined(CONFIG_BT_DEBUG) && */defined(USE_BT_MESH_CUSTOM_LOG)
const char *bt_hex(const void *buf, size_t len)
{
    static const char hex[] = "0123456789abcdef";
    static char hexbufs[4][129];
    static u8_t curbuf;
    const u8_t *b = buf;
    unsigned int mask;
    char *str;
    int i;

    mask = irq_lock();
    str = hexbufs[curbuf++];
    curbuf %= ARRAY_SIZE(hexbufs);
    irq_unlock(mask);

    len = min(len, (sizeof(hexbufs[0]) - 1) / 2);

    for (i = 0; i < len; i++) {
        str[i * 2]     = hex[b[i] >> 4];
        str[i * 2 + 1] = hex[b[i] & 0xf];
    }

    str[i * 2] = '\0';

    return str;
}
#else
const char *bt_hex(const void *buf, size_t len)
{
#if 0
	static const char hint_str[] = "N/A";
	return hint_str;
#else
    static const char hex[] = "0123456789abcdef";
    static char hexbufs[4][129];
    static u8_t curbuf;
    const u8_t *b = buf;
    unsigned int mask;
    char *str;
    int i;

    mask = irq_lock();
    str = hexbufs[curbuf++];
    curbuf %= ARRAY_SIZE(hexbufs);
    irq_unlock(mask);

    len = min(len, (sizeof(hexbufs[0]) - 1) / 2);

    for (i = 0; i < len; i++) {
        str[i * 2]     = hex[b[i] >> 4];
        str[i * 2 + 1] = hex[b[i] & 0xf];
    }

    str[i * 2] = '\0';

    return str;
#endif
}
#endif

void dump_print(char *p, size_t len)
{
    size_t i = 0;

    BT_PRT("===============================================\n");
    while(i < len) {
        BT_PRT("%02X ", (u8_t)p[i++]);
        if(i%16 == 0) BT_PRT("\n");
    }
    if(i%16 != 0) BT_PRT("\n");
    BT_PRT("===============================================\n");
}

u8_t stringtohex(char *str, u8_t *out, u8_t count)
{
    u8_t i = 0, j = 0;
    u8_t n = 0;

    memset(out , 0, count);
    if(strlen(str) != count<<1)
    {
        BT_PRT("Lancelot string size error, %d\r\n", strlen(str));
        return 0;
    }
    
    while(i < count)
    {
        while(j < 2)
        {
            n = i*2+j;
            if(str[n] >= 'a' && str[n] <= 'f')
            {
                out[i] = out[i]<<4 | (str[n] - 'a' + 10);
            }
            else if(str[n] >= 'A' && str[n] <= 'F')
            {
                out[i] = out[i]<<4 | (str[n] - 'A' + 10);
            }
            else if(str[n] >= '0' && str[n] <= '9')
            {
                out[i] = out[i]<<4 | (str[n] - '0');
            }
            else
            {
                BT_PRT("Lancelot string error, %c\r\n", str[n]);
                return 0;
            }
            j++;
        }
        //BT_PRT(">>%02x %d %d\r\n", out[i], i ,j);
        j = 0;
        i++;
    }
    return count;
}

void hextostring(const uint8_t *source, char *dest, int len)
{
    int i;
    char tmp[3];

    for (i = 0; i < len; i++) {
        sprintf(tmp, "%02x", (unsigned char)source[i]);
        memcpy(&dest[i * 2], tmp, 2);
    }
}

#if defined(CONFIG_BT_DEBUG)
const char *bt_addr_str(const bt_addr_t *addr)
{
    static char bufs[2][BT_ADDR_STR_LEN];
    static u8_t cur;
    char *str;

    str = bufs[cur++];
    cur %= ARRAY_SIZE(bufs);
    bt_addr_to_str(addr, str, sizeof(bufs[cur]));

    return str;
}

const char *bt_addr_le_str(const bt_addr_le_t *addr)
{
    static char bufs[2][BT_ADDR_LE_STR_LEN];
    static u8_t cur;
    char *str;

    str = bufs[cur++];
    cur %= ARRAY_SIZE(bufs);
    bt_addr_le_to_str(addr, str, sizeof(bufs[cur]));

    return str;
}
#endif /* CONFIG_BT_DEBUG */

