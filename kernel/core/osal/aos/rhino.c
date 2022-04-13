/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <time.h>
#include <sys/prctl.h>
#include <pthread.h>
#include "debug_api.h"
#include "utask.h"
#include <aos/hal/uart.h>
#ifdef AOS_COMP_CLI
#include "aos/cli.h"
#endif

#if (AOS_KERNEL_OBJECT > 0)
    #include "object_api.h"
#endif

extern const char *sysinfo_kernel_version;

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC == 0)
    #warning "RHINO_CONFIG_KOBJ_DYN_ALLOC is disabled!"
#endif

#define MS2TICK(ms) krhino_ms_to_ticks(ms)

static uint32_t used_bitmap;
static uint64_t start_time_ms = 0;

#ifndef DEFAULT_UTC_OFFSET_SEC
    #define DEFAULT_UTC_OFFSET_SEC (8 * 3600)
#endif

static uint64_t utc_offset_ms = (DEFAULT_UTC_OFFSET_SEC * 1000);

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
int aos_task_new(const char *name, void (*fn)(void *), void *arg,
                 size_t stack_size)
{
    int ret;

    ktask_t *task_handle = NULL;

#if (RHINO_CONFIG_SCHED_CFS > 0)
    ret =
                (int)krhino_cfs_task_dyn_create(&task_handle, name, arg, AOS_DEFAULT_APP_PRI,
                        stack_size / sizeof(cpu_stack_t), fn, 1u);
#else
    ret =
                (int)krhino_task_dyn_create(&task_handle, name, arg, AOS_DEFAULT_APP_PRI,
                                            0, stack_size / sizeof(cpu_stack_t), fn, 1u);
#endif
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    return ret;
}

int aos_task_new_ext(aos_hdl_t *task, const char *name, void (*fn)(void *),
                     void *arg, size_t stack_size, int prio, uint32_t flags)
{
    int ret;

#if (RHINO_CONFIG_SCHED_CFS > 0)
    ret =
                (int)krhino_cfs_task_dyn_create((ktask_t **)(&(task->hdl)), name, arg, AOS_DEFAULT_APP_PRI,
                        stack_size / sizeof(cpu_stack_t), fn, flags);
#else
    ret =
                (int)krhino_task_dyn_create((ktask_t **)(&(task->hdl)), name, arg, prio,
                                            0, stack_size / sizeof(cpu_stack_t), fn, flags);
#endif
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    return ret;
}

int aos_task_resume(aos_hdl_t *task)
{
    int ret;
    ret = krhino_task_resume(task->hdl);
    if (ret != 0) {
        return -1;
    }
    return 0;
}

void aos_task_exit(int code)
{
    (void)code;

    krhino_task_dyn_del(NULL);
}

int aos_task_delete(aos_hdl_t *task)
{
    int ret;
    if (task == NULL) {
        return -1;
    }

    ret = (int)krhino_utask_del(task->hdl);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    return ret;
}

int aos_task_yield(void)
{
    return krhino_task_yield();
}


int aos_task_name_get(aos_hdl_t *task, const char **name)
{
    if (task == NULL || task->hdl == NULL) {
        return -1;
    }

    *name = ((ktask_t *)(task->hdl))->task_name;
    return 0;
}
#endif

void *aos_task_now(void)
{
    return krhino_cur_task_get();
}

const char *aos_task_name(void)
{
    return krhino_cur_task_get()->task_name;
}

int aos_task_key_create(aos_task_key_t *key)
{
    int i;

    for (i = RHINO_CONFIG_TASK_INFO_NUM - 1; i >= 0; i--) {
        if (!((1 << i) & used_bitmap)) {
            used_bitmap |= 1 << i;
            *key = i;

            return 0;
        }
    }

    return -EINVAL;
}

void aos_task_key_delete(aos_task_key_t key)
{
    if (key >= RHINO_CONFIG_TASK_INFO_NUM) {
        return;
    }

    used_bitmap &= ~(1 << key);
}

#if (RHINO_CONFIG_TASK_INFO > 0)
int aos_task_setspecific(aos_task_key_t key, void *vp)
{
    int ret;
    ret = krhino_task_info_set(krhino_cur_task_get(), key, vp);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    return ret;
}

