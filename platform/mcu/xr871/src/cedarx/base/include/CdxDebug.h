/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxDebug.h
 * Description : Debug
 * History :
 *
 */

#ifndef CDX_DEBUG_H
#define CDX_DEBUG_H
#include <pthread.h>
#ifdef LINUX_PLATFORM
#include <sys/syscall.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef LINUX_PLATFORM
#define gettid() syscall(__NR_gettid)

void CdxDumpThreadStack(pthread_t tid);

void CdxCallStack(void);
#else

static inline void CdxDumpThreadStack(pthread_t tid) {

}

static inline void CdxCallStack(pthread_t tid) {

}
#endif
#ifdef __cplusplus
}
#endif

#endif
