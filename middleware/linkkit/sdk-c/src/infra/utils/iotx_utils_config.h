/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef __LITE_UTILS_CONFIG_H__
#define __LITE_UTILS_CONFIG_H__

#define UTILS_printf                HAL_Printf
#define UTILS_malloc                HAL_Malloc
#define UTILS_vsnprintf             HAL_Vsnprintf
#define UTILS_free                  HAL_Free

#ifndef WITH_MEM_STATS
    #ifdef __UBUNTU_SDK_DEMO__
        #define WITH_MEM_STATS              0
        #define WITH_MEM_STATS_PER_MODULE   0
    #else
        #define WITH_MEM_STATS              0
        #define WITH_MEM_STATS_PER_MODULE   0
    #endif
#endif

#endif  /* __LITE_UTILS_CONFIG_H__ */
