/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef CALLBACK_TASK_H
#define CALLBACK_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

int cb_task_start(size_t kstack_size, size_t ustack_size, uint8_t prio);

#ifdef __cplusplus
}
#endif

#endif /* CALLBACK_TASK_H */

