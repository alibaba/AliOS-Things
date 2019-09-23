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

/* system includes */
#include "aos/kernel.h"

/* lwIP includes. */
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "arch/sys_arch.h"

#include "k_api.h"

static aos_mutex_t sys_arch_mutex;

//#define      NET_TASK_NUME 2
//#define      NET_TASK_STACK_SIZE 1024

//ktask_t       g_net_task[NET_TASK_NUME];
//cpu_stack_t  g_net_task_stack[NET_TASK_NUME][NET_TASK_STACK_SIZE];

/*-----------------------------------------------------------------------------------*/
/*
    err_t sys_sem_new(sys_sem_t *sem, u8_t count)
    Creates a new semaphore.
*/
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
    err_t ret = ERR_MEM;
    int stat = aos_sem_new(sem,count);

    if (stat == 0) {
        ret = ERR_OK;
    }
    return ret;
}

/*-----------------------------------------------------------------------------------*/
/*
    void sys_sem_free(sys_sem_t *sem)

    Deallocates a semaphore.
*/
void sys_sem_free(sys_sem_t *sem)
{
    if ((sem != NULL)) {
        aos_sem_free(sem);
    }
}


/*-----------------------------------------------------------------------------------*/
/*
    void sys_sem_signal(sys_sem_t *sem)

    Signals a semaphore.
*/
void sys_sem_signal(sys_sem_t *sem)
{
    aos_sem_signal(sem);
}

#ifdef LWIP_TASK_CANCEL
u32_t sys_arch_sem_wait_ext(sys_sem_t *sem, u32_t timeout)
{
    u32_t begin_ms, end_ms, elapsed_ms;
    u32_t ret;

    if (sem == NULL)
        return SYS_ARCH_TIMEOUT;

    begin_ms = sys_now();

    if( timeout != 0UL ) {
        ret = aos_sem_wait(sem,timeout);
        if(ret == 0) {
            end_ms = sys_now();

            elapsed_ms = end_ms - begin_ms;

            ret = elapsed_ms;
        } else {
            ret = SYS_ARCH_TIMEOUT;
        }
    } else {
        while( 1 ) {
            ret = aos_sem_wait(sem, AOS_WAIT_FOREVER);

            if(ret == 0) {
                break;
            }
            if(ret == RHINO_TASK_CANCELED) {
                return SYS_ARCH_TIMEOUT;
            }
        }

        end_ms = sys_now();

        elapsed_ms = end_ms - begin_ms;

        if( elapsed_ms == 0UL ) {
            elapsed_ms = 1UL;
        }

        ret = elapsed_ms;
    }

    return ret;
}
#endif /* LWIP_TASK_CANCEL */
/*-----------------------------------------------------------------------------------*/
/*
  Blocks the thread while waiting for the semaphore to be
  signaled. If the "timeout" argument is non-zero, the thread should
  only be blocked for the specified time (measured in
  milliseconds).

  If the timeout argument is non-zero, the return value is the number of
  milliseconds spent waiting for the semaphore to be signaled. If the
  semaphore wasn't signaled within the specified time, the return value is
  SYS_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
  (i.e., it was already signaled), the function may return zero.

  Notice that lwIP implements a function with a similar name,
  sys_sem_wait(), that uses the sys_arch_sem_wait() function.
*/
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    u32_t begin_ms, end_ms, elapsed_ms;
    u32_t ret;

    if (sem == NULL)
        return SYS_ARCH_TIMEOUT;

    begin_ms = sys_now();

    if( timeout != 0UL ) {
        ret = aos_sem_wait(sem,timeout);
        if(ret == 0) {
            end_ms = sys_now();

            elapsed_ms = end_ms - begin_ms;

            ret = elapsed_ms;
        } else {
            ret = SYS_ARCH_TIMEOUT;
        }
    } else {
        while( !(aos_sem_wait(sem, AOS_WAIT_FOREVER) == 0));
        end_ms = sys_now();

        elapsed_ms = end_ms - begin_ms;

        if( elapsed_ms == 0UL ) {
            elapsed_ms = 1UL;
        }

        ret = elapsed_ms;
    }

    return ret;
}

#ifndef LWIP_MAILBOX_QUEUE

/*-----------------------------------------------------------------------------------*/
/*
    err_t sys_mbox_new(sys_mbox_t *mbox, int size)

    Creates an empty mailbox for maximum "size" elements.
*/
err_t sys_mbox_new(sys_mbox_t *mb, int size)
{
    struct sys_mbox *mbox;
    LWIP_UNUSED_ARG(size);

    mbox = (struct sys_mbox *)malloc(sizeof(struct sys_mbox));
    if (mbox == NULL) {
        return ERR_MEM;
    }
    memset(mbox, 0, sizeof(struct sys_mbox));

    mbox->first = mbox->last = 0;
    sys_sem_new(&mbox->not_empty, 0);
    sys_sem_new(&mbox->not_full, 0);
    sys_sem_new(&mbox->mutex, 1);
    mbox->wait_send = 0;

    *mb = mbox;
    return ERR_OK;
}

