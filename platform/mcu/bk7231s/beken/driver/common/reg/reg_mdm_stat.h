/**
 * @file reg_mdm_stat.h
 * @brief Definitions of the MDM HW block registers and register access functions.
 *
 * @defgroup REG_MDM_STAT REG_MDM_STAT
 * @ingroup REG
 * @{
 *
 * @brief Definitions of the MDM HW block registers and register access functions.
 */
#ifndef _REG_MDM_STAT_H_
#define _REG_MDM_STAT_H_

#include "co_int.h"
#include "_reg_mdm_stat.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"

/** @brief Number of registers in the REG_MDM_STAT peripheral.
 */
#define REG_MDM_STAT_COUNT 27

/** @brief Decoding mask of the REG_MDM_STAT peripheral registers from the CPU point of view.
 */
#define REG_MDM_STAT_DECODING_MASK 0x0000007F

/**
 * @name NXVERSION register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            PHYConfig   0x0
 * </pre>
 *
 * @{
 */

/// Address of the NXVERSION register
#define MDM_NXVERSION_ADDR   0x01000000
/// Offset of the NXVERSION register from the base address
#define MDM_NXVERSION_OFFSET 0x00000000
/// Index of the NXVERSION register
#define MDM_NXVERSION_INDEX  0x00000000
/// Reset value of the NXVERSION register
#define MDM_NXVERSION_RESET  0x00000000

/**
 * @brief Returns the current value of the NXVERSION register.
 * The NXVERSION register will be read and its value returned.
 * @return The current value of the NXVERSION register.
 */
__INLINE uint32_t mdm_nxversion_get(void)
{
    return REG_PL_RD(MDM_NXVERSION_ADDR);
}

// field definitions
/// PHY_CONFIG field mask
#define MDM_PHY_CONFIG_MASK   ((uint32_t)0xFFFFFFFF)
/// PHY_CONFIG field LSB position
#define MDM_PHY_CONFIG_LSB    0
/// PHY_CONFIG field width
#define MDM_PHY_CONFIG_WIDTH  ((uint32_t)0x00000020)

/// PHY_CONFIG field reset value
#define MDM_PHY_CONFIG_RST    0x0

/**
 * @brief Returns the current value of the PHYConfig field in the NXVERSION register.
 *
 * The NXVERSION register will be read and the PHYConfig field's value will be returned.
 *
 * @return The current value of the PHYConfig field in the NXVERSION register.
 */
__INLINE uint32_t mdm_phy_config_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_NXVERSION_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name TDOFFSETSTAT0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  20:00        COARSEFOANGLE   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TDOFFSETSTAT0 register
#define MDM_TDOFFSETSTAT0_ADDR   0x01000004
/// Offset of the TDOFFSETSTAT0 register from the base address
#define MDM_TDOFFSETSTAT0_OFFSET 0x00000004
/// Index of the TDOFFSETSTAT0 register
#define MDM_TDOFFSETSTAT0_INDEX  0x00000001
/// Reset value of the TDOFFSETSTAT0 register
#define MDM_TDOFFSETSTAT0_RESET  0x00000000

/**
 * @brief Returns the current value of the TDOFFSETSTAT0 register.
 * The TDOFFSETSTAT0 register will be read and its value returned.
 * @return The current value of the TDOFFSETSTAT0 register.
 */
__INLINE uint32_t mdm_tdoffsetstat0_get(void)
{
    return REG_PL_RD(MDM_TDOFFSETSTAT0_ADDR);
}

// field definitions
/// COARSEFOANGLE field mask
#define MDM_COARSEFOANGLE_MASK   ((uint32_t)0x001FFFFF)
/// COARSEFOANGLE field LSB position
#define MDM_COARSEFOANGLE_LSB    0
/// COARSEFOANGLE field width
#define MDM_COARSEFOANGLE_WIDTH  ((uint32_t)0x00000015)

/// COARSEFOANGLE field reset value
#define MDM_COARSEFOANGLE_RST    0x0

/**
 * @brief Returns the current value of the COARSEFOANGLE field in the TDOFFSETSTAT0 register.
 *
 * The TDOFFSETSTAT0 register will be read and the COARSEFOANGLE field's value will be returned.
 *
 * @return The current value of the COARSEFOANGLE field in the TDOFFSETSTAT0 register.
 */
__INLINE uint32_t mdm_coarsefoangle_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDOFFSETSTAT0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x001FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name TDOFFSETSTAT1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  20:00          FINEFOANGLE   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TDOFFSETSTAT1 register
#define MDM_TDOFFSETSTAT1_ADDR   0x01000008
/// Offset of the TDOFFSETSTAT1 register from the base address
#define MDM_TDOFFSETSTAT1_OFFSET 0x00000008
/// Index of the TDOFFSETSTAT1 register
#define MDM_TDOFFSETSTAT1_INDEX  0x00000002
/// Reset value of the TDOFFSETSTAT1 register
#define MDM_TDOFFSETSTAT1_RESET  0x00000000

/**
 * @brief Returns the current value of the TDOFFSETSTAT1 register.
 * The TDOFFSETSTAT1 register will be read and its value returned.
 * @return The current value of the TDOFFSETSTAT1 register.
 */
__INLINE uint32_t mdm_tdoffsetstat1_get(void)
{
    return REG_PL_RD(MDM_TDOFFSETSTAT1_ADDR);
}

// field definitions
/// FINEFOANGLE field mask
#define MDM_FINEFOANGLE_MASK   ((uint32_t)0x001FFFFF)
/// FINEFOANGLE field LSB position
#define MDM_FINEFOANGLE_LSB    0
/// FINEFOANGLE field width
#define MDM_FINEFOANGLE_WIDTH  ((uint32_t)0x00000015)

/// FINEFOANGLE field reset value
#define MDM_FINEFOANGLE_RST    0x0

/**
 * @brief Returns the current value of the FINEFOANGLE field in the TDOFFSETSTAT1 register.
 *
 * The TDOFFSETSTAT1 register will be read and the FINEFOANGLE field's value will be returned.
 *
 * @return The current value of the FINEFOANGLE field in the TDOFFSETSTAT1 register.
 */
__INLINE uint32_t mdm_finefoangle_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDOFFSETSTAT1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x001FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name FDTOFFSETSTAT0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:12         STOSLOPESTAT   0x0
 *  05:00       TDSYNCHOFFSTAT   0x0
 * </pre>
 *
 * @{
 */

/// Address of the FDTOFFSETSTAT0 register
#define MDM_FDTOFFSETSTAT0_ADDR   0x0100000C
/// Offset of the FDTOFFSETSTAT0 register from the base address
#define MDM_FDTOFFSETSTAT0_OFFSET 0x0000000C
/// Index of the FDTOFFSETSTAT0 register
#define MDM_FDTOFFSETSTAT0_INDEX  0x00000003
/// Reset value of the FDTOFFSETSTAT0 register
#define MDM_FDTOFFSETSTAT0_RESET  0x00000000

/**
 * @brief Returns the current value of the FDTOFFSETSTAT0 register.
 * The FDTOFFSETSTAT0 register will be read and its value returned.
 * @return The current value of the FDTOFFSETSTAT0 register.
 */
__INLINE uint32_t mdm_fdtoffsetstat0_get(void)
{
    return REG_PL_RD(MDM_FDTOFFSETSTAT0_ADDR);
}

// field definitions
/// STOSLOPESTAT field mask
#define MDM_STOSLOPESTAT_MASK     ((uint32_t)0x3FFFF000)
/// STOSLOPESTAT field LSB position
#define MDM_STOSLOPESTAT_LSB      12
/// STOSLOPESTAT field width
#define MDM_STOSLOPESTAT_WIDTH    ((uint32_t)0x00000012)
/// TDSYNCHOFFSTAT field mask
#define MDM_TDSYNCHOFFSTAT_MASK   ((uint32_t)0x0000003F)
/// TDSYNCHOFFSTAT field LSB position
#define MDM_TDSYNCHOFFSTAT_LSB    0
/// TDSYNCHOFFSTAT field width
#define MDM_TDSYNCHOFFSTAT_WIDTH  ((uint32_t)0x00000006)

/// STOSLOPESTAT field reset value
#define MDM_STOSLOPESTAT_RST      0x0
/// TDSYNCHOFFSTAT field reset value
#define MDM_TDSYNCHOFFSTAT_RST    0x0

/**
 * @brief Unpacks FDTOFFSETSTAT0's fields from current value of the FDTOFFSETSTAT0 register.
 *
 * Reads the FDTOFFSETSTAT0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] stoslopestat - Will be populated with the current value of this field from the register.
 * @param[out] tdsynchoffstat - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_fdtoffsetstat0_unpack(uint32_t *stoslopestat, uint8_t *tdsynchoffstat)
{
    uint32_t localVal = REG_PL_RD(MDM_FDTOFFSETSTAT0_ADDR);

    *stoslopestat = (localVal & ((uint32_t)0x3FFFF000)) >> 12;
    *tdsynchoffstat = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the STOSLOPESTAT field in the FDTOFFSETSTAT0 register.
 *
 * The FDTOFFSETSTAT0 register will be read and the STOSLOPESTAT field's value will be returned.
 *
 * @return The current value of the STOSLOPESTAT field in the FDTOFFSETSTAT0 register.
 */
__INLINE uint32_t mdm_stoslopestat_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDTOFFSETSTAT0_ADDR);
    return ((localVal & ((uint32_t)0x3FFFF000)) >> 12);
}

/**
 * @brief Returns the current value of the TDSYNCHOFFSTAT field in the FDTOFFSETSTAT0 register.
 *
 * The FDTOFFSETSTAT0 register will be read and the TDSYNCHOFFSTAT field's value will be returned.
 *
 * @return The current value of the TDSYNCHOFFSTAT field in the FDTOFFSETSTAT0 register.
 */
__INLINE uint8_t mdm_tdsynchoffstat_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDTOFFSETSTAT0_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/// @}

/**
 * @name FDTOFFSETSTAT1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  17:00         CPESLOPESTAT   0x0
 * </pre>
 *
 * @{
 */

/// Address of the FDTOFFSETSTAT1 register
#define MDM_FDTOFFSETSTAT1_ADDR   0x01000010
/// Offset of the FDTOFFSETSTAT1 register from the base address
#define MDM_FDTOFFSETSTAT1_OFFSET 0x00000010
/// Index of the FDTOFFSETSTAT1 register
#define MDM_FDTOFFSETSTAT1_INDEX  0x00000004
/// Reset value of the FDTOFFSETSTAT1 register
#define MDM_FDTOFFSETSTAT1_RESET  0x00000000

/**
 * @brief Returns the current value of the FDTOFFSETSTAT1 register.
 * The FDTOFFSETSTAT1 register will be read and its value returned.
 * @return The current value of the FDTOFFSETSTAT1 register.
 */
__INLINE uint32_t mdm_fdtoffsetstat1_get(void)
{
    return REG_PL_RD(MDM_FDTOFFSETSTAT1_ADDR);
}

// field definitions
/// CPESLOPESTAT field mask
#define MDM_CPESLOPESTAT_MASK   ((uint32_t)0x0003FFFF)
/// CPESLOPESTAT field LSB position
#define MDM_CPESLOPESTAT_LSB    0
/// CPESLOPESTAT field width
#define MDM_CPESLOPESTAT_WIDTH  ((uint32_t)0x00000012)

/// CPESLOPESTAT field reset value
#define MDM_CPESLOPESTAT_RST    0x0

/**
 * @brief Returns the current value of the CPESLOPESTAT field in the FDTOFFSETSTAT1 register.
 *
 * The FDTOFFSETSTAT1 register will be read and the CPESLOPESTAT field's value will be returned.
 *
 * @return The current value of the CPESLOPESTAT field in the FDTOFFSETSTAT1 register.
 */
__INLINE uint32_t mdm_cpeslopestat_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDTOFFSETSTAT1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0003FFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name RXFSMSTAT0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:16           FDSYMBOLNR   0x0
 *  15:00           TDSYMBOLNR   0x0
 * </pre>
 *
 * @{
 */

/// Address of the RXFSMSTAT0 register
#define MDM_RXFSMSTAT0_ADDR   0x01000014
/// Offset of the RXFSMSTAT0 register from the base address
#define MDM_RXFSMSTAT0_OFFSET 0x00000014
/// Index of the RXFSMSTAT0 register
#define MDM_RXFSMSTAT0_INDEX  0x00000005
/// Reset value of the RXFSMSTAT0 register
#define MDM_RXFSMSTAT0_RESET  0x00000000

/**
 * @brief Returns the current value of the RXFSMSTAT0 register.
 * The RXFSMSTAT0 register will be read and its value returned.
 * @return The current value of the RXFSMSTAT0 register.
 */
__INLINE uint32_t mdm_rxfsmstat0_get(void)
{
    return REG_PL_RD(MDM_RXFSMSTAT0_ADDR);
}

// field definitions
/// FDSYMBOLNR field mask
#define MDM_FDSYMBOLNR_MASK   ((uint32_t)0xFFFF0000)
/// FDSYMBOLNR field LSB position
#define MDM_FDSYMBOLNR_LSB    16
/// FDSYMBOLNR field width
#define MDM_FDSYMBOLNR_WIDTH  ((uint32_t)0x00000010)
/// TDSYMBOLNR field mask
#define MDM_TDSYMBOLNR_MASK   ((uint32_t)0x0000FFFF)
/// TDSYMBOLNR field LSB position
#define MDM_TDSYMBOLNR_LSB    0
/// TDSYMBOLNR field width
#define MDM_TDSYMBOLNR_WIDTH  ((uint32_t)0x00000010)

/// FDSYMBOLNR field reset value
#define MDM_FDSYMBOLNR_RST    0x0
/// TDSYMBOLNR field reset value
#define MDM_TDSYMBOLNR_RST    0x0

/**
 * @brief Unpacks RXFSMSTAT0's fields from current value of the RXFSMSTAT0 register.
 *
 * Reads the RXFSMSTAT0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] fdsymbolnr - Will be populated with the current value of this field from the register.
 * @param[out] tdsymbolnr - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxfsmstat0_unpack(uint16_t *fdsymbolnr, uint16_t *tdsymbolnr)
{
    uint32_t localVal = REG_PL_RD(MDM_RXFSMSTAT0_ADDR);

    *fdsymbolnr = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *tdsymbolnr = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the FDSYMBOLNR field in the RXFSMSTAT0 register.
 *
 * The RXFSMSTAT0 register will be read and the FDSYMBOLNR field's value will be returned.
 *
 * @return The current value of the FDSYMBOLNR field in the RXFSMSTAT0 register.
 */
__INLINE uint16_t mdm_fdsymbolnr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXFSMSTAT0_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Returns the current value of the TDSYMBOLNR field in the RXFSMSTAT0 register.
 *
 * The RXFSMSTAT0 register will be read and the TDSYMBOLNR field's value will be returned.
 *
 * @return The current value of the TDSYMBOLNR field in the RXFSMSTAT0 register.
 */
__INLINE uint16_t mdm_tdsymbolnr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXFSMSTAT0_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/// @}

/**
 * @name RXFSMSTAT1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:24              FDSTATE   0x0
 *  21:16             GILENGTH   0x0
 *     13               STBCEN   0
 *  11:10              TDBWREG   0x0
 *  09:08              FDBWREG   0x0
 *  06:04           RXMODESTAT   0x0
 *  02:00         FDSYMBOLTYPE   0x0
 * </pre>
 *
 * @{
 */

/// Address of the RXFSMSTAT1 register
#define MDM_RXFSMSTAT1_ADDR   0x01000018
/// Offset of the RXFSMSTAT1 register from the base address
#define MDM_RXFSMSTAT1_OFFSET 0x00000018
/// Index of the RXFSMSTAT1 register
#define MDM_RXFSMSTAT1_INDEX  0x00000006
/// Reset value of the RXFSMSTAT1 register
#define MDM_RXFSMSTAT1_RESET  0x00000000

/**
 * @brief Returns the current value of the RXFSMSTAT1 register.
 * The RXFSMSTAT1 register will be read and its value returned.
 * @return The current value of the RXFSMSTAT1 register.
 */
__INLINE uint32_t mdm_rxfsmstat1_get(void)
{
    return REG_PL_RD(MDM_RXFSMSTAT1_ADDR);
}

// field definitions
/// FDSTATE field mask
#define MDM_FDSTATE_MASK        ((uint32_t)0xFF000000)
/// FDSTATE field LSB position
#define MDM_FDSTATE_LSB         24
/// FDSTATE field width
#define MDM_FDSTATE_WIDTH       ((uint32_t)0x00000008)
/// GILENGTH field mask
#define MDM_GILENGTH_MASK       ((uint32_t)0x003F0000)
/// GILENGTH field LSB position
#define MDM_GILENGTH_LSB        16
/// GILENGTH field width
#define MDM_GILENGTH_WIDTH      ((uint32_t)0x00000006)
/// STBCEN field bit
#define MDM_STBCEN_BIT          ((uint32_t)0x00002000)
/// STBCEN field position
#define MDM_STBCEN_POS          13
/// TDBWREG field mask
#define MDM_TDBWREG_MASK        ((uint32_t)0x00000C00)
/// TDBWREG field LSB position
#define MDM_TDBWREG_LSB         10
/// TDBWREG field width
#define MDM_TDBWREG_WIDTH       ((uint32_t)0x00000002)
/// FDBWREG field mask
#define MDM_FDBWREG_MASK        ((uint32_t)0x00000300)
/// FDBWREG field LSB position
#define MDM_FDBWREG_LSB         8
/// FDBWREG field width
#define MDM_FDBWREG_WIDTH       ((uint32_t)0x00000002)
/// RXMODESTAT field mask
#define MDM_RXMODESTAT_MASK     ((uint32_t)0x00000070)
/// RXMODESTAT field LSB position
#define MDM_RXMODESTAT_LSB      4
/// RXMODESTAT field width
#define MDM_RXMODESTAT_WIDTH    ((uint32_t)0x00000003)
/// FDSYMBOLTYPE field mask
#define MDM_FDSYMBOLTYPE_MASK   ((uint32_t)0x00000007)
/// FDSYMBOLTYPE field LSB position
#define MDM_FDSYMBOLTYPE_LSB    0
/// FDSYMBOLTYPE field width
#define MDM_FDSYMBOLTYPE_WIDTH  ((uint32_t)0x00000003)

/// FDSTATE field reset value
#define MDM_FDSTATE_RST         0x0
/// GILENGTH field reset value
#define MDM_GILENGTH_RST        0x0
/// STBCEN field reset value
#define MDM_STBCEN_RST          0x0
/// TDBWREG field reset value
#define MDM_TDBWREG_RST         0x0
/// FDBWREG field reset value
#define MDM_FDBWREG_RST         0x0
/// RXMODESTAT field reset value
#define MDM_RXMODESTAT_RST      0x0
/// FDSYMBOLTYPE field reset value
#define MDM_FDSYMBOLTYPE_RST    0x0

/**
 * @brief Unpacks RXFSMSTAT1's fields from current value of the RXFSMSTAT1 register.
 *
 * Reads the RXFSMSTAT1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] fdstate - Will be populated with the current value of this field from the register.
 * @param[out] gilength - Will be populated with the current value of this field from the register.
 * @param[out] stbcen - Will be populated with the current value of this field from the register.
 * @param[out] tdbwreg - Will be populated with the current value of this field from the register.
 * @param[out] fdbwreg - Will be populated with the current value of this field from the register.
 * @param[out] rxmodestat - Will be populated with the current value of this field from the register.
 * @param[out] fdsymboltype - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxfsmstat1_unpack(uint8_t *fdstate, uint8_t *gilength, uint8_t *stbcen, uint8_t *tdbwreg, uint8_t *fdbwreg, uint8_t *rxmodestat, uint8_t *fdsymboltype)
{
    uint32_t localVal = REG_PL_RD(MDM_RXFSMSTAT1_ADDR);

    *fdstate = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *gilength = (localVal & ((uint32_t)0x003F0000)) >> 16;
    *stbcen = (localVal & ((uint32_t)0x00002000)) >> 13;
    *tdbwreg = (localVal & ((uint32_t)0x00000C00)) >> 10;
    *fdbwreg = (localVal & ((uint32_t)0x00000300)) >> 8;
    *rxmodestat = (localVal & ((uint32_t)0x00000070)) >> 4;
    *fdsymboltype = (localVal & ((uint32_t)0x00000007)) >> 0;
}

/**
 * @brief Returns the current value of the FDSTATE field in the RXFSMSTAT1 register.
 *
 * The RXFSMSTAT1 register will be read and the FDSTATE field's value will be returned.
 *
 * @return The current value of the FDSTATE field in the RXFSMSTAT1 register.
 */
__INLINE uint8_t mdm_fdstate_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXFSMSTAT1_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Returns the current value of the GILENGTH field in the RXFSMSTAT1 register.
 *
 * The RXFSMSTAT1 register will be read and the GILENGTH field's value will be returned.
 *
 * @return The current value of the GILENGTH field in the RXFSMSTAT1 register.
 */
__INLINE uint8_t mdm_gilength_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXFSMSTAT1_ADDR);
    return ((localVal & ((uint32_t)0x003F0000)) >> 16);
}

