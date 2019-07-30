#ifndef _INFRA_LOG_H_
#define _INFRA_LOG_H_

#include <stdarg.h>
#include "linkkit/infra/infra_defs.h"
#include "linkkit/wrappers/wrappers.h"
#include <ulog/ulog.h>

#define LOG_NONE_LEVEL                  (0)     /* no log printed at all */
#define LOG_CRIT_LEVEL                  (1)     /* current application aborting */
#define LOG_ERR_LEVEL                   (2)     /* current app-module error */
#define LOG_WARNING_LEVEL               (3)     /* using default parameters */
#define LOG_INFO_LEVEL                  (4)     /* running messages */
#define LOG_DEBUG_LEVEL                 (5)     /* debugging messages */
#define LOG_FLOW_LEVEL                  (6)     /* code/packet flow messages */

#define log_multi_line(level, title, fmt, payload, mark)
#define HEXDUMP_DEBUG(buf, len)
#define HEXDUMP_INFO(buf, len)

#if defined(INFRA_LOG) && !defined(INFRA_LOG_ALL_MUTED)
    #define log_flow(mod, ...)

    #if defined(INFRA_LOG_MUTE_DBG)
        #define log_debug(mod, ...)
    #else
        #define log_debug(mod, ...)         ulog(LOG_DEBUG, mod, ULOG_TAG, __VA_ARGS__)
    #endif

    #if defined(INFRA_LOG_MUTE_INF)
        #define log_info(mod, ...)
    #else
        #define log_info(mod, ...)          ulog(LOG_INFO, mod, ULOG_TAG, __VA_ARGS__)
    #endif

    #if defined(INFRA_LOG_MUTE_WRN)
        #define log_warning(mod, ...)
    #else
        #define log_warning(mod, ...)       ulog(LOG_WARNING, mod, ULOG_TAG, __VA_ARGS__)
    #endif

    #if defined(INFRA_LOG_MUTE_ERR)
        #define log_err(mod, ...)
    #else
        #define log_err(mod, ...)           ulog(LOG_ERR, mod, ULOG_TAG, __VA_ARGS__)
    #endif

    #if defined(INFRA_LOG_MUTE_CRT)
        #define log_crit(mod, ...)
    #else
        #define log_crit(mod, ...)          ulog(LOG_CRIT, mod, ULOG_TAG, __VA_ARGS__)
    #endif
#else   /* #if defined(INFRA_LOG) */

    #define log_flow(mod, ...)
    #define log_debug(mod, ...)
    #define log_info(mod, ...)
    #define log_warning(mod, ...)
    #define log_err(mod, ...)
    #define log_crit(mod, ...)

#endif
#endif

