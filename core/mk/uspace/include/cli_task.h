/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef CLI_TASK_H
#define CLI_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

int ucli_task_start(size_t kstack_size, size_t ustack_size, uint8_t prio);

#ifdef __cplusplus
}
#endif

#endif /* CLI_TASK_H */

