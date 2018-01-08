/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <zephyr.h>
#include "timer.h"
#include <misc/util.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BLUETOOTH_DEBUG_CORE)
#include <bluetooth/log.h>

struct mtimer *g_timer_list;

#if defined(__cplusplus)
extern "C"
{
#endif
#define LOCK_HCI_CORE()
#define UNLOCK_HCI_CORE()

struct mtimer *mtimer_start(uint32_t msecs, timeout_handler handler, void *arg)
{
    struct mtimer *timer_new, *t;

    timer_new = (struct mtimer *)malloc(sizeof(struct mtimer));

    if (timer_new == NULL) {
        BT_ERR("timer maloc fail");
        return NULL;
    }

#if NO_SYS
    now = sys_now();

    if (next_timeout == NULL) {
        diff = 0;
        timeouts_last_time = now;
    } else {
        diff = now - timeouts_last_time;
    }

#endif

    timer_new->next = NULL;
    timer_new->h = handler;
    timer_new->arg = arg;

#if NO_SYS
    timer_new->time = msecs + diff;
#else
    timer_new->time = msecs;
#endif

    if (g_timer_list == NULL) {
        g_timer_list = timer_new;
        return timer_new;
    }

    if (g_timer_list->time > msecs) {
        g_timer_list->time -= msecs;
        timer_new->next = g_timer_list;
        g_timer_list = timer_new;
    } else {
        for (t = g_timer_list; t != NULL; t = t->next) {
            timer_new->time -= t->time;

            if (t->next == NULL || t->next->time > timer_new->time) {
                if (t->next != NULL) {
                    t->next->time -= timer_new->time;
                }

                timer_new->next = t->next;
                t->next = timer_new;
                break;
            }
        }
    }

    return timer_new;
}

void mtimer_stop(struct mtimer *timer)
{
    struct mtimer *prev_t, *t;

    if (g_timer_list == NULL) {
        return;
    }

    for (t = g_timer_list, prev_t = NULL; t != NULL; prev_t = t, t = t->next) {
        if (t == timer) {
            /* We have a match */
            /* Unlink from previous in list */
            if (prev_t == NULL) {
                g_timer_list = t->next;
            } else {
                prev_t->next = t->next;
            }

            /* If not the last one, add time of this one back to next */
            if (t->next != NULL) {
                t->next->time += t->time;
            }

            free(t);
            return;
        }
    }

    return;
}

void mtimer_mbox_fetch(k_mbox_t *mbox, void **msg)
{
    uint32_t time_needed;
    struct mtimer *tmptimeout;
    timeout_handler handler;
    void *arg;

again:

    if (!g_timer_list) {
        time_needed = k_mbox_fetch(mbox, msg, 0);
    } else {
        if (g_timer_list->time > 0) {
            time_needed = k_mbox_fetch(mbox, msg, g_timer_list->time);
        } else {
            time_needed = SYS_ARCH_TIMEOUT;
        }

        if (time_needed == SYS_ARCH_TIMEOUT) {
            /* If time == SYS_ARCH_TIMEOUT, a timeout occurred before a message
               could be fetched. We should now call the timeout handler and
               deallocate the memory allocated for the timeout. */
            tmptimeout = g_timer_list;
            g_timer_list = tmptimeout->next;
            handler = tmptimeout->h;
            arg = tmptimeout->arg;
            free(tmptimeout);

            if (handler != NULL) {
                /*lock the core before calling the timeout handler function. */
                LOCK_HCI_CORE();
                handler(arg);
                UNLOCK_HCI_CORE();
            }

            /* We try again to fetch a message from the mbox. */
            goto again;
        } else {
            if (!g_timer_list) {
                return;
            }

            /* If time != SYS_ARCH_TIMEOUT, a message was received before the timeout
               occured. The time variable is set to the number of
               milliseconds we waited for the message. */
            if (time_needed < g_timer_list->time) {
                g_timer_list->time -= time_needed;
            } else {
                g_timer_list->time = 0;
            }
        }
    }
}

#if defined(__cplusplus)
}
#endif
