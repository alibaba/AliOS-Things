/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/* system includes */
#include <aos/aos.h>
#include "sal_err.h"

#include "sal_arch.h"
#include "internal/sal_arch_internal.h"


static aos_mutex_t sal_arch_mutex;

//#define      NET_TASK_NUME 2
//#define      NET_TASK_STACK_SIZE 1024

//ktask_t       g_net_task[NET_TASK_NUME];
//cpu_stack_t  g_net_task_stack[NET_TASK_NUME][NET_TASK_STACK_SIZE];

/*-----------------------------------------------------------------------------------*/
/*
    err_t sal_sem_new(sal_sem_t *sem, uint8_t count)
    Creates a new semaphore.
*/
err_t sal_sem_new(sal_sem_t *sem, uint8_t count)
{
    err_t ret = ERR_MEM;
    int stat = aos_sem_new(sem, count);

    if (stat == 0) {
        ret = ERR_OK;
    }
    return ret;
}

/*-----------------------------------------------------------------------------------*/
/*
    void sal_sem_free(sal_sem_t *sem)

    Deallocates a semaphore.
*/
void sal_sem_free(sal_sem_t *sem)
{
    if ((sem != NULL)) {
        aos_sem_free(sem);
    }
}


/*-----------------------------------------------------------------------------------*/
/*
    void sal_sem_signal(sal_sem_t *sem)

    Signals a semaphore.
*/
void sal_sem_signal(sal_sem_t *sem)
{
    aos_sem_signal(sem);
}


/*-----------------------------------------------------------------------------------*/
/*
  Blocks the thread while waiting for the semaphore to be
  signaled. If the "timeout" argument is non-zero, the thread should
  only be blocked for the specified time (measured in
  milliseconds).

  If the timeout argument is non-zero, the return value is the number of
  milliseconds spent waiting for the semaphore to be signaled. If the
  semaphore wasn't signaled within the specified time, the return value is
  SAL_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
  (i.e., it was already signaled), the function may return zero.

  Notice that SAL implements a function with a similar name,
  sal_sem_wait(), that uses the sal_arch_sem_wait() function.
*/
uint32_t sal_arch_sem_wait(sal_sem_t *sem, uint32_t timeout)
{
    uint32_t begin_ms, end_ms, elapsed_ms;
    uint32_t ret;

    if (sem == NULL) {
        return SAL_ARCH_TIMEOUT;
    }

    begin_ms = sal_now();

    if ( timeout != 0UL ) {
        ret = aos_sem_wait(sem, timeout);
        if (ret == 0) {
            end_ms = sal_now();

            elapsed_ms = end_ms - begin_ms;

            ret = elapsed_ms;
        } else {
            ret = SAL_ARCH_TIMEOUT;
        }
    } else {
        while ( !(aos_sem_wait(sem, AOS_WAIT_FOREVER) == 0));
        end_ms = sal_now();

        elapsed_ms = end_ms - begin_ms;

        if ( elapsed_ms == 0UL ) {
            elapsed_ms = 1UL;
        }

        ret = elapsed_ms;
    }

    return ret;
}

