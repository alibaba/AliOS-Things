/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_LOG_IMPL_H
#define AOS_LOG_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned int aos_log_level;
static inline unsigned int aos_log_get_level(void)
{
    return aos_log_level;
}

enum log_level_bit {
    AOS_LL_V_NONE_BIT = -1,
    AOS_LL_V_FATAL_BIT,
    AOS_LL_V_ERROR_BIT,
    AOS_LL_V_WARN_BIT,
    AOS_LL_V_INFO_BIT,
    AOS_LL_V_DEBUG_BIT,
    AOS_LL_V_MAX_BIT
};

#define AOS_LOG_LEVEL aos_log_get_level()

#define AOS_LL_V_NONE  0
#define AOS_LL_V_ALL   0XFF
#define AOS_LL_V_FATAL (1 << AOS_LL_V_FATAL_BIT)
#define AOS_LL_V_ERROR (1 << AOS_LL_V_ERROR_BIT)
#define AOS_LL_V_WARN  (1 << AOS_LL_V_WARN_BIT)
#define AOS_LL_V_INFO  (1 << AOS_LL_V_INFO_BIT)
#define AOS_LL_V_DEBUG (1 << AOS_LL_V_DEBUG_BIT)

/*
 * color def.
 * see http://stackoverflow.com/questions/3585846/color-text-in-terminal-applications-in-unix
 */
#define COL_DEF "\x1B[0m"  /* white */
#define COL_RED "\x1B[31m" /* red */
#define COL_GRE "\x1B[32m" /* green */
#define COL_BLU "\x1B[34m" /* blue */
#define COL_YEL "\x1B[33m" /* yellow */
#define COL_WHE "\x1B[37m" /* white */
#define COL_CYN "\x1B[36m"
#define COL_MAG "\x1B[35m"

#include <aos/kernel.h>

extern int log_get_mutex(void);

extern void log_release_mutex(void);

extern void log_init_mutex(void);
extern int csp_printf(const char *fmt, ...);
#ifdef CONFIG_LOGMACRO_DETAILS
#define log_print(CON, MOD, COLOR, LVL, ...)  \
    do {                                      \
        if (CON) {                            \
            if(log_get_mutex()){              \
                long long ms = aos_now_ms();  \
                csp_printf(COLOR " [%4d.%03d]<%s> %s [%s#%d] : ", (int)(ms/1000), (int)(ms%1000), LVL, MOD, __FUNCTION__, __LINE__); \
                csp_printf(__VA_ARGS__);      \
                csp_printf("\r\n");           \
            log_release_mutex();              \
            }\
        } \
    } while (0)

#else
#define log_print(CON, MOD, COLOR, LVL, ...) \
    do { \
        if (CON) { \
            if(log_get_mutex()){             \
                csp_printf("[%06d]<" LVL "> ", (unsigned)aos_now_ms(), __VA_ARGS__); \
                csp_printf(__VA_ARGS__);     \
                csp_printf("\r\n");          \
                log_release_mutex();         \
            }                                \
        } \
    } while (0)

#endif

#define void_func(fmt, ...)

#ifndef os_printf
#ifndef csp_printf
int csp_printf(const char *fmt, ...);
#else
extern int csp_printf(const char *fmt, ...);
#endif
#else
extern int csp_printf(const char *fmt, ...);
#endif

#undef LOGF
#undef LOGE
#undef LOGW
#undef LOGI
#undef LOGD
#undef LOG

#define LOG_IMPL(...) \
            log_print(1, "AOS", COL_DEF, "V", __VA_ARGS__)

#ifdef NDEBUG
#define CONFIG_LOGMACRO_SILENT
#endif

#ifdef DEBUG
#define LOGD_IMPL(mod, ...) \
            log_print(AOS_LOG_LEVEL & AOS_LL_V_DEBUG, mod, COL_WHE, "D", __VA_ARGS__)
#else
#define LOGD_IMPL(mod, ...) void_func(__VA_ARGS__)
#endif

#ifdef CONFIG_LOGMACRO_SILENT
#define LOGF_IMPL(mod, ...) void_func(__VA_ARGS__)
#define LOGE_IMPL(mod, ...) void_func(__VA_ARGS__)
#define LOGW_IMPL(mod, ...) void_func(__VA_ARGS__)
#define LOGI_IMPL(mod, ...) void_func(__VA_ARGS__)

#else

#define LOGF_IMPL(mod, ...) \
            log_print(AOS_LOG_LEVEL & AOS_LL_V_FATAL, mod, COL_RED, "F", __VA_ARGS__)
#define LOGE_IMPL(mod, ...) \
            log_print(AOS_LOG_LEVEL & AOS_LL_V_ERROR, mod, COL_YEL, "E", __VA_ARGS__)
#define LOGW_IMPL(mod, ...) \
            log_print(AOS_LOG_LEVEL & AOS_LL_V_WARN, mod, COL_BLU, "W", __VA_ARGS__)
#define LOGI_IMPL(mod, ...) \
            log_print(AOS_LOG_LEVEL & AOS_LL_V_INFO, mod, COL_GRE, "I", __VA_ARGS__)

#endif /* CONFIG_LOGMACRO_SILENT */

#ifdef __cplusplus
}
#endif

#endif /* AOS_LOG_IMPL_H */

