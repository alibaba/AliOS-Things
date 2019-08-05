/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef RES_TASK_H
#define RES_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

int res_task_start(size_t kstack_size, size_t ustack_size, uint8_t prio);

#ifdef __cplusplus
}
#endif

#endif /* RES_TASK_H */
