/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_api.h"
#include "ipc.h"
#include "utask.h"

#define MAX_MSG_SIZE 0xffffffffu

typedef struct {
    kbuf_queue_t *buf_queue;
    klist_t       node;
    uint32_t      owner[RHINO_CONFIG_PROC_MSG_MAX_OWNER_NUM];
    uint32_t      ref_cnt;
} proc_msg_t;

static klist_t   proc_msg_list;
static kmutex_t  proc_msg_mutex;
static int       proc_msg_inited = 0;

static void msg_dyn_create(kbuf_queue_t **queue, const name_t *name,
                           size_t size, size_t max_msg, uint32_t key)
{
    CPSR_ALLOC();

    kbuf_queue_t *queue_obj;

    queue_obj = krhino_mm_alloc(sizeof(kbuf_queue_t));

    if (queue_obj == NULL) {
        *queue = NULL;
        return;
    }

    queue_obj->buf = krhino_mm_alloc(size);

    if (queue_obj->buf == NULL) {
        *queue = NULL;
        krhino_mm_free(queue_obj);
        return;
    }

    /* init the queue blocked list */
    klist_init(&queue_obj->blk_obj.blk_list);

    queue_obj->buf                = queue_obj->buf;
    queue_obj->cur_num            = 0u;
    queue_obj->peak_num           = 0u;
    queue_obj->max_msg_size       = max_msg;
    queue_obj->blk_obj.name       = name;
    queue_obj->blk_obj.blk_policy = BLK_POLICY_PRI;
    queue_obj->mm_alloc_flag      = K_OBJ_DYN_ALLOC;

    queue_obj->blk_obj.obj_type = RHINO_BUF_QUEUE_OBJ_TYPE;

    ringbuf_init(&(queue_obj->ringbuf), queue_obj->buf, size, RINGBUF_TYPE_DYN, max_msg);
    queue_obj->min_free_buf_size  = queue_obj->ringbuf.freesize;

    RHINO_CRITICAL_ENTER();

#if (RHINO_CONFIG_KOBJ_LIST > 0)
    klist_insert(&(g_kobj_list.buf_queue_head), &queue_obj->buf_queue_item);
#endif

    queue_obj->key = key;

    RHINO_CRITICAL_EXIT();

    *queue = queue_obj;
}

size_t krhino_msg_get(uint32_t key, uint32_t flg, size_t size)
{
    kbuf_queue_t *queue = NULL;
    klist_t      *head;
    klist_t      *iter;
    proc_msg_t   *proc_msg;
    int           cur_pid;
    int           full = 0;
    int           i;

    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();
    if (proc_msg_inited == 0) {
        klist_init(&proc_msg_list);
        krhino_mutex_create(&proc_msg_mutex, "proc_msg_mutex");
        proc_msg_inited = 1;
    }
    RHINO_CRITICAL_EXIT();

    krhino_mutex_lock(&proc_msg_mutex, RHINO_WAIT_FOREVER);
    cur_pid = krhino_cur_task_get()->pid;
    head = &proc_msg_list;
    iter = head->next;
    while (iter != head) {
        proc_msg = krhino_list_entry(iter, proc_msg_t, node);
        iter = iter->next;
        if (proc_msg->buf_queue->key == key) {
            for (i = 0; i < RHINO_CONFIG_PROC_MSG_MAX_OWNER_NUM; i++) {
                if (proc_msg->owner[i] == (uint32_t)(-1)) {
                    proc_msg->owner[i] = cur_pid;
                    proc_msg->ref_cnt++;
                    queue = proc_msg->buf_queue;
                    break;
                }
            }

            if (i == RHINO_CONFIG_PROC_MSG_MAX_OWNER_NUM) {
                full = 1;
            }

            if (queue != NULL) {
                break;
            }
        }
    }

    if (full == 1 || queue != NULL) {
        goto out;
    }

    proc_msg = (proc_msg_t*)krhino_mm_alloc(sizeof(proc_msg_t));
    if (proc_msg == NULL) {
        goto out;
    }

    memset(proc_msg, 0, sizeof(proc_msg_t));
    klist_init(&proc_msg->node);
    for (i = 0; i < RHINO_CONFIG_PROC_MSG_MAX_OWNER_NUM; i++) {
        proc_msg->owner[i] = (uint32_t)-1;
    }

    msg_dyn_create(&queue, "buf_queue", size, MAX_MSG_SIZE, key);

    if (queue == NULL) {
        krhino_mm_free(proc_msg);
        goto out;
    }

    proc_msg->buf_queue = queue;
    proc_msg->owner[0]= cur_pid;
    proc_msg->ref_cnt = 1;
    klist_insert(&proc_msg_list, &proc_msg->node);

out:
    krhino_mutex_unlock(&proc_msg_mutex);

    return (size_t)queue;
}

