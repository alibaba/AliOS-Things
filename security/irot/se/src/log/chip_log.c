/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "chip_log.h"

#define COL_SIZE   0x10

#if defined(CONFIG_CHIP_DEBUG)

void chip_log_hex_dump(const char* name, const uint8_t* data, uint32_t len)
{
    uint32_t i;
    char buf[80];
    int pos;

    if (name) {
        chip_log_debug("%s [length = 0x%04x]\n", name, len);
    }
    i = 0;
    pos = 0;
    memset(buf, 0x00, sizeof(buf));
    while (i < len) {
        pos += ls_osa_snprintf(buf + pos, sizeof(buf) - pos, "%02X ", data[i]);
        i++;
        if (i % COL_SIZE == 0x00) {
            pos += ls_osa_snprintf(buf + pos, sizeof(buf) - pos, "\n");
            chip_log_debug("%s", buf);
            pos = 0;
        } else if (i % COL_SIZE == (COL_SIZE >> 1)) {
            pos += ls_osa_snprintf(buf + pos, sizeof(buf) - pos, "  ");
        }
    }

    if (pos > 0) {
        if (i % COL_SIZE == 0x00) {
            chip_log_debug("%s", buf);
        } else {
            chip_log_debug("%s\n", buf);
        }
    }
}

#else  /* CONFIG_CHIP_DEBUG */

void chip_log_hex_dump(const char* name, const uint8_t* data, uint32_t len)
{
    (void)name;
    (void)data;
    (void)len;

    return;
}

#endif /* CONFIG_CHIP_DEBUG */
