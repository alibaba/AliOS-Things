/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifdef RHINO_CONFIG_IPC

#include <stdatomic.h>
#include "k_api.h"
#include "utask.h"
#include "debug_api.h"

#include "kipc_queue.h"
#include "kipc_perf.h"

#define IPC_RETRY_MAX     (4)

#ifdef __cplusplus
extern "C" {
#endif

kstat_t kipc_queue_create(ipc_queue_t **queue, int type, size_t tlen, size_t blk_size)
{
    ipc_queue_t *q = NULL;
    uint8_t *buf = NULL;

    if (type < KIPC_QUEUE_TYP_STREAM || type > KIPC_QUEUE_TYP_FIFO) {
        return -RHINO_INV_PARAM;
    }

    q = (ipc_queue_t *)krhino_mm_alloc(sizeof(*q));
    if (q == NULL) {
        return -RHINO_NO_MEM;
    }
    memset(q, 0, sizeof(*q));

    buf = (uint8_t *)krhino_mm_alloc(tlen * blk_size);
    if (buf == NULL) {
        krhino_mm_free(q);
        return -RHINO_NO_MEM;
    }
    memset(buf, 0, tlen * blk_size);

    if (RHINO_SUCCESS != IPC_PERF_INIT(q, IPC_PERF_QUEUE_CAP_CNT)) {
        krhino_mm_free(q);
        krhino_mm_free(buf);
        return -RHINO_NO_MEM;
    }

    /* init the queue blocked list */
    klist_init(&q->read.blk_obj.blk_list);
    klist_init(&q->write.blk_obj.blk_list);
    q->read.buf = buf;
    q->write.buf = buf;

    q->read.blk_obj.name = "kipc_queue";
    q->read.blk_obj.blk_policy = BLK_POLICY_PRI;
    q->read.blk_obj.obj_type = RHINO_BUF_QUEUE_OBJ_TYPE;
    q->read.tsize = tlen * blk_size;
    q->read.blk_size = blk_size;

    q->write.blk_obj.name = "kipc_queue";
    q->write.blk_obj.blk_policy = BLK_POLICY_PRI;
    q->write.blk_obj.obj_type = RHINO_BUF_QUEUE_OBJ_TYPE;
    q->write.tsize = tlen * blk_size;
    q->write.blk_size = blk_size;

#if (RHINO_CONFIG_TASK_DEL > 0)
    q->read.blk_obj.cancel = 1u;
    q->write.blk_obj.cancel = 1u;
#endif

    switch (type) {
        case KIPC_QUEUE_TYP_STREAM:
            q->read.next = ipc_read_stream_next;
            q->read.cpy = ipc_read_stream_cpy;
            q->read.wakeup = ipc_read_stream_wakeup;
            q->read.delete = ipc_stream_delete;
            q->write.next = ipc_write_stream_next;
            q->write.cpy = ipc_write_stream_cpy;
            q->write.wakeup = ipc_write_stream_wakeup;
            break;
        case KIPC_QUEUE_TYP_MSG:
            q->read.next = ipc_read_msg_next;
            q->read.cpy = ipc_read_msg_cpy;
            q->read.wakeup = ipc_read_msg_wakeup;
            q->read.delete = ipc_msg_delete;
            q->write.next = ipc_write_msg_next;
            q->write.cpy = ipc_write_msg_cpy;
            q->write.wakeup = ipc_write_msg_wakeup;
            break;
        case KIPC_QUEUE_TYP_FIFO:
            q->read.next = ipc_read_fifo_next;
            q->read.cpy = ipc_read_fifo_cpy;
            q->read.wakeup = ipc_read_fifo_wakeup;
            q->read.delete = ipc_fifo_delete;
            q->write.next = ipc_write_fifo_next;
            q->write.cpy = ipc_write_fifo_cpy;
            q->write.wakeup = ipc_write_fifo_wakeup;
            break;
    }

    *queue = q;

    return RHINO_SUCCESS;
}

kstat_t kipc_queue_disable(ipc_queue_t *queue, int flag)
{
    CPSR_ALLOC();

    klist_t *head;

    if (queue == NULL) {
        return -RHINO_INV_PARAM;
    }

    RHINO_CRITICAL_ENTER();
    if (flag & KIPC_PEND_CLR_RX) {
        head = &queue->read.blk_obj.blk_list;
        /* all task blocked on this queue is waken up */
        __atomic_store_n(&queue->read.state, 1, __ATOMIC_SEQ_CST);
        while (!is_klist_empty(head)) {
           pend_task_rm(krhino_list_entry(head->next, ktask_t, task_list));
        }

    }
    if (flag & KIPC_PEND_CLR_TX) {
        head = &queue->write.blk_obj.blk_list;
        /* all task blocked on this queue is waken up */
        __atomic_store_n(&queue->write.state, 1, __ATOMIC_SEQ_CST);
        while (!is_klist_empty(head)) {
           pend_task_rm(krhino_list_entry(head->next, ktask_t, task_list));
        }
    }
    RHINO_CRITICAL_EXIT();

    return RHINO_SUCCESS;
}

kstat_t kipc_queue_enable(ipc_queue_t *queue, int flag)
{
    CPSR_ALLOC();

    if (queue == NULL) {
        return -RHINO_INV_PARAM;
    }

    RHINO_CRITICAL_ENTER();
    if (flag & KIPC_PEND_CLR_RX) {
        __atomic_store_n(&queue->read.state, 0, __ATOMIC_SEQ_CST);
    }
    if (flag & KIPC_PEND_CLR_TX) {
        __atomic_store_n(&queue->write.state, 0, __ATOMIC_SEQ_CST);
    }
    RHINO_CRITICAL_EXIT();

    return RHINO_SUCCESS;
}

kstat_t kipc_queue_del(ipc_queue_t *queue)
{
    CPSR_ALLOC();

    klist_t *head;

    if (queue == NULL) {
        return -RHINO_INV_PARAM;
    }

    if (queue->read.blk_obj.obj_type != RHINO_BUF_QUEUE_OBJ_TYPE ||
        queue->write.blk_obj.obj_type != RHINO_BUF_QUEUE_OBJ_TYPE) {
        return -RHINO_KOBJ_TYPE_ERR;
    }

    IPC_PERF_DEINIT(queue);

    RHINO_CRITICAL_ENTER();
    head = &queue->read.blk_obj.blk_list;
    /* all task blocked on this queue is waken up */
    while (!is_klist_empty(head)) {
        pend_task_rm(krhino_list_entry(head->next, ktask_t, task_list));
    }
    queue->read.blk_obj.obj_type = RHINO_OBJ_TYPE_NONE;
    head = &queue->write.blk_obj.blk_list;
    /* all task blocked on this queue is waken up */
    while (!is_klist_empty(head)) {
        pend_task_rm(krhino_list_entry(head->next, ktask_t, task_list));
    }
    queue->write.blk_obj.obj_type = RHINO_OBJ_TYPE_NONE;
    queue->read.delete(queue);
    krhino_mm_free(queue);
    RHINO_CRITICAL_EXIT();

    RHINO_CRITICAL_ENTER();
    RHINO_CRITICAL_EXIT_SCHED();

    return RHINO_SUCCESS;
}

static kstat_t queue_send(ipc_queue_t *q, void *msg, size_t msg_size, tick_t ticks)
{
    uint8_t *dst_buf = NULL;
    int first, next = -1, second;

    kstat_t  err;
    klist_t *head;
    ktask_t *task;
    uint8_t cur_cpu_num, retry = 0;

    tick_t start, end;

    CPSR_ALLOC();
    IPC_PERF_ALLOC();
    IPC_PERF_ST();

    __atomic_add_fetch(&q->write.access, 1, __ATOMIC_RELEASE);

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_SEND_STEP1_IDX);
    IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_SEND_STEP1_IDX);

    do { /* contest for position */
        IPC_PERF_ST();
        if (unlikely(next >= 0)) {
            cpu_intrpt_restore(cpsr);
        }
        /* memory barrier with load */
        __atomic_load(&q->write.first, &first, __ATOMIC_RELAXED);

        IPC_PERF_ET();
        IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_SEND_STEP2_IDX);
        IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_SEND_STEP2_IDX);
        IPC_PERF_ST();

        /* fast path */
        next = q->write.next(q, &dst_buf, msg_size);
        if (unlikely(next < 0) && unlikely(next != -RHINO_IPC_QUEUE_FULL)) {
            __atomic_sub_fetch(&q->write.access, 1, __ATOMIC_RELAXED);
            if (dst_buf != NULL) {
                krhino_mm_free(dst_buf);
            }
            return next;
        }

        IPC_PERF_ET();
        IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_SEND_STEP3_IDX);
        IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_SEND_STEP3_IDX);
        IPC_PERF_ST();

        if (unlikely(next < 0)) {  /* RHINO_IPC_QUEUE_FULL, middle-path */
            if (ticks == RHINO_NO_WAIT) {
                __atomic_sub_fetch(&q->write.access, 1, __ATOMIC_RELAXED);
                /* no space for the packet, non-block operation */
                if (dst_buf != NULL) {
                    krhino_mm_free(dst_buf);
                }
                return -RHINO_BLK_TIMEOUT;
            }

            __atomic_load(&q->write.first, &first, __ATOMIC_RELAXED);
            next = q->write.next(q, &dst_buf, msg_size);

            IPC_PERF_ET();
            IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_SEND_STEP4_IDX);
            IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_SEND_STEP4_IDX);
            IPC_PERF_ST();
            if (next >= 0) {
                /* ready to write packet */
                __builtin_prefetch(q->write.buf + first, KIPC_PREFETCH_FOR_WR,
                                   KIPC_PREFETCH_CACHE_LOCAL_LVL_0);
                cpsr = cpu_intrpt_save();
            } else if (q->write.state) {
                __atomic_sub_fetch(&q->write.access, 1, __ATOMIC_RELAXED);
                /* no space for the packet, non-block operation */
                if (dst_buf != NULL) {
                    krhino_mm_free(dst_buf);
                }
                return -RHINO_IPC_OBJ_NOT_EXIST;
            } else if (retry < IPC_RETRY_MAX &&
                       __atomic_load_n(&q->read.access, __ATOMIC_RELAXED)) {
                retry ++;
            } else {
                start = krhino_sys_tick_get();
                /* no space for the packet, block operation */
                RHINO_CRITICAL_ENTER();

                IPC_PERF_ET();
                IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_SEND_STEP5_IDX);
                IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_SEND_STEP5_IDX);
                IPC_PERF_ST();

                head = &q->read.blk_obj.blk_list;
                if (q->write.next(q, &dst_buf, msg_size) >= 0) {
                    /* available space to tx */
                    retry = 0;
                    RHINO_CRITICAL_EXIT();  /* time consuming operation */
                } else if (!is_klist_empty(head)) {  /* middle-path */
                    /* rx block list is not empty, there is blocked receive task */
                    task = krhino_list_entry(head->next, ktask_t, task_list);
                    pend_task_wakeup(task);
                    /* try again */
                    RHINO_CRITICAL_EXIT_SCHED();
                    IPC_PERF_ET();
                    IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_SEND_STEP7_IDX);
                    IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_SEND_STEP7_IDX);
                } else {  // slow path
                    cur_cpu_num = cpu_cur_get();
                    task = g_active_task[cur_cpu_num];
                    if (q->write.state) {
                        __atomic_sub_fetch(&q->write.access, 1, __ATOMIC_RELAXED);
                        if (dst_buf != NULL) krhino_mm_free(dst_buf);
                        RHINO_CRITICAL_EXIT();
                        return -RHINO_IPC_OBJ_NOT_EXIST;
                    }
                    pend_to_blk_obj(&q->write.blk_obj, task, ticks);

                    IPC_PERF_ET();
                    IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_SEND_STEP8_IDX);
                    IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_SEND_STEP8_IDX);
                    IPC_PERF_ST();

                    RHINO_CRITICAL_EXIT_SCHED();

                    IPC_PERF_ET();
                    IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_SEND_STEP9_IDX);
                    IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_SEND_STEP9_IDX);
                    IPC_PERF_ST();

                    /* switch back */
                    RHINO_CPU_INTRPT_DISABLE();
                    err = pend_state_end_proc(task, NULL);
                    RHINO_CPU_INTRPT_ENABLE();
                    retry = 0;

                    if (err != RHINO_SUCCESS) {
                        __atomic_sub_fetch(&q->write.access, 1, __ATOMIC_RELAXED);
                        if (dst_buf != NULL) {
                            krhino_mm_free(dst_buf);
                        }
                        return -err;
                    }
                    IPC_PERF_ET();
                    IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_SEND_STEPA_IDX);
                    IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_SEND_STEPA_IDX);
                }
                IPC_PERF_ST();

                end = krhino_sys_tick_get();
                if (end - start >= ticks) {
                    __atomic_sub_fetch(&q->write.access, 1, __ATOMIC_RELAXED);
                    if (dst_buf != NULL) {
                        krhino_mm_free(dst_buf);
                    }
                    return -RHINO_BLK_TIMEOUT;
                }
                ticks -= (end - start);

                IPC_PERF_ET();
                IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_SEND_STEPB_IDX);
                IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_SEND_STEPB_IDX);
            }
        } else {  /* prefetch buffer for write operation */
            __builtin_prefetch(q->write.buf + first, KIPC_PREFETCH_FOR_WR,
                               KIPC_PREFETCH_CACHE_LOCAL_LVL_0);
            cpsr = cpu_intrpt_save();
        }
    } while (likely(next < 0) || unlikely(!__atomic_compare_exchange(&q->write.first,
                                          &first, &next, 0, __ATOMIC_RELEASE, __ATOMIC_RELAXED)));

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_SEND_STEPC_IDX);
    IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_SEND_STEPC_IDX);
    IPC_PERF_ST();

    q->write.cpy(q, first, msg, &msg_size, dst_buf);

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_SEND_STEPD_IDX);
    IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_SEND_STEPD_IDX);
    IPC_PERF_ST();

    /* make sure that copy packet to queue is finished */
    while (1) {
        second = q->write.second;
        if (unlikely(second != first)) {
            RHINO_CRITICAL_ENTER();
            RHINO_CRITICAL_EXIT_SCHED();
            continue;
        }
        if (likely(__atomic_compare_exchange(&q->write.second,
                                             &first, &next, 0, __ATOMIC_RELEASE, __ATOMIC_RELAXED))) {
            break;
        }
    }
    cpu_intrpt_restore(cpsr);

    __atomic_sub_fetch(&q->write.access, 1, __ATOMIC_RELAXED);

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_SEND_STEPE_IDX);
    IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_SEND_STEPE_IDX);
    IPC_PERF_ST();

    /* pre-check, prevent enter critical section */
    head = &q->read.blk_obj.blk_list;
    if (!is_klist_empty(head) && q->read.wakeup(q) == RHINO_SUCCESS) {
        RHINO_CRITICAL_ENTER();  /* time consuming operation */
        /* double check head to prevent dirty of head */
        head = &q->read.blk_obj.blk_list;
        /* prevent task is wakeup by another task */
        if (!is_klist_empty(head)) {
            task = krhino_list_entry(head->next, ktask_t, task_list);
            pend_task_wakeup(task);
        }
        RHINO_CRITICAL_EXIT();  /* time consuming operation */
    }

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_SEND_STEPF_IDX);
    IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_SEND_STEPF_IDX);

    return RHINO_SUCCESS;
}

