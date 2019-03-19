/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <k_api.h>
#include <u_task.h>
#include <u_callback.h>
#include <aos/cli.h>
#include <u_res.h>

extern int linkkit_close(int pid);
extern int fs_fd_close(int pid);
extern int socket_fd_close(int pid);

klist_t group_info_head = {
    &group_info_head,
    &group_info_head
};

static kstat_t task_create(ktask_t *task, const name_t *name, void *arg,
                           uint8_t prio, tick_t ticks, cpu_stack_t *ustack_buf,
                           size_t ustack_size, cpu_stack_t *kstack_buf, size_t kstack_size,
                           task_entry_t entry, uint8_t autorun,uint8_t mm_alloc_flag,
                           uint8_t cpu_num, uint8_t cpu_binded, uint32_t pid, uint8_t is_proc)
{
    CPSR_ALLOC();

    cpu_stack_t *tmp;
    ktask_t     *cur_task;
    ktask_t     *proc_task;
    uint8_t      i = 0;

    NULL_PARA_CHK(task);
    NULL_PARA_CHK(name);
    NULL_PARA_CHK(entry);
    NULL_PARA_CHK(ustack_buf);
    NULL_PARA_CHK(kstack_buf);

    if (kstack_size == 0u) {
        return RHINO_TASK_INV_STACK_SIZE;
    }

    if (prio >= RHINO_CONFIG_PRI_MAX) {
        return RHINO_BEYOND_MAX_PRI;
    }

    RHINO_CRITICAL_ENTER();

    INTRPT_NESTED_LEVEL_CHK();

    /* idle task is only allowed to create once */
    if (prio == RHINO_IDLE_PRI) {
        if (g_idle_task_spawned[cpu_num] > 0u) {
            RHINO_CRITICAL_EXIT();
            return RHINO_IDLE_TASK_EXIST;
        }

        g_idle_task_spawned[cpu_num] = 1u;
    }

    RHINO_CRITICAL_EXIT();

    memset(task, 0, sizeof(ktask_t));

#if (RHINO_CONFIG_SCHED_RR > 0)
    if (ticks > 0u) {
        task->time_total = ticks;
    } else {
        task->time_total = RHINO_CONFIG_TIME_SLICE_DEFAULT;
    }

    task->time_slice   = task->time_total;
    task->sched_policy = KSCHED_RR;
#endif

    if (autorun > 0u) {
        task->task_state    = K_RDY;
    } else {
        task->task_state    = K_SUSPENDED;
        task->suspend_count = 1u;
    }

    /* init all the stack element to 0 */
    task->task_stack_base = kstack_buf;
    tmp = kstack_buf;

    memset(tmp, 0, kstack_size * sizeof(cpu_stack_t));

    task->task_name          = name;
    task->prio               = prio;
    task->b_prio             = prio;
    task->stack_size         = kstack_size;
    task->mm_alloc_flag      = mm_alloc_flag;
    task->cpu_num            = cpu_num;
    task->ustack_size        = ustack_size;
    task->task_ustack_base   = ustack_buf;
    task->mode               = 0x3;
    task->pid                = pid;
    task->is_proc            = is_proc;

    cpu_binded = cpu_binded;
    i          = i;

    if (!is_proc) {
        cur_task = krhino_cur_task_get();
        task->task_group = cur_task->task_group;
        task_group_add_res_obj(cur_task->task_group, RES_OBJ_TASK, task);
    }

#if (RHINO_CONFIG_CPU_NUM > 1)
    task->cpu_binded    = cpu_binded;
#endif

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
#if (RHINO_CONFIG_CPU_STACK_DOWN > 0)
    tmp  = task->task_stack_base;
    for (i = 0; i < RHINO_CONFIG_STK_CHK_WORDS; i++) {
        *tmp++ = RHINO_TASK_STACK_OVF_MAGIC;
    }
#else
    tmp  = (cpu_stack_t *)(task->task_stack_base) + task->stack_size - RHINO_CONFIG_STK_CHK_WORDS;
    for (i = 0; i < RHINO_CONFIG_STK_CHK_WORDS; i++) {
        *tmp++ = RHINO_TASK_STACK_OVF_MAGIC;
    }
#endif
#endif

    cpu_utask_stack_init(task, kstack_buf, kstack_size,
                         ustack_buf, ustack_size,
                         arg, entry);

#if (RHINO_CONFIG_USER_HOOK > 0)
    krhino_task_create_hook(task);
#endif

    TRACE_TASK_CREATE(task);

    RHINO_CRITICAL_ENTER();

#if (RHINO_CONFIG_SYSTEM_STATS > 0)
    klist_insert(&(g_kobj_list.task_head), &task->task_stats_item);
#endif

    if (autorun > 0u) {
        ready_list_add_tail(&g_ready_queue, task);
        /* if system is not start,not call core_sched */
        if (g_sys_stat == RHINO_RUNNING) {
            RHINO_CRITICAL_EXIT_SCHED();
            return RHINO_SUCCESS;
        }
    }

    RHINO_CRITICAL_EXIT();

    return RHINO_SUCCESS;
}

