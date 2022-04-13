/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef __TASK_SCHEDULE_H__
#define __TASK_SCHEDULE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

#ifdef PROGRAMMER
#define SIMPLE_TASK_SWITCH
#endif

#ifdef RTOS
#undef SIMPLE_TASK_SWITCH
#endif

//-------------------------------------------------------
// Types

enum TASK_STATE_T {
    TASK_STATE_NONE,
    TASK_STATE_ACTIVE,
    TASK_STATE_PENDING,
    TASK_STATE_SLEEPING,
};

typedef void (*TASK_ENTRY_T)(void);

//-------------------------------------------------------
// Functions

#ifdef SIMPLE_TASK_SWITCH

#ifndef TASK_SCHEDULE
#define TASK_SCHEDULE                   task_yield()
#endif

int task_setup(uint8_t id, uint32_t sp, uint32_t sp_limit, enum TASK_STATE_T state, TASK_ENTRY_T entry);

bool task_yield(void);

bool task_sleep(void);

bool task_wakeup(uint8_t id);

#else // !SIMPLE_TASK_SWITCH

#define TASK_SCHEDULE                   true

static inline int task_setup(uint8_t id, uint32_t sp, uint32_t sp_limit, enum TASK_STATE_T state, TASK_ENTRY_T entry)
{
    return 0;
}

static inline bool task_yield(void)
{
    return true;
}

static inline bool task_sleep(void)
{
    return true;
}

static inline bool task_wakeup(uint8_t id)
{
    return true;
}

#endif // !SIMPLE_TASK_SWITCH

#ifdef __cplusplus
}
#endif

#endif

