#include "k_api.h"
#include "k_err.h"
#include "k_sys.h"
#include "sxs_io.h"
#include "sxr_mem.h"
#include "sxr_csp.h"
#include "sxs_rmt.h"
#include "sxr_tim.h"
#include "sxr_jbsd.h"
#include "sxr_tksd.h"
#include <aos/kernel.h>
#include <aos/errno.h>


/*sxr send data only modify task state,not call shcedule*/

kstat_t task_resume_sxr(ktask_t *task)
{
    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();

    switch (task->task_state) {
        case K_RDY:
        case K_SLEEP:
        case K_PEND:
            RHINO_CRITICAL_EXIT();
            return RHINO_TASK_NOT_SUSPENDED;
        case K_SUSPENDED:
            task->suspend_count--;

            if (task->suspend_count == 0u) {
                /* Make task ready */
                task->task_state = K_RDY;
                ready_list_add(&g_ready_queue, task);
            }

            break;
        case K_SLEEP_SUSPENDED:
            task->suspend_count--;

            if (task->suspend_count == 0u) {
                task->task_state = K_SLEEP;
            }

            break;
        case K_PEND_SUSPENDED:
            task->suspend_count--;

            if (task->suspend_count == 0u) {
                task->task_state = K_PEND;
            }

            break;
        case K_SEED:
        default:
            RHINO_CRITICAL_EXIT();
            return RHINO_INV_TASK_STATE;
    }

    TRACE_TASK_RESUME(g_active_task[cpu_cur_get()], task);

    RHINO_CRITICAL_EXIT();

    return RHINO_SUCCESS;
}



static kstat_t buf_queue_send_sxr(kbuf_queue_t *queue, void *msg, size_t msg_size)
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

    RHINO_CRITICAL_EXIT();    //RHINO_CRITICAL_EXIT_SCHED();

    return RHINO_SUCCESS;
}

kstat_t krhino_buf_queue_send_sxr(kbuf_queue_t *queue, void *msg, size_t size)
{
    NULL_PARA_CHK(queue);
    NULL_PARA_CHK(msg);

    return buf_queue_send_sxr(queue, msg, size);
}


int aos_queue_send_sxr(aos_queue_t *queue, void *msg, unsigned int size)
{
    int ret;

    if ((queue == NULL) || (msg == NULL)) {
        return -EINVAL;
    }

    ret = krhino_buf_queue_send_sxr(queue->hdl, msg, size);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    ERRNO_MAPPING(ret);
}



