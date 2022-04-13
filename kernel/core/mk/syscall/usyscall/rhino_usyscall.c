/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#if (RHINO_CONFIG_NEWLIBC_REENT > 0)
    #include <reent.h>
#endif
#include "syscall.h"
#include "k_api.h"
#include "rhino_syscall_arg.h"
#include "syscall_no.h"
#include "utask.h"
#include "pthread.h"
#include "umm.h"
/* ------------------- task ------------------- */
ktask_t *krhino_cur_task_get(void)
{
    return (ktask_t *)SYSCALL(SYS_KRHINO_CUR_TASK_GET, (void *)NULL);
}

kstat_t krhino_task_sleep(tick_t tick)
{
    krhino_task_sleep_syscall_arg_t _arg;

    _arg.tick = tick;

    return (kstat_t)SYSCALL(SYS_KRHINO_TASK_SLEEP, (void *)&_arg);
}

kstat_t krhino_task_suspend(ktask_t *task)
{
    krhino_task_suspend_syscall_arg_t _arg;

    _arg.task = task;

    return (kstat_t)SYSCALL(SYS_KRHINO_TASK_SUSPEND, (void *)&_arg);
}

kstat_t krhino_task_resume(ktask_t *task)
{
    krhino_task_resume_syscall_arg_t _arg;

    _arg.task = task;

    return (kstat_t)SYSCALL(SYS_KRHINO_TASK_RESUME, (void *)&_arg);
}

kstat_t krhino_task_wait_abort(ktask_t *task)
{
    krhino_task_wait_abort_syscall_arg_t _arg;

    _arg.task = task;

    return (kstat_t)SYSCALL(SYS_KRHINO_TASK_WAIT_ABORT, (void *)&_arg);
}

kstat_t krhino_task_yield(void)
{
    return (kstat_t)SYSCALL(SYS_KRHINO_TASK_YIELD, NULL);
}

kstat_t krhino_task_pri_change(ktask_t *task, uint8_t pri, uint8_t *old_pri)
{
    krhino_task_pri_change_syscall_arg_t arg;

    arg.task    = task;
    arg.pri     = pri;
    arg.old_pri = old_pri;

    return (kstat_t)SYSCALL(SYS_KRHINO_TASK_PRI_CHANGE, &arg);
}

kstat_t krhino_task_pri_get(ktask_t *task, uint8_t *priority)
{
    kstat_t ret;
    krhino_task_pri_get_syscall_arg_t arg;

    arg.task     = task;
    arg.priority = 0;

    ret = (kstat_t)SYSCALL(SYS_KRHINO_TASK_PRI_GET, &arg);
    if (ret == RHINO_SUCCESS) {
        *priority = arg.priority;
    }

    return ret;
}

kstat_t krhino_task_info_get(ktask_t *task, size_t idx, void **info)
{
    krhino_task_info_get_syscall_arg_t arg;

    arg.task = task;
    arg.idx  = idx;
    arg.info = info;

    return (kstat_t)SYSCALL(SYS_KRHINO_TASK_INFO_GET, &arg);
}

kstat_t krhino_task_info_set(ktask_t *task, size_t idx, void *info)
{
    krhino_task_info_set_syscall_arg_t arg;

    arg.task = task;
    arg.idx  = idx;
    arg.info = info;

    return (kstat_t)SYSCALL(SYS_KRHINO_TASK_INFO_SET, &arg);
}

kstat_t krhino_task_ptcb_get(ktask_t *task, void **ptcb)
{
    krhino_task_ptcb_get_syscall_arg_t arg;

    arg.task = task;
    arg.ptcb = ptcb;

    return (kstat_t)SYSCALL(SYS_KRHINO_TASK_PTCB_GET, &arg);
}

kstat_t krhino_task_ptcb_set(ktask_t *task, void *ptcb)
{
    krhino_task_ptcb_set_syscall_arg_t arg;

    arg.task = task;
    arg.ptcb = ptcb;

    return (kstat_t)SYSCALL(SYS_KRHINO_TASK_PTCB_SET, &arg);
}

kstat_t krhino_task_name_set(ktask_t *task, const char *name)
{
    krhino_task_name_set_syscall_arg_t arg;

    arg.task = task;
    arg.name = name;

    return (kstat_t)SYSCALL(SYS_KRHINO_TASK_NAME_SET, &arg);
}

