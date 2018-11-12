/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <arch_syscall.h>
#include <syscall.h>


/* ------------------- task ------------------- */

ktask_t *krhino_cur_task_get(void)
{
    return SYSCALL(SYS_KRHINO_CUR_TASK_GET, NULL);
}

kstat_t krhino_task_sleep(tick_t tick)
{
    volatile krhino_task_sleep_syscall_arg_t arg;
    arg.tick = tick;

    return SYSCALL(SYS_KRHINO_TASK_SLEEP, &arg);
}

kstat_t krhino_task_del(ktask_t *task)
{
    volatile krhino_task_del_syscall_arg_t arg;
    arg.task = task;

    return SYSCALL(SYS_KRHINO_TASK_DEL, &arg);
}

kstat_t krhino_utask_create(ktask_t *task, const name_t *name,
                            void *arg, uint8_t prio, tick_t ticks,
                            cpu_stack_t *stack_buf,
                            size_t stack_size, size_t kstack_size,
                            task_entry_t entry, uint8_t autorun)
{
    volatile krhino_utask_create_syscall_arg_t syscall_arg;

    syscall_arg.task = task;
    syscall_arg.name = name;
    syscall_arg.arg = arg;
    syscall_arg.prio = prio;
    syscall_arg.ticks = ticks;
    syscall_arg.stack_buf = stack_buf;
    syscall_arg.stack_size = stack_size;
    syscall_arg.kstack_size = kstack_size;
    syscall_arg.entry = entry;
    syscall_arg.autorun = autorun;

    return SYSCALL(SYS_KRHINO_UTASK_CREATE, &syscall_arg);
}

kstat_t krhino_uprocess_create(ktask_t *task, const name_t *name,
                            void *arg, uint8_t prio, tick_t ticks,
                            cpu_stack_t *stack_buf,
                            size_t stack_size, size_t kstack_size,
                            task_entry_t entry, uint32_t pid,
                            uint8_t autorun)
{
    volatile krhino_uprocess_create_syscall_arg_t syscall_arg;

    syscall_arg.task = task;
    syscall_arg.name = name;
    syscall_arg.arg = arg;
    syscall_arg.prio = prio;
    syscall_arg.ticks = ticks;
    syscall_arg.stack_buf = stack_buf;
    syscall_arg.stack_size = stack_size;
    syscall_arg.kstack_size = kstack_size;
    syscall_arg.entry = entry;
    syscall_arg.pid = pid;
    syscall_arg.autorun = autorun;

    return SYSCALL(SYS_KRHINO_UPROCESS_CREATE, &syscall_arg);
}

/* ------------------ time ------------------ */

sys_time_t krhino_sys_tick_get(void)
{
    return SYSCALL(SYS_KRHINO_SYS_TICK_GET, NULL);
}

sys_time_t krhino_sys_time_get(void)
{
    return SYSCALL(SYS_KRHINO_SYS_TIME_GET, NULL);
}

tick_t krhino_ms_to_ticks(sys_time_t ms)
{
    volatile krhino_ms_to_ticks_syscall_arg_t arg;
    arg.ms = ms;

    return SYSCALL(SYS_KRHINO_MS_TO_TICKS, &arg);
}

sys_time_t krhino_ticks_to_ms(tick_t ticks)
{
    volatile krhino_ticks_to_ms_syscall_arg_t arg;

    arg.ticks = ticks;
    return SYSCALL(SYS_KRHINO_TICKS_TO_MS, &arg);

}


/* ------------------ mutex ------------------ */

kstat_t krhino_mutex_create(kmutex_t *mutex, const name_t *name)
{
    volatile krhino_mutex_create_syscall_arg_t arg;

    arg.mutex = mutex;
    arg.name = name;

    return SYSCALL(SYS_KRHINO_MUTEX_CREATE, &arg);
}

kstat_t krhino_mutex_del(kmutex_t *mutex)
{
    volatile krhino_mutex_del_syscall_arg_t arg;

    arg.mutex = mutex;

    return SYSCALL(SYS_KRHINO_MUTEX_DEL, &arg);
}

kstat_t krhino_mutex_lock(kmutex_t *mutex, tick_t ticks)
{
    volatile krhino_mutex_lock_syscall_arg_t arg;

    arg.mutex = mutex;
    arg.ticks = ticks;

    return SYSCALL(SYS_KRHINO_MUTEX_LOCK, &arg);
}

