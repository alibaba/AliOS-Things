/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef MPU_H
#define MPU_H

#include <stdint.h>

typedef struct {
    // MPU type register
    uint32_t type;
    // MPU control register
    uint32_t ctrl;
    // MPU range number register
    uint32_t rnr;
    // MPU region base address register
    uint32_t rbar;
    // MPU region attribute and size register
    uint32_t rasr;
    // MPU alias registers
    uint32_t rbar_a1;
    uint32_t rasr_a1;
    uint32_t rbar_a2;
    uint32_t rasr_a2;
    uint32_t rbar_a3;
    uint32_t rasr_a3;
} MPU_t;

/* System Handler Control and State register */
#define SHCSR                   ((uint32_t*)0xE000ED24UL)

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
    uint32_t base_addr;         /**< MPU base address, should be aligned to 32 bytes */
    uint8_t  range_no;          /**< MPU region nubmer */
    uint8_t  size;              /**< MPU region size, it's MPU size, not memory actual size */
    uint8_t  ext_type;
    uint8_t  access_permission;
    uint8_t  disable_exec;      /**< disable execution */
    uint8_t  subregion_disable; /**< disable MPU sub-region */
    uint8_t  shareable;
    uint8_t  cacheable;
    uint8_t  bufferable;
    uint8_t  enable;            /**< enable MPU region */
} MPU_Region_Init_t;


#define MPU_APP_TEXT_RNG  0 /**< MPU region 0 for application text segment */
#define MPU_APP_DATA_RNG  1 /**< MPU region 1 for application data segment */

#define MPU_MAX_RNG       7 /**< armv7m supports as many as 7 MPU regions */

typedef struct {
    uint32_t start;   /**< the region's start address */
    uint32_t mpusize; /**< MPU region size */
    uint64_t size;    /**< the region's actural size */
} mpu_region_t;

/**
 * @brief read MPU type register
 * @note  If the hardware supports MPU, the return value is non-zero
 *
 * @return the context of MPU type register
 */
uint32_t mpu_get_type(void);

/**
 * @brief enable MPU
 */
void mpu_enable(void);

/**
 * @brief disable MPU
 */
void mpu_disable(void);

/**
 * @brief Configure MPU region
 *
 * @param[in] init  the region's configuration parameter
 */
void mpu_config_region(MPU_Region_Init_t *init);

/**
 * @brief Convert memory physical size to MPU size
 * @note  the input physical size should be 2^n,
 *        where n >= 5
 *
 * @param[in]   the memory's physical size
 *
 * @return On success, return the converted MPU size,
 *         else return 0
 */
uint32_t size_to_mpusize(uint64_t size);

/**
 * @brief Convert MPU size to memory physical size
 *
 * @param[in] mpusize   MPU size
 *
 * @return the converted memory physical size
 *
 */
uint64_t mpusize_to_size(uint32_t mpusize);

/**
 * @brief mpu_enable_region
 *
 * @param[in] region                points to MPU region description struct
 * @param[in] rng_no                MPU region nubmer
 * @param[in] subregion_disable     MPU sub-region configuration
 * @param[in] ext_type
 * @param[in] access_permission     MPU access permission
 * @param[in] disable_exec          Disable execution on this region
 * @param[in] shareable
 * @param[in] cacheable
 * @param[in] bufferable
 */
void mpu_enable_region(mpu_region_t *region, uint8_t rng_no,
                       uint8_t subregion_disable, uint8_t ext_type,
                       uint8_t access_permission, uint8_t disable_exec,
                       uint8_t shareable, uint8_t cacheable, uint8_t bufferable);

/**
 * @brief disable MPU region
 *
 * @param[in] rng_no the region number
 */
void mpu_disable_region(int rng_no);

/**
 * @brief check wheterh the start address and size confirm to MPU region requirement
 *
 * @param[in] addr  MPU region start address
 * @param[in] size  region physical size
 *
 * @return On success, return 0, else return -1
 */
int mpu_region_check(uint32_t addr, uint64_t size);

/**
 * @brief Init system MPU
 *
 * @return On success, return 0, else return -1
 */
int mpu_init(void);

#endif /* MPU_H */

