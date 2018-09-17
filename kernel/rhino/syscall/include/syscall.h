/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef SYSCALL_H
#define SYSCALL_H

/* ------------------- task --------------------*/
#define K_TASK_BASE     (0)

#define SYS_KRHINO_CUR_TASK_GET (K_TASK_BASE + 0)

#define SYS_KRHINO_TASK_SLEEP (K_TASK_BASE + 1)

#define SYS_KRHINO_TASK_DEL (K_TASK_BASE + 2)

#define SYS_KRHINO_UTASK_CREATE (K_TASK_BASE + 3)

#define K_TASK_END      (SYS_KRHINO_UTASK_CREATE)


/* ------------------- time --------------------*/
#define K_TIME_BASE     (K_TASK_END + 1)

#define SYS_KRHINO_SYS_TIME_GET (K_TIME_BASE + 0)

#define SYS_KRHINO_SYS_TICK_GET (K_TIME_BASE + 1)

#define SYS_KRHINO_MS_TO_TICKS (K_TIME_BASE + 2)

#define SYS_KRHINO_TICKS_TO_MS (K_TIME_BASE + 3)

#define K_TIME_END      (SYS_KRHINO_TICKS_TO_MS)


/* ------------------- mutex --------------------*/
#define K_MUTEX_BASE    (K_TIME_END + 1)

#define SYS_KRHINO_MUTEX_CREATE (K_MUTEX_BASE + 0)

#define SYS_KRHINO_MUTEX_DEL (K_MUTEX_BASE + 1)

#define SYS_KRHINO_MUTEX_LOCK (K_MUTEX_BASE + 2)

#define SYS_KRHINO_MUTEX_UNLOCK (K_MUTEX_BASE + 3)

#define K_MUTEXT_END    (SYS_KRHINO_MUTEX_UNLOCK)


/* ------------------ semphore -------------------*/
#define K_SEM_BASE      (K_MUTEXT_END + 1)

#define SYS_KRHINO_SEM_CREATE (K_SEM_BASE + 0)

#define SYS_KRHINO_SEM_DEL (K_SEM_BASE + 1)

#define SYS_KRHINO_SEM_TAKE (K_SEM_BASE + 2)

#define SYS_KRHINO_SEM_GIVE (K_SEM_BASE + 3)

#define K_SEM_END   (SYS_KRHINO_SEM_GIVE)


/* ------------------ buf queue -------------------*/
#define K_BUF_QUEUE_BASE        (K_SEM_END + 1)

#define SYS_KRHINO_BUF_QUEUE_CREATE (K_BUF_QUEUE_BASE + 0)

#define SYS_KRHINO_BUF_QUEUE_DEL (K_BUF_QUEUE_BASE + 1)

#define SYS_KRHINO_BUF_QUEUE_DYN_CREATE (K_BUF_QUEUE_BASE + 2)

#define SYS_KRHINO_BUF_QUEUE_DYN_DEL (K_BUF_QUEUE_BASE + 3)

#define SYS_KRHINO_BUF_QUEUE_SEND (K_BUF_QUEUE_BASE + 4)

#define SYS_KRHINO_BUF_QUEUE_RECV (K_BUF_QUEUE_BASE + 5)

#define SYS_KRHINO_BUF_QUEUE_FLUSH (K_BUF_QUEUE_BASE + 6)

#define SYS_KRHINO_BUF_QUEUE_INFO_GET (K_BUF_QUEUE_BASE + 7)

#define K_BUF_QUEUE_END     (SYS_KRHINO_BUF_QUEUE_INFO_GET)


/* ------------------- mm --------------------*/
#define K_MM_BASE      (K_BUF_QUEUE_END + 1)

#define SYS_KRHINO_MM_ALLOC (K_MM_BASE + 0)

#define SYS_KRHINO_MM_FREE (K_MM_BASE + 1)

#define SYS_KRHINO_MM_REALLOC (K_MM_BASE + 2)

#define K_MM_END       (SYS_KRHINO_MM_REALLOC)


/* ----------------- hal uart -----------------*/
#define HAL_UART_BASE   (K_MM_END + 1)

#define SYS_HAL_UART_INIT (HAL_UART_BASE +0)

#define SYS_HAL_UART_SEND (HAL_UART_BASE + 1)

#define SYS_HAL_UART_RECV (HAL_UART_BASE + 2)

#define SYS_HAL_UART_RECV_II (HAL_UART_BASE + 3)

#define SYS_HAL_UART_FINALISE (HAL_UART_BASE + 4)

#define HAL_UART_END    (SYS_HAL_UART_FINALISE)


/* ------------------- vfs -------------------*/
#define VFS_BASE    (HAL_UART_END + 1)

#define SYS_AOS_LSEEK (VFS_BASE + 0)

