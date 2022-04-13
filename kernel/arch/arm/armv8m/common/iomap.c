/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdlib.h>

#include "k_api.h"
#include "k_mpu.h"
#include "k_proc.h"
#include "iomap.h"
extern uint32_t k_proc_ioregion_exist(uint32_t pid, uintptr_t addr, size_t len);
extern int32_t mpu_get_free_region(uint32_t pid);
extern void mpu_free_region(uint32_t pid, uint32_t region);
extern void k_proc_ioregion_reg(uint32_t pid, io_region_t *pregion);
extern io_region_t *k_proc_ioregion_unreg(uint32_t pid, uintptr_t addr);
extern void mpu_enable_region(mpu_region_t *region, uint8_t rng_no,
                              uint8_t subregion_disable, uint8_t ext_type,
                              uint8_t access_permission, uint8_t disable_exec,
                              uint8_t shareable, uint8_t cacheable, uint8_t bufferable);

int32_t aos_mem_mmap(uintptr_t vaddr, uintptr_t paddr, size_t len, int attr)
{
    uint32_t pid;
    int cacheattr = 1;  /* default normal attr */
    int32_t region_id;
    io_region_t *new_region = NULL;
    pid = k_asid_get();
    (void)paddr;

    if (0 == pid) {
        /* Do not need map */
        return 0;
    }

    if(attr == NORMAL_MEM){
        cacheattr = 1;
    }else if(attr == DEVICE_MEM){
        cacheattr = 0;
    }

    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();
    /* is repeat */
    if (k_proc_ioregion_exist(pid, vaddr, len)) {
        printk("warning: func:%s, region exist, pid:%d, vaddr:%d\r\n", __func__, pid, vaddr);
        RHINO_CRITICAL_EXIT();
        return -1;
    }

    /* alloc rng_no */
    region_id = mpu_get_free_region(pid);
    if (region_id < 0) {
        RHINO_CRITICAL_EXIT();
        return -1;
    }

    /* alloc region */
    new_region = malloc(sizeof(io_region_t));
    if (NULL == new_region) {
        mpu_free_region(pid, region_id);
        printk("error: func:%s, malloc fail, pid:%d, vaddr:%d\r\n", __func__, pid, vaddr);
        RHINO_CRITICAL_EXIT();
        return -1;
    }
    memset(new_region, 0, sizeof(io_region_t));
    new_region->io_region.start = paddr;
    new_region->io_region.size = len;
    new_region->region_id = region_id;
    new_region->memattr = attr;

    /* set MPU */
    mpu_enable_region(&(new_region->io_region), region_id, 0, 0, MPU_AP_RW_RW, 0, 0, cacheattr, 0);

    /* Register to cur pid */
    k_proc_ioregion_reg(pid, new_region);

    RHINO_CRITICAL_EXIT();

    return 0;
}

int32_t aos_io_mmap(uintptr_t vaddr, uintptr_t paddr, size_t len)
{
    return aos_mem_mmap(vaddr, paddr, len, DEVICE_MEM);
}

int32_t aos_io_unmmap(uintptr_t vaddr)
{
    uint32_t pid;
    int32_t region_id;
    io_region_t *free_region;
    pid = k_asid_get();

    if (0 == pid) {
        /* Do not need map */
        return 0;
    }

    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();

    free_region = k_proc_ioregion_unreg(pid, vaddr);
    if (NULL == free_region) {
        RHINO_CRITICAL_EXIT();
        return -1;
    }
    region_id = free_region->region_id;
    mpu_disable_region(region_id);
    free(free_region);
    mpu_free_region(pid, region_id);

    RHINO_CRITICAL_EXIT();

    return 0;
}

uintptr_t aos_io_mva2pa(uintptr_t vaddr)
{
    return vaddr;
}


uintptr_t aos_io_mpa2va(uintptr_t paddr)
{
    return paddr;
}

