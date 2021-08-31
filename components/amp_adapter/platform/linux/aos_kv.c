/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "amp_config.h"
#include "amp_platform.h"
#include "aos_system.h"
#include "aos/kv.h"
#include <stdio.h>
#include <sys/stat.h>

#define LEN 128
char tmp_key[LEN] = {0};
char tmp_val[LEN] = {0};

int aos_kv_set(const char *key, const void *value, int len, int sync)
{
    printf("[key] %s [value] %s [len] %d\r\n", key, (char *)value, len);

    FILE *fp = fopen(key, "wb");
    if (fp == NULL) {
        return -1;
    }
    fwrite(value, 1, len, fp);
    fclose(fp);
    return 0;
}

int aos_kv_get(const char *key, void *buffer, int *buffer_len)
{
    FILE *fp = fopen(key, "rb");
    if (fp == NULL) {
        return -1;
    }

    fread(buffer, *buffer_len, 1, fp);

    fclose(fp);
    return 0;
}

int aos_kv_del(const char *key)
{
    remove(key);
    return 0;
}
