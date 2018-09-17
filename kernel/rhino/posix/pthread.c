/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <pthread.h>

void pthread_cleanup_pop(int execute)
{
    CPSR_ALLOC();

    _pthread_tcb_t *    ptcb;
    _pthread_cleanup_t *cleanup;

    ptcb = _pthread_get_tcb(krhino_cur_task_get());

    if (execute > 0) {
        RHINO_CRITICAL_ENTER();
        cleanup = ptcb->cleanup;
        if (cleanup) {
            ptcb->cleanup = cleanup->prev;
        }
        RHINO_CRITICAL_EXIT();

        if (cleanup != 0) {
            cleanup->cleanup_routine(cleanup->para);
            krhino_mm_free(cleanup);
        }
    }
}

void pthread_cleanup_push(void (*routine)(void *), void *arg)
{
    CPSR_ALLOC();

    _pthread_tcb_t *    ptcb;
    _pthread_cleanup_t *cleanup;

    ptcb = _pthread_get_tcb(krhino_cur_task_get());

    cleanup = (_pthread_cleanup_t *)krhino_mm_alloc(sizeof(_pthread_cleanup_t));
    if (cleanup != 0) {
        cleanup->cleanup_routine = routine;
        cleanup->para            = arg;

        RHINO_CRITICAL_ENTER();
        cleanup->prev = ptcb->cleanup;
        ptcb->cleanup = cleanup;
        RHINO_CRITICAL_EXIT();
    }
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *   arg)
{
    kstat_t         ret = 0;
    void *          stack;
    _pthread_tcb_t *ptcb;

    if (thread == NULL) {
        return -1;
    }

    /* create _pthread_tcb_t */
    ptcb = (_pthread_tcb_t *)krhino_mm_alloc(sizeof(_pthread_tcb_t));
    if (ptcb == 0) {
        return -1;
    }

    memset(ptcb, 0, sizeof(_pthread_tcb_t));

    ptcb->canceled     = 0;
    ptcb->cancel_state = PTHREAD_CANCEL_DISABLE;
    ptcb->cancel_type  = PTHREAD_CANCEL_DEFERRED;
    ptcb->magic        = PTHREAD_MAGIC;

    /* get pthread attr */
    if (attr != 0) {
        ptcb->attr = *attr;
    } else {
        pthread_attr_init(&ptcb->attr);
    }

    /* get stack addr */
    if (ptcb->attr.stackaddr == 0) {
        stack = (void *)krhino_mm_alloc(ptcb->attr.stacksize);
        if (stack == 0) {
            krhino_mm_free(ptcb);
            return -1;
        }
    } else {
        stack = (void *)(ptcb->attr.stackaddr);
    }

    /* create ktask_t */
    ptcb->tid = krhino_mm_alloc(sizeof(ktask_t));
    if (ptcb->tid == 0) {
        if (ptcb->attr.stackaddr == 0) {
            krhino_mm_free(stack);
        }

        krhino_mm_free(ptcb);
        return -1;
    }

    if (ptcb->attr.detachstate == PTHREAD_CREATE_JOINABLE) {
        ret = krhino_sem_dyn_create(&ptcb->join_sem, "join_sem", 0);
        if (ret != RHINO_SUCCESS) {
            if (ptcb->attr.stackaddr == 0) {
                krhino_mm_free(stack);
            }

            krhino_mm_free(ptcb);
            return -1;
        }
    } else {
        ptcb->join_sem = 0;
    }

    ptcb->thread_entry = start_routine;
    ptcb->thread_para  = arg;

    ret = krhino_task_create(ptcb->tid, "task", arg,
                             ptcb->attr.schedparam.sched_priority, 0, stack,
                             (ptcb->attr.stacksize / sizeof(cpu_stack_t)),
                             (task_entry_t)start_routine, 0);

    if (ret != RHINO_SUCCESS) {
        if (ptcb->attr.stackaddr == 0) {
            krhino_mm_free(stack);
        }

        if (ptcb->join_sem != 0) {
            krhino_sem_dyn_del(ptcb->join_sem);
        }
        krhino_mm_free(ptcb);
        return -1;
    }

    *thread                                     = ptcb->tid;
    ptcb->tid->user_info[PTHREAD_USER_INFO_POS] = ptcb;

    ret = krhino_task_resume(ptcb->tid);
    if (ret == RHINO_SUCCESS) {
        return RHINO_SUCCESS;
    }

    if (ptcb->attr.stackaddr == 0) {
        krhino_mm_free(stack);
    }

    krhino_mm_free(ptcb);

    return -1;
}