static kstat_t utask_create(ktask_t **task, const name_t *name, void *arg,
                            uint8_t pri, tick_t ticks, cpu_stack_t *ustack_buf,
                            size_t ustack, size_t kstack, task_entry_t entry,
                            uint8_t cpu_num, uint8_t cpu_binded, uint32_t pid,
                            uint8_t autorun, uint8_t is_proc)
{
    kstat_t      ret;
    cpu_stack_t *ktask_stack;
    ktask_t     *task_obj;

    NULL_PARA_CHK(task);

    if (kstack == 0) {
        return RHINO_INV_PARAM;
    }

    ktask_stack = krhino_mm_alloc(kstack * sizeof(cpu_stack_t));
    if (ktask_stack == NULL) {
        return RHINO_NO_MEM;
    }

    task_obj = krhino_mm_alloc(sizeof(ktask_t));
    if (task_obj == NULL) {
        krhino_mm_free(ktask_stack);
        return RHINO_NO_MEM;
    }

   *task = task_obj;

    ret = task_create(task_obj, name, arg, pri, ticks, ustack_buf, ustack,
                      ktask_stack, kstack, entry, autorun, K_OBJ_DYN_ALLOC,
                      cpu_num, cpu_binded, pid, is_proc);

    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        krhino_mm_free(ktask_stack);
        krhino_mm_free(task_obj);

        return ret;
    }

    return ret;
}

kstat_t krhino_utask_create(ktask_t **task, const name_t *name, void *arg,
                            uint8_t pri, tick_t ticks, cpu_stack_t *ustack_buf,
                            size_t ustack, size_t kstack, task_entry_t entry, uint8_t autorun)
{
    ktask_t *cur_task;
    kstat_t  ret;

    cur_task = krhino_cur_task_get();

    ret = utask_create(task, name, arg, pri, ticks, ustack_buf,
                        ustack, kstack, entry, 0, 0, cur_task->pid, autorun, 0);

    return ret;
}

kstat_t krhino_utask_del(ktask_t *task)
{
    if (NULL == task) {
        task = krhino_cur_task_get();
    }
    task_group_remove_res_obj(task->task_group, RES_OBJ_TASK, task);
    return krhino_task_dyn_del(task);
}

void task_group_list_add(klist_t *node)
{
    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();
    klist_add(&group_info_head, node);
    RHINO_CRITICAL_EXIT();
}

void task_group_list_rm(klist_t *node)
{
    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();
    klist_rm(node);
    RHINO_CRITICAL_EXIT();
}

task_group_t* get_task_group_by_pid(uint32_t pid)
{
    klist_t *head;
    klist_t *list_iter;

    task_group_t *group_iter;
    task_group_t *group;

    CPSR_ALLOC();

    if (pid == 0) {
        return NULL;
    }

    group = NULL;
    head  = &group_info_head;

    RHINO_CRITICAL_ENTER();

    list_iter  = head->next;
    for (; list_iter != head; list_iter = list_iter->next) {
        group_iter = group_info_entry(list_iter, task_group_t, node);
        if (group_iter && (group_iter->pid == pid)) {
            group = group_iter;
            break;
        }
    }

    RHINO_CRITICAL_EXIT();

    return group;
}

