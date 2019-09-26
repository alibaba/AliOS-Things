/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_SD_KSYSCALL_H
#define HAL_SD_KSYSCALL_H

int32_t sys_hal_sd_init_stub(void *arg);

int32_t sys_hal_sd_blks_read_stub(void *arg);

int32_t sys_hal_sd_blks_write_stub(void *arg);

int32_t sys_hal_sd_erase_stub(void *arg);

int32_t sys_hal_sd_stat_get_stub(void *arg);

int32_t sys_hal_sd_info_get_stub(void *arg);

int32_t sys_hal_sd_finalize_stub(void *arg);

#endif /* HAL_SD_KSYSCALL_H */

