/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef DEBUG_INFOGET_H
#define DEBUG_INFOGET_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function will get task handle by its name.
 */
ktask_t *debug_task_find(char *name);

/**
 * This function will return true if task is running.
 */
int debug_task_is_running(ktask_t *task);

/**
 * This function will get the bottom of task stack
 * @param[in]  task  NULL for active task
 * @return  the bottom of stack
 */
void *debug_task_stack_bottom(ktask_t *task);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_INFOGET_H */
