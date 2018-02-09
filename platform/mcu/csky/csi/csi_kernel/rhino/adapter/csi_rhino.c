/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/******************************************************************************
 * @file     csi_rhino.c
 * @brief    the adapter file for the rhino
 * @version  V1.0
 * @date     20. July 2016
 ******************************************************************************/

#include <csi_kernel.h>
#include <k_api.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <csi_config.h>
#include <soc.h>

extern uint32_t dump_mmleak(void);

#define AUTORUN  1
#define TMR_ONE_SHOT_DLY             10
#define TMR_PERIODIC_PERIOD          10
#define RHINO_OS_MS_PERIOD_TICK      (1000 / RHINO_CONFIG_TICKS_PER_SECOND)

k_status_t csi_kernel_init(void)
{
    kstat_t ret = krhino_init();

    if (ret == RHINO_SUCCESS) {
        return 0;
    } else {
        return -EPERM;
    }
}

k_status_t csi_kernel_start(void)
{
    kstat_t ret = krhino_start();

    if (ret == RHINO_SUCCESS) {
        return 0;
    } else {
        return -EPERM;
    }
}

k_sched_stat_t csi_kernel_get_stat(void)
{
    kstat_t get = g_sys_stat;

    if (get == RHINO_STOPPED) {
        return KSCHED_ST_INACTIVE;
    } else if (get == RHINO_RUNNING) {
        return KSCHED_ST_RUNNING;
    }

    return KSCHED_ST_ERROR;
}

int32_t csi_kernel_sched_lock(void)
{
    return -EOPNOTSUPP;
}

int32_t csi_kernel_sched_unlock(void)
{
    return -EOPNOTSUPP;
}

int32_t csi_kernel_sched_restore_lock(int32_t lock)
{
    return -EOPNOTSUPP;
}

uint32_t csi_kernel_sched_suspend(void)
{
    if (g_sys_stat != RHINO_RUNNING) {
        return 0;
    }

    krhino_sched_disable();
    return 0;
}

void csi_kernel_sched_resume(uint32_t sleep_ticks)
{
    if (g_sys_stat != RHINO_RUNNING) {
        return;
    }

    krhino_sched_enable();
}

k_status_t csi_kernel_task_new(k_task_entry_t task, const char *name, void *arg,
                               k_priority_t prio, uint32_t time_quanta,
                               void *stack, uint32_t stack_size, k_task_handle_t *task_handle)
{
    if ((task_handle == NULL) || (stack_size < 4) || (stack_size % 4 != 0) || ((stack_size == 0)&&(stack == NULL)) ||prio <= KPRIO_IDLE || prio > KPRIO_REALTIME7) {
        return -EINVAL;
    }

    k_status_t rc = -1;
#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)

    uint8_t prio_trans = RHINO_CONFIG_USER_PRI_MAX - prio;

    csi_kernel_sched_suspend();

    kstat_t ret;
    if (name) {
        ret = krhino_task_dyn_create((ktask_t **)task_handle, name, arg, prio_trans, time_quanta, stack_size / 4, task, AUTORUN);
    } else {
        ret = krhino_task_dyn_create((ktask_t **)task_handle, "user_task", arg, prio_trans, time_quanta, stack_size / 4, task, AUTORUN);
    }

    if (ret == RHINO_SUCCESS) {
        csi_kernel_sched_resume(0);
        return 0;
    } else {
        csi_kernel_sched_resume(0);
        return -EPERM;
    }
#endif
    return rc;

}

k_status_t csi_kernel_task_del(k_task_handle_t task_handle)
{
    if (task_handle == NULL) {
        return -EINVAL;
    }

    k_status_t rc = -1;
#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)

    kstat_t ret = krhino_task_dyn_del(task_handle);

    if (ret == RHINO_SUCCESS) {
        return 0;
    } else {
        return -EPERM;
    }
#endif
    return rc;
}

k_task_handle_t csi_kernel_task_get_cur(void)
{
    ktask_t *ret;
    ret = g_active_task[cpu_cur_get()];
    return (k_task_handle_t)ret;
}

