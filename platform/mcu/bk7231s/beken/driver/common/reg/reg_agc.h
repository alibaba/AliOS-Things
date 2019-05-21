/**
 * @file reg_agc.h
 * @brief Definitions of the AGC HW block registers and register access functions.
 *
 * @defgroup REG_AGC REG_AGC
 * @ingroup REG
 * @{
 *
 * @brief Definitions of the AGC HW block registers and register access functions.
 */
#ifndef _REG_AGC_H_
#define _REG_AGC_H_

#include "co_int.h"
#include "_reg_agc.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"

/** @brief Number of registers in the REG_AGC peripheral.
 */
#define REG_AGC_COUNT 2091

/** @brief Decoding mask of the REG_AGC peripheral registers from the CPU point of view.
 */
#define REG_AGC_DECODING_MASK 0x00003FFF

/**
 * @name RWNXAGCGAINRG register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:28         RFLOSSANT3DB   0x0
 *  27:24         RFLOSSANT2DB   0x0
 *  23:20         RFLOSSANT1DB   0x0
 *  19:16         RFLOSSANT0DB   0x0
 *  14:08          RFGAINMAXDB   0x64
 *  06:00          RFGAINMINDB   0x0
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCGAINRG register
#define AGC_RWNXAGCGAINRG_ADDR   0x01002000
/// Offset of the RWNXAGCGAINRG register from the base address
#define AGC_RWNXAGCGAINRG_OFFSET 0x00002000
/// Index of the RWNXAGCGAINRG register
#define AGC_RWNXAGCGAINRG_INDEX  0x00000800
/// Reset value of the RWNXAGCGAINRG register
#define AGC_RWNXAGCGAINRG_RESET  0x00006400

/**
 * @brief Returns the current value of the RWNXAGCGAINRG register.
 * The RWNXAGCGAINRG register will be read and its value returned.
 * @return The current value of the RWNXAGCGAINRG register.
 */
__INLINE uint32_t agc_rwnxagcgainrg_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCGAINRG_ADDR);
}

/**
 * @brief Sets the RWNXAGCGAINRG register to a value.
 * The RWNXAGCGAINRG register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcgainrg_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCGAINRG_ADDR, value);
}

// field definitions
/// RFLOSSANT3DB field mask
#define AGC_RFLOSSANT3DB_MASK   ((uint32_t)0xF0000000)
/// RFLOSSANT3DB field LSB position
#define AGC_RFLOSSANT3DB_LSB    28
/// RFLOSSANT3DB field width
#define AGC_RFLOSSANT3DB_WIDTH  ((uint32_t)0x00000004)
/// RFLOSSANT2DB field mask
#define AGC_RFLOSSANT2DB_MASK   ((uint32_t)0x0F000000)
/// RFLOSSANT2DB field LSB position
#define AGC_RFLOSSANT2DB_LSB    24
/// RFLOSSANT2DB field width
#define AGC_RFLOSSANT2DB_WIDTH  ((uint32_t)0x00000004)
/// RFLOSSANT1DB field mask
#define AGC_RFLOSSANT1DB_MASK   ((uint32_t)0x00F00000)
/// RFLOSSANT1DB field LSB position
#define AGC_RFLOSSANT1DB_LSB    20
/// RFLOSSANT1DB field width
#define AGC_RFLOSSANT1DB_WIDTH  ((uint32_t)0x00000004)
/// RFLOSSANT0DB field mask
#define AGC_RFLOSSANT0DB_MASK   ((uint32_t)0x000F0000)
/// RFLOSSANT0DB field LSB position
#define AGC_RFLOSSANT0DB_LSB    16
/// RFLOSSANT0DB field width
#define AGC_RFLOSSANT0DB_WIDTH  ((uint32_t)0x00000004)
/// RFGAINMAXDB field mask
#define AGC_RFGAINMAXDB_MASK    ((uint32_t)0x00007F00)
/// RFGAINMAXDB field LSB position
#define AGC_RFGAINMAXDB_LSB     8
/// RFGAINMAXDB field width
#define AGC_RFGAINMAXDB_WIDTH   ((uint32_t)0x00000007)
/// RFGAINMINDB field mask
#define AGC_RFGAINMINDB_MASK    ((uint32_t)0x0000007F)
/// RFGAINMINDB field LSB position
#define AGC_RFGAINMINDB_LSB     0
/// RFGAINMINDB field width
#define AGC_RFGAINMINDB_WIDTH   ((uint32_t)0x00000007)

/// RFLOSSANT3DB field reset value
#define AGC_RFLOSSANT3DB_RST    0x0
/// RFLOSSANT2DB field reset value
#define AGC_RFLOSSANT2DB_RST    0x0
/// RFLOSSANT1DB field reset value
#define AGC_RFLOSSANT1DB_RST    0x0
/// RFLOSSANT0DB field reset value
#define AGC_RFLOSSANT0DB_RST    0x0
/// RFGAINMAXDB field reset value
#define AGC_RFGAINMAXDB_RST     0x64
/// RFGAINMINDB field reset value
#define AGC_RFGAINMINDB_RST     0x0

/**
 * @brief Constructs a value for the RWNXAGCGAINRG register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rflossant3db - The value to use for the RFLOSSANT3DB field.
 * @param[in] rflossant2db - The value to use for the RFLOSSANT2DB field.
 * @param[in] rflossant1db - The value to use for the RFLOSSANT1DB field.
 * @param[in] rflossant0db - The value to use for the RFLOSSANT0DB field.
 * @param[in] rfgainmaxdb - The value to use for the RFGAINMAXDB field.
 * @param[in] rfgainmindb - The value to use for the RFGAINMINDB field.
 */
__INLINE void agc_rwnxagcgainrg_pack(uint8_t rflossant3db, uint8_t rflossant2db, uint8_t rflossant1db, uint8_t rflossant0db, uint8_t rfgainmaxdb, uint8_t rfgainmindb)
{
    ASSERT_ERR((((uint32_t)rflossant3db << 28) & ~((uint32_t)0xF0000000)) == 0);
    ASSERT_ERR((((uint32_t)rflossant2db << 24) & ~((uint32_t)0x0F000000)) == 0);
    ASSERT_ERR((((uint32_t)rflossant1db << 20) & ~((uint32_t)0x00F00000)) == 0);
    ASSERT_ERR((((uint32_t)rflossant0db << 16) & ~((uint32_t)0x000F0000)) == 0);
    ASSERT_ERR((((uint32_t)rfgainmaxdb << 8) & ~((uint32_t)0x00007F00)) == 0);
    ASSERT_ERR((((uint32_t)rfgainmindb << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(AGC_RWNXAGCGAINRG_ADDR,  ((uint32_t)rflossant3db << 28) | ((uint32_t)rflossant2db << 24) | ((uint32_t)rflossant1db << 20) | ((uint32_t)rflossant0db << 16) | ((uint32_t)rfgainmaxdb << 8) | ((uint32_t)rfgainmindb << 0));
}

/**
 * @brief Unpacks RWNXAGCGAINRG's fields from current value of the RWNXAGCGAINRG register.
 *
 * Reads the RWNXAGCGAINRG register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rflossant3db - Will be populated with the current value of this field from the register.
 * @param[out] rflossant2db - Will be populated with the current value of this field from the register.
 * @param[out] rflossant1db - Will be populated with the current value of this field from the register.
 * @param[out] rflossant0db - Will be populated with the current value of this field from the register.
 * @param[out] rfgainmaxdb - Will be populated with the current value of this field from the register.
 * @param[out] rfgainmindb - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcgainrg_unpack(uint8_t *rflossant3db, uint8_t *rflossant2db, uint8_t *rflossant1db, uint8_t *rflossant0db, uint8_t *rfgainmaxdb, uint8_t *rfgainmindb)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCGAINRG_ADDR);

    *rflossant3db = (localVal & ((uint32_t)0xF0000000)) >> 28;
    *rflossant2db = (localVal & ((uint32_t)0x0F000000)) >> 24;
    *rflossant1db = (localVal & ((uint32_t)0x00F00000)) >> 20;
    *rflossant0db = (localVal & ((uint32_t)0x000F0000)) >> 16;
    *rfgainmaxdb = (localVal & ((uint32_t)0x00007F00)) >> 8;
    *rfgainmindb = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the RFLOSSANT3DB field in the RWNXAGCGAINRG register.
 *
 * The RWNXAGCGAINRG register will be read and the RFLOSSANT3DB field's value will be returned.
 *
 * @return The current value of the RFLOSSANT3DB field in the RWNXAGCGAINRG register.
 */
__INLINE uint8_t agc_rflossant3db_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCGAINRG_ADDR);
    return ((localVal & ((uint32_t)0xF0000000)) >> 28);
}

/**
 * @brief Sets the RFLOSSANT3DB field of the RWNXAGCGAINRG register.
 *
 * The RWNXAGCGAINRG register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rflossant3db - The value to set the field to.
 */
__INLINE void agc_rflossant3db_setf(uint8_t rflossant3db)
{
    ASSERT_ERR((((uint32_t)rflossant3db << 28) & ~((uint32_t)0xF0000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCGAINRG_ADDR, (REG_PL_RD(AGC_RWNXAGCGAINRG_ADDR) & ~((uint32_t)0xF0000000)) | ((uint32_t)rflossant3db << 28));
}

/**
 * @brief Returns the current value of the RFLOSSANT2DB field in the RWNXAGCGAINRG register.
 *
 * The RWNXAGCGAINRG register will be read and the RFLOSSANT2DB field's value will be returned.
 *
 * @return The current value of the RFLOSSANT2DB field in the RWNXAGCGAINRG register.
 */
__INLINE uint8_t agc_rflossant2db_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCGAINRG_ADDR);
    return ((localVal & ((uint32_t)0x0F000000)) >> 24);
}

/**
 * @brief Sets the RFLOSSANT2DB field of the RWNXAGCGAINRG register.
 *
 * The RWNXAGCGAINRG register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rflossant2db - The value to set the field to.
 */
__INLINE void agc_rflossant2db_setf(uint8_t rflossant2db)
{
    ASSERT_ERR((((uint32_t)rflossant2db << 24) & ~((uint32_t)0x0F000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCGAINRG_ADDR, (REG_PL_RD(AGC_RWNXAGCGAINRG_ADDR) & ~((uint32_t)0x0F000000)) | ((uint32_t)rflossant2db << 24));
}

/**
 * @brief Returns the current value of the RFLOSSANT1DB field in the RWNXAGCGAINRG register.
 *
 * The RWNXAGCGAINRG register will be read and the RFLOSSANT1DB field's value will be returned.
 *
 * @return The current value of the RFLOSSANT1DB field in the RWNXAGCGAINRG register.
 */
__INLINE uint8_t agc_rflossant1db_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCGAINRG_ADDR);
    return ((localVal & ((uint32_t)0x00F00000)) >> 20);
}

/**
 * @brief Sets the RFLOSSANT1DB field of the RWNXAGCGAINRG register.
 *
 * The RWNXAGCGAINRG register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rflossant1db - The value to set the field to.
 */
__INLINE void agc_rflossant1db_setf(uint8_t rflossant1db)
{
    ASSERT_ERR((((uint32_t)rflossant1db << 20) & ~((uint32_t)0x00F00000)) == 0);
    REG_PL_WR(AGC_RWNXAGCGAINRG_ADDR, (REG_PL_RD(AGC_RWNXAGCGAINRG_ADDR) & ~((uint32_t)0x00F00000)) | ((uint32_t)rflossant1db << 20));
}

/**
 * @brief Returns the current value of the RFLOSSANT0DB field in the RWNXAGCGAINRG register.
 *
 * The RWNXAGCGAINRG register will be read and the RFLOSSANT0DB field's value will be returned.
 *
 * @return The current value of the RFLOSSANT0DB field in the RWNXAGCGAINRG register.
 */
__INLINE uint8_t agc_rflossant0db_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCGAINRG_ADDR);
    return ((localVal & ((uint32_t)0x000F0000)) >> 16);
}

/**
 * @brief Sets the RFLOSSANT0DB field of the RWNXAGCGAINRG register.
 *
 * The RWNXAGCGAINRG register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rflossant0db - The value to set the field to.
 */
__INLINE void agc_rflossant0db_setf(uint8_t rflossant0db)
{
    ASSERT_ERR((((uint32_t)rflossant0db << 16) & ~((uint32_t)0x000F0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCGAINRG_ADDR, (REG_PL_RD(AGC_RWNXAGCGAINRG_ADDR) & ~((uint32_t)0x000F0000)) | ((uint32_t)rflossant0db << 16));
}

/**
 * @brief Returns the current value of the RFGAINMAXDB field in the RWNXAGCGAINRG register.
 *
 * The RWNXAGCGAINRG register will be read and the RFGAINMAXDB field's value will be returned.
 *
 * @return The current value of the RFGAINMAXDB field in the RWNXAGCGAINRG register.
 */
__INLINE uint8_t agc_rfgainmaxdb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCGAINRG_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}

/**
 * @brief Sets the RFGAINMAXDB field of the RWNXAGCGAINRG register.
 *
 * The RWNXAGCGAINRG register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rfgainmaxdb - The value to set the field to.
 */
__INLINE void agc_rfgainmaxdb_setf(uint8_t rfgainmaxdb)
{
    ASSERT_ERR((((uint32_t)rfgainmaxdb << 8) & ~((uint32_t)0x00007F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCGAINRG_ADDR, (REG_PL_RD(AGC_RWNXAGCGAINRG_ADDR) & ~((uint32_t)0x00007F00)) | ((uint32_t)rfgainmaxdb << 8));
}

/**
 * @brief Returns the current value of the RFGAINMINDB field in the RWNXAGCGAINRG register.
 *
 * The RWNXAGCGAINRG register will be read and the RFGAINMINDB field's value will be returned.
 *
 * @return The current value of the RFGAINMINDB field in the RWNXAGCGAINRG register.
 */
__INLINE uint8_t agc_rfgainmindb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCGAINRG_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/**
 * @brief Sets the RFGAINMINDB field of the RWNXAGCGAINRG register.
 *
 * The RWNXAGCGAINRG register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rfgainmindb - The value to set the field to.
 */
__INLINE void agc_rfgainmindb_setf(uint8_t rfgainmindb)
{
    ASSERT_ERR((((uint32_t)rfgainmindb << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(AGC_RWNXAGCGAINRG_ADDR, (REG_PL_RD(AGC_RWNXAGCGAINRG_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)rfgainmindb << 0));
}

/// @}

/**
 * @name RWNXAGCLNAVGAREF0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  30:28            VGASTEPDB   0x2
 *  27:24             VGAMINDB   0x0
 *  21:16               LNAHDB   0x1E
 *  13:08               LMAMDB   0xF
 *  06:00               LNALDB   0x0
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCLNAVGAREF0 register
#define AGC_RWNXAGCLNAVGAREF0_ADDR   0x01002004
/// Offset of the RWNXAGCLNAVGAREF0 register from the base address
#define AGC_RWNXAGCLNAVGAREF0_OFFSET 0x00002004
/// Index of the RWNXAGCLNAVGAREF0 register
#define AGC_RWNXAGCLNAVGAREF0_INDEX  0x00000801
/// Reset value of the RWNXAGCLNAVGAREF0 register
#define AGC_RWNXAGCLNAVGAREF0_RESET  0x201E0F00

/**
 * @brief Returns the current value of the RWNXAGCLNAVGAREF0 register.
 * The RWNXAGCLNAVGAREF0 register will be read and its value returned.
 * @return The current value of the RWNXAGCLNAVGAREF0 register.
 */
__INLINE uint32_t agc_rwnxagclnavgaref0_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCLNAVGAREF0_ADDR);
}

/**
 * @brief Sets the RWNXAGCLNAVGAREF0 register to a value.
 * The RWNXAGCLNAVGAREF0 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagclnavgaref0_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCLNAVGAREF0_ADDR, value);
}

// field definitions
/// VGASTEPDB field mask
#define AGC_VGASTEPDB_MASK   ((uint32_t)0x70000000)
/// VGASTEPDB field LSB position
#define AGC_VGASTEPDB_LSB    28
/// VGASTEPDB field width
#define AGC_VGASTEPDB_WIDTH  ((uint32_t)0x00000003)
/// VGAMINDB field mask
#define AGC_VGAMINDB_MASK    ((uint32_t)0x0F000000)
/// VGAMINDB field LSB position
#define AGC_VGAMINDB_LSB     24
/// VGAMINDB field width
#define AGC_VGAMINDB_WIDTH   ((uint32_t)0x00000004)
/// LNAHDB field mask
#define AGC_LNAHDB_MASK      ((uint32_t)0x003F0000)
/// LNAHDB field LSB position
#define AGC_LNAHDB_LSB       16
/// LNAHDB field width
#define AGC_LNAHDB_WIDTH     ((uint32_t)0x00000006)
/// LMAMDB field mask
#define AGC_LMAMDB_MASK      ((uint32_t)0x00003F00)
/// LMAMDB field LSB position
#define AGC_LMAMDB_LSB       8
/// LMAMDB field width
#define AGC_LMAMDB_WIDTH     ((uint32_t)0x00000006)
/// LNALDB field mask
#define AGC_LNALDB_MASK      ((uint32_t)0x0000007F)
/// LNALDB field LSB position
#define AGC_LNALDB_LSB       0
/// LNALDB field width
#define AGC_LNALDB_WIDTH     ((uint32_t)0x00000007)

/// VGASTEPDB field reset value
#define AGC_VGASTEPDB_RST    0x2
/// VGAMINDB field reset value
#define AGC_VGAMINDB_RST     0x0
/// LNAHDB field reset value
#define AGC_LNAHDB_RST       0x1E
/// LMAMDB field reset value
#define AGC_LMAMDB_RST       0xF
/// LNALDB field reset value
#define AGC_LNALDB_RST       0x0

/**
 * @brief Constructs a value for the RWNXAGCLNAVGAREF0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] vgastepdb - The value to use for the VGASTEPDB field.
 * @param[in] vgamindb - The value to use for the VGAMINDB field.
 * @param[in] lnahdb - The value to use for the LNAHDB field.
 * @param[in] lmamdb - The value to use for the LMAMDB field.
 * @param[in] lnaldb - The value to use for the LNALDB field.
 */
__INLINE void agc_rwnxagclnavgaref0_pack(uint8_t vgastepdb, uint8_t vgamindb, uint8_t lnahdb, uint8_t lmamdb, uint8_t lnaldb)
{
    ASSERT_ERR((((uint32_t)vgastepdb << 28) & ~((uint32_t)0x70000000)) == 0);
    ASSERT_ERR((((uint32_t)vgamindb << 24) & ~((uint32_t)0x0F000000)) == 0);
    ASSERT_ERR((((uint32_t)lnahdb << 16) & ~((uint32_t)0x003F0000)) == 0);
    ASSERT_ERR((((uint32_t)lmamdb << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)lnaldb << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(AGC_RWNXAGCLNAVGAREF0_ADDR,  ((uint32_t)vgastepdb << 28) | ((uint32_t)vgamindb << 24) | ((uint32_t)lnahdb << 16) | ((uint32_t)lmamdb << 8) | ((uint32_t)lnaldb << 0));
}

/**
 * @brief Unpacks RWNXAGCLNAVGAREF0's fields from current value of the RWNXAGCLNAVGAREF0 register.
 *
 * Reads the RWNXAGCLNAVGAREF0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] vgastepdb - Will be populated with the current value of this field from the register.
 * @param[out] vgamindb - Will be populated with the current value of this field from the register.
 * @param[out] lnahdb - Will be populated with the current value of this field from the register.
 * @param[out] lmamdb - Will be populated with the current value of this field from the register.
 * @param[out] lnaldb - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagclnavgaref0_unpack(uint8_t *vgastepdb, uint8_t *vgamindb, uint8_t *lnahdb, uint8_t *lmamdb, uint8_t *lnaldb)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCLNAVGAREF0_ADDR);

    *vgastepdb = (localVal & ((uint32_t)0x70000000)) >> 28;
    *vgamindb = (localVal & ((uint32_t)0x0F000000)) >> 24;
    *lnahdb = (localVal & ((uint32_t)0x003F0000)) >> 16;
    *lmamdb = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *lnaldb = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the VGASTEPDB field in the RWNXAGCLNAVGAREF0 register.
 *
 * The RWNXAGCLNAVGAREF0 register will be read and the VGASTEPDB field's value will be returned.
 *
 * @return The current value of the VGASTEPDB field in the RWNXAGCLNAVGAREF0 register.
 */
__INLINE uint8_t agc_vgastepdb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCLNAVGAREF0_ADDR);
    return ((localVal & ((uint32_t)0x70000000)) >> 28);
}

/**
 * @brief Sets the VGASTEPDB field of the RWNXAGCLNAVGAREF0 register.
 *
 * The RWNXAGCLNAVGAREF0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] vgastepdb - The value to set the field to.
 */
__INLINE void agc_vgastepdb_setf(uint8_t vgastepdb)
{
    ASSERT_ERR((((uint32_t)vgastepdb << 28) & ~((uint32_t)0x70000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCLNAVGAREF0_ADDR, (REG_PL_RD(AGC_RWNXAGCLNAVGAREF0_ADDR) & ~((uint32_t)0x70000000)) | ((uint32_t)vgastepdb << 28));
}

/**
 * @brief Returns the current value of the VGAMINDB field in the RWNXAGCLNAVGAREF0 register.
 *
 * The RWNXAGCLNAVGAREF0 register will be read and the VGAMINDB field's value will be returned.
 *
 * @return The current value of the VGAMINDB field in the RWNXAGCLNAVGAREF0 register.
 */
__INLINE uint8_t agc_vgamindb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCLNAVGAREF0_ADDR);
    return ((localVal & ((uint32_t)0x0F000000)) >> 24);
}

/**
 * @brief Sets the VGAMINDB field of the RWNXAGCLNAVGAREF0 register.
 *
 * The RWNXAGCLNAVGAREF0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] vgamindb - The value to set the field to.
 */
__INLINE void agc_vgamindb_setf(uint8_t vgamindb)
{
    ASSERT_ERR((((uint32_t)vgamindb << 24) & ~((uint32_t)0x0F000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCLNAVGAREF0_ADDR, (REG_PL_RD(AGC_RWNXAGCLNAVGAREF0_ADDR) & ~((uint32_t)0x0F000000)) | ((uint32_t)vgamindb << 24));
}

/**
 * @brief Returns the current value of the LNAHDB field in the RWNXAGCLNAVGAREF0 register.
 *
 * The RWNXAGCLNAVGAREF0 register will be read and the LNAHDB field's value will be returned.
 *
 * @return The current value of the LNAHDB field in the RWNXAGCLNAVGAREF0 register.
 */
__INLINE uint8_t agc_lnahdb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCLNAVGAREF0_ADDR);
    return ((localVal & ((uint32_t)0x003F0000)) >> 16);
}

/**
 * @brief Sets the LNAHDB field of the RWNXAGCLNAVGAREF0 register.
 *
 * The RWNXAGCLNAVGAREF0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] lnahdb - The value to set the field to.
 */
__INLINE void agc_lnahdb_setf(uint8_t lnahdb)
{
    ASSERT_ERR((((uint32_t)lnahdb << 16) & ~((uint32_t)0x003F0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCLNAVGAREF0_ADDR, (REG_PL_RD(AGC_RWNXAGCLNAVGAREF0_ADDR) & ~((uint32_t)0x003F0000)) | ((uint32_t)lnahdb << 16));
}

/**
 * @brief Returns the current value of the LMAMDB field in the RWNXAGCLNAVGAREF0 register.
 *
 * The RWNXAGCLNAVGAREF0 register will be read and the LMAMDB field's value will be returned.
 *
 * @return The current value of the LMAMDB field in the RWNXAGCLNAVGAREF0 register.
 */
__INLINE uint8_t agc_lmamdb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCLNAVGAREF0_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the LMAMDB field of the RWNXAGCLNAVGAREF0 register.
 *
 * The RWNXAGCLNAVGAREF0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] lmamdb - The value to set the field to.
 */
__INLINE void agc_lmamdb_setf(uint8_t lmamdb)
{
    ASSERT_ERR((((uint32_t)lmamdb << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCLNAVGAREF0_ADDR, (REG_PL_RD(AGC_RWNXAGCLNAVGAREF0_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)lmamdb << 8));
}

/**
 * @brief Returns the current value of the LNALDB field in the RWNXAGCLNAVGAREF0 register.
 *
 * The RWNXAGCLNAVGAREF0 register will be read and the LNALDB field's value will be returned.
 *
 * @return The current value of the LNALDB field in the RWNXAGCLNAVGAREF0 register.
 */
__INLINE uint8_t agc_lnaldb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCLNAVGAREF0_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/**
 * @brief Sets the LNALDB field of the RWNXAGCLNAVGAREF0 register.
 *
 * The RWNXAGCLNAVGAREF0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] lnaldb - The value to set the field to.
 */
__INLINE void agc_lnaldb_setf(uint8_t lnaldb)
{
    ASSERT_ERR((((uint32_t)lnaldb << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(AGC_RWNXAGCLNAVGAREF0_ADDR, (REG_PL_RD(AGC_RWNXAGCLNAVGAREF0_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)lnaldb << 0));
}

/// @}

/**
 * @name RWNXAGCLNAVGAREF1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:28        DIGGAIN80VS20   0x0
 *  27:24        DIGGAIN40VS20   0x0
 *  20:16            VGAINDMAX   0x1F
 *  13:08           LNAHMTHRDB   0x1E
 *  05:00           LNAMLTHRDB   0xF
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCLNAVGAREF1 register
#define AGC_RWNXAGCLNAVGAREF1_ADDR   0x01002008
/// Offset of the RWNXAGCLNAVGAREF1 register from the base address
#define AGC_RWNXAGCLNAVGAREF1_OFFSET 0x00002008
/// Index of the RWNXAGCLNAVGAREF1 register
#define AGC_RWNXAGCLNAVGAREF1_INDEX  0x00000802
/// Reset value of the RWNXAGCLNAVGAREF1 register
#define AGC_RWNXAGCLNAVGAREF1_RESET  0x001F1E0F

/**
 * @brief Returns the current value of the RWNXAGCLNAVGAREF1 register.
 * The RWNXAGCLNAVGAREF1 register will be read and its value returned.
 * @return The current value of the RWNXAGCLNAVGAREF1 register.
 */
__INLINE uint32_t agc_rwnxagclnavgaref1_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCLNAVGAREF1_ADDR);
}

/**
 * @brief Sets the RWNXAGCLNAVGAREF1 register to a value.
 * The RWNXAGCLNAVGAREF1 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagclnavgaref1_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCLNAVGAREF1_ADDR, value);
}

// field definitions
/// DIGGAIN80VS20 field mask
#define AGC_DIGGAIN80VS20_MASK   ((uint32_t)0xF0000000)
/// DIGGAIN80VS20 field LSB position
#define AGC_DIGGAIN80VS20_LSB    28
/// DIGGAIN80VS20 field width
#define AGC_DIGGAIN80VS20_WIDTH  ((uint32_t)0x00000004)
/// DIGGAIN40VS20 field mask
#define AGC_DIGGAIN40VS20_MASK   ((uint32_t)0x0F000000)
/// DIGGAIN40VS20 field LSB position
#define AGC_DIGGAIN40VS20_LSB    24
/// DIGGAIN40VS20 field width
#define AGC_DIGGAIN40VS20_WIDTH  ((uint32_t)0x00000004)
/// VGAINDMAX field mask
#define AGC_VGAINDMAX_MASK       ((uint32_t)0x001F0000)
/// VGAINDMAX field LSB position
#define AGC_VGAINDMAX_LSB        16
/// VGAINDMAX field width
#define AGC_VGAINDMAX_WIDTH      ((uint32_t)0x00000005)
/// LNAHMTHRDB field mask
#define AGC_LNAHMTHRDB_MASK      ((uint32_t)0x00003F00)
/// LNAHMTHRDB field LSB position
#define AGC_LNAHMTHRDB_LSB       8
/// LNAHMTHRDB field width
#define AGC_LNAHMTHRDB_WIDTH     ((uint32_t)0x00000006)
/// LNAMLTHRDB field mask
#define AGC_LNAMLTHRDB_MASK      ((uint32_t)0x0000003F)
/// LNAMLTHRDB field LSB position
#define AGC_LNAMLTHRDB_LSB       0
/// LNAMLTHRDB field width
#define AGC_LNAMLTHRDB_WIDTH     ((uint32_t)0x00000006)

/// DIGGAIN80VS20 field reset value
#define AGC_DIGGAIN80VS20_RST    0x0
/// DIGGAIN40VS20 field reset value
#define AGC_DIGGAIN40VS20_RST    0x0
/// VGAINDMAX field reset value
#define AGC_VGAINDMAX_RST        0x1F
/// LNAHMTHRDB field reset value
#define AGC_LNAHMTHRDB_RST       0x1E
/// LNAMLTHRDB field reset value
#define AGC_LNAMLTHRDB_RST       0xF

/**
 * @brief Constructs a value for the RWNXAGCLNAVGAREF1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] diggain80vs20 - The value to use for the DIGGAIN80VS20 field.
 * @param[in] diggain40vs20 - The value to use for the DIGGAIN40VS20 field.
 * @param[in] vgaindmax - The value to use for the VGAINDMAX field.
 * @param[in] lnahmthrdb - The value to use for the LNAHMTHRDB field.
 * @param[in] lnamlthrdb - The value to use for the LNAMLTHRDB field.
 */
__INLINE void agc_rwnxagclnavgaref1_pack(uint8_t diggain80vs20, uint8_t diggain40vs20, uint8_t vgaindmax, uint8_t lnahmthrdb, uint8_t lnamlthrdb)
{
    ASSERT_ERR((((uint32_t)diggain80vs20 << 28) & ~((uint32_t)0xF0000000)) == 0);
    ASSERT_ERR((((uint32_t)diggain40vs20 << 24) & ~((uint32_t)0x0F000000)) == 0);
    ASSERT_ERR((((uint32_t)vgaindmax << 16) & ~((uint32_t)0x001F0000)) == 0);
    ASSERT_ERR((((uint32_t)lnahmthrdb << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)lnamlthrdb << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCLNAVGAREF1_ADDR,  ((uint32_t)diggain80vs20 << 28) | ((uint32_t)diggain40vs20 << 24) | ((uint32_t)vgaindmax << 16) | ((uint32_t)lnahmthrdb << 8) | ((uint32_t)lnamlthrdb << 0));
}

/**
 * @brief Unpacks RWNXAGCLNAVGAREF1's fields from current value of the RWNXAGCLNAVGAREF1 register.
 *
 * Reads the RWNXAGCLNAVGAREF1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] diggain80vs20 - Will be populated with the current value of this field from the register.
 * @param[out] diggain40vs20 - Will be populated with the current value of this field from the register.
 * @param[out] vgaindmax - Will be populated with the current value of this field from the register.
 * @param[out] lnahmthrdb - Will be populated with the current value of this field from the register.
 * @param[out] lnamlthrdb - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagclnavgaref1_unpack(uint8_t *diggain80vs20, uint8_t *diggain40vs20, uint8_t *vgaindmax, uint8_t *lnahmthrdb, uint8_t *lnamlthrdb)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCLNAVGAREF1_ADDR);

    *diggain80vs20 = (localVal & ((uint32_t)0xF0000000)) >> 28;
    *diggain40vs20 = (localVal & ((uint32_t)0x0F000000)) >> 24;
    *vgaindmax = (localVal & ((uint32_t)0x001F0000)) >> 16;
    *lnahmthrdb = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *lnamlthrdb = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the DIGGAIN80VS20 field in the RWNXAGCLNAVGAREF1 register.
 *
 * The RWNXAGCLNAVGAREF1 register will be read and the DIGGAIN80VS20 field's value will be returned.
 *
 * @return The current value of the DIGGAIN80VS20 field in the RWNXAGCLNAVGAREF1 register.
 */
__INLINE uint8_t agc_diggain80vs20_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCLNAVGAREF1_ADDR);
    return ((localVal & ((uint32_t)0xF0000000)) >> 28);
}

/**
 * @brief Sets the DIGGAIN80VS20 field of the RWNXAGCLNAVGAREF1 register.
 *
 * The RWNXAGCLNAVGAREF1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diggain80vs20 - The value to set the field to.
 */
__INLINE void agc_diggain80vs20_setf(uint8_t diggain80vs20)
{
    ASSERT_ERR((((uint32_t)diggain80vs20 << 28) & ~((uint32_t)0xF0000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCLNAVGAREF1_ADDR, (REG_PL_RD(AGC_RWNXAGCLNAVGAREF1_ADDR) & ~((uint32_t)0xF0000000)) | ((uint32_t)diggain80vs20 << 28));
}

/**
 * @brief Returns the current value of the DIGGAIN40VS20 field in the RWNXAGCLNAVGAREF1 register.
 *
 * The RWNXAGCLNAVGAREF1 register will be read and the DIGGAIN40VS20 field's value will be returned.
 *
 * @return The current value of the DIGGAIN40VS20 field in the RWNXAGCLNAVGAREF1 register.
 */
__INLINE uint8_t agc_diggain40vs20_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCLNAVGAREF1_ADDR);
    return ((localVal & ((uint32_t)0x0F000000)) >> 24);
}

/**
 * @brief Sets the DIGGAIN40VS20 field of the RWNXAGCLNAVGAREF1 register.
 *
 * The RWNXAGCLNAVGAREF1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] diggain40vs20 - The value to set the field to.
 */
__INLINE void agc_diggain40vs20_setf(uint8_t diggain40vs20)
{
    ASSERT_ERR((((uint32_t)diggain40vs20 << 24) & ~((uint32_t)0x0F000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCLNAVGAREF1_ADDR, (REG_PL_RD(AGC_RWNXAGCLNAVGAREF1_ADDR) & ~((uint32_t)0x0F000000)) | ((uint32_t)diggain40vs20 << 24));
}

/**
 * @brief Returns the current value of the VGAINDMAX field in the RWNXAGCLNAVGAREF1 register.
 *
 * The RWNXAGCLNAVGAREF1 register will be read and the VGAINDMAX field's value will be returned.
 *
 * @return The current value of the VGAINDMAX field in the RWNXAGCLNAVGAREF1 register.
 */
__INLINE uint8_t agc_vgaindmax_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCLNAVGAREF1_ADDR);
    return ((localVal & ((uint32_t)0x001F0000)) >> 16);
}

/**
 * @brief Sets the VGAINDMAX field of the RWNXAGCLNAVGAREF1 register.
 *
 * The RWNXAGCLNAVGAREF1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] vgaindmax - The value to set the field to.
 */
__INLINE void agc_vgaindmax_setf(uint8_t vgaindmax)
{
    ASSERT_ERR((((uint32_t)vgaindmax << 16) & ~((uint32_t)0x001F0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCLNAVGAREF1_ADDR, (REG_PL_RD(AGC_RWNXAGCLNAVGAREF1_ADDR) & ~((uint32_t)0x001F0000)) | ((uint32_t)vgaindmax << 16));
}

/**
 * @brief Returns the current value of the LNAHMTHRDB field in the RWNXAGCLNAVGAREF1 register.
 *
 * The RWNXAGCLNAVGAREF1 register will be read and the LNAHMTHRDB field's value will be returned.
 *
 * @return The current value of the LNAHMTHRDB field in the RWNXAGCLNAVGAREF1 register.
 */
__INLINE uint8_t agc_lnahmthrdb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCLNAVGAREF1_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the LNAHMTHRDB field of the RWNXAGCLNAVGAREF1 register.
 *
 * The RWNXAGCLNAVGAREF1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] lnahmthrdb - The value to set the field to.
 */
__INLINE void agc_lnahmthrdb_setf(uint8_t lnahmthrdb)
{
    ASSERT_ERR((((uint32_t)lnahmthrdb << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCLNAVGAREF1_ADDR, (REG_PL_RD(AGC_RWNXAGCLNAVGAREF1_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)lnahmthrdb << 8));
}

/**
 * @brief Returns the current value of the LNAMLTHRDB field in the RWNXAGCLNAVGAREF1 register.
 *
 * The RWNXAGCLNAVGAREF1 register will be read and the LNAMLTHRDB field's value will be returned.
 *
 * @return The current value of the LNAMLTHRDB field in the RWNXAGCLNAVGAREF1 register.
 */
__INLINE uint8_t agc_lnamlthrdb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCLNAVGAREF1_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the LNAMLTHRDB field of the RWNXAGCLNAVGAREF1 register.
 *
 * The RWNXAGCLNAVGAREF1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] lnamlthrdb - The value to set the field to.
 */
__INLINE void agc_lnamlthrdb_setf(uint8_t lnamlthrdb)
{
    ASSERT_ERR((((uint32_t)lnamlthrdb << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCLNAVGAREF1_ADDR, (REG_PL_RD(AGC_RWNXAGCLNAVGAREF1_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)lnamlthrdb << 0));
}

/// @}

/**
 * @name RWNXAGCACI20MARG0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:24        ACI20MARGMCS3   0x2
 *  21:16        ACI20MARGMCS2   0x4
 *  13:08        ACI20MARGMCS1   0x5
 *  05:00        ACI20MARGMCS0   0x7
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCACI20MARG0 register
#define AGC_RWNXAGCACI20MARG0_ADDR   0x0100200C
/// Offset of the RWNXAGCACI20MARG0 register from the base address
#define AGC_RWNXAGCACI20MARG0_OFFSET 0x0000200C
/// Index of the RWNXAGCACI20MARG0 register
#define AGC_RWNXAGCACI20MARG0_INDEX  0x00000803
/// Reset value of the RWNXAGCACI20MARG0 register
#define AGC_RWNXAGCACI20MARG0_RESET  0x02040507

/**
 * @brief Returns the current value of the RWNXAGCACI20MARG0 register.
 * The RWNXAGCACI20MARG0 register will be read and its value returned.
 * @return The current value of the RWNXAGCACI20MARG0 register.
 */
__INLINE uint32_t agc_rwnxagcaci20marg0_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCACI20MARG0_ADDR);
}

/**
 * @brief Sets the RWNXAGCACI20MARG0 register to a value.
 * The RWNXAGCACI20MARG0 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcaci20marg0_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCACI20MARG0_ADDR, value);
}

// field definitions
/// ACI20MARGMCS3 field mask
#define AGC_ACI20MARGMCS3_MASK   ((uint32_t)0x3F000000)
/// ACI20MARGMCS3 field LSB position
#define AGC_ACI20MARGMCS3_LSB    24
/// ACI20MARGMCS3 field width
#define AGC_ACI20MARGMCS3_WIDTH  ((uint32_t)0x00000006)
/// ACI20MARGMCS2 field mask
#define AGC_ACI20MARGMCS2_MASK   ((uint32_t)0x003F0000)
/// ACI20MARGMCS2 field LSB position
#define AGC_ACI20MARGMCS2_LSB    16
/// ACI20MARGMCS2 field width
#define AGC_ACI20MARGMCS2_WIDTH  ((uint32_t)0x00000006)
/// ACI20MARGMCS1 field mask
#define AGC_ACI20MARGMCS1_MASK   ((uint32_t)0x00003F00)
/// ACI20MARGMCS1 field LSB position
#define AGC_ACI20MARGMCS1_LSB    8
/// ACI20MARGMCS1 field width
#define AGC_ACI20MARGMCS1_WIDTH  ((uint32_t)0x00000006)
/// ACI20MARGMCS0 field mask
#define AGC_ACI20MARGMCS0_MASK   ((uint32_t)0x0000003F)
/// ACI20MARGMCS0 field LSB position
#define AGC_ACI20MARGMCS0_LSB    0
/// ACI20MARGMCS0 field width
#define AGC_ACI20MARGMCS0_WIDTH  ((uint32_t)0x00000006)

/// ACI20MARGMCS3 field reset value
#define AGC_ACI20MARGMCS3_RST    0x2
/// ACI20MARGMCS2 field reset value
#define AGC_ACI20MARGMCS2_RST    0x4
/// ACI20MARGMCS1 field reset value
#define AGC_ACI20MARGMCS1_RST    0x5
/// ACI20MARGMCS0 field reset value
#define AGC_ACI20MARGMCS0_RST    0x7

/**
 * @brief Constructs a value for the RWNXAGCACI20MARG0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] aci20margmcs3 - The value to use for the ACI20MARGMCS3 field.
 * @param[in] aci20margmcs2 - The value to use for the ACI20MARGMCS2 field.
 * @param[in] aci20margmcs1 - The value to use for the ACI20MARGMCS1 field.
 * @param[in] aci20margmcs0 - The value to use for the ACI20MARGMCS0 field.
 */
__INLINE void agc_rwnxagcaci20marg0_pack(uint8_t aci20margmcs3, uint8_t aci20margmcs2, uint8_t aci20margmcs1, uint8_t aci20margmcs0)
{
    ASSERT_ERR((((uint32_t)aci20margmcs3 << 24) & ~((uint32_t)0x3F000000)) == 0);
    ASSERT_ERR((((uint32_t)aci20margmcs2 << 16) & ~((uint32_t)0x003F0000)) == 0);
    ASSERT_ERR((((uint32_t)aci20margmcs1 << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)aci20margmcs0 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI20MARG0_ADDR,  ((uint32_t)aci20margmcs3 << 24) | ((uint32_t)aci20margmcs2 << 16) | ((uint32_t)aci20margmcs1 << 8) | ((uint32_t)aci20margmcs0 << 0));
}

/**
 * @brief Unpacks RWNXAGCACI20MARG0's fields from current value of the RWNXAGCACI20MARG0 register.
 *
 * Reads the RWNXAGCACI20MARG0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] aci20margmcs3 - Will be populated with the current value of this field from the register.
 * @param[out] aci20margmcs2 - Will be populated with the current value of this field from the register.
 * @param[out] aci20margmcs1 - Will be populated with the current value of this field from the register.
 * @param[out] aci20margmcs0 - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcaci20marg0_unpack(uint8_t *aci20margmcs3, uint8_t *aci20margmcs2, uint8_t *aci20margmcs1, uint8_t *aci20margmcs0)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI20MARG0_ADDR);

    *aci20margmcs3 = (localVal & ((uint32_t)0x3F000000)) >> 24;
    *aci20margmcs2 = (localVal & ((uint32_t)0x003F0000)) >> 16;
    *aci20margmcs1 = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *aci20margmcs0 = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the ACI20MARGMCS3 field in the RWNXAGCACI20MARG0 register.
 *
 * The RWNXAGCACI20MARG0 register will be read and the ACI20MARGMCS3 field's value will be returned.
 *
 * @return The current value of the ACI20MARGMCS3 field in the RWNXAGCACI20MARG0 register.
 */
__INLINE uint8_t agc_aci20margmcs3_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI20MARG0_ADDR);
    return ((localVal & ((uint32_t)0x3F000000)) >> 24);
}

/**
 * @brief Sets the ACI20MARGMCS3 field of the RWNXAGCACI20MARG0 register.
 *
 * The RWNXAGCACI20MARG0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci20margmcs3 - The value to set the field to.
 */
__INLINE void agc_aci20margmcs3_setf(uint8_t aci20margmcs3)
{
    ASSERT_ERR((((uint32_t)aci20margmcs3 << 24) & ~((uint32_t)0x3F000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI20MARG0_ADDR, (REG_PL_RD(AGC_RWNXAGCACI20MARG0_ADDR) & ~((uint32_t)0x3F000000)) | ((uint32_t)aci20margmcs3 << 24));
}

/**
 * @brief Returns the current value of the ACI20MARGMCS2 field in the RWNXAGCACI20MARG0 register.
 *
 * The RWNXAGCACI20MARG0 register will be read and the ACI20MARGMCS2 field's value will be returned.
 *
 * @return The current value of the ACI20MARGMCS2 field in the RWNXAGCACI20MARG0 register.
 */
__INLINE uint8_t agc_aci20margmcs2_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI20MARG0_ADDR);
    return ((localVal & ((uint32_t)0x003F0000)) >> 16);
}

/**
 * @brief Sets the ACI20MARGMCS2 field of the RWNXAGCACI20MARG0 register.
 *
 * The RWNXAGCACI20MARG0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci20margmcs2 - The value to set the field to.
 */
__INLINE void agc_aci20margmcs2_setf(uint8_t aci20margmcs2)
{
    ASSERT_ERR((((uint32_t)aci20margmcs2 << 16) & ~((uint32_t)0x003F0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI20MARG0_ADDR, (REG_PL_RD(AGC_RWNXAGCACI20MARG0_ADDR) & ~((uint32_t)0x003F0000)) | ((uint32_t)aci20margmcs2 << 16));
}

/**
 * @brief Returns the current value of the ACI20MARGMCS1 field in the RWNXAGCACI20MARG0 register.
 *
 * The RWNXAGCACI20MARG0 register will be read and the ACI20MARGMCS1 field's value will be returned.
 *
 * @return The current value of the ACI20MARGMCS1 field in the RWNXAGCACI20MARG0 register.
 */
__INLINE uint8_t agc_aci20margmcs1_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI20MARG0_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the ACI20MARGMCS1 field of the RWNXAGCACI20MARG0 register.
 *
 * The RWNXAGCACI20MARG0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci20margmcs1 - The value to set the field to.
 */
__INLINE void agc_aci20margmcs1_setf(uint8_t aci20margmcs1)
{
    ASSERT_ERR((((uint32_t)aci20margmcs1 << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI20MARG0_ADDR, (REG_PL_RD(AGC_RWNXAGCACI20MARG0_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)aci20margmcs1 << 8));
}

/**
 * @brief Returns the current value of the ACI20MARGMCS0 field in the RWNXAGCACI20MARG0 register.
 *
 * The RWNXAGCACI20MARG0 register will be read and the ACI20MARGMCS0 field's value will be returned.
 *
 * @return The current value of the ACI20MARGMCS0 field in the RWNXAGCACI20MARG0 register.
 */
__INLINE uint8_t agc_aci20margmcs0_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI20MARG0_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the ACI20MARGMCS0 field of the RWNXAGCACI20MARG0 register.
 *
 * The RWNXAGCACI20MARG0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci20margmcs0 - The value to set the field to.
 */
__INLINE void agc_aci20margmcs0_setf(uint8_t aci20margmcs0)
{
    ASSERT_ERR((((uint32_t)aci20margmcs0 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI20MARG0_ADDR, (REG_PL_RD(AGC_RWNXAGCACI20MARG0_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)aci20margmcs0 << 0));
}

/// @}

/**
 * @name RWNXAGCACI20MARG1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:24        ACI20MARGMCS7   0x0
 *  21:16        ACI20MARGMCS6   0x0
 *  13:08        ACI20MARGMCS5   0x0
 *  05:00        ACI20MARGMCS4   0x1
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCACI20MARG1 register
#define AGC_RWNXAGCACI20MARG1_ADDR   0x01002010
/// Offset of the RWNXAGCACI20MARG1 register from the base address
#define AGC_RWNXAGCACI20MARG1_OFFSET 0x00002010
/// Index of the RWNXAGCACI20MARG1 register
#define AGC_RWNXAGCACI20MARG1_INDEX  0x00000804
/// Reset value of the RWNXAGCACI20MARG1 register
#define AGC_RWNXAGCACI20MARG1_RESET  0x00000001

/**
 * @brief Returns the current value of the RWNXAGCACI20MARG1 register.
 * The RWNXAGCACI20MARG1 register will be read and its value returned.
 * @return The current value of the RWNXAGCACI20MARG1 register.
 */
__INLINE uint32_t agc_rwnxagcaci20marg1_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCACI20MARG1_ADDR);
}

/**
 * @brief Sets the RWNXAGCACI20MARG1 register to a value.
 * The RWNXAGCACI20MARG1 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcaci20marg1_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCACI20MARG1_ADDR, value);
}

// field definitions
/// ACI20MARGMCS7 field mask
#define AGC_ACI20MARGMCS7_MASK   ((uint32_t)0x3F000000)
/// ACI20MARGMCS7 field LSB position
#define AGC_ACI20MARGMCS7_LSB    24
/// ACI20MARGMCS7 field width
#define AGC_ACI20MARGMCS7_WIDTH  ((uint32_t)0x00000006)
/// ACI20MARGMCS6 field mask
#define AGC_ACI20MARGMCS6_MASK   ((uint32_t)0x003F0000)
/// ACI20MARGMCS6 field LSB position
#define AGC_ACI20MARGMCS6_LSB    16
/// ACI20MARGMCS6 field width
#define AGC_ACI20MARGMCS6_WIDTH  ((uint32_t)0x00000006)
/// ACI20MARGMCS5 field mask
#define AGC_ACI20MARGMCS5_MASK   ((uint32_t)0x00003F00)
/// ACI20MARGMCS5 field LSB position
#define AGC_ACI20MARGMCS5_LSB    8
/// ACI20MARGMCS5 field width
#define AGC_ACI20MARGMCS5_WIDTH  ((uint32_t)0x00000006)
/// ACI20MARGMCS4 field mask
#define AGC_ACI20MARGMCS4_MASK   ((uint32_t)0x0000003F)
/// ACI20MARGMCS4 field LSB position
#define AGC_ACI20MARGMCS4_LSB    0
/// ACI20MARGMCS4 field width
#define AGC_ACI20MARGMCS4_WIDTH  ((uint32_t)0x00000006)

/// ACI20MARGMCS7 field reset value
#define AGC_ACI20MARGMCS7_RST    0x0
/// ACI20MARGMCS6 field reset value
#define AGC_ACI20MARGMCS6_RST    0x0
/// ACI20MARGMCS5 field reset value
#define AGC_ACI20MARGMCS5_RST    0x0
/// ACI20MARGMCS4 field reset value
#define AGC_ACI20MARGMCS4_RST    0x1

/**
 * @brief Constructs a value for the RWNXAGCACI20MARG1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] aci20margmcs7 - The value to use for the ACI20MARGMCS7 field.
 * @param[in] aci20margmcs6 - The value to use for the ACI20MARGMCS6 field.
 * @param[in] aci20margmcs5 - The value to use for the ACI20MARGMCS5 field.
 * @param[in] aci20margmcs4 - The value to use for the ACI20MARGMCS4 field.
 */
__INLINE void agc_rwnxagcaci20marg1_pack(uint8_t aci20margmcs7, uint8_t aci20margmcs6, uint8_t aci20margmcs5, uint8_t aci20margmcs4)
{
    ASSERT_ERR((((uint32_t)aci20margmcs7 << 24) & ~((uint32_t)0x3F000000)) == 0);
    ASSERT_ERR((((uint32_t)aci20margmcs6 << 16) & ~((uint32_t)0x003F0000)) == 0);
    ASSERT_ERR((((uint32_t)aci20margmcs5 << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)aci20margmcs4 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI20MARG1_ADDR,  ((uint32_t)aci20margmcs7 << 24) | ((uint32_t)aci20margmcs6 << 16) | ((uint32_t)aci20margmcs5 << 8) | ((uint32_t)aci20margmcs4 << 0));
}

/**
 * @brief Unpacks RWNXAGCACI20MARG1's fields from current value of the RWNXAGCACI20MARG1 register.
 *
 * Reads the RWNXAGCACI20MARG1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] aci20margmcs7 - Will be populated with the current value of this field from the register.
 * @param[out] aci20margmcs6 - Will be populated with the current value of this field from the register.
 * @param[out] aci20margmcs5 - Will be populated with the current value of this field from the register.
 * @param[out] aci20margmcs4 - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcaci20marg1_unpack(uint8_t *aci20margmcs7, uint8_t *aci20margmcs6, uint8_t *aci20margmcs5, uint8_t *aci20margmcs4)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI20MARG1_ADDR);

    *aci20margmcs7 = (localVal & ((uint32_t)0x3F000000)) >> 24;
    *aci20margmcs6 = (localVal & ((uint32_t)0x003F0000)) >> 16;
    *aci20margmcs5 = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *aci20margmcs4 = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the ACI20MARGMCS7 field in the RWNXAGCACI20MARG1 register.
 *
 * The RWNXAGCACI20MARG1 register will be read and the ACI20MARGMCS7 field's value will be returned.
 *
 * @return The current value of the ACI20MARGMCS7 field in the RWNXAGCACI20MARG1 register.
 */
__INLINE uint8_t agc_aci20margmcs7_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI20MARG1_ADDR);
    return ((localVal & ((uint32_t)0x3F000000)) >> 24);
}

/**
 * @brief Sets the ACI20MARGMCS7 field of the RWNXAGCACI20MARG1 register.
 *
 * The RWNXAGCACI20MARG1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci20margmcs7 - The value to set the field to.
 */
__INLINE void agc_aci20margmcs7_setf(uint8_t aci20margmcs7)
{
    ASSERT_ERR((((uint32_t)aci20margmcs7 << 24) & ~((uint32_t)0x3F000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI20MARG1_ADDR, (REG_PL_RD(AGC_RWNXAGCACI20MARG1_ADDR) & ~((uint32_t)0x3F000000)) | ((uint32_t)aci20margmcs7 << 24));
}

/**
 * @brief Returns the current value of the ACI20MARGMCS6 field in the RWNXAGCACI20MARG1 register.
 *
 * The RWNXAGCACI20MARG1 register will be read and the ACI20MARGMCS6 field's value will be returned.
 *
 * @return The current value of the ACI20MARGMCS6 field in the RWNXAGCACI20MARG1 register.
 */
__INLINE uint8_t agc_aci20margmcs6_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI20MARG1_ADDR);
    return ((localVal & ((uint32_t)0x003F0000)) >> 16);
}

/**
 * @brief Sets the ACI20MARGMCS6 field of the RWNXAGCACI20MARG1 register.
 *
 * The RWNXAGCACI20MARG1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci20margmcs6 - The value to set the field to.
 */
__INLINE void agc_aci20margmcs6_setf(uint8_t aci20margmcs6)
{
    ASSERT_ERR((((uint32_t)aci20margmcs6 << 16) & ~((uint32_t)0x003F0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI20MARG1_ADDR, (REG_PL_RD(AGC_RWNXAGCACI20MARG1_ADDR) & ~((uint32_t)0x003F0000)) | ((uint32_t)aci20margmcs6 << 16));
}

/**
 * @brief Returns the current value of the ACI20MARGMCS5 field in the RWNXAGCACI20MARG1 register.
 *
 * The RWNXAGCACI20MARG1 register will be read and the ACI20MARGMCS5 field's value will be returned.
 *
 * @return The current value of the ACI20MARGMCS5 field in the RWNXAGCACI20MARG1 register.
 */
__INLINE uint8_t agc_aci20margmcs5_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI20MARG1_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the ACI20MARGMCS5 field of the RWNXAGCACI20MARG1 register.
 *
 * The RWNXAGCACI20MARG1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci20margmcs5 - The value to set the field to.
 */
__INLINE void agc_aci20margmcs5_setf(uint8_t aci20margmcs5)
{
    ASSERT_ERR((((uint32_t)aci20margmcs5 << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI20MARG1_ADDR, (REG_PL_RD(AGC_RWNXAGCACI20MARG1_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)aci20margmcs5 << 8));
}

/**
 * @brief Returns the current value of the ACI20MARGMCS4 field in the RWNXAGCACI20MARG1 register.
 *
 * The RWNXAGCACI20MARG1 register will be read and the ACI20MARGMCS4 field's value will be returned.
 *
 * @return The current value of the ACI20MARGMCS4 field in the RWNXAGCACI20MARG1 register.
 */
__INLINE uint8_t agc_aci20margmcs4_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI20MARG1_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the ACI20MARGMCS4 field of the RWNXAGCACI20MARG1 register.
 *
 * The RWNXAGCACI20MARG1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci20margmcs4 - The value to set the field to.
 */
__INLINE void agc_aci20margmcs4_setf(uint8_t aci20margmcs4)
{
    ASSERT_ERR((((uint32_t)aci20margmcs4 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI20MARG1_ADDR, (REG_PL_RD(AGC_RWNXAGCACI20MARG1_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)aci20margmcs4 << 0));
}

/// @}

/**
 * @name RWNXAGCACI20MARG2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  13:08        ACI20MARGMCS9   0x0
 *  05:00        ACI20MARGMCS8   0x0
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCACI20MARG2 register
#define AGC_RWNXAGCACI20MARG2_ADDR   0x01002014
/// Offset of the RWNXAGCACI20MARG2 register from the base address
#define AGC_RWNXAGCACI20MARG2_OFFSET 0x00002014
/// Index of the RWNXAGCACI20MARG2 register
#define AGC_RWNXAGCACI20MARG2_INDEX  0x00000805
/// Reset value of the RWNXAGCACI20MARG2 register
#define AGC_RWNXAGCACI20MARG2_RESET  0x00000000

/**
 * @brief Returns the current value of the RWNXAGCACI20MARG2 register.
 * The RWNXAGCACI20MARG2 register will be read and its value returned.
 * @return The current value of the RWNXAGCACI20MARG2 register.
 */
__INLINE uint32_t agc_rwnxagcaci20marg2_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCACI20MARG2_ADDR);
}

/**
 * @brief Sets the RWNXAGCACI20MARG2 register to a value.
 * The RWNXAGCACI20MARG2 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcaci20marg2_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCACI20MARG2_ADDR, value);
}

// field definitions
/// ACI20MARGMCS9 field mask
#define AGC_ACI20MARGMCS9_MASK   ((uint32_t)0x00003F00)
/// ACI20MARGMCS9 field LSB position
#define AGC_ACI20MARGMCS9_LSB    8
/// ACI20MARGMCS9 field width
#define AGC_ACI20MARGMCS9_WIDTH  ((uint32_t)0x00000006)
/// ACI20MARGMCS8 field mask
#define AGC_ACI20MARGMCS8_MASK   ((uint32_t)0x0000003F)
/// ACI20MARGMCS8 field LSB position
#define AGC_ACI20MARGMCS8_LSB    0
/// ACI20MARGMCS8 field width
#define AGC_ACI20MARGMCS8_WIDTH  ((uint32_t)0x00000006)

/// ACI20MARGMCS9 field reset value
#define AGC_ACI20MARGMCS9_RST    0x0
/// ACI20MARGMCS8 field reset value
#define AGC_ACI20MARGMCS8_RST    0x0

/**
 * @brief Constructs a value for the RWNXAGCACI20MARG2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] aci20margmcs9 - The value to use for the ACI20MARGMCS9 field.
 * @param[in] aci20margmcs8 - The value to use for the ACI20MARGMCS8 field.
 */
__INLINE void agc_rwnxagcaci20marg2_pack(uint8_t aci20margmcs9, uint8_t aci20margmcs8)
{
    ASSERT_ERR((((uint32_t)aci20margmcs9 << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)aci20margmcs8 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI20MARG2_ADDR,  ((uint32_t)aci20margmcs9 << 8) | ((uint32_t)aci20margmcs8 << 0));
}

/**
 * @brief Unpacks RWNXAGCACI20MARG2's fields from current value of the RWNXAGCACI20MARG2 register.
 *
 * Reads the RWNXAGCACI20MARG2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] aci20margmcs9 - Will be populated with the current value of this field from the register.
 * @param[out] aci20margmcs8 - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcaci20marg2_unpack(uint8_t *aci20margmcs9, uint8_t *aci20margmcs8)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI20MARG2_ADDR);

    *aci20margmcs9 = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *aci20margmcs8 = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the ACI20MARGMCS9 field in the RWNXAGCACI20MARG2 register.
 *
 * The RWNXAGCACI20MARG2 register will be read and the ACI20MARGMCS9 field's value will be returned.
 *
 * @return The current value of the ACI20MARGMCS9 field in the RWNXAGCACI20MARG2 register.
 */
__INLINE uint8_t agc_aci20margmcs9_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI20MARG2_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the ACI20MARGMCS9 field of the RWNXAGCACI20MARG2 register.
 *
 * The RWNXAGCACI20MARG2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci20margmcs9 - The value to set the field to.
 */
__INLINE void agc_aci20margmcs9_setf(uint8_t aci20margmcs9)
{
    ASSERT_ERR((((uint32_t)aci20margmcs9 << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI20MARG2_ADDR, (REG_PL_RD(AGC_RWNXAGCACI20MARG2_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)aci20margmcs9 << 8));
}

/**
 * @brief Returns the current value of the ACI20MARGMCS8 field in the RWNXAGCACI20MARG2 register.
 *
 * The RWNXAGCACI20MARG2 register will be read and the ACI20MARGMCS8 field's value will be returned.
 *
 * @return The current value of the ACI20MARGMCS8 field in the RWNXAGCACI20MARG2 register.
 */
__INLINE uint8_t agc_aci20margmcs8_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI20MARG2_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the ACI20MARGMCS8 field of the RWNXAGCACI20MARG2 register.
 *
 * The RWNXAGCACI20MARG2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci20margmcs8 - The value to set the field to.
 */
__INLINE void agc_aci20margmcs8_setf(uint8_t aci20margmcs8)
{
    ASSERT_ERR((((uint32_t)aci20margmcs8 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI20MARG2_ADDR, (REG_PL_RD(AGC_RWNXAGCACI20MARG2_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)aci20margmcs8 << 0));
}

/// @}

/**
 * @name RWNXAGCACI40MARG0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:24        ACI40MARGMCS3   0x9
 *  21:16        ACI40MARGMCS2   0xB
 *  13:08        ACI40MARGMCS1   0xD
 *  05:00        ACI40MARGMCS0   0x10
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCACI40MARG0 register
#define AGC_RWNXAGCACI40MARG0_ADDR   0x01002018
/// Offset of the RWNXAGCACI40MARG0 register from the base address
#define AGC_RWNXAGCACI40MARG0_OFFSET 0x00002018
/// Index of the RWNXAGCACI40MARG0 register
#define AGC_RWNXAGCACI40MARG0_INDEX  0x00000806
/// Reset value of the RWNXAGCACI40MARG0 register
#define AGC_RWNXAGCACI40MARG0_RESET  0x090B0D10

/**
 * @brief Returns the current value of the RWNXAGCACI40MARG0 register.
 * The RWNXAGCACI40MARG0 register will be read and its value returned.
 * @return The current value of the RWNXAGCACI40MARG0 register.
 */
__INLINE uint32_t agc_rwnxagcaci40marg0_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCACI40MARG0_ADDR);
}

/**
 * @brief Sets the RWNXAGCACI40MARG0 register to a value.
 * The RWNXAGCACI40MARG0 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcaci40marg0_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCACI40MARG0_ADDR, value);
}

// field definitions
/// ACI40MARGMCS3 field mask
#define AGC_ACI40MARGMCS3_MASK   ((uint32_t)0x3F000000)
/// ACI40MARGMCS3 field LSB position
#define AGC_ACI40MARGMCS3_LSB    24
/// ACI40MARGMCS3 field width
#define AGC_ACI40MARGMCS3_WIDTH  ((uint32_t)0x00000006)
/// ACI40MARGMCS2 field mask
#define AGC_ACI40MARGMCS2_MASK   ((uint32_t)0x003F0000)
/// ACI40MARGMCS2 field LSB position
#define AGC_ACI40MARGMCS2_LSB    16
/// ACI40MARGMCS2 field width
#define AGC_ACI40MARGMCS2_WIDTH  ((uint32_t)0x00000006)
/// ACI40MARGMCS1 field mask
#define AGC_ACI40MARGMCS1_MASK   ((uint32_t)0x00003F00)
/// ACI40MARGMCS1 field LSB position
#define AGC_ACI40MARGMCS1_LSB    8
/// ACI40MARGMCS1 field width
#define AGC_ACI40MARGMCS1_WIDTH  ((uint32_t)0x00000006)
/// ACI40MARGMCS0 field mask
#define AGC_ACI40MARGMCS0_MASK   ((uint32_t)0x0000003F)
/// ACI40MARGMCS0 field LSB position
#define AGC_ACI40MARGMCS0_LSB    0
/// ACI40MARGMCS0 field width
#define AGC_ACI40MARGMCS0_WIDTH  ((uint32_t)0x00000006)

/// ACI40MARGMCS3 field reset value
#define AGC_ACI40MARGMCS3_RST    0x9
/// ACI40MARGMCS2 field reset value
#define AGC_ACI40MARGMCS2_RST    0xB
/// ACI40MARGMCS1 field reset value
#define AGC_ACI40MARGMCS1_RST    0xD
/// ACI40MARGMCS0 field reset value
#define AGC_ACI40MARGMCS0_RST    0x10

/**
 * @brief Constructs a value for the RWNXAGCACI40MARG0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] aci40margmcs3 - The value to use for the ACI40MARGMCS3 field.
 * @param[in] aci40margmcs2 - The value to use for the ACI40MARGMCS2 field.
 * @param[in] aci40margmcs1 - The value to use for the ACI40MARGMCS1 field.
 * @param[in] aci40margmcs0 - The value to use for the ACI40MARGMCS0 field.
 */
__INLINE void agc_rwnxagcaci40marg0_pack(uint8_t aci40margmcs3, uint8_t aci40margmcs2, uint8_t aci40margmcs1, uint8_t aci40margmcs0)
{
    ASSERT_ERR((((uint32_t)aci40margmcs3 << 24) & ~((uint32_t)0x3F000000)) == 0);
    ASSERT_ERR((((uint32_t)aci40margmcs2 << 16) & ~((uint32_t)0x003F0000)) == 0);
    ASSERT_ERR((((uint32_t)aci40margmcs1 << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)aci40margmcs0 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI40MARG0_ADDR,  ((uint32_t)aci40margmcs3 << 24) | ((uint32_t)aci40margmcs2 << 16) | ((uint32_t)aci40margmcs1 << 8) | ((uint32_t)aci40margmcs0 << 0));
}

/**
 * @brief Unpacks RWNXAGCACI40MARG0's fields from current value of the RWNXAGCACI40MARG0 register.
 *
 * Reads the RWNXAGCACI40MARG0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] aci40margmcs3 - Will be populated with the current value of this field from the register.
 * @param[out] aci40margmcs2 - Will be populated with the current value of this field from the register.
 * @param[out] aci40margmcs1 - Will be populated with the current value of this field from the register.
 * @param[out] aci40margmcs0 - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcaci40marg0_unpack(uint8_t *aci40margmcs3, uint8_t *aci40margmcs2, uint8_t *aci40margmcs1, uint8_t *aci40margmcs0)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI40MARG0_ADDR);

    *aci40margmcs3 = (localVal & ((uint32_t)0x3F000000)) >> 24;
    *aci40margmcs2 = (localVal & ((uint32_t)0x003F0000)) >> 16;
    *aci40margmcs1 = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *aci40margmcs0 = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the ACI40MARGMCS3 field in the RWNXAGCACI40MARG0 register.
 *
 * The RWNXAGCACI40MARG0 register will be read and the ACI40MARGMCS3 field's value will be returned.
 *
 * @return The current value of the ACI40MARGMCS3 field in the RWNXAGCACI40MARG0 register.
 */
__INLINE uint8_t agc_aci40margmcs3_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI40MARG0_ADDR);
    return ((localVal & ((uint32_t)0x3F000000)) >> 24);
}

/**
 * @brief Sets the ACI40MARGMCS3 field of the RWNXAGCACI40MARG0 register.
 *
 * The RWNXAGCACI40MARG0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci40margmcs3 - The value to set the field to.
 */
__INLINE void agc_aci40margmcs3_setf(uint8_t aci40margmcs3)
{
    ASSERT_ERR((((uint32_t)aci40margmcs3 << 24) & ~((uint32_t)0x3F000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI40MARG0_ADDR, (REG_PL_RD(AGC_RWNXAGCACI40MARG0_ADDR) & ~((uint32_t)0x3F000000)) | ((uint32_t)aci40margmcs3 << 24));
}

/**
 * @brief Returns the current value of the ACI40MARGMCS2 field in the RWNXAGCACI40MARG0 register.
 *
 * The RWNXAGCACI40MARG0 register will be read and the ACI40MARGMCS2 field's value will be returned.
 *
 * @return The current value of the ACI40MARGMCS2 field in the RWNXAGCACI40MARG0 register.
 */
__INLINE uint8_t agc_aci40margmcs2_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI40MARG0_ADDR);
    return ((localVal & ((uint32_t)0x003F0000)) >> 16);
}

/**
 * @brief Sets the ACI40MARGMCS2 field of the RWNXAGCACI40MARG0 register.
 *
 * The RWNXAGCACI40MARG0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci40margmcs2 - The value to set the field to.
 */
__INLINE void agc_aci40margmcs2_setf(uint8_t aci40margmcs2)
{
    ASSERT_ERR((((uint32_t)aci40margmcs2 << 16) & ~((uint32_t)0x003F0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI40MARG0_ADDR, (REG_PL_RD(AGC_RWNXAGCACI40MARG0_ADDR) & ~((uint32_t)0x003F0000)) | ((uint32_t)aci40margmcs2 << 16));
}

/**
 * @brief Returns the current value of the ACI40MARGMCS1 field in the RWNXAGCACI40MARG0 register.
 *
 * The RWNXAGCACI40MARG0 register will be read and the ACI40MARGMCS1 field's value will be returned.
 *
 * @return The current value of the ACI40MARGMCS1 field in the RWNXAGCACI40MARG0 register.
 */
__INLINE uint8_t agc_aci40margmcs1_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI40MARG0_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the ACI40MARGMCS1 field of the RWNXAGCACI40MARG0 register.
 *
 * The RWNXAGCACI40MARG0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci40margmcs1 - The value to set the field to.
 */
__INLINE void agc_aci40margmcs1_setf(uint8_t aci40margmcs1)
{
    ASSERT_ERR((((uint32_t)aci40margmcs1 << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI40MARG0_ADDR, (REG_PL_RD(AGC_RWNXAGCACI40MARG0_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)aci40margmcs1 << 8));
}

/**
 * @brief Returns the current value of the ACI40MARGMCS0 field in the RWNXAGCACI40MARG0 register.
 *
 * The RWNXAGCACI40MARG0 register will be read and the ACI40MARGMCS0 field's value will be returned.
 *
 * @return The current value of the ACI40MARGMCS0 field in the RWNXAGCACI40MARG0 register.
 */
__INLINE uint8_t agc_aci40margmcs0_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI40MARG0_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the ACI40MARGMCS0 field of the RWNXAGCACI40MARG0 register.
 *
 * The RWNXAGCACI40MARG0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci40margmcs0 - The value to set the field to.
 */
__INLINE void agc_aci40margmcs0_setf(uint8_t aci40margmcs0)
{
    ASSERT_ERR((((uint32_t)aci40margmcs0 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI40MARG0_ADDR, (REG_PL_RD(AGC_RWNXAGCACI40MARG0_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)aci40margmcs0 << 0));
}

/// @}

/**
 * @name RWNXAGCACI40MARG1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:24        ACI40MARGMCS7   0x2
 *  21:16        ACI40MARGMCS6   0x3
 *  13:08        ACI40MARGMCS5   0x3
 *  05:00        ACI40MARGMCS4   0x5
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCACI40MARG1 register
#define AGC_RWNXAGCACI40MARG1_ADDR   0x0100201C
/// Offset of the RWNXAGCACI40MARG1 register from the base address
#define AGC_RWNXAGCACI40MARG1_OFFSET 0x0000201C
/// Index of the RWNXAGCACI40MARG1 register
#define AGC_RWNXAGCACI40MARG1_INDEX  0x00000807
/// Reset value of the RWNXAGCACI40MARG1 register
#define AGC_RWNXAGCACI40MARG1_RESET  0x02030305

/**
 * @brief Returns the current value of the RWNXAGCACI40MARG1 register.
 * The RWNXAGCACI40MARG1 register will be read and its value returned.
 * @return The current value of the RWNXAGCACI40MARG1 register.
 */
__INLINE uint32_t agc_rwnxagcaci40marg1_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCACI40MARG1_ADDR);
}

/**
 * @brief Sets the RWNXAGCACI40MARG1 register to a value.
 * The RWNXAGCACI40MARG1 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcaci40marg1_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCACI40MARG1_ADDR, value);
}

// field definitions
/// ACI40MARGMCS7 field mask
#define AGC_ACI40MARGMCS7_MASK   ((uint32_t)0x3F000000)
/// ACI40MARGMCS7 field LSB position
#define AGC_ACI40MARGMCS7_LSB    24
/// ACI40MARGMCS7 field width
#define AGC_ACI40MARGMCS7_WIDTH  ((uint32_t)0x00000006)
/// ACI40MARGMCS6 field mask
#define AGC_ACI40MARGMCS6_MASK   ((uint32_t)0x003F0000)
/// ACI40MARGMCS6 field LSB position
#define AGC_ACI40MARGMCS6_LSB    16
/// ACI40MARGMCS6 field width
#define AGC_ACI40MARGMCS6_WIDTH  ((uint32_t)0x00000006)
/// ACI40MARGMCS5 field mask
#define AGC_ACI40MARGMCS5_MASK   ((uint32_t)0x00003F00)
/// ACI40MARGMCS5 field LSB position
#define AGC_ACI40MARGMCS5_LSB    8
/// ACI40MARGMCS5 field width
#define AGC_ACI40MARGMCS5_WIDTH  ((uint32_t)0x00000006)
/// ACI40MARGMCS4 field mask
#define AGC_ACI40MARGMCS4_MASK   ((uint32_t)0x0000003F)
/// ACI40MARGMCS4 field LSB position
#define AGC_ACI40MARGMCS4_LSB    0
/// ACI40MARGMCS4 field width
#define AGC_ACI40MARGMCS4_WIDTH  ((uint32_t)0x00000006)

/// ACI40MARGMCS7 field reset value
#define AGC_ACI40MARGMCS7_RST    0x2
/// ACI40MARGMCS6 field reset value
#define AGC_ACI40MARGMCS6_RST    0x3
/// ACI40MARGMCS5 field reset value
#define AGC_ACI40MARGMCS5_RST    0x3
/// ACI40MARGMCS4 field reset value
#define AGC_ACI40MARGMCS4_RST    0x5

/**
 * @brief Constructs a value for the RWNXAGCACI40MARG1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] aci40margmcs7 - The value to use for the ACI40MARGMCS7 field.
 * @param[in] aci40margmcs6 - The value to use for the ACI40MARGMCS6 field.
 * @param[in] aci40margmcs5 - The value to use for the ACI40MARGMCS5 field.
 * @param[in] aci40margmcs4 - The value to use for the ACI40MARGMCS4 field.
 */
__INLINE void agc_rwnxagcaci40marg1_pack(uint8_t aci40margmcs7, uint8_t aci40margmcs6, uint8_t aci40margmcs5, uint8_t aci40margmcs4)
{
    ASSERT_ERR((((uint32_t)aci40margmcs7 << 24) & ~((uint32_t)0x3F000000)) == 0);
    ASSERT_ERR((((uint32_t)aci40margmcs6 << 16) & ~((uint32_t)0x003F0000)) == 0);
    ASSERT_ERR((((uint32_t)aci40margmcs5 << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)aci40margmcs4 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI40MARG1_ADDR,  ((uint32_t)aci40margmcs7 << 24) | ((uint32_t)aci40margmcs6 << 16) | ((uint32_t)aci40margmcs5 << 8) | ((uint32_t)aci40margmcs4 << 0));
}

/**
 * @brief Unpacks RWNXAGCACI40MARG1's fields from current value of the RWNXAGCACI40MARG1 register.
 *
 * Reads the RWNXAGCACI40MARG1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] aci40margmcs7 - Will be populated with the current value of this field from the register.
 * @param[out] aci40margmcs6 - Will be populated with the current value of this field from the register.
 * @param[out] aci40margmcs5 - Will be populated with the current value of this field from the register.
 * @param[out] aci40margmcs4 - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcaci40marg1_unpack(uint8_t *aci40margmcs7, uint8_t *aci40margmcs6, uint8_t *aci40margmcs5, uint8_t *aci40margmcs4)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI40MARG1_ADDR);

    *aci40margmcs7 = (localVal & ((uint32_t)0x3F000000)) >> 24;
    *aci40margmcs6 = (localVal & ((uint32_t)0x003F0000)) >> 16;
    *aci40margmcs5 = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *aci40margmcs4 = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the ACI40MARGMCS7 field in the RWNXAGCACI40MARG1 register.
 *
 * The RWNXAGCACI40MARG1 register will be read and the ACI40MARGMCS7 field's value will be returned.
 *
 * @return The current value of the ACI40MARGMCS7 field in the RWNXAGCACI40MARG1 register.
 */
__INLINE uint8_t agc_aci40margmcs7_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI40MARG1_ADDR);
    return ((localVal & ((uint32_t)0x3F000000)) >> 24);
}

/**
 * @brief Sets the ACI40MARGMCS7 field of the RWNXAGCACI40MARG1 register.
 *
 * The RWNXAGCACI40MARG1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci40margmcs7 - The value to set the field to.
 */
__INLINE void agc_aci40margmcs7_setf(uint8_t aci40margmcs7)
{
    ASSERT_ERR((((uint32_t)aci40margmcs7 << 24) & ~((uint32_t)0x3F000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI40MARG1_ADDR, (REG_PL_RD(AGC_RWNXAGCACI40MARG1_ADDR) & ~((uint32_t)0x3F000000)) | ((uint32_t)aci40margmcs7 << 24));
}

/**
 * @brief Returns the current value of the ACI40MARGMCS6 field in the RWNXAGCACI40MARG1 register.
 *
 * The RWNXAGCACI40MARG1 register will be read and the ACI40MARGMCS6 field's value will be returned.
 *
 * @return The current value of the ACI40MARGMCS6 field in the RWNXAGCACI40MARG1 register.
 */
__INLINE uint8_t agc_aci40margmcs6_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI40MARG1_ADDR);
    return ((localVal & ((uint32_t)0x003F0000)) >> 16);
}

/**
 * @brief Sets the ACI40MARGMCS6 field of the RWNXAGCACI40MARG1 register.
 *
 * The RWNXAGCACI40MARG1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci40margmcs6 - The value to set the field to.
 */
__INLINE void agc_aci40margmcs6_setf(uint8_t aci40margmcs6)
{
    ASSERT_ERR((((uint32_t)aci40margmcs6 << 16) & ~((uint32_t)0x003F0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI40MARG1_ADDR, (REG_PL_RD(AGC_RWNXAGCACI40MARG1_ADDR) & ~((uint32_t)0x003F0000)) | ((uint32_t)aci40margmcs6 << 16));
}

/**
 * @brief Returns the current value of the ACI40MARGMCS5 field in the RWNXAGCACI40MARG1 register.
 *
 * The RWNXAGCACI40MARG1 register will be read and the ACI40MARGMCS5 field's value will be returned.
 *
 * @return The current value of the ACI40MARGMCS5 field in the RWNXAGCACI40MARG1 register.
 */
__INLINE uint8_t agc_aci40margmcs5_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI40MARG1_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the ACI40MARGMCS5 field of the RWNXAGCACI40MARG1 register.
 *
 * The RWNXAGCACI40MARG1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci40margmcs5 - The value to set the field to.
 */
__INLINE void agc_aci40margmcs5_setf(uint8_t aci40margmcs5)
{
    ASSERT_ERR((((uint32_t)aci40margmcs5 << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI40MARG1_ADDR, (REG_PL_RD(AGC_RWNXAGCACI40MARG1_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)aci40margmcs5 << 8));
}

/**
 * @brief Returns the current value of the ACI40MARGMCS4 field in the RWNXAGCACI40MARG1 register.
 *
 * The RWNXAGCACI40MARG1 register will be read and the ACI40MARGMCS4 field's value will be returned.
 *
 * @return The current value of the ACI40MARGMCS4 field in the RWNXAGCACI40MARG1 register.
 */
__INLINE uint8_t agc_aci40margmcs4_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI40MARG1_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the ACI40MARGMCS4 field of the RWNXAGCACI40MARG1 register.
 *
 * The RWNXAGCACI40MARG1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci40margmcs4 - The value to set the field to.
 */
__INLINE void agc_aci40margmcs4_setf(uint8_t aci40margmcs4)
{
    ASSERT_ERR((((uint32_t)aci40margmcs4 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI40MARG1_ADDR, (REG_PL_RD(AGC_RWNXAGCACI40MARG1_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)aci40margmcs4 << 0));
}

/// @}

/**
 * @name RWNXAGCACI40MARG2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  13:08        ACI40MARGMCS9   0x1
 *  05:00        ACI40MARGMCS8   0x1
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCACI40MARG2 register
#define AGC_RWNXAGCACI40MARG2_ADDR   0x01002020
/// Offset of the RWNXAGCACI40MARG2 register from the base address
#define AGC_RWNXAGCACI40MARG2_OFFSET 0x00002020
/// Index of the RWNXAGCACI40MARG2 register
#define AGC_RWNXAGCACI40MARG2_INDEX  0x00000808
/// Reset value of the RWNXAGCACI40MARG2 register
#define AGC_RWNXAGCACI40MARG2_RESET  0x00000101

/**
 * @brief Returns the current value of the RWNXAGCACI40MARG2 register.
 * The RWNXAGCACI40MARG2 register will be read and its value returned.
 * @return The current value of the RWNXAGCACI40MARG2 register.
 */
__INLINE uint32_t agc_rwnxagcaci40marg2_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCACI40MARG2_ADDR);
}

/**
 * @brief Sets the RWNXAGCACI40MARG2 register to a value.
 * The RWNXAGCACI40MARG2 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcaci40marg2_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCACI40MARG2_ADDR, value);
}

// field definitions
/// ACI40MARGMCS9 field mask
#define AGC_ACI40MARGMCS9_MASK   ((uint32_t)0x00003F00)
/// ACI40MARGMCS9 field LSB position
#define AGC_ACI40MARGMCS9_LSB    8
/// ACI40MARGMCS9 field width
#define AGC_ACI40MARGMCS9_WIDTH  ((uint32_t)0x00000006)
/// ACI40MARGMCS8 field mask
#define AGC_ACI40MARGMCS8_MASK   ((uint32_t)0x0000003F)
/// ACI40MARGMCS8 field LSB position
#define AGC_ACI40MARGMCS8_LSB    0
/// ACI40MARGMCS8 field width
#define AGC_ACI40MARGMCS8_WIDTH  ((uint32_t)0x00000006)

/// ACI40MARGMCS9 field reset value
#define AGC_ACI40MARGMCS9_RST    0x1
/// ACI40MARGMCS8 field reset value
#define AGC_ACI40MARGMCS8_RST    0x1

/**
 * @brief Constructs a value for the RWNXAGCACI40MARG2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] aci40margmcs9 - The value to use for the ACI40MARGMCS9 field.
 * @param[in] aci40margmcs8 - The value to use for the ACI40MARGMCS8 field.
 */
__INLINE void agc_rwnxagcaci40marg2_pack(uint8_t aci40margmcs9, uint8_t aci40margmcs8)
{
    ASSERT_ERR((((uint32_t)aci40margmcs9 << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)aci40margmcs8 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI40MARG2_ADDR,  ((uint32_t)aci40margmcs9 << 8) | ((uint32_t)aci40margmcs8 << 0));
}

/**
 * @brief Unpacks RWNXAGCACI40MARG2's fields from current value of the RWNXAGCACI40MARG2 register.
 *
 * Reads the RWNXAGCACI40MARG2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] aci40margmcs9 - Will be populated with the current value of this field from the register.
 * @param[out] aci40margmcs8 - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcaci40marg2_unpack(uint8_t *aci40margmcs9, uint8_t *aci40margmcs8)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI40MARG2_ADDR);

    *aci40margmcs9 = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *aci40margmcs8 = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the ACI40MARGMCS9 field in the RWNXAGCACI40MARG2 register.
 *
 * The RWNXAGCACI40MARG2 register will be read and the ACI40MARGMCS9 field's value will be returned.
 *
 * @return The current value of the ACI40MARGMCS9 field in the RWNXAGCACI40MARG2 register.
 */
__INLINE uint8_t agc_aci40margmcs9_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI40MARG2_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the ACI40MARGMCS9 field of the RWNXAGCACI40MARG2 register.
 *
 * The RWNXAGCACI40MARG2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci40margmcs9 - The value to set the field to.
 */
__INLINE void agc_aci40margmcs9_setf(uint8_t aci40margmcs9)
{
    ASSERT_ERR((((uint32_t)aci40margmcs9 << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI40MARG2_ADDR, (REG_PL_RD(AGC_RWNXAGCACI40MARG2_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)aci40margmcs9 << 8));
}

/**
 * @brief Returns the current value of the ACI40MARGMCS8 field in the RWNXAGCACI40MARG2 register.
 *
 * The RWNXAGCACI40MARG2 register will be read and the ACI40MARGMCS8 field's value will be returned.
 *
 * @return The current value of the ACI40MARGMCS8 field in the RWNXAGCACI40MARG2 register.
 */
__INLINE uint8_t agc_aci40margmcs8_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI40MARG2_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the ACI40MARGMCS8 field of the RWNXAGCACI40MARG2 register.
 *
 * The RWNXAGCACI40MARG2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci40margmcs8 - The value to set the field to.
 */
__INLINE void agc_aci40margmcs8_setf(uint8_t aci40margmcs8)
{
    ASSERT_ERR((((uint32_t)aci40margmcs8 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI40MARG2_ADDR, (REG_PL_RD(AGC_RWNXAGCACI40MARG2_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)aci40margmcs8 << 0));
}

/// @}

/**
 * @name RWNXAGCACI80MARG0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:24        ACI80MARGMCS3   0x18
 *  21:16        ACI80MARGMCS2   0x1B
 *  13:08        ACI80MARGMCS1   0x1D
 *  05:00        ACI80MARGMCS0   0x20
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCACI80MARG0 register
#define AGC_RWNXAGCACI80MARG0_ADDR   0x01002024
/// Offset of the RWNXAGCACI80MARG0 register from the base address
#define AGC_RWNXAGCACI80MARG0_OFFSET 0x00002024
/// Index of the RWNXAGCACI80MARG0 register
#define AGC_RWNXAGCACI80MARG0_INDEX  0x00000809
/// Reset value of the RWNXAGCACI80MARG0 register
#define AGC_RWNXAGCACI80MARG0_RESET  0x181B1D20

/**
 * @brief Returns the current value of the RWNXAGCACI80MARG0 register.
 * The RWNXAGCACI80MARG0 register will be read and its value returned.
 * @return The current value of the RWNXAGCACI80MARG0 register.
 */
__INLINE uint32_t agc_rwnxagcaci80marg0_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCACI80MARG0_ADDR);
}

/**
 * @brief Sets the RWNXAGCACI80MARG0 register to a value.
 * The RWNXAGCACI80MARG0 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcaci80marg0_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCACI80MARG0_ADDR, value);
}

// field definitions
/// ACI80MARGMCS3 field mask
#define AGC_ACI80MARGMCS3_MASK   ((uint32_t)0x3F000000)
/// ACI80MARGMCS3 field LSB position
#define AGC_ACI80MARGMCS3_LSB    24
/// ACI80MARGMCS3 field width
#define AGC_ACI80MARGMCS3_WIDTH  ((uint32_t)0x00000006)
/// ACI80MARGMCS2 field mask
#define AGC_ACI80MARGMCS2_MASK   ((uint32_t)0x003F0000)
/// ACI80MARGMCS2 field LSB position
#define AGC_ACI80MARGMCS2_LSB    16
/// ACI80MARGMCS2 field width
#define AGC_ACI80MARGMCS2_WIDTH  ((uint32_t)0x00000006)
/// ACI80MARGMCS1 field mask
#define AGC_ACI80MARGMCS1_MASK   ((uint32_t)0x00003F00)
/// ACI80MARGMCS1 field LSB position
#define AGC_ACI80MARGMCS1_LSB    8
/// ACI80MARGMCS1 field width
#define AGC_ACI80MARGMCS1_WIDTH  ((uint32_t)0x00000006)
/// ACI80MARGMCS0 field mask
#define AGC_ACI80MARGMCS0_MASK   ((uint32_t)0x0000003F)
/// ACI80MARGMCS0 field LSB position
#define AGC_ACI80MARGMCS0_LSB    0
/// ACI80MARGMCS0 field width
#define AGC_ACI80MARGMCS0_WIDTH  ((uint32_t)0x00000006)

/// ACI80MARGMCS3 field reset value
#define AGC_ACI80MARGMCS3_RST    0x18
/// ACI80MARGMCS2 field reset value
#define AGC_ACI80MARGMCS2_RST    0x1B
/// ACI80MARGMCS1 field reset value
#define AGC_ACI80MARGMCS1_RST    0x1D
/// ACI80MARGMCS0 field reset value
#define AGC_ACI80MARGMCS0_RST    0x20

/**
 * @brief Constructs a value for the RWNXAGCACI80MARG0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] aci80margmcs3 - The value to use for the ACI80MARGMCS3 field.
 * @param[in] aci80margmcs2 - The value to use for the ACI80MARGMCS2 field.
 * @param[in] aci80margmcs1 - The value to use for the ACI80MARGMCS1 field.
 * @param[in] aci80margmcs0 - The value to use for the ACI80MARGMCS0 field.
 */
__INLINE void agc_rwnxagcaci80marg0_pack(uint8_t aci80margmcs3, uint8_t aci80margmcs2, uint8_t aci80margmcs1, uint8_t aci80margmcs0)
{
    ASSERT_ERR((((uint32_t)aci80margmcs3 << 24) & ~((uint32_t)0x3F000000)) == 0);
    ASSERT_ERR((((uint32_t)aci80margmcs2 << 16) & ~((uint32_t)0x003F0000)) == 0);
    ASSERT_ERR((((uint32_t)aci80margmcs1 << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)aci80margmcs0 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI80MARG0_ADDR,  ((uint32_t)aci80margmcs3 << 24) | ((uint32_t)aci80margmcs2 << 16) | ((uint32_t)aci80margmcs1 << 8) | ((uint32_t)aci80margmcs0 << 0));
}

/**
 * @brief Unpacks RWNXAGCACI80MARG0's fields from current value of the RWNXAGCACI80MARG0 register.
 *
 * Reads the RWNXAGCACI80MARG0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] aci80margmcs3 - Will be populated with the current value of this field from the register.
 * @param[out] aci80margmcs2 - Will be populated with the current value of this field from the register.
 * @param[out] aci80margmcs1 - Will be populated with the current value of this field from the register.
 * @param[out] aci80margmcs0 - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcaci80marg0_unpack(uint8_t *aci80margmcs3, uint8_t *aci80margmcs2, uint8_t *aci80margmcs1, uint8_t *aci80margmcs0)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI80MARG0_ADDR);

    *aci80margmcs3 = (localVal & ((uint32_t)0x3F000000)) >> 24;
    *aci80margmcs2 = (localVal & ((uint32_t)0x003F0000)) >> 16;
    *aci80margmcs1 = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *aci80margmcs0 = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the ACI80MARGMCS3 field in the RWNXAGCACI80MARG0 register.
 *
 * The RWNXAGCACI80MARG0 register will be read and the ACI80MARGMCS3 field's value will be returned.
 *
 * @return The current value of the ACI80MARGMCS3 field in the RWNXAGCACI80MARG0 register.
 */
__INLINE uint8_t agc_aci80margmcs3_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI80MARG0_ADDR);
    return ((localVal & ((uint32_t)0x3F000000)) >> 24);
}

/**
 * @brief Sets the ACI80MARGMCS3 field of the RWNXAGCACI80MARG0 register.
 *
 * The RWNXAGCACI80MARG0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci80margmcs3 - The value to set the field to.
 */
__INLINE void agc_aci80margmcs3_setf(uint8_t aci80margmcs3)
{
    ASSERT_ERR((((uint32_t)aci80margmcs3 << 24) & ~((uint32_t)0x3F000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI80MARG0_ADDR, (REG_PL_RD(AGC_RWNXAGCACI80MARG0_ADDR) & ~((uint32_t)0x3F000000)) | ((uint32_t)aci80margmcs3 << 24));
}

/**
 * @brief Returns the current value of the ACI80MARGMCS2 field in the RWNXAGCACI80MARG0 register.
 *
 * The RWNXAGCACI80MARG0 register will be read and the ACI80MARGMCS2 field's value will be returned.
 *
 * @return The current value of the ACI80MARGMCS2 field in the RWNXAGCACI80MARG0 register.
 */
__INLINE uint8_t agc_aci80margmcs2_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI80MARG0_ADDR);
    return ((localVal & ((uint32_t)0x003F0000)) >> 16);
}

/**
 * @brief Sets the ACI80MARGMCS2 field of the RWNXAGCACI80MARG0 register.
 *
 * The RWNXAGCACI80MARG0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci80margmcs2 - The value to set the field to.
 */
__INLINE void agc_aci80margmcs2_setf(uint8_t aci80margmcs2)
{
    ASSERT_ERR((((uint32_t)aci80margmcs2 << 16) & ~((uint32_t)0x003F0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI80MARG0_ADDR, (REG_PL_RD(AGC_RWNXAGCACI80MARG0_ADDR) & ~((uint32_t)0x003F0000)) | ((uint32_t)aci80margmcs2 << 16));
}

/**
 * @brief Returns the current value of the ACI80MARGMCS1 field in the RWNXAGCACI80MARG0 register.
 *
 * The RWNXAGCACI80MARG0 register will be read and the ACI80MARGMCS1 field's value will be returned.
 *
 * @return The current value of the ACI80MARGMCS1 field in the RWNXAGCACI80MARG0 register.
 */
__INLINE uint8_t agc_aci80margmcs1_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI80MARG0_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the ACI80MARGMCS1 field of the RWNXAGCACI80MARG0 register.
 *
 * The RWNXAGCACI80MARG0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci80margmcs1 - The value to set the field to.
 */
__INLINE void agc_aci80margmcs1_setf(uint8_t aci80margmcs1)
{
    ASSERT_ERR((((uint32_t)aci80margmcs1 << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI80MARG0_ADDR, (REG_PL_RD(AGC_RWNXAGCACI80MARG0_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)aci80margmcs1 << 8));
}

/**
 * @brief Returns the current value of the ACI80MARGMCS0 field in the RWNXAGCACI80MARG0 register.
 *
 * The RWNXAGCACI80MARG0 register will be read and the ACI80MARGMCS0 field's value will be returned.
 *
 * @return The current value of the ACI80MARGMCS0 field in the RWNXAGCACI80MARG0 register.
 */
__INLINE uint8_t agc_aci80margmcs0_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI80MARG0_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the ACI80MARGMCS0 field of the RWNXAGCACI80MARG0 register.
 *
 * The RWNXAGCACI80MARG0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci80margmcs0 - The value to set the field to.
 */
__INLINE void agc_aci80margmcs0_setf(uint8_t aci80margmcs0)
{
    ASSERT_ERR((((uint32_t)aci80margmcs0 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI80MARG0_ADDR, (REG_PL_RD(AGC_RWNXAGCACI80MARG0_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)aci80margmcs0 << 0));
}

/// @}

/**
 * @name RWNXAGCACI80MARG1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:24        ACI80MARGMCS7   0xE
 *  21:16        ACI80MARGMCS6   0xF
 *  13:08        ACI80MARGMCS5   0x10
 *  05:00        ACI80MARGMCS4   0x14
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCACI80MARG1 register
#define AGC_RWNXAGCACI80MARG1_ADDR   0x01002028
/// Offset of the RWNXAGCACI80MARG1 register from the base address
#define AGC_RWNXAGCACI80MARG1_OFFSET 0x00002028
/// Index of the RWNXAGCACI80MARG1 register
#define AGC_RWNXAGCACI80MARG1_INDEX  0x0000080A
/// Reset value of the RWNXAGCACI80MARG1 register
#define AGC_RWNXAGCACI80MARG1_RESET  0x0E0F1014

/**
 * @brief Returns the current value of the RWNXAGCACI80MARG1 register.
 * The RWNXAGCACI80MARG1 register will be read and its value returned.
 * @return The current value of the RWNXAGCACI80MARG1 register.
 */
__INLINE uint32_t agc_rwnxagcaci80marg1_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCACI80MARG1_ADDR);
}

/**
 * @brief Sets the RWNXAGCACI80MARG1 register to a value.
 * The RWNXAGCACI80MARG1 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcaci80marg1_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCACI80MARG1_ADDR, value);
}

// field definitions
/// ACI80MARGMCS7 field mask
#define AGC_ACI80MARGMCS7_MASK   ((uint32_t)0x3F000000)
/// ACI80MARGMCS7 field LSB position
#define AGC_ACI80MARGMCS7_LSB    24
/// ACI80MARGMCS7 field width
#define AGC_ACI80MARGMCS7_WIDTH  ((uint32_t)0x00000006)
/// ACI80MARGMCS6 field mask
#define AGC_ACI80MARGMCS6_MASK   ((uint32_t)0x003F0000)
/// ACI80MARGMCS6 field LSB position
#define AGC_ACI80MARGMCS6_LSB    16
/// ACI80MARGMCS6 field width
#define AGC_ACI80MARGMCS6_WIDTH  ((uint32_t)0x00000006)
/// ACI80MARGMCS5 field mask
#define AGC_ACI80MARGMCS5_MASK   ((uint32_t)0x00003F00)
/// ACI80MARGMCS5 field LSB position
#define AGC_ACI80MARGMCS5_LSB    8
/// ACI80MARGMCS5 field width
#define AGC_ACI80MARGMCS5_WIDTH  ((uint32_t)0x00000006)
/// ACI80MARGMCS4 field mask
#define AGC_ACI80MARGMCS4_MASK   ((uint32_t)0x0000003F)
/// ACI80MARGMCS4 field LSB position
#define AGC_ACI80MARGMCS4_LSB    0
/// ACI80MARGMCS4 field width
#define AGC_ACI80MARGMCS4_WIDTH  ((uint32_t)0x00000006)

/// ACI80MARGMCS7 field reset value
#define AGC_ACI80MARGMCS7_RST    0xE
/// ACI80MARGMCS6 field reset value
#define AGC_ACI80MARGMCS6_RST    0xF
/// ACI80MARGMCS5 field reset value
#define AGC_ACI80MARGMCS5_RST    0x10
/// ACI80MARGMCS4 field reset value
#define AGC_ACI80MARGMCS4_RST    0x14

/**
 * @brief Constructs a value for the RWNXAGCACI80MARG1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] aci80margmcs7 - The value to use for the ACI80MARGMCS7 field.
 * @param[in] aci80margmcs6 - The value to use for the ACI80MARGMCS6 field.
 * @param[in] aci80margmcs5 - The value to use for the ACI80MARGMCS5 field.
 * @param[in] aci80margmcs4 - The value to use for the ACI80MARGMCS4 field.
 */
__INLINE void agc_rwnxagcaci80marg1_pack(uint8_t aci80margmcs7, uint8_t aci80margmcs6, uint8_t aci80margmcs5, uint8_t aci80margmcs4)
{
    ASSERT_ERR((((uint32_t)aci80margmcs7 << 24) & ~((uint32_t)0x3F000000)) == 0);
    ASSERT_ERR((((uint32_t)aci80margmcs6 << 16) & ~((uint32_t)0x003F0000)) == 0);
    ASSERT_ERR((((uint32_t)aci80margmcs5 << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)aci80margmcs4 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI80MARG1_ADDR,  ((uint32_t)aci80margmcs7 << 24) | ((uint32_t)aci80margmcs6 << 16) | ((uint32_t)aci80margmcs5 << 8) | ((uint32_t)aci80margmcs4 << 0));
}

/**
 * @brief Unpacks RWNXAGCACI80MARG1's fields from current value of the RWNXAGCACI80MARG1 register.
 *
 * Reads the RWNXAGCACI80MARG1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] aci80margmcs7 - Will be populated with the current value of this field from the register.
 * @param[out] aci80margmcs6 - Will be populated with the current value of this field from the register.
 * @param[out] aci80margmcs5 - Will be populated with the current value of this field from the register.
 * @param[out] aci80margmcs4 - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcaci80marg1_unpack(uint8_t *aci80margmcs7, uint8_t *aci80margmcs6, uint8_t *aci80margmcs5, uint8_t *aci80margmcs4)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI80MARG1_ADDR);

    *aci80margmcs7 = (localVal & ((uint32_t)0x3F000000)) >> 24;
    *aci80margmcs6 = (localVal & ((uint32_t)0x003F0000)) >> 16;
    *aci80margmcs5 = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *aci80margmcs4 = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the ACI80MARGMCS7 field in the RWNXAGCACI80MARG1 register.
 *
 * The RWNXAGCACI80MARG1 register will be read and the ACI80MARGMCS7 field's value will be returned.
 *
 * @return The current value of the ACI80MARGMCS7 field in the RWNXAGCACI80MARG1 register.
 */
__INLINE uint8_t agc_aci80margmcs7_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI80MARG1_ADDR);
    return ((localVal & ((uint32_t)0x3F000000)) >> 24);
}

/**
 * @brief Sets the ACI80MARGMCS7 field of the RWNXAGCACI80MARG1 register.
 *
 * The RWNXAGCACI80MARG1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci80margmcs7 - The value to set the field to.
 */
__INLINE void agc_aci80margmcs7_setf(uint8_t aci80margmcs7)
{
    ASSERT_ERR((((uint32_t)aci80margmcs7 << 24) & ~((uint32_t)0x3F000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI80MARG1_ADDR, (REG_PL_RD(AGC_RWNXAGCACI80MARG1_ADDR) & ~((uint32_t)0x3F000000)) | ((uint32_t)aci80margmcs7 << 24));
}

/**
 * @brief Returns the current value of the ACI80MARGMCS6 field in the RWNXAGCACI80MARG1 register.
 *
 * The RWNXAGCACI80MARG1 register will be read and the ACI80MARGMCS6 field's value will be returned.
 *
 * @return The current value of the ACI80MARGMCS6 field in the RWNXAGCACI80MARG1 register.
 */
__INLINE uint8_t agc_aci80margmcs6_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI80MARG1_ADDR);
    return ((localVal & ((uint32_t)0x003F0000)) >> 16);
}

/**
 * @brief Sets the ACI80MARGMCS6 field of the RWNXAGCACI80MARG1 register.
 *
 * The RWNXAGCACI80MARG1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci80margmcs6 - The value to set the field to.
 */
__INLINE void agc_aci80margmcs6_setf(uint8_t aci80margmcs6)
{
    ASSERT_ERR((((uint32_t)aci80margmcs6 << 16) & ~((uint32_t)0x003F0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI80MARG1_ADDR, (REG_PL_RD(AGC_RWNXAGCACI80MARG1_ADDR) & ~((uint32_t)0x003F0000)) | ((uint32_t)aci80margmcs6 << 16));
}

/**
 * @brief Returns the current value of the ACI80MARGMCS5 field in the RWNXAGCACI80MARG1 register.
 *
 * The RWNXAGCACI80MARG1 register will be read and the ACI80MARGMCS5 field's value will be returned.
 *
 * @return The current value of the ACI80MARGMCS5 field in the RWNXAGCACI80MARG1 register.
 */
__INLINE uint8_t agc_aci80margmcs5_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI80MARG1_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the ACI80MARGMCS5 field of the RWNXAGCACI80MARG1 register.
 *
 * The RWNXAGCACI80MARG1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci80margmcs5 - The value to set the field to.
 */
__INLINE void agc_aci80margmcs5_setf(uint8_t aci80margmcs5)
{
    ASSERT_ERR((((uint32_t)aci80margmcs5 << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI80MARG1_ADDR, (REG_PL_RD(AGC_RWNXAGCACI80MARG1_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)aci80margmcs5 << 8));
}

/**
 * @brief Returns the current value of the ACI80MARGMCS4 field in the RWNXAGCACI80MARG1 register.
 *
 * The RWNXAGCACI80MARG1 register will be read and the ACI80MARGMCS4 field's value will be returned.
 *
 * @return The current value of the ACI80MARGMCS4 field in the RWNXAGCACI80MARG1 register.
 */
__INLINE uint8_t agc_aci80margmcs4_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI80MARG1_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the ACI80MARGMCS4 field of the RWNXAGCACI80MARG1 register.
 *
 * The RWNXAGCACI80MARG1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci80margmcs4 - The value to set the field to.
 */
__INLINE void agc_aci80margmcs4_setf(uint8_t aci80margmcs4)
{
    ASSERT_ERR((((uint32_t)aci80margmcs4 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI80MARG1_ADDR, (REG_PL_RD(AGC_RWNXAGCACI80MARG1_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)aci80margmcs4 << 0));
}

/// @}

/**
 * @name RWNXAGCACI80MARG2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  13:08        ACI80MARGMCS9   0x8
 *  05:00        ACI80MARGMCS8   0xA
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCACI80MARG2 register
#define AGC_RWNXAGCACI80MARG2_ADDR   0x0100202C
/// Offset of the RWNXAGCACI80MARG2 register from the base address
#define AGC_RWNXAGCACI80MARG2_OFFSET 0x0000202C
/// Index of the RWNXAGCACI80MARG2 register
#define AGC_RWNXAGCACI80MARG2_INDEX  0x0000080B
/// Reset value of the RWNXAGCACI80MARG2 register
#define AGC_RWNXAGCACI80MARG2_RESET  0x0000080A

/**
 * @brief Returns the current value of the RWNXAGCACI80MARG2 register.
 * The RWNXAGCACI80MARG2 register will be read and its value returned.
 * @return The current value of the RWNXAGCACI80MARG2 register.
 */
__INLINE uint32_t agc_rwnxagcaci80marg2_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCACI80MARG2_ADDR);
}

/**
 * @brief Sets the RWNXAGCACI80MARG2 register to a value.
 * The RWNXAGCACI80MARG2 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcaci80marg2_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCACI80MARG2_ADDR, value);
}

// field definitions
/// ACI80MARGMCS9 field mask
#define AGC_ACI80MARGMCS9_MASK   ((uint32_t)0x00003F00)
/// ACI80MARGMCS9 field LSB position
#define AGC_ACI80MARGMCS9_LSB    8
/// ACI80MARGMCS9 field width
#define AGC_ACI80MARGMCS9_WIDTH  ((uint32_t)0x00000006)
/// ACI80MARGMCS8 field mask
#define AGC_ACI80MARGMCS8_MASK   ((uint32_t)0x0000003F)
/// ACI80MARGMCS8 field LSB position
#define AGC_ACI80MARGMCS8_LSB    0
/// ACI80MARGMCS8 field width
#define AGC_ACI80MARGMCS8_WIDTH  ((uint32_t)0x00000006)

/// ACI80MARGMCS9 field reset value
#define AGC_ACI80MARGMCS9_RST    0x8
/// ACI80MARGMCS8 field reset value
#define AGC_ACI80MARGMCS8_RST    0xA

/**
 * @brief Constructs a value for the RWNXAGCACI80MARG2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] aci80margmcs9 - The value to use for the ACI80MARGMCS9 field.
 * @param[in] aci80margmcs8 - The value to use for the ACI80MARGMCS8 field.
 */
__INLINE void agc_rwnxagcaci80marg2_pack(uint8_t aci80margmcs9, uint8_t aci80margmcs8)
{
    ASSERT_ERR((((uint32_t)aci80margmcs9 << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)aci80margmcs8 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI80MARG2_ADDR,  ((uint32_t)aci80margmcs9 << 8) | ((uint32_t)aci80margmcs8 << 0));
}

/**
 * @brief Unpacks RWNXAGCACI80MARG2's fields from current value of the RWNXAGCACI80MARG2 register.
 *
 * Reads the RWNXAGCACI80MARG2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] aci80margmcs9 - Will be populated with the current value of this field from the register.
 * @param[out] aci80margmcs8 - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcaci80marg2_unpack(uint8_t *aci80margmcs9, uint8_t *aci80margmcs8)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI80MARG2_ADDR);

    *aci80margmcs9 = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *aci80margmcs8 = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the ACI80MARGMCS9 field in the RWNXAGCACI80MARG2 register.
 *
 * The RWNXAGCACI80MARG2 register will be read and the ACI80MARGMCS9 field's value will be returned.
 *
 * @return The current value of the ACI80MARGMCS9 field in the RWNXAGCACI80MARG2 register.
 */
__INLINE uint8_t agc_aci80margmcs9_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI80MARG2_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the ACI80MARGMCS9 field of the RWNXAGCACI80MARG2 register.
 *
 * The RWNXAGCACI80MARG2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci80margmcs9 - The value to set the field to.
 */
__INLINE void agc_aci80margmcs9_setf(uint8_t aci80margmcs9)
{
    ASSERT_ERR((((uint32_t)aci80margmcs9 << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI80MARG2_ADDR, (REG_PL_RD(AGC_RWNXAGCACI80MARG2_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)aci80margmcs9 << 8));
}

/**
 * @brief Returns the current value of the ACI80MARGMCS8 field in the RWNXAGCACI80MARG2 register.
 *
 * The RWNXAGCACI80MARG2 register will be read and the ACI80MARGMCS8 field's value will be returned.
 *
 * @return The current value of the ACI80MARGMCS8 field in the RWNXAGCACI80MARG2 register.
 */
__INLINE uint8_t agc_aci80margmcs8_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCACI80MARG2_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the ACI80MARGMCS8 field of the RWNXAGCACI80MARG2 register.
 *
 * The RWNXAGCACI80MARG2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aci80margmcs8 - The value to set the field to.
 */
__INLINE void agc_aci80margmcs8_setf(uint8_t aci80margmcs8)
{
    ASSERT_ERR((((uint32_t)aci80margmcs8 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCACI80MARG2_ADDR, (REG_PL_RD(AGC_RWNXAGCACI80MARG2_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)aci80margmcs8 << 0));
}

/// @}

/**
 * @name RWNXAGCCGH register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  28:24          CHGVGAPSTHR   0x1
 *  20:16          CHGVGAPLTHR   0x5
 *  12:08          CHGVGANSTHR   0x1
 *  04:00          CHGVGANLTHR   0x5
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCCGH register
#define AGC_RWNXAGCCGH_ADDR   0x01002030
/// Offset of the RWNXAGCCGH register from the base address
#define AGC_RWNXAGCCGH_OFFSET 0x00002030
/// Index of the RWNXAGCCGH register
#define AGC_RWNXAGCCGH_INDEX  0x0000080C
/// Reset value of the RWNXAGCCGH register
#define AGC_RWNXAGCCGH_RESET  0x01050105

/**
 * @brief Returns the current value of the RWNXAGCCGH register.
 * The RWNXAGCCGH register will be read and its value returned.
 * @return The current value of the RWNXAGCCGH register.
 */
__INLINE uint32_t agc_rwnxagccgh_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCCGH_ADDR);
}

/**
 * @brief Sets the RWNXAGCCGH register to a value.
 * The RWNXAGCCGH register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagccgh_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCCGH_ADDR, value);
}

// field definitions
/// CHGVGAPSTHR field mask
#define AGC_CHGVGAPSTHR_MASK   ((uint32_t)0x1F000000)
/// CHGVGAPSTHR field LSB position
#define AGC_CHGVGAPSTHR_LSB    24
/// CHGVGAPSTHR field width
#define AGC_CHGVGAPSTHR_WIDTH  ((uint32_t)0x00000005)
/// CHGVGAPLTHR field mask
#define AGC_CHGVGAPLTHR_MASK   ((uint32_t)0x001F0000)
/// CHGVGAPLTHR field LSB position
#define AGC_CHGVGAPLTHR_LSB    16
/// CHGVGAPLTHR field width
#define AGC_CHGVGAPLTHR_WIDTH  ((uint32_t)0x00000005)
/// CHGVGANSTHR field mask
#define AGC_CHGVGANSTHR_MASK   ((uint32_t)0x00001F00)
/// CHGVGANSTHR field LSB position
#define AGC_CHGVGANSTHR_LSB    8
/// CHGVGANSTHR field width
#define AGC_CHGVGANSTHR_WIDTH  ((uint32_t)0x00000005)
/// CHGVGANLTHR field mask
#define AGC_CHGVGANLTHR_MASK   ((uint32_t)0x0000001F)
/// CHGVGANLTHR field LSB position
#define AGC_CHGVGANLTHR_LSB    0
/// CHGVGANLTHR field width
#define AGC_CHGVGANLTHR_WIDTH  ((uint32_t)0x00000005)

/// CHGVGAPSTHR field reset value
#define AGC_CHGVGAPSTHR_RST    0x1
/// CHGVGAPLTHR field reset value
#define AGC_CHGVGAPLTHR_RST    0x5
/// CHGVGANSTHR field reset value
#define AGC_CHGVGANSTHR_RST    0x1
/// CHGVGANLTHR field reset value
#define AGC_CHGVGANLTHR_RST    0x5

/**
 * @brief Constructs a value for the RWNXAGCCGH register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] chgvgapsthr - The value to use for the CHGVGAPSTHR field.
 * @param[in] chgvgaplthr - The value to use for the CHGVGAPLTHR field.
 * @param[in] chgvgansthr - The value to use for the CHGVGANSTHR field.
 * @param[in] chgvganlthr - The value to use for the CHGVGANLTHR field.
 */
__INLINE void agc_rwnxagccgh_pack(uint8_t chgvgapsthr, uint8_t chgvgaplthr, uint8_t chgvgansthr, uint8_t chgvganlthr)
{
    ASSERT_ERR((((uint32_t)chgvgapsthr << 24) & ~((uint32_t)0x1F000000)) == 0);
    ASSERT_ERR((((uint32_t)chgvgaplthr << 16) & ~((uint32_t)0x001F0000)) == 0);
    ASSERT_ERR((((uint32_t)chgvgansthr << 8) & ~((uint32_t)0x00001F00)) == 0);
    ASSERT_ERR((((uint32_t)chgvganlthr << 0) & ~((uint32_t)0x0000001F)) == 0);
    REG_PL_WR(AGC_RWNXAGCCGH_ADDR,  ((uint32_t)chgvgapsthr << 24) | ((uint32_t)chgvgaplthr << 16) | ((uint32_t)chgvgansthr << 8) | ((uint32_t)chgvganlthr << 0));
}

/**
 * @brief Unpacks RWNXAGCCGH's fields from current value of the RWNXAGCCGH register.
 *
 * Reads the RWNXAGCCGH register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] chgvgapsthr - Will be populated with the current value of this field from the register.
 * @param[out] chgvgaplthr - Will be populated with the current value of this field from the register.
 * @param[out] chgvgansthr - Will be populated with the current value of this field from the register.
 * @param[out] chgvganlthr - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagccgh_unpack(uint8_t *chgvgapsthr, uint8_t *chgvgaplthr, uint8_t *chgvgansthr, uint8_t *chgvganlthr)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCGH_ADDR);

    *chgvgapsthr = (localVal & ((uint32_t)0x1F000000)) >> 24;
    *chgvgaplthr = (localVal & ((uint32_t)0x001F0000)) >> 16;
    *chgvgansthr = (localVal & ((uint32_t)0x00001F00)) >> 8;
    *chgvganlthr = (localVal & ((uint32_t)0x0000001F)) >> 0;
}

/**
 * @brief Returns the current value of the CHGVGAPSTHR field in the RWNXAGCCGH register.
 *
 * The RWNXAGCCGH register will be read and the CHGVGAPSTHR field's value will be returned.
 *
 * @return The current value of the CHGVGAPSTHR field in the RWNXAGCCGH register.
 */
__INLINE uint8_t agc_chgvgapsthr_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCGH_ADDR);
    return ((localVal & ((uint32_t)0x1F000000)) >> 24);
}

/**
 * @brief Sets the CHGVGAPSTHR field of the RWNXAGCCGH register.
 *
 * The RWNXAGCCGH register will be read, modified to contain the new field value, and written.
 *
 * @param[in] chgvgapsthr - The value to set the field to.
 */
__INLINE void agc_chgvgapsthr_setf(uint8_t chgvgapsthr)
{
    ASSERT_ERR((((uint32_t)chgvgapsthr << 24) & ~((uint32_t)0x1F000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCCGH_ADDR, (REG_PL_RD(AGC_RWNXAGCCGH_ADDR) & ~((uint32_t)0x1F000000)) | ((uint32_t)chgvgapsthr << 24));
}

/**
 * @brief Returns the current value of the CHGVGAPLTHR field in the RWNXAGCCGH register.
 *
 * The RWNXAGCCGH register will be read and the CHGVGAPLTHR field's value will be returned.
 *
 * @return The current value of the CHGVGAPLTHR field in the RWNXAGCCGH register.
 */
__INLINE uint8_t agc_chgvgaplthr_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCGH_ADDR);
    return ((localVal & ((uint32_t)0x001F0000)) >> 16);
}

/**
 * @brief Sets the CHGVGAPLTHR field of the RWNXAGCCGH register.
 *
 * The RWNXAGCCGH register will be read, modified to contain the new field value, and written.
 *
 * @param[in] chgvgaplthr - The value to set the field to.
 */
__INLINE void agc_chgvgaplthr_setf(uint8_t chgvgaplthr)
{
    ASSERT_ERR((((uint32_t)chgvgaplthr << 16) & ~((uint32_t)0x001F0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCCGH_ADDR, (REG_PL_RD(AGC_RWNXAGCCGH_ADDR) & ~((uint32_t)0x001F0000)) | ((uint32_t)chgvgaplthr << 16));
}

/**
 * @brief Returns the current value of the CHGVGANSTHR field in the RWNXAGCCGH register.
 *
 * The RWNXAGCCGH register will be read and the CHGVGANSTHR field's value will be returned.
 *
 * @return The current value of the CHGVGANSTHR field in the RWNXAGCCGH register.
 */
__INLINE uint8_t agc_chgvgansthr_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCGH_ADDR);
    return ((localVal & ((uint32_t)0x00001F00)) >> 8);
}

/**
 * @brief Sets the CHGVGANSTHR field of the RWNXAGCCGH register.
 *
 * The RWNXAGCCGH register will be read, modified to contain the new field value, and written.
 *
 * @param[in] chgvgansthr - The value to set the field to.
 */
__INLINE void agc_chgvgansthr_setf(uint8_t chgvgansthr)
{
    ASSERT_ERR((((uint32_t)chgvgansthr << 8) & ~((uint32_t)0x00001F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCCGH_ADDR, (REG_PL_RD(AGC_RWNXAGCCGH_ADDR) & ~((uint32_t)0x00001F00)) | ((uint32_t)chgvgansthr << 8));
}

/**
 * @brief Returns the current value of the CHGVGANLTHR field in the RWNXAGCCGH register.
 *
 * The RWNXAGCCGH register will be read and the CHGVGANLTHR field's value will be returned.
 *
 * @return The current value of the CHGVGANLTHR field in the RWNXAGCCGH register.
 */
__INLINE uint8_t agc_chgvganlthr_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCGH_ADDR);
    return ((localVal & ((uint32_t)0x0000001F)) >> 0);
}

/**
 * @brief Sets the CHGVGANLTHR field of the RWNXAGCCGH register.
 *
 * The RWNXAGCCGH register will be read, modified to contain the new field value, and written.
 *
 * @param[in] chgvganlthr - The value to set the field to.
 */
__INLINE void agc_chgvganlthr_setf(uint8_t chgvganlthr)
{
    ASSERT_ERR((((uint32_t)chgvganlthr << 0) & ~((uint32_t)0x0000001F)) == 0);
    REG_PL_WR(AGC_RWNXAGCCGH_ADDR, (REG_PL_RD(AGC_RWNXAGCCGH_ADDR) & ~((uint32_t)0x0000001F)) | ((uint32_t)chgvganlthr << 0));
}

/// @}

/**
 * @name RWNXAGCSAT register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  28:24         SATDELAY50NS   0x8
 *  21:16        SATHIGHTHRDBV   0x39
 *  13:08         SATLOWTHRDBV   0x35
 *  05:00            SATTHRDBV   0x36
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCSAT register
#define AGC_RWNXAGCSAT_ADDR   0x01002034
/// Offset of the RWNXAGCSAT register from the base address
#define AGC_RWNXAGCSAT_OFFSET 0x00002034
/// Index of the RWNXAGCSAT register
#define AGC_RWNXAGCSAT_INDEX  0x0000080D
/// Reset value of the RWNXAGCSAT register
#define AGC_RWNXAGCSAT_RESET  0x08393536

/**
 * @brief Returns the current value of the RWNXAGCSAT register.
 * The RWNXAGCSAT register will be read and its value returned.
 * @return The current value of the RWNXAGCSAT register.
 */
__INLINE uint32_t agc_rwnxagcsat_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCSAT_ADDR);
}

/**
 * @brief Sets the RWNXAGCSAT register to a value.
 * The RWNXAGCSAT register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcsat_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCSAT_ADDR, value);
}

// field definitions
/// SATDELAY50NS field mask
#define AGC_SATDELAY50NS_MASK    ((uint32_t)0x1F000000)
/// SATDELAY50NS field LSB position
#define AGC_SATDELAY50NS_LSB     24
/// SATDELAY50NS field width
#define AGC_SATDELAY50NS_WIDTH   ((uint32_t)0x00000005)
/// SATHIGHTHRDBV field mask
#define AGC_SATHIGHTHRDBV_MASK   ((uint32_t)0x003F0000)
/// SATHIGHTHRDBV field LSB position
#define AGC_SATHIGHTHRDBV_LSB    16
/// SATHIGHTHRDBV field width
#define AGC_SATHIGHTHRDBV_WIDTH  ((uint32_t)0x00000006)
/// SATLOWTHRDBV field mask
#define AGC_SATLOWTHRDBV_MASK    ((uint32_t)0x00003F00)
/// SATLOWTHRDBV field LSB position
#define AGC_SATLOWTHRDBV_LSB     8
/// SATLOWTHRDBV field width
#define AGC_SATLOWTHRDBV_WIDTH   ((uint32_t)0x00000006)
/// SATTHRDBV field mask
#define AGC_SATTHRDBV_MASK       ((uint32_t)0x0000003F)
/// SATTHRDBV field LSB position
#define AGC_SATTHRDBV_LSB        0
/// SATTHRDBV field width
#define AGC_SATTHRDBV_WIDTH      ((uint32_t)0x00000006)

/// SATDELAY50NS field reset value
#define AGC_SATDELAY50NS_RST     0x8
/// SATHIGHTHRDBV field reset value
#define AGC_SATHIGHTHRDBV_RST    0x39
/// SATLOWTHRDBV field reset value
#define AGC_SATLOWTHRDBV_RST     0x35
/// SATTHRDBV field reset value
#define AGC_SATTHRDBV_RST        0x36

/**
 * @brief Constructs a value for the RWNXAGCSAT register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] satdelay50ns - The value to use for the SATDELAY50NS field.
 * @param[in] sathighthrdbv - The value to use for the SATHIGHTHRDBV field.
 * @param[in] satlowthrdbv - The value to use for the SATLOWTHRDBV field.
 * @param[in] satthrdbv - The value to use for the SATTHRDBV field.
 */
__INLINE void agc_rwnxagcsat_pack(uint8_t satdelay50ns, uint8_t sathighthrdbv, uint8_t satlowthrdbv, uint8_t satthrdbv)
{
    ASSERT_ERR((((uint32_t)satdelay50ns << 24) & ~((uint32_t)0x1F000000)) == 0);
    ASSERT_ERR((((uint32_t)sathighthrdbv << 16) & ~((uint32_t)0x003F0000)) == 0);
    ASSERT_ERR((((uint32_t)satlowthrdbv << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)satthrdbv << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCSAT_ADDR,  ((uint32_t)satdelay50ns << 24) | ((uint32_t)sathighthrdbv << 16) | ((uint32_t)satlowthrdbv << 8) | ((uint32_t)satthrdbv << 0));
}

/**
 * @brief Unpacks RWNXAGCSAT's fields from current value of the RWNXAGCSAT register.
 *
 * Reads the RWNXAGCSAT register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] satdelay50ns - Will be populated with the current value of this field from the register.
 * @param[out] sathighthrdbv - Will be populated with the current value of this field from the register.
 * @param[out] satlowthrdbv - Will be populated with the current value of this field from the register.
 * @param[out] satthrdbv - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcsat_unpack(uint8_t *satdelay50ns, uint8_t *sathighthrdbv, uint8_t *satlowthrdbv, uint8_t *satthrdbv)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSAT_ADDR);

    *satdelay50ns = (localVal & ((uint32_t)0x1F000000)) >> 24;
    *sathighthrdbv = (localVal & ((uint32_t)0x003F0000)) >> 16;
    *satlowthrdbv = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *satthrdbv = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the SATDELAY50NS field in the RWNXAGCSAT register.
 *
 * The RWNXAGCSAT register will be read and the SATDELAY50NS field's value will be returned.
 *
 * @return The current value of the SATDELAY50NS field in the RWNXAGCSAT register.
 */
__INLINE uint8_t agc_satdelay50ns_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSAT_ADDR);
    return ((localVal & ((uint32_t)0x1F000000)) >> 24);
}

/**
 * @brief Sets the SATDELAY50NS field of the RWNXAGCSAT register.
 *
 * The RWNXAGCSAT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] satdelay50ns - The value to set the field to.
 */
__INLINE void agc_satdelay50ns_setf(uint8_t satdelay50ns)
{
    ASSERT_ERR((((uint32_t)satdelay50ns << 24) & ~((uint32_t)0x1F000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCSAT_ADDR, (REG_PL_RD(AGC_RWNXAGCSAT_ADDR) & ~((uint32_t)0x1F000000)) | ((uint32_t)satdelay50ns << 24));
}

/**
 * @brief Returns the current value of the SATHIGHTHRDBV field in the RWNXAGCSAT register.
 *
 * The RWNXAGCSAT register will be read and the SATHIGHTHRDBV field's value will be returned.
 *
 * @return The current value of the SATHIGHTHRDBV field in the RWNXAGCSAT register.
 */
__INLINE uint8_t agc_sathighthrdbv_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSAT_ADDR);
    return ((localVal & ((uint32_t)0x003F0000)) >> 16);
}

/**
 * @brief Sets the SATHIGHTHRDBV field of the RWNXAGCSAT register.
 *
 * The RWNXAGCSAT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] sathighthrdbv - The value to set the field to.
 */
__INLINE void agc_sathighthrdbv_setf(uint8_t sathighthrdbv)
{
    ASSERT_ERR((((uint32_t)sathighthrdbv << 16) & ~((uint32_t)0x003F0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCSAT_ADDR, (REG_PL_RD(AGC_RWNXAGCSAT_ADDR) & ~((uint32_t)0x003F0000)) | ((uint32_t)sathighthrdbv << 16));
}

/**
 * @brief Returns the current value of the SATLOWTHRDBV field in the RWNXAGCSAT register.
 *
 * The RWNXAGCSAT register will be read and the SATLOWTHRDBV field's value will be returned.
 *
 * @return The current value of the SATLOWTHRDBV field in the RWNXAGCSAT register.
 */
__INLINE uint8_t agc_satlowthrdbv_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSAT_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the SATLOWTHRDBV field of the RWNXAGCSAT register.
 *
 * The RWNXAGCSAT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] satlowthrdbv - The value to set the field to.
 */
__INLINE void agc_satlowthrdbv_setf(uint8_t satlowthrdbv)
{
    ASSERT_ERR((((uint32_t)satlowthrdbv << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCSAT_ADDR, (REG_PL_RD(AGC_RWNXAGCSAT_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)satlowthrdbv << 8));
}

/**
 * @brief Returns the current value of the SATTHRDBV field in the RWNXAGCSAT register.
 *
 * The RWNXAGCSAT register will be read and the SATTHRDBV field's value will be returned.
 *
 * @return The current value of the SATTHRDBV field in the RWNXAGCSAT register.
 */
__INLINE uint8_t agc_satthrdbv_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSAT_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the SATTHRDBV field of the RWNXAGCSAT register.
 *
 * The RWNXAGCSAT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] satthrdbv - The value to set the field to.
 */
__INLINE void agc_satthrdbv_setf(uint8_t satthrdbv)
{
    ASSERT_ERR((((uint32_t)satthrdbv << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCSAT_ADDR, (REG_PL_RD(AGC_RWNXAGCSAT_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)satthrdbv << 0));
}

/// @}

/**
 * @name RWNXAGCCROSS register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  21:12       CROSSDNTHRQDBM   0x280
 *  09:00       CROSSUPTHRQDBM   0x288
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCCROSS register
#define AGC_RWNXAGCCROSS_ADDR   0x01002038
/// Offset of the RWNXAGCCROSS register from the base address
#define AGC_RWNXAGCCROSS_OFFSET 0x00002038
/// Index of the RWNXAGCCROSS register
#define AGC_RWNXAGCCROSS_INDEX  0x0000080E
/// Reset value of the RWNXAGCCROSS register
#define AGC_RWNXAGCCROSS_RESET  0x00280288

/**
 * @brief Returns the current value of the RWNXAGCCROSS register.
 * The RWNXAGCCROSS register will be read and its value returned.
 * @return The current value of the RWNXAGCCROSS register.
 */
__INLINE uint32_t agc_rwnxagccross_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCCROSS_ADDR);
}

/**
 * @brief Sets the RWNXAGCCROSS register to a value.
 * The RWNXAGCCROSS register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagccross_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCCROSS_ADDR, value);
}

// field definitions
/// CROSSDNTHRQDBM field mask
#define AGC_CROSSDNTHRQDBM_MASK   ((uint32_t)0x003FF000)
/// CROSSDNTHRQDBM field LSB position
#define AGC_CROSSDNTHRQDBM_LSB    12
/// CROSSDNTHRQDBM field width
#define AGC_CROSSDNTHRQDBM_WIDTH  ((uint32_t)0x0000000A)
/// CROSSUPTHRQDBM field mask
#define AGC_CROSSUPTHRQDBM_MASK   ((uint32_t)0x000003FF)
/// CROSSUPTHRQDBM field LSB position
#define AGC_CROSSUPTHRQDBM_LSB    0
/// CROSSUPTHRQDBM field width
#define AGC_CROSSUPTHRQDBM_WIDTH  ((uint32_t)0x0000000A)

/// CROSSDNTHRQDBM field reset value
#define AGC_CROSSDNTHRQDBM_RST    0x280
/// CROSSUPTHRQDBM field reset value
#define AGC_CROSSUPTHRQDBM_RST    0x288

/**
 * @brief Constructs a value for the RWNXAGCCROSS register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] crossdnthrqdbm - The value to use for the CROSSDNTHRQDBM field.
 * @param[in] crossupthrqdbm - The value to use for the CROSSUPTHRQDBM field.
 */
__INLINE void agc_rwnxagccross_pack(uint16_t crossdnthrqdbm, uint16_t crossupthrqdbm)
{
    ASSERT_ERR((((uint32_t)crossdnthrqdbm << 12) & ~((uint32_t)0x003FF000)) == 0);
    ASSERT_ERR((((uint32_t)crossupthrqdbm << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCCROSS_ADDR,  ((uint32_t)crossdnthrqdbm << 12) | ((uint32_t)crossupthrqdbm << 0));
}

/**
 * @brief Unpacks RWNXAGCCROSS's fields from current value of the RWNXAGCCROSS register.
 *
 * Reads the RWNXAGCCROSS register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] crossdnthrqdbm - Will be populated with the current value of this field from the register.
 * @param[out] crossupthrqdbm - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagccross_unpack(uint16_t *crossdnthrqdbm, uint16_t *crossupthrqdbm)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCROSS_ADDR);

    *crossdnthrqdbm = (localVal & ((uint32_t)0x003FF000)) >> 12;
    *crossupthrqdbm = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the CROSSDNTHRQDBM field in the RWNXAGCCROSS register.
 *
 * The RWNXAGCCROSS register will be read and the CROSSDNTHRQDBM field's value will be returned.
 *
 * @return The current value of the CROSSDNTHRQDBM field in the RWNXAGCCROSS register.
 */
__INLINE uint16_t agc_crossdnthrqdbm_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCROSS_ADDR);
    return ((localVal & ((uint32_t)0x003FF000)) >> 12);
}

/**
 * @brief Sets the CROSSDNTHRQDBM field of the RWNXAGCCROSS register.
 *
 * The RWNXAGCCROSS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] crossdnthrqdbm - The value to set the field to.
 */
__INLINE void agc_crossdnthrqdbm_setf(uint16_t crossdnthrqdbm)
{
    ASSERT_ERR((((uint32_t)crossdnthrqdbm << 12) & ~((uint32_t)0x003FF000)) == 0);
    REG_PL_WR(AGC_RWNXAGCCROSS_ADDR, (REG_PL_RD(AGC_RWNXAGCCROSS_ADDR) & ~((uint32_t)0x003FF000)) | ((uint32_t)crossdnthrqdbm << 12));
}

/**
 * @brief Returns the current value of the CROSSUPTHRQDBM field in the RWNXAGCCROSS register.
 *
 * The RWNXAGCCROSS register will be read and the CROSSUPTHRQDBM field's value will be returned.
 *
 * @return The current value of the CROSSUPTHRQDBM field in the RWNXAGCCROSS register.
 */
__INLINE uint16_t agc_crossupthrqdbm_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCROSS_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the CROSSUPTHRQDBM field of the RWNXAGCCROSS register.
 *
 * The RWNXAGCCROSS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] crossupthrqdbm - The value to set the field to.
 */
__INLINE void agc_crossupthrqdbm_setf(uint16_t crossupthrqdbm)
{
    ASSERT_ERR((((uint32_t)crossupthrqdbm << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCCROSS_ADDR, (REG_PL_RD(AGC_RWNXAGCCROSS_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)crossupthrqdbm << 0));
}

/// @}

/**
 * @name RWNXAGCRAMP register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  26:24       RAMPDNNDLINDEX   0x7
 *  23:16         RAMPDNGAPQDB   0x20
 *  10:08       RAMPUPNDLINDEX   0x7
 *  07:00         RAMPUPGAPQDB   0x10
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCRAMP register
#define AGC_RWNXAGCRAMP_ADDR   0x0100203C
/// Offset of the RWNXAGCRAMP register from the base address
#define AGC_RWNXAGCRAMP_OFFSET 0x0000203C
/// Index of the RWNXAGCRAMP register
#define AGC_RWNXAGCRAMP_INDEX  0x0000080F
/// Reset value of the RWNXAGCRAMP register
#define AGC_RWNXAGCRAMP_RESET  0x07200710

/**
 * @brief Returns the current value of the RWNXAGCRAMP register.
 * The RWNXAGCRAMP register will be read and its value returned.
 * @return The current value of the RWNXAGCRAMP register.
 */
__INLINE uint32_t agc_rwnxagcramp_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCRAMP_ADDR);
}

/**
 * @brief Sets the RWNXAGCRAMP register to a value.
 * The RWNXAGCRAMP register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcramp_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCRAMP_ADDR, value);
}

// field definitions
/// RAMPDNNDLINDEX field mask
#define AGC_RAMPDNNDLINDEX_MASK   ((uint32_t)0x07000000)
/// RAMPDNNDLINDEX field LSB position
#define AGC_RAMPDNNDLINDEX_LSB    24
/// RAMPDNNDLINDEX field width
#define AGC_RAMPDNNDLINDEX_WIDTH  ((uint32_t)0x00000003)
/// RAMPDNGAPQDB field mask
#define AGC_RAMPDNGAPQDB_MASK     ((uint32_t)0x00FF0000)
/// RAMPDNGAPQDB field LSB position
#define AGC_RAMPDNGAPQDB_LSB      16
/// RAMPDNGAPQDB field width
#define AGC_RAMPDNGAPQDB_WIDTH    ((uint32_t)0x00000008)
/// RAMPUPNDLINDEX field mask
#define AGC_RAMPUPNDLINDEX_MASK   ((uint32_t)0x00000700)
/// RAMPUPNDLINDEX field LSB position
#define AGC_RAMPUPNDLINDEX_LSB    8
/// RAMPUPNDLINDEX field width
#define AGC_RAMPUPNDLINDEX_WIDTH  ((uint32_t)0x00000003)
/// RAMPUPGAPQDB field mask
#define AGC_RAMPUPGAPQDB_MASK     ((uint32_t)0x000000FF)
/// RAMPUPGAPQDB field LSB position
#define AGC_RAMPUPGAPQDB_LSB      0
/// RAMPUPGAPQDB field width
#define AGC_RAMPUPGAPQDB_WIDTH    ((uint32_t)0x00000008)

/// RAMPDNNDLINDEX field reset value
#define AGC_RAMPDNNDLINDEX_RST    0x7
/// RAMPDNGAPQDB field reset value
#define AGC_RAMPDNGAPQDB_RST      0x20
/// RAMPUPNDLINDEX field reset value
#define AGC_RAMPUPNDLINDEX_RST    0x7
/// RAMPUPGAPQDB field reset value
#define AGC_RAMPUPGAPQDB_RST      0x10

/**
 * @brief Constructs a value for the RWNXAGCRAMP register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rampdnndlindex - The value to use for the RAMPDNNDLINDEX field.
 * @param[in] rampdngapqdb - The value to use for the RAMPDNGAPQDB field.
 * @param[in] rampupndlindex - The value to use for the RAMPUPNDLINDEX field.
 * @param[in] rampupgapqdb - The value to use for the RAMPUPGAPQDB field.
 */
__INLINE void agc_rwnxagcramp_pack(uint8_t rampdnndlindex, uint8_t rampdngapqdb, uint8_t rampupndlindex, uint8_t rampupgapqdb)
{
    ASSERT_ERR((((uint32_t)rampdnndlindex << 24) & ~((uint32_t)0x07000000)) == 0);
    ASSERT_ERR((((uint32_t)rampdngapqdb << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)rampupndlindex << 8) & ~((uint32_t)0x00000700)) == 0);
    ASSERT_ERR((((uint32_t)rampupgapqdb << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCRAMP_ADDR,  ((uint32_t)rampdnndlindex << 24) | ((uint32_t)rampdngapqdb << 16) | ((uint32_t)rampupndlindex << 8) | ((uint32_t)rampupgapqdb << 0));
}

/**
 * @brief Unpacks RWNXAGCRAMP's fields from current value of the RWNXAGCRAMP register.
 *
 * Reads the RWNXAGCRAMP register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rampdnndlindex - Will be populated with the current value of this field from the register.
 * @param[out] rampdngapqdb - Will be populated with the current value of this field from the register.
 * @param[out] rampupndlindex - Will be populated with the current value of this field from the register.
 * @param[out] rampupgapqdb - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcramp_unpack(uint8_t *rampdnndlindex, uint8_t *rampdngapqdb, uint8_t *rampupndlindex, uint8_t *rampupgapqdb)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCRAMP_ADDR);

    *rampdnndlindex = (localVal & ((uint32_t)0x07000000)) >> 24;
    *rampdngapqdb = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *rampupndlindex = (localVal & ((uint32_t)0x00000700)) >> 8;
    *rampupgapqdb = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the RAMPDNNDLINDEX field in the RWNXAGCRAMP register.
 *
 * The RWNXAGCRAMP register will be read and the RAMPDNNDLINDEX field's value will be returned.
 *
 * @return The current value of the RAMPDNNDLINDEX field in the RWNXAGCRAMP register.
 */
__INLINE uint8_t agc_rampdnndlindex_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCRAMP_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

/**
 * @brief Sets the RAMPDNNDLINDEX field of the RWNXAGCRAMP register.
 *
 * The RWNXAGCRAMP register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rampdnndlindex - The value to set the field to.
 */
__INLINE void agc_rampdnndlindex_setf(uint8_t rampdnndlindex)
{
    ASSERT_ERR((((uint32_t)rampdnndlindex << 24) & ~((uint32_t)0x07000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCRAMP_ADDR, (REG_PL_RD(AGC_RWNXAGCRAMP_ADDR) & ~((uint32_t)0x07000000)) | ((uint32_t)rampdnndlindex << 24));
}

/**
 * @brief Returns the current value of the RAMPDNGAPQDB field in the RWNXAGCRAMP register.
 *
 * The RWNXAGCRAMP register will be read and the RAMPDNGAPQDB field's value will be returned.
 *
 * @return The current value of the RAMPDNGAPQDB field in the RWNXAGCRAMP register.
 */
__INLINE uint8_t agc_rampdngapqdb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCRAMP_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the RAMPDNGAPQDB field of the RWNXAGCRAMP register.
 *
 * The RWNXAGCRAMP register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rampdngapqdb - The value to set the field to.
 */
__INLINE void agc_rampdngapqdb_setf(uint8_t rampdngapqdb)
{
    ASSERT_ERR((((uint32_t)rampdngapqdb << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCRAMP_ADDR, (REG_PL_RD(AGC_RWNXAGCRAMP_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)rampdngapqdb << 16));
}

/**
 * @brief Returns the current value of the RAMPUPNDLINDEX field in the RWNXAGCRAMP register.
 *
 * The RWNXAGCRAMP register will be read and the RAMPUPNDLINDEX field's value will be returned.
 *
 * @return The current value of the RAMPUPNDLINDEX field in the RWNXAGCRAMP register.
 */
__INLINE uint8_t agc_rampupndlindex_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCRAMP_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

/**
 * @brief Sets the RAMPUPNDLINDEX field of the RWNXAGCRAMP register.
 *
 * The RWNXAGCRAMP register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rampupndlindex - The value to set the field to.
 */
__INLINE void agc_rampupndlindex_setf(uint8_t rampupndlindex)
{
    ASSERT_ERR((((uint32_t)rampupndlindex << 8) & ~((uint32_t)0x00000700)) == 0);
    REG_PL_WR(AGC_RWNXAGCRAMP_ADDR, (REG_PL_RD(AGC_RWNXAGCRAMP_ADDR) & ~((uint32_t)0x00000700)) | ((uint32_t)rampupndlindex << 8));
}

/**
 * @brief Returns the current value of the RAMPUPGAPQDB field in the RWNXAGCRAMP register.
 *
 * The RWNXAGCRAMP register will be read and the RAMPUPGAPQDB field's value will be returned.
 *
 * @return The current value of the RAMPUPGAPQDB field in the RWNXAGCRAMP register.
 */
__INLINE uint8_t agc_rampupgapqdb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCRAMP_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the RAMPUPGAPQDB field of the RWNXAGCRAMP register.
 *
 * The RWNXAGCRAMP register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rampupgapqdb - The value to set the field to.
 */
__INLINE void agc_rampupgapqdb_setf(uint8_t rampupgapqdb)
{
    ASSERT_ERR((((uint32_t)rampupgapqdb << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCRAMP_ADDR, (REG_PL_RD(AGC_RWNXAGCRAMP_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)rampupgapqdb << 0));
}

/// @}

/**
 * @name RWNXAGCSTABDIS register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  27:24         ADCPOWDISNDL   0x8
 *  22:16      ADCPOWDISTHRDBV   0x5D
 *  10:08         PLATNDLINDEX   0x3
 *  04:00            PLATDPQDB   0x8
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCSTABDIS register
#define AGC_RWNXAGCSTABDIS_ADDR   0x01002040
/// Offset of the RWNXAGCSTABDIS register from the base address
#define AGC_RWNXAGCSTABDIS_OFFSET 0x00002040
/// Index of the RWNXAGCSTABDIS register
#define AGC_RWNXAGCSTABDIS_INDEX  0x00000810
/// Reset value of the RWNXAGCSTABDIS register
#define AGC_RWNXAGCSTABDIS_RESET  0x085D0308

/**
 * @brief Returns the current value of the RWNXAGCSTABDIS register.
 * The RWNXAGCSTABDIS register will be read and its value returned.
 * @return The current value of the RWNXAGCSTABDIS register.
 */
__INLINE uint32_t agc_rwnxagcstabdis_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCSTABDIS_ADDR);
}

/**
 * @brief Sets the RWNXAGCSTABDIS register to a value.
 * The RWNXAGCSTABDIS register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcstabdis_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCSTABDIS_ADDR, value);
}

// field definitions
/// ADCPOWDISNDL field mask
#define AGC_ADCPOWDISNDL_MASK      ((uint32_t)0x0F000000)
/// ADCPOWDISNDL field LSB position
#define AGC_ADCPOWDISNDL_LSB       24
/// ADCPOWDISNDL field width
#define AGC_ADCPOWDISNDL_WIDTH     ((uint32_t)0x00000004)
/// ADCPOWDISTHRDBV field mask
#define AGC_ADCPOWDISTHRDBV_MASK   ((uint32_t)0x007F0000)
/// ADCPOWDISTHRDBV field LSB position
#define AGC_ADCPOWDISTHRDBV_LSB    16
/// ADCPOWDISTHRDBV field width
#define AGC_ADCPOWDISTHRDBV_WIDTH  ((uint32_t)0x00000007)
/// PLATNDLINDEX field mask
#define AGC_PLATNDLINDEX_MASK      ((uint32_t)0x00000700)
/// PLATNDLINDEX field LSB position
#define AGC_PLATNDLINDEX_LSB       8
/// PLATNDLINDEX field width
#define AGC_PLATNDLINDEX_WIDTH     ((uint32_t)0x00000003)
/// PLATDPQDB field mask
#define AGC_PLATDPQDB_MASK         ((uint32_t)0x0000001F)
/// PLATDPQDB field LSB position
#define AGC_PLATDPQDB_LSB          0
/// PLATDPQDB field width
#define AGC_PLATDPQDB_WIDTH        ((uint32_t)0x00000005)

/// ADCPOWDISNDL field reset value
#define AGC_ADCPOWDISNDL_RST       0x8
/// ADCPOWDISTHRDBV field reset value
#define AGC_ADCPOWDISTHRDBV_RST    0x5D
/// PLATNDLINDEX field reset value
#define AGC_PLATNDLINDEX_RST       0x3
/// PLATDPQDB field reset value
#define AGC_PLATDPQDB_RST          0x8

/**
 * @brief Constructs a value for the RWNXAGCSTABDIS register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] adcpowdisndl - The value to use for the ADCPOWDISNDL field.
 * @param[in] adcpowdisthrdbv - The value to use for the ADCPOWDISTHRDBV field.
 * @param[in] platndlindex - The value to use for the PLATNDLINDEX field.
 * @param[in] platdpqdb - The value to use for the PLATDPQDB field.
 */
__INLINE void agc_rwnxagcstabdis_pack(uint8_t adcpowdisndl, uint8_t adcpowdisthrdbv, uint8_t platndlindex, uint8_t platdpqdb)
{
    ASSERT_ERR((((uint32_t)adcpowdisndl << 24) & ~((uint32_t)0x0F000000)) == 0);
    ASSERT_ERR((((uint32_t)adcpowdisthrdbv << 16) & ~((uint32_t)0x007F0000)) == 0);
    ASSERT_ERR((((uint32_t)platndlindex << 8) & ~((uint32_t)0x00000700)) == 0);
    ASSERT_ERR((((uint32_t)platdpqdb << 0) & ~((uint32_t)0x0000001F)) == 0);
    REG_PL_WR(AGC_RWNXAGCSTABDIS_ADDR,  ((uint32_t)adcpowdisndl << 24) | ((uint32_t)adcpowdisthrdbv << 16) | ((uint32_t)platndlindex << 8) | ((uint32_t)platdpqdb << 0));
}

/**
 * @brief Unpacks RWNXAGCSTABDIS's fields from current value of the RWNXAGCSTABDIS register.
 *
 * Reads the RWNXAGCSTABDIS register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] adcpowdisndl - Will be populated with the current value of this field from the register.
 * @param[out] adcpowdisthrdbv - Will be populated with the current value of this field from the register.
 * @param[out] platndlindex - Will be populated with the current value of this field from the register.
 * @param[out] platdpqdb - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcstabdis_unpack(uint8_t *adcpowdisndl, uint8_t *adcpowdisthrdbv, uint8_t *platndlindex, uint8_t *platdpqdb)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSTABDIS_ADDR);

    *adcpowdisndl = (localVal & ((uint32_t)0x0F000000)) >> 24;
    *adcpowdisthrdbv = (localVal & ((uint32_t)0x007F0000)) >> 16;
    *platndlindex = (localVal & ((uint32_t)0x00000700)) >> 8;
    *platdpqdb = (localVal & ((uint32_t)0x0000001F)) >> 0;
}

/**
 * @brief Returns the current value of the ADCPOWDISNDL field in the RWNXAGCSTABDIS register.
 *
 * The RWNXAGCSTABDIS register will be read and the ADCPOWDISNDL field's value will be returned.
 *
 * @return The current value of the ADCPOWDISNDL field in the RWNXAGCSTABDIS register.
 */
__INLINE uint8_t agc_adcpowdisndl_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSTABDIS_ADDR);
    return ((localVal & ((uint32_t)0x0F000000)) >> 24);
}

/**
 * @brief Sets the ADCPOWDISNDL field of the RWNXAGCSTABDIS register.
 *
 * The RWNXAGCSTABDIS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] adcpowdisndl - The value to set the field to.
 */
__INLINE void agc_adcpowdisndl_setf(uint8_t adcpowdisndl)
{
    ASSERT_ERR((((uint32_t)adcpowdisndl << 24) & ~((uint32_t)0x0F000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCSTABDIS_ADDR, (REG_PL_RD(AGC_RWNXAGCSTABDIS_ADDR) & ~((uint32_t)0x0F000000)) | ((uint32_t)adcpowdisndl << 24));
}

/**
 * @brief Returns the current value of the ADCPOWDISTHRDBV field in the RWNXAGCSTABDIS register.
 *
 * The RWNXAGCSTABDIS register will be read and the ADCPOWDISTHRDBV field's value will be returned.
 *
 * @return The current value of the ADCPOWDISTHRDBV field in the RWNXAGCSTABDIS register.
 */
__INLINE uint8_t agc_adcpowdisthrdbv_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSTABDIS_ADDR);
    return ((localVal & ((uint32_t)0x007F0000)) >> 16);
}

/**
 * @brief Sets the ADCPOWDISTHRDBV field of the RWNXAGCSTABDIS register.
 *
 * The RWNXAGCSTABDIS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] adcpowdisthrdbv - The value to set the field to.
 */
__INLINE void agc_adcpowdisthrdbv_setf(uint8_t adcpowdisthrdbv)
{
    ASSERT_ERR((((uint32_t)adcpowdisthrdbv << 16) & ~((uint32_t)0x007F0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCSTABDIS_ADDR, (REG_PL_RD(AGC_RWNXAGCSTABDIS_ADDR) & ~((uint32_t)0x007F0000)) | ((uint32_t)adcpowdisthrdbv << 16));
}

/**
 * @brief Returns the current value of the PLATNDLINDEX field in the RWNXAGCSTABDIS register.
 *
 * The RWNXAGCSTABDIS register will be read and the PLATNDLINDEX field's value will be returned.
 *
 * @return The current value of the PLATNDLINDEX field in the RWNXAGCSTABDIS register.
 */
__INLINE uint8_t agc_platndlindex_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSTABDIS_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

/**
 * @brief Sets the PLATNDLINDEX field of the RWNXAGCSTABDIS register.
 *
 * The RWNXAGCSTABDIS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] platndlindex - The value to set the field to.
 */
__INLINE void agc_platndlindex_setf(uint8_t platndlindex)
{
    ASSERT_ERR((((uint32_t)platndlindex << 8) & ~((uint32_t)0x00000700)) == 0);
    REG_PL_WR(AGC_RWNXAGCSTABDIS_ADDR, (REG_PL_RD(AGC_RWNXAGCSTABDIS_ADDR) & ~((uint32_t)0x00000700)) | ((uint32_t)platndlindex << 8));
}

/**
 * @brief Returns the current value of the PLATDPQDB field in the RWNXAGCSTABDIS register.
 *
 * The RWNXAGCSTABDIS register will be read and the PLATDPQDB field's value will be returned.
 *
 * @return The current value of the PLATDPQDB field in the RWNXAGCSTABDIS register.
 */
__INLINE uint8_t agc_platdpqdb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSTABDIS_ADDR);
    return ((localVal & ((uint32_t)0x0000001F)) >> 0);
}

/**
 * @brief Sets the PLATDPQDB field of the RWNXAGCSTABDIS register.
 *
 * The RWNXAGCSTABDIS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] platdpqdb - The value to set the field to.
 */
__INLINE void agc_platdpqdb_setf(uint8_t platdpqdb)
{
    ASSERT_ERR((((uint32_t)platdpqdb << 0) & ~((uint32_t)0x0000001F)) == 0);
    REG_PL_WR(AGC_RWNXAGCSTABDIS_ADDR, (REG_PL_RD(AGC_RWNXAGCSTABDIS_ADDR) & ~((uint32_t)0x0000001F)) | ((uint32_t)platdpqdb << 0));
}

/// @}

/**
 * @name RWNXAGCDCCOMP0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:28          DCRADARTYPE   0x3
 *  25:24          DCMDM20TYPE   0x3
 *  21:20          DCAGC20TYPE   0x1
 *  17:16            DCADCTYPE   0x1
 *  14:08    DCADCHOLDTIME50NS   0x0
 *  06:00    DCADCZEROTIME50NS   0x0
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCDCCOMP0 register
#define AGC_RWNXAGCDCCOMP0_ADDR   0x01002044
/// Offset of the RWNXAGCDCCOMP0 register from the base address
#define AGC_RWNXAGCDCCOMP0_OFFSET 0x00002044
/// Index of the RWNXAGCDCCOMP0 register
#define AGC_RWNXAGCDCCOMP0_INDEX  0x00000811
/// Reset value of the RWNXAGCDCCOMP0 register
#define AGC_RWNXAGCDCCOMP0_RESET  0x33110000

/**
 * @brief Returns the current value of the RWNXAGCDCCOMP0 register.
 * The RWNXAGCDCCOMP0 register will be read and its value returned.
 * @return The current value of the RWNXAGCDCCOMP0 register.
 */
__INLINE uint32_t agc_rwnxagcdccomp0_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCDCCOMP0_ADDR);
}

/**
 * @brief Sets the RWNXAGCDCCOMP0 register to a value.
 * The RWNXAGCDCCOMP0 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcdccomp0_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCDCCOMP0_ADDR, value);
}

// field definitions
/// DCRADARTYPE field mask
#define AGC_DCRADARTYPE_MASK         ((uint32_t)0x30000000)
/// DCRADARTYPE field LSB position
#define AGC_DCRADARTYPE_LSB          28
/// DCRADARTYPE field width
#define AGC_DCRADARTYPE_WIDTH        ((uint32_t)0x00000002)
/// DCMDM20TYPE field mask
#define AGC_DCMDM20TYPE_MASK         ((uint32_t)0x03000000)
/// DCMDM20TYPE field LSB position
#define AGC_DCMDM20TYPE_LSB          24
/// DCMDM20TYPE field width
#define AGC_DCMDM20TYPE_WIDTH        ((uint32_t)0x00000002)
/// DCAGC20TYPE field mask
#define AGC_DCAGC20TYPE_MASK         ((uint32_t)0x00300000)
/// DCAGC20TYPE field LSB position
#define AGC_DCAGC20TYPE_LSB          20
/// DCAGC20TYPE field width
#define AGC_DCAGC20TYPE_WIDTH        ((uint32_t)0x00000002)
/// DCADCTYPE field mask
#define AGC_DCADCTYPE_MASK           ((uint32_t)0x00030000)
/// DCADCTYPE field LSB position
#define AGC_DCADCTYPE_LSB            16
/// DCADCTYPE field width
#define AGC_DCADCTYPE_WIDTH          ((uint32_t)0x00000002)
/// DCADCHOLDTIME50NS field mask
#define AGC_DCADCHOLDTIME50NS_MASK   ((uint32_t)0x00007F00)
/// DCADCHOLDTIME50NS field LSB position
#define AGC_DCADCHOLDTIME50NS_LSB    8
/// DCADCHOLDTIME50NS field width
#define AGC_DCADCHOLDTIME50NS_WIDTH  ((uint32_t)0x00000007)
/// DCADCZEROTIME50NS field mask
#define AGC_DCADCZEROTIME50NS_MASK   ((uint32_t)0x0000007F)
/// DCADCZEROTIME50NS field LSB position
#define AGC_DCADCZEROTIME50NS_LSB    0
/// DCADCZEROTIME50NS field width
#define AGC_DCADCZEROTIME50NS_WIDTH  ((uint32_t)0x00000007)

/// DCRADARTYPE field reset value
#define AGC_DCRADARTYPE_RST          0x3
/// DCMDM20TYPE field reset value
#define AGC_DCMDM20TYPE_RST          0x3
/// DCAGC20TYPE field reset value
#define AGC_DCAGC20TYPE_RST          0x1
/// DCADCTYPE field reset value
#define AGC_DCADCTYPE_RST            0x1
/// DCADCHOLDTIME50NS field reset value
#define AGC_DCADCHOLDTIME50NS_RST    0x0
/// DCADCZEROTIME50NS field reset value
#define AGC_DCADCZEROTIME50NS_RST    0x0

/**
 * @brief Constructs a value for the RWNXAGCDCCOMP0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] dcradartype - The value to use for the DCRADARTYPE field.
 * @param[in] dcmdm20type - The value to use for the DCMDM20TYPE field.
 * @param[in] dcagc20type - The value to use for the DCAGC20TYPE field.
 * @param[in] dcadctype - The value to use for the DCADCTYPE field.
 * @param[in] dcadcholdtime50ns - The value to use for the DCADCHOLDTIME50NS field.
 * @param[in] dcadczerotime50ns - The value to use for the DCADCZEROTIME50NS field.
 */
__INLINE void agc_rwnxagcdccomp0_pack(uint8_t dcradartype, uint8_t dcmdm20type, uint8_t dcagc20type, uint8_t dcadctype, uint8_t dcadcholdtime50ns, uint8_t dcadczerotime50ns)
{
    ASSERT_ERR((((uint32_t)dcradartype << 28) & ~((uint32_t)0x30000000)) == 0);
    ASSERT_ERR((((uint32_t)dcmdm20type << 24) & ~((uint32_t)0x03000000)) == 0);
    ASSERT_ERR((((uint32_t)dcagc20type << 20) & ~((uint32_t)0x00300000)) == 0);
    ASSERT_ERR((((uint32_t)dcadctype << 16) & ~((uint32_t)0x00030000)) == 0);
    ASSERT_ERR((((uint32_t)dcadcholdtime50ns << 8) & ~((uint32_t)0x00007F00)) == 0);
    ASSERT_ERR((((uint32_t)dcadczerotime50ns << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(AGC_RWNXAGCDCCOMP0_ADDR,  ((uint32_t)dcradartype << 28) | ((uint32_t)dcmdm20type << 24) | ((uint32_t)dcagc20type << 20) | ((uint32_t)dcadctype << 16) | ((uint32_t)dcadcholdtime50ns << 8) | ((uint32_t)dcadczerotime50ns << 0));
}

/**
 * @brief Unpacks RWNXAGCDCCOMP0's fields from current value of the RWNXAGCDCCOMP0 register.
 *
 * Reads the RWNXAGCDCCOMP0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] dcradartype - Will be populated with the current value of this field from the register.
 * @param[out] dcmdm20type - Will be populated with the current value of this field from the register.
 * @param[out] dcagc20type - Will be populated with the current value of this field from the register.
 * @param[out] dcadctype - Will be populated with the current value of this field from the register.
 * @param[out] dcadcholdtime50ns - Will be populated with the current value of this field from the register.
 * @param[out] dcadczerotime50ns - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcdccomp0_unpack(uint8_t *dcradartype, uint8_t *dcmdm20type, uint8_t *dcagc20type, uint8_t *dcadctype, uint8_t *dcadcholdtime50ns, uint8_t *dcadczerotime50ns)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDCCOMP0_ADDR);

    *dcradartype = (localVal & ((uint32_t)0x30000000)) >> 28;
    *dcmdm20type = (localVal & ((uint32_t)0x03000000)) >> 24;
    *dcagc20type = (localVal & ((uint32_t)0x00300000)) >> 20;
    *dcadctype = (localVal & ((uint32_t)0x00030000)) >> 16;
    *dcadcholdtime50ns = (localVal & ((uint32_t)0x00007F00)) >> 8;
    *dcadczerotime50ns = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the DCRADARTYPE field in the RWNXAGCDCCOMP0 register.
 *
 * The RWNXAGCDCCOMP0 register will be read and the DCRADARTYPE field's value will be returned.
 *
 * @return The current value of the DCRADARTYPE field in the RWNXAGCDCCOMP0 register.
 */
__INLINE uint8_t agc_dcradartype_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDCCOMP0_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

/**
 * @brief Sets the DCRADARTYPE field of the RWNXAGCDCCOMP0 register.
 *
 * The RWNXAGCDCCOMP0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dcradartype - The value to set the field to.
 */
__INLINE void agc_dcradartype_setf(uint8_t dcradartype)
{
    ASSERT_ERR((((uint32_t)dcradartype << 28) & ~((uint32_t)0x30000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCDCCOMP0_ADDR, (REG_PL_RD(AGC_RWNXAGCDCCOMP0_ADDR) & ~((uint32_t)0x30000000)) | ((uint32_t)dcradartype << 28));
}

/**
 * @brief Returns the current value of the DCMDM20TYPE field in the RWNXAGCDCCOMP0 register.
 *
 * The RWNXAGCDCCOMP0 register will be read and the DCMDM20TYPE field's value will be returned.
 *
 * @return The current value of the DCMDM20TYPE field in the RWNXAGCDCCOMP0 register.
 */
__INLINE uint8_t agc_dcmdm20type_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDCCOMP0_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

/**
 * @brief Sets the DCMDM20TYPE field of the RWNXAGCDCCOMP0 register.
 *
 * The RWNXAGCDCCOMP0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dcmdm20type - The value to set the field to.
 */
__INLINE void agc_dcmdm20type_setf(uint8_t dcmdm20type)
{
    ASSERT_ERR((((uint32_t)dcmdm20type << 24) & ~((uint32_t)0x03000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCDCCOMP0_ADDR, (REG_PL_RD(AGC_RWNXAGCDCCOMP0_ADDR) & ~((uint32_t)0x03000000)) | ((uint32_t)dcmdm20type << 24));
}

/**
 * @brief Returns the current value of the DCAGC20TYPE field in the RWNXAGCDCCOMP0 register.
 *
 * The RWNXAGCDCCOMP0 register will be read and the DCAGC20TYPE field's value will be returned.
 *
 * @return The current value of the DCAGC20TYPE field in the RWNXAGCDCCOMP0 register.
 */
__INLINE uint8_t agc_dcagc20type_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDCCOMP0_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

/**
 * @brief Sets the DCAGC20TYPE field of the RWNXAGCDCCOMP0 register.
 *
 * The RWNXAGCDCCOMP0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dcagc20type - The value to set the field to.
 */
__INLINE void agc_dcagc20type_setf(uint8_t dcagc20type)
{
    ASSERT_ERR((((uint32_t)dcagc20type << 20) & ~((uint32_t)0x00300000)) == 0);
    REG_PL_WR(AGC_RWNXAGCDCCOMP0_ADDR, (REG_PL_RD(AGC_RWNXAGCDCCOMP0_ADDR) & ~((uint32_t)0x00300000)) | ((uint32_t)dcagc20type << 20));
}

/**
 * @brief Returns the current value of the DCADCTYPE field in the RWNXAGCDCCOMP0 register.
 *
 * The RWNXAGCDCCOMP0 register will be read and the DCADCTYPE field's value will be returned.
 *
 * @return The current value of the DCADCTYPE field in the RWNXAGCDCCOMP0 register.
 */
__INLINE uint8_t agc_dcadctype_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDCCOMP0_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

/**
 * @brief Sets the DCADCTYPE field of the RWNXAGCDCCOMP0 register.
 *
 * The RWNXAGCDCCOMP0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dcadctype - The value to set the field to.
 */
__INLINE void agc_dcadctype_setf(uint8_t dcadctype)
{
    ASSERT_ERR((((uint32_t)dcadctype << 16) & ~((uint32_t)0x00030000)) == 0);
    REG_PL_WR(AGC_RWNXAGCDCCOMP0_ADDR, (REG_PL_RD(AGC_RWNXAGCDCCOMP0_ADDR) & ~((uint32_t)0x00030000)) | ((uint32_t)dcadctype << 16));
}

/**
 * @brief Returns the current value of the DCADCHOLDTIME50NS field in the RWNXAGCDCCOMP0 register.
 *
 * The RWNXAGCDCCOMP0 register will be read and the DCADCHOLDTIME50NS field's value will be returned.
 *
 * @return The current value of the DCADCHOLDTIME50NS field in the RWNXAGCDCCOMP0 register.
 */
__INLINE uint8_t agc_dcadcholdtime50ns_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDCCOMP0_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}

/**
 * @brief Sets the DCADCHOLDTIME50NS field of the RWNXAGCDCCOMP0 register.
 *
 * The RWNXAGCDCCOMP0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dcadcholdtime50ns - The value to set the field to.
 */
__INLINE void agc_dcadcholdtime50ns_setf(uint8_t dcadcholdtime50ns)
{
    ASSERT_ERR((((uint32_t)dcadcholdtime50ns << 8) & ~((uint32_t)0x00007F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCDCCOMP0_ADDR, (REG_PL_RD(AGC_RWNXAGCDCCOMP0_ADDR) & ~((uint32_t)0x00007F00)) | ((uint32_t)dcadcholdtime50ns << 8));
}

/**
 * @brief Returns the current value of the DCADCZEROTIME50NS field in the RWNXAGCDCCOMP0 register.
 *
 * The RWNXAGCDCCOMP0 register will be read and the DCADCZEROTIME50NS field's value will be returned.
 *
 * @return The current value of the DCADCZEROTIME50NS field in the RWNXAGCDCCOMP0 register.
 */
__INLINE uint8_t agc_dcadczerotime50ns_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDCCOMP0_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/**
 * @brief Sets the DCADCZEROTIME50NS field of the RWNXAGCDCCOMP0 register.
 *
 * The RWNXAGCDCCOMP0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dcadczerotime50ns - The value to set the field to.
 */
__INLINE void agc_dcadczerotime50ns_setf(uint8_t dcadczerotime50ns)
{
    ASSERT_ERR((((uint32_t)dcadczerotime50ns << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(AGC_RWNXAGCDCCOMP0_ADDR, (REG_PL_RD(AGC_RWNXAGCDCCOMP0_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)dcadczerotime50ns << 0));
}

/// @}

/**
 * @name RWNXAGCDCCOMP1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  30:24   DCMDM20HOLDTIME50NS   0x14
 *  22:16   DCMDM20ZEROTIME50NS   0x0
 *  14:08   DCAGC20HOLDTIME50NS   0x0
 *  06:00   DCAGC20ZEROTIME50NS   0x0
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCDCCOMP1 register
#define AGC_RWNXAGCDCCOMP1_ADDR   0x01002048
/// Offset of the RWNXAGCDCCOMP1 register from the base address
#define AGC_RWNXAGCDCCOMP1_OFFSET 0x00002048
/// Index of the RWNXAGCDCCOMP1 register
#define AGC_RWNXAGCDCCOMP1_INDEX  0x00000812
/// Reset value of the RWNXAGCDCCOMP1 register
#define AGC_RWNXAGCDCCOMP1_RESET  0x14000000

/**
 * @brief Returns the current value of the RWNXAGCDCCOMP1 register.
 * The RWNXAGCDCCOMP1 register will be read and its value returned.
 * @return The current value of the RWNXAGCDCCOMP1 register.
 */
__INLINE uint32_t agc_rwnxagcdccomp1_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCDCCOMP1_ADDR);
}

/**
 * @brief Sets the RWNXAGCDCCOMP1 register to a value.
 * The RWNXAGCDCCOMP1 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcdccomp1_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCDCCOMP1_ADDR, value);
}

// field definitions
/// DCMDM20HOLDTIME50NS field mask
#define AGC_DCMDM20HOLDTIME50NS_MASK   ((uint32_t)0x7F000000)
/// DCMDM20HOLDTIME50NS field LSB position
#define AGC_DCMDM20HOLDTIME50NS_LSB    24
/// DCMDM20HOLDTIME50NS field width
#define AGC_DCMDM20HOLDTIME50NS_WIDTH  ((uint32_t)0x00000007)
/// DCMDM20ZEROTIME50NS field mask
#define AGC_DCMDM20ZEROTIME50NS_MASK   ((uint32_t)0x007F0000)
/// DCMDM20ZEROTIME50NS field LSB position
#define AGC_DCMDM20ZEROTIME50NS_LSB    16
/// DCMDM20ZEROTIME50NS field width
#define AGC_DCMDM20ZEROTIME50NS_WIDTH  ((uint32_t)0x00000007)
/// DCAGC20HOLDTIME50NS field mask
#define AGC_DCAGC20HOLDTIME50NS_MASK   ((uint32_t)0x00007F00)
/// DCAGC20HOLDTIME50NS field LSB position
#define AGC_DCAGC20HOLDTIME50NS_LSB    8
/// DCAGC20HOLDTIME50NS field width
#define AGC_DCAGC20HOLDTIME50NS_WIDTH  ((uint32_t)0x00000007)
/// DCAGC20ZEROTIME50NS field mask
#define AGC_DCAGC20ZEROTIME50NS_MASK   ((uint32_t)0x0000007F)
/// DCAGC20ZEROTIME50NS field LSB position
#define AGC_DCAGC20ZEROTIME50NS_LSB    0
/// DCAGC20ZEROTIME50NS field width
#define AGC_DCAGC20ZEROTIME50NS_WIDTH  ((uint32_t)0x00000007)

/// DCMDM20HOLDTIME50NS field reset value
#define AGC_DCMDM20HOLDTIME50NS_RST    0x14
/// DCMDM20ZEROTIME50NS field reset value
#define AGC_DCMDM20ZEROTIME50NS_RST    0x0
/// DCAGC20HOLDTIME50NS field reset value
#define AGC_DCAGC20HOLDTIME50NS_RST    0x0
/// DCAGC20ZEROTIME50NS field reset value
#define AGC_DCAGC20ZEROTIME50NS_RST    0x0

/**
 * @brief Constructs a value for the RWNXAGCDCCOMP1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] dcmdm20holdtime50ns - The value to use for the DCMDM20HOLDTIME50NS field.
 * @param[in] dcmdm20zerotime50ns - The value to use for the DCMDM20ZEROTIME50NS field.
 * @param[in] dcagc20holdtime50ns - The value to use for the DCAGC20HOLDTIME50NS field.
 * @param[in] dcagc20zerotime50ns - The value to use for the DCAGC20ZEROTIME50NS field.
 */
__INLINE void agc_rwnxagcdccomp1_pack(uint8_t dcmdm20holdtime50ns, uint8_t dcmdm20zerotime50ns, uint8_t dcagc20holdtime50ns, uint8_t dcagc20zerotime50ns)
{
    ASSERT_ERR((((uint32_t)dcmdm20holdtime50ns << 24) & ~((uint32_t)0x7F000000)) == 0);
    ASSERT_ERR((((uint32_t)dcmdm20zerotime50ns << 16) & ~((uint32_t)0x007F0000)) == 0);
    ASSERT_ERR((((uint32_t)dcagc20holdtime50ns << 8) & ~((uint32_t)0x00007F00)) == 0);
    ASSERT_ERR((((uint32_t)dcagc20zerotime50ns << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(AGC_RWNXAGCDCCOMP1_ADDR,  ((uint32_t)dcmdm20holdtime50ns << 24) | ((uint32_t)dcmdm20zerotime50ns << 16) | ((uint32_t)dcagc20holdtime50ns << 8) | ((uint32_t)dcagc20zerotime50ns << 0));
}

/**
 * @brief Unpacks RWNXAGCDCCOMP1's fields from current value of the RWNXAGCDCCOMP1 register.
 *
 * Reads the RWNXAGCDCCOMP1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] dcmdm20holdtime50ns - Will be populated with the current value of this field from the register.
 * @param[out] dcmdm20zerotime50ns - Will be populated with the current value of this field from the register.
 * @param[out] dcagc20holdtime50ns - Will be populated with the current value of this field from the register.
 * @param[out] dcagc20zerotime50ns - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcdccomp1_unpack(uint8_t *dcmdm20holdtime50ns, uint8_t *dcmdm20zerotime50ns, uint8_t *dcagc20holdtime50ns, uint8_t *dcagc20zerotime50ns)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDCCOMP1_ADDR);

    *dcmdm20holdtime50ns = (localVal & ((uint32_t)0x7F000000)) >> 24;
    *dcmdm20zerotime50ns = (localVal & ((uint32_t)0x007F0000)) >> 16;
    *dcagc20holdtime50ns = (localVal & ((uint32_t)0x00007F00)) >> 8;
    *dcagc20zerotime50ns = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the DCMDM20HOLDTIME50NS field in the RWNXAGCDCCOMP1 register.
 *
 * The RWNXAGCDCCOMP1 register will be read and the DCMDM20HOLDTIME50NS field's value will be returned.
 *
 * @return The current value of the DCMDM20HOLDTIME50NS field in the RWNXAGCDCCOMP1 register.
 */
__INLINE uint8_t agc_dcmdm20holdtime50ns_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDCCOMP1_ADDR);
    return ((localVal & ((uint32_t)0x7F000000)) >> 24);
}

/**
 * @brief Sets the DCMDM20HOLDTIME50NS field of the RWNXAGCDCCOMP1 register.
 *
 * The RWNXAGCDCCOMP1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dcmdm20holdtime50ns - The value to set the field to.
 */
__INLINE void agc_dcmdm20holdtime50ns_setf(uint8_t dcmdm20holdtime50ns)
{
    ASSERT_ERR((((uint32_t)dcmdm20holdtime50ns << 24) & ~((uint32_t)0x7F000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCDCCOMP1_ADDR, (REG_PL_RD(AGC_RWNXAGCDCCOMP1_ADDR) & ~((uint32_t)0x7F000000)) | ((uint32_t)dcmdm20holdtime50ns << 24));
}

/**
 * @brief Returns the current value of the DCMDM20ZEROTIME50NS field in the RWNXAGCDCCOMP1 register.
 *
 * The RWNXAGCDCCOMP1 register will be read and the DCMDM20ZEROTIME50NS field's value will be returned.
 *
 * @return The current value of the DCMDM20ZEROTIME50NS field in the RWNXAGCDCCOMP1 register.
 */
__INLINE uint8_t agc_dcmdm20zerotime50ns_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDCCOMP1_ADDR);
    return ((localVal & ((uint32_t)0x007F0000)) >> 16);
}

/**
 * @brief Sets the DCMDM20ZEROTIME50NS field of the RWNXAGCDCCOMP1 register.
 *
 * The RWNXAGCDCCOMP1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dcmdm20zerotime50ns - The value to set the field to.
 */
__INLINE void agc_dcmdm20zerotime50ns_setf(uint8_t dcmdm20zerotime50ns)
{
    ASSERT_ERR((((uint32_t)dcmdm20zerotime50ns << 16) & ~((uint32_t)0x007F0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCDCCOMP1_ADDR, (REG_PL_RD(AGC_RWNXAGCDCCOMP1_ADDR) & ~((uint32_t)0x007F0000)) | ((uint32_t)dcmdm20zerotime50ns << 16));
}

/**
 * @brief Returns the current value of the DCAGC20HOLDTIME50NS field in the RWNXAGCDCCOMP1 register.
 *
 * The RWNXAGCDCCOMP1 register will be read and the DCAGC20HOLDTIME50NS field's value will be returned.
 *
 * @return The current value of the DCAGC20HOLDTIME50NS field in the RWNXAGCDCCOMP1 register.
 */
__INLINE uint8_t agc_dcagc20holdtime50ns_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDCCOMP1_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}

/**
 * @brief Sets the DCAGC20HOLDTIME50NS field of the RWNXAGCDCCOMP1 register.
 *
 * The RWNXAGCDCCOMP1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dcagc20holdtime50ns - The value to set the field to.
 */
__INLINE void agc_dcagc20holdtime50ns_setf(uint8_t dcagc20holdtime50ns)
{
    ASSERT_ERR((((uint32_t)dcagc20holdtime50ns << 8) & ~((uint32_t)0x00007F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCDCCOMP1_ADDR, (REG_PL_RD(AGC_RWNXAGCDCCOMP1_ADDR) & ~((uint32_t)0x00007F00)) | ((uint32_t)dcagc20holdtime50ns << 8));
}

/**
 * @brief Returns the current value of the DCAGC20ZEROTIME50NS field in the RWNXAGCDCCOMP1 register.
 *
 * The RWNXAGCDCCOMP1 register will be read and the DCAGC20ZEROTIME50NS field's value will be returned.
 *
 * @return The current value of the DCAGC20ZEROTIME50NS field in the RWNXAGCDCCOMP1 register.
 */
__INLINE uint8_t agc_dcagc20zerotime50ns_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDCCOMP1_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/**
 * @brief Sets the DCAGC20ZEROTIME50NS field of the RWNXAGCDCCOMP1 register.
 *
 * The RWNXAGCDCCOMP1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dcagc20zerotime50ns - The value to set the field to.
 */
__INLINE void agc_dcagc20zerotime50ns_setf(uint8_t dcagc20zerotime50ns)
{
    ASSERT_ERR((((uint32_t)dcagc20zerotime50ns << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(AGC_RWNXAGCDCCOMP1_ADDR, (REG_PL_RD(AGC_RWNXAGCDCCOMP1_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)dcagc20zerotime50ns << 0));
}

/// @}

/**
 * @name RWNXAGCDCCOMP2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  22:16   DCRADARHOLDTIME50NS   0x0
 *  14:08   DCRADARZEROTIME50NS   0x0
 *  06:00        DELDCSTABLECC   0x0
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCDCCOMP2 register
#define AGC_RWNXAGCDCCOMP2_ADDR   0x0100204C
/// Offset of the RWNXAGCDCCOMP2 register from the base address
#define AGC_RWNXAGCDCCOMP2_OFFSET 0x0000204C
/// Index of the RWNXAGCDCCOMP2 register
#define AGC_RWNXAGCDCCOMP2_INDEX  0x00000813
/// Reset value of the RWNXAGCDCCOMP2 register
#define AGC_RWNXAGCDCCOMP2_RESET  0x00000000

/**
 * @brief Returns the current value of the RWNXAGCDCCOMP2 register.
 * The RWNXAGCDCCOMP2 register will be read and its value returned.
 * @return The current value of the RWNXAGCDCCOMP2 register.
 */
__INLINE uint32_t agc_rwnxagcdccomp2_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCDCCOMP2_ADDR);
}

/**
 * @brief Sets the RWNXAGCDCCOMP2 register to a value.
 * The RWNXAGCDCCOMP2 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcdccomp2_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCDCCOMP2_ADDR, value);
}

// field definitions
/// DCRADARHOLDTIME50NS field mask
#define AGC_DCRADARHOLDTIME50NS_MASK   ((uint32_t)0x007F0000)
/// DCRADARHOLDTIME50NS field LSB position
#define AGC_DCRADARHOLDTIME50NS_LSB    16
/// DCRADARHOLDTIME50NS field width
#define AGC_DCRADARHOLDTIME50NS_WIDTH  ((uint32_t)0x00000007)
/// DCRADARZEROTIME50NS field mask
#define AGC_DCRADARZEROTIME50NS_MASK   ((uint32_t)0x00007F00)
/// DCRADARZEROTIME50NS field LSB position
#define AGC_DCRADARZEROTIME50NS_LSB    8
/// DCRADARZEROTIME50NS field width
#define AGC_DCRADARZEROTIME50NS_WIDTH  ((uint32_t)0x00000007)
/// DELDCSTABLECC field mask
#define AGC_DELDCSTABLECC_MASK         ((uint32_t)0x0000007F)
/// DELDCSTABLECC field LSB position
#define AGC_DELDCSTABLECC_LSB          0
/// DELDCSTABLECC field width
#define AGC_DELDCSTABLECC_WIDTH        ((uint32_t)0x00000007)

/// DCRADARHOLDTIME50NS field reset value
#define AGC_DCRADARHOLDTIME50NS_RST    0x0
/// DCRADARZEROTIME50NS field reset value
#define AGC_DCRADARZEROTIME50NS_RST    0x0
/// DELDCSTABLECC field reset value
#define AGC_DELDCSTABLECC_RST          0x0

/**
 * @brief Constructs a value for the RWNXAGCDCCOMP2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] dcradarholdtime50ns - The value to use for the DCRADARHOLDTIME50NS field.
 * @param[in] dcradarzerotime50ns - The value to use for the DCRADARZEROTIME50NS field.
 * @param[in] deldcstablecc - The value to use for the DELDCSTABLECC field.
 */
__INLINE void agc_rwnxagcdccomp2_pack(uint8_t dcradarholdtime50ns, uint8_t dcradarzerotime50ns, uint8_t deldcstablecc)
{
    ASSERT_ERR((((uint32_t)dcradarholdtime50ns << 16) & ~((uint32_t)0x007F0000)) == 0);
    ASSERT_ERR((((uint32_t)dcradarzerotime50ns << 8) & ~((uint32_t)0x00007F00)) == 0);
    ASSERT_ERR((((uint32_t)deldcstablecc << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(AGC_RWNXAGCDCCOMP2_ADDR,  ((uint32_t)dcradarholdtime50ns << 16) | ((uint32_t)dcradarzerotime50ns << 8) | ((uint32_t)deldcstablecc << 0));
}

/**
 * @brief Unpacks RWNXAGCDCCOMP2's fields from current value of the RWNXAGCDCCOMP2 register.
 *
 * Reads the RWNXAGCDCCOMP2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] dcradarholdtime50ns - Will be populated with the current value of this field from the register.
 * @param[out] dcradarzerotime50ns - Will be populated with the current value of this field from the register.
 * @param[out] deldcstablecc - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcdccomp2_unpack(uint8_t *dcradarholdtime50ns, uint8_t *dcradarzerotime50ns, uint8_t *deldcstablecc)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDCCOMP2_ADDR);

    *dcradarholdtime50ns = (localVal & ((uint32_t)0x007F0000)) >> 16;
    *dcradarzerotime50ns = (localVal & ((uint32_t)0x00007F00)) >> 8;
    *deldcstablecc = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the DCRADARHOLDTIME50NS field in the RWNXAGCDCCOMP2 register.
 *
 * The RWNXAGCDCCOMP2 register will be read and the DCRADARHOLDTIME50NS field's value will be returned.
 *
 * @return The current value of the DCRADARHOLDTIME50NS field in the RWNXAGCDCCOMP2 register.
 */
__INLINE uint8_t agc_dcradarholdtime50ns_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDCCOMP2_ADDR);
    return ((localVal & ((uint32_t)0x007F0000)) >> 16);
}

/**
 * @brief Sets the DCRADARHOLDTIME50NS field of the RWNXAGCDCCOMP2 register.
 *
 * The RWNXAGCDCCOMP2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dcradarholdtime50ns - The value to set the field to.
 */
__INLINE void agc_dcradarholdtime50ns_setf(uint8_t dcradarholdtime50ns)
{
    ASSERT_ERR((((uint32_t)dcradarholdtime50ns << 16) & ~((uint32_t)0x007F0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCDCCOMP2_ADDR, (REG_PL_RD(AGC_RWNXAGCDCCOMP2_ADDR) & ~((uint32_t)0x007F0000)) | ((uint32_t)dcradarholdtime50ns << 16));
}

/**
 * @brief Returns the current value of the DCRADARZEROTIME50NS field in the RWNXAGCDCCOMP2 register.
 *
 * The RWNXAGCDCCOMP2 register will be read and the DCRADARZEROTIME50NS field's value will be returned.
 *
 * @return The current value of the DCRADARZEROTIME50NS field in the RWNXAGCDCCOMP2 register.
 */
__INLINE uint8_t agc_dcradarzerotime50ns_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDCCOMP2_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}

/**
 * @brief Sets the DCRADARZEROTIME50NS field of the RWNXAGCDCCOMP2 register.
 *
 * The RWNXAGCDCCOMP2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dcradarzerotime50ns - The value to set the field to.
 */
__INLINE void agc_dcradarzerotime50ns_setf(uint8_t dcradarzerotime50ns)
{
    ASSERT_ERR((((uint32_t)dcradarzerotime50ns << 8) & ~((uint32_t)0x00007F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCDCCOMP2_ADDR, (REG_PL_RD(AGC_RWNXAGCDCCOMP2_ADDR) & ~((uint32_t)0x00007F00)) | ((uint32_t)dcradarzerotime50ns << 8));
}

/**
 * @brief Returns the current value of the DELDCSTABLECC field in the RWNXAGCDCCOMP2 register.
 *
 * The RWNXAGCDCCOMP2 register will be read and the DELDCSTABLECC field's value will be returned.
 *
 * @return The current value of the DELDCSTABLECC field in the RWNXAGCDCCOMP2 register.
 */
__INLINE uint8_t agc_deldcstablecc_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDCCOMP2_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/**
 * @brief Sets the DELDCSTABLECC field of the RWNXAGCDCCOMP2 register.
 *
 * The RWNXAGCDCCOMP2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] deldcstablecc - The value to set the field to.
 */
__INLINE void agc_deldcstablecc_setf(uint8_t deldcstablecc)
{
    ASSERT_ERR((((uint32_t)deldcstablecc << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(AGC_RWNXAGCDCCOMP2_ADDR, (REG_PL_RD(AGC_RWNXAGCDCCOMP2_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)deldcstablecc << 0));
}

/// @}

/**
 * @name RWNXAGCEVTSAT register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:26              EVT0OP3   0x1
 *  25:20              EVT0OP2   0x10
 *  19:14              EVT0OP1   0x11
 *     13         EVT0PATHCOMB   0
 *  12:11           EVT0OPCOMB   0x1
 *  10:00           EVT0TGTADD   0x4
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCEVTSAT register
#define AGC_RWNXAGCEVTSAT_ADDR   0x01002050
/// Offset of the RWNXAGCEVTSAT register from the base address
#define AGC_RWNXAGCEVTSAT_OFFSET 0x00002050
/// Index of the RWNXAGCEVTSAT register
#define AGC_RWNXAGCEVTSAT_INDEX  0x00000814
/// Reset value of the RWNXAGCEVTSAT register
#define AGC_RWNXAGCEVTSAT_RESET  0x05044804

/**
 * @brief Returns the current value of the RWNXAGCEVTSAT register.
 * The RWNXAGCEVTSAT register will be read and its value returned.
 * @return The current value of the RWNXAGCEVTSAT register.
 */
__INLINE uint32_t agc_rwnxagcevtsat_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCEVTSAT_ADDR);
}

/**
 * @brief Sets the RWNXAGCEVTSAT register to a value.
 * The RWNXAGCEVTSAT register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcevtsat_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCEVTSAT_ADDR, value);
}

// field definitions
/// EVT0OP3 field mask
#define AGC_EVT0OP3_MASK        ((uint32_t)0xFC000000)
/// EVT0OP3 field LSB position
#define AGC_EVT0OP3_LSB         26
/// EVT0OP3 field width
#define AGC_EVT0OP3_WIDTH       ((uint32_t)0x00000006)
/// EVT0OP2 field mask
#define AGC_EVT0OP2_MASK        ((uint32_t)0x03F00000)
/// EVT0OP2 field LSB position
#define AGC_EVT0OP2_LSB         20
/// EVT0OP2 field width
#define AGC_EVT0OP2_WIDTH       ((uint32_t)0x00000006)
/// EVT0OP1 field mask
#define AGC_EVT0OP1_MASK        ((uint32_t)0x000FC000)
/// EVT0OP1 field LSB position
#define AGC_EVT0OP1_LSB         14
/// EVT0OP1 field width
#define AGC_EVT0OP1_WIDTH       ((uint32_t)0x00000006)
/// EVT0PATHCOMB field bit
#define AGC_EVT0PATHCOMB_BIT    ((uint32_t)0x00002000)
/// EVT0PATHCOMB field position
#define AGC_EVT0PATHCOMB_POS    13
/// EVT0OPCOMB field mask
#define AGC_EVT0OPCOMB_MASK     ((uint32_t)0x00001800)
/// EVT0OPCOMB field LSB position
#define AGC_EVT0OPCOMB_LSB      11
/// EVT0OPCOMB field width
#define AGC_EVT0OPCOMB_WIDTH    ((uint32_t)0x00000002)
/// EVT0TGTADD field mask
#define AGC_EVT0TGTADD_MASK     ((uint32_t)0x000007FF)
/// EVT0TGTADD field LSB position
#define AGC_EVT0TGTADD_LSB      0
/// EVT0TGTADD field width
#define AGC_EVT0TGTADD_WIDTH    ((uint32_t)0x0000000B)

/// EVT0OP3 field reset value
#define AGC_EVT0OP3_RST         0x1
/// EVT0OP2 field reset value
#define AGC_EVT0OP2_RST         0x10
/// EVT0OP1 field reset value
#define AGC_EVT0OP1_RST         0x11
/// EVT0PATHCOMB field reset value
#define AGC_EVT0PATHCOMB_RST    0x0
/// EVT0OPCOMB field reset value
#define AGC_EVT0OPCOMB_RST      0x1
/// EVT0TGTADD field reset value
#define AGC_EVT0TGTADD_RST      0x4

/**
 * @brief Constructs a value for the RWNXAGCEVTSAT register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] evt0op3 - The value to use for the EVT0OP3 field.
 * @param[in] evt0op2 - The value to use for the EVT0OP2 field.
 * @param[in] evt0op1 - The value to use for the EVT0OP1 field.
 * @param[in] evt0pathcomb - The value to use for the EVT0PATHCOMB field.
 * @param[in] evt0opcomb - The value to use for the EVT0OPCOMB field.
 * @param[in] evt0tgtadd - The value to use for the EVT0TGTADD field.
 */
__INLINE void agc_rwnxagcevtsat_pack(uint8_t evt0op3, uint8_t evt0op2, uint8_t evt0op1, uint8_t evt0pathcomb, uint8_t evt0opcomb, uint16_t evt0tgtadd)
{
    ASSERT_ERR((((uint32_t)evt0op3 << 26) & ~((uint32_t)0xFC000000)) == 0);
    ASSERT_ERR((((uint32_t)evt0op2 << 20) & ~((uint32_t)0x03F00000)) == 0);
    ASSERT_ERR((((uint32_t)evt0op1 << 14) & ~((uint32_t)0x000FC000)) == 0);
    ASSERT_ERR((((uint32_t)evt0pathcomb << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)evt0opcomb << 11) & ~((uint32_t)0x00001800)) == 0);
    ASSERT_ERR((((uint32_t)evt0tgtadd << 0) & ~((uint32_t)0x000007FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTSAT_ADDR,  ((uint32_t)evt0op3 << 26) | ((uint32_t)evt0op2 << 20) | ((uint32_t)evt0op1 << 14) | ((uint32_t)evt0pathcomb << 13) | ((uint32_t)evt0opcomb << 11) | ((uint32_t)evt0tgtadd << 0));
}

/**
 * @brief Unpacks RWNXAGCEVTSAT's fields from current value of the RWNXAGCEVTSAT register.
 *
 * Reads the RWNXAGCEVTSAT register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] evt0op3 - Will be populated with the current value of this field from the register.
 * @param[out] evt0op2 - Will be populated with the current value of this field from the register.
 * @param[out] evt0op1 - Will be populated with the current value of this field from the register.
 * @param[out] evt0pathcomb - Will be populated with the current value of this field from the register.
 * @param[out] evt0opcomb - Will be populated with the current value of this field from the register.
 * @param[out] evt0tgtadd - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcevtsat_unpack(uint8_t *evt0op3, uint8_t *evt0op2, uint8_t *evt0op1, uint8_t *evt0pathcomb, uint8_t *evt0opcomb, uint16_t *evt0tgtadd)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTSAT_ADDR);

    *evt0op3 = (localVal & ((uint32_t)0xFC000000)) >> 26;
    *evt0op2 = (localVal & ((uint32_t)0x03F00000)) >> 20;
    *evt0op1 = (localVal & ((uint32_t)0x000FC000)) >> 14;
    *evt0pathcomb = (localVal & ((uint32_t)0x00002000)) >> 13;
    *evt0opcomb = (localVal & ((uint32_t)0x00001800)) >> 11;
    *evt0tgtadd = (localVal & ((uint32_t)0x000007FF)) >> 0;
}

/**
 * @brief Returns the current value of the EVT0OP3 field in the RWNXAGCEVTSAT register.
 *
 * The RWNXAGCEVTSAT register will be read and the EVT0OP3 field's value will be returned.
 *
 * @return The current value of the EVT0OP3 field in the RWNXAGCEVTSAT register.
 */
__INLINE uint8_t agc_evt0op3_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTSAT_ADDR);
    return ((localVal & ((uint32_t)0xFC000000)) >> 26);
}

/**
 * @brief Sets the EVT0OP3 field of the RWNXAGCEVTSAT register.
 *
 * The RWNXAGCEVTSAT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt0op3 - The value to set the field to.
 */
__INLINE void agc_evt0op3_setf(uint8_t evt0op3)
{
    ASSERT_ERR((((uint32_t)evt0op3 << 26) & ~((uint32_t)0xFC000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTSAT_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTSAT_ADDR) & ~((uint32_t)0xFC000000)) | ((uint32_t)evt0op3 << 26));
}

/**
 * @brief Returns the current value of the EVT0OP2 field in the RWNXAGCEVTSAT register.
 *
 * The RWNXAGCEVTSAT register will be read and the EVT0OP2 field's value will be returned.
 *
 * @return The current value of the EVT0OP2 field in the RWNXAGCEVTSAT register.
 */
__INLINE uint8_t agc_evt0op2_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTSAT_ADDR);
    return ((localVal & ((uint32_t)0x03F00000)) >> 20);
}

/**
 * @brief Sets the EVT0OP2 field of the RWNXAGCEVTSAT register.
 *
 * The RWNXAGCEVTSAT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt0op2 - The value to set the field to.
 */
__INLINE void agc_evt0op2_setf(uint8_t evt0op2)
{
    ASSERT_ERR((((uint32_t)evt0op2 << 20) & ~((uint32_t)0x03F00000)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTSAT_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTSAT_ADDR) & ~((uint32_t)0x03F00000)) | ((uint32_t)evt0op2 << 20));
}

/**
 * @brief Returns the current value of the EVT0OP1 field in the RWNXAGCEVTSAT register.
 *
 * The RWNXAGCEVTSAT register will be read and the EVT0OP1 field's value will be returned.
 *
 * @return The current value of the EVT0OP1 field in the RWNXAGCEVTSAT register.
 */
__INLINE uint8_t agc_evt0op1_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTSAT_ADDR);
    return ((localVal & ((uint32_t)0x000FC000)) >> 14);
}

/**
 * @brief Sets the EVT0OP1 field of the RWNXAGCEVTSAT register.
 *
 * The RWNXAGCEVTSAT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt0op1 - The value to set the field to.
 */
__INLINE void agc_evt0op1_setf(uint8_t evt0op1)
{
    ASSERT_ERR((((uint32_t)evt0op1 << 14) & ~((uint32_t)0x000FC000)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTSAT_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTSAT_ADDR) & ~((uint32_t)0x000FC000)) | ((uint32_t)evt0op1 << 14));
}

/**
 * @brief Returns the current value of the EVT0PATHCOMB field in the RWNXAGCEVTSAT register.
 *
 * The RWNXAGCEVTSAT register will be read and the EVT0PATHCOMB field's value will be returned.
 *
 * @return The current value of the EVT0PATHCOMB field in the RWNXAGCEVTSAT register.
 */
__INLINE uint8_t agc_evt0pathcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTSAT_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the EVT0PATHCOMB field of the RWNXAGCEVTSAT register.
 *
 * The RWNXAGCEVTSAT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt0pathcomb - The value to set the field to.
 */
__INLINE void agc_evt0pathcomb_setf(uint8_t evt0pathcomb)
{
    ASSERT_ERR((((uint32_t)evt0pathcomb << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTSAT_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTSAT_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)evt0pathcomb << 13));
}

/**
 * @brief Returns the current value of the EVT0OPCOMB field in the RWNXAGCEVTSAT register.
 *
 * The RWNXAGCEVTSAT register will be read and the EVT0OPCOMB field's value will be returned.
 *
 * @return The current value of the EVT0OPCOMB field in the RWNXAGCEVTSAT register.
 */
__INLINE uint8_t agc_evt0opcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTSAT_ADDR);
    return ((localVal & ((uint32_t)0x00001800)) >> 11);
}

/**
 * @brief Sets the EVT0OPCOMB field of the RWNXAGCEVTSAT register.
 *
 * The RWNXAGCEVTSAT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt0opcomb - The value to set the field to.
 */
__INLINE void agc_evt0opcomb_setf(uint8_t evt0opcomb)
{
    ASSERT_ERR((((uint32_t)evt0opcomb << 11) & ~((uint32_t)0x00001800)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTSAT_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTSAT_ADDR) & ~((uint32_t)0x00001800)) | ((uint32_t)evt0opcomb << 11));
}

/**
 * @brief Returns the current value of the EVT0TGTADD field in the RWNXAGCEVTSAT register.
 *
 * The RWNXAGCEVTSAT register will be read and the EVT0TGTADD field's value will be returned.
 *
 * @return The current value of the EVT0TGTADD field in the RWNXAGCEVTSAT register.
 */
__INLINE uint16_t agc_evt0tgtadd_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTSAT_ADDR);
    return ((localVal & ((uint32_t)0x000007FF)) >> 0);
}

/**
 * @brief Sets the EVT0TGTADD field of the RWNXAGCEVTSAT register.
 *
 * The RWNXAGCEVTSAT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt0tgtadd - The value to set the field to.
 */
__INLINE void agc_evt0tgtadd_setf(uint16_t evt0tgtadd)
{
    ASSERT_ERR((((uint32_t)evt0tgtadd << 0) & ~((uint32_t)0x000007FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTSAT_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTSAT_ADDR) & ~((uint32_t)0x000007FF)) | ((uint32_t)evt0tgtadd << 0));
}

/// @}

/**
 * @name RWNXAGCEVTDET register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:26              EVT1OP3   0xF
 *  25:20              EVT1OP2   0x14
 *  19:14              EVT1OP1   0x12
 *     13         EVT1PATHCOMB   0
 *  12:11           EVT1OPCOMB   0x2
 *  10:00           EVT1TGTADD   0x8
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCEVTDET register
#define AGC_RWNXAGCEVTDET_ADDR   0x01002054
/// Offset of the RWNXAGCEVTDET register from the base address
#define AGC_RWNXAGCEVTDET_OFFSET 0x00002054
/// Index of the RWNXAGCEVTDET register
#define AGC_RWNXAGCEVTDET_INDEX  0x00000815
/// Reset value of the RWNXAGCEVTDET register
#define AGC_RWNXAGCEVTDET_RESET  0x3D449008

/**
 * @brief Returns the current value of the RWNXAGCEVTDET register.
 * The RWNXAGCEVTDET register will be read and its value returned.
 * @return The current value of the RWNXAGCEVTDET register.
 */
__INLINE uint32_t agc_rwnxagcevtdet_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCEVTDET_ADDR);
}

/**
 * @brief Sets the RWNXAGCEVTDET register to a value.
 * The RWNXAGCEVTDET register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcevtdet_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCEVTDET_ADDR, value);
}

// field definitions
/// EVT1OP3 field mask
#define AGC_EVT1OP3_MASK        ((uint32_t)0xFC000000)
/// EVT1OP3 field LSB position
#define AGC_EVT1OP3_LSB         26
/// EVT1OP3 field width
#define AGC_EVT1OP3_WIDTH       ((uint32_t)0x00000006)
/// EVT1OP2 field mask
#define AGC_EVT1OP2_MASK        ((uint32_t)0x03F00000)
/// EVT1OP2 field LSB position
#define AGC_EVT1OP2_LSB         20
/// EVT1OP2 field width
#define AGC_EVT1OP2_WIDTH       ((uint32_t)0x00000006)
/// EVT1OP1 field mask
#define AGC_EVT1OP1_MASK        ((uint32_t)0x000FC000)
/// EVT1OP1 field LSB position
#define AGC_EVT1OP1_LSB         14
/// EVT1OP1 field width
#define AGC_EVT1OP1_WIDTH       ((uint32_t)0x00000006)
/// EVT1PATHCOMB field bit
#define AGC_EVT1PATHCOMB_BIT    ((uint32_t)0x00002000)
/// EVT1PATHCOMB field position
#define AGC_EVT1PATHCOMB_POS    13
/// EVT1OPCOMB field mask
#define AGC_EVT1OPCOMB_MASK     ((uint32_t)0x00001800)
/// EVT1OPCOMB field LSB position
#define AGC_EVT1OPCOMB_LSB      11
/// EVT1OPCOMB field width
#define AGC_EVT1OPCOMB_WIDTH    ((uint32_t)0x00000002)
/// EVT1TGTADD field mask
#define AGC_EVT1TGTADD_MASK     ((uint32_t)0x000007FF)
/// EVT1TGTADD field LSB position
#define AGC_EVT1TGTADD_LSB      0
/// EVT1TGTADD field width
#define AGC_EVT1TGTADD_WIDTH    ((uint32_t)0x0000000B)

/// EVT1OP3 field reset value
#define AGC_EVT1OP3_RST         0xF
/// EVT1OP2 field reset value
#define AGC_EVT1OP2_RST         0x14
/// EVT1OP1 field reset value
#define AGC_EVT1OP1_RST         0x12
/// EVT1PATHCOMB field reset value
#define AGC_EVT1PATHCOMB_RST    0x0
/// EVT1OPCOMB field reset value
#define AGC_EVT1OPCOMB_RST      0x2
/// EVT1TGTADD field reset value
#define AGC_EVT1TGTADD_RST      0x8

/**
 * @brief Constructs a value for the RWNXAGCEVTDET register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] evt1op3 - The value to use for the EVT1OP3 field.
 * @param[in] evt1op2 - The value to use for the EVT1OP2 field.
 * @param[in] evt1op1 - The value to use for the EVT1OP1 field.
 * @param[in] evt1pathcomb - The value to use for the EVT1PATHCOMB field.
 * @param[in] evt1opcomb - The value to use for the EVT1OPCOMB field.
 * @param[in] evt1tgtadd - The value to use for the EVT1TGTADD field.
 */
__INLINE void agc_rwnxagcevtdet_pack(uint8_t evt1op3, uint8_t evt1op2, uint8_t evt1op1, uint8_t evt1pathcomb, uint8_t evt1opcomb, uint16_t evt1tgtadd)
{
    ASSERT_ERR((((uint32_t)evt1op3 << 26) & ~((uint32_t)0xFC000000)) == 0);
    ASSERT_ERR((((uint32_t)evt1op2 << 20) & ~((uint32_t)0x03F00000)) == 0);
    ASSERT_ERR((((uint32_t)evt1op1 << 14) & ~((uint32_t)0x000FC000)) == 0);
    ASSERT_ERR((((uint32_t)evt1pathcomb << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)evt1opcomb << 11) & ~((uint32_t)0x00001800)) == 0);
    ASSERT_ERR((((uint32_t)evt1tgtadd << 0) & ~((uint32_t)0x000007FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDET_ADDR,  ((uint32_t)evt1op3 << 26) | ((uint32_t)evt1op2 << 20) | ((uint32_t)evt1op1 << 14) | ((uint32_t)evt1pathcomb << 13) | ((uint32_t)evt1opcomb << 11) | ((uint32_t)evt1tgtadd << 0));
}

/**
 * @brief Unpacks RWNXAGCEVTDET's fields from current value of the RWNXAGCEVTDET register.
 *
 * Reads the RWNXAGCEVTDET register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] evt1op3 - Will be populated with the current value of this field from the register.
 * @param[out] evt1op2 - Will be populated with the current value of this field from the register.
 * @param[out] evt1op1 - Will be populated with the current value of this field from the register.
 * @param[out] evt1pathcomb - Will be populated with the current value of this field from the register.
 * @param[out] evt1opcomb - Will be populated with the current value of this field from the register.
 * @param[out] evt1tgtadd - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcevtdet_unpack(uint8_t *evt1op3, uint8_t *evt1op2, uint8_t *evt1op1, uint8_t *evt1pathcomb, uint8_t *evt1opcomb, uint16_t *evt1tgtadd)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDET_ADDR);

    *evt1op3 = (localVal & ((uint32_t)0xFC000000)) >> 26;
    *evt1op2 = (localVal & ((uint32_t)0x03F00000)) >> 20;
    *evt1op1 = (localVal & ((uint32_t)0x000FC000)) >> 14;
    *evt1pathcomb = (localVal & ((uint32_t)0x00002000)) >> 13;
    *evt1opcomb = (localVal & ((uint32_t)0x00001800)) >> 11;
    *evt1tgtadd = (localVal & ((uint32_t)0x000007FF)) >> 0;
}

/**
 * @brief Returns the current value of the EVT1OP3 field in the RWNXAGCEVTDET register.
 *
 * The RWNXAGCEVTDET register will be read and the EVT1OP3 field's value will be returned.
 *
 * @return The current value of the EVT1OP3 field in the RWNXAGCEVTDET register.
 */
__INLINE uint8_t agc_evt1op3_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDET_ADDR);
    return ((localVal & ((uint32_t)0xFC000000)) >> 26);
}

/**
 * @brief Sets the EVT1OP3 field of the RWNXAGCEVTDET register.
 *
 * The RWNXAGCEVTDET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt1op3 - The value to set the field to.
 */
__INLINE void agc_evt1op3_setf(uint8_t evt1op3)
{
    ASSERT_ERR((((uint32_t)evt1op3 << 26) & ~((uint32_t)0xFC000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDET_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTDET_ADDR) & ~((uint32_t)0xFC000000)) | ((uint32_t)evt1op3 << 26));
}

/**
 * @brief Returns the current value of the EVT1OP2 field in the RWNXAGCEVTDET register.
 *
 * The RWNXAGCEVTDET register will be read and the EVT1OP2 field's value will be returned.
 *
 * @return The current value of the EVT1OP2 field in the RWNXAGCEVTDET register.
 */
__INLINE uint8_t agc_evt1op2_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDET_ADDR);
    return ((localVal & ((uint32_t)0x03F00000)) >> 20);
}

/**
 * @brief Sets the EVT1OP2 field of the RWNXAGCEVTDET register.
 *
 * The RWNXAGCEVTDET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt1op2 - The value to set the field to.
 */
__INLINE void agc_evt1op2_setf(uint8_t evt1op2)
{
    ASSERT_ERR((((uint32_t)evt1op2 << 20) & ~((uint32_t)0x03F00000)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDET_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTDET_ADDR) & ~((uint32_t)0x03F00000)) | ((uint32_t)evt1op2 << 20));
}

/**
 * @brief Returns the current value of the EVT1OP1 field in the RWNXAGCEVTDET register.
 *
 * The RWNXAGCEVTDET register will be read and the EVT1OP1 field's value will be returned.
 *
 * @return The current value of the EVT1OP1 field in the RWNXAGCEVTDET register.
 */
__INLINE uint8_t agc_evt1op1_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDET_ADDR);
    return ((localVal & ((uint32_t)0x000FC000)) >> 14);
}

/**
 * @brief Sets the EVT1OP1 field of the RWNXAGCEVTDET register.
 *
 * The RWNXAGCEVTDET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt1op1 - The value to set the field to.
 */
__INLINE void agc_evt1op1_setf(uint8_t evt1op1)
{
    ASSERT_ERR((((uint32_t)evt1op1 << 14) & ~((uint32_t)0x000FC000)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDET_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTDET_ADDR) & ~((uint32_t)0x000FC000)) | ((uint32_t)evt1op1 << 14));
}

/**
 * @brief Returns the current value of the EVT1PATHCOMB field in the RWNXAGCEVTDET register.
 *
 * The RWNXAGCEVTDET register will be read and the EVT1PATHCOMB field's value will be returned.
 *
 * @return The current value of the EVT1PATHCOMB field in the RWNXAGCEVTDET register.
 */
__INLINE uint8_t agc_evt1pathcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDET_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the EVT1PATHCOMB field of the RWNXAGCEVTDET register.
 *
 * The RWNXAGCEVTDET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt1pathcomb - The value to set the field to.
 */
__INLINE void agc_evt1pathcomb_setf(uint8_t evt1pathcomb)
{
    ASSERT_ERR((((uint32_t)evt1pathcomb << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDET_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTDET_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)evt1pathcomb << 13));
}

/**
 * @brief Returns the current value of the EVT1OPCOMB field in the RWNXAGCEVTDET register.
 *
 * The RWNXAGCEVTDET register will be read and the EVT1OPCOMB field's value will be returned.
 *
 * @return The current value of the EVT1OPCOMB field in the RWNXAGCEVTDET register.
 */
__INLINE uint8_t agc_evt1opcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDET_ADDR);
    return ((localVal & ((uint32_t)0x00001800)) >> 11);
}

/**
 * @brief Sets the EVT1OPCOMB field of the RWNXAGCEVTDET register.
 *
 * The RWNXAGCEVTDET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt1opcomb - The value to set the field to.
 */
__INLINE void agc_evt1opcomb_setf(uint8_t evt1opcomb)
{
    ASSERT_ERR((((uint32_t)evt1opcomb << 11) & ~((uint32_t)0x00001800)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDET_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTDET_ADDR) & ~((uint32_t)0x00001800)) | ((uint32_t)evt1opcomb << 11));
}

/**
 * @brief Returns the current value of the EVT1TGTADD field in the RWNXAGCEVTDET register.
 *
 * The RWNXAGCEVTDET register will be read and the EVT1TGTADD field's value will be returned.
 *
 * @return The current value of the EVT1TGTADD field in the RWNXAGCEVTDET register.
 */
__INLINE uint16_t agc_evt1tgtadd_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDET_ADDR);
    return ((localVal & ((uint32_t)0x000007FF)) >> 0);
}

/**
 * @brief Sets the EVT1TGTADD field of the RWNXAGCEVTDET register.
 *
 * The RWNXAGCEVTDET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt1tgtadd - The value to set the field to.
 */
__INLINE void agc_evt1tgtadd_setf(uint16_t evt1tgtadd)
{
    ASSERT_ERR((((uint32_t)evt1tgtadd << 0) & ~((uint32_t)0x000007FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDET_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTDET_ADDR) & ~((uint32_t)0x000007FF)) | ((uint32_t)evt1tgtadd << 0));
}

/// @}

/**
 * @name RWNXAGCEVTDIS register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:26              EVT2OP3   0xE
 *  25:20              EVT2OP2   0x15
 *  19:14              EVT2OP1   0x16
 *     13         EVT2PATHCOMB   0
 *  12:11           EVT2OPCOMB   0x2
 *  10:00           EVT2TGTADD   0xB
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCEVTDIS register
#define AGC_RWNXAGCEVTDIS_ADDR   0x01002058
/// Offset of the RWNXAGCEVTDIS register from the base address
#define AGC_RWNXAGCEVTDIS_OFFSET 0x00002058
/// Index of the RWNXAGCEVTDIS register
#define AGC_RWNXAGCEVTDIS_INDEX  0x00000816
/// Reset value of the RWNXAGCEVTDIS register
#define AGC_RWNXAGCEVTDIS_RESET  0x3955900B

/**
 * @brief Returns the current value of the RWNXAGCEVTDIS register.
 * The RWNXAGCEVTDIS register will be read and its value returned.
 * @return The current value of the RWNXAGCEVTDIS register.
 */
__INLINE uint32_t agc_rwnxagcevtdis_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCEVTDIS_ADDR);
}

/**
 * @brief Sets the RWNXAGCEVTDIS register to a value.
 * The RWNXAGCEVTDIS register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcevtdis_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCEVTDIS_ADDR, value);
}

// field definitions
/// EVT2OP3 field mask
#define AGC_EVT2OP3_MASK        ((uint32_t)0xFC000000)
/// EVT2OP3 field LSB position
#define AGC_EVT2OP3_LSB         26
/// EVT2OP3 field width
#define AGC_EVT2OP3_WIDTH       ((uint32_t)0x00000006)
/// EVT2OP2 field mask
#define AGC_EVT2OP2_MASK        ((uint32_t)0x03F00000)
/// EVT2OP2 field LSB position
#define AGC_EVT2OP2_LSB         20
/// EVT2OP2 field width
#define AGC_EVT2OP2_WIDTH       ((uint32_t)0x00000006)
/// EVT2OP1 field mask
#define AGC_EVT2OP1_MASK        ((uint32_t)0x000FC000)
/// EVT2OP1 field LSB position
#define AGC_EVT2OP1_LSB         14
/// EVT2OP1 field width
#define AGC_EVT2OP1_WIDTH       ((uint32_t)0x00000006)
/// EVT2PATHCOMB field bit
#define AGC_EVT2PATHCOMB_BIT    ((uint32_t)0x00002000)
/// EVT2PATHCOMB field position
#define AGC_EVT2PATHCOMB_POS    13
/// EVT2OPCOMB field mask
#define AGC_EVT2OPCOMB_MASK     ((uint32_t)0x00001800)
/// EVT2OPCOMB field LSB position
#define AGC_EVT2OPCOMB_LSB      11
/// EVT2OPCOMB field width
#define AGC_EVT2OPCOMB_WIDTH    ((uint32_t)0x00000002)
/// EVT2TGTADD field mask
#define AGC_EVT2TGTADD_MASK     ((uint32_t)0x000007FF)
/// EVT2TGTADD field LSB position
#define AGC_EVT2TGTADD_LSB      0
/// EVT2TGTADD field width
#define AGC_EVT2TGTADD_WIDTH    ((uint32_t)0x0000000B)

/// EVT2OP3 field reset value
#define AGC_EVT2OP3_RST         0xE
/// EVT2OP2 field reset value
#define AGC_EVT2OP2_RST         0x15
/// EVT2OP1 field reset value
#define AGC_EVT2OP1_RST         0x16
/// EVT2PATHCOMB field reset value
#define AGC_EVT2PATHCOMB_RST    0x0
/// EVT2OPCOMB field reset value
#define AGC_EVT2OPCOMB_RST      0x2
/// EVT2TGTADD field reset value
#define AGC_EVT2TGTADD_RST      0xB

/**
 * @brief Constructs a value for the RWNXAGCEVTDIS register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] evt2op3 - The value to use for the EVT2OP3 field.
 * @param[in] evt2op2 - The value to use for the EVT2OP2 field.
 * @param[in] evt2op1 - The value to use for the EVT2OP1 field.
 * @param[in] evt2pathcomb - The value to use for the EVT2PATHCOMB field.
 * @param[in] evt2opcomb - The value to use for the EVT2OPCOMB field.
 * @param[in] evt2tgtadd - The value to use for the EVT2TGTADD field.
 */
__INLINE void agc_rwnxagcevtdis_pack(uint8_t evt2op3, uint8_t evt2op2, uint8_t evt2op1, uint8_t evt2pathcomb, uint8_t evt2opcomb, uint16_t evt2tgtadd)
{
    ASSERT_ERR((((uint32_t)evt2op3 << 26) & ~((uint32_t)0xFC000000)) == 0);
    ASSERT_ERR((((uint32_t)evt2op2 << 20) & ~((uint32_t)0x03F00000)) == 0);
    ASSERT_ERR((((uint32_t)evt2op1 << 14) & ~((uint32_t)0x000FC000)) == 0);
    ASSERT_ERR((((uint32_t)evt2pathcomb << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)evt2opcomb << 11) & ~((uint32_t)0x00001800)) == 0);
    ASSERT_ERR((((uint32_t)evt2tgtadd << 0) & ~((uint32_t)0x000007FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDIS_ADDR,  ((uint32_t)evt2op3 << 26) | ((uint32_t)evt2op2 << 20) | ((uint32_t)evt2op1 << 14) | ((uint32_t)evt2pathcomb << 13) | ((uint32_t)evt2opcomb << 11) | ((uint32_t)evt2tgtadd << 0));
}

/**
 * @brief Unpacks RWNXAGCEVTDIS's fields from current value of the RWNXAGCEVTDIS register.
 *
 * Reads the RWNXAGCEVTDIS register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] evt2op3 - Will be populated with the current value of this field from the register.
 * @param[out] evt2op2 - Will be populated with the current value of this field from the register.
 * @param[out] evt2op1 - Will be populated with the current value of this field from the register.
 * @param[out] evt2pathcomb - Will be populated with the current value of this field from the register.
 * @param[out] evt2opcomb - Will be populated with the current value of this field from the register.
 * @param[out] evt2tgtadd - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcevtdis_unpack(uint8_t *evt2op3, uint8_t *evt2op2, uint8_t *evt2op1, uint8_t *evt2pathcomb, uint8_t *evt2opcomb, uint16_t *evt2tgtadd)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDIS_ADDR);

    *evt2op3 = (localVal & ((uint32_t)0xFC000000)) >> 26;
    *evt2op2 = (localVal & ((uint32_t)0x03F00000)) >> 20;
    *evt2op1 = (localVal & ((uint32_t)0x000FC000)) >> 14;
    *evt2pathcomb = (localVal & ((uint32_t)0x00002000)) >> 13;
    *evt2opcomb = (localVal & ((uint32_t)0x00001800)) >> 11;
    *evt2tgtadd = (localVal & ((uint32_t)0x000007FF)) >> 0;
}

/**
 * @brief Returns the current value of the EVT2OP3 field in the RWNXAGCEVTDIS register.
 *
 * The RWNXAGCEVTDIS register will be read and the EVT2OP3 field's value will be returned.
 *
 * @return The current value of the EVT2OP3 field in the RWNXAGCEVTDIS register.
 */
__INLINE uint8_t agc_evt2op3_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDIS_ADDR);
    return ((localVal & ((uint32_t)0xFC000000)) >> 26);
}

/**
 * @brief Sets the EVT2OP3 field of the RWNXAGCEVTDIS register.
 *
 * The RWNXAGCEVTDIS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt2op3 - The value to set the field to.
 */
__INLINE void agc_evt2op3_setf(uint8_t evt2op3)
{
    ASSERT_ERR((((uint32_t)evt2op3 << 26) & ~((uint32_t)0xFC000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDIS_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTDIS_ADDR) & ~((uint32_t)0xFC000000)) | ((uint32_t)evt2op3 << 26));
}

/**
 * @brief Returns the current value of the EVT2OP2 field in the RWNXAGCEVTDIS register.
 *
 * The RWNXAGCEVTDIS register will be read and the EVT2OP2 field's value will be returned.
 *
 * @return The current value of the EVT2OP2 field in the RWNXAGCEVTDIS register.
 */
__INLINE uint8_t agc_evt2op2_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDIS_ADDR);
    return ((localVal & ((uint32_t)0x03F00000)) >> 20);
}

/**
 * @brief Sets the EVT2OP2 field of the RWNXAGCEVTDIS register.
 *
 * The RWNXAGCEVTDIS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt2op2 - The value to set the field to.
 */
__INLINE void agc_evt2op2_setf(uint8_t evt2op2)
{
    ASSERT_ERR((((uint32_t)evt2op2 << 20) & ~((uint32_t)0x03F00000)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDIS_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTDIS_ADDR) & ~((uint32_t)0x03F00000)) | ((uint32_t)evt2op2 << 20));
}

/**
 * @brief Returns the current value of the EVT2OP1 field in the RWNXAGCEVTDIS register.
 *
 * The RWNXAGCEVTDIS register will be read and the EVT2OP1 field's value will be returned.
 *
 * @return The current value of the EVT2OP1 field in the RWNXAGCEVTDIS register.
 */
__INLINE uint8_t agc_evt2op1_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDIS_ADDR);
    return ((localVal & ((uint32_t)0x000FC000)) >> 14);
}

/**
 * @brief Sets the EVT2OP1 field of the RWNXAGCEVTDIS register.
 *
 * The RWNXAGCEVTDIS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt2op1 - The value to set the field to.
 */
__INLINE void agc_evt2op1_setf(uint8_t evt2op1)
{
    ASSERT_ERR((((uint32_t)evt2op1 << 14) & ~((uint32_t)0x000FC000)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDIS_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTDIS_ADDR) & ~((uint32_t)0x000FC000)) | ((uint32_t)evt2op1 << 14));
}

/**
 * @brief Returns the current value of the EVT2PATHCOMB field in the RWNXAGCEVTDIS register.
 *
 * The RWNXAGCEVTDIS register will be read and the EVT2PATHCOMB field's value will be returned.
 *
 * @return The current value of the EVT2PATHCOMB field in the RWNXAGCEVTDIS register.
 */
__INLINE uint8_t agc_evt2pathcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDIS_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the EVT2PATHCOMB field of the RWNXAGCEVTDIS register.
 *
 * The RWNXAGCEVTDIS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt2pathcomb - The value to set the field to.
 */
__INLINE void agc_evt2pathcomb_setf(uint8_t evt2pathcomb)
{
    ASSERT_ERR((((uint32_t)evt2pathcomb << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDIS_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTDIS_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)evt2pathcomb << 13));
}

/**
 * @brief Returns the current value of the EVT2OPCOMB field in the RWNXAGCEVTDIS register.
 *
 * The RWNXAGCEVTDIS register will be read and the EVT2OPCOMB field's value will be returned.
 *
 * @return The current value of the EVT2OPCOMB field in the RWNXAGCEVTDIS register.
 */
__INLINE uint8_t agc_evt2opcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDIS_ADDR);
    return ((localVal & ((uint32_t)0x00001800)) >> 11);
}

/**
 * @brief Sets the EVT2OPCOMB field of the RWNXAGCEVTDIS register.
 *
 * The RWNXAGCEVTDIS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt2opcomb - The value to set the field to.
 */
__INLINE void agc_evt2opcomb_setf(uint8_t evt2opcomb)
{
    ASSERT_ERR((((uint32_t)evt2opcomb << 11) & ~((uint32_t)0x00001800)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDIS_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTDIS_ADDR) & ~((uint32_t)0x00001800)) | ((uint32_t)evt2opcomb << 11));
}

/**
 * @brief Returns the current value of the EVT2TGTADD field in the RWNXAGCEVTDIS register.
 *
 * The RWNXAGCEVTDIS register will be read and the EVT2TGTADD field's value will be returned.
 *
 * @return The current value of the EVT2TGTADD field in the RWNXAGCEVTDIS register.
 */
__INLINE uint16_t agc_evt2tgtadd_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDIS_ADDR);
    return ((localVal & ((uint32_t)0x000007FF)) >> 0);
}

/**
 * @brief Sets the EVT2TGTADD field of the RWNXAGCEVTDIS register.
 *
 * The RWNXAGCEVTDIS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt2tgtadd - The value to set the field to.
 */
__INLINE void agc_evt2tgtadd_setf(uint16_t evt2tgtadd)
{
    ASSERT_ERR((((uint32_t)evt2tgtadd << 0) & ~((uint32_t)0x000007FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDIS_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTDIS_ADDR) & ~((uint32_t)0x000007FF)) | ((uint32_t)evt2tgtadd << 0));
}

/// @}

/**
 * @name RWNXAGCEVTDSSSDET register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:26              EVT3OP3   0x1
 *  25:20              EVT3OP2   0xF
 *  19:14              EVT3OP1   0x1D
 *     13         EVT3PATHCOMB   0
 *  12:11           EVT3OPCOMB   0x1
 *  10:00           EVT3TGTADD   0xF
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCEVTDSSSDET register
#define AGC_RWNXAGCEVTDSSSDET_ADDR   0x0100205C
/// Offset of the RWNXAGCEVTDSSSDET register from the base address
#define AGC_RWNXAGCEVTDSSSDET_OFFSET 0x0000205C
/// Index of the RWNXAGCEVTDSSSDET register
#define AGC_RWNXAGCEVTDSSSDET_INDEX  0x00000817
/// Reset value of the RWNXAGCEVTDSSSDET register
#define AGC_RWNXAGCEVTDSSSDET_RESET  0x04F7480F

/**
 * @brief Returns the current value of the RWNXAGCEVTDSSSDET register.
 * The RWNXAGCEVTDSSSDET register will be read and its value returned.
 * @return The current value of the RWNXAGCEVTDSSSDET register.
 */
__INLINE uint32_t agc_rwnxagcevtdsssdet_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCEVTDSSSDET_ADDR);
}

/**
 * @brief Sets the RWNXAGCEVTDSSSDET register to a value.
 * The RWNXAGCEVTDSSSDET register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcevtdsssdet_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCEVTDSSSDET_ADDR, value);
}

// field definitions
/// EVT3OP3 field mask
#define AGC_EVT3OP3_MASK        ((uint32_t)0xFC000000)
/// EVT3OP3 field LSB position
#define AGC_EVT3OP3_LSB         26
/// EVT3OP3 field width
#define AGC_EVT3OP3_WIDTH       ((uint32_t)0x00000006)
/// EVT3OP2 field mask
#define AGC_EVT3OP2_MASK        ((uint32_t)0x03F00000)
/// EVT3OP2 field LSB position
#define AGC_EVT3OP2_LSB         20
/// EVT3OP2 field width
#define AGC_EVT3OP2_WIDTH       ((uint32_t)0x00000006)
/// EVT3OP1 field mask
#define AGC_EVT3OP1_MASK        ((uint32_t)0x000FC000)
/// EVT3OP1 field LSB position
#define AGC_EVT3OP1_LSB         14
/// EVT3OP1 field width
#define AGC_EVT3OP1_WIDTH       ((uint32_t)0x00000006)
/// EVT3PATHCOMB field bit
#define AGC_EVT3PATHCOMB_BIT    ((uint32_t)0x00002000)
/// EVT3PATHCOMB field position
#define AGC_EVT3PATHCOMB_POS    13
/// EVT3OPCOMB field mask
#define AGC_EVT3OPCOMB_MASK     ((uint32_t)0x00001800)
/// EVT3OPCOMB field LSB position
#define AGC_EVT3OPCOMB_LSB      11
/// EVT3OPCOMB field width
#define AGC_EVT3OPCOMB_WIDTH    ((uint32_t)0x00000002)
/// EVT3TGTADD field mask
#define AGC_EVT3TGTADD_MASK     ((uint32_t)0x000007FF)
/// EVT3TGTADD field LSB position
#define AGC_EVT3TGTADD_LSB      0
/// EVT3TGTADD field width
#define AGC_EVT3TGTADD_WIDTH    ((uint32_t)0x0000000B)

/// EVT3OP3 field reset value
#define AGC_EVT3OP3_RST         0x1
/// EVT3OP2 field reset value
#define AGC_EVT3OP2_RST         0xF
/// EVT3OP1 field reset value
#define AGC_EVT3OP1_RST         0x1D
/// EVT3PATHCOMB field reset value
#define AGC_EVT3PATHCOMB_RST    0x0
/// EVT3OPCOMB field reset value
#define AGC_EVT3OPCOMB_RST      0x1
/// EVT3TGTADD field reset value
#define AGC_EVT3TGTADD_RST      0xF

/**
 * @brief Constructs a value for the RWNXAGCEVTDSSSDET register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] evt3op3 - The value to use for the EVT3OP3 field.
 * @param[in] evt3op2 - The value to use for the EVT3OP2 field.
 * @param[in] evt3op1 - The value to use for the EVT3OP1 field.
 * @param[in] evt3pathcomb - The value to use for the EVT3PATHCOMB field.
 * @param[in] evt3opcomb - The value to use for the EVT3OPCOMB field.
 * @param[in] evt3tgtadd - The value to use for the EVT3TGTADD field.
 */
__INLINE void agc_rwnxagcevtdsssdet_pack(uint8_t evt3op3, uint8_t evt3op2, uint8_t evt3op1, uint8_t evt3pathcomb, uint8_t evt3opcomb, uint16_t evt3tgtadd)
{
    ASSERT_ERR((((uint32_t)evt3op3 << 26) & ~((uint32_t)0xFC000000)) == 0);
    ASSERT_ERR((((uint32_t)evt3op2 << 20) & ~((uint32_t)0x03F00000)) == 0);
    ASSERT_ERR((((uint32_t)evt3op1 << 14) & ~((uint32_t)0x000FC000)) == 0);
    ASSERT_ERR((((uint32_t)evt3pathcomb << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)evt3opcomb << 11) & ~((uint32_t)0x00001800)) == 0);
    ASSERT_ERR((((uint32_t)evt3tgtadd << 0) & ~((uint32_t)0x000007FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDSSSDET_ADDR,  ((uint32_t)evt3op3 << 26) | ((uint32_t)evt3op2 << 20) | ((uint32_t)evt3op1 << 14) | ((uint32_t)evt3pathcomb << 13) | ((uint32_t)evt3opcomb << 11) | ((uint32_t)evt3tgtadd << 0));
}

/**
 * @brief Unpacks RWNXAGCEVTDSSSDET's fields from current value of the RWNXAGCEVTDSSSDET register.
 *
 * Reads the RWNXAGCEVTDSSSDET register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] evt3op3 - Will be populated with the current value of this field from the register.
 * @param[out] evt3op2 - Will be populated with the current value of this field from the register.
 * @param[out] evt3op1 - Will be populated with the current value of this field from the register.
 * @param[out] evt3pathcomb - Will be populated with the current value of this field from the register.
 * @param[out] evt3opcomb - Will be populated with the current value of this field from the register.
 * @param[out] evt3tgtadd - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcevtdsssdet_unpack(uint8_t *evt3op3, uint8_t *evt3op2, uint8_t *evt3op1, uint8_t *evt3pathcomb, uint8_t *evt3opcomb, uint16_t *evt3tgtadd)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDSSSDET_ADDR);

    *evt3op3 = (localVal & ((uint32_t)0xFC000000)) >> 26;
    *evt3op2 = (localVal & ((uint32_t)0x03F00000)) >> 20;
    *evt3op1 = (localVal & ((uint32_t)0x000FC000)) >> 14;
    *evt3pathcomb = (localVal & ((uint32_t)0x00002000)) >> 13;
    *evt3opcomb = (localVal & ((uint32_t)0x00001800)) >> 11;
    *evt3tgtadd = (localVal & ((uint32_t)0x000007FF)) >> 0;
}

/**
 * @brief Returns the current value of the EVT3OP3 field in the RWNXAGCEVTDSSSDET register.
 *
 * The RWNXAGCEVTDSSSDET register will be read and the EVT3OP3 field's value will be returned.
 *
 * @return The current value of the EVT3OP3 field in the RWNXAGCEVTDSSSDET register.
 */
__INLINE uint8_t agc_evt3op3_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDSSSDET_ADDR);
    return ((localVal & ((uint32_t)0xFC000000)) >> 26);
}

/**
 * @brief Sets the EVT3OP3 field of the RWNXAGCEVTDSSSDET register.
 *
 * The RWNXAGCEVTDSSSDET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt3op3 - The value to set the field to.
 */
__INLINE void agc_evt3op3_setf(uint8_t evt3op3)
{
    ASSERT_ERR((((uint32_t)evt3op3 << 26) & ~((uint32_t)0xFC000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDSSSDET_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTDSSSDET_ADDR) & ~((uint32_t)0xFC000000)) | ((uint32_t)evt3op3 << 26));
}

/**
 * @brief Returns the current value of the EVT3OP2 field in the RWNXAGCEVTDSSSDET register.
 *
 * The RWNXAGCEVTDSSSDET register will be read and the EVT3OP2 field's value will be returned.
 *
 * @return The current value of the EVT3OP2 field in the RWNXAGCEVTDSSSDET register.
 */
__INLINE uint8_t agc_evt3op2_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDSSSDET_ADDR);
    return ((localVal & ((uint32_t)0x03F00000)) >> 20);
}

/**
 * @brief Sets the EVT3OP2 field of the RWNXAGCEVTDSSSDET register.
 *
 * The RWNXAGCEVTDSSSDET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt3op2 - The value to set the field to.
 */
__INLINE void agc_evt3op2_setf(uint8_t evt3op2)
{
    ASSERT_ERR((((uint32_t)evt3op2 << 20) & ~((uint32_t)0x03F00000)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDSSSDET_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTDSSSDET_ADDR) & ~((uint32_t)0x03F00000)) | ((uint32_t)evt3op2 << 20));
}

/**
 * @brief Returns the current value of the EVT3OP1 field in the RWNXAGCEVTDSSSDET register.
 *
 * The RWNXAGCEVTDSSSDET register will be read and the EVT3OP1 field's value will be returned.
 *
 * @return The current value of the EVT3OP1 field in the RWNXAGCEVTDSSSDET register.
 */
__INLINE uint8_t agc_evt3op1_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDSSSDET_ADDR);
    return ((localVal & ((uint32_t)0x000FC000)) >> 14);
}

/**
 * @brief Sets the EVT3OP1 field of the RWNXAGCEVTDSSSDET register.
 *
 * The RWNXAGCEVTDSSSDET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt3op1 - The value to set the field to.
 */
__INLINE void agc_evt3op1_setf(uint8_t evt3op1)
{
    ASSERT_ERR((((uint32_t)evt3op1 << 14) & ~((uint32_t)0x000FC000)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDSSSDET_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTDSSSDET_ADDR) & ~((uint32_t)0x000FC000)) | ((uint32_t)evt3op1 << 14));
}

/**
 * @brief Returns the current value of the EVT3PATHCOMB field in the RWNXAGCEVTDSSSDET register.
 *
 * The RWNXAGCEVTDSSSDET register will be read and the EVT3PATHCOMB field's value will be returned.
 *
 * @return The current value of the EVT3PATHCOMB field in the RWNXAGCEVTDSSSDET register.
 */
__INLINE uint8_t agc_evt3pathcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDSSSDET_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the EVT3PATHCOMB field of the RWNXAGCEVTDSSSDET register.
 *
 * The RWNXAGCEVTDSSSDET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt3pathcomb - The value to set the field to.
 */
__INLINE void agc_evt3pathcomb_setf(uint8_t evt3pathcomb)
{
    ASSERT_ERR((((uint32_t)evt3pathcomb << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDSSSDET_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTDSSSDET_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)evt3pathcomb << 13));
}

/**
 * @brief Returns the current value of the EVT3OPCOMB field in the RWNXAGCEVTDSSSDET register.
 *
 * The RWNXAGCEVTDSSSDET register will be read and the EVT3OPCOMB field's value will be returned.
 *
 * @return The current value of the EVT3OPCOMB field in the RWNXAGCEVTDSSSDET register.
 */
__INLINE uint8_t agc_evt3opcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDSSSDET_ADDR);
    return ((localVal & ((uint32_t)0x00001800)) >> 11);
}

/**
 * @brief Sets the EVT3OPCOMB field of the RWNXAGCEVTDSSSDET register.
 *
 * The RWNXAGCEVTDSSSDET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt3opcomb - The value to set the field to.
 */
__INLINE void agc_evt3opcomb_setf(uint8_t evt3opcomb)
{
    ASSERT_ERR((((uint32_t)evt3opcomb << 11) & ~((uint32_t)0x00001800)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDSSSDET_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTDSSSDET_ADDR) & ~((uint32_t)0x00001800)) | ((uint32_t)evt3opcomb << 11));
}

/**
 * @brief Returns the current value of the EVT3TGTADD field in the RWNXAGCEVTDSSSDET register.
 *
 * The RWNXAGCEVTDSSSDET register will be read and the EVT3TGTADD field's value will be returned.
 *
 * @return The current value of the EVT3TGTADD field in the RWNXAGCEVTDSSSDET register.
 */
__INLINE uint16_t agc_evt3tgtadd_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCEVTDSSSDET_ADDR);
    return ((localVal & ((uint32_t)0x000007FF)) >> 0);
}

/**
 * @brief Sets the EVT3TGTADD field of the RWNXAGCEVTDSSSDET register.
 *
 * The RWNXAGCEVTDSSSDET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] evt3tgtadd - The value to set the field to.
 */
__INLINE void agc_evt3tgtadd_setf(uint16_t evt3tgtadd)
{
    ASSERT_ERR((((uint32_t)evt3tgtadd << 0) & ~((uint32_t)0x000007FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCEVTDSSSDET_ADDR, (REG_PL_RD(AGC_RWNXAGCEVTDSSSDET_ADDR) & ~((uint32_t)0x000007FF)) | ((uint32_t)evt3tgtadd << 0));
}

/// @}

/**
 * @name RWNXAGCCNTL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     16          HTSTFGAINEN   1
 *     12          AGCFSMRESET   0
 *     11           RADARDETEN   0
 *     10            RIFSDETEN   1
 *     09             DSSSONLY   0
 *     08             OFDMONLY   0
 *  07:04             GPSTATUS   0x0
 *  03:00          COMBPATHSEL   0xF
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCCNTL register
#define AGC_RWNXAGCCNTL_ADDR   0x01002060
/// Offset of the RWNXAGCCNTL register from the base address
#define AGC_RWNXAGCCNTL_OFFSET 0x00002060
/// Index of the RWNXAGCCNTL register
#define AGC_RWNXAGCCNTL_INDEX  0x00000818
/// Reset value of the RWNXAGCCNTL register
#define AGC_RWNXAGCCNTL_RESET  0x0001040F

/**
 * @brief Returns the current value of the RWNXAGCCNTL register.
 * The RWNXAGCCNTL register will be read and its value returned.
 * @return The current value of the RWNXAGCCNTL register.
 */
__INLINE uint32_t agc_rwnxagccntl_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCCNTL_ADDR);
}

/**
 * @brief Sets the RWNXAGCCNTL register to a value.
 * The RWNXAGCCNTL register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagccntl_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCCNTL_ADDR, value);
}

// field definitions
/// HTSTFGAINEN field bit
#define AGC_HTSTFGAINEN_BIT    ((uint32_t)0x00010000)
/// HTSTFGAINEN field position
#define AGC_HTSTFGAINEN_POS    16
/// AGCFSMRESET field bit
#define AGC_AGCFSMRESET_BIT    ((uint32_t)0x00001000)
/// AGCFSMRESET field position
#define AGC_AGCFSMRESET_POS    12
/// RADARDETEN field bit
#define AGC_RADARDETEN_BIT     ((uint32_t)0x00000800)
/// RADARDETEN field position
#define AGC_RADARDETEN_POS     11
/// RIFSDETEN field bit
#define AGC_RIFSDETEN_BIT      ((uint32_t)0x00000400)
/// RIFSDETEN field position
#define AGC_RIFSDETEN_POS      10
/// DSSSONLY field bit
#define AGC_DSSSONLY_BIT       ((uint32_t)0x00000200)
/// DSSSONLY field position
#define AGC_DSSSONLY_POS       9
/// OFDMONLY field bit
#define AGC_OFDMONLY_BIT       ((uint32_t)0x00000100)
/// OFDMONLY field position
#define AGC_OFDMONLY_POS       8
/// GPSTATUS field mask
#define AGC_GPSTATUS_MASK      ((uint32_t)0x000000F0)
/// GPSTATUS field LSB position
#define AGC_GPSTATUS_LSB       4
/// GPSTATUS field width
#define AGC_GPSTATUS_WIDTH     ((uint32_t)0x00000004)
/// COMBPATHSEL field mask
#define AGC_COMBPATHSEL_MASK   ((uint32_t)0x0000000F)
/// COMBPATHSEL field LSB position
#define AGC_COMBPATHSEL_LSB    0
/// COMBPATHSEL field width
#define AGC_COMBPATHSEL_WIDTH  ((uint32_t)0x00000004)

/// HTSTFGAINEN field reset value
#define AGC_HTSTFGAINEN_RST    0x1
/// AGCFSMRESET field reset value
#define AGC_AGCFSMRESET_RST    0x0
/// RADARDETEN field reset value
#define AGC_RADARDETEN_RST     0x0
/// RIFSDETEN field reset value
#define AGC_RIFSDETEN_RST      0x1
/// DSSSONLY field reset value
#define AGC_DSSSONLY_RST       0x0
/// OFDMONLY field reset value
#define AGC_OFDMONLY_RST       0x0
/// GPSTATUS field reset value
#define AGC_GPSTATUS_RST       0x0
/// COMBPATHSEL field reset value
#define AGC_COMBPATHSEL_RST    0xF

/**
 * @brief Constructs a value for the RWNXAGCCNTL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] htstfgainen - The value to use for the HTSTFGAINEN field.
 * @param[in] agcfsmreset - The value to use for the AGCFSMRESET field.
 * @param[in] radardeten - The value to use for the RADARDETEN field.
 * @param[in] rifsdeten - The value to use for the RIFSDETEN field.
 * @param[in] dsssonly - The value to use for the DSSSONLY field.
 * @param[in] ofdmonly - The value to use for the OFDMONLY field.
 * @param[in] gpstatus - The value to use for the GPSTATUS field.
 * @param[in] combpathsel - The value to use for the COMBPATHSEL field.
 */
__INLINE void agc_rwnxagccntl_pack(uint8_t htstfgainen, uint8_t agcfsmreset, uint8_t radardeten, uint8_t rifsdeten, uint8_t dsssonly, uint8_t ofdmonly, uint8_t gpstatus, uint8_t combpathsel)
{
    ASSERT_ERR((((uint32_t)htstfgainen << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)agcfsmreset << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)radardeten << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)rifsdeten << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)dsssonly << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)ofdmonly << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)gpstatus << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)combpathsel << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(AGC_RWNXAGCCNTL_ADDR,  ((uint32_t)htstfgainen << 16) | ((uint32_t)agcfsmreset << 12) | ((uint32_t)radardeten << 11) | ((uint32_t)rifsdeten << 10) | ((uint32_t)dsssonly << 9) | ((uint32_t)ofdmonly << 8) | ((uint32_t)gpstatus << 4) | ((uint32_t)combpathsel << 0));
}

/**
 * @brief Unpacks RWNXAGCCNTL's fields from current value of the RWNXAGCCNTL register.
 *
 * Reads the RWNXAGCCNTL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] htstfgainen - Will be populated with the current value of this field from the register.
 * @param[out] agcfsmreset - Will be populated with the current value of this field from the register.
 * @param[out] radardeten - Will be populated with the current value of this field from the register.
 * @param[out] rifsdeten - Will be populated with the current value of this field from the register.
 * @param[out] dsssonly - Will be populated with the current value of this field from the register.
 * @param[out] ofdmonly - Will be populated with the current value of this field from the register.
 * @param[out] gpstatus - Will be populated with the current value of this field from the register.
 * @param[out] combpathsel - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagccntl_unpack(uint8_t *htstfgainen, uint8_t *agcfsmreset, uint8_t *radardeten, uint8_t *rifsdeten, uint8_t *dsssonly, uint8_t *ofdmonly, uint8_t *gpstatus, uint8_t *combpathsel)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCNTL_ADDR);

    *htstfgainen = (localVal & ((uint32_t)0x00010000)) >> 16;
    *agcfsmreset = (localVal & ((uint32_t)0x00001000)) >> 12;
    *radardeten = (localVal & ((uint32_t)0x00000800)) >> 11;
    *rifsdeten = (localVal & ((uint32_t)0x00000400)) >> 10;
    *dsssonly = (localVal & ((uint32_t)0x00000200)) >> 9;
    *ofdmonly = (localVal & ((uint32_t)0x00000100)) >> 8;
    *gpstatus = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *combpathsel = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the HTSTFGAINEN field in the RWNXAGCCNTL register.
 *
 * The RWNXAGCCNTL register will be read and the HTSTFGAINEN field's value will be returned.
 *
 * @return The current value of the HTSTFGAINEN field in the RWNXAGCCNTL register.
 */
__INLINE uint8_t agc_htstfgainen_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the HTSTFGAINEN field of the RWNXAGCCNTL register.
 *
 * The RWNXAGCCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] htstfgainen - The value to set the field to.
 */
__INLINE void agc_htstfgainen_setf(uint8_t htstfgainen)
{
    ASSERT_ERR((((uint32_t)htstfgainen << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(AGC_RWNXAGCCNTL_ADDR, (REG_PL_RD(AGC_RWNXAGCCNTL_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)htstfgainen << 16));
}

/**
 * @brief Returns the current value of the AGCFSMRESET field in the RWNXAGCCNTL register.
 *
 * The RWNXAGCCNTL register will be read and the AGCFSMRESET field's value will be returned.
 *
 * @return The current value of the AGCFSMRESET field in the RWNXAGCCNTL register.
 */
__INLINE uint8_t agc_agcfsmreset_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the AGCFSMRESET field of the RWNXAGCCNTL register.
 *
 * The RWNXAGCCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] agcfsmreset - The value to set the field to.
 */
__INLINE void agc_agcfsmreset_setf(uint8_t agcfsmreset)
{
    ASSERT_ERR((((uint32_t)agcfsmreset << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(AGC_RWNXAGCCNTL_ADDR, (REG_PL_RD(AGC_RWNXAGCCNTL_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)agcfsmreset << 12));
}

/**
 * @brief Returns the current value of the RADARDETEN field in the RWNXAGCCNTL register.
 *
 * The RWNXAGCCNTL register will be read and the RADARDETEN field's value will be returned.
 *
 * @return The current value of the RADARDETEN field in the RWNXAGCCNTL register.
 */
__INLINE uint8_t agc_radardeten_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the RADARDETEN field of the RWNXAGCCNTL register.
 *
 * The RWNXAGCCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] radardeten - The value to set the field to.
 */
__INLINE void agc_radardeten_setf(uint8_t radardeten)
{
    ASSERT_ERR((((uint32_t)radardeten << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(AGC_RWNXAGCCNTL_ADDR, (REG_PL_RD(AGC_RWNXAGCCNTL_ADDR) & ~((uint32_t)0x00000800)) | ((uint32_t)radardeten << 11));
}

/**
 * @brief Returns the current value of the RIFSDETEN field in the RWNXAGCCNTL register.
 *
 * The RWNXAGCCNTL register will be read and the RIFSDETEN field's value will be returned.
 *
 * @return The current value of the RIFSDETEN field in the RWNXAGCCNTL register.
 */
__INLINE uint8_t agc_rifsdeten_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the RIFSDETEN field of the RWNXAGCCNTL register.
 *
 * The RWNXAGCCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rifsdeten - The value to set the field to.
 */
__INLINE void agc_rifsdeten_setf(uint8_t rifsdeten)
{
    ASSERT_ERR((((uint32_t)rifsdeten << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(AGC_RWNXAGCCNTL_ADDR, (REG_PL_RD(AGC_RWNXAGCCNTL_ADDR) & ~((uint32_t)0x00000400)) | ((uint32_t)rifsdeten << 10));
}

/**
 * @brief Returns the current value of the DSSSONLY field in the RWNXAGCCNTL register.
 *
 * The RWNXAGCCNTL register will be read and the DSSSONLY field's value will be returned.
 *
 * @return The current value of the DSSSONLY field in the RWNXAGCCNTL register.
 */
__INLINE uint8_t agc_dsssonly_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the DSSSONLY field of the RWNXAGCCNTL register.
 *
 * The RWNXAGCCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dsssonly - The value to set the field to.
 */
__INLINE void agc_dsssonly_setf(uint8_t dsssonly)
{
    ASSERT_ERR((((uint32_t)dsssonly << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(AGC_RWNXAGCCNTL_ADDR, (REG_PL_RD(AGC_RWNXAGCCNTL_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)dsssonly << 9));
}

/**
 * @brief Returns the current value of the OFDMONLY field in the RWNXAGCCNTL register.
 *
 * The RWNXAGCCNTL register will be read and the OFDMONLY field's value will be returned.
 *
 * @return The current value of the OFDMONLY field in the RWNXAGCCNTL register.
 */
__INLINE uint8_t agc_ofdmonly_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the OFDMONLY field of the RWNXAGCCNTL register.
 *
 * The RWNXAGCCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ofdmonly - The value to set the field to.
 */
__INLINE void agc_ofdmonly_setf(uint8_t ofdmonly)
{
    ASSERT_ERR((((uint32_t)ofdmonly << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(AGC_RWNXAGCCNTL_ADDR, (REG_PL_RD(AGC_RWNXAGCCNTL_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)ofdmonly << 8));
}

/**
 * @brief Returns the current value of the GPSTATUS field in the RWNXAGCCNTL register.
 *
 * The RWNXAGCCNTL register will be read and the GPSTATUS field's value will be returned.
 *
 * @return The current value of the GPSTATUS field in the RWNXAGCCNTL register.
 */
__INLINE uint8_t agc_gpstatus_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCNTL_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the GPSTATUS field of the RWNXAGCCNTL register.
 *
 * The RWNXAGCCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] gpstatus - The value to set the field to.
 */
__INLINE void agc_gpstatus_setf(uint8_t gpstatus)
{
    ASSERT_ERR((((uint32_t)gpstatus << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(AGC_RWNXAGCCNTL_ADDR, (REG_PL_RD(AGC_RWNXAGCCNTL_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)gpstatus << 4));
}

/**
 * @brief Returns the current value of the COMBPATHSEL field in the RWNXAGCCNTL register.
 *
 * The RWNXAGCCNTL register will be read and the COMBPATHSEL field's value will be returned.
 *
 * @return The current value of the COMBPATHSEL field in the RWNXAGCCNTL register.
 */
__INLINE uint8_t agc_combpathsel_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCNTL_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the COMBPATHSEL field of the RWNXAGCCNTL register.
 *
 * The RWNXAGCCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] combpathsel - The value to set the field to.
 */
__INLINE void agc_combpathsel_setf(uint8_t combpathsel)
{
    ASSERT_ERR((((uint32_t)combpathsel << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(AGC_RWNXAGCCNTL_ADDR, (REG_PL_RD(AGC_RWNXAGCCNTL_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)combpathsel << 0));
}

/// @}

/**
 * @name RWNXAGCDSP0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  27:24              NBITADC   0xC
 *  23:16         VPEAKADCQDBV   0xE8
 *  15:08         ADCPOWMINDBV   0xCE
 *  07:00        ADCPOWBIASQDB   0x0
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCDSP0 register
#define AGC_RWNXAGCDSP0_ADDR   0x01002064
/// Offset of the RWNXAGCDSP0 register from the base address
#define AGC_RWNXAGCDSP0_OFFSET 0x00002064
/// Index of the RWNXAGCDSP0 register
#define AGC_RWNXAGCDSP0_INDEX  0x00000819
/// Reset value of the RWNXAGCDSP0 register
#define AGC_RWNXAGCDSP0_RESET  0x0CE8CE00

/**
 * @brief Returns the current value of the RWNXAGCDSP0 register.
 * The RWNXAGCDSP0 register will be read and its value returned.
 * @return The current value of the RWNXAGCDSP0 register.
 */
__INLINE uint32_t agc_rwnxagcdsp0_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCDSP0_ADDR);
}

/**
 * @brief Sets the RWNXAGCDSP0 register to a value.
 * The RWNXAGCDSP0 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcdsp0_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCDSP0_ADDR, value);
}

// field definitions
/// NBITADC field mask
#define AGC_NBITADC_MASK         ((uint32_t)0x0F000000)
/// NBITADC field LSB position
#define AGC_NBITADC_LSB          24
/// NBITADC field width
#define AGC_NBITADC_WIDTH        ((uint32_t)0x00000004)
/// VPEAKADCQDBV field mask
#define AGC_VPEAKADCQDBV_MASK    ((uint32_t)0x00FF0000)
/// VPEAKADCQDBV field LSB position
#define AGC_VPEAKADCQDBV_LSB     16
/// VPEAKADCQDBV field width
#define AGC_VPEAKADCQDBV_WIDTH   ((uint32_t)0x00000008)
/// ADCPOWMINDBV field mask
#define AGC_ADCPOWMINDBV_MASK    ((uint32_t)0x0000FF00)
/// ADCPOWMINDBV field LSB position
#define AGC_ADCPOWMINDBV_LSB     8
/// ADCPOWMINDBV field width
#define AGC_ADCPOWMINDBV_WIDTH   ((uint32_t)0x00000008)
/// ADCPOWBIASQDB field mask
#define AGC_ADCPOWBIASQDB_MASK   ((uint32_t)0x000000FF)
/// ADCPOWBIASQDB field LSB position
#define AGC_ADCPOWBIASQDB_LSB    0
/// ADCPOWBIASQDB field width
#define AGC_ADCPOWBIASQDB_WIDTH  ((uint32_t)0x00000008)

/// NBITADC field reset value
#define AGC_NBITADC_RST          0xC
/// VPEAKADCQDBV field reset value
#define AGC_VPEAKADCQDBV_RST     0xE8
/// ADCPOWMINDBV field reset value
#define AGC_ADCPOWMINDBV_RST     0xCE
/// ADCPOWBIASQDB field reset value
#define AGC_ADCPOWBIASQDB_RST    0x0

/**
 * @brief Constructs a value for the RWNXAGCDSP0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] nbitadc - The value to use for the NBITADC field.
 * @param[in] vpeakadcqdbv - The value to use for the VPEAKADCQDBV field.
 * @param[in] adcpowmindbv - The value to use for the ADCPOWMINDBV field.
 * @param[in] adcpowbiasqdb - The value to use for the ADCPOWBIASQDB field.
 */
__INLINE void agc_rwnxagcdsp0_pack(uint8_t nbitadc, uint8_t vpeakadcqdbv, uint8_t adcpowmindbv, uint8_t adcpowbiasqdb)
{
    ASSERT_ERR((((uint32_t)nbitadc << 24) & ~((uint32_t)0x0F000000)) == 0);
    ASSERT_ERR((((uint32_t)vpeakadcqdbv << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)adcpowmindbv << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)adcpowbiasqdb << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCDSP0_ADDR,  ((uint32_t)nbitadc << 24) | ((uint32_t)vpeakadcqdbv << 16) | ((uint32_t)adcpowmindbv << 8) | ((uint32_t)adcpowbiasqdb << 0));
}

/**
 * @brief Unpacks RWNXAGCDSP0's fields from current value of the RWNXAGCDSP0 register.
 *
 * Reads the RWNXAGCDSP0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] nbitadc - Will be populated with the current value of this field from the register.
 * @param[out] vpeakadcqdbv - Will be populated with the current value of this field from the register.
 * @param[out] adcpowmindbv - Will be populated with the current value of this field from the register.
 * @param[out] adcpowbiasqdb - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcdsp0_unpack(uint8_t *nbitadc, uint8_t *vpeakadcqdbv, uint8_t *adcpowmindbv, uint8_t *adcpowbiasqdb)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDSP0_ADDR);

    *nbitadc = (localVal & ((uint32_t)0x0F000000)) >> 24;
    *vpeakadcqdbv = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *adcpowmindbv = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *adcpowbiasqdb = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the NBITADC field in the RWNXAGCDSP0 register.
 *
 * The RWNXAGCDSP0 register will be read and the NBITADC field's value will be returned.
 *
 * @return The current value of the NBITADC field in the RWNXAGCDSP0 register.
 */
__INLINE uint8_t agc_nbitadc_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDSP0_ADDR);
    return ((localVal & ((uint32_t)0x0F000000)) >> 24);
}

/**
 * @brief Sets the NBITADC field of the RWNXAGCDSP0 register.
 *
 * The RWNXAGCDSP0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nbitadc - The value to set the field to.
 */
__INLINE void agc_nbitadc_setf(uint8_t nbitadc)
{
    ASSERT_ERR((((uint32_t)nbitadc << 24) & ~((uint32_t)0x0F000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCDSP0_ADDR, (REG_PL_RD(AGC_RWNXAGCDSP0_ADDR) & ~((uint32_t)0x0F000000)) | ((uint32_t)nbitadc << 24));
}

/**
 * @brief Returns the current value of the VPEAKADCQDBV field in the RWNXAGCDSP0 register.
 *
 * The RWNXAGCDSP0 register will be read and the VPEAKADCQDBV field's value will be returned.
 *
 * @return The current value of the VPEAKADCQDBV field in the RWNXAGCDSP0 register.
 */
__INLINE uint8_t agc_vpeakadcqdbv_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDSP0_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the VPEAKADCQDBV field of the RWNXAGCDSP0 register.
 *
 * The RWNXAGCDSP0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] vpeakadcqdbv - The value to set the field to.
 */
__INLINE void agc_vpeakadcqdbv_setf(uint8_t vpeakadcqdbv)
{
    ASSERT_ERR((((uint32_t)vpeakadcqdbv << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCDSP0_ADDR, (REG_PL_RD(AGC_RWNXAGCDSP0_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)vpeakadcqdbv << 16));
}

/**
 * @brief Returns the current value of the ADCPOWMINDBV field in the RWNXAGCDSP0 register.
 *
 * The RWNXAGCDSP0 register will be read and the ADCPOWMINDBV field's value will be returned.
 *
 * @return The current value of the ADCPOWMINDBV field in the RWNXAGCDSP0 register.
 */
__INLINE uint8_t agc_adcpowmindbv_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDSP0_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the ADCPOWMINDBV field of the RWNXAGCDSP0 register.
 *
 * The RWNXAGCDSP0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] adcpowmindbv - The value to set the field to.
 */
__INLINE void agc_adcpowmindbv_setf(uint8_t adcpowmindbv)
{
    ASSERT_ERR((((uint32_t)adcpowmindbv << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(AGC_RWNXAGCDSP0_ADDR, (REG_PL_RD(AGC_RWNXAGCDSP0_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)adcpowmindbv << 8));
}

/**
 * @brief Returns the current value of the ADCPOWBIASQDB field in the RWNXAGCDSP0 register.
 *
 * The RWNXAGCDSP0 register will be read and the ADCPOWBIASQDB field's value will be returned.
 *
 * @return The current value of the ADCPOWBIASQDB field in the RWNXAGCDSP0 register.
 */
__INLINE uint8_t agc_adcpowbiasqdb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDSP0_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the ADCPOWBIASQDB field of the RWNXAGCDSP0 register.
 *
 * The RWNXAGCDSP0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] adcpowbiasqdb - The value to set the field to.
 */
__INLINE void agc_adcpowbiasqdb_setf(uint8_t adcpowbiasqdb)
{
    ASSERT_ERR((((uint32_t)adcpowbiasqdb << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCDSP0_ADDR, (REG_PL_RD(AGC_RWNXAGCDSP0_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)adcpowbiasqdb << 0));
}

/// @}

/**
 * @name RWNXAGCDSP1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:28              INBDRND   0x3
 *  23:16    INBDPOW40SBIASQDB   0x0
 *  15:08    INBDPOW20SBIASQDB   0x0
 *  07:00    INBDPOW20PBIASQDB   0x0
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCDSP1 register
#define AGC_RWNXAGCDSP1_ADDR   0x01002068
/// Offset of the RWNXAGCDSP1 register from the base address
#define AGC_RWNXAGCDSP1_OFFSET 0x00002068
/// Index of the RWNXAGCDSP1 register
#define AGC_RWNXAGCDSP1_INDEX  0x0000081A
/// Reset value of the RWNXAGCDSP1 register
#define AGC_RWNXAGCDSP1_RESET  0x30000000

/**
 * @brief Returns the current value of the RWNXAGCDSP1 register.
 * The RWNXAGCDSP1 register will be read and its value returned.
 * @return The current value of the RWNXAGCDSP1 register.
 */
__INLINE uint32_t agc_rwnxagcdsp1_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCDSP1_ADDR);
}

/**
 * @brief Sets the RWNXAGCDSP1 register to a value.
 * The RWNXAGCDSP1 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcdsp1_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCDSP1_ADDR, value);
}

// field definitions
/// INBDRND field mask
#define AGC_INBDRND_MASK             ((uint32_t)0x30000000)
/// INBDRND field LSB position
#define AGC_INBDRND_LSB              28
/// INBDRND field width
#define AGC_INBDRND_WIDTH            ((uint32_t)0x00000002)
/// INBDPOW40SBIASQDB field mask
#define AGC_INBDPOW40SBIASQDB_MASK   ((uint32_t)0x00FF0000)
/// INBDPOW40SBIASQDB field LSB position
#define AGC_INBDPOW40SBIASQDB_LSB    16
/// INBDPOW40SBIASQDB field width
#define AGC_INBDPOW40SBIASQDB_WIDTH  ((uint32_t)0x00000008)
/// INBDPOW20SBIASQDB field mask
#define AGC_INBDPOW20SBIASQDB_MASK   ((uint32_t)0x0000FF00)
/// INBDPOW20SBIASQDB field LSB position
#define AGC_INBDPOW20SBIASQDB_LSB    8
/// INBDPOW20SBIASQDB field width
#define AGC_INBDPOW20SBIASQDB_WIDTH  ((uint32_t)0x00000008)
/// INBDPOW20PBIASQDB field mask
#define AGC_INBDPOW20PBIASQDB_MASK   ((uint32_t)0x000000FF)
/// INBDPOW20PBIASQDB field LSB position
#define AGC_INBDPOW20PBIASQDB_LSB    0
/// INBDPOW20PBIASQDB field width
#define AGC_INBDPOW20PBIASQDB_WIDTH  ((uint32_t)0x00000008)

/// INBDRND field reset value
#define AGC_INBDRND_RST              0x3
/// INBDPOW40SBIASQDB field reset value
#define AGC_INBDPOW40SBIASQDB_RST    0x0
/// INBDPOW20SBIASQDB field reset value
#define AGC_INBDPOW20SBIASQDB_RST    0x0
/// INBDPOW20PBIASQDB field reset value
#define AGC_INBDPOW20PBIASQDB_RST    0x0

/**
 * @brief Constructs a value for the RWNXAGCDSP1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] inbdrnd - The value to use for the INBDRND field.
 * @param[in] inbdpow40sbiasqdb - The value to use for the INBDPOW40SBIASQDB field.
 * @param[in] inbdpow20sbiasqdb - The value to use for the INBDPOW20SBIASQDB field.
 * @param[in] inbdpow20pbiasqdb - The value to use for the INBDPOW20PBIASQDB field.
 */
__INLINE void agc_rwnxagcdsp1_pack(uint8_t inbdrnd, uint8_t inbdpow40sbiasqdb, uint8_t inbdpow20sbiasqdb, uint8_t inbdpow20pbiasqdb)
{
    ASSERT_ERR((((uint32_t)inbdrnd << 28) & ~((uint32_t)0x30000000)) == 0);
    ASSERT_ERR((((uint32_t)inbdpow40sbiasqdb << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)inbdpow20sbiasqdb << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)inbdpow20pbiasqdb << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCDSP1_ADDR,  ((uint32_t)inbdrnd << 28) | ((uint32_t)inbdpow40sbiasqdb << 16) | ((uint32_t)inbdpow20sbiasqdb << 8) | ((uint32_t)inbdpow20pbiasqdb << 0));
}

/**
 * @brief Unpacks RWNXAGCDSP1's fields from current value of the RWNXAGCDSP1 register.
 *
 * Reads the RWNXAGCDSP1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] inbdrnd - Will be populated with the current value of this field from the register.
 * @param[out] inbdpow40sbiasqdb - Will be populated with the current value of this field from the register.
 * @param[out] inbdpow20sbiasqdb - Will be populated with the current value of this field from the register.
 * @param[out] inbdpow20pbiasqdb - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcdsp1_unpack(uint8_t *inbdrnd, uint8_t *inbdpow40sbiasqdb, uint8_t *inbdpow20sbiasqdb, uint8_t *inbdpow20pbiasqdb)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDSP1_ADDR);

    *inbdrnd = (localVal & ((uint32_t)0x30000000)) >> 28;
    *inbdpow40sbiasqdb = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *inbdpow20sbiasqdb = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *inbdpow20pbiasqdb = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the INBDRND field in the RWNXAGCDSP1 register.
 *
 * The RWNXAGCDSP1 register will be read and the INBDRND field's value will be returned.
 *
 * @return The current value of the INBDRND field in the RWNXAGCDSP1 register.
 */
__INLINE uint8_t agc_inbdrnd_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDSP1_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

/**
 * @brief Sets the INBDRND field of the RWNXAGCDSP1 register.
 *
 * The RWNXAGCDSP1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] inbdrnd - The value to set the field to.
 */
__INLINE void agc_inbdrnd_setf(uint8_t inbdrnd)
{
    ASSERT_ERR((((uint32_t)inbdrnd << 28) & ~((uint32_t)0x30000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCDSP1_ADDR, (REG_PL_RD(AGC_RWNXAGCDSP1_ADDR) & ~((uint32_t)0x30000000)) | ((uint32_t)inbdrnd << 28));
}

/**
 * @brief Returns the current value of the INBDPOW40SBIASQDB field in the RWNXAGCDSP1 register.
 *
 * The RWNXAGCDSP1 register will be read and the INBDPOW40SBIASQDB field's value will be returned.
 *
 * @return The current value of the INBDPOW40SBIASQDB field in the RWNXAGCDSP1 register.
 */
__INLINE uint8_t agc_inbdpow40sbiasqdb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDSP1_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the INBDPOW40SBIASQDB field of the RWNXAGCDSP1 register.
 *
 * The RWNXAGCDSP1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] inbdpow40sbiasqdb - The value to set the field to.
 */
__INLINE void agc_inbdpow40sbiasqdb_setf(uint8_t inbdpow40sbiasqdb)
{
    ASSERT_ERR((((uint32_t)inbdpow40sbiasqdb << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCDSP1_ADDR, (REG_PL_RD(AGC_RWNXAGCDSP1_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)inbdpow40sbiasqdb << 16));
}

/**
 * @brief Returns the current value of the INBDPOW20SBIASQDB field in the RWNXAGCDSP1 register.
 *
 * The RWNXAGCDSP1 register will be read and the INBDPOW20SBIASQDB field's value will be returned.
 *
 * @return The current value of the INBDPOW20SBIASQDB field in the RWNXAGCDSP1 register.
 */
__INLINE uint8_t agc_inbdpow20sbiasqdb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDSP1_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the INBDPOW20SBIASQDB field of the RWNXAGCDSP1 register.
 *
 * The RWNXAGCDSP1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] inbdpow20sbiasqdb - The value to set the field to.
 */
__INLINE void agc_inbdpow20sbiasqdb_setf(uint8_t inbdpow20sbiasqdb)
{
    ASSERT_ERR((((uint32_t)inbdpow20sbiasqdb << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(AGC_RWNXAGCDSP1_ADDR, (REG_PL_RD(AGC_RWNXAGCDSP1_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)inbdpow20sbiasqdb << 8));
}

/**
 * @brief Returns the current value of the INBDPOW20PBIASQDB field in the RWNXAGCDSP1 register.
 *
 * The RWNXAGCDSP1 register will be read and the INBDPOW20PBIASQDB field's value will be returned.
 *
 * @return The current value of the INBDPOW20PBIASQDB field in the RWNXAGCDSP1 register.
 */
__INLINE uint8_t agc_inbdpow20pbiasqdb_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDSP1_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the INBDPOW20PBIASQDB field of the RWNXAGCDSP1 register.
 *
 * The RWNXAGCDSP1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] inbdpow20pbiasqdb - The value to set the field to.
 */
__INLINE void agc_inbdpow20pbiasqdb_setf(uint8_t inbdpow20pbiasqdb)
{
    ASSERT_ERR((((uint32_t)inbdpow20pbiasqdb << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCDSP1_ADDR, (REG_PL_RD(AGC_RWNXAGCDSP1_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)inbdpow20pbiasqdb << 0));
}

/// @}

/**
 * @name RWNXAGCDSP2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  24:16        INBDPOWMINDBM   0x19C
 *  15:08             FE40GAIN   0x2
 *  07:00             FE20GAIN   0x2
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCDSP2 register
#define AGC_RWNXAGCDSP2_ADDR   0x0100206C
/// Offset of the RWNXAGCDSP2 register from the base address
#define AGC_RWNXAGCDSP2_OFFSET 0x0000206C
/// Index of the RWNXAGCDSP2 register
#define AGC_RWNXAGCDSP2_INDEX  0x0000081B
/// Reset value of the RWNXAGCDSP2 register
#define AGC_RWNXAGCDSP2_RESET  0x019C0202

/**
 * @brief Returns the current value of the RWNXAGCDSP2 register.
 * The RWNXAGCDSP2 register will be read and its value returned.
 * @return The current value of the RWNXAGCDSP2 register.
 */
__INLINE uint32_t agc_rwnxagcdsp2_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCDSP2_ADDR);
}

/**
 * @brief Sets the RWNXAGCDSP2 register to a value.
 * The RWNXAGCDSP2 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcdsp2_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCDSP2_ADDR, value);
}

// field definitions
/// INBDPOWMINDBM field mask
#define AGC_INBDPOWMINDBM_MASK   ((uint32_t)0x01FF0000)
/// INBDPOWMINDBM field LSB position
#define AGC_INBDPOWMINDBM_LSB    16
/// INBDPOWMINDBM field width
#define AGC_INBDPOWMINDBM_WIDTH  ((uint32_t)0x00000009)
/// FE40GAIN field mask
#define AGC_FE40GAIN_MASK        ((uint32_t)0x0000FF00)
/// FE40GAIN field LSB position
#define AGC_FE40GAIN_LSB         8
/// FE40GAIN field width
#define AGC_FE40GAIN_WIDTH       ((uint32_t)0x00000008)
/// FE20GAIN field mask
#define AGC_FE20GAIN_MASK        ((uint32_t)0x000000FF)
/// FE20GAIN field LSB position
#define AGC_FE20GAIN_LSB         0
/// FE20GAIN field width
#define AGC_FE20GAIN_WIDTH       ((uint32_t)0x00000008)

/// INBDPOWMINDBM field reset value
#define AGC_INBDPOWMINDBM_RST    0x19C
/// FE40GAIN field reset value
#define AGC_FE40GAIN_RST         0x2
/// FE20GAIN field reset value
#define AGC_FE20GAIN_RST         0x2

/**
 * @brief Constructs a value for the RWNXAGCDSP2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] inbdpowmindbm - The value to use for the INBDPOWMINDBM field.
 * @param[in] fe40gain - The value to use for the FE40GAIN field.
 * @param[in] fe20gain - The value to use for the FE20GAIN field.
 */
__INLINE void agc_rwnxagcdsp2_pack(uint16_t inbdpowmindbm, uint8_t fe40gain, uint8_t fe20gain)
{
    ASSERT_ERR((((uint32_t)inbdpowmindbm << 16) & ~((uint32_t)0x01FF0000)) == 0);
    ASSERT_ERR((((uint32_t)fe40gain << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)fe20gain << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCDSP2_ADDR,  ((uint32_t)inbdpowmindbm << 16) | ((uint32_t)fe40gain << 8) | ((uint32_t)fe20gain << 0));
}

/**
 * @brief Unpacks RWNXAGCDSP2's fields from current value of the RWNXAGCDSP2 register.
 *
 * Reads the RWNXAGCDSP2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] inbdpowmindbm - Will be populated with the current value of this field from the register.
 * @param[out] fe40gain - Will be populated with the current value of this field from the register.
 * @param[out] fe20gain - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcdsp2_unpack(uint16_t *inbdpowmindbm, uint8_t *fe40gain, uint8_t *fe20gain)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDSP2_ADDR);

    *inbdpowmindbm = (localVal & ((uint32_t)0x01FF0000)) >> 16;
    *fe40gain = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *fe20gain = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the INBDPOWMINDBM field in the RWNXAGCDSP2 register.
 *
 * The RWNXAGCDSP2 register will be read and the INBDPOWMINDBM field's value will be returned.
 *
 * @return The current value of the INBDPOWMINDBM field in the RWNXAGCDSP2 register.
 */
__INLINE uint16_t agc_inbdpowmindbm_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDSP2_ADDR);
    return ((localVal & ((uint32_t)0x01FF0000)) >> 16);
}

/**
 * @brief Sets the INBDPOWMINDBM field of the RWNXAGCDSP2 register.
 *
 * The RWNXAGCDSP2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] inbdpowmindbm - The value to set the field to.
 */
__INLINE void agc_inbdpowmindbm_setf(uint16_t inbdpowmindbm)
{
    ASSERT_ERR((((uint32_t)inbdpowmindbm << 16) & ~((uint32_t)0x01FF0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCDSP2_ADDR, (REG_PL_RD(AGC_RWNXAGCDSP2_ADDR) & ~((uint32_t)0x01FF0000)) | ((uint32_t)inbdpowmindbm << 16));
}

/**
 * @brief Returns the current value of the FE40GAIN field in the RWNXAGCDSP2 register.
 *
 * The RWNXAGCDSP2 register will be read and the FE40GAIN field's value will be returned.
 *
 * @return The current value of the FE40GAIN field in the RWNXAGCDSP2 register.
 */
__INLINE uint8_t agc_fe40gain_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDSP2_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the FE40GAIN field of the RWNXAGCDSP2 register.
 *
 * The RWNXAGCDSP2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fe40gain - The value to set the field to.
 */
__INLINE void agc_fe40gain_setf(uint8_t fe40gain)
{
    ASSERT_ERR((((uint32_t)fe40gain << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(AGC_RWNXAGCDSP2_ADDR, (REG_PL_RD(AGC_RWNXAGCDSP2_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)fe40gain << 8));
}

/**
 * @brief Returns the current value of the FE20GAIN field in the RWNXAGCDSP2 register.
 *
 * The RWNXAGCDSP2 register will be read and the FE20GAIN field's value will be returned.
 *
 * @return The current value of the FE20GAIN field in the RWNXAGCDSP2 register.
 */
__INLINE uint8_t agc_fe20gain_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDSP2_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the FE20GAIN field of the RWNXAGCDSP2 register.
 *
 * The RWNXAGCDSP2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fe20gain - The value to set the field to.
 */
__INLINE void agc_fe20gain_setf(uint8_t fe20gain)
{
    ASSERT_ERR((((uint32_t)fe20gain << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCDSP2_ADDR, (REG_PL_RD(AGC_RWNXAGCDSP2_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)fe20gain << 0));
}

/// @}

/**
 * @name RWNXAGCCCA0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  22:20      DELCCARAMPDNTAP   0x5
 *  18:16      DELCCARAMPUPTAP   0x5
 *  13:08         CCADETRAMPDN   0x9
 *  05:00         CCADETRAMPUP   0x9
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCCCA0 register
#define AGC_RWNXAGCCCA0_ADDR   0x01002070
/// Offset of the RWNXAGCCCA0 register from the base address
#define AGC_RWNXAGCCCA0_OFFSET 0x00002070
/// Index of the RWNXAGCCCA0 register
#define AGC_RWNXAGCCCA0_INDEX  0x0000081C
/// Reset value of the RWNXAGCCCA0 register
#define AGC_RWNXAGCCCA0_RESET  0x00550909

/**
 * @brief Returns the current value of the RWNXAGCCCA0 register.
 * The RWNXAGCCCA0 register will be read and its value returned.
 * @return The current value of the RWNXAGCCCA0 register.
 */
__INLINE uint32_t agc_rwnxagccca0_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCCCA0_ADDR);
}

/**
 * @brief Sets the RWNXAGCCCA0 register to a value.
 * The RWNXAGCCCA0 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagccca0_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCCCA0_ADDR, value);
}

// field definitions
/// DELCCARAMPDNTAP field mask
#define AGC_DELCCARAMPDNTAP_MASK   ((uint32_t)0x00700000)
/// DELCCARAMPDNTAP field LSB position
#define AGC_DELCCARAMPDNTAP_LSB    20
/// DELCCARAMPDNTAP field width
#define AGC_DELCCARAMPDNTAP_WIDTH  ((uint32_t)0x00000003)
/// DELCCARAMPUPTAP field mask
#define AGC_DELCCARAMPUPTAP_MASK   ((uint32_t)0x00070000)
/// DELCCARAMPUPTAP field LSB position
#define AGC_DELCCARAMPUPTAP_LSB    16
/// DELCCARAMPUPTAP field width
#define AGC_DELCCARAMPUPTAP_WIDTH  ((uint32_t)0x00000003)
/// CCADETRAMPDN field mask
#define AGC_CCADETRAMPDN_MASK      ((uint32_t)0x00003F00)
/// CCADETRAMPDN field LSB position
#define AGC_CCADETRAMPDN_LSB       8
/// CCADETRAMPDN field width
#define AGC_CCADETRAMPDN_WIDTH     ((uint32_t)0x00000006)
/// CCADETRAMPUP field mask
#define AGC_CCADETRAMPUP_MASK      ((uint32_t)0x0000003F)
/// CCADETRAMPUP field LSB position
#define AGC_CCADETRAMPUP_LSB       0
/// CCADETRAMPUP field width
#define AGC_CCADETRAMPUP_WIDTH     ((uint32_t)0x00000006)

/// DELCCARAMPDNTAP field reset value
#define AGC_DELCCARAMPDNTAP_RST    0x5
/// DELCCARAMPUPTAP field reset value
#define AGC_DELCCARAMPUPTAP_RST    0x5
/// CCADETRAMPDN field reset value
#define AGC_CCADETRAMPDN_RST       0x9
/// CCADETRAMPUP field reset value
#define AGC_CCADETRAMPUP_RST       0x9

/**
 * @brief Constructs a value for the RWNXAGCCCA0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] delccarampdntap - The value to use for the DELCCARAMPDNTAP field.
 * @param[in] delccarampuptap - The value to use for the DELCCARAMPUPTAP field.
 * @param[in] ccadetrampdn - The value to use for the CCADETRAMPDN field.
 * @param[in] ccadetrampup - The value to use for the CCADETRAMPUP field.
 */
__INLINE void agc_rwnxagccca0_pack(uint8_t delccarampdntap, uint8_t delccarampuptap, uint8_t ccadetrampdn, uint8_t ccadetrampup)
{
    ASSERT_ERR((((uint32_t)delccarampdntap << 20) & ~((uint32_t)0x00700000)) == 0);
    ASSERT_ERR((((uint32_t)delccarampuptap << 16) & ~((uint32_t)0x00070000)) == 0);
    ASSERT_ERR((((uint32_t)ccadetrampdn << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)ccadetrampup << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCA0_ADDR,  ((uint32_t)delccarampdntap << 20) | ((uint32_t)delccarampuptap << 16) | ((uint32_t)ccadetrampdn << 8) | ((uint32_t)ccadetrampup << 0));
}

/**
 * @brief Unpacks RWNXAGCCCA0's fields from current value of the RWNXAGCCCA0 register.
 *
 * Reads the RWNXAGCCCA0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] delccarampdntap - Will be populated with the current value of this field from the register.
 * @param[out] delccarampuptap - Will be populated with the current value of this field from the register.
 * @param[out] ccadetrampdn - Will be populated with the current value of this field from the register.
 * @param[out] ccadetrampup - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagccca0_unpack(uint8_t *delccarampdntap, uint8_t *delccarampuptap, uint8_t *ccadetrampdn, uint8_t *ccadetrampup)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCA0_ADDR);

    *delccarampdntap = (localVal & ((uint32_t)0x00700000)) >> 20;
    *delccarampuptap = (localVal & ((uint32_t)0x00070000)) >> 16;
    *ccadetrampdn = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *ccadetrampup = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the DELCCARAMPDNTAP field in the RWNXAGCCCA0 register.
 *
 * The RWNXAGCCCA0 register will be read and the DELCCARAMPDNTAP field's value will be returned.
 *
 * @return The current value of the DELCCARAMPDNTAP field in the RWNXAGCCCA0 register.
 */
__INLINE uint8_t agc_delccarampdntap_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCA0_ADDR);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

/**
 * @brief Sets the DELCCARAMPDNTAP field of the RWNXAGCCCA0 register.
 *
 * The RWNXAGCCCA0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] delccarampdntap - The value to set the field to.
 */
__INLINE void agc_delccarampdntap_setf(uint8_t delccarampdntap)
{
    ASSERT_ERR((((uint32_t)delccarampdntap << 20) & ~((uint32_t)0x00700000)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCA0_ADDR, (REG_PL_RD(AGC_RWNXAGCCCA0_ADDR) & ~((uint32_t)0x00700000)) | ((uint32_t)delccarampdntap << 20));
}

/**
 * @brief Returns the current value of the DELCCARAMPUPTAP field in the RWNXAGCCCA0 register.
 *
 * The RWNXAGCCCA0 register will be read and the DELCCARAMPUPTAP field's value will be returned.
 *
 * @return The current value of the DELCCARAMPUPTAP field in the RWNXAGCCCA0 register.
 */
__INLINE uint8_t agc_delccarampuptap_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCA0_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

/**
 * @brief Sets the DELCCARAMPUPTAP field of the RWNXAGCCCA0 register.
 *
 * The RWNXAGCCCA0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] delccarampuptap - The value to set the field to.
 */
__INLINE void agc_delccarampuptap_setf(uint8_t delccarampuptap)
{
    ASSERT_ERR((((uint32_t)delccarampuptap << 16) & ~((uint32_t)0x00070000)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCA0_ADDR, (REG_PL_RD(AGC_RWNXAGCCCA0_ADDR) & ~((uint32_t)0x00070000)) | ((uint32_t)delccarampuptap << 16));
}

/**
 * @brief Returns the current value of the CCADETRAMPDN field in the RWNXAGCCCA0 register.
 *
 * The RWNXAGCCCA0 register will be read and the CCADETRAMPDN field's value will be returned.
 *
 * @return The current value of the CCADETRAMPDN field in the RWNXAGCCCA0 register.
 */
__INLINE uint8_t agc_ccadetrampdn_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCA0_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the CCADETRAMPDN field of the RWNXAGCCCA0 register.
 *
 * The RWNXAGCCCA0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ccadetrampdn - The value to set the field to.
 */
__INLINE void agc_ccadetrampdn_setf(uint8_t ccadetrampdn)
{
    ASSERT_ERR((((uint32_t)ccadetrampdn << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCA0_ADDR, (REG_PL_RD(AGC_RWNXAGCCCA0_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)ccadetrampdn << 8));
}

/**
 * @brief Returns the current value of the CCADETRAMPUP field in the RWNXAGCCCA0 register.
 *
 * The RWNXAGCCCA0 register will be read and the CCADETRAMPUP field's value will be returned.
 *
 * @return The current value of the CCADETRAMPUP field in the RWNXAGCCCA0 register.
 */
__INLINE uint8_t agc_ccadetrampup_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCA0_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the CCADETRAMPUP field of the RWNXAGCCCA0 register.
 *
 * The RWNXAGCCCA0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ccadetrampup - The value to set the field to.
 */
__INLINE void agc_ccadetrampup_setf(uint8_t ccadetrampup)
{
    ASSERT_ERR((((uint32_t)ccadetrampup << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCA0_ADDR, (REG_PL_RD(AGC_RWNXAGCCCA0_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)ccadetrampup << 0));
}

/// @}

/**
 * @name RWNXAGCCCA1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  28:20     INBDCCAPOWMINDBM   0x1B5
 *  19:12        CCAFALLTHRDBM   0xBF
 *     09             DISCCAEN   1
 *     08             SATCCAEN   1
 *  07:00        CCARISETHRDBM   0xC2
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCCCA1 register
#define AGC_RWNXAGCCCA1_ADDR   0x01002074
/// Offset of the RWNXAGCCCA1 register from the base address
#define AGC_RWNXAGCCCA1_OFFSET 0x00002074
/// Index of the RWNXAGCCCA1 register
#define AGC_RWNXAGCCCA1_INDEX  0x0000081D
/// Reset value of the RWNXAGCCCA1 register
#define AGC_RWNXAGCCCA1_RESET  0x1B5BF3C2

/**
 * @brief Returns the current value of the RWNXAGCCCA1 register.
 * The RWNXAGCCCA1 register will be read and its value returned.
 * @return The current value of the RWNXAGCCCA1 register.
 */
__INLINE uint32_t agc_rwnxagccca1_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCCCA1_ADDR);
}

/**
 * @brief Sets the RWNXAGCCCA1 register to a value.
 * The RWNXAGCCCA1 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagccca1_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCCCA1_ADDR, value);
}

// field definitions
/// INBDCCAPOWMINDBM field mask
#define AGC_INBDCCAPOWMINDBM_MASK   ((uint32_t)0x1FF00000)
/// INBDCCAPOWMINDBM field LSB position
#define AGC_INBDCCAPOWMINDBM_LSB    20
/// INBDCCAPOWMINDBM field width
#define AGC_INBDCCAPOWMINDBM_WIDTH  ((uint32_t)0x00000009)
/// CCAFALLTHRDBM field mask
#define AGC_CCAFALLTHRDBM_MASK      ((uint32_t)0x000FF000)
/// CCAFALLTHRDBM field LSB position
#define AGC_CCAFALLTHRDBM_LSB       12
/// CCAFALLTHRDBM field width
#define AGC_CCAFALLTHRDBM_WIDTH     ((uint32_t)0x00000008)
/// DISCCAEN field bit
#define AGC_DISCCAEN_BIT            ((uint32_t)0x00000200)
/// DISCCAEN field position
#define AGC_DISCCAEN_POS            9
/// SATCCAEN field bit
#define AGC_SATCCAEN_BIT            ((uint32_t)0x00000100)
/// SATCCAEN field position
#define AGC_SATCCAEN_POS            8
/// CCARISETHRDBM field mask
#define AGC_CCARISETHRDBM_MASK      ((uint32_t)0x000000FF)
/// CCARISETHRDBM field LSB position
#define AGC_CCARISETHRDBM_LSB       0
/// CCARISETHRDBM field width
#define AGC_CCARISETHRDBM_WIDTH     ((uint32_t)0x00000008)

/// INBDCCAPOWMINDBM field reset value
#define AGC_INBDCCAPOWMINDBM_RST    0x1B5
/// CCAFALLTHRDBM field reset value
#define AGC_CCAFALLTHRDBM_RST       0xBF
/// DISCCAEN field reset value
#define AGC_DISCCAEN_RST            0x1
/// SATCCAEN field reset value
#define AGC_SATCCAEN_RST            0x1
/// CCARISETHRDBM field reset value
#define AGC_CCARISETHRDBM_RST       0xC2

/**
 * @brief Constructs a value for the RWNXAGCCCA1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] inbdccapowmindbm - The value to use for the INBDCCAPOWMINDBM field.
 * @param[in] ccafallthrdbm - The value to use for the CCAFALLTHRDBM field.
 * @param[in] disccaen - The value to use for the DISCCAEN field.
 * @param[in] satccaen - The value to use for the SATCCAEN field.
 * @param[in] ccarisethrdbm - The value to use for the CCARISETHRDBM field.
 */
__INLINE void agc_rwnxagccca1_pack(uint16_t inbdccapowmindbm, uint8_t ccafallthrdbm, uint8_t disccaen, uint8_t satccaen, uint8_t ccarisethrdbm)
{
    ASSERT_ERR((((uint32_t)inbdccapowmindbm << 20) & ~((uint32_t)0x1FF00000)) == 0);
    ASSERT_ERR((((uint32_t)ccafallthrdbm << 12) & ~((uint32_t)0x000FF000)) == 0);
    ASSERT_ERR((((uint32_t)disccaen << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)satccaen << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)ccarisethrdbm << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCA1_ADDR,  ((uint32_t)inbdccapowmindbm << 20) | ((uint32_t)ccafallthrdbm << 12) | ((uint32_t)disccaen << 9) | ((uint32_t)satccaen << 8) | ((uint32_t)ccarisethrdbm << 0));
}

/**
 * @brief Unpacks RWNXAGCCCA1's fields from current value of the RWNXAGCCCA1 register.
 *
 * Reads the RWNXAGCCCA1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] inbdccapowmindbm - Will be populated with the current value of this field from the register.
 * @param[out] ccafallthrdbm - Will be populated with the current value of this field from the register.
 * @param[out] disccaen - Will be populated with the current value of this field from the register.
 * @param[out] satccaen - Will be populated with the current value of this field from the register.
 * @param[out] ccarisethrdbm - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagccca1_unpack(uint16_t *inbdccapowmindbm, uint8_t *ccafallthrdbm, uint8_t *disccaen, uint8_t *satccaen, uint8_t *ccarisethrdbm)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCA1_ADDR);

    *inbdccapowmindbm = (localVal & ((uint32_t)0x1FF00000)) >> 20;
    *ccafallthrdbm = (localVal & ((uint32_t)0x000FF000)) >> 12;
    *disccaen = (localVal & ((uint32_t)0x00000200)) >> 9;
    *satccaen = (localVal & ((uint32_t)0x00000100)) >> 8;
    *ccarisethrdbm = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the INBDCCAPOWMINDBM field in the RWNXAGCCCA1 register.
 *
 * The RWNXAGCCCA1 register will be read and the INBDCCAPOWMINDBM field's value will be returned.
 *
 * @return The current value of the INBDCCAPOWMINDBM field in the RWNXAGCCCA1 register.
 */
__INLINE uint16_t agc_inbdccapowmindbm_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCA1_ADDR);
    return ((localVal & ((uint32_t)0x1FF00000)) >> 20);
}

/**
 * @brief Sets the INBDCCAPOWMINDBM field of the RWNXAGCCCA1 register.
 *
 * The RWNXAGCCCA1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] inbdccapowmindbm - The value to set the field to.
 */
__INLINE void agc_inbdccapowmindbm_setf(uint16_t inbdccapowmindbm)
{
    ASSERT_ERR((((uint32_t)inbdccapowmindbm << 20) & ~((uint32_t)0x1FF00000)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCA1_ADDR, (REG_PL_RD(AGC_RWNXAGCCCA1_ADDR) & ~((uint32_t)0x1FF00000)) | ((uint32_t)inbdccapowmindbm << 20));
}

/**
 * @brief Returns the current value of the CCAFALLTHRDBM field in the RWNXAGCCCA1 register.
 *
 * The RWNXAGCCCA1 register will be read and the CCAFALLTHRDBM field's value will be returned.
 *
 * @return The current value of the CCAFALLTHRDBM field in the RWNXAGCCCA1 register.
 */
__INLINE uint8_t agc_ccafallthrdbm_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCA1_ADDR);
    return ((localVal & ((uint32_t)0x000FF000)) >> 12);
}

/**
 * @brief Sets the CCAFALLTHRDBM field of the RWNXAGCCCA1 register.
 *
 * The RWNXAGCCCA1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ccafallthrdbm - The value to set the field to.
 */
__INLINE void agc_ccafallthrdbm_setf(uint8_t ccafallthrdbm)
{
    ASSERT_ERR((((uint32_t)ccafallthrdbm << 12) & ~((uint32_t)0x000FF000)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCA1_ADDR, (REG_PL_RD(AGC_RWNXAGCCCA1_ADDR) & ~((uint32_t)0x000FF000)) | ((uint32_t)ccafallthrdbm << 12));
}

/**
 * @brief Returns the current value of the DISCCAEN field in the RWNXAGCCCA1 register.
 *
 * The RWNXAGCCCA1 register will be read and the DISCCAEN field's value will be returned.
 *
 * @return The current value of the DISCCAEN field in the RWNXAGCCCA1 register.
 */
__INLINE uint8_t agc_disccaen_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCA1_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the DISCCAEN field of the RWNXAGCCCA1 register.
 *
 * The RWNXAGCCCA1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] disccaen - The value to set the field to.
 */
__INLINE void agc_disccaen_setf(uint8_t disccaen)
{
    ASSERT_ERR((((uint32_t)disccaen << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCA1_ADDR, (REG_PL_RD(AGC_RWNXAGCCCA1_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)disccaen << 9));
}

/**
 * @brief Returns the current value of the SATCCAEN field in the RWNXAGCCCA1 register.
 *
 * The RWNXAGCCCA1 register will be read and the SATCCAEN field's value will be returned.
 *
 * @return The current value of the SATCCAEN field in the RWNXAGCCCA1 register.
 */
__INLINE uint8_t agc_satccaen_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCA1_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the SATCCAEN field of the RWNXAGCCCA1 register.
 *
 * The RWNXAGCCCA1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] satccaen - The value to set the field to.
 */
__INLINE void agc_satccaen_setf(uint8_t satccaen)
{
    ASSERT_ERR((((uint32_t)satccaen << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCA1_ADDR, (REG_PL_RD(AGC_RWNXAGCCCA1_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)satccaen << 8));
}

/**
 * @brief Returns the current value of the CCARISETHRDBM field in the RWNXAGCCCA1 register.
 *
 * The RWNXAGCCCA1 register will be read and the CCARISETHRDBM field's value will be returned.
 *
 * @return The current value of the CCARISETHRDBM field in the RWNXAGCCCA1 register.
 */
__INLINE uint8_t agc_ccarisethrdbm_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCA1_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the CCARISETHRDBM field of the RWNXAGCCCA1 register.
 *
 * The RWNXAGCCCA1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ccarisethrdbm - The value to set the field to.
 */
__INLINE void agc_ccarisethrdbm_setf(uint8_t ccarisethrdbm)
{
    ASSERT_ERR((((uint32_t)ccarisethrdbm << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCA1_ADDR, (REG_PL_RD(AGC_RWNXAGCCCA1_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)ccarisethrdbm << 0));
}

/// @}

/**
 * @name RWNXAGCCCACTRL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:28         CCAFLAG3CTRL   0xA
 *  27:24         CCAFLAG2CTRL   0xA
 *  23:20         CCAFLAG1CTRL   0xA
 *  19:16         CCAFLAG0CTRL   0xA
 *     09             CCADEMOD   1
 *     08              CCACSEN   1
 *  06:04          CCARAMPUDEN   0x7
 *  02:00          CCAENERGYEN   0x7
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCCCACTRL register
#define AGC_RWNXAGCCCACTRL_ADDR   0x01002078
/// Offset of the RWNXAGCCCACTRL register from the base address
#define AGC_RWNXAGCCCACTRL_OFFSET 0x00002078
/// Index of the RWNXAGCCCACTRL register
#define AGC_RWNXAGCCCACTRL_INDEX  0x0000081E
/// Reset value of the RWNXAGCCCACTRL register
#define AGC_RWNXAGCCCACTRL_RESET  0xAAAA0377

/**
 * @brief Returns the current value of the RWNXAGCCCACTRL register.
 * The RWNXAGCCCACTRL register will be read and its value returned.
 * @return The current value of the RWNXAGCCCACTRL register.
 */
__INLINE uint32_t agc_rwnxagcccactrl_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCCCACTRL_ADDR);
}

/**
 * @brief Sets the RWNXAGCCCACTRL register to a value.
 * The RWNXAGCCCACTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcccactrl_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCCCACTRL_ADDR, value);
}

// field definitions
/// CCAFLAG3CTRL field mask
#define AGC_CCAFLAG3CTRL_MASK   ((uint32_t)0xF0000000)
/// CCAFLAG3CTRL field LSB position
#define AGC_CCAFLAG3CTRL_LSB    28
/// CCAFLAG3CTRL field width
#define AGC_CCAFLAG3CTRL_WIDTH  ((uint32_t)0x00000004)
/// CCAFLAG2CTRL field mask
#define AGC_CCAFLAG2CTRL_MASK   ((uint32_t)0x0F000000)
/// CCAFLAG2CTRL field LSB position
#define AGC_CCAFLAG2CTRL_LSB    24
/// CCAFLAG2CTRL field width
#define AGC_CCAFLAG2CTRL_WIDTH  ((uint32_t)0x00000004)
/// CCAFLAG1CTRL field mask
#define AGC_CCAFLAG1CTRL_MASK   ((uint32_t)0x00F00000)
/// CCAFLAG1CTRL field LSB position
#define AGC_CCAFLAG1CTRL_LSB    20
/// CCAFLAG1CTRL field width
#define AGC_CCAFLAG1CTRL_WIDTH  ((uint32_t)0x00000004)
/// CCAFLAG0CTRL field mask
#define AGC_CCAFLAG0CTRL_MASK   ((uint32_t)0x000F0000)
/// CCAFLAG0CTRL field LSB position
#define AGC_CCAFLAG0CTRL_LSB    16
/// CCAFLAG0CTRL field width
#define AGC_CCAFLAG0CTRL_WIDTH  ((uint32_t)0x00000004)
/// CCADEMOD field bit
#define AGC_CCADEMOD_BIT        ((uint32_t)0x00000200)
/// CCADEMOD field position
#define AGC_CCADEMOD_POS        9
/// CCACSEN field bit
#define AGC_CCACSEN_BIT         ((uint32_t)0x00000100)
/// CCACSEN field position
#define AGC_CCACSEN_POS         8
/// CCARAMPUDEN field mask
#define AGC_CCARAMPUDEN_MASK    ((uint32_t)0x00000070)
/// CCARAMPUDEN field LSB position
#define AGC_CCARAMPUDEN_LSB     4
/// CCARAMPUDEN field width
#define AGC_CCARAMPUDEN_WIDTH   ((uint32_t)0x00000003)
/// CCAENERGYEN field mask
#define AGC_CCAENERGYEN_MASK    ((uint32_t)0x00000007)
/// CCAENERGYEN field LSB position
#define AGC_CCAENERGYEN_LSB     0
/// CCAENERGYEN field width
#define AGC_CCAENERGYEN_WIDTH   ((uint32_t)0x00000003)

/// CCAFLAG3CTRL field reset value
#define AGC_CCAFLAG3CTRL_RST    0xA
/// CCAFLAG2CTRL field reset value
#define AGC_CCAFLAG2CTRL_RST    0xA
/// CCAFLAG1CTRL field reset value
#define AGC_CCAFLAG1CTRL_RST    0xA
/// CCAFLAG0CTRL field reset value
#define AGC_CCAFLAG0CTRL_RST    0xA
/// CCADEMOD field reset value
#define AGC_CCADEMOD_RST        0x1
/// CCACSEN field reset value
#define AGC_CCACSEN_RST         0x1
/// CCARAMPUDEN field reset value
#define AGC_CCARAMPUDEN_RST     0x7
/// CCAENERGYEN field reset value
#define AGC_CCAENERGYEN_RST     0x7

/**
 * @brief Constructs a value for the RWNXAGCCCACTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] ccaflag3ctrl - The value to use for the CCAFLAG3CTRL field.
 * @param[in] ccaflag2ctrl - The value to use for the CCAFLAG2CTRL field.
 * @param[in] ccaflag1ctrl - The value to use for the CCAFLAG1CTRL field.
 * @param[in] ccaflag0ctrl - The value to use for the CCAFLAG0CTRL field.
 * @param[in] ccademod - The value to use for the CCADEMOD field.
 * @param[in] ccacsen - The value to use for the CCACSEN field.
 * @param[in] ccarampuden - The value to use for the CCARAMPUDEN field.
 * @param[in] ccaenergyen - The value to use for the CCAENERGYEN field.
 */
__INLINE void agc_rwnxagcccactrl_pack(uint8_t ccaflag3ctrl, uint8_t ccaflag2ctrl, uint8_t ccaflag1ctrl, uint8_t ccaflag0ctrl, uint8_t ccademod, uint8_t ccacsen, uint8_t ccarampuden, uint8_t ccaenergyen)
{
    ASSERT_ERR((((uint32_t)ccaflag3ctrl << 28) & ~((uint32_t)0xF0000000)) == 0);
    ASSERT_ERR((((uint32_t)ccaflag2ctrl << 24) & ~((uint32_t)0x0F000000)) == 0);
    ASSERT_ERR((((uint32_t)ccaflag1ctrl << 20) & ~((uint32_t)0x00F00000)) == 0);
    ASSERT_ERR((((uint32_t)ccaflag0ctrl << 16) & ~((uint32_t)0x000F0000)) == 0);
    ASSERT_ERR((((uint32_t)ccademod << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)ccacsen << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)ccarampuden << 4) & ~((uint32_t)0x00000070)) == 0);
    ASSERT_ERR((((uint32_t)ccaenergyen << 0) & ~((uint32_t)0x00000007)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCACTRL_ADDR,  ((uint32_t)ccaflag3ctrl << 28) | ((uint32_t)ccaflag2ctrl << 24) | ((uint32_t)ccaflag1ctrl << 20) | ((uint32_t)ccaflag0ctrl << 16) | ((uint32_t)ccademod << 9) | ((uint32_t)ccacsen << 8) | ((uint32_t)ccarampuden << 4) | ((uint32_t)ccaenergyen << 0));
}

/**
 * @brief Unpacks RWNXAGCCCACTRL's fields from current value of the RWNXAGCCCACTRL register.
 *
 * Reads the RWNXAGCCCACTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] ccaflag3ctrl - Will be populated with the current value of this field from the register.
 * @param[out] ccaflag2ctrl - Will be populated with the current value of this field from the register.
 * @param[out] ccaflag1ctrl - Will be populated with the current value of this field from the register.
 * @param[out] ccaflag0ctrl - Will be populated with the current value of this field from the register.
 * @param[out] ccademod - Will be populated with the current value of this field from the register.
 * @param[out] ccacsen - Will be populated with the current value of this field from the register.
 * @param[out] ccarampuden - Will be populated with the current value of this field from the register.
 * @param[out] ccaenergyen - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcccactrl_unpack(uint8_t *ccaflag3ctrl, uint8_t *ccaflag2ctrl, uint8_t *ccaflag1ctrl, uint8_t *ccaflag0ctrl, uint8_t *ccademod, uint8_t *ccacsen, uint8_t *ccarampuden, uint8_t *ccaenergyen)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCACTRL_ADDR);

    *ccaflag3ctrl = (localVal & ((uint32_t)0xF0000000)) >> 28;
    *ccaflag2ctrl = (localVal & ((uint32_t)0x0F000000)) >> 24;
    *ccaflag1ctrl = (localVal & ((uint32_t)0x00F00000)) >> 20;
    *ccaflag0ctrl = (localVal & ((uint32_t)0x000F0000)) >> 16;
    *ccademod = (localVal & ((uint32_t)0x00000200)) >> 9;
    *ccacsen = (localVal & ((uint32_t)0x00000100)) >> 8;
    *ccarampuden = (localVal & ((uint32_t)0x00000070)) >> 4;
    *ccaenergyen = (localVal & ((uint32_t)0x00000007)) >> 0;
}

/**
 * @brief Returns the current value of the CCAFLAG3CTRL field in the RWNXAGCCCACTRL register.
 *
 * The RWNXAGCCCACTRL register will be read and the CCAFLAG3CTRL field's value will be returned.
 *
 * @return The current value of the CCAFLAG3CTRL field in the RWNXAGCCCACTRL register.
 */
__INLINE uint8_t agc_ccaflag3ctrl_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCACTRL_ADDR);
    return ((localVal & ((uint32_t)0xF0000000)) >> 28);
}

/**
 * @brief Sets the CCAFLAG3CTRL field of the RWNXAGCCCACTRL register.
 *
 * The RWNXAGCCCACTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ccaflag3ctrl - The value to set the field to.
 */
__INLINE void agc_ccaflag3ctrl_setf(uint8_t ccaflag3ctrl)
{
    ASSERT_ERR((((uint32_t)ccaflag3ctrl << 28) & ~((uint32_t)0xF0000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCACTRL_ADDR, (REG_PL_RD(AGC_RWNXAGCCCACTRL_ADDR) & ~((uint32_t)0xF0000000)) | ((uint32_t)ccaflag3ctrl << 28));
}

/**
 * @brief Returns the current value of the CCAFLAG2CTRL field in the RWNXAGCCCACTRL register.
 *
 * The RWNXAGCCCACTRL register will be read and the CCAFLAG2CTRL field's value will be returned.
 *
 * @return The current value of the CCAFLAG2CTRL field in the RWNXAGCCCACTRL register.
 */
__INLINE uint8_t agc_ccaflag2ctrl_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCACTRL_ADDR);
    return ((localVal & ((uint32_t)0x0F000000)) >> 24);
}

/**
 * @brief Sets the CCAFLAG2CTRL field of the RWNXAGCCCACTRL register.
 *
 * The RWNXAGCCCACTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ccaflag2ctrl - The value to set the field to.
 */
__INLINE void agc_ccaflag2ctrl_setf(uint8_t ccaflag2ctrl)
{
    ASSERT_ERR((((uint32_t)ccaflag2ctrl << 24) & ~((uint32_t)0x0F000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCACTRL_ADDR, (REG_PL_RD(AGC_RWNXAGCCCACTRL_ADDR) & ~((uint32_t)0x0F000000)) | ((uint32_t)ccaflag2ctrl << 24));
}

/**
 * @brief Returns the current value of the CCAFLAG1CTRL field in the RWNXAGCCCACTRL register.
 *
 * The RWNXAGCCCACTRL register will be read and the CCAFLAG1CTRL field's value will be returned.
 *
 * @return The current value of the CCAFLAG1CTRL field in the RWNXAGCCCACTRL register.
 */
__INLINE uint8_t agc_ccaflag1ctrl_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCACTRL_ADDR);
    return ((localVal & ((uint32_t)0x00F00000)) >> 20);
}

/**
 * @brief Sets the CCAFLAG1CTRL field of the RWNXAGCCCACTRL register.
 *
 * The RWNXAGCCCACTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ccaflag1ctrl - The value to set the field to.
 */
__INLINE void agc_ccaflag1ctrl_setf(uint8_t ccaflag1ctrl)
{
    ASSERT_ERR((((uint32_t)ccaflag1ctrl << 20) & ~((uint32_t)0x00F00000)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCACTRL_ADDR, (REG_PL_RD(AGC_RWNXAGCCCACTRL_ADDR) & ~((uint32_t)0x00F00000)) | ((uint32_t)ccaflag1ctrl << 20));
}

/**
 * @brief Returns the current value of the CCAFLAG0CTRL field in the RWNXAGCCCACTRL register.
 *
 * The RWNXAGCCCACTRL register will be read and the CCAFLAG0CTRL field's value will be returned.
 *
 * @return The current value of the CCAFLAG0CTRL field in the RWNXAGCCCACTRL register.
 */
__INLINE uint8_t agc_ccaflag0ctrl_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCACTRL_ADDR);
    return ((localVal & ((uint32_t)0x000F0000)) >> 16);
}

/**
 * @brief Sets the CCAFLAG0CTRL field of the RWNXAGCCCACTRL register.
 *
 * The RWNXAGCCCACTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ccaflag0ctrl - The value to set the field to.
 */
__INLINE void agc_ccaflag0ctrl_setf(uint8_t ccaflag0ctrl)
{
    ASSERT_ERR((((uint32_t)ccaflag0ctrl << 16) & ~((uint32_t)0x000F0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCACTRL_ADDR, (REG_PL_RD(AGC_RWNXAGCCCACTRL_ADDR) & ~((uint32_t)0x000F0000)) | ((uint32_t)ccaflag0ctrl << 16));
}

/**
 * @brief Returns the current value of the CCADEMOD field in the RWNXAGCCCACTRL register.
 *
 * The RWNXAGCCCACTRL register will be read and the CCADEMOD field's value will be returned.
 *
 * @return The current value of the CCADEMOD field in the RWNXAGCCCACTRL register.
 */
__INLINE uint8_t agc_ccademod_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCACTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the CCADEMOD field of the RWNXAGCCCACTRL register.
 *
 * The RWNXAGCCCACTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ccademod - The value to set the field to.
 */
__INLINE void agc_ccademod_setf(uint8_t ccademod)
{
    ASSERT_ERR((((uint32_t)ccademod << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCACTRL_ADDR, (REG_PL_RD(AGC_RWNXAGCCCACTRL_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)ccademod << 9));
}

/**
 * @brief Returns the current value of the CCACSEN field in the RWNXAGCCCACTRL register.
 *
 * The RWNXAGCCCACTRL register will be read and the CCACSEN field's value will be returned.
 *
 * @return The current value of the CCACSEN field in the RWNXAGCCCACTRL register.
 */
__INLINE uint8_t agc_ccacsen_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCACTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the CCACSEN field of the RWNXAGCCCACTRL register.
 *
 * The RWNXAGCCCACTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ccacsen - The value to set the field to.
 */
__INLINE void agc_ccacsen_setf(uint8_t ccacsen)
{
    ASSERT_ERR((((uint32_t)ccacsen << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCACTRL_ADDR, (REG_PL_RD(AGC_RWNXAGCCCACTRL_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)ccacsen << 8));
}

/**
 * @brief Returns the current value of the CCARAMPUDEN field in the RWNXAGCCCACTRL register.
 *
 * The RWNXAGCCCACTRL register will be read and the CCARAMPUDEN field's value will be returned.
 *
 * @return The current value of the CCARAMPUDEN field in the RWNXAGCCCACTRL register.
 */
__INLINE uint8_t agc_ccarampuden_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCACTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

/**
 * @brief Sets the CCARAMPUDEN field of the RWNXAGCCCACTRL register.
 *
 * The RWNXAGCCCACTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ccarampuden - The value to set the field to.
 */
__INLINE void agc_ccarampuden_setf(uint8_t ccarampuden)
{
    ASSERT_ERR((((uint32_t)ccarampuden << 4) & ~((uint32_t)0x00000070)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCACTRL_ADDR, (REG_PL_RD(AGC_RWNXAGCCCACTRL_ADDR) & ~((uint32_t)0x00000070)) | ((uint32_t)ccarampuden << 4));
}

/**
 * @brief Returns the current value of the CCAENERGYEN field in the RWNXAGCCCACTRL register.
 *
 * The RWNXAGCCCACTRL register will be read and the CCAENERGYEN field's value will be returned.
 *
 * @return The current value of the CCAENERGYEN field in the RWNXAGCCCACTRL register.
 */
__INLINE uint8_t agc_ccaenergyen_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCACTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

/**
 * @brief Sets the CCAENERGYEN field of the RWNXAGCCCACTRL register.
 *
 * The RWNXAGCCCACTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ccaenergyen - The value to set the field to.
 */
__INLINE void agc_ccaenergyen_setf(uint8_t ccaenergyen)
{
    ASSERT_ERR((((uint32_t)ccaenergyen << 0) & ~((uint32_t)0x00000007)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCACTRL_ADDR, (REG_PL_RD(AGC_RWNXAGCCCACTRL_ADDR) & ~((uint32_t)0x00000007)) | ((uint32_t)ccaenergyen << 0));
}

/// @}

/**
 * @name RWNXAGCCCASTATE0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  26:16     RXSTATECCA20SSEL   0xD4
 *  10:00     RXSTATECCA20PSEL   0x6FE
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCCCASTATE0 register
#define AGC_RWNXAGCCCASTATE0_ADDR   0x0100207C
/// Offset of the RWNXAGCCCASTATE0 register from the base address
#define AGC_RWNXAGCCCASTATE0_OFFSET 0x0000207C
/// Index of the RWNXAGCCCASTATE0 register
#define AGC_RWNXAGCCCASTATE0_INDEX  0x0000081F
/// Reset value of the RWNXAGCCCASTATE0 register
#define AGC_RWNXAGCCCASTATE0_RESET  0x00D406FE

/**
 * @brief Returns the current value of the RWNXAGCCCASTATE0 register.
 * The RWNXAGCCCASTATE0 register will be read and its value returned.
 * @return The current value of the RWNXAGCCCASTATE0 register.
 */
__INLINE uint32_t agc_rwnxagcccastate0_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCCCASTATE0_ADDR);
}

/**
 * @brief Sets the RWNXAGCCCASTATE0 register to a value.
 * The RWNXAGCCCASTATE0 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcccastate0_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCCCASTATE0_ADDR, value);
}

// field definitions
/// RXSTATECCA20SSEL field mask
#define AGC_RXSTATECCA20SSEL_MASK   ((uint32_t)0x07FF0000)
/// RXSTATECCA20SSEL field LSB position
#define AGC_RXSTATECCA20SSEL_LSB    16
/// RXSTATECCA20SSEL field width
#define AGC_RXSTATECCA20SSEL_WIDTH  ((uint32_t)0x0000000B)
/// RXSTATECCA20PSEL field mask
#define AGC_RXSTATECCA20PSEL_MASK   ((uint32_t)0x000007FF)
/// RXSTATECCA20PSEL field LSB position
#define AGC_RXSTATECCA20PSEL_LSB    0
/// RXSTATECCA20PSEL field width
#define AGC_RXSTATECCA20PSEL_WIDTH  ((uint32_t)0x0000000B)

/// RXSTATECCA20SSEL field reset value
#define AGC_RXSTATECCA20SSEL_RST    0xD4
/// RXSTATECCA20PSEL field reset value
#define AGC_RXSTATECCA20PSEL_RST    0x6FE

/**
 * @brief Constructs a value for the RWNXAGCCCASTATE0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxstatecca20ssel - The value to use for the RXSTATECCA20SSEL field.
 * @param[in] rxstatecca20psel - The value to use for the RXSTATECCA20PSEL field.
 */
__INLINE void agc_rwnxagcccastate0_pack(uint16_t rxstatecca20ssel, uint16_t rxstatecca20psel)
{
    ASSERT_ERR((((uint32_t)rxstatecca20ssel << 16) & ~((uint32_t)0x07FF0000)) == 0);
    ASSERT_ERR((((uint32_t)rxstatecca20psel << 0) & ~((uint32_t)0x000007FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCASTATE0_ADDR,  ((uint32_t)rxstatecca20ssel << 16) | ((uint32_t)rxstatecca20psel << 0));
}

/**
 * @brief Unpacks RWNXAGCCCASTATE0's fields from current value of the RWNXAGCCCASTATE0 register.
 *
 * Reads the RWNXAGCCCASTATE0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxstatecca20ssel - Will be populated with the current value of this field from the register.
 * @param[out] rxstatecca20psel - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcccastate0_unpack(uint16_t *rxstatecca20ssel, uint16_t *rxstatecca20psel)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCASTATE0_ADDR);

    *rxstatecca20ssel = (localVal & ((uint32_t)0x07FF0000)) >> 16;
    *rxstatecca20psel = (localVal & ((uint32_t)0x000007FF)) >> 0;
}

/**
 * @brief Returns the current value of the RXSTATECCA20SSEL field in the RWNXAGCCCASTATE0 register.
 *
 * The RWNXAGCCCASTATE0 register will be read and the RXSTATECCA20SSEL field's value will be returned.
 *
 * @return The current value of the RXSTATECCA20SSEL field in the RWNXAGCCCASTATE0 register.
 */
__INLINE uint16_t agc_rxstatecca20ssel_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCASTATE0_ADDR);
    return ((localVal & ((uint32_t)0x07FF0000)) >> 16);
}

/**
 * @brief Sets the RXSTATECCA20SSEL field of the RWNXAGCCCASTATE0 register.
 *
 * The RWNXAGCCCASTATE0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxstatecca20ssel - The value to set the field to.
 */
__INLINE void agc_rxstatecca20ssel_setf(uint16_t rxstatecca20ssel)
{
    ASSERT_ERR((((uint32_t)rxstatecca20ssel << 16) & ~((uint32_t)0x07FF0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCASTATE0_ADDR, (REG_PL_RD(AGC_RWNXAGCCCASTATE0_ADDR) & ~((uint32_t)0x07FF0000)) | ((uint32_t)rxstatecca20ssel << 16));
}

/**
 * @brief Returns the current value of the RXSTATECCA20PSEL field in the RWNXAGCCCASTATE0 register.
 *
 * The RWNXAGCCCASTATE0 register will be read and the RXSTATECCA20PSEL field's value will be returned.
 *
 * @return The current value of the RXSTATECCA20PSEL field in the RWNXAGCCCASTATE0 register.
 */
__INLINE uint16_t agc_rxstatecca20psel_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCASTATE0_ADDR);
    return ((localVal & ((uint32_t)0x000007FF)) >> 0);
}

/**
 * @brief Sets the RXSTATECCA20PSEL field of the RWNXAGCCCASTATE0 register.
 *
 * The RWNXAGCCCASTATE0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxstatecca20psel - The value to set the field to.
 */
__INLINE void agc_rxstatecca20psel_setf(uint16_t rxstatecca20psel)
{
    ASSERT_ERR((((uint32_t)rxstatecca20psel << 0) & ~((uint32_t)0x000007FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCASTATE0_ADDR, (REG_PL_RD(AGC_RWNXAGCCCASTATE0_ADDR) & ~((uint32_t)0x000007FF)) | ((uint32_t)rxstatecca20psel << 0));
}

/// @}

/**
 * @name RWNXAGCCCASTATE1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  10:00     RXSTATECCA40SSEL   0x80
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCCCASTATE1 register
#define AGC_RWNXAGCCCASTATE1_ADDR   0x01002080
/// Offset of the RWNXAGCCCASTATE1 register from the base address
#define AGC_RWNXAGCCCASTATE1_OFFSET 0x00002080
/// Index of the RWNXAGCCCASTATE1 register
#define AGC_RWNXAGCCCASTATE1_INDEX  0x00000820
/// Reset value of the RWNXAGCCCASTATE1 register
#define AGC_RWNXAGCCCASTATE1_RESET  0x00000080

/**
 * @brief Returns the current value of the RWNXAGCCCASTATE1 register.
 * The RWNXAGCCCASTATE1 register will be read and its value returned.
 * @return The current value of the RWNXAGCCCASTATE1 register.
 */
__INLINE uint32_t agc_rwnxagcccastate1_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCCCASTATE1_ADDR);
}

/**
 * @brief Sets the RWNXAGCCCASTATE1 register to a value.
 * The RWNXAGCCCASTATE1 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcccastate1_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCCCASTATE1_ADDR, value);
}

// field definitions
/// RXSTATECCA40SSEL field mask
#define AGC_RXSTATECCA40SSEL_MASK   ((uint32_t)0x000007FF)
/// RXSTATECCA40SSEL field LSB position
#define AGC_RXSTATECCA40SSEL_LSB    0
/// RXSTATECCA40SSEL field width
#define AGC_RXSTATECCA40SSEL_WIDTH  ((uint32_t)0x0000000B)

/// RXSTATECCA40SSEL field reset value
#define AGC_RXSTATECCA40SSEL_RST    0x80

/**
 * @brief Returns the current value of the RXSTATECCA40SSEL field in the RWNXAGCCCASTATE1 register.
 *
 * The RWNXAGCCCASTATE1 register will be read and the RXSTATECCA40SSEL field's value will be returned.
 *
 * @return The current value of the RXSTATECCA40SSEL field in the RWNXAGCCCASTATE1 register.
 */
__INLINE uint16_t agc_rxstatecca40ssel_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCASTATE1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000007FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the RXSTATECCA40SSEL field of the RWNXAGCCCASTATE1 register.
 *
 * The RWNXAGCCCASTATE1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxstatecca40ssel - The value to set the field to.
 */
__INLINE void agc_rxstatecca40ssel_setf(uint16_t rxstatecca40ssel)
{
    ASSERT_ERR((((uint32_t)rxstatecca40ssel << 0) & ~((uint32_t)0x000007FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCASTATE1_ADDR, (uint32_t)rxstatecca40ssel << 0);
}

/// @}

/**
 * @name RWNXAGCCCATIMEOUT register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00           CCATIMEOUT   0x9312A
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCCCATIMEOUT register
#define AGC_RWNXAGCCCATIMEOUT_ADDR   0x01002084
/// Offset of the RWNXAGCCCATIMEOUT register from the base address
#define AGC_RWNXAGCCCATIMEOUT_OFFSET 0x00002084
/// Index of the RWNXAGCCCATIMEOUT register
#define AGC_RWNXAGCCCATIMEOUT_INDEX  0x00000821
/// Reset value of the RWNXAGCCCATIMEOUT register
#define AGC_RWNXAGCCCATIMEOUT_RESET  0x0009312A

/**
 * @brief Returns the current value of the RWNXAGCCCATIMEOUT register.
 * The RWNXAGCCCATIMEOUT register will be read and its value returned.
 * @return The current value of the RWNXAGCCCATIMEOUT register.
 */
__INLINE uint32_t agc_rwnxagcccatimeout_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCCCATIMEOUT_ADDR);
}

/**
 * @brief Sets the RWNXAGCCCATIMEOUT register to a value.
 * The RWNXAGCCCATIMEOUT register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcccatimeout_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCCCATIMEOUT_ADDR, value);
}

// field definitions
/// CCATIMEOUT field mask
#define AGC_CCATIMEOUT_MASK   ((uint32_t)0xFFFFFFFF)
/// CCATIMEOUT field LSB position
#define AGC_CCATIMEOUT_LSB    0
/// CCATIMEOUT field width
#define AGC_CCATIMEOUT_WIDTH  ((uint32_t)0x00000020)

/// CCATIMEOUT field reset value
#define AGC_CCATIMEOUT_RST    0x9312A

/**
 * @brief Returns the current value of the CCATIMEOUT field in the RWNXAGCCCATIMEOUT register.
 *
 * The RWNXAGCCCATIMEOUT register will be read and the CCATIMEOUT field's value will be returned.
 *
 * @return The current value of the CCATIMEOUT field in the RWNXAGCCCATIMEOUT register.
 */
__INLINE uint32_t agc_ccatimeout_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCCATIMEOUT_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the CCATIMEOUT field of the RWNXAGCCCATIMEOUT register.
 *
 * The RWNXAGCCCATIMEOUT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ccatimeout - The value to set the field to.
 */
__INLINE void agc_ccatimeout_setf(uint32_t ccatimeout)
{
    ASSERT_ERR((((uint32_t)ccatimeout << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(AGC_RWNXAGCCCATIMEOUT_ADDR, (uint32_t)ccatimeout << 0);
}

/// @}

/**
 * @name RWNXAGCSNR0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  23:20           NFVGATHRM2   0xA
 *  19:16           NFVGATHRM1   0x8
 *  15:12           NFVGATHRM0   0x6
 *  11:08           NFVGATHRL2   0xA
 *  07:04           NFVGATHRL1   0x8
 *  03:00           NFVGATHRL0   0x6
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCSNR0 register
#define AGC_RWNXAGCSNR0_ADDR   0x01002088
/// Offset of the RWNXAGCSNR0 register from the base address
#define AGC_RWNXAGCSNR0_OFFSET 0x00002088
/// Index of the RWNXAGCSNR0 register
#define AGC_RWNXAGCSNR0_INDEX  0x00000822
/// Reset value of the RWNXAGCSNR0 register
#define AGC_RWNXAGCSNR0_RESET  0x00A86A86

/**
 * @brief Returns the current value of the RWNXAGCSNR0 register.
 * The RWNXAGCSNR0 register will be read and its value returned.
 * @return The current value of the RWNXAGCSNR0 register.
 */
__INLINE uint32_t agc_rwnxagcsnr0_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCSNR0_ADDR);
}

/**
 * @brief Sets the RWNXAGCSNR0 register to a value.
 * The RWNXAGCSNR0 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcsnr0_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCSNR0_ADDR, value);
}

// field definitions
/// NFVGATHRM2 field mask
#define AGC_NFVGATHRM2_MASK   ((uint32_t)0x00F00000)
/// NFVGATHRM2 field LSB position
#define AGC_NFVGATHRM2_LSB    20
/// NFVGATHRM2 field width
#define AGC_NFVGATHRM2_WIDTH  ((uint32_t)0x00000004)
/// NFVGATHRM1 field mask
#define AGC_NFVGATHRM1_MASK   ((uint32_t)0x000F0000)
/// NFVGATHRM1 field LSB position
#define AGC_NFVGATHRM1_LSB    16
/// NFVGATHRM1 field width
#define AGC_NFVGATHRM1_WIDTH  ((uint32_t)0x00000004)
/// NFVGATHRM0 field mask
#define AGC_NFVGATHRM0_MASK   ((uint32_t)0x0000F000)
/// NFVGATHRM0 field LSB position
#define AGC_NFVGATHRM0_LSB    12
/// NFVGATHRM0 field width
#define AGC_NFVGATHRM0_WIDTH  ((uint32_t)0x00000004)
/// NFVGATHRL2 field mask
#define AGC_NFVGATHRL2_MASK   ((uint32_t)0x00000F00)
/// NFVGATHRL2 field LSB position
#define AGC_NFVGATHRL2_LSB    8
/// NFVGATHRL2 field width
#define AGC_NFVGATHRL2_WIDTH  ((uint32_t)0x00000004)
/// NFVGATHRL1 field mask
#define AGC_NFVGATHRL1_MASK   ((uint32_t)0x000000F0)
/// NFVGATHRL1 field LSB position
#define AGC_NFVGATHRL1_LSB    4
/// NFVGATHRL1 field width
#define AGC_NFVGATHRL1_WIDTH  ((uint32_t)0x00000004)
/// NFVGATHRL0 field mask
#define AGC_NFVGATHRL0_MASK   ((uint32_t)0x0000000F)
/// NFVGATHRL0 field LSB position
#define AGC_NFVGATHRL0_LSB    0
/// NFVGATHRL0 field width
#define AGC_NFVGATHRL0_WIDTH  ((uint32_t)0x00000004)

/// NFVGATHRM2 field reset value
#define AGC_NFVGATHRM2_RST    0xA
/// NFVGATHRM1 field reset value
#define AGC_NFVGATHRM1_RST    0x8
/// NFVGATHRM0 field reset value
#define AGC_NFVGATHRM0_RST    0x6
/// NFVGATHRL2 field reset value
#define AGC_NFVGATHRL2_RST    0xA
/// NFVGATHRL1 field reset value
#define AGC_NFVGATHRL1_RST    0x8
/// NFVGATHRL0 field reset value
#define AGC_NFVGATHRL0_RST    0x6

/**
 * @brief Constructs a value for the RWNXAGCSNR0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] nfvgathrm2 - The value to use for the NFVGATHRM2 field.
 * @param[in] nfvgathrm1 - The value to use for the NFVGATHRM1 field.
 * @param[in] nfvgathrm0 - The value to use for the NFVGATHRM0 field.
 * @param[in] nfvgathrl2 - The value to use for the NFVGATHRL2 field.
 * @param[in] nfvgathrl1 - The value to use for the NFVGATHRL1 field.
 * @param[in] nfvgathrl0 - The value to use for the NFVGATHRL0 field.
 */
__INLINE void agc_rwnxagcsnr0_pack(uint8_t nfvgathrm2, uint8_t nfvgathrm1, uint8_t nfvgathrm0, uint8_t nfvgathrl2, uint8_t nfvgathrl1, uint8_t nfvgathrl0)
{
    ASSERT_ERR((((uint32_t)nfvgathrm2 << 20) & ~((uint32_t)0x00F00000)) == 0);
    ASSERT_ERR((((uint32_t)nfvgathrm1 << 16) & ~((uint32_t)0x000F0000)) == 0);
    ASSERT_ERR((((uint32_t)nfvgathrm0 << 12) & ~((uint32_t)0x0000F000)) == 0);
    ASSERT_ERR((((uint32_t)nfvgathrl2 << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)nfvgathrl1 << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)nfvgathrl0 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR0_ADDR,  ((uint32_t)nfvgathrm2 << 20) | ((uint32_t)nfvgathrm1 << 16) | ((uint32_t)nfvgathrm0 << 12) | ((uint32_t)nfvgathrl2 << 8) | ((uint32_t)nfvgathrl1 << 4) | ((uint32_t)nfvgathrl0 << 0));
}

/**
 * @brief Unpacks RWNXAGCSNR0's fields from current value of the RWNXAGCSNR0 register.
 *
 * Reads the RWNXAGCSNR0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] nfvgathrm2 - Will be populated with the current value of this field from the register.
 * @param[out] nfvgathrm1 - Will be populated with the current value of this field from the register.
 * @param[out] nfvgathrm0 - Will be populated with the current value of this field from the register.
 * @param[out] nfvgathrl2 - Will be populated with the current value of this field from the register.
 * @param[out] nfvgathrl1 - Will be populated with the current value of this field from the register.
 * @param[out] nfvgathrl0 - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcsnr0_unpack(uint8_t *nfvgathrm2, uint8_t *nfvgathrm1, uint8_t *nfvgathrm0, uint8_t *nfvgathrl2, uint8_t *nfvgathrl1, uint8_t *nfvgathrl0)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR0_ADDR);

    *nfvgathrm2 = (localVal & ((uint32_t)0x00F00000)) >> 20;
    *nfvgathrm1 = (localVal & ((uint32_t)0x000F0000)) >> 16;
    *nfvgathrm0 = (localVal & ((uint32_t)0x0000F000)) >> 12;
    *nfvgathrl2 = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *nfvgathrl1 = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *nfvgathrl0 = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the NFVGATHRM2 field in the RWNXAGCSNR0 register.
 *
 * The RWNXAGCSNR0 register will be read and the NFVGATHRM2 field's value will be returned.
 *
 * @return The current value of the NFVGATHRM2 field in the RWNXAGCSNR0 register.
 */
__INLINE uint8_t agc_nfvgathrm2_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR0_ADDR);
    return ((localVal & ((uint32_t)0x00F00000)) >> 20);
}

/**
 * @brief Sets the NFVGATHRM2 field of the RWNXAGCSNR0 register.
 *
 * The RWNXAGCSNR0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvgathrm2 - The value to set the field to.
 */
__INLINE void agc_nfvgathrm2_setf(uint8_t nfvgathrm2)
{
    ASSERT_ERR((((uint32_t)nfvgathrm2 << 20) & ~((uint32_t)0x00F00000)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR0_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR0_ADDR) & ~((uint32_t)0x00F00000)) | ((uint32_t)nfvgathrm2 << 20));
}

/**
 * @brief Returns the current value of the NFVGATHRM1 field in the RWNXAGCSNR0 register.
 *
 * The RWNXAGCSNR0 register will be read and the NFVGATHRM1 field's value will be returned.
 *
 * @return The current value of the NFVGATHRM1 field in the RWNXAGCSNR0 register.
 */
__INLINE uint8_t agc_nfvgathrm1_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR0_ADDR);
    return ((localVal & ((uint32_t)0x000F0000)) >> 16);
}

/**
 * @brief Sets the NFVGATHRM1 field of the RWNXAGCSNR0 register.
 *
 * The RWNXAGCSNR0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvgathrm1 - The value to set the field to.
 */
__INLINE void agc_nfvgathrm1_setf(uint8_t nfvgathrm1)
{
    ASSERT_ERR((((uint32_t)nfvgathrm1 << 16) & ~((uint32_t)0x000F0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR0_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR0_ADDR) & ~((uint32_t)0x000F0000)) | ((uint32_t)nfvgathrm1 << 16));
}

/**
 * @brief Returns the current value of the NFVGATHRM0 field in the RWNXAGCSNR0 register.
 *
 * The RWNXAGCSNR0 register will be read and the NFVGATHRM0 field's value will be returned.
 *
 * @return The current value of the NFVGATHRM0 field in the RWNXAGCSNR0 register.
 */
__INLINE uint8_t agc_nfvgathrm0_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR0_ADDR);
    return ((localVal & ((uint32_t)0x0000F000)) >> 12);
}

/**
 * @brief Sets the NFVGATHRM0 field of the RWNXAGCSNR0 register.
 *
 * The RWNXAGCSNR0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvgathrm0 - The value to set the field to.
 */
__INLINE void agc_nfvgathrm0_setf(uint8_t nfvgathrm0)
{
    ASSERT_ERR((((uint32_t)nfvgathrm0 << 12) & ~((uint32_t)0x0000F000)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR0_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR0_ADDR) & ~((uint32_t)0x0000F000)) | ((uint32_t)nfvgathrm0 << 12));
}

/**
 * @brief Returns the current value of the NFVGATHRL2 field in the RWNXAGCSNR0 register.
 *
 * The RWNXAGCSNR0 register will be read and the NFVGATHRL2 field's value will be returned.
 *
 * @return The current value of the NFVGATHRL2 field in the RWNXAGCSNR0 register.
 */
__INLINE uint8_t agc_nfvgathrl2_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR0_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the NFVGATHRL2 field of the RWNXAGCSNR0 register.
 *
 * The RWNXAGCSNR0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvgathrl2 - The value to set the field to.
 */
__INLINE void agc_nfvgathrl2_setf(uint8_t nfvgathrl2)
{
    ASSERT_ERR((((uint32_t)nfvgathrl2 << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR0_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR0_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)nfvgathrl2 << 8));
}

/**
 * @brief Returns the current value of the NFVGATHRL1 field in the RWNXAGCSNR0 register.
 *
 * The RWNXAGCSNR0 register will be read and the NFVGATHRL1 field's value will be returned.
 *
 * @return The current value of the NFVGATHRL1 field in the RWNXAGCSNR0 register.
 */
__INLINE uint8_t agc_nfvgathrl1_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR0_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the NFVGATHRL1 field of the RWNXAGCSNR0 register.
 *
 * The RWNXAGCSNR0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvgathrl1 - The value to set the field to.
 */
__INLINE void agc_nfvgathrl1_setf(uint8_t nfvgathrl1)
{
    ASSERT_ERR((((uint32_t)nfvgathrl1 << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR0_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR0_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)nfvgathrl1 << 4));
}

/**
 * @brief Returns the current value of the NFVGATHRL0 field in the RWNXAGCSNR0 register.
 *
 * The RWNXAGCSNR0 register will be read and the NFVGATHRL0 field's value will be returned.
 *
 * @return The current value of the NFVGATHRL0 field in the RWNXAGCSNR0 register.
 */
__INLINE uint8_t agc_nfvgathrl0_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR0_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the NFVGATHRL0 field of the RWNXAGCSNR0 register.
 *
 * The RWNXAGCSNR0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvgathrl0 - The value to set the field to.
 */
__INLINE void agc_nfvgathrl0_setf(uint8_t nfvgathrl0)
{
    ASSERT_ERR((((uint32_t)nfvgathrl0 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR0_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR0_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)nfvgathrl0 << 0));
}

/// @}

/**
 * @name RWNXAGCSNR1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     24           SNRMEASSEL   0
 *  11:08           NFVGATHRH2   0xA
 *  07:04           NFVGATHRH1   0x8
 *  03:00           NFVGATHRH0   0x6
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCSNR1 register
#define AGC_RWNXAGCSNR1_ADDR   0x0100208C
/// Offset of the RWNXAGCSNR1 register from the base address
#define AGC_RWNXAGCSNR1_OFFSET 0x0000208C
/// Index of the RWNXAGCSNR1 register
#define AGC_RWNXAGCSNR1_INDEX  0x00000823
/// Reset value of the RWNXAGCSNR1 register
#define AGC_RWNXAGCSNR1_RESET  0x00000A86

/**
 * @brief Returns the current value of the RWNXAGCSNR1 register.
 * The RWNXAGCSNR1 register will be read and its value returned.
 * @return The current value of the RWNXAGCSNR1 register.
 */
__INLINE uint32_t agc_rwnxagcsnr1_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCSNR1_ADDR);
}

/**
 * @brief Sets the RWNXAGCSNR1 register to a value.
 * The RWNXAGCSNR1 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcsnr1_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCSNR1_ADDR, value);
}

// field definitions
/// SNRMEASSEL field bit
#define AGC_SNRMEASSEL_BIT    ((uint32_t)0x01000000)
/// SNRMEASSEL field position
#define AGC_SNRMEASSEL_POS    24
/// NFVGATHRH2 field mask
#define AGC_NFVGATHRH2_MASK   ((uint32_t)0x00000F00)
/// NFVGATHRH2 field LSB position
#define AGC_NFVGATHRH2_LSB    8
/// NFVGATHRH2 field width
#define AGC_NFVGATHRH2_WIDTH  ((uint32_t)0x00000004)
/// NFVGATHRH1 field mask
#define AGC_NFVGATHRH1_MASK   ((uint32_t)0x000000F0)
/// NFVGATHRH1 field LSB position
#define AGC_NFVGATHRH1_LSB    4
/// NFVGATHRH1 field width
#define AGC_NFVGATHRH1_WIDTH  ((uint32_t)0x00000004)
/// NFVGATHRH0 field mask
#define AGC_NFVGATHRH0_MASK   ((uint32_t)0x0000000F)
/// NFVGATHRH0 field LSB position
#define AGC_NFVGATHRH0_LSB    0
/// NFVGATHRH0 field width
#define AGC_NFVGATHRH0_WIDTH  ((uint32_t)0x00000004)

/// SNRMEASSEL field reset value
#define AGC_SNRMEASSEL_RST    0x0
/// NFVGATHRH2 field reset value
#define AGC_NFVGATHRH2_RST    0xA
/// NFVGATHRH1 field reset value
#define AGC_NFVGATHRH1_RST    0x8
/// NFVGATHRH0 field reset value
#define AGC_NFVGATHRH0_RST    0x6

/**
 * @brief Constructs a value for the RWNXAGCSNR1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] snrmeassel - The value to use for the SNRMEASSEL field.
 * @param[in] nfvgathrh2 - The value to use for the NFVGATHRH2 field.
 * @param[in] nfvgathrh1 - The value to use for the NFVGATHRH1 field.
 * @param[in] nfvgathrh0 - The value to use for the NFVGATHRH0 field.
 */
__INLINE void agc_rwnxagcsnr1_pack(uint8_t snrmeassel, uint8_t nfvgathrh2, uint8_t nfvgathrh1, uint8_t nfvgathrh0)
{
    ASSERT_ERR((((uint32_t)snrmeassel << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)nfvgathrh2 << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)nfvgathrh1 << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)nfvgathrh0 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR1_ADDR,  ((uint32_t)snrmeassel << 24) | ((uint32_t)nfvgathrh2 << 8) | ((uint32_t)nfvgathrh1 << 4) | ((uint32_t)nfvgathrh0 << 0));
}

/**
 * @brief Unpacks RWNXAGCSNR1's fields from current value of the RWNXAGCSNR1 register.
 *
 * Reads the RWNXAGCSNR1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] snrmeassel - Will be populated with the current value of this field from the register.
 * @param[out] nfvgathrh2 - Will be populated with the current value of this field from the register.
 * @param[out] nfvgathrh1 - Will be populated with the current value of this field from the register.
 * @param[out] nfvgathrh0 - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcsnr1_unpack(uint8_t *snrmeassel, uint8_t *nfvgathrh2, uint8_t *nfvgathrh1, uint8_t *nfvgathrh0)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR1_ADDR);

    *snrmeassel = (localVal & ((uint32_t)0x01000000)) >> 24;
    *nfvgathrh2 = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *nfvgathrh1 = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *nfvgathrh0 = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the SNRMEASSEL field in the RWNXAGCSNR1 register.
 *
 * The RWNXAGCSNR1 register will be read and the SNRMEASSEL field's value will be returned.
 *
 * @return The current value of the SNRMEASSEL field in the RWNXAGCSNR1 register.
 */
__INLINE uint8_t agc_snrmeassel_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR1_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the SNRMEASSEL field of the RWNXAGCSNR1 register.
 *
 * The RWNXAGCSNR1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] snrmeassel - The value to set the field to.
 */
__INLINE void agc_snrmeassel_setf(uint8_t snrmeassel)
{
    ASSERT_ERR((((uint32_t)snrmeassel << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR1_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR1_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)snrmeassel << 24));
}

/**
 * @brief Returns the current value of the NFVGATHRH2 field in the RWNXAGCSNR1 register.
 *
 * The RWNXAGCSNR1 register will be read and the NFVGATHRH2 field's value will be returned.
 *
 * @return The current value of the NFVGATHRH2 field in the RWNXAGCSNR1 register.
 */
__INLINE uint8_t agc_nfvgathrh2_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR1_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the NFVGATHRH2 field of the RWNXAGCSNR1 register.
 *
 * The RWNXAGCSNR1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvgathrh2 - The value to set the field to.
 */
__INLINE void agc_nfvgathrh2_setf(uint8_t nfvgathrh2)
{
    ASSERT_ERR((((uint32_t)nfvgathrh2 << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR1_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR1_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)nfvgathrh2 << 8));
}

/**
 * @brief Returns the current value of the NFVGATHRH1 field in the RWNXAGCSNR1 register.
 *
 * The RWNXAGCSNR1 register will be read and the NFVGATHRH1 field's value will be returned.
 *
 * @return The current value of the NFVGATHRH1 field in the RWNXAGCSNR1 register.
 */
__INLINE uint8_t agc_nfvgathrh1_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR1_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the NFVGATHRH1 field of the RWNXAGCSNR1 register.
 *
 * The RWNXAGCSNR1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvgathrh1 - The value to set the field to.
 */
__INLINE void agc_nfvgathrh1_setf(uint8_t nfvgathrh1)
{
    ASSERT_ERR((((uint32_t)nfvgathrh1 << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR1_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR1_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)nfvgathrh1 << 4));
}

/**
 * @brief Returns the current value of the NFVGATHRH0 field in the RWNXAGCSNR1 register.
 *
 * The RWNXAGCSNR1 register will be read and the NFVGATHRH0 field's value will be returned.
 *
 * @return The current value of the NFVGATHRH0 field in the RWNXAGCSNR1 register.
 */
__INLINE uint8_t agc_nfvgathrh0_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR1_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the NFVGATHRH0 field of the RWNXAGCSNR1 register.
 *
 * The RWNXAGCSNR1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvgathrh0 - The value to set the field to.
 */
__INLINE void agc_nfvgathrh0_setf(uint8_t nfvgathrh0)
{
    ASSERT_ERR((((uint32_t)nfvgathrh0 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR1_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR1_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)nfvgathrh0 << 0));
}

/// @}

/**
 * @name RWNXAGCSNR2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:24            NFVALL3DB   0x20
 *  21:16            NFVALL2DB   0x21
 *  13:08            NFVALL1DB   0x28
 *  05:00            NFVALL0DB   0x28
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCSNR2 register
#define AGC_RWNXAGCSNR2_ADDR   0x01002090
/// Offset of the RWNXAGCSNR2 register from the base address
#define AGC_RWNXAGCSNR2_OFFSET 0x00002090
/// Index of the RWNXAGCSNR2 register
#define AGC_RWNXAGCSNR2_INDEX  0x00000824
/// Reset value of the RWNXAGCSNR2 register
#define AGC_RWNXAGCSNR2_RESET  0x20212828

/**
 * @brief Returns the current value of the RWNXAGCSNR2 register.
 * The RWNXAGCSNR2 register will be read and its value returned.
 * @return The current value of the RWNXAGCSNR2 register.
 */
__INLINE uint32_t agc_rwnxagcsnr2_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCSNR2_ADDR);
}

/**
 * @brief Sets the RWNXAGCSNR2 register to a value.
 * The RWNXAGCSNR2 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcsnr2_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCSNR2_ADDR, value);
}

// field definitions
/// NFVALL3DB field mask
#define AGC_NFVALL3DB_MASK   ((uint32_t)0x3F000000)
/// NFVALL3DB field LSB position
#define AGC_NFVALL3DB_LSB    24
/// NFVALL3DB field width
#define AGC_NFVALL3DB_WIDTH  ((uint32_t)0x00000006)
/// NFVALL2DB field mask
#define AGC_NFVALL2DB_MASK   ((uint32_t)0x003F0000)
/// NFVALL2DB field LSB position
#define AGC_NFVALL2DB_LSB    16
/// NFVALL2DB field width
#define AGC_NFVALL2DB_WIDTH  ((uint32_t)0x00000006)
/// NFVALL1DB field mask
#define AGC_NFVALL1DB_MASK   ((uint32_t)0x00003F00)
/// NFVALL1DB field LSB position
#define AGC_NFVALL1DB_LSB    8
/// NFVALL1DB field width
#define AGC_NFVALL1DB_WIDTH  ((uint32_t)0x00000006)
/// NFVALL0DB field mask
#define AGC_NFVALL0DB_MASK   ((uint32_t)0x0000003F)
/// NFVALL0DB field LSB position
#define AGC_NFVALL0DB_LSB    0
/// NFVALL0DB field width
#define AGC_NFVALL0DB_WIDTH  ((uint32_t)0x00000006)

/// NFVALL3DB field reset value
#define AGC_NFVALL3DB_RST    0x20
/// NFVALL2DB field reset value
#define AGC_NFVALL2DB_RST    0x21
/// NFVALL1DB field reset value
#define AGC_NFVALL1DB_RST    0x28
/// NFVALL0DB field reset value
#define AGC_NFVALL0DB_RST    0x28

/**
 * @brief Constructs a value for the RWNXAGCSNR2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] nfvall3db - The value to use for the NFVALL3DB field.
 * @param[in] nfvall2db - The value to use for the NFVALL2DB field.
 * @param[in] nfvall1db - The value to use for the NFVALL1DB field.
 * @param[in] nfvall0db - The value to use for the NFVALL0DB field.
 */
__INLINE void agc_rwnxagcsnr2_pack(uint8_t nfvall3db, uint8_t nfvall2db, uint8_t nfvall1db, uint8_t nfvall0db)
{
    ASSERT_ERR((((uint32_t)nfvall3db << 24) & ~((uint32_t)0x3F000000)) == 0);
    ASSERT_ERR((((uint32_t)nfvall2db << 16) & ~((uint32_t)0x003F0000)) == 0);
    ASSERT_ERR((((uint32_t)nfvall1db << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)nfvall0db << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR2_ADDR,  ((uint32_t)nfvall3db << 24) | ((uint32_t)nfvall2db << 16) | ((uint32_t)nfvall1db << 8) | ((uint32_t)nfvall0db << 0));
}

/**
 * @brief Unpacks RWNXAGCSNR2's fields from current value of the RWNXAGCSNR2 register.
 *
 * Reads the RWNXAGCSNR2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] nfvall3db - Will be populated with the current value of this field from the register.
 * @param[out] nfvall2db - Will be populated with the current value of this field from the register.
 * @param[out] nfvall1db - Will be populated with the current value of this field from the register.
 * @param[out] nfvall0db - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcsnr2_unpack(uint8_t *nfvall3db, uint8_t *nfvall2db, uint8_t *nfvall1db, uint8_t *nfvall0db)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR2_ADDR);

    *nfvall3db = (localVal & ((uint32_t)0x3F000000)) >> 24;
    *nfvall2db = (localVal & ((uint32_t)0x003F0000)) >> 16;
    *nfvall1db = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *nfvall0db = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the NFVALL3DB field in the RWNXAGCSNR2 register.
 *
 * The RWNXAGCSNR2 register will be read and the NFVALL3DB field's value will be returned.
 *
 * @return The current value of the NFVALL3DB field in the RWNXAGCSNR2 register.
 */
__INLINE uint8_t agc_nfvall3db_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR2_ADDR);
    return ((localVal & ((uint32_t)0x3F000000)) >> 24);
}

/**
 * @brief Sets the NFVALL3DB field of the RWNXAGCSNR2 register.
 *
 * The RWNXAGCSNR2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvall3db - The value to set the field to.
 */
__INLINE void agc_nfvall3db_setf(uint8_t nfvall3db)
{
    ASSERT_ERR((((uint32_t)nfvall3db << 24) & ~((uint32_t)0x3F000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR2_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR2_ADDR) & ~((uint32_t)0x3F000000)) | ((uint32_t)nfvall3db << 24));
}

/**
 * @brief Returns the current value of the NFVALL2DB field in the RWNXAGCSNR2 register.
 *
 * The RWNXAGCSNR2 register will be read and the NFVALL2DB field's value will be returned.
 *
 * @return The current value of the NFVALL2DB field in the RWNXAGCSNR2 register.
 */
__INLINE uint8_t agc_nfvall2db_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR2_ADDR);
    return ((localVal & ((uint32_t)0x003F0000)) >> 16);
}

/**
 * @brief Sets the NFVALL2DB field of the RWNXAGCSNR2 register.
 *
 * The RWNXAGCSNR2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvall2db - The value to set the field to.
 */
__INLINE void agc_nfvall2db_setf(uint8_t nfvall2db)
{
    ASSERT_ERR((((uint32_t)nfvall2db << 16) & ~((uint32_t)0x003F0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR2_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR2_ADDR) & ~((uint32_t)0x003F0000)) | ((uint32_t)nfvall2db << 16));
}

/**
 * @brief Returns the current value of the NFVALL1DB field in the RWNXAGCSNR2 register.
 *
 * The RWNXAGCSNR2 register will be read and the NFVALL1DB field's value will be returned.
 *
 * @return The current value of the NFVALL1DB field in the RWNXAGCSNR2 register.
 */
__INLINE uint8_t agc_nfvall1db_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR2_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the NFVALL1DB field of the RWNXAGCSNR2 register.
 *
 * The RWNXAGCSNR2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvall1db - The value to set the field to.
 */
__INLINE void agc_nfvall1db_setf(uint8_t nfvall1db)
{
    ASSERT_ERR((((uint32_t)nfvall1db << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR2_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR2_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)nfvall1db << 8));
}

/**
 * @brief Returns the current value of the NFVALL0DB field in the RWNXAGCSNR2 register.
 *
 * The RWNXAGCSNR2 register will be read and the NFVALL0DB field's value will be returned.
 *
 * @return The current value of the NFVALL0DB field in the RWNXAGCSNR2 register.
 */
__INLINE uint8_t agc_nfvall0db_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR2_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the NFVALL0DB field of the RWNXAGCSNR2 register.
 *
 * The RWNXAGCSNR2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvall0db - The value to set the field to.
 */
__INLINE void agc_nfvall0db_setf(uint8_t nfvall0db)
{
    ASSERT_ERR((((uint32_t)nfvall0db << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR2_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR2_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)nfvall0db << 0));
}

/// @}

/**
 * @name RWNXAGCSNR3 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:24            NFVALM3DB   0x11
 *  21:16            NFVALM2DB   0x12
 *  13:08            NFVALM1DB   0x16
 *  05:00            NFVALM0DB   0x17
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCSNR3 register
#define AGC_RWNXAGCSNR3_ADDR   0x01002094
/// Offset of the RWNXAGCSNR3 register from the base address
#define AGC_RWNXAGCSNR3_OFFSET 0x00002094
/// Index of the RWNXAGCSNR3 register
#define AGC_RWNXAGCSNR3_INDEX  0x00000825
/// Reset value of the RWNXAGCSNR3 register
#define AGC_RWNXAGCSNR3_RESET  0x11121617

/**
 * @brief Returns the current value of the RWNXAGCSNR3 register.
 * The RWNXAGCSNR3 register will be read and its value returned.
 * @return The current value of the RWNXAGCSNR3 register.
 */
__INLINE uint32_t agc_rwnxagcsnr3_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCSNR3_ADDR);
}

/**
 * @brief Sets the RWNXAGCSNR3 register to a value.
 * The RWNXAGCSNR3 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcsnr3_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCSNR3_ADDR, value);
}

// field definitions
/// NFVALM3DB field mask
#define AGC_NFVALM3DB_MASK   ((uint32_t)0x3F000000)
/// NFVALM3DB field LSB position
#define AGC_NFVALM3DB_LSB    24
/// NFVALM3DB field width
#define AGC_NFVALM3DB_WIDTH  ((uint32_t)0x00000006)
/// NFVALM2DB field mask
#define AGC_NFVALM2DB_MASK   ((uint32_t)0x003F0000)
/// NFVALM2DB field LSB position
#define AGC_NFVALM2DB_LSB    16
/// NFVALM2DB field width
#define AGC_NFVALM2DB_WIDTH  ((uint32_t)0x00000006)
/// NFVALM1DB field mask
#define AGC_NFVALM1DB_MASK   ((uint32_t)0x00003F00)
/// NFVALM1DB field LSB position
#define AGC_NFVALM1DB_LSB    8
/// NFVALM1DB field width
#define AGC_NFVALM1DB_WIDTH  ((uint32_t)0x00000006)
/// NFVALM0DB field mask
#define AGC_NFVALM0DB_MASK   ((uint32_t)0x0000003F)
/// NFVALM0DB field LSB position
#define AGC_NFVALM0DB_LSB    0
/// NFVALM0DB field width
#define AGC_NFVALM0DB_WIDTH  ((uint32_t)0x00000006)

/// NFVALM3DB field reset value
#define AGC_NFVALM3DB_RST    0x11
/// NFVALM2DB field reset value
#define AGC_NFVALM2DB_RST    0x12
/// NFVALM1DB field reset value
#define AGC_NFVALM1DB_RST    0x16
/// NFVALM0DB field reset value
#define AGC_NFVALM0DB_RST    0x17

/**
 * @brief Constructs a value for the RWNXAGCSNR3 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] nfvalm3db - The value to use for the NFVALM3DB field.
 * @param[in] nfvalm2db - The value to use for the NFVALM2DB field.
 * @param[in] nfvalm1db - The value to use for the NFVALM1DB field.
 * @param[in] nfvalm0db - The value to use for the NFVALM0DB field.
 */
__INLINE void agc_rwnxagcsnr3_pack(uint8_t nfvalm3db, uint8_t nfvalm2db, uint8_t nfvalm1db, uint8_t nfvalm0db)
{
    ASSERT_ERR((((uint32_t)nfvalm3db << 24) & ~((uint32_t)0x3F000000)) == 0);
    ASSERT_ERR((((uint32_t)nfvalm2db << 16) & ~((uint32_t)0x003F0000)) == 0);
    ASSERT_ERR((((uint32_t)nfvalm1db << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)nfvalm0db << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR3_ADDR,  ((uint32_t)nfvalm3db << 24) | ((uint32_t)nfvalm2db << 16) | ((uint32_t)nfvalm1db << 8) | ((uint32_t)nfvalm0db << 0));
}

/**
 * @brief Unpacks RWNXAGCSNR3's fields from current value of the RWNXAGCSNR3 register.
 *
 * Reads the RWNXAGCSNR3 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] nfvalm3db - Will be populated with the current value of this field from the register.
 * @param[out] nfvalm2db - Will be populated with the current value of this field from the register.
 * @param[out] nfvalm1db - Will be populated with the current value of this field from the register.
 * @param[out] nfvalm0db - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcsnr3_unpack(uint8_t *nfvalm3db, uint8_t *nfvalm2db, uint8_t *nfvalm1db, uint8_t *nfvalm0db)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR3_ADDR);

    *nfvalm3db = (localVal & ((uint32_t)0x3F000000)) >> 24;
    *nfvalm2db = (localVal & ((uint32_t)0x003F0000)) >> 16;
    *nfvalm1db = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *nfvalm0db = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the NFVALM3DB field in the RWNXAGCSNR3 register.
 *
 * The RWNXAGCSNR3 register will be read and the NFVALM3DB field's value will be returned.
 *
 * @return The current value of the NFVALM3DB field in the RWNXAGCSNR3 register.
 */
__INLINE uint8_t agc_nfvalm3db_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR3_ADDR);
    return ((localVal & ((uint32_t)0x3F000000)) >> 24);
}

/**
 * @brief Sets the NFVALM3DB field of the RWNXAGCSNR3 register.
 *
 * The RWNXAGCSNR3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvalm3db - The value to set the field to.
 */
__INLINE void agc_nfvalm3db_setf(uint8_t nfvalm3db)
{
    ASSERT_ERR((((uint32_t)nfvalm3db << 24) & ~((uint32_t)0x3F000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR3_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR3_ADDR) & ~((uint32_t)0x3F000000)) | ((uint32_t)nfvalm3db << 24));
}

/**
 * @brief Returns the current value of the NFVALM2DB field in the RWNXAGCSNR3 register.
 *
 * The RWNXAGCSNR3 register will be read and the NFVALM2DB field's value will be returned.
 *
 * @return The current value of the NFVALM2DB field in the RWNXAGCSNR3 register.
 */
__INLINE uint8_t agc_nfvalm2db_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR3_ADDR);
    return ((localVal & ((uint32_t)0x003F0000)) >> 16);
}

/**
 * @brief Sets the NFVALM2DB field of the RWNXAGCSNR3 register.
 *
 * The RWNXAGCSNR3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvalm2db - The value to set the field to.
 */
__INLINE void agc_nfvalm2db_setf(uint8_t nfvalm2db)
{
    ASSERT_ERR((((uint32_t)nfvalm2db << 16) & ~((uint32_t)0x003F0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR3_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR3_ADDR) & ~((uint32_t)0x003F0000)) | ((uint32_t)nfvalm2db << 16));
}

/**
 * @brief Returns the current value of the NFVALM1DB field in the RWNXAGCSNR3 register.
 *
 * The RWNXAGCSNR3 register will be read and the NFVALM1DB field's value will be returned.
 *
 * @return The current value of the NFVALM1DB field in the RWNXAGCSNR3 register.
 */
__INLINE uint8_t agc_nfvalm1db_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR3_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the NFVALM1DB field of the RWNXAGCSNR3 register.
 *
 * The RWNXAGCSNR3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvalm1db - The value to set the field to.
 */
__INLINE void agc_nfvalm1db_setf(uint8_t nfvalm1db)
{
    ASSERT_ERR((((uint32_t)nfvalm1db << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR3_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR3_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)nfvalm1db << 8));
}

/**
 * @brief Returns the current value of the NFVALM0DB field in the RWNXAGCSNR3 register.
 *
 * The RWNXAGCSNR3 register will be read and the NFVALM0DB field's value will be returned.
 *
 * @return The current value of the NFVALM0DB field in the RWNXAGCSNR3 register.
 */
__INLINE uint8_t agc_nfvalm0db_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR3_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the NFVALM0DB field of the RWNXAGCSNR3 register.
 *
 * The RWNXAGCSNR3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvalm0db - The value to set the field to.
 */
__INLINE void agc_nfvalm0db_setf(uint8_t nfvalm0db)
{
    ASSERT_ERR((((uint32_t)nfvalm0db << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR3_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR3_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)nfvalm0db << 0));
}

/// @}

/**
 * @name RWNXAGCSNR4 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:24            NFVALH3DB   0x4
 *  21:16            NFVALH2DB   0x6
 *  13:08            NFVALH1DB   0x7
 *  05:00            NFVALH0DB   0x8
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCSNR4 register
#define AGC_RWNXAGCSNR4_ADDR   0x01002098
/// Offset of the RWNXAGCSNR4 register from the base address
#define AGC_RWNXAGCSNR4_OFFSET 0x00002098
/// Index of the RWNXAGCSNR4 register
#define AGC_RWNXAGCSNR4_INDEX  0x00000826
/// Reset value of the RWNXAGCSNR4 register
#define AGC_RWNXAGCSNR4_RESET  0x04060708

/**
 * @brief Returns the current value of the RWNXAGCSNR4 register.
 * The RWNXAGCSNR4 register will be read and its value returned.
 * @return The current value of the RWNXAGCSNR4 register.
 */
__INLINE uint32_t agc_rwnxagcsnr4_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCSNR4_ADDR);
}

/**
 * @brief Sets the RWNXAGCSNR4 register to a value.
 * The RWNXAGCSNR4 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcsnr4_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCSNR4_ADDR, value);
}

// field definitions
/// NFVALH3DB field mask
#define AGC_NFVALH3DB_MASK   ((uint32_t)0x3F000000)
/// NFVALH3DB field LSB position
#define AGC_NFVALH3DB_LSB    24
/// NFVALH3DB field width
#define AGC_NFVALH3DB_WIDTH  ((uint32_t)0x00000006)
/// NFVALH2DB field mask
#define AGC_NFVALH2DB_MASK   ((uint32_t)0x003F0000)
/// NFVALH2DB field LSB position
#define AGC_NFVALH2DB_LSB    16
/// NFVALH2DB field width
#define AGC_NFVALH2DB_WIDTH  ((uint32_t)0x00000006)
/// NFVALH1DB field mask
#define AGC_NFVALH1DB_MASK   ((uint32_t)0x00003F00)
/// NFVALH1DB field LSB position
#define AGC_NFVALH1DB_LSB    8
/// NFVALH1DB field width
#define AGC_NFVALH1DB_WIDTH  ((uint32_t)0x00000006)
/// NFVALH0DB field mask
#define AGC_NFVALH0DB_MASK   ((uint32_t)0x0000003F)
/// NFVALH0DB field LSB position
#define AGC_NFVALH0DB_LSB    0
/// NFVALH0DB field width
#define AGC_NFVALH0DB_WIDTH  ((uint32_t)0x00000006)

/// NFVALH3DB field reset value
#define AGC_NFVALH3DB_RST    0x4
/// NFVALH2DB field reset value
#define AGC_NFVALH2DB_RST    0x6
/// NFVALH1DB field reset value
#define AGC_NFVALH1DB_RST    0x7
/// NFVALH0DB field reset value
#define AGC_NFVALH0DB_RST    0x8

/**
 * @brief Constructs a value for the RWNXAGCSNR4 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] nfvalh3db - The value to use for the NFVALH3DB field.
 * @param[in] nfvalh2db - The value to use for the NFVALH2DB field.
 * @param[in] nfvalh1db - The value to use for the NFVALH1DB field.
 * @param[in] nfvalh0db - The value to use for the NFVALH0DB field.
 */
__INLINE void agc_rwnxagcsnr4_pack(uint8_t nfvalh3db, uint8_t nfvalh2db, uint8_t nfvalh1db, uint8_t nfvalh0db)
{
    ASSERT_ERR((((uint32_t)nfvalh3db << 24) & ~((uint32_t)0x3F000000)) == 0);
    ASSERT_ERR((((uint32_t)nfvalh2db << 16) & ~((uint32_t)0x003F0000)) == 0);
    ASSERT_ERR((((uint32_t)nfvalh1db << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)nfvalh0db << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR4_ADDR,  ((uint32_t)nfvalh3db << 24) | ((uint32_t)nfvalh2db << 16) | ((uint32_t)nfvalh1db << 8) | ((uint32_t)nfvalh0db << 0));
}

/**
 * @brief Unpacks RWNXAGCSNR4's fields from current value of the RWNXAGCSNR4 register.
 *
 * Reads the RWNXAGCSNR4 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] nfvalh3db - Will be populated with the current value of this field from the register.
 * @param[out] nfvalh2db - Will be populated with the current value of this field from the register.
 * @param[out] nfvalh1db - Will be populated with the current value of this field from the register.
 * @param[out] nfvalh0db - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcsnr4_unpack(uint8_t *nfvalh3db, uint8_t *nfvalh2db, uint8_t *nfvalh1db, uint8_t *nfvalh0db)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR4_ADDR);

    *nfvalh3db = (localVal & ((uint32_t)0x3F000000)) >> 24;
    *nfvalh2db = (localVal & ((uint32_t)0x003F0000)) >> 16;
    *nfvalh1db = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *nfvalh0db = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the NFVALH3DB field in the RWNXAGCSNR4 register.
 *
 * The RWNXAGCSNR4 register will be read and the NFVALH3DB field's value will be returned.
 *
 * @return The current value of the NFVALH3DB field in the RWNXAGCSNR4 register.
 */
__INLINE uint8_t agc_nfvalh3db_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR4_ADDR);
    return ((localVal & ((uint32_t)0x3F000000)) >> 24);
}

/**
 * @brief Sets the NFVALH3DB field of the RWNXAGCSNR4 register.
 *
 * The RWNXAGCSNR4 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvalh3db - The value to set the field to.
 */
__INLINE void agc_nfvalh3db_setf(uint8_t nfvalh3db)
{
    ASSERT_ERR((((uint32_t)nfvalh3db << 24) & ~((uint32_t)0x3F000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR4_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR4_ADDR) & ~((uint32_t)0x3F000000)) | ((uint32_t)nfvalh3db << 24));
}

/**
 * @brief Returns the current value of the NFVALH2DB field in the RWNXAGCSNR4 register.
 *
 * The RWNXAGCSNR4 register will be read and the NFVALH2DB field's value will be returned.
 *
 * @return The current value of the NFVALH2DB field in the RWNXAGCSNR4 register.
 */
__INLINE uint8_t agc_nfvalh2db_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR4_ADDR);
    return ((localVal & ((uint32_t)0x003F0000)) >> 16);
}

/**
 * @brief Sets the NFVALH2DB field of the RWNXAGCSNR4 register.
 *
 * The RWNXAGCSNR4 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvalh2db - The value to set the field to.
 */
__INLINE void agc_nfvalh2db_setf(uint8_t nfvalh2db)
{
    ASSERT_ERR((((uint32_t)nfvalh2db << 16) & ~((uint32_t)0x003F0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR4_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR4_ADDR) & ~((uint32_t)0x003F0000)) | ((uint32_t)nfvalh2db << 16));
}

/**
 * @brief Returns the current value of the NFVALH1DB field in the RWNXAGCSNR4 register.
 *
 * The RWNXAGCSNR4 register will be read and the NFVALH1DB field's value will be returned.
 *
 * @return The current value of the NFVALH1DB field in the RWNXAGCSNR4 register.
 */
__INLINE uint8_t agc_nfvalh1db_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR4_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the NFVALH1DB field of the RWNXAGCSNR4 register.
 *
 * The RWNXAGCSNR4 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvalh1db - The value to set the field to.
 */
__INLINE void agc_nfvalh1db_setf(uint8_t nfvalh1db)
{
    ASSERT_ERR((((uint32_t)nfvalh1db << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR4_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR4_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)nfvalh1db << 8));
}

/**
 * @brief Returns the current value of the NFVALH0DB field in the RWNXAGCSNR4 register.
 *
 * The RWNXAGCSNR4 register will be read and the NFVALH0DB field's value will be returned.
 *
 * @return The current value of the NFVALH0DB field in the RWNXAGCSNR4 register.
 */
__INLINE uint8_t agc_nfvalh0db_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCSNR4_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the NFVALH0DB field of the RWNXAGCSNR4 register.
 *
 * The RWNXAGCSNR4 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nfvalh0db - The value to set the field to.
 */
__INLINE void agc_nfvalh0db_setf(uint8_t nfvalh0db)
{
    ASSERT_ERR((((uint32_t)nfvalh0db << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(AGC_RWNXAGCSNR4_ADDR, (REG_PL_RD(AGC_RWNXAGCSNR4_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)nfvalh0db << 0));
}

/// @}

/**
 * @name RWNXAGCCOMP register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:24       IDPOWGAPDNQDBM   0x28
 *  23:16       IDPOWGAPUPQDBM   0x28
 *  15:08         POWINFTHRDBM   0xBA
 *  07:00         POWSUPTHRDBM   0xBA
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCCOMP register
#define AGC_RWNXAGCCOMP_ADDR   0x0100209C
/// Offset of the RWNXAGCCOMP register from the base address
#define AGC_RWNXAGCCOMP_OFFSET 0x0000209C
/// Index of the RWNXAGCCOMP register
#define AGC_RWNXAGCCOMP_INDEX  0x00000827
/// Reset value of the RWNXAGCCOMP register
#define AGC_RWNXAGCCOMP_RESET  0x2828BABA

/**
 * @brief Returns the current value of the RWNXAGCCOMP register.
 * The RWNXAGCCOMP register will be read and its value returned.
 * @return The current value of the RWNXAGCCOMP register.
 */
__INLINE uint32_t agc_rwnxagccomp_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCCOMP_ADDR);
}

/**
 * @brief Sets the RWNXAGCCOMP register to a value.
 * The RWNXAGCCOMP register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagccomp_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCCOMP_ADDR, value);
}

// field definitions
/// IDPOWGAPDNQDBM field mask
#define AGC_IDPOWGAPDNQDBM_MASK   ((uint32_t)0xFF000000)
/// IDPOWGAPDNQDBM field LSB position
#define AGC_IDPOWGAPDNQDBM_LSB    24
/// IDPOWGAPDNQDBM field width
#define AGC_IDPOWGAPDNQDBM_WIDTH  ((uint32_t)0x00000008)
/// IDPOWGAPUPQDBM field mask
#define AGC_IDPOWGAPUPQDBM_MASK   ((uint32_t)0x00FF0000)
/// IDPOWGAPUPQDBM field LSB position
#define AGC_IDPOWGAPUPQDBM_LSB    16
/// IDPOWGAPUPQDBM field width
#define AGC_IDPOWGAPUPQDBM_WIDTH  ((uint32_t)0x00000008)
/// POWINFTHRDBM field mask
#define AGC_POWINFTHRDBM_MASK     ((uint32_t)0x0000FF00)
/// POWINFTHRDBM field LSB position
#define AGC_POWINFTHRDBM_LSB      8
/// POWINFTHRDBM field width
#define AGC_POWINFTHRDBM_WIDTH    ((uint32_t)0x00000008)
/// POWSUPTHRDBM field mask
#define AGC_POWSUPTHRDBM_MASK     ((uint32_t)0x000000FF)
/// POWSUPTHRDBM field LSB position
#define AGC_POWSUPTHRDBM_LSB      0
/// POWSUPTHRDBM field width
#define AGC_POWSUPTHRDBM_WIDTH    ((uint32_t)0x00000008)

/// IDPOWGAPDNQDBM field reset value
#define AGC_IDPOWGAPDNQDBM_RST    0x28
/// IDPOWGAPUPQDBM field reset value
#define AGC_IDPOWGAPUPQDBM_RST    0x28
/// POWINFTHRDBM field reset value
#define AGC_POWINFTHRDBM_RST      0xBA
/// POWSUPTHRDBM field reset value
#define AGC_POWSUPTHRDBM_RST      0xBA

/**
 * @brief Constructs a value for the RWNXAGCCOMP register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] idpowgapdnqdbm - The value to use for the IDPOWGAPDNQDBM field.
 * @param[in] idpowgapupqdbm - The value to use for the IDPOWGAPUPQDBM field.
 * @param[in] powinfthrdbm - The value to use for the POWINFTHRDBM field.
 * @param[in] powsupthrdbm - The value to use for the POWSUPTHRDBM field.
 */
__INLINE void agc_rwnxagccomp_pack(uint8_t idpowgapdnqdbm, uint8_t idpowgapupqdbm, uint8_t powinfthrdbm, uint8_t powsupthrdbm)
{
    ASSERT_ERR((((uint32_t)idpowgapdnqdbm << 24) & ~((uint32_t)0xFF000000)) == 0);
    ASSERT_ERR((((uint32_t)idpowgapupqdbm << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)powinfthrdbm << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)powsupthrdbm << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCCOMP_ADDR,  ((uint32_t)idpowgapdnqdbm << 24) | ((uint32_t)idpowgapupqdbm << 16) | ((uint32_t)powinfthrdbm << 8) | ((uint32_t)powsupthrdbm << 0));
}

/**
 * @brief Unpacks RWNXAGCCOMP's fields from current value of the RWNXAGCCOMP register.
 *
 * Reads the RWNXAGCCOMP register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] idpowgapdnqdbm - Will be populated with the current value of this field from the register.
 * @param[out] idpowgapupqdbm - Will be populated with the current value of this field from the register.
 * @param[out] powinfthrdbm - Will be populated with the current value of this field from the register.
 * @param[out] powsupthrdbm - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagccomp_unpack(uint8_t *idpowgapdnqdbm, uint8_t *idpowgapupqdbm, uint8_t *powinfthrdbm, uint8_t *powsupthrdbm)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCOMP_ADDR);

    *idpowgapdnqdbm = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *idpowgapupqdbm = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *powinfthrdbm = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *powsupthrdbm = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the IDPOWGAPDNQDBM field in the RWNXAGCCOMP register.
 *
 * The RWNXAGCCOMP register will be read and the IDPOWGAPDNQDBM field's value will be returned.
 *
 * @return The current value of the IDPOWGAPDNQDBM field in the RWNXAGCCOMP register.
 */
__INLINE uint8_t agc_idpowgapdnqdbm_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCOMP_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Sets the IDPOWGAPDNQDBM field of the RWNXAGCCOMP register.
 *
 * The RWNXAGCCOMP register will be read, modified to contain the new field value, and written.
 *
 * @param[in] idpowgapdnqdbm - The value to set the field to.
 */
__INLINE void agc_idpowgapdnqdbm_setf(uint8_t idpowgapdnqdbm)
{
    ASSERT_ERR((((uint32_t)idpowgapdnqdbm << 24) & ~((uint32_t)0xFF000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCCOMP_ADDR, (REG_PL_RD(AGC_RWNXAGCCOMP_ADDR) & ~((uint32_t)0xFF000000)) | ((uint32_t)idpowgapdnqdbm << 24));
}

/**
 * @brief Returns the current value of the IDPOWGAPUPQDBM field in the RWNXAGCCOMP register.
 *
 * The RWNXAGCCOMP register will be read and the IDPOWGAPUPQDBM field's value will be returned.
 *
 * @return The current value of the IDPOWGAPUPQDBM field in the RWNXAGCCOMP register.
 */
__INLINE uint8_t agc_idpowgapupqdbm_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCOMP_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the IDPOWGAPUPQDBM field of the RWNXAGCCOMP register.
 *
 * The RWNXAGCCOMP register will be read, modified to contain the new field value, and written.
 *
 * @param[in] idpowgapupqdbm - The value to set the field to.
 */
__INLINE void agc_idpowgapupqdbm_setf(uint8_t idpowgapupqdbm)
{
    ASSERT_ERR((((uint32_t)idpowgapupqdbm << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCCOMP_ADDR, (REG_PL_RD(AGC_RWNXAGCCOMP_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)idpowgapupqdbm << 16));
}

/**
 * @brief Returns the current value of the POWINFTHRDBM field in the RWNXAGCCOMP register.
 *
 * The RWNXAGCCOMP register will be read and the POWINFTHRDBM field's value will be returned.
 *
 * @return The current value of the POWINFTHRDBM field in the RWNXAGCCOMP register.
 */
__INLINE uint8_t agc_powinfthrdbm_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCOMP_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the POWINFTHRDBM field of the RWNXAGCCOMP register.
 *
 * The RWNXAGCCOMP register will be read, modified to contain the new field value, and written.
 *
 * @param[in] powinfthrdbm - The value to set the field to.
 */
__INLINE void agc_powinfthrdbm_setf(uint8_t powinfthrdbm)
{
    ASSERT_ERR((((uint32_t)powinfthrdbm << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(AGC_RWNXAGCCOMP_ADDR, (REG_PL_RD(AGC_RWNXAGCCOMP_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)powinfthrdbm << 8));
}

/**
 * @brief Returns the current value of the POWSUPTHRDBM field in the RWNXAGCCOMP register.
 *
 * The RWNXAGCCOMP register will be read and the POWSUPTHRDBM field's value will be returned.
 *
 * @return The current value of the POWSUPTHRDBM field in the RWNXAGCCOMP register.
 */
__INLINE uint8_t agc_powsupthrdbm_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCCOMP_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the POWSUPTHRDBM field of the RWNXAGCCOMP register.
 *
 * The RWNXAGCCOMP register will be read, modified to contain the new field value, and written.
 *
 * @param[in] powsupthrdbm - The value to set the field to.
 */
__INLINE void agc_powsupthrdbm_setf(uint8_t powsupthrdbm)
{
    ASSERT_ERR((((uint32_t)powsupthrdbm << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCCOMP_ADDR, (REG_PL_RD(AGC_RWNXAGCCOMP_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)powsupthrdbm << 0));
}

/// @}

/**
 * @name RWNXAGCRADAR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  27:24          FREQSINGGAP   0x0
 *  23:16             FREQSING   0x0
 *  11:08           FOMSINGGAP   0x0
 *  07:00              FOMSING   0x0
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCRADAR register
#define AGC_RWNXAGCRADAR_ADDR   0x010020A0
/// Offset of the RWNXAGCRADAR register from the base address
#define AGC_RWNXAGCRADAR_OFFSET 0x000020A0
/// Index of the RWNXAGCRADAR register
#define AGC_RWNXAGCRADAR_INDEX  0x00000828
/// Reset value of the RWNXAGCRADAR register
#define AGC_RWNXAGCRADAR_RESET  0x00000000

/**
 * @brief Returns the current value of the RWNXAGCRADAR register.
 * The RWNXAGCRADAR register will be read and its value returned.
 * @return The current value of the RWNXAGCRADAR register.
 */
__INLINE uint32_t agc_rwnxagcradar_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCRADAR_ADDR);
}

/**
 * @brief Sets the RWNXAGCRADAR register to a value.
 * The RWNXAGCRADAR register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcradar_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCRADAR_ADDR, value);
}

// field definitions
/// FREQSINGGAP field mask
#define AGC_FREQSINGGAP_MASK   ((uint32_t)0x0F000000)
/// FREQSINGGAP field LSB position
#define AGC_FREQSINGGAP_LSB    24
/// FREQSINGGAP field width
#define AGC_FREQSINGGAP_WIDTH  ((uint32_t)0x00000004)
/// FREQSING field mask
#define AGC_FREQSING_MASK      ((uint32_t)0x00FF0000)
/// FREQSING field LSB position
#define AGC_FREQSING_LSB       16
/// FREQSING field width
#define AGC_FREQSING_WIDTH     ((uint32_t)0x00000008)
/// FOMSINGGAP field mask
#define AGC_FOMSINGGAP_MASK    ((uint32_t)0x00000F00)
/// FOMSINGGAP field LSB position
#define AGC_FOMSINGGAP_LSB     8
/// FOMSINGGAP field width
#define AGC_FOMSINGGAP_WIDTH   ((uint32_t)0x00000004)
/// FOMSING field mask
#define AGC_FOMSING_MASK       ((uint32_t)0x000000FF)
/// FOMSING field LSB position
#define AGC_FOMSING_LSB        0
/// FOMSING field width
#define AGC_FOMSING_WIDTH      ((uint32_t)0x00000008)

/// FREQSINGGAP field reset value
#define AGC_FREQSINGGAP_RST    0x0
/// FREQSING field reset value
#define AGC_FREQSING_RST       0x0
/// FOMSINGGAP field reset value
#define AGC_FOMSINGGAP_RST     0x0
/// FOMSING field reset value
#define AGC_FOMSING_RST        0x0

/**
 * @brief Constructs a value for the RWNXAGCRADAR register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] freqsinggap - The value to use for the FREQSINGGAP field.
 * @param[in] freqsing - The value to use for the FREQSING field.
 * @param[in] fomsinggap - The value to use for the FOMSINGGAP field.
 * @param[in] fomsing - The value to use for the FOMSING field.
 */
__INLINE void agc_rwnxagcradar_pack(uint8_t freqsinggap, uint8_t freqsing, uint8_t fomsinggap, uint8_t fomsing)
{
    ASSERT_ERR((((uint32_t)freqsinggap << 24) & ~((uint32_t)0x0F000000)) == 0);
    ASSERT_ERR((((uint32_t)freqsing << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)fomsinggap << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)fomsing << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCRADAR_ADDR,  ((uint32_t)freqsinggap << 24) | ((uint32_t)freqsing << 16) | ((uint32_t)fomsinggap << 8) | ((uint32_t)fomsing << 0));
}

/**
 * @brief Unpacks RWNXAGCRADAR's fields from current value of the RWNXAGCRADAR register.
 *
 * Reads the RWNXAGCRADAR register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] freqsinggap - Will be populated with the current value of this field from the register.
 * @param[out] freqsing - Will be populated with the current value of this field from the register.
 * @param[out] fomsinggap - Will be populated with the current value of this field from the register.
 * @param[out] fomsing - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcradar_unpack(uint8_t *freqsinggap, uint8_t *freqsing, uint8_t *fomsinggap, uint8_t *fomsing)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCRADAR_ADDR);

    *freqsinggap = (localVal & ((uint32_t)0x0F000000)) >> 24;
    *freqsing = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *fomsinggap = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *fomsing = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the FREQSINGGAP field in the RWNXAGCRADAR register.
 *
 * The RWNXAGCRADAR register will be read and the FREQSINGGAP field's value will be returned.
 *
 * @return The current value of the FREQSINGGAP field in the RWNXAGCRADAR register.
 */
__INLINE uint8_t agc_freqsinggap_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCRADAR_ADDR);
    return ((localVal & ((uint32_t)0x0F000000)) >> 24);
}

/**
 * @brief Sets the FREQSINGGAP field of the RWNXAGCRADAR register.
 *
 * The RWNXAGCRADAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] freqsinggap - The value to set the field to.
 */
__INLINE void agc_freqsinggap_setf(uint8_t freqsinggap)
{
    ASSERT_ERR((((uint32_t)freqsinggap << 24) & ~((uint32_t)0x0F000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCRADAR_ADDR, (REG_PL_RD(AGC_RWNXAGCRADAR_ADDR) & ~((uint32_t)0x0F000000)) | ((uint32_t)freqsinggap << 24));
}

/**
 * @brief Returns the current value of the FREQSING field in the RWNXAGCRADAR register.
 *
 * The RWNXAGCRADAR register will be read and the FREQSING field's value will be returned.
 *
 * @return The current value of the FREQSING field in the RWNXAGCRADAR register.
 */
__INLINE uint8_t agc_freqsing_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCRADAR_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the FREQSING field of the RWNXAGCRADAR register.
 *
 * The RWNXAGCRADAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] freqsing - The value to set the field to.
 */
__INLINE void agc_freqsing_setf(uint8_t freqsing)
{
    ASSERT_ERR((((uint32_t)freqsing << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(AGC_RWNXAGCRADAR_ADDR, (REG_PL_RD(AGC_RWNXAGCRADAR_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)freqsing << 16));
}

/**
 * @brief Returns the current value of the FOMSINGGAP field in the RWNXAGCRADAR register.
 *
 * The RWNXAGCRADAR register will be read and the FOMSINGGAP field's value will be returned.
 *
 * @return The current value of the FOMSINGGAP field in the RWNXAGCRADAR register.
 */
__INLINE uint8_t agc_fomsinggap_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCRADAR_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the FOMSINGGAP field of the RWNXAGCRADAR register.
 *
 * The RWNXAGCRADAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fomsinggap - The value to set the field to.
 */
__INLINE void agc_fomsinggap_setf(uint8_t fomsinggap)
{
    ASSERT_ERR((((uint32_t)fomsinggap << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(AGC_RWNXAGCRADAR_ADDR, (REG_PL_RD(AGC_RWNXAGCRADAR_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)fomsinggap << 8));
}

/**
 * @brief Returns the current value of the FOMSING field in the RWNXAGCRADAR register.
 *
 * The RWNXAGCRADAR register will be read and the FOMSING field's value will be returned.
 *
 * @return The current value of the FOMSING field in the RWNXAGCRADAR register.
 */
__INLINE uint8_t agc_fomsing_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCRADAR_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the FOMSING field of the RWNXAGCRADAR register.
 *
 * The RWNXAGCRADAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fomsing - The value to set the field to.
 */
__INLINE void agc_fomsing_setf(uint8_t fomsing)
{
    ASSERT_ERR((((uint32_t)fomsing << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCRADAR_ADDR, (REG_PL_RD(AGC_RWNXAGCRADAR_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)fomsing << 0));
}

/// @}

/**
 * @name RWNXAGCRADARTIM register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31      PULSETIMERCLEAR   0
 *  18:16      PULSEIRQFIFOTHR   0x3
 *  07:00      PULSEIRQTIMEOUT   0x8
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCRADARTIM register
#define AGC_RWNXAGCRADARTIM_ADDR   0x010020A4
/// Offset of the RWNXAGCRADARTIM register from the base address
#define AGC_RWNXAGCRADARTIM_OFFSET 0x000020A4
/// Index of the RWNXAGCRADARTIM register
#define AGC_RWNXAGCRADARTIM_INDEX  0x00000829
/// Reset value of the RWNXAGCRADARTIM register
#define AGC_RWNXAGCRADARTIM_RESET  0x00030008

/**
 * @brief Returns the current value of the RWNXAGCRADARTIM register.
 * The RWNXAGCRADARTIM register will be read and its value returned.
 * @return The current value of the RWNXAGCRADARTIM register.
 */
__INLINE uint32_t agc_rwnxagcradartim_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCRADARTIM_ADDR);
}

/**
 * @brief Sets the RWNXAGCRADARTIM register to a value.
 * The RWNXAGCRADARTIM register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcradartim_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCRADARTIM_ADDR, value);
}

// field definitions
/// PULSETIMERCLEAR field bit
#define AGC_PULSETIMERCLEAR_BIT    ((uint32_t)0x80000000)
/// PULSETIMERCLEAR field position
#define AGC_PULSETIMERCLEAR_POS    31
/// PULSEIRQFIFOTHR field mask
#define AGC_PULSEIRQFIFOTHR_MASK   ((uint32_t)0x00070000)
/// PULSEIRQFIFOTHR field LSB position
#define AGC_PULSEIRQFIFOTHR_LSB    16
/// PULSEIRQFIFOTHR field width
#define AGC_PULSEIRQFIFOTHR_WIDTH  ((uint32_t)0x00000003)
/// PULSEIRQTIMEOUT field mask
#define AGC_PULSEIRQTIMEOUT_MASK   ((uint32_t)0x000000FF)
/// PULSEIRQTIMEOUT field LSB position
#define AGC_PULSEIRQTIMEOUT_LSB    0
/// PULSEIRQTIMEOUT field width
#define AGC_PULSEIRQTIMEOUT_WIDTH  ((uint32_t)0x00000008)

/// PULSETIMERCLEAR field reset value
#define AGC_PULSETIMERCLEAR_RST    0x0
/// PULSEIRQFIFOTHR field reset value
#define AGC_PULSEIRQFIFOTHR_RST    0x3
/// PULSEIRQTIMEOUT field reset value
#define AGC_PULSEIRQTIMEOUT_RST    0x8

/**
 * @brief Constructs a value for the RWNXAGCRADARTIM register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] pulsetimerclear - The value to use for the PULSETIMERCLEAR field.
 * @param[in] pulseirqfifothr - The value to use for the PULSEIRQFIFOTHR field.
 * @param[in] pulseirqtimeout - The value to use for the PULSEIRQTIMEOUT field.
 */
__INLINE void agc_rwnxagcradartim_pack(uint8_t pulsetimerclear, uint8_t pulseirqfifothr, uint8_t pulseirqtimeout)
{
    ASSERT_ERR((((uint32_t)pulsetimerclear << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)pulseirqfifothr << 16) & ~((uint32_t)0x00070000)) == 0);
    ASSERT_ERR((((uint32_t)pulseirqtimeout << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCRADARTIM_ADDR,  ((uint32_t)pulsetimerclear << 31) | ((uint32_t)pulseirqfifothr << 16) | ((uint32_t)pulseirqtimeout << 0));
}

/**
 * @brief Unpacks RWNXAGCRADARTIM's fields from current value of the RWNXAGCRADARTIM register.
 *
 * Reads the RWNXAGCRADARTIM register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] pulsetimerclear - Will be populated with the current value of this field from the register.
 * @param[out] pulseirqfifothr - Will be populated with the current value of this field from the register.
 * @param[out] pulseirqtimeout - Will be populated with the current value of this field from the register.
 */
__INLINE void agc_rwnxagcradartim_unpack(uint8_t *pulsetimerclear, uint8_t *pulseirqfifothr, uint8_t *pulseirqtimeout)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCRADARTIM_ADDR);

    *pulsetimerclear = (localVal & ((uint32_t)0x80000000)) >> 31;
    *pulseirqfifothr = (localVal & ((uint32_t)0x00070000)) >> 16;
    *pulseirqtimeout = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the PULSETIMERCLEAR field in the RWNXAGCRADARTIM register.
 *
 * The RWNXAGCRADARTIM register will be read and the PULSETIMERCLEAR field's value will be returned.
 *
 * @return The current value of the PULSETIMERCLEAR field in the RWNXAGCRADARTIM register.
 */
__INLINE uint8_t agc_pulsetimerclear_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCRADARTIM_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the PULSETIMERCLEAR field of the RWNXAGCRADARTIM register.
 *
 * The RWNXAGCRADARTIM register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pulsetimerclear - The value to set the field to.
 */
__INLINE void agc_pulsetimerclear_setf(uint8_t pulsetimerclear)
{
    ASSERT_ERR((((uint32_t)pulsetimerclear << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(AGC_RWNXAGCRADARTIM_ADDR, (REG_PL_RD(AGC_RWNXAGCRADARTIM_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)pulsetimerclear << 31));
}

/**
 * @brief Returns the current value of the PULSEIRQFIFOTHR field in the RWNXAGCRADARTIM register.
 *
 * The RWNXAGCRADARTIM register will be read and the PULSEIRQFIFOTHR field's value will be returned.
 *
 * @return The current value of the PULSEIRQFIFOTHR field in the RWNXAGCRADARTIM register.
 */
__INLINE uint8_t agc_pulseirqfifothr_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCRADARTIM_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

/**
 * @brief Sets the PULSEIRQFIFOTHR field of the RWNXAGCRADARTIM register.
 *
 * The RWNXAGCRADARTIM register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pulseirqfifothr - The value to set the field to.
 */
__INLINE void agc_pulseirqfifothr_setf(uint8_t pulseirqfifothr)
{
    ASSERT_ERR((((uint32_t)pulseirqfifothr << 16) & ~((uint32_t)0x00070000)) == 0);
    REG_PL_WR(AGC_RWNXAGCRADARTIM_ADDR, (REG_PL_RD(AGC_RWNXAGCRADARTIM_ADDR) & ~((uint32_t)0x00070000)) | ((uint32_t)pulseirqfifothr << 16));
}

/**
 * @brief Returns the current value of the PULSEIRQTIMEOUT field in the RWNXAGCRADARTIM register.
 *
 * The RWNXAGCRADARTIM register will be read and the PULSEIRQTIMEOUT field's value will be returned.
 *
 * @return The current value of the PULSEIRQTIMEOUT field in the RWNXAGCRADARTIM register.
 */
__INLINE uint8_t agc_pulseirqtimeout_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCRADARTIM_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the PULSEIRQTIMEOUT field of the RWNXAGCRADARTIM register.
 *
 * The RWNXAGCRADARTIM register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pulseirqtimeout - The value to set the field to.
 */
__INLINE void agc_pulseirqtimeout_setf(uint8_t pulseirqtimeout)
{
    ASSERT_ERR((((uint32_t)pulseirqtimeout << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCRADARTIM_ADDR, (REG_PL_RD(AGC_RWNXAGCRADARTIM_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)pulseirqtimeout << 0));
}

/// @}

/**
 * @name RWNXAGCDSP3 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  07:00        INBDPOWDBVTHR   0xBF
 * </pre>
 *
 * @{
 */

/// Address of the RWNXAGCDSP3 register
#define AGC_RWNXAGCDSP3_ADDR   0x010020A8
/// Offset of the RWNXAGCDSP3 register from the base address
#define AGC_RWNXAGCDSP3_OFFSET 0x000020A8
/// Index of the RWNXAGCDSP3 register
#define AGC_RWNXAGCDSP3_INDEX  0x0000082A
/// Reset value of the RWNXAGCDSP3 register
#define AGC_RWNXAGCDSP3_RESET  0x000000BF

/**
 * @brief Returns the current value of the RWNXAGCDSP3 register.
 * The RWNXAGCDSP3 register will be read and its value returned.
 * @return The current value of the RWNXAGCDSP3 register.
 */
__INLINE uint32_t agc_rwnxagcdsp3_get(void)
{
    return REG_PL_RD(AGC_RWNXAGCDSP3_ADDR);
}

/**
 * @brief Sets the RWNXAGCDSP3 register to a value.
 * The RWNXAGCDSP3 register will be written.
 * @param value - The value to write.
 */
__INLINE void agc_rwnxagcdsp3_set(uint32_t value)
{
    REG_PL_WR(AGC_RWNXAGCDSP3_ADDR, value);
}

// field definitions
/// INBDPOWDBVTHR field mask
#define AGC_INBDPOWDBVTHR_MASK   ((uint32_t)0x000000FF)
/// INBDPOWDBVTHR field LSB position
#define AGC_INBDPOWDBVTHR_LSB    0
/// INBDPOWDBVTHR field width
#define AGC_INBDPOWDBVTHR_WIDTH  ((uint32_t)0x00000008)

/// INBDPOWDBVTHR field reset value
#define AGC_INBDPOWDBVTHR_RST    0xBF

/**
 * @brief Returns the current value of the INBDPOWDBVTHR field in the RWNXAGCDSP3 register.
 *
 * The RWNXAGCDSP3 register will be read and the INBDPOWDBVTHR field's value will be returned.
 *
 * @return The current value of the INBDPOWDBVTHR field in the RWNXAGCDSP3 register.
 */
__INLINE uint8_t agc_inbdpowdbvthr_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGC_RWNXAGCDSP3_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000000FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the INBDPOWDBVTHR field of the RWNXAGCDSP3 register.
 *
 * The RWNXAGCDSP3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] inbdpowdbvthr - The value to set the field to.
 */
__INLINE void agc_inbdpowdbvthr_setf(uint8_t inbdpowdbvthr)
{
    ASSERT_ERR((((uint32_t)inbdpowdbvthr << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(AGC_RWNXAGCDSP3_ADDR, (uint32_t)inbdpowdbvthr << 0);
}

/// @}

/* On SOC platform, write agc value to (0x01C0A000) may not right */
#define PHY_AGC_UCODE_ADDR               (0x0100A000)

#endif // _REG_AGC_H_

/// @}

