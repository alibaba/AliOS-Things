/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef SYSCALL_H
#define SYSCALL_H

/* ------------------- task --------------------*/
#define K_TASK_BASE     (0)

#define SYS_KRHINO_CUR_TASK_GET (K_TASK_BASE + 0)

#define SYS_KRHINO_TASK_INFO_SET (K_TASK_BASE + 1)

#define SYS_KRHINO_TASK_INFO_GET (K_TASK_BASE + 2)

#define SYS_KRHINO_TASK_SLEEP (K_TASK_BASE + 3)

#define SYS_KRHINO_TASK_DYN_CREATE (K_TASK_BASE + 4)

#define SYS_KRHINO_TASK_DEL (K_TASK_BASE + 5)

#define SYS_KRHINO_UTASK_CREATE (K_TASK_BASE + 6)

#define K_TASK_END      (SYS_KRHINO_UTASK_CREATE)


/* ------------------- time --------------------*/
#define K_TIME_BASE     (K_TASK_END + 1)

#define SYS_KRHINO_SYS_TIME_GET (K_TIME_BASE + 0)

#define SYS_KRHINO_SYS_TICK_GET (K_TIME_BASE + 1)

#define SYS_KRHINO_MS_TO_TICKS (K_TIME_BASE + 2)

#define SYS_KRHINO_TICKS_TO_MS (K_TIME_BASE + 3)

#define K_TIME_END      (SYS_KRHINO_TICKS_TO_MS)


/* ------------------- timer --------------------*/
#define K_TIMER_BASE    (K_TIME_END + 1)

#define SYS_KRHINO_TIMER_DYN_CREATE (K_TIMER_BASE + 4)

#define SYS_KRHINO_TIMER_DYN_DEL (K_TIMER_BASE + 5)

#define SYS_KRHINO_TIMER_START (K_TIMER_BASE + 6)

#define SYS_KRHINO_TIMER_STOP (K_TIMER_BASE + 7)

#define SYS_KRHINO_TIMER_CHANGE (K_TIMER_BASE + 8)

#define K_TIMER_END     (SYS_KRHINO_TIMER_CHANGE)


/* ------------------- mutex --------------------*/
#define K_MUTEX_BASE    (K_TIMER_END + 1)

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


/* ---------------- task semphore ----------------*/
#define K_TASK_SEM_BASE     (K_SEM_END + 1)

#define SYS_KRHINO_TASK_SEM_CREATE (K_TASK_SEM_BASE + 0)

#define SYS_KRINO_TASK_SEM_DEL (K_TASK_SEM_BASE + 1)

#define SYS_KRHINO_TASK_SEM_GIVE (K_TASK_SEM_BASE + 2)

#define SYS_KRHINO_TASK_SEM_TAKE (K_TASK_SEM_BASE + 3)

#define SYS_KRHINO_TASK_SEM_COUNT_SET (K_TASK_SEM_BASE + 4)

#define SYS_KRHINO_TASK_SEM_COUNT_GET (K_TASK_SEM_BASE + 5)

#define K_TASK_SEM_END      (SYS_KRHINO_TASK_SEM_COUNT_GET)


/* ------------------ queue -------------------*/
#define K_QUEUE_BASE    (K_TASK_SEM_END + 1)

#define SYS_RHINO_QUEUE_CREATE (K_QUEUE_BASE + 0)

#define SYS_RHINO_QUEUE_DYN_CREATE (K_QUEUE_BASE + 1)

#define SYS_RHINO_QUEUE_DEL (K_QUEUE_BASE + 2)

#define SYS_RHINO_QUEUE_DYN_DEL (K_QUEUE_BASE + 3)

#define SYS_RHINO_QUEUE_BACK_SEND (K_QUEUE_BASE + 4)

#define SYS_RHINO_QUEUE_ALL_SEND (K_QUEUE_BASE + 5)

#define SYS_RHINO_QUEUE_RECV (K_QUEUE_BASE + 6)

#define SYS_RHINO_QUEUE_IS_FULL (K_QUEUE_BASE + 7)

#define SYS_RHINO_QUEUE_FLUSH (K_QUEUE_BASE + 8)

#define SYS_RHINO_QUEUE_INFO_GET (K_QUEUE_BASE + 9)

#define K_QUEUE_END     (SYS_RHINO_QUEUE_INFO_GET)


