/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

#if (RHINO_CONFIG_BUF_QUEUE > 0)

static kstat_t buf_queue_create(kbuf_queue_t *queue, const name_t *name,
                                void *buf, size_t size, size_t max_msg, uint8_t mm_alloc_flag, size_t type)
{
    CPSR_ALLOC();

    NULL_PARA_CHK(queue);
    NULL_PARA_CHK(buf);
    NULL_PARA_CHK(name);

    if (max_msg == 0u) {
        return RHINO_INV_PARAM;
    }

    if (size == 0u) {
        return RHINO_BUF_QUEUE_SIZE_ZERO;
    }

    /* init the queue blocked list */
    klist_init(&queue->blk_obj.blk_list);

    queue->buf                = buf;
    queue->cur_num            = 0u;
    queue->peak_num           = 0u;
    queue->max_msg_size       = max_msg;
    queue->blk_obj.name       = name;
    queue->blk_obj.blk_policy = BLK_POLICY_PRI;
    queue->mm_alloc_flag      = mm_alloc_flag;

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    RHINO_CRITICAL_ENTER();
    klist_insert(&(g_kobj_list.buf_queue_head), &queue->buf_queue_item);
    RHINO_CRITICAL_EXIT();
#endif

    queue->blk_obj.obj_type = RHINO_BUF_QUEUE_OBJ_TYPE;

    ringbuf_init(&(queue->ringbuf), buf, size, type, max_msg);
    queue->min_free_buf_size  = queue->ringbuf.freesize;
    TRACE_BUF_QUEUE_CREATE(krhino_cur_task_get(), queue);

    return RHINO_SUCCESS;
}

kstat_t krhino_buf_queue_create(kbuf_queue_t *queue, const name_t *name,
                                void *buf, size_t size, size_t max_msg)
{
    return buf_queue_create(queue, name, buf, size, max_msg, K_OBJ_STATIC_ALLOC, RINGBUF_TYPE_DYN);
}

kstat_t krhino_fix_buf_queue_create(kbuf_queue_t *queue, const name_t *name,
                                   void *buf, size_t msg_size, size_t msg_num)
{
    return buf_queue_create(queue, name, buf, msg_size * msg_num, msg_size, K_OBJ_STATIC_ALLOC, RINGBUF_TYPE_FIX);
}

kstat_t krhino_buf_queue_del(kbuf_queue_t *queue)
{
    CPSR_ALLOC();

    klist_t *head;

    NULL_PARA_CHK(queue);

    RHINO_CRITICAL_ENTER();

    INTRPT_NESTED_LEVEL_CHK();

    if (queue->blk_obj.obj_type != RHINO_BUF_QUEUE_OBJ_TYPE) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_TYPE_ERR;
    }

    if (queue->mm_alloc_flag != K_OBJ_STATIC_ALLOC) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_DEL_ERR;
    }

    head = &queue->blk_obj.blk_list;

    queue->blk_obj.obj_type = RHINO_OBJ_TYPE_NONE;

    /* all task blocked on this queue is waken up */
    while (!is_klist_empty(head)) {

        pend_task_rm(krhino_list_entry(head->next, ktask_t, task_list));
    }

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    klist_rm(&queue->buf_queue_item);
#endif

    ringbuf_reset(&queue->ringbuf);

    RHINO_CRITICAL_EXIT_SCHED();

    return RHINO_SUCCESS;
}

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
kstat_t krhino_buf_queue_dyn_create(kbuf_queue_t **queue, const name_t *name,
                                    size_t size, size_t max_msg)
{
    kstat_t      stat;
    kbuf_queue_t *queue_obj;

    NULL_PARA_CHK(queue);

    if (size == 0u) {
        return RHINO_BUF_QUEUE_SIZE_ZERO;
    }

    queue_obj = krhino_mm_alloc(sizeof(kbuf_queue_t));

    if (queue_obj == NULL) {
        return RHINO_NO_MEM;
    }

    queue_obj->buf = krhino_mm_alloc(size);

    if (queue_obj->buf == NULL) {
        krhino_mm_free(queue_obj);
        return RHINO_NO_MEM;
    }

    stat = buf_queue_create(queue_obj, name, queue_obj->buf, size, max_msg,
                            K_OBJ_DYN_ALLOC, RINGBUF_TYPE_DYN);

    if (stat != RHINO_SUCCESS) {
        krhino_mm_free(queue_obj->buf);
        krhino_mm_free(queue_obj);
        return stat;
    }

    *queue = queue_obj;

    return RHINO_SUCCESS;
}