k_task_stat_t csi_kernel_task_get_stat(k_task_handle_t task_handle)
{
    if (task_handle == NULL) {
        return KTASK_ST_ERROR;
    }

    if (csi_kernel_task_get_cur() == task_handle) {
        return KTASK_ST_RUNNING;
    }

    task_stat_t get;
    ktask_t *handle = (ktask_t *)task_handle;
    get = handle->task_state;

    switch (get) {
        case K_PEND:
        case K_SUSPENDED:
        case K_PEND_SUSPENDED:
        case K_SLEEP:
        case K_SLEEP_SUSPENDED:
            return KTASK_ST_BLOCKED;
            break;

        case K_DELETED:
            return KTASK_ST_TERMINATED;
            break;

        case K_RDY:
            return KTASK_ST_READY;
            break;

        default:
            return KTASK_ST_ERROR;
    }
}

k_status_t csi_kernel_task_set_prio(k_task_handle_t task_handle, k_priority_t priority)
{
    if (task_handle  == NULL || priority <= KPRIO_IDLE || priority > KPRIO_REALTIME7) {
        return -EINVAL;
    }

    uint8_t prio = RHINO_CONFIG_USER_PRI_MAX - priority;
    uint8_t old;
    kstat_t ret = krhino_task_pri_change(task_handle, prio, &old);

    if (ret == RHINO_SUCCESS) {
        return 0;
    } else {
        return -EPERM;
    }
}

k_priority_t csi_kernel_task_get_prio(k_task_handle_t task_handle)
{
    if (task_handle == NULL) {
        return KPRIO_ERROR;
    }

    ktask_t *handle = (ktask_t *)task_handle;
    uint8_t ret = handle->prio;
    ret = RHINO_CONFIG_USER_PRI_MAX - ret;
    return ret;
}

const char *csi_kernel_task_get_name(k_task_handle_t task_handle)
{
    if (task_handle == NULL) {
        return NULL;
    }

    ktask_t *handle = (ktask_t *)task_handle;

    return handle->task_name;
}

k_status_t csi_kernel_task_suspend(k_task_handle_t task_handle)
{
    if (task_handle == NULL) {
        return -EINVAL;
    }

    kstat_t ret = krhino_task_suspend(task_handle);

    if (ret == RHINO_SUCCESS) {
        return 0;
    } else {
        return -EPERM;
    }
}

k_status_t csi_kernel_task_resume(k_task_handle_t task_handle)
{
    if (task_handle == NULL) {
        return -EINVAL;
    }

    kstat_t ret = krhino_task_resume(task_handle);

    if (ret == RHINO_SUCCESS) {
        return 0;
    } else {
        return -EPERM;
    }
}

k_status_t csi_kernel_task_terminate(k_task_handle_t task_handle)
{
    return csi_kernel_task_del(task_handle);
}

void csi_kernel_task_exit(void)
{
    csi_kernel_task_del(csi_kernel_task_get_cur());
}

k_status_t csi_kernel_task_yield(void)
{
    kstat_t ret = krhino_task_yield();

    if (ret == RHINO_SUCCESS) {
        return 0;
    } else {
        return -EPERM;
    }
}

uint32_t csi_kernel_task_get_count(void)
{
    klist_t *taskhead;
    klist_t *taskend;
    klist_t *tmp;

    taskhead = &g_kobj_list.task_head;
    taskend  = taskhead;

    uint32_t ret = 0;

    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        ret ++;
    }

    return ret;
}

uint32_t csi_kernel_task_get_stack_size(k_task_handle_t task_handle)
{
    if (task_handle == NULL) {
        return 0;
    }

    ktask_t *handle = (ktask_t *)task_handle;
    return 4 * handle->stack_size;
}

uint32_t csi_kernel_task_get_stack_space(k_task_handle_t task_handle)
{
    if (task_handle == NULL) {
        return 0;
    }

    uint32_t stack_free;
    kstat_t ret = krhino_task_stack_min_free(task_handle, &stack_free);

    if (ret == RHINO_SUCCESS) {
        return 4 * stack_free;
    } else {
        return 0;
    }
}

