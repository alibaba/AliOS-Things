/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef FS_FAT_H
#define FS_FAT_H

#ifdef __cplusplus
 extern "C" {
#endif
 
int fatfs_register(void);
int fatfs_unregister(void);
 
#ifdef __cplusplus
 }
#endif

 
#endif
