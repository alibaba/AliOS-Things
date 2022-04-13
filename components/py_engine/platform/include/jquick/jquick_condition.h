//
// jquick_condition.h
//
// Created by liujuncheng@alibaba-inc.com on 2020/11/11.
//
// Copyright (c) 2020 Alibaba All rights reserved.
//

#ifndef __APPX_JQUICK_CONDITION_H__
#define __APPX_JQUICK_CONDITION_H__

#include "jquick_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* JQuick_Condition;

JQuick_Condition jquick_condition_create();

int jquick_condition_wait(
        JQuick_Condition condition,
        JQuick_Mutex mutex);

int jquick_condition_wait_with_timeout(
        JQuick_Condition condition,
        JQuick_Mutex mutex,
        int millisecond);

int jquick_condition_signal(
        JQuick_Condition condition);

int jquick_condition_broadcast(
        JQuick_Condition condition);

int jquick_condition_destroy(
        JQuick_Condition condition);

#ifdef __cplusplus
}
#endif

#endif  // __APPX_JQUICK_CONDITION_H__
