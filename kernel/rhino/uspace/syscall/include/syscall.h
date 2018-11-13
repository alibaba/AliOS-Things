/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef SYSCALL_H
#define SYSCALL_H

#include <hal/hal.h>
#include <k_api.h>
#include <syscall_no.h>

/* ------------------- task  ------------------- */
typedef struct  {
    tick_t tick;
} krhino_task_sleep_syscall_arg_t;

typedef struct {
    ktask_t *task;
} krhino_task_del_syscall_arg_t;

typedef struct {
    ktask_t      *task;
    const name_t *name;
    void         *arg;
    uint8_t       prio;
    tick_t        ticks;
    cpu_stack_t  *stack_buf;
    size_t        stack_size;
    size_t        kstack_size;
    task_entry_t  entry;
    uint8_t       autorun;
} krhino_utask_create_syscall_arg_t;

typedef struct {
    ktask_t      *task;
    const name_t *name;
    void         *arg;
    uint8_t       prio;
    tick_t        ticks;
    cpu_stack_t  *stack_buf;
    size_t        stack_size;
    size_t        kstack_size;
    task_entry_t  entry;
    uint32_t      pid;
    uint8_t       autorun;
} krhino_uprocess_create_syscall_arg_t;

/* ------------------- time  ------------------- */
typedef struct {
    sys_time_t ms;
} krhino_ms_to_ticks_syscall_arg_t;

typedef struct {
    tick_t ticks;
} krhino_ticks_to_ms_syscall_arg_t;

/* ----------------- mutex ------------------- */
typedef struct {
    kmutex_t     *mutex;
    const name_t *name;
    uint8_t       mm_alloc_flag;
} krhino_mutex_create_syscall_arg_t;

typedef struct {
    kmutex_t *mutex;
} krhino_mutex_del_syscall_arg_t;

typedef struct {
    kmutex_t *mutex;
    tick_t    ticks;
} krhino_mutex_lock_syscall_arg_t;

typedef struct {
    kmutex_t *mutex;
} krhino_mutex_unlock_syscall_arg_t;

/* ---------------- semphore ---------------- */
typedef struct {
    ksem_t       *sem;
    const name_t *name;
    sem_count_t   count;
} krhino_sem_create_syscall_arg_t;

typedef struct {
    ksem_t *sem;
} krhino_sem_del_syscall_arg_t;

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

typedef struct {
    ksem_t      *sem;
    sem_count_t *count;
} krhino_sem_count_get_syscall_arg_t;

typedef struct {
    ksem_t      *sem;
    sem_count_t *count;
} krhino_sem_count_set_syscall_arg_t;

/* ---------------- buf queue -----------------*/
typedef struct {
    kbuf_queue_t *queue;
    const char   *name;
    void         *buf;
    size_t        size;
    size_t        max_msg;
} krhino_buf_queue_create_syscall_arg_t;

typedef struct {
    kbuf_queue_t *queue;
    const char   *name;
    void         *buf;
    size_t        msg_size;
    size_t        msg_num;
} krhino_fix_buf_queue_create_syscall_arg_t;

typedef struct {
    kbuf_queue_t *queue;
} krhino_buf_queue_del_syscall_arg_t;

typedef struct {
    kbuf_queue_t **queue;
    const char    *name;
    size_t         size;
    size_t         max_msg;
} krhino_buf_queue_dyn_create_syscall_arg_t;

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
    tick_t        ticks;
    void         *msg;
    size_t       *size;
} krhino_buf_queue_recv_syscall_arg_t;

typedef struct {
    kbuf_queue_t *queue;
} krhino_buf_queue_flush_syscall_arg_t;

typedef struct {
    kbuf_queue_t      *queue;
    kbuf_queue_info_t *info;
} krhino_buf_queue_info_get_syscall_arg_t;

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

/* ------------------ hal uart ----------------- */
typedef struct {
    uart_dev_t *uart;
} hal_uart_init_syscall_arg_t;

typedef struct {
    uart_dev_t *uart;
    const void *data;
    uint32_t    size;
    uint32_t    timeout;
} hal_uart_send_syscall_arg_t;

typedef struct {
    uart_dev_t *uart;
    void       *data;
    uint32_t    expect_size;
    uint32_t    timeout;
} hal_uart_recv_syscall_arg_t;

typedef struct {
    uart_dev_t *uart;
    void       *data;
    uint32_t    expect_size;
    uint32_t   *recv_size;
    uint32_t    timeout;
} hal_uart_recv_II_syscall_arg_t;

typedef struct {
    uart_dev_t *uart;
} hal_uart_finalize_syscall_arg_t;

/* ------------------- vfs ------------------- */
typedef struct {
    int   fd;
    off_t offset;
    int   whence;
} aos_lseek_syscall_arg_t;

typedef struct {
    int fd;
} aos_close_syscall_arg_t;

typedef struct {
    int     fd;
    void   *buf;
    size_t  nbytes;
} aos_read_syscall_arg_t;

typedef struct {
    int         fd;
    const void *buf;
    size_t      nbytes;
} aos_write_syscall_arg_t;

#endif /* SYSCALL_H */

