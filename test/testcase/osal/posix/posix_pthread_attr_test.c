/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "k_api.h"
#include "posix/pthread.h"
#include "ulog/ulog.h"

void posix_pthread_attr_init_case(void);
void posix_pthread_attr_destroy_case(void);
void posix_pthread_attr_setdetachstate_case(void);
void posix_pthread_attr_getdetachstate_case(void);
void posix_pthread_attr_setschedpolicy_case(void);
void posix_pthread_attr_getschedpolicy_case(void);
void posix_pthread_attr_setschedparam_case(void);
void posix_pthread_attr_getschedparam_case(void);
void posix_pthread_attr_setstacksize_case(void);
void posix_pthread_attr_getstacksize_case(void);
void posix_pthread_attr_setstackaddr_case(void);
void posix_pthread_attr_getstackaddr_case(void);
void posix_pthread_attr_setstack_case(void);
void posix_pthread_attr_getstack_case(void);
void posix_pthread_attr_setinheritsched_case(void);
void posix_pthread_attr_getinheritsched_case(void);
void posix_pthread_attr_setscope_case(void);
void posix_pthread_attr_getscope_case(void);

/*************************** test pthread_attr_init *****************************/
void posix_pthread_attr_init_case(void)
{
    pthread_attr_t attr;

    char *casename = "posix_pthread_attr_init_case";
    int   flag     = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    pthread_attr_init(&attr);

    if ((attr.stacksize == DEFAULT_THREAD_STACK_SIZE)
       &&(attr.schedparam.sched_priority == DEFAULT_THREAD_PRIORITY)
       &&(attr.schedparam.slice == DEFAULT_THREAD_SLICE)
       &&(attr.detachstate == PTHREAD_CREATE_JOINABLE)
       &&(attr.contentionscope == PTHREAD_SCOPE_SYSTEM)
       &&(attr.inheritsched == PTHREAD_EXPLICIT_SCHED)
       &&(attr.guardsize == 0)
       &&(attr.stackaddr == NULL)
       &&(attr.is_initialized == 1)) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

/*************************** test pthread_attr_destroy *****************************/
void posix_pthread_attr_destroy_case(void)
{
    pthread_attr_t attr;

    char *casename = "posix_pthread_attr_destroy_case";
    int   flag     = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    pthread_attr_destroy(&attr);

    if ((attr.stacksize == 0)
       &&(attr.schedparam.sched_priority == 0)
       &&(attr.schedparam.slice == 0)
       &&(attr.detachstate == 0)
       &&(attr.contentionscope == 0)
       &&(attr.inheritsched == 0)
       &&(attr.guardsize == 0)
       &&(attr.stackaddr == NULL)
       &&(attr.is_initialized == 0)) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

/*************************** test pthread_attr_setdetachstate *****************************/
void posix_pthread_attr_setdetachstate_case(void)
{
    pthread_attr_t attr;

    char *casename = "posix_pthread_attr_setdetachstate_case";
    int   flag     = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    if (attr.detachstate == PTHREAD_CREATE_JOINABLE) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

/*************************** test pthread_attr_getdetachstate *****************************/
void posix_pthread_attr_getdetachstate_case(void)
{
    pthread_attr_t attr;

    char *casename    = "posix_pthread_attr_getdetachstate_case";
    int   flag        = 0;
    int   detachstate = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    attr.detachstate = PTHREAD_CREATE_JOINABLE;
    pthread_attr_getdetachstate(&attr, &detachstate);

    if (detachstate == PTHREAD_CREATE_JOINABLE) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

/*************************** test pthread_attr_setschedpolicy *****************************/
void posix_pthread_attr_setschedpolicy_case(void)
{
    pthread_attr_t attr;

    char *casename = "posix_pthread_attr_setschedpolicy_case";
    int   flag     = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

    if (attr.schedpolicy == SCHED_FIFO) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

/*************************** test pthread_attr_getschedpolicy *****************************/
void posix_pthread_attr_getschedpolicy_case(void)
{
    pthread_attr_t attr;

    char *casename    = "posix_pthread_attr_getschedpolicy_case";
    int   flag        = 0;
    int   schedpolicy = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    attr.schedpolicy = SCHED_FIFO;
    pthread_attr_getschedpolicy(&attr, &schedpolicy);

    if (schedpolicy == SCHED_FIFO) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

/*************************** test pthread_attr_setschedparam *****************************/
void posix_pthread_attr_setschedparam_case(void)
{
    pthread_attr_t attr;

    char *casename = "posix_pthread_attr_setschedparam_case";
    int   flag     = 0;

    struct sched_param param;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    param.sched_priority = DEFAULT_THREAD_PRIORITY;
    param.slice = DEFAULT_THREAD_SLICE;

    pthread_attr_setschedparam(&attr, &param);

    if ((attr.schedparam.sched_priority == DEFAULT_THREAD_PRIORITY)
        &&(attr.schedparam.slice == DEFAULT_THREAD_SLICE)) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

/*************************** test pthread_attr_getschedparam *****************************/
void posix_pthread_attr_getschedparam_case(void)
{
    pthread_attr_t attr;

    char *casename = "posix_pthread_attr_getschedparam_case";
    int   flag     = 0;

    struct sched_param param;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    attr.schedparam.sched_priority == DEFAULT_THREAD_PRIORITY;
    attr.schedparam.slice == DEFAULT_THREAD_SLICE;

    memset(&param, 0, sizeof(struct sched_param));
    pthread_attr_getschedparam(&attr, &param);

    if ((param.sched_priority == DEFAULT_THREAD_PRIORITY)
        &&(param.slice == DEFAULT_THREAD_SLICE)) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

/*************************** test pthread_attr_setstacksize *****************************/
void posix_pthread_attr_setstacksize_case(void)
{
    pthread_attr_t attr;

    char *casename = "posix_pthread_attr_setstacksize_case";
    int   flag     = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    pthread_attr_setstacksize(&attr, 1024);

    if (attr.stacksize == 1024) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

/*************************** test pthread_attr_getstacksize *****************************/
void posix_pthread_attr_getstacksize_case(void)
{
    pthread_attr_t attr;

    char *casename  = "posix_pthread_attr_getstacksize_case";
    int   flag      = 0;
    int   stacksize = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    attr.stacksize = 1024;
    pthread_attr_getstacksize(&attr, &stacksize);

    if (stacksize == 1024) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

/*************************** test pthread_attr_setstackaddr *****************************/
void posix_pthread_attr_setstackaddr_case(void)
{
    pthread_attr_t attr;

    char *casename = "posix_pthread_attr_setstackaddr_case";
    int   flag     = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    pthread_attr_setstackaddr(&attr, 0x12345678);

    if (attr.stackaddr == 0x12345678) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

/*************************** test pthread_attr_getstackaddr *****************************/
void posix_pthread_attr_getstackaddr_case(void)
{
    pthread_attr_t attr;

    char *casename  = "posix_pthread_attr_getstackaddr_case";
    int   flag      = 0;
    int   stackaddr = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    attr.stackaddr = 0x12345678;
    pthread_attr_getstackaddr(&attr, &stackaddr);

    if (stackaddr == 0x12345678) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

/*************************** test pthread_attr_setstack *****************************/
void posix_pthread_attr_setstack_case(void)
{
    pthread_attr_t attr;

    char *casename = "posix_pthread_attr_setstack_case";
    int   flag     = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    pthread_attr_setstack(&attr, 0x12345678, 1024);
    if ((attr.stackaddr == 0x12345678) && (attr.stacksize == 1024)) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

/*************************** test pthread_attr_getstack *****************************/
void posix_pthread_attr_getstack_case(void)
{
    pthread_attr_t attr;

    char *casename  = "posix_pthread_attr_getstack_case";
    int   flag      = 0;
    int   stackaddr = 0;
    int   stacksize = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    attr.stackaddr = 0x12345678;
    attr.stacksize = 1024;
    pthread_attr_getstack(&attr, &stackaddr, &stacksize);

    if ((stackaddr == 0x12345678) && (stacksize = 1024)) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

/*************************** test pthread_attr_setinheritsched *****************************/
void posix_pthread_attr_setinheritsched_case(void)
{
    pthread_attr_t attr;

    char *casename = "posix_pthread_attr_setinheritsched_case";
    int   flag     = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

    if (attr.inheritsched == PTHREAD_EXPLICIT_SCHED) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

/*************************** test pthread_attr_getinheritsched *****************************/
void posix_pthread_attr_getinheritsched_case(void)
{
    pthread_attr_t attr;

    char *casename     = "posix_pthread_attr_getinheritsched_case";
    int   flag         = 0;
    int   inheritsched = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    attr.inheritsched = PTHREAD_EXPLICIT_SCHED;
    pthread_attr_getinheritsched(&attr, &inheritsched);

    if (inheritsched == PTHREAD_EXPLICIT_SCHED) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

/*************************** test pthread_attr_setscope *****************************/
void posix_pthread_attr_setscope_case(void)
{
    pthread_attr_t attr;

    char *casename = "posix_pthread_attr_setscope_case";
    int   flag     = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    if (attr.contentionscope == PTHREAD_SCOPE_SYSTEM) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

/*************************** test pthread_attr_getscope *****************************/
void posix_pthread_attr_getscope_case(void)
{
    pthread_attr_t attr;

    char *casename        = "posix_pthread_attr_getscope_case";
    int   flag            = 0;
    int   contentionscope = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    attr.contentionscope = PTHREAD_SCOPE_SYSTEM;
    pthread_attr_getscope(&attr, &contentionscope);

    if (contentionscope == PTHREAD_SCOPE_SYSTEM) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}