int task_group_add_res_obj(task_group_t *group, unsigned int type, void *res)
{
    int ret = 0;

    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();

    switch(type) {
        case RES_OBJ_TASK:
            klist_init(&((ktask_t*)res)->task_user);
            klist_insert(&group->kobj_list.task_head, &((ktask_t*)res)->task_user);
            group->task_cnt++;
            break;
        case RES_OBJ_MUTEX:
            klist_insert(&group->kobj_list.mutex_head, &((kmutex_t*)res)->blk_obj.obj_list);
            break;
        case RES_OBJ_SEM:
            klist_insert(&group->kobj_list.sem_head, &((ksem_t*)res)->blk_obj.obj_list);
            break;
        case RES_OBJ_QUEUE:
            klist_insert(&group->kobj_list.queue_head, &((kqueue_t*)res)->blk_obj.obj_list);
            break;
        case RES_OBJ_BUF_QUEUE:
            klist_insert(&group->kobj_list.buf_queue_head, &((kbuf_queue_t*)res)->blk_obj.obj_list);
            break;
        case RES_OBJ_EVENT:
            klist_insert(&group->kobj_list.event_head, &((kevent_t*)res)->blk_obj.obj_list);
            break;
        default:
            ret = -1;
            break;
    }

    RHINO_CRITICAL_EXIT();

    return ret;
}

int task_group_remove_res_obj(task_group_t *group, unsigned int type, void *res)
{
    int ret = 0;

    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();

    switch(type) {
        case RES_OBJ_TASK:
            klist_rm(&((ktask_t*)res)->task_user);
            group->task_cnt--;
            break;
        case RES_OBJ_MUTEX:
            klist_rm(&((kmutex_t*)res)->blk_obj.obj_list);
            break;
        case RES_OBJ_SEM:
            klist_rm(&((ksem_t*)res)->blk_obj.obj_list);
            break;
        case RES_OBJ_QUEUE:
            klist_rm(&((kqueue_t*)res)->blk_obj.obj_list);
            break;
        case RES_OBJ_BUF_QUEUE:
            klist_rm(&((kbuf_queue_t*)res)->blk_obj.obj_list);
            break;
        case RES_OBJ_EVENT:
            klist_rm(&((kevent_t*)res)->blk_obj.obj_list);
            break;
        default:
            ret = -1;
            break;
    }

    RHINO_CRITICAL_EXIT();

    return ret;
}

void task_group_release(task_group_t *group)
{
    krhino_sem_del(&group->res_sem);

    krhino_sem_del(&group->fs_sem);

    krhino_sem_del(&group->socket_sem);

    krhino_sem_del(&group->exit_sem);

    if (NULL != group->res_request_q) {
        krhino_buf_queue_dyn_del(group->res_request_q);
        group->res_request_q = NULL;
    }

    if (NULL != group->res_reply_q) {
        krhino_buf_queue_dyn_del(group->res_reply_q);
        group->res_request_q = NULL;
    }

    if (NULL != group->cb_call_buf_queue) {
        krhino_buf_queue_dyn_del(group->cb_call_buf_queue);
        group->cb_call_buf_queue = NULL;
    }

    if (NULL != group->cb_ret_buf_queue) {
        krhino_buf_queue_dyn_del(group->cb_ret_buf_queue);
        group->cb_ret_buf_queue = NULL;
    }

    if (NULL != group->cli_q) {
        krhino_buf_queue_dyn_del(group->cli_q);
        group->cli_q = NULL;
    }

    task_group_list_rm(&group->node);

    krhino_mm_free(group);

    printf("process %s is unloaded\r\n", group->pname);
}

