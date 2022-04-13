/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef XZ_EXPORT_H
#define XZ_EXPORT_H
#include "aos/hal/flash.h"

#define XZ_SUCCESS                  0
#define XZ_INPURT_PARAM_ERR        -1
#define XZ_MALLOC_FAILED           -2
#define XZ_READ_FLASH_FAILED       -3
#define XZ_WRITE_FLASH_FAILED      -4
#define XZ_ERASE_FLASH_FAILED      -5
#define XZ_CRC_ERROR               -6
#define XZ_UNPROCESS_FAILED        -7
#define XZ_VERIFY_FAILED           -8
#define XZ_FILE_IDENTIFY_FAILED    -9

int xz_file_uncompress(hal_partition_t des_partition, hal_partition_t src_partition, unsigned int xz_size, unsigned short xz_crc16);
#endif /*XZ_EXPORT_H */
