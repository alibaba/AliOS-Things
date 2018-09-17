/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef REAL_TIME_DEFAULT_CONFIG_H
#define REAL_TIME_DEFAULT_CONFIG_H

#include <aos/aos.h>
#include <k_api.h>

#ifndef ENABLE_INTRPT_RESPOND_WAKEUP
#define ENABLE_INTRPT_RESPOND_WAKEUP    1   /* enable interrupt respond and wakeup task test*/
#endif

#ifndef ENABLE_INTRPT_PREEMPT
#define ENABLE_INTRPT_PREEMPT           1   /* enabel interrupt preempt test */
#endif

#ifndef RTTEST_DEBUG
#define RTTEST_DEBUG          0
#endif

/* test case parameter */
#define TEST_ITERATION        5000
#define TEST_TASK_PRIORITY    5    /* >= 3 */
#define TLF_MEMORY_ALLOC_SIZE 64   /* memory size to allocate */
#define BLK_MEMORY_ALLOC_SIZE 16   /* memory size to allocate */
#define MM_BLK_POOL_SIZE      2048
#define MM_BLK_SIZE           32
#define TEST_QUEUE_MSG_SIZE   30

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
