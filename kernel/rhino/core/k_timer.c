/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

#if (RHINO_CONFIG_TIMER > 0)
static void timer_list_pri_insert(klist_t *head, ktimer_t *timer)
{
    tick_t    val;
    klist_t  *q;
    klist_t  *start;
    klist_t  *end;
    ktimer_t *task_iter_temp;

    start = end = head;
    val = timer->remain;

    for (q = start->next; q != end; q = q->next) {
        task_iter_temp = krhino_list_entry(q, ktimer_t, timer_list);
        if ((task_iter_temp->match - g_timer_count) > val) {
            break;
        }
    }

    klist_insert(q, &timer->timer_list);
}

static void timer_list_rm(ktimer_t *timer)
{
    klist_t *head;

    head = timer->to_head;
    if (head != NULL) {
        klist_rm(&timer->timer_list);
        timer->to_head = NULL;
    }
}

static kstat_t timer_create(ktimer_t *timer, const name_t *name, timer_cb_t cb,
                            tick_t first, tick_t round, void *arg, uint8_t auto_run,
                            uint8_t mm_alloc_flag)
{
    CPSR_ALLOC();

    NULL_PARA_CHK(timer);
    NULL_PARA_CHK(name);
    NULL_PARA_CHK(cb);

    if (first == 0u) {
        return RHINO_INV_PARAM;
    }

    RHINO_CRITICAL_ENTER();
    INTRPT_NESTED_LEVEL_CHK();
    RHINO_CRITICAL_EXIT();

    timer->name          = name;
    timer->cb            = cb;
    timer->init_count    = first;
    timer->round_ticks   = round;
    timer->remain        = 0u;
    timer->match         = 0u;
    timer->timer_state   = TIMER_DEACTIVE;
    timer->to_head       = NULL;
    timer->mm_alloc_flag = mm_alloc_flag;
    timer->timer_cb_arg  = arg;
    klist_init(&timer->timer_list);

    timer->obj_type = RHINO_TIMER_OBJ_TYPE;

    if (auto_run > 0u) {
        krhino_timer_start(timer);
    }

    TRACE_TIMER_CREATE(krhino_cur_task_get(), timer);

    return RHINO_SUCCESS;
}

kstat_t krhino_timer_create(ktimer_t *timer, const name_t *name, timer_cb_t cb,
                            tick_t first, tick_t round, void *arg, uint8_t auto_run)
{
    return timer_create(timer, name, cb, first, round, arg, auto_run,
                        K_OBJ_STATIC_ALLOC);
}

kstat_t krhino_timer_del(ktimer_t *timer)
{
    NULL_PARA_CHK(timer);

    krhino_mutex_lock(&g_timer_mutex, RHINO_WAIT_FOREVER);

    if (timer->obj_type != RHINO_TIMER_OBJ_TYPE) {
        krhino_mutex_unlock(&g_timer_mutex);
        return RHINO_KOBJ_TYPE_ERR;
    }

    if (timer->timer_state != TIMER_DEACTIVE) {
        krhino_mutex_unlock(&g_timer_mutex);
        return RHINO_TIMER_STATE_INV;
    }

    if (timer->mm_alloc_flag != K_OBJ_STATIC_ALLOC) {
        krhino_mutex_unlock(&g_timer_mutex);
        return RHINO_KOBJ_DEL_ERR;
    }

    timer->obj_type = RHINO_OBJ_TYPE_NONE;

    TRACE_TIMER_DEL(krhino_cur_task_get(), timer);

    krhino_mutex_unlock(&g_timer_mutex);

    return RHINO_SUCCESS;
}

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
kstat_t krhino_timer_dyn_create(ktimer_t **timer, const name_t *name,
                                timer_cb_t cb,
                                tick_t first, tick_t round, void *arg, uint8_t auto_run)
{
    kstat_t   ret;
    ktimer_t *timer_obj;

    NULL_PARA_CHK(timer);

    timer_obj = krhino_mm_alloc(sizeof(ktimer_t));
    if (timer_obj == NULL) {
        return RHINO_NO_MEM;
    }

    ret = timer_create(timer_obj, name, cb, first, round, arg, auto_run,
                       K_OBJ_DYN_ALLOC);
    if (ret != RHINO_SUCCESS) {
        krhino_mm_free(timer_obj);

        return ret;
    }

    *timer = timer_obj;

    return ret;
}