/*-----------------------------------------------------------------------------------*/
/*
  Deallocates a mailbox. If there are messages still present in the
  mailbox when the mailbox is deallocated, it is an indication of a
  programming error in lwIP and the developer should be notified.
*/
void sys_mbox_free(sys_mbox_t *mb)
{
    if ((mb != NULL) && (*mb != SYS_MBOX_NULL)) {
        struct sys_mbox *mbox = *mb;

        sys_arch_sem_wait(&mbox->mutex, 0);

        sys_sem_free(&mbox->not_empty);
        sys_sem_free(&mbox->not_full);
        sys_sem_free(&mbox->mutex);
        /*  LWIP_DEBUGF("sys_mbox_free: mbox 0x%lx\n", mbox); */
        free(mbox);
    }
}

/*-----------------------------------------------------------------------------------*/
/*
    void sys_mbox_post(sys_mbox_t *mbox, void *msg)

    Posts the "msg" to the mailbox. This function have to block until  the "msg" is really posted.
*/
void sys_mbox_post(sys_mbox_t *mb, void *msg)
{
    u8_t first;
    struct sys_mbox *mbox;
    LWIP_ASSERT("invalid mbox", (mb != NULL) && (*mb != NULL));
    mbox = *mb;

    sys_arch_sem_wait(&mbox->mutex, 0);

    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_post: mbox %p msg %p\n", (void *)mbox, (void *)msg));

    while ((mbox->last + 1) >= (mbox->first + SYS_MBOX_SIZE)) {
        mbox->wait_send++;
        sys_sem_signal(&mbox->mutex);
        sys_arch_sem_wait(&mbox->not_full, 0);
        sys_arch_sem_wait(&mbox->mutex, 0);
        mbox->wait_send--;
    }

    mbox->msgs[mbox->last % SYS_MBOX_SIZE] = msg;

    if (mbox->last == mbox->first) {
        first = 1;
    } else {
        first = 0;
    }

    mbox->last++;

    if (first) {
        sys_sem_signal(&mbox->not_empty);
    }

    sys_sem_signal(&mbox->mutex);
}

/*
    err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)

    Try to post the "msg" to the mailbox. Returns ERR_MEM if this one  is full, else, ERR_OK if the "msg" is posted.
*/
err_t sys_mbox_trypost(sys_mbox_t *mb, void *msg)
{
    u8_t first;
    struct sys_mbox *mbox;
    LWIP_ASSERT("invalid mbox", (mb != NULL) && (*mb != NULL));
    mbox = *mb;

    sys_arch_sem_wait(&mbox->mutex, 0);

    LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_trypost: mbox %p msg %p\n",
                            (void *)mbox, (void *)msg));

    if ((mbox->last + 1) >= (mbox->first + SYS_MBOX_SIZE)) {
        sys_sem_signal(&mbox->mutex);
        return ERR_MEM;
    }

    mbox->msgs[mbox->last % SYS_MBOX_SIZE] = msg;

    if (mbox->last == mbox->first) {
        first = 1;
    } else {
        first = 0;
    }

    mbox->last++;

    if (first) {
        sys_sem_signal(&mbox->not_empty);
    }

    sys_sem_signal(&mbox->mutex);
    return ERR_OK;
}

