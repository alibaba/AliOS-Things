/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef MPU_H
#define MPU_H


typedef struct {
    // MPU type register
    unsigned int type;
    // MPU control register
    unsigned int ctrl;
    // MPU range number register
    unsigned int rnr;
    // MPU region base address register
    unsigned int rbar;
    // MPU region attribute and size register
    unsigned int rasr;
    // MPU alias registers
    unsigned int rbar_a1;
    unsigned int rasr_a1;
    unsigned int rbar_a2;
    unsigned int rasr_a2;
    unsigned int rbar_a3;
    unsigned int rasr_a3;
} MPU_t;


/* System Handler Control and State register */
#define SHCSR_M                  ((unsigned int*)0xE000ED24UL)

/* MPU registers */

#define MPU_BASE                 (0xE000ED90UL)
#define MPU                      ((MPU_t*)(MPU_BASE))

#define MPU_TYPE_IREGION_OFFSET     (16U)

#define MPU_TYPE_IREGION_MASK       (0xFFUL << MPU_TYPE_IREGION_OFFSET)

#define MPU_TYPE_DREGION_OFFSET     (8U)

#define MPU_TYPE_DREGION_MASK       (0xFFUL << MPU_TYPE_DREGION_OFFSET)

#define MPU_TYPE_SEPARATE_OFFSET    (0U)

#define MPU_TYPE_SEPARATE_MASK      (1UL)

#define MPU_CTRL_PRIVDEFENA_OFFSET  (2U)

#define MPU_CTRL_PRIVDEFENA_MASK    (1UL << MPU_CTRL_PRIVDEFENA_OFFSET)

#define MPU_CTRL_HFNMIENA_OFFSET    (1U)

#define MPU_CTRL_HFNMIENA_MASK      (1UL << MPU_CTRL_HFNMIENA_OFFSET)

#define MPU_CTRL_ENABLE_OFFSET      (0U)

#define MPU_CTRL_ENABLE_MASK        (1UL)

#define MPU_RNR_REGION_OFFSET       (0U)

#define MPU_RNR_REGION_MASK         (0xFFUL)

#define MPU_RBAR_ADDR_OFFSET        (5U)

#define MPU_RBAR_ADDR_MASK          (0x7FFFFFFUL << MPU_RBAR_ADDR_OFFSET)

#define MPU_RBAR_VALID_OFFSET       (4U)

#define MPU_RBAR_VALID_MASK         (1UL << MPU_RBAR_VALID_OFFSET)

#define MPU_RBAR_REGION_OFFSET      (0U)

#define MPU_RBAR_REGION_MASK        (0xFUL)

#define MPU_RASR_ATTRS_OFFSET       (16U)

#define MPU_RASR_ATTRS_MASK         (0xFFFFUL << MPU_RASR_ATTRS_OFFSET)

#define MPU_RASR_XN_OFFSET          (28U)

#define MPU_RASR_XN_MASK            (1UL << MPU_RASR_XN_OFFSET)

#define MPU_RASR_AP_OFFSET          (24U)

#define MPU_RASR_AP_MASK            (0x7UL << MPU_RASR_AP_OFFSET)

#define MPU_RASR_TEX_OFFSET         (19U)

#define MPU_RASR_TEX_MASK           (0x7UL << MPU_RASR_TEX_OFFSET)

#define MPU_RASR_S_OFFSET           (18U)

#define MPU_RASR_S_MASK             (1UL << MPU_RASR_S_OFFSET)

#define MPU_RASR_C_OFFSET           (17U)

#define MPU_RASR_C_MASK             (1UL << MPU_RASR_C_OFFSET)

#define MPU_RASR_B_OFFSET           (16U)

#define MPU_RASR_B_MASK             (1UL << MPU_RASR_B_OFFSET)

#define MPU_RASR_SRD_OFFSET         (8U)

#define MPU_RASR_SRD_MASK           (0xFFUL << MPU_RASR_SRD_OFFSET)

#define MPU_RASR_SIZE_OFFSET        (1U)

#define MPU_RASR_SIZE_MASK          (0x1FUL << MPU_RASR_SIZE_OFFSET)

#define MPU_RASR_ENABLE_OFFSET      (0U)

#define MPU_RASR_ENABLE_MASK        (1UL)


/* MPU regions size */

#define MPU_REGION_SIZE_32B      (0x04U)
#define MPU_REGION_SIZE_64B      (0x05U)
#define MPU_REGION_SIZE_128B     (0x06U)
#define MPU_REGION_SIZE_256B     (0x07U)
#define MPU_REGION_SIZE_512B     (0x08U)
#define MPU_REGION_SIZE_1KB      (0x09U)
#define MPU_REGION_SIZE_2KB      (0x0AU)
#define MPU_REGION_SIZE_4KB      (0x0BU)
#define MPU_REGION_SIZE_8KB      (0x0CU)
#define MPU_REGION_SIZE_16KB     (0x0DU)
#define MPU_REGION_SIZE_32KB     (0x0EU)
#define MPU_REGION_SIZE_64KB     (0x0FU)
#define MPU_REGION_SIZE_128KB    (0x10U)
#define MPU_REGION_SIZE_256KB    (0x11U)
#define MPU_REGION_SIZE_512KB    (0x12U)
#define MPU_REGION_SIZE_1MB      (0x13U)
#define MPU_REGION_SIZE_2MB      (0x14U)
#define MPU_REGION_SIZE_4MB      (0x15U)
#define MPU_REGION_SIZE_8MB      (0x16U)
#define MPU_REGION_SIZE_16MB     (0x17U)
#define MPU_REGION_SIZE_32MB     (0x18U)
#define MPU_REGION_SIZE_64MB     (0x19U)
#define MPU_REGION_SIZE_128MB    (0x1AU)
#define MPU_REGION_SIZE_256MB    (0x1BU)
#define MPU_REGION_SIZE_512MB    (0x1CU)
#define MPU_REGION_SIZE_1GB      (0x1DU)
#define MPU_REGION_SIZE_2GB      (0x1EU)
#define MPU_REGION_SIZE_4GB      (0x1FU)

#define MPU_AP_NA_NA             (0x00U)
#define MPU_AP_RW_NA             (0x01U)
#define MPU_AP_RW_RO             (0x02U)
#define MPU_AP_RW_RW             (0x03U)
#define MPU_AP_RESV              (0x04U)
#define MPU_AP_RO_NA             (0x05U)
#define MPU_AP_RO_RO             (0x06U)


typedef struct {
    unsigned long base_addr;
    unsigned char range_no;
    unsigned char size;
    unsigned char ext_type;
    unsigned char access_permission;
    unsigned char disable_exec;
    unsigned char subregion_disable;
    unsigned char shareable;
    unsigned char cacheable;
    unsigned char bufferable;
    unsigned char enable;
} MPU_Region_Init_t;

#if (DEBUG_CONFIG_PANIC > 0)
/**
 * set mpu region for memory unauthorized access check
 *
 * @param[in]  addr_start   monitor start addr
 * @param[in]  addr_size    monitor size
 * @param[in]  mode         prohibit access(0) or read only(>0)
 */
void debug_memory_access_err_check(unsigned long addr_start, unsigned long addr_size, unsigned int mode);
#endif

#endif // MPU_H

