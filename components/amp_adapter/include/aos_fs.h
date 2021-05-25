/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef AMP_FS_H
#define AMP_FS_H

#include <fcntl.h>
#include "aos_platform.h"

#ifdef AOS_COMP_VFS
#include <aos/vfs.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief aos_fs_init() initializes vfs system.
 *
 * @param[in] NULL
 *
 * @return  On success, return new file descriptor.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
int aos_fs_init();


/**
 * @brief get file system total size in bytes;
 *
 * @return      fs total size in bytes; negative if failed.
 */
int aos_fs_totalsize(void);

/**
 * @brief get file system used size in bytes;
 *
 * @return      fs used size in bytes; negative if failed.
 */
int aos_fs_usedsize(void);

/**
 * @brief get file system free size in bytes;
 *
 * @return      fs free size in bytes; negative if failed.
 */
int aos_fs_freesize(void);

/**
 * @brief       kv componment(key-value) initialize
 *
 * @return      0: success, -1: failed
 */
int aos_fs_type(unsigned int mode);

int aos_rmdir_r(const char *path);

#if defined(__cplusplus)
}
#endif
#endif /* AMP_FS_H */
