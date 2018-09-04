/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_VFS_CONFIG_H
#define AOS_VFS_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif
#define VFS_FALSE 0u
#define VFS_TRUE 1u

#define AOS_CONFIG_VFS_DEV_NODES 25
/*mem 1000 byte*/
#define AOS_CONFIG_VFS_DEV_MEM 2000
#define AOS_CONFIG_VFS_POLL_SUPPORT 1
#define AOS_CONFIG_VFS_FD_OFFSET 512

#define MAX_FILE_NUM (AOS_CONFIG_VFS_DEV_NODES * 2)

#ifdef _WIN32
#define CHECK_IF_NON_VFS_FD(_SOCKET)         \
    ((_SOCKET < AOS_CONFIG_VFS_FD_OFFSET) || \
     (_SOCKET > (AOS_CONFIG_VFS_FD_OFFSET + AOS_CONFIG_VFS_DEV_NODES)))
#endif

#ifdef __cplusplus
}
#endif

#endif
