/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef AOS_OBJ_KSYSCALL_H
#define AOS_OBJ_KSYSCALL_H

#include "aos_obj_syscall_arg.h"
/* common */
int sys_aos_handle_delete_stub(aos_obj_delete_syscall_arg_t *arg);
int sys_aos_handle_close_stub(aos_obj_comm_syscall_arg_t *arg);
int sys_aos_handle_valid_stub(aos_obj_comm_syscall_arg_t *arg);
/* aos_sem_xxx stub */
int sys_aos_sem_open_stub(aos_sem_open_syscall_arg_t *arg);
int sys_aos_sem_new_stub(aos_sem_new_syscall_arg_t *arg);
int sys_aos_sem_wait_stub(aos_sem_wait_syscall_arg_t *arg);
int sys_aos_sem_getvalue_stub(aos_sem_getvalue_syscall_arg_t *arg);
int sys_aos_sem_unlink_stub(aos_obj_unlink_syscall_arg_t *arg);
void sys_aos_sem_signal_stub(aos_obj_comm_syscall_arg_t *arg);
void sys_aos_sem_signal_all_stub(aos_obj_comm_syscall_arg_t *arg);
/* aos_event_xxx stub */
int sys_aos_event_open_stub(aos_event_open_syscall_arg_t *arg);
int sys_aos_event_new_stub(aos_event_new_syscall_arg_t *arg);
int sys_aos_event_get_stub(aos_event_get_syscall_arg_t *arg);
int sys_aos_event_set_stub(aos_event_set_syscall_arg_t *arg);
int sys_aos_event_unlink_stub(aos_obj_unlink_syscall_arg_t *arg);
/* aos_mutex_xxx stub */
int sys_aos_mutex_new_stub(aos_mutex_new_syscall_arg_t *arg);
int sys_aos_mutex_lock_stub(aos_mutex_wait_syscall_arg_t *arg);
int sys_aos_mutex_unlock_stub(aos_mutex_wait_syscall_arg_t *arg);
/* aos_queue_xxx stub */
int sys_aos_queue_open_stub(aos_queue_open_syscall_arg_t *arg);
int sys_aos_queue_new_stub(aos_queue_new_syscall_arg_t *arg);
int sys_aos_queue_send_stub(aos_queue_send_syscall_arg_t *arg);
int sys_aos_queue_recv_stub(aos_queue_recv_syscall_arg_t *arg);
int sys_aos_queue_unlink_stub(aos_obj_unlink_syscall_arg_t *arg);
int sys_aos_queue_get_count_stub(aos_obj_comm_syscall_arg_t *arg);
/* aos_timer_xxx stub */
int sys_aos_timer_new_ext_stub(aos_timer_new_syscall_arg_t *arg);
int sys_aos_timer_start_stub(aos_obj_comm_syscall_arg_t *arg);
int sys_aos_timer_stop_stub(aos_obj_comm_syscall_arg_t *arg);
int sys_aos_timer_change_stub(aos_timer_change_syscall_arg_t *arg);
int sys_aos_timer_change_once_stub(aos_timer_change_syscall_arg_t *arg);
/* aos futex stub */
int sys_aos_futex_stub(void *arg);
#endif /* AOS_OBJ_KSYSCALL_H */

