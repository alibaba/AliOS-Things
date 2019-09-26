/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef FS_FD_H
#define FS_FD_H

#include "task_group.h"

#ifdef __cplusplus
extern "C" {
#endif

/* when process exits, close the fds that are not closed yet */
int fs_fd_close(int pid);

/* decrese the ref_cnt of the fd */
int fs_fd_dec_ref(int pid, int fd);

/* increase the ref_cnt of the fd */
int fs_fd_inc_ref(int pid, int fd);

int fs_init(task_group_t *group);

void fs_destory(task_group_t *group);

#ifdef __cplusplus
}
#endif

#endif /* FS_FD_H */

