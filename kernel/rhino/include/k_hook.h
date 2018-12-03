/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_HOOK_H
#define K_HOOK_H

#if (RHINO_CONFIG_USER_HOOK > 0)
/**
 * This function will provide init hook
 */
void krhino_init_hook(void);

/**
 * This function will provide system start hook
 */
void krhino_start_hook(void);

/**
 * This function will provide task create hook
 * @param[in]  task  pointer to the task
 */
void krhino_task_create_hook(ktask_t *task);

/**
 * This function will provide task delete hook
 * @param[in]  task  pointer to the task
 */
void krhino_task_del_hook(ktask_t *task, res_free_t *arg);

/**
 * This function will provide task abort hook
 * @param[in]  task  pointer to the task
 */
void krhino_task_abort_hook(ktask_t *task);

/**
 * This function will provide task switch hook
 */
void krhino_task_switch_hook(ktask_t *orgin, ktask_t *dest);

/**
 * This function will provide system tick hook
 */
void krhino_tick_hook(void);

/**
 * This function will provide idle hook
 */
void krhino_idle_hook(void);

/**
 * This function will provide idle pre hook
 */
void krhino_idle_pre_hook(void);

/**
 * This function will provide  krhino_mm_alloc hook
 */
void krhino_mm_alloc_hook(void *mem, size_t size);
#endif

#endif /* K_HOOK_H */

