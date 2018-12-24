/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <mpu.h>

unsigned int mpu_get_type(void)
{
    return ((MPU->type) & MPU_TYPE_DREGION_MASK) >> MPU_TYPE_DREGION_OFFSET;
}

void mpu_enable(void)
{
    MPU->ctrl = MPU_CTRL_ENABLE_MASK | MPU_CTRL_PRIVDEFENA_MASK;

    /* Enable memory manage fault */
    *(SHCSR) |= (1<<16);

    __asm__ ("dsb 0xf\t\n"
             "isb\t\n"
             :::"memory");
}

void mpu_disable(void)
{
    MPU->ctrl = 0U;

    __asm__("dsb 0xf\t\n"
            "isb\t\n"
            :::"memory");
}


void mpu_config_region(MPU_Region_Init_t *init)
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

    __asm__("dsb 0xf\t\n"
            "isb\t\n"
            :::"memory");
}

unsigned int size_to_mpusize(unsigned int size)
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
        case 0x100000000: return MPU_REGION_SIZE_4GB;

        default: return 0;
    }
}

unsigned int mpusize_to_size(unsigned int mpusize)
{
    switch (mpusize) {
        case MPU_REGION_SIZE_32B:   return 0x20;
        case MPU_REGION_SIZE_64B:   return 0x40;
        case MPU_REGION_SIZE_128B:  return 0x80;
        case MPU_REGION_SIZE_256B:  return 0x100;
        case MPU_REGION_SIZE_512B:  return 0x200;
        case MPU_REGION_SIZE_1KB:   return 0x400;
        case MPU_REGION_SIZE_2KB:   return 0x800;
        case MPU_REGION_SIZE_4KB:   return 0x1000;
        case MPU_REGION_SIZE_8KB:   return 0x2000;
        case MPU_REGION_SIZE_16KB:  return 0x4000;
        case MPU_REGION_SIZE_32KB:  return 0x8000;
        case MPU_REGION_SIZE_64KB:  return 0x10000;
        case MPU_REGION_SIZE_128KB: return 0x20000;
        case MPU_REGION_SIZE_256KB: return 0x40000;
        case MPU_REGION_SIZE_512KB: return 0x80000;
        case MPU_REGION_SIZE_1MB:   return 0x100000;
        case MPU_REGION_SIZE_2MB:   return 0x200000;
        case MPU_REGION_SIZE_4MB:   return 0x400000;
        case MPU_REGION_SIZE_8MB:   return 0x800000;
        case MPU_REGION_SIZE_16MB:  return 0x1000000;
        case MPU_REGION_SIZE_32MB:  return 0x2000000;
        case MPU_REGION_SIZE_64MB:  return 0x4000000;
        case MPU_REGION_SIZE_128MB: return 0x8000000;
        case MPU_REGION_SIZE_256MB: return 0x10000000;
        case MPU_REGION_SIZE_512MB: return 0x20000000;
        case MPU_REGION_SIZE_1GB:   return 0x40000000;
        case MPU_REGION_SIZE_2GB:   return 0x80000000;
        case MPU_REGION_SIZE_4GB:   return 0x100000000;

        default: return 0;
    }
}

