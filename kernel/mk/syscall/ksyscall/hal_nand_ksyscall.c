/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_config.h"

#if (RHINO_CONFIG_HAL_NAND_SYSCALL > 0)

#include <stdint.h>
#include "aos/hal/nand.h"
#include "hal_nand_syscall_arg.h"

int32_t sys_hal_nand_init_stub(void *arg)
{
    hal_nand_init_syscall_arg_t *_arg = arg;

    return hal_nand_init(_arg->nand);
}

int32_t sys_hal_nand_read_page_stub(void *arg)
{
    hal_nand_read_page_syscall_arg_t *_arg = arg;

    return hal_nand_read_page(_arg->nand, _arg->addr, _arg->data, _arg->page_count);
}

int32_t sys_hal_nand_write_page_stub(void *arg)
{
    hal_nand_write_page_syscall_arg_t *_arg = arg;

    return hal_nand_write_page(_arg->nand, _arg->addr, _arg->data, _arg->page_count);
}

int32_t sys_hal_nand_read_spare_stub(void *arg)
{
    hal_nand_read_spare_syscall_arg_t *_arg = arg;

    return hal_nand_read_spare(_arg->nand, _arg->addr, _arg->data, _arg->data_len);
}

int32_t sys_hal_nand_write_spare_stub(void *arg)
{
    hal_nand_write_spare_syscall_arg_t *_arg = arg;

    return hal_nand_write_spare(_arg->nand, _arg->addr, _arg->data, _arg->data_len);
}

int32_t sys_hal_nand_erase_block_stub(void *arg)
{
    hal_nand_erase_block_syscall_arg_t *_arg = arg;

    return hal_nand_erase_block(_arg->nand, _arg->addr);
}

int32_t sys_hal_nand_finalize_stub(void *arg)
{
    hal_nand_finalize_syscall_arg_t *_arg = arg;

    return hal_nand_finalize(_arg->nand);
}

#endif /* RHINO_CONFIG_HAL_NAND_SYSCALL */