void *aos_task_getspecific(aos_task_key_t key)
{
    void *vp = NULL;

    krhino_task_info_get(krhino_cur_task_get(), key, &vp);

    return vp;
}
int aos_task_info_set(aos_hdl_t *task, size_t idx, void *info)
{
    if (task == NULL) {
        return -1;
    }

    return krhino_task_info_set(task->hdl, idx, info);
}

int aos_task_info_get(aos_hdl_t *task, size_t idx, void **info)
{
    if ((task == NULL) || (info == NULL)) {
        return -1;
    }

    return krhino_task_info_get(task->hdl, idx, info);
}
#endif

int aos_task_ptcb_get(aos_hdl_t *task, void **ptcb)
{
    ktask_t *cur_task = NULL;
    if ((task == NULL) || (ptcb == NULL)) {
        return -1;
    }

    if (task->hdl == NULL) {
        cur_task = krhino_cur_task_get();
        *(ptcb) = cur_task->ptcb;
        task->hdl = cur_task;
    } else {
        *(ptcb) = ((ktask_t *)(task->hdl))->ptcb;
    }

    return 0;
}

int aos_task_ptcb_set(aos_hdl_t *task, void *ptcb)
{
    if (task == NULL || task->hdl == NULL) {
        return -1;
    }

    ((ktask_t *)(task->hdl))->ptcb = ptcb;
    return 0;
}

int aos_task_pri_change(aos_hdl_t *task, uint8_t pri, uint8_t *old_pri)
{
    if (task == NULL) {
        return -1;
    }

    return krhino_task_pri_change(task->hdl, pri, old_pri);
}

int aos_task_pri_get(aos_hdl_t *task, uint8_t *priority)
{
    if ((task == NULL) || (priority == NULL)) {
        return -1;
    }

    *priority = ((ktask_t *)(task->hdl))->b_prio;

    return 0;
}

int aos_task_sched_policy_set(aos_hdl_t *task, uint8_t policy, uint8_t pri)
{
    if (task == NULL) {
        return -1;
    }

    if (policy != KSCHED_FIFO && policy != KSCHED_RR && policy != KSCHED_CFS) {
        return -1;
    }

    return krhino_sched_policy_set(task->hdl, policy, pri);
}

