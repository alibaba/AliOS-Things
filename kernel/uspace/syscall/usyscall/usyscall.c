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
    volatile krhino_task_sleep_syscall_arg_t _arg;

    _arg.tick = tick;

    return SYSCALL(SYS_KRHINO_TASK_SLEEP, &_arg);
}

kstat_t krhino_utask_create(ktask_t **task, const name_t *name,
                            void *arg, uint8_t prio, tick_t ticks,
                            cpu_stack_t *stack_buf, size_t stack_size,
                            size_t kstack_size, task_entry_t entry,
                            uint8_t autorun)
{
    volatile krhino_utask_create_syscall_arg_t _arg;

    _arg.task        = task;
    _arg.name        = name;
    _arg.arg         = arg;
    _arg.prio        = prio;
    _arg.ticks       = ticks;
    _arg.stack_buf   = stack_buf;
    _arg.stack_size  = stack_size;
    _arg.kstack_size = kstack_size;
    _arg.entry       = entry;
    _arg.autorun     = autorun;

    return SYSCALL(SYS_KRHINO_UTASK_CREATE, &_arg);
}

kstat_t krhino_utask_del(ktask_t *task)
{
    volatile krhino_utask_del_syscall_arg_t _arg;

    _arg.task = task;

    return SYSCALL(SYS_KRHINO_UTASK_DEL, &_arg);
}

/* ------------------- process ------------------- */
kstat_t krhino_uprocess_create(ktask_t **task, const name_t *name,
                               void *arg, uint8_t prio, tick_t ticks,
                               cpu_stack_t *stack_buf, size_t stack_size,
                               size_t kstack_size, task_entry_t entry,
                               uint32_t pid, uint8_t autorun)
{
    volatile krhino_uprocess_create_syscall_arg_t _arg;

    _arg.task        = task;
    _arg.name        = name;
    _arg.arg         = arg;
    _arg.prio        = prio;
    _arg.ticks       = ticks;
    _arg.stack_buf   = stack_buf;
    _arg.stack_size  = stack_size;
    _arg.kstack_size = kstack_size;
    _arg.entry       = entry;
    _arg.pid         = pid;
    _arg.autorun     = autorun;

    return SYSCALL(SYS_KRHINO_UPROCESS_CREATE, &_arg);
}

kstat_t krhino_uprocess_exit(void)
{
    return SYSCALL(SYS_KRHINO_UPROCESS_EXIT, NULL);
}

void krhino_uprocess_res_get(int32_t id, void **res)
{
    volatile krhino_uprocess_res_get_syscall_arg_t _arg;

    _arg.id  = id;
    _arg.res = res;

    SYSCALL(SYS_KRHINO_UPROCESS_RES_GET, &_arg);
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
    volatile krhino_ms_to_ticks_syscall_arg_t _arg;

    _arg.ms = ms;

    return SYSCALL(SYS_KRHINO_MS_TO_TICKS, &_arg);
}

sys_time_t krhino_ticks_to_ms(tick_t ticks)
{
    volatile krhino_ticks_to_ms_syscall_arg_t _arg;

    _arg.ticks = ticks;

    return SYSCALL(SYS_KRHINO_TICKS_TO_MS, &_arg);
}

/* ------------------ mutex ------------------ */
kstat_t krhino_mutex_dyn_create(kmutex_t **mutex, const name_t *name)
{
    volatile krhino_mutex_dyn_create_syscall_arg_t _arg;

    _arg.mutex = mutex;
    _arg.name  = name;

    return SYSCALL(SYS_KRHINO_MUTEX_DYN_CREATE, &_arg);
}

kstat_t krhino_mutex_dyn_del(kmutex_t *mutex)
{
    volatile krhino_mutex_dyn_del_syscall_arg_t _arg;

    _arg.mutex = mutex;

    return SYSCALL(SYS_KRHINO_MUTEX_DYN_DEL, &_arg);
}

kstat_t krhino_mutex_lock(kmutex_t *mutex, tick_t ticks)
{
    volatile krhino_mutex_lock_syscall_arg_t _arg;

    _arg.mutex = mutex;
    _arg.ticks = ticks;

    return SYSCALL(SYS_KRHINO_MUTEX_LOCK, &_arg);
}

kstat_t krhino_mutex_unlock(kmutex_t *mutex)
{
    volatile krhino_mutex_unlock_syscall_arg_t _arg;

    _arg.mutex = mutex;

    return SYSCALL(SYS_KRHINO_MUTEX_UNLOCK, &_arg);
}

/* ------------------ semphore ------------------ */
kstat_t krhino_sem_dyn_create(ksem_t **sem, const name_t *name, sem_count_t count)
{
    volatile krhino_sem_dyn_create_syscall_arg_t _arg;

    _arg.sem   = sem;
    _arg.name  = name;
    _arg.count = count;

    return SYSCALL(SYS_KRHINO_SEM_DYN_CREATE, &_arg);
}

