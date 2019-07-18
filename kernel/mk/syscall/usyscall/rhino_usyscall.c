/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "syscall.h"
#include "k_api.h"
#include "rhino_syscall_arg.h"
#include "syscall_no.h"

/* ------------------- task ------------------- */
ktask_t *krhino_cur_task_get(void)
{
    return (ktask_t*)SYSCALL(SYS_KRHINO_CUR_TASK_GET, (void*)NULL);
}

kstat_t krhino_task_sleep(tick_t tick)
{
    krhino_task_sleep_syscall_arg_t _arg;

    _arg.tick = tick;

    return (kstat_t)SYSCALL(SYS_KRHINO_TASK_SLEEP, (void*)&_arg);
}

kstat_t krhino_task_suspend(ktask_t *task)
{
    krhino_task_suspend_syscall_arg_t _arg;

    _arg.task = task;

    return (kstat_t)SYSCALL(SYS_KRHINO_TASK_SUSPEND, (void*)&_arg);
}

kstat_t krhino_task_resume(ktask_t *task)
{
    krhino_task_resume_syscall_arg_t _arg;

    _arg.task = task;

    return (kstat_t)SYSCALL(SYS_KRHINO_TASK_RESUME, (void*)&_arg);
}

#if (RHINO_CONFIG_TASK_WAIT_ABORT > 0)
kstat_t krhino_task_wait_abort(ktask_t *task)
{
    krhino_task_wait_abort_syscall_arg_t _arg;

    _arg.task = task;

    return (kstat_t)SYSCALL(SYS_KRHINO_TASK_WAIT_ABORT, (void*)&_arg);
}
#endif

kstat_t krhino_utask_create(ktask_t **task, const name_t *name,
                            void *arg, uint8_t prio, tick_t ticks,
                            cpu_stack_t *stack_buf, size_t stack_size,
                            size_t kstack_size, task_entry_t entry,
                            uint8_t autorun)
{
    krhino_utask_create_syscall_arg_t _arg;

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

    return (kstat_t)SYSCALL(SYS_KRHINO_UTASK_CREATE, (void*)&_arg);
}

kstat_t krhino_utask_del(ktask_t *task)
{
    krhino_utask_del_syscall_arg_t _arg;

    _arg.task = task;

    return (kstat_t)SYSCALL(SYS_KRHINO_UTASK_DEL, (void*)&_arg);
}

/* ------------------- process ------------------- */
kstat_t krhino_uprocess_create(const name_t *app_name, ktask_t **task,
                               const name_t *name, void *arg,
                               uint8_t prio, tick_t ticks,
                               cpu_stack_t *stack_buf, size_t stack_size,
                               size_t kstack_size, task_entry_t entry,
                               uint32_t pid, uint8_t autorun)
{
    krhino_uprocess_create_syscall_arg_t _arg;

    _arg.app_name    = app_name;
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

    return (kstat_t)SYSCALL(SYS_KRHINO_UPROCESS_CREATE, (void*)&_arg);
}

kstat_t krhino_uprocess_exit(void)
{
    return (kstat_t)SYSCALL(SYS_KRHINO_UPROCESS_EXIT, NULL);
}

void krhino_uprocess_res_get(int32_t id, void **res)
{
    krhino_uprocess_res_get_syscall_arg_t _arg;

    _arg.id  = id;
    _arg.res = res;

    SYSCALL(SYS_KRHINO_UPROCESS_RES_GET, (void*)&_arg);
}

/* ------------------ time ------------------ */
sys_time_t krhino_sys_tick_get(void)
{
    sys_time_t sys_time;

    SYSCALL(SYS_KRHINO_SYS_TICK_GET, (void*)&sys_time);

    return sys_time;
}

sys_time_t krhino_sys_time_get(void)
{
    sys_time_t sys_time;

    SYSCALL(SYS_KRHINO_SYS_TIME_GET, (void*)&sys_time);

    return sys_time;
}

tick_t krhino_ms_to_ticks(sys_time_t ms)
{
    krhino_ms_to_ticks_syscall_arg_t _arg;

    _arg.ms = ms;

    SYSCALL(SYS_KRHINO_MS_TO_TICKS, (void*)&_arg);

    return _arg.tick;
}

sys_time_t krhino_ticks_to_ms(tick_t ticks)
{
    krhino_ticks_to_ms_syscall_arg_t _arg;

    _arg.ticks = ticks;

    SYSCALL(SYS_KRHINO_TICKS_TO_MS, (void*)&_arg);

    return _arg.ms;
}

/* ------------------ mutex ------------------ */
kstat_t krhino_mutex_dyn_create(kmutex_t **mutex, const name_t *name)
{
    krhino_mutex_dyn_create_syscall_arg_t _arg;

    _arg.mutex = mutex;
    _arg.name  = name;

    return (kstat_t)SYSCALL(SYS_KRHINO_MUTEX_DYN_CREATE, (void*)&_arg);
}

