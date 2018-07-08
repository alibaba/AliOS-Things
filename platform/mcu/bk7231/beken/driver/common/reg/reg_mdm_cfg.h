/**
 * @file reg_mdm_cfg.h
 * @brief Definitions of the MDM HW block registers and register access functions.
 *
 * @defgroup REG_MDM_CFG REG_MDM_CFG
 * @ingroup REG
 * @{
 *
 * @brief Definitions of the MDM HW block registers and register access functions.
 */
#ifndef _REG_MDM_CFG_H_
#define _REG_MDM_CFG_H_

#include "co_int.h"
#include "_reg_mdm_cfg.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"

/** @brief Number of registers in the REG_MDM_CFG peripheral.
 */
#define REG_MDM_CFG_COUNT 550

/** @brief Decoding mask of the REG_MDM_CFG peripheral registers from the CPU point of view.
 */
#define REG_MDM_CFG_DECODING_MASK 0x00000FFF

/**
 * @name RXMODES register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     17           PSSELECT40   0
 *     16           PSSELECT20   0
 *     14            FORCENSS1   0
 *     13              FORCE40   0
 *     12              FORCE20   0
 *     09             RXSTBCEN   1
 *     08             DUPMRCEN   1
 *     04           RXALLOWVHT   1
 *     03            RXALLOWGF   1
 *     02            RXALLOWMM   1
 *     01        RXALLOWLEGACY   1
 *     00          RXALLOWDSSS   1
 * </pre>
 *
 * @{
 */

/// Address of the RXMODES register
#define MDM_RXMODES_ADDR   0x01000800
/// Offset of the RXMODES register from the base address
#define MDM_RXMODES_OFFSET 0x00000800
/// Index of the RXMODES register
#define MDM_RXMODES_INDEX  0x00000200
/// Reset value of the RXMODES register
#define MDM_RXMODES_RESET  0x0000031F

/**
 * @brief Returns the current value of the RXMODES register.
 * The RXMODES register will be read and its value returned.
 * @return The current value of the RXMODES register.
 */
__INLINE uint32_t mdm_rxmodes_get(void)
{
    return REG_PL_RD(MDM_RXMODES_ADDR);
}

/**
 * @brief Sets the RXMODES register to a value.
 * The RXMODES register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxmodes_set(uint32_t value)
{
    REG_PL_WR(MDM_RXMODES_ADDR, value);
}

// field definitions
/// PSSELECT40 field bit
#define MDM_PSSELECT40_BIT       ((uint32_t)0x00020000)
/// PSSELECT40 field position
#define MDM_PSSELECT40_POS       17
/// PSSELECT20 field bit
#define MDM_PSSELECT20_BIT       ((uint32_t)0x00010000)
/// PSSELECT20 field position
#define MDM_PSSELECT20_POS       16
/// FORCENSS1 field bit
#define MDM_FORCENSS1_BIT        ((uint32_t)0x00004000)
/// FORCENSS1 field position
#define MDM_FORCENSS1_POS        14
/// FORCE40 field bit
#define MDM_FORCE40_BIT          ((uint32_t)0x00002000)
/// FORCE40 field position
#define MDM_FORCE40_POS          13
/// FORCE20 field bit
#define MDM_FORCE20_BIT          ((uint32_t)0x00001000)
/// FORCE20 field position
#define MDM_FORCE20_POS          12
/// RXSTBCEN field bit
#define MDM_RXSTBCEN_BIT         ((uint32_t)0x00000200)
/// RXSTBCEN field position
#define MDM_RXSTBCEN_POS         9
/// DUPMRCEN field bit
#define MDM_DUPMRCEN_BIT         ((uint32_t)0x00000100)
/// DUPMRCEN field position
#define MDM_DUPMRCEN_POS         8
/// RXALLOWVHT field bit
#define MDM_RXALLOWVHT_BIT       ((uint32_t)0x00000010)
/// RXALLOWVHT field position
#define MDM_RXALLOWVHT_POS       4
/// RXALLOWGF field bit
#define MDM_RXALLOWGF_BIT        ((uint32_t)0x00000008)
/// RXALLOWGF field position
#define MDM_RXALLOWGF_POS        3
/// RXALLOWMM field bit
#define MDM_RXALLOWMM_BIT        ((uint32_t)0x00000004)
/// RXALLOWMM field position
#define MDM_RXALLOWMM_POS        2
/// RXALLOWLEGACY field bit
#define MDM_RXALLOWLEGACY_BIT    ((uint32_t)0x00000002)
/// RXALLOWLEGACY field position
#define MDM_RXALLOWLEGACY_POS    1
/// RXALLOWDSSS field bit
#define MDM_RXALLOWDSSS_BIT      ((uint32_t)0x00000001)
/// RXALLOWDSSS field position
#define MDM_RXALLOWDSSS_POS      0

/// PSSELECT40 field reset value
#define MDM_PSSELECT40_RST       0x0
/// PSSELECT20 field reset value
#define MDM_PSSELECT20_RST       0x0
/// FORCENSS1 field reset value
#define MDM_FORCENSS1_RST        0x0
/// FORCE40 field reset value
#define MDM_FORCE40_RST          0x0
/// FORCE20 field reset value
#define MDM_FORCE20_RST          0x0
/// RXSTBCEN field reset value
#define MDM_RXSTBCEN_RST         0x1
/// DUPMRCEN field reset value
#define MDM_DUPMRCEN_RST         0x1
/// RXALLOWVHT field reset value
#define MDM_RXALLOWVHT_RST       0x1
/// RXALLOWGF field reset value
#define MDM_RXALLOWGF_RST        0x1
/// RXALLOWMM field reset value
#define MDM_RXALLOWMM_RST        0x1
/// RXALLOWLEGACY field reset value
#define MDM_RXALLOWLEGACY_RST    0x1
/// RXALLOWDSSS field reset value
#define MDM_RXALLOWDSSS_RST      0x1

/**
 * @brief Constructs a value for the RXMODES register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] psselect40 - The value to use for the PSSELECT40 field.
 * @param[in] psselect20 - The value to use for the PSSELECT20 field.
 * @param[in] forcenss1 - The value to use for the FORCENSS1 field.
 * @param[in] force40 - The value to use for the FORCE40 field.
 * @param[in] force20 - The value to use for the FORCE20 field.
 * @param[in] rxstbcen - The value to use for the RXSTBCEN field.
 * @param[in] dupmrcen - The value to use for the DUPMRCEN field.
 * @param[in] rxallowvht - The value to use for the RXALLOWVHT field.
 * @param[in] rxallowgf - The value to use for the RXALLOWGF field.
 * @param[in] rxallowmm - The value to use for the RXALLOWMM field.
 * @param[in] rxallowlegacy - The value to use for the RXALLOWLEGACY field.
 * @param[in] rxallowdsss - The value to use for the RXALLOWDSSS field.
 */
__INLINE void mdm_rxmodes_pack(uint8_t psselect40, uint8_t psselect20, uint8_t forcenss1, uint8_t force40, uint8_t force20, uint8_t rxstbcen, uint8_t dupmrcen, uint8_t rxallowvht, uint8_t rxallowgf, uint8_t rxallowmm, uint8_t rxallowlegacy, uint8_t rxallowdsss)
{
    ASSERT_ERR((((uint32_t)psselect40 << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)psselect20 << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)forcenss1 << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)force40 << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)force20 << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)rxstbcen << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)dupmrcen << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)rxallowvht << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)rxallowgf << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)rxallowmm << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)rxallowlegacy << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)rxallowdsss << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_RXMODES_ADDR,  ((uint32_t)psselect40 << 17) | ((uint32_t)psselect20 << 16) | ((uint32_t)forcenss1 << 14) | ((uint32_t)force40 << 13) | ((uint32_t)force20 << 12) | ((uint32_t)rxstbcen << 9) | ((uint32_t)dupmrcen << 8) | ((uint32_t)rxallowvht << 4) | ((uint32_t)rxallowgf << 3) | ((uint32_t)rxallowmm << 2) | ((uint32_t)rxallowlegacy << 1) | ((uint32_t)rxallowdsss << 0));
}

/**
 * @brief Unpacks RXMODES's fields from current value of the RXMODES register.
 *
 * Reads the RXMODES register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] psselect40 - Will be populated with the current value of this field from the register.
 * @param[out] psselect20 - Will be populated with the current value of this field from the register.
 * @param[out] forcenss1 - Will be populated with the current value of this field from the register.
 * @param[out] force40 - Will be populated with the current value of this field from the register.
 * @param[out] force20 - Will be populated with the current value of this field from the register.
 * @param[out] rxstbcen - Will be populated with the current value of this field from the register.
 * @param[out] dupmrcen - Will be populated with the current value of this field from the register.
 * @param[out] rxallowvht - Will be populated with the current value of this field from the register.
 * @param[out] rxallowgf - Will be populated with the current value of this field from the register.
 * @param[out] rxallowmm - Will be populated with the current value of this field from the register.
 * @param[out] rxallowlegacy - Will be populated with the current value of this field from the register.
 * @param[out] rxallowdsss - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxmodes_unpack(uint8_t *psselect40, uint8_t *psselect20, uint8_t *forcenss1, uint8_t *force40, uint8_t *force20, uint8_t *rxstbcen, uint8_t *dupmrcen, uint8_t *rxallowvht, uint8_t *rxallowgf, uint8_t *rxallowmm, uint8_t *rxallowlegacy, uint8_t *rxallowdsss)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODES_ADDR);

    *psselect40 = (localVal & ((uint32_t)0x00020000)) >> 17;
    *psselect20 = (localVal & ((uint32_t)0x00010000)) >> 16;
    *forcenss1 = (localVal & ((uint32_t)0x00004000)) >> 14;
    *force40 = (localVal & ((uint32_t)0x00002000)) >> 13;
    *force20 = (localVal & ((uint32_t)0x00001000)) >> 12;
    *rxstbcen = (localVal & ((uint32_t)0x00000200)) >> 9;
    *dupmrcen = (localVal & ((uint32_t)0x00000100)) >> 8;
    *rxallowvht = (localVal & ((uint32_t)0x00000010)) >> 4;
    *rxallowgf = (localVal & ((uint32_t)0x00000008)) >> 3;
    *rxallowmm = (localVal & ((uint32_t)0x00000004)) >> 2;
    *rxallowlegacy = (localVal & ((uint32_t)0x00000002)) >> 1;
    *rxallowdsss = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the PSSELECT40 field in the RXMODES register.
 *
 * The RXMODES register will be read and the PSSELECT40 field's value will be returned.
 *
 * @return The current value of the PSSELECT40 field in the RXMODES register.
 */
__INLINE uint8_t mdm_psselect40_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODES_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Sets the PSSELECT40 field of the RXMODES register.
 *
 * The RXMODES register will be read, modified to contain the new field value, and written.
 *
 * @param[in] psselect40 - The value to set the field to.
 */
__INLINE void mdm_psselect40_setf(uint8_t psselect40)
{
    ASSERT_ERR((((uint32_t)psselect40 << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(MDM_RXMODES_ADDR, (REG_PL_RD(MDM_RXMODES_ADDR) & ~((uint32_t)0x00020000)) | ((uint32_t)psselect40 << 17));
}

/**
 * @brief Returns the current value of the PSSELECT20 field in the RXMODES register.
 *
 * The RXMODES register will be read and the PSSELECT20 field's value will be returned.
 *
 * @return The current value of the PSSELECT20 field in the RXMODES register.
 */
__INLINE uint8_t mdm_psselect20_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODES_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the PSSELECT20 field of the RXMODES register.
 *
 * The RXMODES register will be read, modified to contain the new field value, and written.
 *
 * @param[in] psselect20 - The value to set the field to.
 */
__INLINE void mdm_psselect20_setf(uint8_t psselect20)
{
    ASSERT_ERR((((uint32_t)psselect20 << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(MDM_RXMODES_ADDR, (REG_PL_RD(MDM_RXMODES_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)psselect20 << 16));
}

/**
 * @brief Returns the current value of the FORCENSS1 field in the RXMODES register.
 *
 * The RXMODES register will be read and the FORCENSS1 field's value will be returned.
 *
 * @return The current value of the FORCENSS1 field in the RXMODES register.
 */
__INLINE uint8_t mdm_forcenss1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODES_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Sets the FORCENSS1 field of the RXMODES register.
 *
 * The RXMODES register will be read, modified to contain the new field value, and written.
 *
 * @param[in] forcenss1 - The value to set the field to.
 */
__INLINE void mdm_forcenss1_setf(uint8_t forcenss1)
{
    ASSERT_ERR((((uint32_t)forcenss1 << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(MDM_RXMODES_ADDR, (REG_PL_RD(MDM_RXMODES_ADDR) & ~((uint32_t)0x00004000)) | ((uint32_t)forcenss1 << 14));
}

/**
 * @brief Returns the current value of the FORCE40 field in the RXMODES register.
 *
 * The RXMODES register will be read and the FORCE40 field's value will be returned.
 *
 * @return The current value of the FORCE40 field in the RXMODES register.
 */
__INLINE uint8_t mdm_force40_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODES_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the FORCE40 field of the RXMODES register.
 *
 * The RXMODES register will be read, modified to contain the new field value, and written.
 *
 * @param[in] force40 - The value to set the field to.
 */
__INLINE void mdm_force40_setf(uint8_t force40)
{
    ASSERT_ERR((((uint32_t)force40 << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(MDM_RXMODES_ADDR, (REG_PL_RD(MDM_RXMODES_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)force40 << 13));
}

/**
 * @brief Returns the current value of the FORCE20 field in the RXMODES register.
 *
 * The RXMODES register will be read and the FORCE20 field's value will be returned.
 *
 * @return The current value of the FORCE20 field in the RXMODES register.
 */
__INLINE uint8_t mdm_force20_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODES_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the FORCE20 field of the RXMODES register.
 *
 * The RXMODES register will be read, modified to contain the new field value, and written.
 *
 * @param[in] force20 - The value to set the field to.
 */
__INLINE void mdm_force20_setf(uint8_t force20)
{
    ASSERT_ERR((((uint32_t)force20 << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(MDM_RXMODES_ADDR, (REG_PL_RD(MDM_RXMODES_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)force20 << 12));
}

/**
 * @brief Returns the current value of the RXSTBCEN field in the RXMODES register.
 *
 * The RXMODES register will be read and the RXSTBCEN field's value will be returned.
 *
 * @return The current value of the RXSTBCEN field in the RXMODES register.
 */
__INLINE uint8_t mdm_rxstbcen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODES_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the RXSTBCEN field of the RXMODES register.
 *
 * The RXMODES register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxstbcen - The value to set the field to.
 */
__INLINE void mdm_rxstbcen_setf(uint8_t rxstbcen)
{
    ASSERT_ERR((((uint32_t)rxstbcen << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(MDM_RXMODES_ADDR, (REG_PL_RD(MDM_RXMODES_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)rxstbcen << 9));
}

/**
 * @brief Returns the current value of the DUPMRCEN field in the RXMODES register.
 *
 * The RXMODES register will be read and the DUPMRCEN field's value will be returned.
 *
 * @return The current value of the DUPMRCEN field in the RXMODES register.
 */
__INLINE uint8_t mdm_dupmrcen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODES_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the DUPMRCEN field of the RXMODES register.
 *
 * The RXMODES register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dupmrcen - The value to set the field to.
 */
__INLINE void mdm_dupmrcen_setf(uint8_t dupmrcen)
{
    ASSERT_ERR((((uint32_t)dupmrcen << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(MDM_RXMODES_ADDR, (REG_PL_RD(MDM_RXMODES_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)dupmrcen << 8));
}

/**
 * @brief Returns the current value of the RXALLOWVHT field in the RXMODES register.
 *
 * The RXMODES register will be read and the RXALLOWVHT field's value will be returned.
 *
 * @return The current value of the RXALLOWVHT field in the RXMODES register.
 */
__INLINE uint8_t mdm_rxallowvht_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODES_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the RXALLOWVHT field of the RXMODES register.
 *
 * The RXMODES register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxallowvht - The value to set the field to.
 */
__INLINE void mdm_rxallowvht_setf(uint8_t rxallowvht)
{
    ASSERT_ERR((((uint32_t)rxallowvht << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(MDM_RXMODES_ADDR, (REG_PL_RD(MDM_RXMODES_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)rxallowvht << 4));
}

/**
 * @brief Returns the current value of the RXALLOWGF field in the RXMODES register.
 *
 * The RXMODES register will be read and the RXALLOWGF field's value will be returned.
 *
 * @return The current value of the RXALLOWGF field in the RXMODES register.
 */
__INLINE uint8_t mdm_rxallowgf_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODES_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the RXALLOWGF field of the RXMODES register.
 *
 * The RXMODES register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxallowgf - The value to set the field to.
 */
__INLINE void mdm_rxallowgf_setf(uint8_t rxallowgf)
{
    ASSERT_ERR((((uint32_t)rxallowgf << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MDM_RXMODES_ADDR, (REG_PL_RD(MDM_RXMODES_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)rxallowgf << 3));
}

/**
 * @brief Returns the current value of the RXALLOWMM field in the RXMODES register.
 *
 * The RXMODES register will be read and the RXALLOWMM field's value will be returned.
 *
 * @return The current value of the RXALLOWMM field in the RXMODES register.
 */
__INLINE uint8_t mdm_rxallowmm_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODES_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the RXALLOWMM field of the RXMODES register.
 *
 * The RXMODES register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxallowmm - The value to set the field to.
 */
__INLINE void mdm_rxallowmm_setf(uint8_t rxallowmm)
{
    ASSERT_ERR((((uint32_t)rxallowmm << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(MDM_RXMODES_ADDR, (REG_PL_RD(MDM_RXMODES_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)rxallowmm << 2));
}

/**
 * @brief Returns the current value of the RXALLOWLEGACY field in the RXMODES register.
 *
 * The RXMODES register will be read and the RXALLOWLEGACY field's value will be returned.
 *
 * @return The current value of the RXALLOWLEGACY field in the RXMODES register.
 */
__INLINE uint8_t mdm_rxallowlegacy_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODES_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the RXALLOWLEGACY field of the RXMODES register.
 *
 * The RXMODES register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxallowlegacy - The value to set the field to.
 */
__INLINE void mdm_rxallowlegacy_setf(uint8_t rxallowlegacy)
{
    ASSERT_ERR((((uint32_t)rxallowlegacy << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(MDM_RXMODES_ADDR, (REG_PL_RD(MDM_RXMODES_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)rxallowlegacy << 1));
}

/**
 * @brief Returns the current value of the RXALLOWDSSS field in the RXMODES register.
 *
 * The RXMODES register will be read and the RXALLOWDSSS field's value will be returned.
 *
 * @return The current value of the RXALLOWDSSS field in the RXMODES register.
 */
__INLINE uint8_t mdm_rxallowdsss_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODES_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the RXALLOWDSSS field of the RXMODES register.
 *
 * The RXMODES register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxallowdsss - The value to set the field to.
 */
__INLINE void mdm_rxallowdsss_setf(uint8_t rxallowdsss)
{
    ASSERT_ERR((((uint32_t)rxallowdsss << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_RXMODES_ADDR, (REG_PL_RD(MDM_RXMODES_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)rxallowdsss << 0));
}

/// @}

/**
 * @name RXTXPWRCTRL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     02                 TXON   1
 *     01             AGCCCAON   1
 *     00                 RXON   1
 * </pre>
 *
 * @{
 */

/// Address of the RXTXPWRCTRL register
#define MDM_RXTXPWRCTRL_ADDR   0x01000804
/// Offset of the RXTXPWRCTRL register from the base address
#define MDM_RXTXPWRCTRL_OFFSET 0x00000804
/// Index of the RXTXPWRCTRL register
#define MDM_RXTXPWRCTRL_INDEX  0x00000201
/// Reset value of the RXTXPWRCTRL register
#define MDM_RXTXPWRCTRL_RESET  0x00000007

/**
 * @brief Returns the current value of the RXTXPWRCTRL register.
 * The RXTXPWRCTRL register will be read and its value returned.
 * @return The current value of the RXTXPWRCTRL register.
 */
__INLINE uint32_t mdm_rxtxpwrctrl_get(void)
{
    return REG_PL_RD(MDM_RXTXPWRCTRL_ADDR);
}

/**
 * @brief Sets the RXTXPWRCTRL register to a value.
 * The RXTXPWRCTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxtxpwrctrl_set(uint32_t value)
{
    REG_PL_WR(MDM_RXTXPWRCTRL_ADDR, value);
}

// field definitions
/// TXON field bit
#define MDM_TXON_BIT        ((uint32_t)0x00000004)
/// TXON field position
#define MDM_TXON_POS        2
/// AGCCCAON field bit
#define MDM_AGCCCAON_BIT    ((uint32_t)0x00000002)
/// AGCCCAON field position
#define MDM_AGCCCAON_POS    1
/// RXON field bit
#define MDM_RXON_BIT        ((uint32_t)0x00000001)
/// RXON field position
#define MDM_RXON_POS        0

/// TXON field reset value
#define MDM_TXON_RST        0x1
/// AGCCCAON field reset value
#define MDM_AGCCCAON_RST    0x1
/// RXON field reset value
#define MDM_RXON_RST        0x1

/**
 * @brief Constructs a value for the RXTXPWRCTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] txon - The value to use for the TXON field.
 * @param[in] agcccaon - The value to use for the AGCCCAON field.
 * @param[in] rxon - The value to use for the RXON field.
 */
__INLINE void mdm_rxtxpwrctrl_pack(uint8_t txon, uint8_t agcccaon, uint8_t rxon)
{
    ASSERT_ERR((((uint32_t)txon << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)agcccaon << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)rxon << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_RXTXPWRCTRL_ADDR,  ((uint32_t)txon << 2) | ((uint32_t)agcccaon << 1) | ((uint32_t)rxon << 0));
}

/**
 * @brief Unpacks RXTXPWRCTRL's fields from current value of the RXTXPWRCTRL register.
 *
 * Reads the RXTXPWRCTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txon - Will be populated with the current value of this field from the register.
 * @param[out] agcccaon - Will be populated with the current value of this field from the register.
 * @param[out] rxon - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxtxpwrctrl_unpack(uint8_t *txon, uint8_t *agcccaon, uint8_t *rxon)
{
    uint32_t localVal = REG_PL_RD(MDM_RXTXPWRCTRL_ADDR);

    *txon = (localVal & ((uint32_t)0x00000004)) >> 2;
    *agcccaon = (localVal & ((uint32_t)0x00000002)) >> 1;
    *rxon = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the TXON field in the RXTXPWRCTRL register.
 *
 * The RXTXPWRCTRL register will be read and the TXON field's value will be returned.
 *
 * @return The current value of the TXON field in the RXTXPWRCTRL register.
 */
__INLINE uint8_t mdm_txon_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXTXPWRCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the TXON field of the RXTXPWRCTRL register.
 *
 * The RXTXPWRCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txon - The value to set the field to.
 */
__INLINE void mdm_txon_setf(uint8_t txon)
{
    ASSERT_ERR((((uint32_t)txon << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(MDM_RXTXPWRCTRL_ADDR, (REG_PL_RD(MDM_RXTXPWRCTRL_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)txon << 2));
}

/**
 * @brief Returns the current value of the AGCCCAON field in the RXTXPWRCTRL register.
 *
 * The RXTXPWRCTRL register will be read and the AGCCCAON field's value will be returned.
 *
 * @return The current value of the AGCCCAON field in the RXTXPWRCTRL register.
 */
__INLINE uint8_t mdm_agcccaon_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXTXPWRCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the AGCCCAON field of the RXTXPWRCTRL register.
 *
 * The RXTXPWRCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] agcccaon - The value to set the field to.
 */
__INLINE void mdm_agcccaon_setf(uint8_t agcccaon)
{
    ASSERT_ERR((((uint32_t)agcccaon << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(MDM_RXTXPWRCTRL_ADDR, (REG_PL_RD(MDM_RXTXPWRCTRL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)agcccaon << 1));
}

/**
 * @brief Returns the current value of the RXON field in the RXTXPWRCTRL register.
 *
 * The RXTXPWRCTRL register will be read and the RXON field's value will be returned.
 *
 * @return The current value of the RXON field in the RXTXPWRCTRL register.
 */
__INLINE uint8_t mdm_rxon_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXTXPWRCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the RXON field of the RXTXPWRCTRL register.
 *
 * The RXTXPWRCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxon - The value to set the field to.
 */
__INLINE void mdm_rxon_setf(uint8_t rxon)
{
    ASSERT_ERR((((uint32_t)rxon << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_RXTXPWRCTRL_ADDR, (REG_PL_RD(MDM_RXTXPWRCTRL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)rxon << 0));
}

/// @}

/**
 * @name STOCPECTRL0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31        CFGSTO4TDCOMP   1
 *     30           CFGCPEWALK   1
 *     29            CFGSTOREF   1
 *     28            CFGCPEREF   1
 *     25         CPESLOPELGHT   1
 *     24         STOSLOPELGHT   1
 *     23              CPEMODE   1
 *     22              STOMODE   1
 *  16:11       MAXNTDSYNCHOFF   0x1F
 *  10:05       MAXPTDSYNCHOFF   0x1F
 *     04          STOFDCOMPEN   1
 *     03        CPESLOPEESTEN   1
 *     02          CPEFDCOMPEN   1
 *     01        STOSLOPEESTEN   1
 *     00          STOTDCOMPEN   1
 * </pre>
 *
 * @{
 */

/// Address of the STOCPECTRL0 register
#define MDM_STOCPECTRL0_ADDR   0x01000808
/// Offset of the STOCPECTRL0 register from the base address
#define MDM_STOCPECTRL0_OFFSET 0x00000808
/// Index of the STOCPECTRL0 register
#define MDM_STOCPECTRL0_INDEX  0x00000202
/// Reset value of the STOCPECTRL0 register
#define MDM_STOCPECTRL0_RESET  0xF3C0FBFF

/**
 * @brief Returns the current value of the STOCPECTRL0 register.
 * The STOCPECTRL0 register will be read and its value returned.
 * @return The current value of the STOCPECTRL0 register.
 */
__INLINE uint32_t mdm_stocpectrl0_get(void)
{
    return REG_PL_RD(MDM_STOCPECTRL0_ADDR);
}

/**
 * @brief Sets the STOCPECTRL0 register to a value.
 * The STOCPECTRL0 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_stocpectrl0_set(uint32_t value)
{
    REG_PL_WR(MDM_STOCPECTRL0_ADDR, value);
}

// field definitions
/// CFGSTO4TDCOMP field bit
#define MDM_CFGSTO4TDCOMP_BIT     ((uint32_t)0x80000000)
/// CFGSTO4TDCOMP field position
#define MDM_CFGSTO4TDCOMP_POS     31
/// CFGCPEWALK field bit
#define MDM_CFGCPEWALK_BIT        ((uint32_t)0x40000000)
/// CFGCPEWALK field position
#define MDM_CFGCPEWALK_POS        30
/// CFGSTOREF field bit
#define MDM_CFGSTOREF_BIT         ((uint32_t)0x20000000)
/// CFGSTOREF field position
#define MDM_CFGSTOREF_POS         29
/// CFGCPEREF field bit
#define MDM_CFGCPEREF_BIT         ((uint32_t)0x10000000)
/// CFGCPEREF field position
#define MDM_CFGCPEREF_POS         28
/// CPESLOPELGHT field bit
#define MDM_CPESLOPELGHT_BIT      ((uint32_t)0x02000000)
/// CPESLOPELGHT field position
#define MDM_CPESLOPELGHT_POS      25
/// STOSLOPELGHT field bit
#define MDM_STOSLOPELGHT_BIT      ((uint32_t)0x01000000)
/// STOSLOPELGHT field position
#define MDM_STOSLOPELGHT_POS      24
/// CPEMODE field bit
#define MDM_CPEMODE_BIT           ((uint32_t)0x00800000)
/// CPEMODE field position
#define MDM_CPEMODE_POS           23
/// STOMODE field bit
#define MDM_STOMODE_BIT           ((uint32_t)0x00400000)
/// STOMODE field position
#define MDM_STOMODE_POS           22
/// MAXNTDSYNCHOFF field mask
#define MDM_MAXNTDSYNCHOFF_MASK   ((uint32_t)0x0001F800)
/// MAXNTDSYNCHOFF field LSB position
#define MDM_MAXNTDSYNCHOFF_LSB    11
/// MAXNTDSYNCHOFF field width
#define MDM_MAXNTDSYNCHOFF_WIDTH  ((uint32_t)0x00000006)
/// MAXPTDSYNCHOFF field mask
#define MDM_MAXPTDSYNCHOFF_MASK   ((uint32_t)0x000007E0)
/// MAXPTDSYNCHOFF field LSB position
#define MDM_MAXPTDSYNCHOFF_LSB    5
/// MAXPTDSYNCHOFF field width
#define MDM_MAXPTDSYNCHOFF_WIDTH  ((uint32_t)0x00000006)
/// STOFDCOMPEN field bit
#define MDM_STOFDCOMPEN_BIT       ((uint32_t)0x00000010)
/// STOFDCOMPEN field position
#define MDM_STOFDCOMPEN_POS       4
/// CPESLOPEESTEN field bit
#define MDM_CPESLOPEESTEN_BIT     ((uint32_t)0x00000008)
/// CPESLOPEESTEN field position
#define MDM_CPESLOPEESTEN_POS     3
/// CPEFDCOMPEN field bit
#define MDM_CPEFDCOMPEN_BIT       ((uint32_t)0x00000004)
/// CPEFDCOMPEN field position
#define MDM_CPEFDCOMPEN_POS       2
/// STOSLOPEESTEN field bit
#define MDM_STOSLOPEESTEN_BIT     ((uint32_t)0x00000002)
/// STOSLOPEESTEN field position
#define MDM_STOSLOPEESTEN_POS     1
/// STOTDCOMPEN field bit
#define MDM_STOTDCOMPEN_BIT       ((uint32_t)0x00000001)
/// STOTDCOMPEN field position
#define MDM_STOTDCOMPEN_POS       0

/// CFGSTO4TDCOMP field reset value
#define MDM_CFGSTO4TDCOMP_RST     0x1
/// CFGCPEWALK field reset value
#define MDM_CFGCPEWALK_RST        0x1
/// CFGSTOREF field reset value
#define MDM_CFGSTOREF_RST         0x1
/// CFGCPEREF field reset value
#define MDM_CFGCPEREF_RST         0x1
/// CPESLOPELGHT field reset value
#define MDM_CPESLOPELGHT_RST      0x1
/// STOSLOPELGHT field reset value
#define MDM_STOSLOPELGHT_RST      0x1
/// CPEMODE field reset value
#define MDM_CPEMODE_RST           0x1
/// STOMODE field reset value
#define MDM_STOMODE_RST           0x1
/// MAXNTDSYNCHOFF field reset value
#define MDM_MAXNTDSYNCHOFF_RST    0x1F
/// MAXPTDSYNCHOFF field reset value
#define MDM_MAXPTDSYNCHOFF_RST    0x1F
/// STOFDCOMPEN field reset value
#define MDM_STOFDCOMPEN_RST       0x1
/// CPESLOPEESTEN field reset value
#define MDM_CPESLOPEESTEN_RST     0x1
/// CPEFDCOMPEN field reset value
#define MDM_CPEFDCOMPEN_RST       0x1
/// STOSLOPEESTEN field reset value
#define MDM_STOSLOPEESTEN_RST     0x1
/// STOTDCOMPEN field reset value
#define MDM_STOTDCOMPEN_RST       0x1

/**
 * @brief Constructs a value for the STOCPECTRL0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] cfgsto4tdcomp - The value to use for the CFGSTO4TDCOMP field.
 * @param[in] cfgcpewalk - The value to use for the CFGCPEWALK field.
 * @param[in] cfgstoref - The value to use for the CFGSTOREF field.
 * @param[in] cfgcperef - The value to use for the CFGCPEREF field.
 * @param[in] cpeslopelght - The value to use for the CPESLOPELGHT field.
 * @param[in] stoslopelght - The value to use for the STOSLOPELGHT field.
 * @param[in] cpemode - The value to use for the CPEMODE field.
 * @param[in] stomode - The value to use for the STOMODE field.
 * @param[in] maxntdsynchoff - The value to use for the MAXNTDSYNCHOFF field.
 * @param[in] maxptdsynchoff - The value to use for the MAXPTDSYNCHOFF field.
 * @param[in] stofdcompen - The value to use for the STOFDCOMPEN field.
 * @param[in] cpeslopeesten - The value to use for the CPESLOPEESTEN field.
 * @param[in] cpefdcompen - The value to use for the CPEFDCOMPEN field.
 * @param[in] stoslopeesten - The value to use for the STOSLOPEESTEN field.
 * @param[in] stotdcompen - The value to use for the STOTDCOMPEN field.
 */
__INLINE void mdm_stocpectrl0_pack(uint8_t cfgsto4tdcomp, uint8_t cfgcpewalk, uint8_t cfgstoref, uint8_t cfgcperef, uint8_t cpeslopelght, uint8_t stoslopelght, uint8_t cpemode, uint8_t stomode, uint8_t maxntdsynchoff, uint8_t maxptdsynchoff, uint8_t stofdcompen, uint8_t cpeslopeesten, uint8_t cpefdcompen, uint8_t stoslopeesten, uint8_t stotdcompen)
{
    ASSERT_ERR((((uint32_t)cfgsto4tdcomp << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)cfgcpewalk << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)cfgstoref << 29) & ~((uint32_t)0x20000000)) == 0);
    ASSERT_ERR((((uint32_t)cfgcperef << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)cpeslopelght << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)stoslopelght << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)cpemode << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)stomode << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)maxntdsynchoff << 11) & ~((uint32_t)0x0001F800)) == 0);
    ASSERT_ERR((((uint32_t)maxptdsynchoff << 5) & ~((uint32_t)0x000007E0)) == 0);
    ASSERT_ERR((((uint32_t)stofdcompen << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)cpeslopeesten << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)cpefdcompen << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)stoslopeesten << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)stotdcompen << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_STOCPECTRL0_ADDR,  ((uint32_t)cfgsto4tdcomp << 31) | ((uint32_t)cfgcpewalk << 30) | ((uint32_t)cfgstoref << 29) | ((uint32_t)cfgcperef << 28) | ((uint32_t)cpeslopelght << 25) | ((uint32_t)stoslopelght << 24) | ((uint32_t)cpemode << 23) | ((uint32_t)stomode << 22) | ((uint32_t)maxntdsynchoff << 11) | ((uint32_t)maxptdsynchoff << 5) | ((uint32_t)stofdcompen << 4) | ((uint32_t)cpeslopeesten << 3) | ((uint32_t)cpefdcompen << 2) | ((uint32_t)stoslopeesten << 1) | ((uint32_t)stotdcompen << 0));
}

/**
 * @brief Unpacks STOCPECTRL0's fields from current value of the STOCPECTRL0 register.
 *
 * Reads the STOCPECTRL0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] cfgsto4tdcomp - Will be populated with the current value of this field from the register.
 * @param[out] cfgcpewalk - Will be populated with the current value of this field from the register.
 * @param[out] cfgstoref - Will be populated with the current value of this field from the register.
 * @param[out] cfgcperef - Will be populated with the current value of this field from the register.
 * @param[out] cpeslopelght - Will be populated with the current value of this field from the register.
 * @param[out] stoslopelght - Will be populated with the current value of this field from the register.
 * @param[out] cpemode - Will be populated with the current value of this field from the register.
 * @param[out] stomode - Will be populated with the current value of this field from the register.
 * @param[out] maxntdsynchoff - Will be populated with the current value of this field from the register.
 * @param[out] maxptdsynchoff - Will be populated with the current value of this field from the register.
 * @param[out] stofdcompen - Will be populated with the current value of this field from the register.
 * @param[out] cpeslopeesten - Will be populated with the current value of this field from the register.
 * @param[out] cpefdcompen - Will be populated with the current value of this field from the register.
 * @param[out] stoslopeesten - Will be populated with the current value of this field from the register.
 * @param[out] stotdcompen - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_stocpectrl0_unpack(uint8_t *cfgsto4tdcomp, uint8_t *cfgcpewalk, uint8_t *cfgstoref, uint8_t *cfgcperef, uint8_t *cpeslopelght, uint8_t *stoslopelght, uint8_t *cpemode, uint8_t *stomode, uint8_t *maxntdsynchoff, uint8_t *maxptdsynchoff, uint8_t *stofdcompen, uint8_t *cpeslopeesten, uint8_t *cpefdcompen, uint8_t *stoslopeesten, uint8_t *stotdcompen)
{
    uint32_t localVal = REG_PL_RD(MDM_STOCPECTRL0_ADDR);

    *cfgsto4tdcomp = (localVal & ((uint32_t)0x80000000)) >> 31;
    *cfgcpewalk = (localVal & ((uint32_t)0x40000000)) >> 30;
    *cfgstoref = (localVal & ((uint32_t)0x20000000)) >> 29;
    *cfgcperef = (localVal & ((uint32_t)0x10000000)) >> 28;
    *cpeslopelght = (localVal & ((uint32_t)0x02000000)) >> 25;
    *stoslopelght = (localVal & ((uint32_t)0x01000000)) >> 24;
    *cpemode = (localVal & ((uint32_t)0x00800000)) >> 23;
    *stomode = (localVal & ((uint32_t)0x00400000)) >> 22;
    *maxntdsynchoff = (localVal & ((uint32_t)0x0001F800)) >> 11;
    *maxptdsynchoff = (localVal & ((uint32_t)0x000007E0)) >> 5;
    *stofdcompen = (localVal & ((uint32_t)0x00000010)) >> 4;
    *cpeslopeesten = (localVal & ((uint32_t)0x00000008)) >> 3;
    *cpefdcompen = (localVal & ((uint32_t)0x00000004)) >> 2;
    *stoslopeesten = (localVal & ((uint32_t)0x00000002)) >> 1;
    *stotdcompen = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the CFGSTO4TDCOMP field in the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read and the CFGSTO4TDCOMP field's value will be returned.
 *
 * @return The current value of the CFGSTO4TDCOMP field in the STOCPECTRL0 register.
 */
__INLINE uint8_t mdm_cfgsto4tdcomp_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_STOCPECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the CFGSTO4TDCOMP field of the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cfgsto4tdcomp - The value to set the field to.
 */
__INLINE void mdm_cfgsto4tdcomp_setf(uint8_t cfgsto4tdcomp)
{
    ASSERT_ERR((((uint32_t)cfgsto4tdcomp << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(MDM_STOCPECTRL0_ADDR, (REG_PL_RD(MDM_STOCPECTRL0_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)cfgsto4tdcomp << 31));
}

/**
 * @brief Returns the current value of the CFGCPEWALK field in the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read and the CFGCPEWALK field's value will be returned.
 *
 * @return The current value of the CFGCPEWALK field in the STOCPECTRL0 register.
 */
__INLINE uint8_t mdm_cfgcpewalk_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_STOCPECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

/**
 * @brief Sets the CFGCPEWALK field of the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cfgcpewalk - The value to set the field to.
 */
__INLINE void mdm_cfgcpewalk_setf(uint8_t cfgcpewalk)
{
    ASSERT_ERR((((uint32_t)cfgcpewalk << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_PL_WR(MDM_STOCPECTRL0_ADDR, (REG_PL_RD(MDM_STOCPECTRL0_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)cfgcpewalk << 30));
}

/**
 * @brief Returns the current value of the CFGSTOREF field in the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read and the CFGSTOREF field's value will be returned.
 *
 * @return The current value of the CFGSTOREF field in the STOCPECTRL0 register.
 */
__INLINE uint8_t mdm_cfgstoref_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_STOCPECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

/**
 * @brief Sets the CFGSTOREF field of the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cfgstoref - The value to set the field to.
 */
__INLINE void mdm_cfgstoref_setf(uint8_t cfgstoref)
{
    ASSERT_ERR((((uint32_t)cfgstoref << 29) & ~((uint32_t)0x20000000)) == 0);
    REG_PL_WR(MDM_STOCPECTRL0_ADDR, (REG_PL_RD(MDM_STOCPECTRL0_ADDR) & ~((uint32_t)0x20000000)) | ((uint32_t)cfgstoref << 29));
}

/**
 * @brief Returns the current value of the CFGCPEREF field in the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read and the CFGCPEREF field's value will be returned.
 *
 * @return The current value of the CFGCPEREF field in the STOCPECTRL0 register.
 */
__INLINE uint8_t mdm_cfgcperef_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_STOCPECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the CFGCPEREF field of the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cfgcperef - The value to set the field to.
 */
__INLINE void mdm_cfgcperef_setf(uint8_t cfgcperef)
{
    ASSERT_ERR((((uint32_t)cfgcperef << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(MDM_STOCPECTRL0_ADDR, (REG_PL_RD(MDM_STOCPECTRL0_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)cfgcperef << 28));
}

/**
 * @brief Returns the current value of the CPESLOPELGHT field in the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read and the CPESLOPELGHT field's value will be returned.
 *
 * @return The current value of the CPESLOPELGHT field in the STOCPECTRL0 register.
 */
__INLINE uint8_t mdm_cpeslopelght_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_STOCPECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

/**
 * @brief Sets the CPESLOPELGHT field of the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cpeslopelght - The value to set the field to.
 */
__INLINE void mdm_cpeslopelght_setf(uint8_t cpeslopelght)
{
    ASSERT_ERR((((uint32_t)cpeslopelght << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(MDM_STOCPECTRL0_ADDR, (REG_PL_RD(MDM_STOCPECTRL0_ADDR) & ~((uint32_t)0x02000000)) | ((uint32_t)cpeslopelght << 25));
}

/**
 * @brief Returns the current value of the STOSLOPELGHT field in the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read and the STOSLOPELGHT field's value will be returned.
 *
 * @return The current value of the STOSLOPELGHT field in the STOCPECTRL0 register.
 */
__INLINE uint8_t mdm_stoslopelght_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_STOCPECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the STOSLOPELGHT field of the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] stoslopelght - The value to set the field to.
 */
__INLINE void mdm_stoslopelght_setf(uint8_t stoslopelght)
{
    ASSERT_ERR((((uint32_t)stoslopelght << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(MDM_STOCPECTRL0_ADDR, (REG_PL_RD(MDM_STOCPECTRL0_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)stoslopelght << 24));
}

/**
 * @brief Returns the current value of the CPEMODE field in the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read and the CPEMODE field's value will be returned.
 *
 * @return The current value of the CPEMODE field in the STOCPECTRL0 register.
 */
__INLINE uint8_t mdm_cpemode_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_STOCPECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Sets the CPEMODE field of the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cpemode - The value to set the field to.
 */
__INLINE void mdm_cpemode_setf(uint8_t cpemode)
{
    ASSERT_ERR((((uint32_t)cpemode << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(MDM_STOCPECTRL0_ADDR, (REG_PL_RD(MDM_STOCPECTRL0_ADDR) & ~((uint32_t)0x00800000)) | ((uint32_t)cpemode << 23));
}

/**
 * @brief Returns the current value of the STOMODE field in the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read and the STOMODE field's value will be returned.
 *
 * @return The current value of the STOMODE field in the STOCPECTRL0 register.
 */
__INLINE uint8_t mdm_stomode_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_STOCPECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Sets the STOMODE field of the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] stomode - The value to set the field to.
 */
__INLINE void mdm_stomode_setf(uint8_t stomode)
{
    ASSERT_ERR((((uint32_t)stomode << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_PL_WR(MDM_STOCPECTRL0_ADDR, (REG_PL_RD(MDM_STOCPECTRL0_ADDR) & ~((uint32_t)0x00400000)) | ((uint32_t)stomode << 22));
}

/**
 * @brief Returns the current value of the MAXNTDSYNCHOFF field in the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read and the MAXNTDSYNCHOFF field's value will be returned.
 *
 * @return The current value of the MAXNTDSYNCHOFF field in the STOCPECTRL0 register.
 */
__INLINE uint8_t mdm_maxntdsynchoff_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_STOCPECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x0001F800)) >> 11);
}

/**
 * @brief Sets the MAXNTDSYNCHOFF field of the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maxntdsynchoff - The value to set the field to.
 */
__INLINE void mdm_maxntdsynchoff_setf(uint8_t maxntdsynchoff)
{
    ASSERT_ERR((((uint32_t)maxntdsynchoff << 11) & ~((uint32_t)0x0001F800)) == 0);
    REG_PL_WR(MDM_STOCPECTRL0_ADDR, (REG_PL_RD(MDM_STOCPECTRL0_ADDR) & ~((uint32_t)0x0001F800)) | ((uint32_t)maxntdsynchoff << 11));
}

/**
 * @brief Returns the current value of the MAXPTDSYNCHOFF field in the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read and the MAXPTDSYNCHOFF field's value will be returned.
 *
 * @return The current value of the MAXPTDSYNCHOFF field in the STOCPECTRL0 register.
 */
__INLINE uint8_t mdm_maxptdsynchoff_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_STOCPECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x000007E0)) >> 5);
}

/**
 * @brief Sets the MAXPTDSYNCHOFF field of the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maxptdsynchoff - The value to set the field to.
 */
__INLINE void mdm_maxptdsynchoff_setf(uint8_t maxptdsynchoff)
{
    ASSERT_ERR((((uint32_t)maxptdsynchoff << 5) & ~((uint32_t)0x000007E0)) == 0);
    REG_PL_WR(MDM_STOCPECTRL0_ADDR, (REG_PL_RD(MDM_STOCPECTRL0_ADDR) & ~((uint32_t)0x000007E0)) | ((uint32_t)maxptdsynchoff << 5));
}

/**
 * @brief Returns the current value of the STOFDCOMPEN field in the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read and the STOFDCOMPEN field's value will be returned.
 *
 * @return The current value of the STOFDCOMPEN field in the STOCPECTRL0 register.
 */
__INLINE uint8_t mdm_stofdcompen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_STOCPECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the STOFDCOMPEN field of the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] stofdcompen - The value to set the field to.
 */
__INLINE void mdm_stofdcompen_setf(uint8_t stofdcompen)
{
    ASSERT_ERR((((uint32_t)stofdcompen << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(MDM_STOCPECTRL0_ADDR, (REG_PL_RD(MDM_STOCPECTRL0_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)stofdcompen << 4));
}

/**
 * @brief Returns the current value of the CPESLOPEESTEN field in the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read and the CPESLOPEESTEN field's value will be returned.
 *
 * @return The current value of the CPESLOPEESTEN field in the STOCPECTRL0 register.
 */
__INLINE uint8_t mdm_cpeslopeesten_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_STOCPECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the CPESLOPEESTEN field of the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cpeslopeesten - The value to set the field to.
 */
__INLINE void mdm_cpeslopeesten_setf(uint8_t cpeslopeesten)
{
    ASSERT_ERR((((uint32_t)cpeslopeesten << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MDM_STOCPECTRL0_ADDR, (REG_PL_RD(MDM_STOCPECTRL0_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)cpeslopeesten << 3));
}

/**
 * @brief Returns the current value of the CPEFDCOMPEN field in the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read and the CPEFDCOMPEN field's value will be returned.
 *
 * @return The current value of the CPEFDCOMPEN field in the STOCPECTRL0 register.
 */
__INLINE uint8_t mdm_cpefdcompen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_STOCPECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the CPEFDCOMPEN field of the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cpefdcompen - The value to set the field to.
 */
__INLINE void mdm_cpefdcompen_setf(uint8_t cpefdcompen)
{
    ASSERT_ERR((((uint32_t)cpefdcompen << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(MDM_STOCPECTRL0_ADDR, (REG_PL_RD(MDM_STOCPECTRL0_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)cpefdcompen << 2));
}

/**
 * @brief Returns the current value of the STOSLOPEESTEN field in the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read and the STOSLOPEESTEN field's value will be returned.
 *
 * @return The current value of the STOSLOPEESTEN field in the STOCPECTRL0 register.
 */
__INLINE uint8_t mdm_stoslopeesten_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_STOCPECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the STOSLOPEESTEN field of the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] stoslopeesten - The value to set the field to.
 */
__INLINE void mdm_stoslopeesten_setf(uint8_t stoslopeesten)
{
    ASSERT_ERR((((uint32_t)stoslopeesten << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(MDM_STOCPECTRL0_ADDR, (REG_PL_RD(MDM_STOCPECTRL0_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)stoslopeesten << 1));
}

/**
 * @brief Returns the current value of the STOTDCOMPEN field in the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read and the STOTDCOMPEN field's value will be returned.
 *
 * @return The current value of the STOTDCOMPEN field in the STOCPECTRL0 register.
 */
__INLINE uint8_t mdm_stotdcompen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_STOCPECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the STOTDCOMPEN field of the STOCPECTRL0 register.
 *
 * The STOCPECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] stotdcompen - The value to set the field to.
 */
__INLINE void mdm_stotdcompen_setf(uint8_t stotdcompen)
{
    ASSERT_ERR((((uint32_t)stotdcompen << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_STOCPECTRL0_ADDR, (REG_PL_RD(MDM_STOCPECTRL0_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)stotdcompen << 0));
}

/// @}

/**
 * @name STOCPECTRL1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  17:00       STOSLOPEFORCED   0x0
 * </pre>
 *
 * @{
 */

/// Address of the STOCPECTRL1 register
#define MDM_STOCPECTRL1_ADDR   0x0100080C
/// Offset of the STOCPECTRL1 register from the base address
#define MDM_STOCPECTRL1_OFFSET 0x0000080C
/// Index of the STOCPECTRL1 register
#define MDM_STOCPECTRL1_INDEX  0x00000203
/// Reset value of the STOCPECTRL1 register
#define MDM_STOCPECTRL1_RESET  0x00000000

/**
 * @brief Returns the current value of the STOCPECTRL1 register.
 * The STOCPECTRL1 register will be read and its value returned.
 * @return The current value of the STOCPECTRL1 register.
 */
__INLINE uint32_t mdm_stocpectrl1_get(void)
{
    return REG_PL_RD(MDM_STOCPECTRL1_ADDR);
}

/**
 * @brief Sets the STOCPECTRL1 register to a value.
 * The STOCPECTRL1 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_stocpectrl1_set(uint32_t value)
{
    REG_PL_WR(MDM_STOCPECTRL1_ADDR, value);
}

// field definitions
/// STOSLOPEFORCED field mask
#define MDM_STOSLOPEFORCED_MASK   ((uint32_t)0x0003FFFF)
/// STOSLOPEFORCED field LSB position
#define MDM_STOSLOPEFORCED_LSB    0
/// STOSLOPEFORCED field width
#define MDM_STOSLOPEFORCED_WIDTH  ((uint32_t)0x00000012)

/// STOSLOPEFORCED field reset value
#define MDM_STOSLOPEFORCED_RST    0x0

/**
 * @brief Returns the current value of the STOSLOPEFORCED field in the STOCPECTRL1 register.
 *
 * The STOCPECTRL1 register will be read and the STOSLOPEFORCED field's value will be returned.
 *
 * @return The current value of the STOSLOPEFORCED field in the STOCPECTRL1 register.
 */
__INLINE uint32_t mdm_stoslopeforced_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_STOCPECTRL1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0003FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the STOSLOPEFORCED field of the STOCPECTRL1 register.
 *
 * The STOCPECTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] stoslopeforced - The value to set the field to.
 */
__INLINE void mdm_stoslopeforced_setf(uint32_t stoslopeforced)
{
    ASSERT_ERR((((uint32_t)stoslopeforced << 0) & ~((uint32_t)0x0003FFFF)) == 0);
    REG_PL_WR(MDM_STOCPECTRL1_ADDR, (uint32_t)stoslopeforced << 0);
}

/// @}

/**
 * @name STOCPECTRL2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  17:00       CPESLOPEFORCED   0x0
 * </pre>
 *
 * @{
 */

/// Address of the STOCPECTRL2 register
#define MDM_STOCPECTRL2_ADDR   0x01000810
/// Offset of the STOCPECTRL2 register from the base address
#define MDM_STOCPECTRL2_OFFSET 0x00000810
/// Index of the STOCPECTRL2 register
#define MDM_STOCPECTRL2_INDEX  0x00000204
/// Reset value of the STOCPECTRL2 register
#define MDM_STOCPECTRL2_RESET  0x00000000

/**
 * @brief Returns the current value of the STOCPECTRL2 register.
 * The STOCPECTRL2 register will be read and its value returned.
 * @return The current value of the STOCPECTRL2 register.
 */
__INLINE uint32_t mdm_stocpectrl2_get(void)
{
    return REG_PL_RD(MDM_STOCPECTRL2_ADDR);
}

/**
 * @brief Sets the STOCPECTRL2 register to a value.
 * The STOCPECTRL2 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_stocpectrl2_set(uint32_t value)
{
    REG_PL_WR(MDM_STOCPECTRL2_ADDR, value);
}

// field definitions
/// CPESLOPEFORCED field mask
#define MDM_CPESLOPEFORCED_MASK   ((uint32_t)0x0003FFFF)
/// CPESLOPEFORCED field LSB position
#define MDM_CPESLOPEFORCED_LSB    0
/// CPESLOPEFORCED field width
#define MDM_CPESLOPEFORCED_WIDTH  ((uint32_t)0x00000012)

/// CPESLOPEFORCED field reset value
#define MDM_CPESLOPEFORCED_RST    0x0

/**
 * @brief Returns the current value of the CPESLOPEFORCED field in the STOCPECTRL2 register.
 *
 * The STOCPECTRL2 register will be read and the CPESLOPEFORCED field's value will be returned.
 *
 * @return The current value of the CPESLOPEFORCED field in the STOCPECTRL2 register.
 */
__INLINE uint32_t mdm_cpeslopeforced_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_STOCPECTRL2_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0003FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the CPESLOPEFORCED field of the STOCPECTRL2 register.
 *
 * The STOCPECTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cpeslopeforced - The value to set the field to.
 */
__INLINE void mdm_cpeslopeforced_setf(uint32_t cpeslopeforced)
{
    ASSERT_ERR((((uint32_t)cpeslopeforced << 0) & ~((uint32_t)0x0003FFFF)) == 0);
    REG_PL_WR(MDM_STOCPECTRL2_ADDR, (uint32_t)cpeslopeforced << 0);
}

/// @}

/**
 * @name EQUALCTRL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:29         FDDCWEIGHTP2   0x2
 *  28:26         FDDCWEIGHTP1   0x3
 *  25:23         FDDCWEIGHTM1   0x3
 *  22:20         FDDCWEIGHTM2   0x2
 *  18:16          QPBSKNSDTHR   0x4
 *  15:12          QBPSKDETTHR   0xC
 *  11:04     FDDCSNRTHRESHOLD   0x18
 *     03           FDDCCOMPEN   1
 *  02:00        EQUALCFGSATSB   0x3
 * </pre>
 *
 * @{
 */

/// Address of the EQUALCTRL register
#define MDM_EQUALCTRL_ADDR   0x01000814
/// Offset of the EQUALCTRL register from the base address
#define MDM_EQUALCTRL_OFFSET 0x00000814
/// Index of the EQUALCTRL register
#define MDM_EQUALCTRL_INDEX  0x00000205
/// Reset value of the EQUALCTRL register
#define MDM_EQUALCTRL_RESET  0x4DA4C18B

/**
 * @brief Returns the current value of the EQUALCTRL register.
 * The EQUALCTRL register will be read and its value returned.
 * @return The current value of the EQUALCTRL register.
 */
__INLINE uint32_t mdm_equalctrl_get(void)
{
    return REG_PL_RD(MDM_EQUALCTRL_ADDR);
}

/**
 * @brief Sets the EQUALCTRL register to a value.
 * The EQUALCTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_equalctrl_set(uint32_t value)
{
    REG_PL_WR(MDM_EQUALCTRL_ADDR, value);
}

// field definitions
/// FDDCWEIGHTP2 field mask
#define MDM_FDDCWEIGHTP2_MASK       ((uint32_t)0xE0000000)
/// FDDCWEIGHTP2 field LSB position
#define MDM_FDDCWEIGHTP2_LSB        29
/// FDDCWEIGHTP2 field width
#define MDM_FDDCWEIGHTP2_WIDTH      ((uint32_t)0x00000003)
/// FDDCWEIGHTP1 field mask
#define MDM_FDDCWEIGHTP1_MASK       ((uint32_t)0x1C000000)
/// FDDCWEIGHTP1 field LSB position
#define MDM_FDDCWEIGHTP1_LSB        26
/// FDDCWEIGHTP1 field width
#define MDM_FDDCWEIGHTP1_WIDTH      ((uint32_t)0x00000003)
/// FDDCWEIGHTM1 field mask
#define MDM_FDDCWEIGHTM1_MASK       ((uint32_t)0x03800000)
/// FDDCWEIGHTM1 field LSB position
#define MDM_FDDCWEIGHTM1_LSB        23
/// FDDCWEIGHTM1 field width
#define MDM_FDDCWEIGHTM1_WIDTH      ((uint32_t)0x00000003)
/// FDDCWEIGHTM2 field mask
#define MDM_FDDCWEIGHTM2_MASK       ((uint32_t)0x00700000)
/// FDDCWEIGHTM2 field LSB position
#define MDM_FDDCWEIGHTM2_LSB        20
/// FDDCWEIGHTM2 field width
#define MDM_FDDCWEIGHTM2_WIDTH      ((uint32_t)0x00000003)
/// QPBSKNSDTHR field mask
#define MDM_QPBSKNSDTHR_MASK        ((uint32_t)0x00070000)
/// QPBSKNSDTHR field LSB position
#define MDM_QPBSKNSDTHR_LSB         16
/// QPBSKNSDTHR field width
#define MDM_QPBSKNSDTHR_WIDTH       ((uint32_t)0x00000003)
/// QBPSKDETTHR field mask
#define MDM_QBPSKDETTHR_MASK        ((uint32_t)0x0000F000)
/// QBPSKDETTHR field LSB position
#define MDM_QBPSKDETTHR_LSB         12
/// QBPSKDETTHR field width
#define MDM_QBPSKDETTHR_WIDTH       ((uint32_t)0x00000004)
/// FDDCSNRTHRESHOLD field mask
#define MDM_FDDCSNRTHRESHOLD_MASK   ((uint32_t)0x00000FF0)
/// FDDCSNRTHRESHOLD field LSB position
#define MDM_FDDCSNRTHRESHOLD_LSB    4
/// FDDCSNRTHRESHOLD field width
#define MDM_FDDCSNRTHRESHOLD_WIDTH  ((uint32_t)0x00000008)
/// FDDCCOMPEN field bit
#define MDM_FDDCCOMPEN_BIT          ((uint32_t)0x00000008)
/// FDDCCOMPEN field position
#define MDM_FDDCCOMPEN_POS          3
/// EQUALCFGSATSB field mask
#define MDM_EQUALCFGSATSB_MASK      ((uint32_t)0x00000007)
/// EQUALCFGSATSB field LSB position
#define MDM_EQUALCFGSATSB_LSB       0
/// EQUALCFGSATSB field width
#define MDM_EQUALCFGSATSB_WIDTH     ((uint32_t)0x00000003)

/// FDDCWEIGHTP2 field reset value
#define MDM_FDDCWEIGHTP2_RST        0x2
/// FDDCWEIGHTP1 field reset value
#define MDM_FDDCWEIGHTP1_RST        0x3
/// FDDCWEIGHTM1 field reset value
#define MDM_FDDCWEIGHTM1_RST        0x3
/// FDDCWEIGHTM2 field reset value
#define MDM_FDDCWEIGHTM2_RST        0x2
/// QPBSKNSDTHR field reset value
#define MDM_QPBSKNSDTHR_RST         0x4
/// QBPSKDETTHR field reset value
#define MDM_QBPSKDETTHR_RST         0xC
/// FDDCSNRTHRESHOLD field reset value
#define MDM_FDDCSNRTHRESHOLD_RST    0x18
/// FDDCCOMPEN field reset value
#define MDM_FDDCCOMPEN_RST          0x1
/// EQUALCFGSATSB field reset value
#define MDM_EQUALCFGSATSB_RST       0x3

/**
 * @brief Constructs a value for the EQUALCTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] fddcweightp2 - The value to use for the FDDCWEIGHTP2 field.
 * @param[in] fddcweightp1 - The value to use for the FDDCWEIGHTP1 field.
 * @param[in] fddcweightm1 - The value to use for the FDDCWEIGHTM1 field.
 * @param[in] fddcweightm2 - The value to use for the FDDCWEIGHTM2 field.
 * @param[in] qpbsknsdthr - The value to use for the QPBSKNSDTHR field.
 * @param[in] qbpskdetthr - The value to use for the QBPSKDETTHR field.
 * @param[in] fddcsnrthreshold - The value to use for the FDDCSNRTHRESHOLD field.
 * @param[in] fddccompen - The value to use for the FDDCCOMPEN field.
 * @param[in] equalcfgsatsb - The value to use for the EQUALCFGSATSB field.
 */
__INLINE void mdm_equalctrl_pack(uint8_t fddcweightp2, uint8_t fddcweightp1, uint8_t fddcweightm1, uint8_t fddcweightm2, uint8_t qpbsknsdthr, uint8_t qbpskdetthr, uint8_t fddcsnrthreshold, uint8_t fddccompen, uint8_t equalcfgsatsb)
{
    ASSERT_ERR((((uint32_t)fddcweightp2 << 29) & ~((uint32_t)0xE0000000)) == 0);
    ASSERT_ERR((((uint32_t)fddcweightp1 << 26) & ~((uint32_t)0x1C000000)) == 0);
    ASSERT_ERR((((uint32_t)fddcweightm1 << 23) & ~((uint32_t)0x03800000)) == 0);
    ASSERT_ERR((((uint32_t)fddcweightm2 << 20) & ~((uint32_t)0x00700000)) == 0);
    ASSERT_ERR((((uint32_t)qpbsknsdthr << 16) & ~((uint32_t)0x00070000)) == 0);
    ASSERT_ERR((((uint32_t)qbpskdetthr << 12) & ~((uint32_t)0x0000F000)) == 0);
    ASSERT_ERR((((uint32_t)fddcsnrthreshold << 4) & ~((uint32_t)0x00000FF0)) == 0);
    ASSERT_ERR((((uint32_t)fddccompen << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)equalcfgsatsb << 0) & ~((uint32_t)0x00000007)) == 0);
    REG_PL_WR(MDM_EQUALCTRL_ADDR,  ((uint32_t)fddcweightp2 << 29) | ((uint32_t)fddcweightp1 << 26) | ((uint32_t)fddcweightm1 << 23) | ((uint32_t)fddcweightm2 << 20) | ((uint32_t)qpbsknsdthr << 16) | ((uint32_t)qbpskdetthr << 12) | ((uint32_t)fddcsnrthreshold << 4) | ((uint32_t)fddccompen << 3) | ((uint32_t)equalcfgsatsb << 0));
}

/**
 * @brief Unpacks EQUALCTRL's fields from current value of the EQUALCTRL register.
 *
 * Reads the EQUALCTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] fddcweightp2 - Will be populated with the current value of this field from the register.
 * @param[out] fddcweightp1 - Will be populated with the current value of this field from the register.
 * @param[out] fddcweightm1 - Will be populated with the current value of this field from the register.
 * @param[out] fddcweightm2 - Will be populated with the current value of this field from the register.
 * @param[out] qpbsknsdthr - Will be populated with the current value of this field from the register.
 * @param[out] qbpskdetthr - Will be populated with the current value of this field from the register.
 * @param[out] fddcsnrthreshold - Will be populated with the current value of this field from the register.
 * @param[out] fddccompen - Will be populated with the current value of this field from the register.
 * @param[out] equalcfgsatsb - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_equalctrl_unpack(uint8_t *fddcweightp2, uint8_t *fddcweightp1, uint8_t *fddcweightm1, uint8_t *fddcweightm2, uint8_t *qpbsknsdthr, uint8_t *qbpskdetthr, uint8_t *fddcsnrthreshold, uint8_t *fddccompen, uint8_t *equalcfgsatsb)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL_ADDR);

    *fddcweightp2 = (localVal & ((uint32_t)0xE0000000)) >> 29;
    *fddcweightp1 = (localVal & ((uint32_t)0x1C000000)) >> 26;
    *fddcweightm1 = (localVal & ((uint32_t)0x03800000)) >> 23;
    *fddcweightm2 = (localVal & ((uint32_t)0x00700000)) >> 20;
    *qpbsknsdthr = (localVal & ((uint32_t)0x00070000)) >> 16;
    *qbpskdetthr = (localVal & ((uint32_t)0x0000F000)) >> 12;
    *fddcsnrthreshold = (localVal & ((uint32_t)0x00000FF0)) >> 4;
    *fddccompen = (localVal & ((uint32_t)0x00000008)) >> 3;
    *equalcfgsatsb = (localVal & ((uint32_t)0x00000007)) >> 0;
}

/**
 * @brief Returns the current value of the FDDCWEIGHTP2 field in the EQUALCTRL register.
 *
 * The EQUALCTRL register will be read and the FDDCWEIGHTP2 field's value will be returned.
 *
 * @return The current value of the FDDCWEIGHTP2 field in the EQUALCTRL register.
 */
__INLINE uint8_t mdm_fddcweightp2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL_ADDR);
    return ((localVal & ((uint32_t)0xE0000000)) >> 29);
}

/**
 * @brief Sets the FDDCWEIGHTP2 field of the EQUALCTRL register.
 *
 * The EQUALCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fddcweightp2 - The value to set the field to.
 */
__INLINE void mdm_fddcweightp2_setf(uint8_t fddcweightp2)
{
    ASSERT_ERR((((uint32_t)fddcweightp2 << 29) & ~((uint32_t)0xE0000000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL_ADDR, (REG_PL_RD(MDM_EQUALCTRL_ADDR) & ~((uint32_t)0xE0000000)) | ((uint32_t)fddcweightp2 << 29));
}

/**
 * @brief Returns the current value of the FDDCWEIGHTP1 field in the EQUALCTRL register.
 *
 * The EQUALCTRL register will be read and the FDDCWEIGHTP1 field's value will be returned.
 *
 * @return The current value of the FDDCWEIGHTP1 field in the EQUALCTRL register.
 */
__INLINE uint8_t mdm_fddcweightp1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL_ADDR);
    return ((localVal & ((uint32_t)0x1C000000)) >> 26);
}

/**
 * @brief Sets the FDDCWEIGHTP1 field of the EQUALCTRL register.
 *
 * The EQUALCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fddcweightp1 - The value to set the field to.
 */
__INLINE void mdm_fddcweightp1_setf(uint8_t fddcweightp1)
{
    ASSERT_ERR((((uint32_t)fddcweightp1 << 26) & ~((uint32_t)0x1C000000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL_ADDR, (REG_PL_RD(MDM_EQUALCTRL_ADDR) & ~((uint32_t)0x1C000000)) | ((uint32_t)fddcweightp1 << 26));
}

/**
 * @brief Returns the current value of the FDDCWEIGHTM1 field in the EQUALCTRL register.
 *
 * The EQUALCTRL register will be read and the FDDCWEIGHTM1 field's value will be returned.
 *
 * @return The current value of the FDDCWEIGHTM1 field in the EQUALCTRL register.
 */
__INLINE uint8_t mdm_fddcweightm1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL_ADDR);
    return ((localVal & ((uint32_t)0x03800000)) >> 23);
}

/**
 * @brief Sets the FDDCWEIGHTM1 field of the EQUALCTRL register.
 *
 * The EQUALCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fddcweightm1 - The value to set the field to.
 */
__INLINE void mdm_fddcweightm1_setf(uint8_t fddcweightm1)
{
    ASSERT_ERR((((uint32_t)fddcweightm1 << 23) & ~((uint32_t)0x03800000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL_ADDR, (REG_PL_RD(MDM_EQUALCTRL_ADDR) & ~((uint32_t)0x03800000)) | ((uint32_t)fddcweightm1 << 23));
}

/**
 * @brief Returns the current value of the FDDCWEIGHTM2 field in the EQUALCTRL register.
 *
 * The EQUALCTRL register will be read and the FDDCWEIGHTM2 field's value will be returned.
 *
 * @return The current value of the FDDCWEIGHTM2 field in the EQUALCTRL register.
 */
__INLINE uint8_t mdm_fddcweightm2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

/**
 * @brief Sets the FDDCWEIGHTM2 field of the EQUALCTRL register.
 *
 * The EQUALCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fddcweightm2 - The value to set the field to.
 */
__INLINE void mdm_fddcweightm2_setf(uint8_t fddcweightm2)
{
    ASSERT_ERR((((uint32_t)fddcweightm2 << 20) & ~((uint32_t)0x00700000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL_ADDR, (REG_PL_RD(MDM_EQUALCTRL_ADDR) & ~((uint32_t)0x00700000)) | ((uint32_t)fddcweightm2 << 20));
}

/**
 * @brief Returns the current value of the QPBSKNSDTHR field in the EQUALCTRL register.
 *
 * The EQUALCTRL register will be read and the QPBSKNSDTHR field's value will be returned.
 *
 * @return The current value of the QPBSKNSDTHR field in the EQUALCTRL register.
 */
__INLINE uint8_t mdm_qpbsknsdthr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

/**
 * @brief Sets the QPBSKNSDTHR field of the EQUALCTRL register.
 *
 * The EQUALCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] qpbsknsdthr - The value to set the field to.
 */
__INLINE void mdm_qpbsknsdthr_setf(uint8_t qpbsknsdthr)
{
    ASSERT_ERR((((uint32_t)qpbsknsdthr << 16) & ~((uint32_t)0x00070000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL_ADDR, (REG_PL_RD(MDM_EQUALCTRL_ADDR) & ~((uint32_t)0x00070000)) | ((uint32_t)qpbsknsdthr << 16));
}

/**
 * @brief Returns the current value of the QBPSKDETTHR field in the EQUALCTRL register.
 *
 * The EQUALCTRL register will be read and the QBPSKDETTHR field's value will be returned.
 *
 * @return The current value of the QBPSKDETTHR field in the EQUALCTRL register.
 */
__INLINE uint8_t mdm_qbpskdetthr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL_ADDR);
    return ((localVal & ((uint32_t)0x0000F000)) >> 12);
}

/**
 * @brief Sets the QBPSKDETTHR field of the EQUALCTRL register.
 *
 * The EQUALCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] qbpskdetthr - The value to set the field to.
 */
__INLINE void mdm_qbpskdetthr_setf(uint8_t qbpskdetthr)
{
    ASSERT_ERR((((uint32_t)qbpskdetthr << 12) & ~((uint32_t)0x0000F000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL_ADDR, (REG_PL_RD(MDM_EQUALCTRL_ADDR) & ~((uint32_t)0x0000F000)) | ((uint32_t)qbpskdetthr << 12));
}

/**
 * @brief Returns the current value of the FDDCSNRTHRESHOLD field in the EQUALCTRL register.
 *
 * The EQUALCTRL register will be read and the FDDCSNRTHRESHOLD field's value will be returned.
 *
 * @return The current value of the FDDCSNRTHRESHOLD field in the EQUALCTRL register.
 */
__INLINE uint8_t mdm_fddcsnrthreshold_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000FF0)) >> 4);
}

/**
 * @brief Sets the FDDCSNRTHRESHOLD field of the EQUALCTRL register.
 *
 * The EQUALCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fddcsnrthreshold - The value to set the field to.
 */
__INLINE void mdm_fddcsnrthreshold_setf(uint8_t fddcsnrthreshold)
{
    ASSERT_ERR((((uint32_t)fddcsnrthreshold << 4) & ~((uint32_t)0x00000FF0)) == 0);
    REG_PL_WR(MDM_EQUALCTRL_ADDR, (REG_PL_RD(MDM_EQUALCTRL_ADDR) & ~((uint32_t)0x00000FF0)) | ((uint32_t)fddcsnrthreshold << 4));
}

/**
 * @brief Returns the current value of the FDDCCOMPEN field in the EQUALCTRL register.
 *
 * The EQUALCTRL register will be read and the FDDCCOMPEN field's value will be returned.
 *
 * @return The current value of the FDDCCOMPEN field in the EQUALCTRL register.
 */
__INLINE uint8_t mdm_fddccompen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the FDDCCOMPEN field of the EQUALCTRL register.
 *
 * The EQUALCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fddccompen - The value to set the field to.
 */
__INLINE void mdm_fddccompen_setf(uint8_t fddccompen)
{
    ASSERT_ERR((((uint32_t)fddccompen << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MDM_EQUALCTRL_ADDR, (REG_PL_RD(MDM_EQUALCTRL_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)fddccompen << 3));
}

/**
 * @brief Returns the current value of the EQUALCFGSATSB field in the EQUALCTRL register.
 *
 * The EQUALCTRL register will be read and the EQUALCFGSATSB field's value will be returned.
 *
 * @return The current value of the EQUALCFGSATSB field in the EQUALCTRL register.
 */
__INLINE uint8_t mdm_equalcfgsatsb_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

/**
 * @brief Sets the EQUALCFGSATSB field of the EQUALCTRL register.
 *
 * The EQUALCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] equalcfgsatsb - The value to set the field to.
 */
__INLINE void mdm_equalcfgsatsb_setf(uint8_t equalcfgsatsb)
{
    ASSERT_ERR((((uint32_t)equalcfgsatsb << 0) & ~((uint32_t)0x00000007)) == 0);
    REG_PL_WR(MDM_EQUALCTRL_ADDR, (REG_PL_RD(MDM_EQUALCTRL_ADDR) & ~((uint32_t)0x00000007)) | ((uint32_t)equalcfgsatsb << 0));
}

/// @}

/**
 * @name SMOOTHCTRL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  27:20        TDCYCROTVAL80   0x18
 *     19             SMOOTHEN   1
 *     18       CFGSMOOTHFORCE   1
 *  17:16            CFGSMOOTH   0x0
 *  15:08        TDCYCROTVAL40   0xC
 *  07:00        TDCYCROTVAL20   0x6
 * </pre>
 *
 * @{
 */

/// Address of the SMOOTHCTRL register
#define MDM_SMOOTHCTRL_ADDR   0x01000818
/// Offset of the SMOOTHCTRL register from the base address
#define MDM_SMOOTHCTRL_OFFSET 0x00000818
/// Index of the SMOOTHCTRL register
#define MDM_SMOOTHCTRL_INDEX  0x00000206
/// Reset value of the SMOOTHCTRL register
#define MDM_SMOOTHCTRL_RESET  0x018C0C06

/**
 * @brief Returns the current value of the SMOOTHCTRL register.
 * The SMOOTHCTRL register will be read and its value returned.
 * @return The current value of the SMOOTHCTRL register.
 */
__INLINE uint32_t mdm_smoothctrl_get(void)
{
    return REG_PL_RD(MDM_SMOOTHCTRL_ADDR);
}

/**
 * @brief Sets the SMOOTHCTRL register to a value.
 * The SMOOTHCTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_smoothctrl_set(uint32_t value)
{
    REG_PL_WR(MDM_SMOOTHCTRL_ADDR, value);
}

// field definitions
/// TDCYCROTVAL80 field mask
#define MDM_TDCYCROTVAL80_MASK    ((uint32_t)0x0FF00000)
/// TDCYCROTVAL80 field LSB position
#define MDM_TDCYCROTVAL80_LSB     20
/// TDCYCROTVAL80 field width
#define MDM_TDCYCROTVAL80_WIDTH   ((uint32_t)0x00000008)
/// SMOOTHEN field bit
#define MDM_SMOOTHEN_BIT          ((uint32_t)0x00080000)
/// SMOOTHEN field position
#define MDM_SMOOTHEN_POS          19
/// CFGSMOOTHFORCE field bit
#define MDM_CFGSMOOTHFORCE_BIT    ((uint32_t)0x00040000)
/// CFGSMOOTHFORCE field position
#define MDM_CFGSMOOTHFORCE_POS    18
/// CFGSMOOTH field mask
#define MDM_CFGSMOOTH_MASK        ((uint32_t)0x00030000)
/// CFGSMOOTH field LSB position
#define MDM_CFGSMOOTH_LSB         16
/// CFGSMOOTH field width
#define MDM_CFGSMOOTH_WIDTH       ((uint32_t)0x00000002)
/// TDCYCROTVAL40 field mask
#define MDM_TDCYCROTVAL40_MASK    ((uint32_t)0x0000FF00)
/// TDCYCROTVAL40 field LSB position
#define MDM_TDCYCROTVAL40_LSB     8
/// TDCYCROTVAL40 field width
#define MDM_TDCYCROTVAL40_WIDTH   ((uint32_t)0x00000008)
/// TDCYCROTVAL20 field mask
#define MDM_TDCYCROTVAL20_MASK    ((uint32_t)0x000000FF)
/// TDCYCROTVAL20 field LSB position
#define MDM_TDCYCROTVAL20_LSB     0
/// TDCYCROTVAL20 field width
#define MDM_TDCYCROTVAL20_WIDTH   ((uint32_t)0x00000008)

/// TDCYCROTVAL80 field reset value
#define MDM_TDCYCROTVAL80_RST     0x18
/// SMOOTHEN field reset value
#define MDM_SMOOTHEN_RST          0x1
/// CFGSMOOTHFORCE field reset value
#define MDM_CFGSMOOTHFORCE_RST    0x1
/// CFGSMOOTH field reset value
#define MDM_CFGSMOOTH_RST         0x0
/// TDCYCROTVAL40 field reset value
#define MDM_TDCYCROTVAL40_RST     0xC
/// TDCYCROTVAL20 field reset value
#define MDM_TDCYCROTVAL20_RST     0x6

/**
 * @brief Constructs a value for the SMOOTHCTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] tdcycrotval80 - The value to use for the TDCYCROTVAL80 field.
 * @param[in] smoothen - The value to use for the SMOOTHEN field.
 * @param[in] cfgsmoothforce - The value to use for the CFGSMOOTHFORCE field.
 * @param[in] cfgsmooth - The value to use for the CFGSMOOTH field.
 * @param[in] tdcycrotval40 - The value to use for the TDCYCROTVAL40 field.
 * @param[in] tdcycrotval20 - The value to use for the TDCYCROTVAL20 field.
 */
__INLINE void mdm_smoothctrl_pack(uint8_t tdcycrotval80, uint8_t smoothen, uint8_t cfgsmoothforce, uint8_t cfgsmooth, uint8_t tdcycrotval40, uint8_t tdcycrotval20)
{
    ASSERT_ERR((((uint32_t)tdcycrotval80 << 20) & ~((uint32_t)0x0FF00000)) == 0);
    ASSERT_ERR((((uint32_t)smoothen << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)cfgsmoothforce << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)cfgsmooth << 16) & ~((uint32_t)0x00030000)) == 0);
    ASSERT_ERR((((uint32_t)tdcycrotval40 << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)tdcycrotval20 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_SMOOTHCTRL_ADDR,  ((uint32_t)tdcycrotval80 << 20) | ((uint32_t)smoothen << 19) | ((uint32_t)cfgsmoothforce << 18) | ((uint32_t)cfgsmooth << 16) | ((uint32_t)tdcycrotval40 << 8) | ((uint32_t)tdcycrotval20 << 0));
}

/**
 * @brief Unpacks SMOOTHCTRL's fields from current value of the SMOOTHCTRL register.
 *
 * Reads the SMOOTHCTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] tdcycrotval80 - Will be populated with the current value of this field from the register.
 * @param[out] smoothen - Will be populated with the current value of this field from the register.
 * @param[out] cfgsmoothforce - Will be populated with the current value of this field from the register.
 * @param[out] cfgsmooth - Will be populated with the current value of this field from the register.
 * @param[out] tdcycrotval40 - Will be populated with the current value of this field from the register.
 * @param[out] tdcycrotval20 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_smoothctrl_unpack(uint8_t *tdcycrotval80, uint8_t *smoothen, uint8_t *cfgsmoothforce, uint8_t *cfgsmooth, uint8_t *tdcycrotval40, uint8_t *tdcycrotval20)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHCTRL_ADDR);

    *tdcycrotval80 = (localVal & ((uint32_t)0x0FF00000)) >> 20;
    *smoothen = (localVal & ((uint32_t)0x00080000)) >> 19;
    *cfgsmoothforce = (localVal & ((uint32_t)0x00040000)) >> 18;
    *cfgsmooth = (localVal & ((uint32_t)0x00030000)) >> 16;
    *tdcycrotval40 = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *tdcycrotval20 = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the TDCYCROTVAL80 field in the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read and the TDCYCROTVAL80 field's value will be returned.
 *
 * @return The current value of the TDCYCROTVAL80 field in the SMOOTHCTRL register.
 */
__INLINE uint8_t mdm_tdcycrotval80_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHCTRL_ADDR);
    return ((localVal & ((uint32_t)0x0FF00000)) >> 20);
}

/**
 * @brief Sets the TDCYCROTVAL80 field of the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdcycrotval80 - The value to set the field to.
 */
__INLINE void mdm_tdcycrotval80_setf(uint8_t tdcycrotval80)
{
    ASSERT_ERR((((uint32_t)tdcycrotval80 << 20) & ~((uint32_t)0x0FF00000)) == 0);
    REG_PL_WR(MDM_SMOOTHCTRL_ADDR, (REG_PL_RD(MDM_SMOOTHCTRL_ADDR) & ~((uint32_t)0x0FF00000)) | ((uint32_t)tdcycrotval80 << 20));
}

/**
 * @brief Returns the current value of the SMOOTHEN field in the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read and the SMOOTHEN field's value will be returned.
 *
 * @return The current value of the SMOOTHEN field in the SMOOTHCTRL register.
 */
__INLINE uint8_t mdm_smoothen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Sets the SMOOTHEN field of the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] smoothen - The value to set the field to.
 */
__INLINE void mdm_smoothen_setf(uint8_t smoothen)
{
    ASSERT_ERR((((uint32_t)smoothen << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(MDM_SMOOTHCTRL_ADDR, (REG_PL_RD(MDM_SMOOTHCTRL_ADDR) & ~((uint32_t)0x00080000)) | ((uint32_t)smoothen << 19));
}

/**
 * @brief Returns the current value of the CFGSMOOTHFORCE field in the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read and the CFGSMOOTHFORCE field's value will be returned.
 *
 * @return The current value of the CFGSMOOTHFORCE field in the SMOOTHCTRL register.
 */
__INLINE uint8_t mdm_cfgsmoothforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Sets the CFGSMOOTHFORCE field of the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cfgsmoothforce - The value to set the field to.
 */
__INLINE void mdm_cfgsmoothforce_setf(uint8_t cfgsmoothforce)
{
    ASSERT_ERR((((uint32_t)cfgsmoothforce << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(MDM_SMOOTHCTRL_ADDR, (REG_PL_RD(MDM_SMOOTHCTRL_ADDR) & ~((uint32_t)0x00040000)) | ((uint32_t)cfgsmoothforce << 18));
}

/**
 * @brief Returns the current value of the CFGSMOOTH field in the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read and the CFGSMOOTH field's value will be returned.
 *
 * @return The current value of the CFGSMOOTH field in the SMOOTHCTRL register.
 */
__INLINE uint8_t mdm_cfgsmooth_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

/**
 * @brief Sets the CFGSMOOTH field of the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cfgsmooth - The value to set the field to.
 */
__INLINE void mdm_cfgsmooth_setf(uint8_t cfgsmooth)
{
    ASSERT_ERR((((uint32_t)cfgsmooth << 16) & ~((uint32_t)0x00030000)) == 0);
    REG_PL_WR(MDM_SMOOTHCTRL_ADDR, (REG_PL_RD(MDM_SMOOTHCTRL_ADDR) & ~((uint32_t)0x00030000)) | ((uint32_t)cfgsmooth << 16));
}

/**
 * @brief Returns the current value of the TDCYCROTVAL40 field in the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read and the TDCYCROTVAL40 field's value will be returned.
 *
 * @return The current value of the TDCYCROTVAL40 field in the SMOOTHCTRL register.
 */
__INLINE uint8_t mdm_tdcycrotval40_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHCTRL_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the TDCYCROTVAL40 field of the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdcycrotval40 - The value to set the field to.
 */
__INLINE void mdm_tdcycrotval40_setf(uint8_t tdcycrotval40)
{
    ASSERT_ERR((((uint32_t)tdcycrotval40 << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(MDM_SMOOTHCTRL_ADDR, (REG_PL_RD(MDM_SMOOTHCTRL_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)tdcycrotval40 << 8));
}

/**
 * @brief Returns the current value of the TDCYCROTVAL20 field in the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read and the TDCYCROTVAL20 field's value will be returned.
 *
 * @return The current value of the TDCYCROTVAL20 field in the SMOOTHCTRL register.
 */
__INLINE uint8_t mdm_tdcycrotval20_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHCTRL_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the TDCYCROTVAL20 field of the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdcycrotval20 - The value to set the field to.
 */
__INLINE void mdm_tdcycrotval20_setf(uint8_t tdcycrotval20)
{
    ASSERT_ERR((((uint32_t)tdcycrotval20 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_SMOOTHCTRL_ADDR, (REG_PL_RD(MDM_SMOOTHCTRL_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)tdcycrotval20 << 0));
}

/// @}

/**
 * @name DCESTIMCTRL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31              DCESTEN   1
 *  30:24            STARTHTDC   0x0
 *  22:16              STARTDC   0x0
 *  13:08        DELAYNORMALGI   0x11
 *  06:00            WAITHTSTF   0xF
 * </pre>
 *
 * @{
 */

/// Address of the DCESTIMCTRL register
#define MDM_DCESTIMCTRL_ADDR   0x0100081C
/// Offset of the DCESTIMCTRL register from the base address
#define MDM_DCESTIMCTRL_OFFSET 0x0000081C
/// Index of the DCESTIMCTRL register
#define MDM_DCESTIMCTRL_INDEX  0x00000207
/// Reset value of the DCESTIMCTRL register
#define MDM_DCESTIMCTRL_RESET  0x8000110F

/**
 * @brief Returns the current value of the DCESTIMCTRL register.
 * The DCESTIMCTRL register will be read and its value returned.
 * @return The current value of the DCESTIMCTRL register.
 */
__INLINE uint32_t mdm_dcestimctrl_get(void)
{
    return REG_PL_RD(MDM_DCESTIMCTRL_ADDR);
}

/**
 * @brief Sets the DCESTIMCTRL register to a value.
 * The DCESTIMCTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_dcestimctrl_set(uint32_t value)
{
    REG_PL_WR(MDM_DCESTIMCTRL_ADDR, value);
}

// field definitions
/// DCESTEN field bit
#define MDM_DCESTEN_BIT          ((uint32_t)0x80000000)
/// DCESTEN field position
#define MDM_DCESTEN_POS          31
/// STARTHTDC field mask
#define MDM_STARTHTDC_MASK       ((uint32_t)0x7F000000)
/// STARTHTDC field LSB position
#define MDM_STARTHTDC_LSB        24
/// STARTHTDC field width
#define MDM_STARTHTDC_WIDTH      ((uint32_t)0x00000007)
/// STARTDC field mask
#define MDM_STARTDC_MASK         ((uint32_t)0x007F0000)
/// STARTDC field LSB position
#define MDM_STARTDC_LSB          16
/// STARTDC field width
#define MDM_STARTDC_WIDTH        ((uint32_t)0x00000007)
/// DELAYNORMALGI field mask
#define MDM_DELAYNORMALGI_MASK   ((uint32_t)0x00003F00)
/// DELAYNORMALGI field LSB position
#define MDM_DELAYNORMALGI_LSB    8
/// DELAYNORMALGI field width
#define MDM_DELAYNORMALGI_WIDTH  ((uint32_t)0x00000006)
/// WAITHTSTF field mask
#define MDM_WAITHTSTF_MASK       ((uint32_t)0x0000007F)
/// WAITHTSTF field LSB position
#define MDM_WAITHTSTF_LSB        0
/// WAITHTSTF field width
#define MDM_WAITHTSTF_WIDTH      ((uint32_t)0x00000007)

/// DCESTEN field reset value
#define MDM_DCESTEN_RST          0x1
/// STARTHTDC field reset value
#define MDM_STARTHTDC_RST        0x0
/// STARTDC field reset value
#define MDM_STARTDC_RST          0x0
/// DELAYNORMALGI field reset value
#define MDM_DELAYNORMALGI_RST    0x11
/// WAITHTSTF field reset value
#define MDM_WAITHTSTF_RST        0xF

/**
 * @brief Constructs a value for the DCESTIMCTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] dcesten - The value to use for the DCESTEN field.
 * @param[in] starthtdc - The value to use for the STARTHTDC field.
 * @param[in] startdc - The value to use for the STARTDC field.
 * @param[in] delaynormalgi - The value to use for the DELAYNORMALGI field.
 * @param[in] waithtstf - The value to use for the WAITHTSTF field.
 */
__INLINE void mdm_dcestimctrl_pack(uint8_t dcesten, uint8_t starthtdc, uint8_t startdc, uint8_t delaynormalgi, uint8_t waithtstf)
{
    ASSERT_ERR((((uint32_t)dcesten << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)starthtdc << 24) & ~((uint32_t)0x7F000000)) == 0);
    ASSERT_ERR((((uint32_t)startdc << 16) & ~((uint32_t)0x007F0000)) == 0);
    ASSERT_ERR((((uint32_t)delaynormalgi << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)waithtstf << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_DCESTIMCTRL_ADDR,  ((uint32_t)dcesten << 31) | ((uint32_t)starthtdc << 24) | ((uint32_t)startdc << 16) | ((uint32_t)delaynormalgi << 8) | ((uint32_t)waithtstf << 0));
}

/**
 * @brief Unpacks DCESTIMCTRL's fields from current value of the DCESTIMCTRL register.
 *
 * Reads the DCESTIMCTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] dcesten - Will be populated with the current value of this field from the register.
 * @param[out] starthtdc - Will be populated with the current value of this field from the register.
 * @param[out] startdc - Will be populated with the current value of this field from the register.
 * @param[out] delaynormalgi - Will be populated with the current value of this field from the register.
 * @param[out] waithtstf - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_dcestimctrl_unpack(uint8_t *dcesten, uint8_t *starthtdc, uint8_t *startdc, uint8_t *delaynormalgi, uint8_t *waithtstf)
{
    uint32_t localVal = REG_PL_RD(MDM_DCESTIMCTRL_ADDR);

    *dcesten = (localVal & ((uint32_t)0x80000000)) >> 31;
    *starthtdc = (localVal & ((uint32_t)0x7F000000)) >> 24;
    *startdc = (localVal & ((uint32_t)0x007F0000)) >> 16;
    *delaynormalgi = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *waithtstf = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the DCESTEN field in the DCESTIMCTRL register.
 *
 * The DCESTIMCTRL register will be read and the DCESTEN field's value will be returned.
 *
 * @return The current value of the DCESTEN field in the DCESTIMCTRL register.
 */
__INLINE uint8_t mdm_dcesten_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_DCESTIMCTRL_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the DCESTEN field of the DCESTIMCTRL register.
 *
 * The DCESTIMCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dcesten - The value to set the field to.
 */
__INLINE void mdm_dcesten_setf(uint8_t dcesten)
{
    ASSERT_ERR((((uint32_t)dcesten << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(MDM_DCESTIMCTRL_ADDR, (REG_PL_RD(MDM_DCESTIMCTRL_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)dcesten << 31));
}

/**
 * @brief Returns the current value of the STARTHTDC field in the DCESTIMCTRL register.
 *
 * The DCESTIMCTRL register will be read and the STARTHTDC field's value will be returned.
 *
 * @return The current value of the STARTHTDC field in the DCESTIMCTRL register.
 */
__INLINE uint8_t mdm_starthtdc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_DCESTIMCTRL_ADDR);
    return ((localVal & ((uint32_t)0x7F000000)) >> 24);
}

/**
 * @brief Sets the STARTHTDC field of the DCESTIMCTRL register.
 *
 * The DCESTIMCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] starthtdc - The value to set the field to.
 */
__INLINE void mdm_starthtdc_setf(uint8_t starthtdc)
{
    ASSERT_ERR((((uint32_t)starthtdc << 24) & ~((uint32_t)0x7F000000)) == 0);
    REG_PL_WR(MDM_DCESTIMCTRL_ADDR, (REG_PL_RD(MDM_DCESTIMCTRL_ADDR) & ~((uint32_t)0x7F000000)) | ((uint32_t)starthtdc << 24));
}

/**
 * @brief Returns the current value of the STARTDC field in the DCESTIMCTRL register.
 *
 * The DCESTIMCTRL register will be read and the STARTDC field's value will be returned.
 *
 * @return The current value of the STARTDC field in the DCESTIMCTRL register.
 */
__INLINE uint8_t mdm_startdc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_DCESTIMCTRL_ADDR);
    return ((localVal & ((uint32_t)0x007F0000)) >> 16);
}

/**
 * @brief Sets the STARTDC field of the DCESTIMCTRL register.
 *
 * The DCESTIMCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] startdc - The value to set the field to.
 */
__INLINE void mdm_startdc_setf(uint8_t startdc)
{
    ASSERT_ERR((((uint32_t)startdc << 16) & ~((uint32_t)0x007F0000)) == 0);
    REG_PL_WR(MDM_DCESTIMCTRL_ADDR, (REG_PL_RD(MDM_DCESTIMCTRL_ADDR) & ~((uint32_t)0x007F0000)) | ((uint32_t)startdc << 16));
}

/**
 * @brief Returns the current value of the DELAYNORMALGI field in the DCESTIMCTRL register.
 *
 * The DCESTIMCTRL register will be read and the DELAYNORMALGI field's value will be returned.
 *
 * @return The current value of the DELAYNORMALGI field in the DCESTIMCTRL register.
 */
__INLINE uint8_t mdm_delaynormalgi_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_DCESTIMCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the DELAYNORMALGI field of the DCESTIMCTRL register.
 *
 * The DCESTIMCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] delaynormalgi - The value to set the field to.
 */
__INLINE void mdm_delaynormalgi_setf(uint8_t delaynormalgi)
{
    ASSERT_ERR((((uint32_t)delaynormalgi << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(MDM_DCESTIMCTRL_ADDR, (REG_PL_RD(MDM_DCESTIMCTRL_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)delaynormalgi << 8));
}

/**
 * @brief Returns the current value of the WAITHTSTF field in the DCESTIMCTRL register.
 *
 * The DCESTIMCTRL register will be read and the WAITHTSTF field's value will be returned.
 *
 * @return The current value of the WAITHTSTF field in the DCESTIMCTRL register.
 */
__INLINE uint8_t mdm_waithtstf_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_DCESTIMCTRL_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/**
 * @brief Sets the WAITHTSTF field of the DCESTIMCTRL register.
 *
 * The DCESTIMCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] waithtstf - The value to set the field to.
 */
__INLINE void mdm_waithtstf_setf(uint8_t waithtstf)
{
    ASSERT_ERR((((uint32_t)waithtstf << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_DCESTIMCTRL_ADDR, (REG_PL_RD(MDM_DCESTIMCTRL_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)waithtstf << 0));
}

/// @}

/**
 * @name FECTRL0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  22:16      TX80DIGGAINLIN0   0x20
 *  14:08      TX40DIGGAINLIN0   0x20
 *  06:00      TX20DIGGAINLIN0   0x20
 * </pre>
 *
 * @{
 */

/// Address of the FECTRL0 register
#define MDM_FECTRL0_ADDR   0x01000820
/// Offset of the FECTRL0 register from the base address
#define MDM_FECTRL0_OFFSET 0x00000820
/// Index of the FECTRL0 register
#define MDM_FECTRL0_INDEX  0x00000208
/// Reset value of the FECTRL0 register
#define MDM_FECTRL0_RESET  0x00202020

/**
 * @brief Returns the current value of the FECTRL0 register.
 * The FECTRL0 register will be read and its value returned.
 * @return The current value of the FECTRL0 register.
 */
__INLINE uint32_t mdm_fectrl0_get(void)
{
    return REG_PL_RD(MDM_FECTRL0_ADDR);
}

/**
 * @brief Sets the FECTRL0 register to a value.
 * The FECTRL0 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_fectrl0_set(uint32_t value)
{
    REG_PL_WR(MDM_FECTRL0_ADDR, value);
}

// field definitions
/// TX80DIGGAINLIN0 field mask
#define MDM_TX80DIGGAINLIN0_MASK   ((uint32_t)0x007F0000)
/// TX80DIGGAINLIN0 field LSB position
#define MDM_TX80DIGGAINLIN0_LSB    16
/// TX80DIGGAINLIN0 field width
#define MDM_TX80DIGGAINLIN0_WIDTH  ((uint32_t)0x00000007)
/// TX40DIGGAINLIN0 field mask
#define MDM_TX40DIGGAINLIN0_MASK   ((uint32_t)0x00007F00)
/// TX40DIGGAINLIN0 field LSB position
#define MDM_TX40DIGGAINLIN0_LSB    8
/// TX40DIGGAINLIN0 field width
#define MDM_TX40DIGGAINLIN0_WIDTH  ((uint32_t)0x00000007)
/// TX20DIGGAINLIN0 field mask
#define MDM_TX20DIGGAINLIN0_MASK   ((uint32_t)0x0000007F)
/// TX20DIGGAINLIN0 field LSB position
#define MDM_TX20DIGGAINLIN0_LSB    0
/// TX20DIGGAINLIN0 field width
#define MDM_TX20DIGGAINLIN0_WIDTH  ((uint32_t)0x00000007)

/// TX80DIGGAINLIN0 field reset value
#define MDM_TX80DIGGAINLIN0_RST    0x20
/// TX40DIGGAINLIN0 field reset value
#define MDM_TX40DIGGAINLIN0_RST    0x20
/// TX20DIGGAINLIN0 field reset value
#define MDM_TX20DIGGAINLIN0_RST    0x20

/**
 * @brief Constructs a value for the FECTRL0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] tx80diggainlin0 - The value to use for the TX80DIGGAINLIN0 field.
 * @param[in] tx40diggainlin0 - The value to use for the TX40DIGGAINLIN0 field.
 * @param[in] tx20diggainlin0 - The value to use for the TX20DIGGAINLIN0 field.
 */
__INLINE void mdm_fectrl0_pack(uint8_t tx80diggainlin0, uint8_t tx40diggainlin0, uint8_t tx20diggainlin0)
{
    ASSERT_ERR((((uint32_t)tx80diggainlin0 << 16) & ~((uint32_t)0x007F0000)) == 0);
    ASSERT_ERR((((uint32_t)tx40diggainlin0 << 8) & ~((uint32_t)0x00007F00)) == 0);
    ASSERT_ERR((((uint32_t)tx20diggainlin0 << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_FECTRL0_ADDR,  ((uint32_t)tx80diggainlin0 << 16) | ((uint32_t)tx40diggainlin0 << 8) | ((uint32_t)tx20diggainlin0 << 0));
}

/**
 * @brief Unpacks FECTRL0's fields from current value of the FECTRL0 register.
 *
 * Reads the FECTRL0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] tx80diggainlin0 - Will be populated with the current value of this field from the register.
 * @param[out] tx40diggainlin0 - Will be populated with the current value of this field from the register.
 * @param[out] tx20diggainlin0 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_fectrl0_unpack(uint8_t *tx80diggainlin0, uint8_t *tx40diggainlin0, uint8_t *tx20diggainlin0)
{
    uint32_t localVal = REG_PL_RD(MDM_FECTRL0_ADDR);

    *tx80diggainlin0 = (localVal & ((uint32_t)0x007F0000)) >> 16;
    *tx40diggainlin0 = (localVal & ((uint32_t)0x00007F00)) >> 8;
    *tx20diggainlin0 = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the TX80DIGGAINLIN0 field in the FECTRL0 register.
 *
 * The FECTRL0 register will be read and the TX80DIGGAINLIN0 field's value will be returned.
 *
 * @return The current value of the TX80DIGGAINLIN0 field in the FECTRL0 register.
 */
__INLINE uint8_t mdm_tx80diggainlin0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x007F0000)) >> 16);
}

/**
 * @brief Sets the TX80DIGGAINLIN0 field of the FECTRL0 register.
 *
 * The FECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tx80diggainlin0 - The value to set the field to.
 */
__INLINE void mdm_tx80diggainlin0_setf(uint8_t tx80diggainlin0)
{
    ASSERT_ERR((((uint32_t)tx80diggainlin0 << 16) & ~((uint32_t)0x007F0000)) == 0);
    REG_PL_WR(MDM_FECTRL0_ADDR, (REG_PL_RD(MDM_FECTRL0_ADDR) & ~((uint32_t)0x007F0000)) | ((uint32_t)tx80diggainlin0 << 16));
}

/**
 * @brief Returns the current value of the TX40DIGGAINLIN0 field in the FECTRL0 register.
 *
 * The FECTRL0 register will be read and the TX40DIGGAINLIN0 field's value will be returned.
 *
 * @return The current value of the TX40DIGGAINLIN0 field in the FECTRL0 register.
 */
__INLINE uint8_t mdm_tx40diggainlin0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}

/**
 * @brief Sets the TX40DIGGAINLIN0 field of the FECTRL0 register.
 *
 * The FECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tx40diggainlin0 - The value to set the field to.
 */
__INLINE void mdm_tx40diggainlin0_setf(uint8_t tx40diggainlin0)
{
    ASSERT_ERR((((uint32_t)tx40diggainlin0 << 8) & ~((uint32_t)0x00007F00)) == 0);
    REG_PL_WR(MDM_FECTRL0_ADDR, (REG_PL_RD(MDM_FECTRL0_ADDR) & ~((uint32_t)0x00007F00)) | ((uint32_t)tx40diggainlin0 << 8));
}

/**
 * @brief Returns the current value of the TX20DIGGAINLIN0 field in the FECTRL0 register.
 *
 * The FECTRL0 register will be read and the TX20DIGGAINLIN0 field's value will be returned.
 *
 * @return The current value of the TX20DIGGAINLIN0 field in the FECTRL0 register.
 */
__INLINE uint8_t mdm_tx20diggainlin0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/**
 * @brief Sets the TX20DIGGAINLIN0 field of the FECTRL0 register.
 *
 * The FECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tx20diggainlin0 - The value to set the field to.
 */
__INLINE void mdm_tx20diggainlin0_setf(uint8_t tx20diggainlin0)
{
    ASSERT_ERR((((uint32_t)tx20diggainlin0 << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_FECTRL0_ADDR, (REG_PL_RD(MDM_FECTRL0_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)tx20diggainlin0 << 0));
}

/// @}

/**
 * @name SMOOTHSNRTHR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:08     SMOOTHSNRTHRHIGH   0x1B
 *  07:00      SMOOTHSNRTHRMID   0xF
 * </pre>
 *
 * @{
 */

/// Address of the SMOOTHSNRTHR register
#define MDM_SMOOTHSNRTHR_ADDR   0x01000830
/// Offset of the SMOOTHSNRTHR register from the base address
#define MDM_SMOOTHSNRTHR_OFFSET 0x00000830
/// Index of the SMOOTHSNRTHR register
#define MDM_SMOOTHSNRTHR_INDEX  0x0000020C
/// Reset value of the SMOOTHSNRTHR register
#define MDM_SMOOTHSNRTHR_RESET  0x00001B0F

/**
 * @brief Returns the current value of the SMOOTHSNRTHR register.
 * The SMOOTHSNRTHR register will be read and its value returned.
 * @return The current value of the SMOOTHSNRTHR register.
 */
__INLINE uint32_t mdm_smoothsnrthr_get(void)
{
    return REG_PL_RD(MDM_SMOOTHSNRTHR_ADDR);
}

/**
 * @brief Sets the SMOOTHSNRTHR register to a value.
 * The SMOOTHSNRTHR register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_smoothsnrthr_set(uint32_t value)
{
    REG_PL_WR(MDM_SMOOTHSNRTHR_ADDR, value);
}

// field definitions
/// SMOOTHSNRTHRHIGH field mask
#define MDM_SMOOTHSNRTHRHIGH_MASK   ((uint32_t)0x0000FF00)
/// SMOOTHSNRTHRHIGH field LSB position
#define MDM_SMOOTHSNRTHRHIGH_LSB    8
/// SMOOTHSNRTHRHIGH field width
#define MDM_SMOOTHSNRTHRHIGH_WIDTH  ((uint32_t)0x00000008)
/// SMOOTHSNRTHRMID field mask
#define MDM_SMOOTHSNRTHRMID_MASK    ((uint32_t)0x000000FF)
/// SMOOTHSNRTHRMID field LSB position
#define MDM_SMOOTHSNRTHRMID_LSB     0
/// SMOOTHSNRTHRMID field width
#define MDM_SMOOTHSNRTHRMID_WIDTH   ((uint32_t)0x00000008)

/// SMOOTHSNRTHRHIGH field reset value
#define MDM_SMOOTHSNRTHRHIGH_RST    0x1B
/// SMOOTHSNRTHRMID field reset value
#define MDM_SMOOTHSNRTHRMID_RST     0xF

/**
 * @brief Constructs a value for the SMOOTHSNRTHR register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] smoothsnrthrhigh - The value to use for the SMOOTHSNRTHRHIGH field.
 * @param[in] smoothsnrthrmid - The value to use for the SMOOTHSNRTHRMID field.
 */
__INLINE void mdm_smoothsnrthr_pack(uint8_t smoothsnrthrhigh, uint8_t smoothsnrthrmid)
{
    ASSERT_ERR((((uint32_t)smoothsnrthrhigh << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)smoothsnrthrmid << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_SMOOTHSNRTHR_ADDR,  ((uint32_t)smoothsnrthrhigh << 8) | ((uint32_t)smoothsnrthrmid << 0));
}

/**
 * @brief Unpacks SMOOTHSNRTHR's fields from current value of the SMOOTHSNRTHR register.
 *
 * Reads the SMOOTHSNRTHR register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] smoothsnrthrhigh - Will be populated with the current value of this field from the register.
 * @param[out] smoothsnrthrmid - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_smoothsnrthr_unpack(uint8_t *smoothsnrthrhigh, uint8_t *smoothsnrthrmid)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHSNRTHR_ADDR);

    *smoothsnrthrhigh = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *smoothsnrthrmid = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the SMOOTHSNRTHRHIGH field in the SMOOTHSNRTHR register.
 *
 * The SMOOTHSNRTHR register will be read and the SMOOTHSNRTHRHIGH field's value will be returned.
 *
 * @return The current value of the SMOOTHSNRTHRHIGH field in the SMOOTHSNRTHR register.
 */
__INLINE uint8_t mdm_smoothsnrthrhigh_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHSNRTHR_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the SMOOTHSNRTHRHIGH field of the SMOOTHSNRTHR register.
 *
 * The SMOOTHSNRTHR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] smoothsnrthrhigh - The value to set the field to.
 */
__INLINE void mdm_smoothsnrthrhigh_setf(uint8_t smoothsnrthrhigh)
{
    ASSERT_ERR((((uint32_t)smoothsnrthrhigh << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(MDM_SMOOTHSNRTHR_ADDR, (REG_PL_RD(MDM_SMOOTHSNRTHR_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)smoothsnrthrhigh << 8));
}

/**
 * @brief Returns the current value of the SMOOTHSNRTHRMID field in the SMOOTHSNRTHR register.
 *
 * The SMOOTHSNRTHR register will be read and the SMOOTHSNRTHRMID field's value will be returned.
 *
 * @return The current value of the SMOOTHSNRTHRMID field in the SMOOTHSNRTHR register.
 */
__INLINE uint8_t mdm_smoothsnrthrmid_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHSNRTHR_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the SMOOTHSNRTHRMID field of the SMOOTHSNRTHR register.
 *
 * The SMOOTHSNRTHR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] smoothsnrthrmid - The value to set the field to.
 */
__INLINE void mdm_smoothsnrthrmid_setf(uint8_t smoothsnrthrmid)
{
    ASSERT_ERR((((uint32_t)smoothsnrthrmid << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_SMOOTHSNRTHR_ADDR, (REG_PL_RD(MDM_SMOOTHSNRTHR_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)smoothsnrthrmid << 0));
}

/// @}

/**
 * @name RXCTRL0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  20:16        TDBWDETWEIGHT   0x16
 *     13         RXC2SDISBCFG   0
 *     12          RXIQSWAPCFG   0
 *     05          TDBWFORCE40   0
 *     04          TDBWFORCE20   0
 *     02         RATEERRORCFG   1
 *     01        LSIGPARITYCFG   1
 *     00          PROPMODECFG   0
 * </pre>
 *
 * @{
 */

/// Address of the RXCTRL0 register
#define MDM_RXCTRL0_ADDR   0x01000834
/// Offset of the RXCTRL0 register from the base address
#define MDM_RXCTRL0_OFFSET 0x00000834
/// Index of the RXCTRL0 register
#define MDM_RXCTRL0_INDEX  0x0000020D
/// Reset value of the RXCTRL0 register
#define MDM_RXCTRL0_RESET  0x00160006

/**
 * @brief Returns the current value of the RXCTRL0 register.
 * The RXCTRL0 register will be read and its value returned.
 * @return The current value of the RXCTRL0 register.
 */
__INLINE uint32_t mdm_rxctrl0_get(void)
{
    return REG_PL_RD(MDM_RXCTRL0_ADDR);
}

/**
 * @brief Sets the RXCTRL0 register to a value.
 * The RXCTRL0 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxctrl0_set(uint32_t value)
{
    REG_PL_WR(MDM_RXCTRL0_ADDR, value);
}

// field definitions
/// TDBWDETWEIGHT field mask
#define MDM_TDBWDETWEIGHT_MASK   ((uint32_t)0x001F0000)
/// TDBWDETWEIGHT field LSB position
#define MDM_TDBWDETWEIGHT_LSB    16
/// TDBWDETWEIGHT field width
#define MDM_TDBWDETWEIGHT_WIDTH  ((uint32_t)0x00000005)
/// RXC2SDISBCFG field bit
#define MDM_RXC2SDISBCFG_BIT     ((uint32_t)0x00002000)
/// RXC2SDISBCFG field position
#define MDM_RXC2SDISBCFG_POS     13
/// RXIQSWAPCFG field bit
#define MDM_RXIQSWAPCFG_BIT      ((uint32_t)0x00001000)
/// RXIQSWAPCFG field position
#define MDM_RXIQSWAPCFG_POS      12
/// TDBWFORCE40 field bit
#define MDM_TDBWFORCE40_BIT      ((uint32_t)0x00000020)
/// TDBWFORCE40 field position
#define MDM_TDBWFORCE40_POS      5
/// TDBWFORCE20 field bit
#define MDM_TDBWFORCE20_BIT      ((uint32_t)0x00000010)
/// TDBWFORCE20 field position
#define MDM_TDBWFORCE20_POS      4
/// RATEERRORCFG field bit
#define MDM_RATEERRORCFG_BIT     ((uint32_t)0x00000004)
/// RATEERRORCFG field position
#define MDM_RATEERRORCFG_POS     2
/// LSIGPARITYCFG field bit
#define MDM_LSIGPARITYCFG_BIT    ((uint32_t)0x00000002)
/// LSIGPARITYCFG field position
#define MDM_LSIGPARITYCFG_POS    1
/// PROPMODECFG field bit
#define MDM_PROPMODECFG_BIT      ((uint32_t)0x00000001)
/// PROPMODECFG field position
#define MDM_PROPMODECFG_POS      0

/// TDBWDETWEIGHT field reset value
#define MDM_TDBWDETWEIGHT_RST    0x16
/// RXC2SDISBCFG field reset value
#define MDM_RXC2SDISBCFG_RST     0x0
/// RXIQSWAPCFG field reset value
#define MDM_RXIQSWAPCFG_RST      0x0
/// TDBWFORCE40 field reset value
#define MDM_TDBWFORCE40_RST      0x0
/// TDBWFORCE20 field reset value
#define MDM_TDBWFORCE20_RST      0x0
/// RATEERRORCFG field reset value
#define MDM_RATEERRORCFG_RST     0x1
/// LSIGPARITYCFG field reset value
#define MDM_LSIGPARITYCFG_RST    0x1
/// PROPMODECFG field reset value
#define MDM_PROPMODECFG_RST      0x0

/**
 * @brief Constructs a value for the RXCTRL0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] tdbwdetweight - The value to use for the TDBWDETWEIGHT field.
 * @param[in] rxc2sdisbcfg - The value to use for the RXC2SDISBCFG field.
 * @param[in] rxiqswapcfg - The value to use for the RXIQSWAPCFG field.
 * @param[in] tdbwforce40 - The value to use for the TDBWFORCE40 field.
 * @param[in] tdbwforce20 - The value to use for the TDBWFORCE20 field.
 * @param[in] rateerrorcfg - The value to use for the RATEERRORCFG field.
 * @param[in] lsigparitycfg - The value to use for the LSIGPARITYCFG field.
 * @param[in] propmodecfg - The value to use for the PROPMODECFG field.
 */
__INLINE void mdm_rxctrl0_pack(uint8_t tdbwdetweight, uint8_t rxc2sdisbcfg, uint8_t rxiqswapcfg, uint8_t tdbwforce40, uint8_t tdbwforce20, uint8_t rateerrorcfg, uint8_t lsigparitycfg, uint8_t propmodecfg)
{
    ASSERT_ERR((((uint32_t)tdbwdetweight << 16) & ~((uint32_t)0x001F0000)) == 0);
    ASSERT_ERR((((uint32_t)rxc2sdisbcfg << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)rxiqswapcfg << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)tdbwforce40 << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)tdbwforce20 << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)rateerrorcfg << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)lsigparitycfg << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)propmodecfg << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_RXCTRL0_ADDR,  ((uint32_t)tdbwdetweight << 16) | ((uint32_t)rxc2sdisbcfg << 13) | ((uint32_t)rxiqswapcfg << 12) | ((uint32_t)tdbwforce40 << 5) | ((uint32_t)tdbwforce20 << 4) | ((uint32_t)rateerrorcfg << 2) | ((uint32_t)lsigparitycfg << 1) | ((uint32_t)propmodecfg << 0));
}

/**
 * @brief Unpacks RXCTRL0's fields from current value of the RXCTRL0 register.
 *
 * Reads the RXCTRL0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] tdbwdetweight - Will be populated with the current value of this field from the register.
 * @param[out] rxc2sdisbcfg - Will be populated with the current value of this field from the register.
 * @param[out] rxiqswapcfg - Will be populated with the current value of this field from the register.
 * @param[out] tdbwforce40 - Will be populated with the current value of this field from the register.
 * @param[out] tdbwforce20 - Will be populated with the current value of this field from the register.
 * @param[out] rateerrorcfg - Will be populated with the current value of this field from the register.
 * @param[out] lsigparitycfg - Will be populated with the current value of this field from the register.
 * @param[out] propmodecfg - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxctrl0_unpack(uint8_t *tdbwdetweight, uint8_t *rxc2sdisbcfg, uint8_t *rxiqswapcfg, uint8_t *tdbwforce40, uint8_t *tdbwforce20, uint8_t *rateerrorcfg, uint8_t *lsigparitycfg, uint8_t *propmodecfg)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL0_ADDR);

    *tdbwdetweight = (localVal & ((uint32_t)0x001F0000)) >> 16;
    *rxc2sdisbcfg = (localVal & ((uint32_t)0x00002000)) >> 13;
    *rxiqswapcfg = (localVal & ((uint32_t)0x00001000)) >> 12;
    *tdbwforce40 = (localVal & ((uint32_t)0x00000020)) >> 5;
    *tdbwforce20 = (localVal & ((uint32_t)0x00000010)) >> 4;
    *rateerrorcfg = (localVal & ((uint32_t)0x00000004)) >> 2;
    *lsigparitycfg = (localVal & ((uint32_t)0x00000002)) >> 1;
    *propmodecfg = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the TDBWDETWEIGHT field in the RXCTRL0 register.
 *
 * The RXCTRL0 register will be read and the TDBWDETWEIGHT field's value will be returned.
 *
 * @return The current value of the TDBWDETWEIGHT field in the RXCTRL0 register.
 */
__INLINE uint8_t mdm_tdbwdetweight_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x001F0000)) >> 16);
}

/**
 * @brief Sets the TDBWDETWEIGHT field of the RXCTRL0 register.
 *
 * The RXCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdbwdetweight - The value to set the field to.
 */
__INLINE void mdm_tdbwdetweight_setf(uint8_t tdbwdetweight)
{
    ASSERT_ERR((((uint32_t)tdbwdetweight << 16) & ~((uint32_t)0x001F0000)) == 0);
    REG_PL_WR(MDM_RXCTRL0_ADDR, (REG_PL_RD(MDM_RXCTRL0_ADDR) & ~((uint32_t)0x001F0000)) | ((uint32_t)tdbwdetweight << 16));
}

/**
 * @brief Returns the current value of the RXC2SDISBCFG field in the RXCTRL0 register.
 *
 * The RXCTRL0 register will be read and the RXC2SDISBCFG field's value will be returned.
 *
 * @return The current value of the RXC2SDISBCFG field in the RXCTRL0 register.
 */
__INLINE uint8_t mdm_rxc2sdisbcfg_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the RXC2SDISBCFG field of the RXCTRL0 register.
 *
 * The RXCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxc2sdisbcfg - The value to set the field to.
 */
__INLINE void mdm_rxc2sdisbcfg_setf(uint8_t rxc2sdisbcfg)
{
    ASSERT_ERR((((uint32_t)rxc2sdisbcfg << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(MDM_RXCTRL0_ADDR, (REG_PL_RD(MDM_RXCTRL0_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)rxc2sdisbcfg << 13));
}

/**
 * @brief Returns the current value of the RXIQSWAPCFG field in the RXCTRL0 register.
 *
 * The RXCTRL0 register will be read and the RXIQSWAPCFG field's value will be returned.
 *
 * @return The current value of the RXIQSWAPCFG field in the RXCTRL0 register.
 */
__INLINE uint8_t mdm_rxiqswapcfg_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the RXIQSWAPCFG field of the RXCTRL0 register.
 *
 * The RXCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxiqswapcfg - The value to set the field to.
 */
__INLINE void mdm_rxiqswapcfg_setf(uint8_t rxiqswapcfg)
{
    ASSERT_ERR((((uint32_t)rxiqswapcfg << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(MDM_RXCTRL0_ADDR, (REG_PL_RD(MDM_RXCTRL0_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)rxiqswapcfg << 12));
}

/**
 * @brief Returns the current value of the TDBWFORCE40 field in the RXCTRL0 register.
 *
 * The RXCTRL0 register will be read and the TDBWFORCE40 field's value will be returned.
 *
 * @return The current value of the TDBWFORCE40 field in the RXCTRL0 register.
 */
__INLINE uint8_t mdm_tdbwforce40_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the TDBWFORCE40 field of the RXCTRL0 register.
 *
 * The RXCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdbwforce40 - The value to set the field to.
 */
__INLINE void mdm_tdbwforce40_setf(uint8_t tdbwforce40)
{
    ASSERT_ERR((((uint32_t)tdbwforce40 << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(MDM_RXCTRL0_ADDR, (REG_PL_RD(MDM_RXCTRL0_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)tdbwforce40 << 5));
}

/**
 * @brief Returns the current value of the TDBWFORCE20 field in the RXCTRL0 register.
 *
 * The RXCTRL0 register will be read and the TDBWFORCE20 field's value will be returned.
 *
 * @return The current value of the TDBWFORCE20 field in the RXCTRL0 register.
 */
__INLINE uint8_t mdm_tdbwforce20_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the TDBWFORCE20 field of the RXCTRL0 register.
 *
 * The RXCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdbwforce20 - The value to set the field to.
 */
__INLINE void mdm_tdbwforce20_setf(uint8_t tdbwforce20)
{
    ASSERT_ERR((((uint32_t)tdbwforce20 << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(MDM_RXCTRL0_ADDR, (REG_PL_RD(MDM_RXCTRL0_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)tdbwforce20 << 4));
}

/**
 * @brief Returns the current value of the RATEERRORCFG field in the RXCTRL0 register.
 *
 * The RXCTRL0 register will be read and the RATEERRORCFG field's value will be returned.
 *
 * @return The current value of the RATEERRORCFG field in the RXCTRL0 register.
 */
__INLINE uint8_t mdm_rateerrorcfg_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the RATEERRORCFG field of the RXCTRL0 register.
 *
 * The RXCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rateerrorcfg - The value to set the field to.
 */
__INLINE void mdm_rateerrorcfg_setf(uint8_t rateerrorcfg)
{
    ASSERT_ERR((((uint32_t)rateerrorcfg << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(MDM_RXCTRL0_ADDR, (REG_PL_RD(MDM_RXCTRL0_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)rateerrorcfg << 2));
}

/**
 * @brief Returns the current value of the LSIGPARITYCFG field in the RXCTRL0 register.
 *
 * The RXCTRL0 register will be read and the LSIGPARITYCFG field's value will be returned.
 *
 * @return The current value of the LSIGPARITYCFG field in the RXCTRL0 register.
 */
__INLINE uint8_t mdm_lsigparitycfg_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the LSIGPARITYCFG field of the RXCTRL0 register.
 *
 * The RXCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] lsigparitycfg - The value to set the field to.
 */
__INLINE void mdm_lsigparitycfg_setf(uint8_t lsigparitycfg)
{
    ASSERT_ERR((((uint32_t)lsigparitycfg << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(MDM_RXCTRL0_ADDR, (REG_PL_RD(MDM_RXCTRL0_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)lsigparitycfg << 1));
}

/**
 * @brief Returns the current value of the PROPMODECFG field in the RXCTRL0 register.
 *
 * The RXCTRL0 register will be read and the PROPMODECFG field's value will be returned.
 *
 * @return The current value of the PROPMODECFG field in the RXCTRL0 register.
 */
__INLINE uint8_t mdm_propmodecfg_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the PROPMODECFG field of the RXCTRL0 register.
 *
 * The RXCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] propmodecfg - The value to set the field to.
 */
__INLINE void mdm_propmodecfg_setf(uint8_t propmodecfg)
{
    ASSERT_ERR((((uint32_t)propmodecfg << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_RXCTRL0_ADDR, (REG_PL_RD(MDM_RXCTRL0_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)propmodecfg << 0));
}

/// @}

/**
 * @name TXCTRL0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     29         TXC2SDISBCFG   0
 *     28          TXIQSWAPCFG   0
 *  09:00         TXSTARTDELAY   0xB4
 * </pre>
 *
 * @{
 */

/// Address of the TXCTRL0 register
#define MDM_TXCTRL0_ADDR   0x01000838
/// Offset of the TXCTRL0 register from the base address
#define MDM_TXCTRL0_OFFSET 0x00000838
/// Index of the TXCTRL0 register
#define MDM_TXCTRL0_INDEX  0x0000020E
/// Reset value of the TXCTRL0 register
#define MDM_TXCTRL0_RESET  0x000000B4

/**
 * @brief Returns the current value of the TXCTRL0 register.
 * The TXCTRL0 register will be read and its value returned.
 * @return The current value of the TXCTRL0 register.
 */
__INLINE uint32_t mdm_txctrl0_get(void)
{
    return REG_PL_RD(MDM_TXCTRL0_ADDR);
}

/**
 * @brief Sets the TXCTRL0 register to a value.
 * The TXCTRL0 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_txctrl0_set(uint32_t value)
{
    REG_PL_WR(MDM_TXCTRL0_ADDR, value);
}

// field definitions
/// TXC2SDISBCFG field bit
#define MDM_TXC2SDISBCFG_BIT    ((uint32_t)0x20000000)
/// TXC2SDISBCFG field position
#define MDM_TXC2SDISBCFG_POS    29
/// TXIQSWAPCFG field bit
#define MDM_TXIQSWAPCFG_BIT     ((uint32_t)0x10000000)
/// TXIQSWAPCFG field position
#define MDM_TXIQSWAPCFG_POS     28
/// TXSTARTDELAY field mask
#define MDM_TXSTARTDELAY_MASK   ((uint32_t)0x000003FF)
/// TXSTARTDELAY field LSB position
#define MDM_TXSTARTDELAY_LSB    0
/// TXSTARTDELAY field width
#define MDM_TXSTARTDELAY_WIDTH  ((uint32_t)0x0000000A)

/// TXC2SDISBCFG field reset value
#define MDM_TXC2SDISBCFG_RST    0x0
/// TXIQSWAPCFG field reset value
#define MDM_TXIQSWAPCFG_RST     0x0
/// TXSTARTDELAY field reset value
#define MDM_TXSTARTDELAY_RST    0xB4

/**
 * @brief Constructs a value for the TXCTRL0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] txc2sdisbcfg - The value to use for the TXC2SDISBCFG field.
 * @param[in] txiqswapcfg - The value to use for the TXIQSWAPCFG field.
 * @param[in] txstartdelay - The value to use for the TXSTARTDELAY field.
 */
__INLINE void mdm_txctrl0_pack(uint8_t txc2sdisbcfg, uint8_t txiqswapcfg, uint16_t txstartdelay)
{
    ASSERT_ERR((((uint32_t)txc2sdisbcfg << 29) & ~((uint32_t)0x20000000)) == 0);
    ASSERT_ERR((((uint32_t)txiqswapcfg << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)txstartdelay << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(MDM_TXCTRL0_ADDR,  ((uint32_t)txc2sdisbcfg << 29) | ((uint32_t)txiqswapcfg << 28) | ((uint32_t)txstartdelay << 0));
}

/**
 * @brief Unpacks TXCTRL0's fields from current value of the TXCTRL0 register.
 *
 * Reads the TXCTRL0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txc2sdisbcfg - Will be populated with the current value of this field from the register.
 * @param[out] txiqswapcfg - Will be populated with the current value of this field from the register.
 * @param[out] txstartdelay - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_txctrl0_unpack(uint8_t *txc2sdisbcfg, uint8_t *txiqswapcfg, uint16_t *txstartdelay)
{
    uint32_t localVal = REG_PL_RD(MDM_TXCTRL0_ADDR);

    *txc2sdisbcfg = (localVal & ((uint32_t)0x20000000)) >> 29;
    *txiqswapcfg = (localVal & ((uint32_t)0x10000000)) >> 28;
    *txstartdelay = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the TXC2SDISBCFG field in the TXCTRL0 register.
 *
 * The TXCTRL0 register will be read and the TXC2SDISBCFG field's value will be returned.
 *
 * @return The current value of the TXC2SDISBCFG field in the TXCTRL0 register.
 */
__INLINE uint8_t mdm_txc2sdisbcfg_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

/**
 * @brief Sets the TXC2SDISBCFG field of the TXCTRL0 register.
 *
 * The TXCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txc2sdisbcfg - The value to set the field to.
 */
__INLINE void mdm_txc2sdisbcfg_setf(uint8_t txc2sdisbcfg)
{
    ASSERT_ERR((((uint32_t)txc2sdisbcfg << 29) & ~((uint32_t)0x20000000)) == 0);
    REG_PL_WR(MDM_TXCTRL0_ADDR, (REG_PL_RD(MDM_TXCTRL0_ADDR) & ~((uint32_t)0x20000000)) | ((uint32_t)txc2sdisbcfg << 29));
}

/**
 * @brief Returns the current value of the TXIQSWAPCFG field in the TXCTRL0 register.
 *
 * The TXCTRL0 register will be read and the TXIQSWAPCFG field's value will be returned.
 *
 * @return The current value of the TXIQSWAPCFG field in the TXCTRL0 register.
 */
__INLINE uint8_t mdm_txiqswapcfg_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the TXIQSWAPCFG field of the TXCTRL0 register.
 *
 * The TXCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txiqswapcfg - The value to set the field to.
 */
__INLINE void mdm_txiqswapcfg_setf(uint8_t txiqswapcfg)
{
    ASSERT_ERR((((uint32_t)txiqswapcfg << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(MDM_TXCTRL0_ADDR, (REG_PL_RD(MDM_TXCTRL0_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)txiqswapcfg << 28));
}

/**
 * @brief Returns the current value of the TXSTARTDELAY field in the TXCTRL0 register.
 *
 * The TXCTRL0 register will be read and the TXSTARTDELAY field's value will be returned.
 *
 * @return The current value of the TXSTARTDELAY field in the TXCTRL0 register.
 */
__INLINE uint16_t mdm_txstartdelay_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the TXSTARTDELAY field of the TXCTRL0 register.
 *
 * The TXCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txstartdelay - The value to set the field to.
 */
__INLINE void mdm_txstartdelay_setf(uint16_t txstartdelay)
{
    ASSERT_ERR((((uint32_t)txstartdelay << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(MDM_TXCTRL0_ADDR, (REG_PL_RD(MDM_TXCTRL0_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)txstartdelay << 0));
}

/// @}

/**
 * @name RXCTRL1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  12:00           RXNDBPSMAX   0xC30
 * </pre>
 *
 * @{
 */

/// Address of the RXCTRL1 register
#define MDM_RXCTRL1_ADDR   0x0100083C
/// Offset of the RXCTRL1 register from the base address
#define MDM_RXCTRL1_OFFSET 0x0000083C
/// Index of the RXCTRL1 register
#define MDM_RXCTRL1_INDEX  0x0000020F
/// Reset value of the RXCTRL1 register
#define MDM_RXCTRL1_RESET  0x00000C30

/**
 * @brief Returns the current value of the RXCTRL1 register.
 * The RXCTRL1 register will be read and its value returned.
 * @return The current value of the RXCTRL1 register.
 */
__INLINE uint32_t mdm_rxctrl1_get(void)
{
    return REG_PL_RD(MDM_RXCTRL1_ADDR);
}

/**
 * @brief Sets the RXCTRL1 register to a value.
 * The RXCTRL1 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxctrl1_set(uint32_t value)
{
    REG_PL_WR(MDM_RXCTRL1_ADDR, value);
}

// field definitions
/// RXNDBPSMAX field mask
#define MDM_RXNDBPSMAX_MASK   ((uint32_t)0x00001FFF)
/// RXNDBPSMAX field LSB position
#define MDM_RXNDBPSMAX_LSB    0
/// RXNDBPSMAX field width
#define MDM_RXNDBPSMAX_WIDTH  ((uint32_t)0x0000000D)

/// RXNDBPSMAX field reset value
#define MDM_RXNDBPSMAX_RST    0xC30

/**
 * @brief Returns the current value of the RXNDBPSMAX field in the RXCTRL1 register.
 *
 * The RXCTRL1 register will be read and the RXNDBPSMAX field's value will be returned.
 *
 * @return The current value of the RXNDBPSMAX field in the RXCTRL1 register.
 */
__INLINE uint16_t mdm_rxndbpsmax_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00001FFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the RXNDBPSMAX field of the RXCTRL1 register.
 *
 * The RXCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxndbpsmax - The value to set the field to.
 */
__INLINE void mdm_rxndbpsmax_setf(uint16_t rxndbpsmax)
{
    ASSERT_ERR((((uint32_t)rxndbpsmax << 0) & ~((uint32_t)0x00001FFF)) == 0);
    REG_PL_WR(MDM_RXCTRL1_ADDR, (uint32_t)rxndbpsmax << 0);
}

/// @}

/**
 * @name CLKGATECTRL0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31           PHYTXCLKEN   1
 *     30           PHYRXCLKEN   1
 *     29          AGCMEMCLKEN   1
 *     28             AGCCLKEN   1
 *     27             FE3CLKEN   1
 *     26             FE2CLKEN   1
 *     25             FE1CLKEN   1
 *     24             FE0CLKEN   1
 *     23          STOCPECLKEN   1
 *     22             EQUCLKEN   1
 *     21         TDCOMP3CLKEN   1
 *     20         TDCOMP2CLKEN   1
 *     19         TDCOMP1CLKEN   1
 *     18         TDCOMP0CLKEN   1
 *     17         TDFOESTCLKEN   1
 *     16             TBECLKEN   1
 *     15             VTBCLKEN   1
 *     14         FFT3MEMCLKEN   1
 *     13            FFT3CLKEN   1
 *     12         FFT2MEMCLKEN   1
 *     11            FFT2CLKEN   1
 *     10         FFT1MEMCLKEN   1
 *     09            FFT1CLKEN   1
 *     08         FFT0MEMCLKEN   1
 *     07            FFT0CLKEN   1
 *     06           CHESTCLKEN   1
 *     05            GMEMCLKEN   1
 *     04            HMEMCLKEN   1
 *     03            QMEMCLKEN   1
 *     02        INTLVMEMCLKEN   1
 *     01        INTLVVTBCLKEN   1
 *     00        INTLVPHYCLKEN   1
 * </pre>
 *
 * @{
 */

/// Address of the CLKGATECTRL0 register
#define MDM_CLKGATECTRL0_ADDR   0x01000840
/// Offset of the CLKGATECTRL0 register from the base address
#define MDM_CLKGATECTRL0_OFFSET 0x00000840
/// Index of the CLKGATECTRL0 register
#define MDM_CLKGATECTRL0_INDEX  0x00000210
/// Reset value of the CLKGATECTRL0 register
#define MDM_CLKGATECTRL0_RESET  0xFFFFFFFF

/**
 * @brief Returns the current value of the CLKGATECTRL0 register.
 * The CLKGATECTRL0 register will be read and its value returned.
 * @return The current value of the CLKGATECTRL0 register.
 */
__INLINE uint32_t mdm_clkgatectrl0_get(void)
{
    return REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
}

/**
 * @brief Sets the CLKGATECTRL0 register to a value.
 * The CLKGATECTRL0 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_clkgatectrl0_set(uint32_t value)
{
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, value);
}

// field definitions
/// PHYTXCLKEN field bit
#define MDM_PHYTXCLKEN_BIT       ((uint32_t)0x80000000)
/// PHYTXCLKEN field position
#define MDM_PHYTXCLKEN_POS       31
/// PHYRXCLKEN field bit
#define MDM_PHYRXCLKEN_BIT       ((uint32_t)0x40000000)
/// PHYRXCLKEN field position
#define MDM_PHYRXCLKEN_POS       30
/// AGCMEMCLKEN field bit
#define MDM_AGCMEMCLKEN_BIT      ((uint32_t)0x20000000)
/// AGCMEMCLKEN field position
#define MDM_AGCMEMCLKEN_POS      29
/// AGCCLKEN field bit
#define MDM_AGCCLKEN_BIT         ((uint32_t)0x10000000)
/// AGCCLKEN field position
#define MDM_AGCCLKEN_POS         28
/// FE3CLKEN field bit
#define MDM_FE3CLKEN_BIT         ((uint32_t)0x08000000)
/// FE3CLKEN field position
#define MDM_FE3CLKEN_POS         27
/// FE2CLKEN field bit
#define MDM_FE2CLKEN_BIT         ((uint32_t)0x04000000)
/// FE2CLKEN field position
#define MDM_FE2CLKEN_POS         26
/// FE1CLKEN field bit
#define MDM_FE1CLKEN_BIT         ((uint32_t)0x02000000)
/// FE1CLKEN field position
#define MDM_FE1CLKEN_POS         25
/// FE0CLKEN field bit
#define MDM_FE0CLKEN_BIT         ((uint32_t)0x01000000)
/// FE0CLKEN field position
#define MDM_FE0CLKEN_POS         24
/// STOCPECLKEN field bit
#define MDM_STOCPECLKEN_BIT      ((uint32_t)0x00800000)
/// STOCPECLKEN field position
#define MDM_STOCPECLKEN_POS      23
/// EQUCLKEN field bit
#define MDM_EQUCLKEN_BIT         ((uint32_t)0x00400000)
/// EQUCLKEN field position
#define MDM_EQUCLKEN_POS         22
/// TDCOMP3CLKEN field bit
#define MDM_TDCOMP3CLKEN_BIT     ((uint32_t)0x00200000)
/// TDCOMP3CLKEN field position
#define MDM_TDCOMP3CLKEN_POS     21
/// TDCOMP2CLKEN field bit
#define MDM_TDCOMP2CLKEN_BIT     ((uint32_t)0x00100000)
/// TDCOMP2CLKEN field position
#define MDM_TDCOMP2CLKEN_POS     20
/// TDCOMP1CLKEN field bit
#define MDM_TDCOMP1CLKEN_BIT     ((uint32_t)0x00080000)
/// TDCOMP1CLKEN field position
#define MDM_TDCOMP1CLKEN_POS     19
/// TDCOMP0CLKEN field bit
#define MDM_TDCOMP0CLKEN_BIT     ((uint32_t)0x00040000)
/// TDCOMP0CLKEN field position
#define MDM_TDCOMP0CLKEN_POS     18
/// TDFOESTCLKEN field bit
#define MDM_TDFOESTCLKEN_BIT     ((uint32_t)0x00020000)
/// TDFOESTCLKEN field position
#define MDM_TDFOESTCLKEN_POS     17
/// TBECLKEN field bit
#define MDM_TBECLKEN_BIT         ((uint32_t)0x00010000)
/// TBECLKEN field position
#define MDM_TBECLKEN_POS         16
/// VTBCLKEN field bit
#define MDM_VTBCLKEN_BIT         ((uint32_t)0x00008000)
/// VTBCLKEN field position
#define MDM_VTBCLKEN_POS         15
/// FFT3MEMCLKEN field bit
#define MDM_FFT3MEMCLKEN_BIT     ((uint32_t)0x00004000)
/// FFT3MEMCLKEN field position
#define MDM_FFT3MEMCLKEN_POS     14
/// FFT3CLKEN field bit
#define MDM_FFT3CLKEN_BIT        ((uint32_t)0x00002000)
/// FFT3CLKEN field position
#define MDM_FFT3CLKEN_POS        13
/// FFT2MEMCLKEN field bit
#define MDM_FFT2MEMCLKEN_BIT     ((uint32_t)0x00001000)
/// FFT2MEMCLKEN field position
#define MDM_FFT2MEMCLKEN_POS     12
/// FFT2CLKEN field bit
#define MDM_FFT2CLKEN_BIT        ((uint32_t)0x00000800)
/// FFT2CLKEN field position
#define MDM_FFT2CLKEN_POS        11
/// FFT1MEMCLKEN field bit
#define MDM_FFT1MEMCLKEN_BIT     ((uint32_t)0x00000400)
/// FFT1MEMCLKEN field position
#define MDM_FFT1MEMCLKEN_POS     10
/// FFT1CLKEN field bit
#define MDM_FFT1CLKEN_BIT        ((uint32_t)0x00000200)
/// FFT1CLKEN field position
#define MDM_FFT1CLKEN_POS        9
/// FFT0MEMCLKEN field bit
#define MDM_FFT0MEMCLKEN_BIT     ((uint32_t)0x00000100)
/// FFT0MEMCLKEN field position
#define MDM_FFT0MEMCLKEN_POS     8
/// FFT0CLKEN field bit
#define MDM_FFT0CLKEN_BIT        ((uint32_t)0x00000080)
/// FFT0CLKEN field position
#define MDM_FFT0CLKEN_POS        7
/// CHESTCLKEN field bit
#define MDM_CHESTCLKEN_BIT       ((uint32_t)0x00000040)
/// CHESTCLKEN field position
#define MDM_CHESTCLKEN_POS       6
/// GMEMCLKEN field bit
#define MDM_GMEMCLKEN_BIT        ((uint32_t)0x00000020)
/// GMEMCLKEN field position
#define MDM_GMEMCLKEN_POS        5
/// HMEMCLKEN field bit
#define MDM_HMEMCLKEN_BIT        ((uint32_t)0x00000010)
/// HMEMCLKEN field position
#define MDM_HMEMCLKEN_POS        4
/// QMEMCLKEN field bit
#define MDM_QMEMCLKEN_BIT        ((uint32_t)0x00000008)
/// QMEMCLKEN field position
#define MDM_QMEMCLKEN_POS        3
/// INTLVMEMCLKEN field bit
#define MDM_INTLVMEMCLKEN_BIT    ((uint32_t)0x00000004)
/// INTLVMEMCLKEN field position
#define MDM_INTLVMEMCLKEN_POS    2
/// INTLVVTBCLKEN field bit
#define MDM_INTLVVTBCLKEN_BIT    ((uint32_t)0x00000002)
/// INTLVVTBCLKEN field position
#define MDM_INTLVVTBCLKEN_POS    1
/// INTLVPHYCLKEN field bit
#define MDM_INTLVPHYCLKEN_BIT    ((uint32_t)0x00000001)
/// INTLVPHYCLKEN field position
#define MDM_INTLVPHYCLKEN_POS    0

/// PHYTXCLKEN field reset value
#define MDM_PHYTXCLKEN_RST       0x1
/// PHYRXCLKEN field reset value
#define MDM_PHYRXCLKEN_RST       0x1
/// AGCMEMCLKEN field reset value
#define MDM_AGCMEMCLKEN_RST      0x1
/// AGCCLKEN field reset value
#define MDM_AGCCLKEN_RST         0x1
/// FE3CLKEN field reset value
#define MDM_FE3CLKEN_RST         0x1
/// FE2CLKEN field reset value
#define MDM_FE2CLKEN_RST         0x1
/// FE1CLKEN field reset value
#define MDM_FE1CLKEN_RST         0x1
/// FE0CLKEN field reset value
#define MDM_FE0CLKEN_RST         0x1
/// STOCPECLKEN field reset value
#define MDM_STOCPECLKEN_RST      0x1
/// EQUCLKEN field reset value
#define MDM_EQUCLKEN_RST         0x1
/// TDCOMP3CLKEN field reset value
#define MDM_TDCOMP3CLKEN_RST     0x1
/// TDCOMP2CLKEN field reset value
#define MDM_TDCOMP2CLKEN_RST     0x1
/// TDCOMP1CLKEN field reset value
#define MDM_TDCOMP1CLKEN_RST     0x1
/// TDCOMP0CLKEN field reset value
#define MDM_TDCOMP0CLKEN_RST     0x1
/// TDFOESTCLKEN field reset value
#define MDM_TDFOESTCLKEN_RST     0x1
/// TBECLKEN field reset value
#define MDM_TBECLKEN_RST         0x1
/// VTBCLKEN field reset value
#define MDM_VTBCLKEN_RST         0x1
/// FFT3MEMCLKEN field reset value
#define MDM_FFT3MEMCLKEN_RST     0x1
/// FFT3CLKEN field reset value
#define MDM_FFT3CLKEN_RST        0x1
/// FFT2MEMCLKEN field reset value
#define MDM_FFT2MEMCLKEN_RST     0x1
/// FFT2CLKEN field reset value
#define MDM_FFT2CLKEN_RST        0x1
/// FFT1MEMCLKEN field reset value
#define MDM_FFT1MEMCLKEN_RST     0x1
/// FFT1CLKEN field reset value
#define MDM_FFT1CLKEN_RST        0x1
/// FFT0MEMCLKEN field reset value
#define MDM_FFT0MEMCLKEN_RST     0x1
/// FFT0CLKEN field reset value
#define MDM_FFT0CLKEN_RST        0x1
/// CHESTCLKEN field reset value
#define MDM_CHESTCLKEN_RST       0x1
/// GMEMCLKEN field reset value
#define MDM_GMEMCLKEN_RST        0x1
/// HMEMCLKEN field reset value
#define MDM_HMEMCLKEN_RST        0x1
/// QMEMCLKEN field reset value
#define MDM_QMEMCLKEN_RST        0x1
/// INTLVMEMCLKEN field reset value
#define MDM_INTLVMEMCLKEN_RST    0x1
/// INTLVVTBCLKEN field reset value
#define MDM_INTLVVTBCLKEN_RST    0x1
/// INTLVPHYCLKEN field reset value
#define MDM_INTLVPHYCLKEN_RST    0x1

/**
 * @brief Constructs a value for the CLKGATECTRL0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] phytxclken - The value to use for the PHYTXCLKEN field.
 * @param[in] phyrxclken - The value to use for the PHYRXCLKEN field.
 * @param[in] agcmemclken - The value to use for the AGCMEMCLKEN field.
 * @param[in] agcclken - The value to use for the AGCCLKEN field.
 * @param[in] fe3clken - The value to use for the FE3CLKEN field.
 * @param[in] fe2clken - The value to use for the FE2CLKEN field.
 * @param[in] fe1clken - The value to use for the FE1CLKEN field.
 * @param[in] fe0clken - The value to use for the FE0CLKEN field.
 * @param[in] stocpeclken - The value to use for the STOCPECLKEN field.
 * @param[in] equclken - The value to use for the EQUCLKEN field.
 * @param[in] tdcomp3clken - The value to use for the TDCOMP3CLKEN field.
 * @param[in] tdcomp2clken - The value to use for the TDCOMP2CLKEN field.
 * @param[in] tdcomp1clken - The value to use for the TDCOMP1CLKEN field.
 * @param[in] tdcomp0clken - The value to use for the TDCOMP0CLKEN field.
 * @param[in] tdfoestclken - The value to use for the TDFOESTCLKEN field.
 * @param[in] tbeclken - The value to use for the TBECLKEN field.
 * @param[in] vtbclken - The value to use for the VTBCLKEN field.
 * @param[in] fft3memclken - The value to use for the FFT3MEMCLKEN field.
 * @param[in] fft3clken - The value to use for the FFT3CLKEN field.
 * @param[in] fft2memclken - The value to use for the FFT2MEMCLKEN field.
 * @param[in] fft2clken - The value to use for the FFT2CLKEN field.
 * @param[in] fft1memclken - The value to use for the FFT1MEMCLKEN field.
 * @param[in] fft1clken - The value to use for the FFT1CLKEN field.
 * @param[in] fft0memclken - The value to use for the FFT0MEMCLKEN field.
 * @param[in] fft0clken - The value to use for the FFT0CLKEN field.
 * @param[in] chestclken - The value to use for the CHESTCLKEN field.
 * @param[in] gmemclken - The value to use for the GMEMCLKEN field.
 * @param[in] hmemclken - The value to use for the HMEMCLKEN field.
 * @param[in] qmemclken - The value to use for the QMEMCLKEN field.
 * @param[in] intlvmemclken - The value to use for the INTLVMEMCLKEN field.
 * @param[in] intlvvtbclken - The value to use for the INTLVVTBCLKEN field.
 * @param[in] intlvphyclken - The value to use for the INTLVPHYCLKEN field.
 */
__INLINE void mdm_clkgatectrl0_pack(uint8_t phytxclken, uint8_t phyrxclken, uint8_t agcmemclken, uint8_t agcclken, uint8_t fe3clken, uint8_t fe2clken, uint8_t fe1clken, uint8_t fe0clken, uint8_t stocpeclken, uint8_t equclken, uint8_t tdcomp3clken, uint8_t tdcomp2clken, uint8_t tdcomp1clken, uint8_t tdcomp0clken, uint8_t tdfoestclken, uint8_t tbeclken, uint8_t vtbclken, uint8_t fft3memclken, uint8_t fft3clken, uint8_t fft2memclken, uint8_t fft2clken, uint8_t fft1memclken, uint8_t fft1clken, uint8_t fft0memclken, uint8_t fft0clken, uint8_t chestclken, uint8_t gmemclken, uint8_t hmemclken, uint8_t qmemclken, uint8_t intlvmemclken, uint8_t intlvvtbclken, uint8_t intlvphyclken)
{
    ASSERT_ERR((((uint32_t)phytxclken << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)phyrxclken << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)agcmemclken << 29) & ~((uint32_t)0x20000000)) == 0);
    ASSERT_ERR((((uint32_t)agcclken << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)fe3clken << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)fe2clken << 26) & ~((uint32_t)0x04000000)) == 0);
    ASSERT_ERR((((uint32_t)fe1clken << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)fe0clken << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)stocpeclken << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)equclken << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)tdcomp3clken << 21) & ~((uint32_t)0x00200000)) == 0);
    ASSERT_ERR((((uint32_t)tdcomp2clken << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)tdcomp1clken << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)tdcomp0clken << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)tdfoestclken << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)tbeclken << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)vtbclken << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)fft3memclken << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)fft3clken << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)fft2memclken << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)fft2clken << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)fft1memclken << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)fft1clken << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)fft0memclken << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)fft0clken << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)chestclken << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)gmemclken << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)hmemclken << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)qmemclken << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)intlvmemclken << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)intlvvtbclken << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)intlvphyclken << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR,  ((uint32_t)phytxclken << 31) | ((uint32_t)phyrxclken << 30) | ((uint32_t)agcmemclken << 29) | ((uint32_t)agcclken << 28) | ((uint32_t)fe3clken << 27) | ((uint32_t)fe2clken << 26) | ((uint32_t)fe1clken << 25) | ((uint32_t)fe0clken << 24) | ((uint32_t)stocpeclken << 23) | ((uint32_t)equclken << 22) | ((uint32_t)tdcomp3clken << 21) | ((uint32_t)tdcomp2clken << 20) | ((uint32_t)tdcomp1clken << 19) | ((uint32_t)tdcomp0clken << 18) | ((uint32_t)tdfoestclken << 17) | ((uint32_t)tbeclken << 16) | ((uint32_t)vtbclken << 15) | ((uint32_t)fft3memclken << 14) | ((uint32_t)fft3clken << 13) | ((uint32_t)fft2memclken << 12) | ((uint32_t)fft2clken << 11) | ((uint32_t)fft1memclken << 10) | ((uint32_t)fft1clken << 9) | ((uint32_t)fft0memclken << 8) | ((uint32_t)fft0clken << 7) | ((uint32_t)chestclken << 6) | ((uint32_t)gmemclken << 5) | ((uint32_t)hmemclken << 4) | ((uint32_t)qmemclken << 3) | ((uint32_t)intlvmemclken << 2) | ((uint32_t)intlvvtbclken << 1) | ((uint32_t)intlvphyclken << 0));
}

/**
 * @brief Unpacks CLKGATECTRL0's fields from current value of the CLKGATECTRL0 register.
 *
 * Reads the CLKGATECTRL0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] phytxclken - Will be populated with the current value of this field from the register.
 * @param[out] phyrxclken - Will be populated with the current value of this field from the register.
 * @param[out] agcmemclken - Will be populated with the current value of this field from the register.
 * @param[out] agcclken - Will be populated with the current value of this field from the register.
 * @param[out] fe3clken - Will be populated with the current value of this field from the register.
 * @param[out] fe2clken - Will be populated with the current value of this field from the register.
 * @param[out] fe1clken - Will be populated with the current value of this field from the register.
 * @param[out] fe0clken - Will be populated with the current value of this field from the register.
 * @param[out] stocpeclken - Will be populated with the current value of this field from the register.
 * @param[out] equclken - Will be populated with the current value of this field from the register.
 * @param[out] tdcomp3clken - Will be populated with the current value of this field from the register.
 * @param[out] tdcomp2clken - Will be populated with the current value of this field from the register.
 * @param[out] tdcomp1clken - Will be populated with the current value of this field from the register.
 * @param[out] tdcomp0clken - Will be populated with the current value of this field from the register.
 * @param[out] tdfoestclken - Will be populated with the current value of this field from the register.
 * @param[out] tbeclken - Will be populated with the current value of this field from the register.
 * @param[out] vtbclken - Will be populated with the current value of this field from the register.
 * @param[out] fft3memclken - Will be populated with the current value of this field from the register.
 * @param[out] fft3clken - Will be populated with the current value of this field from the register.
 * @param[out] fft2memclken - Will be populated with the current value of this field from the register.
 * @param[out] fft2clken - Will be populated with the current value of this field from the register.
 * @param[out] fft1memclken - Will be populated with the current value of this field from the register.
 * @param[out] fft1clken - Will be populated with the current value of this field from the register.
 * @param[out] fft0memclken - Will be populated with the current value of this field from the register.
 * @param[out] fft0clken - Will be populated with the current value of this field from the register.
 * @param[out] chestclken - Will be populated with the current value of this field from the register.
 * @param[out] gmemclken - Will be populated with the current value of this field from the register.
 * @param[out] hmemclken - Will be populated with the current value of this field from the register.
 * @param[out] qmemclken - Will be populated with the current value of this field from the register.
 * @param[out] intlvmemclken - Will be populated with the current value of this field from the register.
 * @param[out] intlvvtbclken - Will be populated with the current value of this field from the register.
 * @param[out] intlvphyclken - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_clkgatectrl0_unpack(uint8_t *phytxclken, uint8_t *phyrxclken, uint8_t *agcmemclken, uint8_t *agcclken, uint8_t *fe3clken, uint8_t *fe2clken, uint8_t *fe1clken, uint8_t *fe0clken, uint8_t *stocpeclken, uint8_t *equclken, uint8_t *tdcomp3clken, uint8_t *tdcomp2clken, uint8_t *tdcomp1clken, uint8_t *tdcomp0clken, uint8_t *tdfoestclken, uint8_t *tbeclken, uint8_t *vtbclken, uint8_t *fft3memclken, uint8_t *fft3clken, uint8_t *fft2memclken, uint8_t *fft2clken, uint8_t *fft1memclken, uint8_t *fft1clken, uint8_t *fft0memclken, uint8_t *fft0clken, uint8_t *chestclken, uint8_t *gmemclken, uint8_t *hmemclken, uint8_t *qmemclken, uint8_t *intlvmemclken, uint8_t *intlvvtbclken, uint8_t *intlvphyclken)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);

    *phytxclken = (localVal & ((uint32_t)0x80000000)) >> 31;
    *phyrxclken = (localVal & ((uint32_t)0x40000000)) >> 30;
    *agcmemclken = (localVal & ((uint32_t)0x20000000)) >> 29;
    *agcclken = (localVal & ((uint32_t)0x10000000)) >> 28;
    *fe3clken = (localVal & ((uint32_t)0x08000000)) >> 27;
    *fe2clken = (localVal & ((uint32_t)0x04000000)) >> 26;
    *fe1clken = (localVal & ((uint32_t)0x02000000)) >> 25;
    *fe0clken = (localVal & ((uint32_t)0x01000000)) >> 24;
    *stocpeclken = (localVal & ((uint32_t)0x00800000)) >> 23;
    *equclken = (localVal & ((uint32_t)0x00400000)) >> 22;
    *tdcomp3clken = (localVal & ((uint32_t)0x00200000)) >> 21;
    *tdcomp2clken = (localVal & ((uint32_t)0x00100000)) >> 20;
    *tdcomp1clken = (localVal & ((uint32_t)0x00080000)) >> 19;
    *tdcomp0clken = (localVal & ((uint32_t)0x00040000)) >> 18;
    *tdfoestclken = (localVal & ((uint32_t)0x00020000)) >> 17;
    *tbeclken = (localVal & ((uint32_t)0x00010000)) >> 16;
    *vtbclken = (localVal & ((uint32_t)0x00008000)) >> 15;
    *fft3memclken = (localVal & ((uint32_t)0x00004000)) >> 14;
    *fft3clken = (localVal & ((uint32_t)0x00002000)) >> 13;
    *fft2memclken = (localVal & ((uint32_t)0x00001000)) >> 12;
    *fft2clken = (localVal & ((uint32_t)0x00000800)) >> 11;
    *fft1memclken = (localVal & ((uint32_t)0x00000400)) >> 10;
    *fft1clken = (localVal & ((uint32_t)0x00000200)) >> 9;
    *fft0memclken = (localVal & ((uint32_t)0x00000100)) >> 8;
    *fft0clken = (localVal & ((uint32_t)0x00000080)) >> 7;
    *chestclken = (localVal & ((uint32_t)0x00000040)) >> 6;
    *gmemclken = (localVal & ((uint32_t)0x00000020)) >> 5;
    *hmemclken = (localVal & ((uint32_t)0x00000010)) >> 4;
    *qmemclken = (localVal & ((uint32_t)0x00000008)) >> 3;
    *intlvmemclken = (localVal & ((uint32_t)0x00000004)) >> 2;
    *intlvvtbclken = (localVal & ((uint32_t)0x00000002)) >> 1;
    *intlvphyclken = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the PHYTXCLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the PHYTXCLKEN field's value will be returned.
 *
 * @return The current value of the PHYTXCLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_phytxclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the PHYTXCLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] phytxclken - The value to set the field to.
 */
__INLINE void mdm_phytxclken_setf(uint8_t phytxclken)
{
    ASSERT_ERR((((uint32_t)phytxclken << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)phytxclken << 31));
}

/**
 * @brief Returns the current value of the PHYRXCLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the PHYRXCLKEN field's value will be returned.
 *
 * @return The current value of the PHYRXCLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_phyrxclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

/**
 * @brief Sets the PHYRXCLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] phyrxclken - The value to set the field to.
 */
__INLINE void mdm_phyrxclken_setf(uint8_t phyrxclken)
{
    ASSERT_ERR((((uint32_t)phyrxclken << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)phyrxclken << 30));
}

/**
 * @brief Returns the current value of the AGCMEMCLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the AGCMEMCLKEN field's value will be returned.
 *
 * @return The current value of the AGCMEMCLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_agcmemclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

/**
 * @brief Sets the AGCMEMCLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] agcmemclken - The value to set the field to.
 */
__INLINE void mdm_agcmemclken_setf(uint8_t agcmemclken)
{
    ASSERT_ERR((((uint32_t)agcmemclken << 29) & ~((uint32_t)0x20000000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x20000000)) | ((uint32_t)agcmemclken << 29));
}

/**
 * @brief Returns the current value of the AGCCLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the AGCCLKEN field's value will be returned.
 *
 * @return The current value of the AGCCLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_agcclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the AGCCLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] agcclken - The value to set the field to.
 */
__INLINE void mdm_agcclken_setf(uint8_t agcclken)
{
    ASSERT_ERR((((uint32_t)agcclken << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)agcclken << 28));
}

/**
 * @brief Returns the current value of the FE3CLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the FE3CLKEN field's value will be returned.
 *
 * @return The current value of the FE3CLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_fe3clken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Sets the FE3CLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fe3clken - The value to set the field to.
 */
__INLINE void mdm_fe3clken_setf(uint8_t fe3clken)
{
    ASSERT_ERR((((uint32_t)fe3clken << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x08000000)) | ((uint32_t)fe3clken << 27));
}

/**
 * @brief Returns the current value of the FE2CLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the FE2CLKEN field's value will be returned.
 *
 * @return The current value of the FE2CLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_fe2clken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

/**
 * @brief Sets the FE2CLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fe2clken - The value to set the field to.
 */
__INLINE void mdm_fe2clken_setf(uint8_t fe2clken)
{
    ASSERT_ERR((((uint32_t)fe2clken << 26) & ~((uint32_t)0x04000000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x04000000)) | ((uint32_t)fe2clken << 26));
}

/**
 * @brief Returns the current value of the FE1CLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the FE1CLKEN field's value will be returned.
 *
 * @return The current value of the FE1CLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_fe1clken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

/**
 * @brief Sets the FE1CLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fe1clken - The value to set the field to.
 */
__INLINE void mdm_fe1clken_setf(uint8_t fe1clken)
{
    ASSERT_ERR((((uint32_t)fe1clken << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x02000000)) | ((uint32_t)fe1clken << 25));
}

/**
 * @brief Returns the current value of the FE0CLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the FE0CLKEN field's value will be returned.
 *
 * @return The current value of the FE0CLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_fe0clken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the FE0CLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fe0clken - The value to set the field to.
 */
__INLINE void mdm_fe0clken_setf(uint8_t fe0clken)
{
    ASSERT_ERR((((uint32_t)fe0clken << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)fe0clken << 24));
}

/**
 * @brief Returns the current value of the STOCPECLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the STOCPECLKEN field's value will be returned.
 *
 * @return The current value of the STOCPECLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_stocpeclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Sets the STOCPECLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] stocpeclken - The value to set the field to.
 */
__INLINE void mdm_stocpeclken_setf(uint8_t stocpeclken)
{
    ASSERT_ERR((((uint32_t)stocpeclken << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00800000)) | ((uint32_t)stocpeclken << 23));
}

/**
 * @brief Returns the current value of the EQUCLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the EQUCLKEN field's value will be returned.
 *
 * @return The current value of the EQUCLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_equclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Sets the EQUCLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] equclken - The value to set the field to.
 */
__INLINE void mdm_equclken_setf(uint8_t equclken)
{
    ASSERT_ERR((((uint32_t)equclken << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00400000)) | ((uint32_t)equclken << 22));
}

/**
 * @brief Returns the current value of the TDCOMP3CLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the TDCOMP3CLKEN field's value will be returned.
 *
 * @return The current value of the TDCOMP3CLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_tdcomp3clken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Sets the TDCOMP3CLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdcomp3clken - The value to set the field to.
 */
__INLINE void mdm_tdcomp3clken_setf(uint8_t tdcomp3clken)
{
    ASSERT_ERR((((uint32_t)tdcomp3clken << 21) & ~((uint32_t)0x00200000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00200000)) | ((uint32_t)tdcomp3clken << 21));
}

/**
 * @brief Returns the current value of the TDCOMP2CLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the TDCOMP2CLKEN field's value will be returned.
 *
 * @return The current value of the TDCOMP2CLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_tdcomp2clken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Sets the TDCOMP2CLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdcomp2clken - The value to set the field to.
 */
__INLINE void mdm_tdcomp2clken_setf(uint8_t tdcomp2clken)
{
    ASSERT_ERR((((uint32_t)tdcomp2clken << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00100000)) | ((uint32_t)tdcomp2clken << 20));
}

/**
 * @brief Returns the current value of the TDCOMP1CLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the TDCOMP1CLKEN field's value will be returned.
 *
 * @return The current value of the TDCOMP1CLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_tdcomp1clken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Sets the TDCOMP1CLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdcomp1clken - The value to set the field to.
 */
__INLINE void mdm_tdcomp1clken_setf(uint8_t tdcomp1clken)
{
    ASSERT_ERR((((uint32_t)tdcomp1clken << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00080000)) | ((uint32_t)tdcomp1clken << 19));
}

/**
 * @brief Returns the current value of the TDCOMP0CLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the TDCOMP0CLKEN field's value will be returned.
 *
 * @return The current value of the TDCOMP0CLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_tdcomp0clken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Sets the TDCOMP0CLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdcomp0clken - The value to set the field to.
 */
__INLINE void mdm_tdcomp0clken_setf(uint8_t tdcomp0clken)
{
    ASSERT_ERR((((uint32_t)tdcomp0clken << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00040000)) | ((uint32_t)tdcomp0clken << 18));
}

/**
 * @brief Returns the current value of the TDFOESTCLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the TDFOESTCLKEN field's value will be returned.
 *
 * @return The current value of the TDFOESTCLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_tdfoestclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Sets the TDFOESTCLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdfoestclken - The value to set the field to.
 */
__INLINE void mdm_tdfoestclken_setf(uint8_t tdfoestclken)
{
    ASSERT_ERR((((uint32_t)tdfoestclken << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00020000)) | ((uint32_t)tdfoestclken << 17));
}

/**
 * @brief Returns the current value of the TBECLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the TBECLKEN field's value will be returned.
 *
 * @return The current value of the TBECLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_tbeclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the TBECLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tbeclken - The value to set the field to.
 */
__INLINE void mdm_tbeclken_setf(uint8_t tbeclken)
{
    ASSERT_ERR((((uint32_t)tbeclken << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)tbeclken << 16));
}

/**
 * @brief Returns the current value of the VTBCLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the VTBCLKEN field's value will be returned.
 *
 * @return The current value of the VTBCLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_vtbclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

/**
 * @brief Sets the VTBCLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] vtbclken - The value to set the field to.
 */
__INLINE void mdm_vtbclken_setf(uint8_t vtbclken)
{
    ASSERT_ERR((((uint32_t)vtbclken << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00008000)) | ((uint32_t)vtbclken << 15));
}

/**
 * @brief Returns the current value of the FFT3MEMCLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the FFT3MEMCLKEN field's value will be returned.
 *
 * @return The current value of the FFT3MEMCLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_fft3memclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Sets the FFT3MEMCLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft3memclken - The value to set the field to.
 */
__INLINE void mdm_fft3memclken_setf(uint8_t fft3memclken)
{
    ASSERT_ERR((((uint32_t)fft3memclken << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00004000)) | ((uint32_t)fft3memclken << 14));
}

/**
 * @brief Returns the current value of the FFT3CLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the FFT3CLKEN field's value will be returned.
 *
 * @return The current value of the FFT3CLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_fft3clken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the FFT3CLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft3clken - The value to set the field to.
 */
__INLINE void mdm_fft3clken_setf(uint8_t fft3clken)
{
    ASSERT_ERR((((uint32_t)fft3clken << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)fft3clken << 13));
}

/**
 * @brief Returns the current value of the FFT2MEMCLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the FFT2MEMCLKEN field's value will be returned.
 *
 * @return The current value of the FFT2MEMCLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_fft2memclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the FFT2MEMCLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft2memclken - The value to set the field to.
 */
__INLINE void mdm_fft2memclken_setf(uint8_t fft2memclken)
{
    ASSERT_ERR((((uint32_t)fft2memclken << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)fft2memclken << 12));
}

/**
 * @brief Returns the current value of the FFT2CLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the FFT2CLKEN field's value will be returned.
 *
 * @return The current value of the FFT2CLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_fft2clken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the FFT2CLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft2clken - The value to set the field to.
 */
__INLINE void mdm_fft2clken_setf(uint8_t fft2clken)
{
    ASSERT_ERR((((uint32_t)fft2clken << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00000800)) | ((uint32_t)fft2clken << 11));
}

/**
 * @brief Returns the current value of the FFT1MEMCLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the FFT1MEMCLKEN field's value will be returned.
 *
 * @return The current value of the FFT1MEMCLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_fft1memclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the FFT1MEMCLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft1memclken - The value to set the field to.
 */
__INLINE void mdm_fft1memclken_setf(uint8_t fft1memclken)
{
    ASSERT_ERR((((uint32_t)fft1memclken << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00000400)) | ((uint32_t)fft1memclken << 10));
}

/**
 * @brief Returns the current value of the FFT1CLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the FFT1CLKEN field's value will be returned.
 *
 * @return The current value of the FFT1CLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_fft1clken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the FFT1CLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft1clken - The value to set the field to.
 */
__INLINE void mdm_fft1clken_setf(uint8_t fft1clken)
{
    ASSERT_ERR((((uint32_t)fft1clken << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)fft1clken << 9));
}

/**
 * @brief Returns the current value of the FFT0MEMCLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the FFT0MEMCLKEN field's value will be returned.
 *
 * @return The current value of the FFT0MEMCLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_fft0memclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the FFT0MEMCLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft0memclken - The value to set the field to.
 */
__INLINE void mdm_fft0memclken_setf(uint8_t fft0memclken)
{
    ASSERT_ERR((((uint32_t)fft0memclken << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)fft0memclken << 8));
}

/**
 * @brief Returns the current value of the FFT0CLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the FFT0CLKEN field's value will be returned.
 *
 * @return The current value of the FFT0CLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_fft0clken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the FFT0CLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft0clken - The value to set the field to.
 */
__INLINE void mdm_fft0clken_setf(uint8_t fft0clken)
{
    ASSERT_ERR((((uint32_t)fft0clken << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)fft0clken << 7));
}

/**
 * @brief Returns the current value of the CHESTCLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the CHESTCLKEN field's value will be returned.
 *
 * @return The current value of the CHESTCLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_chestclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the CHESTCLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] chestclken - The value to set the field to.
 */
__INLINE void mdm_chestclken_setf(uint8_t chestclken)
{
    ASSERT_ERR((((uint32_t)chestclken << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)chestclken << 6));
}

/**
 * @brief Returns the current value of the GMEMCLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the GMEMCLKEN field's value will be returned.
 *
 * @return The current value of the GMEMCLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_gmemclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the GMEMCLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] gmemclken - The value to set the field to.
 */
__INLINE void mdm_gmemclken_setf(uint8_t gmemclken)
{
    ASSERT_ERR((((uint32_t)gmemclken << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)gmemclken << 5));
}

/**
 * @brief Returns the current value of the HMEMCLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the HMEMCLKEN field's value will be returned.
 *
 * @return The current value of the HMEMCLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_hmemclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the HMEMCLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] hmemclken - The value to set the field to.
 */
__INLINE void mdm_hmemclken_setf(uint8_t hmemclken)
{
    ASSERT_ERR((((uint32_t)hmemclken << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)hmemclken << 4));
}

/**
 * @brief Returns the current value of the QMEMCLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the QMEMCLKEN field's value will be returned.
 *
 * @return The current value of the QMEMCLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_qmemclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the QMEMCLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] qmemclken - The value to set the field to.
 */
__INLINE void mdm_qmemclken_setf(uint8_t qmemclken)
{
    ASSERT_ERR((((uint32_t)qmemclken << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)qmemclken << 3));
}

/**
 * @brief Returns the current value of the INTLVMEMCLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the INTLVMEMCLKEN field's value will be returned.
 *
 * @return The current value of the INTLVMEMCLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_intlvmemclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the INTLVMEMCLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] intlvmemclken - The value to set the field to.
 */
__INLINE void mdm_intlvmemclken_setf(uint8_t intlvmemclken)
{
    ASSERT_ERR((((uint32_t)intlvmemclken << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)intlvmemclken << 2));
}

/**
 * @brief Returns the current value of the INTLVVTBCLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the INTLVVTBCLKEN field's value will be returned.
 *
 * @return The current value of the INTLVVTBCLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_intlvvtbclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the INTLVVTBCLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] intlvvtbclken - The value to set the field to.
 */
__INLINE void mdm_intlvvtbclken_setf(uint8_t intlvvtbclken)
{
    ASSERT_ERR((((uint32_t)intlvvtbclken << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)intlvvtbclken << 1));
}

/**
 * @brief Returns the current value of the INTLVPHYCLKEN field in the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read and the INTLVPHYCLKEN field's value will be returned.
 *
 * @return The current value of the INTLVPHYCLKEN field in the CLKGATECTRL0 register.
 */
__INLINE uint8_t mdm_intlvphyclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the INTLVPHYCLKEN field of the CLKGATECTRL0 register.
 *
 * The CLKGATECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] intlvphyclken - The value to set the field to.
 */
__INLINE void mdm_intlvphyclken_setf(uint8_t intlvphyclken)
{
    ASSERT_ERR((((uint32_t)intlvphyclken << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL0_ADDR, (REG_PL_RD(MDM_CLKGATECTRL0_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)intlvphyclken << 0));
}

/// @}

/**
 * @name CLKGATECTRL1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     05        RADARTIMCLKEN   1
 *     04          MDMBTXCLKEN   1
 *     03          MDMBRXCLKEN   1
 * </pre>
 *
 * @{
 */

/// Address of the CLKGATECTRL1 register
#define MDM_CLKGATECTRL1_ADDR   0x01000844
/// Offset of the CLKGATECTRL1 register from the base address
#define MDM_CLKGATECTRL1_OFFSET 0x00000844
/// Index of the CLKGATECTRL1 register
#define MDM_CLKGATECTRL1_INDEX  0x00000211
/// Reset value of the CLKGATECTRL1 register
#define MDM_CLKGATECTRL1_RESET  0x00000038

/**
 * @brief Returns the current value of the CLKGATECTRL1 register.
 * The CLKGATECTRL1 register will be read and its value returned.
 * @return The current value of the CLKGATECTRL1 register.
 */
__INLINE uint32_t mdm_clkgatectrl1_get(void)
{
    return REG_PL_RD(MDM_CLKGATECTRL1_ADDR);
}

/**
 * @brief Sets the CLKGATECTRL1 register to a value.
 * The CLKGATECTRL1 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_clkgatectrl1_set(uint32_t value)
{
    REG_PL_WR(MDM_CLKGATECTRL1_ADDR, value);
}

// field definitions
/// RADARTIMCLKEN field bit
#define MDM_RADARTIMCLKEN_BIT    ((uint32_t)0x00000020)
/// RADARTIMCLKEN field position
#define MDM_RADARTIMCLKEN_POS    5
/// MDMBTXCLKEN field bit
#define MDM_MDMBTXCLKEN_BIT      ((uint32_t)0x00000010)
/// MDMBTXCLKEN field position
#define MDM_MDMBTXCLKEN_POS      4
/// MDMBRXCLKEN field bit
#define MDM_MDMBRXCLKEN_BIT      ((uint32_t)0x00000008)
/// MDMBRXCLKEN field position
#define MDM_MDMBRXCLKEN_POS      3

/// RADARTIMCLKEN field reset value
#define MDM_RADARTIMCLKEN_RST    0x1
/// MDMBTXCLKEN field reset value
#define MDM_MDMBTXCLKEN_RST      0x1
/// MDMBRXCLKEN field reset value
#define MDM_MDMBRXCLKEN_RST      0x1

/**
 * @brief Constructs a value for the CLKGATECTRL1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] radartimclken - The value to use for the RADARTIMCLKEN field.
 * @param[in] mdmbtxclken - The value to use for the MDMBTXCLKEN field.
 * @param[in] mdmbrxclken - The value to use for the MDMBRXCLKEN field.
 */
__INLINE void mdm_clkgatectrl1_pack(uint8_t radartimclken, uint8_t mdmbtxclken, uint8_t mdmbrxclken)
{
    ASSERT_ERR((((uint32_t)radartimclken << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)mdmbtxclken << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)mdmbrxclken << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL1_ADDR,  ((uint32_t)radartimclken << 5) | ((uint32_t)mdmbtxclken << 4) | ((uint32_t)mdmbrxclken << 3));
}

/**
 * @brief Unpacks CLKGATECTRL1's fields from current value of the CLKGATECTRL1 register.
 *
 * Reads the CLKGATECTRL1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] radartimclken - Will be populated with the current value of this field from the register.
 * @param[out] mdmbtxclken - Will be populated with the current value of this field from the register.
 * @param[out] mdmbrxclken - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_clkgatectrl1_unpack(uint8_t *radartimclken, uint8_t *mdmbtxclken, uint8_t *mdmbrxclken)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL1_ADDR);

    *radartimclken = (localVal & ((uint32_t)0x00000020)) >> 5;
    *mdmbtxclken = (localVal & ((uint32_t)0x00000010)) >> 4;
    *mdmbrxclken = (localVal & ((uint32_t)0x00000008)) >> 3;
}

/**
 * @brief Returns the current value of the RADARTIMCLKEN field in the CLKGATECTRL1 register.
 *
 * The CLKGATECTRL1 register will be read and the RADARTIMCLKEN field's value will be returned.
 *
 * @return The current value of the RADARTIMCLKEN field in the CLKGATECTRL1 register.
 */
__INLINE uint8_t mdm_radartimclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the RADARTIMCLKEN field of the CLKGATECTRL1 register.
 *
 * The CLKGATECTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] radartimclken - The value to set the field to.
 */
__INLINE void mdm_radartimclken_setf(uint8_t radartimclken)
{
    ASSERT_ERR((((uint32_t)radartimclken << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL1_ADDR, (REG_PL_RD(MDM_CLKGATECTRL1_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)radartimclken << 5));
}

/**
 * @brief Returns the current value of the MDMBTXCLKEN field in the CLKGATECTRL1 register.
 *
 * The CLKGATECTRL1 register will be read and the MDMBTXCLKEN field's value will be returned.
 *
 * @return The current value of the MDMBTXCLKEN field in the CLKGATECTRL1 register.
 */
__INLINE uint8_t mdm_mdmbtxclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the MDMBTXCLKEN field of the CLKGATECTRL1 register.
 *
 * The CLKGATECTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mdmbtxclken - The value to set the field to.
 */
__INLINE void mdm_mdmbtxclken_setf(uint8_t mdmbtxclken)
{
    ASSERT_ERR((((uint32_t)mdmbtxclken << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL1_ADDR, (REG_PL_RD(MDM_CLKGATECTRL1_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)mdmbtxclken << 4));
}

/**
 * @brief Returns the current value of the MDMBRXCLKEN field in the CLKGATECTRL1 register.
 *
 * The CLKGATECTRL1 register will be read and the MDMBRXCLKEN field's value will be returned.
 *
 * @return The current value of the MDMBRXCLKEN field in the CLKGATECTRL1 register.
 */
__INLINE uint8_t mdm_mdmbrxclken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATECTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the MDMBRXCLKEN field of the CLKGATECTRL1 register.
 *
 * The CLKGATECTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mdmbrxclken - The value to set the field to.
 */
__INLINE void mdm_mdmbrxclken_setf(uint8_t mdmbrxclken)
{
    ASSERT_ERR((((uint32_t)mdmbrxclken << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MDM_CLKGATECTRL1_ADDR, (REG_PL_RD(MDM_CLKGATECTRL1_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)mdmbrxclken << 3));
}

/// @}

/**
 * @name MEMCLKCTRL0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     03        AGCMEMCLKCTRL   1
 * </pre>
 *
 * @{
 */

/// Address of the MEMCLKCTRL0 register
#define MDM_MEMCLKCTRL0_ADDR   0x01000848
/// Offset of the MEMCLKCTRL0 register from the base address
#define MDM_MEMCLKCTRL0_OFFSET 0x00000848
/// Index of the MEMCLKCTRL0 register
#define MDM_MEMCLKCTRL0_INDEX  0x00000212
/// Reset value of the MEMCLKCTRL0 register
#define MDM_MEMCLKCTRL0_RESET  0x00000008

/**
 * @brief Returns the current value of the MEMCLKCTRL0 register.
 * The MEMCLKCTRL0 register will be read and its value returned.
 * @return The current value of the MEMCLKCTRL0 register.
 */
__INLINE uint32_t mdm_memclkctrl0_get(void)
{
    return REG_PL_RD(MDM_MEMCLKCTRL0_ADDR);
}

/**
 * @brief Sets the MEMCLKCTRL0 register to a value.
 * The MEMCLKCTRL0 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_memclkctrl0_set(uint32_t value)
{
    REG_PL_WR(MDM_MEMCLKCTRL0_ADDR, value);
}

// field definitions
/// AGCMEMCLKCTRL field bit
#define MDM_AGCMEMCLKCTRL_BIT    ((uint32_t)0x00000008)
/// AGCMEMCLKCTRL field position
#define MDM_AGCMEMCLKCTRL_POS    3

/// AGCMEMCLKCTRL field reset value
#define MDM_AGCMEMCLKCTRL_RST    0x1

/**
 * @brief Returns the current value of the AGCMEMCLKCTRL field in the MEMCLKCTRL0 register.
 *
 * The MEMCLKCTRL0 register will be read and the AGCMEMCLKCTRL field's value will be returned.
 *
 * @return The current value of the AGCMEMCLKCTRL field in the MEMCLKCTRL0 register.
 */
__INLINE uint8_t mdm_agcmemclkctrl_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MEMCLKCTRL0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000008)) == 0);
    return (localVal >> 3);
}

/**
 * @brief Sets the AGCMEMCLKCTRL field of the MEMCLKCTRL0 register.
 *
 * The MEMCLKCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] agcmemclkctrl - The value to set the field to.
 */
__INLINE void mdm_agcmemclkctrl_setf(uint8_t agcmemclkctrl)
{
    ASSERT_ERR((((uint32_t)agcmemclkctrl << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MDM_MEMCLKCTRL0_ADDR, (uint32_t)agcmemclkctrl << 3);
}

/// @}

/**
 * @name IRQCTRL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     14      IRQHTSIGVALIDEN   0
 *     13       IRQLSIGVALIDEN   0
 *     12      IRQTXOVERFLOWEN   0
 *     11      IRQTXUNDERRUNEN   0
 *     10      IRQCCA40SFALLEN   0
 *     09      IRQCCA40SRISEEN   0
 *     08           IRQTXENDEN   0
 *     07           IRQRXENDEN   0
 *     06      IRQCCA20PFALLEN   0
 *     05      IRQCCA20SFALLEN   0
 *     04      IRQCCA20PRISEEN   0
 *     03      IRQCCA20SRISEEN   0
 *     02        IRQPHYERROREN   0
 *     01        IRQRADARDETEN   0
 *     00      IRQCCATIMEOUTEN   0
 * </pre>
 *
 * @{
 */

/// Address of the IRQCTRL register
#define MDM_IRQCTRL_ADDR   0x0100084C
/// Offset of the IRQCTRL register from the base address
#define MDM_IRQCTRL_OFFSET 0x0000084C
/// Index of the IRQCTRL register
#define MDM_IRQCTRL_INDEX  0x00000213
/// Reset value of the IRQCTRL register
#define MDM_IRQCTRL_RESET  0x00000000

/**
 * @brief Returns the current value of the IRQCTRL register.
 * The IRQCTRL register will be read and its value returned.
 * @return The current value of the IRQCTRL register.
 */
__INLINE uint32_t mdm_irqctrl_get(void)
{
    return REG_PL_RD(MDM_IRQCTRL_ADDR);
}

/**
 * @brief Sets the IRQCTRL register to a value.
 * The IRQCTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_irqctrl_set(uint32_t value)
{
    REG_PL_WR(MDM_IRQCTRL_ADDR, value);
}

// field definitions
/// IRQHTSIGVALIDEN field bit
#define MDM_IRQHTSIGVALIDEN_BIT    ((uint32_t)0x00004000)
/// IRQHTSIGVALIDEN field position
#define MDM_IRQHTSIGVALIDEN_POS    14
/// IRQLSIGVALIDEN field bit
#define MDM_IRQLSIGVALIDEN_BIT     ((uint32_t)0x00002000)
/// IRQLSIGVALIDEN field position
#define MDM_IRQLSIGVALIDEN_POS     13
/// IRQTXOVERFLOWEN field bit
#define MDM_IRQTXOVERFLOWEN_BIT    ((uint32_t)0x00001000)
/// IRQTXOVERFLOWEN field position
#define MDM_IRQTXOVERFLOWEN_POS    12
/// IRQTXUNDERRUNEN field bit
#define MDM_IRQTXUNDERRUNEN_BIT    ((uint32_t)0x00000800)
/// IRQTXUNDERRUNEN field position
#define MDM_IRQTXUNDERRUNEN_POS    11
/// IRQCCA40SFALLEN field bit
#define MDM_IRQCCA40SFALLEN_BIT    ((uint32_t)0x00000400)
/// IRQCCA40SFALLEN field position
#define MDM_IRQCCA40SFALLEN_POS    10
/// IRQCCA40SRISEEN field bit
#define MDM_IRQCCA40SRISEEN_BIT    ((uint32_t)0x00000200)
/// IRQCCA40SRISEEN field position
#define MDM_IRQCCA40SRISEEN_POS    9
/// IRQTXENDEN field bit
#define MDM_IRQTXENDEN_BIT         ((uint32_t)0x00000100)
/// IRQTXENDEN field position
#define MDM_IRQTXENDEN_POS         8
/// IRQRXENDEN field bit
#define MDM_IRQRXENDEN_BIT         ((uint32_t)0x00000080)
/// IRQRXENDEN field position
#define MDM_IRQRXENDEN_POS         7
/// IRQCCA20PFALLEN field bit
#define MDM_IRQCCA20PFALLEN_BIT    ((uint32_t)0x00000040)
/// IRQCCA20PFALLEN field position
#define MDM_IRQCCA20PFALLEN_POS    6
/// IRQCCA20SFALLEN field bit
#define MDM_IRQCCA20SFALLEN_BIT    ((uint32_t)0x00000020)
/// IRQCCA20SFALLEN field position
#define MDM_IRQCCA20SFALLEN_POS    5
/// IRQCCA20PRISEEN field bit
#define MDM_IRQCCA20PRISEEN_BIT    ((uint32_t)0x00000010)
/// IRQCCA20PRISEEN field position
#define MDM_IRQCCA20PRISEEN_POS    4
/// IRQCCA20SRISEEN field bit
#define MDM_IRQCCA20SRISEEN_BIT    ((uint32_t)0x00000008)
/// IRQCCA20SRISEEN field position
#define MDM_IRQCCA20SRISEEN_POS    3
/// IRQPHYERROREN field bit
#define MDM_IRQPHYERROREN_BIT      ((uint32_t)0x00000004)
/// IRQPHYERROREN field position
#define MDM_IRQPHYERROREN_POS      2
/// IRQRADARDETEN field bit
#define MDM_IRQRADARDETEN_BIT      ((uint32_t)0x00000002)
/// IRQRADARDETEN field position
#define MDM_IRQRADARDETEN_POS      1
/// IRQCCATIMEOUTEN field bit
#define MDM_IRQCCATIMEOUTEN_BIT    ((uint32_t)0x00000001)
/// IRQCCATIMEOUTEN field position
#define MDM_IRQCCATIMEOUTEN_POS    0

/// IRQHTSIGVALIDEN field reset value
#define MDM_IRQHTSIGVALIDEN_RST    0x0
/// IRQLSIGVALIDEN field reset value
#define MDM_IRQLSIGVALIDEN_RST     0x0
/// IRQTXOVERFLOWEN field reset value
#define MDM_IRQTXOVERFLOWEN_RST    0x0
/// IRQTXUNDERRUNEN field reset value
#define MDM_IRQTXUNDERRUNEN_RST    0x0
/// IRQCCA40SFALLEN field reset value
#define MDM_IRQCCA40SFALLEN_RST    0x0
/// IRQCCA40SRISEEN field reset value
#define MDM_IRQCCA40SRISEEN_RST    0x0
/// IRQTXENDEN field reset value
#define MDM_IRQTXENDEN_RST         0x0
/// IRQRXENDEN field reset value
#define MDM_IRQRXENDEN_RST         0x0
/// IRQCCA20PFALLEN field reset value
#define MDM_IRQCCA20PFALLEN_RST    0x0
/// IRQCCA20SFALLEN field reset value
#define MDM_IRQCCA20SFALLEN_RST    0x0
/// IRQCCA20PRISEEN field reset value
#define MDM_IRQCCA20PRISEEN_RST    0x0
/// IRQCCA20SRISEEN field reset value
#define MDM_IRQCCA20SRISEEN_RST    0x0
/// IRQPHYERROREN field reset value
#define MDM_IRQPHYERROREN_RST      0x0
/// IRQRADARDETEN field reset value
#define MDM_IRQRADARDETEN_RST      0x0
/// IRQCCATIMEOUTEN field reset value
#define MDM_IRQCCATIMEOUTEN_RST    0x0

/**
 * @brief Constructs a value for the IRQCTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] irqhtsigvaliden - The value to use for the IRQHTSIGVALIDEN field.
 * @param[in] irqlsigvaliden - The value to use for the IRQLSIGVALIDEN field.
 * @param[in] irqtxoverflowen - The value to use for the IRQTXOVERFLOWEN field.
 * @param[in] irqtxunderrunen - The value to use for the IRQTXUNDERRUNEN field.
 * @param[in] irqcca40sfallen - The value to use for the IRQCCA40SFALLEN field.
 * @param[in] irqcca40sriseen - The value to use for the IRQCCA40SRISEEN field.
 * @param[in] irqtxenden - The value to use for the IRQTXENDEN field.
 * @param[in] irqrxenden - The value to use for the IRQRXENDEN field.
 * @param[in] irqcca20pfallen - The value to use for the IRQCCA20PFALLEN field.
 * @param[in] irqcca20sfallen - The value to use for the IRQCCA20SFALLEN field.
 * @param[in] irqcca20priseen - The value to use for the IRQCCA20PRISEEN field.
 * @param[in] irqcca20sriseen - The value to use for the IRQCCA20SRISEEN field.
 * @param[in] irqphyerroren - The value to use for the IRQPHYERROREN field.
 * @param[in] irqradardeten - The value to use for the IRQRADARDETEN field.
 * @param[in] irqccatimeouten - The value to use for the IRQCCATIMEOUTEN field.
 */
__INLINE void mdm_irqctrl_pack(uint8_t irqhtsigvaliden, uint8_t irqlsigvaliden, uint8_t irqtxoverflowen, uint8_t irqtxunderrunen, uint8_t irqcca40sfallen, uint8_t irqcca40sriseen, uint8_t irqtxenden, uint8_t irqrxenden, uint8_t irqcca20pfallen, uint8_t irqcca20sfallen, uint8_t irqcca20priseen, uint8_t irqcca20sriseen, uint8_t irqphyerroren, uint8_t irqradardeten, uint8_t irqccatimeouten)
{
    ASSERT_ERR((((uint32_t)irqhtsigvaliden << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)irqlsigvaliden << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)irqtxoverflowen << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)irqtxunderrunen << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)irqcca40sfallen << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)irqcca40sriseen << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)irqtxenden << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)irqrxenden << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)irqcca20pfallen << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)irqcca20sfallen << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)irqcca20priseen << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)irqcca20sriseen << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)irqphyerroren << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)irqradardeten << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)irqccatimeouten << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_IRQCTRL_ADDR,  ((uint32_t)irqhtsigvaliden << 14) | ((uint32_t)irqlsigvaliden << 13) | ((uint32_t)irqtxoverflowen << 12) | ((uint32_t)irqtxunderrunen << 11) | ((uint32_t)irqcca40sfallen << 10) | ((uint32_t)irqcca40sriseen << 9) | ((uint32_t)irqtxenden << 8) | ((uint32_t)irqrxenden << 7) | ((uint32_t)irqcca20pfallen << 6) | ((uint32_t)irqcca20sfallen << 5) | ((uint32_t)irqcca20priseen << 4) | ((uint32_t)irqcca20sriseen << 3) | ((uint32_t)irqphyerroren << 2) | ((uint32_t)irqradardeten << 1) | ((uint32_t)irqccatimeouten << 0));
}

/**
 * @brief Unpacks IRQCTRL's fields from current value of the IRQCTRL register.
 *
 * Reads the IRQCTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] irqhtsigvaliden - Will be populated with the current value of this field from the register.
 * @param[out] irqlsigvaliden - Will be populated with the current value of this field from the register.
 * @param[out] irqtxoverflowen - Will be populated with the current value of this field from the register.
 * @param[out] irqtxunderrunen - Will be populated with the current value of this field from the register.
 * @param[out] irqcca40sfallen - Will be populated with the current value of this field from the register.
 * @param[out] irqcca40sriseen - Will be populated with the current value of this field from the register.
 * @param[out] irqtxenden - Will be populated with the current value of this field from the register.
 * @param[out] irqrxenden - Will be populated with the current value of this field from the register.
 * @param[out] irqcca20pfallen - Will be populated with the current value of this field from the register.
 * @param[out] irqcca20sfallen - Will be populated with the current value of this field from the register.
 * @param[out] irqcca20priseen - Will be populated with the current value of this field from the register.
 * @param[out] irqcca20sriseen - Will be populated with the current value of this field from the register.
 * @param[out] irqphyerroren - Will be populated with the current value of this field from the register.
 * @param[out] irqradardeten - Will be populated with the current value of this field from the register.
 * @param[out] irqccatimeouten - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_irqctrl_unpack(uint8_t *irqhtsigvaliden, uint8_t *irqlsigvaliden, uint8_t *irqtxoverflowen, uint8_t *irqtxunderrunen, uint8_t *irqcca40sfallen, uint8_t *irqcca40sriseen, uint8_t *irqtxenden, uint8_t *irqrxenden, uint8_t *irqcca20pfallen, uint8_t *irqcca20sfallen, uint8_t *irqcca20priseen, uint8_t *irqcca20sriseen, uint8_t *irqphyerroren, uint8_t *irqradardeten, uint8_t *irqccatimeouten)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQCTRL_ADDR);

    *irqhtsigvaliden = (localVal & ((uint32_t)0x00004000)) >> 14;
    *irqlsigvaliden = (localVal & ((uint32_t)0x00002000)) >> 13;
    *irqtxoverflowen = (localVal & ((uint32_t)0x00001000)) >> 12;
    *irqtxunderrunen = (localVal & ((uint32_t)0x00000800)) >> 11;
    *irqcca40sfallen = (localVal & ((uint32_t)0x00000400)) >> 10;
    *irqcca40sriseen = (localVal & ((uint32_t)0x00000200)) >> 9;
    *irqtxenden = (localVal & ((uint32_t)0x00000100)) >> 8;
    *irqrxenden = (localVal & ((uint32_t)0x00000080)) >> 7;
    *irqcca20pfallen = (localVal & ((uint32_t)0x00000040)) >> 6;
    *irqcca20sfallen = (localVal & ((uint32_t)0x00000020)) >> 5;
    *irqcca20priseen = (localVal & ((uint32_t)0x00000010)) >> 4;
    *irqcca20sriseen = (localVal & ((uint32_t)0x00000008)) >> 3;
    *irqphyerroren = (localVal & ((uint32_t)0x00000004)) >> 2;
    *irqradardeten = (localVal & ((uint32_t)0x00000002)) >> 1;
    *irqccatimeouten = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the IRQHTSIGVALIDEN field in the IRQCTRL register.
 *
 * The IRQCTRL register will be read and the IRQHTSIGVALIDEN field's value will be returned.
 *
 * @return The current value of the IRQHTSIGVALIDEN field in the IRQCTRL register.
 */
__INLINE uint8_t mdm_irqhtsigvaliden_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Sets the IRQHTSIGVALIDEN field of the IRQCTRL register.
 *
 * The IRQCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqhtsigvaliden - The value to set the field to.
 */
__INLINE void mdm_irqhtsigvaliden_setf(uint8_t irqhtsigvaliden)
{
    ASSERT_ERR((((uint32_t)irqhtsigvaliden << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(MDM_IRQCTRL_ADDR, (REG_PL_RD(MDM_IRQCTRL_ADDR) & ~((uint32_t)0x00004000)) | ((uint32_t)irqhtsigvaliden << 14));
}

/**
 * @brief Returns the current value of the IRQLSIGVALIDEN field in the IRQCTRL register.
 *
 * The IRQCTRL register will be read and the IRQLSIGVALIDEN field's value will be returned.
 *
 * @return The current value of the IRQLSIGVALIDEN field in the IRQCTRL register.
 */
__INLINE uint8_t mdm_irqlsigvaliden_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the IRQLSIGVALIDEN field of the IRQCTRL register.
 *
 * The IRQCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqlsigvaliden - The value to set the field to.
 */
__INLINE void mdm_irqlsigvaliden_setf(uint8_t irqlsigvaliden)
{
    ASSERT_ERR((((uint32_t)irqlsigvaliden << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(MDM_IRQCTRL_ADDR, (REG_PL_RD(MDM_IRQCTRL_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)irqlsigvaliden << 13));
}

/**
 * @brief Returns the current value of the IRQTXOVERFLOWEN field in the IRQCTRL register.
 *
 * The IRQCTRL register will be read and the IRQTXOVERFLOWEN field's value will be returned.
 *
 * @return The current value of the IRQTXOVERFLOWEN field in the IRQCTRL register.
 */
__INLINE uint8_t mdm_irqtxoverflowen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the IRQTXOVERFLOWEN field of the IRQCTRL register.
 *
 * The IRQCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqtxoverflowen - The value to set the field to.
 */
__INLINE void mdm_irqtxoverflowen_setf(uint8_t irqtxoverflowen)
{
    ASSERT_ERR((((uint32_t)irqtxoverflowen << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(MDM_IRQCTRL_ADDR, (REG_PL_RD(MDM_IRQCTRL_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)irqtxoverflowen << 12));
}

/**
 * @brief Returns the current value of the IRQTXUNDERRUNEN field in the IRQCTRL register.
 *
 * The IRQCTRL register will be read and the IRQTXUNDERRUNEN field's value will be returned.
 *
 * @return The current value of the IRQTXUNDERRUNEN field in the IRQCTRL register.
 */
__INLINE uint8_t mdm_irqtxunderrunen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the IRQTXUNDERRUNEN field of the IRQCTRL register.
 *
 * The IRQCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqtxunderrunen - The value to set the field to.
 */
__INLINE void mdm_irqtxunderrunen_setf(uint8_t irqtxunderrunen)
{
    ASSERT_ERR((((uint32_t)irqtxunderrunen << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(MDM_IRQCTRL_ADDR, (REG_PL_RD(MDM_IRQCTRL_ADDR) & ~((uint32_t)0x00000800)) | ((uint32_t)irqtxunderrunen << 11));
}

/**
 * @brief Returns the current value of the IRQCCA40SFALLEN field in the IRQCTRL register.
 *
 * The IRQCTRL register will be read and the IRQCCA40SFALLEN field's value will be returned.
 *
 * @return The current value of the IRQCCA40SFALLEN field in the IRQCTRL register.
 */
__INLINE uint8_t mdm_irqcca40sfallen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the IRQCCA40SFALLEN field of the IRQCTRL register.
 *
 * The IRQCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqcca40sfallen - The value to set the field to.
 */
__INLINE void mdm_irqcca40sfallen_setf(uint8_t irqcca40sfallen)
{
    ASSERT_ERR((((uint32_t)irqcca40sfallen << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(MDM_IRQCTRL_ADDR, (REG_PL_RD(MDM_IRQCTRL_ADDR) & ~((uint32_t)0x00000400)) | ((uint32_t)irqcca40sfallen << 10));
}

/**
 * @brief Returns the current value of the IRQCCA40SRISEEN field in the IRQCTRL register.
 *
 * The IRQCTRL register will be read and the IRQCCA40SRISEEN field's value will be returned.
 *
 * @return The current value of the IRQCCA40SRISEEN field in the IRQCTRL register.
 */
__INLINE uint8_t mdm_irqcca40sriseen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the IRQCCA40SRISEEN field of the IRQCTRL register.
 *
 * The IRQCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqcca40sriseen - The value to set the field to.
 */
__INLINE void mdm_irqcca40sriseen_setf(uint8_t irqcca40sriseen)
{
    ASSERT_ERR((((uint32_t)irqcca40sriseen << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(MDM_IRQCTRL_ADDR, (REG_PL_RD(MDM_IRQCTRL_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)irqcca40sriseen << 9));
}

/**
 * @brief Returns the current value of the IRQTXENDEN field in the IRQCTRL register.
 *
 * The IRQCTRL register will be read and the IRQTXENDEN field's value will be returned.
 *
 * @return The current value of the IRQTXENDEN field in the IRQCTRL register.
 */
__INLINE uint8_t mdm_irqtxenden_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the IRQTXENDEN field of the IRQCTRL register.
 *
 * The IRQCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqtxenden - The value to set the field to.
 */
__INLINE void mdm_irqtxenden_setf(uint8_t irqtxenden)
{
    ASSERT_ERR((((uint32_t)irqtxenden << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(MDM_IRQCTRL_ADDR, (REG_PL_RD(MDM_IRQCTRL_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)irqtxenden << 8));
}

/**
 * @brief Returns the current value of the IRQRXENDEN field in the IRQCTRL register.
 *
 * The IRQCTRL register will be read and the IRQRXENDEN field's value will be returned.
 *
 * @return The current value of the IRQRXENDEN field in the IRQCTRL register.
 */
__INLINE uint8_t mdm_irqrxenden_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the IRQRXENDEN field of the IRQCTRL register.
 *
 * The IRQCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqrxenden - The value to set the field to.
 */
__INLINE void mdm_irqrxenden_setf(uint8_t irqrxenden)
{
    ASSERT_ERR((((uint32_t)irqrxenden << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(MDM_IRQCTRL_ADDR, (REG_PL_RD(MDM_IRQCTRL_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)irqrxenden << 7));
}

/**
 * @brief Returns the current value of the IRQCCA20PFALLEN field in the IRQCTRL register.
 *
 * The IRQCTRL register will be read and the IRQCCA20PFALLEN field's value will be returned.
 *
 * @return The current value of the IRQCCA20PFALLEN field in the IRQCTRL register.
 */
__INLINE uint8_t mdm_irqcca20pfallen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the IRQCCA20PFALLEN field of the IRQCTRL register.
 *
 * The IRQCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqcca20pfallen - The value to set the field to.
 */
__INLINE void mdm_irqcca20pfallen_setf(uint8_t irqcca20pfallen)
{
    ASSERT_ERR((((uint32_t)irqcca20pfallen << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(MDM_IRQCTRL_ADDR, (REG_PL_RD(MDM_IRQCTRL_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)irqcca20pfallen << 6));
}

/**
 * @brief Returns the current value of the IRQCCA20SFALLEN field in the IRQCTRL register.
 *
 * The IRQCTRL register will be read and the IRQCCA20SFALLEN field's value will be returned.
 *
 * @return The current value of the IRQCCA20SFALLEN field in the IRQCTRL register.
 */
__INLINE uint8_t mdm_irqcca20sfallen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the IRQCCA20SFALLEN field of the IRQCTRL register.
 *
 * The IRQCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqcca20sfallen - The value to set the field to.
 */
__INLINE void mdm_irqcca20sfallen_setf(uint8_t irqcca20sfallen)
{
    ASSERT_ERR((((uint32_t)irqcca20sfallen << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(MDM_IRQCTRL_ADDR, (REG_PL_RD(MDM_IRQCTRL_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)irqcca20sfallen << 5));
}

/**
 * @brief Returns the current value of the IRQCCA20PRISEEN field in the IRQCTRL register.
 *
 * The IRQCTRL register will be read and the IRQCCA20PRISEEN field's value will be returned.
 *
 * @return The current value of the IRQCCA20PRISEEN field in the IRQCTRL register.
 */
__INLINE uint8_t mdm_irqcca20priseen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the IRQCCA20PRISEEN field of the IRQCTRL register.
 *
 * The IRQCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqcca20priseen - The value to set the field to.
 */
__INLINE void mdm_irqcca20priseen_setf(uint8_t irqcca20priseen)
{
    ASSERT_ERR((((uint32_t)irqcca20priseen << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(MDM_IRQCTRL_ADDR, (REG_PL_RD(MDM_IRQCTRL_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)irqcca20priseen << 4));
}

/**
 * @brief Returns the current value of the IRQCCA20SRISEEN field in the IRQCTRL register.
 *
 * The IRQCTRL register will be read and the IRQCCA20SRISEEN field's value will be returned.
 *
 * @return The current value of the IRQCCA20SRISEEN field in the IRQCTRL register.
 */
__INLINE uint8_t mdm_irqcca20sriseen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the IRQCCA20SRISEEN field of the IRQCTRL register.
 *
 * The IRQCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqcca20sriseen - The value to set the field to.
 */
__INLINE void mdm_irqcca20sriseen_setf(uint8_t irqcca20sriseen)
{
    ASSERT_ERR((((uint32_t)irqcca20sriseen << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MDM_IRQCTRL_ADDR, (REG_PL_RD(MDM_IRQCTRL_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)irqcca20sriseen << 3));
}

/**
 * @brief Returns the current value of the IRQPHYERROREN field in the IRQCTRL register.
 *
 * The IRQCTRL register will be read and the IRQPHYERROREN field's value will be returned.
 *
 * @return The current value of the IRQPHYERROREN field in the IRQCTRL register.
 */
__INLINE uint8_t mdm_irqphyerroren_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the IRQPHYERROREN field of the IRQCTRL register.
 *
 * The IRQCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqphyerroren - The value to set the field to.
 */
__INLINE void mdm_irqphyerroren_setf(uint8_t irqphyerroren)
{
    ASSERT_ERR((((uint32_t)irqphyerroren << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(MDM_IRQCTRL_ADDR, (REG_PL_RD(MDM_IRQCTRL_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)irqphyerroren << 2));
}

/**
 * @brief Returns the current value of the IRQRADARDETEN field in the IRQCTRL register.
 *
 * The IRQCTRL register will be read and the IRQRADARDETEN field's value will be returned.
 *
 * @return The current value of the IRQRADARDETEN field in the IRQCTRL register.
 */
__INLINE uint8_t mdm_irqradardeten_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the IRQRADARDETEN field of the IRQCTRL register.
 *
 * The IRQCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqradardeten - The value to set the field to.
 */
__INLINE void mdm_irqradardeten_setf(uint8_t irqradardeten)
{
    ASSERT_ERR((((uint32_t)irqradardeten << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(MDM_IRQCTRL_ADDR, (REG_PL_RD(MDM_IRQCTRL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)irqradardeten << 1));
}

/**
 * @brief Returns the current value of the IRQCCATIMEOUTEN field in the IRQCTRL register.
 *
 * The IRQCTRL register will be read and the IRQCCATIMEOUTEN field's value will be returned.
 *
 * @return The current value of the IRQCCATIMEOUTEN field in the IRQCTRL register.
 */
__INLINE uint8_t mdm_irqccatimeouten_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the IRQCCATIMEOUTEN field of the IRQCTRL register.
 *
 * The IRQCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqccatimeouten - The value to set the field to.
 */
__INLINE void mdm_irqccatimeouten_setf(uint8_t irqccatimeouten)
{
    ASSERT_ERR((((uint32_t)irqccatimeouten << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_IRQCTRL_ADDR, (REG_PL_RD(MDM_IRQCTRL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)irqccatimeouten << 0));
}

/// @}

/**
 * @name IRQACK register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     14     IRQHTSIGVALIDACK   0
 *     13      IRQLSIGVALIDACK   0
 *     12     IRQTXOVERFLOWACK   0
 *     11     IRQTXUNDERRUNACK   0
 *     10     IRQCCA40SFALLACK   0
 *     09     IRQCCA40SRISEACK   0
 *     08          IRQTXENDACK   0
 *     07          IRQRXENDACK   0
 *     06     IRQCCA20PFALLACK   0
 *     05     IRQCCA20SFALLACK   0
 *     04     IRQCCA20PRISEACK   0
 *     03     IRQCCA20SRISEACK   0
 *     02       IRQPHYERRORACK   0
 *     01       IRQRADARDETACK   0
 *     00     IRQCCATIMEOUTACK   0
 * </pre>
 *
 * @{
 */

/// Address of the IRQACK register
#define MDM_IRQACK_ADDR   0x01000850
/// Offset of the IRQACK register from the base address
#define MDM_IRQACK_OFFSET 0x00000850
/// Index of the IRQACK register
#define MDM_IRQACK_INDEX  0x00000214
/// Reset value of the IRQACK register
#define MDM_IRQACK_RESET  0x00000000

/**
 * @brief Returns the current value of the IRQACK register.
 * The IRQACK register will be read and its value returned.
 * @return The current value of the IRQACK register.
 */
__INLINE uint32_t mdm_irqack_get(void)
{
    return REG_PL_RD(MDM_IRQACK_ADDR);
}

/**
 * @brief Sets the IRQACK register to a value.
 * The IRQACK register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_irqack_set(uint32_t value)
{
    REG_PL_WR(MDM_IRQACK_ADDR, value);
}

// field definitions
/// IRQHTSIGVALIDACK field bit
#define MDM_IRQHTSIGVALIDACK_BIT    ((uint32_t)0x00004000)
/// IRQHTSIGVALIDACK field position
#define MDM_IRQHTSIGVALIDACK_POS    14
/// IRQLSIGVALIDACK field bit
#define MDM_IRQLSIGVALIDACK_BIT     ((uint32_t)0x00002000)
/// IRQLSIGVALIDACK field position
#define MDM_IRQLSIGVALIDACK_POS     13
/// IRQTXOVERFLOWACK field bit
#define MDM_IRQTXOVERFLOWACK_BIT    ((uint32_t)0x00001000)
/// IRQTXOVERFLOWACK field position
#define MDM_IRQTXOVERFLOWACK_POS    12
/// IRQTXUNDERRUNACK field bit
#define MDM_IRQTXUNDERRUNACK_BIT    ((uint32_t)0x00000800)
/// IRQTXUNDERRUNACK field position
#define MDM_IRQTXUNDERRUNACK_POS    11
/// IRQCCA40SFALLACK field bit
#define MDM_IRQCCA40SFALLACK_BIT    ((uint32_t)0x00000400)
/// IRQCCA40SFALLACK field position
#define MDM_IRQCCA40SFALLACK_POS    10
/// IRQCCA40SRISEACK field bit
#define MDM_IRQCCA40SRISEACK_BIT    ((uint32_t)0x00000200)
/// IRQCCA40SRISEACK field position
#define MDM_IRQCCA40SRISEACK_POS    9
/// IRQTXENDACK field bit
#define MDM_IRQTXENDACK_BIT         ((uint32_t)0x00000100)
/// IRQTXENDACK field position
#define MDM_IRQTXENDACK_POS         8
/// IRQRXENDACK field bit
#define MDM_IRQRXENDACK_BIT         ((uint32_t)0x00000080)
/// IRQRXENDACK field position
#define MDM_IRQRXENDACK_POS         7
/// IRQCCA20PFALLACK field bit
#define MDM_IRQCCA20PFALLACK_BIT    ((uint32_t)0x00000040)
/// IRQCCA20PFALLACK field position
#define MDM_IRQCCA20PFALLACK_POS    6
/// IRQCCA20SFALLACK field bit
#define MDM_IRQCCA20SFALLACK_BIT    ((uint32_t)0x00000020)
/// IRQCCA20SFALLACK field position
#define MDM_IRQCCA20SFALLACK_POS    5
/// IRQCCA20PRISEACK field bit
#define MDM_IRQCCA20PRISEACK_BIT    ((uint32_t)0x00000010)
/// IRQCCA20PRISEACK field position
#define MDM_IRQCCA20PRISEACK_POS    4
/// IRQCCA20SRISEACK field bit
#define MDM_IRQCCA20SRISEACK_BIT    ((uint32_t)0x00000008)
/// IRQCCA20SRISEACK field position
#define MDM_IRQCCA20SRISEACK_POS    3
/// IRQPHYERRORACK field bit
#define MDM_IRQPHYERRORACK_BIT      ((uint32_t)0x00000004)
/// IRQPHYERRORACK field position
#define MDM_IRQPHYERRORACK_POS      2
/// IRQRADARDETACK field bit
#define MDM_IRQRADARDETACK_BIT      ((uint32_t)0x00000002)
/// IRQRADARDETACK field position
#define MDM_IRQRADARDETACK_POS      1
/// IRQCCATIMEOUTACK field bit
#define MDM_IRQCCATIMEOUTACK_BIT    ((uint32_t)0x00000001)
/// IRQCCATIMEOUTACK field position
#define MDM_IRQCCATIMEOUTACK_POS    0

/// IRQHTSIGVALIDACK field reset value
#define MDM_IRQHTSIGVALIDACK_RST    0x0
/// IRQLSIGVALIDACK field reset value
#define MDM_IRQLSIGVALIDACK_RST     0x0
/// IRQTXOVERFLOWACK field reset value
#define MDM_IRQTXOVERFLOWACK_RST    0x0
/// IRQTXUNDERRUNACK field reset value
#define MDM_IRQTXUNDERRUNACK_RST    0x0
/// IRQCCA40SFALLACK field reset value
#define MDM_IRQCCA40SFALLACK_RST    0x0
/// IRQCCA40SRISEACK field reset value
#define MDM_IRQCCA40SRISEACK_RST    0x0
/// IRQTXENDACK field reset value
#define MDM_IRQTXENDACK_RST         0x0
/// IRQRXENDACK field reset value
#define MDM_IRQRXENDACK_RST         0x0
/// IRQCCA20PFALLACK field reset value
#define MDM_IRQCCA20PFALLACK_RST    0x0
/// IRQCCA20SFALLACK field reset value
#define MDM_IRQCCA20SFALLACK_RST    0x0
/// IRQCCA20PRISEACK field reset value
#define MDM_IRQCCA20PRISEACK_RST    0x0
/// IRQCCA20SRISEACK field reset value
#define MDM_IRQCCA20SRISEACK_RST    0x0
/// IRQPHYERRORACK field reset value
#define MDM_IRQPHYERRORACK_RST      0x0
/// IRQRADARDETACK field reset value
#define MDM_IRQRADARDETACK_RST      0x0
/// IRQCCATIMEOUTACK field reset value
#define MDM_IRQCCATIMEOUTACK_RST    0x0

/**
 * @brief Constructs a value for the IRQACK register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] irqhtsigvalidack - The value to use for the IRQHTSIGVALIDACK field.
 * @param[in] irqlsigvalidack - The value to use for the IRQLSIGVALIDACK field.
 * @param[in] irqtxoverflowack - The value to use for the IRQTXOVERFLOWACK field.
 * @param[in] irqtxunderrunack - The value to use for the IRQTXUNDERRUNACK field.
 * @param[in] irqcca40sfallack - The value to use for the IRQCCA40SFALLACK field.
 * @param[in] irqcca40sriseack - The value to use for the IRQCCA40SRISEACK field.
 * @param[in] irqtxendack - The value to use for the IRQTXENDACK field.
 * @param[in] irqrxendack - The value to use for the IRQRXENDACK field.
 * @param[in] irqcca20pfallack - The value to use for the IRQCCA20PFALLACK field.
 * @param[in] irqcca20sfallack - The value to use for the IRQCCA20SFALLACK field.
 * @param[in] irqcca20priseack - The value to use for the IRQCCA20PRISEACK field.
 * @param[in] irqcca20sriseack - The value to use for the IRQCCA20SRISEACK field.
 * @param[in] irqphyerrorack - The value to use for the IRQPHYERRORACK field.
 * @param[in] irqradardetack - The value to use for the IRQRADARDETACK field.
 * @param[in] irqccatimeoutack - The value to use for the IRQCCATIMEOUTACK field.
 */
__INLINE void mdm_irqack_pack(uint8_t irqhtsigvalidack, uint8_t irqlsigvalidack, uint8_t irqtxoverflowack, uint8_t irqtxunderrunack, uint8_t irqcca40sfallack, uint8_t irqcca40sriseack, uint8_t irqtxendack, uint8_t irqrxendack, uint8_t irqcca20pfallack, uint8_t irqcca20sfallack, uint8_t irqcca20priseack, uint8_t irqcca20sriseack, uint8_t irqphyerrorack, uint8_t irqradardetack, uint8_t irqccatimeoutack)
{
    ASSERT_ERR((((uint32_t)irqhtsigvalidack << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)irqlsigvalidack << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)irqtxoverflowack << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)irqtxunderrunack << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)irqcca40sfallack << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)irqcca40sriseack << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)irqtxendack << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)irqrxendack << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)irqcca20pfallack << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)irqcca20sfallack << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)irqcca20priseack << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)irqcca20sriseack << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)irqphyerrorack << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)irqradardetack << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)irqccatimeoutack << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_IRQACK_ADDR,  ((uint32_t)irqhtsigvalidack << 14) | ((uint32_t)irqlsigvalidack << 13) | ((uint32_t)irqtxoverflowack << 12) | ((uint32_t)irqtxunderrunack << 11) | ((uint32_t)irqcca40sfallack << 10) | ((uint32_t)irqcca40sriseack << 9) | ((uint32_t)irqtxendack << 8) | ((uint32_t)irqrxendack << 7) | ((uint32_t)irqcca20pfallack << 6) | ((uint32_t)irqcca20sfallack << 5) | ((uint32_t)irqcca20priseack << 4) | ((uint32_t)irqcca20sriseack << 3) | ((uint32_t)irqphyerrorack << 2) | ((uint32_t)irqradardetack << 1) | ((uint32_t)irqccatimeoutack << 0));
}

/**
 * @brief Unpacks IRQACK's fields from current value of the IRQACK register.
 *
 * Reads the IRQACK register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] irqhtsigvalidack - Will be populated with the current value of this field from the register.
 * @param[out] irqlsigvalidack - Will be populated with the current value of this field from the register.
 * @param[out] irqtxoverflowack - Will be populated with the current value of this field from the register.
 * @param[out] irqtxunderrunack - Will be populated with the current value of this field from the register.
 * @param[out] irqcca40sfallack - Will be populated with the current value of this field from the register.
 * @param[out] irqcca40sriseack - Will be populated with the current value of this field from the register.
 * @param[out] irqtxendack - Will be populated with the current value of this field from the register.
 * @param[out] irqrxendack - Will be populated with the current value of this field from the register.
 * @param[out] irqcca20pfallack - Will be populated with the current value of this field from the register.
 * @param[out] irqcca20sfallack - Will be populated with the current value of this field from the register.
 * @param[out] irqcca20priseack - Will be populated with the current value of this field from the register.
 * @param[out] irqcca20sriseack - Will be populated with the current value of this field from the register.
 * @param[out] irqphyerrorack - Will be populated with the current value of this field from the register.
 * @param[out] irqradardetack - Will be populated with the current value of this field from the register.
 * @param[out] irqccatimeoutack - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_irqack_unpack(uint8_t *irqhtsigvalidack, uint8_t *irqlsigvalidack, uint8_t *irqtxoverflowack, uint8_t *irqtxunderrunack, uint8_t *irqcca40sfallack, uint8_t *irqcca40sriseack, uint8_t *irqtxendack, uint8_t *irqrxendack, uint8_t *irqcca20pfallack, uint8_t *irqcca20sfallack, uint8_t *irqcca20priseack, uint8_t *irqcca20sriseack, uint8_t *irqphyerrorack, uint8_t *irqradardetack, uint8_t *irqccatimeoutack)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQACK_ADDR);

    *irqhtsigvalidack = (localVal & ((uint32_t)0x00004000)) >> 14;
    *irqlsigvalidack = (localVal & ((uint32_t)0x00002000)) >> 13;
    *irqtxoverflowack = (localVal & ((uint32_t)0x00001000)) >> 12;
    *irqtxunderrunack = (localVal & ((uint32_t)0x00000800)) >> 11;
    *irqcca40sfallack = (localVal & ((uint32_t)0x00000400)) >> 10;
    *irqcca40sriseack = (localVal & ((uint32_t)0x00000200)) >> 9;
    *irqtxendack = (localVal & ((uint32_t)0x00000100)) >> 8;
    *irqrxendack = (localVal & ((uint32_t)0x00000080)) >> 7;
    *irqcca20pfallack = (localVal & ((uint32_t)0x00000040)) >> 6;
    *irqcca20sfallack = (localVal & ((uint32_t)0x00000020)) >> 5;
    *irqcca20priseack = (localVal & ((uint32_t)0x00000010)) >> 4;
    *irqcca20sriseack = (localVal & ((uint32_t)0x00000008)) >> 3;
    *irqphyerrorack = (localVal & ((uint32_t)0x00000004)) >> 2;
    *irqradardetack = (localVal & ((uint32_t)0x00000002)) >> 1;
    *irqccatimeoutack = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the IRQHTSIGVALIDACK field in the IRQACK register.
 *
 * The IRQACK register will be read and the IRQHTSIGVALIDACK field's value will be returned.
 *
 * @return The current value of the IRQHTSIGVALIDACK field in the IRQACK register.
 */
__INLINE uint8_t mdm_irqhtsigvalidack_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQACK_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Sets the IRQHTSIGVALIDACK field of the IRQACK register.
 *
 * The IRQACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqhtsigvalidack - The value to set the field to.
 */
__INLINE void mdm_irqhtsigvalidack_setf(uint8_t irqhtsigvalidack)
{
    ASSERT_ERR((((uint32_t)irqhtsigvalidack << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(MDM_IRQACK_ADDR, (uint32_t)irqhtsigvalidack << 14);
}

/**
 * @brief Returns the current value of the IRQLSIGVALIDACK field in the IRQACK register.
 *
 * The IRQACK register will be read and the IRQLSIGVALIDACK field's value will be returned.
 *
 * @return The current value of the IRQLSIGVALIDACK field in the IRQACK register.
 */
__INLINE uint8_t mdm_irqlsigvalidack_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQACK_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the IRQLSIGVALIDACK field of the IRQACK register.
 *
 * The IRQACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqlsigvalidack - The value to set the field to.
 */
__INLINE void mdm_irqlsigvalidack_setf(uint8_t irqlsigvalidack)
{
    ASSERT_ERR((((uint32_t)irqlsigvalidack << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(MDM_IRQACK_ADDR, (uint32_t)irqlsigvalidack << 13);
}

/**
 * @brief Returns the current value of the IRQTXOVERFLOWACK field in the IRQACK register.
 *
 * The IRQACK register will be read and the IRQTXOVERFLOWACK field's value will be returned.
 *
 * @return The current value of the IRQTXOVERFLOWACK field in the IRQACK register.
 */
__INLINE uint8_t mdm_irqtxoverflowack_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQACK_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the IRQTXOVERFLOWACK field of the IRQACK register.
 *
 * The IRQACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqtxoverflowack - The value to set the field to.
 */
__INLINE void mdm_irqtxoverflowack_setf(uint8_t irqtxoverflowack)
{
    ASSERT_ERR((((uint32_t)irqtxoverflowack << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(MDM_IRQACK_ADDR, (uint32_t)irqtxoverflowack << 12);
}

/**
 * @brief Returns the current value of the IRQTXUNDERRUNACK field in the IRQACK register.
 *
 * The IRQACK register will be read and the IRQTXUNDERRUNACK field's value will be returned.
 *
 * @return The current value of the IRQTXUNDERRUNACK field in the IRQACK register.
 */
__INLINE uint8_t mdm_irqtxunderrunack_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQACK_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the IRQTXUNDERRUNACK field of the IRQACK register.
 *
 * The IRQACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqtxunderrunack - The value to set the field to.
 */
__INLINE void mdm_irqtxunderrunack_setf(uint8_t irqtxunderrunack)
{
    ASSERT_ERR((((uint32_t)irqtxunderrunack << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(MDM_IRQACK_ADDR, (uint32_t)irqtxunderrunack << 11);
}

/**
 * @brief Returns the current value of the IRQCCA40SFALLACK field in the IRQACK register.
 *
 * The IRQACK register will be read and the IRQCCA40SFALLACK field's value will be returned.
 *
 * @return The current value of the IRQCCA40SFALLACK field in the IRQACK register.
 */
__INLINE uint8_t mdm_irqcca40sfallack_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQACK_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the IRQCCA40SFALLACK field of the IRQACK register.
 *
 * The IRQACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqcca40sfallack - The value to set the field to.
 */
__INLINE void mdm_irqcca40sfallack_setf(uint8_t irqcca40sfallack)
{
    ASSERT_ERR((((uint32_t)irqcca40sfallack << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(MDM_IRQACK_ADDR, (uint32_t)irqcca40sfallack << 10);
}

/**
 * @brief Returns the current value of the IRQCCA40SRISEACK field in the IRQACK register.
 *
 * The IRQACK register will be read and the IRQCCA40SRISEACK field's value will be returned.
 *
 * @return The current value of the IRQCCA40SRISEACK field in the IRQACK register.
 */
__INLINE uint8_t mdm_irqcca40sriseack_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQACK_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the IRQCCA40SRISEACK field of the IRQACK register.
 *
 * The IRQACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqcca40sriseack - The value to set the field to.
 */
__INLINE void mdm_irqcca40sriseack_setf(uint8_t irqcca40sriseack)
{
    ASSERT_ERR((((uint32_t)irqcca40sriseack << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(MDM_IRQACK_ADDR, (uint32_t)irqcca40sriseack << 9);
}

/**
 * @brief Returns the current value of the IRQTXENDACK field in the IRQACK register.
 *
 * The IRQACK register will be read and the IRQTXENDACK field's value will be returned.
 *
 * @return The current value of the IRQTXENDACK field in the IRQACK register.
 */
__INLINE uint8_t mdm_irqtxendack_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQACK_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the IRQTXENDACK field of the IRQACK register.
 *
 * The IRQACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqtxendack - The value to set the field to.
 */
__INLINE void mdm_irqtxendack_setf(uint8_t irqtxendack)
{
    ASSERT_ERR((((uint32_t)irqtxendack << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(MDM_IRQACK_ADDR, (uint32_t)irqtxendack << 8);
}

/**
 * @brief Returns the current value of the IRQRXENDACK field in the IRQACK register.
 *
 * The IRQACK register will be read and the IRQRXENDACK field's value will be returned.
 *
 * @return The current value of the IRQRXENDACK field in the IRQACK register.
 */
__INLINE uint8_t mdm_irqrxendack_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQACK_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the IRQRXENDACK field of the IRQACK register.
 *
 * The IRQACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqrxendack - The value to set the field to.
 */
__INLINE void mdm_irqrxendack_setf(uint8_t irqrxendack)
{
    ASSERT_ERR((((uint32_t)irqrxendack << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(MDM_IRQACK_ADDR, (uint32_t)irqrxendack << 7);
}

/**
 * @brief Returns the current value of the IRQCCA20PFALLACK field in the IRQACK register.
 *
 * The IRQACK register will be read and the IRQCCA20PFALLACK field's value will be returned.
 *
 * @return The current value of the IRQCCA20PFALLACK field in the IRQACK register.
 */
__INLINE uint8_t mdm_irqcca20pfallack_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQACK_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the IRQCCA20PFALLACK field of the IRQACK register.
 *
 * The IRQACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqcca20pfallack - The value to set the field to.
 */
__INLINE void mdm_irqcca20pfallack_setf(uint8_t irqcca20pfallack)
{
    ASSERT_ERR((((uint32_t)irqcca20pfallack << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(MDM_IRQACK_ADDR, (uint32_t)irqcca20pfallack << 6);
}

/**
 * @brief Returns the current value of the IRQCCA20SFALLACK field in the IRQACK register.
 *
 * The IRQACK register will be read and the IRQCCA20SFALLACK field's value will be returned.
 *
 * @return The current value of the IRQCCA20SFALLACK field in the IRQACK register.
 */
__INLINE uint8_t mdm_irqcca20sfallack_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQACK_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the IRQCCA20SFALLACK field of the IRQACK register.
 *
 * The IRQACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqcca20sfallack - The value to set the field to.
 */
__INLINE void mdm_irqcca20sfallack_setf(uint8_t irqcca20sfallack)
{
    ASSERT_ERR((((uint32_t)irqcca20sfallack << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(MDM_IRQACK_ADDR, (uint32_t)irqcca20sfallack << 5);
}

/**
 * @brief Returns the current value of the IRQCCA20PRISEACK field in the IRQACK register.
 *
 * The IRQACK register will be read and the IRQCCA20PRISEACK field's value will be returned.
 *
 * @return The current value of the IRQCCA20PRISEACK field in the IRQACK register.
 */
__INLINE uint8_t mdm_irqcca20priseack_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQACK_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the IRQCCA20PRISEACK field of the IRQACK register.
 *
 * The IRQACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqcca20priseack - The value to set the field to.
 */
__INLINE void mdm_irqcca20priseack_setf(uint8_t irqcca20priseack)
{
    ASSERT_ERR((((uint32_t)irqcca20priseack << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(MDM_IRQACK_ADDR, (uint32_t)irqcca20priseack << 4);
}

/**
 * @brief Returns the current value of the IRQCCA20SRISEACK field in the IRQACK register.
 *
 * The IRQACK register will be read and the IRQCCA20SRISEACK field's value will be returned.
 *
 * @return The current value of the IRQCCA20SRISEACK field in the IRQACK register.
 */
__INLINE uint8_t mdm_irqcca20sriseack_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQACK_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the IRQCCA20SRISEACK field of the IRQACK register.
 *
 * The IRQACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqcca20sriseack - The value to set the field to.
 */
__INLINE void mdm_irqcca20sriseack_setf(uint8_t irqcca20sriseack)
{
    ASSERT_ERR((((uint32_t)irqcca20sriseack << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MDM_IRQACK_ADDR, (uint32_t)irqcca20sriseack << 3);
}

/**
 * @brief Returns the current value of the IRQPHYERRORACK field in the IRQACK register.
 *
 * The IRQACK register will be read and the IRQPHYERRORACK field's value will be returned.
 *
 * @return The current value of the IRQPHYERRORACK field in the IRQACK register.
 */
__INLINE uint8_t mdm_irqphyerrorack_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQACK_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the IRQPHYERRORACK field of the IRQACK register.
 *
 * The IRQACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqphyerrorack - The value to set the field to.
 */
__INLINE void mdm_irqphyerrorack_setf(uint8_t irqphyerrorack)
{
    ASSERT_ERR((((uint32_t)irqphyerrorack << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(MDM_IRQACK_ADDR, (uint32_t)irqphyerrorack << 2);
}

/**
 * @brief Returns the current value of the IRQRADARDETACK field in the IRQACK register.
 *
 * The IRQACK register will be read and the IRQRADARDETACK field's value will be returned.
 *
 * @return The current value of the IRQRADARDETACK field in the IRQACK register.
 */
__INLINE uint8_t mdm_irqradardetack_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQACK_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the IRQRADARDETACK field of the IRQACK register.
 *
 * The IRQACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqradardetack - The value to set the field to.
 */
__INLINE void mdm_irqradardetack_setf(uint8_t irqradardetack)
{
    ASSERT_ERR((((uint32_t)irqradardetack << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(MDM_IRQACK_ADDR, (uint32_t)irqradardetack << 1);
}

/**
 * @brief Returns the current value of the IRQCCATIMEOUTACK field in the IRQACK register.
 *
 * The IRQACK register will be read and the IRQCCATIMEOUTACK field's value will be returned.
 *
 * @return The current value of the IRQCCATIMEOUTACK field in the IRQACK register.
 */
__INLINE uint8_t mdm_irqccatimeoutack_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQACK_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the IRQCCATIMEOUTACK field of the IRQACK register.
 *
 * The IRQACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] irqccatimeoutack - The value to set the field to.
 */
__INLINE void mdm_irqccatimeoutack_setf(uint8_t irqccatimeoutack)
{
    ASSERT_ERR((((uint32_t)irqccatimeoutack << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_IRQACK_ADDR, (uint32_t)irqccatimeoutack << 0);
}

/// @}

/**
 * @name TDCYCLSHTRL0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  11:08             LTDCYCL4   0x3
 *  07:04             LTDCYCL3   0x2
 *  03:00             LTDCYCL2   0x1
 * </pre>
 *
 * @{
 */

/// Address of the TDCYCLSHTRL0 register
#define MDM_TDCYCLSHTRL0_ADDR   0x01000854
/// Offset of the TDCYCLSHTRL0 register from the base address
#define MDM_TDCYCLSHTRL0_OFFSET 0x00000854
/// Index of the TDCYCLSHTRL0 register
#define MDM_TDCYCLSHTRL0_INDEX  0x00000215
/// Reset value of the TDCYCLSHTRL0 register
#define MDM_TDCYCLSHTRL0_RESET  0x00000321

/**
 * @brief Returns the current value of the TDCYCLSHTRL0 register.
 * The TDCYCLSHTRL0 register will be read and its value returned.
 * @return The current value of the TDCYCLSHTRL0 register.
 */
__INLINE uint32_t mdm_tdcyclshtrl0_get(void)
{
    return REG_PL_RD(MDM_TDCYCLSHTRL0_ADDR);
}

/**
 * @brief Sets the TDCYCLSHTRL0 register to a value.
 * The TDCYCLSHTRL0 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_tdcyclshtrl0_set(uint32_t value)
{
    REG_PL_WR(MDM_TDCYCLSHTRL0_ADDR, value);
}

// field definitions
/// LTDCYCL4 field mask
#define MDM_LTDCYCL4_MASK   ((uint32_t)0x00000F00)
/// LTDCYCL4 field LSB position
#define MDM_LTDCYCL4_LSB    8
/// LTDCYCL4 field width
#define MDM_LTDCYCL4_WIDTH  ((uint32_t)0x00000004)
/// LTDCYCL3 field mask
#define MDM_LTDCYCL3_MASK   ((uint32_t)0x000000F0)
/// LTDCYCL3 field LSB position
#define MDM_LTDCYCL3_LSB    4
/// LTDCYCL3 field width
#define MDM_LTDCYCL3_WIDTH  ((uint32_t)0x00000004)
/// LTDCYCL2 field mask
#define MDM_LTDCYCL2_MASK   ((uint32_t)0x0000000F)
/// LTDCYCL2 field LSB position
#define MDM_LTDCYCL2_LSB    0
/// LTDCYCL2 field width
#define MDM_LTDCYCL2_WIDTH  ((uint32_t)0x00000004)

/// LTDCYCL4 field reset value
#define MDM_LTDCYCL4_RST    0x3
/// LTDCYCL3 field reset value
#define MDM_LTDCYCL3_RST    0x2
/// LTDCYCL2 field reset value
#define MDM_LTDCYCL2_RST    0x1

/**
 * @brief Constructs a value for the TDCYCLSHTRL0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] ltdcycl4 - The value to use for the LTDCYCL4 field.
 * @param[in] ltdcycl3 - The value to use for the LTDCYCL3 field.
 * @param[in] ltdcycl2 - The value to use for the LTDCYCL2 field.
 */
__INLINE void mdm_tdcyclshtrl0_pack(uint8_t ltdcycl4, uint8_t ltdcycl3, uint8_t ltdcycl2)
{
    ASSERT_ERR((((uint32_t)ltdcycl4 << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)ltdcycl3 << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)ltdcycl2 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_TDCYCLSHTRL0_ADDR,  ((uint32_t)ltdcycl4 << 8) | ((uint32_t)ltdcycl3 << 4) | ((uint32_t)ltdcycl2 << 0));
}

/**
 * @brief Unpacks TDCYCLSHTRL0's fields from current value of the TDCYCLSHTRL0 register.
 *
 * Reads the TDCYCLSHTRL0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] ltdcycl4 - Will be populated with the current value of this field from the register.
 * @param[out] ltdcycl3 - Will be populated with the current value of this field from the register.
 * @param[out] ltdcycl2 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_tdcyclshtrl0_unpack(uint8_t *ltdcycl4, uint8_t *ltdcycl3, uint8_t *ltdcycl2)
{
    uint32_t localVal = REG_PL_RD(MDM_TDCYCLSHTRL0_ADDR);

    *ltdcycl4 = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *ltdcycl3 = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *ltdcycl2 = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the LTDCYCL4 field in the TDCYCLSHTRL0 register.
 *
 * The TDCYCLSHTRL0 register will be read and the LTDCYCL4 field's value will be returned.
 *
 * @return The current value of the LTDCYCL4 field in the TDCYCLSHTRL0 register.
 */
__INLINE uint8_t mdm_ltdcycl4_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDCYCLSHTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the LTDCYCL4 field of the TDCYCLSHTRL0 register.
 *
 * The TDCYCLSHTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ltdcycl4 - The value to set the field to.
 */
__INLINE void mdm_ltdcycl4_setf(uint8_t ltdcycl4)
{
    ASSERT_ERR((((uint32_t)ltdcycl4 << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(MDM_TDCYCLSHTRL0_ADDR, (REG_PL_RD(MDM_TDCYCLSHTRL0_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)ltdcycl4 << 8));
}

/**
 * @brief Returns the current value of the LTDCYCL3 field in the TDCYCLSHTRL0 register.
 *
 * The TDCYCLSHTRL0 register will be read and the LTDCYCL3 field's value will be returned.
 *
 * @return The current value of the LTDCYCL3 field in the TDCYCLSHTRL0 register.
 */
__INLINE uint8_t mdm_ltdcycl3_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDCYCLSHTRL0_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the LTDCYCL3 field of the TDCYCLSHTRL0 register.
 *
 * The TDCYCLSHTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ltdcycl3 - The value to set the field to.
 */
__INLINE void mdm_ltdcycl3_setf(uint8_t ltdcycl3)
{
    ASSERT_ERR((((uint32_t)ltdcycl3 << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(MDM_TDCYCLSHTRL0_ADDR, (REG_PL_RD(MDM_TDCYCLSHTRL0_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)ltdcycl3 << 4));
}

/**
 * @brief Returns the current value of the LTDCYCL2 field in the TDCYCLSHTRL0 register.
 *
 * The TDCYCLSHTRL0 register will be read and the LTDCYCL2 field's value will be returned.
 *
 * @return The current value of the LTDCYCL2 field in the TDCYCLSHTRL0 register.
 */
__INLINE uint8_t mdm_ltdcycl2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDCYCLSHTRL0_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the LTDCYCL2 field of the TDCYCLSHTRL0 register.
 *
 * The TDCYCLSHTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ltdcycl2 - The value to set the field to.
 */
__INLINE void mdm_ltdcycl2_setf(uint8_t ltdcycl2)
{
    ASSERT_ERR((((uint32_t)ltdcycl2 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_TDCYCLSHTRL0_ADDR, (REG_PL_RD(MDM_TDCYCLSHTRL0_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)ltdcycl2 << 0));
}

/// @}

/**
 * @name TDCYCLSHTRL1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  11:08            HTTDCYCL4   0xC
 *  07:04            HTTDCYCL3   0x4
 *  03:00            HTTDCYCL2   0x8
 * </pre>
 *
 * @{
 */

/// Address of the TDCYCLSHTRL1 register
#define MDM_TDCYCLSHTRL1_ADDR   0x01000858
/// Offset of the TDCYCLSHTRL1 register from the base address
#define MDM_TDCYCLSHTRL1_OFFSET 0x00000858
/// Index of the TDCYCLSHTRL1 register
#define MDM_TDCYCLSHTRL1_INDEX  0x00000216
/// Reset value of the TDCYCLSHTRL1 register
#define MDM_TDCYCLSHTRL1_RESET  0x00000C48

/**
 * @brief Returns the current value of the TDCYCLSHTRL1 register.
 * The TDCYCLSHTRL1 register will be read and its value returned.
 * @return The current value of the TDCYCLSHTRL1 register.
 */
__INLINE uint32_t mdm_tdcyclshtrl1_get(void)
{
    return REG_PL_RD(MDM_TDCYCLSHTRL1_ADDR);
}

/**
 * @brief Sets the TDCYCLSHTRL1 register to a value.
 * The TDCYCLSHTRL1 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_tdcyclshtrl1_set(uint32_t value)
{
    REG_PL_WR(MDM_TDCYCLSHTRL1_ADDR, value);
}

// field definitions
/// HTTDCYCL4 field mask
#define MDM_HTTDCYCL4_MASK   ((uint32_t)0x00000F00)
/// HTTDCYCL4 field LSB position
#define MDM_HTTDCYCL4_LSB    8
/// HTTDCYCL4 field width
#define MDM_HTTDCYCL4_WIDTH  ((uint32_t)0x00000004)
/// HTTDCYCL3 field mask
#define MDM_HTTDCYCL3_MASK   ((uint32_t)0x000000F0)
/// HTTDCYCL3 field LSB position
#define MDM_HTTDCYCL3_LSB    4
/// HTTDCYCL3 field width
#define MDM_HTTDCYCL3_WIDTH  ((uint32_t)0x00000004)
/// HTTDCYCL2 field mask
#define MDM_HTTDCYCL2_MASK   ((uint32_t)0x0000000F)
/// HTTDCYCL2 field LSB position
#define MDM_HTTDCYCL2_LSB    0
/// HTTDCYCL2 field width
#define MDM_HTTDCYCL2_WIDTH  ((uint32_t)0x00000004)

/// HTTDCYCL4 field reset value
#define MDM_HTTDCYCL4_RST    0xC
/// HTTDCYCL3 field reset value
#define MDM_HTTDCYCL3_RST    0x4
/// HTTDCYCL2 field reset value
#define MDM_HTTDCYCL2_RST    0x8

/**
 * @brief Constructs a value for the TDCYCLSHTRL1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] httdcycl4 - The value to use for the HTTDCYCL4 field.
 * @param[in] httdcycl3 - The value to use for the HTTDCYCL3 field.
 * @param[in] httdcycl2 - The value to use for the HTTDCYCL2 field.
 */
__INLINE void mdm_tdcyclshtrl1_pack(uint8_t httdcycl4, uint8_t httdcycl3, uint8_t httdcycl2)
{
    ASSERT_ERR((((uint32_t)httdcycl4 << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)httdcycl3 << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)httdcycl2 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_TDCYCLSHTRL1_ADDR,  ((uint32_t)httdcycl4 << 8) | ((uint32_t)httdcycl3 << 4) | ((uint32_t)httdcycl2 << 0));
}

/**
 * @brief Unpacks TDCYCLSHTRL1's fields from current value of the TDCYCLSHTRL1 register.
 *
 * Reads the TDCYCLSHTRL1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] httdcycl4 - Will be populated with the current value of this field from the register.
 * @param[out] httdcycl3 - Will be populated with the current value of this field from the register.
 * @param[out] httdcycl2 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_tdcyclshtrl1_unpack(uint8_t *httdcycl4, uint8_t *httdcycl3, uint8_t *httdcycl2)
{
    uint32_t localVal = REG_PL_RD(MDM_TDCYCLSHTRL1_ADDR);

    *httdcycl4 = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *httdcycl3 = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *httdcycl2 = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the HTTDCYCL4 field in the TDCYCLSHTRL1 register.
 *
 * The TDCYCLSHTRL1 register will be read and the HTTDCYCL4 field's value will be returned.
 *
 * @return The current value of the HTTDCYCL4 field in the TDCYCLSHTRL1 register.
 */
__INLINE uint8_t mdm_httdcycl4_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDCYCLSHTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the HTTDCYCL4 field of the TDCYCLSHTRL1 register.
 *
 * The TDCYCLSHTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] httdcycl4 - The value to set the field to.
 */
__INLINE void mdm_httdcycl4_setf(uint8_t httdcycl4)
{
    ASSERT_ERR((((uint32_t)httdcycl4 << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(MDM_TDCYCLSHTRL1_ADDR, (REG_PL_RD(MDM_TDCYCLSHTRL1_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)httdcycl4 << 8));
}

/**
 * @brief Returns the current value of the HTTDCYCL3 field in the TDCYCLSHTRL1 register.
 *
 * The TDCYCLSHTRL1 register will be read and the HTTDCYCL3 field's value will be returned.
 *
 * @return The current value of the HTTDCYCL3 field in the TDCYCLSHTRL1 register.
 */
__INLINE uint8_t mdm_httdcycl3_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDCYCLSHTRL1_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the HTTDCYCL3 field of the TDCYCLSHTRL1 register.
 *
 * The TDCYCLSHTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] httdcycl3 - The value to set the field to.
 */
__INLINE void mdm_httdcycl3_setf(uint8_t httdcycl3)
{
    ASSERT_ERR((((uint32_t)httdcycl3 << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(MDM_TDCYCLSHTRL1_ADDR, (REG_PL_RD(MDM_TDCYCLSHTRL1_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)httdcycl3 << 4));
}

/**
 * @brief Returns the current value of the HTTDCYCL2 field in the TDCYCLSHTRL1 register.
 *
 * The TDCYCLSHTRL1 register will be read and the HTTDCYCL2 field's value will be returned.
 *
 * @return The current value of the HTTDCYCL2 field in the TDCYCLSHTRL1 register.
 */
__INLINE uint8_t mdm_httdcycl2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDCYCLSHTRL1_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the HTTDCYCL2 field of the TDCYCLSHTRL1 register.
 *
 * The TDCYCLSHTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] httdcycl2 - The value to set the field to.
 */
__INLINE void mdm_httdcycl2_setf(uint8_t httdcycl2)
{
    ASSERT_ERR((((uint32_t)httdcycl2 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_TDCYCLSHTRL1_ADDR, (REG_PL_RD(MDM_TDCYCLSHTRL1_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)httdcycl2 << 0));
}

/// @}

/**
 * @name SCRAMBLERCTRL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  07:00            SCRAMSEED   0x0
 * </pre>
 *
 * @{
 */

/// Address of the SCRAMBLERCTRL register
#define MDM_SCRAMBLERCTRL_ADDR   0x0100085C
/// Offset of the SCRAMBLERCTRL register from the base address
#define MDM_SCRAMBLERCTRL_OFFSET 0x0000085C
/// Index of the SCRAMBLERCTRL register
#define MDM_SCRAMBLERCTRL_INDEX  0x00000217
/// Reset value of the SCRAMBLERCTRL register
#define MDM_SCRAMBLERCTRL_RESET  0x00000000

/**
 * @brief Returns the current value of the SCRAMBLERCTRL register.
 * The SCRAMBLERCTRL register will be read and its value returned.
 * @return The current value of the SCRAMBLERCTRL register.
 */
__INLINE uint32_t mdm_scramblerctrl_get(void)
{
    return REG_PL_RD(MDM_SCRAMBLERCTRL_ADDR);
}

/**
 * @brief Sets the SCRAMBLERCTRL register to a value.
 * The SCRAMBLERCTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_scramblerctrl_set(uint32_t value)
{
    REG_PL_WR(MDM_SCRAMBLERCTRL_ADDR, value);
}

// field definitions
/// SCRAMSEED field mask
#define MDM_SCRAMSEED_MASK   ((uint32_t)0x000000FF)
/// SCRAMSEED field LSB position
#define MDM_SCRAMSEED_LSB    0
/// SCRAMSEED field width
#define MDM_SCRAMSEED_WIDTH  ((uint32_t)0x00000008)

/// SCRAMSEED field reset value
#define MDM_SCRAMSEED_RST    0x0

/**
 * @brief Returns the current value of the SCRAMSEED field in the SCRAMBLERCTRL register.
 *
 * The SCRAMBLERCTRL register will be read and the SCRAMSEED field's value will be returned.
 *
 * @return The current value of the SCRAMSEED field in the SCRAMBLERCTRL register.
 */
__INLINE uint8_t mdm_scramseed_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SCRAMBLERCTRL_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000000FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the SCRAMSEED field of the SCRAMBLERCTRL register.
 *
 * The SCRAMBLERCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] scramseed - The value to set the field to.
 */
__INLINE void mdm_scramseed_setf(uint8_t scramseed)
{
    ASSERT_ERR((((uint32_t)scramseed << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_SCRAMBLERCTRL_ADDR, (uint32_t)scramseed << 0);
}

/// @}

/**
 * @name TBECTRL0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:24   CrossCorrSNRThreshold   0xC
 *  23:16     TBECountAdjust80   0x11
 *  15:08     TBECountAdjust40   0x7
 *  07:00     TBECountAdjust20   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TBECTRL0 register
#define MDM_TBECTRL0_ADDR   0x01000860
/// Offset of the TBECTRL0 register from the base address
#define MDM_TBECTRL0_OFFSET 0x00000860
/// Index of the TBECTRL0 register
#define MDM_TBECTRL0_INDEX  0x00000218
/// Reset value of the TBECTRL0 register
#define MDM_TBECTRL0_RESET  0x0C110700

/**
 * @brief Returns the current value of the TBECTRL0 register.
 * The TBECTRL0 register will be read and its value returned.
 * @return The current value of the TBECTRL0 register.
 */
__INLINE uint32_t mdm_tbectrl0_get(void)
{
    return REG_PL_RD(MDM_TBECTRL0_ADDR);
}

/**
 * @brief Sets the TBECTRL0 register to a value.
 * The TBECTRL0 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_tbectrl0_set(uint32_t value)
{
    REG_PL_WR(MDM_TBECTRL0_ADDR, value);
}

// field definitions
/// CROSS_CORR_SNR_THRESHOLD field mask
#define MDM_CROSS_CORR_SNR_THRESHOLD_MASK   ((uint32_t)0xFF000000)
/// CROSS_CORR_SNR_THRESHOLD field LSB position
#define MDM_CROSS_CORR_SNR_THRESHOLD_LSB    24
/// CROSS_CORR_SNR_THRESHOLD field width
#define MDM_CROSS_CORR_SNR_THRESHOLD_WIDTH  ((uint32_t)0x00000008)
/// TBE_COUNT_ADJUST_80 field mask
#define MDM_TBE_COUNT_ADJUST_80_MASK        ((uint32_t)0x00FF0000)
/// TBE_COUNT_ADJUST_80 field LSB position
#define MDM_TBE_COUNT_ADJUST_80_LSB         16
/// TBE_COUNT_ADJUST_80 field width
#define MDM_TBE_COUNT_ADJUST_80_WIDTH       ((uint32_t)0x00000008)
/// TBE_COUNT_ADJUST_40 field mask
#define MDM_TBE_COUNT_ADJUST_40_MASK        ((uint32_t)0x0000FF00)
/// TBE_COUNT_ADJUST_40 field LSB position
#define MDM_TBE_COUNT_ADJUST_40_LSB         8
/// TBE_COUNT_ADJUST_40 field width
#define MDM_TBE_COUNT_ADJUST_40_WIDTH       ((uint32_t)0x00000008)
/// TBE_COUNT_ADJUST_20 field mask
#define MDM_TBE_COUNT_ADJUST_20_MASK        ((uint32_t)0x000000FF)
/// TBE_COUNT_ADJUST_20 field LSB position
#define MDM_TBE_COUNT_ADJUST_20_LSB         0
/// TBE_COUNT_ADJUST_20 field width
#define MDM_TBE_COUNT_ADJUST_20_WIDTH       ((uint32_t)0x00000008)

/// CROSS_CORR_SNR_THRESHOLD field reset value
#define MDM_CROSS_CORR_SNR_THRESHOLD_RST    0xC
/// TBE_COUNT_ADJUST_80 field reset value
#define MDM_TBE_COUNT_ADJUST_80_RST         0x11
/// TBE_COUNT_ADJUST_40 field reset value
#define MDM_TBE_COUNT_ADJUST_40_RST         0x7
/// TBE_COUNT_ADJUST_20 field reset value
#define MDM_TBE_COUNT_ADJUST_20_RST         0x0

/**
 * @brief Constructs a value for the TBECTRL0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] crosscorrsnrthreshold - The value to use for the CrossCorrSNRThreshold field.
 * @param[in] tbecountadjust80 - The value to use for the TBECountAdjust80 field.
 * @param[in] tbecountadjust40 - The value to use for the TBECountAdjust40 field.
 * @param[in] tbecountadjust20 - The value to use for the TBECountAdjust20 field.
 */
__INLINE void mdm_tbectrl0_pack(uint8_t crosscorrsnrthreshold, uint8_t tbecountadjust80, uint8_t tbecountadjust40, uint8_t tbecountadjust20)
{
    ASSERT_ERR((((uint32_t)crosscorrsnrthreshold << 24) & ~((uint32_t)0xFF000000)) == 0);
    ASSERT_ERR((((uint32_t)tbecountadjust80 << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)tbecountadjust40 << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)tbecountadjust20 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_TBECTRL0_ADDR,  ((uint32_t)crosscorrsnrthreshold << 24) | ((uint32_t)tbecountadjust80 << 16) | ((uint32_t)tbecountadjust40 << 8) | ((uint32_t)tbecountadjust20 << 0));
}

/**
 * @brief Unpacks TBECTRL0's fields from current value of the TBECTRL0 register.
 *
 * Reads the TBECTRL0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] crosscorrsnrthreshold - Will be populated with the current value of this field from the register.
 * @param[out] tbecountadjust80 - Will be populated with the current value of this field from the register.
 * @param[out] tbecountadjust40 - Will be populated with the current value of this field from the register.
 * @param[out] tbecountadjust20 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_tbectrl0_unpack(uint8_t *crosscorrsnrthreshold, uint8_t *tbecountadjust80, uint8_t *tbecountadjust40, uint8_t *tbecountadjust20)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL0_ADDR);

    *crosscorrsnrthreshold = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *tbecountadjust80 = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *tbecountadjust40 = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *tbecountadjust20 = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the CrossCorrSNRThreshold field in the TBECTRL0 register.
 *
 * The TBECTRL0 register will be read and the CrossCorrSNRThreshold field's value will be returned.
 *
 * @return The current value of the CrossCorrSNRThreshold field in the TBECTRL0 register.
 */
__INLINE uint8_t mdm_cross_corr_snr_threshold_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL0_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Sets the CrossCorrSNRThreshold field of the TBECTRL0 register.
 *
 * The TBECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] crosscorrsnrthreshold - The value to set the field to.
 */
__INLINE void mdm_cross_corr_snr_threshold_setf(uint8_t crosscorrsnrthreshold)
{
    ASSERT_ERR((((uint32_t)crosscorrsnrthreshold << 24) & ~((uint32_t)0xFF000000)) == 0);
    REG_PL_WR(MDM_TBECTRL0_ADDR, (REG_PL_RD(MDM_TBECTRL0_ADDR) & ~((uint32_t)0xFF000000)) | ((uint32_t)crosscorrsnrthreshold << 24));
}

/**
 * @brief Returns the current value of the TBECountAdjust80 field in the TBECTRL0 register.
 *
 * The TBECTRL0 register will be read and the TBECountAdjust80 field's value will be returned.
 *
 * @return The current value of the TBECountAdjust80 field in the TBECTRL0 register.
 */
__INLINE uint8_t mdm_tbe_count_adjust_80_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the TBECountAdjust80 field of the TBECTRL0 register.
 *
 * The TBECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tbecountadjust80 - The value to set the field to.
 */
__INLINE void mdm_tbe_count_adjust_80_setf(uint8_t tbecountadjust80)
{
    ASSERT_ERR((((uint32_t)tbecountadjust80 << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(MDM_TBECTRL0_ADDR, (REG_PL_RD(MDM_TBECTRL0_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)tbecountadjust80 << 16));
}

/**
 * @brief Returns the current value of the TBECountAdjust40 field in the TBECTRL0 register.
 *
 * The TBECTRL0 register will be read and the TBECountAdjust40 field's value will be returned.
 *
 * @return The current value of the TBECountAdjust40 field in the TBECTRL0 register.
 */
__INLINE uint8_t mdm_tbe_count_adjust_40_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the TBECountAdjust40 field of the TBECTRL0 register.
 *
 * The TBECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tbecountadjust40 - The value to set the field to.
 */
__INLINE void mdm_tbe_count_adjust_40_setf(uint8_t tbecountadjust40)
{
    ASSERT_ERR((((uint32_t)tbecountadjust40 << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(MDM_TBECTRL0_ADDR, (REG_PL_RD(MDM_TBECTRL0_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)tbecountadjust40 << 8));
}

/**
 * @brief Returns the current value of the TBECountAdjust20 field in the TBECTRL0 register.
 *
 * The TBECTRL0 register will be read and the TBECountAdjust20 field's value will be returned.
 *
 * @return The current value of the TBECountAdjust20 field in the TBECTRL0 register.
 */
__INLINE uint8_t mdm_tbe_count_adjust_20_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL0_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the TBECountAdjust20 field of the TBECTRL0 register.
 *
 * The TBECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tbecountadjust20 - The value to set the field to.
 */
__INLINE void mdm_tbe_count_adjust_20_setf(uint8_t tbecountadjust20)
{
    ASSERT_ERR((((uint32_t)tbecountadjust20 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_TBECTRL0_ADDR, (REG_PL_RD(MDM_TBECTRL0_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)tbecountadjust20 << 0));
}

/// @}

/**
 * @name TBECTRL1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  30:24   PeakSearchStopHighSNR   0x3C
 *  22:16   PeakSearchStartHighSNR   0x14
 *  14:08   PeakSearchStopLowSNR   0x3E
 *  06:00   PeakSearchStartLowSNR   0x14
 * </pre>
 *
 * @{
 */

/// Address of the TBECTRL1 register
#define MDM_TBECTRL1_ADDR   0x01000864
/// Offset of the TBECTRL1 register from the base address
#define MDM_TBECTRL1_OFFSET 0x00000864
/// Index of the TBECTRL1 register
#define MDM_TBECTRL1_INDEX  0x00000219
/// Reset value of the TBECTRL1 register
#define MDM_TBECTRL1_RESET  0x3C143E14

/**
 * @brief Returns the current value of the TBECTRL1 register.
 * The TBECTRL1 register will be read and its value returned.
 * @return The current value of the TBECTRL1 register.
 */
__INLINE uint32_t mdm_tbectrl1_get(void)
{
    return REG_PL_RD(MDM_TBECTRL1_ADDR);
}

/**
 * @brief Sets the TBECTRL1 register to a value.
 * The TBECTRL1 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_tbectrl1_set(uint32_t value)
{
    REG_PL_WR(MDM_TBECTRL1_ADDR, value);
}

// field definitions
/// PEAK_SEARCH_STOP_HIGH_SNR field mask
#define MDM_PEAK_SEARCH_STOP_HIGH_SNR_MASK    ((uint32_t)0x7F000000)
/// PEAK_SEARCH_STOP_HIGH_SNR field LSB position
#define MDM_PEAK_SEARCH_STOP_HIGH_SNR_LSB     24
/// PEAK_SEARCH_STOP_HIGH_SNR field width
#define MDM_PEAK_SEARCH_STOP_HIGH_SNR_WIDTH   ((uint32_t)0x00000007)
/// PEAK_SEARCH_START_HIGH_SNR field mask
#define MDM_PEAK_SEARCH_START_HIGH_SNR_MASK   ((uint32_t)0x007F0000)
/// PEAK_SEARCH_START_HIGH_SNR field LSB position
#define MDM_PEAK_SEARCH_START_HIGH_SNR_LSB    16
/// PEAK_SEARCH_START_HIGH_SNR field width
#define MDM_PEAK_SEARCH_START_HIGH_SNR_WIDTH  ((uint32_t)0x00000007)
/// PEAK_SEARCH_STOP_LOW_SNR field mask
#define MDM_PEAK_SEARCH_STOP_LOW_SNR_MASK     ((uint32_t)0x00007F00)
/// PEAK_SEARCH_STOP_LOW_SNR field LSB position
#define MDM_PEAK_SEARCH_STOP_LOW_SNR_LSB      8
/// PEAK_SEARCH_STOP_LOW_SNR field width
#define MDM_PEAK_SEARCH_STOP_LOW_SNR_WIDTH    ((uint32_t)0x00000007)
/// PEAK_SEARCH_START_LOW_SNR field mask
#define MDM_PEAK_SEARCH_START_LOW_SNR_MASK    ((uint32_t)0x0000007F)
/// PEAK_SEARCH_START_LOW_SNR field LSB position
#define MDM_PEAK_SEARCH_START_LOW_SNR_LSB     0
/// PEAK_SEARCH_START_LOW_SNR field width
#define MDM_PEAK_SEARCH_START_LOW_SNR_WIDTH   ((uint32_t)0x00000007)

/// PEAK_SEARCH_STOP_HIGH_SNR field reset value
#define MDM_PEAK_SEARCH_STOP_HIGH_SNR_RST     0x3C
/// PEAK_SEARCH_START_HIGH_SNR field reset value
#define MDM_PEAK_SEARCH_START_HIGH_SNR_RST    0x14
/// PEAK_SEARCH_STOP_LOW_SNR field reset value
#define MDM_PEAK_SEARCH_STOP_LOW_SNR_RST      0x3E
/// PEAK_SEARCH_START_LOW_SNR field reset value
#define MDM_PEAK_SEARCH_START_LOW_SNR_RST     0x14

/**
 * @brief Constructs a value for the TBECTRL1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] peaksearchstophighsnr - The value to use for the PeakSearchStopHighSNR field.
 * @param[in] peaksearchstarthighsnr - The value to use for the PeakSearchStartHighSNR field.
 * @param[in] peaksearchstoplowsnr - The value to use for the PeakSearchStopLowSNR field.
 * @param[in] peaksearchstartlowsnr - The value to use for the PeakSearchStartLowSNR field.
 */
__INLINE void mdm_tbectrl1_pack(uint8_t peaksearchstophighsnr, uint8_t peaksearchstarthighsnr, uint8_t peaksearchstoplowsnr, uint8_t peaksearchstartlowsnr)
{
    ASSERT_ERR((((uint32_t)peaksearchstophighsnr << 24) & ~((uint32_t)0x7F000000)) == 0);
    ASSERT_ERR((((uint32_t)peaksearchstarthighsnr << 16) & ~((uint32_t)0x007F0000)) == 0);
    ASSERT_ERR((((uint32_t)peaksearchstoplowsnr << 8) & ~((uint32_t)0x00007F00)) == 0);
    ASSERT_ERR((((uint32_t)peaksearchstartlowsnr << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_TBECTRL1_ADDR,  ((uint32_t)peaksearchstophighsnr << 24) | ((uint32_t)peaksearchstarthighsnr << 16) | ((uint32_t)peaksearchstoplowsnr << 8) | ((uint32_t)peaksearchstartlowsnr << 0));
}

/**
 * @brief Unpacks TBECTRL1's fields from current value of the TBECTRL1 register.
 *
 * Reads the TBECTRL1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] peaksearchstophighsnr - Will be populated with the current value of this field from the register.
 * @param[out] peaksearchstarthighsnr - Will be populated with the current value of this field from the register.
 * @param[out] peaksearchstoplowsnr - Will be populated with the current value of this field from the register.
 * @param[out] peaksearchstartlowsnr - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_tbectrl1_unpack(uint8_t *peaksearchstophighsnr, uint8_t *peaksearchstarthighsnr, uint8_t *peaksearchstoplowsnr, uint8_t *peaksearchstartlowsnr)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL1_ADDR);

    *peaksearchstophighsnr = (localVal & ((uint32_t)0x7F000000)) >> 24;
    *peaksearchstarthighsnr = (localVal & ((uint32_t)0x007F0000)) >> 16;
    *peaksearchstoplowsnr = (localVal & ((uint32_t)0x00007F00)) >> 8;
    *peaksearchstartlowsnr = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the PeakSearchStopHighSNR field in the TBECTRL1 register.
 *
 * The TBECTRL1 register will be read and the PeakSearchStopHighSNR field's value will be returned.
 *
 * @return The current value of the PeakSearchStopHighSNR field in the TBECTRL1 register.
 */
__INLINE uint8_t mdm_peak_search_stop_high_snr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL1_ADDR);
    return ((localVal & ((uint32_t)0x7F000000)) >> 24);
}

/**
 * @brief Sets the PeakSearchStopHighSNR field of the TBECTRL1 register.
 *
 * The TBECTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] peaksearchstophighsnr - The value to set the field to.
 */
__INLINE void mdm_peak_search_stop_high_snr_setf(uint8_t peaksearchstophighsnr)
{
    ASSERT_ERR((((uint32_t)peaksearchstophighsnr << 24) & ~((uint32_t)0x7F000000)) == 0);
    REG_PL_WR(MDM_TBECTRL1_ADDR, (REG_PL_RD(MDM_TBECTRL1_ADDR) & ~((uint32_t)0x7F000000)) | ((uint32_t)peaksearchstophighsnr << 24));
}

/**
 * @brief Returns the current value of the PeakSearchStartHighSNR field in the TBECTRL1 register.
 *
 * The TBECTRL1 register will be read and the PeakSearchStartHighSNR field's value will be returned.
 *
 * @return The current value of the PeakSearchStartHighSNR field in the TBECTRL1 register.
 */
__INLINE uint8_t mdm_peak_search_start_high_snr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL1_ADDR);
    return ((localVal & ((uint32_t)0x007F0000)) >> 16);
}

/**
 * @brief Sets the PeakSearchStartHighSNR field of the TBECTRL1 register.
 *
 * The TBECTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] peaksearchstarthighsnr - The value to set the field to.
 */
__INLINE void mdm_peak_search_start_high_snr_setf(uint8_t peaksearchstarthighsnr)
{
    ASSERT_ERR((((uint32_t)peaksearchstarthighsnr << 16) & ~((uint32_t)0x007F0000)) == 0);
    REG_PL_WR(MDM_TBECTRL1_ADDR, (REG_PL_RD(MDM_TBECTRL1_ADDR) & ~((uint32_t)0x007F0000)) | ((uint32_t)peaksearchstarthighsnr << 16));
}

/**
 * @brief Returns the current value of the PeakSearchStopLowSNR field in the TBECTRL1 register.
 *
 * The TBECTRL1 register will be read and the PeakSearchStopLowSNR field's value will be returned.
 *
 * @return The current value of the PeakSearchStopLowSNR field in the TBECTRL1 register.
 */
__INLINE uint8_t mdm_peak_search_stop_low_snr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}

/**
 * @brief Sets the PeakSearchStopLowSNR field of the TBECTRL1 register.
 *
 * The TBECTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] peaksearchstoplowsnr - The value to set the field to.
 */
__INLINE void mdm_peak_search_stop_low_snr_setf(uint8_t peaksearchstoplowsnr)
{
    ASSERT_ERR((((uint32_t)peaksearchstoplowsnr << 8) & ~((uint32_t)0x00007F00)) == 0);
    REG_PL_WR(MDM_TBECTRL1_ADDR, (REG_PL_RD(MDM_TBECTRL1_ADDR) & ~((uint32_t)0x00007F00)) | ((uint32_t)peaksearchstoplowsnr << 8));
}

/**
 * @brief Returns the current value of the PeakSearchStartLowSNR field in the TBECTRL1 register.
 *
 * The TBECTRL1 register will be read and the PeakSearchStartLowSNR field's value will be returned.
 *
 * @return The current value of the PeakSearchStartLowSNR field in the TBECTRL1 register.
 */
__INLINE uint8_t mdm_peak_search_start_low_snr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL1_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/**
 * @brief Sets the PeakSearchStartLowSNR field of the TBECTRL1 register.
 *
 * The TBECTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] peaksearchstartlowsnr - The value to set the field to.
 */
__INLINE void mdm_peak_search_start_low_snr_setf(uint8_t peaksearchstartlowsnr)
{
    ASSERT_ERR((((uint32_t)peaksearchstartlowsnr << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_TBECTRL1_ADDR, (REG_PL_RD(MDM_TBECTRL1_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)peaksearchstartlowsnr << 0));
}

/// @}

/**
 * @name TBECTRL2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:28    TDAdjust80ShortGI   0x0
 *  27:24    TDAdjust40ShortGI   0x0
 *  23:20    TDAdjust20ShortGI   0x0
 *  16:12      PeakSearchDelta   0x7
 *  07:00              TBEBias   0x5
 * </pre>
 *
 * @{
 */

/// Address of the TBECTRL2 register
#define MDM_TBECTRL2_ADDR   0x01000868
/// Offset of the TBECTRL2 register from the base address
#define MDM_TBECTRL2_OFFSET 0x00000868
/// Index of the TBECTRL2 register
#define MDM_TBECTRL2_INDEX  0x0000021A
/// Reset value of the TBECTRL2 register
#define MDM_TBECTRL2_RESET  0x00007005

/**
 * @brief Returns the current value of the TBECTRL2 register.
 * The TBECTRL2 register will be read and its value returned.
 * @return The current value of the TBECTRL2 register.
 */
__INLINE uint32_t mdm_tbectrl2_get(void)
{
    return REG_PL_RD(MDM_TBECTRL2_ADDR);
}

/**
 * @brief Sets the TBECTRL2 register to a value.
 * The TBECTRL2 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_tbectrl2_set(uint32_t value)
{
    REG_PL_WR(MDM_TBECTRL2_ADDR, value);
}

// field definitions
/// TD_ADJUST_80_SHORT_GI field mask
#define MDM_TD_ADJUST_80_SHORT_GI_MASK   ((uint32_t)0xF0000000)
/// TD_ADJUST_80_SHORT_GI field LSB position
#define MDM_TD_ADJUST_80_SHORT_GI_LSB    28
/// TD_ADJUST_80_SHORT_GI field width
#define MDM_TD_ADJUST_80_SHORT_GI_WIDTH  ((uint32_t)0x00000004)
/// TD_ADJUST_40_SHORT_GI field mask
#define MDM_TD_ADJUST_40_SHORT_GI_MASK   ((uint32_t)0x0F000000)
/// TD_ADJUST_40_SHORT_GI field LSB position
#define MDM_TD_ADJUST_40_SHORT_GI_LSB    24
/// TD_ADJUST_40_SHORT_GI field width
#define MDM_TD_ADJUST_40_SHORT_GI_WIDTH  ((uint32_t)0x00000004)
/// TD_ADJUST_20_SHORT_GI field mask
#define MDM_TD_ADJUST_20_SHORT_GI_MASK   ((uint32_t)0x00F00000)
/// TD_ADJUST_20_SHORT_GI field LSB position
#define MDM_TD_ADJUST_20_SHORT_GI_LSB    20
/// TD_ADJUST_20_SHORT_GI field width
#define MDM_TD_ADJUST_20_SHORT_GI_WIDTH  ((uint32_t)0x00000004)
/// PEAK_SEARCH_DELTA field mask
#define MDM_PEAK_SEARCH_DELTA_MASK       ((uint32_t)0x0001F000)
/// PEAK_SEARCH_DELTA field LSB position
#define MDM_PEAK_SEARCH_DELTA_LSB        12
/// PEAK_SEARCH_DELTA field width
#define MDM_PEAK_SEARCH_DELTA_WIDTH      ((uint32_t)0x00000005)
/// TBE_BIAS field mask
#define MDM_TBE_BIAS_MASK                ((uint32_t)0x000000FF)
/// TBE_BIAS field LSB position
#define MDM_TBE_BIAS_LSB                 0
/// TBE_BIAS field width
#define MDM_TBE_BIAS_WIDTH               ((uint32_t)0x00000008)

/// TD_ADJUST_80_SHORT_GI field reset value
#define MDM_TD_ADJUST_80_SHORT_GI_RST    0x0
/// TD_ADJUST_40_SHORT_GI field reset value
#define MDM_TD_ADJUST_40_SHORT_GI_RST    0x0
/// TD_ADJUST_20_SHORT_GI field reset value
#define MDM_TD_ADJUST_20_SHORT_GI_RST    0x0
/// PEAK_SEARCH_DELTA field reset value
#define MDM_PEAK_SEARCH_DELTA_RST        0x7
/// TBE_BIAS field reset value
#define MDM_TBE_BIAS_RST                 0x5

/**
 * @brief Constructs a value for the TBECTRL2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] tdadjust80shortgi - The value to use for the TDAdjust80ShortGI field.
 * @param[in] tdadjust40shortgi - The value to use for the TDAdjust40ShortGI field.
 * @param[in] tdadjust20shortgi - The value to use for the TDAdjust20ShortGI field.
 * @param[in] peaksearchdelta - The value to use for the PeakSearchDelta field.
 * @param[in] tbebias - The value to use for the TBEBias field.
 */
__INLINE void mdm_tbectrl2_pack(uint8_t tdadjust80shortgi, uint8_t tdadjust40shortgi, uint8_t tdadjust20shortgi, uint8_t peaksearchdelta, uint8_t tbebias)
{
    ASSERT_ERR((((uint32_t)tdadjust80shortgi << 28) & ~((uint32_t)0xF0000000)) == 0);
    ASSERT_ERR((((uint32_t)tdadjust40shortgi << 24) & ~((uint32_t)0x0F000000)) == 0);
    ASSERT_ERR((((uint32_t)tdadjust20shortgi << 20) & ~((uint32_t)0x00F00000)) == 0);
    ASSERT_ERR((((uint32_t)peaksearchdelta << 12) & ~((uint32_t)0x0001F000)) == 0);
    ASSERT_ERR((((uint32_t)tbebias << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_TBECTRL2_ADDR,  ((uint32_t)tdadjust80shortgi << 28) | ((uint32_t)tdadjust40shortgi << 24) | ((uint32_t)tdadjust20shortgi << 20) | ((uint32_t)peaksearchdelta << 12) | ((uint32_t)tbebias << 0));
}

/**
 * @brief Unpacks TBECTRL2's fields from current value of the TBECTRL2 register.
 *
 * Reads the TBECTRL2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] tdadjust80shortgi - Will be populated with the current value of this field from the register.
 * @param[out] tdadjust40shortgi - Will be populated with the current value of this field from the register.
 * @param[out] tdadjust20shortgi - Will be populated with the current value of this field from the register.
 * @param[out] peaksearchdelta - Will be populated with the current value of this field from the register.
 * @param[out] tbebias - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_tbectrl2_unpack(uint8_t *tdadjust80shortgi, uint8_t *tdadjust40shortgi, uint8_t *tdadjust20shortgi, uint8_t *peaksearchdelta, uint8_t *tbebias)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL2_ADDR);

    *tdadjust80shortgi = (localVal & ((uint32_t)0xF0000000)) >> 28;
    *tdadjust40shortgi = (localVal & ((uint32_t)0x0F000000)) >> 24;
    *tdadjust20shortgi = (localVal & ((uint32_t)0x00F00000)) >> 20;
    *peaksearchdelta = (localVal & ((uint32_t)0x0001F000)) >> 12;
    *tbebias = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the TDAdjust80ShortGI field in the TBECTRL2 register.
 *
 * The TBECTRL2 register will be read and the TDAdjust80ShortGI field's value will be returned.
 *
 * @return The current value of the TDAdjust80ShortGI field in the TBECTRL2 register.
 */
__INLINE uint8_t mdm_td_adjust_80_short_gi_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL2_ADDR);
    return ((localVal & ((uint32_t)0xF0000000)) >> 28);
}

/**
 * @brief Sets the TDAdjust80ShortGI field of the TBECTRL2 register.
 *
 * The TBECTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdadjust80shortgi - The value to set the field to.
 */
__INLINE void mdm_td_adjust_80_short_gi_setf(uint8_t tdadjust80shortgi)
{
    ASSERT_ERR((((uint32_t)tdadjust80shortgi << 28) & ~((uint32_t)0xF0000000)) == 0);
    REG_PL_WR(MDM_TBECTRL2_ADDR, (REG_PL_RD(MDM_TBECTRL2_ADDR) & ~((uint32_t)0xF0000000)) | ((uint32_t)tdadjust80shortgi << 28));
}

/**
 * @brief Returns the current value of the TDAdjust40ShortGI field in the TBECTRL2 register.
 *
 * The TBECTRL2 register will be read and the TDAdjust40ShortGI field's value will be returned.
 *
 * @return The current value of the TDAdjust40ShortGI field in the TBECTRL2 register.
 */
__INLINE uint8_t mdm_td_adjust_40_short_gi_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL2_ADDR);
    return ((localVal & ((uint32_t)0x0F000000)) >> 24);
}

/**
 * @brief Sets the TDAdjust40ShortGI field of the TBECTRL2 register.
 *
 * The TBECTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdadjust40shortgi - The value to set the field to.
 */
__INLINE void mdm_td_adjust_40_short_gi_setf(uint8_t tdadjust40shortgi)
{
    ASSERT_ERR((((uint32_t)tdadjust40shortgi << 24) & ~((uint32_t)0x0F000000)) == 0);
    REG_PL_WR(MDM_TBECTRL2_ADDR, (REG_PL_RD(MDM_TBECTRL2_ADDR) & ~((uint32_t)0x0F000000)) | ((uint32_t)tdadjust40shortgi << 24));
}

/**
 * @brief Returns the current value of the TDAdjust20ShortGI field in the TBECTRL2 register.
 *
 * The TBECTRL2 register will be read and the TDAdjust20ShortGI field's value will be returned.
 *
 * @return The current value of the TDAdjust20ShortGI field in the TBECTRL2 register.
 */
__INLINE uint8_t mdm_td_adjust_20_short_gi_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00F00000)) >> 20);
}

/**
 * @brief Sets the TDAdjust20ShortGI field of the TBECTRL2 register.
 *
 * The TBECTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdadjust20shortgi - The value to set the field to.
 */
__INLINE void mdm_td_adjust_20_short_gi_setf(uint8_t tdadjust20shortgi)
{
    ASSERT_ERR((((uint32_t)tdadjust20shortgi << 20) & ~((uint32_t)0x00F00000)) == 0);
    REG_PL_WR(MDM_TBECTRL2_ADDR, (REG_PL_RD(MDM_TBECTRL2_ADDR) & ~((uint32_t)0x00F00000)) | ((uint32_t)tdadjust20shortgi << 20));
}

/**
 * @brief Returns the current value of the PeakSearchDelta field in the TBECTRL2 register.
 *
 * The TBECTRL2 register will be read and the PeakSearchDelta field's value will be returned.
 *
 * @return The current value of the PeakSearchDelta field in the TBECTRL2 register.
 */
__INLINE uint8_t mdm_peak_search_delta_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL2_ADDR);
    return ((localVal & ((uint32_t)0x0001F000)) >> 12);
}

/**
 * @brief Sets the PeakSearchDelta field of the TBECTRL2 register.
 *
 * The TBECTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] peaksearchdelta - The value to set the field to.
 */
__INLINE void mdm_peak_search_delta_setf(uint8_t peaksearchdelta)
{
    ASSERT_ERR((((uint32_t)peaksearchdelta << 12) & ~((uint32_t)0x0001F000)) == 0);
    REG_PL_WR(MDM_TBECTRL2_ADDR, (REG_PL_RD(MDM_TBECTRL2_ADDR) & ~((uint32_t)0x0001F000)) | ((uint32_t)peaksearchdelta << 12));
}

/**
 * @brief Returns the current value of the TBEBias field in the TBECTRL2 register.
 *
 * The TBECTRL2 register will be read and the TBEBias field's value will be returned.
 *
 * @return The current value of the TBEBias field in the TBECTRL2 register.
 */
__INLINE uint8_t mdm_tbe_bias_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL2_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the TBEBias field of the TBECTRL2 register.
 *
 * The TBECTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tbebias - The value to set the field to.
 */
__INLINE void mdm_tbe_bias_setf(uint8_t tbebias)
{
    ASSERT_ERR((((uint32_t)tbebias << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_TBECTRL2_ADDR, (REG_PL_RD(MDM_TBECTRL2_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)tbebias << 0));
}

/// @}

/**
 * @name TDFOCTRL0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  21:12   AutoCorrCompareRatioHighSNR   0x200
 *  09:00   AutoCorrCompareRatioLowSNR   0x19A
 * </pre>
 *
 * @{
 */

/// Address of the TDFOCTRL0 register
#define MDM_TDFOCTRL0_ADDR   0x0100086C
/// Offset of the TDFOCTRL0 register from the base address
#define MDM_TDFOCTRL0_OFFSET 0x0000086C
/// Index of the TDFOCTRL0 register
#define MDM_TDFOCTRL0_INDEX  0x0000021B
/// Reset value of the TDFOCTRL0 register
#define MDM_TDFOCTRL0_RESET  0x0020019A

/**
 * @brief Returns the current value of the TDFOCTRL0 register.
 * The TDFOCTRL0 register will be read and its value returned.
 * @return The current value of the TDFOCTRL0 register.
 */
__INLINE uint32_t mdm_tdfoctrl0_get(void)
{
    return REG_PL_RD(MDM_TDFOCTRL0_ADDR);
}

/**
 * @brief Sets the TDFOCTRL0 register to a value.
 * The TDFOCTRL0 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_tdfoctrl0_set(uint32_t value)
{
    REG_PL_WR(MDM_TDFOCTRL0_ADDR, value);
}

// field definitions
/// AUTO_CORR_COMPARE_RATIO_HIGH_SNR field mask
#define MDM_AUTO_CORR_COMPARE_RATIO_HIGH_SNR_MASK   ((uint32_t)0x003FF000)
/// AUTO_CORR_COMPARE_RATIO_HIGH_SNR field LSB position
#define MDM_AUTO_CORR_COMPARE_RATIO_HIGH_SNR_LSB    12
/// AUTO_CORR_COMPARE_RATIO_HIGH_SNR field width
#define MDM_AUTO_CORR_COMPARE_RATIO_HIGH_SNR_WIDTH  ((uint32_t)0x0000000A)
/// AUTO_CORR_COMPARE_RATIO_LOW_SNR field mask
#define MDM_AUTO_CORR_COMPARE_RATIO_LOW_SNR_MASK    ((uint32_t)0x000003FF)
/// AUTO_CORR_COMPARE_RATIO_LOW_SNR field LSB position
#define MDM_AUTO_CORR_COMPARE_RATIO_LOW_SNR_LSB     0
/// AUTO_CORR_COMPARE_RATIO_LOW_SNR field width
#define MDM_AUTO_CORR_COMPARE_RATIO_LOW_SNR_WIDTH   ((uint32_t)0x0000000A)

/// AUTO_CORR_COMPARE_RATIO_HIGH_SNR field reset value
#define MDM_AUTO_CORR_COMPARE_RATIO_HIGH_SNR_RST    0x200
/// AUTO_CORR_COMPARE_RATIO_LOW_SNR field reset value
#define MDM_AUTO_CORR_COMPARE_RATIO_LOW_SNR_RST     0x19A

/**
 * @brief Constructs a value for the TDFOCTRL0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] autocorrcompareratiohighsnr - The value to use for the AutoCorrCompareRatioHighSNR field.
 * @param[in] autocorrcompareratiolowsnr - The value to use for the AutoCorrCompareRatioLowSNR field.
 */
__INLINE void mdm_tdfoctrl0_pack(uint16_t autocorrcompareratiohighsnr, uint16_t autocorrcompareratiolowsnr)
{
    ASSERT_ERR((((uint32_t)autocorrcompareratiohighsnr << 12) & ~((uint32_t)0x003FF000)) == 0);
    ASSERT_ERR((((uint32_t)autocorrcompareratiolowsnr << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(MDM_TDFOCTRL0_ADDR,  ((uint32_t)autocorrcompareratiohighsnr << 12) | ((uint32_t)autocorrcompareratiolowsnr << 0));
}

/**
 * @brief Unpacks TDFOCTRL0's fields from current value of the TDFOCTRL0 register.
 *
 * Reads the TDFOCTRL0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] autocorrcompareratiohighsnr - Will be populated with the current value of this field from the register.
 * @param[out] autocorrcompareratiolowsnr - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_tdfoctrl0_unpack(uint16_t *autocorrcompareratiohighsnr, uint16_t *autocorrcompareratiolowsnr)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL0_ADDR);

    *autocorrcompareratiohighsnr = (localVal & ((uint32_t)0x003FF000)) >> 12;
    *autocorrcompareratiolowsnr = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the AutoCorrCompareRatioHighSNR field in the TDFOCTRL0 register.
 *
 * The TDFOCTRL0 register will be read and the AutoCorrCompareRatioHighSNR field's value will be returned.
 *
 * @return The current value of the AutoCorrCompareRatioHighSNR field in the TDFOCTRL0 register.
 */
__INLINE uint16_t mdm_auto_corr_compare_ratio_high_snr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x003FF000)) >> 12);
}

/**
 * @brief Sets the AutoCorrCompareRatioHighSNR field of the TDFOCTRL0 register.
 *
 * The TDFOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] autocorrcompareratiohighsnr - The value to set the field to.
 */
__INLINE void mdm_auto_corr_compare_ratio_high_snr_setf(uint16_t autocorrcompareratiohighsnr)
{
    ASSERT_ERR((((uint32_t)autocorrcompareratiohighsnr << 12) & ~((uint32_t)0x003FF000)) == 0);
    REG_PL_WR(MDM_TDFOCTRL0_ADDR, (REG_PL_RD(MDM_TDFOCTRL0_ADDR) & ~((uint32_t)0x003FF000)) | ((uint32_t)autocorrcompareratiohighsnr << 12));
}

/**
 * @brief Returns the current value of the AutoCorrCompareRatioLowSNR field in the TDFOCTRL0 register.
 *
 * The TDFOCTRL0 register will be read and the AutoCorrCompareRatioLowSNR field's value will be returned.
 *
 * @return The current value of the AutoCorrCompareRatioLowSNR field in the TDFOCTRL0 register.
 */
__INLINE uint16_t mdm_auto_corr_compare_ratio_low_snr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the AutoCorrCompareRatioLowSNR field of the TDFOCTRL0 register.
 *
 * The TDFOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] autocorrcompareratiolowsnr - The value to set the field to.
 */
__INLINE void mdm_auto_corr_compare_ratio_low_snr_setf(uint16_t autocorrcompareratiolowsnr)
{
    ASSERT_ERR((((uint32_t)autocorrcompareratiolowsnr << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(MDM_TDFOCTRL0_ADDR, (REG_PL_RD(MDM_TDFOCTRL0_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)autocorrcompareratiolowsnr << 0));
}

/// @}

/**
 * @name TDFOCTRL1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  20:12   AutoCorrPlatSumEnd   0x42
 *  08:00   AutoCorrPlatSumStart   0x32
 * </pre>
 *
 * @{
 */

/// Address of the TDFOCTRL1 register
#define MDM_TDFOCTRL1_ADDR   0x01000870
/// Offset of the TDFOCTRL1 register from the base address
#define MDM_TDFOCTRL1_OFFSET 0x00000870
/// Index of the TDFOCTRL1 register
#define MDM_TDFOCTRL1_INDEX  0x0000021C
/// Reset value of the TDFOCTRL1 register
#define MDM_TDFOCTRL1_RESET  0x00042032

/**
 * @brief Returns the current value of the TDFOCTRL1 register.
 * The TDFOCTRL1 register will be read and its value returned.
 * @return The current value of the TDFOCTRL1 register.
 */
__INLINE uint32_t mdm_tdfoctrl1_get(void)
{
    return REG_PL_RD(MDM_TDFOCTRL1_ADDR);
}

/**
 * @brief Sets the TDFOCTRL1 register to a value.
 * The TDFOCTRL1 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_tdfoctrl1_set(uint32_t value)
{
    REG_PL_WR(MDM_TDFOCTRL1_ADDR, value);
}

// field definitions
/// AUTO_CORR_PLAT_SUM_END field mask
#define MDM_AUTO_CORR_PLAT_SUM_END_MASK     ((uint32_t)0x001FF000)
/// AUTO_CORR_PLAT_SUM_END field LSB position
#define MDM_AUTO_CORR_PLAT_SUM_END_LSB      12
/// AUTO_CORR_PLAT_SUM_END field width
#define MDM_AUTO_CORR_PLAT_SUM_END_WIDTH    ((uint32_t)0x00000009)
/// AUTO_CORR_PLAT_SUM_START field mask
#define MDM_AUTO_CORR_PLAT_SUM_START_MASK   ((uint32_t)0x000001FF)
/// AUTO_CORR_PLAT_SUM_START field LSB position
#define MDM_AUTO_CORR_PLAT_SUM_START_LSB    0
/// AUTO_CORR_PLAT_SUM_START field width
#define MDM_AUTO_CORR_PLAT_SUM_START_WIDTH  ((uint32_t)0x00000009)

/// AUTO_CORR_PLAT_SUM_END field reset value
#define MDM_AUTO_CORR_PLAT_SUM_END_RST      0x42
/// AUTO_CORR_PLAT_SUM_START field reset value
#define MDM_AUTO_CORR_PLAT_SUM_START_RST    0x32

/**
 * @brief Constructs a value for the TDFOCTRL1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] autocorrplatsumend - The value to use for the AutoCorrPlatSumEnd field.
 * @param[in] autocorrplatsumstart - The value to use for the AutoCorrPlatSumStart field.
 */
__INLINE void mdm_tdfoctrl1_pack(uint16_t autocorrplatsumend, uint16_t autocorrplatsumstart)
{
    ASSERT_ERR((((uint32_t)autocorrplatsumend << 12) & ~((uint32_t)0x001FF000)) == 0);
    ASSERT_ERR((((uint32_t)autocorrplatsumstart << 0) & ~((uint32_t)0x000001FF)) == 0);
    REG_PL_WR(MDM_TDFOCTRL1_ADDR,  ((uint32_t)autocorrplatsumend << 12) | ((uint32_t)autocorrplatsumstart << 0));
}

/**
 * @brief Unpacks TDFOCTRL1's fields from current value of the TDFOCTRL1 register.
 *
 * Reads the TDFOCTRL1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] autocorrplatsumend - Will be populated with the current value of this field from the register.
 * @param[out] autocorrplatsumstart - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_tdfoctrl1_unpack(uint16_t *autocorrplatsumend, uint16_t *autocorrplatsumstart)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL1_ADDR);

    *autocorrplatsumend = (localVal & ((uint32_t)0x001FF000)) >> 12;
    *autocorrplatsumstart = (localVal & ((uint32_t)0x000001FF)) >> 0;
}

/**
 * @brief Returns the current value of the AutoCorrPlatSumEnd field in the TDFOCTRL1 register.
 *
 * The TDFOCTRL1 register will be read and the AutoCorrPlatSumEnd field's value will be returned.
 *
 * @return The current value of the AutoCorrPlatSumEnd field in the TDFOCTRL1 register.
 */
__INLINE uint16_t mdm_auto_corr_plat_sum_end_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x001FF000)) >> 12);
}

/**
 * @brief Sets the AutoCorrPlatSumEnd field of the TDFOCTRL1 register.
 *
 * The TDFOCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] autocorrplatsumend - The value to set the field to.
 */
__INLINE void mdm_auto_corr_plat_sum_end_setf(uint16_t autocorrplatsumend)
{
    ASSERT_ERR((((uint32_t)autocorrplatsumend << 12) & ~((uint32_t)0x001FF000)) == 0);
    REG_PL_WR(MDM_TDFOCTRL1_ADDR, (REG_PL_RD(MDM_TDFOCTRL1_ADDR) & ~((uint32_t)0x001FF000)) | ((uint32_t)autocorrplatsumend << 12));
}

/**
 * @brief Returns the current value of the AutoCorrPlatSumStart field in the TDFOCTRL1 register.
 *
 * The TDFOCTRL1 register will be read and the AutoCorrPlatSumStart field's value will be returned.
 *
 * @return The current value of the AutoCorrPlatSumStart field in the TDFOCTRL1 register.
 */
__INLINE uint16_t mdm_auto_corr_plat_sum_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x000001FF)) >> 0);
}

/**
 * @brief Sets the AutoCorrPlatSumStart field of the TDFOCTRL1 register.
 *
 * The TDFOCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] autocorrplatsumstart - The value to set the field to.
 */
__INLINE void mdm_auto_corr_plat_sum_start_setf(uint16_t autocorrplatsumstart)
{
    ASSERT_ERR((((uint32_t)autocorrplatsumstart << 0) & ~((uint32_t)0x000001FF)) == 0);
    REG_PL_WR(MDM_TDFOCTRL1_ADDR, (REG_PL_RD(MDM_TDFOCTRL1_ADDR) & ~((uint32_t)0x000001FF)) | ((uint32_t)autocorrplatsumstart << 0));
}

/// @}

/**
 * @name TDFOCTRL2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  28:20     AutoCorrPhSumEnd   0x4C
 *  18:10   AutoCorrPhSumStart   0x1C
 *  08:00   AutoCorrPlatFallSearchStart   0x44
 * </pre>
 *
 * @{
 */

/// Address of the TDFOCTRL2 register
#define MDM_TDFOCTRL2_ADDR   0x01000874
/// Offset of the TDFOCTRL2 register from the base address
#define MDM_TDFOCTRL2_OFFSET 0x00000874
/// Index of the TDFOCTRL2 register
#define MDM_TDFOCTRL2_INDEX  0x0000021D
/// Reset value of the TDFOCTRL2 register
#define MDM_TDFOCTRL2_RESET  0x04C07044

/**
 * @brief Returns the current value of the TDFOCTRL2 register.
 * The TDFOCTRL2 register will be read and its value returned.
 * @return The current value of the TDFOCTRL2 register.
 */
__INLINE uint32_t mdm_tdfoctrl2_get(void)
{
    return REG_PL_RD(MDM_TDFOCTRL2_ADDR);
}

/**
 * @brief Sets the TDFOCTRL2 register to a value.
 * The TDFOCTRL2 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_tdfoctrl2_set(uint32_t value)
{
    REG_PL_WR(MDM_TDFOCTRL2_ADDR, value);
}

// field definitions
/// AUTO_CORR_PH_SUM_END field mask
#define MDM_AUTO_CORR_PH_SUM_END_MASK               ((uint32_t)0x1FF00000)
/// AUTO_CORR_PH_SUM_END field LSB position
#define MDM_AUTO_CORR_PH_SUM_END_LSB                20
/// AUTO_CORR_PH_SUM_END field width
#define MDM_AUTO_CORR_PH_SUM_END_WIDTH              ((uint32_t)0x00000009)
/// AUTO_CORR_PH_SUM_START field mask
#define MDM_AUTO_CORR_PH_SUM_START_MASK             ((uint32_t)0x0007FC00)
/// AUTO_CORR_PH_SUM_START field LSB position
#define MDM_AUTO_CORR_PH_SUM_START_LSB              10
/// AUTO_CORR_PH_SUM_START field width
#define MDM_AUTO_CORR_PH_SUM_START_WIDTH            ((uint32_t)0x00000009)
/// AUTO_CORR_PLAT_FALL_SEARCH_START field mask
#define MDM_AUTO_CORR_PLAT_FALL_SEARCH_START_MASK   ((uint32_t)0x000001FF)
/// AUTO_CORR_PLAT_FALL_SEARCH_START field LSB position
#define MDM_AUTO_CORR_PLAT_FALL_SEARCH_START_LSB    0
/// AUTO_CORR_PLAT_FALL_SEARCH_START field width
#define MDM_AUTO_CORR_PLAT_FALL_SEARCH_START_WIDTH  ((uint32_t)0x00000009)

/// AUTO_CORR_PH_SUM_END field reset value
#define MDM_AUTO_CORR_PH_SUM_END_RST                0x4C
/// AUTO_CORR_PH_SUM_START field reset value
#define MDM_AUTO_CORR_PH_SUM_START_RST              0x1C
/// AUTO_CORR_PLAT_FALL_SEARCH_START field reset value
#define MDM_AUTO_CORR_PLAT_FALL_SEARCH_START_RST    0x44

/**
 * @brief Constructs a value for the TDFOCTRL2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] autocorrphsumend - The value to use for the AutoCorrPhSumEnd field.
 * @param[in] autocorrphsumstart - The value to use for the AutoCorrPhSumStart field.
 * @param[in] autocorrplatfallsearchstart - The value to use for the AutoCorrPlatFallSearchStart field.
 */
__INLINE void mdm_tdfoctrl2_pack(uint16_t autocorrphsumend, uint16_t autocorrphsumstart, uint16_t autocorrplatfallsearchstart)
{
    ASSERT_ERR((((uint32_t)autocorrphsumend << 20) & ~((uint32_t)0x1FF00000)) == 0);
    ASSERT_ERR((((uint32_t)autocorrphsumstart << 10) & ~((uint32_t)0x0007FC00)) == 0);
    ASSERT_ERR((((uint32_t)autocorrplatfallsearchstart << 0) & ~((uint32_t)0x000001FF)) == 0);
    REG_PL_WR(MDM_TDFOCTRL2_ADDR,  ((uint32_t)autocorrphsumend << 20) | ((uint32_t)autocorrphsumstart << 10) | ((uint32_t)autocorrplatfallsearchstart << 0));
}

/**
 * @brief Unpacks TDFOCTRL2's fields from current value of the TDFOCTRL2 register.
 *
 * Reads the TDFOCTRL2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] autocorrphsumend - Will be populated with the current value of this field from the register.
 * @param[out] autocorrphsumstart - Will be populated with the current value of this field from the register.
 * @param[out] autocorrplatfallsearchstart - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_tdfoctrl2_unpack(uint16_t *autocorrphsumend, uint16_t *autocorrphsumstart, uint16_t *autocorrplatfallsearchstart)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL2_ADDR);

    *autocorrphsumend = (localVal & ((uint32_t)0x1FF00000)) >> 20;
    *autocorrphsumstart = (localVal & ((uint32_t)0x0007FC00)) >> 10;
    *autocorrplatfallsearchstart = (localVal & ((uint32_t)0x000001FF)) >> 0;
}

/**
 * @brief Returns the current value of the AutoCorrPhSumEnd field in the TDFOCTRL2 register.
 *
 * The TDFOCTRL2 register will be read and the AutoCorrPhSumEnd field's value will be returned.
 *
 * @return The current value of the AutoCorrPhSumEnd field in the TDFOCTRL2 register.
 */
__INLINE uint16_t mdm_auto_corr_ph_sum_end_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL2_ADDR);
    return ((localVal & ((uint32_t)0x1FF00000)) >> 20);
}

/**
 * @brief Sets the AutoCorrPhSumEnd field of the TDFOCTRL2 register.
 *
 * The TDFOCTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] autocorrphsumend - The value to set the field to.
 */
__INLINE void mdm_auto_corr_ph_sum_end_setf(uint16_t autocorrphsumend)
{
    ASSERT_ERR((((uint32_t)autocorrphsumend << 20) & ~((uint32_t)0x1FF00000)) == 0);
    REG_PL_WR(MDM_TDFOCTRL2_ADDR, (REG_PL_RD(MDM_TDFOCTRL2_ADDR) & ~((uint32_t)0x1FF00000)) | ((uint32_t)autocorrphsumend << 20));
}

/**
 * @brief Returns the current value of the AutoCorrPhSumStart field in the TDFOCTRL2 register.
 *
 * The TDFOCTRL2 register will be read and the AutoCorrPhSumStart field's value will be returned.
 *
 * @return The current value of the AutoCorrPhSumStart field in the TDFOCTRL2 register.
 */
__INLINE uint16_t mdm_auto_corr_ph_sum_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL2_ADDR);
    return ((localVal & ((uint32_t)0x0007FC00)) >> 10);
}

/**
 * @brief Sets the AutoCorrPhSumStart field of the TDFOCTRL2 register.
 *
 * The TDFOCTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] autocorrphsumstart - The value to set the field to.
 */
__INLINE void mdm_auto_corr_ph_sum_start_setf(uint16_t autocorrphsumstart)
{
    ASSERT_ERR((((uint32_t)autocorrphsumstart << 10) & ~((uint32_t)0x0007FC00)) == 0);
    REG_PL_WR(MDM_TDFOCTRL2_ADDR, (REG_PL_RD(MDM_TDFOCTRL2_ADDR) & ~((uint32_t)0x0007FC00)) | ((uint32_t)autocorrphsumstart << 10));
}

/**
 * @brief Returns the current value of the AutoCorrPlatFallSearchStart field in the TDFOCTRL2 register.
 *
 * The TDFOCTRL2 register will be read and the AutoCorrPlatFallSearchStart field's value will be returned.
 *
 * @return The current value of the AutoCorrPlatFallSearchStart field in the TDFOCTRL2 register.
 */
__INLINE uint16_t mdm_auto_corr_plat_fall_search_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL2_ADDR);
    return ((localVal & ((uint32_t)0x000001FF)) >> 0);
}

/**
 * @brief Sets the AutoCorrPlatFallSearchStart field of the TDFOCTRL2 register.
 *
 * The TDFOCTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] autocorrplatfallsearchstart - The value to set the field to.
 */
__INLINE void mdm_auto_corr_plat_fall_search_start_setf(uint16_t autocorrplatfallsearchstart)
{
    ASSERT_ERR((((uint32_t)autocorrplatfallsearchstart << 0) & ~((uint32_t)0x000001FF)) == 0);
    REG_PL_WR(MDM_TDFOCTRL2_ADDR, (REG_PL_RD(MDM_TDFOCTRL2_ADDR) & ~((uint32_t)0x000001FF)) | ((uint32_t)autocorrplatfallsearchstart << 0));
}

/// @}

/**
 * @name TDFOCTRL3 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  23:12   TDFOPlatFallTimeOut   0x1F4
 *  11:04     TDFOSNRThreshold   0xC
 *  03:00    AutoCorrFallCount   0x1
 * </pre>
 *
 * @{
 */

/// Address of the TDFOCTRL3 register
#define MDM_TDFOCTRL3_ADDR   0x01000878
/// Offset of the TDFOCTRL3 register from the base address
#define MDM_TDFOCTRL3_OFFSET 0x00000878
/// Index of the TDFOCTRL3 register
#define MDM_TDFOCTRL3_INDEX  0x0000021E
/// Reset value of the TDFOCTRL3 register
#define MDM_TDFOCTRL3_RESET  0x001F40C1

/**
 * @brief Returns the current value of the TDFOCTRL3 register.
 * The TDFOCTRL3 register will be read and its value returned.
 * @return The current value of the TDFOCTRL3 register.
 */
__INLINE uint32_t mdm_tdfoctrl3_get(void)
{
    return REG_PL_RD(MDM_TDFOCTRL3_ADDR);
}

/**
 * @brief Sets the TDFOCTRL3 register to a value.
 * The TDFOCTRL3 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_tdfoctrl3_set(uint32_t value)
{
    REG_PL_WR(MDM_TDFOCTRL3_ADDR, value);
}

// field definitions
/// TDFO_PLAT_FALL_TIME_OUT field mask
#define MDM_TDFO_PLAT_FALL_TIME_OUT_MASK   ((uint32_t)0x00FFF000)
/// TDFO_PLAT_FALL_TIME_OUT field LSB position
#define MDM_TDFO_PLAT_FALL_TIME_OUT_LSB    12
/// TDFO_PLAT_FALL_TIME_OUT field width
#define MDM_TDFO_PLAT_FALL_TIME_OUT_WIDTH  ((uint32_t)0x0000000C)
/// TDFOSNR_THRESHOLD field mask
#define MDM_TDFOSNR_THRESHOLD_MASK         ((uint32_t)0x00000FF0)
/// TDFOSNR_THRESHOLD field LSB position
#define MDM_TDFOSNR_THRESHOLD_LSB          4
/// TDFOSNR_THRESHOLD field width
#define MDM_TDFOSNR_THRESHOLD_WIDTH        ((uint32_t)0x00000008)
/// AUTO_CORR_FALL_COUNT field mask
#define MDM_AUTO_CORR_FALL_COUNT_MASK      ((uint32_t)0x0000000F)
/// AUTO_CORR_FALL_COUNT field LSB position
#define MDM_AUTO_CORR_FALL_COUNT_LSB       0
/// AUTO_CORR_FALL_COUNT field width
#define MDM_AUTO_CORR_FALL_COUNT_WIDTH     ((uint32_t)0x00000004)

/// TDFO_PLAT_FALL_TIME_OUT field reset value
#define MDM_TDFO_PLAT_FALL_TIME_OUT_RST    0x1F4
/// TDFOSNR_THRESHOLD field reset value
#define MDM_TDFOSNR_THRESHOLD_RST          0xC
/// AUTO_CORR_FALL_COUNT field reset value
#define MDM_AUTO_CORR_FALL_COUNT_RST       0x1

/**
 * @brief Constructs a value for the TDFOCTRL3 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] tdfoplatfalltimeout - The value to use for the TDFOPlatFallTimeOut field.
 * @param[in] tdfosnrthreshold - The value to use for the TDFOSNRThreshold field.
 * @param[in] autocorrfallcount - The value to use for the AutoCorrFallCount field.
 */
__INLINE void mdm_tdfoctrl3_pack(uint16_t tdfoplatfalltimeout, uint8_t tdfosnrthreshold, uint8_t autocorrfallcount)
{
    ASSERT_ERR((((uint32_t)tdfoplatfalltimeout << 12) & ~((uint32_t)0x00FFF000)) == 0);
    ASSERT_ERR((((uint32_t)tdfosnrthreshold << 4) & ~((uint32_t)0x00000FF0)) == 0);
    ASSERT_ERR((((uint32_t)autocorrfallcount << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_TDFOCTRL3_ADDR,  ((uint32_t)tdfoplatfalltimeout << 12) | ((uint32_t)tdfosnrthreshold << 4) | ((uint32_t)autocorrfallcount << 0));
}

/**
 * @brief Unpacks TDFOCTRL3's fields from current value of the TDFOCTRL3 register.
 *
 * Reads the TDFOCTRL3 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] tdfoplatfalltimeout - Will be populated with the current value of this field from the register.
 * @param[out] tdfosnrthreshold - Will be populated with the current value of this field from the register.
 * @param[out] autocorrfallcount - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_tdfoctrl3_unpack(uint16_t *tdfoplatfalltimeout, uint8_t *tdfosnrthreshold, uint8_t *autocorrfallcount)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL3_ADDR);

    *tdfoplatfalltimeout = (localVal & ((uint32_t)0x00FFF000)) >> 12;
    *tdfosnrthreshold = (localVal & ((uint32_t)0x00000FF0)) >> 4;
    *autocorrfallcount = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the TDFOPlatFallTimeOut field in the TDFOCTRL3 register.
 *
 * The TDFOCTRL3 register will be read and the TDFOPlatFallTimeOut field's value will be returned.
 *
 * @return The current value of the TDFOPlatFallTimeOut field in the TDFOCTRL3 register.
 */
__INLINE uint16_t mdm_tdfo_plat_fall_time_out_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00FFF000)) >> 12);
}

/**
 * @brief Sets the TDFOPlatFallTimeOut field of the TDFOCTRL3 register.
 *
 * The TDFOCTRL3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdfoplatfalltimeout - The value to set the field to.
 */
__INLINE void mdm_tdfo_plat_fall_time_out_setf(uint16_t tdfoplatfalltimeout)
{
    ASSERT_ERR((((uint32_t)tdfoplatfalltimeout << 12) & ~((uint32_t)0x00FFF000)) == 0);
    REG_PL_WR(MDM_TDFOCTRL3_ADDR, (REG_PL_RD(MDM_TDFOCTRL3_ADDR) & ~((uint32_t)0x00FFF000)) | ((uint32_t)tdfoplatfalltimeout << 12));
}

/**
 * @brief Returns the current value of the TDFOSNRThreshold field in the TDFOCTRL3 register.
 *
 * The TDFOCTRL3 register will be read and the TDFOSNRThreshold field's value will be returned.
 *
 * @return The current value of the TDFOSNRThreshold field in the TDFOCTRL3 register.
 */
__INLINE uint8_t mdm_tdfosnr_threshold_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000FF0)) >> 4);
}

/**
 * @brief Sets the TDFOSNRThreshold field of the TDFOCTRL3 register.
 *
 * The TDFOCTRL3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdfosnrthreshold - The value to set the field to.
 */
__INLINE void mdm_tdfosnr_threshold_setf(uint8_t tdfosnrthreshold)
{
    ASSERT_ERR((((uint32_t)tdfosnrthreshold << 4) & ~((uint32_t)0x00000FF0)) == 0);
    REG_PL_WR(MDM_TDFOCTRL3_ADDR, (REG_PL_RD(MDM_TDFOCTRL3_ADDR) & ~((uint32_t)0x00000FF0)) | ((uint32_t)tdfosnrthreshold << 4));
}

/**
 * @brief Returns the current value of the AutoCorrFallCount field in the TDFOCTRL3 register.
 *
 * The TDFOCTRL3 register will be read and the AutoCorrFallCount field's value will be returned.
 *
 * @return The current value of the AutoCorrFallCount field in the TDFOCTRL3 register.
 */
__INLINE uint8_t mdm_auto_corr_fall_count_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL3_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the AutoCorrFallCount field of the TDFOCTRL3 register.
 *
 * The TDFOCTRL3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] autocorrfallcount - The value to set the field to.
 */
__INLINE void mdm_auto_corr_fall_count_setf(uint8_t autocorrfallcount)
{
    ASSERT_ERR((((uint32_t)autocorrfallcount << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_TDFOCTRL3_ADDR, (REG_PL_RD(MDM_TDFOCTRL3_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)autocorrfallcount << 0));
}

/// @}

/**
 * @name CLKGATEFCTRL0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31        PHYTXCLKFORCE   0
 *     30        PHYRXCLKFORCE   0
 *     29       AGCMEMCLKFORCE   0
 *     28          AGCCLKFORCE   0
 *     27          FE3CLKFORCE   0
 *     26          FE2CLKFORCE   0
 *     25          FE1CLKFORCE   0
 *     24          FE0CLKFORCE   0
 *     23       STOCPECLKFORCE   0
 *     22          EQUCLKFORCE   0
 *     21      TDCOMP3CLKFORCE   0
 *     20      TDCOMP2CLKFORCE   0
 *     19      TDCOMP1CLKFORCE   0
 *     18      TDCOMP0CLKFORCE   0
 *     17      TDFOESTCLKFORCE   0
 *     16          TBECLKFORCE   0
 *     15          VTBCLKFORCE   0
 *     14      FFT3MEMCLKFORCE   0
 *     13         FFT3CLKFORCE   0
 *     12      FFT2MEMCLKFORCE   0
 *     11         FFT2CLKFORCE   0
 *     10      FFT1MEMCLKFORCE   0
 *     09         FFT1CLKFORCE   0
 *     08      FFT0MEMCLKFORCE   0
 *     07         FFT0CLKFORCE   0
 *     06        CHESTCLKFORCE   0
 *     05         GMEMCLKFORCE   0
 *     04         HMEMCLKFORCE   0
 *     03         QMEMCLKFORCE   0
 *     02     INTLVMEMCLKFORCE   0
 *     01     INTLVVTBCLKFORCE   0
 *     00     INTLVPHYCLKFORCE   0
 * </pre>
 *
 * @{
 */

/// Address of the CLKGATEFCTRL0 register
#define MDM_CLKGATEFCTRL0_ADDR   0x0100087C
/// Offset of the CLKGATEFCTRL0 register from the base address
#define MDM_CLKGATEFCTRL0_OFFSET 0x0000087C
/// Index of the CLKGATEFCTRL0 register
#define MDM_CLKGATEFCTRL0_INDEX  0x0000021F
/// Reset value of the CLKGATEFCTRL0 register
#define MDM_CLKGATEFCTRL0_RESET  0x00000000

/**
 * @brief Returns the current value of the CLKGATEFCTRL0 register.
 * The CLKGATEFCTRL0 register will be read and its value returned.
 * @return The current value of the CLKGATEFCTRL0 register.
 */
__INLINE uint32_t mdm_clkgatefctrl0_get(void)
{
    return REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
}

/**
 * @brief Sets the CLKGATEFCTRL0 register to a value.
 * The CLKGATEFCTRL0 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_clkgatefctrl0_set(uint32_t value)
{
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, value);
}

// field definitions
/// PHYTXCLKFORCE field bit
#define MDM_PHYTXCLKFORCE_BIT       ((uint32_t)0x80000000)
/// PHYTXCLKFORCE field position
#define MDM_PHYTXCLKFORCE_POS       31
/// PHYRXCLKFORCE field bit
#define MDM_PHYRXCLKFORCE_BIT       ((uint32_t)0x40000000)
/// PHYRXCLKFORCE field position
#define MDM_PHYRXCLKFORCE_POS       30
/// AGCMEMCLKFORCE field bit
#define MDM_AGCMEMCLKFORCE_BIT      ((uint32_t)0x20000000)
/// AGCMEMCLKFORCE field position
#define MDM_AGCMEMCLKFORCE_POS      29
/// AGCCLKFORCE field bit
#define MDM_AGCCLKFORCE_BIT         ((uint32_t)0x10000000)
/// AGCCLKFORCE field position
#define MDM_AGCCLKFORCE_POS         28
/// FE3CLKFORCE field bit
#define MDM_FE3CLKFORCE_BIT         ((uint32_t)0x08000000)
/// FE3CLKFORCE field position
#define MDM_FE3CLKFORCE_POS         27
/// FE2CLKFORCE field bit
#define MDM_FE2CLKFORCE_BIT         ((uint32_t)0x04000000)
/// FE2CLKFORCE field position
#define MDM_FE2CLKFORCE_POS         26
/// FE1CLKFORCE field bit
#define MDM_FE1CLKFORCE_BIT         ((uint32_t)0x02000000)
/// FE1CLKFORCE field position
#define MDM_FE1CLKFORCE_POS         25
/// FE0CLKFORCE field bit
#define MDM_FE0CLKFORCE_BIT         ((uint32_t)0x01000000)
/// FE0CLKFORCE field position
#define MDM_FE0CLKFORCE_POS         24
/// STOCPECLKFORCE field bit
#define MDM_STOCPECLKFORCE_BIT      ((uint32_t)0x00800000)
/// STOCPECLKFORCE field position
#define MDM_STOCPECLKFORCE_POS      23
/// EQUCLKFORCE field bit
#define MDM_EQUCLKFORCE_BIT         ((uint32_t)0x00400000)
/// EQUCLKFORCE field position
#define MDM_EQUCLKFORCE_POS         22
/// TDCOMP3CLKFORCE field bit
#define MDM_TDCOMP3CLKFORCE_BIT     ((uint32_t)0x00200000)
/// TDCOMP3CLKFORCE field position
#define MDM_TDCOMP3CLKFORCE_POS     21
/// TDCOMP2CLKFORCE field bit
#define MDM_TDCOMP2CLKFORCE_BIT     ((uint32_t)0x00100000)
/// TDCOMP2CLKFORCE field position
#define MDM_TDCOMP2CLKFORCE_POS     20
/// TDCOMP1CLKFORCE field bit
#define MDM_TDCOMP1CLKFORCE_BIT     ((uint32_t)0x00080000)
/// TDCOMP1CLKFORCE field position
#define MDM_TDCOMP1CLKFORCE_POS     19
/// TDCOMP0CLKFORCE field bit
#define MDM_TDCOMP0CLKFORCE_BIT     ((uint32_t)0x00040000)
/// TDCOMP0CLKFORCE field position
#define MDM_TDCOMP0CLKFORCE_POS     18
/// TDFOESTCLKFORCE field bit
#define MDM_TDFOESTCLKFORCE_BIT     ((uint32_t)0x00020000)
/// TDFOESTCLKFORCE field position
#define MDM_TDFOESTCLKFORCE_POS     17
/// TBECLKFORCE field bit
#define MDM_TBECLKFORCE_BIT         ((uint32_t)0x00010000)
/// TBECLKFORCE field position
#define MDM_TBECLKFORCE_POS         16
/// VTBCLKFORCE field bit
#define MDM_VTBCLKFORCE_BIT         ((uint32_t)0x00008000)
/// VTBCLKFORCE field position
#define MDM_VTBCLKFORCE_POS         15
/// FFT3MEMCLKFORCE field bit
#define MDM_FFT3MEMCLKFORCE_BIT     ((uint32_t)0x00004000)
/// FFT3MEMCLKFORCE field position
#define MDM_FFT3MEMCLKFORCE_POS     14
/// FFT3CLKFORCE field bit
#define MDM_FFT3CLKFORCE_BIT        ((uint32_t)0x00002000)
/// FFT3CLKFORCE field position
#define MDM_FFT3CLKFORCE_POS        13
/// FFT2MEMCLKFORCE field bit
#define MDM_FFT2MEMCLKFORCE_BIT     ((uint32_t)0x00001000)
/// FFT2MEMCLKFORCE field position
#define MDM_FFT2MEMCLKFORCE_POS     12
/// FFT2CLKFORCE field bit
#define MDM_FFT2CLKFORCE_BIT        ((uint32_t)0x00000800)
/// FFT2CLKFORCE field position
#define MDM_FFT2CLKFORCE_POS        11
/// FFT1MEMCLKFORCE field bit
#define MDM_FFT1MEMCLKFORCE_BIT     ((uint32_t)0x00000400)
/// FFT1MEMCLKFORCE field position
#define MDM_FFT1MEMCLKFORCE_POS     10
/// FFT1CLKFORCE field bit
#define MDM_FFT1CLKFORCE_BIT        ((uint32_t)0x00000200)
/// FFT1CLKFORCE field position
#define MDM_FFT1CLKFORCE_POS        9
/// FFT0MEMCLKFORCE field bit
#define MDM_FFT0MEMCLKFORCE_BIT     ((uint32_t)0x00000100)
/// FFT0MEMCLKFORCE field position
#define MDM_FFT0MEMCLKFORCE_POS     8
/// FFT0CLKFORCE field bit
#define MDM_FFT0CLKFORCE_BIT        ((uint32_t)0x00000080)
/// FFT0CLKFORCE field position
#define MDM_FFT0CLKFORCE_POS        7
/// CHESTCLKFORCE field bit
#define MDM_CHESTCLKFORCE_BIT       ((uint32_t)0x00000040)
/// CHESTCLKFORCE field position
#define MDM_CHESTCLKFORCE_POS       6
/// GMEMCLKFORCE field bit
#define MDM_GMEMCLKFORCE_BIT        ((uint32_t)0x00000020)
/// GMEMCLKFORCE field position
#define MDM_GMEMCLKFORCE_POS        5
/// HMEMCLKFORCE field bit
#define MDM_HMEMCLKFORCE_BIT        ((uint32_t)0x00000010)
/// HMEMCLKFORCE field position
#define MDM_HMEMCLKFORCE_POS        4
/// QMEMCLKFORCE field bit
#define MDM_QMEMCLKFORCE_BIT        ((uint32_t)0x00000008)
/// QMEMCLKFORCE field position
#define MDM_QMEMCLKFORCE_POS        3
/// INTLVMEMCLKFORCE field bit
#define MDM_INTLVMEMCLKFORCE_BIT    ((uint32_t)0x00000004)
/// INTLVMEMCLKFORCE field position
#define MDM_INTLVMEMCLKFORCE_POS    2
/// INTLVVTBCLKFORCE field bit
#define MDM_INTLVVTBCLKFORCE_BIT    ((uint32_t)0x00000002)
/// INTLVVTBCLKFORCE field position
#define MDM_INTLVVTBCLKFORCE_POS    1
/// INTLVPHYCLKFORCE field bit
#define MDM_INTLVPHYCLKFORCE_BIT    ((uint32_t)0x00000001)
/// INTLVPHYCLKFORCE field position
#define MDM_INTLVPHYCLKFORCE_POS    0

/// PHYTXCLKFORCE field reset value
#define MDM_PHYTXCLKFORCE_RST       0x0
/// PHYRXCLKFORCE field reset value
#define MDM_PHYRXCLKFORCE_RST       0x0
/// AGCMEMCLKFORCE field reset value
#define MDM_AGCMEMCLKFORCE_RST      0x0
/// AGCCLKFORCE field reset value
#define MDM_AGCCLKFORCE_RST         0x0
/// FE3CLKFORCE field reset value
#define MDM_FE3CLKFORCE_RST         0x0
/// FE2CLKFORCE field reset value
#define MDM_FE2CLKFORCE_RST         0x0
/// FE1CLKFORCE field reset value
#define MDM_FE1CLKFORCE_RST         0x0
/// FE0CLKFORCE field reset value
#define MDM_FE0CLKFORCE_RST         0x0
/// STOCPECLKFORCE field reset value
#define MDM_STOCPECLKFORCE_RST      0x0
/// EQUCLKFORCE field reset value
#define MDM_EQUCLKFORCE_RST         0x0
/// TDCOMP3CLKFORCE field reset value
#define MDM_TDCOMP3CLKFORCE_RST     0x0
/// TDCOMP2CLKFORCE field reset value
#define MDM_TDCOMP2CLKFORCE_RST     0x0
/// TDCOMP1CLKFORCE field reset value
#define MDM_TDCOMP1CLKFORCE_RST     0x0
/// TDCOMP0CLKFORCE field reset value
#define MDM_TDCOMP0CLKFORCE_RST     0x0
/// TDFOESTCLKFORCE field reset value
#define MDM_TDFOESTCLKFORCE_RST     0x0
/// TBECLKFORCE field reset value
#define MDM_TBECLKFORCE_RST         0x0
/// VTBCLKFORCE field reset value
#define MDM_VTBCLKFORCE_RST         0x0
/// FFT3MEMCLKFORCE field reset value
#define MDM_FFT3MEMCLKFORCE_RST     0x0
/// FFT3CLKFORCE field reset value
#define MDM_FFT3CLKFORCE_RST        0x0
/// FFT2MEMCLKFORCE field reset value
#define MDM_FFT2MEMCLKFORCE_RST     0x0
/// FFT2CLKFORCE field reset value
#define MDM_FFT2CLKFORCE_RST        0x0
/// FFT1MEMCLKFORCE field reset value
#define MDM_FFT1MEMCLKFORCE_RST     0x0
/// FFT1CLKFORCE field reset value
#define MDM_FFT1CLKFORCE_RST        0x0
/// FFT0MEMCLKFORCE field reset value
#define MDM_FFT0MEMCLKFORCE_RST     0x0
/// FFT0CLKFORCE field reset value
#define MDM_FFT0CLKFORCE_RST        0x0
/// CHESTCLKFORCE field reset value
#define MDM_CHESTCLKFORCE_RST       0x0
/// GMEMCLKFORCE field reset value
#define MDM_GMEMCLKFORCE_RST        0x0
/// HMEMCLKFORCE field reset value
#define MDM_HMEMCLKFORCE_RST        0x0
/// QMEMCLKFORCE field reset value
#define MDM_QMEMCLKFORCE_RST        0x0
/// INTLVMEMCLKFORCE field reset value
#define MDM_INTLVMEMCLKFORCE_RST    0x0
/// INTLVVTBCLKFORCE field reset value
#define MDM_INTLVVTBCLKFORCE_RST    0x0
/// INTLVPHYCLKFORCE field reset value
#define MDM_INTLVPHYCLKFORCE_RST    0x0

/**
 * @brief Constructs a value for the CLKGATEFCTRL0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] phytxclkforce - The value to use for the PHYTXCLKFORCE field.
 * @param[in] phyrxclkforce - The value to use for the PHYRXCLKFORCE field.
 * @param[in] agcmemclkforce - The value to use for the AGCMEMCLKFORCE field.
 * @param[in] agcclkforce - The value to use for the AGCCLKFORCE field.
 * @param[in] fe3clkforce - The value to use for the FE3CLKFORCE field.
 * @param[in] fe2clkforce - The value to use for the FE2CLKFORCE field.
 * @param[in] fe1clkforce - The value to use for the FE1CLKFORCE field.
 * @param[in] fe0clkforce - The value to use for the FE0CLKFORCE field.
 * @param[in] stocpeclkforce - The value to use for the STOCPECLKFORCE field.
 * @param[in] equclkforce - The value to use for the EQUCLKFORCE field.
 * @param[in] tdcomp3clkforce - The value to use for the TDCOMP3CLKFORCE field.
 * @param[in] tdcomp2clkforce - The value to use for the TDCOMP2CLKFORCE field.
 * @param[in] tdcomp1clkforce - The value to use for the TDCOMP1CLKFORCE field.
 * @param[in] tdcomp0clkforce - The value to use for the TDCOMP0CLKFORCE field.
 * @param[in] tdfoestclkforce - The value to use for the TDFOESTCLKFORCE field.
 * @param[in] tbeclkforce - The value to use for the TBECLKFORCE field.
 * @param[in] vtbclkforce - The value to use for the VTBCLKFORCE field.
 * @param[in] fft3memclkforce - The value to use for the FFT3MEMCLKFORCE field.
 * @param[in] fft3clkforce - The value to use for the FFT3CLKFORCE field.
 * @param[in] fft2memclkforce - The value to use for the FFT2MEMCLKFORCE field.
 * @param[in] fft2clkforce - The value to use for the FFT2CLKFORCE field.
 * @param[in] fft1memclkforce - The value to use for the FFT1MEMCLKFORCE field.
 * @param[in] fft1clkforce - The value to use for the FFT1CLKFORCE field.
 * @param[in] fft0memclkforce - The value to use for the FFT0MEMCLKFORCE field.
 * @param[in] fft0clkforce - The value to use for the FFT0CLKFORCE field.
 * @param[in] chestclkforce - The value to use for the CHESTCLKFORCE field.
 * @param[in] gmemclkforce - The value to use for the GMEMCLKFORCE field.
 * @param[in] hmemclkforce - The value to use for the HMEMCLKFORCE field.
 * @param[in] qmemclkforce - The value to use for the QMEMCLKFORCE field.
 * @param[in] intlvmemclkforce - The value to use for the INTLVMEMCLKFORCE field.
 * @param[in] intlvvtbclkforce - The value to use for the INTLVVTBCLKFORCE field.
 * @param[in] intlvphyclkforce - The value to use for the INTLVPHYCLKFORCE field.
 */
__INLINE void mdm_clkgatefctrl0_pack(uint8_t phytxclkforce, uint8_t phyrxclkforce, uint8_t agcmemclkforce, uint8_t agcclkforce, uint8_t fe3clkforce, uint8_t fe2clkforce, uint8_t fe1clkforce, uint8_t fe0clkforce, uint8_t stocpeclkforce, uint8_t equclkforce, uint8_t tdcomp3clkforce, uint8_t tdcomp2clkforce, uint8_t tdcomp1clkforce, uint8_t tdcomp0clkforce, uint8_t tdfoestclkforce, uint8_t tbeclkforce, uint8_t vtbclkforce, uint8_t fft3memclkforce, uint8_t fft3clkforce, uint8_t fft2memclkforce, uint8_t fft2clkforce, uint8_t fft1memclkforce, uint8_t fft1clkforce, uint8_t fft0memclkforce, uint8_t fft0clkforce, uint8_t chestclkforce, uint8_t gmemclkforce, uint8_t hmemclkforce, uint8_t qmemclkforce, uint8_t intlvmemclkforce, uint8_t intlvvtbclkforce, uint8_t intlvphyclkforce)
{
    ASSERT_ERR((((uint32_t)phytxclkforce << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)phyrxclkforce << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)agcmemclkforce << 29) & ~((uint32_t)0x20000000)) == 0);
    ASSERT_ERR((((uint32_t)agcclkforce << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)fe3clkforce << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)fe2clkforce << 26) & ~((uint32_t)0x04000000)) == 0);
    ASSERT_ERR((((uint32_t)fe1clkforce << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)fe0clkforce << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)stocpeclkforce << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)equclkforce << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)tdcomp3clkforce << 21) & ~((uint32_t)0x00200000)) == 0);
    ASSERT_ERR((((uint32_t)tdcomp2clkforce << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)tdcomp1clkforce << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)tdcomp0clkforce << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)tdfoestclkforce << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)tbeclkforce << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)vtbclkforce << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)fft3memclkforce << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)fft3clkforce << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)fft2memclkforce << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)fft2clkforce << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)fft1memclkforce << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)fft1clkforce << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)fft0memclkforce << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)fft0clkforce << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)chestclkforce << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)gmemclkforce << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)hmemclkforce << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)qmemclkforce << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)intlvmemclkforce << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)intlvvtbclkforce << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)intlvphyclkforce << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR,  ((uint32_t)phytxclkforce << 31) | ((uint32_t)phyrxclkforce << 30) | ((uint32_t)agcmemclkforce << 29) | ((uint32_t)agcclkforce << 28) | ((uint32_t)fe3clkforce << 27) | ((uint32_t)fe2clkforce << 26) | ((uint32_t)fe1clkforce << 25) | ((uint32_t)fe0clkforce << 24) | ((uint32_t)stocpeclkforce << 23) | ((uint32_t)equclkforce << 22) | ((uint32_t)tdcomp3clkforce << 21) | ((uint32_t)tdcomp2clkforce << 20) | ((uint32_t)tdcomp1clkforce << 19) | ((uint32_t)tdcomp0clkforce << 18) | ((uint32_t)tdfoestclkforce << 17) | ((uint32_t)tbeclkforce << 16) | ((uint32_t)vtbclkforce << 15) | ((uint32_t)fft3memclkforce << 14) | ((uint32_t)fft3clkforce << 13) | ((uint32_t)fft2memclkforce << 12) | ((uint32_t)fft2clkforce << 11) | ((uint32_t)fft1memclkforce << 10) | ((uint32_t)fft1clkforce << 9) | ((uint32_t)fft0memclkforce << 8) | ((uint32_t)fft0clkforce << 7) | ((uint32_t)chestclkforce << 6) | ((uint32_t)gmemclkforce << 5) | ((uint32_t)hmemclkforce << 4) | ((uint32_t)qmemclkforce << 3) | ((uint32_t)intlvmemclkforce << 2) | ((uint32_t)intlvvtbclkforce << 1) | ((uint32_t)intlvphyclkforce << 0));
}

/**
 * @brief Unpacks CLKGATEFCTRL0's fields from current value of the CLKGATEFCTRL0 register.
 *
 * Reads the CLKGATEFCTRL0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] phytxclkforce - Will be populated with the current value of this field from the register.
 * @param[out] phyrxclkforce - Will be populated with the current value of this field from the register.
 * @param[out] agcmemclkforce - Will be populated with the current value of this field from the register.
 * @param[out] agcclkforce - Will be populated with the current value of this field from the register.
 * @param[out] fe3clkforce - Will be populated with the current value of this field from the register.
 * @param[out] fe2clkforce - Will be populated with the current value of this field from the register.
 * @param[out] fe1clkforce - Will be populated with the current value of this field from the register.
 * @param[out] fe0clkforce - Will be populated with the current value of this field from the register.
 * @param[out] stocpeclkforce - Will be populated with the current value of this field from the register.
 * @param[out] equclkforce - Will be populated with the current value of this field from the register.
 * @param[out] tdcomp3clkforce - Will be populated with the current value of this field from the register.
 * @param[out] tdcomp2clkforce - Will be populated with the current value of this field from the register.
 * @param[out] tdcomp1clkforce - Will be populated with the current value of this field from the register.
 * @param[out] tdcomp0clkforce - Will be populated with the current value of this field from the register.
 * @param[out] tdfoestclkforce - Will be populated with the current value of this field from the register.
 * @param[out] tbeclkforce - Will be populated with the current value of this field from the register.
 * @param[out] vtbclkforce - Will be populated with the current value of this field from the register.
 * @param[out] fft3memclkforce - Will be populated with the current value of this field from the register.
 * @param[out] fft3clkforce - Will be populated with the current value of this field from the register.
 * @param[out] fft2memclkforce - Will be populated with the current value of this field from the register.
 * @param[out] fft2clkforce - Will be populated with the current value of this field from the register.
 * @param[out] fft1memclkforce - Will be populated with the current value of this field from the register.
 * @param[out] fft1clkforce - Will be populated with the current value of this field from the register.
 * @param[out] fft0memclkforce - Will be populated with the current value of this field from the register.
 * @param[out] fft0clkforce - Will be populated with the current value of this field from the register.
 * @param[out] chestclkforce - Will be populated with the current value of this field from the register.
 * @param[out] gmemclkforce - Will be populated with the current value of this field from the register.
 * @param[out] hmemclkforce - Will be populated with the current value of this field from the register.
 * @param[out] qmemclkforce - Will be populated with the current value of this field from the register.
 * @param[out] intlvmemclkforce - Will be populated with the current value of this field from the register.
 * @param[out] intlvvtbclkforce - Will be populated with the current value of this field from the register.
 * @param[out] intlvphyclkforce - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_clkgatefctrl0_unpack(uint8_t *phytxclkforce, uint8_t *phyrxclkforce, uint8_t *agcmemclkforce, uint8_t *agcclkforce, uint8_t *fe3clkforce, uint8_t *fe2clkforce, uint8_t *fe1clkforce, uint8_t *fe0clkforce, uint8_t *stocpeclkforce, uint8_t *equclkforce, uint8_t *tdcomp3clkforce, uint8_t *tdcomp2clkforce, uint8_t *tdcomp1clkforce, uint8_t *tdcomp0clkforce, uint8_t *tdfoestclkforce, uint8_t *tbeclkforce, uint8_t *vtbclkforce, uint8_t *fft3memclkforce, uint8_t *fft3clkforce, uint8_t *fft2memclkforce, uint8_t *fft2clkforce, uint8_t *fft1memclkforce, uint8_t *fft1clkforce, uint8_t *fft0memclkforce, uint8_t *fft0clkforce, uint8_t *chestclkforce, uint8_t *gmemclkforce, uint8_t *hmemclkforce, uint8_t *qmemclkforce, uint8_t *intlvmemclkforce, uint8_t *intlvvtbclkforce, uint8_t *intlvphyclkforce)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);

    *phytxclkforce = (localVal & ((uint32_t)0x80000000)) >> 31;
    *phyrxclkforce = (localVal & ((uint32_t)0x40000000)) >> 30;
    *agcmemclkforce = (localVal & ((uint32_t)0x20000000)) >> 29;
    *agcclkforce = (localVal & ((uint32_t)0x10000000)) >> 28;
    *fe3clkforce = (localVal & ((uint32_t)0x08000000)) >> 27;
    *fe2clkforce = (localVal & ((uint32_t)0x04000000)) >> 26;
    *fe1clkforce = (localVal & ((uint32_t)0x02000000)) >> 25;
    *fe0clkforce = (localVal & ((uint32_t)0x01000000)) >> 24;
    *stocpeclkforce = (localVal & ((uint32_t)0x00800000)) >> 23;
    *equclkforce = (localVal & ((uint32_t)0x00400000)) >> 22;
    *tdcomp3clkforce = (localVal & ((uint32_t)0x00200000)) >> 21;
    *tdcomp2clkforce = (localVal & ((uint32_t)0x00100000)) >> 20;
    *tdcomp1clkforce = (localVal & ((uint32_t)0x00080000)) >> 19;
    *tdcomp0clkforce = (localVal & ((uint32_t)0x00040000)) >> 18;
    *tdfoestclkforce = (localVal & ((uint32_t)0x00020000)) >> 17;
    *tbeclkforce = (localVal & ((uint32_t)0x00010000)) >> 16;
    *vtbclkforce = (localVal & ((uint32_t)0x00008000)) >> 15;
    *fft3memclkforce = (localVal & ((uint32_t)0x00004000)) >> 14;
    *fft3clkforce = (localVal & ((uint32_t)0x00002000)) >> 13;
    *fft2memclkforce = (localVal & ((uint32_t)0x00001000)) >> 12;
    *fft2clkforce = (localVal & ((uint32_t)0x00000800)) >> 11;
    *fft1memclkforce = (localVal & ((uint32_t)0x00000400)) >> 10;
    *fft1clkforce = (localVal & ((uint32_t)0x00000200)) >> 9;
    *fft0memclkforce = (localVal & ((uint32_t)0x00000100)) >> 8;
    *fft0clkforce = (localVal & ((uint32_t)0x00000080)) >> 7;
    *chestclkforce = (localVal & ((uint32_t)0x00000040)) >> 6;
    *gmemclkforce = (localVal & ((uint32_t)0x00000020)) >> 5;
    *hmemclkforce = (localVal & ((uint32_t)0x00000010)) >> 4;
    *qmemclkforce = (localVal & ((uint32_t)0x00000008)) >> 3;
    *intlvmemclkforce = (localVal & ((uint32_t)0x00000004)) >> 2;
    *intlvvtbclkforce = (localVal & ((uint32_t)0x00000002)) >> 1;
    *intlvphyclkforce = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the PHYTXCLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the PHYTXCLKFORCE field's value will be returned.
 *
 * @return The current value of the PHYTXCLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_phytxclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the PHYTXCLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] phytxclkforce - The value to set the field to.
 */
__INLINE void mdm_phytxclkforce_setf(uint8_t phytxclkforce)
{
    ASSERT_ERR((((uint32_t)phytxclkforce << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)phytxclkforce << 31));
}

/**
 * @brief Returns the current value of the PHYRXCLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the PHYRXCLKFORCE field's value will be returned.
 *
 * @return The current value of the PHYRXCLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_phyrxclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

/**
 * @brief Sets the PHYRXCLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] phyrxclkforce - The value to set the field to.
 */
__INLINE void mdm_phyrxclkforce_setf(uint8_t phyrxclkforce)
{
    ASSERT_ERR((((uint32_t)phyrxclkforce << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)phyrxclkforce << 30));
}

/**
 * @brief Returns the current value of the AGCMEMCLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the AGCMEMCLKFORCE field's value will be returned.
 *
 * @return The current value of the AGCMEMCLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_agcmemclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

/**
 * @brief Sets the AGCMEMCLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] agcmemclkforce - The value to set the field to.
 */
__INLINE void mdm_agcmemclkforce_setf(uint8_t agcmemclkforce)
{
    ASSERT_ERR((((uint32_t)agcmemclkforce << 29) & ~((uint32_t)0x20000000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x20000000)) | ((uint32_t)agcmemclkforce << 29));
}

/**
 * @brief Returns the current value of the AGCCLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the AGCCLKFORCE field's value will be returned.
 *
 * @return The current value of the AGCCLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_agcclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the AGCCLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] agcclkforce - The value to set the field to.
 */
__INLINE void mdm_agcclkforce_setf(uint8_t agcclkforce)
{
    ASSERT_ERR((((uint32_t)agcclkforce << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)agcclkforce << 28));
}

/**
 * @brief Returns the current value of the FE3CLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the FE3CLKFORCE field's value will be returned.
 *
 * @return The current value of the FE3CLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_fe3clkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Sets the FE3CLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fe3clkforce - The value to set the field to.
 */
__INLINE void mdm_fe3clkforce_setf(uint8_t fe3clkforce)
{
    ASSERT_ERR((((uint32_t)fe3clkforce << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x08000000)) | ((uint32_t)fe3clkforce << 27));
}

/**
 * @brief Returns the current value of the FE2CLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the FE2CLKFORCE field's value will be returned.
 *
 * @return The current value of the FE2CLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_fe2clkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

/**
 * @brief Sets the FE2CLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fe2clkforce - The value to set the field to.
 */
__INLINE void mdm_fe2clkforce_setf(uint8_t fe2clkforce)
{
    ASSERT_ERR((((uint32_t)fe2clkforce << 26) & ~((uint32_t)0x04000000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x04000000)) | ((uint32_t)fe2clkforce << 26));
}

/**
 * @brief Returns the current value of the FE1CLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the FE1CLKFORCE field's value will be returned.
 *
 * @return The current value of the FE1CLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_fe1clkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

/**
 * @brief Sets the FE1CLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fe1clkforce - The value to set the field to.
 */
__INLINE void mdm_fe1clkforce_setf(uint8_t fe1clkforce)
{
    ASSERT_ERR((((uint32_t)fe1clkforce << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x02000000)) | ((uint32_t)fe1clkforce << 25));
}

/**
 * @brief Returns the current value of the FE0CLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the FE0CLKFORCE field's value will be returned.
 *
 * @return The current value of the FE0CLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_fe0clkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the FE0CLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fe0clkforce - The value to set the field to.
 */
__INLINE void mdm_fe0clkforce_setf(uint8_t fe0clkforce)
{
    ASSERT_ERR((((uint32_t)fe0clkforce << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)fe0clkforce << 24));
}

/**
 * @brief Returns the current value of the STOCPECLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the STOCPECLKFORCE field's value will be returned.
 *
 * @return The current value of the STOCPECLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_stocpeclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Sets the STOCPECLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] stocpeclkforce - The value to set the field to.
 */
__INLINE void mdm_stocpeclkforce_setf(uint8_t stocpeclkforce)
{
    ASSERT_ERR((((uint32_t)stocpeclkforce << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00800000)) | ((uint32_t)stocpeclkforce << 23));
}

/**
 * @brief Returns the current value of the EQUCLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the EQUCLKFORCE field's value will be returned.
 *
 * @return The current value of the EQUCLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_equclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Sets the EQUCLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] equclkforce - The value to set the field to.
 */
__INLINE void mdm_equclkforce_setf(uint8_t equclkforce)
{
    ASSERT_ERR((((uint32_t)equclkforce << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00400000)) | ((uint32_t)equclkforce << 22));
}

/**
 * @brief Returns the current value of the TDCOMP3CLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the TDCOMP3CLKFORCE field's value will be returned.
 *
 * @return The current value of the TDCOMP3CLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_tdcomp3clkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Sets the TDCOMP3CLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdcomp3clkforce - The value to set the field to.
 */
__INLINE void mdm_tdcomp3clkforce_setf(uint8_t tdcomp3clkforce)
{
    ASSERT_ERR((((uint32_t)tdcomp3clkforce << 21) & ~((uint32_t)0x00200000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00200000)) | ((uint32_t)tdcomp3clkforce << 21));
}

/**
 * @brief Returns the current value of the TDCOMP2CLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the TDCOMP2CLKFORCE field's value will be returned.
 *
 * @return The current value of the TDCOMP2CLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_tdcomp2clkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Sets the TDCOMP2CLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdcomp2clkforce - The value to set the field to.
 */
__INLINE void mdm_tdcomp2clkforce_setf(uint8_t tdcomp2clkforce)
{
    ASSERT_ERR((((uint32_t)tdcomp2clkforce << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00100000)) | ((uint32_t)tdcomp2clkforce << 20));
}

/**
 * @brief Returns the current value of the TDCOMP1CLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the TDCOMP1CLKFORCE field's value will be returned.
 *
 * @return The current value of the TDCOMP1CLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_tdcomp1clkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Sets the TDCOMP1CLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdcomp1clkforce - The value to set the field to.
 */
__INLINE void mdm_tdcomp1clkforce_setf(uint8_t tdcomp1clkforce)
{
    ASSERT_ERR((((uint32_t)tdcomp1clkforce << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00080000)) | ((uint32_t)tdcomp1clkforce << 19));
}

/**
 * @brief Returns the current value of the TDCOMP0CLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the TDCOMP0CLKFORCE field's value will be returned.
 *
 * @return The current value of the TDCOMP0CLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_tdcomp0clkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Sets the TDCOMP0CLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdcomp0clkforce - The value to set the field to.
 */
__INLINE void mdm_tdcomp0clkforce_setf(uint8_t tdcomp0clkforce)
{
    ASSERT_ERR((((uint32_t)tdcomp0clkforce << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00040000)) | ((uint32_t)tdcomp0clkforce << 18));
}

/**
 * @brief Returns the current value of the TDFOESTCLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the TDFOESTCLKFORCE field's value will be returned.
 *
 * @return The current value of the TDFOESTCLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_tdfoestclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Sets the TDFOESTCLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdfoestclkforce - The value to set the field to.
 */
__INLINE void mdm_tdfoestclkforce_setf(uint8_t tdfoestclkforce)
{
    ASSERT_ERR((((uint32_t)tdfoestclkforce << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00020000)) | ((uint32_t)tdfoestclkforce << 17));
}

/**
 * @brief Returns the current value of the TBECLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the TBECLKFORCE field's value will be returned.
 *
 * @return The current value of the TBECLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_tbeclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the TBECLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tbeclkforce - The value to set the field to.
 */
__INLINE void mdm_tbeclkforce_setf(uint8_t tbeclkforce)
{
    ASSERT_ERR((((uint32_t)tbeclkforce << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)tbeclkforce << 16));
}

/**
 * @brief Returns the current value of the VTBCLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the VTBCLKFORCE field's value will be returned.
 *
 * @return The current value of the VTBCLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_vtbclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

/**
 * @brief Sets the VTBCLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] vtbclkforce - The value to set the field to.
 */
__INLINE void mdm_vtbclkforce_setf(uint8_t vtbclkforce)
{
    ASSERT_ERR((((uint32_t)vtbclkforce << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00008000)) | ((uint32_t)vtbclkforce << 15));
}

/**
 * @brief Returns the current value of the FFT3MEMCLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the FFT3MEMCLKFORCE field's value will be returned.
 *
 * @return The current value of the FFT3MEMCLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_fft3memclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Sets the FFT3MEMCLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft3memclkforce - The value to set the field to.
 */
__INLINE void mdm_fft3memclkforce_setf(uint8_t fft3memclkforce)
{
    ASSERT_ERR((((uint32_t)fft3memclkforce << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00004000)) | ((uint32_t)fft3memclkforce << 14));
}

/**
 * @brief Returns the current value of the FFT3CLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the FFT3CLKFORCE field's value will be returned.
 *
 * @return The current value of the FFT3CLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_fft3clkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the FFT3CLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft3clkforce - The value to set the field to.
 */
__INLINE void mdm_fft3clkforce_setf(uint8_t fft3clkforce)
{
    ASSERT_ERR((((uint32_t)fft3clkforce << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)fft3clkforce << 13));
}

/**
 * @brief Returns the current value of the FFT2MEMCLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the FFT2MEMCLKFORCE field's value will be returned.
 *
 * @return The current value of the FFT2MEMCLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_fft2memclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the FFT2MEMCLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft2memclkforce - The value to set the field to.
 */
__INLINE void mdm_fft2memclkforce_setf(uint8_t fft2memclkforce)
{
    ASSERT_ERR((((uint32_t)fft2memclkforce << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)fft2memclkforce << 12));
}

/**
 * @brief Returns the current value of the FFT2CLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the FFT2CLKFORCE field's value will be returned.
 *
 * @return The current value of the FFT2CLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_fft2clkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the FFT2CLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft2clkforce - The value to set the field to.
 */
__INLINE void mdm_fft2clkforce_setf(uint8_t fft2clkforce)
{
    ASSERT_ERR((((uint32_t)fft2clkforce << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00000800)) | ((uint32_t)fft2clkforce << 11));
}

/**
 * @brief Returns the current value of the FFT1MEMCLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the FFT1MEMCLKFORCE field's value will be returned.
 *
 * @return The current value of the FFT1MEMCLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_fft1memclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the FFT1MEMCLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft1memclkforce - The value to set the field to.
 */
__INLINE void mdm_fft1memclkforce_setf(uint8_t fft1memclkforce)
{
    ASSERT_ERR((((uint32_t)fft1memclkforce << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00000400)) | ((uint32_t)fft1memclkforce << 10));
}

/**
 * @brief Returns the current value of the FFT1CLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the FFT1CLKFORCE field's value will be returned.
 *
 * @return The current value of the FFT1CLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_fft1clkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the FFT1CLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft1clkforce - The value to set the field to.
 */
__INLINE void mdm_fft1clkforce_setf(uint8_t fft1clkforce)
{
    ASSERT_ERR((((uint32_t)fft1clkforce << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)fft1clkforce << 9));
}

/**
 * @brief Returns the current value of the FFT0MEMCLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the FFT0MEMCLKFORCE field's value will be returned.
 *
 * @return The current value of the FFT0MEMCLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_fft0memclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the FFT0MEMCLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft0memclkforce - The value to set the field to.
 */
__INLINE void mdm_fft0memclkforce_setf(uint8_t fft0memclkforce)
{
    ASSERT_ERR((((uint32_t)fft0memclkforce << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)fft0memclkforce << 8));
}

/**
 * @brief Returns the current value of the FFT0CLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the FFT0CLKFORCE field's value will be returned.
 *
 * @return The current value of the FFT0CLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_fft0clkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the FFT0CLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft0clkforce - The value to set the field to.
 */
__INLINE void mdm_fft0clkforce_setf(uint8_t fft0clkforce)
{
    ASSERT_ERR((((uint32_t)fft0clkforce << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)fft0clkforce << 7));
}

/**
 * @brief Returns the current value of the CHESTCLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the CHESTCLKFORCE field's value will be returned.
 *
 * @return The current value of the CHESTCLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_chestclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the CHESTCLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] chestclkforce - The value to set the field to.
 */
__INLINE void mdm_chestclkforce_setf(uint8_t chestclkforce)
{
    ASSERT_ERR((((uint32_t)chestclkforce << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)chestclkforce << 6));
}

/**
 * @brief Returns the current value of the GMEMCLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the GMEMCLKFORCE field's value will be returned.
 *
 * @return The current value of the GMEMCLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_gmemclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the GMEMCLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] gmemclkforce - The value to set the field to.
 */
__INLINE void mdm_gmemclkforce_setf(uint8_t gmemclkforce)
{
    ASSERT_ERR((((uint32_t)gmemclkforce << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)gmemclkforce << 5));
}

/**
 * @brief Returns the current value of the HMEMCLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the HMEMCLKFORCE field's value will be returned.
 *
 * @return The current value of the HMEMCLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_hmemclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the HMEMCLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] hmemclkforce - The value to set the field to.
 */
__INLINE void mdm_hmemclkforce_setf(uint8_t hmemclkforce)
{
    ASSERT_ERR((((uint32_t)hmemclkforce << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)hmemclkforce << 4));
}

/**
 * @brief Returns the current value of the QMEMCLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the QMEMCLKFORCE field's value will be returned.
 *
 * @return The current value of the QMEMCLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_qmemclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the QMEMCLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] qmemclkforce - The value to set the field to.
 */
__INLINE void mdm_qmemclkforce_setf(uint8_t qmemclkforce)
{
    ASSERT_ERR((((uint32_t)qmemclkforce << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)qmemclkforce << 3));
}

/**
 * @brief Returns the current value of the INTLVMEMCLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the INTLVMEMCLKFORCE field's value will be returned.
 *
 * @return The current value of the INTLVMEMCLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_intlvmemclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the INTLVMEMCLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] intlvmemclkforce - The value to set the field to.
 */
__INLINE void mdm_intlvmemclkforce_setf(uint8_t intlvmemclkforce)
{
    ASSERT_ERR((((uint32_t)intlvmemclkforce << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)intlvmemclkforce << 2));
}

/**
 * @brief Returns the current value of the INTLVVTBCLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the INTLVVTBCLKFORCE field's value will be returned.
 *
 * @return The current value of the INTLVVTBCLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_intlvvtbclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the INTLVVTBCLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] intlvvtbclkforce - The value to set the field to.
 */
__INLINE void mdm_intlvvtbclkforce_setf(uint8_t intlvvtbclkforce)
{
    ASSERT_ERR((((uint32_t)intlvvtbclkforce << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)intlvvtbclkforce << 1));
}

/**
 * @brief Returns the current value of the INTLVPHYCLKFORCE field in the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read and the INTLVPHYCLKFORCE field's value will be returned.
 *
 * @return The current value of the INTLVPHYCLKFORCE field in the CLKGATEFCTRL0 register.
 */
__INLINE uint8_t mdm_intlvphyclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the INTLVPHYCLKFORCE field of the CLKGATEFCTRL0 register.
 *
 * The CLKGATEFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] intlvphyclkforce - The value to set the field to.
 */
__INLINE void mdm_intlvphyclkforce_setf(uint8_t intlvphyclkforce)
{
    ASSERT_ERR((((uint32_t)intlvphyclkforce << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)intlvphyclkforce << 0));
}

/// @}

/**
 * @name CLKGATEFCTRL1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     05     RADARTIMCLKFORCE   0
 *     04       MDMBTXCLKFORCE   0
 *     03       MDMBRXCLKFORCE   0
 * </pre>
 *
 * @{
 */

/// Address of the CLKGATEFCTRL1 register
#define MDM_CLKGATEFCTRL1_ADDR   0x01000880
/// Offset of the CLKGATEFCTRL1 register from the base address
#define MDM_CLKGATEFCTRL1_OFFSET 0x00000880
/// Index of the CLKGATEFCTRL1 register
#define MDM_CLKGATEFCTRL1_INDEX  0x00000220
/// Reset value of the CLKGATEFCTRL1 register
#define MDM_CLKGATEFCTRL1_RESET  0x00000000

/**
 * @brief Returns the current value of the CLKGATEFCTRL1 register.
 * The CLKGATEFCTRL1 register will be read and its value returned.
 * @return The current value of the CLKGATEFCTRL1 register.
 */
__INLINE uint32_t mdm_clkgatefctrl1_get(void)
{
    return REG_PL_RD(MDM_CLKGATEFCTRL1_ADDR);
}

/**
 * @brief Sets the CLKGATEFCTRL1 register to a value.
 * The CLKGATEFCTRL1 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_clkgatefctrl1_set(uint32_t value)
{
    REG_PL_WR(MDM_CLKGATEFCTRL1_ADDR, value);
}

// field definitions
/// RADARTIMCLKFORCE field bit
#define MDM_RADARTIMCLKFORCE_BIT    ((uint32_t)0x00000020)
/// RADARTIMCLKFORCE field position
#define MDM_RADARTIMCLKFORCE_POS    5
/// MDMBTXCLKFORCE field bit
#define MDM_MDMBTXCLKFORCE_BIT      ((uint32_t)0x00000010)
/// MDMBTXCLKFORCE field position
#define MDM_MDMBTXCLKFORCE_POS      4
/// MDMBRXCLKFORCE field bit
#define MDM_MDMBRXCLKFORCE_BIT      ((uint32_t)0x00000008)
/// MDMBRXCLKFORCE field position
#define MDM_MDMBRXCLKFORCE_POS      3

/// RADARTIMCLKFORCE field reset value
#define MDM_RADARTIMCLKFORCE_RST    0x0
/// MDMBTXCLKFORCE field reset value
#define MDM_MDMBTXCLKFORCE_RST      0x0
/// MDMBRXCLKFORCE field reset value
#define MDM_MDMBRXCLKFORCE_RST      0x0

/**
 * @brief Constructs a value for the CLKGATEFCTRL1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] radartimclkforce - The value to use for the RADARTIMCLKFORCE field.
 * @param[in] mdmbtxclkforce - The value to use for the MDMBTXCLKFORCE field.
 * @param[in] mdmbrxclkforce - The value to use for the MDMBRXCLKFORCE field.
 */
__INLINE void mdm_clkgatefctrl1_pack(uint8_t radartimclkforce, uint8_t mdmbtxclkforce, uint8_t mdmbrxclkforce)
{
    ASSERT_ERR((((uint32_t)radartimclkforce << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)mdmbtxclkforce << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)mdmbrxclkforce << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL1_ADDR,  ((uint32_t)radartimclkforce << 5) | ((uint32_t)mdmbtxclkforce << 4) | ((uint32_t)mdmbrxclkforce << 3));
}

/**
 * @brief Unpacks CLKGATEFCTRL1's fields from current value of the CLKGATEFCTRL1 register.
 *
 * Reads the CLKGATEFCTRL1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] radartimclkforce - Will be populated with the current value of this field from the register.
 * @param[out] mdmbtxclkforce - Will be populated with the current value of this field from the register.
 * @param[out] mdmbrxclkforce - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_clkgatefctrl1_unpack(uint8_t *radartimclkforce, uint8_t *mdmbtxclkforce, uint8_t *mdmbrxclkforce)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL1_ADDR);

    *radartimclkforce = (localVal & ((uint32_t)0x00000020)) >> 5;
    *mdmbtxclkforce = (localVal & ((uint32_t)0x00000010)) >> 4;
    *mdmbrxclkforce = (localVal & ((uint32_t)0x00000008)) >> 3;
}

/**
 * @brief Returns the current value of the RADARTIMCLKFORCE field in the CLKGATEFCTRL1 register.
 *
 * The CLKGATEFCTRL1 register will be read and the RADARTIMCLKFORCE field's value will be returned.
 *
 * @return The current value of the RADARTIMCLKFORCE field in the CLKGATEFCTRL1 register.
 */
__INLINE uint8_t mdm_radartimclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the RADARTIMCLKFORCE field of the CLKGATEFCTRL1 register.
 *
 * The CLKGATEFCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] radartimclkforce - The value to set the field to.
 */
__INLINE void mdm_radartimclkforce_setf(uint8_t radartimclkforce)
{
    ASSERT_ERR((((uint32_t)radartimclkforce << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL1_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL1_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)radartimclkforce << 5));
}

/**
 * @brief Returns the current value of the MDMBTXCLKFORCE field in the CLKGATEFCTRL1 register.
 *
 * The CLKGATEFCTRL1 register will be read and the MDMBTXCLKFORCE field's value will be returned.
 *
 * @return The current value of the MDMBTXCLKFORCE field in the CLKGATEFCTRL1 register.
 */
__INLINE uint8_t mdm_mdmbtxclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the MDMBTXCLKFORCE field of the CLKGATEFCTRL1 register.
 *
 * The CLKGATEFCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mdmbtxclkforce - The value to set the field to.
 */
__INLINE void mdm_mdmbtxclkforce_setf(uint8_t mdmbtxclkforce)
{
    ASSERT_ERR((((uint32_t)mdmbtxclkforce << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL1_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL1_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)mdmbtxclkforce << 4));
}

/**
 * @brief Returns the current value of the MDMBRXCLKFORCE field in the CLKGATEFCTRL1 register.
 *
 * The CLKGATEFCTRL1 register will be read and the MDMBRXCLKFORCE field's value will be returned.
 *
 * @return The current value of the MDMBRXCLKFORCE field in the CLKGATEFCTRL1 register.
 */
__INLINE uint8_t mdm_mdmbrxclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CLKGATEFCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the MDMBRXCLKFORCE field of the CLKGATEFCTRL1 register.
 *
 * The CLKGATEFCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mdmbrxclkforce - The value to set the field to.
 */
__INLINE void mdm_mdmbrxclkforce_setf(uint8_t mdmbrxclkforce)
{
    ASSERT_ERR((((uint32_t)mdmbrxclkforce << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MDM_CLKGATEFCTRL1_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL1_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)mdmbrxclkforce << 3));
}

/// @}

/**
 * @name DCOFFSET0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  23:16         TXIDCOFFSET0   0x0
 *  07:00         TXQDCOFFSET0   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DCOFFSET0 register
#define MDM_DCOFFSET0_ADDR   0x01000884
/// Offset of the DCOFFSET0 register from the base address
#define MDM_DCOFFSET0_OFFSET 0x00000884
/// Index of the DCOFFSET0 register
#define MDM_DCOFFSET0_INDEX  0x00000221
/// Reset value of the DCOFFSET0 register
#define MDM_DCOFFSET0_RESET  0x00000000

/**
 * @brief Returns the current value of the DCOFFSET0 register.
 * The DCOFFSET0 register will be read and its value returned.
 * @return The current value of the DCOFFSET0 register.
 */
__INLINE uint32_t mdm_dcoffset0_get(void)
{
    return REG_PL_RD(MDM_DCOFFSET0_ADDR);
}

/**
 * @brief Sets the DCOFFSET0 register to a value.
 * The DCOFFSET0 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_dcoffset0_set(uint32_t value)
{
    REG_PL_WR(MDM_DCOFFSET0_ADDR, value);
}

// field definitions
/// TXIDCOFFSET0 field mask
#define MDM_TXIDCOFFSET0_MASK   ((uint32_t)0x00FF0000)
/// TXIDCOFFSET0 field LSB position
#define MDM_TXIDCOFFSET0_LSB    16
/// TXIDCOFFSET0 field width
#define MDM_TXIDCOFFSET0_WIDTH  ((uint32_t)0x00000008)
/// TXQDCOFFSET0 field mask
#define MDM_TXQDCOFFSET0_MASK   ((uint32_t)0x000000FF)
/// TXQDCOFFSET0 field LSB position
#define MDM_TXQDCOFFSET0_LSB    0
/// TXQDCOFFSET0 field width
#define MDM_TXQDCOFFSET0_WIDTH  ((uint32_t)0x00000008)

/// TXIDCOFFSET0 field reset value
#define MDM_TXIDCOFFSET0_RST    0x0
/// TXQDCOFFSET0 field reset value
#define MDM_TXQDCOFFSET0_RST    0x0

/**
 * @brief Constructs a value for the DCOFFSET0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] txidcoffset0 - The value to use for the TXIDCOFFSET0 field.
 * @param[in] txqdcoffset0 - The value to use for the TXQDCOFFSET0 field.
 */
__INLINE void mdm_dcoffset0_pack(uint8_t txidcoffset0, uint8_t txqdcoffset0)
{
    ASSERT_ERR((((uint32_t)txidcoffset0 << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)txqdcoffset0 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_DCOFFSET0_ADDR,  ((uint32_t)txidcoffset0 << 16) | ((uint32_t)txqdcoffset0 << 0));
}

/**
 * @brief Unpacks DCOFFSET0's fields from current value of the DCOFFSET0 register.
 *
 * Reads the DCOFFSET0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txidcoffset0 - Will be populated with the current value of this field from the register.
 * @param[out] txqdcoffset0 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_dcoffset0_unpack(uint8_t *txidcoffset0, uint8_t *txqdcoffset0)
{
    uint32_t localVal = REG_PL_RD(MDM_DCOFFSET0_ADDR);

    *txidcoffset0 = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *txqdcoffset0 = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the TXIDCOFFSET0 field in the DCOFFSET0 register.
 *
 * The DCOFFSET0 register will be read and the TXIDCOFFSET0 field's value will be returned.
 *
 * @return The current value of the TXIDCOFFSET0 field in the DCOFFSET0 register.
 */
__INLINE uint8_t mdm_txidcoffset0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_DCOFFSET0_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the TXIDCOFFSET0 field of the DCOFFSET0 register.
 *
 * The DCOFFSET0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txidcoffset0 - The value to set the field to.
 */
__INLINE void mdm_txidcoffset0_setf(uint8_t txidcoffset0)
{
    ASSERT_ERR((((uint32_t)txidcoffset0 << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(MDM_DCOFFSET0_ADDR, (REG_PL_RD(MDM_DCOFFSET0_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)txidcoffset0 << 16));
}

/**
 * @brief Returns the current value of the TXQDCOFFSET0 field in the DCOFFSET0 register.
 *
 * The DCOFFSET0 register will be read and the TXQDCOFFSET0 field's value will be returned.
 *
 * @return The current value of the TXQDCOFFSET0 field in the DCOFFSET0 register.
 */
__INLINE uint8_t mdm_txqdcoffset0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_DCOFFSET0_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the TXQDCOFFSET0 field of the DCOFFSET0 register.
 *
 * The DCOFFSET0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txqdcoffset0 - The value to set the field to.
 */
__INLINE void mdm_txqdcoffset0_setf(uint8_t txqdcoffset0)
{
    ASSERT_ERR((((uint32_t)txqdcoffset0 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_DCOFFSET0_ADDR, (REG_PL_RD(MDM_DCOFFSET0_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)txqdcoffset0 << 0));
}

/// @}

/**
 * @name SWRESET register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     12           AGCSWRESET   0
 *     08           DSPSWRESET   0
 *     04           FSMSWRESET   0
 *     00           MDMSWRESET   0
 * </pre>
 *
 * @{
 */

/// Address of the SWRESET register
#define MDM_SWRESET_ADDR   0x01000894
/// Offset of the SWRESET register from the base address
#define MDM_SWRESET_OFFSET 0x00000894
/// Index of the SWRESET register
#define MDM_SWRESET_INDEX  0x00000225
/// Reset value of the SWRESET register
#define MDM_SWRESET_RESET  0x00000000

/**
 * @brief Returns the current value of the SWRESET register.
 * The SWRESET register will be read and its value returned.
 * @return The current value of the SWRESET register.
 */
__INLINE uint32_t mdm_swreset_get(void)
{
    return REG_PL_RD(MDM_SWRESET_ADDR);
}

/**
 * @brief Sets the SWRESET register to a value.
 * The SWRESET register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_swreset_set(uint32_t value)
{
    REG_PL_WR(MDM_SWRESET_ADDR, value);
}

// field definitions
/// AGCSWRESET field bit
#define MDM_AGCSWRESET_BIT    ((uint32_t)0x00001000)
/// AGCSWRESET field position
#define MDM_AGCSWRESET_POS    12
/// DSPSWRESET field bit
#define MDM_DSPSWRESET_BIT    ((uint32_t)0x00000100)
/// DSPSWRESET field position
#define MDM_DSPSWRESET_POS    8
/// FSMSWRESET field bit
#define MDM_FSMSWRESET_BIT    ((uint32_t)0x00000010)
/// FSMSWRESET field position
#define MDM_FSMSWRESET_POS    4
/// MDMSWRESET field bit
#define MDM_MDMSWRESET_BIT    ((uint32_t)0x00000001)
/// MDMSWRESET field position
#define MDM_MDMSWRESET_POS    0

/// AGCSWRESET field reset value
#define MDM_AGCSWRESET_RST    0x0
/// DSPSWRESET field reset value
#define MDM_DSPSWRESET_RST    0x0
/// FSMSWRESET field reset value
#define MDM_FSMSWRESET_RST    0x0
/// MDMSWRESET field reset value
#define MDM_MDMSWRESET_RST    0x0

/**
 * @brief Constructs a value for the SWRESET register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] agcswreset - The value to use for the AGCSWRESET field.
 * @param[in] dspswreset - The value to use for the DSPSWRESET field.
 * @param[in] fsmswreset - The value to use for the FSMSWRESET field.
 * @param[in] mdmswreset - The value to use for the MDMSWRESET field.
 */
__INLINE void mdm_swreset_pack(uint8_t agcswreset, uint8_t dspswreset, uint8_t fsmswreset, uint8_t mdmswreset)
{
    ASSERT_ERR((((uint32_t)agcswreset << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)dspswreset << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)fsmswreset << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)mdmswreset << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_SWRESET_ADDR,  ((uint32_t)agcswreset << 12) | ((uint32_t)dspswreset << 8) | ((uint32_t)fsmswreset << 4) | ((uint32_t)mdmswreset << 0));
}

/**
 * @brief Unpacks SWRESET's fields from current value of the SWRESET register.
 *
 * Reads the SWRESET register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] agcswreset - Will be populated with the current value of this field from the register.
 * @param[out] dspswreset - Will be populated with the current value of this field from the register.
 * @param[out] fsmswreset - Will be populated with the current value of this field from the register.
 * @param[out] mdmswreset - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_swreset_unpack(uint8_t *agcswreset, uint8_t *dspswreset, uint8_t *fsmswreset, uint8_t *mdmswreset)
{
    uint32_t localVal = REG_PL_RD(MDM_SWRESET_ADDR);

    *agcswreset = (localVal & ((uint32_t)0x00001000)) >> 12;
    *dspswreset = (localVal & ((uint32_t)0x00000100)) >> 8;
    *fsmswreset = (localVal & ((uint32_t)0x00000010)) >> 4;
    *mdmswreset = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the AGCSWRESET field in the SWRESET register.
 *
 * The SWRESET register will be read and the AGCSWRESET field's value will be returned.
 *
 * @return The current value of the AGCSWRESET field in the SWRESET register.
 */
__INLINE uint8_t mdm_agcswreset_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SWRESET_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the AGCSWRESET field of the SWRESET register.
 *
 * The SWRESET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] agcswreset - The value to set the field to.
 */
__INLINE void mdm_agcswreset_setf(uint8_t agcswreset)
{
    ASSERT_ERR((((uint32_t)agcswreset << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(MDM_SWRESET_ADDR, (REG_PL_RD(MDM_SWRESET_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)agcswreset << 12));
}

/**
 * @brief Returns the current value of the DSPSWRESET field in the SWRESET register.
 *
 * The SWRESET register will be read and the DSPSWRESET field's value will be returned.
 *
 * @return The current value of the DSPSWRESET field in the SWRESET register.
 */
__INLINE uint8_t mdm_dspswreset_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SWRESET_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the DSPSWRESET field of the SWRESET register.
 *
 * The SWRESET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dspswreset - The value to set the field to.
 */
__INLINE void mdm_dspswreset_setf(uint8_t dspswreset)
{
    ASSERT_ERR((((uint32_t)dspswreset << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(MDM_SWRESET_ADDR, (REG_PL_RD(MDM_SWRESET_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)dspswreset << 8));
}

/**
 * @brief Returns the current value of the FSMSWRESET field in the SWRESET register.
 *
 * The SWRESET register will be read and the FSMSWRESET field's value will be returned.
 *
 * @return The current value of the FSMSWRESET field in the SWRESET register.
 */
__INLINE uint8_t mdm_fsmswreset_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SWRESET_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the FSMSWRESET field of the SWRESET register.
 *
 * The SWRESET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fsmswreset - The value to set the field to.
 */
__INLINE void mdm_fsmswreset_setf(uint8_t fsmswreset)
{
    ASSERT_ERR((((uint32_t)fsmswreset << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(MDM_SWRESET_ADDR, (REG_PL_RD(MDM_SWRESET_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)fsmswreset << 4));
}

/**
 * @brief Returns the current value of the MDMSWRESET field in the SWRESET register.
 *
 * The SWRESET register will be read and the MDMSWRESET field's value will be returned.
 *
 * @return The current value of the MDMSWRESET field in the SWRESET register.
 */
__INLINE uint8_t mdm_mdmswreset_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SWRESET_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the MDMSWRESET field of the SWRESET register.
 *
 * The SWRESET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mdmswreset - The value to set the field to.
 */
__INLINE void mdm_mdmswreset_setf(uint8_t mdmswreset)
{
    ASSERT_ERR((((uint32_t)mdmswreset << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_SWRESET_ADDR, (REG_PL_RD(MDM_SWRESET_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)mdmswreset << 0));
}

/// @}


#endif // _REG_MDM_CFG_H_

/// @}

