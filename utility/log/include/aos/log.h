/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_LOG_H
#define AOS_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <aos/internal/log_impl.h>

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

#ifdef __cplusplus
}
#endif

#endif /* AOS_LOG_H */

