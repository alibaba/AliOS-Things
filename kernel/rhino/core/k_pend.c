/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

RHINO_INLINE void pend_list_add(klist_t *head, ktask_t *task)
{
    klist_t *tmp;
    klist_t *list_start = head;
    klist_t *list_end   = head;

    for (tmp = list_start->next; tmp != list_end; tmp = tmp->next) {
        if (krhino_list_entry(tmp, ktask_t, task_list)->prio > task->prio) {
            break;
        }
    }

    klist_insert(tmp, &task->task_list);
}

void pend_task_wakeup(ktask_t *task)
{
    /* wake up task depend on the different state of task */
    switch (task->task_state) {
        case K_PEND:
            /* remove task on the block list because task is waken up */
            klist_rm(&task->task_list);
            /* add to the ready list again */
            ready_list_add(&g_ready_queue, task);
            task->task_state = K_RDY;
            break;
        case K_PEND_SUSPENDED:
            /* remove task on the block list because task is waken up */
            klist_rm(&task->task_list);
            task->task_state = K_SUSPENDED;
            break;
        default:
            k_err_proc(RHINO_SYS_FATAL_ERR);
            break;
    }

    /* remove task on the tick list because task is waken up */
    tick_list_rm(task);

    task->blk_state = BLK_FINISH;
    task->blk_obj   = NULL;
}

void pend_to_blk_obj(blk_obj_t *blk_obj, ktask_t *task, tick_t timeout)
{
    /* task need to remember which object is blocked on */
    task->blk_obj = blk_obj;

    if (timeout != RHINO_WAIT_FOREVER) {
        tick_list_insert(task, timeout);
    }

    task->task_state = K_PEND;

    /* remove from the ready list */
    ready_list_rm(&g_ready_queue, task);

    if (blk_obj->blk_policy == BLK_POLICY_FIFO) {
        /* add to the end of blocked objet list */
        klist_insert(&blk_obj->blk_list, &task->task_list);
    } else {
        /* add to the prio sorted block list */
        pend_list_add(&blk_obj->blk_list, task);
    }
}

void pend_task_rm(ktask_t *task)
{
    switch (task->task_state) {
        case K_PEND:
            /* remove task on the block list because task is waken up */
            klist_rm(&task->task_list);
            /*add to the ready list again*/
            ready_list_add(&g_ready_queue, task);
            task->task_state = K_RDY;
            break;
        case K_PEND_SUSPENDED:
            /* remove task on the block list because task is waken up */
            klist_rm(&task->task_list);
            task->task_state = K_SUSPENDED;
            break;
        default:
            k_err_proc(RHINO_SYS_FATAL_ERR);
            break;
    }

    /* remove task on the tick list because task is waken up */
    tick_list_rm(task);
    task->blk_state = BLK_DEL;

    /* task is nothing blocked on so reset it to NULL */
    task->blk_obj = NULL;
}

void pend_list_reorder(ktask_t *task)
{
    if (task->blk_obj->blk_policy == BLK_POLICY_PRI) {
        /* remove it first and add it again in prio sorted list */
        klist_rm(&task->task_list);
        pend_list_add(&task->blk_obj->blk_list, task);
    }
}

kstat_t pend_state_end_proc(ktask_t *task)
{
    kstat_t status;

    switch (task->blk_state) {
        case BLK_FINISH:
            status = RHINO_SUCCESS;
            break;
        case BLK_ABORT:
            status = RHINO_BLK_ABORT;
            break;
        case BLK_TIMEOUT:
            status = RHINO_BLK_TIMEOUT;
            break;
        case BLK_DEL:
            status = RHINO_BLK_DEL;
            break;
        default:
            k_err_proc(RHINO_BLK_INV_STATE);
            status = RHINO_BLK_INV_STATE;
            break;
    }

    return status;
}

