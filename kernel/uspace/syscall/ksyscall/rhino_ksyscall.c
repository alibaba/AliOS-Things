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
    krhino_mutex_dyn_create_syscall_arg_t *_arg = arg;

    return krhino_mutex_dyn_create(_arg->mutex, _arg->name);
}

kstat_t sys_krhino_mutex_dyn_del_stub(void *arg)
{
    krhino_mutex_dyn_del_syscall_arg_t *_arg = arg;

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
    krhino_sem_dyn_create_syscall_arg_t *_arg = arg;

    return krhino_sem_dyn_create(_arg->sem, _arg->name, _arg->count);
}

kstat_t sys_krhino_sem_dyn_del_stub(void *arg)
{
    krhino_sem_dyn_del_syscall_arg_t *_arg = arg;

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
    krhino_queue_dyn_create_syscall_arg_t *_arg = arg;

    return krhino_queue_dyn_create(_arg->queue, _arg->name, _arg->msg_num);
}

kstat_t sys_krhino_queue_dyn_del_stub(void *arg)
{
    krhino_queue_dyn_del_syscall_arg_t *_arg = arg;

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

    krhino_buf_queue_dyn_create_syscall_arg_t *_arg = arg;

    return krhino_buf_queue_dyn_create(_arg->queue, _arg->name,
                                       _arg->size, _arg->max_msg);
}

kstat_t sys_krhino_fix_buf_queue_dyn_create_stub(void *arg)
{

    krhino_fix_buf_queue_dyn_create_syscall_arg_t *_arg = arg;

    return krhino_fix_buf_queue_dyn_create(_arg->queue, _arg->name,
                                           _arg->msg_size, _arg->msg_num);
}

kstat_t sys_krhino_buf_queue_dyn_del_stub(void *arg)
{
    krhino_buf_queue_dyn_del_syscall_arg_t *_arg = arg;

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

