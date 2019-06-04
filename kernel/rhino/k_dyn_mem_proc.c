/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_api.h"

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)

#if (RHINO_CONFIG_USER_SPACE > 0)

#include "task_group.h"
#include "res.h"
#include "k_proc.h"

static void free_user_res(task_group_t *group)
{
    kmutex_t     *mutex;
    klist_t      *head, *iter;

    /*mutex res exit*/
    head = &(group->kobj_list.mutex_head);
    iter = head->next;

    while (iter != head) {
        mutex = krhino_list_entry(iter, kmutex_t, blk_obj.obj_list);
        iter = iter->next;
        krhino_mutex_dyn_del(mutex);
    }

    /*sem res exit*/
#if (RHINO_CONFIG_SEM > 0)
    ksem_t       *sem;
    head = &(group->kobj_list.sem_head);
    iter = head->next;

    while (iter != head) {
        sem = krhino_list_entry(iter, ksem_t, blk_obj.obj_list);
        iter = iter->next;
        krhino_sem_dyn_del(sem);
    }
#endif

    /*queue res exit*/
#if (RHINO_CONFIG_QUEUE > 0)
    kqueue_t     *queue;
    head = &(group->kobj_list.queue_head);
    iter = head->next;

    while (iter != head) {
        queue = krhino_list_entry(iter, kqueue_t, blk_obj.obj_list);
        iter = iter->next;
        krhino_queue_dyn_del(queue);
    }
#endif

    /*buf queue res exit*/
#if (RHINO_CONFIG_BUF_QUEUE > 0)
    kbuf_queue_t *buf_queue;
    head = &(group->kobj_list.buf_queue_head);
    iter = head->next;

    while (iter != head) {
        buf_queue = krhino_list_entry(iter, kbuf_queue_t, blk_obj.obj_list);
        iter = iter->next;
        krhino_buf_queue_dyn_del(buf_queue);
    }
#endif

    /* event res exit*/
#if (RHINO_CONFIG_EVENT_FLAG > 0)
    kevent_t     *event;
    head = &(group->kobj_list.event_head);
    iter = head->next;

    while (iter != head) {
        event = krhino_list_entry(iter, kevent_t, blk_obj.obj_list);
        iter = iter->next;
        krhino_event_dyn_del(event);
    }
#endif
}

static void proc_free(ktask_t *task)
{
    task_group_t *group;
    int           pid;

    group = task->task_group;
    if (NULL == group) {
        return;
    }

    task_group_remove_res_obj(group, RES_OBJ_TASK, task);

    if (group->task_cnt > 0) {
        if (group->state == TGS_NORMAL) {
            res_free(group->pid, task->task_ustack_base);
        }
    } else {
        pid = group->pid;
        free_user_res(group);
        task_group_release(group);
        task_group_destory(group);
        k_proc_unload(pid);
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

