/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>

#include "http_string.h"

bool http_str_search(char *src, char *searched, uint32_t offset,
                     uint32_t scope, httpc_param_t *param)
{
    uint32_t i = 0;

    if (!src || !searched) {
        printf("%s invalid argument\r\n", __func__);
        return false;
    }

    if (scope < strlen(searched)) {
        printf("scope less than searched string length\r\n");
        return false;
    }

    while (i <= (scope - strlen(searched))) {
        if (memcmp(src + offset + i, searched, strlen(searched)) == 0) {
            if (param) {
                param->param = src + offset;
                param->len = i;
            }
            return true;
        }
        i++;
    }

    return false;
}

bool http_str_insensitive_cmp(char *src, char *dest, uint32_t len)
{
    uint32_t pos = 0;
    uint32_t dest_len = 0;
    char *src_input = src;
    char *dest_input = dest;
    char tmp_a;
    char tmp_b;

    if (src == NULL || dest == NULL) {
        return false;
    }

    dest_len = strlen(dest);

    if (len == 0) {
        len = strlen(src);
    }

    if (dest_len != len) {
        return false;
    }

    for (pos = 0; pos < len; pos++) {
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
    }

    return true;
}
