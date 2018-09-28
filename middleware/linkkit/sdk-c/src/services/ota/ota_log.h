/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _OTA_LOG_H_
#define _OTA_LOG_H_

#include <stdarg.h>
#include <stdio.h>

#define ota_snprintf      HAL_Snprintf
#define ota_malloc        HAL_Malloc
#define ota_free          HAL_Free

#if 1
#define OTA_LOG_D(fmt, ...) printf("[%s:%d]"fmt"\r\n" ,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define OTA_LOG_I(fmt, ...) printf("[%s:%d]"fmt"\r\n" ,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define OTA_LOG_W(fmt, ...) printf("[%s:%d]"fmt"\r\n" ,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define OTA_LOG_E(fmt, ...) printf("[%s:%d]"fmt"\r\n" ,__FUNCTION__,__LINE__,##__VA_ARGS__)
#endif
#endif  // _OTA_LOG_H_