static kstat_t queue_recv(ipc_queue_t *q, void *msg, size_t *size, tick_t ticks)
{
    int first, next = -1, second;

    kstat_t  err;
    klist_t *head;
    ktask_t *task;
    uint8_t cur_cpu_num, retry = 0;

    tick_t start, end;

    CPSR_ALLOC();
    IPC_PERF_ALLOC();
    IPC_PERF_ST();

    __atomic_add_fetch(&q->read.access, 1, __ATOMIC_RELEASE);

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_RECV_STEP1_IDX);
    IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_RECV_STEP1_IDX);

    do { /* contest for position */
        IPC_PERF_ST();
        if (unlikely(next >= 0)) {
            cpu_intrpt_restore(cpsr);
        }
        /* memory barrier with load */
        __atomic_load(&q->read.first, &first, __ATOMIC_RELAXED);

        IPC_PERF_ET();
        IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_RECV_STEP2_IDX);
        IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_RECV_STEP2_IDX);
        IPC_PERF_ST();

        /* fast path */
        next = q->read.next(q, NULL, *size);
        if (unlikely(next < 0) && unlikely(next != -RHINO_IPC_QUEUE_EMPTY)) {
            __atomic_sub_fetch(&q->read.access, 1, __ATOMIC_RELAXED);
            return next;
        }

        IPC_PERF_ET();
        IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_RECV_STEP3_IDX);
        IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_RECV_STEP3_IDX);
        IPC_PERF_ST();

        if (unlikely(next < 0)) {  /* RHINO_IPC_QUEUE_EMPTY, middle path */
            if (ticks == RHINO_NO_WAIT) {
                __atomic_sub_fetch(&q->read.access, 1, __ATOMIC_RELAXED);
                /* no space for the packet, non-block operation */
                return -RHINO_BLK_TIMEOUT;
            }
            __atomic_load(&q->read.first, &first, __ATOMIC_RELAXED);
            next = q->read.next(q, NULL, *size);

            IPC_PERF_ET();
            IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_RECV_STEP4_IDX);
            IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_RECV_STEP4_IDX);
            IPC_PERF_ST();

            if (next >= 0) {
                __builtin_prefetch(q->read.buf + first, KIPC_PREFETCH_FOR_RD,
                                   KIPC_PREFETCH_CACHE_LOCAL_LVL_0);
                cpsr = cpu_intrpt_save();
            } else if (q->read.state) {
                __atomic_sub_fetch(&q->read.access, 1, __ATOMIC_RELAXED);
                /* no space for the packet, non-block operation */
                return -RHINO_IPC_OBJ_NOT_EXIST;
            } else if (retry < IPC_RETRY_MAX &&
                       __atomic_load_n(&q->write.access, __ATOMIC_RELAXED)) {
                retry ++;
            } else {
                start = krhino_sys_tick_get();
                RHINO_CRITICAL_ENTER();

                IPC_PERF_ET();
                IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_RECV_STEP5_IDX);
                IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_RECV_STEP5_IDX);
                IPC_PERF_ST();

                head = &q->write.blk_obj.blk_list;
                if (q->read.next(q, NULL, *size) >= 0) {
                    retry = 0;
                    RHINO_CRITICAL_EXIT();  /* time consuming operation */
                } else if (!is_klist_empty(head)) {  /* middle path */
                    task = krhino_list_entry(head->next, ktask_t, task_list);
                    pend_task_wakeup(task);
                    /* try again */
                    RHINO_CRITICAL_EXIT_SCHED();
                    IPC_PERF_ET();
                    IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_RECV_STEP7_IDX);
                    IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_RECV_STEP7_IDX);
                } else {  /* slow path */
                    cur_cpu_num = cpu_cur_get();
                    task = g_active_task[cur_cpu_num];
                    if (q->read.state) {
                        __atomic_sub_fetch(&q->read.access, 1, __ATOMIC_RELAXED);
                        RHINO_CRITICAL_EXIT();
                        return -RHINO_IPC_OBJ_NOT_EXIST;
                    }
                    pend_to_blk_obj(&q->read.blk_obj, task, ticks);

                    IPC_PERF_ET();
                    IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_RECV_STEP8_IDX);
                    IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_RECV_STEP8_IDX);
                    IPC_PERF_ST();

                    /* rx block list is not empty, there is blocked receive task */
                    RHINO_CRITICAL_EXIT_SCHED();

                    IPC_PERF_ET();
                    IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_RECV_STEP9_IDX);
                    IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_RECV_STEP9_IDX);
                    IPC_PERF_ST();

                    /* switch back */
                    RHINO_CPU_INTRPT_DISABLE();
                    err = pend_state_end_proc(task, NULL);
                    RHINO_CPU_INTRPT_ENABLE();
                    retry = 0;

                    if (err != RHINO_SUCCESS) {
                        __atomic_sub_fetch(&q->read.access, 1, __ATOMIC_RELAXED);
                        return -err;
                    }

                    IPC_PERF_ET();
                    IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_RECV_STEPA_IDX);
                    IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_RECV_STEPA_IDX);
                }
                IPC_PERF_ST();

                end = krhino_sys_tick_get();
                if (end - start >= ticks) {
                    __atomic_sub_fetch(&q->read.access, 1, __ATOMIC_RELAXED);
                    return -RHINO_BLK_TIMEOUT;
                }
                ticks -= (end - start);

                IPC_PERF_ET();
                IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_RECV_STEPB_IDX);
                IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_RECV_STEPB_IDX);
            }
        } else {  /* prefetch data for read operation */
            __builtin_prefetch(q->read.buf + first, KIPC_PREFETCH_FOR_RD,
                               KIPC_PREFETCH_CACHE_LOCAL_LVL_0);
            cpsr = cpu_intrpt_save();
        }
    } while (likely(next < 0) || unlikely(!__atomic_compare_exchange(&q->read.first,
                                          &first, &next, 0, __ATOMIC_RELEASE, __ATOMIC_RELAXED)));

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_RECV_STEPC_IDX);
    IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_RECV_STEPC_IDX);
    IPC_PERF_ST();

    q->read.cpy(q, first, msg, size, NULL);

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_RECV_STEPD_IDX);
    IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_RECV_STEPD_IDX);
    IPC_PERF_ST();

    /* make sure that copy packet to queue is finished */
    while (1) {
        second = q->read.second;
        if (unlikely(second != first)) {
            RHINO_CRITICAL_ENTER();
            RHINO_CRITICAL_EXIT_SCHED();
            continue;
        }
        if (likely(__atomic_compare_exchange(&q->read.second,
                                             &first, &next, 0, __ATOMIC_RELEASE, __ATOMIC_RELAXED))) {
            break;
        }
    }
    cpu_intrpt_restore(cpsr);

    __atomic_sub_fetch(&q->read.access, 1, __ATOMIC_RELAXED);

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_RECV_STEPE_IDX);
    IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_RECV_STEPE_IDX);
    IPC_PERF_ST();

    head = &q->write.blk_obj.blk_list;
    if (!is_klist_empty(head) && q->write.wakeup(q) == RHINO_SUCCESS) {
        RHINO_CRITICAL_ENTER();  /* time consuming operation */
        /* double check head to prevent dirty of head */
        head = &q->write.blk_obj.blk_list;
        /* prevent task is wakeup by another task */
        if (!is_klist_empty(head)) {
            task = krhino_list_entry(head->next, ktask_t, task_list);
            pend_task_wakeup(task);
        }
        RHINO_CRITICAL_EXIT();  /* time consuming operation */
    }

    IPC_PERF_ET();
    IPC_PERF_UPDATE_ST(q, IPC_PERF_QUEUE_RECV_STEPF_IDX);
    IPC_PERF_UPDATE_ET(q, IPC_PERF_QUEUE_RECV_STEPF_IDX);

    return RHINO_SUCCESS;
}

kstat_t kipc_queue_send(ipc_queue_t *q, void *msg, size_t msg_size, tick_t ticks)
{
    return queue_send(q, msg, msg_size, ticks);
}

kstat_t kipc_queue_recv(ipc_queue_t *q, void *msg, size_t *size, tick_t ticks)
{
    return queue_recv(q, msg, size, ticks);
}

int kipc_queue_full(ipc_queue_t *q)
{
    if (q->write.next(q, NULL, q->write.blk_size) < 0) {
        return 1;
    }

    return 0;
}

int kipc_queue_empty(ipc_queue_t *q)
{
    if (q->read.second == q->write.second) {
        return 1;
    }

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif
