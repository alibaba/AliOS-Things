/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

#if (RHINO_CONFIG_KOBJ_SET > 0)
void krhino_kobj_set_notify(blk_obj_t *obj, kobj_set_t *handle)
{
    kstat_t stat;
    stat = krhino_queue_back_send(&(handle->queue), obj);

    if (stat != RHINO_SUCCESS) {
        k_err_proc(stat);
    }
}

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
kstat_t krhino_kobj_set_dyn_create(kobj_set_t **handle, const name_t *name,
                                   size_t msg_num)
{
    kstat_t     stat;
    kobj_set_t *kobj_set;
    void       *start;

    if (handle == NULL) {
        return RHINO_NULL_PTR;
    }

    kobj_set = krhino_mm_alloc(sizeof(kobj_set_t));

    if (kobj_set == NULL) {
        return RHINO_NO_MEM;
    }

    start = krhino_mm_alloc(sizeof(void *)*msg_num);

    if (start == NULL) {
        return RHINO_NO_MEM;
    }

    stat  = krhino_queue_create(&(kobj_set->queue), name, (void **)start, msg_num);

    if (stat != RHINO_SUCCESS) {
        krhino_mm_free(kobj_set);
        krhino_mm_free(start);
        return stat;
    }

    kobj_set->notify = krhino_kobj_set_notify;

    *handle = kobj_set;

    return RHINO_SUCCESS;
}

kstat_t krhino_kobj_set_dyn_del(kobj_set_t *handle)
{
    kstat_t stat;

    if (handle == NULL) {
        return RHINO_NULL_PTR;
    }

    stat = krhino_queue_del(&(handle->queue));
    krhino_mm_free((void *)((handle->queue).msg_q.queue_start));
    krhino_mm_free((void *)handle);

    return stat;
}
#endif

kstat_t krhino_kobj_set_create(kobj_set_t *handle, const name_t *name,
                               void **start, size_t msg_num)
{
    kstat_t     stat;

    if (handle == NULL) {
        return RHINO_NULL_PTR;
    }

    stat  = krhino_queue_create(&(handle->queue), name, start, msg_num);

    if (stat != RHINO_SUCCESS) {
        return stat;
    }

    handle->notify = krhino_kobj_set_notify;

    return RHINO_SUCCESS;
}

kstat_t krhino_kobj_set_del(kobj_set_t *handle)
{
    kstat_t stat;

    if (handle == NULL) {
        return RHINO_NULL_PTR;
    }

    stat = krhino_queue_del(&(handle->queue));
    return stat;
}

static kstat_t kobj_internal_count_get(blk_obj_t *obj, size_t *count)
{
    /* now we support semaphone,queue,buf_queue */
    switch (obj->obj_type) {
        case RHINO_SEM_OBJ_TYPE:
            *count = ((ksem_t *)obj)->count;
            break;

        case RHINO_BUF_QUEUE_OBJ_TYPE:
            *count = ((kbuf_queue_t *)obj)->cur_num;
            break;

        case RHINO_QUEUE_OBJ_TYPE:
            *count = ((kqueue_t *)obj)->msg_q.cur_num;
            break;

        default:
            *count = 0;
            return RHINO_KOBJ_TYPE_ERR;
    }

    return RHINO_SUCCESS;
}

static kstat_t kobj_insert_prepare(kobj_set_t *handle, blk_obj_t *obj)
{
    CPSR_ALLOC();

    klist_t   *blk_list_head;
    msg_info_t info;
    size_t     free;
    size_t     count;
    kstat_t    stat;

    RHINO_CRITICAL_ENTER();

    blk_list_head = &(obj->blk_list);

    /* obj with blocked task can not treated */
    if (!is_klist_empty(blk_list_head)) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_BLK;
    }

    krhino_queue_info_get(&(handle->queue), &info);
    free = info.msg_q.size - info.msg_q.cur_num;

    stat = kobj_internal_count_get(obj, &count);

    if (stat != RHINO_SUCCESS) {
        RHINO_CRITICAL_EXIT();
        return stat;
    }

    if (count > free) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_SET_FULL;
    }

    for (; count > 0; count--) {
        krhino_queue_back_send(&(handle->queue), (void *) obj);
    }

    RHINO_CRITICAL_EXIT();

    return RHINO_SUCCESS;
}

kstat_t krhino_kobj_set_insert(blk_obj_t *obj, kobj_set_t *handle)
{
    CPSR_ALLOC();
    kstat_t stat;

    if (obj ==  NULL) {
        return RHINO_NULL_PTR;
    }

    if (handle ==  NULL) {
        return RHINO_NULL_PTR;
    }

    stat = kobj_insert_prepare(handle, obj);

    if (stat != RHINO_SUCCESS) {
        return stat;
    }

    RHINO_CRITICAL_ENTER();

    obj->handle = handle;

    RHINO_CRITICAL_EXIT();

    return RHINO_SUCCESS;
}

kstat_t krhino_kobj_set_rm(blk_obj_t *obj)
{
    CPSR_ALLOC();
    kstat_t stat;
    size_t  count;

    if (obj ==  NULL) {
        return RHINO_NULL_PTR;
    }

    RHINO_CRITICAL_ENTER();

    stat = kobj_internal_count_get(obj, &count);

    if (stat != RHINO_SUCCESS) {
        RHINO_CRITICAL_EXIT();
        return stat;
    }

    if (count != 0) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_DEL_ERR;
    }

    obj->handle = NULL;

    RHINO_CRITICAL_EXIT();

    return RHINO_SUCCESS;
}

kstat_t krhino_kobj_select(kobj_set_t *handle, blk_obj_t **obj, tick_t ticks)
{
    kstat_t stat;

    if (handle ==  NULL) {
        return RHINO_NULL_PTR;
    }

    stat = krhino_queue_recv(&(handle->queue), ticks, (void **)obj);
    return stat;
}
#endif  /* RHINO_CONFIG_KOBJ_SET */

