/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

/* convert int to ascii(HEX)
   while using format % in libc, malloc/free is involved.
   this function avoid using malloc/free. so it works when heap corrupt. */
char *k_int2str(int num, char *str)
{
    char index[] = "0123456789ABCDEF";

    str[7] = index[num % 16];
    num /= 16;
    str[6] = index[num % 16];
    num /= 16;
    str[5] = index[num % 16];
    num /= 16;
    str[4] = index[num % 16];
    num /= 16;
    str[3] = index[num % 16];
    num /= 16;
    str[2] = index[num % 16];
    num /= 16;
    str[1] = index[num % 16];
    num /= 16;
    str[0] = index[num % 16];
    num /= 16;

    return str;
}

void k_err_proc(kstat_t err)
{
    if (g_err_proc != NULL) {
        g_err_proc(err);
    }
}

