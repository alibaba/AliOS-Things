/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

static char dec2hex(char c)
{
    if (0 <= c && c <= 9) {
        return c + '0';
    } else {
        if( 10 <= c && c <= 15 ) {
            return c + 'A' - 10;
        } else {
            return -1;
        }
    }
}

void uvoice_urlencode(char *url)
{
    int i = 0;
    int len = strlen( url );
    int res_len = 0;
    char *res = malloc(len * 3 + 1);
	memset(res, 0, len * 3 + 1);
    for (i = 0; i < len; ++i) {
        char c = url[i];
        if (('0' <= c && c <= '9') ||
			('a' <= c && c <= 'z') ||
			('A' <= c && c <= 'Z') ||
			c == '/' || c == '.') {
            res[res_len++] = c;
        } else {
            int j = c;
            if (j < 0)
                j += 256;
            int i1, i0;
            i1 = j / 16;
            i0 = j - i1 * 16;
            res[res_len++] = '%';
            res[res_len++] = dec2hex((char)i1);
            res[res_len++] = dec2hex((char)i0);
        }
    }
    res[res_len] = '\0';
    memcpy(url, res, res_len + 1);
    free(res);
}