/* ---------------- workqueue -----------------*/
#define K_WORKQUEUE_BASE  (K_QUEUE_END + 1)

#define SYS_KRHINO_WORKQUEUE_CREATE (K_WORKQUEUE_BASE + 0)

#define SYS_KRHINO_WORK_INIT (K_WORKQUEUE_BASE + 1)

#define SYS_KRHINO_WORK_RUN (K_WORKQUEUE_BASE + 2)

#define SYS_KRHINO_WORK_SCHED (K_WORKQUEUE_BASE + 3)

#define SYS_KRHINO_WORK_CANCEL (K_WORKQUEUE_BASE + 4)

#define K_WORKQUEUE_END  (SYS_KRHINO_WORK_CANCEL)


/* ------------------- event --------------------*/
#define K_EVENT_BASE (K_WORKQUEUE_END + 1)

#define SYS_KRHINO_EVENT_CREATE (K_EVENT_BASE + 0)

#define SYS_RHINO_EVENT_DYN_CREATE (K_EVENT_BASE + 1)

#define SYS_KRHINO_EVENT_DEL (K_EVENT_BASE + 2)

#define SYS_KRHINO_EVENT_DYN_DEL (K_EVENT_BASE + 3)

#define SYS_KRHINO_EVENT_GET (K_EVENT_BASE + 4)

#define SYS_KRHINO_EVENT_SET (K_EVENT_BASE + 5)

#define K_EVENT_END     (SYS_KRHINO_EVENT_SET)


/* ----------------- buf_queue ----------------*/
#define K_BUF_QUEUE_BASE        (K_EVENT_END + 1)

#define SYS_KRHINO_BUF_QUEUE_SEND (K_BUF_QUEUE_BASE + 0)

#define SYS_KRHINO_BUF_QUEUE_RECV (K_BUF_QUEUE_BASE + 1)

#define SYS_KRHINO_BUF_QUEUE_CREATE (K_BUF_QUEUE_BASE + 2)

#define SYS_KRHINO_BUF_QUEUE_DEL (K_BUF_QUEUE_BASE + 3)

#define K_BUF_QUEUE_END     (SYS_KRHINO_BUF_QUEUE_DEL)


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


/* ----------------- hal i2c -----------------*/
#define HAL_I2C_BASE    (HAL_UART_END + 1)

#define SYS_HAL_I2C_INIT (HAL_I2C_BASE + 0)

#define SYS_HAL_I2C_MASTER_SEND (HAL_I2C_BASE + 1)

#define SYS_HAL_I2C_MASTER_RECV (HAL_I2C_BASE + 2)

#define SYS_HAL_I2C_SLAVE_SEND (HAL_I2C_BASE + 3)

#define SYS_HAL_I2C_SLAVE_RECV (HAL_I2C_BASE + 4)

#define SYS_HAL_I2C_MEM_WRITE (HAL_I2C_BASE + 5)

#define SYS_HAL_I2C_MEM_READ (HAL_I2C_BASE + 6)

#define SYS_HAL_I2C_FINALIZE (HAL_I2C_BASE + 7)

#define HAL_I2C_END     (SYS_HAL_I2C_FINALIZE)


/* ----------------- hal spi -----------------*/
#define HAL_SPI_BASE    (HAL_I2C_END + 1)

#define SYS_HAL_SPI_INIT (HAL_SPI_BASE + 0)

#define SYS_HAL_SPI_SEND (HAL_SPI_BASE + 1)

#define SYS_HAL_SPI_RECV (HAL_SPI_BASE + 2)

#define SYS_HAL_SPI_SEDN_RECV (HAL_SPI_BASE + 3)

#define SYS_HALE_SPI_FINALIZE (HAL_SPI_BASE + 4)

#define HAL_SPI_END     (SYS_HALE_SPI_FINALIZE)

/* ------------------- vfs -------------------*/
#define VFS_BASE    (HAL_SPI_END + 1)

#define SYS_AOS_LSEEK (VFS_BASE + 0)

#define SYS_AOS_CLOSE (VFS_BASE + 1)

#define SYS_AOS_READ (VFS_BASE + 2)

#define SYS_AOS_WRITE (VFS_BASE + 3)

#define VFS_END     (SYS_AOS_WRITE)


#include <k_api.h>
#include <hal/hal.h>

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

