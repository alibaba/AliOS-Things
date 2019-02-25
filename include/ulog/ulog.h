/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef ULOG_H
#define ULOG_H

#include "ulog_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_EMERG   0 /* system is unusable */
#define LOG_ALERT   1 /* action must be taken immediately */
#define LOG_CRIT    2 /* critical conditions */
#define LOG_ERR     3 /* error conditions */
#define LOG_WARNING 4 /* warning conditions */
#define LOG_NOTICE  5 /* normal, but significant, condition */
#define LOG_INFO    6 /* informational message */
#define LOG_DEBUG   7 /* debug-level message */
#define LOG_NONE    8 /* used in stop filter, all log will pop out */

typedef enum {
    AOS_LL_NONE  = LOG_ALERT,   /* disable log */
    AOS_LL_FATAL = LOG_CRIT,    /* fatal log will output */
    AOS_LL_ERROR = LOG_ERR,     /* fatal + error log will output */
    AOS_LL_WARN  = LOG_WARNING, /* fatal + warn + error log will output(default level) */
    AOS_LL_INFO  = LOG_INFO,    /* info + warn + error log will output */
    AOS_LL_DEBUG = LOG_DEBUG,   /* debug + info + warn + error + fatal log will output */
} aos_log_level_t;

/**
 * Module Type, used for distinguish funcion module of whole system.
 *
 */
typedef enum{
    MOD_KERNEL   = 0,  /* Log produced in kernel */
    MOD_SECURITY = 4,  /* Log produced in security and authority  */
    MOD_SYSLOG   = 5,  /* Log produced in syslog self */
    MOD_NET      = 7,  /* Log produced in Net, inlude lwip, bluetooth, CANopen, etc  */
    MOD_MID      = 16, /* Log produced in middleware, include uagent, udata, linkkit */
    MOD_USR1         , /* Log produced in user app */
    MOD_USR2         , /* Log produced in user app */
}MOD_TYPE;

#if SYNC_LOG_DETAILS

/*
 * Function prototype for syncronized log text, Recommed use below brief API instead of this
 *
 *
 * @param  s         Serverity of Log
 * @param  mod       Module name
 * @param  f         Usually File name
 * @param  l         Usually Line number of comment
 * @param  fmt, ...  Variable Parameter, support format print to log
 * @return  0: success; other: fail.
 */
int rt_log(const unsigned char s, const char* mod, const char* f, const unsigned long l, const char *fmt, ...);

/*
 * Log at the level set by aos_set_log_level().
 *
 * @param[in]  fmt  same as printf() usage.
 */
#define LOG(...) rt_log(LOG_EMERG, "", __FILE__, __LINE__, __VA_ARGS__)

#else
/*
 * Function prototype for syncronized log text, Recommed use below brief API instead of this
 *
 * @param  s         Serverity of Log
 * @param  mod       module name, shall not longer than MOD_MAX_LEN, or else be trimmed
 * @param  fmt, ...  Variable Parameter, support format print to log
 * @return  0: success; other: fail.
 */
#if SYNC_LOG_MOD
int rt_log(const unsigned char s, const char* mod, const char *fmt, ...);
#else
int rt_log(const unsigned char s, const char *fmt, ...);
#endif

/*
 * Log at the level set by aos_set_log_level().
 *
 * @param[in]  fmt  same as printf() usage.
 */
#if SYNC_LOG_MOD
#define LOG(...) rt_log(LOG_EMERG, "", __VA_ARGS__)
#else
#define LOG(...) rt_log(LOG_EMERG, __VA_ARGS__)
#endif

#endif

#ifdef CONFIG_LOGMACRO_SILENT
#define LOGF(mod, ...)
#define LOGE(mod, ...)
#define LOGW(mod, ...)
#define LOGI(mod, ...)
#define LOGD(mod, ...)
#else

#if SYNC_LOG_DETAILS

/*
 * Log at fatal level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  fmt  same as printf() usage.
 */
#define LOGF(mod, ...) rt_log(LOG_CRIT, mod, __FILE__, __LINE__, __VA_ARGS__)

/*
 * Log at error level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  fmt  same as printf() usage.
 */
#define LOGE(mod, ...) rt_log(LOG_ERR, mod, __FILE__, __LINE__, __VA_ARGS__)

/*
 * Log at warning level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  fmt  same as printf() usage.
 */
#define LOGW(mod, ...) rt_log(LOG_WARNING, mod, __FILE__, __LINE__,  __VA_ARGS__)

/*
 * Log at info level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  fmt  same as printf() usage.
 */
#define LOGI(mod, ...) rt_log(LOG_INFO, mod, __FILE__, __LINE__,  __VA_ARGS__)

/*
 * Log at debug level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  fmt  same as printf() usage.
 */
