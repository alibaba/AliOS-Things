/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

void hal_loop_task_setspecific(unsigned int key, void *ctx);
void *hal_loop_task_getspecific(unsigned int key);
int hal_loop_task_key_create(unsigned int *key);
void hal_loop_task_key_delete(unsigned int key);
