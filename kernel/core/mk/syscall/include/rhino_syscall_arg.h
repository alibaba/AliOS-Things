/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef RHINO_SYSCALL_H
#define RHINO_SYSCALL_H

#include <k_api.h>

/* ------------------- utask ------------------- */
typedef struct  {
    tick_t tick;
} krhino_task_sleep_syscall_arg_t;

typedef struct  {
    ktask_t *task;
} krhino_task_suspend_syscall_arg_t;

typedef struct  {
    ktask_t *task;
} krhino_task_resume_syscall_arg_t;

typedef struct  {
    ktask_t *task;
} krhino_task_wait_abort_syscall_arg_t;

typedef struct  {
    ktask_t *task;
    uint8_t  pri;
    uint8_t *old_pri;
} krhino_task_pri_change_syscall_arg_t;

typedef struct {
    ktask_t *task;
    size_t   idx;
    void    *info;
} krhino_task_info_set_syscall_arg_t;

typedef struct {
    ktask_t  *task;
    size_t    idx;
    void    **info;
} krhino_task_info_get_syscall_arg_t;

typedef struct {
    ktask_t  *task;
    void     **ptcb;
} krhino_task_ptcb_get_syscall_arg_t;

typedef struct {
    ktask_t  *task;
    void     *ptcb;
} krhino_task_ptcb_set_syscall_arg_t;

typedef struct {
    ktask_t    *task;
    const char *name;
} krhino_task_name_set_syscall_arg_t;

typedef struct {
    ktask_t     *task;
    const char **name;
} krhino_task_name_get_syscall_arg_t;

typedef struct {
    ktask_t *task;
    uint32_t   slice;
} krhino_task_time_slice_set_syscall_arg_t;

typedef struct {
    ktask_t *task;
    uint32_t slice;
} krhino_task_time_slice_get_syscall_arg_t;

typedef struct {
    ktask_t      **task;
    const name_t  *name;
    void          *arg;
    uint8_t        prio;
    tick_t         ticks;
    cpu_stack_t   *stack_buf;
    size_t         stack_size;
    size_t         kstack_size;
    task_entry_t   entry;
    uint8_t        flag;
} krhino_utask_create_syscall_arg_t;

typedef struct {
    ktask_t *task;
} krhino_utask_del_syscall_arg_t;

typedef struct {
    ktask_t *task;
    uint8_t priority;
} krhino_task_pri_get_syscall_arg_t;

/* ------------------- sched ------------------- */
typedef struct {
    ktask_t *task;
    uint8_t  policy;
    uint8_t  pri;
} krhino_sched_policy_set_syscall_arg_t;

typedef struct {
    ktask_t *task;
    uint8_t *policy;
} krhino_sched_policy_get_syscall_arg_t;

typedef struct {
    int policy;
} krhino_sched_get_priority_syscall_arg_t;

/* ------------------- time  ------------------- */
// typedef struct {
//     sys_time_t ms;
//     tick_t     tick;
// } krhino_ms_to_ticks_syscall_arg_t;

// typedef struct {
//     tick_t     ticks;
//     sys_time_t ms;
// } krhino_ticks_to_ms_syscall_arg_t;

/* ------------------- process ------------------- */
typedef struct {
    const name_t  *app_name;
    const name_t  *name;
    void          *arg;
    uint8_t        prio;
    tick_t         ticks;
    cpu_stack_t   *stack_buf;
    size_t         stack_size;
    size_t         kstack_size;
    task_entry_t   entry;
    void          *ret_entry;
    uint32_t       pid;
    uint8_t        autorun;
} krhino_uprocess_create_syscall_arg_t;

typedef struct {
    const name_t  *name;
} krhino_process_load_syscall_arg_t;

typedef struct {
    const char  *ram_addr;
    void *      exit_pm_fun;
} krhino_process_ramload_syscall_arg_t;

typedef struct {
    int32_t   pid;
} krhino_process_unload_syscall_arg_t;

/* ---------------- proc msg ---------------- */
typedef struct {
    uint32_t key;
    uint32_t flg;
    size_t   size;
} krhino_msg_get_syscall_arg_t;

typedef struct {
    size_t  msg_id;
    void   *msg;
    size_t  msg_sz;
} krhino_msg_snd_syscall_arg_t;

typedef struct {
    size_t  msg_id;
    tick_t  ticks;
    void   *msg;
    size_t *msg_sz;
} krhino_msg_recv_syscall_arg_t;

typedef struct {
    size_t msq_id;
} krhino_msg_del_syscall_arg_t;

typedef struct {
    const char *name;
    size_t buf_len;
    int *obj_handle;
} krhino_ipc_stream_arg_t;

typedef struct {
    const char *name;
    size_t msg_count;
    int *obj_handle;
} krhino_ipc_msg_arg_t;

typedef struct {
    const char *name;
    size_t elem_size;
    size_t elem_count;
    int *obj_handle;
} krhino_ipc_fifo_arg_t;

typedef struct {
    int obj_handle;
    void *data;
    size_t dlen;
    tick_t  ticks;
} krhino_ipc_send_arg_t;

typedef struct {
    int obj_handle;
    void *data;
    size_t *dlen;
    tick_t  ticks;
} krhino_ipc_recv_arg_t;

typedef struct {
    int obj_handle;
} krhino_ipc_del_arg_t;

typedef struct {
    int obj_handle;
    int optyp;
    void *val;
} krhino_ipc_opt_arg_t;

#endif /* RHINO_SYSCALL_H */