kstat_t krhino_mutex_dyn_del(kmutex_t *mutex)
{
    krhino_mutex_dyn_del_syscall_arg_t _arg;

    _arg.mutex = mutex;

    return (kstat_t)SYSCALL(SYS_KRHINO_MUTEX_DYN_DEL, (void*)&_arg);
}

kstat_t krhino_mutex_lock(kmutex_t *mutex, tick_t ticks)
{
    krhino_mutex_lock_syscall_arg_t _arg;

    _arg.mutex = mutex;
    _arg.ticks = ticks;

    return (kstat_t)SYSCALL(SYS_KRHINO_MUTEX_LOCK, (void*)&_arg);
}

kstat_t krhino_mutex_unlock(kmutex_t *mutex)
{
    krhino_mutex_unlock_syscall_arg_t _arg;

    _arg.mutex = mutex;

    return (kstat_t)SYSCALL(SYS_KRHINO_MUTEX_UNLOCK, (void*)&_arg);
}

/* ------------------ semphore ------------------ */
kstat_t krhino_sem_dyn_create(ksem_t **sem, const name_t *name, sem_count_t count)
{
    krhino_sem_dyn_create_syscall_arg_t _arg;

    _arg.sem   = sem;
    _arg.name  = name;
    _arg.count = count;

    return (kstat_t)SYSCALL(SYS_KRHINO_SEM_DYN_CREATE, (void*)&_arg);
}

kstat_t krhino_sem_dyn_del(ksem_t *sem)
{
    krhino_sem_dyn_del_syscall_arg_t _arg;

    _arg.sem = sem;

    return (kstat_t)SYSCALL(SYS_KRHINO_SEM_DYN_DEL, (void*)&_arg);
}

kstat_t krhino_sem_give(ksem_t *sem)
{
    krhino_sem_give_syscall_arg_t _arg;

    _arg.sem = sem;

    return (kstat_t)SYSCALL(SYS_KRHINO_SEM_GIVE, (void*)&_arg);
}

kstat_t krhino_sem_take(ksem_t *sem, tick_t ticks)
{
    krhino_sem_take_syscall_arg_t _arg;

    _arg.sem   = sem;
    _arg.ticks = ticks;

    return (kstat_t)SYSCALL(SYS_KRHINO_SEM_TAKE, (void*)&_arg);
}

/* -------------------- queue ---------------------*/
kstat_t krhino_queue_dyn_create(kqueue_t **queue, const name_t *name,
                                size_t msg_num)
{
    krhino_queue_dyn_create_syscall_arg_t _arg;

    _arg.queue   = queue;
    _arg.name    = name;
    _arg.msg_num = msg_num;

    return (kstat_t)SYSCALL(SYS_KRHINO_QUEUE_DYN_CREATE, (void*)&_arg);
}

kstat_t krhino_queue_dyn_del(kqueue_t *queue)
{
    krhino_queue_dyn_del_syscall_arg_t _arg;

    _arg.queue = queue;

    return (kstat_t)SYSCALL(SYS_KRHINO_QUEUE_DYN_DEL, (void*)&_arg);
}

kstat_t krhino_queue_back_send(kqueue_t *queue, void *msg)
{
    krhino_queue_back_send_syscall_arg_t _arg;

    _arg.queue = queue;
    _arg. msg  = msg;

    return (kstat_t)SYSCALL(SYS_KRHINO_QUEUE_BACK_SEND, (void*)&_arg);
}

kstat_t krhino_queue_all_send(kqueue_t *queue, void *msg)
{
    krhino_queue_all_send_syscall_arg_t _arg;

    _arg.queue = queue;
    _arg.msg   = msg;

    return (kstat_t)SYSCALL(SYS_KRHINO_QUEUE_ALL_SEND, (void*)&_arg);
}

kstat_t krhino_queue_recv(kqueue_t *queue, tick_t ticks, void **msg)
{
    krhino_queue_recv_syscall_arg_t _arg;

    _arg.queue  = queue;
    _arg.ticks  = ticks;
    _arg.msg    = msg;

    return (kstat_t)SYSCALL(SYS_KRHINO_QUEUE_RECV, (void*)&_arg);
}

kstat_t krhino_queue_flush(kqueue_t *queue)
{
    krhino_queue_flush_syscall_arg_t _arg;

    _arg.queue = queue;

    return (kstat_t)SYSCALL(SYS_KRHINO_QUEUE_FLUSH, (void*)&_arg);
}

/* ------------------ buf queue -------------------*/
kstat_t krhino_buf_queue_dyn_create(kbuf_queue_t **queue, const name_t *name,
                                    size_t size, size_t max_msg)
{
    krhino_buf_queue_dyn_create_syscall_arg_t _arg;

    _arg.queue   = queue;
    _arg.name    = name;
    _arg.size    = size;
    _arg.max_msg = max_msg;

    return (kstat_t)SYSCALL(SYS_KRHINO_BUF_QUEUE_DYN_CREATE, (void*)&_arg);
}

