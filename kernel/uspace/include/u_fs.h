/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef U_FS_H
#define U_FS_H

/* when process exits, close the fds that are not closed yet */
int fs_fd_close(int pid);

/* decrese the ref_cnt of the fd */
int fs_fd_dec_ref(int pid, int fd);

/* increase the ref_cnt of the fd */
int fs_fd_inc_ref(int pid, int fd);


#endif /* U_FS_H */

