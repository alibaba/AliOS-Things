/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <u_task.h>
#include <u_proc_msg.h>
#include <rhino_syscall_arg.h>

/* ------------------ task ------------------ */
ktask_t *sys_krhino_cur_task_get_stub(void *arg)
{
    arg = arg;

    return krhino_cur_task_get();
}

void sys_krhino_task_sleep_stub(void *arg)
{
    krhino_task_sleep_syscall_arg_t *_arg = arg;

    krhino_task_sleep(_arg->tick);
}

kstat_t sys_krhino_utask_create_stub(void *arg)
{
    krhino_utask_create_syscall_arg_t *_arg = arg;

    return krhino_utask_create(_arg->task, _arg->name, _arg->arg,
                               _arg->prio, _arg->ticks, _arg->stack_buf,
                               _arg->stack_size, _arg->kstack_size,
                               _arg->entry, _arg->autorun);
}

kstat_t sys_krhino_utask_del_stub(void *arg)
{
    krhino_utask_del_syscall_arg_t *_arg = arg;

    return krhino_utask_del(_arg->task);
}

/* ------------------ process ------------------ */
kstat_t sys_krhino_uprocess_create_stub(void *arg)
{
    krhino_uprocess_create_syscall_arg_t *_arg = arg;

    return krhino_uprocess_create(_arg->task, _arg->name, _arg->arg,
                                  _arg->prio, _arg->ticks, _arg->stack_buf,
                                  _arg->stack_size, _arg->kstack_size,
                                  _arg->entry, _arg->pid, _arg->autorun);
}

kstat_t sys_krhino_uprocess_exit_stub(void *arg)
{
    arg = arg;

    return krhino_uprocess_exit();
}

void sys_krhino_uprocess_res_get_stub(void *arg)
{
    krhino_uprocess_res_get_syscall_arg_t *_arg = arg;

    krhino_uprocess_res_get(_arg->id, _arg->res);
}

/* ------------------- time ------------------ */
sys_time_t sys_krhino_sys_tick_get_stub(void *arg)
{
    arg = arg;

    return krhino_sys_tick_get();
}

sys_time_t sys_krhino_sys_time_get_stub(void *arg)
{
    arg = arg;

    return krhino_sys_time_get();
}

tick_t sys_krhino_ms_to_ticks_stub(void *arg)
{
    krhino_ms_to_ticks_syscall_arg_t *_arg = arg;

    return krhino_ms_to_ticks(_arg->ms);
}

sys_time_t sys_krhino_ticks_to_ms_stub(void *arg)
{
    krhino_ticks_to_ms_syscall_arg_t *_arg = arg;

    return krhino_ticks_to_ms(_arg->ticks);
}

/* ------------------ mutex ------------------ */
kstat_t sys_krhino_mutex_dyn_create_stub(void *arg)
{
    CPSR_ALLOC();

    krhino_mutex_dyn_create_syscall_arg_t *_arg = arg;

    ktask_t  *cur_task, *proc_task;
    kstat_t   ret;
    kmutex_t *mutex;

    ret = krhino_mutex_dyn_create(_arg->mutex, _arg->name);
    if (ret != RHINO_SUCCESS) {
        printf("mutex create syscall errno: %d\n", ret);
        return ret;
    }

    mutex = *(_arg->mutex);

    cur_task = krhino_cur_task_get();
    proc_task = (ktask_t *)cur_task->proc_addr;

    RHINO_CRITICAL_ENTER();
    klist_insert(&(proc_task->kobj_list.mutex_head), &mutex->blk_obj.obj_list);
    RHINO_CRITICAL_EXIT();

    return ret;
}

kstat_t sys_krhino_mutex_dyn_del_stub(void *arg)
{
    CPSR_ALLOC();

    krhino_mutex_dyn_del_syscall_arg_t *_arg = arg;

    kmutex_t *mutex = _arg->mutex;

    RHINO_CRITICAL_ENTER();
    klist_rm(&mutex->blk_obj.obj_list);
    RHINO_CRITICAL_EXIT();

    return krhino_mutex_dyn_del(_arg->mutex);
}

kstat_t sys_krhino_mutex_lock_stub(void *arg)
{
    krhino_mutex_lock_syscall_arg_t *_arg = arg;

    return krhino_mutex_lock(_arg->mutex, _arg->ticks);
}

kstat_t sys_krhino_mutex_unlock_stub(void *arg)
{
    krhino_mutex_unlock_syscall_arg_t *_arg = arg;

    return krhino_mutex_unlock(_arg->mutex);
}