kstat_t krhino_timer_dyn_del(ktimer_t *timer)
{
    NULL_PARA_CHK(timer);

    krhino_mutex_lock(&g_timer_mutex, RHINO_WAIT_FOREVER);

    if (timer->obj_type != RHINO_TIMER_OBJ_TYPE) {
        krhino_mutex_unlock(&g_timer_mutex);
        return RHINO_KOBJ_TYPE_ERR;
    }

    if (timer->timer_state != TIMER_DEACTIVE) {
        krhino_mutex_unlock(&g_timer_mutex);
        return RHINO_TIMER_STATE_INV;
    }

    if (timer->mm_alloc_flag != K_OBJ_DYN_ALLOC) {
        krhino_mutex_unlock(&g_timer_mutex);
        return RHINO_KOBJ_DEL_ERR;
    }

    timer->obj_type = RHINO_OBJ_TYPE_NONE;

    krhino_mutex_unlock(&g_timer_mutex);

    krhino_mm_free(timer);

    return RHINO_SUCCESS;
}
#endif

kstat_t krhino_timer_start(ktimer_t *timer)
{
    CPSR_ALLOC();

    NULL_PARA_CHK(timer);

    RHINO_CRITICAL_ENTER();
    INTRPT_NESTED_LEVEL_CHK();
    RHINO_CRITICAL_EXIT();

    krhino_mutex_lock(&g_timer_mutex, RHINO_WAIT_FOREVER);

    if (timer->obj_type != RHINO_TIMER_OBJ_TYPE) {
        krhino_mutex_unlock(&g_timer_mutex);
        return RHINO_KOBJ_TYPE_ERR;
    }

    if (timer->timer_state == TIMER_ACTIVE) {
        krhino_mutex_unlock(&g_timer_mutex);
        return RHINO_TIMER_STATE_INV;
    }

    timer->match   = g_timer_count + timer->init_count;
    /* sort by remain time */
    timer->remain  = timer->init_count;
    /* used by timer delete */
    timer->to_head = &g_timer_head;

    timer_list_pri_insert(&g_timer_head, timer);
    timer->timer_state = TIMER_ACTIVE;

    krhino_mutex_unlock(&g_timer_mutex);

    return RHINO_SUCCESS;
}

kstat_t krhino_timer_stop(ktimer_t *timer)
{
    CPSR_ALLOC();

    NULL_PARA_CHK(timer);

    RHINO_CRITICAL_ENTER();
    INTRPT_NESTED_LEVEL_CHK();
    RHINO_CRITICAL_EXIT();

    krhino_mutex_lock(&g_timer_mutex, RHINO_WAIT_FOREVER);

    if (timer->obj_type != RHINO_TIMER_OBJ_TYPE) {
        krhino_mutex_unlock(&g_timer_mutex);
        return RHINO_KOBJ_TYPE_ERR;
    }

    if (timer->timer_state == TIMER_DEACTIVE) {
        krhino_mutex_unlock(&g_timer_mutex);
        return RHINO_TIMER_STATE_INV;
    }

    timer_list_rm(timer);
    timer->timer_state = TIMER_DEACTIVE;

    krhino_mutex_unlock(&g_timer_mutex);

    return RHINO_SUCCESS;
}

