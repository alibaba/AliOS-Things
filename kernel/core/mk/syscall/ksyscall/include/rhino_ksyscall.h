/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef RHINO_KSYSCALL_H
#define RHINO_KSYSCALL_H

#include "k_api.h"

/* ------------------ task ------------------ */
ktask_t *sys_krhino_cur_task_get_stub(void *arg);

void sys_krhino_task_sleep_stub(void *arg);

kstat_t sys_krhino_task_suspend_stub(void *arg);

kstat_t sys_krhino_task_resume_stub(void *arg);

kstat_t sys_krhino_task_wait_abort_stub(void *arg);

kstat_t sys_krhino_task_yield_stub(void);

kstat_t sys_krhino_task_pri_change_stub(void *arg);

kstat_t sys_krhino_task_info_set_stub(void *arg);

kstat_t sys_krhino_task_info_get_stub(void *arg);

kstat_t sys_krhino_task_name_set_stub(void *arg);

kstat_t sys_krhino_task_name_get_stub(void *arg);

kstat_t sys_krhino_task_ptcb_set_stub(void *arg);

kstat_t sys_krhino_task_ptcb_get_stub(void *arg);

kstat_t sys_krhino_task_time_slice_set_stub(void *arg);

kstat_t sys_krhino_task_time_slice_get_stub(void *arg);

kstat_t sys_krhino_utask_create_stub(void *arg);

kstat_t sys_krhino_utask_del_stub(void *arg);

void *sys_krhino_utask_couple_data_stub(void *arg);

kstat_t sys_krhino_task_pri_get_stub(void *arg);
uint32_t sys_krhino_task_id_get_stub(void *arg);

/* ------------------ sched ------------------ */
kstat_t sys_krhino_sched_policy_set_stub(void *arg);

kstat_t sys_krhino_sched_policy_get_stub(void *arg);

int sys_krhino_sched_get_priority_max_stub(void *arg);

int sys_krhino_sched_get_priority_min_stub(void *arg);

/* ------------------ process ------------------ */
kstat_t sys_krhino_uprocess_create_stub(void *arg);

void sys_krhino_uprocess_exit_stub(void *arg);

int sys_krhino_process_load_stub(void *arg);

int sys_krhino_process_ramload_stub(void *arg);

int sys_krhino_process_unload_stub(void *arg);

void sys_krhino_uprocess_id_get_stub(void *arg);

/* ----------------- proc msg -------------------- */
size_t sys_krhino_msg_get_stub(void *arg);

kstat_t sys_krhino_msg_snd_stub(void *arg);

kstat_t sys_krhino_msg_recv_stub(void *arg);

kstat_t sys_krhino_msg_del_stub(void *arg);

kstat_t sys_krhino_ipc_create_stream_stub(void *arg);

kstat_t sys_krhino_ipc_create_fifo_stub(void *arg);

kstat_t sys_krhino_ipc_create_msg_stub(void *arg);

kstat_t sys_krhino_ipc_send_stub(void *arg);

kstat_t sys_krhino_ipc_sendto_stub(void *arg);

kstat_t sys_krhino_ipc_send_owner_stub(void *arg);

kstat_t sys_krhino_ipc_setopt_stub(void *arg);

kstat_t sys_krhino_ipc_getopt_stub(void *arg);

kstat_t sys_krhino_ipc_recv_stub(void *arg);

tick_t sys_krhino_ipc_cycle_stub(void *arg);

kstat_t sys_krhino_ipc_del_stub(void *arg);

kstat_t sys_krhino_ipc_perf_stub(void *arg);

#endif /* RHINO_KSYSCALL_H */

