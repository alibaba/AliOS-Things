/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/hal/nor.h"

#ifdef HAL_NOR_MODULE_ENABLED

int32_t hal_nor_init(nor_dev_t *nor)
{

}

int32_t hal_nor_finalize(nor_dev_t *nor)
{

}

int32_t hal_nor_read(nor_dev_t *nor, uint32_t *addr, uint8_t *data, uint32_t len)
{

}

int32_t hal_nor_write(nor_dev_t *nor, uint32_t *addr, uint8_t *data, uint32_t len)
{

}

int32_t hal_nor_erase_block(nor_dev_t *nor, uint32_t *addr, uint32_t block_count)
{

}

int32_t hal_nor_erase_chip(nor_dev_t *nor, uint32_t *addr)
{

}

#endif /* HAL_NOR_MODULE_ENABLED */