/**
 * @brief Returns the current value of the STBCEN field in the RXFSMSTAT1 register.
 *
 * The RXFSMSTAT1 register will be read and the STBCEN field's value will be returned.
 *
 * @return The current value of the STBCEN field in the RXFSMSTAT1 register.
 */
__INLINE uint8_t mdm_stbcen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXFSMSTAT1_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Returns the current value of the TDBWREG field in the RXFSMSTAT1 register.
 *
 * The RXFSMSTAT1 register will be read and the TDBWREG field's value will be returned.
 *
 * @return The current value of the TDBWREG field in the RXFSMSTAT1 register.
 */
__INLINE uint8_t mdm_tdbwreg_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXFSMSTAT1_ADDR);
    return ((localVal & ((uint32_t)0x00000C00)) >> 10);
}

/**
 * @brief Returns the current value of the FDBWREG field in the RXFSMSTAT1 register.
 *
 * The RXFSMSTAT1 register will be read and the FDBWREG field's value will be returned.
 *
 * @return The current value of the FDBWREG field in the RXFSMSTAT1 register.
 */
__INLINE uint8_t mdm_fdbwreg_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXFSMSTAT1_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

/**
 * @brief Returns the current value of the RXMODESTAT field in the RXFSMSTAT1 register.
 *
 * The RXFSMSTAT1 register will be read and the RXMODESTAT field's value will be returned.
 *
 * @return The current value of the RXMODESTAT field in the RXFSMSTAT1 register.
 */
__INLINE uint8_t mdm_rxmodestat_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXFSMSTAT1_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

/**
 * @brief Returns the current value of the FDSYMBOLTYPE field in the RXFSMSTAT1 register.
 *
 * The RXFSMSTAT1 register will be read and the FDSYMBOLTYPE field's value will be returned.
 *
 * @return The current value of the FDSYMBOLTYPE field in the RXFSMSTAT1 register.
 */
__INLINE uint8_t mdm_fdsymboltype_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXFSMSTAT1_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

/// @}

/**
 * @name TXFSMSTAT1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     00       PhyTxAbortStat   0
 * </pre>
 *
 * @{
 */

/// Address of the TXFSMSTAT1 register
#define MDM_TXFSMSTAT1_ADDR   0x0100001C
/// Offset of the TXFSMSTAT1 register from the base address
#define MDM_TXFSMSTAT1_OFFSET 0x0000001C
/// Index of the TXFSMSTAT1 register
#define MDM_TXFSMSTAT1_INDEX  0x00000007
/// Reset value of the TXFSMSTAT1 register
#define MDM_TXFSMSTAT1_RESET  0x00000000

/**
 * @brief Returns the current value of the TXFSMSTAT1 register.
 * The TXFSMSTAT1 register will be read and its value returned.
 * @return The current value of the TXFSMSTAT1 register.
 */
__INLINE uint32_t mdm_txfsmstat1_get(void)
{
    return REG_PL_RD(MDM_TXFSMSTAT1_ADDR);
}

// field definitions
/// PHY_TX_ABORT_STAT field bit
#define MDM_PHY_TX_ABORT_STAT_BIT    ((uint32_t)0x00000001)
/// PHY_TX_ABORT_STAT field position
#define MDM_PHY_TX_ABORT_STAT_POS    0

/// PHY_TX_ABORT_STAT field reset value
#define MDM_PHY_TX_ABORT_STAT_RST    0x0

/**
 * @brief Returns the current value of the PhyTxAbortStat field in the TXFSMSTAT1 register.
 *
 * The TXFSMSTAT1 register will be read and the PhyTxAbortStat field's value will be returned.
 *
 * @return The current value of the PhyTxAbortStat field in the TXFSMSTAT1 register.
 */
