/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <hal/hal.h>
#include <k_api.h>
#include <syscall.h>
#include <stdio.h>
#include <u_task.h>
#include <u_proc_msg.h>

/* ------------------ task ------------------ */
static ktask_t *sys_krhino_cur_task_get_stub(void *arg)
{
    arg = arg;

    return krhino_cur_task_get();
}

static void sys_krhino_task_sleep_stub(void *arg)
{
    krhino_task_sleep_syscall_arg_t *_arg = arg;

    krhino_task_sleep(_arg->tick);
}

static kstat_t sys_krhino_uprocess_create_stub(void *arg)
{
    krhino_uprocess_create_syscall_arg_t *_arg = arg;

    return krhino_uprocess_create(_arg->task, _arg->name, _arg->arg,
                                  _arg->prio, _arg->ticks, _arg->stack_buf,
                                  _arg->stack_size, _arg->kstack_size,
                                  _arg->entry, _arg->pid, _arg->autorun);
}

static kstat_t sys_krhino_uprocess_exit_stub(void *arg)
{
    arg = arg;

    return krhino_uprocess_exit();
}

static kstat_t sys_krhino_utask_create_stub(void *arg)
{
    krhino_utask_create_syscall_arg_t *_arg = arg;

    return krhino_utask_create(_arg->task, _arg->name, _arg->arg,
                               _arg->prio, _arg->ticks, _arg->stack_buf,
                               _arg->stack_size, _arg->kstack_size,
                               _arg->entry, _arg->autorun);
}

static kstat_t sys_krhino_utask_del_stub(void *arg)
{
    krhino_utask_del_syscall_arg_t *_arg = arg;

    return krhino_utask_del(_arg->task);
}

/* ------------------- time ------------------ */
static sys_time_t sys_krhino_sys_tick_get_stub(void *arg)
{
    arg = arg;

    return krhino_sys_tick_get();
}

static sys_time_t sys_krhino_sys_time_get_stub(void *arg)
{
    arg = arg;

    return krhino_sys_time_get();
}

static tick_t sys_krhino_ms_to_ticks_stub(void *arg)
{
    krhino_ms_to_ticks_syscall_arg_t *_arg = arg;

    return krhino_ms_to_ticks(_arg->ms);
}

static sys_time_t sys_krhino_ticks_to_ms_stub(void *arg)
{
    krhino_ticks_to_ms_syscall_arg_t *_arg = arg;

    return krhino_ticks_to_ms(_arg->ticks);
}

/* ------------------ mutex ------------------ */
static kstat_t sys_krhino_mutex_dyn_create_stub(void *arg)
{
    krhino_mutex_dyn_create_syscall_arg_t *_arg = arg;

    return krhino_mutex_dyn_create(_arg->mutex, _arg->name);
}

static kstat_t sys_krhino_mutex_dyn_del_stub(void *arg)
{
    krhino_mutex_dyn_del_syscall_arg_t *_arg = arg;

    return krhino_mutex_dyn_del(_arg->mutex);
}

static kstat_t sys_krhino_mutex_lock_stub(void *arg)
{
    krhino_mutex_lock_syscall_arg_t *_arg = arg;

    return krhino_mutex_lock(_arg->mutex, _arg->ticks);
}

static kstat_t sys_krhino_mutex_unlock_stub(void *arg)
{
    krhino_mutex_unlock_syscall_arg_t *_arg = arg;

    return krhino_mutex_unlock(_arg->mutex);
}

/* ------------------ semphore ------------------ */
static kstat_t sys_krhino_sem_dyn_create_stub(void *arg)
{
    krhino_sem_dyn_create_syscall_arg_t *_arg = arg;

    return krhino_sem_dyn_create(_arg->sem, _arg->name, _arg->count);
}

static kstat_t sys_krhino_sem_dyn_del_stub(void *arg)
{
    krhino_sem_dyn_del_syscall_arg_t *_arg = arg;

    return krhino_sem_dyn_del(_arg->sem);
}

static kstat_t sys_krhino_sem_give_stub(void *arg)
{
    krhino_sem_give_syscall_arg_t *_arg = arg;

    return krhino_sem_give(_arg->sem);
}

static kstat_t sys_krhino_sem_take_stub(void *arg)
{
    krhino_sem_take_syscall_arg_t *_arg = arg;

    return krhino_sem_take(_arg->sem, _arg->ticks);
}

/* ------------------ buf queue -------------------*/
static kstat_t sys_krhino_buf_queue_create_stub(void *arg)
{
    krhino_buf_queue_create_syscall_arg_t *_arg = arg;

    return krhino_buf_queue_create(_arg->queue, _arg->name, _arg->buf,
                                   _arg->size, _arg->max_msg);
}

