/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "config.h"
#include "log.h"

#if (ID2_DEBUG)

#define COL_SIZE 0x10

void id2_log_hex_dump(const char* name, const uint8_t* in_data, uint32_t in_len)
{
    uint32_t i;
    char buf[80];
    int pos;
    if (name)
    {
        id2_log_debug("%s [length = 0x%04X]\n", name, in_len);
    }
    i = 0;
    pos = 0;
    memset(buf, 0x00, sizeof(buf));
    while (i < in_len)
    {
        pos += snprintf(buf + pos, sizeof(buf) - pos, "%02X ", in_data[i]);
        i++;
        if (i % COL_SIZE == 0x00)
        {
            pos += snprintf(buf + pos, sizeof(buf) - pos, "\n");
            id2_log_debug("%s", buf);
            pos = 0;
        }
        else if (i % COL_SIZE == (COL_SIZE >> 1))
        {
            pos += snprintf(buf + pos, sizeof(buf) - pos, "  ");
        }
        else
        {
        }
    }
    if (pos > 0)
    {
        if (i % COL_SIZE == 0x00)
        {
            id2_log_debug("%s", buf);
        }
        else
        {
            id2_log_debug("%s\n", buf);
        }
    }
}

#endif
