/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

#if (RHINO_CONFIG_EVENT_FLAG > 0)
static kstat_t event_create(kevent_t *event, const name_t *name, uint32_t flags,
                            uint8_t mm_alloc_flag)
{
    CPSR_ALLOC();

    NULL_PARA_CHK(event);
    NULL_PARA_CHK(name);

    /* init the list */
    klist_init(&event->blk_obj.blk_list);
    event->blk_obj.blk_policy = BLK_POLICY_PRI;
    event->blk_obj.name       = name;
    event->flags              = flags;
    event->mm_alloc_flag      = mm_alloc_flag;

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    RHINO_CRITICAL_ENTER();
    klist_insert(&(g_kobj_list.event_head), &event->event_item);
    RHINO_CRITICAL_EXIT();
#endif

    TRACE_EVENT_CREATE(krhino_cur_task_get(), event, name, flags);

    event->blk_obj.obj_type = RHINO_EVENT_OBJ_TYPE;

    return RHINO_SUCCESS;
}

kstat_t krhino_event_create(kevent_t *event, const name_t *name, uint32_t flags)
{
    return event_create(event, name, flags, K_OBJ_STATIC_ALLOC);
}

kstat_t krhino_event_del(kevent_t *event)
{
    CPSR_ALLOC();

    klist_t *blk_list_head;

    NULL_PARA_CHK(event);

    RHINO_CRITICAL_ENTER();

    INTRPT_NESTED_LEVEL_CHK();

    if (event->blk_obj.obj_type != RHINO_EVENT_OBJ_TYPE) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_TYPE_ERR;
    }

    if (event->mm_alloc_flag != K_OBJ_STATIC_ALLOC) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_DEL_ERR;
    }

    blk_list_head = &event->blk_obj.blk_list;

    event->blk_obj.obj_type = RHINO_OBJ_TYPE_NONE;

    while (!is_klist_empty(blk_list_head)) {
        pend_task_rm(krhino_list_entry(blk_list_head->next, ktask_t, task_list));
    }

    event->flags = 0u;

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    klist_rm(&event->event_item);
#endif

    TRACE_EVENT_DEL(g_active_task[cpu_cur_get()], event);

    RHINO_CRITICAL_EXIT_SCHED();

    return RHINO_SUCCESS;
}

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
kstat_t krhino_event_dyn_create(kevent_t **event, const name_t *name,
                                uint32_t flags)
{
    kstat_t   stat;
    kevent_t *event_obj;

    if (event == NULL) {
        return RHINO_NULL_PTR;
    }

    event_obj = krhino_mm_alloc(sizeof(kevent_t));

    if (event_obj == NULL) {
        return RHINO_NO_MEM;
    }

    stat = event_create(event_obj, name, flags, K_OBJ_DYN_ALLOC);

    if (stat != RHINO_SUCCESS) {
        krhino_mm_free(event_obj);
        return stat;
    }

    *event = event_obj;

    return stat;
}

kstat_t krhino_event_dyn_del(kevent_t *event)
{
    CPSR_ALLOC();

    klist_t *blk_list_head;

    NULL_PARA_CHK(event);

    RHINO_CRITICAL_ENTER();

    INTRPT_NESTED_LEVEL_CHK();

    if (event->blk_obj.obj_type != RHINO_EVENT_OBJ_TYPE) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_TYPE_ERR;
    }

    if (event->mm_alloc_flag != K_OBJ_DYN_ALLOC) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_DEL_ERR;
    }

    blk_list_head = &event->blk_obj.blk_list;

    event->blk_obj.obj_type = RHINO_OBJ_TYPE_NONE;

    while (!is_klist_empty(blk_list_head)) {
        pend_task_rm(krhino_list_entry(blk_list_head->next, ktask_t, task_list));
    }

    event->flags = 0u;

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    klist_rm(&event->event_item);
#endif

    RHINO_CRITICAL_EXIT_SCHED();

    krhino_mm_free(event);

    return RHINO_SUCCESS;
}
#endif

