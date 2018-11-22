/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <u_proc_msg.h>

#define MAX_MSG_SIZE 0xffffffffu

static void msg_dyn_create(kbuf_queue_t **queue, const name_t *name,
                           size_t size, size_t max_msg, uint32_t key)
{
    CPSR_ALLOC();

    kbuf_queue_t *queue_obj;
    klist_t      *tmp;
    klist_t      *head;
    kbuf_queue_t *queue_tmp;

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
    head = &g_kobj_list.buf_queue_head;
    for (tmp = head->next; tmp != head; tmp = tmp->next) {
        queue_tmp = krhino_list_entry(tmp, kbuf_queue_t, buf_queue_item);
        if (queue_tmp->key == key) {
            *queue = queue_tmp;
            RHINO_CRITICAL_EXIT();
            krhino_mm_free(queue_obj->buf);
            krhino_mm_free(queue_obj);
            return;
        }
    }
    klist_insert(&(g_kobj_list.buf_queue_head), &queue_obj->buf_queue_item);
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

kstat_t krhino_msg_del(size_t msq_id)
{
    return krhino_buf_queue_del((kbuf_queue_t *)msq_id);
}

