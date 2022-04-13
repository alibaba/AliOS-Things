/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _IO_WDG_H_
#define _IO_WDG_H_

#include <stdbool.h>

#define IOC_WDG_BASE 'W'
#define IOC_WDG_RELOAD IOC_WDG_BASE + 1 // reload watchdog

int vfs_wdg_drv_init (void);

#endif //_IO_WDG_H_
