/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef XZ_HAL_PLAT_H
#define XZ_HAL_PLAT_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "aos/hal/flash.h"

#define SECTOR_SIZE 0x1000    /* flash sector size */
#ifndef XZ_BUF_SIZE
#define XZ_BUF_SIZE 2048
#endif

/* flash APIs */
int xz_flash_erase(hal_partition_t flash_partition, unsigned int offset, unsigned int len);
int xz_flash_read_data(hal_partition_t flash_partition, unsigned int offset, unsigned char *buf, unsigned int len);
int xz_flash_write_data(hal_partition_t flash_partition, unsigned int offset, unsigned char *buf, unsigned int len);

#endif /*XZ_HAL_PLAT_H */
