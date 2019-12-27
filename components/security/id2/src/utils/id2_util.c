/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "id2_priv.h"

const static char* const string_table = "0123456789ABCDEF";

/* 0, 1, 2, ... -> "0123456789ABCDEF" */
int hex_to_char(uint8_t hex)
{
    int ch = -1;

    if (hex <= 0x0F) {
        ch = string_table[hex];
    }

    return ch;
}

/* "0123456789ABCDEF" -> 0, 1, 2, ... */
int char_to_hex(char c)
{
    int hex = -1;

    if (c >= '0' && c <= '9') {
        hex = c - '0';
    } else if (c >= 'a' && c <= 'f') {
        hex = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        hex = c - 'A' + 10;
    }

    return hex;
}

int hex_to_string(const uint8_t* in, uint32_t in_len, uint8_t* out, uint32_t* out_len)
{
    int32_t i;
    uint8_t temp;
    uint8_t high, low;

    for (i = in_len - 1; i >= 0; i--) {
        temp = in[i];
        high = (temp >> 4) & 0x0F;
        low  = (temp >> 0) & 0x0F;

        out[i * 2] = string_table[high];
        out[i * 2 + 1] = string_table[low];
    }

    *out_len = in_len * 2;

    return 0;
}

int string_to_hex(const uint8_t* in, uint32_t in_len, uint8_t* out, uint32_t* out_len)
{
    uint32_t i = 0;
    int high, low;

    if (in == NULL || out == NULL || in_len % 2 != 0) {
        return -1;
    }

    if (*out_len * 2 < in_len) {
        return -1;
    }

    while (i < in_len) {
        high = char_to_hex(in[i]);
        if (high < 0) {
            return -1;
        }

        low = char_to_hex(in[i + 1]);
        if (low < 0) {
            return -1;
        }

        out[i >> 1] = (uint8_t)((high << 4) | (low & 0x0F));
        i += 2;
    }

    *out_len = in_len >> 1;

    return 0;
}

#if defined(CONFIG_ID2_DEBUG)
void id2_log_hex_dump(const char* name, const uint8_t* data, uint32_t len)
{
    uint32_t i;
    char buf[80];
    int pos;
    int col_size = 16;

    if (name) {
        id2_log_debug("%s [length = 0x%04x]\n", name, len);
    }
    i = 0;
    pos = 0;
    memset(buf, 0x00, sizeof(buf));
    while (i < len) {
        pos += ls_osa_snprintf(buf + pos, sizeof(buf) - pos, "%02X ", data[i]);
        i++;
        if (i % col_size == 0x00) {
            pos += ls_osa_snprintf(buf + pos, sizeof(buf) - pos, "\n");
            id2_log_debug("%s", buf);
            pos = 0;
        } else if (i % col_size == (col_size >> 1)) {
            pos += ls_osa_snprintf(buf + pos, sizeof(buf) - pos, "  ");
        }
    }

    if (pos > 0) {
        if (i % col_size == 0x00) {
            id2_log_debug("%s", buf);
        } else {
            id2_log_debug("%s\n", buf);
        }
    }

    return;
}
#else
void id2_log_hex_dump(const char* name, const uint8_t* data, uint32_t len)
{
    (void)name;
    (void)data;
    (void)len;

    return;
}
#endif