/*-----------------------------------------------------------------------------------*/
/*
    err_t sys_mbox_new(sys_mbox_t *mbox, int size)

    Creates an empty mailbox for maximum "size" elements.
*/
err_t sal_mbox_new(sal_mbox_t *mb, int size)
{
    void *msg_start;
    err_t ret = ERR_MEM;

    msg_start = (void*)aos_malloc(size * sizeof(void *));
    if (msg_start == NULL) {
        return ERR_MEM;
    }

    int stat = aos_queue_new(mb,msg_start,size * sizeof(void *),sizeof(void *));

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
void sal_mbox_free(sal_mbox_t *mb)
{
    void *start;

    if ((mb != NULL)) {
        start = aos_queue_buf_ptr(mb);
        if(start != NULL)
            aos_free(start);
        aos_queue_free(mb);
    }
}

/*-----------------------------------------------------------------------------------*/
/*
    void sys_mbox_post(sys_mbox_t *mbox, void *msg)

    Posts the "msg" to the mailbox. This function have to block until  the "msg" is really posted.
*/
void sal_mbox_post(sal_mbox_t *mb, void *msg)
{
    aos_queue_send(mb,&msg,sizeof(void*));
}

/*
    err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)

    Try to post the "msg" to the mailbox. Returns ERR_MEM if this one  is full, else, ERR_OK if the "msg" is posted.
*/
err_t sal_mbox_trypost(sal_mbox_t *mb, void *msg)
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
u32_t sal_arch_mbox_fetch(sal_mbox_t *mb, void **msg, u32_t timeout)
{
    u32_t begin_ms, end_ms, elapsed_ms;
    u32_t len;
    u32_t ret;

    if (mb == NULL)
        return SAL_ARCH_TIMEOUT;

    begin_ms = sal_now();

    if( timeout != 0UL ) {

        if(aos_queue_recv(mb,timeout,msg,&len) == 0) {
            end_ms = sal_now();
            elapsed_ms = end_ms - begin_ms;
            ret = elapsed_ms;
        } else {
            ret = SAL_ARCH_TIMEOUT;
        }
    } else {
        while(aos_queue_recv(mb,AOS_WAIT_FOREVER,msg,&len) != 0);
        end_ms = sal_now();
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
    it immediately returns with the code  SAL_MBOX_EMPTY.
*/
u32_t sal_arch_mbox_tryfetch(sal_mbox_t *mb, void **msg)
{
    u32_t len;

    if(aos_queue_recv(mb,0u,msg,&len) != 0 ) {
        return SAL_MBOX_EMPTY;
    } else {
        return ERR_OK;
    }
}


/** Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return a new mutex
 *
 **/
err_t sal_mutex_new(sal_mutex_t *mutex)
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
void sal_mutex_lock(sal_mutex_t *mutex)
{
    aos_mutex_lock(mutex, AOS_WAIT_FOREVER);
}

/** Unlock a mutex
 * @param mutex the mutex to unlock */
void sal_mutex_unlock(sal_mutex_t *mutex)
{
    aos_mutex_unlock(mutex);
}


/** Delete a semaphore
 * @param mutex the mutex to delete
 **/
void sal_mutex_free(sal_mutex_t *mutex)
{
    aos_mutex_free(mutex);
}

/*
    uint32_t sal_now(void)

    This optional function returns the current time in milliseconds (don't care  for wraparound,
    this is only used for time diffs).
*/
uint32_t sal_now(void)
{
    return aos_now_ms();
}



#if SAL_LIGHTWEIGHT_PROT
/*
  This optional function does a "fast" critical region protection and returns
  the previous protection level. This function is only called during very short
  critical regions. An embedded system which supports ISR-based drivers might
  want to implement this function by disabling interrupts. Task-based systems
  might want to implement this by using a mutex or disabling tasking. This
  function should support recursive calls from the same task or interrupt. In
  other words, sal_arch_protect() could be called while already protected. In
  that case the return value indicates that it is already protected.

  sal_arch_protect() is only required if your port is supporting an operating
  system.
*/
sal_prot_t sal_arch_protect(void)
{
    aos_mutex_lock(&sal_arch_mutex, AOS_WAIT_FOREVER);
    return 0;
}

/*
  This optional function does a "fast" set of critical region protection to the
  value specified by pval. See the documentation for sal_arch_protect() for
  more information. This function is only required if your port is supporting
  an operating system.
*/
void sal_arch_unprotect(sal_prot_t pval)
{
    aos_mutex_unlock(&sal_arch_mutex);
}

#endif
/*
 * Prints an assertion messages and aborts execution.
 */
void sal_arch_assert(const char *file, int line)
{
}

/*
    void sal_mutet_init(void)

    Is called to initialize the sal_arch layer.
*/
void sal_mutex_arch_init(void)
{
    aos_mutex_new(&sal_arch_mutex);
}

void sal_mutex_arch_free(void)
{
    aos_mutex_free(&sal_arch_mutex);
}

