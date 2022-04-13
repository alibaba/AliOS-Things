/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _VFS_WRAPPER_H_
#define _VFS_WRAPPER_H_

int vfs_wrapper_path_register(struct u_dev_node *node);
int vfs_wrapper_path_unregister (struct u_dev_node *node);

#endif //_VFS_WRAPPER_H_
