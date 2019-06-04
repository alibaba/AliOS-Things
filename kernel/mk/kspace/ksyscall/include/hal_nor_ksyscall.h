/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_NOR_KSYSCALL_H
#define HAL_NOR_KSYSCALL_H

int32_t sys_hal_nor_init_stub(void *arg);

int32_t sys_hal_nor_read_stub(void *arg);

int32_t sys_hal_nor_write_stub(void *arg);

int32_t sys_hal_nor_erase_block_stub(void *arg);

int32_t sys_hal_nor_erase_chip_stub(void *arg);

int32_t sys_hal_nor_finalize_stub(void *arg);

#endif /* HAL_NOR_KSYSCALL_H */

