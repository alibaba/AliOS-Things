/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef RAMFS_ERR_H
#define RAMFS_ERR_H

#define RAMFS_OK              0
#define RAMFS_ERR_HW         -10000 /* Low level hard ware error */
#define RAMFS_ERR_FS         -10001 /* Error in the file system structure */
#define RAMFS_ERR_NOT_EXIST  -10002 /* File or directory is not exists */
#define RAMFS_ERR_FULL       -10003 /* File system is full */
#define RAMFS_ERR_LOCKED     -10004 /* The file is already opened */
#define RAMFS_ERR_DENIED     -10005 /* Access denied */
#define RAMFS_ERR_BUSY       -10006 /* The file system can't handle it, try later */
#define RAMFS_ERR_TIMEOUT    -10007 /* Process timeout */
#define RAMFS_ERR_NOT_IMP    -10008 /* Request function is not implemented */
#define RAMFS_ERR_OUT_OF_MEM -10009 /* Not enough memory for an interal operation */
#define RAMFS_ERR_INV_PARAM  -10010 /* Invalid parameter among arguments */
#define RAMFS_ERR_UNKOWN     -10011 /* Other unknown error */
#define RAMFS_ERR_PATH       -10012 /* Path error */
#define RAMFS_ERR_MALLOC     -10013 /* Malloc error */
#define RAMFS_ERR_LINK_MAX   -10014 /* Exceed RAMFS_LINK_MAX */
#define RAMFS_ERR_EXIST      -10015 /* File or directory is existed */

#endif /* RAMFS_ERR_H */