#define SYS_AOS_CLOSE (VFS_BASE + 1)

#define SYS_AOS_READ (VFS_BASE + 2)

#define SYS_AOS_WRITE (VFS_BASE + 3)

#define VFS_END     (SYS_AOS_WRITE)


#include <k_api.h>
#include <hal/hal.h>

/* ------------------- task  ------------------- */
typedef struct  {
    tick_t tick;
} krhino_task_sleep_syscall_arg_t;

typedef struct {
    ktask_t *task;
} krhino_task_del_syscall_arg_t;

typedef struct {
    ktask_t *task;
    const name_t *name;
    void *arg;
    uint8_t prio;
    tick_t ticks;
    cpu_stack_t *stack_buf;
    size_t stack_size;
    size_t kstack_size;
    task_entry_t entry;
    uint8_t autorun;
} krhino_utask_create_syscall_arg_t;

typedef struct {
    uart_dev_t *uart;
    const void *data;
    uint32_t size;
    uint32_t timeout;
} hal_uart_send_syscall_arg_t;

/* ------------------- time  ------------------- */
typedef struct {
    sys_time_t ms;
} krhino_ms_to_ticks_syscall_arg_t;

typedef struct {
    tick_t ticks;
} krhino_ticks_to_ms_syscall_arg_t;

/* ----------------- mutex ------------------- */
typedef struct {
    kmutex_t *mutex;
    const name_t *name;
    uint8_t mm_alloc_flag;
} krhino_mutex_create_syscall_arg_t;


typedef struct {
    kmutex_t *mutex;
} krhino_mutex_del_syscall_arg_t;

typedef struct {
    kmutex_t *mutex;
    tick_t ticks;
} krhino_mutex_lock_syscall_arg_t;

typedef struct {
    kmutex_t *mutex;
} krhino_mutex_unlock_syscall_arg_t;

/* ---------------- semphore ---------------- */
typedef struct {
    ksem_t *sem;
    const name_t *name;
    sem_count_t count;
} krhino_sem_create_syscall_arg_t;

typedef struct {
    ksem_t *sem;
} krhino_sem_del_syscall_arg_t;


typedef struct {
    ksem_t *sem;
    tick_t ticks;
} krhino_sem_take_syscall_arg_t;

typedef struct {
    ksem_t *sem;
} krhino_sem_give_syscall_arg_t;

typedef struct {
    ksem_t *sem;
} krhino_sem_give_all_syscall_arg_t;

typedef struct {
    ksem_t *sem;
    sem_count_t *count;
} krhino_sem_count_get_syscall_arg_t;

typedef struct {
    ksem_t *sem;
    sem_count_t *count;
} krhino_sem_count_set_syscall_arg_t;

/* ------------------ buf queue -------------------*/

typedef struct {
    kbuf_queue_t *queue;
    const char *name;
    void *buf;
    size_t size;
    size_t max_msg;
} krhino_buf_queue_create_syscall_arg_t;

typedef struct {
    kbuf_queue_t *queue;
} krhino_buf_queue_del_syscall_arg_t;

typedef struct {
    kbuf_queue_t **queue;
    const char *name;
    size_t size;
    size_t max_msg;
} krhino_buf_queue_dyn_create_syscall_arg_t;

typedef struct {
    kbuf_queue_t *queue;
} krhino_buf_queue_dyn_del_syscall_arg_t;

typedef struct {
    kbuf_queue_t *queue;
    void *msg;
    size_t size;
} krhino_buf_queue_send_syscall_arg_t;

typedef struct {
    kbuf_queue_t *queue;
    tick_t ticks;
    void *msg;
    size_t *size;
} krhino_buf_queue_recv_syscall_arg_t;

typedef struct {
    kbuf_queue_t *queue;
} krhino_buf_queue_flush_syscall_arg_t;

typedef struct {
    kbuf_queue_t *queue;
    kbuf_queue_info_t *info;
} krhino_buf_queue_info_get_syscall_arg_t;


/* ------------------- mm ------------------- */
typedef struct {
    size_t size;
} krhino_mm_alloc_syscall_arg_t;

typedef struct {
    void *ptr;
} krhino_mm_free_syscall_arg_t;

typedef struct {
    void *oldmem;
    size_t newsize;
} krhino_mm_realloc_syscall_arg_t;


/* ------------------- vfs ------------------- */
typedef struct {
    int fd;
    off_t offset;
    int whence;
} aos_lseek_syscall_arg_t;

typedef struct {
    int fd;
} aos_close_syscall_arg_t;

typedef struct {
    int fd;
    void *buf;
    size_t nbytes;
} aos_read_syscall_arg_t;

typedef struct {
    int fd;
    const void *buf;
    size_t nbytes;
} aos_write_syscall_arg_t;

#endif // SYSCALL_H