kstat_t krhino_task_name_get(ktask_t *task, const char **name)
{
    krhino_task_name_get_syscall_arg_t arg;

    arg.task = task;
    arg.name = name;

    return (kstat_t)SYSCALL(SYS_KRHINO_TASK_NAME_GET, &arg);
}

kstat_t krhino_task_time_slice_set(ktask_t *task, size_t slice)
{
    krhino_task_time_slice_set_syscall_arg_t arg;

    arg.task  = task;
    arg.slice = slice;

    return (kstat_t)SYSCALL(SYS_KRHINO_TASK_TIME_SLICE_SET, &arg);
}

kstat_t krhino_task_time_slice_get(ktask_t *task, uint32_t *slice)
{
    kstat_t ret = 0;
    krhino_task_time_slice_get_syscall_arg_t arg;

    arg.task  = task;
    arg.slice = 0;

    ret = (kstat_t)SYSCALL(SYS_KRHINO_TASK_TIME_SLICE_GET, &arg);
    if (ret == RHINO_SUCCESS) {
        *slice = arg.slice;
    }

    return ret;
}

uint32_t krhino_task_id_get()
{
    return (uint32_t)SYSCALL(SYS_KRHINO_TASK_ID_GET, NULL);
}

kstat_t krhino_utask_create(ktask_t **task, const name_t *name,
                            void *arg, uint8_t prio, tick_t ticks,
                            cpu_stack_t *stack_buf, size_t stack_size,
                            size_t kstack_size, task_entry_t entry,
                            uint8_t flag)
{
    krhino_utask_create_syscall_arg_t _arg;
    void *ptcb = NULL;
    kstat_t ret = 0;

    _arg.task        = task;
    _arg.name        = name;
    _arg.arg         = arg;
    _arg.prio        = prio;
    _arg.ticks       = ticks;
    _arg.stack_buf   = stack_buf;
    _arg.stack_size  = stack_size;
    _arg.kstack_size = kstack_size;
    _arg.entry       = entry;
    _arg.flag        = flag;

    if (name == NULL) {
        return -RHINO_INV_PARAM;
    }
    if (strcmp(name, "posix_thread") != 0) {
        /* Prepare ptcb for native user task in case that call pthread_self. */
        ptcb = umm_alloc(PTHREAD_TCB_SIZE);
        if (ptcb == NULL) {
            return -RHINO_NO_MEM;
        }
        memset(ptcb, 0, sizeof(PTHREAD_TCB_SIZE));
    }
    ret = (kstat_t)SYSCALL(SYS_KRHINO_UTASK_CREATE, (void *)&_arg);
    if (ret != RHINO_SUCCESS) {
        umm_free(ptcb);
        return ret;
    }
    return krhino_task_ptcb_set(*task, ptcb);
}

kstat_t krhino_utask_del(ktask_t *task)
{
    void *ptcb = NULL;
    krhino_utask_del_syscall_arg_t _arg;

    /* Free the ptcb of user space in case that it's native task but called pthread_self. */
    krhino_task_ptcb_get(task, &ptcb);
    if (ptcb != NULL) {
        umm_free(ptcb);
        krhino_task_ptcb_set(task, NULL);
    }

    _arg.task = task;

#if (RHINO_CONFIG_NEWLIBC_REENT > 0)
    struct utask_couple_data *data;
    data = krhino_utask_couple_data();

    _reclaim_reent(&(data->newlibc_reent));
#endif


    return (kstat_t)SYSCALL(SYS_KRHINO_UTASK_DEL, (void *)&_arg);
}

void *krhino_utask_couple_data(void)
{
    return (void *)SYSCALL(SYS_KRHINO_UTASK_COUPLE_DATA, NULL);
}

/* ------------------- sched ------------------- */
kstat_t krhino_sched_policy_set(ktask_t *task, uint8_t policy, uint8_t pri)
{
    krhino_sched_policy_set_syscall_arg_t arg;

    arg.task   = task;
    arg.policy = policy;
    arg.pri    = pri;

    return (kstat_t)SYSCALL(SYS_KRHINO_SCHED_POLICY_SET, &arg);
}
kstat_t krhino_sched_policy_get(ktask_t *task, uint8_t *policy)
{
    krhino_sched_policy_get_syscall_arg_t arg;

    arg.task   = task;
    arg.policy = policy;

    return (kstat_t)SYSCALL(SYS_KRHINO_SCHED_POLICY_GET, &arg);
}

