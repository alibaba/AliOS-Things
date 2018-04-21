/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef FS_SPIFFS_H
#define FS_SPIFFS_H

#ifdef __cplusplus
 extern "C" {
#endif
 
int vfs_spiffs_register(void);
int vfs_spiffs_unregister(void);
 
#ifdef __cplusplus
 }
#endif

 
#endif