kstat_t krhino_sem_dyn_del(ksem_t *sem)
{
    volatile krhino_sem_dyn_del_syscall_arg_t _arg;

    _arg.sem = sem;

    return SYSCALL(SYS_KRHINO_SEM_DYN_DEL, &_arg);
}

kstat_t krhino_sem_give(ksem_t *sem)
{
    volatile krhino_sem_give_syscall_arg_t _arg;

    _arg.sem = sem;

    return SYSCALL(SYS_KRHINO_SEM_GIVE, &_arg);
}

kstat_t krhino_sem_take(ksem_t *sem, tick_t ticks)
{
    volatile krhino_sem_take_syscall_arg_t _arg;

    _arg.sem   = sem;
    _arg.ticks = ticks;

    return SYSCALL(SYS_KRHINO_SEM_TAKE, &_arg);
}

/* -------------------- queue ---------------------*/
kstat_t krhino_queue_dyn_create(kqueue_t **queue, const name_t *name,
                                size_t msg_num)
{
    volatile krhino_queue_dyn_create_syscall_arg_t arg;

    arg.queue   = queue;
    arg.name    = name;
    arg.msg_num = msg_num;

    return SYSCALL(SYS_KRHINO_QUEUE_DYN_CREATE, &arg);
}

kstat_t krhino_queue_dyn_del(kqueue_t *queue)
{
    volatile krhino_queue_dyn_del_syscall_arg_t arg;

    arg.queue = queue;

    return SYSCALL(SYS_KRHINO_QUEUE_DYN_DEL, &arg);
}

kstat_t krhino_queue_back_send(kqueue_t *queue, void *msg)
{
    volatile krhino_queue_back_send_syscall_arg_t arg;

    arg.queue = queue;
    arg. msg  = msg;

    return SYSCALL(SYS_KRHINO_QUEUE_BACK_SEND, &arg);
}

kstat_t krhino_queue_all_send(kqueue_t *queue, void *msg)
{
    volatile krhino_queue_all_send_syscall_arg_t arg;

    arg.queue = queue;
    arg.msg   = msg;

    return SYSCALL(SYS_KRHINO_QUEUE_ALL_SEND, &arg);
}

kstat_t krhino_queue_recv(kqueue_t *queue, tick_t ticks, void **msg)
{
    volatile krhino_queue_recv_syscall_arg_t arg;

    arg.queue  = queue;
    arg.ticks  = ticks;
    arg.msg    = msg;

    return SYSCALL(SYS_KRHINO_QUEUE_RECV, &arg);
}

kstat_t krhino_queue_flush(kqueue_t *queue)
{
    volatile krhino_queue_flush_syscall_arg_t arg;

    arg.queue = queue;

    return SYSCALL(SYS_KRHINO_QUEUE_FLUSH, &arg);
}

/* ------------------ buf queue -------------------*/
kstat_t krhino_buf_queue_create(kbuf_queue_t *queue, const name_t *name,
                                void *buf, size_t size, size_t max_msg)
{
    volatile krhino_buf_queue_create_syscall_arg_t _arg;

    _arg.queue   = queue;
    _arg.name    = name;
    _arg.buf     = buf;
    _arg.size    = size;
    _arg.max_msg = max_msg;

    return SYSCALL(SYS_KRHINO_BUF_QUEUE_CREATE, &_arg);
}

kstat_t krhino_fix_buf_queue_create(kbuf_queue_t *queue, const name_t *name,
                                    void *buf, size_t msg_size, size_t msg_num)
{
    volatile krhino_fix_buf_queue_create_syscall_arg_t _arg;

    _arg.queue    = queue;
    _arg.name     = name;
    _arg.buf      = buf;
    _arg.msg_size = msg_size;
    _arg.msg_num  = msg_num;

    return SYSCALL(SYS_KRHINO_FIX_BUF_QUEUE_CREATE, &_arg);
}

kstat_t krhino_buf_queue_del(kbuf_queue_t *queue)
{
    volatile krhino_buf_queue_del_syscall_arg_t _arg;

    _arg.queue = queue;

    return SYSCALL(SYS_KRHINO_BUF_QUEUE_DEL, &_arg);
}

kstat_t krhino_buf_queue_dyn_create(kbuf_queue_t **queue, const name_t *name,
                                    size_t size, size_t max_msg)
{
    volatile krhino_buf_queue_dyn_create_syscall_arg_t _arg;

    _arg.queue   = queue;
    _arg.name    = name;
    _arg.size    = size;
    _arg.max_msg = max_msg;

    return SYSCALL(SYS_KRHINO_BUF_QUEUE_DYN_CREATE, &_arg);
}

kstat_t krhino_buf_queue_dyn_del(kbuf_queue_t *queue)
{
    volatile krhino_buf_queue_dyn_del_syscall_arg_t _arg;

    _arg.queue = queue;

    return SYSCALL(SYS_KRHINO_BUF_QUEUE_DYN_DEL, &_arg);
}

kstat_t krhino_buf_queue_send(kbuf_queue_t *queue, void *msg, size_t size)
{
    volatile krhino_buf_queue_send_syscall_arg_t _arg;

    _arg.queue = queue;
    _arg.msg   = msg;
    _arg.size  = size;

    return SYSCALL(SYS_KRHINO_BUF_QUEUE_SEND, &_arg);
}

