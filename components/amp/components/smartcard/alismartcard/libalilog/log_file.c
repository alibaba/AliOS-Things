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
 * log_file.c: log file impl
 *
 */
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "log.h"
#include "aliyun_iot_smartcard_os_hal.h"

//default storage is 1024 KByte.
#ifndef DEFAULT_STORAGE_SIZE
#define DEFAULT_STORAGE_SIZE 1024
#endif

//default size of each log file is 128 KByte.
#ifndef DEFAULT_FILE_SIZE
#define DEFAULT_FILE_SIZE   128
#endif

unsigned long long g_total_size = DEFAULT_STORAGE_SIZE;
unsigned int g_single_size = DEFAULT_FILE_SIZE;
static unsigned long long g_cur_dir_size = 0;
char *g_dir_name = NULL;

extern void log_fs_add_content(const char *str);
extern void log_fs_destroy();
extern int log_fs_init();

static unsigned long long get_dir_size(cchar *dir)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    unsigned long long total_size = 0;
    char sub_dir[512];

    if (!dir || !(dp = opendir(dir))) {
        return 0;
    }

    if (-1 == lstat(dir, &statbuf)) {
        closedir(dp);
        return 0;
    }

    total_size += statbuf.st_size;

    while ((entry = readdir(dp)) != NULL) {
        snprintf(sub_dir, sizeof(sub_dir), "%s/%s", dir, entry->d_name);

        if (-1 == lstat(sub_dir, &statbuf)) {
            break;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            if (strcmp(".", entry->d_name) == 0 ||
                strcmp("..", entry->d_name) == 0) {
                continue;
            }

            total_size += get_dir_size(sub_dir);
        } else {
            total_size += statbuf.st_size;
        }
    }

    closedir(dp);
    return total_size;
}

static void create_dir(cchar *dir)
{
    char cmd[256] = {0};
    snprintf(cmd, sizeof(cmd), "mkdir -p %s", dir);
    system(cmd);

    if (g_dir_name) {
        HAL_Free(g_dir_name);
    }

    g_dir_name = (char *)HAL_Malloc(strlen(dir) + 2);

    if (!g_dir_name) {
        return;
    }

    memset(g_dir_name, 0, strlen(dir) + 2);
    strncpy(g_dir_name, dir, strlen(dir));
    strncat(g_dir_name, "/", 1);

    g_cur_dir_size = get_dir_size(g_dir_name);
}

static int remove_file(cchar *dir, cchar *file)
{
    char path[512] = {0};
    int ret = 0;

    if (!dir || !file || strlen(file) <= 0) {
        return -1;
    }

    snprintf(path, sizeof(path), "%s/%s", dir, file);
    ret = remove(path);
    return ret;
}

extern char *g_log_prefix;
extern const char *g_log_postfix;
static unsigned long get_log_utc(const char *file, unsigned long *index)
{
    if (!file) {
        return 0;
    }

    //format: .//log_sample_x/log_2018-02-05-20-21-42-4444444-4.txt
    char *tmp = NULL;
    unsigned long utc = 0;
    int i = 0;

    tmp = strstr(file, g_log_prefix);

    if (!tmp) {
        return 0;
    }

    while (tmp) {
        tmp = strchr(tmp, '-');

        if (!tmp) {
            break;
        }

        tmp += 1;
        i++;

        if (i == 6) {
            utc = atol(tmp);
        } else if (i == 7) {
            *index = atol(tmp);
            break;
        }
    }

    return utc;
}


static unsigned long long del_old_logs(cchar *dir)
{
    DIR *dp = NULL;
    struct dirent *entry;
    struct stat statbuf;
    unsigned long long size = 0;
    time_t min_date = 0x7fffffff;
    char del_file_name[512] = {0};
    char sub_dir[512];
    unsigned long long dir_size = 0;
    unsigned long index = 0;
    unsigned long last_index = 0;
    unsigned long utc = 0;

    if (!dir || !(dp = opendir(dir))) {
        HAL_Printf("failed to open dir: %s\n", dir);
        return 0;
    }

    if (-1 == lstat(dir, &statbuf)) {
        HAL_Printf("failed to stat dir: %s, msg: %s \n", dir, strerror(errno));
        closedir(dp);
        return 0;
    }

    while ((entry = readdir(dp)) != NULL) {
        snprintf(sub_dir, sizeof(sub_dir), "%s/%s", dir, entry->d_name);

        if (-1 == lstat(sub_dir, &statbuf)) {
            HAL_Printf("failed to stat dir: %s, msg: %s, return\n", dir, strerror(errno));
            closedir(dp);
            return 0;
        }

        if (!S_ISREG(statbuf.st_mode)) {
            continue;
        }

        dir_size += statbuf.st_size;
        //only delete file prefixed by "log_"
        utc = get_log_utc(entry->d_name, &index);

        //HAL_Printf("file: %s, timestamp: %ld, min_date: %ld, size: %ld\n",entry->d_name,utc,min_date,statbuf.st_size);
        if (strstr(entry->d_name, g_log_prefix)) {
            if (utc < min_date) {
                min_date = utc;
                last_index = index;
                size = statbuf.st_size;
                memset(del_file_name, 0, sizeof(del_file_name));
                strncpy(del_file_name, entry->d_name, sizeof(del_file_name) - 1);
            } else if (utc == min_date) {
                if (index < last_index) {
                    last_index = index;
                    size = statbuf.st_size;
                    memset(del_file_name, 0, sizeof(del_file_name));
                    strncpy(del_file_name, entry->d_name, sizeof(del_file_name) - 1);
                }
            }
        }
    }

    closedir(dp);

    return dir_size - (remove_file(g_dir_name, del_file_name) == 0 ? size : 0);
}

static int log_is_init = 0;

void log_file_print(int lvl, const char *str)
{
    if (!log_is_init) {
        return;
    }

    if (!str) {
        return;
    }

    log_fs_add_content(str);
    g_cur_dir_size += strlen(str) + 1;

    if (g_cur_dir_size >= g_total_size * 1024) {
        g_cur_dir_size = del_old_logs(g_dir_name);
    }
}

uint8_t log_file_init(cchar *dir, int total_size, int single_size)
{
    if (!dir) {
        HAL_Printf("log file init error: dir should not be NULL. \n");
        return -1;
    }

    if (total_size < single_size) {
        HAL_Printf("total size should be larger than single_size.\n");
        return -2;
    }

    create_dir(dir);
    g_total_size = total_size > 0 ? total_size : DEFAULT_STORAGE_SIZE;
    g_single_size = single_size > 0 ? single_size : DEFAULT_FILE_SIZE;

    log_fs_init();

    log_is_init = 1;
    HAL_Printf("log file init ok, total size: %lld KB, single file size: %d KB\n", g_total_size, g_single_size);
    return 0;
}

void log_file_destroy(void)
{
    if (g_dir_name) {
        HAL_Free(g_dir_name);
        g_dir_name = NULL;
    }

    log_fs_destroy();
    log_is_init = 0;
}

