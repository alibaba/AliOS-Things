#ifndef __JOYLINK_LOG_H__
#define __JOYLINK_LOG_H__

#include <stdio.h>

#if defined(__MTK_7687__)
#elif defined(__ICOMM_6166__)
#else
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <time.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <dirent.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define JL_LOG_LEVEL_FATAL      (0)
#define JL_LOG_LEVEL_NOTICE     (1)
#define JL_LOG_LEVEL_INFO       (2)
#define JL_LOG_LEVEL_ERROR      (3)
#define JL_LOG_LEVEL_WARN       (4)
#define JL_LOG_LEVEL_DEBUG      (5)

#define Black   0;30
#define Red     0;31
#define Green   0;32
#define Brown   0;33
#define Blue    0;34
#define Purple  0;35
#define Cyan    0;36

//#define JL_LOG_LEVEL  JL_LOG_LEVEL_DEBUG
#define JL_LOG_LEVEL  JL_LOG_LEVEL_NOTICE

#define log_fatal(format, ...) \
    do{\
        if(JL_LOG_LEVEL >= JL_LOG_LEVEL_FATAL){\
            printf("\033[0;31m[FATAL][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
            fflush(stdout);\
            printf("\033[0m"); \
        }\
    }while(0)

#define log_notice(format, ...) \
    do{\
        if(JL_LOG_LEVEL >= JL_LOG_LEVEL_NOTICE){\
            printf("\033[0;36m[NOTICE][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
            fflush(stdout);\
            printf("\033[0m"); \
        }\
    }while(0)

#define log_info(format, ...) \
    do{\
        if(JL_LOG_LEVEL >= JL_LOG_LEVEL_INFO){\
            printf("\033[1;36m[INFO][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
            fflush(stdout);\
            printf("\033[0m"); \
        }\
    }while(0)

#define log_error(format, ...) \
    do{\
        if(JL_LOG_LEVEL >= JL_LOG_LEVEL_ERROR){\
            printf("\033[0;31m[ERROR][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
            fflush(stdout);\
            printf("\033[0m"); \
        }\
    }while(0)

#define log_warn(format, ...) \
    do{\
        if(JL_LOG_LEVEL >= JL_LOG_LEVEL_WARN){\
            printf("\033[1;33m[WARN][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
            fflush(stdout);\
            printf("\033[0m"); \
        }\
    }while(0)

#define log_debug(format, ...) \
    do{\
        if(JL_LOG_LEVEL >= JL_LOG_LEVEL_DEBUG){\
            printf("\033[1;32m[DEBUG][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
            fflush(stdout);\
            printf("\033[0m"); \
        }\
    }while(0)

#ifdef __cplusplus
}
#endif

#endif /* __LOGGING_H__ */
