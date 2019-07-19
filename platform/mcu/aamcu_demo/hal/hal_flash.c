/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifdef HAL_FLASH_MODULE_ENABLED

#include "aos/hal/flash.h"

int32_t hal_flash_info_get(hal_partition_t pno, hal_logic_partition_t *partition)
{

}

int32_t hal_flash_write(hal_partition_t pno, uint32_t *poff, const void *buf, uint32_t buf_size)
{

}

int32_t hal_flash_read(hal_partition_t pno, uint32_t *poff, void *buf, uint32_t buf_size)
{

}

int32_t hal_flash_erase(hal_partition_t pno, uint32_t off_set, uint32_t size)
{

}

#endif /* HAL_FLASH_MODULE_ENABLED */

