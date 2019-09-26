/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef U_TASK_H
#define U_TASK_H

#include "k_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/* user resource ID */
#define USER_RES_REQUEST_BUF_QUEUE 0x00UL
#define USER_RES_REPLY_BUF_QUEUE   0x01UL
#define USER_CLI_BUF_QUEUE         0x02UL
#define USER_CB_CALL_BUF_QUEUE     0x03UL
#define USER_CB_RET_BUF_QUEUE      0x04UL

kstat_t krhino_utask_create(ktask_t **task, const name_t *name, void *arg,
                            uint8_t pri, tick_t ticks, cpu_stack_t *ustack_buf,
                            size_t ustack, size_t kstack, task_entry_t entry,
                            uint8_t autorun);

kstat_t krhino_utask_del(ktask_t *task);

kstat_t krhino_uprocess_create(const name_t *process_name, ktask_t **task,
                               const name_t *name, void *arg,
                               uint8_t pri, tick_t ticks, cpu_stack_t *ustack_buf,
                               size_t ustack, size_t kstack, task_entry_t entry,
                               uint32_t pid, uint8_t autorun);

/* the function only should be called in user space */
kstat_t krhino_utask_dyn_create(ktask_t **task, const name_t *name, void *arg,
                                uint8_t pri, tick_t ticks ,size_t ustack,
                                size_t kstack, task_entry_t entry, uint8_t autorun);

void krhino_utask_cancel_check(void);

int krhino_uprocess_kill(int pid);

void krhino_uprocess_exit(void);

int krhino_uprocess_is_try_exit(void);

void krhino_uprocess_try_exit(void);

void krhino_uprocess_res_get(int32_t id, void **res);

kstat_t krhino_utask_stack_min_free(ktask_t *task, size_t *free);

#ifdef __cplusplus
}
#endif

#endif /* U_TASK_H */

