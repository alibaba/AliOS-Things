/* Copyright (2017) Baidu Inc. All rights reserved.
 *
 * File: duerapp_system_info.h
 * Auth: Zhong Shuai(zhongshuai@baidu.com)
 * Desc: Provide the information about the system
 */

#ifndef BAIDU_DUER_LIBDUER_DEVICE_PLATFORM_INCLUDE_LIGHTDUER_STATISTICS_H
#define BAIDU_DUER_LIBDUER_DEVICE_PLATFORM_INCLUDE_LIGHTDUER_STATISTICS_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Initialise System Info Module
 *
 * @param void:
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */

int duer_statistics_initialize(void);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIBDUER_DEVICE_PLATFORM_INCLUDE_LIGHTDUER_STATISTICS_H