/* ------------------ semphore ------------------ */
kstat_t sys_krhino_sem_dyn_create_stub(void *arg)
{
    CPSR_ALLOC();

    krhino_sem_dyn_create_syscall_arg_t *_arg = arg;

    ktask_t *cur_task, *proc_task;
    kstat_t  ret;
    ksem_t  *sem;

    ret = krhino_sem_dyn_create(_arg->sem, _arg->name, _arg->count);
    if (ret != RHINO_SUCCESS) {
        printf("sem create syscall errno: %d\n", ret);
        return ret;
    }

    sem = *(_arg->sem);

    cur_task = krhino_cur_task_get();
    proc_task = (ktask_t *)cur_task->proc_addr;

    RHINO_CRITICAL_ENTER();
    klist_insert(&(proc_task->kobj_list.sem_head), &sem->blk_obj.obj_list);
    RHINO_CRITICAL_EXIT();

    return ret;
}

kstat_t sys_krhino_sem_dyn_del_stub(void *arg)
{
    CPSR_ALLOC();

    krhino_sem_dyn_del_syscall_arg_t *_arg = arg;

    ksem_t *sem = _arg->sem;

    RHINO_CRITICAL_ENTER();
    klist_rm(&sem->blk_obj.obj_list);
    RHINO_CRITICAL_EXIT();

    return krhino_sem_dyn_del(_arg->sem);
}

kstat_t sys_krhino_sem_give_stub(void *arg)
{
    krhino_sem_give_syscall_arg_t *_arg = arg;

    return krhino_sem_give(_arg->sem);
}

kstat_t sys_krhino_sem_take_stub(void *arg)
{
    krhino_sem_take_syscall_arg_t *_arg = arg;

    return krhino_sem_take(_arg->sem, _arg->ticks);
}

/* -------------------- queue --------------------*/
kstat_t sys_krhino_queue_dyn_create_stub(void *arg)
{
    CPSR_ALLOC();

    krhino_queue_dyn_create_syscall_arg_t *_arg = arg;

    ktask_t  *cur_task, *proc_task;
    kstat_t   ret;
    kqueue_t *queue;

    ret = krhino_queue_dyn_create(_arg->queue, _arg->name, _arg->msg_num);
    if (ret != RHINO_SUCCESS) {
        printf("queue create syscall errno: %d\n", ret);
        return ret;
    }

    queue = *(_arg->queue);

    cur_task = krhino_cur_task_get();
    proc_task = (ktask_t *)cur_task->proc_addr;

    RHINO_CRITICAL_ENTER();
    klist_insert(&(proc_task->kobj_list.queue_head), &queue->blk_obj.obj_list);
    RHINO_CRITICAL_EXIT();

    return ret;
}

kstat_t sys_krhino_queue_dyn_del_stub(void *arg)
{
    CPSR_ALLOC();

    krhino_queue_dyn_del_syscall_arg_t *_arg = arg;

    kqueue_t *queue = _arg->queue;

    RHINO_CRITICAL_ENTER();
    klist_rm(&queue->blk_obj.obj_list);
    RHINO_CRITICAL_EXIT();

    return krhino_queue_dyn_del(_arg->queue);
}

kstat_t sys_krhino_queue_back_send_stub(void *arg)
{
     krhino_queue_back_send_syscall_arg_t *_arg = arg;

     return krhino_queue_back_send(_arg->queue, _arg->msg);
}

kstat_t sys_krhino_queue_all_send_stub(void *arg)
{
     krhino_queue_all_send_syscall_arg_t *_arg = arg;

     return krhino_queue_all_send(_arg->queue, _arg->msg);
}

 kstat_t sys_krhino_queue_recv_stub(void *arg)
{
     krhino_queue_recv_syscall_arg_t *_arg = arg;

     return krhino_queue_recv(_arg->queue, _arg->ticks, _arg->msg);
}

 kstat_t sys_krhino_queue_flush_stub(void *arg)
{
     krhino_queue_flush_syscall_arg_t *_arg = arg;

     return krhino_queue_flush(_arg->queue);
}

