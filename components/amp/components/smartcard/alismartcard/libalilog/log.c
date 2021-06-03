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
 * log.c: log impl
 *
 */
#include <stdarg.h>
#include <time.h>

#include "log.h"
#include "aliyun_iot_smartcard_os_hal.h"

#define MODULE_NAME_LEN 64
#define MAX_MSG_LEN     512*10

//func:line content
#define LOG_FMT_BRF_MIN "%s:%d "

//<tag> file-func:line content
#define LOG_FMT_BRF     "<%s> %s-%s:%d "

//date [module] level <tag> file-func:line content
#define LOG_FMT_VRB     "%s [%s] %s <%s> %s-%s:%d "

char             g_mode_name[MODULE_NAME_LEN] = "LOG";
static LOG_STORE_TYPE   g_store_type;
static LOG_MODE         g_out_mode = LOG_MOD_VERBOSE;
static uint8_t          g_log_lvl = LOG_LEVEL_DEBUG;
static bool             g_log_storage = false;
static const char       *g_log_desc[] = {"DBG", "INF",
                                         "WRN", "ERR",
                                         "FTL"
                                        };
extern void log_file_destroy(void);
extern void log_file_print(int lvl, const char *str);

void log_set_level(LOG_LEVEL level)
{
    g_log_lvl = level;
}

void log_get_level(LOG_LEVEL *level)
{
    *level = (LOG_LEVEL) g_log_lvl;
}

void log_set_storage(bool storage)
{
    g_log_storage = storage;
}

void log_get_storage(bool *storage)
{
    *storage = g_log_storage;
}

static char *get_timestamp(char *buf, int len, time_t cur_time)
{
    struct tm tm_time;

    localtime_r(&cur_time, &tm_time);

    snprintf(buf, len, "%04d-%02d-%02d %02d:%02d:%02d",
             1900 + tm_time.tm_year, 1 + tm_time.tm_mon,
             tm_time.tm_mday, tm_time.tm_hour,
             tm_time.tm_min, tm_time.tm_sec);
    return buf;
}

#define color_len_fin strlen(COL_DEF)
#define color_len_start strlen(COL_RED)
void log_print(LOG_LEVEL lvl, cchar *color, cchar *t, cchar *f, cchar *func, int l, cchar *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char buf[MAX_MSG_LEN] = {0};
    char *tmp = NULL;
    int len = 0;
    char buf_date[20] = {0};
    time_t cur_time = time(NULL);

    t = !t ? "\b" : t;
    func = !func ? "\b" : func;

    if (f) {
        f = strrchr(f, '/');

        if (f) {
            f = f + 1;
        }
    }

    f = !f ? "\b" : f;

    //add color support
    if (color) {
        snprintf(buf, MAX_MSG_LEN, "%s", color);
    }

    if (g_out_mode == LOG_MOD_VERBOSE)
        snprintf(buf + strlen(buf), MAX_MSG_LEN - strlen(buf), LOG_FMT_VRB,
                 get_timestamp(buf_date, 20, cur_time), g_mode_name,
                 g_log_desc[lvl], t, f, func, l);
    else if (g_out_mode == LOG_MOD_BRIEF) {
        snprintf(buf + strlen(buf), MAX_MSG_LEN - strlen(buf), LOG_FMT_BRF, t, f, func, l);
    } else if (g_out_mode == LOG_MOD_BRIEF_MIN) {
        snprintf(buf + strlen(buf), MAX_MSG_LEN - strlen(buf), LOG_FMT_BRF_MIN, func, l);
    }

    len = MAX_MSG_LEN - strlen(buf) - color_len_fin - 5;
    len = len <= 0 ? 0 : len;
    tmp = buf + strlen(buf);

    if (vsnprintf(tmp, len, fmt, ap) > len) {
        strncat(buf, "...\n", 4);
    }

    if (buf[strlen(buf) - 1] != '\n') {
        strncat(buf, "\n", 1);
    }

    if (color) {
        strncat(buf, COL_DEF, strlen(COL_DEF));
    }

#if 1

    if (lvl >= g_log_lvl) {
        HAL_Printf("%s", buf);
    }

#endif

    //we should cut the last color string before save to flash.
    if (color) {
        buf[strlen(buf) - color_len_fin] = '\0';
    }

    if (g_log_storage) {

        if (g_store_type >= LOG_FILE && lvl >= g_log_lvl) {
            log_file_print(lvl, buf + (color ? color_len_start : 0));
        }

    }

    va_end(ap);
}

int log_init(cchar *dir, cchar *name, LOG_STORE_TYPE type, LOG_MODE mode)
{
    int ret = 0;

    if (!name) {
        HAL_Printf("log init error: name should not be NULL\n");
        return -1;
    }

    memset(g_mode_name, 0, MODULE_NAME_LEN);
    snprintf(g_mode_name, MODULE_NAME_LEN, "%s", name);
    g_store_type = type;
    g_out_mode = mode;

    char file_path[MAX_MSG_LEN] = {0};

    if (g_log_storage) {

        if (dir) {
            snprintf(file_path, sizeof(file_path), "%s/%s", dir, name);
        } else {
            snprintf(file_path, sizeof(file_path), "%s", name);
        }

        if (g_store_type >= LOG_FILE) {
            ret = log_file_init(file_path, -1, -1);
        }

    }

    HAL_Printf("log init: module: %s, type: %d\n", name, type);
    return ret;
}

void log_destroy(void)
{
    if (g_log_storage) {
        if (g_store_type >= LOG_FILE) {
            log_file_destroy();
        }

    }
}