kstat_t krhino_buf_queue_dyn_del(kbuf_queue_t *queue)
{
    CPSR_ALLOC();
    klist_t *head;

    NULL_PARA_CHK(queue);

    RHINO_CRITICAL_ENTER();

    INTRPT_NESTED_LEVEL_CHK();

    if (queue->blk_obj.obj_type != RHINO_BUF_QUEUE_OBJ_TYPE) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_TYPE_ERR;
    }

    if (queue->mm_alloc_flag != K_OBJ_DYN_ALLOC) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_DEL_ERR;
    }

    head = &queue->blk_obj.blk_list;

    queue->blk_obj.obj_type = RHINO_OBJ_TYPE_NONE;

    while (!is_klist_empty(head)) {
        pend_task_rm(krhino_list_entry(head->next, ktask_t, task_list));
    }

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    klist_rm(&queue->buf_queue_item);
#endif

    ringbuf_reset(&queue->ringbuf);

    RHINO_CRITICAL_EXIT_SCHED();
    krhino_mm_free(queue->buf);
    krhino_mm_free(queue);

    return RHINO_SUCCESS;
}
#endif

static kstat_t buf_queue_send(kbuf_queue_t *queue, void *msg, size_t msg_size)
{
    CPSR_ALLOC();

    klist_t *head;
    ktask_t *task;
    kstat_t  err;

    uint8_t  cur_cpu_num;

    /* this is only needed when system zero interrupt feature is enabled */
#if (RHINO_CONFIG_INTRPT_GUARD > 0)
    soc_intrpt_guard();
#endif

    RHINO_CRITICAL_ENTER();

    if (queue->blk_obj.obj_type != RHINO_BUF_QUEUE_OBJ_TYPE) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_TYPE_ERR;
    }

    cur_cpu_num = cpu_cur_get();
    (void)cur_cpu_num;

    if (msg_size > queue->max_msg_size) {
        TRACE_BUF_QUEUE_MAX(g_active_task[cur_cpu_num], queue, msg, msg_size);
        RHINO_CRITICAL_EXIT();
        return RHINO_BUF_QUEUE_MSG_SIZE_OVERFLOW;
    }

    if (msg_size == 0) {
        RHINO_CRITICAL_EXIT();
        return RHINO_INV_PARAM;
    }

    head = &queue->blk_obj.blk_list;

    /* buf queue is not full here, if there is no blocked receive task */
    if (is_klist_empty(head)) {

        err = ringbuf_push(&(queue->ringbuf), msg, msg_size);

        if (err != RHINO_SUCCESS) {
            RHINO_CRITICAL_EXIT();
            if (err == RHINO_RINGBUF_FULL) {
                err =  RHINO_BUF_QUEUE_FULL;
            }
            return err;
        }

        queue->cur_num++;

        if (queue->peak_num  < queue->cur_num) {
            queue->peak_num  = queue->cur_num;
        }

        if (queue->min_free_buf_size > queue->ringbuf.freesize) {
            queue->min_free_buf_size = queue->ringbuf.freesize;
        }

        TRACE_BUF_QUEUE_POST(g_active_task[cur_cpu_num], queue, msg, msg_size);

        RHINO_CRITICAL_EXIT();
        return RHINO_SUCCESS;
    }

    task = krhino_list_entry(head->next, ktask_t, task_list);
    memcpy(task->msg, msg, msg_size);
    task->bq_msg_size = msg_size;

    pend_task_wakeup(task);

    TRACE_BUF_QUEUE_TASK_WAKE(g_active_task[cur_cpu_num], task, queue);

    RHINO_CRITICAL_EXIT_SCHED();

    return RHINO_SUCCESS;
}

