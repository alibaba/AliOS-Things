/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include <stdarg.h>
#include <fcntl.h>
#include "sys/types.h"
#include "syscall.h"
#include "aos_obj_syscall_arg.h"
#include "syscall_no.h"

static int aos_handle_is_valid(obj_handle_t handle)
{
    if (handle == 0) {
        return 0;
    }
    aos_obj_comm_syscall_arg_t arg;
    arg.handle = handle;
    return (int)SYSCALL(SYS_AOS_HANDLE_VALID, &arg);
}

static int aos_handle_delete(obj_handle_t handle, int option)
{
    if (handle == 0) {
        return -EINVAL;
    }
    aos_obj_delete_syscall_arg_t arg;
    arg.handle = handle;
    arg.option = option;
    return (int)SYSCALL(SYS_AOS_HANDLE_DELETE, &arg);
}

static int aos_handle_close(obj_handle_t handle)
{
    if (handle == 0) {
        return -EINVAL;
    }

    aos_obj_comm_syscall_arg_t arg;
    arg.handle = handle;
    return (int)SYSCALL(SYS_AOS_HANDLE_CLOSE, &arg);
}

/* semphore */
int aos_sem_open(aos_hdl_t *sem, const char *name, int flag, uint32_t value)
{
    if (sem == NULL || name == NULL) {
        return -EINVAL;
    }

    aos_sem_open_syscall_arg_t arg;
    arg.phandle = sem;
    arg.name = name;
    arg.flag = flag;
    arg.count = value;

    return (int)SYSCALL(SYS_AOS_SEM_OPEN, &arg);
}

int aos_sem_new(aos_hdl_t *sem, uint32_t count)
{
    if (sem == NULL) {
        return -EINVAL;
    }

    aos_sem_new_syscall_arg_t arg;
    arg.phandle = sem;
    arg.count = count;
    return (int)SYSCALL(SYS_AOS_SEM_NEW, &arg);
}

void aos_sem_free(aos_hdl_t *sem)
{
    if (sem == NULL) {
        return;
    }

    if (aos_handle_delete((obj_handle_t)sem->hdl, 0) == 0) {
        sem->hdl = NULL;
    }
}

int aos_sem_close(aos_hdl_t *sem)
{
    int ret;
    if (sem == NULL) {
        return -EINVAL;
    }

    ret = aos_handle_close((obj_handle_t)sem->hdl);
    if (ret == 0) {
        sem->hdl = NULL;
    }
    return ret;
}

int aos_sem_wait(aos_hdl_t *sem, uint32_t  timeout)
{
    if (sem == NULL) {
        return -EINVAL;
    }

    aos_sem_wait_syscall_arg_t arg;
    arg.handle = (obj_handle_t)sem->hdl;
    arg.timeout = timeout;
    return (int)SYSCALL(SYS_AOS_SEM_WAIT, &arg);
}

void aos_sem_signal(aos_hdl_t *sem)
{

    if (sem == NULL) {
        return;
    }
    aos_obj_comm_syscall_arg_t arg;
    arg.handle = (obj_handle_t)sem->hdl;
    SYSCALL(SYS_AOS_SEM_POST, &arg);
}

int aos_sem_is_valid(aos_hdl_t *hdl)
{

    if (hdl == NULL || hdl->hdl == NULL) {
        return 0;
    }

    return aos_handle_is_valid((obj_handle_t)hdl->hdl);
}

void aos_sem_signal_all(aos_hdl_t *sem)
{

    if (sem == NULL) {
        return;
    }
    aos_obj_comm_syscall_arg_t arg;
    arg.handle = (obj_handle_t)sem->hdl;
    SYSCALL(SYS_AOS_SEM_POST_ALL, &arg);
}

int aos_sem_getvalue(aos_hdl_t *sem, uint32_t *val)
{
    if (sem == NULL) {
        return -EINVAL;
    }

    aos_sem_getvalue_syscall_arg_t arg;
    arg.handle = (obj_handle_t)sem->hdl;
    arg.val = val;
    return (int)SYSCALL(SYS_AOS_SEM_GETVALUE, &arg);
}

int aos_sem_unlink(const char *name)
{
    if (name == NULL) {
        return -EINVAL;
    }

    aos_obj_unlink_syscall_arg_t arg;
    arg.name = name;
    return (int)SYSCALL(SYS_AOS_SEM_UNLINK, &arg);
}

/* event*/
#if (AOS_KOBJ_EVENT > 0)
int aos_event_new(aos_hdl_t *event, uint32_t value)
{
    if (event == NULL) {
        return -EINVAL;
    }

    aos_event_new_syscall_arg_t arg;
    arg.phandle = event;
    arg.count = value;

    return (int)SYSCALL(SYS_AOS_EVENT_NEW, &arg);
}

void aos_event_free(aos_hdl_t *event)
{
    if (event == NULL) {
        return;
    }

    if (aos_handle_delete((obj_handle_t)event->hdl, 0) == 0) {
        event->hdl = NULL;
    }
}