void pthread_exit(void *value)
{
    CPSR_ALLOC();

    _pthread_tcb_t *ptcb;

    ptcb = _pthread_get_tcb(krhino_cur_task_get());

    if (ptcb != NULL) {
        RHINO_CRITICAL_ENTER();
        ptcb->cancel_state = PTHREAD_CANCEL_DISABLE;
        ptcb->return_value = value;
        RHINO_CRITICAL_EXIT();

        /* ptcb->join_sem semaphore should be released by krhino_task_del_hook
         * if ptcb->attr.detachstate is PTHREAD_CREATE_DETACHED, task stack and
         * task structure should be release by krhino_task_del_hook
         */
        krhino_task_del(krhino_cur_task_get());
    }
}

int pthread_detach(pthread_t thread)
{
    CPSR_ALLOC();

    kstat_t         ret = 0;
    _pthread_tcb_t *ptcb;

    if (thread == NULL) {
        return -1;
    }

    if (thread == krhino_cur_task_get()) {
        return -1;
    }

    ptcb = _pthread_get_tcb(thread);

    if (ptcb == NULL) {
        return -1;
    }

    if (thread->task_state != K_DELETED) {
        if (ptcb->attr.detachstate == PTHREAD_CREATE_JOINABLE) {
            if (ptcb->join_sem != NULL) {
                krhino_sem_give(ptcb->join_sem);
            }

            RHINO_CRITICAL_ENTER();
            ptcb->attr.detachstate = PTHREAD_CREATE_DETACHED;
            RHINO_CRITICAL_EXIT();
        }
    } else {
        if (ptcb->attr.stackaddr == NULL) {
            krhino_mm_free(ptcb->tid->task_stack_base);
        }

        if (ptcb->join_sem != NULL) {
            ret = krhino_sem_dyn_del(ptcb->join_sem);
        }

        krhino_mm_free(ptcb->tid);
        krhino_mm_free(ptcb);
    }

    if (ret != 0) {
        return -1;
    }

    return 0;
}

int pthread_join(pthread_t thread, void **retval)
{
    kstat_t         ret = 0;
    _pthread_tcb_t *ptcb;

    if (thread == NULL) {
        return -1;
    }

    if (thread == krhino_cur_task_get()) {
        return -1;
    }

    ptcb = _pthread_get_tcb(thread);

    if (ptcb == NULL) {
        return -1;
    }

    if (ptcb->attr.detachstate == PTHREAD_CREATE_DETACHED) {
        return -1;
    }

    ret = krhino_sem_take(ptcb->join_sem, RHINO_WAIT_FOREVER);
    if (ret == RHINO_SUCCESS) {
        if (retval != 0) {
            *retval = ptcb->return_value;
        }

        krhino_sem_dyn_del(ptcb->join_sem);

        if (ptcb->attr.detachstate == PTHREAD_CREATE_JOINABLE) {
            if (ptcb->attr.stackaddr == 0) {
                krhino_mm_free(ptcb->tid->task_stack_base);
            }

            krhino_mm_free(ptcb->tid);
            krhino_mm_free(ptcb);
        }
    } else {
        return -1;
    }

    return 0;
}

int pthread_cancel(pthread_t thread)
{
    return 0;
}

void pthread_testcancel(void)
{
    return;
}

int pthread_setcancelstate(int state, int *oldstate)
{
    return 0;
}

int pthread_setcanceltype(int type, int *oldtype)
{
    return 0;
}

int pthread_kill(pthread_t thread, int sig)
{
    /* This api should not be used, and will not be supported */
    return 0;
}

int pthread_equal(pthread_t t1, pthread_t t2)
{
    return (int)(t1 == t2);
}

int pthread_setschedparam(pthread_t thread, /* thread               */
                          int       policy, /* new policy           */
                          const struct sched_param *param /* new parameters */
)
{
    kstat_t stat = RHINO_SUCCESS;
    uint8_t old_pri;

    if (param != 0) {
        return -1;
    }

    if (policy == SCHED_FIFO) {
        stat = krhino_sched_policy_set((ktask_t *)thread, KSCHED_FIFO);
    } else if (policy == SCHED_RR) {
        stat = krhino_sched_policy_set((ktask_t *)thread, KSCHED_RR);
    } else {
        return -1;
    }

    if (stat != RHINO_SUCCESS) {
        return -1;
    }

    /* change the priority of pthread */
    stat = krhino_task_pri_change(
      (ktask_t *)thread, PRI_CONVERT_PX_RH(param->sched_priority), &old_pri);
    if (stat != RHINO_SUCCESS) {
        return -1;
    }

    return 0;
}

pthread_t pthread_self(void)
{
    return krhino_cur_task_get();
}