/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include <misc/stack.h>
#include "misc/util.h"
#include "work.h"

#define SCHEDULER_TASK_PRIO         32
#define MSG_LIST_MAX_NODE           100
_stack_element_t schedule_task_stack[2048];
static struct k_thread schedule_task_thread_data;

struct list_node {
    int id;
    void *msg;
    struct list_node *next;
};

struct msg_list{
    int num_node;
    unsigned int key;
    struct list_node *head;
    struct list_node *tail;
};

struct msg_list g_scheduler_msg_list;

extern struct k_work_q g_work_queue;

void bt_mesh_schedule_message_handle(int msg_id, void *msg);

int schedule_list_try_lock(void)
{
    struct msg_list *list = &g_scheduler_msg_list;

    list->key = irq_lock();
    return 0;
}

int schedule_list_unlock(void)
{
    struct msg_list *list = &g_scheduler_msg_list;

    if (list->key != 0x55aa55aa) {
        irq_unlock(list->key);
        list->key = 0x55aa55aa;
    }
    return 0;
}

void schedule_list_init(void)
{
    struct msg_list *list = &g_scheduler_msg_list;

    list->num_node = 0;
    list->key = 0x55aa55aa;
    list->head = NULL;
    list->tail = NULL;
}

int schedule_list_append(struct list_node *node)
{
    struct list_node *tail;
    struct msg_list *list = &g_scheduler_msg_list;

    if (list->num_node >= MSG_LIST_MAX_NODE)
        return -1;

    schedule_list_try_lock();

    node->next = NULL;
    tail = list->tail;
    if (tail) {
        tail->next = node;
        list->tail = node;
    } else {
        list->head = list->tail = node;
    }
    list->num_node++;

    schedule_list_unlock();

    return 0;
}

struct list_node *schedule_list_get(void)
{
    struct list_node *head;
    struct msg_list *list = &g_scheduler_msg_list;

    if (list->num_node <= 0)
        return NULL;

    schedule_list_try_lock();

    head = list->head;
    if (head)
        list->head = head->next;
    else
        list->head = NULL;

    if (list->tail == head) {
        list->tail = NULL;
    }
    list->num_node--;

    schedule_list_unlock();

    return head;
}


int scheduler_msg_send(int msg_id, void *pMsg)
{
    struct list_node *msg;
    int ret;

    msg = k_malloc(sizeof(struct list_node));
    if (msg == NULL) {
        return -1;
    }

    msg->id = msg_id;
    msg->msg = pMsg;

    ret = schedule_list_append(msg);
    if (ret) {
        k_free(msg);
    }
    return ret;
}

int scheduler_msg_get(int *msg_id, void **pMsg)
{
    struct list_node *msg;

    msg = schedule_list_get();
    if (msg) {
        *msg_id = msg->id;
        *pMsg = msg->msg;

        k_free(msg);
        return 0;
    }

    return -1;
}

void scheduler_process_msg(void)
{
    void *msg;
    int msg_id;

    while (scheduler_msg_get(&msg_id, &msg) == 0) {
        bt_mesh_schedule_message_handle(msg_id, msg);
    }
}



void scheduler_loop(struct k_poll_event *events)
{
    struct k_work *work;
    uint32_t now;
    int delayed_ms = 0;

    while (1) {
        delayed_ms = K_FOREVER;

        if (k_queue_is_empty(&g_work_queue.queue) == 0) {
            work = k_queue_first_entry(&g_work_queue.queue);
            now = k_uptime_get_32();

            delayed_ms = 0;
            if (now < (work->start_ms + work->timeout)) {
                delayed_ms = work->start_ms + work->timeout - now;
            }

            if (delayed_ms == 0) {
                delayed_ms = 1;
            }
        }

        if (k_queue_is_empty(&g_work_queue.queue) == 0) {
            work = k_queue_first_entry(&g_work_queue.queue);
            now = k_uptime_get_32();

            if (now >= (work->start_ms + work->timeout)) {
                k_queue_remove(&g_work_queue.queue, work);
                if (atomic_test_and_clear_bit(work->flags, K_WORK_STATE_PENDING)) {
                    work->handler(work);
                }
            }
        }

        scheduler_process_msg();

        k_sleep(10);
    }
}

static void schedule_task(void *p1, void *p2, void *p3)
{
    static struct k_poll_event events[1];

    scheduler_loop(events);
}

void schedule_task_init(void)
{
    schedule_list_init();
    k_thread_create(&schedule_task_thread_data, schedule_task_stack,
                    K_THREAD_STACK_SIZEOF(schedule_task_stack), schedule_task, NULL,
                    NULL, NULL, SCHEDULER_TASK_PRIO, 0, K_NO_WAIT);
}

