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

kstat_t sys_krhino_utask_create_stub(void *arg);

kstat_t sys_krhino_utask_del_stub(void *arg);

/* ------------------ process ------------------ */
kstat_t sys_krhino_uprocess_create_stub(void *arg);

void sys_krhino_uprocess_exit_stub(void *arg);

void sys_krhino_uprocess_res_get_stub(void *arg);

/* ------------------- time ------------------ */
void sys_krhino_sys_tick_get_stub(void *arg);

void sys_krhino_sys_time_get_stub(void *arg);

void sys_krhino_ms_to_ticks_stub(void *arg);

void sys_krhino_ticks_to_ms_stub(void *arg);

/* ------------------ mutex ------------------ */
kstat_t sys_krhino_mutex_dyn_create_stub(void *arg);

kstat_t sys_krhino_mutex_dyn_del_stub(void *arg);

kstat_t sys_krhino_mutex_lock_stub(void *arg);

kstat_t sys_krhino_mutex_unlock_stub(void *arg);

/* ------------------ semphore ------------------ */
kstat_t sys_krhino_sem_dyn_create_stub(void *arg);

kstat_t sys_krhino_sem_dyn_del_stub(void *arg);

kstat_t sys_krhino_sem_give_stub(void *arg);

kstat_t sys_krhino_sem_take_stub(void *arg);

/* -------------------- queue --------------------*/
kstat_t sys_krhino_queue_dyn_create_stub(void *arg);

kstat_t sys_krhino_queue_dyn_del_stub(void *arg);

kstat_t sys_krhino_queue_back_send_stub(void *arg);

kstat_t sys_krhino_queue_all_send_stub(void *arg);

kstat_t sys_krhino_queue_recv_stub(void *arg);

kstat_t sys_krhino_queue_flush_stub(void *arg);

/* ------------------ buf queue -------------------*/
kstat_t sys_krhino_buf_queue_dyn_create_stub(void *arg);

kstat_t sys_krhino_fix_buf_queue_dyn_create_stub(void *arg);

kstat_t sys_krhino_buf_queue_dyn_del_stub(void *arg);

kstat_t sys_krhino_buf_queue_send_stub(void *arg);

kstat_t sys_krhino_buf_queue_recv_stub(void *arg);

kstat_t sys_krhino_buf_queue_flush_stub(void *arg);

/* ----------------- event ------------------ */
kstat_t sys_krhino_event_dyn_create_stub(void *arg);

kstat_t sys_krhino_event_dyn_del_stub(void *arg);

kstat_t sys_krhino_event_get_stub(void *arg);

kstat_t sys_krhino_event_set_stub(void *arg);

/* ----------------- proc msg -------------------- */
size_t sys_krhino_msg_get_stub(void *arg);

kstat_t sys_krhino_msg_snd_stub(void *arg);

kstat_t sys_krhino_msg_recv_stub(void *arg);

kstat_t sys_krhino_msg_del_stub(void *arg);

#endif /* RHINO_KSYSCALL_H */

