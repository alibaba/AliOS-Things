/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "chip_config.h"
#include "chiplog.h"


void chip_log(const char *fmt, ...)
{
    va_list args;

    printf(" ID2=> ");
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

#if (CHIP_DEBUG)

#define COL_SIZE 0x10
void chip_log_hex_dump(const char *name, const uint8_t *in_data,
                       uint32_t in_len)
{
    uint32_t i;
    char     buf[80];
    int      pos;
    if (name) {
        chip_log_debug("%s [length = 0x%04X]\n", name, in_len);
    }
    i   = 0;
    pos = 0;
    memset(buf, 0x00, sizeof(buf));
    while (i < in_len) {
        pos += snprintf(buf + pos, sizeof(buf) - pos, "%02X ", in_data[i]);
        i++;
        if (i % COL_SIZE == 0x00) {
            pos += snprintf(buf + pos, sizeof(buf) - pos, "\n");
            chip_log_debug("%s", buf);
            pos = 0;
        } else if (i % COL_SIZE == (COL_SIZE >> 1)) {
            pos += snprintf(buf + pos, sizeof(buf) - pos, "  ");
        } else {
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

#endif