static kstat_t sys_krhino_fix_buf_queue_create_stub(void *arg)
{
    krhino_fix_buf_queue_create_syscall_arg_t *_arg = arg;

    return krhino_fix_buf_queue_create(_arg->queue, _arg->name, _arg->buf,
                                       _arg->msg_size, _arg->msg_num);
}

static kstat_t sys_krhino_buf_queue_del_stub(void *arg)
{
    krhino_buf_queue_del_syscall_arg_t *_arg = arg;

    return krhino_buf_queue_del(_arg->queue);
}

static kstat_t sys_krhino_buf_queue_dyn_create_stub(void *arg)
{

    krhino_buf_queue_dyn_create_syscall_arg_t *_arg = arg;

    return krhino_buf_queue_dyn_create(_arg->queue, _arg->name,
                                       _arg->size, _arg->max_msg);
}

static kstat_t sys_krhino_buf_queue_dyn_del_stub(void *arg)
{
    krhino_buf_queue_dyn_del_syscall_arg_t *_arg = arg;

    return krhino_buf_queue_dyn_del(_arg->queue);
}

static kstat_t sys_krhino_buf_queue_send_stub(void *arg)
{
    krhino_buf_queue_send_syscall_arg_t *_arg = arg;

    return krhino_buf_queue_send(_arg->queue, _arg->msg, _arg->size);
}

static kstat_t sys_krhino_buf_queue_recv_stub(void *arg)
{
    krhino_buf_queue_recv_syscall_arg_t *_arg = arg;

    return krhino_buf_queue_recv(_arg->queue, _arg->ticks, _arg->msg, _arg->size);
}

static kstat_t sys_krhino_buf_queue_flush_stub(void *arg)
{
    krhino_buf_queue_flush_syscall_arg_t *_arg = arg;

    return krhino_buf_queue_flush(_arg->queue);
}

static kstat_t sys_krhino_buf_queue_info_get_stub(void *arg)
{
    krhino_buf_queue_info_get_syscall_arg_t *_arg = arg;

    return krhino_buf_queue_info_get(_arg->queue, _arg->info);
}

/* ----------------- proc msg -------------------- */
static size_t sys_krhino_msg_get_stub(void *arg)
{
    krhino_msg_get_syscall_arg_t *_arg = arg;

    return krhino_msg_get(_arg->key, _arg->flg, _arg->size);
}

static kstat_t sys_krhino_msg_snd_stub(void *arg)
{
    krhino_msg_snd_syscall_arg_t *_arg = arg;

    return krhino_msg_snd(_arg->msg_id, _arg->msg, _arg->msg_sz);
}

static kstat_t sys_krhino_msg_recv_stub(void *arg)
{
    krhino_msg_recv_syscall_arg_t *_arg = arg;

    return krhino_msg_recv(_arg->msg_id, _arg->ticks, _arg->msg, _arg->msg_sz);
}

/* ----------------- hal uart ------------------- */
static int32_t sys_hal_uart_init_stub(void *arg)
{
    hal_uart_init_syscall_arg_t *_arg = arg;

    return hal_uart_init(_arg->uart);
}

static int32_t sys_hal_uart_send_stub(void *arg)
{
    hal_uart_send_syscall_arg_t *_arg = arg;

    return hal_uart_send(_arg->uart, _arg->data, _arg->size, _arg->timeout);
}

/* this function is almost discarded by kernel */
static int32_t sys_hal_uart_recv_stub(void *arg)
{
    hal_uart_recv_syscall_arg_t *_arg = arg;

    return 0;
}

static int32_t sys_hal_uart_recv_II_stub(void *arg)
{
    hal_uart_recv_II_syscall_arg_t *_arg = arg;

    return hal_uart_recv_II(_arg->uart, _arg->data, _arg->expect_size,
                            _arg->recv_size, _arg->timeout);
}

static int32_t sys_hal_uart_finalize_stub(void *arg)
{
    hal_uart_finalize_syscall_arg_t *_arg = arg;

    return hal_uart_finalize(_arg->uart);
}

/* -------------------- vfs ---------------------- */
static off_t sys_aos_lseek_stub(void *arg)
{
    aos_lseek_syscall_arg_t *_arg = arg;

    return aos_lseek(_arg->fd, _arg->offset, _arg->whence);
}

static int sys_aos_close_stub(void *arg)
{
    aos_close_syscall_arg_t *_arg = arg;

    return aos_close(_arg->fd);
}

static ssize_t sys_aos_read_stub(void *arg)
{
    aos_read_syscall_arg_t *_arg = arg;

    return aos_read(_arg->fd, _arg->buf, _arg->nbytes);
}

static ssize_t sys_aos_write_stub(void *arg)
{
    aos_read_syscall_arg_t *_arg = arg;

    return aos_write(_arg->fd, _arg->buf, _arg->nbytes);
}

