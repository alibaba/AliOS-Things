/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

void tick_list_init(void)
{
   klist_init(&g_tick_head);

#if (RHINO_CONFIG_DYNTICKLESS > 0)
    g_next_intrpt_ticks = RHINO_CONFIG_NEXT_INTRPT_TICKS;
#endif
}

RHINO_INLINE void tick_list_pri_insert(klist_t *head, ktask_t *task)
{
    tick_t   val;
    klist_t *q;
    klist_t *list_start;
    klist_t *list_end;
    ktask_t  *task_iter_temp;

    list_start = list_end = head;
    val = task->tick_remain;

    for (q = list_start->next; q != list_end; q = q->next) {
        task_iter_temp = krhino_list_entry(q, ktask_t, tick_list);
        if ((task_iter_temp->tick_match - g_tick_count) > val) {
            break;
        }
    }

    klist_insert(q, &task->tick_list);

#if (RHINO_CONFIG_DYNTICKLESS > 0)
    task_iter_temp = krhino_list_entry(head->next, ktask_t, tick_list);

    if (g_next_intrpt_ticks > task_iter_temp->tick_match - g_tick_count) {
        g_next_intrpt_ticks = task_iter_temp->tick_match - g_tick_count;
        soc_tick_intrpt_set(g_next_intrpt_ticks, g_elapsed_ticks);
    }
#endif
}

void tick_list_insert(ktask_t *task, tick_t time)
{
    klist_t *tick_head_ptr;

    if (time > 0u) {
        task->tick_match  = g_tick_count + time;
        task->tick_remain = time;

        tick_head_ptr = &g_tick_head;
        tick_list_pri_insert(tick_head_ptr, task);
        task->tick_head = tick_head_ptr;
    }
}

void tick_list_rm(ktask_t *task)
{
    klist_t *tick_head_ptr = task->tick_head;

    if (tick_head_ptr != NULL) {
        klist_rm(&task->tick_list);
        task->tick_head = NULL;
    }
}

void tick_list_update(void)
{
    CPSR_ALLOC();

    klist_t *tick_head_ptr;
    ktask_t  *p_tcb;
    klist_t *iter;
    klist_t *iter_temp;
    tick_i_t delta;

    RHINO_CRITICAL_ENTER();

#if (RHINO_CONFIG_DYNTICKLESS > 0)
    soc_dyntick_proc();
    g_tick_count       += g_pend_intrpt_ticks;
    g_sys_time_tick    += g_pend_intrpt_ticks;
#else
    g_tick_count++;
    g_sys_time_tick++;
#endif

    tick_head_ptr = &g_tick_head;
    iter          =  tick_head_ptr->next;

    while (RHINO_TRUE) {
        /* search all the time list if possible */
        if (iter != tick_head_ptr) {
            iter_temp = iter->next;
            p_tcb     = krhino_list_entry(iter, ktask_t, tick_list);
            delta = (tick_i_t)p_tcb->tick_match - (tick_i_t)g_tick_count;
            /* since time list is sorted by remain time, so just campare  the absolute time */
            if (delta <= 0) {
                switch (p_tcb->task_state) {
                    case K_SLEEP:
                        p_tcb->blk_state  = BLK_FINISH;
                        p_tcb->task_state = K_RDY;
                        tick_list_rm(p_tcb);
                        ready_list_add(&g_ready_queue, p_tcb);
                        break;
                    case K_PEND:
                        tick_list_rm(p_tcb);
                        /* remove task on the block list because task is timeout */
                        klist_rm(&p_tcb->task_list);
                        ready_list_add(&g_ready_queue, p_tcb);
                        p_tcb->blk_state  = BLK_TIMEOUT;
                        p_tcb->task_state = K_RDY;
                        mutex_task_pri_reset(p_tcb);
                        p_tcb->blk_obj    = NULL;
                        break;
                    case K_PEND_SUSPENDED:
                        tick_list_rm(p_tcb);
                        /* remove task on the block list because task is timeout */
                        klist_rm(&p_tcb->task_list);
                        p_tcb->blk_state  = BLK_TIMEOUT;
                        p_tcb->task_state = K_SUSPENDED;
                        mutex_task_pri_reset(p_tcb);
                        p_tcb->blk_obj    = NULL;
                        break;
                    case K_SLEEP_SUSPENDED:
                        p_tcb->task_state = K_SUSPENDED;
                        p_tcb->blk_state  = BLK_FINISH;
                        tick_list_rm(p_tcb);
                        break;
                    default:
                        k_err_proc(RHINO_SYS_FATAL_ERR);
                        break;
                }

                iter = iter_temp;
            } else {
                break;
            }
        } else {
            break;
        }
    }

#if (RHINO_CONFIG_DYNTICKLESS > 0)

    if (tick_head_ptr->next != tick_head_ptr) {
        p_tcb = krhino_list_entry(tick_head_ptr->next, ktask_t, tick_list);
        g_next_intrpt_ticks = p_tcb->tick_match - g_tick_count;
    } else {
        g_next_intrpt_ticks = RHINO_CONFIG_NEXT_INTRPT_TICKS;
    }

    soc_tick_intrpt_set(g_next_intrpt_ticks, 0);
#endif

    RHINO_CRITICAL_EXIT();
}

