//
// jquick_thread.h
//
// Created by liujuncheng@alibaba-inc.com on 2020/11/12.
//
// Copyright (c) 2020 Alibaba All rights reserved.
//

#ifndef __APPX_JQUICK_THREAD_H__
#define __APPX_JQUICK_THREAD_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void* JQuick_Thread;
typedef void* (*Runner)(void* arg);

JQuick_Thread jquick_thread_create(
        const char* name,
        Runner runner,
        void* args);

JQuick_Thread jquick_thread_create_with_stack_size(
        const char* name,
        Runner runner,
        void* args,
        int stackSize);

void jquick_thread_set_current_name(
        const char* name);

void jquick_thread_set_name(
        JQuick_Thread thread,
        const char* name);

JQuick_Thread jquick_thread_get_current();

int jquick_thread_set_priority(
        JQuick_Thread thread,
        int priority);

#ifdef __cplusplus
}
#endif

#endif  // __APPX_JQUICK_THREAD_H__
