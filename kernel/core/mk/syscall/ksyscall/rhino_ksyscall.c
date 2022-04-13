/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <sched.h>
#include "k_api.h"
#include "utask.h"
#include "task_group.h"
#include "k_proc.h"
#include "ipc.h"
#include "rhino_syscall_arg.h"

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

kstat_t sys_krhino_task_suspend_stub(void *arg)
{
    krhino_task_suspend_syscall_arg_t *_arg = arg;

    return krhino_task_suspend(_arg->task);
}

kstat_t sys_krhino_task_resume_stub(void *arg)
{
    krhino_task_resume_syscall_arg_t *_arg = arg;

    return krhino_task_resume(_arg->task);
}

kstat_t sys_krhino_task_wait_abort_stub(void *arg)
{
    krhino_task_wait_abort_syscall_arg_t *_arg = arg;

    return krhino_task_wait_abort(_arg->task);
}

kstat_t sys_krhino_task_yield_stub(void *arg)
{
    arg = arg;

    return krhino_task_yield();
}

kstat_t sys_krhino_task_pri_change_stub(void *arg)
{
    krhino_task_pri_change_syscall_arg_t *_arg = arg;

    return krhino_task_pri_change(_arg->task, _arg->pri, _arg->old_pri);
}

kstat_t sys_krhino_task_info_get_stub(void *arg)
{
    krhino_task_info_get_syscall_arg_t *_arg = arg;

    return krhino_task_info_get(_arg->task, _arg->idx, _arg->info);
}

kstat_t sys_krhino_task_info_set_stub(void *arg)
{
    krhino_task_info_set_syscall_arg_t *_arg = arg;

    return krhino_task_info_set(_arg->task, _arg->idx, _arg->info);
}

kstat_t sys_krhino_task_ptcb_set_stub(void *arg)
{
    krhino_task_ptcb_set_syscall_arg_t *_arg = arg;

    krhino_task_ptcb_set(_arg->task, _arg->ptcb);

    return RHINO_SUCCESS;
}

kstat_t sys_krhino_task_ptcb_get_stub(void *arg)
{
    krhino_task_ptcb_get_syscall_arg_t *_arg = arg;

    krhino_task_ptcb_get(_arg->task, _arg->ptcb);

    return RHINO_SUCCESS;
}

kstat_t sys_krhino_task_name_set_stub(void *arg)
{
    krhino_task_name_set_syscall_arg_t *_arg = arg;

    if (_arg->task == NULL) {
        return RHINO_INV_PARAM;
    }

    _arg->task->task_name = _arg->name;

    return RHINO_SUCCESS;
}

kstat_t sys_krhino_task_name_get_stub(void *arg)
{
    krhino_task_name_get_syscall_arg_t *_arg = arg;

    if ((_arg->name == NULL) || (_arg->task == NULL)) {
        return RHINO_INV_PARAM;
    }

    *_arg->name = _arg->task->task_name;

    return RHINO_SUCCESS;
}

kstat_t sys_krhino_task_time_slice_set_stub(void *arg)
{
    krhino_task_time_slice_set_syscall_arg_t *_arg = arg;

    return krhino_task_time_slice_set(_arg->task, _arg->slice);
}

kstat_t sys_krhino_task_time_slice_get_stub(void *arg)
{
    krhino_task_time_slice_get_syscall_arg_t *_arg = arg;

    if (_arg->task == NULL) {
        return RHINO_INV_PARAM;
    }

    _arg->slice = _arg->task->time_slice;

    return RHINO_SUCCESS;
}

kstat_t sys_krhino_utask_create_stub(void *arg)
{
    krhino_utask_create_syscall_arg_t *_arg = arg;

    return krhino_utask_create(_arg->task, _arg->name, _arg->arg,
                               _arg->prio, _arg->ticks, _arg->stack_buf,
                               _arg->stack_size, _arg->kstack_size,
                               _arg->entry, _arg->flag);
}

kstat_t sys_krhino_utask_del_stub(void *arg)
{
    krhino_utask_del_syscall_arg_t *_arg = arg;

    return krhino_utask_del(_arg->task);
}

void *sys_krhino_utask_couple_data_stub(void *arg)
{
    ktask_t *cur;

    cur = krhino_cur_task_get();

    return (void *)(cur->task_ustack_base + cur->ustack_size);
}

kstat_t sys_krhino_task_pri_get_stub(void *arg)
{
    krhino_task_pri_get_syscall_arg_t *_arg = arg;

    if (_arg->task == NULL) {
        return RHINO_INV_PARAM;
    }
    _arg->priority = _arg->task->b_prio;

    return RHINO_SUCCESS;
}

uint32_t sys_krhino_task_id_get_stub(void *arg)
{
    (void) arg;

    return krhino_cur_task_get()->task_id;
}

/* ------------------ sched ------------------ */
kstat_t sys_krhino_sched_policy_set_stub(void *arg)
{
    krhino_sched_policy_set_syscall_arg_t *_arg = arg;

    return krhino_sched_policy_set(_arg->task, _arg->policy, _arg->pri);
}

