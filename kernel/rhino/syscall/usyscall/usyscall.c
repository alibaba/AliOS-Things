/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <arch_syscall.h>
#include <syscall.h>
#include <u_task.h>

#if (RHINO_CONFIG_USER_SPACE > 0)

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

int32_t hal_uart_send(uart_dev_t *uart, const void *data,
                      uint32_t size, uint32_t timeout)
{
    volatile hal_uart_send_syscall_arg_t arg;

    arg.uart = uart;
    arg.data = data;
    arg.size = size;
    arg.timeout = timeout;

    return SYSCALL(SYS_HAL_UART_SEND, &arg);
}

kstat_t krhino_utask_create(ktask_t *task,
                            const name_t *name,
                            void *arg,
                            uint8_t pri,
                            tick_t ticks,
                            cpu_stack_t *ustack_buf,
                            size_t ustack,
                            size_t kstack,
                            task_entry_t entry,
                            uint8_t autorun);



kstat_t krhino_utask_create(ktask_t *task, const name_t *name, void *arg,
                           uint8_t prio, tick_t ticks, cpu_stack_t *stack_buf,
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

void *krhino_mm_alloc(size_t size)
{
    volatile krhino_mm_alloc_syscall_arg_t arg;

    arg.size = size;

    return SYSCALL(SYS_KRHINO_MM_ALLOC, &arg);
}

void krhino_mm_free(void *ptr)
{
    volatile krhino_mm_free_syscall_arg_t arg;

    arg.ptr = ptr;

    return SYSCALL(SYS_KRHINO_MM_FREE, &arg);
}

void *krhino_mm_realloc(void *oldmem, size_t newsize)
{
    volatile krhino_mm_realloc_syscall_arg_t arg;

    arg.oldmem = oldmem;
    arg.newsize = newsize;

    return SYSCALL(SYS_KRHINO_MM_ALLOC, &arg);
}
#endif // RHINO_CONFIG_USER_SPACE

