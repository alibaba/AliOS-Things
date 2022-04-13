/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_MPU_H
#define K_MPU_H

#include <stdint.h>

typedef struct {
    /* MPU type register */
    uint32_t type;
    /* MPU control register */
    uint32_t ctrl;
    /* MPU range number register */
    uint32_t rnr;
    /* MPU region base address register */
    uint32_t rbar;
    /* MPU Region Limit Address Register */
    uint32_t rlar;
    // MPU alias registers
    uint32_t rbar_a1;
    uint32_t rlar_a1;
    uint32_t rbar_a2;
    uint32_t rlar_a2;
    uint32_t rbar_a3;
    uint32_t rlar_a3;
    uint32_t dummy;
    /* MPU Memory Attribute Indirection Register 0 */
    uint32_t mair0;
    /* MPU Memory Attribute Indirection Register 1 */
    uint32_t mair1;
} MPU_t;

#ifndef SHCSR
/* System Handler Control and State register */
#define SHCSR                       ((uint32_t*)0xE000ED24UL)
#endif

#ifndef MPU_BASE
/* MPU registers base address */
#define MPU_BASE                    (0xE000ED90UL)
#endif

#ifndef MPU
#define MPU                         ((MPU_t*)(MPU_BASE))
#endif

/* The bit field of TYPE */
#define MPU_TYPE_DREGION_OFFSET     (8U)
#define MPU_TYPE_DREGION_MASK       (0xFFUL << MPU_TYPE_DREGION_OFFSET)

#define MPU_TYPE_SEPARATE_OFFSET    (0U)
#define MPU_TYPE_SEPARATE_MASK      (1UL)

/* The bit field of CTRL */
#define MPU_CTRL_PRIVDEFENA_OFFSET  (2U)
#define MPU_CTRL_PRIVDEFENA_MASK    (1UL << MPU_CTRL_PRIVDEFENA_OFFSET)

#define MPU_CTRL_HFNMIENA_OFFSET    (1U)
#define MPU_CTRL_HFNMIENA_MASK      (1UL << MPU_CTRL_HFNMIENA_OFFSET)

#define MPU_CTRL_ENABLE_OFFSET      (0U)
#define MPU_CTRL_ENABLE_MASK        (1UL)

/* The bit field of RNR */
#define MPU_RNR_REGION_OFFSET       (0U)
#define MPU_RNR_REGION_MASK         (0xFFUL)

/* The bit field of RLAR */
#define MPU_RLAR_ENABLE_OFFSET      (0U)
#define MPU_RLAR_ENABLE_MASK        (1UL)

#define MPU_RLAR_ATTRINDX_OFFSET    (1U)
#define MPU_RLAR_ATTRINDX_MASK      (7UL << MPU_RLAR_ATTRINDX_OFFSET)

/* The bit field of RBAR */
#define MPU_RBAR_XN_OFFSET          (0U)
#define MPU_RBAR_XN_MASK            (1UL << MPU_RBAR_XN_OFFSET)

#define MPU_RBAR_AP_OFFSET          (1U)
#define MPU_RBAR_AP_MASK            (0x3UL << MPU_RBAR_AP_OFFSET)

#define MPU_RBAR_ADDR_OFFSET        (5U)
#define MPU_RBAR_ADDR_MASK          (0x7FFFFFFUL << MPU_RBAR_ADDR_OFFSET)

/* Access permissions */
#define MPU_AP_RW_NA             (0x00U)
#define MPU_AP_RW_RW             (0x01U)
#define MPU_AP_RO_NA             (0x02U)
#define MPU_AP_RO_RO             (0x03U)

/* The bit field of MAIR */
/* 0:NORMAL  1:FOR IO*/
#define MPU_ATTR_MAIR0             ((MPU_ATTR_DEVICE_NGNRNE << 8) | MPU_ATTR_CACHE_WT_NTRAN )

/* memory attribute */
/* cache_write_through_notransient*/
#define MPU_ATTR_CACHE_WT_NTRAN  0x22
#define MPU_ATTR_DEVICE_NGNRNE   0x00

/* MPU region configure */
#define MPU_APP_TEXT_RNG        0 /**< MPU region 0 for application text segment */
#define MPU_APP_DATA_RNG        1 /**< MPU region 1 for application data segment */
#define MPU_APP_USER_START_RNG  2 /**< MPU region 2 for application user start */

#define MPU_MAX_RNG             8 /**< armv7m supports as many as 8 MPU regions */

typedef struct {
    uint32_t base_addr;         /**< MPU base address, should be aligned to 32 bytes */
    uint32_t size;
    uint8_t  range_no;          /**< MPU region nubmer */
    uint8_t  ext_type;
    uint8_t  access_permission;
    uint8_t  disable_exec;      /**< disable execution */
    uint8_t  subregion_disable; /**< disable MPU sub-region */
    uint8_t  shareable;
    uint8_t  cacheable;
    uint8_t  bufferable;
    uint8_t  enable;            /**< enable MPU region */
} MPU_Region_Init_t;

typedef struct {
    uint32_t start;   /**< the region's start address */
    uint32_t mpusize; /**< MPU region size */
    uint32_t size;    /**< the region's actural size */
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
int mpu_region_check(uint32_t addr, uint32_t size);

/**
 * @brief Init system MPU
 *
 * @return On success, return 0, else return -1
 */
int mpu_init(void);

#endif /* K_MPU_H */

