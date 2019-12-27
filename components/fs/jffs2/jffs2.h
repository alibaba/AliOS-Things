/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef FS_JFFS2_H
#define FS_JFFS2_H

#ifdef __cplusplus
extern "C" {
#endif

int32_t jffs2_register(uint32_t start_addr, uint32_t length);
int32_t jffs2_unregister(void);

#ifdef __cplusplus
}
#endif

#endif