uint32_t csi_kernel_task_list(k_task_handle_t *task_array, uint32_t array_items)
{
    if (task_array == NULL || array_items== 0) {
        return 0;
    }

    uint32_t real_tsk_num = 0;
    klist_t *taskhead;
    klist_t *taskend;
    klist_t *tmp;
    ktask_t  *task;

    k_task_handle_t *tk_tmp = task_array;
    taskhead = &g_kobj_list.task_head;
    taskend  = taskhead;

#ifdef CONFIG_BACKTRACE
    uint32_t task_free;
    size_t irq_flags;
#endif
#ifdef CONFIG_STACK_GUARD
    int      stack_flags;
#endif

    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        real_tsk_num ++;
        task  = krhino_list_entry(tmp, ktask_t, task_stats_item);
#ifdef CONFIG_BACKTRACE
        krhino_task_stack_min_free(task, &task_free);
        printf("\n%s:\n\t    state %d, pri %d, stack: total %d, free %d\n",
                task->task_name, task->task_state,task->prio, 4 * task->stack_size, 4 * task_free);
#endif
    }

    if (array_items < real_tsk_num) {
        real_tsk_num = array_items;
    }
    for (tmp = taskhead->next; tmp != taskend && real_tsk_num >= 1; tmp = tmp->next) {
        task  = krhino_list_entry(tmp, ktask_t, task_stats_item);
        *tk_tmp = task;
        tk_tmp ++;
    }

#ifdef CONFIG_BACKTRACE
    irq_flags = cpu_intrpt_save();

#ifdef CONFIG_STACK_GUARD
    extern int stack_guard_save(void);
    stack_flags = stack_guard_save();
#endif

    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        task  = krhino_list_entry(tmp, ktask_t, task_stats_item);
        krhino_task_stack_min_free(task, &task_free);

        printf("\n%s:\n", task->task_name);
        extern int csky_task_backtrace(void *stack, char *buf, int len);
        csky_task_backtrace(task->task_stack, NULL, 0);
    }

#ifdef CONFIG_STACK_GUARD
    extern void stack_guard_restore(int value);
    stack_guard_restore(stack_flags);
#endif

    cpu_intrpt_restore(irq_flags);

#endif//CONFIG_BACKTRACE

    return real_tsk_num;
}

k_status_t csi_kernel_intrpt_enter(void)
{
    kstat_t ret = krhino_intrpt_enter();

    if (ret == RHINO_SUCCESS) {
        return 0;
    } else {
        return -EPERM;
    }
    return 0;
}

k_status_t csi_kernel_intrpt_exit(void)
{
    krhino_intrpt_exit();
    return 0;
}


k_status_t csi_kernel_delay(uint32_t ticks)
{
    kstat_t ret = krhino_task_sleep(ticks);

    if (ret == RHINO_SUCCESS) {
        return 0;
    } else {
        return -EPERM;
    }
}

k_status_t csi_kernel_delay_until(uint64_t ticks)
{
    return -EOPNOTSUPP;
}

uint64_t csi_kernel_tick2ms(uint32_t ticks)
{
    return ((uint64_t)ticks * RHINO_OS_MS_PERIOD_TICK);
}

uint64_t csi_kernel_ms2tick(uint32_t ms)
{
    if (ms < RHINO_OS_MS_PERIOD_TICK) {
        return 0;
    }

    return (((uint64_t)ms) / RHINO_OS_MS_PERIOD_TICK);
}

k_status_t csi_kernel_delay_ms(uint32_t ms)
{
    uint32_t ms_get = ms;
    if ((ms < RHINO_OS_MS_PERIOD_TICK) && (ms != 0)) {
        ms_get = RHINO_OS_MS_PERIOD_TICK;
    }

    uint64_t ticks = csi_kernel_ms2tick(ms_get);
    kstat_t ret = krhino_task_sleep(ticks);

    if (ret == RHINO_SUCCESS) {
        return 0;
    } else {
        return -EPERM;
    }
}

uint64_t csi_kernel_get_ticks(void)
{
    return (uint64_t)krhino_sys_tick_get();
}

uint64_t csi_kernel_suspend_tick(void)
{
//    return next_task_wake_tick_get();
    return 0;
}

void csi_kernel_update_tick(uint32_t ms)
{
    extern uint64_t g_sys_tick_count;
    uint32_t ticks = ms * RHINO_CONFIG_TICKS_PER_SECOND / 1000;

    CPSR_ALLOC();

    RHINO_CPU_INTRPT_DISABLE();
    g_sys_tick_count += ticks;

    RHINO_CPU_INTRPT_ENABLE();
}

