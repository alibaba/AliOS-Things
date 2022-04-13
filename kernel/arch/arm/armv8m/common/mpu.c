/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_api.h"
#include "k_mpu.h"
#include "k_proc.h"

#define RHINO_CONFIG_MPU_DEBUG

#ifdef RHINO_CONFIG_MPU_DEBUG
    #define DEBUG(fmt, arg...) printf(fmt, ##arg)
#else
    #define DEBUG(fmt, arg...)
#endif

uint32_t mpu_get_type(void)
{
    return ((MPU->type) & MPU_TYPE_DREGION_MASK) >> MPU_TYPE_DREGION_OFFSET;
}

void mpu_enable(void)
{
    MPU->ctrl = MPU_CTRL_ENABLE_MASK | MPU_CTRL_PRIVDEFENA_MASK;

    /* Enable memory manage fault */
    *(SHCSR) |= (1 << 16);

    RHINO_ASM("dsb 0xf\t\n"
              "isb\t\n"
              ::: "memory");
}

void mpu_disable(void)
{
    MPU->ctrl = 0U;

    RHINO_ASM("dsb 0xf\t\n"
              "isb\t\n"
              ::: "memory");
}

void mpu_config_region(MPU_Region_Init_t *init)
{
    uint8_t mair;
    MPU->rnr = init->range_no;

    if (init->cacheable) {
        mair = 0;
    } else {
        mair = 1;
    }

    if (init->enable) {
        MPU->rbar = init->base_addr | (init->access_permission << MPU_RBAR_AP_OFFSET);
        MPU->rlar = ((init->base_addr + init->size - 1) & (~0x1f))
                    | (init->enable << MPU_RLAR_ENABLE_OFFSET | mair << MPU_RLAR_ATTRINDX_OFFSET);
    } else {
        MPU->rbar = 0;
        MPU->rlar = 0;
    }

    RHINO_ASM("dsb 0xf\t\n"
              "isb\t\n"
              ::: "memory");
}

void mpu_enable_region(mpu_region_t *region, uint8_t rng_no,
                       uint8_t subregion_disable, uint8_t ext_type,
                       uint8_t access_permission, uint8_t disable_exec,
                       uint8_t shareable, uint8_t cacheable, uint8_t bufferable)
{
    MPU_Region_Init_t init;

    CPSR_ALLOC();

    init.range_no          = rng_no;
    init.base_addr         = region->start;
    init.size              = region->size;
    init.subregion_disable = subregion_disable;
    init.ext_type          = ext_type;
    init.access_permission = access_permission;
    init.disable_exec      = disable_exec;
    init.shareable         = shareable;
    init.cacheable         = cacheable;
    init.bufferable        = bufferable;
    init.enable            = 1;

    RHINO_CRITICAL_ENTER();

    mpu_config_region(&init);

    RHINO_CRITICAL_EXIT();
}

void mpu_disable_region(int rng_no)
{
    MPU_Region_Init_t init = {0};

    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();

    init.range_no = rng_no;
    init.enable   = 0;
    mpu_config_region(&init);

    RHINO_CRITICAL_EXIT();
}

int mpu_region_check(uint32_t addr, uint32_t size)
{
    if ((size == 0) || (addr % 32 != 0)) {
        DEBUG("error: mpu region addr 0x%x, size 0x%x\r\n",
              addr, size);
        return -1;
    }

    return 0;
}

int mpu_init(void)
{
    unsigned int region_cnt;

    region_cnt = mpu_get_type();
    if (!region_cnt) {
        DEBUG("%s: system doesn't support mpu\r\n", __func__);
        return -1;
    } else {
        DEBUG("%s: system mpu type 0x%x\r\n", __func__, region_cnt);
    }

    MPU->mair0 = MPU_ATTR_MAIR0;

    /* privileged mode uses the back ground region */
    mpu_enable();

    return 0;
}

static uint8_t g_mpu_region[MPU_MAX_RNG] = {0};

int32_t mpu_get_free_region(uint32_t pid)
{
    uint8_t index;
    uint8_t bitmap;
    if (pid == 0) {
        /*kernel */
        return -1;
    }
    bitmap = g_mpu_region[pid];
    for (index = MPU_APP_USER_START_RNG; index < MPU_MAX_RNG; index++) {
        if ((bitmap & (1UL << index)) == 0) {
            g_mpu_region[pid] |= 1UL << index;
            break;
        }
    }
    if (index == MPU_MAX_RNG) {
        DEBUG("err: no free mpu region for pid:%d\r\n", pid);
        return -1;
    }

    return index;
}

void mpu_free_region(uint32_t pid, uint32_t region)
{
    if (pid == 0) {
        /*kernel */
        return;
    }
    if (region < MPU_APP_USER_START_RNG || region >= MPU_MAX_RNG) {
        return;
    }
    g_mpu_region[pid] &= ~(1UL << region);
    return;
}

void dump_pgtbl(int32_t asid, unsigned long start, unsigned long end, int32_t verbose)
{
    extern void k_debug_dump_mpu(uint32_t pid);
    k_debug_dump_mpu(asid);
    return;
}
