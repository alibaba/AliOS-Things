/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "panic_mpu.h"
#include "k_compiler.h"
#include "debug_api.h"

typedef struct {
    unsigned long start;
    unsigned long size;
    unsigned long mpusize;
} mem_region_t;

#if (DEBUG_CONFIG_PANIC > 0)

static void mpu_enable(void);
static void mpu_disable(void);
static void mpu_config_region(MPU_Region_Init_t *init);
static unsigned int size_to_mpusize(unsigned int size);

/*
 * Func: mpu is valid in mcu
 * IN  : none
 * Out : 0 -- valid; 1 -- invalid
 * */
static unsigned int mpu_is_valid(void)
{
    return ((MPU->type) & MPU_TYPE_DREGION_MASK == 0) ? 1: 0;
}

static void mpu_enable(void)
{
    MPU->ctrl = MPU_CTRL_ENABLE_MASK | MPU_CTRL_PRIVDEFENA_MASK;

    /* Enable memory manage fault */
    *(SHCSR_M) |= (1<<16);

    OS_DSB();
    OS_ISB();
    OS_DMB();
}

static void mpu_disable(void)
{
    MPU->ctrl = 0U;

    OS_DSB();
    OS_ISB();
    OS_DMB();
}

static void enable_region(mem_region_t *region, int rng_no,
                          int subregion_disable, int ext_type,
                          int access_permission, int disable_exec,
                          int shareable, int cacheable, int bufferable)
{
    MPU_Region_Init_t init;

    init.range_no          = rng_no;
    init.base_addr         = region->start;
    init.size              = region->mpusize;
    init.subregion_disable = subregion_disable;
    init.ext_type          = ext_type;
    init.access_permission = access_permission;
    init.disable_exec      = disable_exec;
    init.shareable         = shareable;
    init.cacheable         = cacheable;
    init.bufferable        = bufferable;
    init.enable            = 1;

    mpu_config_region(&init);
}


static void mpu_config_region(MPU_Region_Init_t *init)
{
    MPU->rnr = init->range_no;

    if (init->enable) {
        MPU->rbar = init->base_addr;
        MPU->rasr = (init->disable_exec << MPU_RASR_XN_OFFSET
                    | init->access_permission << MPU_RASR_AP_OFFSET
                    | init->ext_type << MPU_RASR_TEX_OFFSET
                    | init->shareable << MPU_RASR_S_OFFSET
                    | init->cacheable << MPU_RASR_C_OFFSET
                    | init->bufferable << MPU_RASR_B_OFFSET
                    | init->subregion_disable << MPU_RASR_SRD_OFFSET
                    | init->size << MPU_RASR_SIZE_OFFSET
                    | init->enable << MPU_RASR_ENABLE_OFFSET);
    } else {
        MPU->rbar = 0;
        MPU->rasr = 0;
    }

    OS_DSB();
    OS_ISB();
    OS_DMB();
}

static unsigned int size_to_mpusize(unsigned int size)
{
    switch (size) {
        case 0x20:        return MPU_REGION_SIZE_32B;
        case 0x40:        return MPU_REGION_SIZE_64B;
        case 0x80:        return MPU_REGION_SIZE_128B;
        case 0x100:       return MPU_REGION_SIZE_256B;
        case 0x200:       return MPU_REGION_SIZE_512B;
        case 0x400:       return MPU_REGION_SIZE_1KB;
        case 0x800:       return MPU_REGION_SIZE_2KB;
        case 0x1000:      return MPU_REGION_SIZE_4KB;
        case 0x2000:      return MPU_REGION_SIZE_8KB;
        case 0x4000:      return MPU_REGION_SIZE_16KB;
        case 0x8000:      return MPU_REGION_SIZE_32KB;
        case 0x10000:     return MPU_REGION_SIZE_64KB;
        case 0x20000:     return MPU_REGION_SIZE_128KB;
        case 0x40000:     return MPU_REGION_SIZE_256KB;
        case 0x80000:     return MPU_REGION_SIZE_512KB;
        case 0x100000:    return MPU_REGION_SIZE_1MB;
        case 0x200000:    return MPU_REGION_SIZE_2MB;
        case 0x400000:    return MPU_REGION_SIZE_4MB;
        case 0x800000:    return MPU_REGION_SIZE_8MB;
        case 0x1000000:   return MPU_REGION_SIZE_16MB;
        case 0x2000000:   return MPU_REGION_SIZE_32MB;
        case 0x4000000:   return MPU_REGION_SIZE_64MB;
        case 0x8000000:   return MPU_REGION_SIZE_128MB;
        case 0x10000000:  return MPU_REGION_SIZE_256MB;
        case 0x20000000:  return MPU_REGION_SIZE_512MB;
        case 0x40000000:  return MPU_REGION_SIZE_1GB;
        case 0x80000000:  return MPU_REGION_SIZE_2GB;

        default: return 0;
    }
}

static void mpu_set(unsigned long addr, unsigned long size, unsigned int mode)
{
    mem_region_t region;

    region.start   = addr;
    region.size    = size;
    region.mpusize = size_to_mpusize(region.size);

    mpu_disable();

    enable_region(&region, 0, 0, 0, mode, 0, 0, 1, 1);

    mpu_enable();
}

static unsigned int mpu_check(unsigned long addr, unsigned long size)
{
    if ((size < 0x20) || (size > 0x80000000)) {
        printf("mpu region size error\r\n");
        return 1;
    }

    if (mpu_is_valid() != 0) {
        printf("error:no mpu in mcu\r\n");
        return 1;
    }

    return 0;
}

/**
 * set mpu region for memory unauthorized access check
 *
 * @param[in]  addr_start   monitor start addr
 * @param[in]  addr_size    monitor size
 * @param[in]  mode         prohibit access (0) or read only(>0)
 */
void debug_memory_access_err_check(unsigned long addr_start, unsigned long addr_size, unsigned int mode)
{
    unsigned int mpu_region_type;

    if (mpu_check(addr_start, addr_size) != 0)
        return;

    if ((addr_start % addr_size) != 0)
        addr_start = (addr_start + (addr_size - 1)) & ~(addr_size - 1);

    if (mode > 0) {
        mpu_region_type = MPU_AP_RO_NA;
    } else
        mpu_region_type = MPU_AP_NA_NA;

    mpu_set(addr_start, addr_size, mpu_region_type);
    printf("mpu addr: 0x%x\n", addr_start);
}

void debug_task_stack_ovf_check(char *task_name)
{
    int         ret;
    ktask_t     *task;
    cpu_stack_t *task_stack_base;

    if (task_name == NULL) {
        printf("error: task name invalid\r\n");
        return;
    }

    task = debug_task_find(task_name);
    if (task == NULL) {
        printf("error: task do not exist\r\n");
        return;
    }

    task_stack_base = task->task_stack_base;
    if (task_stack_base == NULL) {
        printf("error: task_stack_base err\r\n");
        return;
    }

    debug_memory_access_err_check((unsigned long)task_stack_base, 0x20, 1);
}

void debug_check_mem_access_disable(void)
{
    mpu_disable();
}

#endif
