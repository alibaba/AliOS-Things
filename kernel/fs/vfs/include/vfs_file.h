/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_VFS_FILE_H
#define AOS_VFS_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

int get_fd(file_t *file);

file_t *get_file(int fd);

file_t *new_file(inode_t *node);

void del_file(file_t *file);

#ifdef __cplusplus
}
#endif

#endif