kstat_t krhino_msg_snd(size_t msq_id, void *msg, size_t msg_sz)
{
    return krhino_buf_queue_send((kbuf_queue_t *)msq_id, msg, msg_sz);
}

kstat_t krhino_msg_recv(size_t msq_id, tick_t ticks, void *msg, size_t *msg_sz)
{
    return krhino_buf_queue_recv((kbuf_queue_t *)msq_id, ticks, msg, msg_sz);
}

kstat_t krhino_msg_del(size_t msg_id)
{
    kstat_t     stat;
    klist_t    *head;
    klist_t    *iter;
    proc_msg_t *proc_msg;
    int         cur_pid;
    int         found;
    int         i;

    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();
    if (proc_msg_inited == 0) {
        RHINO_CRITICAL_EXIT();
        return RHINO_SUCCESS;
    }
    RHINO_CRITICAL_EXIT();

    krhino_mutex_lock(&proc_msg_mutex, RHINO_WAIT_FOREVER);

    found = 0;
    head  = &proc_msg_list;
    iter  = head->next;
    while (iter != head) {
        proc_msg = krhino_list_entry(iter, proc_msg_t, node);
        iter = iter->next;
        if (proc_msg->buf_queue == (kbuf_queue_t*)msg_id) {
            found = 1;
            break;
        }
    }

    stat = RHINO_SUCCESS;
    if (found == 1) {
        cur_pid = krhino_cur_task_get()->pid;
        for (i = 0; i < RHINO_CONFIG_PROC_MSG_MAX_OWNER_NUM; i++) {
            if (proc_msg->owner[i] == cur_pid) {
                proc_msg->owner[i] = (uint32_t)-1;
                proc_msg->ref_cnt--;
                break;
            }
        }

        if (proc_msg->ref_cnt == 0) {
            klist_rm(&proc_msg->node);
            stat = krhino_buf_queue_dyn_del(proc_msg->buf_queue);
            krhino_mm_free(proc_msg);
        }
    }

    krhino_mutex_unlock(&proc_msg_mutex);

    return stat;
}

void msg_exit(uint32_t pid)
{
    klist_t    *head;
    klist_t    *iter;
    proc_msg_t *proc_msg;
    int         i;

    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();
    if (proc_msg_inited == 0) {
        RHINO_CRITICAL_EXIT();
        return;
    }
    RHINO_CRITICAL_EXIT();

    krhino_mutex_lock(&proc_msg_mutex, RHINO_WAIT_FOREVER);

    head  = &proc_msg_list;
    iter  = head->next;
    while (iter != head) {
        proc_msg = krhino_list_entry(iter, proc_msg_t, node);
        iter = iter->next;
        for (i = 0; i < RHINO_CONFIG_PROC_MSG_MAX_OWNER_NUM; i++) {
            if (proc_msg->owner[i] == pid) {
                proc_msg->owner[i] = (uint32_t)-1;
                proc_msg->ref_cnt--;
            }
        }

        if (proc_msg->ref_cnt == 0) {
            klist_rm(&proc_msg->node);
            krhino_buf_queue_dyn_del(proc_msg->buf_queue);
            krhino_mm_free(proc_msg);
        }
    }

    krhino_mutex_unlock(&proc_msg_mutex);
}

