/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>

#include "http_string.h"

bool http_str_search(char *src, char *searched, uint32_t offset,
                     uint32_t scope, httpc_param_t *param)
{
    char *src_start_pos;
    char *dst_start_pos;
    char orig_char;
    uint32_t pos = 0;

    src_start_pos = src + offset;
    orig_char = src_start_pos[scope];

    src_start_pos[scope] = 0;

    dst_start_pos = strstr(src_start_pos, searched);
    if (dst_start_pos == NULL) {
        return false;
    }
    pos = dst_start_pos - src_start_pos + 1;

    src_start_pos[scope] = orig_char;

    if (pos == 0) {
        return false;
    }

    if (param) {
        param->param = src_start_pos;
        param->len = pos - 1;
    }

    return true;
}

bool http_str_insensitive_cmp(char *src, char *dest, uint32_t len)
{
    uint32_t pos = 0;
    uint32_t dest_len = strlen(dest);
    char *src_input = src;
    char *dest_input = dest;
    char tmp_a;
    char tmp_b;

    if (len == 0) {
        len = strlen(src);
    }

    if (dest_len != len) {
        return false;
    }

    while (src_input || dest_input) {
        if (len > 0 && pos == len) {
            return true;
        }
        tmp_a = *src_input;
        tmp_b = *dest_input;

        if (tmp_a >= 64 && tmp_b <= 90) {
            tmp_a += 32;
        }

        if (tmp_b >= 64 && tmp_b <= 90) {
            tmp_b += 32;
        }

        if (tmp_a != tmp_b) {
            return false;
        }

        src_input++;
        dest_input++;
        pos++;
    }

    return true;
}
