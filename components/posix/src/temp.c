/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <aos/kernel.h>

#define TEMP_PATH_MAX 64
#define TEMP_FILE_PREFIX "/data/tmp"

pthread_mutex_t g_tmpnam_lock = PTHREAD_MUTEX_INITIALIZER;

char *tmpnam(char *s)
{
    int ret;
    unsigned int seed;
    static int temp_name_series = 0;
    char *temp_name_prefix = TEMP_FILE_PREFIX;
    char  temp_name_series_buf[64] = {0};

    if (temp_name_series >= TMP_MAX) {
        return NULL;
    }

    seed = aos_now_ms() + random();
    srandom(seed);
    ret = pthread_mutex_lock(&g_tmpnam_lock);
    if (ret != 0) {
        return NULL;
    }
    temp_name_series++;
    snprintf(temp_name_series_buf, sizeof(temp_name_series_buf) - 1,
        "_%d%d%d", temp_name_series, random(), random());
    pthread_mutex_unlock(&g_tmpnam_lock);

    /* Temp file should be unpredictable names for security consideration. */
    strncpy(s, temp_name_prefix, TEMP_PATH_MAX);
    strncat(s, temp_name_series_buf, TEMP_PATH_MAX - strlen(s) - 1);

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