int aos_event_open(aos_hdl_t *event, const char *name, int o_flag, uint32_t value)
{
    if (event == NULL || name == NULL) {
        return -EINVAL;
    }

    aos_event_open_syscall_arg_t arg;
    arg.phandle = event;
    arg.name = name;
    arg.flag = o_flag;
    arg.count = value;

    return (int)SYSCALL(SYS_AOS_EVENT_OPEN, &arg);
}
int aos_event_close(aos_hdl_t *event)
{
    int ret;
    if (event == NULL) {
        return -EINVAL;
    }

    ret = aos_handle_close((obj_handle_t)event->hdl);
    if (ret == 0) {
        event->hdl = NULL;
    }
    return ret;
}

int aos_event_unlink(const char *name)
{
    aos_obj_unlink_syscall_arg_t arg;
    if (name == NULL) {
        return -EINVAL;
    }

    arg.name = name;
    return (int)SYSCALL(SYS_AOS_EVENT_UNLINK, &arg);
}

int aos_event_get(aos_hdl_t *event, uint32_t value, uint8_t opt, uint32_t *actl_value, uint32_t  timeout)
{
    if (event == NULL) {
        return -EINVAL;
    }

    aos_event_get_syscall_arg_t arg;
    arg.handle = (obj_handle_t)event->hdl;
    arg.value = value;
    arg.opt = opt;
    arg.actl_value = actl_value;
    arg.timeout = timeout;
    return (int)SYSCALL(SYS_AOS_EVENT_GET, &arg);
}
int aos_event_set(aos_hdl_t *event, uint32_t value, uint8_t opt)
{
    if (event == NULL) {
        return -EINVAL;
    }

    aos_event_set_syscall_arg_t arg;
    arg.handle = (obj_handle_t)event->hdl;
    arg.value = value;
    arg.opt = opt;

    return (int)SYSCALL(SYS_AOS_EVENT_SET, &arg);
}
#endif
/*queue*/
int aos_queue_open(aos_hdl_t *queue, const char *name, int flag, size_t size, size_t max_msg)
{
    if (queue == NULL || name == NULL) {
        return -EINVAL;
    }

    aos_queue_open_syscall_arg_t arg;
    arg.phandle = queue;
    arg.name = name;
    arg.flag = flag;
    arg.size = size;
    arg.max_msg = max_msg;

    return (int)SYSCALL(SYS_AOS_QUEUE_OPEN, &arg);
}

int aos_queue_new(aos_hdl_t *queue, size_t size, size_t max_msg)
{
    if (queue == NULL) {
        return -EINVAL;
    }

    aos_queue_new_syscall_arg_t arg;
    arg.phandle = queue;
    arg.size = size;
    arg.max_msg = max_msg;

    return (int)SYSCALL(SYS_AOS_QUEUE_NEW, &arg);
}

int aos_queue_close(aos_hdl_t *queue)
{
    int ret;
    if (queue == NULL) {
        return -EINVAL;
    }

    ret = aos_handle_close((obj_handle_t)queue->hdl);
    if (ret == 0) {
        queue->hdl = NULL;
    }
    return ret;
}

int aos_queue_unlink(const char *name)
{
    aos_obj_unlink_syscall_arg_t arg;
    if (name == NULL) {
        return -EINVAL;
    }

    arg.name = name;
    return (int)SYSCALL(SYS_AOS_QUEUE_UNLINK, &arg);
}

void aos_queue_free(aos_hdl_t *queue)
{
    if (queue == NULL) {
        return;
    }

    if (aos_handle_delete((obj_handle_t)queue->hdl, 0) == 0) {
        queue->hdl = NULL;
    }
}

int aos_queue_send(aos_hdl_t *queue, void *msg, size_t size)
{
    if (queue == NULL || queue->hdl == NULL || msg == NULL) {
        return -EINVAL;
    }

    aos_queue_send_syscall_arg_t arg;
    arg.handle = (obj_handle_t)queue->hdl;
    arg.msg = msg;
    arg.size = size;

    return (int)SYSCALL(SYS_AOS_QUEUE_SEND, &arg);
}

int aos_queue_get_count(aos_hdl_t *queue)
{
    aos_obj_comm_syscall_arg_t arg;
    arg.handle = (obj_handle_t)queue->hdl;

    return (int)SYSCALL(SYS_AOS_QUEUE_GET_COUNT, &arg);
}

int aos_queue_recv(aos_hdl_t *queue, uint32_t ms, void *msg, size_t *size)
{
    if (queue == NULL || msg == NULL || size == NULL) {
        return -EINVAL;
    }

    aos_queue_recv_syscall_arg_t arg;
    arg.handle = (obj_handle_t)queue->hdl;
    arg.ms = ms;
    arg.msg = msg;
    arg.size = size;

    return (int)SYSCALL(SYS_AOS_QUEUE_RECV, &arg);
}

int aos_queue_is_valid(aos_hdl_t *hdl)
{

    if (hdl == NULL || hdl->hdl == NULL) {
        return 0;
    }

    return aos_handle_is_valid((obj_handle_t)hdl->hdl);
}

