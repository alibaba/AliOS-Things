/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef TASK_H
#define TASK_H

typedef void (*task_func_t)(void *arg);

typedef unsigned long task_t;

int task_create(task_t *task, size_t stack_szie, task_func_t entry, void *arg);

int task_exit(void);

#endif /* TASK_H */
