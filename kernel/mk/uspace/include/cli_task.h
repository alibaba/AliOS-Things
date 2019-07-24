/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef CLI_TASK_H
#define CLI_TASK_H

int ucli_task_start(size_t kstack_size, size_t ustack_size, uint8_t prio);

#endif /* CLI_TASK_H */

