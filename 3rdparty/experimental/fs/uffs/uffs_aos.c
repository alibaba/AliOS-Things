/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "uffs_port.h"
#include "uffs/uffs_os.h"
#include <aos/aos.h>
#include <stdio.h>
#include <stdlib.h>

#define PFX "os  : "
#define UFFS_TAG "uffs"

int uffs_SemCreate(OSSEM *sem)
{
    return aos_mutex_new((aos_mutex_t *)sem);
}

int uffs_SemWait(OSSEM sem)
{
    return aos_mutex_lock((aos_mutex_t *)(&sem), AOS_WAIT_FOREVER);
}

int uffs_SemSignal(OSSEM sem)
{
    return aos_mutex_unlock((aos_mutex_t *)(&sem));
}

int uffs_SemDelete(OSSEM *sem)
{
    aos_mutex_free((aos_mutex_t *)sem);
    *sem = 0;
    return 0;
}

int uffs_OSGetTaskId(void)
{
    return 0;
}

unsigned int uffs_GetCurDateTime(void)
{
    return 0;
}

#if CONFIG_USE_SYSTEM_MEMORY_ALLOCATOR > 0
static void * sys_malloc(struct uffs_DeviceSt *dev, unsigned int size)
{
    uffs_Perror(UFFS_MSG_NORMAL, "system memory alloc %d bytes", size);
    return aos_malloc(size);
}

static URET sys_free(struct uffs_DeviceSt *dev, void *p)
{
    aos_free(p);
    return U_SUCC;
}

void uffs_MemSetupSystemAllocator(uffs_MemAllocator *allocator)
{
    allocator->malloc = sys_malloc;
    allocator->free = sys_free;
}
#endif

static void output_dbg_msg(const char *msg);
static struct uffs_DebugMsgOutputSt m_dbg_ops = {
    output_dbg_msg,
    NULL,
};

static void output_dbg_msg(const char *msg)
{
    LOGD(UFFS_TAG, "%s", msg);
}

void uffs_SetupDebugOutput(void)
{
    uffs_InitDebugMessageOutput(&m_dbg_ops, UFFS_MSG_NOISY);
}
