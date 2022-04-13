/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#if defined(USER_SPACE_DRIVER) && defined(ENABLE_IO_MMAP)
#include <iomap.h>
#endif

#include <drivers/u_ld.h>

/**
 * unified driver initialization entry
 * do register map prior to call driver's init entry in user space
 *
 * @param drv_name - driver's name
 * @param drv_init_entry - pointer to driver's init entry
 * @param get_hw_info - pointer to the function to get driver's register info
 * @return 0 for success; negative for failure
 *
 */
int _unify_driver_init(const char *drv_name, OS_DRIVER_ENTRY drv_init_entry, DEVICE_HW_REG_INFO get_hw_info) {

    TRACE_DRV_ENTRY();
#if defined(USER_SPACE_DRIVER) && defined(ENABLE_IO_MMAP)
    /* this part is necessary only when the driver is in user space */

    if (get_hw_info) {
        hw_reg_array_info_t *info = (get_hw_info)();
        /* if driver's hw register info is not 0, do memory map on these register/length array one by one */
        if (info && info->size) {
            int i = 0;
            int j = 0;
            int ret = 0;
            hw_reg_info_t *p_reg_info = NULL;

            printf("%s: get_hw_info: size:%d, hw:%p\r\n", drv_name, info->size, info->hw);
            for (i = 0; i < info->size; i++) {
                p_reg_info = (info->hw[0] + i);

                if (!p_reg_info || !p_reg_info->addr || !p_reg_info->length) {
                    ret = -EINVAL;
                    goto err;
                }

                printf("%s: i:%d, p_reg_info:%p, addr:0x%lx, length:%d\r\n",
                       drv_name, i, p_reg_info, p_reg_info->addr, p_reg_info->length);
                ret = aos_io_mmap(p_reg_info->addr, p_reg_info->addr, p_reg_info->length);
                printf("%s: aos_io_mmap on [%lx,0x%ld] return %d\r\n",
                       drv_name, p_reg_info->addr, p_reg_info->addr, ret);
                if (!ret)
                    continue;

                err:
                printf("%s: hw_reg_array_info_t error, size:%d, invalid hw[%d]:%p\r\n",
                       drv_name, info->size, i, info->hw[i]);

                // unmap the driver if do memory map fails
                for (j = 0; j < i; j++) {
                    p_reg_info = info->hw[j];
                    aos_io_unmmap(p_reg_info->addr);
                }
                printf("%s: map hw register failed, ignore %s\r\n", drv_name, drv_name);
                return ret;
            }
        }
    }
#endif
    /* do driver initialization */
    return (drv_init_entry)();
}