__INLINE uint8_t mdm_phy_tx_abort_stat_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXFSMSTAT1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000001)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name ERRSTAT register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     06           TXOVERFLOW   0
 *     05          TXFORMATERR   0
 *     04           TXUNDERRUN   0
 *     03            AGCUNLOCK   0
 *     02          RXFORMATERR   0
 *     01             HTSIGCRC   0
 *     00           LSIGPARITY   0
 * </pre>
 *
 * @{
 */

/// Address of the ERRSTAT register
#define MDM_ERRSTAT_ADDR   0x01000020
/// Offset of the ERRSTAT register from the base address
#define MDM_ERRSTAT_OFFSET 0x00000020
/// Index of the ERRSTAT register
#define MDM_ERRSTAT_INDEX  0x00000008
/// Reset value of the ERRSTAT register
#define MDM_ERRSTAT_RESET  0x00000000

/**
 * @brief Returns the current value of the ERRSTAT register.
 * The ERRSTAT register will be read and its value returned.
 * @return The current value of the ERRSTAT register.
 */
__INLINE uint32_t mdm_errstat_get(void)
{
    return REG_PL_RD(MDM_ERRSTAT_ADDR);
}

// field definitions
/// TXOVERFLOW field bit
#define MDM_TXOVERFLOW_BIT     ((uint32_t)0x00000040)
/// TXOVERFLOW field position
#define MDM_TXOVERFLOW_POS     6
/// TXFORMATERR field bit
#define MDM_TXFORMATERR_BIT    ((uint32_t)0x00000020)
/// TXFORMATERR field position
#define MDM_TXFORMATERR_POS    5
/// TXUNDERRUN field bit
#define MDM_TXUNDERRUN_BIT     ((uint32_t)0x00000010)
/// TXUNDERRUN field position
#define MDM_TXUNDERRUN_POS     4
/// AGCUNLOCK field bit
#define MDM_AGCUNLOCK_BIT      ((uint32_t)0x00000008)
/// AGCUNLOCK field position
#define MDM_AGCUNLOCK_POS      3
/// RXFORMATERR field bit
#define MDM_RXFORMATERR_BIT    ((uint32_t)0x00000004)
/// RXFORMATERR field position
#define MDM_RXFORMATERR_POS    2
/// HTSIGCRC field bit
#define MDM_HTSIGCRC_BIT       ((uint32_t)0x00000002)
/// HTSIGCRC field position
#define MDM_HTSIGCRC_POS       1
/// LSIGPARITY field bit
#define MDM_LSIGPARITY_BIT     ((uint32_t)0x00000001)
/// LSIGPARITY field position
#define MDM_LSIGPARITY_POS     0

/// TXOVERFLOW field reset value
#define MDM_TXOVERFLOW_RST     0x0
/// TXFORMATERR field reset value
#define MDM_TXFORMATERR_RST    0x0
/// TXUNDERRUN field reset value
#define MDM_TXUNDERRUN_RST     0x0
/// AGCUNLOCK field reset value
#define MDM_AGCUNLOCK_RST      0x0
/// RXFORMATERR field reset value
#define MDM_RXFORMATERR_RST    0x0
/// HTSIGCRC field reset value
#define MDM_HTSIGCRC_RST       0x0
/// LSIGPARITY field reset value
#define MDM_LSIGPARITY_RST     0x0

/**
 * @brief Unpacks ERRSTAT's fields from current value of the ERRSTAT register.
 *
 * Reads the ERRSTAT register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txoverflow - Will be populated with the current value of this field from the register.
 * @param[out] txformaterr - Will be populated with the current value of this field from the register.
 * @param[out] txunderrun - Will be populated with the current value of this field from the register.
 * @param[out] agcunlock - Will be populated with the current value of this field from the register.
 * @param[out] rxformaterr - Will be populated with the current value of this field from the register.
 * @param[out] htsigcrc - Will be populated with the current value of this field from the register.
 * @param[out] lsigparity - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_errstat_unpack(uint8_t *txoverflow, uint8_t *txformaterr, uint8_t *txunderrun, uint8_t *agcunlock, uint8_t *rxformaterr, uint8_t *htsigcrc, uint8_t *lsigparity)
{
    uint32_t localVal = REG_PL_RD(MDM_ERRSTAT_ADDR);

    *txoverflow = (localVal & ((uint32_t)0x00000040)) >> 6;
    *txformaterr = (localVal & ((uint32_t)0x00000020)) >> 5;
    *txunderrun = (localVal & ((uint32_t)0x00000010)) >> 4;
    *agcunlock = (localVal & ((uint32_t)0x00000008)) >> 3;
    *rxformaterr = (localVal & ((uint32_t)0x00000004)) >> 2;
    *htsigcrc = (localVal & ((uint32_t)0x00000002)) >> 1;
    *lsigparity = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the TXOVERFLOW field in the ERRSTAT register.
 *
 * The ERRSTAT register will be read and the TXOVERFLOW field's value will be returned.
 *
 * @return The current value of the TXOVERFLOW field in the ERRSTAT register.
 */
__INLINE uint8_t mdm_txoverflow_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_ERRSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Returns the current value of the TXFORMATERR field in the ERRSTAT register.
 *
 * The ERRSTAT register will be read and the TXFORMATERR field's value will be returned.
 *
 * @return The current value of the TXFORMATERR field in the ERRSTAT register.
 */
__INLINE uint8_t mdm_txformaterr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_ERRSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Returns the current value of the TXUNDERRUN field in the ERRSTAT register.
 *
 * The ERRSTAT register will be read and the TXUNDERRUN field's value will be returned.
 *
 * @return The current value of the TXUNDERRUN field in the ERRSTAT register.
 */
__INLINE uint8_t mdm_txunderrun_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_ERRSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Returns the current value of the AGCUNLOCK field in the ERRSTAT register.
 *
 * The ERRSTAT register will be read and the AGCUNLOCK field's value will be returned.
 *
 * @return The current value of the AGCUNLOCK field in the ERRSTAT register.
 */
__INLINE uint8_t mdm_agcunlock_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_ERRSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Returns the current value of the RXFORMATERR field in the ERRSTAT register.
 *
 * The ERRSTAT register will be read and the RXFORMATERR field's value will be returned.
 *
 * @return The current value of the RXFORMATERR field in the ERRSTAT register.
 */
__INLINE uint8_t mdm_rxformaterr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_ERRSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Returns the current value of the HTSIGCRC field in the ERRSTAT register.
 *
 * The ERRSTAT register will be read and the HTSIGCRC field's value will be returned.
 *
 * @return The current value of the HTSIGCRC field in the ERRSTAT register.
 */
__INLINE uint8_t mdm_htsigcrc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_ERRSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Returns the current value of the LSIGPARITY field in the ERRSTAT register.
 *
 * The ERRSTAT register will be read and the LSIGPARITY field's value will be returned.
 *
 * @return The current value of the LSIGPARITY field in the ERRSTAT register.
 */
__INLINE uint8_t mdm_lsigparity_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_ERRSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/// @}

/**
 * @name IRQSTAT register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     14        IRQHTSIGVALID   0
 *     13         IRQLSIGVALID   0
 *     12        IRQTXOVERFLOW   0
 *     11        IRQTXUNDERRUN   0
 *     10        IRQCCA40SFALL   0
 *     09        IRQCCA40SRISE   0
 *     08             IRQTXEND   0
 *     07             IRQRXEND   0
 *     06        IRQCCA20PFALL   0
 *     05        IRQCCA20SFALL   0
 *     04        IRQCCA20PRISE   0
 *     03        IRQCCA20SRISE   0
 *     02          IRQPHYERROR   0
 *     01          IRQRADARDET   0
 *     00        IRQCCATIMEOUT   0
 * </pre>
 *
 * @{
 */

/// Address of the IRQSTAT register
#define MDM_IRQSTAT_ADDR   0x01000024
/// Offset of the IRQSTAT register from the base address
#define MDM_IRQSTAT_OFFSET 0x00000024
/// Index of the IRQSTAT register
#define MDM_IRQSTAT_INDEX  0x00000009
/// Reset value of the IRQSTAT register
#define MDM_IRQSTAT_RESET  0x00000000

/**
 * @brief Returns the current value of the IRQSTAT register.
 * The IRQSTAT register will be read and its value returned.
 * @return The current value of the IRQSTAT register.
 */
__INLINE uint32_t mdm_irqstat_get(void)
{
    return REG_PL_RD(MDM_IRQSTAT_ADDR);
}

// field definitions
/// IRQHTSIGVALID field bit
#define MDM_IRQHTSIGVALID_BIT    ((uint32_t)0x00004000)
/// IRQHTSIGVALID field position
#define MDM_IRQHTSIGVALID_POS    14
/// IRQLSIGVALID field bit
#define MDM_IRQLSIGVALID_BIT     ((uint32_t)0x00002000)
/// IRQLSIGVALID field position
#define MDM_IRQLSIGVALID_POS     13
/// IRQTXOVERFLOW field bit
#define MDM_IRQTXOVERFLOW_BIT    ((uint32_t)0x00001000)
/// IRQTXOVERFLOW field position
#define MDM_IRQTXOVERFLOW_POS    12
/// IRQTXUNDERRUN field bit
#define MDM_IRQTXUNDERRUN_BIT    ((uint32_t)0x00000800)
/// IRQTXUNDERRUN field position
#define MDM_IRQTXUNDERRUN_POS    11
/// IRQCCA40SFALL field bit
#define MDM_IRQCCA40SFALL_BIT    ((uint32_t)0x00000400)
/// IRQCCA40SFALL field position
#define MDM_IRQCCA40SFALL_POS    10
/// IRQCCA40SRISE field bit
#define MDM_IRQCCA40SRISE_BIT    ((uint32_t)0x00000200)
/// IRQCCA40SRISE field position
#define MDM_IRQCCA40SRISE_POS    9
/// IRQTXEND field bit
#define MDM_IRQTXEND_BIT         ((uint32_t)0x00000100)
/// IRQTXEND field position
#define MDM_IRQTXEND_POS         8
/// IRQRXEND field bit
#define MDM_IRQRXEND_BIT         ((uint32_t)0x00000080)
/// IRQRXEND field position
#define MDM_IRQRXEND_POS         7
/// IRQCCA20PFALL field bit
#define MDM_IRQCCA20PFALL_BIT    ((uint32_t)0x00000040)
/// IRQCCA20PFALL field position
#define MDM_IRQCCA20PFALL_POS    6
/// IRQCCA20SFALL field bit
#define MDM_IRQCCA20SFALL_BIT    ((uint32_t)0x00000020)
/// IRQCCA20SFALL field position
#define MDM_IRQCCA20SFALL_POS    5
/// IRQCCA20PRISE field bit
#define MDM_IRQCCA20PRISE_BIT    ((uint32_t)0x00000010)
/// IRQCCA20PRISE field position
#define MDM_IRQCCA20PRISE_POS    4
/// IRQCCA20SRISE field bit
#define MDM_IRQCCA20SRISE_BIT    ((uint32_t)0x00000008)
/// IRQCCA20SRISE field position
#define MDM_IRQCCA20SRISE_POS    3
/// IRQPHYERROR field bit
#define MDM_IRQPHYERROR_BIT      ((uint32_t)0x00000004)
/// IRQPHYERROR field position
#define MDM_IRQPHYERROR_POS      2
/// IRQRADARDET field bit
#define MDM_IRQRADARDET_BIT      ((uint32_t)0x00000002)
/// IRQRADARDET field position
#define MDM_IRQRADARDET_POS      1
/// IRQCCATIMEOUT field bit
#define MDM_IRQCCATIMEOUT_BIT    ((uint32_t)0x00000001)
/// IRQCCATIMEOUT field position
#define MDM_IRQCCATIMEOUT_POS    0

/// IRQHTSIGVALID field reset value
#define MDM_IRQHTSIGVALID_RST    0x0
/// IRQLSIGVALID field reset value
#define MDM_IRQLSIGVALID_RST     0x0
/// IRQTXOVERFLOW field reset value
#define MDM_IRQTXOVERFLOW_RST    0x0
/// IRQTXUNDERRUN field reset value
#define MDM_IRQTXUNDERRUN_RST    0x0
/// IRQCCA40SFALL field reset value
#define MDM_IRQCCA40SFALL_RST    0x0
/// IRQCCA40SRISE field reset value
#define MDM_IRQCCA40SRISE_RST    0x0
/// IRQTXEND field reset value
#define MDM_IRQTXEND_RST         0x0
/// IRQRXEND field reset value
#define MDM_IRQRXEND_RST         0x0
/// IRQCCA20PFALL field reset value
#define MDM_IRQCCA20PFALL_RST    0x0
/// IRQCCA20SFALL field reset value
#define MDM_IRQCCA20SFALL_RST    0x0
/// IRQCCA20PRISE field reset value
#define MDM_IRQCCA20PRISE_RST    0x0
/// IRQCCA20SRISE field reset value
#define MDM_IRQCCA20SRISE_RST    0x0
/// IRQPHYERROR field reset value
#define MDM_IRQPHYERROR_RST      0x0
/// IRQRADARDET field reset value
#define MDM_IRQRADARDET_RST      0x0
/// IRQCCATIMEOUT field reset value
#define MDM_IRQCCATIMEOUT_RST    0x0

/**
 * @brief Unpacks IRQSTAT's fields from current value of the IRQSTAT register.
 *
 * Reads the IRQSTAT register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] irqhtsigvalid - Will be populated with the current value of this field from the register.
 * @param[out] irqlsigvalid - Will be populated with the current value of this field from the register.
 * @param[out] irqtxoverflow - Will be populated with the current value of this field from the register.
 * @param[out] irqtxunderrun - Will be populated with the current value of this field from the register.
 * @param[out] irqcca40sfall - Will be populated with the current value of this field from the register.
 * @param[out] irqcca40srise - Will be populated with the current value of this field from the register.
 * @param[out] irqtxend - Will be populated with the current value of this field from the register.
 * @param[out] irqrxend - Will be populated with the current value of this field from the register.
 * @param[out] irqcca20pfall - Will be populated with the current value of this field from the register.
 * @param[out] irqcca20sfall - Will be populated with the current value of this field from the register.
 * @param[out] irqcca20prise - Will be populated with the current value of this field from the register.
 * @param[out] irqcca20srise - Will be populated with the current value of this field from the register.
 * @param[out] irqphyerror - Will be populated with the current value of this field from the register.
 * @param[out] irqradardet - Will be populated with the current value of this field from the register.
 * @param[out] irqccatimeout - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_irqstat_unpack(uint8_t *irqhtsigvalid, uint8_t *irqlsigvalid, uint8_t *irqtxoverflow, uint8_t *irqtxunderrun, uint8_t *irqcca40sfall, uint8_t *irqcca40srise, uint8_t *irqtxend, uint8_t *irqrxend, uint8_t *irqcca20pfall, uint8_t *irqcca20sfall, uint8_t *irqcca20prise, uint8_t *irqcca20srise, uint8_t *irqphyerror, uint8_t *irqradardet, uint8_t *irqccatimeout)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQSTAT_ADDR);

    *irqhtsigvalid = (localVal & ((uint32_t)0x00004000)) >> 14;
    *irqlsigvalid = (localVal & ((uint32_t)0x00002000)) >> 13;
    *irqtxoverflow = (localVal & ((uint32_t)0x00001000)) >> 12;
    *irqtxunderrun = (localVal & ((uint32_t)0x00000800)) >> 11;
    *irqcca40sfall = (localVal & ((uint32_t)0x00000400)) >> 10;
    *irqcca40srise = (localVal & ((uint32_t)0x00000200)) >> 9;
    *irqtxend = (localVal & ((uint32_t)0x00000100)) >> 8;
    *irqrxend = (localVal & ((uint32_t)0x00000080)) >> 7;
    *irqcca20pfall = (localVal & ((uint32_t)0x00000040)) >> 6;
    *irqcca20sfall = (localVal & ((uint32_t)0x00000020)) >> 5;
    *irqcca20prise = (localVal & ((uint32_t)0x00000010)) >> 4;
    *irqcca20srise = (localVal & ((uint32_t)0x00000008)) >> 3;
    *irqphyerror = (localVal & ((uint32_t)0x00000004)) >> 2;
    *irqradardet = (localVal & ((uint32_t)0x00000002)) >> 1;
    *irqccatimeout = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the IRQHTSIGVALID field in the IRQSTAT register.
 *
 * The IRQSTAT register will be read and the IRQHTSIGVALID field's value will be returned.
 *
 * @return The current value of the IRQHTSIGVALID field in the IRQSTAT register.
 */
__INLINE uint8_t mdm_irqhtsigvalid_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Returns the current value of the IRQLSIGVALID field in the IRQSTAT register.
 *
 * The IRQSTAT register will be read and the IRQLSIGVALID field's value will be returned.
 *
 * @return The current value of the IRQLSIGVALID field in the IRQSTAT register.
 */
__INLINE uint8_t mdm_irqlsigvalid_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Returns the current value of the IRQTXOVERFLOW field in the IRQSTAT register.
 *
 * The IRQSTAT register will be read and the IRQTXOVERFLOW field's value will be returned.
 *
 * @return The current value of the IRQTXOVERFLOW field in the IRQSTAT register.
 */
__INLINE uint8_t mdm_irqtxoverflow_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Returns the current value of the IRQTXUNDERRUN field in the IRQSTAT register.
 *
 * The IRQSTAT register will be read and the IRQTXUNDERRUN field's value will be returned.
 *
 * @return The current value of the IRQTXUNDERRUN field in the IRQSTAT register.
 */
__INLINE uint8_t mdm_irqtxunderrun_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Returns the current value of the IRQCCA40SFALL field in the IRQSTAT register.
 *
 * The IRQSTAT register will be read and the IRQCCA40SFALL field's value will be returned.
 *
 * @return The current value of the IRQCCA40SFALL field in the IRQSTAT register.
 */
__INLINE uint8_t mdm_irqcca40sfall_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Returns the current value of the IRQCCA40SRISE field in the IRQSTAT register.
 *
 * The IRQSTAT register will be read and the IRQCCA40SRISE field's value will be returned.
 *
 * @return The current value of the IRQCCA40SRISE field in the IRQSTAT register.
 */
__INLINE uint8_t mdm_irqcca40srise_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Returns the current value of the IRQTXEND field in the IRQSTAT register.
 *
 * The IRQSTAT register will be read and the IRQTXEND field's value will be returned.
 *
 * @return The current value of the IRQTXEND field in the IRQSTAT register.
 */
__INLINE uint8_t mdm_irqtxend_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Returns the current value of the IRQRXEND field in the IRQSTAT register.
 *
 * The IRQSTAT register will be read and the IRQRXEND field's value will be returned.
 *
 * @return The current value of the IRQRXEND field in the IRQSTAT register.
 */
__INLINE uint8_t mdm_irqrxend_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Returns the current value of the IRQCCA20PFALL field in the IRQSTAT register.
 *
 * The IRQSTAT register will be read and the IRQCCA20PFALL field's value will be returned.
 *
 * @return The current value of the IRQCCA20PFALL field in the IRQSTAT register.
 */
__INLINE uint8_t mdm_irqcca20pfall_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Returns the current value of the IRQCCA20SFALL field in the IRQSTAT register.
 *
 * The IRQSTAT register will be read and the IRQCCA20SFALL field's value will be returned.
 *
 * @return The current value of the IRQCCA20SFALL field in the IRQSTAT register.
 */
__INLINE uint8_t mdm_irqcca20sfall_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Returns the current value of the IRQCCA20PRISE field in the IRQSTAT register.
 *
 * The IRQSTAT register will be read and the IRQCCA20PRISE field's value will be returned.
 *
 * @return The current value of the IRQCCA20PRISE field in the IRQSTAT register.
 */
__INLINE uint8_t mdm_irqcca20prise_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Returns the current value of the IRQCCA20SRISE field in the IRQSTAT register.
 *
 * The IRQSTAT register will be read and the IRQCCA20SRISE field's value will be returned.
 *
 * @return The current value of the IRQCCA20SRISE field in the IRQSTAT register.
 */
__INLINE uint8_t mdm_irqcca20srise_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Returns the current value of the IRQPHYERROR field in the IRQSTAT register.
 *
 * The IRQSTAT register will be read and the IRQPHYERROR field's value will be returned.
 *
 * @return The current value of the IRQPHYERROR field in the IRQSTAT register.
 */
__INLINE uint8_t mdm_irqphyerror_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Returns the current value of the IRQRADARDET field in the IRQSTAT register.
 *
 * The IRQSTAT register will be read and the IRQRADARDET field's value will be returned.
 *
 * @return The current value of the IRQRADARDET field in the IRQSTAT register.
 */
__INLINE uint8_t mdm_irqradardet_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Returns the current value of the IRQCCATIMEOUT field in the IRQSTAT register.
 *
 * The IRQSTAT register will be read and the IRQCCATIMEOUT field's value will be returned.
 *
 * @return The current value of the IRQCCATIMEOUT field in the IRQSTAT register.
 */
__INLINE uint8_t mdm_irqccatimeout_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_IRQSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/// @}

/**
 * @name RXVECTOR0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:12            RXLEGRATE   0x0
 *  11:00          RXLEGLENGTH   0x0
 * </pre>
 *
 * @{
 */

/// Address of the RXVECTOR0 register
#define MDM_RXVECTOR0_ADDR   0x01000028
/// Offset of the RXVECTOR0 register from the base address
#define MDM_RXVECTOR0_OFFSET 0x00000028
/// Index of the RXVECTOR0 register
#define MDM_RXVECTOR0_INDEX  0x0000000A
/// Reset value of the RXVECTOR0 register
#define MDM_RXVECTOR0_RESET  0x00000000

/**
 * @brief Returns the current value of the RXVECTOR0 register.
 * The RXVECTOR0 register will be read and its value returned.
 * @return The current value of the RXVECTOR0 register.
 */
__INLINE uint32_t mdm_rxvector0_get(void)
{
    return REG_PL_RD(MDM_RXVECTOR0_ADDR);
}

// field definitions
/// RXLEGRATE field mask
#define MDM_RXLEGRATE_MASK     ((uint32_t)0x0000F000)
/// RXLEGRATE field LSB position
#define MDM_RXLEGRATE_LSB      12
/// RXLEGRATE field width
#define MDM_RXLEGRATE_WIDTH    ((uint32_t)0x00000004)
/// RXLEGLENGTH field mask
#define MDM_RXLEGLENGTH_MASK   ((uint32_t)0x00000FFF)
/// RXLEGLENGTH field LSB position
#define MDM_RXLEGLENGTH_LSB    0
/// RXLEGLENGTH field width
#define MDM_RXLEGLENGTH_WIDTH  ((uint32_t)0x0000000C)

/// RXLEGRATE field reset value
#define MDM_RXLEGRATE_RST      0x0
/// RXLEGLENGTH field reset value
#define MDM_RXLEGLENGTH_RST    0x0

/**
 * @brief Unpacks RXVECTOR0's fields from current value of the RXVECTOR0 register.
 *
 * Reads the RXVECTOR0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxlegrate - Will be populated with the current value of this field from the register.
 * @param[out] rxleglength - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxvector0_unpack(uint8_t *rxlegrate, uint16_t *rxleglength)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR0_ADDR);

    *rxlegrate = (localVal & ((uint32_t)0x0000F000)) >> 12;
    *rxleglength = (localVal & ((uint32_t)0x00000FFF)) >> 0;
}

/**
 * @brief Returns the current value of the RXLEGRATE field in the RXVECTOR0 register.
 *
 * The RXVECTOR0 register will be read and the RXLEGRATE field's value will be returned.
 *
 * @return The current value of the RXLEGRATE field in the RXVECTOR0 register.
 */
__INLINE uint8_t mdm_rxlegrate_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR0_ADDR);
    return ((localVal & ((uint32_t)0x0000F000)) >> 12);
}

/**
 * @brief Returns the current value of the RXLEGLENGTH field in the RXVECTOR0 register.
 *
 * The RXVECTOR0 register will be read and the RXLEGLENGTH field's value will be returned.
 *
 * @return The current value of the RXLEGLENGTH field in the RXVECTOR0 register.
 */
__INLINE uint16_t mdm_rxleglength_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR0_ADDR);
    return ((localVal & ((uint32_t)0x00000FFF)) >> 0);
}

/// @}

/**
 * @name RXVECTOR1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31            RXPRETYPE   0
 *  30:24                RXMCS   0x0
 *     23          RXSMOOTHING   0
 *  22:21               RXSTBC   0x0
 *     20            RXSHORTGI   0
 *  19:00           RXHTLENGTH   0x0
 * </pre>
 *
 * @{
 */

/// Address of the RXVECTOR1 register
#define MDM_RXVECTOR1_ADDR   0x0100002C
/// Offset of the RXVECTOR1 register from the base address
#define MDM_RXVECTOR1_OFFSET 0x0000002C
/// Index of the RXVECTOR1 register
#define MDM_RXVECTOR1_INDEX  0x0000000B
/// Reset value of the RXVECTOR1 register
#define MDM_RXVECTOR1_RESET  0x00000000

/**
 * @brief Returns the current value of the RXVECTOR1 register.
 * The RXVECTOR1 register will be read and its value returned.
 * @return The current value of the RXVECTOR1 register.
 */
__INLINE uint32_t mdm_rxvector1_get(void)
{
    return REG_PL_RD(MDM_RXVECTOR1_ADDR);
}

