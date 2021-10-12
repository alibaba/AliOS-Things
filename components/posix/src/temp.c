/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define TEMP_PATH_MAX 64
#define TEMP_FILE_NAME_MAGIC "/tmp/du2s5sz3"

pthread_mutex_t g_tmpnam_lock = PTHREAD_MUTEX_INITIALIZER;

char *tmpnam(char *s)
{
    int ret;
    static int temp_name_series = 0;
    char *temp_name_prefix = TEMP_FILE_NAME_MAGIC;
    char  temp_name_series_buf[8] = {0};

    if (temp_name_series >= TMP_MAX) {
        return NULL;
    }

    ret = pthread_mutex_lock(&g_tmpnam_lock);
    if (ret != 0) {
        return NULL;
    }

    strncpy(s, temp_name_prefix, TEMP_PATH_MAX);
    snprintf(temp_name_series_buf, sizeof(temp_name_series_buf) - 1, "_%d", temp_name_series);
    strncat(s, temp_name_series_buf, TEMP_PATH_MAX - strlen(s) - 1);

    temp_name_series++;

    pthread_mutex_unlock(&g_tmpnam_lock);

    return s;
}

FILE *tmpfile(void)
{
    char  path[TEMP_PATH_MAX] = {0};

    if (tmpnam(path) == NULL) {
        return NULL;
    }

    return fopen(path, "w+");
}
