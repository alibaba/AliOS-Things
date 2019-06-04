/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_config.h"

#if (RHINO_CONFIG_HAL_FLASH_SYSCALL > 0)

#include <stdint.h>
#include <string.h>
#include "aos/hal/flash.h"
#include "hal_flash_syscall_arg.h"

hal_logic_partition_t* sys_hal_flash_copy_info_stub(void *arg)
{
    hal_flash_copy_info_syscall_arg_t *_arg = arg;
    hal_logic_partition_t *src, *dest;

    src  = hal_flash_get_info(_arg->in_partition);
    dest = _arg->partition;
    memcpy((void*)dest, src, sizeof(hal_logic_partition_t));
    dest->partition_description = NULL;

    return dest;
}

int32_t sys_hal_flash_erase_stub(void *arg)
{
    hal_flash_erase_syscall_arg_t *_arg = arg;

    return hal_flash_erase(_arg->in_partition, _arg->off_set, _arg->size);
}

int32_t sys_hal_flash_write_stub(void *arg)
{
    hal_flash_write_syscall_arg_t *_arg = arg;

    return hal_flash_write(_arg->in_partition, _arg->off_set,
                           _arg->in_buf, _arg->in_buf_len);
}

int32_t sys_hal_flash_erase_write_stub(void *arg)
{
    hal_flash_erase_write_syscall_arg_t *_arg = arg;

    return hal_flash_erase_write(_arg->in_partition, _arg->off_set,
                                 _arg->in_buf, _arg->in_buf_len);
}

int32_t sys_hal_flash_read_stub(void *arg)
{
    hal_flash_read_syscall_arg_t *_arg = arg;

    return hal_flash_read(_arg->in_partition, _arg->off_set,
                          _arg->out_buf, _arg->in_buf_len);
}

int32_t sys_hal_flash_enable_secure_stub(void *arg)
{
    hal_flash_enable_secure_syscall_arg_t *_arg = arg;

    return hal_flash_enable_secure(_arg->partition, _arg->off_set, _arg->size);
}

int32_t sys_hal_flash_dis_secure_stub(void *arg)
{
    hal_flash_dis_secure_syscall_arg_t *_arg = arg;

    return hal_flash_dis_secure(_arg->in_partition, _arg->off_set, _arg->size);
}

#endif /*RHINO_CONFIG_HAL_FLASH_SYSCALL */