/*-----------------------------------------------------------------------------------*/
/*
  Blocks the thread until a message arrives in the mailbox, but does
  not block the thread longer than "timeout" milliseconds (similar to
  the sys_arch_sem_wait() function). The "msg" argument is a result
  parameter that is set by the function (i.e., by doing "*msg =
  ptr"). The "msg" parameter maybe NULL to indicate that the message
  should be dropped.

  The return values are the same as for the sys_arch_sem_wait() function:
  Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a
  timeout.

  Note that a function with a similar name, sys_mbox_fetch(), is
  implemented by lwIP.
*/
u32_t sys_arch_mbox_fetch(sys_mbox_t *mb, void **msg, u32_t timeout)
{
    u32_t time_needed = 0;
    struct sys_mbox *mbox;
    LWIP_ASSERT("invalid mbox", (mb != NULL));
    mbox = *mb;

    /* The mutex lock is quick so we don't bother with the timeout
     stuff here. */
#ifdef LWIP_TASK_CANCEL
    sys_arch_sem_wait_ext(&mbox->mutex, 0);
#else
    sys_arch_sem_wait(&mbox->mutex, 0);
#endif

    while (mbox->first == mbox->last) {
        sys_sem_signal(&mbox->mutex);

        /* We block while waiting for a mail to arrive in the mailbox. We
           must be prepared to timeout. */
        if (timeout != 0) {
            time_needed = sys_arch_sem_wait(&mbox->not_empty, timeout);
            if (time_needed == SYS_ARCH_TIMEOUT) {
                return SYS_ARCH_TIMEOUT;
            }
        } else {
#ifdef LWIP_TASK_CANCEL
            sys_arch_sem_wait_ext(&mbox->not_empty, 0);
#else
            sys_arch_sem_wait(&mbox->not_empty, 0);
#endif
        }

#ifdef LWIP_TASK_CANCEL
        sys_arch_sem_wait_ext(&mbox->mutex, 0);
#else
        sys_arch_sem_wait(&mbox->mutex, 0);
#endif
    }

    if (msg != NULL) {
        LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_fetch: mbox %p msg %p\n", (void *)mbox, *msg));
        *msg = mbox->msgs[mbox->first % SYS_MBOX_SIZE];
    } else {
        LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_fetch: mbox %p, null msg\n", (void *)mbox));
    }

    mbox->first++;

    if (mbox->wait_send) {
        sys_sem_signal(&mbox->not_full);
    }

    sys_sem_signal(&mbox->mutex);

    return time_needed;
}

/*
    u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)

    similar to sys_arch_mbox_fetch, however if a message is not  present in the mailbox,
    it immediately returns with the code  SYS_MBOX_EMPTY.
*/
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mb, void **msg)
{
    struct sys_mbox *mbox;
    LWIP_ASSERT("invalid mbox", (mb != NULL) && (*mb != NULL));
    mbox = *mb;

    sys_arch_sem_wait(&mbox->mutex, 0);

    if (mbox->first == mbox->last) {
        sys_sem_signal(&mbox->mutex);
        return SYS_MBOX_EMPTY;
    }

    if (msg != NULL) {
        LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_tryfetch: mbox %p msg %p\n", (void *)mbox, *msg));
        *msg = mbox->msgs[mbox->first % SYS_MBOX_SIZE];
    } else {
        LWIP_DEBUGF(SYS_DEBUG, ("sys_mbox_tryfetch: mbox %p, null msg\n", (void *)mbox));
    }

    mbox->first++;

    if (mbox->wait_send) {
        sys_sem_signal(&mbox->not_full);
    }

    sys_sem_signal(&mbox->mutex);

    return 0;
}
#else

/*-----------------------------------------------------------------------------------*/
/*
    err_t sys_mbox_new(sys_mbox_t *mbox, int size)

    Creates an empty mailbox for maximum "size" elements.
*/
err_t sys_mbox_new(sys_mbox_t *mb, int size)
{
    void *msg_start;
    err_t ret = ERR_MEM;
    size_t ptr_size = sizeof(void *);

    msg_start = malloc(size * ptr_size);
    if (msg_start == NULL) {
        return ERR_MEM;
    }

    int stat = aos_queue_new(mb,msg_start,size * ptr_size,ptr_size);

    if (stat == 0) {
        ret = ERR_OK;
    }
    return ret;
}

/*-----------------------------------------------------------------------------------*/
/*
  Deallocates a mailbox. If there are messages still present in the
  mailbox when the mailbox is deallocated, it is an indication of a
  programming error in lwIP and the developer should be notified.
*/
void sys_mbox_free(sys_mbox_t *mb)
{
    void *start;

    if ((mb != NULL)) {
        start = aos_queue_buf_ptr(mb);
        if(start != NULL)
            free(start);
        aos_queue_free(mb);
    }
}

/*-----------------------------------------------------------------------------------*/
/*
    void sys_mbox_post(sys_mbox_t *mbox, void *msg)

    Posts the "msg" to the mailbox. This function have to block until  the "msg" is really posted.
*/
void sys_mbox_post(sys_mbox_t *mb, void *msg)
{
    aos_queue_send(mb,&msg,sizeof(void*));
}

/*
    err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)

    Try to post the "msg" to the mailbox. Returns ERR_MEM if this one  is full, else, ERR_OK if the "msg" is posted.
*/
err_t sys_mbox_trypost(sys_mbox_t *mb, void *msg)
{
    if (aos_queue_send(mb,&msg,sizeof(void*)) != 0)
        return ERR_MEM;
    else
        return ERR_OK;
}

