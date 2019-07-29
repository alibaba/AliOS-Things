/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef FS_FATFS_H
#define FS_FATFS_H

#ifdef __cplusplus
extern "C" {
#endif

int32_t fatfs_register(void);
int32_t fatfs_unregister(void);

#ifdef __cplusplus
}
#endif

#endif