// field definitions
/// RXPRETYPE field bit
#define MDM_RXPRETYPE_BIT      ((uint32_t)0x80000000)
/// RXPRETYPE field position
#define MDM_RXPRETYPE_POS      31
/// RXMCS field mask
#define MDM_RXMCS_MASK         ((uint32_t)0x7F000000)
/// RXMCS field LSB position
#define MDM_RXMCS_LSB          24
/// RXMCS field width
#define MDM_RXMCS_WIDTH        ((uint32_t)0x00000007)
/// RXSMOOTHING field bit
#define MDM_RXSMOOTHING_BIT    ((uint32_t)0x00800000)
/// RXSMOOTHING field position
#define MDM_RXSMOOTHING_POS    23
/// RXSTBC field mask
#define MDM_RXSTBC_MASK        ((uint32_t)0x00600000)
/// RXSTBC field LSB position
#define MDM_RXSTBC_LSB         21
/// RXSTBC field width
#define MDM_RXSTBC_WIDTH       ((uint32_t)0x00000002)
/// RXSHORTGI field bit
#define MDM_RXSHORTGI_BIT      ((uint32_t)0x00100000)
/// RXSHORTGI field position
#define MDM_RXSHORTGI_POS      20
/// RXHTLENGTH field mask
#define MDM_RXHTLENGTH_MASK    ((uint32_t)0x000FFFFF)
/// RXHTLENGTH field LSB position
#define MDM_RXHTLENGTH_LSB     0
/// RXHTLENGTH field width
#define MDM_RXHTLENGTH_WIDTH   ((uint32_t)0x00000014)

/// RXPRETYPE field reset value
#define MDM_RXPRETYPE_RST      0x0
/// RXMCS field reset value
#define MDM_RXMCS_RST          0x0
/// RXSMOOTHING field reset value
#define MDM_RXSMOOTHING_RST    0x0
/// RXSTBC field reset value
#define MDM_RXSTBC_RST         0x0
/// RXSHORTGI field reset value
#define MDM_RXSHORTGI_RST      0x0
/// RXHTLENGTH field reset value
#define MDM_RXHTLENGTH_RST     0x0

/**
 * @brief Unpacks RXVECTOR1's fields from current value of the RXVECTOR1 register.
 *
 * Reads the RXVECTOR1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxpretype - Will be populated with the current value of this field from the register.
 * @param[out] rxmcs - Will be populated with the current value of this field from the register.
 * @param[out] rxsmoothing - Will be populated with the current value of this field from the register.
 * @param[out] rxstbc - Will be populated with the current value of this field from the register.
 * @param[out] rxshortgi - Will be populated with the current value of this field from the register.
 * @param[out] rxhtlength - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxvector1_unpack(uint8_t *rxpretype, uint8_t *rxmcs, uint8_t *rxsmoothing, uint8_t *rxstbc, uint8_t *rxshortgi, uint32_t *rxhtlength)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR1_ADDR);

    *rxpretype = (localVal & ((uint32_t)0x80000000)) >> 31;
    *rxmcs = (localVal & ((uint32_t)0x7F000000)) >> 24;
    *rxsmoothing = (localVal & ((uint32_t)0x00800000)) >> 23;
    *rxstbc = (localVal & ((uint32_t)0x00600000)) >> 21;
    *rxshortgi = (localVal & ((uint32_t)0x00100000)) >> 20;
    *rxhtlength = (localVal & ((uint32_t)0x000FFFFF)) >> 0;
}

/**
 * @brief Returns the current value of the RXPRETYPE field in the RXVECTOR1 register.
 *
 * The RXVECTOR1 register will be read and the RXPRETYPE field's value will be returned.
 *
 * @return The current value of the RXPRETYPE field in the RXVECTOR1 register.
 */
__INLINE uint8_t mdm_rxpretype_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR1_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Returns the current value of the RXMCS field in the RXVECTOR1 register.
 *
 * The RXVECTOR1 register will be read and the RXMCS field's value will be returned.
 *
 * @return The current value of the RXMCS field in the RXVECTOR1 register.
 */
__INLINE uint8_t mdm_rxmcs_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR1_ADDR);
    return ((localVal & ((uint32_t)0x7F000000)) >> 24);
}

/**
 * @brief Returns the current value of the RXSMOOTHING field in the RXVECTOR1 register.
 *
 * The RXVECTOR1 register will be read and the RXSMOOTHING field's value will be returned.
 *
 * @return The current value of the RXSMOOTHING field in the RXVECTOR1 register.
 */
__INLINE uint8_t mdm_rxsmoothing_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR1_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Returns the current value of the RXSTBC field in the RXVECTOR1 register.
 *
 * The RXVECTOR1 register will be read and the RXSTBC field's value will be returned.
 *
 * @return The current value of the RXSTBC field in the RXVECTOR1 register.
 */
__INLINE uint8_t mdm_rxstbc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR1_ADDR);
    return ((localVal & ((uint32_t)0x00600000)) >> 21);
}

/**
 * @brief Returns the current value of the RXSHORTGI field in the RXVECTOR1 register.
 *
 * The RXVECTOR1 register will be read and the RXSHORTGI field's value will be returned.
 *
 * @return The current value of the RXSHORTGI field in the RXVECTOR1 register.
 */
__INLINE uint8_t mdm_rxshortgi_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR1_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Returns the current value of the RXHTLENGTH field in the RXVECTOR1 register.
 *
 * The RXVECTOR1 register will be read and the RXHTLENGTH field's value will be returned.
 *
 * @return The current value of the RXHTLENGTH field in the RXVECTOR1 register.
 */
__INLINE uint32_t mdm_rxhtlength_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR1_ADDR);
    return ((localVal & ((uint32_t)0x000FFFFF)) >> 0);
}

/// @}

/**
 * @name RXVECTOR2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:24         RXANTENNASET   0x0
 *     23     RXDOZENOTALLOWED   0
 *     22              RXDYNBW   0
 *     21          RXFECCODING   0
 *     20             RXAGGREG   0
 *  17:16           RXNUMEXTSS   0x0
 *     12           RXSOUNDING   0
 *  10:08             RXNUMSTS   0x0
 *  05:04               RXCHBW   0x0
 *  02:00             RXFORMAT   0x0
 * </pre>
 *
 * @{
 */

/// Address of the RXVECTOR2 register
#define MDM_RXVECTOR2_ADDR   0x01000030
/// Offset of the RXVECTOR2 register from the base address
#define MDM_RXVECTOR2_OFFSET 0x00000030
/// Index of the RXVECTOR2 register
#define MDM_RXVECTOR2_INDEX  0x0000000C
/// Reset value of the RXVECTOR2 register
#define MDM_RXVECTOR2_RESET  0x00000000

/**
 * @brief Returns the current value of the RXVECTOR2 register.
 * The RXVECTOR2 register will be read and its value returned.
 * @return The current value of the RXVECTOR2 register.
 */
__INLINE uint32_t mdm_rxvector2_get(void)
{
    return REG_PL_RD(MDM_RXVECTOR2_ADDR);
}

// field definitions
/// RXANTENNASET field mask
#define MDM_RXANTENNASET_MASK       ((uint32_t)0xFF000000)
/// RXANTENNASET field LSB position
#define MDM_RXANTENNASET_LSB        24
/// RXANTENNASET field width
#define MDM_RXANTENNASET_WIDTH      ((uint32_t)0x00000008)
/// RXDOZENOTALLOWED field bit
#define MDM_RXDOZENOTALLOWED_BIT    ((uint32_t)0x00800000)
/// RXDOZENOTALLOWED field position
#define MDM_RXDOZENOTALLOWED_POS    23
/// RXDYNBW field bit
#define MDM_RXDYNBW_BIT             ((uint32_t)0x00400000)
/// RXDYNBW field position
#define MDM_RXDYNBW_POS             22
/// RXFECCODING field bit
#define MDM_RXFECCODING_BIT         ((uint32_t)0x00200000)
/// RXFECCODING field position
#define MDM_RXFECCODING_POS         21
/// RXAGGREG field bit
#define MDM_RXAGGREG_BIT            ((uint32_t)0x00100000)
/// RXAGGREG field position
#define MDM_RXAGGREG_POS            20
/// RXNUMEXTSS field mask
#define MDM_RXNUMEXTSS_MASK         ((uint32_t)0x00030000)
/// RXNUMEXTSS field LSB position
#define MDM_RXNUMEXTSS_LSB          16
/// RXNUMEXTSS field width
#define MDM_RXNUMEXTSS_WIDTH        ((uint32_t)0x00000002)
/// RXSOUNDING field bit
#define MDM_RXSOUNDING_BIT          ((uint32_t)0x00001000)
/// RXSOUNDING field position
#define MDM_RXSOUNDING_POS          12
/// RXNUMSTS field mask
#define MDM_RXNUMSTS_MASK           ((uint32_t)0x00000700)
/// RXNUMSTS field LSB position
#define MDM_RXNUMSTS_LSB            8
/// RXNUMSTS field width
#define MDM_RXNUMSTS_WIDTH          ((uint32_t)0x00000003)
/// RXCHBW field mask
#define MDM_RXCHBW_MASK             ((uint32_t)0x00000030)
/// RXCHBW field LSB position
#define MDM_RXCHBW_LSB              4
/// RXCHBW field width
#define MDM_RXCHBW_WIDTH            ((uint32_t)0x00000002)
/// RXFORMAT field mask
#define MDM_RXFORMAT_MASK           ((uint32_t)0x00000007)
/// RXFORMAT field LSB position
#define MDM_RXFORMAT_LSB            0
/// RXFORMAT field width
#define MDM_RXFORMAT_WIDTH          ((uint32_t)0x00000003)

/// RXANTENNASET field reset value
#define MDM_RXANTENNASET_RST        0x0
/// RXDOZENOTALLOWED field reset value
#define MDM_RXDOZENOTALLOWED_RST    0x0
/// RXDYNBW field reset value
#define MDM_RXDYNBW_RST             0x0
/// RXFECCODING field reset value
#define MDM_RXFECCODING_RST         0x0
/// RXAGGREG field reset value
#define MDM_RXAGGREG_RST            0x0
/// RXNUMEXTSS field reset value
#define MDM_RXNUMEXTSS_RST          0x0
/// RXSOUNDING field reset value
#define MDM_RXSOUNDING_RST          0x0
/// RXNUMSTS field reset value
#define MDM_RXNUMSTS_RST            0x0
/// RXCHBW field reset value
#define MDM_RXCHBW_RST              0x0
/// RXFORMAT field reset value
#define MDM_RXFORMAT_RST            0x0

/**
 * @brief Unpacks RXVECTOR2's fields from current value of the RXVECTOR2 register.
 *
 * Reads the RXVECTOR2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxantennaset - Will be populated with the current value of this field from the register.
 * @param[out] rxdozenotallowed - Will be populated with the current value of this field from the register.
 * @param[out] rxdynbw - Will be populated with the current value of this field from the register.
 * @param[out] rxfeccoding - Will be populated with the current value of this field from the register.
 * @param[out] rxaggreg - Will be populated with the current value of this field from the register.
 * @param[out] rxnumextss - Will be populated with the current value of this field from the register.
 * @param[out] rxsounding - Will be populated with the current value of this field from the register.
 * @param[out] rxnumsts - Will be populated with the current value of this field from the register.
 * @param[out] rxchbw - Will be populated with the current value of this field from the register.
 * @param[out] rxformat - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxvector2_unpack(uint8_t *rxantennaset, uint8_t *rxdozenotallowed, uint8_t *rxdynbw, uint8_t *rxfeccoding, uint8_t *rxaggreg, uint8_t *rxnumextss, uint8_t *rxsounding, uint8_t *rxnumsts, uint8_t *rxchbw, uint8_t *rxformat)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR2_ADDR);

    *rxantennaset = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *rxdozenotallowed = (localVal & ((uint32_t)0x00800000)) >> 23;
    *rxdynbw = (localVal & ((uint32_t)0x00400000)) >> 22;
    *rxfeccoding = (localVal & ((uint32_t)0x00200000)) >> 21;
    *rxaggreg = (localVal & ((uint32_t)0x00100000)) >> 20;
    *rxnumextss = (localVal & ((uint32_t)0x00030000)) >> 16;
    *rxsounding = (localVal & ((uint32_t)0x00001000)) >> 12;
    *rxnumsts = (localVal & ((uint32_t)0x00000700)) >> 8;
    *rxchbw = (localVal & ((uint32_t)0x00000030)) >> 4;
    *rxformat = (localVal & ((uint32_t)0x00000007)) >> 0;
}

/**
 * @brief Returns the current value of the RXANTENNASET field in the RXVECTOR2 register.
 *
 * The RXVECTOR2 register will be read and the RXANTENNASET field's value will be returned.
 *
 * @return The current value of the RXANTENNASET field in the RXVECTOR2 register.
 */
__INLINE uint8_t mdm_rxantennaset_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR2_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Returns the current value of the RXDOZENOTALLOWED field in the RXVECTOR2 register.
 *
 * The RXVECTOR2 register will be read and the RXDOZENOTALLOWED field's value will be returned.
 *
 * @return The current value of the RXDOZENOTALLOWED field in the RXVECTOR2 register.
 */
__INLINE uint8_t mdm_rxdozenotallowed_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR2_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Returns the current value of the RXDYNBW field in the RXVECTOR2 register.
 *
 * The RXVECTOR2 register will be read and the RXDYNBW field's value will be returned.
 *
 * @return The current value of the RXDYNBW field in the RXVECTOR2 register.
 */
__INLINE uint8_t mdm_rxdynbw_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR2_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Returns the current value of the RXFECCODING field in the RXVECTOR2 register.
 *
 * The RXVECTOR2 register will be read and the RXFECCODING field's value will be returned.
 *
 * @return The current value of the RXFECCODING field in the RXVECTOR2 register.
 */
__INLINE uint8_t mdm_rxfeccoding_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR2_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Returns the current value of the RXAGGREG field in the RXVECTOR2 register.
 *
 * The RXVECTOR2 register will be read and the RXAGGREG field's value will be returned.
 *
 * @return The current value of the RXAGGREG field in the RXVECTOR2 register.
 */
__INLINE uint8_t mdm_rxaggreg_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR2_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Returns the current value of the RXNUMEXTSS field in the RXVECTOR2 register.
 *
 * The RXVECTOR2 register will be read and the RXNUMEXTSS field's value will be returned.
 *
 * @return The current value of the RXNUMEXTSS field in the RXVECTOR2 register.
 */
__INLINE uint8_t mdm_rxnumextss_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR2_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

/**
 * @brief Returns the current value of the RXSOUNDING field in the RXVECTOR2 register.
 *
 * The RXVECTOR2 register will be read and the RXSOUNDING field's value will be returned.
 *
 * @return The current value of the RXSOUNDING field in the RXVECTOR2 register.
 */
__INLINE uint8_t mdm_rxsounding_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR2_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Returns the current value of the RXNUMSTS field in the RXVECTOR2 register.
 *
 * The RXVECTOR2 register will be read and the RXNUMSTS field's value will be returned.
 *
 * @return The current value of the RXNUMSTS field in the RXVECTOR2 register.
 */
__INLINE uint8_t mdm_rxnumsts_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR2_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

/**
 * @brief Returns the current value of the RXCHBW field in the RXVECTOR2 register.
 *
 * The RXVECTOR2 register will be read and the RXCHBW field's value will be returned.
 *
 * @return The current value of the RXCHBW field in the RXVECTOR2 register.
 */
__INLINE uint8_t mdm_rxchbw_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR2_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

/**
 * @brief Returns the current value of the RXFORMAT field in the RXVECTOR2 register.
 *
 * The RXVECTOR2 register will be read and the RXFORMAT field's value will be returned.
 *
 * @return The current value of the RXFORMAT field in the RXVECTOR2 register.
 */
__INLINE uint8_t mdm_rxformat_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR2_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

/// @}

/**
 * @name RXVECTOR3 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:24                RSSI4   0x0
 *  23:16                RSSI3   0x0
 *  15:08                RSSI2   0x0
 *  07:00                RSSI1   0x0
 * </pre>
 *
 * @{
 */

/// Address of the RXVECTOR3 register
#define MDM_RXVECTOR3_ADDR   0x01000034
/// Offset of the RXVECTOR3 register from the base address
#define MDM_RXVECTOR3_OFFSET 0x00000034
/// Index of the RXVECTOR3 register
#define MDM_RXVECTOR3_INDEX  0x0000000D
/// Reset value of the RXVECTOR3 register
#define MDM_RXVECTOR3_RESET  0x00000000

/**
 * @brief Returns the current value of the RXVECTOR3 register.
 * The RXVECTOR3 register will be read and its value returned.
 * @return The current value of the RXVECTOR3 register.
 */
__INLINE uint32_t mdm_rxvector3_get(void)
{
    return REG_PL_RD(MDM_RXVECTOR3_ADDR);
}

// field definitions
/// RSSI4 field mask
#define MDM_RSSI4_MASK   ((uint32_t)0xFF000000)
/// RSSI4 field LSB position
#define MDM_RSSI4_LSB    24
/// RSSI4 field width
#define MDM_RSSI4_WIDTH  ((uint32_t)0x00000008)
/// RSSI3 field mask
#define MDM_RSSI3_MASK   ((uint32_t)0x00FF0000)
/// RSSI3 field LSB position
#define MDM_RSSI3_LSB    16
/// RSSI3 field width
#define MDM_RSSI3_WIDTH  ((uint32_t)0x00000008)
/// RSSI2 field mask
#define MDM_RSSI2_MASK   ((uint32_t)0x0000FF00)
/// RSSI2 field LSB position
#define MDM_RSSI2_LSB    8
/// RSSI2 field width
#define MDM_RSSI2_WIDTH  ((uint32_t)0x00000008)
/// RSSI1 field mask
#define MDM_RSSI1_MASK   ((uint32_t)0x000000FF)
/// RSSI1 field LSB position
#define MDM_RSSI1_LSB    0
/// RSSI1 field width
#define MDM_RSSI1_WIDTH  ((uint32_t)0x00000008)