int krhino_sched_get_priority_max(int policy)
{
    krhino_sched_get_priority_syscall_arg_t arg;

    arg.policy = policy;

    return (int)SYSCALL(SYS_KRHINO_SCHED_GET_PRIORITY_MAX, &arg);
}

int krhino_sched_get_priority_min(int policy)
{
    krhino_sched_get_priority_syscall_arg_t arg;

    arg.policy = policy;

    return (int)SYSCALL(SYS_KRHINO_SCHED_GET_PRIORITY_MIN, &arg);
}

/* ------------------- process ------------------- */
kstat_t krhino_uprocess_create(const name_t *app_name, const name_t *name, void *arg,
                               uint8_t prio, tick_t ticks,
                               cpu_stack_t *stack_buf, size_t stack_size,
                               size_t kstack_size, task_entry_t entry,
                               void *ret_entry, uint32_t pid, uint8_t autorun)
{
    krhino_uprocess_create_syscall_arg_t _arg;

    _arg.app_name    = app_name;
    _arg.name        = name;
    _arg.arg         = arg;
    _arg.prio        = prio;
    _arg.ticks       = ticks;
    _arg.stack_buf   = stack_buf;
    _arg.stack_size  = stack_size;
    _arg.kstack_size = kstack_size;
    _arg.entry       = entry;
    _arg.ret_entry   = ret_entry;
    _arg.pid         = pid;
    _arg.autorun     = autorun;

    return (kstat_t)SYSCALL(SYS_KRHINO_UPROCESS_CREATE, (void *)&_arg);
}

RHINO_WEAK int proc_map_id_remove(int pid)
{
    return 0;
}

void krhino_uprocess_exit(void)
{
    SYSCALL(SYS_KRHINO_UPROCESS_EXIT, NULL);
}

int krhino_process_load(const name_t *app_name)
{
    krhino_process_load_syscall_arg_t _arg;
    _arg.name = app_name;
    return SYSCALL(SYS_KRHINO_UPROCESS_LOAD, (void *)&_arg);
}

int krhino_process_ramload(const char *ram_addr)
{
    krhino_process_ramload_syscall_arg_t _arg;
    _arg.ram_addr = ram_addr;
    _arg.exit_pm_fun = (void *)proc_map_id_remove;
    return SYSCALL(SYS_KRHINO_UPROCESS_RAMLOAD, (void *)&_arg);
}

int krhino_process_unload(int32_t pid)
{
    krhino_process_unload_syscall_arg_t _arg;
    _arg.pid = pid;

    return SYSCALL(SYS_KRHINO_UPROCESS_UNLOAD, (void *)&_arg);
}

uint32_t krhino_uprocess_id_get(void)
{
    return (uint32_t)SYSCALL(SYS_KRHINO_UPROCESS_ID_GET, NULL);
}

/* ----------------- proc msg ----------------- */
size_t krhino_msg_get(uint32_t key, uint32_t flg, size_t size)
{
    krhino_msg_get_syscall_arg_t _arg;

    _arg.key  = key;
    _arg.flg  = flg;
    _arg.size = size;

    return (size_t)SYSCALL(SYS_KRHINO_MSG_GET, (void *)&_arg);
}

kstat_t krhino_msg_snd(size_t msg_id, void *msg, size_t msg_sz)
{
    krhino_msg_snd_syscall_arg_t _arg;

    _arg.msg_id = msg_id;
    _arg.msg    = msg;
    _arg.msg_sz = msg_sz;

    return (kstat_t)SYSCALL(SYS_KRHINO_MSG_SND, (void *)&_arg);
}

kstat_t krhino_msg_recv(size_t msg_id, tick_t ticks, void *msg, size_t *msg_sz)
{
    krhino_msg_recv_syscall_arg_t _arg;

    _arg.msg_id = msg_id;
    _arg.ticks  = ticks;
    _arg.msg    = msg;
    _arg.msg_sz = msg_sz;

    return (kstat_t)SYSCALL(SYS_KRHINO_MSG_RECV, (void *)&_arg);
}

kstat_t krhino_msg_del(size_t msq_id)
{
    krhino_msg_del_syscall_arg_t _arg;

    _arg.msq_id = msq_id;

    return (kstat_t)SYSCALL(SYS_KRHINO_MSG_DEL, (void *)&_arg);
}

