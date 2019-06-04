/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_config.h"

#if (RHINO_CONFIG_UIPC > 0)

#include "k_api.h"
#include "ipc.h"
#include "utask.h"
#include "task_group.h"

#define MAX_MSG_SIZE 0xffffffffu

/**
 * FIXME: 1. record buf_queue refcnt, it can be deleted unless it's
 *           refcnt reaches to zero
 */
static void msg_dyn_create(kbuf_queue_t **queue, const name_t *name,
                           size_t size, size_t max_msg, uint32_t key)
{
    CPSR_ALLOC();

    kbuf_queue_t *queue_obj;
    klist_t      *group_head;
    klist_t      *group_iter;
    klist_t      *queue_head;
    klist_t      *queue_iter;
    kbuf_queue_t *queue_tmp;
    task_group_t *cur_group;
    task_group_t *group;
    ktask_t      *cur;

    cur = krhino_cur_task_get();
    cur_group = (task_group_t*)cur->task_group;
    if (cur_group == NULL) {
        *queue = NULL;
        return;
    }

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


    group_head = task_group_get_list_head();

    for (group_iter  = group_head->next; group_iter != group_head; group_iter = group_iter->next) {
        group = group_info_entry(group_iter, task_group_t, node);
        queue_head = &group->kobj_list.buf_queue_head;
        for (queue_iter = queue_head->next; queue_iter != queue_head; queue_iter = queue_iter->next){
            queue_tmp = krhino_list_entry(queue_iter, kbuf_queue_t, buf_queue_item);
            if (queue_tmp->key == key) {
                *queue = queue_tmp;
                RHINO_CRITICAL_EXIT();
                krhino_mm_free(queue_obj->buf);
                krhino_mm_free(queue_obj);
                return;
            }
        }
    }

    klist_insert(&(cur_group->kobj_list.buf_queue_head), &queue_obj->buf_queue_item);
    queue_obj->key = key;
    RHINO_CRITICAL_EXIT();

    *queue = queue_obj;
}

size_t krhino_msg_get(uint32_t key, uint32_t flg, size_t size)
{
    kbuf_queue_t *queue = NULL;

    msg_dyn_create(&queue, "buf_queue", size, MAX_MSG_SIZE, key);

    if (queue == NULL) {
        return 0;
    }

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
    kstat_t stat;

    stat = krhino_buf_queue_del((kbuf_queue_t *)msg_id);
    if (stat == RHINO_SUCCESS) {
        task_group_remove_res_obj(NULL, RES_OBJ_BUF_QUEUE, (void*)msg_id);
    }

    return stat;
}

#endif /* RHINO_CONFIG_UIPC */

