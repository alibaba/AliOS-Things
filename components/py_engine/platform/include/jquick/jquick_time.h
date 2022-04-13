//
// jquick_time.h
//
// Created by liujuncheng@alibaba-inc.com on 2020/11/12.
//
// Copyright (c) 2020 Alibaba All rights reserved.
//

#ifndef __APPX_JQUICK_TIME_H__
#define __APPX_JQUICK_TIME_H__

#ifdef __cplusplus
extern "C" {
#endif

/** get the millisecond from the system boot
 *
 */
long long jquick_get_current_time();

#ifdef __cplusplus
}
#endif

#endif  // __APPX_JQUICK_TIME_H__
