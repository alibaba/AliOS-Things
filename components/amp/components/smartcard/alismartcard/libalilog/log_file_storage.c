/*
 * Copyright (c) 2019 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description:
 * log_file_storage.c: log file storage impl
 *
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "log.h"
#include "aliyun_iot_smartcard_os_hal.h"

#define MAX_PATH_LENGTH     128
#define MAX_FILENAME_LENGTH 512
#define MAX_FILE_BUF_SIZE   4096

static FILE         *g_fp = NULL;
static char         *g_file_cache_buf = NULL;
static int          g_file_cache_size = 0;
static int          g_flag_file_init = 0;
static void         *g_file_locker;
static int          g_log_file_size = 0;
static char         g_file_name[MAX_FILENAME_LENGTH] = {0};
static unsigned int g_log_index = 0;
char *g_log_prefix = "log_";
const char *g_log_postfix = ".txt";

extern char *g_dir_name;
extern unsigned int g_single_size;

static void save_last_file()
{
    int fd;

    if (g_fp) {
        fflush(g_fp);
        fd = fileno(g_fp);
        fsync(fd);
        fclose(g_fp);
        g_fp = NULL;
        g_log_file_size = 0;
    }
}

static char *gen_log_name(char *file_name, int len)
{
    if (!file_name || len <= 0) {
        return NULL;
    }

    time_t timep;
    struct tm now;

    time(&timep);
    localtime_r(&timep, &now);

    memset(file_name, 0, len);

    if (g_dir_name) {
        snprintf(file_name, len, "%s", g_dir_name);
    }

    if (g_log_prefix)
        snprintf(file_name + strlen(file_name),
                 len - strlen(file_name), "%s", g_log_prefix);

    //add time
    strftime(file_name + strlen(file_name),
             len - strlen(file_name), "%Y-%m-%d-%H-%M-%S", &now);

    if (g_log_postfix)
        snprintf(file_name + strlen(file_name),
                 len - strlen(file_name), "-%lu-%d%s", timep,
                 ++g_log_index, g_log_postfix);

    HAL_Printf("new file name:  %s\n", file_name);
    return file_name;
}

static void create_symlink(char *file_name)
{
    char cmd[512] = {0};
    char mod_dir[512] = {0};
    char *log_name = NULL;
    char *mod_name = NULL;
    char *p = NULL;

    strncpy(mod_dir, g_dir_name, sizeof(mod_dir) - 1);
    p = strrchr(mod_dir, '/');

    if (NULL != p) {
        *p = 0;
    }

    mod_name = strrchr(mod_dir, '/');

    if (NULL == mod_name) {
        mod_name = mod_dir;
    } else {
        mod_name = mod_name + 1;
    }

    log_name = strrchr(file_name, '/');

    if (NULL == log_name) {
        log_name = file_name;
    } else {
        log_name = log_name + 1;
    }

    snprintf(cmd, sizeof(cmd), "ln -sf %s %s/%s.INFO", log_name, g_dir_name, mod_name);
    system(cmd);
}

static int create_new_file()
{
    char *file_name = gen_log_name(g_file_name, sizeof(g_file_name));

    save_last_file();
    g_fp = fopen(file_name, "a+");

    if (!g_fp) {
        HAL_Printf("failed to create file: %s:%s\n", file_name, strerror(errno));
        return -1;
    }

    if ((g_file_cache_buf != NULL) && (g_file_cache_size > 0)) {
        setvbuf(g_fp, g_file_cache_buf, _IOFBF, g_file_cache_size);
    } else {
        setbuf(g_fp, NULL);
    }

    g_log_file_size = 0;
    create_symlink(file_name);

    return 0;
}

static void __add_content(const char *log)
{
    if (g_log_file_size + strlen(log) > g_single_size * 1024) {
        create_new_file();
    }

    if (g_fp) {
        fwrite(log, 1, strlen(log), g_fp);
        g_log_file_size += strlen(log);
    }
}

int log_fs_init()
{
    int ret = 0;

    if (g_flag_file_init == 1) {
        return 0;
    }

    g_file_locker = HAL_MutexCreate();

    if (g_file_locker == NULL) {
        HAL_Printf("log fs init failed.\n");
        return -1;
    }

    HAL_MutexLock(g_file_locker);
    ret = create_new_file();

    if (ret != 0) {
        HAL_MutexUnlock(g_file_locker);
        return -1;
    }

    g_flag_file_init = 1;
    HAL_MutexUnlock(g_file_locker);
    return 0;
}

void log_fs_add_content(const char *str)
{
    if (!str || (!g_flag_file_init && 0 != log_fs_init())) {
        return;
    }

    HAL_MutexLock(g_file_locker);
    __add_content(str);
    HAL_MutexUnlock(g_file_locker);
}

void log_fs_destroy()
{
    if (g_file_cache_buf) {
        HAL_Free(g_file_cache_buf);
        g_file_cache_buf = NULL;
    }

    if (g_flag_file_init != 1) {
        return;
    }

    HAL_MutexLock(g_file_locker);
    save_last_file();
    HAL_MutexUnlock(g_file_locker);
    HAL_MutexDestroy(g_file_locker);
    g_file_locker = NULL;
}

uint8_t log_file_set_cache_size(int buff_size)
{
    if (buff_size < 0) {
        HAL_Printf("input cache size is invalid.\n");
        return -1;
    }

    if (buff_size > MAX_FILE_BUF_SIZE) {
        g_file_cache_size = MAX_FILE_BUF_SIZE;
    } else {
        g_file_cache_size = buff_size;
    }

    if (g_file_cache_buf) {
        HAL_Free(g_file_cache_buf);
    }

    g_file_cache_buf = (char *)HAL_Malloc(g_file_cache_size);

    if (!g_file_cache_buf) {
        HAL_Printf("malloc file cache size failed.\n");
        return -1;
    }

    HAL_Printf("set log file cache size:%d.\n", g_file_cache_size);
    return 0;
}