/// RSSI4 field reset value
#define MDM_RSSI4_RST    0x0
/// RSSI3 field reset value
#define MDM_RSSI3_RST    0x0
/// RSSI2 field reset value
#define MDM_RSSI2_RST    0x0
/// RSSI1 field reset value
#define MDM_RSSI1_RST    0x0

/**
 * @brief Unpacks RXVECTOR3's fields from current value of the RXVECTOR3 register.
 *
 * Reads the RXVECTOR3 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rssi4 - Will be populated with the current value of this field from the register.
 * @param[out] rssi3 - Will be populated with the current value of this field from the register.
 * @param[out] rssi2 - Will be populated with the current value of this field from the register.
 * @param[out] rssi1 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxvector3_unpack(uint8_t *rssi4, uint8_t *rssi3, uint8_t *rssi2, uint8_t *rssi1)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR3_ADDR);

    *rssi4 = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *rssi3 = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *rssi2 = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *rssi1 = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the RSSI4 field in the RXVECTOR3 register.
 *
 * The RXVECTOR3 register will be read and the RSSI4 field's value will be returned.
 *
 * @return The current value of the RSSI4 field in the RXVECTOR3 register.
 */
__INLINE uint8_t mdm_rssi4_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR3_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Returns the current value of the RSSI3 field in the RXVECTOR3 register.
 *
 * The RXVECTOR3 register will be read and the RSSI3 field's value will be returned.
 *
 * @return The current value of the RSSI3 field in the RXVECTOR3 register.
 */
__INLINE uint8_t mdm_rssi3_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR3_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Returns the current value of the RSSI2 field in the RXVECTOR3 register.
 *
 * The RXVECTOR3 register will be read and the RSSI2 field's value will be returned.
 *
 * @return The current value of the RSSI2 field in the RXVECTOR3 register.
 */
__INLINE uint8_t mdm_rssi2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR3_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Returns the current value of the RSSI1 field in the RXVECTOR3 register.
 *
 * The RXVECTOR3 register will be read and the RSSI1 field's value will be returned.
 *
 * @return The current value of the RSSI1 field in the RXVECTOR3 register.
 */
__INLINE uint8_t mdm_rssi1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR3_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/// @}

/**
 * @name RXVECTOR4 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:24            RXGROUPID   0x0
 *  20:12         RXPARTIALAID   0x0
 *  07:00                 RCPI   0x0
 * </pre>
 *
 * @{
 */

/// Address of the RXVECTOR4 register
#define MDM_RXVECTOR4_ADDR   0x01000038
/// Offset of the RXVECTOR4 register from the base address
#define MDM_RXVECTOR4_OFFSET 0x00000038
/// Index of the RXVECTOR4 register
#define MDM_RXVECTOR4_INDEX  0x0000000E
/// Reset value of the RXVECTOR4 register
#define MDM_RXVECTOR4_RESET  0x00000000

/**
 * @brief Returns the current value of the RXVECTOR4 register.
 * The RXVECTOR4 register will be read and its value returned.
 * @return The current value of the RXVECTOR4 register.
 */
__INLINE uint32_t mdm_rxvector4_get(void)
{
    return REG_PL_RD(MDM_RXVECTOR4_ADDR);
}

// field definitions
/// RXGROUPID field mask
#define MDM_RXGROUPID_MASK      ((uint32_t)0x3F000000)
/// RXGROUPID field LSB position
#define MDM_RXGROUPID_LSB       24
/// RXGROUPID field width
#define MDM_RXGROUPID_WIDTH     ((uint32_t)0x00000006)
/// RXPARTIALAID field mask
#define MDM_RXPARTIALAID_MASK   ((uint32_t)0x001FF000)
/// RXPARTIALAID field LSB position
#define MDM_RXPARTIALAID_LSB    12
/// RXPARTIALAID field width
#define MDM_RXPARTIALAID_WIDTH  ((uint32_t)0x00000009)
/// RCPI field mask
#define MDM_RCPI_MASK           ((uint32_t)0x000000FF)
/// RCPI field LSB position
#define MDM_RCPI_LSB            0
/// RCPI field width
#define MDM_RCPI_WIDTH          ((uint32_t)0x00000008)

/// RXGROUPID field reset value
#define MDM_RXGROUPID_RST       0x0
/// RXPARTIALAID field reset value
#define MDM_RXPARTIALAID_RST    0x0
/// RCPI field reset value
#define MDM_RCPI_RST            0x0

/**
 * @brief Unpacks RXVECTOR4's fields from current value of the RXVECTOR4 register.
 *
 * Reads the RXVECTOR4 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxgroupid - Will be populated with the current value of this field from the register.
 * @param[out] rxpartialaid - Will be populated with the current value of this field from the register.
 * @param[out] rcpi - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxvector4_unpack(uint8_t *rxgroupid, uint16_t *rxpartialaid, uint8_t *rcpi)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR4_ADDR);

    *rxgroupid = (localVal & ((uint32_t)0x3F000000)) >> 24;
    *rxpartialaid = (localVal & ((uint32_t)0x001FF000)) >> 12;
    *rcpi = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the RXGROUPID field in the RXVECTOR4 register.
 *
 * The RXVECTOR4 register will be read and the RXGROUPID field's value will be returned.
 *
 * @return The current value of the RXGROUPID field in the RXVECTOR4 register.
 */
__INLINE uint8_t mdm_rxgroupid_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR4_ADDR);
    return ((localVal & ((uint32_t)0x3F000000)) >> 24);
}

/**
 * @brief Returns the current value of the RXPARTIALAID field in the RXVECTOR4 register.
 *
 * The RXVECTOR4 register will be read and the RXPARTIALAID field's value will be returned.
 *
 * @return The current value of the RXPARTIALAID field in the RXVECTOR4 register.
 */
__INLINE uint16_t mdm_rxpartialaid_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR4_ADDR);
    return ((localVal & ((uint32_t)0x001FF000)) >> 12);
}

/**
 * @brief Returns the current value of the RCPI field in the RXVECTOR4 register.
 *
 * The RXVECTOR4 register will be read and the RCPI field's value will be returned.
 *
 * @return The current value of the RCPI field in the RXVECTOR4 register.
 */
__INLINE uint8_t mdm_rcpi_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR4_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/// @}

/**
 * @name RXVECTOR5 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:24                 EVM4   0x0
 *  23:16                 EVM3   0x0
 *  15:08                 EVM2   0x0
 *  07:00                 EVM1   0x0
 * </pre>
 *
 * @{
 */

/// Address of the RXVECTOR5 register
#define MDM_RXVECTOR5_ADDR   0x0100003C
/// Offset of the RXVECTOR5 register from the base address
#define MDM_RXVECTOR5_OFFSET 0x0000003C
/// Index of the RXVECTOR5 register
#define MDM_RXVECTOR5_INDEX  0x0000000F
/// Reset value of the RXVECTOR5 register
#define MDM_RXVECTOR5_RESET  0x00000000

/**
 * @brief Returns the current value of the RXVECTOR5 register.
 * The RXVECTOR5 register will be read and its value returned.
 * @return The current value of the RXVECTOR5 register.
 */
__INLINE uint32_t mdm_rxvector5_get(void)
{
    return REG_PL_RD(MDM_RXVECTOR5_ADDR);
}

// field definitions
/// EVM4 field mask
#define MDM_EVM4_MASK   ((uint32_t)0xFF000000)
/// EVM4 field LSB position
#define MDM_EVM4_LSB    24
/// EVM4 field width
#define MDM_EVM4_WIDTH  ((uint32_t)0x00000008)
/// EVM3 field mask
#define MDM_EVM3_MASK   ((uint32_t)0x00FF0000)
/// EVM3 field LSB position
#define MDM_EVM3_LSB    16
/// EVM3 field width
#define MDM_EVM3_WIDTH  ((uint32_t)0x00000008)
/// EVM2 field mask
#define MDM_EVM2_MASK   ((uint32_t)0x0000FF00)
/// EVM2 field LSB position
#define MDM_EVM2_LSB    8
/// EVM2 field width
#define MDM_EVM2_WIDTH  ((uint32_t)0x00000008)
/// EVM1 field mask
#define MDM_EVM1_MASK   ((uint32_t)0x000000FF)
/// EVM1 field LSB position
#define MDM_EVM1_LSB    0
/// EVM1 field width
#define MDM_EVM1_WIDTH  ((uint32_t)0x00000008)

/// EVM4 field reset value
#define MDM_EVM4_RST    0x0
/// EVM3 field reset value
#define MDM_EVM3_RST    0x0
/// EVM2 field reset value
#define MDM_EVM2_RST    0x0
/// EVM1 field reset value
#define MDM_EVM1_RST    0x0

/**
 * @brief Unpacks RXVECTOR5's fields from current value of the RXVECTOR5 register.
 *
 * Reads the RXVECTOR5 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] evm4 - Will be populated with the current value of this field from the register.
 * @param[out] evm3 - Will be populated with the current value of this field from the register.
 * @param[out] evm2 - Will be populated with the current value of this field from the register.
 * @param[out] evm1 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxvector5_unpack(uint8_t *evm4, uint8_t *evm3, uint8_t *evm2, uint8_t *evm1)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR5_ADDR);

    *evm4 = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *evm3 = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *evm2 = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *evm1 = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the EVM4 field in the RXVECTOR5 register.
 *
 * The RXVECTOR5 register will be read and the EVM4 field's value will be returned.
 *
 * @return The current value of the EVM4 field in the RXVECTOR5 register.
 */
__INLINE uint8_t mdm_evm4_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR5_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Returns the current value of the EVM3 field in the RXVECTOR5 register.
 *
 * The RXVECTOR5 register will be read and the EVM3 field's value will be returned.
 *
 * @return The current value of the EVM3 field in the RXVECTOR5 register.
 */
__INLINE uint8_t mdm_evm3_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR5_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Returns the current value of the EVM2 field in the RXVECTOR5 register.
 *
 * The RXVECTOR5 register will be read and the EVM2 field's value will be returned.
 *
 * @return The current value of the EVM2 field in the RXVECTOR5 register.
 */
__INLINE uint8_t mdm_evm2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR5_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Returns the current value of the EVM1 field in the RXVECTOR5 register.
 *
 * The RXVECTOR5 register will be read and the EVM1 field's value will be returned.
 *
 * @return The current value of the EVM1 field in the RXVECTOR5 register.
 */
__INLINE uint8_t mdm_evm1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXVECTOR5_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/// @}

/**
 * @name TXVECTOR0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:24             SMMINDEX   0x0
 *  23:16         TXANTENNASET   0x0
 *  13:12               TXCHBW   0x0
 *     11          TXSMOOTHING   0
 *     10              TXDYNBW   0
 *     09          TXFECCODING   0
 *     08           TXSOUNDING   0
 *  07:00            TXPWLEVEL   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TXVECTOR0 register
#define MDM_TXVECTOR0_ADDR   0x01000040
/// Offset of the TXVECTOR0 register from the base address
#define MDM_TXVECTOR0_OFFSET 0x00000040
/// Index of the TXVECTOR0 register
#define MDM_TXVECTOR0_INDEX  0x00000010
/// Reset value of the TXVECTOR0 register
#define MDM_TXVECTOR0_RESET  0x00000000

/**
 * @brief Returns the current value of the TXVECTOR0 register.
 * The TXVECTOR0 register will be read and its value returned.
 * @return The current value of the TXVECTOR0 register.
 */
__INLINE uint32_t mdm_txvector0_get(void)
{
    return REG_PL_RD(MDM_TXVECTOR0_ADDR);
}

// field definitions
/// SMMINDEX field mask
#define MDM_SMMINDEX_MASK       ((uint32_t)0xFF000000)
/// SMMINDEX field LSB position
#define MDM_SMMINDEX_LSB        24
/// SMMINDEX field width
#define MDM_SMMINDEX_WIDTH      ((uint32_t)0x00000008)
/// TXANTENNASET field mask
#define MDM_TXANTENNASET_MASK   ((uint32_t)0x00FF0000)
/// TXANTENNASET field LSB position
#define MDM_TXANTENNASET_LSB    16
/// TXANTENNASET field width
#define MDM_TXANTENNASET_WIDTH  ((uint32_t)0x00000008)
/// TXCHBW field mask
#define MDM_TXCHBW_MASK         ((uint32_t)0x00003000)
/// TXCHBW field LSB position
#define MDM_TXCHBW_LSB          12
/// TXCHBW field width
#define MDM_TXCHBW_WIDTH        ((uint32_t)0x00000002)
/// TXSMOOTHING field bit
#define MDM_TXSMOOTHING_BIT     ((uint32_t)0x00000800)
/// TXSMOOTHING field position
#define MDM_TXSMOOTHING_POS     11
/// TXDYNBW field bit
#define MDM_TXDYNBW_BIT         ((uint32_t)0x00000400)
/// TXDYNBW field position
#define MDM_TXDYNBW_POS         10
/// TXFECCODING field bit
#define MDM_TXFECCODING_BIT     ((uint32_t)0x00000200)
/// TXFECCODING field position
#define MDM_TXFECCODING_POS     9
/// TXSOUNDING field bit
#define MDM_TXSOUNDING_BIT      ((uint32_t)0x00000100)
/// TXSOUNDING field position
#define MDM_TXSOUNDING_POS      8
/// TXPWLEVEL field mask
#define MDM_TXPWLEVEL_MASK      ((uint32_t)0x000000FF)
/// TXPWLEVEL field LSB position
#define MDM_TXPWLEVEL_LSB       0
/// TXPWLEVEL field width
#define MDM_TXPWLEVEL_WIDTH     ((uint32_t)0x00000008)

/// SMMINDEX field reset value
#define MDM_SMMINDEX_RST        0x0
/// TXANTENNASET field reset value
#define MDM_TXANTENNASET_RST    0x0
/// TXCHBW field reset value
#define MDM_TXCHBW_RST          0x0
/// TXSMOOTHING field reset value
#define MDM_TXSMOOTHING_RST     0x0
/// TXDYNBW field reset value
#define MDM_TXDYNBW_RST         0x0
/// TXFECCODING field reset value
#define MDM_TXFECCODING_RST     0x0
/// TXSOUNDING field reset value
#define MDM_TXSOUNDING_RST      0x0
/// TXPWLEVEL field reset value
#define MDM_TXPWLEVEL_RST       0x0

/**
 * @brief Unpacks TXVECTOR0's fields from current value of the TXVECTOR0 register.
 *
 * Reads the TXVECTOR0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] smmindex - Will be populated with the current value of this field from the register.
 * @param[out] txantennaset - Will be populated with the current value of this field from the register.
 * @param[out] txchbw - Will be populated with the current value of this field from the register.
 * @param[out] txsmoothing - Will be populated with the current value of this field from the register.
 * @param[out] txdynbw - Will be populated with the current value of this field from the register.
 * @param[out] txfeccoding - Will be populated with the current value of this field from the register.
 * @param[out] txsounding - Will be populated with the current value of this field from the register.
 * @param[out] txpwlevel - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_txvector0_unpack(uint8_t *smmindex, uint8_t *txantennaset, uint8_t *txchbw, uint8_t *txsmoothing, uint8_t *txdynbw, uint8_t *txfeccoding, uint8_t *txsounding, uint8_t *txpwlevel)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR0_ADDR);

    *smmindex = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *txantennaset = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *txchbw = (localVal & ((uint32_t)0x00003000)) >> 12;
    *txsmoothing = (localVal & ((uint32_t)0x00000800)) >> 11;
    *txdynbw = (localVal & ((uint32_t)0x00000400)) >> 10;
    *txfeccoding = (localVal & ((uint32_t)0x00000200)) >> 9;
    *txsounding = (localVal & ((uint32_t)0x00000100)) >> 8;
    *txpwlevel = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the SMMINDEX field in the TXVECTOR0 register.
 *
 * The TXVECTOR0 register will be read and the SMMINDEX field's value will be returned.
 *
 * @return The current value of the SMMINDEX field in the TXVECTOR0 register.
 */
__INLINE uint8_t mdm_smmindex_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR0_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Returns the current value of the TXANTENNASET field in the TXVECTOR0 register.
 *
 * The TXVECTOR0 register will be read and the TXANTENNASET field's value will be returned.
 *
 * @return The current value of the TXANTENNASET field in the TXVECTOR0 register.
 */
__INLINE uint8_t mdm_txantennaset_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR0_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Returns the current value of the TXCHBW field in the TXVECTOR0 register.
 *
 * The TXVECTOR0 register will be read and the TXCHBW field's value will be returned.
 *
 * @return The current value of the TXCHBW field in the TXVECTOR0 register.
 */
