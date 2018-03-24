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
 
#ifndef __SYS_ARCH_H__
#define __SYS_ARCH_H__

#include "espos_task.h"
#include "espos_semaphore.h"
#include "espos_mutex.h"
#include "espos_queue.h"
#include "espos_scheduler.h"

typedef espos_sem_t sys_sem_t;
typedef espos_mutex_t sys_mutex_t;
typedef espos_queue_t sys_mbox_t;
typedef espos_task_t sys_thread_t;

#define sys_mbox_valid( x ) ( ( ( *x ) == ESPOS_OBJ_NONE) ? false : true )
#define sys_mbox_set_invalid( x ) ( ( *x ) = ESPOS_OBJ_NONE )
#define sys_sem_valid( x ) ( ( ( *x ) == ESPOS_OBJ_NONE) ? false : true )
#define sys_sem_set_invalid( x ) ( ( *x ) = ESPOS_OBJ_NONE )

#define LWIP_COMPAT_MUTEX 0

#endif /* __SYS_ARCH_H__ */

