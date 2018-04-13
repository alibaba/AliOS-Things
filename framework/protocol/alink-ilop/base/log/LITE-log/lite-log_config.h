#ifndef __LITE_LOG_CONFIG_H__
#define __LITE_LOG_CONFIG_H__

#define LITE_printf                     printf
#define LITE_sprintf                    sprintf
#define LITE_snprintf                   snprintf
#define LITE_vsnprintf                  vsnprintf

#define LITE_LOG_ENABLED

#define LOG_MSG_MAXLEN                  (255)
#define LOG_MOD_NAME_LEN                (7)
#define LOG_PREFIX_FMT                  "[%s] %s(%d): "
#define HEXDUMP_SEP_LINE                "+" \
    "-----------------------" \
    "-----------------------" \
    "-----------------------"

#if defined(_PLATFORM_IS_LINUX_)
#undef  LOG_MSG_MAXLEN
#define LOG_MSG_MAXLEN                  (512)
#endif

#endif  /* __LITE_LOG_CONFIG_H__ */

