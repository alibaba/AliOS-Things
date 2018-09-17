/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_JFFS2_H
#define AOS_JFFS2_H

#ifdef __cplusplus
 extern "C" {
#endif
 
int vfs_jffs2_register(uint32_t start_addr, uint32_t length);
int vfs_jffs2_unregister(void);
 
#ifdef __cplusplus
 }
#endif

 
#endif

