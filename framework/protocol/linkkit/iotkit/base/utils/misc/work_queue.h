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

#ifndef _WORK_QUEUE_H_
#define _WORK_QUEUE_H_

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#include "lite-log.h"
#include "lite-list.h"

struct work_struct;

typedef void (*work_func_t)(struct work_struct *work);

typedef struct work_struct {
    list_head_t entry;
    work_func_t func; /* work entry */
    void *data; /* data used by func */
    uint32_t time_left;   /* for delayed work */
    char prio; /* lower prio means high priority */
    const char *name; /* work item name */
}work_struct_t;

#define ACCS_WORK_PRIO                  (1)
#define SUBDEV_LOGIN_WORK_PRIO          (2)
#define OTA_WORK_PRIO                   (3)
#define DOWNLOAD_WORK_PRIO              (4)

#define DEFAULT_WORK_PRIO               (LOWEST_WORK_PRIO - 1)
#define LOWEST_WORK_PRIO                (127)

/* work queue max execution time(block time) for work */
#define WQ_MAX_EXEC_TIME_MS             (10 * 1000)

int work_queue_init(void);
int work_queue_exit(void);

/**
 * queue_work - queue work on a workqueue
 * @work: work to queue
 * @note: worker will not free this work_struct
 *
 * Returns 0 if @work was already on a queue, 1 otherwise.
 */
int queue_work(struct work_struct *work);

/**
 * queue_delayed_work - queue work on a workqueue after delay
 * @dwork: delayable work to queue
 * @delay: ms to wait before queueing
 * @note: worker will not free this work_struct
 *
 * Returns 0 if @work was already on a queue, 1 otherwise.
 */
int queue_delayed_work(struct work_struct *dwork, unsigned int delayed_ms);

/**
 * cancel_work - cancel a work
 * @work: the work to cancel
 * @Note: cancel_work may fail to cancel a cycle work because cycle work
 *          will keep re-queue itself to workqueue
 * Returns 1 if @work was found and deleted from the workqueue list,
 *          3 if @work not found in work_queue(work finished or never exist).
 *          0 work_queue is uninitialized(work_queue module quit)
 */
int cancel_work(struct work_struct *work);

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
