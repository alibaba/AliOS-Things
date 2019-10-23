/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef FSFD_H
#define FSFD_H

#include "task_group.h"

#ifdef __cplusplus
extern "C" {
#endif

/* when process exits, close the fds that are not closed yet */
int fsfd_close(int pid);

/* decrese the ref_cnt of the fd */
int fsfd_ref(int pid, int fd);

/* increase the ref_cnt of the fd */
int fsfd_deref(int pid, int fd);

int fsfd_init(task_group_t *group);

void fsfd_exit(task_group_t *group);

#ifdef __cplusplus
}
#endif

#endif /* FSFD_H */