kstat_t krhino_uprocess_create(const name_t *process_name, ktask_t **task,
                               const name_t *name, void *arg,
                               uint8_t pri, tick_t ticks, cpu_stack_t *ustack_buf,
                               size_t ustack, size_t kstack, task_entry_t entry,
                               uint32_t pid, uint8_t autorun)
{
    CPSR_ALLOC();

    ktask_t  *task_tmp;
    kbuf_queue_t *res_request_q;
    kbuf_queue_t *res_reply_q;
    kbuf_queue_t *cli_buf_q;
    task_group_t *group;
    int pname_len;
    kstat_t   ret;

    ret = utask_create(task, name, arg, pri, ticks, ustack_buf,
                        ustack, kstack, entry, 0, 0, pid, 0, 1);

    if (ret != RHINO_SUCCESS) {
        return ret;
    }

    task_tmp = *task;

    //TODO: Remember to delete the group_info when task exit.
    group = (task_group_t*)krhino_mm_alloc(sizeof(task_group_t));
    if (NULL == group) {
        ret = RHINO_NO_MEM;
        goto delete_task;
    }

    memset(group, 0, sizeof(task_group_t));
    group->pid = pid;
    pname_len = strlen((const char*)process_name);
    if (pname_len >= PNAME_MAX_LEN)
        pname_len = PNAME_MAX_LEN - 1;
    memcpy(group->pname, (const char*)process_name, pname_len);

    group->state = TGS_NORMAL;

    /* init process exit semaphore */
    krhino_sem_create(&group->exit_sem, group->pname, 0);

    /* process kobj init */
    klist_init(&(group->kobj_list.task_head));
    klist_init(&(group->kobj_list.mutex_head));
    klist_init(&(group->kobj_list.sem_head));
    klist_init(&(group->kobj_list.queue_head));
    klist_init(&(group->kobj_list.buf_queue_head));
    klist_init(&(group->kobj_list.event_head));
    task_tmp->task_group = group;
    task_group_add_res_obj(group, RES_OBJ_TASK, (void*)(*task));

    klist_init(&group->node);
    task_group_list_add(&group->node);

    klist_init(&group->fs_fd_list);
    krhino_sem_create(&group->fs_sem, "fs_fd_sem", 1);

    klist_init(&group->socket_fd_list);
    krhino_sem_create(&group->socket_sem, "socket_fd_sem", 1);

    group->linkkit_running = 0;

    ret = krhino_fix_buf_queue_dyn_create(&res_request_q,
                                    "res_request_buf_queue",
                                    sizeof(res_request_msg_t),
                                    RES_QUEUE_MSG_NUM);
    if (ret != RHINO_SUCCESS) {
        goto delete_group_info;
    }
    printf("%s: created res_request_q 0x%x\r\n", __func__, res_request_q);

    group->res_request_q = res_request_q;

    ret = krhino_fix_buf_queue_dyn_create(&res_reply_q,
                                    "res_reply_buf_queue",
                                    sizeof(res_reply_msg_t),
                                    RES_QUEUE_MSG_NUM);
    if (ret != RHINO_SUCCESS) {
        goto delete_res_request_q;
    }
    printf("%s: created res_reply_q 0x%x\r\n", __func__, res_reply_q);

    group->res_reply_q = res_reply_q;

    krhino_sem_create(&group->res_sem, "res_sem", 1);

    ret = krhino_fix_buf_queue_dyn_create(&cli_buf_q,
                                          "cli_buf_queue",
                                          sizeof(ucli_msg_t),
                                          2);
    if (ret != RHINO_SUCCESS) {
        goto delete_res_reply_q;
    }
    group->cli_q = cli_buf_q;

    ret = create_cb_buf_queues(task_tmp);
    if (ret != RHINO_SUCCESS) {
        goto delete_res_reply_q;
    }

    if (autorun > 0) {
        krhino_task_resume(task_tmp);
    }

    return RHINO_SUCCESS;

delete_res_reply_q:
    krhino_sem_del(&group->res_sem);
    krhino_buf_queue_dyn_del(res_reply_q);
    group->res_reply_q = NULL;

delete_res_request_q:
    krhino_buf_queue_dyn_del(res_request_q);
    group->res_request_q = NULL;

delete_group_info:
    krhino_sem_del(&group->fs_sem);
    krhino_sem_del(&group->socket_sem);
    task_group_list_rm(&group->node);
    krhino_mm_free(group);

delete_task:
    krhino_utask_del(task_tmp);

    return ret;
}

