/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 *
 * Alibaba Group retains all right, title and interest (including all
 * intellectual property rights) in and to this computer program, which is
 * protected by applicable intellectual property laws.  Unless you have
 * obtained a separate written license from Alibaba Group., you are not
 * authorized to utilize all or a part of this computer program for any
 * purpose (including reproduction, distribution, modification, and
 * compilation into object code), and you must immediately destroy or
 * return to Alibaba Group all copies of this computer program.  If you
 * are licensed by Alibaba Group, your rights to utilize this computer
 * program are limited by the terms of that license.  To obtain a license,
 * please contact Alibaba Group.
 *
 * This computer program contains trade secrets owned by Alibaba Group.
 * and, unless unauthorized by Alibaba Group in writing, you agree to
 * maintain the confidentiality of this computer program and related
 * information and to not disclose this computer program and related
 * information to any other person or entity.
 *
 * THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
 * Alibaba Group EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, TITLE, AND NONINFRINGEMENT.
 */
#include "work_queue.h"
#include "lite-utils.h"
#include "iot_import.h"

/* work queue global variable */
static LIST_HEAD(g_wq_list);

#define WQ_THREAD_NAME      "work queue"
#define PLATFORM_WAIT_INFINITE          (~0)

#define WQ_IS_RUNNING       (1)
#define WQ_IS_STOPPING      (0) /* waiting work queue thread quitting */
#define WQ_IS_STOPPED       (-1)
static int g_wq_state = WQ_IS_STOPPED;
static void *g_wq_sem;
static void *g_wq_thread;
static void *g_wq_mutex;
static unsigned int g_wq_timestamp; /* work start time */
static volatile struct work_struct *g_wq_current;

static inline int wq_is_running(void) { return g_wq_state == WQ_IS_RUNNING; }
static inline int wq_is_stopping(void) { return g_wq_state == WQ_IS_STOPPING; }
static inline int wq_is_stopped(void) { return g_wq_state == WQ_IS_STOPPED; }
static inline void wq_set_running(void) { g_wq_state = WQ_IS_RUNNING; }
static inline void wq_set_stopping(void) { g_wq_state = WQ_IS_STOPPING; }
static inline void wq_set_stopped(void) { g_wq_state = WQ_IS_STOPPED; }

#define WQ_NEW_DWORK_FLAG			(0x80000000)
#define ASSERT_FAILED_DO() \
do{ \
    while(1){\
    log_err("#####assert failed!######\r\n"); \
    HAL_SleepMs(1000); \
    }\
}while(0)

