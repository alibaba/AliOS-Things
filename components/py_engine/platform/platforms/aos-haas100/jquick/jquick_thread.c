//
// jquick_thread.cpp
//
// Created by liujuncheng@alibaba-inc.com on 2020/10/23.
//
// Copyright (c) 2020 Alibaba All rights reserved.
//

#include "jquick_thread.h"
#include <pthread.h>
#include <stdlib.h>
//#include "utils/log.h"
#include "HaasLog.h"
#include <memory.h>

JQuick_Thread jquick_thread_create(const char* name, Runner runner, void* args)
{
    JQuick_Thread thread = (JQuick_Thread)malloc(sizeof(pthread_t));
    if (!thread) {
        LOG_E("JQuick_Thread: out of memory\n");

        return NULL;
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int v = pthread_create((pthread_t*)thread, &attr, runner, args);
    if (v == 0) {
        return thread;
    }

    LOG_E("JQuick_Thread: Create JQuick_Thread failed\n");

    free(thread);

    return NULL;
}

JQuick_Thread jquick_thread_create_with_stack_size(
        const char* name,
        Runner runner,
        void* args,
        int stackSize)
{
    JQuick_Thread thread = (JQuick_Thread)malloc(sizeof(pthread_t));
    if (!thread) {
        LOG_E("JQuick_Thread: out of memory\n");

        return NULL;
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, stackSize);
    int v = pthread_create((pthread_t*)thread, &attr, runner, args);
    if (v == 0) {
        return thread;
    }

    LOG_E("JQuick_Thread: Create JQuick_Thread failed\n");

    free(thread);

    return NULL;
}

int jquick_thread_set_priority(JQuick_Thread thread, int priority)
{
    struct sched_param sched;
    memset(&sched, 0, sizeof(struct sched_param));
    sched.sched_priority = priority;

    return pthread_setschedparam(*(pthread_t*)thread, SCHED_RR, &sched);
}

void jquick_thread_set_name(
        JQuick_Thread thread,
        const char* name)
{
}

void jquick_thread_set_current_name(const char* name)
{
#if __DARWIN__
    pthread_setname_np(name);
#endif
}

JQuick_Thread jquick_thread_get_current()
{
    return (void*)pthread_self();
}