#if (OS_KOBJ_TIMER > 0)
int aos_timer_new(aos_hdl_t *timer, void (*fn)(void *, void *), void *arg,
                  uint8_t ms, uint8_t repeat)
{
    if (timer == NULL || fn == NULL) {
        return -EINVAL;
    }

    aos_timer_new_syscall_arg_t sys_arg;
    sys_arg.phandle = timer;
    sys_arg.fn = fn;
    sys_arg.arg = arg;
    sys_arg.ms = ms;
    sys_arg.repeat = repeat;
    sys_arg.auto_run = 1;

    return (int)SYSCALL(SYS_AOS_TIMER_NEW_EXT, &sys_arg);
}

int aos_timer_new_ext(aos_hdl_t *timer, void (*fn)(void *, void *), void *arg,
                      uint32_t ms, uint8_t repeat, uint8_t auto_run)
{
    if (timer == NULL || fn == NULL) {
        return -EINVAL;
    }

    aos_timer_new_syscall_arg_t sys_arg;
    sys_arg.phandle = timer;
    sys_arg.fn = fn;
    sys_arg.arg = arg;
    sys_arg.ms = ms;
    sys_arg.repeat = repeat;
    sys_arg.auto_run = auto_run;

    return (int)SYSCALL(SYS_AOS_TIMER_NEW_EXT, &sys_arg);
}

void aos_timer_free(aos_hdl_t *timer)
{
    if (timer == NULL) {
        return;
    }

    if (aos_handle_delete((obj_handle_t)timer->hdl, 0) == 0) {
        timer->hdl = NULL;
    }
}

int aos_timer_start(aos_hdl_t *timer)
{
    if (timer == NULL) {
        return -EINVAL;
    }

    aos_obj_comm_syscall_arg_t arg;
    arg.handle = (obj_handle_t)timer->hdl;

    return (int)SYSCALL(SYS_AOS_TIMER_START, &arg);
}

int aos_timer_stop(aos_hdl_t *timer)
{
    if (timer == NULL) {
        return -EINVAL;
    }

    aos_obj_comm_syscall_arg_t arg;
    arg.handle = (obj_handle_t)timer->hdl;

    return (int)SYSCALL(SYS_AOS_TIMER_STOP, &arg);
}

int aos_timer_change(aos_hdl_t *timer, uint32_t ms)
{
    if (timer == NULL) {
        return -EINVAL;
    }

    aos_timer_change_syscall_arg_t arg;
    arg.handle = (obj_handle_t)timer->hdl;
    arg.ms = ms;

    return (int)SYSCALL(SYS_AOS_TIMER_CHANGE, &arg);
}
int aos_timer_change_once(aos_hdl_t *timer, uint32_t ms)
{
    if (timer == NULL) {
        return -EINVAL;
    }

    aos_timer_change_syscall_arg_t arg;
    arg.handle = (obj_handle_t) timer->hdl;
    arg.ms = ms;

    return (int)SYSCALL(SYS_AOS_TIMER_CHANGE_ONCE, &arg);
}
#endif

/* aos mutex */
int aos_mutex_new(aos_hdl_t *mutex)
{
    if (mutex == NULL) {
        return -EINVAL;
    }

    aos_mutex_new_syscall_arg_t sys_arg;
    sys_arg.phandle = mutex;

    return (int)SYSCALL(SYS_AOS_MUTEX_NEW, &sys_arg);
}
void aos_mutex_free(aos_hdl_t *mutex)
{
    if (mutex == NULL) {
        return;
    }

    if (aos_handle_delete((obj_handle_t)mutex->hdl, 0) == 0) {
        mutex->hdl = NULL;
    }
}
int aos_mutex_lock(aos_hdl_t *mutex, uint32_t timeout)
{
    if (mutex == NULL) {
        return -EINVAL;
    }

    aos_mutex_wait_syscall_arg_t arg;
    arg.handle = (obj_handle_t)mutex->hdl;
    arg.timeout = timeout;

    return (int)SYSCALL(SYS_AOS_MUTEX_LOCK, &arg);
}
int aos_mutex_unlock(aos_hdl_t *mutex)
{
    if (mutex == NULL) {
        return -EINVAL;
    }

    aos_obj_comm_syscall_arg_t arg;
    arg.handle = (obj_handle_t)mutex->hdl;

    return (int)SYSCALL(SYS_AOS_MUTEX_UNLOCK, &arg);
}
int aos_mutex_is_valid(aos_hdl_t *mutex)
{
    if (mutex == NULL) {
        return 0;
    }

    return aos_handle_is_valid((obj_handle_t)mutex->hdl);
}

/* ------------------ futex ------------------ */
int aos_do_futex(uint32_t *uaddr, int futex_op, uint32_t val, uint32_t timeout)
{
    aos_futex_syscall_arg_t _arg;

    _arg.uaddr = uaddr;
    _arg.futex_op = futex_op;
    _arg.val = val;
    _arg.timeout = timeout;

    return SYSCALL(SYS_AOS_FUTEX, (void *)&_arg);
}