/**************************************************************
 *   syscall table
 *************************************************************/
void *syscall_tbl[] = {
                       /* ------------------- task ----------------------*/
                       [SYS_KRHINO_CUR_TASK_GET]    = sys_krhino_cur_task_get_stub,
                       [SYS_KRHINO_TASK_SLEEP]      = sys_krhino_task_sleep_stub,
                       [SYS_KRHINO_UPROCESS_CREATE] = sys_krhino_uprocess_create_stub,
                       [SYS_KRHINO_UPROCESS_EXIT]   = sys_krhino_uprocess_exit_stub,
                       [SYS_KRHINO_UTASK_CREATE]    = sys_krhino_utask_create_stub,
                       [SYS_KRHINO_UTASK_DEL]       = sys_krhino_utask_del_stub,

                       /* ------------------- time ----------------------*/
                       [SYS_KRHINO_SYS_TIME_GET] = sys_krhino_sys_time_get_stub,
                       [SYS_KRHINO_SYS_TICK_GET] = sys_krhino_sys_tick_get_stub,
                       [SYS_KRHINO_MS_TO_TICKS]  = sys_krhino_ms_to_ticks_stub,
                       [SYS_KRHINO_TICKS_TO_MS]  = sys_krhino_ticks_to_ms_stub,

                       /* ------------------- mutex ---------------------*/
                       [SYS_KRHINO_MUTEX_DYN_CREATE] = sys_krhino_mutex_dyn_create_stub,
                       [SYS_KRHINO_MUTEX_DYN_DEL]    = sys_krhino_mutex_dyn_del_stub,
                       [SYS_KRHINO_MUTEX_LOCK]       = sys_krhino_mutex_lock_stub,
                       [SYS_KRHINO_MUTEX_UNLOCK]     = sys_krhino_mutex_unlock_stub,

                       /* ------------------ semphore --------------------*/
                       [SYS_KRHINO_SEM_DYN_CREATE] = sys_krhino_sem_dyn_create_stub,
                       [SYS_KRHINO_SEM_DYN_DEL]    = sys_krhino_sem_dyn_del_stub,
                       [SYS_KRHINO_SEM_TAKE]       = sys_krhino_sem_take_stub,
                       [SYS_KRHINO_SEM_GIVE]       = sys_krhino_sem_give_stub,

                       /* ------------------ buf queue --------------------*/
                       [SYS_KRHINO_BUF_QUEUE_CREATE]     = sys_krhino_buf_queue_create_stub,
                       [SYS_KRHINO_FIX_BUF_QUEUE_CREATE] = sys_krhino_fix_buf_queue_create_stub,
                       [SYS_KRHINO_BUF_QUEUE_DEL]        = sys_krhino_buf_queue_del_stub,
                       [SYS_KRHINO_BUF_QUEUE_DYN_CREATE] = sys_krhino_buf_queue_dyn_create_stub,
                       [SYS_KRHINO_BUF_QUEUE_DYN_DEL]    = sys_krhino_buf_queue_dyn_del_stub,
                       [SYS_KRHINO_BUF_QUEUE_SEND]       = sys_krhino_buf_queue_send_stub,
                       [SYS_KRHINO_BUF_QUEUE_RECV]       = sys_krhino_buf_queue_recv_stub,
                       [SYS_KRHINO_BUF_QUEUE_FLUSH]      = sys_krhino_buf_queue_flush_stub,
                       [SYS_KRHINO_BUF_QUEUE_INFO_GET]   = sys_krhino_buf_queue_info_get_stub,

                       /* ------------------ u_proc_msg -------------------*/
                       [SYS_KRHINO_MSG_GET]  = sys_krhino_msg_get_stub,
                       [SYS_KRHINO_MSG_SND]  = sys_krhino_msg_snd_stub,
                       [SYS_KRHINO_MSG_RECV] = sys_krhino_msg_recv_stub,

                       /* ------------------- hal uart ---------------------*/
                       [SYS_HAL_UART_INIT]     = sys_hal_uart_init_stub,
                       [SYS_HAL_UART_SEND]     = sys_hal_uart_send_stub,
                       [SYS_HAL_UART_RECV]     = sys_hal_uart_recv_stub,
                       [SYS_HAL_UART_RECV_II]  = sys_hal_uart_recv_II_stub,
                       [SYS_HAL_UART_FINALIZE] = sys_hal_uart_finalize_stub,

                       /* ---------------- vfs ------------------*/
                       [SYS_AOS_LSEEK] = sys_aos_lseek_stub,
                       [SYS_AOS_CLOSE] = sys_aos_close_stub,
                       [SYS_AOS_READ]  = sys_aos_read_stub,
                       [SYS_AOS_WRITE] = sys_aos_write_stub,

                       /* ---------------- end ------------------*/
                       NULL
                      };

