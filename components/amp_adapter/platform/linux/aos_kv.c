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

#define LEN 128
char tmp_key[LEN] = {0};
char tmp_val[LEN] = {0};

int aos_kv_set(const char *key, const void *value, int len, int sync)
{
    printf("[%s] \r\n", __FUNCTION__);
    printf("[key] %s [value] %s [len] %d\r\n", key, (char *)value, len);

    strncpy(tmp_key, key, strlen(key));
    strncpy(tmp_val, value, strlen(value));
    return 0;
}

int aos_kv_get(const char *key, void *buffer, int *buffer_len)
{
    printf("[%s] \r\n", __FUNCTION__);

    if(strcmp(key, tmp_key) == 0) {
        strcpy(buffer, tmp_val);
        printf("kv get : %s\r\n", (char *)buffer);
        *buffer_len = strlen((char *)buffer);
        printf("len : %d\r\n", *buffer_len);
        return 0;
    } else {
        printf("[%s] fail\r\n", __FUNCTION__);
        return -1;
    }
}

int aos_kv_del(const char *key)
{
    printf("[%s] \r\n", __FUNCTION__);
    memset(tmp_key, 0, LEN);
    memset(tmp_val, 0, LEN);
    return 0;
}