uint32_t csi_kernel_get_tick_freq(void)
{
    return RHINO_CONFIG_TICKS_PER_SECOND;
}

uint32_t csi_kernel_get_systimer_freq(void)
{
    return SYSTEM_CLOCK;
}

typedef struct tmr_ad {
    k_timer_cb_t   func;
    ktimer_t       tmr;
} tmr_ad_t;

typedef struct tmr_arg {
    void       *arg;
    tmr_ad_t   *tmr_above;
} tmr_arg_t;

static void tmr_adapt_cb(void *timer, void *arg)
{
    ktimer_t  *get = (ktimer_t *)timer;
    tmr_arg_t *arg_above = (tmr_arg_t *)(get->timer_cb_arg);
    tmr_ad_t  *tmr_above = (tmr_ad_t *)arg_above->tmr_above;
    tmr_above->func(arg_above->arg);
    return;
}

k_timer_handle_t csi_kernel_timer_new(k_timer_cb_t func, k_timer_type_t type, void *arg)
{
    if (type < 0 || type > 3 || func == NULL) {
        return NULL;
    }

    tick_t first = TMR_ONE_SHOT_DLY;
    tick_t round;

    if (type == KTIMER_TYPE_ONCE) {
        round = 0;
    } else {
        round = TMR_PERIODIC_PERIOD;
    }

    tmr_ad_t  *handle_ad = (tmr_ad_t *)malloc(sizeof(tmr_ad_t));
    tmr_arg_t *get_arg = (tmr_arg_t *)malloc(sizeof(tmr_arg_t));

    if (handle_ad == NULL || get_arg == NULL) {
        return NULL;
    }

    get_arg->arg = arg;

    kstat_t ret = krhino_timer_create(&(handle_ad->tmr), "UserTmr", (timer_cb_t)tmr_adapt_cb, first, round, get_arg, 0);

    if (ret == RHINO_SUCCESS) {
        handle_ad->func = func;
        get_arg->tmr_above = handle_ad;
        return handle_ad;
    } else {
        return NULL;
    }
}

k_status_t csi_kernel_timer_del(k_timer_handle_t timer_handle)
{
    if (timer_handle == NULL) {
        return -EINVAL;
    }

    tmr_ad_t *handle_ad = (tmr_ad_t *)timer_handle;
    kstat_t ret = krhino_timer_del(&(handle_ad->tmr));

    if (ret == RHINO_SUCCESS) {
        free(((ktimer_t *)(&(handle_ad->tmr)))->timer_cb_arg);
        free(timer_handle);
        return 0;
    } else {
        return -EPERM;
    }
}

k_status_t csi_kernel_timer_start(k_timer_handle_t timer_handle, uint32_t ticks)
{
    if (timer_handle == NULL || ticks == 0) {
        return -EINVAL;
    }

    tick_t round;
    tmr_ad_t *handle_ad = (tmr_ad_t *)timer_handle;
    ktimer_t *handle = (ktimer_t *)(&(handle_ad->tmr));

    round = handle->round_ticks;

    tick_t tr;
    tick_t tf = ticks;

    if (round != 0) {
        tr = ticks;
    } else {
        tr = 0;
    }

    kstat_t ret1 = krhino_timer_change(handle, tf, tr);

    if (ret1 == RHINO_SUCCESS) {
        kstat_t ret2 = krhino_timer_start(handle);

        if (ret2 == RHINO_SUCCESS) {
            return 0;
        } else {
            return -EPERM;
        }
    } else {
        return -EPERM;
    }
}

k_status_t csi_kernel_timer_stop(k_timer_handle_t timer_handle)
{
    if (timer_handle == NULL) {
        return -EINVAL;
    }

    tmr_ad_t *handle_ad = (tmr_ad_t *)timer_handle;
    ktimer_t *handle = (ktimer_t *)(&(handle_ad->tmr));

    kstat_t ret = krhino_timer_stop(handle);

    if (ret == RHINO_SUCCESS) {
        return 0;
    } else {
        return -EPERM;
    }
}

