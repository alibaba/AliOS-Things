/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef ULOG_H
#define ULOG_H

#include "uring_fifo.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ulog follows syslog , like <131>Jan 01 00:00:01.180 SOC OS_helloworld.c[13]: helloworld
 *   PRI  |HEADER|MSG[TAG+Content]
 *   more detail please refer as BSD syslog.
 *
 * You can call ULOGOS(), ULOG() to log after you initialized the log function via ulog_init in initial.
 * You can config the ulog module in ulog_config.h
 * Resource cost: 1.5K Byte ROM, <0.5K static, 0.5K task stack depth,  2~8K FIFO for async log
 */

#define LOG_EMERG   0 /* system is unusable */
#define LOG_ALERT   1 /* action must be taken immediately */
#define LOG_CRIT    2 /* critical conditions */
#define LOG_ERR     3 /* error conditions */
#define LOG_WARNING 4 /* warning conditions */
#define LOG_NOTICE  5 /* normal, but significant, condition */
#define LOG_INFO    6 /* informational message */
#define LOG_DEBUG   7 /* debug-level message */
#define LOG_NONE    8 /* used in stop filter, all log will pop out */

/**
 * Function prototype for log text
 *
 * REMARK: Recommed use below brief API instead of this
 *
 * @param  M    Module type, reference @MOD_TYPE
 * @param  S    Serverity of Log, choose from above value,
 * @param  F    Usually File name
 * @param  L    Usually Line number of comment
 * @param  ...  Variable Parameter, Log text try to log
 */
#define ULOG(M, S, F, L, ...) \
            if(log_init && S<push_stop_filter_level) { \
                post_log(M, S, F, L, __VA_ARGS__ );    \
            }

#define ULOGOS(S, ...) ULOG(MOD_OS,S,__FILE__,__LINE__,__VA_ARGS__)

/**
 * Function prototype for log init
 *
 * @param host_name host name, like "SOC", character shall no more than 8 to save resouce
 */
extern void ulog_init(const uint8_t host_name[8], const uint16_t depth);

#ifdef __cplusplus
}
#endif

#endif /* ULOG_H */