kstat_t krhino_timer_change(ktimer_t *timer, tick_t first, tick_t round)
{
    CPSR_ALLOC();

    NULL_PARA_CHK(timer);

    RHINO_CRITICAL_ENTER();
    INTRPT_NESTED_LEVEL_CHK();
    RHINO_CRITICAL_EXIT();

    krhino_mutex_lock(&g_timer_mutex, RHINO_WAIT_FOREVER);

    if (first == 0u) {
        krhino_mutex_unlock(&g_timer_mutex);
        return RHINO_INV_PARAM;
    }

    if (timer->obj_type != RHINO_TIMER_OBJ_TYPE) {
        krhino_mutex_unlock(&g_timer_mutex);
        return RHINO_KOBJ_TYPE_ERR;
    }

    if (timer->timer_state != TIMER_DEACTIVE) {
        krhino_mutex_unlock(&g_timer_mutex);
        return RHINO_TIMER_STATE_INV;
    }

    timer->init_count  = first;
    timer->round_ticks = round;

    krhino_mutex_unlock(&g_timer_mutex);

    return RHINO_SUCCESS;
}

kstat_t krhino_timer_arg_change(ktimer_t *timer, void *arg)
{
    CPSR_ALLOC();

    NULL_PARA_CHK(timer);

    RHINO_CRITICAL_ENTER();
    INTRPT_NESTED_LEVEL_CHK();
    RHINO_CRITICAL_EXIT();

    krhino_mutex_lock(&g_timer_mutex, RHINO_WAIT_FOREVER);

    if (timer->obj_type != RHINO_TIMER_OBJ_TYPE) {
        krhino_mutex_unlock(&g_timer_mutex);
        return RHINO_KOBJ_TYPE_ERR;
    }

    if (timer->timer_state != TIMER_DEACTIVE) {
        krhino_mutex_unlock(&g_timer_mutex);
        return RHINO_TIMER_STATE_INV;
    }

    timer->timer_cb_arg  = arg;

    krhino_mutex_unlock(&g_timer_mutex);

    return RHINO_SUCCESS;
}

static void timer_task(void *pa)
{
    klist_t  *timer_head;
    klist_t  *iter;
    ktimer_t *timer;

    (void)pa;

    krhino_sem_count_set(&g_timer_sem, 0);

    while (RHINO_TRUE) {
#if (RHINO_CONFIG_DYNTICKLESS > 0)
        krhino_task_sleep(RHINO_CONFIG_TIMER_RATE);
#else
        krhino_sem_take(&g_timer_sem, RHINO_WAIT_FOREVER);
#endif
        krhino_mutex_lock(&g_timer_mutex, RHINO_WAIT_FOREVER);

        g_timer_count++;

        timer_head = &g_timer_head;
        iter = timer_head->next;

        while (RHINO_TRUE) {
            if (iter != timer_head) {
                timer = krhino_list_entry(iter, ktimer_t, timer_list);

                if (g_timer_count == timer->match) {
                    iter = iter->next;
                    timer_list_rm(timer);

                    if (timer->round_ticks > 0u) {
                        timer->remain  = timer->round_ticks;
                        timer->match   = g_timer_count + timer->remain;
                        timer->to_head = &g_timer_head;
                        timer_list_pri_insert(&g_timer_head, timer);
                    } else {
                        timer->timer_state = TIMER_DEACTIVE;
                    }

                    /* any way both condition need to call registered timer function */
                    /* the registered timer function should not touch any timer related API,otherwise system will be crashed */
                    timer->cb(timer, timer->timer_cb_arg);
                } else {
                    break;
                }
            } else {
                break;
            }
        }

        krhino_mutex_unlock(&g_timer_mutex);
    }
}

void timer_task_sched(void)
{
    g_timer_ctrl--;

    if (g_timer_ctrl == 0u) {
        g_timer_ctrl = RHINO_CONFIG_TIMER_RATE;
        krhino_sem_give(&g_timer_sem);
    }
}

void ktimer_init(void)
{
    g_timer_ctrl = RHINO_CONFIG_TIMER_RATE;

    klist_init(&g_timer_head);

    krhino_task_create(&g_timer_task, "timer_task", NULL,
                       RHINO_CONFIG_TIMER_TASK_PRI, 0u, g_timer_task_stack,
                       RHINO_CONFIG_TIMER_TASK_STACK_SIZE, timer_task, 1u);

    krhino_sem_create(&g_timer_sem, "timer_sem", 0u);

    krhino_mutex_create(&g_timer_mutex, "g_timer_mutex");
}
#endif /* RHINO_CONFIG_TIMER */