k_timer_stat_t csi_kernel_timer_get_stat(k_timer_handle_t timer_handle)
{
    if (timer_handle == NULL) {
        return KTIMER_ST_INACTIVE;
    }

    tmr_ad_t *handle_ad = (tmr_ad_t *)timer_handle;
    ktimer_t *handle = (ktimer_t *)(&(handle_ad->tmr));
    k_timer_state_t get = handle->timer_state;

    if (get == TIMER_DEACTIVE) {
        return KTIMER_ST_INACTIVE;
    } else {
        return KTIMER_ST_ACTIVE;
    }
}

k_event_handle_t csi_kernel_event_new(void)
{
#if (RHINO_CONFIG_EVENT_FLAG > 0)
    kevent_t *handle = (kevent_t *)malloc(sizeof(kevent_t));

    if (handle == NULL) {
        return NULL;
    }

    kstat_t ret = krhino_event_create(handle, "UserEvent", 0);

    if (ret == RHINO_SUCCESS) {
        return handle;
    } else {
        return NULL;
    }
#else
    return NULL;
#endif

}

k_status_t csi_kernel_event_del(k_event_handle_t ev_handle)
{
#if (RHINO_CONFIG_EVENT_FLAG > 0)
    if (ev_handle == NULL) {
        return -EINVAL;
    }

    kstat_t ret = krhino_event_del(ev_handle);

    if (ret == RHINO_SUCCESS) {
        free(ev_handle);
        return 0;
    } else {
        return -EPERM;
    }
#else
    return -EOPNOTSUPP;
#endif
}

k_status_t csi_kernel_event_set(k_event_handle_t ev_handle, uint32_t flags, uint32_t *ret_flags)
{
#if (RHINO_CONFIG_EVENT_FLAG > 0)
    if (ev_handle == NULL || ret_flags == NULL) {
        return -EINVAL;
    }

    kstat_t ret = krhino_event_set(ev_handle, flags, RHINO_OR);

    if (ret == RHINO_SUCCESS) {
        kevent_t *handle = (kevent_t *)ev_handle;
        *ret_flags = handle->flags;
        return 0;
    } else {
        return -EPERM;
    }
#else
    return -EOPNOTSUPP;
#endif
}

k_status_t csi_kernel_event_clear(k_event_handle_t ev_handle, uint32_t flags, uint32_t *ret_flags)
{
    return -EOPNOTSUPP;
}

k_status_t csi_kernel_event_get(k_event_handle_t ev_handle, uint32_t *ret_flags)
{
#if (RHINO_CONFIG_EVENT_FLAG > 0)
    if (ev_handle == NULL || ret_flags == NULL) {
        return -EINVAL;
    }

    kevent_t *handle = (kevent_t *)ev_handle;
    *ret_flags = handle->flags;
    return 0;
#else
    return -EOPNOTSUPP;
#endif
}

k_status_t csi_kernel_event_wait(k_event_handle_t ev_handle, uint32_t flags,
                                 k_event_opt_t options, uint8_t clr_on_exit,
                                 uint32_t *actl_flags, int32_t timeout)
{
#if (RHINO_CONFIG_EVENT_FLAG > 0)
    if (ev_handle == NULL || actl_flags == NULL
        || ((clr_on_exit != 0) && (clr_on_exit != 1))) {
        return -EINVAL;
    }

    if (options == KEVENT_OPT_CLR_ANY || options == KEVENT_OPT_CLR_ALL) {
        return -EOPNOTSUPP;
    }

    uint8_t opt = 0;

    if (options == KEVENT_OPT_SET_ANY) {
        if (clr_on_exit == 1) {
            opt = RHINO_OR_CLEAR;
        } else {
            opt = RHINO_OR;
        }
    } else if (options == KEVENT_OPT_SET_ALL) {
        if (clr_on_exit == 1) {
            opt = RHINO_AND_CLEAR;
        } else {
            opt = RHINO_AND;
        }
    }

    tick_t t;

    if (timeout < 0) {
        t = RHINO_WAIT_FOREVER;
    } else {
        t = timeout;
    }

    kstat_t ret = krhino_event_get(ev_handle, flags, opt, actl_flags, t);

    if (ret == RHINO_SUCCESS) {
        return 0;
    } else {
        return -EPERM;
    }
#else
    return -EOPNOTSUPP;
#endif
}

