/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <zephyr.h>
#include <string.h>
#include <misc/util.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BLUETOOTH_DEBUG_CORE)

#include <bluetooth/log.h>
#include "mbox.h"

struct timer *g_timer_list;

#if defined(__cplusplus)
extern "C"
{
#endif

int k_mbox_new(k_mbox_t **mb, int size)
{
    k_mbox_t *mbox;
    UNUSED(size);

    mbox = (k_mbox_t *)malloc(sizeof(k_mbox_t));

    if (mbox == NULL) {
        return ERR_MEM;
    }

    memset(mbox, 0, sizeof(k_mbox_t));

    mbox->first = mbox->last = 0;
    k_sem_init(&mbox->not_empty, 0, 0);
    k_sem_init(&mbox->not_full, 0, 0);
    k_sem_init(&mbox->mutex, 1, 1);
    mbox->wait_send = 0;

    *mb = mbox;
    BT_DBG("k_mbox_new: mbox 0x%lx\n", mbox);
    return ERR_OK;
}

/*-----------------------------------------------------------------------------------*/
/*
  Deallocates a mailbox. If there are messages still present in the
  mailbox when the mailbox is deallocated, it is an indication of a
  programming error in lwIP and the developer should be notified.
*/
void k_mbox_free(k_mbox_t *mb)
{
    if (mb != NULL) {
        k_mbox_t *mbox = mb;

        k_sem_take(&mbox->mutex, K_FOREVER);

        k_sem_delete(&mbox->not_empty);
        k_sem_delete(&mbox->not_full);
        k_sem_delete(&mbox->mutex);
        BT_DBG("sys_mbox_free: mbox 0x%lx\n", mbox);
        free(mbox);
    }
}

/*-----------------------------------------------------------------------------------*/
/*
    void sys_mbox_post(k_mbox_t *mbox, void *msg)

    Posts the "msg" to the mailbox. This function have to block until  the "msg" is really posted.
*/
void k_mbox_post(k_mbox_t *mb, void *msg)
{
    u8_t first;
    k_mbox_t *mbox;

    if (NULL == mb) {
        BT_ERR("invaild mbox");
        return;
    }

    mbox = mb;

    k_sem_take(&mbox->mutex, K_FOREVER);

    BT_DBG("sys_mbox_post: mbox %p msg %p\n", (void *)mbox, (void *)msg);

    while ((mbox->last + 1) >= (mbox->first + K_MBOX_SIZE)) {
        mbox->wait_send++;
        k_sem_give(&mbox->mutex);
        k_sem_take(&mbox->not_full, K_FOREVER);
        k_sem_take(&mbox->mutex, K_FOREVER);
        mbox->wait_send--;
    }

    mbox->msgs[mbox->last % K_MBOX_SIZE] = msg;

    if (mbox->last == mbox->first) {
        first = 1;
    } else {
        first = 0;
    }

    mbox->last++;

    if (first) {
        k_sem_give(&mbox->not_empty);
    }

    k_sem_give(&mbox->mutex);
}

/*
    err_t k_mbox_trypost(k_mbox_t *mbox, void *msg)

    Try to post the "msg" to the mailbox. Returns ERR_MEM if this one  is full, else, ERR_OK if the "msg" is posted.
*/
int k_mbox_trypost(k_mbox_t *mb, void *msg)
{
    u8_t first;
    k_mbox_t *mbox;

    if (NULL == mb) {
        BT_ERR("invaild mbox");
        return ERR_MEM;
    }

    mbox = mb;

    k_sem_take(&mbox->mutex, K_FOREVER);

    BT_DBG("k_mbox_trypost: mbox %p msg %p\n", (void *)mbox, (void *)msg);

    if ((mbox->last + 1) >= (mbox->first + K_MBOX_SIZE)) {
        k_sem_give(&mbox->mutex);
        return ERR_MEM;
    }

    mbox->msgs[mbox->last % K_MBOX_SIZE] = msg;

    if (mbox->last == mbox->first) {
        first = 1;
    } else {
        first = 0;
    }

    mbox->last++;

    if (first) {
        k_sem_give(&mbox->not_empty);
    }

    k_sem_give(&mbox->mutex);
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
int k_mbox_fetch(k_mbox_t *mb, void **msg, uint32_t timeout)
{
    uint32_t time_needed = 0;
    k_mbox_t *mbox;

    if (NULL == mb) {
        BT_ERR("invaild mbox");
        return ERR_MEM;
    }

    mbox = mb;
    BT_DBG("mbox %p\n", mbox);

    /* The mutex lock is quick so we don't bother with the timeout
     stuff here. */
    k_sem_take(&mbox->mutex, K_FOREVER);

    BT_DBG("mutex taken\n");

    while (mbox->first == mbox->last) {
        k_sem_give(&mbox->mutex);

        /* We block while waiting for a mail to arrive in the mailbox. We
           must be prepared to timeout. */
        if (timeout != 0) {
            time_needed = k_sem_take(&mbox->not_empty, timeout);

            if (time_needed == SYS_ARCH_TIMEOUT) {
                return SYS_ARCH_TIMEOUT;
            }
        } else {
            k_sem_take(&mbox->not_empty, K_FOREVER);
        }

        k_sem_take(&mbox->mutex, K_FOREVER);
    }

    if (msg != NULL) {
        BT_DBG("sys_mbox_fetch: mbox %p msg %p\n", (void *)mbox, *msg);
        *msg = mbox->msgs[mbox->first % K_MBOX_SIZE];
    } else {
        BT_DBG("sys_mbox_fetch: mbox %p, null msg\n", (void *)mbox);
    }

    mbox->first++;

    if (mbox->wait_send) {
        k_sem_give(&mbox->not_full);
    }

    k_sem_give(&mbox->mutex);

    return time_needed;
}

/*
    uint32_t sys_arch_mbox_tryfetch(k_mbox_t *mbox, void **msg)

    similar to sys_arch_mbox_fetch, however if a message is not  present in the mailbox,
    it immediately returns with the code  SYS_MBOX_EMPTY.
*/
int k_mbox_tryfetch(k_mbox_t *mb, void **msg)
{
    k_mbox_t *mbox;

    if (NULL == mb) {
        BT_ERR("invaild mbox");
        return ERR_MEM;
    }

    mbox = mb;

    k_sem_take(&mbox->mutex, K_FOREVER);

    if (mbox->first == mbox->last) {
        k_sem_give(&mbox->mutex);
        return SYS_MBOX_EMPTY;
    }

    if (msg != NULL) {
        BT_DBG("k_mbox_tryfetch: mbox %p msg %p\n", (void *)mbox, *msg);
        *msg = mbox->msgs[mbox->first % K_MBOX_SIZE];
    } else {
        BT_DBG("k_mbox_tryfetch: mbox %p, null msg\n", (void *)mbox);
    }

    mbox->first++;

    if (mbox->wait_send) {
        k_sem_give(&mbox->not_full);
    }

    k_sem_give(&mbox->mutex);

    return 0;
}

#if defined(__cplusplus)
}
#endif
