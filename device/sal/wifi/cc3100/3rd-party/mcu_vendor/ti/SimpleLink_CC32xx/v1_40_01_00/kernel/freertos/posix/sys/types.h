/*
 * Copyright (c) 2016-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== sys/types.h ========
 */

#ifndef os_freertos_posix_sys_types__include
#define os_freertos_posix_sys_types__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>


#if !defined(__GNUC__) || defined(__ti__)
/*
 *  ssize_t is a signed size_t.  It is the return type for mqueue
 *  receive functions, so we need to define it for TI and IAR.
 */
typedef long int ssize_t;

typedef uint32_t clockid_t;
typedef unsigned long useconds_t;
typedef unsigned long timer_t;

#else
#include <../include/sys/types.h>
#endif

/*
 *************************************************************************
 *                      posix types
 *************************************************************************
 */

/*
 *  ======== pthread_attr_t ========
 */
typedef struct pthread_attr_t {
    int priority;
    void *stack;
    size_t stacksize;
    size_t guardsize;
    int  detachstate;
} pthread_attr_t;

typedef uint32_t pthread_barrierattr_t;
typedef uint32_t pthread_condattr_t;

typedef void *pthread_key_t;

typedef struct pthread_mutexattr_t {
    int type;
    int protocol;
} pthread_mutexattr_t;

typedef uint32_t pthread_rwlockattr_t;

typedef void *pthread_t;
typedef void *pthread_barrier_t;
typedef void *pthread_cond_t;
typedef void *pthread_mutex_t;
typedef uint32_t pthread_once_t;

typedef void *pthread_rwlock_t;
struct _pthread_cleanup_context {
    void      (*fxn)(void *);
    void      *arg;
    int       cancelType;
    pthread_t thread;
    struct _pthread_cleanup_context *next;
};

#ifdef __cplusplus
}
#endif

#endif