k_mutex_handle_t csi_kernel_mutex_new(void)
{
    kmutex_t *handle = (kmutex_t *)malloc(sizeof(kmutex_t));

    if (handle == NULL) {
        return NULL;
    }

    kstat_t ret = krhino_mutex_create(handle, "UserMutex");

    if (ret == RHINO_SUCCESS) {
        return handle;
    } else {
        return NULL;
    }
}

k_status_t csi_kernel_mutex_del(k_mutex_handle_t mutex_handle)
{
    if (mutex_handle == NULL) {
        return -EINVAL;
    }

    kstat_t ret = krhino_mutex_del(mutex_handle);

    if (ret == RHINO_SUCCESS) {
        free(mutex_handle);
        return 0;
    } else {
        return -EPERM;
    }
}

k_status_t csi_kernel_mutex_lock(k_mutex_handle_t mutex_handle, int32_t timeout)
{
    if (mutex_handle == NULL) {
        return -EINVAL;
    }

    tick_t t;

    if (timeout < 0) {
        t = RHINO_WAIT_FOREVER;
    } else {
        t = timeout;
    }

    kstat_t ret = krhino_mutex_lock(mutex_handle, t);

    if (ret == RHINO_SUCCESS || ret == RHINO_MUTEX_OWNER_NESTED) {
        return 0;
    } else {
        return -EPERM;
    }
}

k_status_t csi_kernel_mutex_unlock(k_mutex_handle_t mutex_handle)
{
    if (mutex_handle == NULL) {
        return -EINVAL;
    }

    kstat_t ret = krhino_mutex_unlock(mutex_handle);

    if (ret == RHINO_SUCCESS || ret == RHINO_MUTEX_OWNER_NESTED) {
        return 0;
    } else {
        return -EPERM;
    }
}

k_task_handle_t csi_kernel_mutex_get_owner(k_mutex_handle_t mutex_handle)
{
    if (mutex_handle == NULL) {
        return NULL;
    }

    kmutex_t *handle = (kmutex_t *)mutex_handle;
    return  handle->mutex_task;
}

k_sem_handle_t csi_kernel_sem_new(int32_t max_count, int32_t initial_count)
{
    if (max_count <= 0 || initial_count < 0) {
        return NULL;
    }

    if (max_count < initial_count) {
        return NULL;
    }

    ksem_t *handle = (ksem_t *)malloc(sizeof(ksem_t));

    if (handle == NULL) {
        return NULL;
    }

    kstat_t ret = krhino_sem_create(handle, "UserSem", initial_count);

    if (ret == RHINO_SUCCESS) {
        return handle;
    } else {
        return NULL;
    }
}

k_status_t csi_kernel_sem_del(k_sem_handle_t sem_handle)
{
    if (sem_handle == NULL) {
        return -EINVAL;
    }

    kstat_t ret = krhino_sem_del(sem_handle);

    if (ret == RHINO_SUCCESS) {
        free(sem_handle);
        return 0;
    } else {
        return -EPERM;
    }
}

k_status_t csi_kernel_sem_wait(k_sem_handle_t sem_handle, int32_t timeout)
{
    if (sem_handle == NULL) {
        return -EINVAL;
    }

    tick_t t;

    if (timeout < 0) {
        t = RHINO_WAIT_FOREVER;
    } else {
        t = timeout;
    }

    kstat_t ret = krhino_sem_take(sem_handle, t);

    if (ret == RHINO_SUCCESS) {
        return 0;
    } else {
        return -EPERM;
    }
}

k_status_t csi_kernel_sem_post(k_sem_handle_t sem_handle)
{
    if (sem_handle == NULL) {
        return -EINVAL;
    }

    kstat_t ret = krhino_sem_give(sem_handle);

    if (ret == RHINO_SUCCESS) {
        return 0;
    } else {
        return -EPERM;
    }
}

int32_t csi_kernel_sem_get_count(k_sem_handle_t sem_handle)
{
    if (sem_handle == NULL) {
        return -EINVAL;
    }

    sem_count_t cnt;
    kstat_t ret = krhino_sem_count_get(sem_handle, &cnt);

    if (ret == RHINO_SUCCESS) {
        return (int32_t)cnt;
    } else {
        return -EPERM;
    }
}

