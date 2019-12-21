/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef LWIP_ARCH_SYS_ARCH_H
#define LWIP_ARCH_SYS_ARCH_H

#include <errno.h>
#include "aos/kernel.h"

#define SYS_MBOX_NULL NULL
#define SYS_SEM_NULL  NULL

struct sys_mbox_msg {
    struct sys_mbox_msg *next;
    void *msg;
};

typedef aos_sem_t sys_sem_t;

#define sys_sem_valid(sem)           aos_sem_is_valid(sem)
#define sys_sem_set_invalid(sem)     do { if(sem != NULL) { (sem)->hdl = NULL; }}while(0)

typedef u32_t sys_prot_t;

typedef aos_mutex_t sys_mutex_t;
#define sys_mutex_valid(mutex)       aos_mutex_is_valid(mutex)
#define sys_mutex_set_invalid(mutex) do { if(mutex != NULL) { (mutex)->hdl = NULL; }}while(0)

#ifndef LWIP_MAILBOX_QUEUE
#define SYS_MBOX_SIZE 128

struct sys_mbox {
    int first, last;
    void *msgs[SYS_MBOX_SIZE];
    sys_sem_t not_empty;
    sys_sem_t not_full;
    sys_sem_t mutex;
    int wait_send;
};

typedef struct sys_mbox *sys_mbox_t;
#define sys_mbox_valid(mbox)       #error TODO:impl
#define sys_mbox_set_invalid(mbox) #error TODO:impl
#else
typedef aos_queue_t sys_mbox_t;
#define sys_mbox_valid(mbox)       aos_queue_is_valid(mbox)
#define sys_mbox_set_invalid(mbox) do { if(mbox != NULL) { (mbox)->hdl = NULL; }}while(0)
#endif

typedef void *sys_thread_t;

#endif /* LWIP_ARCH_SYS_ARCH_H */

