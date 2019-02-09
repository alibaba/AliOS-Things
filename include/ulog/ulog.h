/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef ULOG_H
#define ULOG_H

#include "ulog_config.h"
#include "log_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AOS_LL_NONE,  /* disable log */
    AOS_LL_FATAL, /* fatal log will output */
    AOS_LL_ERROR, /* fatal + error log will output */
    AOS_LL_WARN,  /* fatal + warn + error log will output(default level) */
    AOS_LL_INFO,  /* info + warn + error log will output */
    AOS_LL_DEBUG, /* debug + info + warn + error + fatal log will output */
} aos_log_level_t;

extern unsigned int aos_log_level;

/**
 * Get the log level.
 */
static inline int aos_get_log_level(void)
{
    return aos_log_level;
}

/**
 * Set the log level.
 *
 * @param[in]  log_level  level to be set,must be one of AOS_LL_NONE,AOS_LL_FATAL,AOS_LL_ERROR,AOS_LL_WARN,AOS_LL_INFO or AOS_LL_DEBUG.
 */
void aos_set_log_level(aos_log_level_t log_level);

/*
 * Log at fatal level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  fmt  same as printf() usage.
 */
#define LOGF(mod, ...) LOGF_IMPL(mod, __VA_ARGS__)

/*
 * Log at error level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  fmt  same as printf() usage.
 */
#define LOGE(mod, ...) LOGE_IMPL(mod, __VA_ARGS__)

/*
 * Log at warning level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  fmt  same as printf() usage.
 */
#define LOGW(mod, ...) LOGW_IMPL(mod, __VA_ARGS__)

/*
 * Log at info level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  fmt  same as printf() usage.
 */
#define LOGI(mod, ...) LOGI_IMPL(mod, __VA_ARGS__)

/*
 * Log at debug level.
 *
 * @param[in]  mod  string description of module.
 * @param[in]  fmt  same as printf() usage.
 */
#define LOGD(mod, ...) LOGD_IMPL(mod, __VA_ARGS__)

/*
 * Log at the level set by aos_set_log_level().
 *
 * @param[in]  fmt  same as printf() usage.
 */
#define LOG(...) LOG_IMPL(__VA_ARGS__)

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
* Module Type, used for distinguish funcion module of whole system, attached in front of TAG in MSG part.
* The log level can be filter based on the module type. User may pass below enum except MOD_SIZE &
* MOD_UNKNOWN.
*/
typedef enum{
    MOD_UNKNOWN,/* log text is "UN" if pass illegal parameter to module type */
    MOD_OS,     /* Log produced in OS , log string is "OS" */
    MOD_NET,    /* Log produced in Net, inlude lwip, bluetooth, etc , log string is "NT" */
    MOD_CLOUD,  /* Log produced in sdk which focus on communication with cloud , log string is "CD" */
    MOD_APP,    /* Log produced in user app, log string is "AP" */
    MOD_SIZE
}MOD_TYPE;


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
int ulog   (const MOD_TYPE m, const unsigned char s, const char* f, const unsigned long l, const char *fmt, ...);


/**
* Function prototype for log on OS related text, abbreviation of ULOG on OS
*
* REMARK: Log produced in OS , "OS_" will be attached in tag of log text
*
* @param  S    Serverity of Log, choose from above value,
* @param  ...  Variable Parameter, Log text try to log
*/
#define ULOGOS(S, ...) ulog(MOD_OS, S, __FILE__, __LINE__, __VA_ARGS__)

/**
* Function prototype for log on net(ble, CANopen, ...) related text, abbreviation of ULOG on net
*
* REMARK: "NT_" will be attached in tag of log text
*
* @param  S    Serverity of Log, choose from above value,
* @param  ...  Variable Parameter, Log text try to log
*/
#define ULOGNET(S, ...) ulog(MOD_NET, S, __FILE__, __LINE__, __VA_ARGS__)

/**
* Function prototype for log on Cloud related text(e.g. linkkit SDK), abbreviation of ULOG on SDK
*
* REMARK: "CD_" will be attached in tag of log text
*
* @param  S    Serverity of Log, choose from above value,
* @param  ...  Variable Parameter, Log text try to log
*/
#define ULOGCLOUD(S, ...) ulog(MOD_CLOUD, S, __FILE__, __LINE__, __VA_ARGS__)

/**
* Function prototype for log on APP related text, abbreviation of ULOG on APP
*
* REMARK: "AP_" will be attached in tag of log text
*
* @param  S    Serverity of Log, choose from above value,
* @param  ...  Variable Parameter, Log text try to log
*/
#define ULOGAPP(S, ...) ulog(MOD_APP, S, __FILE__, __LINE__, __VA_ARGS__)

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