/* ------------------ buf queue -------------------*/
kstat_t sys_krhino_buf_queue_dyn_create_stub(void *arg)
{
    CPSR_ALLOC();

    krhino_buf_queue_dyn_create_syscall_arg_t *_arg = arg;

    ktask_t  *cur_task, *proc_task;
    kstat_t   ret;
    kbuf_queue_t *buf_queue;

    ret = krhino_buf_queue_dyn_create(_arg->queue, _arg->name,
            _arg->size, _arg->max_msg);
    if (ret != RHINO_SUCCESS) {
        printf("buf queue create syscall errno: %d\n", ret);
        return ret;
    }

    buf_queue = *(_arg->queue);

    cur_task = krhino_cur_task_get();
    proc_task = (ktask_t *)cur_task->proc_addr;

    RHINO_CRITICAL_ENTER();
    klist_insert(&(proc_task->kobj_list.buf_queue_head), &buf_queue->blk_obj.obj_list);
    RHINO_CRITICAL_EXIT();

    return ret;
}

kstat_t sys_krhino_fix_buf_queue_dyn_create_stub(void *arg)
{

    CPSR_ALLOC();

    krhino_fix_buf_queue_dyn_create_syscall_arg_t *_arg = arg;

    ktask_t  *cur_task, *proc_task;
    kstat_t   ret;
    kbuf_queue_t *buf_queue;

    ret = krhino_fix_buf_queue_dyn_create(_arg->queue, _arg->name,
            _arg->msg_size, _arg->msg_num);
    if (ret != RHINO_SUCCESS) {
        printf("fix buf queue create syscall errno: %d\n", ret);
        return ret;
    }

    buf_queue = *(_arg->queue);

    cur_task = krhino_cur_task_get();
    proc_task = (ktask_t *)cur_task->proc_addr;

    RHINO_CRITICAL_ENTER();
    klist_insert(&(proc_task->kobj_list.buf_queue_head), &buf_queue->blk_obj.obj_list);
    RHINO_CRITICAL_EXIT();

    return ret;
}

kstat_t sys_krhino_buf_queue_dyn_del_stub(void *arg)
{
    CPSR_ALLOC();

    krhino_buf_queue_dyn_del_syscall_arg_t *_arg = arg;

    kbuf_queue_t *buf_queue = _arg->queue;

    RHINO_CRITICAL_ENTER();
    klist_rm(&buf_queue->blk_obj.obj_list);
    RHINO_CRITICAL_EXIT();

    return krhino_buf_queue_dyn_del(_arg->queue);
}

kstat_t sys_krhino_buf_queue_send_stub(void *arg)
{
    krhino_buf_queue_send_syscall_arg_t *_arg = arg;

    return krhino_buf_queue_send(_arg->queue, _arg->msg, _arg->size);
}

kstat_t sys_krhino_buf_queue_recv_stub(void *arg)
{
    krhino_buf_queue_recv_syscall_arg_t *_arg = arg;

    return krhino_buf_queue_recv(_arg->queue, _arg->ticks, _arg->msg, _arg->size);
}

kstat_t sys_krhino_buf_queue_flush_stub(void *arg)
{
    krhino_buf_queue_flush_syscall_arg_t *_arg = arg;

    return krhino_buf_queue_flush(_arg->queue);
}

/* ----------------- event ------------------ */
kstat_t sys_krhino_event_dyn_create_stub(void *arg)
{
    krhino_event_dyn_create_syscall_arg_t *_arg = arg;

    return krhino_event_dyn_create(_arg->event, _arg->name, _arg->flags);
}

kstat_t sys_krhino_event_dyn_del_stub(void *arg)
{
    krhino_event_dyn_del_syscall_arg_t *_arg = arg;

    return krhino_event_dyn_del(_arg->event);
}

kstat_t sys_krhino_event_get_stub(void *arg)
{
    krhino_event_get_syscall_arg_t *_arg = arg;

    return krhino_event_get(_arg->event, _arg->flags, _arg->opt,
                            _arg->actl_flags, _arg->ticks);
}

kstat_t sys_krhino_event_set_stub(void *arg)
{
    krhino_event_set_syscall_arg_t *_arg = arg;

    return krhino_event_set(_arg->event, _arg->flags, _arg->opt);
}

/* ----------------- proc msg -------------------- */
size_t sys_krhino_msg_get_stub(void *arg)
{
    krhino_msg_get_syscall_arg_t *_arg = arg;

    return krhino_msg_get(_arg->key, _arg->flg, _arg->size);
}

kstat_t sys_krhino_msg_snd_stub(void *arg)
{
    krhino_msg_snd_syscall_arg_t *_arg = arg;

    return krhino_msg_snd(_arg->msg_id, _arg->msg, _arg->msg_sz);
}

kstat_t sys_krhino_msg_recv_stub(void *arg)
{
    krhino_msg_recv_syscall_arg_t *_arg = arg;

    return krhino_msg_recv(_arg->msg_id, _arg->ticks, _arg->msg, _arg->msg_sz);
}

