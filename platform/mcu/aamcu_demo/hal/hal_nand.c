/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/hal/nand.h"

#ifdef HAL_NAND_MODULE_ENABLED

int32_t hal_nand_init(nand_dev_t *nand)
{

}

int32_t hal_nand_finalize(nand_dev_t *nand)
{

}

int32_t hal_nand_read_page(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t page_count)
{

}

int32_t hal_nand_write_page(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t page_count)
{

}

int32_t hal_nand_read_spare(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t data_len)
{

}

int32_t hal_nand_write_spare(nand_dev_t *nand, nand_addr_t *addr, uint8_t *data, uint32_t data_len)
{

}

int32_t hal_nand_erase_block(nand_dev_t *nand, nand_addr_t *addr)
{

}

#endif /* HAL_NAND_MODULE_ENABLED */