kstat_t sys_krhino_sched_policy_get_stub(void *arg)
{
    krhino_sched_policy_get_syscall_arg_t *_arg = arg;

    return krhino_sched_policy_get(_arg->task, _arg->policy);
}

int sys_krhino_sched_get_priority_max_stub(void *arg)
{
    krhino_sched_get_priority_syscall_arg_t *_arg = arg;

    return krhino_sched_get_priority_max(_arg->policy);
}

int sys_krhino_sched_get_priority_min_stub(void *arg)
{
    krhino_sched_get_priority_syscall_arg_t *_arg = arg;

    return krhino_sched_get_priority_min(_arg->policy);
}

/* ------------------ process ------------------ */
kstat_t sys_krhino_uprocess_create_stub(void *arg)
{
    krhino_uprocess_create_syscall_arg_t *_arg = arg;

    return krhino_uprocess_create(_arg->app_name, _arg->name, _arg->arg,
                                  _arg->prio, _arg->ticks, _arg->stack_buf,
                                  _arg->stack_size, _arg->kstack_size, _arg->entry,
                                  _arg->ret_entry,  _arg->pid, _arg->autorun);
}

void sys_krhino_uprocess_exit_stub(void *arg)
{
    arg = arg;

    krhino_uprocess_exit();
}

int sys_krhino_process_load_stub(void *arg)
{
    krhino_process_load_syscall_arg_t *_arg = arg;

    return k_proc_load(_arg->name, NULL);
}

int sys_krhino_process_ramload_stub(void *arg)
{
    krhino_process_ramload_syscall_arg_t *_arg = arg;
    void* exit_pm_fun = _arg->exit_pm_fun;
    krhino_set_pm_exit_fun(exit_pm_fun);

    return k_proc_ramload(_arg->ram_addr, NULL);
}

int sys_krhino_process_unload_stub(void *arg)
{
    krhino_process_unload_syscall_arg_t *_arg = arg;

    return krhino_uprocess_kill(_arg->pid);
}

uint32_t sys_krhino_uprocess_id_get_stub(void *arg)
{
    arg = arg;

    return krhino_uprocess_id_get();
}

/* ----------------- proc msg -------------------- */
#if (RHINO_CONFIG_IPC > 0)
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

kstat_t sys_krhino_msg_del_stub(void *arg)
{
    krhino_msg_del_syscall_arg_t *_arg = arg;

    return krhino_msg_del(_arg->msq_id);
}

kstat_t sys_krhino_ipc_create_stream_stub(void *arg)
{
    krhino_ipc_stream_arg_t *_arg = arg;

    return aos_ipc_create_stream(_arg->name, _arg->buf_len, _arg->obj_handle);
}

kstat_t sys_krhino_ipc_create_msg_stub(void *arg)
{
    krhino_ipc_msg_arg_t *_arg = arg;

    return aos_ipc_create_msg(_arg->name, _arg->msg_count, _arg->obj_handle);
}

kstat_t sys_krhino_ipc_create_fifo_stub(void *arg)
{
    krhino_ipc_fifo_arg_t *_arg = arg;

    return aos_ipc_create_fifo(_arg->name, _arg->elem_size,
                               _arg->elem_count, _arg->obj_handle);
}

kstat_t sys_krhino_ipc_send_stub(void *arg)
{
    krhino_ipc_send_arg_t *_arg = arg;

    return aos_ipc_send(_arg->obj_handle, _arg->data,
                        _arg->dlen, _arg->ticks);
}

kstat_t sys_krhino_ipc_sendto_stub(void *arg)
{
    krhino_ipc_send_arg_t *_arg = arg;

    return aos_ipc_sendto(_arg->obj_handle, _arg->data,
                          _arg->dlen, _arg->ticks);
}

kstat_t sys_krhino_ipc_send_owner_stub(void *arg)
{
    krhino_ipc_send_arg_t *_arg = arg;

    return aos_ipc_send_owner(_arg->obj_handle, _arg->data,
                              _arg->dlen, _arg->ticks);
}

kstat_t sys_krhino_ipc_recv_stub(void *arg)
{
    krhino_ipc_recv_arg_t *_arg = arg;

    return aos_ipc_recv(_arg->obj_handle, _arg->data,
                        _arg->dlen, _arg->ticks);
}

tick_t sys_krhino_ipc_cycle_stub(void *arg)
{
    return aos_ipc_cycle();
}

kstat_t sys_krhino_ipc_del_stub(void *arg)
{
    krhino_ipc_del_arg_t *_arg = arg;

    return aos_ipc_del(_arg->obj_handle);
}

kstat_t sys_krhino_ipc_setopt_stub(void *arg)
{
    krhino_ipc_opt_arg_t *_arg = arg;

    return aos_ipc_setopt(_arg->obj_handle, _arg->optyp, _arg->val);
}

kstat_t sys_krhino_ipc_getopt_stub(void *arg)
{
    krhino_ipc_opt_arg_t *_arg = arg;

    return aos_ipc_getopt(_arg->obj_handle, _arg->optyp, _arg->val);
}

kstat_t sys_krhino_ipc_perf_stub(void *arg)
{
    return aos_ipc_perf();
}

#endif /* RHINO_CONFIG_UIPC */