int krhino_uprocess_kill(int pid)
{
    task_group_t *group;

    group = get_task_group_by_pid(pid);
    if (NULL == group)
        return -1;

    printf("kill process %s, pid %d\r\n", group->pname, pid);

    u_res_kill(pid);

    return 0;
}

int krhino_uprocess_kill_force(int pid)
{
    task_group_t  *group;

    group = get_task_group_by_pid(pid);
    if (NULL != group) {
        krhino_sem_give(&group->exit_sem);
    }

    return 0;
}

kstat_t krhino_uprocess_exit(void)
{
    ktask_t  *cur_task;
    task_group_t  *group;

    cur_task = krhino_cur_task_get();
    group = cur_task->task_group;

    krhino_sem_give(&group->exit_sem);
    krhino_task_suspend(cur_task);

    return RHINO_SUCCESS;
}

extern int aos_cli_process_exit(int pid);
kstat_t krhino_uprocess_join(void)
{
    ktask_t  *cur_task;
    task_group_t *group;
    klist_t *iter;
    ktask_t *task;
    klist_t  *head;
    int       retry = 0;
    kstat_t   ret;

    CPSR_ALLOC();

    cur_task = krhino_cur_task_get();
    task_pri_change(cur_task, RHINO_CONFIG_USER_PRI_MAX);
    /* make sure the task is running at the expected priority */
    krhino_task_yield();

    group = cur_task->task_group;
    krhino_sem_take(&group->exit_sem, RHINO_WAIT_FOREVER);

    // don't free task ustack when process exits
    group->state = TGS_EXITING;
    head = &group->kobj_list.task_head;

#if 0
    RHINO_CRITICAL_ENTER();
    // 1. suspend all tasks
    iter = head->next;
    while (iter != head) {
        task = krhino_list_entry(iter, ktask_t, task_user);
        iter = iter->next;
        if (cur_task != task) {
            krhino_task_suspend(task);
        }
    }
    RHINO_CRITICAL_EXIT();
#endif

    // 2. delete all tasks
    RHINO_CRITICAL_ENTER();
    while (!is_klist_empty(head)) {
        iter = head->next;
        task = krhino_list_entry(iter, ktask_t, task_user);
        RHINO_CRITICAL_EXIT();
        if (cur_task != task) {
            task_group_remove_res_obj(group, RES_OBJ_TASK, task);
            krhino_task_dyn_del(task);
        } else {
	    if (!retry) {
                task_group_remove_res_obj(group, RES_OBJ_TASK, task);
                task_group_add_res_obj(group, RES_OBJ_TASK, task);
		retry = 1;
	    } else {
	     	break;
	    }
	}
        RHINO_CRITICAL_ENTER();
    }

    RHINO_CRITICAL_EXIT();

    aos_cli_process_exit(group->pid);

    linkkit_close(group->pid);

    // fs_fd_close(group->pid);
    socket_fd_close(group->pid);

    task_group_remove_res_obj(group, RES_OBJ_TASK, cur_task);
    krhino_task_dyn_del(cur_task);

    return RHINO_SUCCESS;
}

void krhino_uprocess_res_get(int32_t id, void **res)
{
    CPSR_ALLOC();

    ktask_t *cur_task;
    task_group_t *group;
    klist_t *head;

    cur_task = krhino_cur_task_get();
    group = cur_task->task_group;

    switch(id) {
        case USER_RES_REQUEST_BUF_QUEUE:
            *res = group->res_request_q;
            break;
        case USER_RES_REPLY_BUF_QUEUE:
            *res = group->res_reply_q;
            break;
        case USER_CLI_BUF_QUEUE:
            *res = group->cli_q;
            break;
        case USER_CB_CALL_BUF_QUEUE:
            if (group && (group->cb_call_buf_queue)) {
                *res = group->cb_call_buf_queue;
            } else {
                *res = NULL;
            }
            break;
        case USER_CB_RET_BUF_QUEUE:
            if (group && (group->cb_ret_buf_queue)) {
                *res = group->cb_ret_buf_queue;
            } else {
                *res = NULL;
            }
            break;
        default:
            printf("%s: Unknown request, id 0x%x\r\n", __func__, id);
            break;
    }
}

