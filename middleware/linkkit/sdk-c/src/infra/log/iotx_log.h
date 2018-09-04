/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
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
 */


#ifndef __LITE_LOG_H__
#define __LITE_LOG_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "iotx_log_config.h"

typedef enum _LOGLEVEL {
    LOG_EMERG_LEVEL = 0,    /* OS system is unavailable */
    LOG_CRIT_LEVEL,         /* current application aborting */
    LOG_ERR_LEVEL,          /* current app-module error */
    LOG_WARNING_LEVEL,      /* using default parameters */
    LOG_INFO_LEVEL,         /* running messages */
    LOG_DEBUG_LEVEL,        /* debugging messages */
} LOGLEVEL;

void    LITE_openlog(const char *ident);
void    LITE_closelog(void);
int     LITE_log_enabled(void);
char   *LITE_get_logname(void);
int     LITE_get_loglevel(void);
void    LITE_set_loglevel(int level);
int     LITE_hexdump(const char *title, const void *buf, const int len);

void    LITE_syslog_routine(char *m, const char *f, const int l, const int level, const char *fmt, va_list *params);
void    LITE_syslog(char *m, const char *f, const int l, const int level, const char *fmt, ...);

extern  int mqtt_publish(char *topic, int qos, void *data, int len);
void    LITE_syslog_upload_to_cloud(const char *module, const int level, const char *fmt, ...);

#ifdef SDK_DEBUG_LEVEL_NONE
#define log_warning(mod ,...)
#define log_info(mod ,...)
#define log_debug(mod ,...)
#else
#define log_warning(mod ,...)            LITE_syslog(mod, __FUNCTION__, __LINE__, LOG_WARNING_LEVEL, __VA_ARGS__)
#define log_info(mod ,...)               LITE_syslog(mod, __FUNCTION__, __LINE__, LOG_INFO_LEVEL, __VA_ARGS__)
#define log_debug(mod ,...)              LITE_syslog(mod, __FUNCTION__, __LINE__, LOG_DEBUG_LEVEL, __VA_ARGS__)
#endif
#define log_emerg(mod ,...)              LITE_syslog(mod, __FUNCTION__, __LINE__, LOG_EMERG_LEVEL, __VA_ARGS__)
#define log_crit(mod ,...)               LITE_syslog(mod, __FUNCTION__, __LINE__, LOG_CRIT_LEVEL, __VA_ARGS__)
#define log_err(mod ,...)                LITE_syslog(mod, __FUNCTION__, __LINE__, LOG_ERR_LEVEL, __VA_ARGS__)
#define log_err_online(mod, ...)         LITE_syslog_post(mod, LOG_ERR_LEVEL, __VA_ARGS__)

int     log_multi_line_internal(const char *f, const int l,
                                const char *title, int level, char *payload, const char *mark);
#define log_multi_line(level, title, fmt, payload, mark) \
    log_multi_line_internal(__func__, __LINE__, title, level, payload, mark)

void    LITE_rich_hexdump(const char *f, const int l,
                          const int level,
                          const char *buf_str,
                          const void *buf_ptr,
                          const int buf_len
                         );

#if defined(__GLIBC__)
#define HEXDUMP_DEBUG(buf, len) \
    LITE_rich_hexdump(__func__, __LINE__, LOG_DEBUG_LEVEL, #buf, (const void *)buf, (const int)len)

#define HEXDUMP_INFO(buf, len)      \
    LITE_rich_hexdump(__func__, __LINE__, LOG_INFO_LEVEL, #buf, (const void *)buf, (const int)len)
#else
#define HEXDUMP_DEBUG(buf, len) \
    LITE_rich_hexdump(__func__, __LINE__, LOG_DEBUG_LEVEL, #buf, buf, len)

#define HEXDUMP_INFO(buf, len)      \
    LITE_rich_hexdump(__func__, __LINE__, LOG_INFO_LEVEL, #buf, buf, len)
#endif

#if defined(__cplusplus)
}
#endif
#endif  /* __LITE_LOG_H__ */
