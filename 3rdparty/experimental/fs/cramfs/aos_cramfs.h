/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_CRAMFS_H
#define AOS_CRAMFS_H

#ifdef __cplusplus
 extern "C" {
#endif
 
int vfs_cramfs_register(uint32_t start_addr, uint32_t length);
int vfs_cramfs_unregister(void);
 
#ifdef __cplusplus
 }
#endif

 
#endif

