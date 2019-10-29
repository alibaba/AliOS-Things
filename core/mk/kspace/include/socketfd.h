/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef SOCKET_FD_H
#define SOCKET_FD_H

#include "task_group.h"

#ifdef __cplusplus
extern "C" {
#endif

/* when process exits, close the fds that are not closed yet */
int socket_fd_close(int pid);

/* decrese the ref_cnt of the fd */
int socket_fd_dec_ref(int pid, int fd);

/* increase the ref_cnt of the fd */
int socket_fd_inc_ref(int pid, int fd);

int socket_init(task_group_t *group);

void socket_exit(task_group_t *group);

#ifdef __cplusplus
}
#endif

#endif /* SOCKET_FD_H */

