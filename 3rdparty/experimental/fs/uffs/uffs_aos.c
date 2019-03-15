/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "uffs_port.h"
#include "uffs/uffs_os.h"
#include <stdio.h>
#include <stdlib.h>

#include "k_api.h"

#define PFX "os  : "
#define UFFS_TAG "uffs"

#define UFFS_WAIT_FOREVER 0xFFFFFFFF

int32_t uffs_SemCreate(OSSEM *sem)
{
    return krhino_mutex_create((kmutex_t *)sem, "uffs");
}

int32_t uffs_SemWait(OSSEM sem)
{
    return krhino_mutex_lock((kmutex_t *)(&sem), UFFS_WAIT_FOREVER);
}

int32_t uffs_SemSignal(OSSEM sem)
{
    return krhino_mutex_unlock((kmutex_t *)(&sem));
}

int32_t uffs_SemDelete(OSSEM *sem)
{
    krhino_mutex_free((kmutex_t *)sem);
    return 0;
}

int32_t uffs_OSGetTaskId(void)
{
    return 0;
}

uint32_t uffs_GetCurDateTime(void)
{
    return 0;
}

#if CONFIG_USE_SYSTEM_MEMORY_ALLOCATOR > 0
static void * sys_malloc(struct uffs_DeviceSt *dev, uint32_t size)
{
    uffs_Perror(UFFS_MSG_NORMAL, "system memory alloc %d bytes", size);
    return krhino_mm_alloc(size);
}

static URET sys_free(struct uffs_DeviceSt *dev, void *p)
{
    krhino_mm_free(p);
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
