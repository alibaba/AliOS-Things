/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#if defined(USER_SPACE_DRIVER) && (USER_SPACE_DRIVER == 1)
#include "plat_addr_map.h"
#include "plat_types.h"
#include "drivers/u_ld.h"

extern int32_t aos_io_mmap(uintptr_t vaddr, uintptr_t paddr, size_t len);
extern int32_t aos_io_unmmap(uintptr_t vaddr);
extern uintptr_t aos_io_mva2pa(uintptr_t vaddr);
extern uintptr_t aos_io_mpa2va(uintptr_t paddr);

static hw_reg_info_t _hw_reg_info[] = {
    {
        FLASH_NC_BASE, 0x1000000
    },
    {
        CMU_BASE, 0x60000000//0x200000
    },
    {
        ICACHE_CTRL_BASE, 0x100
    },
    {
        DCACHE_CTRL_BASE, 0x100
    },

};

static hw_reg_array_info_t _hw_reg_array = {
    .size = sizeof(_hw_reg_info)/sizeof(_hw_reg_info[0]),
    .hw = {(hw_reg_info_t *)&_hw_reg_info}
};

static hw_reg_array_info_t* get_hw_reg_info (void) {
    return &_hw_reg_array;
}

static int _reg_map_init (void) {
    return 0;
}

EARLY_DRIVER_FULL_ENTRY(_reg_map_init, get_hw_reg_info)
#endif