#ifdef DEBUG
#define LOGD(mod, ...) rt_log(LOG_DEBUG, mod, __FILE__, __LINE__,  __VA_ARGS__)
#else
#define LOGD(mod, ...)
#endif
#else

/*
 * Log at fatal level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  fmt  same as printf() usage.
 */
#if SYNC_LOG_MOD
#define LOGF(mod, ...) rt_log(LOG_CRIT, mod, __VA_ARGS__)
#else
#define LOGF(mod, ...) rt_log(LOG_CRIT, __VA_ARGS__)
#endif

/*
 * Log at error level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  fmt  same as printf() usage.
 */
#if SYNC_LOG_MOD
#define LOGE(mod, ...) rt_log(LOG_ERR, mod, __VA_ARGS__)
#else
#define LOGE(mod, ...) rt_log(LOG_ERR, __VA_ARGS__)
#endif

/*
 * Log at warning level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  fmt  same as printf() usage.
 */
#if SYNC_LOG_MOD
#define LOGW(mod, ...) rt_log(LOG_WARNING, mod, __VA_ARGS__)
#else
#define LOGW(mod, ...) rt_log(LOG_WARNING, __VA_ARGS__)
#endif

/*
 * Log at info level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  fmt  same as printf() usage.
 */
#if SYNC_LOG_MOD
#define LOGI(mod, ...) rt_log(LOG_INFO, mod, __VA_ARGS__)
#else
#define LOGI(mod, ...) rt_log(LOG_INFO, __VA_ARGS__)
#endif

/*
 * Log at debug level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  fmt  same as printf() usage.
 */
#ifdef DEBUG
#if SYNC_LOG_MOD
#define LOGD(mod, ...) rt_log(LOG_DEBUG, mod, __VA_ARGS__)
#else
#define LOGD(mod, ...) rt_log(LOG_DEBUG, __VA_ARGS__)
#endif

#else
#define LOGD(mod, ...)
#endif

#endif /* CONFIG_LOGMACRO_DETAILS */
#endif /* CONFIG_LOGMACRO_SILENT */

/**
 * Set the log level.
 *
 * @param[in]  log_level  level to be set,must be one of AOS_LL_NONE,AOS_LL_FATAL,AOS_LL_ERROR,AOS_LL_WARN,AOS_LL_INFO or AOS_LL_DEBUG.
 */
void aos_set_log_level(aos_log_level_t log_level);

/**
 * Function prototype for log text, Recommed use below brief API instead of this
 *
 * REMARK: call this function will cost at least 64+ULOG_CONFIG_SYSLOG_SIZE*2 in syslog mode
 *          64 as normal local variable cost, temp buffer of size ULOG_CONFIG_SYSLOG_SIZE
 *          is used to format syslog message, another ULOG_CONFIG_SYSLOG_SIZE cost because
 *          aos message queue used. In future a customized queue will be designed to reduce the cost.
 *          cost is less if select EXTREAM_LOG_TEXT in the case of same SYSLOG_SIZE as normal local variable cost reduce
 *
 *
 * @param  m    Module type, reference @MOD_TYPE
 * @param  s    Serverity of Log, choose from above value,
 * @param  f    Usually File name
 * @param  l    Usually Line number of comment
 * @param  fmt, ...  Variable Parameter, support format print to log
 * @return  0: success. use type int is align with aos_*
 */
int ulog (const MOD_TYPE m, const unsigned char s, const char* f, const unsigned long l, const char *fmt, ...);

/**
 * Function prototype for ayncronized ulog
 *
 *
 * @param MOD, only choose from enum MOD_TYPE
 * @param  S    Serverity of Log, choose from above value,
 * @param  ...  Variable Parameter, Log text try to log
 */
#define ULOGA(MOD, S, ...) ulog(MOD, S, __FILE__, __LINE__, __VA_ARGS__)

/**
 * Function prototype for log init
 *
 * @param host_name host name, like "SOC", character shall no more than 8 to save resouce
 */
extern void ulog_init(const char host_name[8]);

/**
 * Function prototype for ulog management
 *
 * @param cmd_str command string, user fill the command string to manage ulog, format
 *        shall as below:
 *        "tcpip on=1" or "tcpip off=1" to notice the tcpip feature is install or not, which
 *        have impact on the ulog pop out via udp session;
 *        "listen ip=XXX.XXX.XXX.XXX port=XXX" to notice the syslog listener's address
 *        and/or port, only passing "listen ip=XXX.XXX.XXX.XXX" if you keep the port.
 *        For more command string, you can refer as ulog_man_handler_service in ulog.c
 */
extern void ulog_man(const char *cmd_str);

#ifdef __cplusplus
}
#endif

#endif /* ULOG_H */