k_mpool_handle_t csi_kernel_mpool_new(void *p_addr, int32_t block_count, int32_t block_size)
{
    if (p_addr == NULL || block_count < 0 || block_size <= 0 || (block_size % 4 != 0)) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_BLK > 0)
    mblk_pool_t *handle = (mblk_pool_t *)malloc(sizeof(mblk_pool_t));

    if (handle == NULL) {
        return NULL;
    }

    kstat_t ret = krhino_mblk_pool_init(handle, "UserMp", p_addr, block_size, block_count * block_size);

    if (ret == RHINO_SUCCESS) {
        return handle;
    } else {
        return NULL;
    }

#else
    return NULL;
#endif
}

k_status_t csi_kernel_mpool_del(k_mpool_handle_t mp_handle)
{
    if (mp_handle == NULL) {
        return -EINVAL;
    }

    memset(mp_handle, 0, sizeof(mblk_pool_t));
    free(mp_handle);
    return 0;
}

void *csi_kernel_mpool_alloc(k_mpool_handle_t mp_handle, int32_t timeout)
{
    if (mp_handle == NULL) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_BLK > 0)
    uint32_t g_csi_rhino_blk = 0;
    kstat_t ret = krhino_mblk_alloc(mp_handle,(void **)&g_csi_rhino_blk);

    if (ret == RHINO_SUCCESS) {
        return (void *)g_csi_rhino_blk;
    } else {
        return NULL;
    }

#else
    return NULL;
#endif
}

k_status_t csi_kernel_mpool_free(k_mpool_handle_t mp_handle, void *block)
{
    if (mp_handle == NULL || block == NULL) {
        return -EINVAL;
    }

#if (RHINO_CONFIG_MM_BLK > 0)
    kstat_t ret = krhino_mblk_free(mp_handle, block);

    if (ret == RHINO_SUCCESS) {
        return 0;
    } else {
        return -EPERM;
    }

#else
    return -EPERM;
#endif
}

int32_t csi_kernel_mpool_get_count(k_mpool_handle_t mp_handle)
{
    if (mp_handle == NULL) {
        return 0;
    }

    mblk_pool_t *handle = (mblk_pool_t *)mp_handle;
    int32_t used = handle->blk_whole - handle->blk_avail;
    return used;
}

uint32_t csi_kernel_mpool_get_capacity(k_mpool_handle_t mp_handle)
{
    if (mp_handle == NULL) {
        return 0;
    }

    mblk_pool_t *handle = (mblk_pool_t *)mp_handle;
    return handle->blk_whole;
}

uint32_t csi_kernel_mpool_get_block_size(k_mpool_handle_t mp_handle)
{
    if (mp_handle == NULL) {
        return 0;
    }

    mblk_pool_t *handle = (mblk_pool_t *)mp_handle;
    return handle->blk_size;
}

typedef struct mq_adapter {
    kbuf_queue_t    *buf_q;
    int32_t         msg_size;
    int32_t         msg_count;
} mq_adapter_t;
k_msgq_handle_t csi_kernel_msgq_new(int32_t msg_count, int32_t msg_size)
{
    if (msg_count <= 0 || msg_size <= 0) {
        return NULL;
    }

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
    mq_adapter_t *handle = (mq_adapter_t *)malloc(sizeof(mq_adapter_t));

    if (handle == NULL) {
        return NULL;
    }

    kstat_t ret  = krhino_buf_queue_dyn_create(&(handle->buf_q), "UserMsgQ", msg_size * (msg_count + 1), msg_size);

    if (ret == RHINO_SUCCESS) {
        handle->msg_count = msg_count;
        handle->msg_size = msg_size;
        return (k_msgq_handle_t)handle;
    } else {
        free(handle);
        return NULL;
    }

#else
    return NULL;
#endif
}

k_status_t csi_kernel_msgq_del(k_msgq_handle_t mq_handle)
{
    if (!mq_handle) {
        return -EINVAL;
    }

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
    mq_adapter_t *handle = (mq_adapter_t *)mq_handle;
    kstat_t ret  = krhino_buf_queue_dyn_del(handle->buf_q);

    if (ret == RHINO_SUCCESS) {
        free(mq_handle);
        return 0;
    } else {
        return -EPERM;
    }

#else
    return -EPERM;
#endif
}