#define OS_ASSERT(expr, fmt, args...) \
do{ \
    if (!(expr)) { \
        log_err(fmt, ## args); \
        ASSERT_FAILED_DO(); \
    } \
}while(0)


static inline uint64_t time_elapsed_ms_since(uint64_t start_timestamp)
{
    uint64_t now = HAL_UptimeMs();

    return now - start_timestamp;
}


static inline int wq_is_new_dwork(struct work_struct *work)
{
    return !!(work->time_left & WQ_NEW_DWORK_FLAG);
}

static inline void wq_set_new_dwork(struct work_struct *work)
{
    work->time_left |= WQ_NEW_DWORK_FLAG;
}

static inline void wq_clear_new_dwork(struct work_struct *work)
{
    work->time_left &= ~WQ_NEW_DWORK_FLAG;
}


/**
 * queue_work - queue work on a workqueue
 * @work: work to queue
 *
 * Returns 0 if @work was already on a queue, 1 otherwise.
 */
int queue_work(struct work_struct *work)
{
    int newitem = 1;
    struct work_struct *item;

    if (wq_is_stopping())
        return 1;

    if (wq_is_stopped())
        work_queue_init();

    if (g_wq_current && time_elapsed_ms_since(g_wq_timestamp) > WQ_MAX_EXEC_TIME_MS)
        log_err("work %s block > %d ms!!!",
                g_wq_current->name, WQ_MAX_EXEC_TIME_MS);

    /* check the redundancy */
    HAL_MutexLock(g_wq_mutex);

    list_for_each_entry(item, &g_wq_list, entry, work_struct_t) {
        if (work == item) {
            newitem = 0;
            break;
        }
    }

    /* Note: don't allow one-shot work to re-queue itself */
    if (work == g_wq_current && !work->time_left)
	    newitem = 0;

    if (newitem) {
        list_add(&work->entry, &g_wq_list);
    }

    HAL_MutexUnlock(g_wq_mutex);

    /* kick the worker anyhow */
    HAL_SemaphorePost(g_wq_sem);

    return newitem;
}

/**
 * queue_delayed_work - queue work on a workqueue after delay
 * @dwork: delayable work to queue
 * @delay: ms to wait before queueing
 *
 * Returns 0 if @work was already on a queue, 1 otherwise.
 */
int queue_delayed_work(struct work_struct *dwork, unsigned int delay_ms)
{
    OS_ASSERT(!(delay_ms & WQ_NEW_DWORK_FLAG),
            "delay work %s time %d over range",
            dwork->name, delay_ms);

    HAL_MutexLock(g_wq_mutex);
    dwork->time_left = delay_ms;
    wq_set_new_dwork(dwork);
    HAL_MutexUnlock(g_wq_mutex);

    return queue_work(dwork);
}

/**
 * cancel_work - cancel a work
 * @work: the work to cancel
 * @Note: cancel_work may fail to cancel a cycle work because cycle work
 *          will keep re-queue itself to workqueue
 * Returns 1 if @work was found and deleted from the workqueue list,
 *          3 if @work not found in work_queue(work finished or never exist).
 *          0 work_queue is uninitialized(work_queue module quit)
 */
int cancel_work(struct work_struct *work)
{
    struct work_struct *item;
    int cancel = 3;

    if (!wq_is_running())
        return 0;

retry:
    /* A work can be in 3 state: in queue, working, deleted(not exist) */
    HAL_MutexLock(g_wq_mutex);
    list_for_each_entry(item, &g_wq_list, entry, work_struct_t) {
        if (work == item) {
            list_del(&work->entry);
            cancel = 1;
            break;
        }
    }
    HAL_MutexUnlock(g_wq_mutex);

    if (work == g_wq_current) {
        log_info("work %s is working, wait it exit", work->name);
        while (work == g_wq_current)
            HAL_SleepMs(100);
        goto retry;
    }

    return cancel;
}


static void *worker_thread(void *arg)
{
#define MAX_COUNTDOWN_TIME		(~0)
    struct work_struct *item, *work = NULL;
    uint64_t start, time_elpased, min_time_left = MAX_COUNTDOWN_TIME;
    int prio;

    log_debug("");
    //OS_CHECK_PARAMS(MAX_COUNTDOWN_TIME == ~0);

    while (wq_is_running()) {

        start = HAL_UptimeMs();
        HAL_SemaphoreWait(g_wq_sem, min_time_left);
        time_elpased = time_elapsed_ms_since(start);

        work = NULL;
        prio = LOWEST_WORK_PRIO;
        min_time_left = MAX_COUNTDOWN_TIME;

        HAL_MutexLock(g_wq_mutex);
        list_for_each_entry(item, &g_wq_list, entry, work_struct_t) {
            //log_debug("name:%s, time_left:0x%x", item->name, item->time_left);

            if (time_elpased >= item->time_left) {
                item->time_left = 0;
                /* ready to go */
            }

            if (item->time_left) {/* delayed work */
                if (wq_is_new_dwork(item)) {
                    wq_clear_new_dwork(item);
                } else {
                    item->time_left -= time_elpased;
                }
                if (min_time_left > item->time_left) {
                    min_time_left = item->time_left;
                }
            } else if (item->prio < prio) {
                prio = item->prio;
                work = item;
                //log_debug("name:%s, prio:%d,", item->name, item->prio);
            }
        }
        HAL_MutexUnlock(g_wq_mutex);

        if (work) {
            /* del the work first to allow the work re-queue itself */
            HAL_MutexLock(g_wq_mutex);
            list_del(&work->entry);
            g_wq_timestamp = HAL_UptimeMs();
            g_wq_current = work;
            HAL_MutexUnlock(g_wq_mutex);

            work->func(work);

            g_wq_current = NULL;
            min_time_left = 0;
        } else if (min_time_left != MAX_COUNTDOWN_TIME) {
            // log_debug("work queue: countdown timer 0x%x ms", min_time_left);
        } else {
            /* goto idle loop */
            log_info("work queue: idle...");
        }
    }

    log_debug("");

    HAL_ThreadDetach(g_wq_thread);
    return NULL;
}

int work_queue_init(void)
{
    int ret = -1;
    if (wq_is_running())
        return 0;

    g_wq_sem = (void *)HAL_SemaphoreCreate();
    g_wq_mutex = HAL_MutexCreate();

    INIT_LIST_HEAD(&g_wq_list);
    wq_set_running();

    hal_os_thread_param_t threadParams = {
            .priority = os_thread_priority_normal,
            .stack_addr = 0,
            .stack_size = 2048,
            .detach_state = 0,
            .name = "work_queue",
    };

    int stack_used = 0;
    ret = HAL_ThreadCreate(&g_wq_thread, worker_thread, NULL, &threadParams, &stack_used);
    OS_ASSERT(ret == 0, "thread create failed!");

    return 0;
}

int work_queue_stop(void)
{
    wq_set_stopping();
    /* wake up worker thread */
    HAL_SemaphorePost(g_wq_sem);
    log_debug("");

    return 0;
}

int work_queue_exit(void)
{
    if (!wq_is_running())
        return 0;

    work_queue_stop();

    if (g_wq_thread) {
        HAL_ThreadDetach(g_wq_thread);
        g_wq_thread = NULL;
    }
    log_debug("");

    HAL_MutexLock(g_wq_mutex);
    if (!list_empty(&g_wq_list)) {
        struct work_struct *item;
        log_err("dump unfinished work in queue");
        list_for_each_entry(item, &g_wq_list, entry, work_struct_t) {
            log_err("\t\twork: %s", item->name);
        }
        OS_ASSERT(0, "unfinished work in queue!!!");
    }
    HAL_MutexUnlock(g_wq_mutex);

    if (g_wq_sem) {
        HAL_SemaphoreDestroy(g_wq_sem);
        g_wq_sem = NULL;
    }
    if (g_wq_mutex) {
        HAL_MutexDestroy(g_wq_mutex);
        g_wq_mutex = NULL;
    }

    wq_set_stopped();
    log_debug("");

    return 0;
}