__INLINE uint8_t mdm_txchbw_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR0_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

/**
 * @brief Returns the current value of the TXSMOOTHING field in the TXVECTOR0 register.
 *
 * The TXVECTOR0 register will be read and the TXSMOOTHING field's value will be returned.
 *
 * @return The current value of the TXSMOOTHING field in the TXVECTOR0 register.
 */
__INLINE uint8_t mdm_txsmoothing_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR0_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Returns the current value of the TXDYNBW field in the TXVECTOR0 register.
 *
 * The TXVECTOR0 register will be read and the TXDYNBW field's value will be returned.
 *
 * @return The current value of the TXDYNBW field in the TXVECTOR0 register.
 */
__INLINE uint8_t mdm_txdynbw_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR0_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Returns the current value of the TXFECCODING field in the TXVECTOR0 register.
 *
 * The TXVECTOR0 register will be read and the TXFECCODING field's value will be returned.
 *
 * @return The current value of the TXFECCODING field in the TXVECTOR0 register.
 */
__INLINE uint8_t mdm_txfeccoding_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR0_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Returns the current value of the TXSOUNDING field in the TXVECTOR0 register.
 *
 * The TXVECTOR0 register will be read and the TXSOUNDING field's value will be returned.
 *
 * @return The current value of the TXSOUNDING field in the TXVECTOR0 register.
 */
__INLINE uint8_t mdm_txsounding_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR0_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Returns the current value of the TXPWLEVEL field in the TXVECTOR0 register.
 *
 * The TXVECTOR0 register will be read and the TXPWLEVEL field's value will be returned.
 *
 * @return The current value of the TXPWLEVEL field in the TXVECTOR0 register.
 */
__INLINE uint8_t mdm_txpwlevel_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR0_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/// @}

/**
 * @name TXVECTOR1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  22:20             TXNUMSTS   0x0
 *  17:16           TXNUMEXTSS   0x0
 *  14:12             TXFORMAT   0x0
 *     08              PRETYPE   0
 *  06:00                TXMCS   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TXVECTOR1 register
#define MDM_TXVECTOR1_ADDR   0x01000044
/// Offset of the TXVECTOR1 register from the base address
#define MDM_TXVECTOR1_OFFSET 0x00000044
/// Index of the TXVECTOR1 register
#define MDM_TXVECTOR1_INDEX  0x00000011
/// Reset value of the TXVECTOR1 register
#define MDM_TXVECTOR1_RESET  0x00000000

/**
 * @brief Returns the current value of the TXVECTOR1 register.
 * The TXVECTOR1 register will be read and its value returned.
 * @return The current value of the TXVECTOR1 register.
 */
__INLINE uint32_t mdm_txvector1_get(void)
{
    return REG_PL_RD(MDM_TXVECTOR1_ADDR);
}

// field definitions
/// TXNUMSTS field mask
#define MDM_TXNUMSTS_MASK     ((uint32_t)0x00700000)
/// TXNUMSTS field LSB position
#define MDM_TXNUMSTS_LSB      20
/// TXNUMSTS field width
#define MDM_TXNUMSTS_WIDTH    ((uint32_t)0x00000003)
/// TXNUMEXTSS field mask
#define MDM_TXNUMEXTSS_MASK   ((uint32_t)0x00030000)
/// TXNUMEXTSS field LSB position
#define MDM_TXNUMEXTSS_LSB    16
/// TXNUMEXTSS field width
#define MDM_TXNUMEXTSS_WIDTH  ((uint32_t)0x00000002)
/// TXFORMAT field mask
#define MDM_TXFORMAT_MASK     ((uint32_t)0x00007000)
/// TXFORMAT field LSB position
#define MDM_TXFORMAT_LSB      12
/// TXFORMAT field width
#define MDM_TXFORMAT_WIDTH    ((uint32_t)0x00000003)
/// PRETYPE field bit
#define MDM_PRETYPE_BIT       ((uint32_t)0x00000100)
/// PRETYPE field position
#define MDM_PRETYPE_POS       8
/// TXMCS field mask
#define MDM_TXMCS_MASK        ((uint32_t)0x0000007F)
/// TXMCS field LSB position
#define MDM_TXMCS_LSB         0
/// TXMCS field width
#define MDM_TXMCS_WIDTH       ((uint32_t)0x00000007)

/// TXNUMSTS field reset value
#define MDM_TXNUMSTS_RST      0x0
/// TXNUMEXTSS field reset value
#define MDM_TXNUMEXTSS_RST    0x0
/// TXFORMAT field reset value
#define MDM_TXFORMAT_RST      0x0
/// PRETYPE field reset value
#define MDM_PRETYPE_RST       0x0
/// TXMCS field reset value
#define MDM_TXMCS_RST         0x0

/**
 * @brief Unpacks TXVECTOR1's fields from current value of the TXVECTOR1 register.
 *
 * Reads the TXVECTOR1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txnumsts - Will be populated with the current value of this field from the register.
 * @param[out] txnumextss - Will be populated with the current value of this field from the register.
 * @param[out] txformat - Will be populated with the current value of this field from the register.
 * @param[out] pretype - Will be populated with the current value of this field from the register.
 * @param[out] txmcs - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_txvector1_unpack(uint8_t *txnumsts, uint8_t *txnumextss, uint8_t *txformat, uint8_t *pretype, uint8_t *txmcs)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR1_ADDR);

    *txnumsts = (localVal & ((uint32_t)0x00700000)) >> 20;
    *txnumextss = (localVal & ((uint32_t)0x00030000)) >> 16;
    *txformat = (localVal & ((uint32_t)0x00007000)) >> 12;
    *pretype = (localVal & ((uint32_t)0x00000100)) >> 8;
    *txmcs = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the TXNUMSTS field in the TXVECTOR1 register.
 *
 * The TXVECTOR1 register will be read and the TXNUMSTS field's value will be returned.
 *
 * @return The current value of the TXNUMSTS field in the TXVECTOR1 register.
 */
__INLINE uint8_t mdm_txnumsts_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR1_ADDR);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

/**
 * @brief Returns the current value of the TXNUMEXTSS field in the TXVECTOR1 register.
 *
 * The TXVECTOR1 register will be read and the TXNUMEXTSS field's value will be returned.
 *
 * @return The current value of the TXNUMEXTSS field in the TXVECTOR1 register.
 */
__INLINE uint8_t mdm_txnumextss_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR1_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

/**
 * @brief Returns the current value of the TXFORMAT field in the TXVECTOR1 register.
 *
 * The TXVECTOR1 register will be read and the TXFORMAT field's value will be returned.
 *
 * @return The current value of the TXFORMAT field in the TXVECTOR1 register.
 */
__INLINE uint8_t mdm_txformat_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR1_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

/**
 * @brief Returns the current value of the PRETYPE field in the TXVECTOR1 register.
 *
 * The TXVECTOR1 register will be read and the PRETYPE field's value will be returned.
 *
 * @return The current value of the PRETYPE field in the TXVECTOR1 register.
 */
__INLINE uint8_t mdm_pretype_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR1_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Returns the current value of the TXMCS field in the TXVECTOR1 register.
 *
 * The TXVECTOR1 register will be read and the TXMCS field's value will be returned.
 *
 * @return The current value of the TXMCS field in the TXVECTOR1 register.
 */
__INLINE uint8_t mdm_txmcs_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR1_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/// @}

/**
 * @name TXVECTOR2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:16              SERVICE   0x0
 *  15:12            TXLEGRATE   0x0
 *  11:00          TXLEGLENTGH   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TXVECTOR2 register
#define MDM_TXVECTOR2_ADDR   0x01000048
/// Offset of the TXVECTOR2 register from the base address
#define MDM_TXVECTOR2_OFFSET 0x00000048
/// Index of the TXVECTOR2 register
#define MDM_TXVECTOR2_INDEX  0x00000012
/// Reset value of the TXVECTOR2 register
#define MDM_TXVECTOR2_RESET  0x00000000

/**
 * @brief Returns the current value of the TXVECTOR2 register.
 * The TXVECTOR2 register will be read and its value returned.
 * @return The current value of the TXVECTOR2 register.
 */
__INLINE uint32_t mdm_txvector2_get(void)
{
    return REG_PL_RD(MDM_TXVECTOR2_ADDR);
}

// field definitions
/// SERVICE field mask
#define MDM_SERVICE_MASK       ((uint32_t)0xFFFF0000)
/// SERVICE field LSB position
#define MDM_SERVICE_LSB        16
/// SERVICE field width
#define MDM_SERVICE_WIDTH      ((uint32_t)0x00000010)
/// TXLEGRATE field mask
#define MDM_TXLEGRATE_MASK     ((uint32_t)0x0000F000)
/// TXLEGRATE field LSB position
#define MDM_TXLEGRATE_LSB      12
/// TXLEGRATE field width
#define MDM_TXLEGRATE_WIDTH    ((uint32_t)0x00000004)
/// TXLEGLENTGH field mask
#define MDM_TXLEGLENTGH_MASK   ((uint32_t)0x00000FFF)
/// TXLEGLENTGH field LSB position
#define MDM_TXLEGLENTGH_LSB    0
/// TXLEGLENTGH field width
#define MDM_TXLEGLENTGH_WIDTH  ((uint32_t)0x0000000C)

/// SERVICE field reset value
#define MDM_SERVICE_RST        0x0
/// TXLEGRATE field reset value
#define MDM_TXLEGRATE_RST      0x0
/// TXLEGLENTGH field reset value
#define MDM_TXLEGLENTGH_RST    0x0

/**
 * @brief Unpacks TXVECTOR2's fields from current value of the TXVECTOR2 register.
 *
 * Reads the TXVECTOR2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] service - Will be populated with the current value of this field from the register.
 * @param[out] txlegrate - Will be populated with the current value of this field from the register.
 * @param[out] txleglentgh - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_txvector2_unpack(uint16_t *service, uint8_t *txlegrate, uint16_t *txleglentgh)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR2_ADDR);

    *service = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *txlegrate = (localVal & ((uint32_t)0x0000F000)) >> 12;
    *txleglentgh = (localVal & ((uint32_t)0x00000FFF)) >> 0;
}

/**
 * @brief Returns the current value of the SERVICE field in the TXVECTOR2 register.
 *
 * The TXVECTOR2 register will be read and the SERVICE field's value will be returned.
 *
 * @return The current value of the SERVICE field in the TXVECTOR2 register.
 */
__INLINE uint16_t mdm_service_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR2_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Returns the current value of the TXLEGRATE field in the TXVECTOR2 register.
 *
 * The TXVECTOR2 register will be read and the TXLEGRATE field's value will be returned.
 *
 * @return The current value of the TXLEGRATE field in the TXVECTOR2 register.
 */
__INLINE uint8_t mdm_txlegrate_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR2_ADDR);
    return ((localVal & ((uint32_t)0x0000F000)) >> 12);
}

/**
 * @brief Returns the current value of the TXLEGLENTGH field in the TXVECTOR2 register.
 *
 * The TXVECTOR2 register will be read and the TXLEGLENTGH field's value will be returned.
 *
 * @return The current value of the TXLEGLENTGH field in the TXVECTOR2 register.
 */
__INLINE uint16_t mdm_txleglentgh_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR2_ADDR);
    return ((localVal & ((uint32_t)0x00000FFF)) >> 0);
}

/// @}

/**
 * @name TXVECTOR3 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:28               TXSTBC   0x0
 *  26:24                  NTX   0x0
 *     22            TXSHORTGI   0
 *     21     TXDOZENOTALLOWED   0
 *     20             TXAGGREG   0
 *  19:00           TXHTLENGTH   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TXVECTOR3 register
#define MDM_TXVECTOR3_ADDR   0x0100004C
/// Offset of the TXVECTOR3 register from the base address
#define MDM_TXVECTOR3_OFFSET 0x0000004C
/// Index of the TXVECTOR3 register
#define MDM_TXVECTOR3_INDEX  0x00000013
/// Reset value of the TXVECTOR3 register
#define MDM_TXVECTOR3_RESET  0x00000000

/**
 * @brief Returns the current value of the TXVECTOR3 register.
 * The TXVECTOR3 register will be read and its value returned.
 * @return The current value of the TXVECTOR3 register.
 */
__INLINE uint32_t mdm_txvector3_get(void)
{
    return REG_PL_RD(MDM_TXVECTOR3_ADDR);
}

// field definitions
/// TXSTBC field mask
#define MDM_TXSTBC_MASK             ((uint32_t)0x30000000)
/// TXSTBC field LSB position
#define MDM_TXSTBC_LSB              28
/// TXSTBC field width
#define MDM_TXSTBC_WIDTH            ((uint32_t)0x00000002)
/// NTX field mask
#define MDM_NTX_MASK                ((uint32_t)0x07000000)
/// NTX field LSB position
#define MDM_NTX_LSB                 24
/// NTX field width
#define MDM_NTX_WIDTH               ((uint32_t)0x00000003)
/// TXSHORTGI field bit
#define MDM_TXSHORTGI_BIT           ((uint32_t)0x00400000)
/// TXSHORTGI field position
#define MDM_TXSHORTGI_POS           22
/// TXDOZENOTALLOWED field bit
#define MDM_TXDOZENOTALLOWED_BIT    ((uint32_t)0x00200000)
/// TXDOZENOTALLOWED field position
#define MDM_TXDOZENOTALLOWED_POS    21
/// TXAGGREG field bit
#define MDM_TXAGGREG_BIT            ((uint32_t)0x00100000)
/// TXAGGREG field position
#define MDM_TXAGGREG_POS            20
/// TXHTLENGTH field mask
#define MDM_TXHTLENGTH_MASK         ((uint32_t)0x000FFFFF)
/// TXHTLENGTH field LSB position
#define MDM_TXHTLENGTH_LSB          0
/// TXHTLENGTH field width
#define MDM_TXHTLENGTH_WIDTH        ((uint32_t)0x00000014)

/// TXSTBC field reset value
#define MDM_TXSTBC_RST              0x0
/// NTX field reset value
#define MDM_NTX_RST                 0x0
/// TXSHORTGI field reset value
#define MDM_TXSHORTGI_RST           0x0
/// TXDOZENOTALLOWED field reset value
#define MDM_TXDOZENOTALLOWED_RST    0x0
/// TXAGGREG field reset value
#define MDM_TXAGGREG_RST            0x0
/// TXHTLENGTH field reset value
#define MDM_TXHTLENGTH_RST          0x0

/**
 * @brief Unpacks TXVECTOR3's fields from current value of the TXVECTOR3 register.
 *
 * Reads the TXVECTOR3 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txstbc - Will be populated with the current value of this field from the register.
 * @param[out] ntx - Will be populated with the current value of this field from the register.
 * @param[out] txshortgi - Will be populated with the current value of this field from the register.
 * @param[out] txdozenotallowed - Will be populated with the current value of this field from the register.
 * @param[out] txaggreg - Will be populated with the current value of this field from the register.
 * @param[out] txhtlength - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_txvector3_unpack(uint8_t *txstbc, uint8_t *ntx, uint8_t *txshortgi, uint8_t *txdozenotallowed, uint8_t *txaggreg, uint32_t *txhtlength)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR3_ADDR);

    *txstbc = (localVal & ((uint32_t)0x30000000)) >> 28;
    *ntx = (localVal & ((uint32_t)0x07000000)) >> 24;
    *txshortgi = (localVal & ((uint32_t)0x00400000)) >> 22;
    *txdozenotallowed = (localVal & ((uint32_t)0x00200000)) >> 21;
    *txaggreg = (localVal & ((uint32_t)0x00100000)) >> 20;
    *txhtlength = (localVal & ((uint32_t)0x000FFFFF)) >> 0;
}

/**
 * @brief Returns the current value of the TXSTBC field in the TXVECTOR3 register.
 *
 * The TXVECTOR3 register will be read and the TXSTBC field's value will be returned.
 *
 * @return The current value of the TXSTBC field in the TXVECTOR3 register.
 */
__INLINE uint8_t mdm_txstbc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR3_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

/**
 * @brief Returns the current value of the NTX field in the TXVECTOR3 register.
 *
 * The TXVECTOR3 register will be read and the NTX field's value will be returned.
 *
 * @return The current value of the NTX field in the TXVECTOR3 register.
 */
__INLINE uint8_t mdm_ntx_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR3_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

/**
 * @brief Returns the current value of the TXSHORTGI field in the TXVECTOR3 register.
 *
 * The TXVECTOR3 register will be read and the TXSHORTGI field's value will be returned.
 *
 * @return The current value of the TXSHORTGI field in the TXVECTOR3 register.
 */
__INLINE uint8_t mdm_txshortgi_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR3_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Returns the current value of the TXDOZENOTALLOWED field in the TXVECTOR3 register.
 *
 * The TXVECTOR3 register will be read and the TXDOZENOTALLOWED field's value will be returned.
 *
 * @return The current value of the TXDOZENOTALLOWED field in the TXVECTOR3 register.
 */
