/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "k_config.h"
#include "syscall.h"
#include "hal_nand_syscall_arg.h"
#include "syscall_no.h"

int32_t hal_nand_init(nand_dev_t *nand)
{
    hal_nand_init_syscall_arg_t _arg;

    _arg.nand = nand;

    return (int32_t)SYSCALL(SYS_HAL_NAND_INIT, (void*)&_arg);
}

int32_t hal_nand_read_page(nand_dev_t *nand, nand_addr_t *addr,
                           uint8_t *data, uint32_t page_count)
{
    hal_nand_read_page_syscall_arg_t _arg;

    _arg.nand       = nand;
    _arg.addr       = addr;
    _arg.data       = data;
    _arg.page_count = page_count;

    return (int32_t)SYSCALL(SYS_HAL_NAND_READ_PAGE, (void*)&_arg);
}

int32_t hal_nand_write_page(nand_dev_t *nand, nand_addr_t *addr,
                            uint8_t *data, uint32_t page_count)
{
    hal_nand_write_page_syscall_arg_t _arg;

    _arg.nand       = nand;
    _arg.addr       = addr;
    _arg.data       = data;
    _arg.page_count = page_count;

    return (int32_t)SYSCALL(SYS_HAL_NAND_WRITE_PAGE, (void*)&_arg);
}

int32_t hal_nand_read_spare(nand_dev_t *nand, nand_addr_t *addr,
                            uint8_t *data, uint32_t data_len)
{
    hal_nand_read_spare_syscall_arg_t _arg;

    _arg.nand     = nand;
    _arg.addr     = addr;
    _arg.data     = data;
    _arg.data_len = data_len;

    return (int32_t)SYSCALL(SYS_HAL_NAND_READ_SPARE, (void*)&_arg);
}

int32_t hal_nand_write_spare(nand_dev_t *nand, nand_addr_t *addr,
                             uint8_t *data, uint32_t data_len)
{
    hal_nand_write_spare_syscall_arg_t _arg;

    _arg.nand     = nand;
    _arg.addr     = addr;
    _arg.data     = data;
    _arg.data_len = data_len;

    return (int32_t)SYSCALL(SYS_HAL_NAND_WRITE_SPARE, (void*)&_arg);
}

int32_t hal_nand_erase_block(nand_dev_t *nand, nand_addr_t *addr)
{
    hal_nand_erase_block_syscall_arg_t _arg;

    _arg.nand = nand;
    _arg.addr = addr;

    return (int32_t)SYSCALL(SYS_HAL_NAND_ERASE_BLOCK, (void*)&_arg);
}

int32_t hal_nand_finalize(nand_dev_t *nand)
{
    hal_nand_finalize_syscall_arg_t _arg;

    _arg.nand = nand;

    return (int32_t)SYSCALL(SYS_HAL_NAND_FINALIZE, (void*)&_arg);
}

