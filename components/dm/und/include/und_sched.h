/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UND_SCHED_H
#define UND_SCHED_H

#ifdef __cplusplus
extern "C" {
#endif

int und_sched_init(void *sched_task);

int und_sched_deinit();

int und_sched_start(int cycle_ms);

#ifdef __cplusplus
}
#endif

#endif  /* end of UND_SCHED_H */