__INLINE uint8_t mdm_txdozenotallowed_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR3_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Returns the current value of the TXAGGREG field in the TXVECTOR3 register.
 *
 * The TXVECTOR3 register will be read and the TXAGGREG field's value will be returned.
 *
 * @return The current value of the TXAGGREG field in the TXVECTOR3 register.
 */
__INLINE uint8_t mdm_txaggreg_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR3_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Returns the current value of the TXHTLENGTH field in the TXVECTOR3 register.
 *
 * The TXVECTOR3 register will be read and the TXHTLENGTH field's value will be returned.
 *
 * @return The current value of the TXHTLENGTH field in the TXVECTOR3 register.
 */
__INLINE uint32_t mdm_txhtlength_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR3_ADDR);
    return ((localVal & ((uint32_t)0x000FFFFF)) >> 0);
}

/// @}

/**
 * @name TXVECTOR4 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  17:12            TXGROUPID   0x0
 *  08:00         TXPARTIALAID   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TXVECTOR4 register
#define MDM_TXVECTOR4_ADDR   0x01000050
/// Offset of the TXVECTOR4 register from the base address
#define MDM_TXVECTOR4_OFFSET 0x00000050
/// Index of the TXVECTOR4 register
#define MDM_TXVECTOR4_INDEX  0x00000014
/// Reset value of the TXVECTOR4 register
#define MDM_TXVECTOR4_RESET  0x00000000

/**
 * @brief Returns the current value of the TXVECTOR4 register.
 * The TXVECTOR4 register will be read and its value returned.
 * @return The current value of the TXVECTOR4 register.
 */
__INLINE uint32_t mdm_txvector4_get(void)
{
    return REG_PL_RD(MDM_TXVECTOR4_ADDR);
}

// field definitions
/// TXGROUPID field mask
#define MDM_TXGROUPID_MASK      ((uint32_t)0x0003F000)
/// TXGROUPID field LSB position
#define MDM_TXGROUPID_LSB       12
/// TXGROUPID field width
#define MDM_TXGROUPID_WIDTH     ((uint32_t)0x00000006)
/// TXPARTIALAID field mask
#define MDM_TXPARTIALAID_MASK   ((uint32_t)0x000001FF)
/// TXPARTIALAID field LSB position
#define MDM_TXPARTIALAID_LSB    0
/// TXPARTIALAID field width
#define MDM_TXPARTIALAID_WIDTH  ((uint32_t)0x00000009)

/// TXGROUPID field reset value
#define MDM_TXGROUPID_RST       0x0
/// TXPARTIALAID field reset value
#define MDM_TXPARTIALAID_RST    0x0

/**
 * @brief Unpacks TXVECTOR4's fields from current value of the TXVECTOR4 register.
 *
 * Reads the TXVECTOR4 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txgroupid - Will be populated with the current value of this field from the register.
 * @param[out] txpartialaid - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_txvector4_unpack(uint8_t *txgroupid, uint16_t *txpartialaid)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR4_ADDR);

    *txgroupid = (localVal & ((uint32_t)0x0003F000)) >> 12;
    *txpartialaid = (localVal & ((uint32_t)0x000001FF)) >> 0;
}

/**
 * @brief Returns the current value of the TXGROUPID field in the TXVECTOR4 register.
 *
 * The TXVECTOR4 register will be read and the TXGROUPID field's value will be returned.
 *
 * @return The current value of the TXGROUPID field in the TXVECTOR4 register.
 */
__INLINE uint8_t mdm_txgroupid_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR4_ADDR);
    return ((localVal & ((uint32_t)0x0003F000)) >> 12);
}

/**
 * @brief Returns the current value of the TXPARTIALAID field in the TXVECTOR4 register.
 *
 * The TXVECTOR4 register will be read and the TXPARTIALAID field's value will be returned.
 *
 * @return The current value of the TXPARTIALAID field in the TXVECTOR4 register.
 */
__INLINE uint16_t mdm_txpartialaid_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECTOR4_ADDR);
    return ((localVal & ((uint32_t)0x000001FF)) >> 0);
}

/// @}

/**
 * @name TXVECDECSTAT register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     07          TXVECGI2ERR   0
 *     06           TXVECBWERR   0
 *     05         MTXVECGI1ERR   0
 *     04          TXVECMCSERR   0
 *     03          TXVECLEGERR   0
 *     02         TXVECNULLERR   0
 *     01          TXLECLENERR   0
 *     00        TXVECSOUNDERR   0
 * </pre>
 *
 * @{
 */

/// Address of the TXVECDECSTAT register
#define MDM_TXVECDECSTAT_ADDR   0x01000054
/// Offset of the TXVECDECSTAT register from the base address
#define MDM_TXVECDECSTAT_OFFSET 0x00000054
/// Index of the TXVECDECSTAT register
#define MDM_TXVECDECSTAT_INDEX  0x00000015
/// Reset value of the TXVECDECSTAT register
#define MDM_TXVECDECSTAT_RESET  0x00000000

/**
 * @brief Returns the current value of the TXVECDECSTAT register.
 * The TXVECDECSTAT register will be read and its value returned.
 * @return The current value of the TXVECDECSTAT register.
 */
__INLINE uint32_t mdm_txvecdecstat_get(void)
{
    return REG_PL_RD(MDM_TXVECDECSTAT_ADDR);
}

// field definitions
/// TXVECGI2ERR field bit
#define MDM_TXVECGI2ERR_BIT      ((uint32_t)0x00000080)
/// TXVECGI2ERR field position
#define MDM_TXVECGI2ERR_POS      7
/// TXVECBWERR field bit
#define MDM_TXVECBWERR_BIT       ((uint32_t)0x00000040)
/// TXVECBWERR field position
#define MDM_TXVECBWERR_POS       6
/// MTXVECGI1ERR field bit
#define MDM_MTXVECGI1ERR_BIT     ((uint32_t)0x00000020)
/// MTXVECGI1ERR field position
#define MDM_MTXVECGI1ERR_POS     5
/// TXVECMCSERR field bit
#define MDM_TXVECMCSERR_BIT      ((uint32_t)0x00000010)
/// TXVECMCSERR field position
#define MDM_TXVECMCSERR_POS      4
/// TXVECLEGERR field bit
#define MDM_TXVECLEGERR_BIT      ((uint32_t)0x00000008)
/// TXVECLEGERR field position
#define MDM_TXVECLEGERR_POS      3
/// TXVECNULLERR field bit
#define MDM_TXVECNULLERR_BIT     ((uint32_t)0x00000004)
/// TXVECNULLERR field position
#define MDM_TXVECNULLERR_POS     2
/// TXLECLENERR field bit
#define MDM_TXLECLENERR_BIT      ((uint32_t)0x00000002)
/// TXLECLENERR field position
#define MDM_TXLECLENERR_POS      1
/// TXVECSOUNDERR field bit
#define MDM_TXVECSOUNDERR_BIT    ((uint32_t)0x00000001)
/// TXVECSOUNDERR field position
#define MDM_TXVECSOUNDERR_POS    0

/// TXVECGI2ERR field reset value
#define MDM_TXVECGI2ERR_RST      0x0
/// TXVECBWERR field reset value
#define MDM_TXVECBWERR_RST       0x0
/// MTXVECGI1ERR field reset value
#define MDM_MTXVECGI1ERR_RST     0x0
/// TXVECMCSERR field reset value
#define MDM_TXVECMCSERR_RST      0x0
/// TXVECLEGERR field reset value
#define MDM_TXVECLEGERR_RST      0x0
/// TXVECNULLERR field reset value
#define MDM_TXVECNULLERR_RST     0x0
/// TXLECLENERR field reset value
#define MDM_TXLECLENERR_RST      0x0
/// TXVECSOUNDERR field reset value
#define MDM_TXVECSOUNDERR_RST    0x0

/**
 * @brief Unpacks TXVECDECSTAT's fields from current value of the TXVECDECSTAT register.
 *
 * Reads the TXVECDECSTAT register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txvecgi2err - Will be populated with the current value of this field from the register.
 * @param[out] txvecbwerr - Will be populated with the current value of this field from the register.
 * @param[out] mtxvecgi1err - Will be populated with the current value of this field from the register.
 * @param[out] txvecmcserr - Will be populated with the current value of this field from the register.
 * @param[out] txveclegerr - Will be populated with the current value of this field from the register.
 * @param[out] txvecnullerr - Will be populated with the current value of this field from the register.
 * @param[out] txleclenerr - Will be populated with the current value of this field from the register.
 * @param[out] txvecsounderr - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_txvecdecstat_unpack(uint8_t *txvecgi2err, uint8_t *txvecbwerr, uint8_t *mtxvecgi1err, uint8_t *txvecmcserr, uint8_t *txveclegerr, uint8_t *txvecnullerr, uint8_t *txleclenerr, uint8_t *txvecsounderr)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECDECSTAT_ADDR);

    *txvecgi2err = (localVal & ((uint32_t)0x00000080)) >> 7;
    *txvecbwerr = (localVal & ((uint32_t)0x00000040)) >> 6;
    *mtxvecgi1err = (localVal & ((uint32_t)0x00000020)) >> 5;
    *txvecmcserr = (localVal & ((uint32_t)0x00000010)) >> 4;
    *txveclegerr = (localVal & ((uint32_t)0x00000008)) >> 3;
    *txvecnullerr = (localVal & ((uint32_t)0x00000004)) >> 2;
    *txleclenerr = (localVal & ((uint32_t)0x00000002)) >> 1;
    *txvecsounderr = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the TXVECGI2ERR field in the TXVECDECSTAT register.
 *
 * The TXVECDECSTAT register will be read and the TXVECGI2ERR field's value will be returned.
 *
 * @return The current value of the TXVECGI2ERR field in the TXVECDECSTAT register.
 */
__INLINE uint8_t mdm_txvecgi2err_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECDECSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Returns the current value of the TXVECBWERR field in the TXVECDECSTAT register.
 *
 * The TXVECDECSTAT register will be read and the TXVECBWERR field's value will be returned.
 *
 * @return The current value of the TXVECBWERR field in the TXVECDECSTAT register.
 */
__INLINE uint8_t mdm_txvecbwerr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECDECSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Returns the current value of the MTXVECGI1ERR field in the TXVECDECSTAT register.
 *
 * The TXVECDECSTAT register will be read and the MTXVECGI1ERR field's value will be returned.
 *
 * @return The current value of the MTXVECGI1ERR field in the TXVECDECSTAT register.
 */
__INLINE uint8_t mdm_mtxvecgi1err_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECDECSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Returns the current value of the TXVECMCSERR field in the TXVECDECSTAT register.
 *
 * The TXVECDECSTAT register will be read and the TXVECMCSERR field's value will be returned.
 *
 * @return The current value of the TXVECMCSERR field in the TXVECDECSTAT register.
 */
__INLINE uint8_t mdm_txvecmcserr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECDECSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Returns the current value of the TXVECLEGERR field in the TXVECDECSTAT register.
 *
 * The TXVECDECSTAT register will be read and the TXVECLEGERR field's value will be returned.
 *
 * @return The current value of the TXVECLEGERR field in the TXVECDECSTAT register.
 */
__INLINE uint8_t mdm_txveclegerr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECDECSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Returns the current value of the TXVECNULLERR field in the TXVECDECSTAT register.
 *
 * The TXVECDECSTAT register will be read and the TXVECNULLERR field's value will be returned.
 *
 * @return The current value of the TXVECNULLERR field in the TXVECDECSTAT register.
 */
__INLINE uint8_t mdm_txvecnullerr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECDECSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Returns the current value of the TXLECLENERR field in the TXVECDECSTAT register.
 *
 * The TXVECDECSTAT register will be read and the TXLECLENERR field's value will be returned.
 *
 * @return The current value of the TXLECLENERR field in the TXVECDECSTAT register.
 */
__INLINE uint8_t mdm_txleclenerr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECDECSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Returns the current value of the TXVECSOUNDERR field in the TXVECDECSTAT register.
 *
 * The TXVECDECSTAT register will be read and the TXVECSOUNDERR field's value will be returned.
 *
 * @return The current value of the TXVECSOUNDERR field in the TXVECDECSTAT register.
 */
__INLINE uint8_t mdm_txvecsounderr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXVECDECSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/// @}

/**
 * @name MIMOCMDSTAT register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:24            MIMOCMDP3   0x0
 *  23:16            MIMOCMDP2   0x0
 *  15:08            MIMOCMDP1   0x0
 *  07:00              MIMOCMD   0x0
 * </pre>
 *
 * @{
 */

/// Address of the MIMOCMDSTAT register
#define MDM_MIMOCMDSTAT_ADDR   0x01000058
/// Offset of the MIMOCMDSTAT register from the base address
#define MDM_MIMOCMDSTAT_OFFSET 0x00000058
/// Index of the MIMOCMDSTAT register
#define MDM_MIMOCMDSTAT_INDEX  0x00000016
/// Reset value of the MIMOCMDSTAT register
#define MDM_MIMOCMDSTAT_RESET  0x00000000

/**
 * @brief Returns the current value of the MIMOCMDSTAT register.
 * The MIMOCMDSTAT register will be read and its value returned.
 * @return The current value of the MIMOCMDSTAT register.
 */
__INLINE uint32_t mdm_mimocmdstat_get(void)
{
    return REG_PL_RD(MDM_MIMOCMDSTAT_ADDR);
}

// field definitions
/// MIMOCMDP3 field mask
#define MDM_MIMOCMDP3_MASK   ((uint32_t)0xFF000000)
/// MIMOCMDP3 field LSB position
#define MDM_MIMOCMDP3_LSB    24
/// MIMOCMDP3 field width
#define MDM_MIMOCMDP3_WIDTH  ((uint32_t)0x00000008)
/// MIMOCMDP2 field mask
#define MDM_MIMOCMDP2_MASK   ((uint32_t)0x00FF0000)
/// MIMOCMDP2 field LSB position
#define MDM_MIMOCMDP2_LSB    16
/// MIMOCMDP2 field width
#define MDM_MIMOCMDP2_WIDTH  ((uint32_t)0x00000008)
/// MIMOCMDP1 field mask
#define MDM_MIMOCMDP1_MASK   ((uint32_t)0x0000FF00)
/// MIMOCMDP1 field LSB position
#define MDM_MIMOCMDP1_LSB    8
/// MIMOCMDP1 field width
#define MDM_MIMOCMDP1_WIDTH  ((uint32_t)0x00000008)
/// MIMOCMD field mask
#define MDM_MIMOCMD_MASK     ((uint32_t)0x000000FF)
/// MIMOCMD field LSB position
#define MDM_MIMOCMD_LSB      0
/// MIMOCMD field width
#define MDM_MIMOCMD_WIDTH    ((uint32_t)0x00000008)

/// MIMOCMDP3 field reset value
#define MDM_MIMOCMDP3_RST    0x0
/// MIMOCMDP2 field reset value
#define MDM_MIMOCMDP2_RST    0x0
/// MIMOCMDP1 field reset value
#define MDM_MIMOCMDP1_RST    0x0
/// MIMOCMD field reset value
#define MDM_MIMOCMD_RST      0x0

/**
 * @brief Unpacks MIMOCMDSTAT's fields from current value of the MIMOCMDSTAT register.
 *
 * Reads the MIMOCMDSTAT register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] mimocmdp3 - Will be populated with the current value of this field from the register.
 * @param[out] mimocmdp2 - Will be populated with the current value of this field from the register.
 * @param[out] mimocmdp1 - Will be populated with the current value of this field from the register.
 * @param[out] mimocmd - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_mimocmdstat_unpack(uint8_t *mimocmdp3, uint8_t *mimocmdp2, uint8_t *mimocmdp1, uint8_t *mimocmd)
{
    uint32_t localVal = REG_PL_RD(MDM_MIMOCMDSTAT_ADDR);

    *mimocmdp3 = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *mimocmdp2 = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *mimocmdp1 = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *mimocmd = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the MIMOCMDP3 field in the MIMOCMDSTAT register.
 *
 * The MIMOCMDSTAT register will be read and the MIMOCMDP3 field's value will be returned.
 *
 * @return The current value of the MIMOCMDP3 field in the MIMOCMDSTAT register.
 */
__INLINE uint8_t mdm_mimocmdp3_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MIMOCMDSTAT_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Returns the current value of the MIMOCMDP2 field in the MIMOCMDSTAT register.
 *
 * The MIMOCMDSTAT register will be read and the MIMOCMDP2 field's value will be returned.
 *
 * @return The current value of the MIMOCMDP2 field in the MIMOCMDSTAT register.
 */
__INLINE uint8_t mdm_mimocmdp2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MIMOCMDSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Returns the current value of the MIMOCMDP1 field in the MIMOCMDSTAT register.
 *
 * The MIMOCMDSTAT register will be read and the MIMOCMDP1 field's value will be returned.
 *
 * @return The current value of the MIMOCMDP1 field in the MIMOCMDSTAT register.
 */