kstat_t krhino_event_get(kevent_t *event, uint32_t flags, uint8_t opt,
                         uint32_t *actl_flags, tick_t ticks)
{
    CPSR_ALLOC();

    kstat_t stat;
    uint8_t status;
    uint8_t cur_cpu_num;

    NULL_PARA_CHK(event);
    NULL_PARA_CHK(actl_flags);

    if ((opt != RHINO_AND) && (opt != RHINO_OR) && (opt != RHINO_AND_CLEAR) &&
        (opt != RHINO_OR_CLEAR)) {
        return RHINO_NO_THIS_EVENT_OPT;
    }

    RHINO_CRITICAL_ENTER();

    INTRPT_NESTED_LEVEL_CHK();

    if (event->blk_obj.obj_type != RHINO_EVENT_OBJ_TYPE) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_TYPE_ERR;
    }

    cur_cpu_num = cpu_cur_get();

    /* if option is AND MASK or OR MASK */
    if (opt & RHINO_FLAGS_AND_MASK) {
        if ((event->flags & flags) == flags) {
            status = RHINO_TRUE;
        } else {
            status = RHINO_FALSE;
        }
    } else {
        if ((event->flags & flags) > 0u) {
            status = RHINO_TRUE;
        } else {
            status = RHINO_FALSE;
        }
    }

    if (status == RHINO_TRUE) {
        *actl_flags = event->flags;

        if (opt & RHINO_FLAGS_CLEAR_MASK) {
            event->flags &= ~flags;
        }

        TRACE_EVENT_GET(g_active_task[cur_cpu_num], event);
        RHINO_CRITICAL_EXIT();

        return RHINO_SUCCESS;
    }

    /* can't get event, and return immediately if wait_option is RHINO_NO_WAIT */
    if (ticks == RHINO_NO_WAIT) {
        RHINO_CRITICAL_EXIT();
        return RHINO_NO_PEND_WAIT;
    }

    /* system is locked so task can not be blocked just return immediately */
    if (g_sched_lock[cur_cpu_num] > 0u) {
        RHINO_CRITICAL_EXIT();
        return RHINO_SCHED_DISABLE;
    }

    /* remember the passed information */
    g_active_task[cur_cpu_num]->pend_option = opt;
    g_active_task[cur_cpu_num]->pend_flags  = flags;
    g_active_task[cur_cpu_num]->pend_info   = actl_flags;

    pend_to_blk_obj((blk_obj_t *)event, g_active_task[cur_cpu_num], ticks);

    TRACE_EVENT_GET_BLK(g_active_task[cur_cpu_num], event, ticks);

    RHINO_CRITICAL_EXIT_SCHED();

    RHINO_CPU_INTRPT_DISABLE();

    /* so the task is waked up, need know which reason cause wake up */
    stat = pend_state_end_proc(g_active_task[cpu_cur_get()]);

    RHINO_CPU_INTRPT_ENABLE();

    return stat;
}

static kstat_t event_set(kevent_t *event, uint32_t flags, uint8_t opt)
{
    CPSR_ALLOC();

    klist_t  *iter;
    klist_t  *event_head;
    klist_t  *iter_temp;
    ktask_t  *task;
    uint8_t   status;
    uint32_t  cur_event_flags;

    /* this is only needed when system zero interrupt feature is enabled */
#if (RHINO_CONFIG_INTRPT_GUARD > 0)
    soc_intrpt_guard();
#endif

    status = RHINO_FALSE;

    RHINO_CRITICAL_ENTER();

    if (event->blk_obj.obj_type != RHINO_EVENT_OBJ_TYPE) {
        RHINO_CRITICAL_EXIT();
        return RHINO_KOBJ_TYPE_ERR;
    }

    event_head = &event->blk_obj.blk_list;

    /* if the set_option is AND_MASK, it just clears the flags and will return immediately */
    if (opt & RHINO_FLAGS_AND_MASK) {
        event->flags &= flags;

        RHINO_CRITICAL_EXIT();
        return RHINO_SUCCESS;
    } else {
        event->flags |= flags;
    }

    cur_event_flags = event->flags;
    iter = event_head->next;

    /* if list is not empty */
    while (iter != event_head) {
        task = krhino_list_entry(iter, ktask_t, task_list);
        iter_temp = iter->next;

        if (task->pend_option & RHINO_FLAGS_AND_MASK) {
            if ((cur_event_flags & task->pend_flags) == task->pend_flags) {
                status = RHINO_TRUE;
            } else {
                status = RHINO_FALSE;
            }
        } else {
            if (cur_event_flags & task->pend_flags) {
                status = RHINO_TRUE;
            } else {
                status = RHINO_FALSE;
            }
        }

        if (status == RHINO_TRUE) {
            (*(uint32_t *)(task->pend_info)) = cur_event_flags;

            /* the task condition is met, just wake this task */
            pend_task_wakeup(task);

            TRACE_EVENT_TASK_WAKE(g_active_task[cpu_cur_get()], task, event);

            /* does it need to clear the flags */
            if (task->pend_option & RHINO_FLAGS_CLEAR_MASK) {
                event->flags &= ~(task->pend_flags);
            }
        }

        iter = iter_temp;
    }

    RHINO_CRITICAL_EXIT_SCHED();

    return RHINO_SUCCESS;
}

kstat_t krhino_event_set(kevent_t *event, uint32_t flags, uint8_t opt)
{
    NULL_PARA_CHK(event);

    if ((opt != RHINO_AND) && (opt != RHINO_OR)) {
        return RHINO_NO_THIS_EVENT_OPT;
    }

    return event_set(event, flags, opt);
}
#endif /* RHINO_CONFIG_EVENT_FLAG */

