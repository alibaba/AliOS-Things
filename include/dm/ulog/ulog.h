/**
 * @file ulog.h
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef ULOG_H
#define ULOG_H

/** @addtogroup aos_ulog ulog
 *  Optional policy of log, which can output into default direction, virtual file 
 *  system, mqtt channel and syslog udp.
 *
 *  @{
 */

/**
 * Important!!! Switch on NDEBUG will make no log produced(except use API LOG),
 * it doesn't work even you try to increase log level.
 */
#ifdef NDEBUG
#define CONFIG_LOGMACRO_SILENT
#endif

#define ULOG_TAG __FILE__, __LINE__

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

/**
 * Function prototype for syncronized log text, Recommed using below brief API LOGX instead of this.
 *
 * @param[in]  s         Serverity of Log
 * @param[in]  mod       Module name
 * @param[in]  f         Usually File name
 * @param[in]  l         Usually Line number of comment
 * @param[in]  fmt, ...  Variable Parameter, support format print to log
 * 
 * @return  0 on success, negative error on failure.
 */
int ulog(const unsigned char s, const char *mod, const char *f,
         const unsigned long l, const char *fmt, ...);

/**
 * Log at the alert level, brief using of ulog.
 *
 * @param[in]  ...  same as printf() usage.
 * 
 * @return  0 on success, negative error on failure.
 */
#define LOG(...) ulog(LOG_ALERT, "AOS", ULOG_TAG, __VA_ARGS__)

#ifdef CONFIG_LOGMACRO_SILENT
#define LOGF(mod, ...)
#define LOGE(mod, ...)
#define LOGW(mod, ...)
#define LOGI(mod, ...)
#define LOGD(mod, ...)

#else /* !CONFIG_LOGMACRO_SILENT */
/**
 * Log at fatal level, brief using of ulog.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  ...  same as printf() usage.
 * 
 * @return  0 on success, negative error on failure.
 */
#define LOGF(mod, ...) ulog(LOG_CRIT, mod, ULOG_TAG, __VA_ARGS__)

/**
 * Log at error level, brief using of ulog.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  ...  same as printf() usage.
 * 
 * @return  0 on success, negative error on failure.
 */
#define LOGE(mod, ...) ulog(LOG_ERR, mod, ULOG_TAG, __VA_ARGS__)

/**
 * Log at warning level, brief using of ulog.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  ...  same as printf() usage.
 * 
 * @return  0 on success, negative error on failure.
 */
#define LOGW(mod, ...) ulog(LOG_WARNING, mod, ULOG_TAG, __VA_ARGS__)

/**
 * Log at info level, brief using of ulog.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  ...  same as printf() usage.
 * 
 * @return  0 on success, negative error on failure.
 */
#define LOGI(mod, ...) ulog(LOG_INFO, mod, ULOG_TAG, __VA_ARGS__)

/**
 * Log at debug level, brief using of ulog.
 *
 * @note: This Log API take effect only the switcher 'DEBUG' is switch on.
 * @param[in]  mod  string description of module.
 * @param[in]  ...  same as printf() usage.
 * 
 * @return  0 on success, negative error on failure.
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
 * 
 * @return  0 on success, negative error on failure.
 */
int aos_set_log_level(aos_log_level_t log_level);

/**
 * Function prototype for log init. This is called in system level automatically,
 * it is not necessary calling it in application. 
 *
 */
void ulog_init(void);

/**
 * Function prototype for ulog management.
 *
 * @param[in] cmd_str command string, user fill the command string to manage ulog, format
 *            shall as below:
 *            "tcpip service=1" or "tcpip service=0" to notice the tcpip feature is install or not,
 *            which have impact on the ulog pop out via udp session;
 *            "listen ip=16777343" to nofice the syslog listener's address is 127.0.0.1;
 *            "fspause=1" or "fspause=0" to command pause/resume into file system or not;
 *            "ulog file up=1" to notify upload log file 1 via http.
 * 
 * @return  0 on Success, -EINVAL: invalid parameter, -EPERM: ulog module not initied yet,
 *          -EACCES: this function not work as there is no ASYNC method using. Other fail
 *          reason may come from kernel message queue.
 */
int ulog_man(const char *cmd_str);

/**
 * Function prototype for get ulog list recorded in file system.
 * Precondition of use this function is switch on ULOG_POP_FS_ENABLE
 * The result is usually like below:
 * {"time":"   4.500",list:[{"idx":1,"start":"   0.000","end":""}]}
 * context of time is system time when called, list show the start time and end time of special index.
 * If value of end time is empty, means this log file is logging.
 *
 * @param[in] buf  buffer received the context of file list
 * @param[in] len  length of buffer
 * 
 * @return  0 sucessfully, -EINVAL param illegal, -ENOMEM not enough room to receive file
 *          list, -EIO system error.
 */
int aos_get_ulog_list(char *buf, const unsigned short len);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* ULOG_H */

