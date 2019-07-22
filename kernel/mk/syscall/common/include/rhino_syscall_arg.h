/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef SYSCALL_H
#define SYSCALL_H

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
    uint8_t        autorun;
} krhino_utask_create_syscall_arg_t;

typedef struct {
    ktask_t *task;
} krhino_utask_del_syscall_arg_t;

/* ------------------- process ------------------- */
typedef struct {
    const name_t  *app_name;
    ktask_t      **task;
    const name_t  *name;
    void          *arg;
    uint8_t        prio;
    tick_t         ticks;
    cpu_stack_t   *stack_buf;
    size_t         stack_size;
    size_t         kstack_size;
    task_entry_t   entry;
    uint32_t       pid;
    uint8_t        autorun;
} krhino_uprocess_create_syscall_arg_t;

typedef struct {
    int32_t   id;
    void    **res;
} krhino_uprocess_res_get_syscall_arg_t;

/* ------------------- time  ------------------- */
typedef struct {
    sys_time_t ms;
    tick_t     tick;
} krhino_ms_to_ticks_syscall_arg_t;

typedef struct {
    tick_t     ticks;
    sys_time_t ms;
} krhino_ticks_to_ms_syscall_arg_t;

/* ----------------- mutex ------------------- */
typedef struct {
    kmutex_t     **mutex;
    const name_t  *name;
} krhino_mutex_dyn_create_syscall_arg_t;

typedef struct {
    kmutex_t *mutex;
} krhino_mutex_dyn_del_syscall_arg_t;

typedef struct {
    kmutex_t *mutex;
    tick_t    ticks;
} krhino_mutex_lock_syscall_arg_t;

typedef struct {
    kmutex_t *mutex;
} krhino_mutex_unlock_syscall_arg_t;

/* ---------------- semphore ---------------- */
typedef struct {
    ksem_t       **sem;
    const name_t  *name;
    sem_count_t    count;
} krhino_sem_dyn_create_syscall_arg_t;

typedef struct {
    ksem_t *sem;
} krhino_sem_dyn_del_syscall_arg_t;

typedef struct {
    ksem_t *sem;
    tick_t  ticks;
} krhino_sem_take_syscall_arg_t;

typedef struct {
    ksem_t *sem;
} krhino_sem_give_syscall_arg_t;

typedef struct {
    ksem_t *sem;
} krhino_sem_give_all_syscall_arg_t;

/* -------------------- queue ---------------------*/
typedef struct {
    kqueue_t     **queue;
    const name_t  *name;
    size_t         msg_num;
} krhino_queue_dyn_create_syscall_arg_t;

typedef struct {
    kqueue_t *queue;
} krhino_queue_dyn_del_syscall_arg_t;

typedef struct {
    kqueue_t *queue;
    void *msg;
} krhino_queue_back_send_syscall_arg_t;

typedef struct {
    kqueue_t *queue;
    void *msg;
} krhino_queue_all_send_syscall_arg_t;

typedef struct {
    kqueue_t *queue;
    void    **msg;
    tick_t    ticks;
} krhino_queue_recv_syscall_arg_t;

typedef struct {
    kqueue_t *queue;
} krhino_queue_flush_syscall_arg_t;

/* ---------------- buf queue -----------------*/
typedef struct {
    kbuf_queue_t **queue;
    const char    *name;
    size_t         size;
    size_t         max_msg;
} krhino_buf_queue_dyn_create_syscall_arg_t;

typedef struct {
    kbuf_queue_t **queue;
    const char    *name;
    size_t         msg_size;
    size_t         msg_num;
} krhino_fix_buf_queue_dyn_create_syscall_arg_t;

typedef struct {
    kbuf_queue_t *queue;
} krhino_buf_queue_dyn_del_syscall_arg_t;

typedef struct {
    kbuf_queue_t *queue;
    void         *msg;
    size_t        size;
} krhino_buf_queue_send_syscall_arg_t;

typedef struct {
    kbuf_queue_t *queue;
    void         *msg;
    tick_t        ticks;
    size_t       *size;
} krhino_buf_queue_recv_syscall_arg_t;

typedef struct {
    kbuf_queue_t *queue;
} krhino_buf_queue_flush_syscall_arg_t;

/* ----------------- event ------------------ */
typedef struct {
    kevent_t     **event;
    const name_t  *name;
    uint32_t       flags;
} krhino_event_dyn_create_syscall_arg_t;

typedef struct {
    kevent_t *event;
} krhino_event_dyn_del_syscall_arg_t;

typedef struct {
    kevent_t *event;
    uint32_t  flags;
    uint8_t   opt;
    uint32_t *actl_flags;
    tick_t    ticks;
} krhino_event_get_syscall_arg_t;

typedef struct {
    kevent_t *event;
    uint32_t  flags;
    uint8_t   opt;
} krhino_event_set_syscall_arg_t;

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

#endif /* SYSCALL_H */