int aos_task_sched_policy_get(aos_hdl_t *task, uint8_t *policy)
{
    int ret = 0;

    if ((task == NULL) || (policy == NULL)) {
        return -1;
    }

    ret = krhino_sched_policy_get(task->hdl, policy);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int aos_task_sched_policy_get_default()
{
#if (RHINO_CONFIG_SCHED_CFS > 0)
    return KSCHED_CFS;
#else
    return KSCHED_RR;
#endif
}

int aos_task_time_slice_set(aos_hdl_t *task, uint32_t slice)
{
    if (task == NULL) {
        return -1;
    }

    return krhino_task_time_slice_set(task->hdl, MS2TICK(slice));
}

int aos_task_time_slice_get(aos_hdl_t *task, uint32_t *slice)
{
    if ((task == NULL) || (slice == NULL)) {
        return -1;
    }

    *slice = (uint32_t)krhino_ticks_to_ms(((ktask_t *)(task->hdl))->time_slice);
    return 0;
}

int aos_sched_get_priority_max(int policy)
{
    return krhino_sched_get_priority_max(policy);
}

int aos_sched_get_priority_min(int policy)
{
    return krhino_sched_get_priority_min(policy);
}

#if (!AOS_KOBJ_TIMER)
#if (RHINO_CONFIG_TIMER > 0)
int aos_timer_new(aos_hdl_t *timer, void (*fn)(void *, void *), void *arg,
                  uint32_t ms, uint8_t repeat)
{
    kstat_t ret;

    if (timer == NULL) {
        return -EINVAL;
    }

    if (repeat == 0) {
        ret = krhino_timer_dyn_create(((ktimer_t **)(&timer->hdl)), "AOS",
                                      (timer_cb_t)fn, MS2TICK(ms), 0, arg, 1);
    } else {
        ret = krhino_timer_dyn_create(((ktimer_t **)(&timer->hdl)), "AOS",
                                      (timer_cb_t)fn, MS2TICK(ms), MS2TICK(ms),
                                      arg, 1);
    }

    if (ret != RHINO_SUCCESS) {
        return ret;
    }

    return 0;
}

int aos_timer_new_ext(aos_hdl_t *timer, void (*fn)(void *, void *), void *arg,
                      uint32_t ms, uint8_t repeat, uint8_t auto_run)
{
    kstat_t ret;

    if (timer == NULL) {
        return -EINVAL;
    }

    if (repeat == 0) {
        ret = krhino_timer_dyn_create(((ktimer_t **)(&timer->hdl)), "AOS",
                                      (timer_cb_t)fn, MS2TICK(ms), 0, arg,
                                      auto_run);
    } else {
        ret = krhino_timer_dyn_create(((ktimer_t **)(&timer->hdl)), "AOS",
                                      (timer_cb_t)fn, MS2TICK(ms), MS2TICK(ms),
                                      arg, auto_run);
    }

    if (ret != RHINO_SUCCESS) {
        return ret;
    }

    return 0;
}

void aos_timer_free(aos_hdl_t *timer)
{
    if (timer == NULL) {
        return;
    }

    krhino_timer_dyn_del(timer->hdl);
    timer->hdl = NULL;
}

int aos_timer_start(aos_hdl_t *timer)
{
    int ret;

    if (timer == NULL) {
        return -EINVAL;
    }

    ret = krhino_timer_start(timer->hdl);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    return ret;
}

int aos_timer_stop(aos_hdl_t *timer)
{
    int ret;

    if (timer == NULL) {
        return -EINVAL;
    }

    ret = krhino_timer_stop(timer->hdl);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    return ret;
}

int aos_timer_change(aos_hdl_t *timer, uint32_t ms)
{
    int ret;

    if (timer == NULL) {
        return -EINVAL;
    }

    ret = krhino_timer_change(timer->hdl, MS2TICK(ms), MS2TICK(ms));
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    return ret;
}

int aos_timer_change_once(aos_hdl_t *timer, uint32_t ms)
{
    int ret;

    if (timer == NULL) {
        return -EINVAL;
    }

    ret = krhino_timer_change(timer->hdl, MS2TICK(ms), 0);

    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    return ret;
}

int aos_timer_is_valid(aos_hdl_t *timer)
{
    ktimer_t *k_timer;

    if (timer == NULL) {
        return 0;
    }

    k_timer = timer->hdl;
    if (k_timer == NULL) {
        return 0;
    }

    if (k_timer->obj_type != RHINO_TIMER_OBJ_TYPE) {
        return 0;
    }

    return 1;
}

int aos_timer_gettime(aos_hdl_t *timer, uint64_t value[4])
{
    ktimer_t *ktimer = NULL;
    uint64_t init_ms;
    uint64_t round_ms;
    if (timer == NULL) {
        return -EINVAL;
    }
    ktimer = (ktimer_t *)timer->hdl;
    init_ms = (uint64_t)krhino_ticks_to_ms(ktimer->init_count);
    round_ms = (uint64_t)krhino_ticks_to_ms(ktimer->round_ticks);

    value[0] = init_ms / 1000;
    value[1] = (init_ms % 1000) * 1000000UL;
    value[2] = round_ms / 1000;
    value[3] = (round_ms % 1000) * 1000000UL ;

    return 0;
}

#endif
#endif /* AOS_KOBJ_TIMER */

void *aos_zalloc(size_t size)
{
    void *tmp = NULL;

    if (size == 0) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    tmp = krhino_mm_alloc(size | AOS_UNSIGNED_INT_MSB);
    krhino_owner_return_addr(tmp);
#else
    tmp = krhino_mm_alloc(size);
#endif

    if (tmp) {
        memset(tmp, 0, size);
    }

    return tmp;
}

void *aos_malloc(size_t size)
{
    void *tmp = NULL;

    if (size == 0) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    tmp = krhino_mm_alloc(size | AOS_UNSIGNED_INT_MSB);
    krhino_owner_return_addr(tmp);
#else
    tmp = krhino_mm_alloc(size);
#endif

    return tmp;
}

void *aos_calloc(size_t nitems, size_t size)
{
    void *tmp = NULL;
    size_t len = nitems * size;

    if (len == 0) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    tmp = krhino_mm_alloc(len | AOS_UNSIGNED_INT_MSB);
    krhino_owner_return_addr(tmp);
#else
    tmp = krhino_mm_alloc(len);
#endif

    if (tmp) {
        memset(tmp, 0, len);
    }

    return tmp;
}

void *aos_realloc(void *mem, size_t size)
{
    void *tmp = NULL;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    tmp = krhino_mm_realloc(mem, size | AOS_UNSIGNED_INT_MSB);
    krhino_owner_return_addr(tmp);
#else
    tmp = krhino_mm_realloc(mem, size);
#endif

    return tmp;
}

void aos_alloc_trace(void *addr, uintptr_t allocator)
{
#if (RHINO_CONFIG_MM_DEBUG > 0u)
    krhino_owner_attach(addr, allocator);
#endif
}

void aos_free(void *mem)
{
    if (mem == NULL) {
        return;
    }

    krhino_mm_free(mem);
}

void aos_calendar_time_set(uint64_t now_ms)
{
    start_time_ms = now_ms - krhino_sys_time_get();
}

uint64_t aos_calendar_time_get(void)
{
    return krhino_sys_time_get() + start_time_ms;
}

void aos_utc_offset_set(uint64_t offset_ms)
{
    utc_offset_ms = offset_ms;
}

uint64_t aos_utc_offset_get(void)
{
    return utc_offset_ms;
}

uint64_t aos_calendar_localtime_get(void)
{
    return aos_calendar_time_get() + aos_utc_offset_get();
}

uint64_t aos_now(void)
{
    return krhino_sys_time_get() * 1000 * 1000;
}

uint64_t aos_now_ms(void)
{
    return krhino_sys_time_get();
}

char *aos_now_time_str(char *buffer, size_t len)
{
    const uint64_t ms = aos_now_ms();
    if (buffer != NULL && len > 0) {
        time_t actualTime = { ms / 1000 };
        memset(buffer, 0, len);
        strftime(buffer, len, "%m-%d %H:%M:%S", gmtime(&actualTime));
        if ((int)(len - strlen(buffer) - 1) > 0) {
            const uint32_t milli    = ms % 1000;
            char      msStr[8] = {0};
            snprintf(msStr, sizeof(msStr), ".%03d", milli);
            strncat(buffer, msStr, len - strlen(buffer) - 1);
        }
    }

    return buffer;
}

void aos_msleep(uint32_t ms)
{
    krhino_task_sleep(MS2TICK(ms));
}

void aos_init(void)
{
    krhino_init();
}

void aos_start(void)
{
    krhino_start();
}

void aos_abort(void)
{
    k_err_proc(RHINO_SYS_FATAL_ERR);
}

void aos_exit(int status)
{
    /* The status and wait are not supported by kernel. */
    krhino_uprocess_exit();
}

extern pthread_mutex_t g_pthread_lock;
int aos_prctl(int option, ...)
{
    va_list       ap;
    unsigned long arg;
    char         *p_str;
    ktask_t      *p_tcb;
    int           ret = -1;

    if (option == PR_SET_NAME) {
        /* set ap to the start stack address of argument list */
        va_start(ap, option);

        /*
         * checkout the first argument from argument list, and force it
         * to  unsigned long, the ap address will be moved at same time.
         */

        arg = va_arg(ap, unsigned long);

        va_end(ap);

        p_str = (char *)arg;
        if (NULL == p_str) {
            return 1;
        }

        /* get TCB of current task */
        ret = pthread_mutex_lock(&g_pthread_lock);
        if (ret != 0) {
            return -1;
        }

        p_tcb = krhino_cur_task_get();
        p_tcb->task_name = p_str;

        pthread_mutex_unlock(&g_pthread_lock);

        return 0;
    } else {
        return 1;
    }
}

uint32_t aos_getpid(void)
{
    return krhino_uprocess_id_get();
}

uint32_t aos_gettid(void)
{
    return krhino_cur_task_get()->task_id;
}

int aos_do_futex(uint32_t *uaddr, int futex_op, uint32_t val, uint32_t timeout)
{
    tick_t tmp = timeout == AOS_WAIT_FOREVER ? RHINO_WAIT_FOREVER : MS2TICK(timeout);
    int ret = krhino_futex(uaddr, futex_op, val, tmp);
    return ret;
}


int aos_print(void *console, const void *buf, int size)
{
#ifdef AOS_COMP_CLI
    if (console == NULL) {
        /* console is null called by vfs, not null called by usapce prink syscall */
        return aos_cli_console_write(NULL, buf, size);
    }
#endif
    extern int hal_uart_print(const char *buf, int size);
    return hal_uart_print((char *)buf, size);
}