k_status_t csi_kernel_msgq_put(k_msgq_handle_t mq_handle, const void *msg_ptr, uint8_t front_or_back, int32_t timeout)
{
    if ((!mq_handle) || (msg_ptr == NULL) || (front_or_back > 1)) {
        return -EINVAL;
    }

    mq_adapter_t *handle = (mq_adapter_t *)mq_handle;

    if (front_or_back == 0) {
        kstat_t ret = krhino_buf_queue_send(handle->buf_q, (void *)msg_ptr, handle->msg_size);

        if (ret == RHINO_SUCCESS) {
            return 0;
        } else {
            return -EPERM;
        }
    } else if (front_or_back == 1) {
        kstat_t ret = krhino_buf_queue_send_front(handle->buf_q, (void *)msg_ptr, handle->msg_size);

        if (ret == RHINO_SUCCESS) {
            return 0;
        } else {
            return -EPERM;
        }
    }

    return -EPERM;
}


k_status_t csi_kernel_msgq_get(k_msgq_handle_t mq_handle, void *msg_ptr, int32_t timeout)
{
    if (mq_handle == NULL || msg_ptr == NULL) {
        return -EINVAL;
    }

    tick_t t;

    if (timeout < 0) {
        t = RHINO_WAIT_FOREVER;
    } else {
        t = timeout;
    }

    mq_adapter_t *handle = (mq_adapter_t *)mq_handle;
    uint32_t size;
    kstat_t ret = krhino_buf_queue_recv(handle->buf_q, t, msg_ptr, &size);

    if (ret == RHINO_SUCCESS) {
        return 0;
    } else {
        return -EPERM;
    }
}

int32_t csi_kernel_msgq_get_count(k_msgq_handle_t mq_handle)
{
    if (mq_handle == NULL) {
        return -EINVAL;
    }

    mq_adapter_t *handle = (mq_adapter_t *)mq_handle;
    kbuf_queue_info_t info;
    kstat_t ret = krhino_buf_queue_info_get(handle->buf_q, &info);

    if (ret == RHINO_SUCCESS) {
        int32_t cnt = info.cur_num;
        return cnt;
    } else {
        return -EPERM;
    }
}

uint32_t csi_kernel_msgq_get_capacity(k_msgq_handle_t mq_handle)
{
    if (mq_handle == NULL) {
        return 0;
    }

    mq_adapter_t *handle = (mq_adapter_t *)mq_handle;
    return handle->msg_count;
}

uint32_t csi_kernel_msgq_get_msg_size(k_msgq_handle_t mq_handle)
{
    if (mq_handle == NULL) {
        return 0;
    }

    mq_adapter_t *handle = (mq_adapter_t *)mq_handle;
    return handle->buf_q->max_msg_size;
}

k_status_t csi_kernel_msgq_flush(k_msgq_handle_t mq_handle)
{
    if (mq_handle == NULL) {
        return -EINVAL;
    }

    mq_adapter_t *handle = (mq_adapter_t *)mq_handle;
    kstat_t ret = krhino_buf_queue_flush(handle->buf_q);

    if (ret == RHINO_SUCCESS) {
        return 0;
    } else {
        return -EPERM;
    }
}

void *csi_kernel_malloc(int32_t size, void *caller)
{
    void *ret;

    csi_kernel_sched_suspend();
    ret = krhino_mm_alloc(size);
    csi_kernel_sched_resume(0);
    return ret;
}

void csi_kernel_free(void *ptr, void *caller)
{
    csi_kernel_sched_suspend();
    krhino_mm_free(ptr);
    csi_kernel_sched_resume(0);
}

void *csi_kernel_realloc(void *ptr, int32_t size, void *caller)
{
    void *new_ptr;

    new_ptr = krhino_mm_realloc(ptr, size);

    return new_ptr;
}

k_status_t csi_kernel_get_mminfo(int32_t *total, int32_t *used, int32_t *free, int32_t *peak)
{
    #if 0
    *total = g_kmm_head->used_size + g_kmm_head->free_size;
    *used = g_kmm_head->used_size;
    *free = g_kmm_head->free_size;
    *peak = g_kmm_head->maxused_size;
    #endif
    return 0;
}

k_status_t csi_kernel_mm_dump(void)
{
#if (RHINO_CONFIG_MM_DEBUG > 0)
    dumpsys_mm_info_func(NULL, 0);
#endif
    return 0;
}
