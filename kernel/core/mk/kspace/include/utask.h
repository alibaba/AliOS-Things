/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef U_TASK_H
#define U_TASK_H

#if (RHINO_CONFIG_NEWLIBC_REENT > 0)
    #include <reent.h>
#endif
#include "k_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/* user stack is allocated dynamically */
#define USTACK_DYN_ALLOC     (1u<<2)

/* task auto run after it's created */
#define UTASK_AUTORUN       (1u)

struct utask_couple_data {
#if (RHINO_CONFIG_NEWLIBC_REENT > 0)
    /* use newlibc reent */
    struct _reent newlibc_reent;
#else
    int _errno;
#endif
};

kstat_t krhino_utask_create(ktask_t **task, const name_t *name, void *arg,
                            uint8_t pri, tick_t ticks, cpu_stack_t *ustack_buf,
                            size_t ustack, size_t kstack, task_entry_t entry,
                            uint8_t flag);

kstat_t krhino_utask_del(ktask_t *task);

kstat_t krhino_uprocess_create(const name_t *process_name, const name_t *name, void *arg,
                               uint8_t pri, tick_t ticks, cpu_stack_t *ustack_buf,
                               size_t ustack, size_t kstack, task_entry_t entry,
                               void *ret_entry, uint32_t pid, uint8_t autorun);

/* the function only should be called in user space */
kstat_t krhino_utask_dyn_create(ktask_t **task, const name_t *name, void *arg,
                                uint8_t pri, tick_t ticks, size_t ustack,
                                size_t kstack, task_entry_t entry, uint8_t autorun);

/* Interface same as kernel. User and kernel stack size are the same .*/
kstat_t krhino_task_dyn_create(ktask_t **task, const name_t *name, void *arg,
                               uint8_t pri, tick_t ticks, size_t stack,
                               task_entry_t entry, uint8_t autorun);

void krhino_utask_deathbed(void);

void krhino_utask_cancel_check(void);

void *krhino_utask_couple_data(void);

int krhino_uprocess_kill(int pid);

void krhino_uprocess_exit(void);

int krhino_uprocess_is_try_exit(void);

void krhino_uprocess_try_exit(void);

uint32_t krhino_uprocess_id_get(void);

uint32_t krhino_task_id_get(void);
kstat_t krhino_task_name_set(ktask_t *task, const char *name);

kstat_t krhino_task_name_get(ktask_t *task, const char **name);

tick_t krhino_sched_time_slice_get(ktask_t *task);

kstat_t krhino_utask_stack_min_free(ktask_t *task, size_t *free);

int32_t krhino_uprocess_load(const char *file, const char *name);

void krhino_set_pm_exit_fun(void * exit_fun);

void krhino_pm_callback_free_pid(uint32_t pid);

int krhino_process_unload(int32_t pid);

#ifdef __cplusplus
}
#endif

#endif /* U_TASK_H */

