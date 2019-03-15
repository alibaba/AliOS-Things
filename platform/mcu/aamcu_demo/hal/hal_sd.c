/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/hal/sd.h"

#ifdef HAL_SD_MODULE_ENABLED

int32_t hal_sd_init(sd_dev_t *sd)
{

}

int32_t hal_sd_blks_read(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                         uint32_t blks, uint32_t timeout)
{

}

int32_t hal_sd_blks_write(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                          uint32_t blks, uint32_t timeout)
{

}

int32_t hal_sd_erase(sd_dev_t *sd, uint32_t blk_start_addr, uint32_t blk_end_addr)
{

}

int32_t hal_sd_stat_get(sd_dev_t *sd, hal_sd_stat *stat)
{

}


int32_t hal_sd_info_get(sd_dev_t *sd, hal_sd_info_t *info)
{

}

int32_t hal_sd_finalize(sd_dev_t *sd)
{

}

#endif /* HAL_SD_MODULE_ENABLED */

