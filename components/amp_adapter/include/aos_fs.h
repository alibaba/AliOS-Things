/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef AMP_FS_H
#define AMP_FS_H

#include <fcntl.h>
#include "amp_platform.h"

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

int aos_rmdir_r(const char *path);

#if defined(__cplusplus)
}
#endif
#endif /* AMP_FS_H */
