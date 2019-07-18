/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef ULOG_H
#define ULOG_H

#include "ulog_config.h"

/**
 * Important!!! Switch on NDEBUG will make no log produced(except use API ULOG),
 * it doesn't work even you try to increase log level.
 */
#ifdef NDEBUG
#define CONFIG_LOGMACRO_SILENT
#endif

#if SYNC_LOG_DETAILS
#define ULOG_TAG __FILE__, __LINE__
#else
#define ULOG_TAG "", 0
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
    AOS_LL_NONE  = LOG_EMERG,   /* disable log */
    AOS_LL_FATAL = LOG_CRIT,    /* fatal log will output */
    AOS_LL_ERROR = LOG_ERR,     /* fatal + error log will output */
    AOS_LL_WARN  = LOG_WARNING, /* fatal + warn + error log will output(default level) */
    AOS_LL_INFO  = LOG_INFO,    /* info + warn + error log will output */
    AOS_LL_DEBUG = LOG_DEBUG,   /* debug + info + warn + error + fatal log will output */
} aos_log_level_t;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Function prototype for syncronized log text, Recommed use below brief API LOGX
 *
 * @param  s         Serverity of Log
 * @param  mod       Module name
 * @param  f         Usually File name
 * @param  l         Usually Line number of comment
 * @param  fmt, ...  Variable Parameter, support format print to log
 * @return  0: success; other: fail.
 */
int ulog(const unsigned char s, const char *mod, const char *f,
         const unsigned long l, const char *fmt, ...);

/*
 * Log at the level set by aos_set_log_level().
 *
 * @param[in]  ...  same as printf() usage.
 */
#define LOG(...) ulog(LOG_ALERT, "AOS", ULOG_TAG, __VA_ARGS__)

#ifdef CONFIG_LOGMACRO_SILENT
#define LOGF(mod, ...)
#define LOGE(mod, ...)
#define LOGW(mod, ...)
#define LOGI(mod, ...)
#define LOGD(mod, ...)

#else /* !CONFIG_LOGMACRO_SILENT */
/*
 * Log at fatal level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  ...  same as printf() usage.
 */
#define LOGF(mod, ...) ulog(LOG_CRIT, mod, ULOG_TAG, __VA_ARGS__)

/*
 * Log at error level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  ...  same as printf() usage.
 */
#define LOGE(mod, ...) ulog(LOG_ERR, mod, ULOG_TAG, __VA_ARGS__)

/*
 * Log at warning level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  ...  same as printf() usage.
 */
#define LOGW(mod, ...) ulog(LOG_WARNING, mod, ULOG_TAG, __VA_ARGS__)

/*
 * Log at info level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  ...  same as printf() usage.
 */
#define LOGI(mod, ...) ulog(LOG_INFO, mod, ULOG_TAG, __VA_ARGS__)

/*
 * Log at debug level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  ...  same as printf() usage.
 */
#ifdef DEBUG
#define LOGD(mod, ...) ulog(LOG_DEBUG, mod, ULOG_TAG,  __VA_ARGS__)
#else
#define LOGD(mod, ...)
#endif

#endif /* CONFIG_LOGMACRO_SILENT */

/**
 * Set the log level.
 *
 * @param[in]  log_level  level to be set,must be one of AOS_LL_NONE,AOS_LL_FATAL,
 * AOS_LL_ERROR,AOS_LL_WARN,AOS_LL_INFO or AOS_LL_DEBUG.
 */
int aos_set_log_level(aos_log_level_t log_level);

/**
 * Function prototype for log init
 *
 * @param host_name host name, like "SOC", character shall no more than 8 to save resouce
 */
void ulog_init(void);

/**
 * Function prototype for ulog management
 *
 * @param cmd_str command string, user fill the command string to manage ulog, format
 *        shall as below:
 *        "tcpip on=1" or "tcpip off=1" to notice the tcpip feature is install or not,
 *        which have impact on the ulog pop out via udp session;
 *        "listen ip=XXX.XXX.XXX.XXX" to notice the syslog listener's address
 *        and/or port, only passing "listen ip=XXX.XXX.XXX.XXX" if you keep the port.
 *        For more command string, you can refer as ulog_man_handler_service in ulog.c
 * @return 0: Success, -EINVAL: invalid parameter, -EPERM: ulog module not initied yet,
 *        -EACCES: this function not work as there is no ASYNC method using. Other fail
 *        reason may come from kernel message queue.
 */
int ulog_man(const char *cmd_str);

/**
 * Function prototype for get ulog list recorded in file system.
 * Precondition of use this function is switch on ULOG_POP_FS_ENABLE
 *
 * @param buf  buffer received the context of file list
 * @param len  length of buffer
 * @return  0 sucessfully, -EINVAL param illegal, -ENOMEM not enough room to receive file
 *          list, -EIO system error.
 */
int aos_get_ulog_list(char *buf, const unsigned short len);

#ifdef __cplusplus
}
#endif

#endif /* ULOG_H */

