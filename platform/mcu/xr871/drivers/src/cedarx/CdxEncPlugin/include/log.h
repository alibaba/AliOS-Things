#ifndef LOG_H
#define LOG_H
#ifndef LOG_TAG
#define LOG_TAG "AllwinnerAudioCodec"
#endif

#include "cdx_malloc_dbg.h"

#define OPTION_LOG_LEVEL_WARNING

#if 0
#ifdef OS_ANDROID
    #include <cutils/log.h>

    #define LOG_LEVEL_ERROR     ANDROID_LOG_ERROR
    #define LOG_LEVEL_WARNING   ANDROID_LOG_WARN
    #define LOG_LEVEL_INFO      ANDROID_LOG_INFO
    #define LOG_LEVEL_VERBOSE   ANDROID_LOG_VERBOSE
    #define LOG_LEVEL_DEBUG     ANDROID_LOG_DEBUG

    #define AWLOG(level, fmt, arg...)  \
        LOG_PRI(level, LOG_TAG, "<%s:%u>: "fmt, __FUNCTION__, __LINE__, ##arg)

#elif (defined OS_LINUX)
    #include <stdio.h>
    #include <string.h>

    #define LOG_LEVEL_ERROR     "error  "
    #define LOG_LEVEL_WARNING   "warning"
    #define LOG_LEVEL_INFO      "info   "
    #define LOG_LEVEL_VERBOSE   "verbose"
    #define LOG_LEVEL_DEBUG     "debug  "

    #define AWLOG(level, fmt, arg...)  \
        printf("%s: %s <%s:%u>: "fmt"\n", level, LOG_TAG, __FUNCTION__, __LINE__, ##arg)
#else
    #error "invalid configuration of os."
#endif
#endif



	#ifndef LOG_LEVEL_ERROR
    #define LOG_LEVEL_ERROR     "error  "
	#endif
	#ifndef LOG_LEVEL_WARNING
    #define LOG_LEVEL_WARNING   "warning"
	#endif
	#ifndef LOG_LEVEL_INFO
    #define LOG_LEVEL_INFO      "info   "
	#endif
	#ifndef LOG_LEVEL_VERBOSE
    #define LOG_LEVEL_VERBOSE   "verbose"
	#endif
	#ifndef LOG_LEVEL_DEBUG
    #define LOG_LEVEL_DEBUG     "debug  "
	#endif

int wrap_printf(const char *fmt, ...);
#define AWLOG(level, fmt, arg...)  \
        wrap_printf("%s: %s <%s:%u>: "fmt"\n", level, LOG_TAG, __func__, __LINE__, ##arg)

void __printf_time(const char *f, unsigned int l);
#define printf_time() __printf_time(__func__, __LINE__)

#define logd(fmt, arg...) //AWLOG(LOG_LEVEL_DEBUG, fmt, ##arg)

#ifdef OPTION_LOG_LEVEL_CLOSE

#define loge(fmt, arg...)
#define logw(fmt, arg...)
#define logi(fmt, arg...)
#define logv(fmt, arg...)

#elif (defined OPTION_LOG_LEVEL_ERROR)

#define loge(fmt, arg...) AWLOG(LOG_LEVEL_ERROR, "\033[40;31m"fmt"\033[0m", ##arg)
#define logw(fmt, arg...)
#define logi(fmt, arg...)
#define logv(fmt, arg...)

#elif (defined OPTION_LOG_LEVEL_WARNING)

#define loge(fmt, arg...) AWLOG(LOG_LEVEL_ERROR, "\033[40;31m"fmt"\033[0m", ##arg)
#define logw(fmt, arg...) AWLOG(LOG_LEVEL_WARNING, fmt, ##arg)
#define logi(fmt, arg...)
#define logv(fmt, arg...)

#elif (defined OPTION_LOG_LEVEL_DEFAULT)

#define loge(fmt, arg...) AWLOG(LOG_LEVEL_ERROR, "\033[40;31m"fmt"\033[0m", ##arg)
#define logw(fmt, arg...) AWLOG(LOG_LEVEL_WARNING, fmt, ##arg)
#define logi(fmt, arg...) AWLOG(LOG_LEVEL_INFO, fmt, ##arg)
#define logv(fmt, arg...)

#elif (defined OPTION_LOG_LEVEL_DETAIL)

#define loge(fmt, arg...) AWLOG(LOG_LEVEL_ERROR, "\033[40;31m"fmt"\033[0m", ##arg)
#define logw(fmt, arg...) AWLOG(LOG_LEVEL_WARNING, fmt, ##arg)
#define logi(fmt, arg...) AWLOG(LOG_LEVEL_INFO, fmt, ##arg)
#define logv(fmt, arg...) AWLOG(LOG_LEVEL_VERBOSE, fmt, ##arg)

#else
    #error "invalid configuration of debug level."
#endif

#endif