__INLINE uint8_t mdm_mimocmdp1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MIMOCMDSTAT_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Returns the current value of the MIMOCMD field in the MIMOCMDSTAT register.
 *
 * The MIMOCMDSTAT register will be read and the MIMOCMD field's value will be returned.
 *
 * @return The current value of the MIMOCMD field in the MIMOCMDSTAT register.
 */
__INLINE uint8_t mdm_mimocmd_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MIMOCMDSTAT_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/// @}

/**
 * @name TBESTAT0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  23:08     TBEMaxCorrValSum   0x0
 *  07:00             TBECOUNT   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TBESTAT0 register
#define MDM_TBESTAT0_ADDR   0x0100005C
/// Offset of the TBESTAT0 register from the base address
#define MDM_TBESTAT0_OFFSET 0x0000005C
/// Index of the TBESTAT0 register
#define MDM_TBESTAT0_INDEX  0x00000017
/// Reset value of the TBESTAT0 register
#define MDM_TBESTAT0_RESET  0x00000000

/**
 * @brief Returns the current value of the TBESTAT0 register.
 * The TBESTAT0 register will be read and its value returned.
 * @return The current value of the TBESTAT0 register.
 */
__INLINE uint32_t mdm_tbestat0_get(void)
{
    return REG_PL_RD(MDM_TBESTAT0_ADDR);
}

// field definitions
/// TBE_MAX_CORR_VAL_SUM field mask
#define MDM_TBE_MAX_CORR_VAL_SUM_MASK   ((uint32_t)0x00FFFF00)
/// TBE_MAX_CORR_VAL_SUM field LSB position
#define MDM_TBE_MAX_CORR_VAL_SUM_LSB    8
/// TBE_MAX_CORR_VAL_SUM field width
#define MDM_TBE_MAX_CORR_VAL_SUM_WIDTH  ((uint32_t)0x00000010)
/// TBECOUNT field mask
#define MDM_TBECOUNT_MASK               ((uint32_t)0x000000FF)
/// TBECOUNT field LSB position
#define MDM_TBECOUNT_LSB                0
/// TBECOUNT field width
#define MDM_TBECOUNT_WIDTH              ((uint32_t)0x00000008)

/// TBE_MAX_CORR_VAL_SUM field reset value
#define MDM_TBE_MAX_CORR_VAL_SUM_RST    0x0
/// TBECOUNT field reset value
#define MDM_TBECOUNT_RST                0x0

/**
 * @brief Unpacks TBESTAT0's fields from current value of the TBESTAT0 register.
 *
 * Reads the TBESTAT0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] tbemaxcorrvalsum - Will be populated with the current value of this field from the register.
 * @param[out] tbecount - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_tbestat0_unpack(uint16_t *tbemaxcorrvalsum, uint8_t *tbecount)
{
    uint32_t localVal = REG_PL_RD(MDM_TBESTAT0_ADDR);

    *tbemaxcorrvalsum = (localVal & ((uint32_t)0x00FFFF00)) >> 8;
    *tbecount = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the TBEMaxCorrValSum field in the TBESTAT0 register.
 *
 * The TBESTAT0 register will be read and the TBEMaxCorrValSum field's value will be returned.
 *
 * @return The current value of the TBEMaxCorrValSum field in the TBESTAT0 register.
 */
__INLINE uint16_t mdm_tbe_max_corr_val_sum_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBESTAT0_ADDR);
    return ((localVal & ((uint32_t)0x00FFFF00)) >> 8);
}

/**
 * @brief Returns the current value of the TBECOUNT field in the TBESTAT0 register.
 *
 * The TBESTAT0 register will be read and the TBECOUNT field's value will be returned.
 *
 * @return The current value of the TBECOUNT field in the TBESTAT0 register.
 */
__INLINE uint8_t mdm_tbecount_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBESTAT0_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/// @}

/**
 * @name AGCGAINSTAT register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  30:24             AGCGAIN3   0x0
 *  22:16             AGCGAIN2   0x0
 *  14:08             AGCGAIN1   0x0
 *  06:00             AGCGAIN0   0x0
 * </pre>
 *
 * @{
 */

/// Address of the AGCGAINSTAT register
#define MDM_AGCGAINSTAT_ADDR   0x01000060
/// Offset of the AGCGAINSTAT register from the base address
#define MDM_AGCGAINSTAT_OFFSET 0x00000060
/// Index of the AGCGAINSTAT register
#define MDM_AGCGAINSTAT_INDEX  0x00000018
/// Reset value of the AGCGAINSTAT register
#define MDM_AGCGAINSTAT_RESET  0x00000000

/**
 * @brief Returns the current value of the AGCGAINSTAT register.
 * The AGCGAINSTAT register will be read and its value returned.
 * @return The current value of the AGCGAINSTAT register.
 */
__INLINE uint32_t mdm_agcgainstat_get(void)
{
    return REG_PL_RD(MDM_AGCGAINSTAT_ADDR);
}

// field definitions
/// AGCGAIN3 field mask
#define MDM_AGCGAIN3_MASK   ((uint32_t)0x7F000000)
/// AGCGAIN3 field LSB position
#define MDM_AGCGAIN3_LSB    24
/// AGCGAIN3 field width
#define MDM_AGCGAIN3_WIDTH  ((uint32_t)0x00000007)
/// AGCGAIN2 field mask
#define MDM_AGCGAIN2_MASK   ((uint32_t)0x007F0000)
/// AGCGAIN2 field LSB position
#define MDM_AGCGAIN2_LSB    16
/// AGCGAIN2 field width
#define MDM_AGCGAIN2_WIDTH  ((uint32_t)0x00000007)
/// AGCGAIN1 field mask
#define MDM_AGCGAIN1_MASK   ((uint32_t)0x00007F00)
/// AGCGAIN1 field LSB position
#define MDM_AGCGAIN1_LSB    8
/// AGCGAIN1 field width
#define MDM_AGCGAIN1_WIDTH  ((uint32_t)0x00000007)
/// AGCGAIN0 field mask
#define MDM_AGCGAIN0_MASK   ((uint32_t)0x0000007F)
/// AGCGAIN0 field LSB position
#define MDM_AGCGAIN0_LSB    0
/// AGCGAIN0 field width
#define MDM_AGCGAIN0_WIDTH  ((uint32_t)0x00000007)

/// AGCGAIN3 field reset value
#define MDM_AGCGAIN3_RST    0x0
/// AGCGAIN2 field reset value
#define MDM_AGCGAIN2_RST    0x0
/// AGCGAIN1 field reset value
#define MDM_AGCGAIN1_RST    0x0
/// AGCGAIN0 field reset value
#define MDM_AGCGAIN0_RST    0x0

/**
 * @brief Unpacks AGCGAINSTAT's fields from current value of the AGCGAINSTAT register.
 *
 * Reads the AGCGAINSTAT register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] agcgain3 - Will be populated with the current value of this field from the register.
 * @param[out] agcgain2 - Will be populated with the current value of this field from the register.
 * @param[out] agcgain1 - Will be populated with the current value of this field from the register.
 * @param[out] agcgain0 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_agcgainstat_unpack(uint8_t *agcgain3, uint8_t *agcgain2, uint8_t *agcgain1, uint8_t *agcgain0)
{
    uint32_t localVal = REG_PL_RD(MDM_AGCGAINSTAT_ADDR);

    *agcgain3 = (localVal & ((uint32_t)0x7F000000)) >> 24;
    *agcgain2 = (localVal & ((uint32_t)0x007F0000)) >> 16;
    *agcgain1 = (localVal & ((uint32_t)0x00007F00)) >> 8;
    *agcgain0 = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the AGCGAIN3 field in the AGCGAINSTAT register.
 *
 * The AGCGAINSTAT register will be read and the AGCGAIN3 field's value will be returned.
 *
 * @return The current value of the AGCGAIN3 field in the AGCGAINSTAT register.
 */
__INLINE uint8_t mdm_agcgain3_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_AGCGAINSTAT_ADDR);
    return ((localVal & ((uint32_t)0x7F000000)) >> 24);
}

/**
 * @brief Returns the current value of the AGCGAIN2 field in the AGCGAINSTAT register.
 *
 * The AGCGAINSTAT register will be read and the AGCGAIN2 field's value will be returned.
 *
 * @return The current value of the AGCGAIN2 field in the AGCGAINSTAT register.
 */
__INLINE uint8_t mdm_agcgain2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_AGCGAINSTAT_ADDR);
    return ((localVal & ((uint32_t)0x007F0000)) >> 16);
}

/**
 * @brief Returns the current value of the AGCGAIN1 field in the AGCGAINSTAT register.
 *
 * The AGCGAINSTAT register will be read and the AGCGAIN1 field's value will be returned.
 *
 * @return The current value of the AGCGAIN1 field in the AGCGAINSTAT register.
 */
__INLINE uint8_t mdm_agcgain1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_AGCGAINSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}

/**
 * @brief Returns the current value of the AGCGAIN0 field in the AGCGAINSTAT register.
 *
 * The AGCGAINSTAT register will be read and the AGCGAIN0 field's value will be returned.
 *
 * @return The current value of the AGCGAIN0 field in the AGCGAINSTAT register.
 */
__INLINE uint8_t mdm_agcgain0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_AGCGAINSTAT_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/// @}

/**
 * @name AGCADCPOWSTAT register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  30:24           ADCPOWDBV3   0x0
 *  22:16           ADCPOWDBV2   0x0
 *  14:08           ADCPOWDBV1   0x0
 *  06:00           ADCPOWDBV0   0x0
 * </pre>
 *
 * @{
 */

/// Address of the AGCADCPOWSTAT register
#define MDM_AGCADCPOWSTAT_ADDR   0x01000064
/// Offset of the AGCADCPOWSTAT register from the base address
#define MDM_AGCADCPOWSTAT_OFFSET 0x00000064
/// Index of the AGCADCPOWSTAT register
#define MDM_AGCADCPOWSTAT_INDEX  0x00000019
/// Reset value of the AGCADCPOWSTAT register
#define MDM_AGCADCPOWSTAT_RESET  0x00000000

/**
 * @brief Returns the current value of the AGCADCPOWSTAT register.
 * The AGCADCPOWSTAT register will be read and its value returned.
 * @return The current value of the AGCADCPOWSTAT register.
 */
__INLINE uint32_t mdm_agcadcpowstat_get(void)
{
    return REG_PL_RD(MDM_AGCADCPOWSTAT_ADDR);
}

// field definitions
/// ADCPOWDBV3 field mask
#define MDM_ADCPOWDBV3_MASK   ((uint32_t)0x7F000000)
/// ADCPOWDBV3 field LSB position
#define MDM_ADCPOWDBV3_LSB    24
/// ADCPOWDBV3 field width
#define MDM_ADCPOWDBV3_WIDTH  ((uint32_t)0x00000007)
/// ADCPOWDBV2 field mask
#define MDM_ADCPOWDBV2_MASK   ((uint32_t)0x007F0000)
/// ADCPOWDBV2 field LSB position
#define MDM_ADCPOWDBV2_LSB    16
/// ADCPOWDBV2 field width
#define MDM_ADCPOWDBV2_WIDTH  ((uint32_t)0x00000007)
/// ADCPOWDBV1 field mask
#define MDM_ADCPOWDBV1_MASK   ((uint32_t)0x00007F00)
/// ADCPOWDBV1 field LSB position
#define MDM_ADCPOWDBV1_LSB    8
/// ADCPOWDBV1 field width
#define MDM_ADCPOWDBV1_WIDTH  ((uint32_t)0x00000007)
/// ADCPOWDBV0 field mask
#define MDM_ADCPOWDBV0_MASK   ((uint32_t)0x0000007F)
/// ADCPOWDBV0 field LSB position
#define MDM_ADCPOWDBV0_LSB    0
/// ADCPOWDBV0 field width
#define MDM_ADCPOWDBV0_WIDTH  ((uint32_t)0x00000007)

/// ADCPOWDBV3 field reset value
#define MDM_ADCPOWDBV3_RST    0x0
/// ADCPOWDBV2 field reset value
#define MDM_ADCPOWDBV2_RST    0x0
/// ADCPOWDBV1 field reset value
#define MDM_ADCPOWDBV1_RST    0x0
/// ADCPOWDBV0 field reset value
#define MDM_ADCPOWDBV0_RST    0x0

/**
 * @brief Unpacks AGCADCPOWSTAT's fields from current value of the AGCADCPOWSTAT register.
 *
 * Reads the AGCADCPOWSTAT register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] adcpowdbv3 - Will be populated with the current value of this field from the register.
 * @param[out] adcpowdbv2 - Will be populated with the current value of this field from the register.
 * @param[out] adcpowdbv1 - Will be populated with the current value of this field from the register.
 * @param[out] adcpowdbv0 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_agcadcpowstat_unpack(uint8_t *adcpowdbv3, uint8_t *adcpowdbv2, uint8_t *adcpowdbv1, uint8_t *adcpowdbv0)
{
    uint32_t localVal = REG_PL_RD(MDM_AGCADCPOWSTAT_ADDR);

    *adcpowdbv3 = (localVal & ((uint32_t)0x7F000000)) >> 24;
    *adcpowdbv2 = (localVal & ((uint32_t)0x007F0000)) >> 16;
    *adcpowdbv1 = (localVal & ((uint32_t)0x00007F00)) >> 8;
    *adcpowdbv0 = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the ADCPOWDBV3 field in the AGCADCPOWSTAT register.
 *
 * The AGCADCPOWSTAT register will be read and the ADCPOWDBV3 field's value will be returned.
 *
 * @return The current value of the ADCPOWDBV3 field in the AGCADCPOWSTAT register.
 */
__INLINE uint8_t mdm_adcpowdbv3_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_AGCADCPOWSTAT_ADDR);
    return ((localVal & ((uint32_t)0x7F000000)) >> 24);
}

/**
 * @brief Returns the current value of the ADCPOWDBV2 field in the AGCADCPOWSTAT register.
 *
 * The AGCADCPOWSTAT register will be read and the ADCPOWDBV2 field's value will be returned.
 *
 * @return The current value of the ADCPOWDBV2 field in the AGCADCPOWSTAT register.
 */
__INLINE uint8_t mdm_adcpowdbv2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_AGCADCPOWSTAT_ADDR);
    return ((localVal & ((uint32_t)0x007F0000)) >> 16);
}

/**
 * @brief Returns the current value of the ADCPOWDBV1 field in the AGCADCPOWSTAT register.
 *
 * The AGCADCPOWSTAT register will be read and the ADCPOWDBV1 field's value will be returned.
 *
 * @return The current value of the ADCPOWDBV1 field in the AGCADCPOWSTAT register.
 */
__INLINE uint8_t mdm_adcpowdbv1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_AGCADCPOWSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}

/**
 * @brief Returns the current value of the ADCPOWDBV0 field in the AGCADCPOWSTAT register.
 *
 * The AGCADCPOWSTAT register will be read and the ADCPOWDBV0 field's value will be returned.
 *
 * @return The current value of the ADCPOWDBV0 field in the AGCADCPOWSTAT register.
 */
__INLINE uint8_t mdm_adcpowdbv0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_AGCADCPOWSTAT_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/// @}

/**
 * @name RADARFIFOSTAT register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     00         RADFIFOEMPTY   0
 * </pre>
 *
 * @{
 */

/// Address of the RADARFIFOSTAT register
#define MDM_RADARFIFOSTAT_ADDR   0x01000068
/// Offset of the RADARFIFOSTAT register from the base address
#define MDM_RADARFIFOSTAT_OFFSET 0x00000068
/// Index of the RADARFIFOSTAT register
#define MDM_RADARFIFOSTAT_INDEX  0x0000001A
/// Reset value of the RADARFIFOSTAT register
#define MDM_RADARFIFOSTAT_RESET  0x00000000

/**
 * @brief Returns the current value of the RADARFIFOSTAT register.
 * The RADARFIFOSTAT register will be read and its value returned.
 * @return The current value of the RADARFIFOSTAT register.
 */
__INLINE uint32_t mdm_radarfifostat_get(void)
{
    return REG_PL_RD(MDM_RADARFIFOSTAT_ADDR);
}

// field definitions
/// RADFIFOEMPTY field bit
#define MDM_RADFIFOEMPTY_BIT    ((uint32_t)0x00000001)
/// RADFIFOEMPTY field position
#define MDM_RADFIFOEMPTY_POS    0

/// RADFIFOEMPTY field reset value
#define MDM_RADFIFOEMPTY_RST    0x0

/**
 * @brief Returns the current value of the RADFIFOEMPTY field in the RADARFIFOSTAT register.
 *
 * The RADARFIFOSTAT register will be read and the RADFIFOEMPTY field's value will be returned.
 *
 * @return The current value of the RADFIFOEMPTY field in the RADARFIFOSTAT register.
 */
__INLINE uint8_t mdm_radfifoempty_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RADARFIFOSTAT_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000001)) == 0);
    return (localVal >> 0);
}

/// @}


#endif // _REG_MDM_STAT_H_

/// @}