kstat_t krhino_buf_queue_recv(kbuf_queue_t *queue, tick_t ticks,
                              void *msg, size_t *size)
{
    volatile krhino_buf_queue_recv_syscall_arg_t _arg;

    _arg.queue = queue;
    _arg.ticks = ticks;
    _arg.msg   = msg;
    _arg.size  = size;

    return SYSCALL(SYS_KRHINO_BUF_QUEUE_RECV, &_arg);
}

kstat_t krhino_buf_queue_flush(kbuf_queue_t *queue)
{
    volatile krhino_buf_queue_flush_syscall_arg_t _arg;

    _arg.queue = queue;

    return SYSCALL(SYS_KRHINO_BUF_QUEUE_FLUSH, &_arg);
}

kstat_t krhino_buf_queue_info_get(kbuf_queue_t *queue, kbuf_queue_info_t *info)
{
    volatile krhino_buf_queue_info_get_syscall_arg_t _arg;

    _arg.queue = queue;
    _arg.info  = info;

    return SYSCALL(SYS_KRHINO_BUF_QUEUE_INFO_GET, &_arg);
}

/* ----------------- proc msg ----------------- */
size_t krhino_msg_get(uint32_t key, uint32_t flg, size_t size)
{
    volatile krhino_msg_get_syscall_arg_t _arg;

    _arg.key  = key;
    _arg.flg  = flg;
    _arg.size = size;

    return SYSCALL(SYS_KRHINO_MSG_GET, &_arg);
}

kstat_t krhino_msg_snd(size_t msg_id, void *msg, size_t msg_sz)
{
    volatile krhino_msg_snd_syscall_arg_t _arg;

    _arg.msg_id = msg_id;
    _arg.msg    = msg;
    _arg.msg_sz = msg_sz;

    return SYSCALL(SYS_KRHINO_MSG_SND, &_arg);
}

kstat_t krhino_msg_recv(size_t msg_id, tick_t ticks, void *msg, size_t *msg_sz)
{
    volatile krhino_msg_recv_syscall_arg_t _arg;

    _arg.msg_id = msg_id;
    _arg.ticks  = ticks;
    _arg.msg    = msg;
    _arg.msg_sz = msg_sz;

    return SYSCALL(SYS_KRHINO_MSG_RECV, &_arg);
}

/* ---------------- hal uart ---------------- */
int32_t hal_uart_init(uart_dev_t *uart)
{
    volatile hal_uart_init_syscall_arg_t _arg;

    _arg.uart = uart;

    return SYSCALL(SYS_HAL_UART_INIT, &_arg);
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size,
                      uint32_t timeout)
{
    volatile hal_uart_send_syscall_arg_t _arg;

    _arg.uart    = uart;
    _arg.data    = data;
    _arg.size    = size;
    _arg.timeout = timeout;

    return SYSCALL(SYS_HAL_UART_SEND, &_arg);
}

int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t timeout)
{
    volatile hal_uart_recv_syscall_arg_t _arg;

    _arg.uart        = uart;
    _arg.data        = data;
    _arg.expect_size = expect_size;
    _arg.timeout     = timeout;

    return SYSCALL(SYS_HAL_UART_RECV, &_arg);
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout)
{
    volatile hal_uart_recv_II_syscall_arg_t _arg;

    _arg.uart        = uart;
    _arg.data        = data;
    _arg.expect_size = expect_size;
    _arg.recv_size   = recv_size;
    _arg.timeout     = timeout;

    return SYSCALL(SYS_HAL_UART_RECV_II, &_arg);
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    volatile hal_uart_finalize_syscall_arg_t _arg;

    _arg.uart = uart;

    return SYSCALL(SYS_HAL_UART_FINALIZE, &_arg);
}

/* ------------------ vfs ------------------ */
off_t aos_lseek(int fd, off_t offset, int whence)
{
    volatile aos_lseek_syscall_arg_t _arg;

    _arg.fd     = fd;
    _arg.offset = offset;
    _arg.whence = whence;

    return SYSCALL(SYS_AOS_LSEEK, &_arg);
}

int aos_close(int fd)
{
    volatile aos_close_syscall_arg_t _arg;

    _arg.fd = fd;

    return SYSCALL(SYS_AOS_CLOSE, &_arg);
}

ssize_t aos_read(int fd, void *buf, size_t nbytes)
{
    volatile aos_read_syscall_arg_t _arg;

    _arg.fd     = fd;
    _arg.buf    = buf;
    _arg.nbytes = nbytes;

    return SYSCALL(SYS_AOS_READ, &_arg);
}

ssize_t aos_write(int fd, const void *buf, size_t nbytes)
{
    volatile aos_write_syscall_arg_t _arg;

    _arg.fd     = fd;
    _arg.buf    = buf;
    _arg.nbytes = nbytes;

    return SYSCALL(SYS_AOS_WRITE, &_arg);
}

