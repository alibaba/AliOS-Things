/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_FLASH_KSYSCALL_H
#define HAL_FLASH_KSYSCALL_H

#include "aos/hal/flash.h"

hal_logic_partition_t* sys_hal_flash_copy_info_stub(void *arg);

int32_t sys_hal_flash_erase_stub(void *arg);

int32_t sys_hal_flash_write_stub(void *arg);

int32_t sys_hal_flash_erase_write_stub(void *arg);

int32_t sys_hal_flash_read_stub(void *arg);

int32_t sys_hal_flash_enable_secure_stub(void *arg);

int32_t sys_hal_flash_dis_secure_stub(void *arg);

#endif /* HAL_FLASH_KSYSCALL_H */

