/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_NAND_KSYSCALL_H
#define HAL_NAND_KSYSCALL_H

int32_t sys_hal_nand_init_stub(void *arg);

int32_t sys_hal_nand_read_page_stub(void *arg);

int32_t sys_hal_nand_write_page_stub(void *arg);

int32_t sys_hal_nand_read_spare_stub(void *arg);

int32_t sys_hal_nand_write_spare_stub(void *arg);

int32_t sys_hal_nand_erase_block_stub(void *arg);

int32_t sys_hal_nand_finalize_stub(void *arg);

#endif /* HAL_NAND_KSYSCALL_H */

