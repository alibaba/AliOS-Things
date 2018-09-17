/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_INFOGET_H
#define K_INFOGET_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * This function will get task handle by its name.
     */
    ktask_t *krhino_task_find(char *name);

    /**
     * This function will return true if task is ready.
     */
    int krhino_is_task_ready(ktask_t *task);

    /**
     * This function will get the bottom of task stack
     * @param[in]  task  NULL for active task
     * @return  the bottom of stack
     */
    void *krhino_task_stack_bottom(ktask_t *task);

#ifdef __cplusplus
}
#endif

#endif /* K_INFOGET_H */