kstat_t krhino_mutex_unlock(kmutex_t *mutex)
{
    volatile krhino_mutex_unlock_syscall_arg_t arg;

    arg.mutex = mutex;

    return SYSCALL(SYS_KRHINO_MUTEX_UNLOCK, &arg);
}

/* ------------------ semphore ------------------ */

kstat_t krhino_sem_create(ksem_t *sem,
                          const name_t *name,
                          sem_count_t count)
{
    volatile krhino_sem_create_syscall_arg_t arg;

    arg.sem = sem;
    arg.name = name;
    arg.count = count;

    return SYSCALL(SYS_KRHINO_SEM_CREATE, &arg);
}

kstat_t krhino_sem_del(ksem_t *sem)
{
    volatile krhino_sem_del_syscall_arg_t arg;

    arg.sem = sem;

    return SYSCALL(SYS_KRHINO_SEM_DEL, &arg);
}


kstat_t krhino_sem_give(ksem_t *sem)
{
    volatile krhino_sem_give_syscall_arg_t arg;

    arg.sem = sem;

    return SYSCALL(SYS_KRHINO_SEM_GIVE, &arg);
}


kstat_t krhino_sem_take(ksem_t *sem, tick_t ticks)
{
    volatile krhino_sem_take_syscall_arg_t arg;

    arg.sem = sem;
    arg.ticks = ticks;

    return SYSCALL(SYS_KRHINO_SEM_TAKE, &arg);
}

/* ------------------ buf queue -------------------*/

kstat_t krhino_buf_queue_create(kbuf_queue_t *queue,
                                const name_t *name,
                                void *buf, size_t size,
                                size_t max_msg)
{
    volatile krhino_buf_queue_create_syscall_arg_t arg;

    arg.queue = queue;
    arg.name = name;
    arg.buf = buf;
    arg.size = size;
    arg.max_msg = max_msg;

    return SYSCALL(SYS_KRHINO_BUF_QUEUE_CREATE, &arg);
}

kstat_t krhino_fix_buf_queue_create(kbuf_queue_t *queue,
                                    const name_t *name,
                                    void *buf,
                                    size_t msg_size,
                                    size_t msg_num)
{
    volatile krhino_fix_buf_queue_create_syscall_arg_t arg;

    arg.queue = queue;
    arg.name = name;
    arg.buf = buf;
    arg.msg_size = msg_size;
    arg.msg_num = msg_num;

    return SYSCALL(SYS_KRHINO_FIX_BUF_QUEUE_CREATE, &arg);

}

kstat_t krhino_buf_queue_del(kbuf_queue_t *queue)
{
    volatile krhino_buf_queue_del_syscall_arg_t arg;

    arg.queue = queue;

    return SYSCALL(SYS_KRHINO_BUF_QUEUE_DEL, &arg);
}

kstat_t krhino_buf_queue_dyn_create(kbuf_queue_t **queue,
                                    const name_t *name,
                                    size_t size,
                                    size_t max_msg)
{
    volatile krhino_buf_queue_dyn_create_syscall_arg_t arg;

    arg.queue = queue;
    arg.name = name;
    arg.size = size;
    arg.max_msg = max_msg;

    return SYSCALL(SYS_KRHINO_BUF_QUEUE_DYN_CREATE, &arg);
}

kstat_t krhino_buf_queue_dyn_del(kbuf_queue_t *queue)
{
    volatile krhino_buf_queue_dyn_del_syscall_arg_t arg;

    arg.queue = queue;

    return SYSCALL(SYS_KRHINO_BUF_QUEUE_DYN_DEL, &arg);
}

kstat_t krhino_buf_queue_send(kbuf_queue_t *queue,
                              void *msg, size_t size)
{
    volatile krhino_buf_queue_send_syscall_arg_t arg;

    arg.queue = queue;
    arg.msg = msg;
    arg.size = size;

    return SYSCALL(SYS_KRHINO_BUF_QUEUE_SEND, &arg);
}

kstat_t krhino_buf_queue_recv(kbuf_queue_t *queue,
                              tick_t ticks, void *msg,
                              size_t *size)
{
    volatile krhino_buf_queue_recv_syscall_arg_t arg;

    arg.queue = queue;
    arg.ticks = ticks;
    arg.msg = msg;
    arg.size = size;

    return SYSCALL(SYS_KRHINO_BUF_QUEUE_RECV, &arg);
}

kstat_t krhino_buf_queue_flush(kbuf_queue_t *queue)
{
    volatile krhino_buf_queue_flush_syscall_arg_t arg;

    arg.queue = queue;

    return SYSCALL(SYS_KRHINO_BUF_QUEUE_FLUSH, &arg);
}

