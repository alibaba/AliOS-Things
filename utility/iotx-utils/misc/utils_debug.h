/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _IOTX_COMMON_DEBUG_H_
#define _IOTX_COMMON_DEBUG_H_

#include "lite-log.h"

#ifdef IOTX_DEBUG

#define DEBUG_PUTS(fmt, args ...) \
    do{ \
        HAL_Printf(fmt, ## args); \
    }while(0)

#define ASSERT_FAILED_DO() \
    do{ \
        while(1){\
            HAL_SleepMs(1000); \
            HAL_Printf("assert failed\r\n"); \
        }\
    }while(0)

#define IOTX_ASSERT(expr, fmt, args...) \
    do{ \
        if (!(expr)) { \
            DEBUG_PUTS("###ASSERT FAILED###, file=%s, line=%d\r\n", __FILE__, __LINE__); \
            if (NULL != fmt) { \
                DEBUG_PUTS(fmt, ## args); \
                HAL_Printf("\r\n"); \
            } \
            ASSERT_FAILED_DO(); \
        } \
    }while(0)



#define IOTX_TRACE(fmt, args...) \
    DEBUG_PUTS(fmt, ## args)
#else

#define TRACE(fmt, args...) do{ }while(0)

#endif


#endif /* _IOTX_COMMON_DEBUG_H_ */
