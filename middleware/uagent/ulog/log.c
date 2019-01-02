/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "log_impl.h"
#include "aos/kernel.h"
#include "ulog/ulog.h"

unsigned int aos_log_level = AOS_LL_V_DEBUG | AOS_LL_V_INFO | AOS_LL_V_WARN |
                             AOS_LL_V_ERROR | AOS_LL_V_FATAL;
static aos_mutex_t log_mutex;

int log_get_mutex()
{
    return 0 == aos_mutex_lock(&log_mutex, AOS_WAIT_FOREVER);
}

void log_release_mutex()
{
    aos_mutex_unlock(&log_mutex);
}

#ifndef csp_printf
#ifdef _WIN32
// definition will allocate in hw.c
extern int csp_printf(const char *fmt, ...);
#else
__attribute__((weak)) int csp_printf(const char *fmt, ...)
{
    va_list args;
    int     ret;


    va_start(args, fmt);
    ret = vprintf(fmt, args);
    va_end(args);

    fflush(stdout);


    return ret;
}
#endif
#endif

void aos_set_log_level(aos_log_level_t log_level)
{
    unsigned int value = 0;

    switch (log_level) {
        case AOS_LL_NONE:
            value |= AOS_LL_V_NONE;
            break;
        case AOS_LL_DEBUG:
            value |= AOS_LL_V_DEBUG;
        case AOS_LL_INFO:
            value |= AOS_LL_V_INFO;
        case AOS_LL_WARN:
            value |= AOS_LL_V_WARN;
        case AOS_LL_ERROR:
            value |= AOS_LL_V_ERROR;
        case AOS_LL_FATAL:
            value |= AOS_LL_V_FATAL;
            break;
        default:
            break;
    }

    aos_log_level = value;
}

/* log init mutex */
void log_init_mutex(void)
{
    aos_mutex_new(&log_mutex);
}