kstat_t krhino_buf_queue_info_get(kbuf_queue_t *queue,
                                  kbuf_queue_info_t *info)
{
    volatile krhino_buf_queue_info_get_syscall_arg_t arg;

    arg.queue = queue;
    arg.info = info;

    return SYSCALL(SYS_KRHINO_BUF_QUEUE_INFO_GET, &arg);
}


/* ----------------- proc msg ----------------- */

size_t krhino_msg_get(uint32_t key, uint32_t flg, size_t size)
{
    volatile krhino_msg_get_syscall_arg_t arg;

    arg.key = key;
    arg.flg = flg;
    arg.size = size;

    return SYSCALL(SYS_KRHINO_MSG_GET, &arg);
}

kstat_t krhino_msg_snd(size_t msg_id,
                       void *msg,
                       size_t msg_sz)
{
    volatile krhino_msg_snd_syscall_arg_t arg;

    arg.msg_id = msg_id;
    arg.msg = msg;
    arg.msg_sz = msg_sz;

    return SYSCALL(SYS_KRHINO_MSG_SND, &arg);
}

kstat_t krhino_msg_recv(size_t msg_id,
                        tick_t ticks,
                        void *msg,
                        size_t *msg_sz)
{
    volatile krhino_msg_recv_syscall_arg_t arg;

    arg.msg_id = msg_id;
    arg.ticks = ticks;
    arg.msg = msg;
    arg.msg_sz = msg_sz;

    return SYSCALL(SYS_KRHINO_MSG_RECV, &arg);
}


/* ---------------- hal uart ---------------- */

int32_t hal_uart_init(uart_dev_t *uart)
{
    volatile hal_uart_init_syscall_arg_t arg;

    arg.uart = uart;

    return SYSCALL(SYS_HAL_UART_INIT, &arg);
}

int32_t hal_uart_send(uart_dev_t *uart,
                      const void *data,
                      uint32_t size,
                      uint32_t timeout)
{
    volatile hal_uart_send_syscall_arg_t arg;

    arg.uart = uart;
    arg.data = data;
    arg.size = size;
    arg.timeout = timeout;

    return SYSCALL(SYS_HAL_UART_SEND, &arg);
}

int32_t hal_uart_recv(uart_dev_t *uart,
                      void *data,
                      uint32_t expect_size,
                      uint32_t timeout)
{
    volatile hal_uart_recv_syscall_arg_t arg;

    arg.uart = uart;
    arg.data = data;
    arg.expect_size = expect_size;
    arg.timeout = timeout;

    return SYSCALL(SYS_HAL_UART_RECV, &arg);
}

int32_t hal_uart_recv_II(uart_dev_t *uart,
                         void *data,
                         uint32_t expect_size,
                         uint32_t *recv_size,
                         uint32_t timeout)
{
    volatile hal_uart_recv_II_syscall_arg_t arg;

    arg.uart = uart;
    arg.data = data;
    arg.expect_size = expect_size;
    arg.recv_size = recv_size;
    arg.timeout = timeout;

    return SYSCALL(SYS_HAL_UART_RECV_II, &arg);
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    volatile hal_uart_finalize_syscall_arg_t arg;

    arg.uart = uart;

    return SYSCALL(SYS_HAL_UART_FINALIZE, &arg);
}


/* ------------------ vfs ------------------ */

off_t aos_lseek(int fd, off_t offset, int whence)
{
    volatile aos_lseek_syscall_arg_t arg;

    arg.fd = fd;
    arg.offset = offset;
    arg.whence = whence;

    return SYSCALL(SYS_AOS_LSEEK, &arg);
}

int aos_close(int fd)
{
    volatile aos_close_syscall_arg_t arg;

    arg.fd = fd;

    return SYSCALL(SYS_AOS_CLOSE, &arg);
}

ssize_t aos_read(int fd,
                 void *buf,
                 size_t nbytes)
{
    volatile aos_read_syscall_arg_t arg;

    arg.fd = fd;
    arg.buf = buf;
    arg.nbytes = nbytes;

    return SYSCALL(SYS_AOS_READ, &arg);
}

ssize_t aos_write(int fd,
                 const void *buf,
                 size_t nbytes)
{
    volatile aos_write_syscall_arg_t arg;

    arg.fd = fd;
    arg.buf = buf;
    arg.nbytes = nbytes;

    return SYSCALL(SYS_AOS_WRITE, &arg);
}