kstat_t krhino_fix_buf_queue_dyn_create(kbuf_queue_t **queue, const name_t *name,
                                        size_t msg_size, size_t msg_num)
{
    krhino_fix_buf_queue_dyn_create_syscall_arg_t _arg;

    _arg.queue    = queue;
    _arg.name     = name;
    _arg.msg_size = msg_size;
    _arg.msg_num  = msg_num;

    return (kstat_t)SYSCALL(SYS_KRHINO_FIX_BUF_QUEUE_DYN_CREATE, (void*)&_arg);
}

kstat_t krhino_buf_queue_dyn_del(kbuf_queue_t *queue)
{
    krhino_buf_queue_dyn_del_syscall_arg_t _arg;

    _arg.queue = queue;

    return (kstat_t)SYSCALL(SYS_KRHINO_BUF_QUEUE_DYN_DEL, (void*)&_arg);
}

kstat_t krhino_buf_queue_send(kbuf_queue_t *queue, void *msg, size_t size)
{
    krhino_buf_queue_send_syscall_arg_t _arg;

    _arg.queue = queue;
    _arg.msg   = msg;
    _arg.size  = size;

    return (kstat_t)SYSCALL(SYS_KRHINO_BUF_QUEUE_SEND, (void*)&_arg);
}

kstat_t krhino_buf_queue_recv(kbuf_queue_t *queue, tick_t ticks,
                              void *msg, size_t *size)
{
    krhino_buf_queue_recv_syscall_arg_t _arg;

    _arg.queue = queue;
    _arg.ticks = ticks;
    _arg.msg   = msg;
    _arg.size  = size;

    return (kstat_t)SYSCALL(SYS_KRHINO_BUF_QUEUE_RECV, (void*)&_arg);
}

kstat_t krhino_buf_queue_flush(kbuf_queue_t *queue)
{
    krhino_buf_queue_flush_syscall_arg_t _arg;

    _arg.queue = queue;

    return (kstat_t)SYSCALL(SYS_KRHINO_BUF_QUEUE_FLUSH, (void*)&_arg);
}

/* ----------------- event ------------------ */
kstat_t krhino_event_dyn_create(kevent_t **event, const name_t *name, uint32_t flags)
{
    krhino_event_dyn_create_syscall_arg_t _arg;

    _arg.event = event;
    _arg.name  = name;
    _arg.flags = flags;

    return (kstat_t)SYSCALL(SYS_KRHINO_EVENT_DYN_CREATE, (void*)&_arg);
}

kstat_t krhino_event_dyn_del(kevent_t *event)
{
    krhino_event_dyn_del_syscall_arg_t _arg;

    _arg.event = event;

    return (kstat_t)SYSCALL(SYS_KRHINO_EVENT_DYN_DEL, (void*)&_arg);
}

kstat_t krhino_event_get(kevent_t *event, uint32_t flags, uint8_t opt,
                         uint32_t *actl_flags, tick_t ticks)
{
    krhino_event_get_syscall_arg_t _arg;

    _arg.event      = event;
    _arg.flags      = flags;
    _arg.opt        = opt;
    _arg.actl_flags = actl_flags;
    _arg.ticks      = ticks;

    return (kstat_t)SYSCALL(SYS_KRHINO_EVENT_GET, (void*)&_arg);
}

kstat_t krhino_event_set(kevent_t *event, uint32_t flags, uint8_t opt)
{
    krhino_event_set_syscall_arg_t _arg;

    _arg.event = event;
    _arg.flags = flags;
    _arg.opt   = opt;

    return (kstat_t)SYSCALL(SYS_KRHINO_EVENT_SET, (void*)&_arg);
}

/* ----------------- proc msg ----------------- */
size_t krhino_msg_get(uint32_t key, uint32_t flg, size_t size)
{
    krhino_msg_get_syscall_arg_t _arg;

    _arg.key  = key;
    _arg.flg  = flg;
    _arg.size = size;

    return (size_t)SYSCALL(SYS_KRHINO_MSG_GET, (void*)&_arg);
}

kstat_t krhino_msg_snd(size_t msg_id, void *msg, size_t msg_sz)
{
    krhino_msg_snd_syscall_arg_t _arg;

    _arg.msg_id = msg_id;
    _arg.msg    = msg;
    _arg.msg_sz = msg_sz;

    return (kstat_t)SYSCALL(SYS_KRHINO_MSG_SND, (void*)&_arg);
}

kstat_t krhino_msg_recv(size_t msg_id, tick_t ticks, void *msg, size_t *msg_sz)
{
    krhino_msg_recv_syscall_arg_t _arg;

    _arg.msg_id = msg_id;
    _arg.ticks  = ticks;
    _arg.msg    = msg;
    _arg.msg_sz = msg_sz;

    return (kstat_t)SYSCALL(SYS_KRHINO_MSG_RECV, (void*)&_arg);
}

kstat_t krhino_msg_del(size_t msq_id)
{
    krhino_msg_del_syscall_arg_t _arg;

    _arg.msq_id = msq_id;

    return (kstat_t)SYSCALL(SYS_KRHINO_MSG_DEL, (void*)&_arg);
}

