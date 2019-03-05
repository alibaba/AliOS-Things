/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_api.h"

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)

#if (RHINO_CONFIG_USER_SPACE > 0)

static void res_user_free(ktask_t *task)
{
    klist_t      *listnode;
    kmutex_t     *mutex;
    ksem_t       *sem;
    kqueue_t     *queue;
    kbuf_queue_t *buf_queue;
    kevent_t     *event;
    klist_t      *head, *end, *tmp;

    /* mutex res exit */
    head = &(task->kobj_list.mutex_head);
    end  = head;
    tmp  = head->next;

    while (tmp != end) {
        mutex = krhino_list_entry(tmp, kmutex_t, blk_obj.obj_list);
        tmp   = tmp->next;
        krhino_mutex_dyn_del(mutex);
    }

    /* sem res exit */
    head = &(task->kobj_list.sem_head);
    end  = head;
    tmp  = head->next;

    while (tmp != end) {
        sem = krhino_list_entry(tmp, ksem_t, blk_obj.obj_list);
        tmp = tmp->next;
        krhino_sem_dyn_del(sem);
    }

    /* queue res exit */
    head = &(task->kobj_list.queue_head);
    end  = head;
    tmp  = head->next;

    while (tmp != end) {
        queue = krhino_list_entry(tmp, kqueue_t, blk_obj.obj_list);
        tmp   = tmp->next;
        krhino_queue_dyn_del(queue);
    }

    /* buf queue res exit */
    head = &(task->kobj_list.buf_queue_head);
    end  = head;
    tmp  = head->next;

    while (tmp != end) {
        buf_queue = krhino_list_entry(tmp, kbuf_queue_t, blk_obj.obj_list);
        tmp       = tmp->next;
        krhino_buf_queue_dyn_del(buf_queue);
    }
}

static void proc_free(ktask_t *task)
{
    CPSR_ALLOC();

    ktask_t        *proc = task->proc_addr;
    kqueue_t       *res_q;
    klist_t        *head = &task->task_head;
    static klist_t *head_tmp;

    if (task->is_proc == 1u) {
        res_q = proc->res_q;

        /* save task_head before utask delete */
        head_tmp = head;

        RHINO_CRITICAL_ENTER();
        klist_rm(&task->task_user);

        RHINO_CRITICAL_EXIT();
        krhino_queue_back_send(res_q, 0);
    } else {
        if (task->proc_addr != 0) {
            res_q = proc->res_q;

            RHINO_CRITICAL_ENTER();
            klist_rm(&task->task_user);

            if (head_tmp->next == head_tmp) {
                /* for next process exit */
                head_tmp = head;

                RHINO_CRITICAL_EXIT();

                res_user_free(task->proc_addr);
                k_proc_unload(task->pid);
            }
            RHINO_CRITICAL_EXIT();
            krhino_queue_back_send(res_q, task->task_ustack_base);
        }
    }
}
#endif

void dyn_mem_proc_task(void *arg)
{
    CPSR_ALLOC();

    size_t      i;
    kstat_t     ret;
    res_free_t *res_free;
    res_free_t  tmp;

    (void)arg;

    while (1) {
        ret = krhino_sem_take(&g_res_sem, RHINO_WAIT_FOREVER);
        if (ret != RHINO_SUCCESS) {
            k_err_proc(RHINO_DYN_MEM_PROC_ERR);
        }

        while (1) {
            RHINO_CRITICAL_ENTER();
            if (!is_klist_empty(&g_res_list)) {
                res_free = krhino_list_entry(g_res_list.next, res_free_t, res_list);
                klist_rm(&res_free->res_list);
                RHINO_CRITICAL_EXIT();
#if (RHINO_CONFIG_USER_SPACE > 0)
                proc_free((ktask_t *)(res_free->res[1]));
#endif
                memcpy(&tmp, res_free, sizeof(res_free_t));
                for (i = 0; i < tmp.cnt; i++) {
                    krhino_mm_free(tmp.res[i]);
                }
            } else {
                RHINO_CRITICAL_EXIT();
                break;
            }
        }
    }
}

void dyn_mem_proc_task_start(void)
{
    krhino_task_create(&g_dyn_task, "dyn_mem_proc_task", 0, RHINO_CONFIG_K_DYN_MEM_TASK_PRI,
                       0, g_dyn_task_stack, RHINO_CONFIG_K_DYN_TASK_STACK, dyn_mem_proc_task, 1);
}

#endif