kstat_t aos_ipc_create_stream(const char *name, size_t buf_len, int *obj_handle)
{
    krhino_ipc_stream_arg_t _arg;

    _arg.name = name;
    _arg.buf_len = buf_len;
    _arg.obj_handle = obj_handle;

    return (kstat_t)SYSCALL(SYS_KRHINO_IPC_STREAM, (void *)&_arg);
}

kstat_t aos_ipc_create_msg(const char *name, size_t msg_count, int *obj_handle)
{
    krhino_ipc_msg_arg_t _arg;

    _arg.name = name;
    _arg.msg_count = msg_count;
    _arg.obj_handle = obj_handle;

    return (kstat_t)SYSCALL(SYS_KRHINO_IPC_MSG, (void *)&_arg);
}

kstat_t aos_ipc_create_fifo(const char *name, size_t elem_size,
                            size_t elem_count, int *obj_handle)
{
    krhino_ipc_fifo_arg_t _arg;

    _arg.name = name;
    _arg.elem_size = elem_size;
    _arg.elem_count = elem_count;
    _arg.obj_handle = obj_handle;

    return (kstat_t)SYSCALL(SYS_KRHINO_IPC_FIFO, (void *)&_arg);
}

kstat_t aos_ipc_send(int obj_handle, void *data, size_t dlen, tick_t ticks)
{
    krhino_ipc_send_arg_t _arg;

    _arg.obj_handle = obj_handle;
    _arg.data = data;
    _arg.dlen = dlen;
    _arg.ticks = ticks;

    return (kstat_t)SYSCALL(SYS_KRHINO_IPC_SEND, (void *)&_arg);
}

kstat_t aos_ipc_sendto(int obj_handle, void *data, size_t dlen, tick_t ticks)
{
    krhino_ipc_send_arg_t _arg;

    _arg.obj_handle = obj_handle;
    _arg.data = data;
    _arg.dlen = dlen;
    _arg.ticks = ticks;

    return (kstat_t)SYSCALL(SYS_KRHINO_IPC_SENDTO, (void *)&_arg);
}

kstat_t aos_ipc_send_owner(int obj_handle, void *data, size_t dlen, tick_t ticks)
{
    krhino_ipc_send_arg_t _arg;

    _arg.obj_handle = obj_handle;
    _arg.data = data;
    _arg.dlen = dlen;
    _arg.ticks = ticks;

    return (kstat_t)SYSCALL(SYS_KRHINO_IPC_SEND_OWNER, (void *)&_arg);
}

kstat_t aos_ipc_setopt(int obj_handle, int optyp, void *val)
{
    krhino_ipc_opt_arg_t _arg;

    _arg.obj_handle = obj_handle;
    _arg.optyp = optyp;
    _arg.val = val;

    return (kstat_t)SYSCALL(SYS_KRHINO_IPC_SETOPT, (void *)&_arg);
}

kstat_t aos_ipc_getopt(int obj_handle, int optyp, void *val)
{
    krhino_ipc_opt_arg_t _arg;

    _arg.obj_handle = obj_handle;
    _arg.optyp = optyp;
    _arg.val = val;

    return (kstat_t)SYSCALL(SYS_KRHINO_IPC_GETOPT, (void *)&_arg);
}

kstat_t aos_ipc_recv(int obj_handle, void *data, size_t *dlen, tick_t ticks)
{
    krhino_ipc_recv_arg_t _arg;

    _arg.obj_handle = obj_handle;
    _arg.data = data;
    _arg.dlen = dlen;
    _arg.ticks = ticks;

    return (kstat_t)SYSCALL(SYS_KRHINO_IPC_RECV, (void *)&_arg);
}

tick_t aos_ipc_cycle()
{
    return (tick_t)SYSCALL(SYS_KRHINO_IPC_CYCLE, NULL);
}

kstat_t aos_ipc_del(int obj_handle)
{
    krhino_ipc_del_arg_t _arg;

    _arg.obj_handle = obj_handle;

    return (kstat_t)SYSCALL(SYS_KRHINO_IPC_DEL, (void *)&_arg);
}

kstat_t aos_ipc_perf()
{
    return (kstat_t)SYSCALL(SYS_KRHINO_IPC_PERF, NULL);
}
