/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _IO_RAMDRV_H_
#define _IO_RAMDRV_H_

#include <stdbool.h>

#define IOC_RAMDRV_BASE 'A'
#define IOC_RAMDRV_LOOPBACK IOC_RAMDRV_BASE + 1 // loopback test

#define RAMDRV_MAGIC_NUM 0x55

typedef struct io_ramdrv_arg {
    char rbuf[16];
    char wbuf[16];
} io_ramdrv_arg_t;

int vfs_ramdrv_drv_init (void);
int vfs_ramdrv_drv_deinit (void);

#endif //_IO_RAMDRV_H_