kstat_t krhino_buf_queue_send(kbuf_queue_t *queue, void *msg, size_t size)
{
    NULL_PARA_CHK(queue);
    NULL_PARA_CHK(msg);

    return buf_queue_send(queue, msg, size);
}

kstat_t krhino_buf_queue_recv(kbuf_queue_t *queue, tick_t ticks, void *msg,
                              size_t *size)
{
    CPSR_ALLOC();

    kstat_t ret;
    uint8_t cur_cpu_num;

    NULL_PARA_CHK(queue);
    NULL_PARA_CHK(msg);
    NULL_PARA_CHK(size);

    RHINO_CRITICAL_ENTER();

    cur_cpu_num = cpu_cur_get();

    if ((g_intrpt_nested_level[cur_cpu_num] > 0u) && (ticks != RHINO_NO_WAIT)) {
        RHINO_CRITICAL_EXIT();
        return RHINO_NOT_CALLED_BY_INTRPT;
    }

    if (queue->blk_obj.obj_type != RHINO_BUF_QUEUE_OBJ_TYPE) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_TYPE_ERR;
    }

    if (!ringbuf_is_empty(&(queue->ringbuf))) {
        ringbuf_pop(&(queue->ringbuf), msg, size);
        queue->cur_num --;
        RHINO_CRITICAL_EXIT();
        return RHINO_SUCCESS;
    }

    if (ticks == RHINO_NO_WAIT) {
        *size = 0u;
        RHINO_CRITICAL_EXIT();
        return RHINO_NO_PEND_WAIT;
    }

    if (g_sched_lock[cur_cpu_num] > 0u) {
        *size = 0u;
        RHINO_CRITICAL_EXIT();
        return RHINO_SCHED_DISABLE;
    }

    g_active_task[cur_cpu_num]->msg = msg;
    pend_to_blk_obj((blk_obj_t *)queue, g_active_task[cur_cpu_num], ticks);

    TRACE_BUF_QUEUE_GET_BLK(g_active_task[cur_cpu_num], queue, ticks);

    RHINO_CRITICAL_EXIT_SCHED();

    RHINO_CPU_INTRPT_DISABLE();

    cur_cpu_num = cpu_cur_get();

    ret = pend_state_end_proc(g_active_task[cur_cpu_num]);

    switch (ret) {
        case RHINO_SUCCESS:
            *size = g_active_task[cur_cpu_num]->bq_msg_size;
            break;

        default:
            *size = 0u;
            break;
    }

    RHINO_CPU_INTRPT_ENABLE();

    return ret;
}

kstat_t krhino_buf_queue_flush(kbuf_queue_t *queue)
{
    CPSR_ALLOC();

    NULL_PARA_CHK(queue);

    RHINO_CRITICAL_ENTER();

    INTRPT_NESTED_LEVEL_CHK();

    if (queue->blk_obj.obj_type != RHINO_BUF_QUEUE_OBJ_TYPE) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_TYPE_ERR;
    }

    ringbuf_reset(&(queue->ringbuf));
    queue->cur_num           = 0u;
    queue->peak_num          = 0u;
    queue->min_free_buf_size = queue->ringbuf.freesize;

    RHINO_CRITICAL_EXIT();

    return RHINO_SUCCESS;
}

kstat_t krhino_buf_queue_info_get(kbuf_queue_t *queue, kbuf_queue_info_t *info)
{
    CPSR_ALLOC();

    NULL_PARA_CHK(queue);
    NULL_PARA_CHK(info);

    RHINO_CRITICAL_ENTER();

    if (queue->blk_obj.obj_type != RHINO_BUF_QUEUE_OBJ_TYPE) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_TYPE_ERR;
    }

    info->free_buf_size = queue->ringbuf.freesize;
    info->min_free_buf_size = queue->min_free_buf_size;
    info->buf_size = queue->ringbuf.end - queue->ringbuf.buf;
    info->max_msg_size = queue->max_msg_size;
    info->cur_num = queue->cur_num;
    info->peak_num = queue->peak_num;

    RHINO_CRITICAL_EXIT();

    return RHINO_SUCCESS;
}

#endif