/*-----------------------------------------------------------------------------------*/
/*
  Blocks the thread until a message arrives in the mailbox, but does
  not block the thread longer than "timeout" milliseconds (similar to
  the sys_arch_sem_wait() function). The "msg" argument is a result
  parameter that is set by the function (i.e., by doing "*msg =
  ptr"). The "msg" parameter maybe NULL to indicate that the message
  should be dropped.

  The return values are the same as for the sys_arch_sem_wait() function:
  Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a
  timeout.

  Note that a function with a similar name, sys_mbox_fetch(), is
  implemented by lwIP.
*/
u32_t sys_arch_mbox_fetch(sys_mbox_t *mb, void **msg, u32_t timeout)
{
    u32_t begin_ms, end_ms, elapsed_ms;
    u32_t len;
    u32_t ret;

    if (mb == NULL)
        return SYS_ARCH_TIMEOUT;

    begin_ms = sys_now();

    if( timeout != 0UL ) {

        if(aos_queue_recv(mb,timeout,msg,(unsigned int *)&len) == 0) {
            end_ms = sys_now();
            elapsed_ms = end_ms - begin_ms;
            ret = elapsed_ms;
        } else {
            ret = SYS_ARCH_TIMEOUT;
        }
    } else {

        do {
            ret = aos_queue_recv(mb,AOS_WAIT_FOREVER,msg,(unsigned int*)&len);
            if(ret == RHINO_TASK_CANCELED) {
               return SYS_ARCH_TIMEOUT;
            }
        } while(ret != 0);
        end_ms = sys_now();
        elapsed_ms = end_ms - begin_ms;

        if( elapsed_ms == 0UL ) {
            elapsed_ms = 1UL;
        }

        ret = elapsed_ms;
    }

    return ret;
}

/*
    u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)

    similar to sys_arch_mbox_fetch, however if a message is not  present in the mailbox,
    it immediately returns with the code  SYS_MBOX_EMPTY.
*/
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mb, void **msg)
{
    u32_t len;

    if(aos_queue_recv(mb,0u,msg,(unsigned int*)&len) != 0 ) {
        return SYS_MBOX_EMPTY;
    } else {
        return ERR_OK;
    }
}
#endif

/** Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return a new mutex
 *
 **/
err_t sys_mutex_new(sys_mutex_t *mutex)
{
    err_t ret = ERR_MEM;
    int stat = aos_mutex_new(mutex);

    if (stat == 0) {
        ret = ERR_OK;
    }
    return ret;
}

/** Lock a mutex
 * @param mutex the mutex to lock
 **/
void sys_mutex_lock(sys_mutex_t *mutex)
{
    aos_mutex_lock(mutex,AOS_WAIT_FOREVER);
}

/** Unlock a mutex
 * @param mutex the mutex to unlock */
void sys_mutex_unlock(sys_mutex_t *mutex)
{
    aos_mutex_unlock(mutex);
}


/** Delete a semaphore
 * @param mutex the mutex to delete
 **/
void sys_mutex_free(sys_mutex_t *mutex)
{
    aos_mutex_free(mutex);
}

/*
    u32_t sys_now(void)

    This optional function returns the current time in milliseconds (don't care  for wraparound,
    this is only used for time diffs).
*/
u32_t sys_now(void)
{
    return aos_now_ms();
}

/*
    u32_t sys_jiffies(void)
    This optional function returns the current time in milliseconds (don't care  for wraparound,
    this is only used for time diffs).
*/

u32_t sys_jiffies(void)
{
    return aos_now();
}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
    aos_task_t task_handle;

    aos_task_new_ext(&task_handle,name,thread,arg,stacksize,prio);

    return (sys_thread_t)task_handle.hdl;
}

#if SYS_LIGHTWEIGHT_PROT
/*
  This optional function does a "fast" critical region protection and returns
  the previous protection level. This function is only called during very short
  critical regions. An embedded system which supports ISR-based drivers might
  want to implement this function by disabling interrupts. Task-based systems
  might want to implement this by using a mutex or disabling tasking. This
  function should support recursive calls from the same task or interrupt. In
  other words, sys_arch_protect() could be called while already protected. In
  that case the return value indicates that it is already protected.

  sys_arch_protect() is only required if your port is supporting an operating
  system.
*/
sys_prot_t sys_arch_protect(void)
{
    aos_mutex_lock(&sys_arch_mutex, AOS_WAIT_FOREVER);
    return 0;
}

/*
  This optional function does a "fast" set of critical region protection to the
  value specified by pval. See the documentation for sys_arch_protect() for
  more information. This function is only required if your port is supporting
  an operating system.
*/
void sys_arch_unprotect(sys_prot_t pval)
{
    aos_mutex_unlock(&sys_arch_mutex);
}

#endif
/*
 * Prints an assertion messages and aborts execution.
 */
void sys_arch_assert(const char *file, int line)
{
}

/******************************************************/

int net_close(int sockfd)
{
    return 0;
}

/*
    void sys_init(void)

    Is called to initialize the sys_arch layer.
*/
void sys_init(void)
{
    aos_mutex_new(&sys_arch_mutex);
}

