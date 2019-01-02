/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef RAMFS_ALIOS_H
#define RAMFS_ALIOS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>
#include "vfs_inode.h"
#include "vfs_register.h"
#include "ramfs.h"

#define MAX_RAMFS_FILE_NAME_BYTES 32

    typedef struct
    {
        aos_dir_t    aos_dir;
        ramfs_dir_t  ramfs_dir;
        aos_dirent_t aos_dirent;
    } vfs_ramfs_dir_t;

    /* the root of ramfs is "/ramfs", all files must be stored in this directory
     */
    int ramfs_register(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
