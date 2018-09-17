/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
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

void    LITE_openlog(const char *ident);
void    LITE_closelog(void);
int     LITE_get_loglevel(void);
void    LITE_set_loglevel(int level);
int     LITE_hexdump(const char *title, const void *buf, const int len);

void    LITE_syslog_routine(char *m, const char *f, const int l, const int level, const char *fmt, va_list *params);
void    LITE_syslog(char *m, const char *f, const int l, const int level, const char *fmt, ...);

#define LOG_EMERG_LEVEL                 (0)     /* OS system is unavailable */
#define LOG_CRIT_LEVEL                  (1)     /* current application aborting */
#define LOG_ERR_LEVEL                   (2)     /* current app-module error */
#define LOG_WARNING_LEVEL               (3)     /* using default parameters */
#define LOG_INFO_LEVEL                  (4)     /* running messages */
#define LOG_DEBUG_LEVEL                 (5)     /* debugging messages */

#if (CONFIG_BLDTIME_MUTE_DBGLOG)
#define log_debug(mod, ...)
#else

#if (CONFIG_RUNTIME_LOG_LEVEL <= LOG_DEBUG_LEVEL)
#define log_debug(mod, ...)         LITE_syslog(mod, __FUNCTION__, __LINE__, LOG_DEBUG_LEVEL, __VA_ARGS__)
#else
#define log_debug(mod, ...)         LITE_printf("[dbg] "), LITE_printf(__VA_ARGS__), LITE_printf("\r\n")
#endif

#endif  /* CONFIG_BLDTIME_MUTE_DBGLOG */

#if (CONFIG_RUNTIME_LOG_LEVEL <= LOG_INFO_LEVEL)
#define log_info(mod, ...)          LITE_syslog(mod, __FUNCTION__, __LINE__, LOG_INFO_LEVEL, __VA_ARGS__)
#else
#define log_info(mod, ...)          LITE_printf("[inf] "), LITE_printf(__VA_ARGS__), LITE_printf("\r\n")
#endif

#if (CONFIG_RUNTIME_LOG_LEVEL <= LOG_WARNING_LEVEL)
#define log_warning(mod, ...)       LITE_syslog(mod, __FUNCTION__, __LINE__, LOG_WARNING_LEVEL, __VA_ARGS__)
#else
#define log_warning(mod, ...)       LITE_printf("[wrn] "), LITE_printf(__VA_ARGS__), LITE_printf("\r\n")
#endif

#if (CONFIG_RUNTIME_LOG_LEVEL <= LOG_ERR_LEVEL)
#define log_err(mod, ...)           LITE_syslog(mod, __FUNCTION__, __LINE__, LOG_ERR_LEVEL, __VA_ARGS__)
#else
#define log_err(mod, ...)           LITE_printf("[err] "), LITE_printf(__VA_ARGS__), LITE_printf("\r\n")
#endif

#if (CONFIG_RUNTIME_LOG_LEVEL <= LOG_CRIT_LEVEL)
#define log_crit(mod, ...)          LITE_syslog(mod, __FUNCTION__, __LINE__, LOG_CRIT_LEVEL, __VA_ARGS__)
#else
#define log_crit(mod, ...)          LITE_printf("[crt] "), LITE_printf(__VA_ARGS__), LITE_printf("\r\n")
#endif

#if (CONFIG_RUNTIME_LOG_LEVEL <= LOG_EMERG_LEVEL)
#define log_emerg(mod, ...)         LITE_syslog(mod, __FUNCTION__, __LINE__, LOG_EMERG_LEVEL, __VA_ARGS__)
#else
#define log_emerg(mod, ...)         LITE_printf("[emg] "), LITE_printf(__VA_ARGS__), LITE_printf("\r\n")
#endif

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

#define HEXDUMP_DEBUG(buf, len) \
    LITE_rich_hexdump(__func__, __LINE__, LOG_DEBUG_LEVEL, #buf, (const void *)buf, (const int)len)

#define HEXDUMP_INFO(buf, len)      \
    LITE_rich_hexdump(__func__, __LINE__, LOG_INFO_LEVEL, #buf, (const void *)buf, (const int)len)

#if defined(__cplusplus)
}
#endif
#endif  /* __LITE_LOG_H__ */
