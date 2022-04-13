/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <string.h>

#include <aos/kernel.h>
//#include <aos/osal_debug.h>

#include <uservice/uservice.h>

#include "internal.h"

#define TAG "utask"

static int g_utask_softwdt_timeout = 0;
static AOS_SLIST_HEAD(utask_list);

#define LOOP_TIME_MS 1000
#define MIN_SOFTWDT_TIME 2000 /*ms*/

#if 0
static void task_will(void *args)
{
    utask_t *task = (utask_t*)args;

    LOGE(TAG, "utask: %s crash!", aos_task_get_name(&task->task));
    if (task->current_rpc) {
        LOGE(TAG, "uservice_name: %s, rpc_cmd: %d", task->current_rpc->srv->name, task->current_rpc->cmd_id);
    }
}
#endif

static void utask_entry(void *data)
{
    utask_t *task = (utask_t *)data;

    rpc_t  rpc;
    size_t size;

    while (task->running) {
        if (aos_queue_recv(&task->queue, LOOP_TIME_MS, &rpc, &size) == 0) {

#if defined(CONFIG_DEBUG) && defined(CONFIG_DEBUG_UTASK)
            int count = aos_queue_get_count(&task->queue) + 1;
            if (count > task->queue_max_used)
                task->queue_max_used = count;

            struct rpc_record *node;
            int               found = 0;
            slist_for_each_entry(&task->rpc_reclist, node, struct rpc_record, next) {
                if (node->cmd_id == rpc.cmd_id && node->srv == rpc.srv) {
                    node->count++;
                    found = 1;
                    break;
                }
            }

            if (found == 0) {
                node = aos_zalloc(sizeof(struct rpc_record));
                node->cmd_id = rpc.cmd_id;
                node->srv = rpc.srv;
                node->count = 1;
                slist_add(&node->next, &task->rpc_reclist);
            }
#endif
            if (rpc.srv->process_rpc) {
                task->current_rpc = &rpc;

                if (g_utask_softwdt_timeout >= MIN_SOFTWDT_TIME) {
                    //aos_task_wdt_attach(task_will, task);
                    //aos_task_wdt_feed(g_utask_softwdt_timeout);
                    // simulaton timeout
                    // sleep(11);
                    //uservice_lock(rpc.srv);
                    rpc.srv->process_rpc(rpc.srv->context, &rpc);
                    //uservice_unlock(rpc.srv);
                    //aos_task_wdt_detach();
                } else {
                    //uservice_lock(rpc.srv);
                    rpc.srv->process_rpc(rpc.srv->context, &rpc);
                    //uservice_unlock(rpc.srv);
                }
                task->current_rpc = NULL;
            } else {
                rpc_reply(&rpc);
            }
        }
    }

    aos_sem_signal(&task->running_wait);
}

utask_t *utask_new(const char *name, size_t stack_size, int queue_count, int prio)
{
    if (stack_size <= 0 || queue_count <= 0)
        return NULL;
    int queue_buffer_size = queue_count * sizeof(rpc_t);

    utask_t *task = aos_zalloc(sizeof(utask_t) + queue_buffer_size);

    if (task == NULL)
        return NULL;

    task->running = 1;
    slist_init(&task->uservice_lists);
    slist_init(&task->rpc_buffer_gc_cache);
    slist_add_tail(&task->node, &utask_list);

    task->queue_count = queue_count;
    task->queue_buffer = (uint8_t*)task + sizeof(utask_t);

    if (aos_queue_new(&task->queue, task->queue_buffer, queue_buffer_size, sizeof(rpc_t)) != 0)
        goto out0;

    if (aos_mutex_new(&task->mutex) != 0)
        goto out1;

    if (aos_sem_new(&task->running_wait, 0) != 0)
        goto out2;

    if (aos_task_new_ext(&task->task, name ? name : "utask", utask_entry, task, stack_size, prio) != 0)
        goto out3;

    return task;

out3:
    aos_sem_free(&task->running_wait);
out2:
    aos_mutex_free(&task->mutex);
out1:
    aos_queue_free(&task->queue);
out0:
    aos_free(task);

    return NULL;
}

void utask_join(utask_t *task)
{
    aos_assert(task);

    task->running = 0;
    aos_sem_wait(&task->running_wait, AOS_WAIT_FOREVER);

    slist_del(&task->node, &utask_list);

    uservice_t *node;
    slist_t *tmp;
    slist_for_each_entry_safe(&task->uservice_lists, tmp, node, uservice_t, next) {
        utask_remove(task, node);
    }

    aos_sem_free(&task->running_wait);
    aos_mutex_free(&task->mutex);
    aos_queue_free(&task->queue);
    aos_free(task);
}

void utask_destroy(utask_t *task)
{
    aos_assert(task);
    utask_join(task);
}

void utask_add(utask_t *task, uservice_t *srv)
{
    aos_assert(task && srv);

    if (srv->task == NULL) {
        TASK_LOCK(task);
        srv->task = task;
        slist_add_tail(&srv->next, &task->uservice_lists);
        TASK_UNLOCK(task);
    }
}

void utask_remove(utask_t *task, uservice_t *srv)
{
    aos_assert(task && srv);

    TASK_LOCK(task);

#if defined(CONFIG_DEBUG) && defined(CONFIG_DEBUG_UTASK)
    struct rpc_record *node;
    slist_t *tmp;
    slist_for_each_entry_safe(&task->rpc_reclist, tmp, node, struct rpc_record, next) {
        if (node->srv == srv) {
            slist_del(&node->next, &task->rpc_reclist);
            free(node);
        }
    }
#endif

    slist_del(&srv->next, &task->uservice_lists);
    srv->task = NULL;

    TASK_UNLOCK(task);
}

void utask_lock(utask_t *task)
{
    aos_assert(task);
    TASK_LOCK(task);
}

void utask_unlock(utask_t *task)
{
    aos_assert(task);
    TASK_UNLOCK(task);
}

void utask_set_softwdt_timeout(int ms)
{
    if (ms == 0) {
        g_utask_softwdt_timeout = 0;
    } else {
        g_utask_softwdt_timeout = (ms < MIN_SOFTWDT_TIME) ? MIN_SOFTWDT_TIME : ms;
    }
}

uservice_t *task_get(utask_t *task, const char *name)
{
    aos_assert(task);

    uservice_t *node;

    TASK_LOCK(task);

    slist_for_each_entry(&task->uservice_lists, node, uservice_t, next) {
        if (strcmp(node->name, name) == 0) {
            TASK_UNLOCK(task);
            return node;
        }
    }

    TASK_UNLOCK(task);

    return NULL;
}

void tasks_debug()
{
    utask_t *utask;
    slist_for_each_entry(&utask_list, utask, utask_t, node) {
#if defined(CONFIG_DEBUG) && defined(CONFIG_DEBUG_UTASK)
        struct rpc_record *rpc;

        printf("utask: %s, queue total count: %d, max used: %d\n------------------------------------------------------\n",
               aos_task_get_name(&utask->task), utask->queue_count, utask->queue_max_used);
        printf("\t%05s\t\%05s\tSERVER\n", "CMD_ID", "COUNT");
        slist_for_each_entry(&utask->rpc_reclist, rpc, struct rpc_record, next) {
            printf("\t%05d\t%05d\t%s\n", rpc->cmd_id, rpc->count, rpc->srv->name);
        }
        printf("\n");
#endif
    }
}
