/**
 * @file k_hook.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_HOOK_H
#define K_HOOK_H

/** @addtogroup aos_rhino hook
 *  OS hook, called when specific OS operations
 *
 *  @{
 */

#if (RHINO_CONFIG_USER_HOOK > 0)

/**
 * Called in 'krhino_init' - initialization process
 *
 * @return none
 */
void krhino_init_hook(void);

/**
 * Called in 'krhino_start' - just before the first task runs
 *
 * @return none
 */
void krhino_start_hook(void);

/**
 * Called in 'krhino_task_create' and 'krhino_task_dyn_create'
 *
 * @param[in]  task  pointer to the created task
 *
 * @return none
 */
void krhino_task_create_hook(ktask_t *task);

/**
 * Called in 'krhino_task_del_hook' and 'krhino_task_dyn_del'
 *
 * @param[in]  task  pointer to the deleted task
 * @param[in]  arg   useless now
 *
 * @return none
 */
void krhino_task_del_hook(ktask_t *task, res_free_t *arg);

/**
 * Called in 'krhino_task_wait_abort' and 'krhino_task_cancel'
 *
 * @param[in]  task  pointer to the canceled and abort task
 *
 * @return none
 */
void krhino_task_abort_hook(ktask_t *task);

/**
 * Called when task switching
 *
 * @param[in]  orgin  pointer to the switch out task
 * @param[in]  dest   pointer to the switch in task
 *
 * @return none
 */
void krhino_task_switch_hook(ktask_t *orgin, ktask_t *dest);

/**
 * Called in every system tick process
 *
 * @return none
 */
void krhino_tick_hook(void);

/**
 * Called in idle task loop
 *
 * @return none
 */
void krhino_idle_hook(void);

/**
 * Called before idle task loop
 *
 * @return none
 */
void krhino_idle_pre_hook(void);

/**
 * Called in 'krhino_mm_alloc'
 *
 * @param[in]  mem      alloced memory block
 * @param[in]  size     alloced size
 *
 * @return none
 */
void krhino_mm_alloc_hook(void *mem, size_t size);
#endif

/** @} */

#endif /* K_HOOK_H */

