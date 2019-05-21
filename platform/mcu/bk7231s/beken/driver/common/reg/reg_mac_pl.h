/**
 * @file reg_mac_pl.h
 * @brief Definitions of the NXMAC HW block registers and register access functions.
 *
 * @defgroup REG_MAC_PL REG_MAC_PL
 * @ingroup REG
 * @{
 *
 * @brief Definitions of the NXMAC HW block registers and register access functions.
 */
#ifndef _REG_MAC_PL_H_
#define _REG_MAC_PL_H_

#include "co_int.h"
#include "_reg_mac_pl.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"

/*lint -e(91)*/
/** @brief Number of registers in the REG_MAC_PL peripheral.
 */
#define REG_MAC_PL_COUNT 351

/** @brief Decoding mask of the REG_MAC_PL peripheral registers from the CPU point of view.
 */
#define REG_MAC_PL_DECODING_MASK 0x000007FF

/**
 * @name NEXT_TBTT register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00             nextTBTT   0x0
 * </pre>
 *
 * @{
 */

/// Address of the NEXT_TBTT register
#define NXMAC_NEXT_TBTT_ADDR   0xC0008040
/// Offset of the NEXT_TBTT register from the base address
#define NXMAC_NEXT_TBTT_OFFSET 0x00000040
/// Index of the NEXT_TBTT register
#define NXMAC_NEXT_TBTT_INDEX  0x00000010
/// Reset value of the NEXT_TBTT register
#define NXMAC_NEXT_TBTT_RESET  0x00000000

/**
 * @brief Returns the current value of the NEXT_TBTT register.
 * The NEXT_TBTT register will be read and its value returned.
 * @return The current value of the NEXT_TBTT register.
 */
__INLINE uint32_t nxmac_next_tbtt_get(void)
{
    return REG_PL_RD(NXMAC_NEXT_TBTT_ADDR);
}

// field definitions
/// NEXT_TBTT field mask
#define NXMAC_NEXT_TBTT_MASK   ((uint32_t)0x0000FFFF)
/// NEXT_TBTT field LSB position
#define NXMAC_NEXT_TBTT_LSB    0
/// NEXT_TBTT field width
#define NXMAC_NEXT_TBTT_WIDTH  ((uint32_t)0x00000010)

/// NEXT_TBTT field reset value
#define NXMAC_NEXT_TBTT_RST    0x0

/**
 * @brief Returns the current value of the nextTBTT field in the NEXT_TBTT register.
 *
 * The NEXT_TBTT register will be read and the nextTBTT field's value will be returned.
 *
 * @return The current value of the nextTBTT field in the NEXT_TBTT register.
 */
__INLINE uint16_t nxmac_next_tbtt_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_NEXT_TBTT_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name DOZE_CNTRL_2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31       wakeUpFromDoze   0
 *     00             wakeUpSW   1
 * </pre>
 *
 * @{
 */

/// Address of the DOZE_CNTRL_2 register
#define NXMAC_DOZE_CNTRL_2_ADDR   0xC0008048
/// Offset of the DOZE_CNTRL_2 register from the base address
#define NXMAC_DOZE_CNTRL_2_OFFSET 0x00000048
/// Index of the DOZE_CNTRL_2 register
#define NXMAC_DOZE_CNTRL_2_INDEX  0x00000012
/// Reset value of the DOZE_CNTRL_2 register
#define NXMAC_DOZE_CNTRL_2_RESET  0x00000001

/**
 * @brief Returns the current value of the DOZE_CNTRL_2 register.
 * The DOZE_CNTRL_2 register will be read and its value returned.
 * @return The current value of the DOZE_CNTRL_2 register.
 */
__INLINE uint32_t nxmac_doze_cntrl_2_get(void)
{
    return REG_PL_RD(NXMAC_DOZE_CNTRL_2_ADDR);
}

/**
 * @brief Sets the DOZE_CNTRL_2 register to a value.
 * The DOZE_CNTRL_2 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_doze_cntrl_2_set(uint32_t value)
{
    REG_PL_WR(NXMAC_DOZE_CNTRL_2_ADDR, value);
}

// field definitions
/// WAKE_UP_FROM_DOZE field bit
#define NXMAC_WAKE_UP_FROM_DOZE_BIT    ((uint32_t)0x80000000)
/// WAKE_UP_FROM_DOZE field position
#define NXMAC_WAKE_UP_FROM_DOZE_POS    31
/// WAKE_UP_SW field bit
#define NXMAC_WAKE_UP_SW_BIT           ((uint32_t)0x00000001)
/// WAKE_UP_SW field position
#define NXMAC_WAKE_UP_SW_POS           0

/// WAKE_UP_FROM_DOZE field reset value
#define NXMAC_WAKE_UP_FROM_DOZE_RST    0x0
/// WAKE_UP_SW field reset value
#define NXMAC_WAKE_UP_SW_RST           0x1

/**
 * @brief Constructs a value for the DOZE_CNTRL_2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] wakeupfromdoze - The value to use for the wakeUpFromDoze field.
 * @param[in] wakeupsw - The value to use for the wakeUpSW field.
 */
__INLINE void nxmac_doze_cntrl_2_pack(uint8_t wakeupfromdoze, uint8_t wakeupsw)
{
    ASSERT_ERR((((uint32_t)wakeupfromdoze << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)wakeupsw << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_DOZE_CNTRL_2_ADDR,  ((uint32_t)wakeupfromdoze << 31) | ((uint32_t)wakeupsw << 0));
}

/**
 * @brief Unpacks DOZE_CNTRL_2's fields from current value of the DOZE_CNTRL_2 register.
 *
 * Reads the DOZE_CNTRL_2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] wakeupfromdoze - Will be populated with the current value of this field from the register.
 * @param[out] wakeupsw - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_doze_cntrl_2_unpack(uint8_t *wakeupfromdoze, uint8_t *wakeupsw)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DOZE_CNTRL_2_ADDR);

    *wakeupfromdoze = (localVal & ((uint32_t)0x80000000)) >> 31;
    *wakeupsw = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the wakeUpFromDoze field in the DOZE_CNTRL_2 register.
 *
 * The DOZE_CNTRL_2 register will be read and the wakeUpFromDoze field's value will be returned.
 *
 * @return The current value of the wakeUpFromDoze field in the DOZE_CNTRL_2 register.
 */
__INLINE uint8_t nxmac_wake_up_from_doze_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DOZE_CNTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the wakeUpFromDoze field of the DOZE_CNTRL_2 register.
 *
 * The DOZE_CNTRL_2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] wakeupfromdoze - The value to set the field to.
 */
__INLINE void nxmac_wake_up_from_doze_setf(uint8_t wakeupfromdoze)
{
    ASSERT_ERR((((uint32_t)wakeupfromdoze << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(NXMAC_DOZE_CNTRL_2_ADDR, (REG_PL_RD(NXMAC_DOZE_CNTRL_2_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)wakeupfromdoze << 31));
}

/**
 * @brief Returns the current value of the wakeUpSW field in the DOZE_CNTRL_2 register.
 *
 * The DOZE_CNTRL_2 register will be read and the wakeUpSW field's value will be returned.
 *
 * @return The current value of the wakeUpSW field in the DOZE_CNTRL_2 register.
 */
__INLINE uint8_t nxmac_wake_up_sw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DOZE_CNTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the wakeUpSW field of the DOZE_CNTRL_2 register.
 *
 * The DOZE_CNTRL_2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] wakeupsw - The value to set the field to.
 */
__INLINE void nxmac_wake_up_sw_setf(uint8_t wakeupsw)
{
    ASSERT_ERR((((uint32_t)wakeupsw << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_DOZE_CNTRL_2_ADDR, (REG_PL_RD(NXMAC_DOZE_CNTRL_2_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)wakeupsw << 0));
}

/// @}

/**
 * @name MAC_CNTRL_2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     00            softReset   0
 * </pre>
 *
 * @{
 */

/// Address of the MAC_CNTRL_2 register
#define NXMAC_MAC_CNTRL_2_ADDR   0xC0008050
/// Offset of the MAC_CNTRL_2 register from the base address
#define NXMAC_MAC_CNTRL_2_OFFSET 0x00000050
/// Index of the MAC_CNTRL_2 register
#define NXMAC_MAC_CNTRL_2_INDEX  0x00000014
/// Reset value of the MAC_CNTRL_2 register
#define NXMAC_MAC_CNTRL_2_RESET  0x00000000

/**
 * @brief Returns the current value of the MAC_CNTRL_2 register.
 * The MAC_CNTRL_2 register will be read and its value returned.
 * @return The current value of the MAC_CNTRL_2 register.
 */
__INLINE uint32_t nxmac_mac_cntrl_2_get(void)
{
    return REG_PL_RD(NXMAC_MAC_CNTRL_2_ADDR);
}

/**
 * @brief Sets the MAC_CNTRL_2 register to a value.
 * The MAC_CNTRL_2 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_mac_cntrl_2_set(uint32_t value)
{
    REG_PL_WR(NXMAC_MAC_CNTRL_2_ADDR, value);
}

// field definitions
/// SOFT_RESET field bit
#define NXMAC_SOFT_RESET_BIT    ((uint32_t)0x00000001)
/// SOFT_RESET field position
#define NXMAC_SOFT_RESET_POS    0

/// SOFT_RESET field reset value
#define NXMAC_SOFT_RESET_RST    0x0

/**
 * @brief Returns the current value of the softReset field in the MAC_CNTRL_2 register.
 *
 * The MAC_CNTRL_2 register will be read and the softReset field's value will be returned.
 *
 * @return The current value of the softReset field in the MAC_CNTRL_2 register.
 */
__INLINE uint8_t nxmac_soft_reset_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_CNTRL_2_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000001)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the softReset field of the MAC_CNTRL_2 register.
 *
 * The MAC_CNTRL_2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] softreset - The value to set the field to.
 */
__INLINE void nxmac_soft_reset_setf(uint8_t softreset)
{
    ASSERT_ERR((((uint32_t)softreset << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_MAC_CNTRL_2_ADDR, (uint32_t)softreset << 0);
}

/// @}

/**
 * @name GEN_INT_STATUS register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     25     rxPayloadDMADead   0
 *     24      rxHeaderDMADead   0
 *     23           phyRxStart   0
 *     22               phyErr   0
 *     21     macPHYIFUnderRun   0
 *     20                hwErr   0
 *     19           impSecDTIM   0
 *     18           impPriDTIM   0
 *     17         bcnTxDMADead   0
 *     16         ac3TxDMADead   0
 *     15         ac2TxDMADead   0
 *     14         ac1TxDMADead   0
 *     13         ac0TxDMADead   0
 *     12              ptError   0
 *     11               timSet   0
 *     10             olbcDSSS   0
 *     09             olbcOFDM   0
 *     08       rxFIFOOverFlow   0
 *     07           rxDMAEmpty   0
 *     06     macPHYIFOverflow   0
 *     05            coexEvent   0
 *     03         absGenTimers   0
 *     02        idleInterrupt   0
 *     01           impSecTBTT   0
 *     00           impPriTBTT   0
 * </pre>
 *
 * @{
 */

/// Address of the GEN_INT_STATUS register
#define NXMAC_GEN_INT_STATUS_ADDR   0xC000806C
/// Offset of the GEN_INT_STATUS register from the base address
#define NXMAC_GEN_INT_STATUS_OFFSET 0x0000006C
/// Index of the GEN_INT_STATUS register
#define NXMAC_GEN_INT_STATUS_INDEX  0x0000001B
/// Reset value of the GEN_INT_STATUS register
#define NXMAC_GEN_INT_STATUS_RESET  0x00000000

/**
 * @brief Returns the current value of the GEN_INT_STATUS register.
 * The GEN_INT_STATUS register will be read and its value returned.
 * @return The current value of the GEN_INT_STATUS register.
 */
__INLINE uint32_t nxmac_gen_int_status_get(void)
{
    return REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
}

/**
 * @brief Sets the GEN_INT_STATUS register to a value.
 * The GEN_INT_STATUS register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_gen_int_status_set(uint32_t value)
{
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, value);
}

// fields defined in symmetrical set/clear register
/**
 * @brief Constructs a value for the GEN_INT_STATUS register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxpayloaddmadead - The value to use for the rxPayloadDMADead field.
 * @param[in] rxheaderdmadead - The value to use for the rxHeaderDMADead field.
 * @param[in] phyrxstart - The value to use for the phyRxStart field.
 * @param[in] phyerr - The value to use for the phyErr field.
 * @param[in] macphyifunderrun - The value to use for the macPHYIFUnderRun field.
 * @param[in] hwerr - The value to use for the hwErr field.
 * @param[in] impsecdtim - The value to use for the impSecDTIM field.
 * @param[in] imppridtim - The value to use for the impPriDTIM field.
 * @param[in] bcntxdmadead - The value to use for the bcnTxDMADead field.
 * @param[in] ac3txdmadead - The value to use for the ac3TxDMADead field.
 * @param[in] ac2txdmadead - The value to use for the ac2TxDMADead field.
 * @param[in] ac1txdmadead - The value to use for the ac1TxDMADead field.
 * @param[in] ac0txdmadead - The value to use for the ac0TxDMADead field.
 * @param[in] pterror - The value to use for the ptError field.
 * @param[in] timset - The value to use for the timSet field.
 * @param[in] olbcdsss - The value to use for the olbcDSSS field.
 * @param[in] olbcofdm - The value to use for the olbcOFDM field.
 * @param[in] rxfifooverflow - The value to use for the rxFIFOOverFlow field.
 * @param[in] rxdmaempty - The value to use for the rxDMAEmpty field.
 * @param[in] macphyifoverflow - The value to use for the macPHYIFOverflow field.
 * @param[in] coexevent - The value to use for the coexEvent field.
 * @param[in] idleinterrupt - The value to use for the idleInterrupt field.
 * @param[in] impsectbtt - The value to use for the impSecTBTT field.
 * @param[in] imppritbtt - The value to use for the impPriTBTT field.
 */
__INLINE void nxmac_gen_int_status_pack(uint8_t rxpayloaddmadead, uint8_t rxheaderdmadead, uint8_t phyrxstart, uint8_t phyerr, uint8_t macphyifunderrun, uint8_t hwerr, uint8_t impsecdtim, uint8_t imppridtim, uint8_t bcntxdmadead, uint8_t ac3txdmadead, uint8_t ac2txdmadead, uint8_t ac1txdmadead, uint8_t ac0txdmadead, uint8_t pterror, uint8_t timset, uint8_t olbcdsss, uint8_t olbcofdm, uint8_t rxfifooverflow, uint8_t rxdmaempty, uint8_t macphyifoverflow, uint8_t coexevent, uint8_t idleinterrupt, uint8_t impsectbtt, uint8_t imppritbtt)
{
    ASSERT_ERR((((uint32_t)rxpayloaddmadead << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)rxheaderdmadead << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)phyrxstart << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)phyerr << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)macphyifunderrun << 21) & ~((uint32_t)0x00200000)) == 0);
    ASSERT_ERR((((uint32_t)hwerr << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)impsecdtim << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)imppridtim << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)bcntxdmadead << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)ac3txdmadead << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)ac2txdmadead << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)ac1txdmadead << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)ac0txdmadead << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)pterror << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)timset << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)olbcdsss << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)olbcofdm << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)rxfifooverflow << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)rxdmaempty << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)macphyifoverflow << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)coexevent << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)idleinterrupt << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)impsectbtt << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)imppritbtt << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR,  ((uint32_t)rxpayloaddmadead << 25) | ((uint32_t)rxheaderdmadead << 24) | ((uint32_t)phyrxstart << 23) | ((uint32_t)phyerr << 22) | ((uint32_t)macphyifunderrun << 21) | ((uint32_t)hwerr << 20) | ((uint32_t)impsecdtim << 19) | ((uint32_t)imppridtim << 18) | ((uint32_t)bcntxdmadead << 17) | ((uint32_t)ac3txdmadead << 16) | ((uint32_t)ac2txdmadead << 15) | ((uint32_t)ac1txdmadead << 14) | ((uint32_t)ac0txdmadead << 13) | ((uint32_t)pterror << 12) | ((uint32_t)timset << 11) | ((uint32_t)olbcdsss << 10) | ((uint32_t)olbcofdm << 9) | ((uint32_t)rxfifooverflow << 8) | ((uint32_t)rxdmaempty << 7) | ((uint32_t)macphyifoverflow << 6) | ((uint32_t)coexevent << 5) | ((uint32_t)idleinterrupt << 2) | ((uint32_t)impsectbtt << 1) | ((uint32_t)imppritbtt << 0));
}

/**
 * @brief Unpacks GEN_INT_STATUS's fields from current value of the GEN_INT_STATUS register.
 *
 * Reads the GEN_INT_STATUS register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxpayloaddmadead - Will be populated with the current value of this field from the register.
 * @param[out] rxheaderdmadead - Will be populated with the current value of this field from the register.
 * @param[out] phyrxstart - Will be populated with the current value of this field from the register.
 * @param[out] phyerr - Will be populated with the current value of this field from the register.
 * @param[out] macphyifunderrun - Will be populated with the current value of this field from the register.
 * @param[out] hwerr - Will be populated with the current value of this field from the register.
 * @param[out] impsecdtim - Will be populated with the current value of this field from the register.
 * @param[out] imppridtim - Will be populated with the current value of this field from the register.
 * @param[out] bcntxdmadead - Will be populated with the current value of this field from the register.
 * @param[out] ac3txdmadead - Will be populated with the current value of this field from the register.
 * @param[out] ac2txdmadead - Will be populated with the current value of this field from the register.
 * @param[out] ac1txdmadead - Will be populated with the current value of this field from the register.
 * @param[out] ac0txdmadead - Will be populated with the current value of this field from the register.
 * @param[out] pterror - Will be populated with the current value of this field from the register.
 * @param[out] timset - Will be populated with the current value of this field from the register.
 * @param[out] olbcdsss - Will be populated with the current value of this field from the register.
 * @param[out] olbcofdm - Will be populated with the current value of this field from the register.
 * @param[out] rxfifooverflow - Will be populated with the current value of this field from the register.
 * @param[out] rxdmaempty - Will be populated with the current value of this field from the register.
 * @param[out] macphyifoverflow - Will be populated with the current value of this field from the register.
 * @param[out] coexevent - Will be populated with the current value of this field from the register.
 * @param[out] absgentimers - Will be populated with the current value of this field from the register.
 * @param[out] idleinterrupt - Will be populated with the current value of this field from the register.
 * @param[out] impsectbtt - Will be populated with the current value of this field from the register.
 * @param[out] imppritbtt - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_gen_int_status_unpack(uint8_t *rxpayloaddmadead, uint8_t *rxheaderdmadead, uint8_t *phyrxstart, uint8_t *phyerr, uint8_t *macphyifunderrun, uint8_t *hwerr, uint8_t *impsecdtim, uint8_t *imppridtim, uint8_t *bcntxdmadead, uint8_t *ac3txdmadead, uint8_t *ac2txdmadead, uint8_t *ac1txdmadead, uint8_t *ac0txdmadead, uint8_t *pterror, uint8_t *timset, uint8_t *olbcdsss, uint8_t *olbcofdm, uint8_t *rxfifooverflow, uint8_t *rxdmaempty, uint8_t *macphyifoverflow, uint8_t *coexevent, uint8_t *absgentimers, uint8_t *idleinterrupt, uint8_t *impsectbtt, uint8_t *imppritbtt)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);

    *rxpayloaddmadead = (localVal & ((uint32_t)0x02000000)) >> 25;
    *rxheaderdmadead = (localVal & ((uint32_t)0x01000000)) >> 24;
    *phyrxstart = (localVal & ((uint32_t)0x00800000)) >> 23;
    *phyerr = (localVal & ((uint32_t)0x00400000)) >> 22;
    *macphyifunderrun = (localVal & ((uint32_t)0x00200000)) >> 21;
    *hwerr = (localVal & ((uint32_t)0x00100000)) >> 20;
    *impsecdtim = (localVal & ((uint32_t)0x00080000)) >> 19;
    *imppridtim = (localVal & ((uint32_t)0x00040000)) >> 18;
    *bcntxdmadead = (localVal & ((uint32_t)0x00020000)) >> 17;
    *ac3txdmadead = (localVal & ((uint32_t)0x00010000)) >> 16;
    *ac2txdmadead = (localVal & ((uint32_t)0x00008000)) >> 15;
    *ac1txdmadead = (localVal & ((uint32_t)0x00004000)) >> 14;
    *ac0txdmadead = (localVal & ((uint32_t)0x00002000)) >> 13;
    *pterror = (localVal & ((uint32_t)0x00001000)) >> 12;
    *timset = (localVal & ((uint32_t)0x00000800)) >> 11;
    *olbcdsss = (localVal & ((uint32_t)0x00000400)) >> 10;
    *olbcofdm = (localVal & ((uint32_t)0x00000200)) >> 9;
    *rxfifooverflow = (localVal & ((uint32_t)0x00000100)) >> 8;
    *rxdmaempty = (localVal & ((uint32_t)0x00000080)) >> 7;
    *macphyifoverflow = (localVal & ((uint32_t)0x00000040)) >> 6;
    *coexevent = (localVal & ((uint32_t)0x00000020)) >> 5;
    *absgentimers = (localVal & ((uint32_t)0x00000008)) >> 3;
    *idleinterrupt = (localVal & ((uint32_t)0x00000004)) >> 2;
    *impsectbtt = (localVal & ((uint32_t)0x00000002)) >> 1;
    *imppritbtt = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the rxPayloadDMADead field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the rxPayloadDMADead field's value will be returned.
 *
 * @return The current value of the rxPayloadDMADead field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_rx_payload_dma_dead_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

/**
 * @brief Sets the rxPayloadDMADead field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxpayloaddmadead - The value to set the field to.
 */
__INLINE void nxmac_status_rx_payload_dma_dead_setf(uint8_t rxpayloaddmadead)
{
    ASSERT_ERR((((uint32_t)rxpayloaddmadead << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)rxpayloaddmadead << 25);
}

/**
 * @brief Returns the current value of the rxHeaderDMADead field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the rxHeaderDMADead field's value will be returned.
 *
 * @return The current value of the rxHeaderDMADead field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_rx_header_dma_dead_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the rxHeaderDMADead field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxheaderdmadead - The value to set the field to.
 */
__INLINE void nxmac_status_rx_header_dma_dead_setf(uint8_t rxheaderdmadead)
{
    ASSERT_ERR((((uint32_t)rxheaderdmadead << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)rxheaderdmadead << 24);
}

/**
 * @brief Returns the current value of the phyRxStart field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the phyRxStart field's value will be returned.
 *
 * @return The current value of the phyRxStart field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_phy_rx_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Sets the phyRxStart field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] phyrxstart - The value to set the field to.
 */
__INLINE void nxmac_status_phy_rx_start_setf(uint8_t phyrxstart)
{
    ASSERT_ERR((((uint32_t)phyrxstart << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)phyrxstart << 23);
}

/**
 * @brief Returns the current value of the phyErr field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the phyErr field's value will be returned.
 *
 * @return The current value of the phyErr field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_phy_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Sets the phyErr field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] phyerr - The value to set the field to.
 */
__INLINE void nxmac_status_phy_err_setf(uint8_t phyerr)
{
    ASSERT_ERR((((uint32_t)phyerr << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)phyerr << 22);
}

/**
 * @brief Returns the current value of the macPHYIFUnderRun field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the macPHYIFUnderRun field's value will be returned.
 *
 * @return The current value of the macPHYIFUnderRun field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_mac_phyif_under_run_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Sets the macPHYIFUnderRun field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macphyifunderrun - The value to set the field to.
 */
__INLINE void nxmac_status_mac_phyif_under_run_setf(uint8_t macphyifunderrun)
{
    ASSERT_ERR((((uint32_t)macphyifunderrun << 21) & ~((uint32_t)0x00200000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)macphyifunderrun << 21);
}

/**
 * @brief Returns the current value of the hwErr field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the hwErr field's value will be returned.
 *
 * @return The current value of the hwErr field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_hw_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Sets the hwErr field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] hwerr - The value to set the field to.
 */
__INLINE void nxmac_status_hw_err_setf(uint8_t hwerr)
{
    ASSERT_ERR((((uint32_t)hwerr << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)hwerr << 20);
}

/**
 * @brief Returns the current value of the impSecDTIM field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the impSecDTIM field's value will be returned.
 *
 * @return The current value of the impSecDTIM field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_imp_sec_dtim_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Sets the impSecDTIM field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] impsecdtim - The value to set the field to.
 */
__INLINE void nxmac_status_imp_sec_dtim_setf(uint8_t impsecdtim)
{
    ASSERT_ERR((((uint32_t)impsecdtim << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)impsecdtim << 19);
}

/**
 * @brief Returns the current value of the impPriDTIM field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the impPriDTIM field's value will be returned.
 *
 * @return The current value of the impPriDTIM field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_imp_pri_dtim_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Sets the impPriDTIM field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] imppridtim - The value to set the field to.
 */
__INLINE void nxmac_status_imp_pri_dtim_setf(uint8_t imppridtim)
{
    ASSERT_ERR((((uint32_t)imppridtim << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)imppridtim << 18);
}

/**
 * @brief Returns the current value of the bcnTxDMADead field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the bcnTxDMADead field's value will be returned.
 *
 * @return The current value of the bcnTxDMADead field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_bcn_tx_dma_dead_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Sets the bcnTxDMADead field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bcntxdmadead - The value to set the field to.
 */
__INLINE void nxmac_status_bcn_tx_dma_dead_setf(uint8_t bcntxdmadead)
{
    ASSERT_ERR((((uint32_t)bcntxdmadead << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)bcntxdmadead << 17);
}

/**
 * @brief Returns the current value of the ac3TxDMADead field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the ac3TxDMADead field's value will be returned.
 *
 * @return The current value of the ac3TxDMADead field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_3_tx_dma_dead_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the ac3TxDMADead field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac3txdmadead - The value to set the field to.
 */
__INLINE void nxmac_status_ac_3_tx_dma_dead_setf(uint8_t ac3txdmadead)
{
    ASSERT_ERR((((uint32_t)ac3txdmadead << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)ac3txdmadead << 16);
}

/**
 * @brief Returns the current value of the ac2TxDMADead field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the ac2TxDMADead field's value will be returned.
 *
 * @return The current value of the ac2TxDMADead field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_2_tx_dma_dead_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

/**
 * @brief Sets the ac2TxDMADead field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac2txdmadead - The value to set the field to.
 */
__INLINE void nxmac_status_ac_2_tx_dma_dead_setf(uint8_t ac2txdmadead)
{
    ASSERT_ERR((((uint32_t)ac2txdmadead << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)ac2txdmadead << 15);
}

/**
 * @brief Returns the current value of the ac1TxDMADead field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the ac1TxDMADead field's value will be returned.
 *
 * @return The current value of the ac1TxDMADead field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_1_tx_dma_dead_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Sets the ac1TxDMADead field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac1txdmadead - The value to set the field to.
 */
__INLINE void nxmac_status_ac_1_tx_dma_dead_setf(uint8_t ac1txdmadead)
{
    ASSERT_ERR((((uint32_t)ac1txdmadead << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)ac1txdmadead << 14);
}

/**
 * @brief Returns the current value of the ac0TxDMADead field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the ac0TxDMADead field's value will be returned.
 *
 * @return The current value of the ac0TxDMADead field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_0_tx_dma_dead_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the ac0TxDMADead field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac0txdmadead - The value to set the field to.
 */
__INLINE void nxmac_status_ac_0_tx_dma_dead_setf(uint8_t ac0txdmadead)
{
    ASSERT_ERR((((uint32_t)ac0txdmadead << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)ac0txdmadead << 13);
}

/**
 * @brief Returns the current value of the ptError field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the ptError field's value will be returned.
 *
 * @return The current value of the ptError field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_pt_error_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the ptError field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pterror - The value to set the field to.
 */
__INLINE void nxmac_status_pt_error_setf(uint8_t pterror)
{
    ASSERT_ERR((((uint32_t)pterror << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)pterror << 12);
}

/**
 * @brief Returns the current value of the timSet field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the timSet field's value will be returned.
 *
 * @return The current value of the timSet field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_tim_set_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the timSet field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] timset - The value to set the field to.
 */
__INLINE void nxmac_status_tim_set_setf(uint8_t timset)
{
    ASSERT_ERR((((uint32_t)timset << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)timset << 11);
}

/**
 * @brief Returns the current value of the olbcDSSS field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the olbcDSSS field's value will be returned.
 *
 * @return The current value of the olbcDSSS field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_olbc_dsss_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the olbcDSSS field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] olbcdsss - The value to set the field to.
 */
__INLINE void nxmac_status_olbc_dsss_setf(uint8_t olbcdsss)
{
    ASSERT_ERR((((uint32_t)olbcdsss << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)olbcdsss << 10);
}

/**
 * @brief Returns the current value of the olbcOFDM field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the olbcOFDM field's value will be returned.
 *
 * @return The current value of the olbcOFDM field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_olbc_ofdm_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the olbcOFDM field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] olbcofdm - The value to set the field to.
 */
__INLINE void nxmac_status_olbc_ofdm_setf(uint8_t olbcofdm)
{
    ASSERT_ERR((((uint32_t)olbcofdm << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)olbcofdm << 9);
}

/**
 * @brief Returns the current value of the rxFIFOOverFlow field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the rxFIFOOverFlow field's value will be returned.
 *
 * @return The current value of the rxFIFOOverFlow field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_rx_fifo_over_flow_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the rxFIFOOverFlow field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxfifooverflow - The value to set the field to.
 */
__INLINE void nxmac_status_rx_fifo_over_flow_setf(uint8_t rxfifooverflow)
{
    ASSERT_ERR((((uint32_t)rxfifooverflow << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)rxfifooverflow << 8);
}

/**
 * @brief Returns the current value of the rxDMAEmpty field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the rxDMAEmpty field's value will be returned.
 *
 * @return The current value of the rxDMAEmpty field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_rx_dma_empty_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the rxDMAEmpty field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxdmaempty - The value to set the field to.
 */
__INLINE void nxmac_status_rx_dma_empty_setf(uint8_t rxdmaempty)
{
    ASSERT_ERR((((uint32_t)rxdmaempty << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)rxdmaempty << 7);
}

/**
 * @brief Returns the current value of the macPHYIFOverflow field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the macPHYIFOverflow field's value will be returned.
 *
 * @return The current value of the macPHYIFOverflow field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_mac_phyif_overflow_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the macPHYIFOverflow field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macphyifoverflow - The value to set the field to.
 */
__INLINE void nxmac_status_mac_phyif_overflow_setf(uint8_t macphyifoverflow)
{
    ASSERT_ERR((((uint32_t)macphyifoverflow << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)macphyifoverflow << 6);
}

/**
 * @brief Returns the current value of the coexEvent field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the coexEvent field's value will be returned.
 *
 * @return The current value of the coexEvent field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_coex_event_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the coexEvent field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexevent - The value to set the field to.
 */
__INLINE void nxmac_status_coex_event_setf(uint8_t coexevent)
{
    ASSERT_ERR((((uint32_t)coexevent << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)coexevent << 5);
}

/**
 * @brief Returns the current value of the absGenTimers field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the absGenTimers field's value will be returned.
 *
 * @return The current value of the absGenTimers field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_abs_gen_timers_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Returns the current value of the idleInterrupt field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the idleInterrupt field's value will be returned.
 *
 * @return The current value of the idleInterrupt field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_idle_interrupt_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the idleInterrupt field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] idleinterrupt - The value to set the field to.
 */
__INLINE void nxmac_status_idle_interrupt_setf(uint8_t idleinterrupt)
{
    ASSERT_ERR((((uint32_t)idleinterrupt << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)idleinterrupt << 2);
}

/**
 * @brief Returns the current value of the impSecTBTT field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the impSecTBTT field's value will be returned.
 *
 * @return The current value of the impSecTBTT field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_imp_sec_tbtt_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the impSecTBTT field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] impsectbtt - The value to set the field to.
 */
__INLINE void nxmac_status_imp_sec_tbtt_setf(uint8_t impsectbtt)
{
    ASSERT_ERR((((uint32_t)impsectbtt << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)impsectbtt << 1);
}

/**
 * @brief Returns the current value of the impPriTBTT field in the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read and the impPriTBTT field's value will be returned.
 *
 * @return The current value of the impPriTBTT field in the GEN_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_imp_pri_tbtt_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the impPriTBTT field of the GEN_INT_STATUS register.
 *
 * The GEN_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] imppritbtt - The value to set the field to.
 */
__INLINE void nxmac_status_imp_pri_tbtt_setf(uint8_t imppritbtt)
{
    ASSERT_ERR((((uint32_t)imppritbtt << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_STATUS_ADDR, (uint32_t)imppritbtt << 0);
}

/// @}

/**
 * @name GEN_INT_ACK register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     25     rxPayloadDMADead   0
 *     24      rxHeaderDMADead   0
 *     23           phyRxStart   0
 *     22               phyErr   0
 *     21     macPHYIFUnderRun   0
 *     20                hwErr   0
 *     19           impSecDTIM   0
 *     18           impPriDTIM   0
 *     17         bcnTxDMADead   0
 *     16         ac3TxDMADead   0
 *     15         ac2TxDMADead   0
 *     14         ac1TxDMADead   0
 *     13         ac0TxDMADead   0
 *     12              ptError   0
 *     11               timSet   0
 *     10             olbcDSSS   0
 *     09             olbcOFDM   0
 *     08       rxFIFOOverFlow   0
 *     07           rxDMAEmpty   0
 *     06     macPHYIFOverflow   0
 *     05            coexEvent   0
 *     02        idleInterrupt   0
 *     01           impSecTBTT   0
 *     00           impPriTBTT   0
 * </pre>
 *
 * @{
 */

/// Address of the GEN_INT_ACK register
#define NXMAC_GEN_INT_ACK_ADDR   0xC0008070
/// Offset of the GEN_INT_ACK register from the base address
#define NXMAC_GEN_INT_ACK_OFFSET 0x00000070
/// Index of the GEN_INT_ACK register
#define NXMAC_GEN_INT_ACK_INDEX  0x0000001C
/// Reset value of the GEN_INT_ACK register
#define NXMAC_GEN_INT_ACK_RESET  0x00000000

/**
 * @brief Sets the GEN_INT_ACK register to a value.
 * The GEN_INT_ACK register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_gen_int_ack_clear(uint32_t value)
{
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, value);
}

// fields defined in symmetrical set/clear register
/**
 * @brief Constructs a value for the GEN_INT_ACK register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxpayloaddmadead - The value to use for the rxPayloadDMADead field.
 * @param[in] rxheaderdmadead - The value to use for the rxHeaderDMADead field.
 * @param[in] phyrxstart - The value to use for the phyRxStart field.
 * @param[in] phyerr - The value to use for the phyErr field.
 * @param[in] macphyifunderrun - The value to use for the macPHYIFUnderRun field.
 * @param[in] hwerr - The value to use for the hwErr field.
 * @param[in] impsecdtim - The value to use for the impSecDTIM field.
 * @param[in] imppridtim - The value to use for the impPriDTIM field.
 * @param[in] bcntxdmadead - The value to use for the bcnTxDMADead field.
 * @param[in] ac3txdmadead - The value to use for the ac3TxDMADead field.
 * @param[in] ac2txdmadead - The value to use for the ac2TxDMADead field.
 * @param[in] ac1txdmadead - The value to use for the ac1TxDMADead field.
 * @param[in] ac0txdmadead - The value to use for the ac0TxDMADead field.
 * @param[in] pterror - The value to use for the ptError field.
 * @param[in] timset - The value to use for the timSet field.
 * @param[in] olbcdsss - The value to use for the olbcDSSS field.
 * @param[in] olbcofdm - The value to use for the olbcOFDM field.
 * @param[in] rxfifooverflow - The value to use for the rxFIFOOverFlow field.
 * @param[in] rxdmaempty - The value to use for the rxDMAEmpty field.
 * @param[in] macphyifoverflow - The value to use for the macPHYIFOverflow field.
 * @param[in] coexevent - The value to use for the coexEvent field.
 * @param[in] idleinterrupt - The value to use for the idleInterrupt field.
 * @param[in] impsectbtt - The value to use for the impSecTBTT field.
 * @param[in] imppritbtt - The value to use for the impPriTBTT field.
 */
__INLINE void nxmac_gen_int_ack_pack(uint8_t rxpayloaddmadead, uint8_t rxheaderdmadead, uint8_t phyrxstart, uint8_t phyerr, uint8_t macphyifunderrun, uint8_t hwerr, uint8_t impsecdtim, uint8_t imppridtim, uint8_t bcntxdmadead, uint8_t ac3txdmadead, uint8_t ac2txdmadead, uint8_t ac1txdmadead, uint8_t ac0txdmadead, uint8_t pterror, uint8_t timset, uint8_t olbcdsss, uint8_t olbcofdm, uint8_t rxfifooverflow, uint8_t rxdmaempty, uint8_t macphyifoverflow, uint8_t coexevent, uint8_t idleinterrupt, uint8_t impsectbtt, uint8_t imppritbtt)
{
    ASSERT_ERR((((uint32_t)rxpayloaddmadead << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)rxheaderdmadead << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)phyrxstart << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)phyerr << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)macphyifunderrun << 21) & ~((uint32_t)0x00200000)) == 0);
    ASSERT_ERR((((uint32_t)hwerr << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)impsecdtim << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)imppridtim << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)bcntxdmadead << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)ac3txdmadead << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)ac2txdmadead << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)ac1txdmadead << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)ac0txdmadead << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)pterror << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)timset << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)olbcdsss << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)olbcofdm << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)rxfifooverflow << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)rxdmaempty << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)macphyifoverflow << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)coexevent << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)idleinterrupt << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)impsectbtt << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)imppritbtt << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR,  ((uint32_t)rxpayloaddmadead << 25) | ((uint32_t)rxheaderdmadead << 24) | ((uint32_t)phyrxstart << 23) | ((uint32_t)phyerr << 22) | ((uint32_t)macphyifunderrun << 21) | ((uint32_t)hwerr << 20) | ((uint32_t)impsecdtim << 19) | ((uint32_t)imppridtim << 18) | ((uint32_t)bcntxdmadead << 17) | ((uint32_t)ac3txdmadead << 16) | ((uint32_t)ac2txdmadead << 15) | ((uint32_t)ac1txdmadead << 14) | ((uint32_t)ac0txdmadead << 13) | ((uint32_t)pterror << 12) | ((uint32_t)timset << 11) | ((uint32_t)olbcdsss << 10) | ((uint32_t)olbcofdm << 9) | ((uint32_t)rxfifooverflow << 8) | ((uint32_t)rxdmaempty << 7) | ((uint32_t)macphyifoverflow << 6) | ((uint32_t)coexevent << 5) | ((uint32_t)idleinterrupt << 2) | ((uint32_t)impsectbtt << 1) | ((uint32_t)imppritbtt << 0));
}

/**
 * @brief Sets the rxPayloadDMADead field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxpayloaddmadead - The value to set the field to.
 */
__INLINE void nxmac_ack_rx_payload_dma_dead_clearf(uint8_t rxpayloaddmadead)
{
    ASSERT_ERR((((uint32_t)rxpayloaddmadead << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)rxpayloaddmadead << 25);
}

/**
 * @brief Sets the rxHeaderDMADead field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxheaderdmadead - The value to set the field to.
 */
__INLINE void nxmac_ack_rx_header_dma_dead_clearf(uint8_t rxheaderdmadead)
{
    ASSERT_ERR((((uint32_t)rxheaderdmadead << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)rxheaderdmadead << 24);
}

/**
 * @brief Sets the phyRxStart field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] phyrxstart - The value to set the field to.
 */
__INLINE void nxmac_ack_phy_rx_start_clearf(uint8_t phyrxstart)
{
    ASSERT_ERR((((uint32_t)phyrxstart << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)phyrxstart << 23);
}

/**
 * @brief Sets the phyErr field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] phyerr - The value to set the field to.
 */
__INLINE void nxmac_ack_phy_err_clearf(uint8_t phyerr)
{
    ASSERT_ERR((((uint32_t)phyerr << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)phyerr << 22);
}

/**
 * @brief Sets the macPHYIFUnderRun field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macphyifunderrun - The value to set the field to.
 */
__INLINE void nxmac_ack_mac_phyif_under_run_clearf(uint8_t macphyifunderrun)
{
    ASSERT_ERR((((uint32_t)macphyifunderrun << 21) & ~((uint32_t)0x00200000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)macphyifunderrun << 21);
}

/**
 * @brief Sets the hwErr field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] hwerr - The value to set the field to.
 */
__INLINE void nxmac_ack_hw_err_clearf(uint8_t hwerr)
{
    ASSERT_ERR((((uint32_t)hwerr << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)hwerr << 20);
}

/**
 * @brief Sets the impSecDTIM field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] impsecdtim - The value to set the field to.
 */
__INLINE void nxmac_ack_imp_sec_dtim_clearf(uint8_t impsecdtim)
{
    ASSERT_ERR((((uint32_t)impsecdtim << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)impsecdtim << 19);
}

/**
 * @brief Sets the impPriDTIM field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] imppridtim - The value to set the field to.
 */
__INLINE void nxmac_ack_imp_pri_dtim_clearf(uint8_t imppridtim)
{
    ASSERT_ERR((((uint32_t)imppridtim << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)imppridtim << 18);
}

/**
 * @brief Sets the bcnTxDMADead field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bcntxdmadead - The value to set the field to.
 */
__INLINE void nxmac_ack_bcn_tx_dma_dead_clearf(uint8_t bcntxdmadead)
{
    ASSERT_ERR((((uint32_t)bcntxdmadead << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)bcntxdmadead << 17);
}

/**
 * @brief Sets the ac3TxDMADead field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac3txdmadead - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_3_tx_dma_dead_clearf(uint8_t ac3txdmadead)
{
    ASSERT_ERR((((uint32_t)ac3txdmadead << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)ac3txdmadead << 16);
}

/**
 * @brief Sets the ac2TxDMADead field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac2txdmadead - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_2_tx_dma_dead_clearf(uint8_t ac2txdmadead)
{
    ASSERT_ERR((((uint32_t)ac2txdmadead << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)ac2txdmadead << 15);
}

/**
 * @brief Sets the ac1TxDMADead field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac1txdmadead - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_1_tx_dma_dead_clearf(uint8_t ac1txdmadead)
{
    ASSERT_ERR((((uint32_t)ac1txdmadead << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)ac1txdmadead << 14);
}

/**
 * @brief Sets the ac0TxDMADead field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac0txdmadead - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_0_tx_dma_dead_clearf(uint8_t ac0txdmadead)
{
    ASSERT_ERR((((uint32_t)ac0txdmadead << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)ac0txdmadead << 13);
}

/**
 * @brief Sets the ptError field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pterror - The value to set the field to.
 */
__INLINE void nxmac_ack_pt_error_clearf(uint8_t pterror)
{
    ASSERT_ERR((((uint32_t)pterror << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)pterror << 12);
}

/**
 * @brief Sets the timSet field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] timset - The value to set the field to.
 */
__INLINE void nxmac_ack_tim_set_clearf(uint8_t timset)
{
    ASSERT_ERR((((uint32_t)timset << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)timset << 11);
}

/**
 * @brief Sets the olbcDSSS field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] olbcdsss - The value to set the field to.
 */
__INLINE void nxmac_ack_olbc_dsss_clearf(uint8_t olbcdsss)
{
    ASSERT_ERR((((uint32_t)olbcdsss << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)olbcdsss << 10);
}

/**
 * @brief Sets the olbcOFDM field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] olbcofdm - The value to set the field to.
 */
__INLINE void nxmac_ack_olbc_ofdm_clearf(uint8_t olbcofdm)
{
    ASSERT_ERR((((uint32_t)olbcofdm << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)olbcofdm << 9);
}

/**
 * @brief Sets the rxFIFOOverFlow field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxfifooverflow - The value to set the field to.
 */
__INLINE void nxmac_ack_rx_fifo_over_flow_clearf(uint8_t rxfifooverflow)
{
    ASSERT_ERR((((uint32_t)rxfifooverflow << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)rxfifooverflow << 8);
}

/**
 * @brief Sets the rxDMAEmpty field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxdmaempty - The value to set the field to.
 */
__INLINE void nxmac_ack_rx_dma_empty_clearf(uint8_t rxdmaempty)
{
    ASSERT_ERR((((uint32_t)rxdmaempty << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)rxdmaempty << 7);
}

/**
 * @brief Sets the macPHYIFOverflow field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macphyifoverflow - The value to set the field to.
 */
__INLINE void nxmac_ack_mac_phyif_overflow_clearf(uint8_t macphyifoverflow)
{
    ASSERT_ERR((((uint32_t)macphyifoverflow << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)macphyifoverflow << 6);
}

/**
 * @brief Sets the coexEvent field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexevent - The value to set the field to.
 */
__INLINE void nxmac_ack_coex_event_clearf(uint8_t coexevent)
{
    ASSERT_ERR((((uint32_t)coexevent << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)coexevent << 5);
}

/**
 * @brief Sets the idleInterrupt field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] idleinterrupt - The value to set the field to.
 */
__INLINE void nxmac_ack_idle_interrupt_clearf(uint8_t idleinterrupt)
{
    ASSERT_ERR((((uint32_t)idleinterrupt << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)idleinterrupt << 2);
}

/**
 * @brief Sets the impSecTBTT field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] impsectbtt - The value to set the field to.
 */
__INLINE void nxmac_ack_imp_sec_tbtt_clearf(uint8_t impsectbtt)
{
    ASSERT_ERR((((uint32_t)impsectbtt << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)impsectbtt << 1);
}

/**
 * @brief Sets the impPriTBTT field of the GEN_INT_ACK register.
 *
 * The GEN_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] imppritbtt - The value to set the field to.
 */
__INLINE void nxmac_ack_imp_pri_tbtt_clearf(uint8_t imppritbtt)
{
    ASSERT_ERR((((uint32_t)imppritbtt << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ACK_ADDR, (uint32_t)imppritbtt << 0);
}

/// @}

/**
 * @name GEN_INT_ENABLE register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31       masterGenIntEn   0
 *     25     rxPayloadDMADead   0
 *     24      rxHeaderDMADead   0
 *     23           phyRxStart   0
 *     22               phyErr   0
 *     21     macPHYIFUnderRun   0
 *     20                hwErr   0
 *     19           impSecDTIM   0
 *     18           impPriDTIM   0
 *     17         bcnTxDMADead   0
 *     16         ac3TxDMADead   0
 *     15         ac2TxDMADead   0
 *     14         ac1TxDMADead   0
 *     13         ac0TxDMADead   0
 *     12              ptError   0
 *     11               timSet   0
 *     10             olbcDSSS   0
 *     09             olbcOFDM   0
 *     08       rxFIFOOverFlow   0
 *     07           rxDMAEmpty   0
 *     06     macPHYIFOverflow   0
 *     05            coexEvent   0
 *     03         absGenTimers   0
 *     02        idleInterrupt   0
 *     01           impSecTBTT   0
 *     00           impPriTBTT   0
 * </pre>
 *
 * @{
 */

/// Address of the GEN_INT_ENABLE register
#define NXMAC_GEN_INT_ENABLE_ADDR   0xC0008074
/// Offset of the GEN_INT_ENABLE register from the base address
#define NXMAC_GEN_INT_ENABLE_OFFSET 0x00000074
/// Index of the GEN_INT_ENABLE register
#define NXMAC_GEN_INT_ENABLE_INDEX  0x0000001D
/// Reset value of the GEN_INT_ENABLE register
#define NXMAC_GEN_INT_ENABLE_RESET  0x00000000

/**
 * @brief Returns the current value of the GEN_INT_ENABLE register.
 * The GEN_INT_ENABLE register will be read and its value returned.
 * @return The current value of the GEN_INT_ENABLE register.
 */
__INLINE uint32_t nxmac_gen_int_enable_get(void)
{
    return REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
}

/**
 * @brief Sets the GEN_INT_ENABLE register to a value.
 * The GEN_INT_ENABLE register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_gen_int_enable_set(uint32_t value)
{
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, value);
}

// field definitions
/// MASTER_GEN_INT_EN field bit
#define NXMAC_MASTER_GEN_INT_EN_BIT      ((uint32_t)0x80000000)
/// MASTER_GEN_INT_EN field position
#define NXMAC_MASTER_GEN_INT_EN_POS      31
/// RX_PAYLOAD_DMA_DEAD field bit
#define NXMAC_RX_PAYLOAD_DMA_DEAD_BIT    ((uint32_t)0x02000000)
/// RX_PAYLOAD_DMA_DEAD field position
#define NXMAC_RX_PAYLOAD_DMA_DEAD_POS    25
/// RX_HEADER_DMA_DEAD field bit
#define NXMAC_RX_HEADER_DMA_DEAD_BIT     ((uint32_t)0x01000000)
/// RX_HEADER_DMA_DEAD field position
#define NXMAC_RX_HEADER_DMA_DEAD_POS     24
/// PHY_RX_START field bit
#define NXMAC_PHY_RX_START_BIT           ((uint32_t)0x00800000)
/// PHY_RX_START field position
#define NXMAC_PHY_RX_START_POS           23
/// PHY_ERR field bit
#define NXMAC_PHY_ERR_BIT                ((uint32_t)0x00400000)
/// PHY_ERR field position
#define NXMAC_PHY_ERR_POS                22
/// MAC_PHYIF_UNDER_RUN field bit
#define NXMAC_MAC_PHYIF_UNDER_RUN_BIT    ((uint32_t)0x00200000)
/// MAC_PHYIF_UNDER_RUN field position
#define NXMAC_MAC_PHYIF_UNDER_RUN_POS    21
/// HW_ERR field bit
#define NXMAC_HW_ERR_BIT                 ((uint32_t)0x00100000)
/// HW_ERR field position
#define NXMAC_HW_ERR_POS                 20
/// IMP_SEC_DTIM field bit
#define NXMAC_IMP_SEC_DTIM_BIT           ((uint32_t)0x00080000)
/// IMP_SEC_DTIM field position
#define NXMAC_IMP_SEC_DTIM_POS           19
/// IMP_PRI_DTIM field bit
#define NXMAC_IMP_PRI_DTIM_BIT           ((uint32_t)0x00040000)
/// IMP_PRI_DTIM field position
#define NXMAC_IMP_PRI_DTIM_POS           18
/// BCN_TX_DMA_DEAD field bit
#define NXMAC_BCN_TX_DMA_DEAD_BIT        ((uint32_t)0x00020000)
/// BCN_TX_DMA_DEAD field position
#define NXMAC_BCN_TX_DMA_DEAD_POS        17
/// AC_3_TX_DMA_DEAD field bit
#define NXMAC_AC_3_TX_DMA_DEAD_BIT       ((uint32_t)0x00010000)
/// AC_3_TX_DMA_DEAD field position
#define NXMAC_AC_3_TX_DMA_DEAD_POS       16
/// AC_2_TX_DMA_DEAD field bit
#define NXMAC_AC_2_TX_DMA_DEAD_BIT       ((uint32_t)0x00008000)
/// AC_2_TX_DMA_DEAD field position
#define NXMAC_AC_2_TX_DMA_DEAD_POS       15
/// AC_1_TX_DMA_DEAD field bit
#define NXMAC_AC_1_TX_DMA_DEAD_BIT       ((uint32_t)0x00004000)
/// AC_1_TX_DMA_DEAD field position
#define NXMAC_AC_1_TX_DMA_DEAD_POS       14
/// AC_0_TX_DMA_DEAD field bit
#define NXMAC_AC_0_TX_DMA_DEAD_BIT       ((uint32_t)0x00002000)
/// AC_0_TX_DMA_DEAD field position
#define NXMAC_AC_0_TX_DMA_DEAD_POS       13
/// PT_ERROR field bit
#define NXMAC_PT_ERROR_BIT               ((uint32_t)0x00001000)
/// PT_ERROR field position
#define NXMAC_PT_ERROR_POS               12
/// TIM_SET field bit
#define NXMAC_TIM_SET_BIT                ((uint32_t)0x00000800)
/// TIM_SET field position
#define NXMAC_TIM_SET_POS                11
/// OLBC_DSSS field bit
#define NXMAC_OLBC_DSSS_BIT              ((uint32_t)0x00000400)
/// OLBC_DSSS field position
#define NXMAC_OLBC_DSSS_POS              10
/// OLBC_OFDM field bit
#define NXMAC_OLBC_OFDM_BIT              ((uint32_t)0x00000200)
/// OLBC_OFDM field position
#define NXMAC_OLBC_OFDM_POS              9
/// RX_FIFO_OVER_FLOW field bit
#define NXMAC_RX_FIFO_OVER_FLOW_BIT      ((uint32_t)0x00000100)
/// RX_FIFO_OVER_FLOW field position
#define NXMAC_RX_FIFO_OVER_FLOW_POS      8
/// RX_DMA_EMPTY field bit
#define NXMAC_RX_DMA_EMPTY_BIT           ((uint32_t)0x00000080)
/// RX_DMA_EMPTY field position
#define NXMAC_RX_DMA_EMPTY_POS           7
/// MAC_PHYIF_OVERFLOW field bit
#define NXMAC_MAC_PHYIF_OVERFLOW_BIT     ((uint32_t)0x00000040)
/// MAC_PHYIF_OVERFLOW field position
#define NXMAC_MAC_PHYIF_OVERFLOW_POS     6
/// COEX_EVENT field bit
#define NXMAC_COEX_EVENT_BIT             ((uint32_t)0x00000020)
/// COEX_EVENT field position
#define NXMAC_COEX_EVENT_POS             5
/// ABS_GEN_TIMERS field bit
#define NXMAC_ABS_GEN_TIMERS_BIT         ((uint32_t)0x00000008)
/// ABS_GEN_TIMERS field position
#define NXMAC_ABS_GEN_TIMERS_POS         3
/// IDLE_INTERRUPT field bit
#define NXMAC_IDLE_INTERRUPT_BIT         ((uint32_t)0x00000004)
/// IDLE_INTERRUPT field position
#define NXMAC_IDLE_INTERRUPT_POS         2
/// IMP_SEC_TBTT field bit
#define NXMAC_IMP_SEC_TBTT_BIT           ((uint32_t)0x00000002)
/// IMP_SEC_TBTT field position
#define NXMAC_IMP_SEC_TBTT_POS           1
/// IMP_PRI_TBTT field bit
#define NXMAC_IMP_PRI_TBTT_BIT           ((uint32_t)0x00000001)
/// IMP_PRI_TBTT field position
#define NXMAC_IMP_PRI_TBTT_POS           0

/// MASTER_GEN_INT_EN field reset value
#define NXMAC_MASTER_GEN_INT_EN_RST      0x0
/// RX_PAYLOAD_DMA_DEAD field reset value
#define NXMAC_RX_PAYLOAD_DMA_DEAD_RST    0x0
/// RX_HEADER_DMA_DEAD field reset value
#define NXMAC_RX_HEADER_DMA_DEAD_RST     0x0
/// PHY_RX_START field reset value
#define NXMAC_PHY_RX_START_RST           0x0
/// PHY_ERR field reset value
#define NXMAC_PHY_ERR_RST                0x0
/// MAC_PHYIF_UNDER_RUN field reset value
#define NXMAC_MAC_PHYIF_UNDER_RUN_RST    0x0
/// HW_ERR field reset value
#define NXMAC_HW_ERR_RST                 0x0
/// IMP_SEC_DTIM field reset value
#define NXMAC_IMP_SEC_DTIM_RST           0x0
/// IMP_PRI_DTIM field reset value
#define NXMAC_IMP_PRI_DTIM_RST           0x0
/// BCN_TX_DMA_DEAD field reset value
#define NXMAC_BCN_TX_DMA_DEAD_RST        0x0
/// AC_3_TX_DMA_DEAD field reset value
#define NXMAC_AC_3_TX_DMA_DEAD_RST       0x0
/// AC_2_TX_DMA_DEAD field reset value
#define NXMAC_AC_2_TX_DMA_DEAD_RST       0x0
/// AC_1_TX_DMA_DEAD field reset value
#define NXMAC_AC_1_TX_DMA_DEAD_RST       0x0
/// AC_0_TX_DMA_DEAD field reset value
#define NXMAC_AC_0_TX_DMA_DEAD_RST       0x0
/// PT_ERROR field reset value
#define NXMAC_PT_ERROR_RST               0x0
/// TIM_SET field reset value
#define NXMAC_TIM_SET_RST                0x0
/// OLBC_DSSS field reset value
#define NXMAC_OLBC_DSSS_RST              0x0
/// OLBC_OFDM field reset value
#define NXMAC_OLBC_OFDM_RST              0x0
/// RX_FIFO_OVER_FLOW field reset value
#define NXMAC_RX_FIFO_OVER_FLOW_RST      0x0
/// RX_DMA_EMPTY field reset value
#define NXMAC_RX_DMA_EMPTY_RST           0x0
/// MAC_PHYIF_OVERFLOW field reset value
#define NXMAC_MAC_PHYIF_OVERFLOW_RST     0x0
/// COEX_EVENT field reset value
#define NXMAC_COEX_EVENT_RST             0x0
/// ABS_GEN_TIMERS field reset value
#define NXMAC_ABS_GEN_TIMERS_RST         0x0
/// IDLE_INTERRUPT field reset value
#define NXMAC_IDLE_INTERRUPT_RST         0x0
/// IMP_SEC_TBTT field reset value
#define NXMAC_IMP_SEC_TBTT_RST           0x0
/// IMP_PRI_TBTT field reset value
#define NXMAC_IMP_PRI_TBTT_RST           0x0

/**
 * @brief Constructs a value for the GEN_INT_ENABLE register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] mastergeninten - The value to use for the masterGenIntEn field.
 * @param[in] rxpayloaddmadead - The value to use for the rxPayloadDMADead field.
 * @param[in] rxheaderdmadead - The value to use for the rxHeaderDMADead field.
 * @param[in] phyrxstart - The value to use for the phyRxStart field.
 * @param[in] phyerr - The value to use for the phyErr field.
 * @param[in] macphyifunderrun - The value to use for the macPHYIFUnderRun field.
 * @param[in] hwerr - The value to use for the hwErr field.
 * @param[in] impsecdtim - The value to use for the impSecDTIM field.
 * @param[in] imppridtim - The value to use for the impPriDTIM field.
 * @param[in] bcntxdmadead - The value to use for the bcnTxDMADead field.
 * @param[in] ac3txdmadead - The value to use for the ac3TxDMADead field.
 * @param[in] ac2txdmadead - The value to use for the ac2TxDMADead field.
 * @param[in] ac1txdmadead - The value to use for the ac1TxDMADead field.
 * @param[in] ac0txdmadead - The value to use for the ac0TxDMADead field.
 * @param[in] pterror - The value to use for the ptError field.
 * @param[in] timset - The value to use for the timSet field.
 * @param[in] olbcdsss - The value to use for the olbcDSSS field.
 * @param[in] olbcofdm - The value to use for the olbcOFDM field.
 * @param[in] rxfifooverflow - The value to use for the rxFIFOOverFlow field.
 * @param[in] rxdmaempty - The value to use for the rxDMAEmpty field.
 * @param[in] macphyifoverflow - The value to use for the macPHYIFOverflow field.
 * @param[in] coexevent - The value to use for the coexEvent field.
 * @param[in] absgentimers - The value to use for the absGenTimers field.
 * @param[in] idleinterrupt - The value to use for the idleInterrupt field.
 * @param[in] impsectbtt - The value to use for the impSecTBTT field.
 * @param[in] imppritbtt - The value to use for the impPriTBTT field.
 */
__INLINE void nxmac_gen_int_enable_pack(uint8_t mastergeninten, uint8_t rxpayloaddmadead, uint8_t rxheaderdmadead, uint8_t phyrxstart, uint8_t phyerr, uint8_t macphyifunderrun, uint8_t hwerr, uint8_t impsecdtim, uint8_t imppridtim, uint8_t bcntxdmadead, uint8_t ac3txdmadead, uint8_t ac2txdmadead, uint8_t ac1txdmadead, uint8_t ac0txdmadead, uint8_t pterror, uint8_t timset, uint8_t olbcdsss, uint8_t olbcofdm, uint8_t rxfifooverflow, uint8_t rxdmaempty, uint8_t macphyifoverflow, uint8_t coexevent, uint8_t absgentimers, uint8_t idleinterrupt, uint8_t impsectbtt, uint8_t imppritbtt)
{
    ASSERT_ERR((((uint32_t)mastergeninten << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)rxpayloaddmadead << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)rxheaderdmadead << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)phyrxstart << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)phyerr << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)macphyifunderrun << 21) & ~((uint32_t)0x00200000)) == 0);
    ASSERT_ERR((((uint32_t)hwerr << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)impsecdtim << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)imppridtim << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)bcntxdmadead << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)ac3txdmadead << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)ac2txdmadead << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)ac1txdmadead << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)ac0txdmadead << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)pterror << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)timset << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)olbcdsss << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)olbcofdm << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)rxfifooverflow << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)rxdmaempty << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)macphyifoverflow << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)coexevent << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)absgentimers << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)idleinterrupt << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)impsectbtt << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)imppritbtt << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR,  ((uint32_t)mastergeninten << 31) | ((uint32_t)rxpayloaddmadead << 25) | ((uint32_t)rxheaderdmadead << 24) | ((uint32_t)phyrxstart << 23) | ((uint32_t)phyerr << 22) | ((uint32_t)macphyifunderrun << 21) | ((uint32_t)hwerr << 20) | ((uint32_t)impsecdtim << 19) | ((uint32_t)imppridtim << 18) | ((uint32_t)bcntxdmadead << 17) | ((uint32_t)ac3txdmadead << 16) | ((uint32_t)ac2txdmadead << 15) | ((uint32_t)ac1txdmadead << 14) | ((uint32_t)ac0txdmadead << 13) | ((uint32_t)pterror << 12) | ((uint32_t)timset << 11) | ((uint32_t)olbcdsss << 10) | ((uint32_t)olbcofdm << 9) | ((uint32_t)rxfifooverflow << 8) | ((uint32_t)rxdmaempty << 7) | ((uint32_t)macphyifoverflow << 6) | ((uint32_t)coexevent << 5) | ((uint32_t)absgentimers << 3) | ((uint32_t)idleinterrupt << 2) | ((uint32_t)impsectbtt << 1) | ((uint32_t)imppritbtt << 0));
}

/**
 * @brief Unpacks GEN_INT_ENABLE's fields from current value of the GEN_INT_ENABLE register.
 *
 * Reads the GEN_INT_ENABLE register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] mastergeninten - Will be populated with the current value of this field from the register.
 * @param[out] rxpayloaddmadead - Will be populated with the current value of this field from the register.
 * @param[out] rxheaderdmadead - Will be populated with the current value of this field from the register.
 * @param[out] phyrxstart - Will be populated with the current value of this field from the register.
 * @param[out] phyerr - Will be populated with the current value of this field from the register.
 * @param[out] macphyifunderrun - Will be populated with the current value of this field from the register.
 * @param[out] hwerr - Will be populated with the current value of this field from the register.
 * @param[out] impsecdtim - Will be populated with the current value of this field from the register.
 * @param[out] imppridtim - Will be populated with the current value of this field from the register.
 * @param[out] bcntxdmadead - Will be populated with the current value of this field from the register.
 * @param[out] ac3txdmadead - Will be populated with the current value of this field from the register.
 * @param[out] ac2txdmadead - Will be populated with the current value of this field from the register.
 * @param[out] ac1txdmadead - Will be populated with the current value of this field from the register.
 * @param[out] ac0txdmadead - Will be populated with the current value of this field from the register.
 * @param[out] pterror - Will be populated with the current value of this field from the register.
 * @param[out] timset - Will be populated with the current value of this field from the register.
 * @param[out] olbcdsss - Will be populated with the current value of this field from the register.
 * @param[out] olbcofdm - Will be populated with the current value of this field from the register.
 * @param[out] rxfifooverflow - Will be populated with the current value of this field from the register.
 * @param[out] rxdmaempty - Will be populated with the current value of this field from the register.
 * @param[out] macphyifoverflow - Will be populated with the current value of this field from the register.
 * @param[out] coexevent - Will be populated with the current value of this field from the register.
 * @param[out] absgentimers - Will be populated with the current value of this field from the register.
 * @param[out] idleinterrupt - Will be populated with the current value of this field from the register.
 * @param[out] impsectbtt - Will be populated with the current value of this field from the register.
 * @param[out] imppritbtt - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_gen_int_enable_unpack(uint8_t *mastergeninten, uint8_t *rxpayloaddmadead, uint8_t *rxheaderdmadead, uint8_t *phyrxstart, uint8_t *phyerr, uint8_t *macphyifunderrun, uint8_t *hwerr, uint8_t *impsecdtim, uint8_t *imppridtim, uint8_t *bcntxdmadead, uint8_t *ac3txdmadead, uint8_t *ac2txdmadead, uint8_t *ac1txdmadead, uint8_t *ac0txdmadead, uint8_t *pterror, uint8_t *timset, uint8_t *olbcdsss, uint8_t *olbcofdm, uint8_t *rxfifooverflow, uint8_t *rxdmaempty, uint8_t *macphyifoverflow, uint8_t *coexevent, uint8_t *absgentimers, uint8_t *idleinterrupt, uint8_t *impsectbtt, uint8_t *imppritbtt)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);

    *mastergeninten = (localVal & ((uint32_t)0x80000000)) >> 31;
    *rxpayloaddmadead = (localVal & ((uint32_t)0x02000000)) >> 25;
    *rxheaderdmadead = (localVal & ((uint32_t)0x01000000)) >> 24;
    *phyrxstart = (localVal & ((uint32_t)0x00800000)) >> 23;
    *phyerr = (localVal & ((uint32_t)0x00400000)) >> 22;
    *macphyifunderrun = (localVal & ((uint32_t)0x00200000)) >> 21;
    *hwerr = (localVal & ((uint32_t)0x00100000)) >> 20;
    *impsecdtim = (localVal & ((uint32_t)0x00080000)) >> 19;
    *imppridtim = (localVal & ((uint32_t)0x00040000)) >> 18;
    *bcntxdmadead = (localVal & ((uint32_t)0x00020000)) >> 17;
    *ac3txdmadead = (localVal & ((uint32_t)0x00010000)) >> 16;
    *ac2txdmadead = (localVal & ((uint32_t)0x00008000)) >> 15;
    *ac1txdmadead = (localVal & ((uint32_t)0x00004000)) >> 14;
    *ac0txdmadead = (localVal & ((uint32_t)0x00002000)) >> 13;
    *pterror = (localVal & ((uint32_t)0x00001000)) >> 12;
    *timset = (localVal & ((uint32_t)0x00000800)) >> 11;
    *olbcdsss = (localVal & ((uint32_t)0x00000400)) >> 10;
    *olbcofdm = (localVal & ((uint32_t)0x00000200)) >> 9;
    *rxfifooverflow = (localVal & ((uint32_t)0x00000100)) >> 8;
    *rxdmaempty = (localVal & ((uint32_t)0x00000080)) >> 7;
    *macphyifoverflow = (localVal & ((uint32_t)0x00000040)) >> 6;
    *coexevent = (localVal & ((uint32_t)0x00000020)) >> 5;
    *absgentimers = (localVal & ((uint32_t)0x00000008)) >> 3;
    *idleinterrupt = (localVal & ((uint32_t)0x00000004)) >> 2;
    *impsectbtt = (localVal & ((uint32_t)0x00000002)) >> 1;
    *imppritbtt = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the masterGenIntEn field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the masterGenIntEn field's value will be returned.
 *
 * @return The current value of the masterGenIntEn field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_master_gen_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the masterGenIntEn field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mastergeninten - The value to set the field to.
 */
__INLINE void nxmac_enable_master_gen_int_en_setf(uint8_t mastergeninten)
{
    ASSERT_ERR((((uint32_t)mastergeninten << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)mastergeninten << 31));
}

/**
 * @brief Returns the current value of the rxPayloadDMADead field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the rxPayloadDMADead field's value will be returned.
 *
 * @return The current value of the rxPayloadDMADead field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_rx_payload_dma_dead_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

/**
 * @brief Sets the rxPayloadDMADead field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxpayloaddmadead - The value to set the field to.
 */
__INLINE void nxmac_enable_rx_payload_dma_dead_setf(uint8_t rxpayloaddmadead)
{
    ASSERT_ERR((((uint32_t)rxpayloaddmadead << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x02000000)) | ((uint32_t)rxpayloaddmadead << 25));
}

/**
 * @brief Returns the current value of the rxHeaderDMADead field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the rxHeaderDMADead field's value will be returned.
 *
 * @return The current value of the rxHeaderDMADead field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_rx_header_dma_dead_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the rxHeaderDMADead field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxheaderdmadead - The value to set the field to.
 */
__INLINE void nxmac_enable_rx_header_dma_dead_setf(uint8_t rxheaderdmadead)
{
    ASSERT_ERR((((uint32_t)rxheaderdmadead << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)rxheaderdmadead << 24));
}

/**
 * @brief Returns the current value of the phyRxStart field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the phyRxStart field's value will be returned.
 *
 * @return The current value of the phyRxStart field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_phy_rx_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Sets the phyRxStart field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] phyrxstart - The value to set the field to.
 */
__INLINE void nxmac_enable_phy_rx_start_setf(uint8_t phyrxstart)
{
    ASSERT_ERR((((uint32_t)phyrxstart << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00800000)) | ((uint32_t)phyrxstart << 23));
}

/**
 * @brief Returns the current value of the phyErr field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the phyErr field's value will be returned.
 *
 * @return The current value of the phyErr field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_phy_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Sets the phyErr field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] phyerr - The value to set the field to.
 */
__INLINE void nxmac_enable_phy_err_setf(uint8_t phyerr)
{
    ASSERT_ERR((((uint32_t)phyerr << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00400000)) | ((uint32_t)phyerr << 22));
}

/**
 * @brief Returns the current value of the macPHYIFUnderRun field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the macPHYIFUnderRun field's value will be returned.
 *
 * @return The current value of the macPHYIFUnderRun field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_mac_phyif_under_run_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Sets the macPHYIFUnderRun field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macphyifunderrun - The value to set the field to.
 */
__INLINE void nxmac_enable_mac_phyif_under_run_setf(uint8_t macphyifunderrun)
{
    ASSERT_ERR((((uint32_t)macphyifunderrun << 21) & ~((uint32_t)0x00200000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00200000)) | ((uint32_t)macphyifunderrun << 21));
}

/**
 * @brief Returns the current value of the hwErr field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the hwErr field's value will be returned.
 *
 * @return The current value of the hwErr field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_hw_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Sets the hwErr field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] hwerr - The value to set the field to.
 */
__INLINE void nxmac_enable_hw_err_setf(uint8_t hwerr)
{
    ASSERT_ERR((((uint32_t)hwerr << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00100000)) | ((uint32_t)hwerr << 20));
}

/**
 * @brief Returns the current value of the impSecDTIM field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the impSecDTIM field's value will be returned.
 *
 * @return The current value of the impSecDTIM field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_imp_sec_dtim_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Sets the impSecDTIM field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] impsecdtim - The value to set the field to.
 */
__INLINE void nxmac_enable_imp_sec_dtim_setf(uint8_t impsecdtim)
{
    ASSERT_ERR((((uint32_t)impsecdtim << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00080000)) | ((uint32_t)impsecdtim << 19));
}

/**
 * @brief Returns the current value of the impPriDTIM field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the impPriDTIM field's value will be returned.
 *
 * @return The current value of the impPriDTIM field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_imp_pri_dtim_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Sets the impPriDTIM field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] imppridtim - The value to set the field to.
 */
__INLINE void nxmac_enable_imp_pri_dtim_setf(uint8_t imppridtim)
{
    ASSERT_ERR((((uint32_t)imppridtim << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00040000)) | ((uint32_t)imppridtim << 18));
}

/**
 * @brief Returns the current value of the bcnTxDMADead field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the bcnTxDMADead field's value will be returned.
 *
 * @return The current value of the bcnTxDMADead field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_bcn_tx_dma_dead_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Sets the bcnTxDMADead field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bcntxdmadead - The value to set the field to.
 */
__INLINE void nxmac_enable_bcn_tx_dma_dead_setf(uint8_t bcntxdmadead)
{
    ASSERT_ERR((((uint32_t)bcntxdmadead << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00020000)) | ((uint32_t)bcntxdmadead << 17));
}

/**
 * @brief Returns the current value of the ac3TxDMADead field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the ac3TxDMADead field's value will be returned.
 *
 * @return The current value of the ac3TxDMADead field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_3_tx_dma_dead_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the ac3TxDMADead field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac3txdmadead - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_3_tx_dma_dead_setf(uint8_t ac3txdmadead)
{
    ASSERT_ERR((((uint32_t)ac3txdmadead << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)ac3txdmadead << 16));
}

/**
 * @brief Returns the current value of the ac2TxDMADead field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the ac2TxDMADead field's value will be returned.
 *
 * @return The current value of the ac2TxDMADead field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_2_tx_dma_dead_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

/**
 * @brief Sets the ac2TxDMADead field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac2txdmadead - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_2_tx_dma_dead_setf(uint8_t ac2txdmadead)
{
    ASSERT_ERR((((uint32_t)ac2txdmadead << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00008000)) | ((uint32_t)ac2txdmadead << 15));
}

/**
 * @brief Returns the current value of the ac1TxDMADead field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the ac1TxDMADead field's value will be returned.
 *
 * @return The current value of the ac1TxDMADead field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_1_tx_dma_dead_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Sets the ac1TxDMADead field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac1txdmadead - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_1_tx_dma_dead_setf(uint8_t ac1txdmadead)
{
    ASSERT_ERR((((uint32_t)ac1txdmadead << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00004000)) | ((uint32_t)ac1txdmadead << 14));
}

/**
 * @brief Returns the current value of the ac0TxDMADead field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the ac0TxDMADead field's value will be returned.
 *
 * @return The current value of the ac0TxDMADead field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_0_tx_dma_dead_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the ac0TxDMADead field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac0txdmadead - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_0_tx_dma_dead_setf(uint8_t ac0txdmadead)
{
    ASSERT_ERR((((uint32_t)ac0txdmadead << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)ac0txdmadead << 13));
}

/**
 * @brief Returns the current value of the ptError field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the ptError field's value will be returned.
 *
 * @return The current value of the ptError field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_pt_error_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the ptError field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pterror - The value to set the field to.
 */
__INLINE void nxmac_enable_pt_error_setf(uint8_t pterror)
{
    ASSERT_ERR((((uint32_t)pterror << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)pterror << 12));
}

/**
 * @brief Returns the current value of the timSet field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the timSet field's value will be returned.
 *
 * @return The current value of the timSet field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_tim_set_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the timSet field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] timset - The value to set the field to.
 */
__INLINE void nxmac_enable_tim_set_setf(uint8_t timset)
{
    ASSERT_ERR((((uint32_t)timset << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00000800)) | ((uint32_t)timset << 11));
}

/**
 * @brief Returns the current value of the olbcDSSS field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the olbcDSSS field's value will be returned.
 *
 * @return The current value of the olbcDSSS field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_olbc_dsss_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the olbcDSSS field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] olbcdsss - The value to set the field to.
 */
__INLINE void nxmac_enable_olbc_dsss_setf(uint8_t olbcdsss)
{
    ASSERT_ERR((((uint32_t)olbcdsss << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00000400)) | ((uint32_t)olbcdsss << 10));
}

/**
 * @brief Returns the current value of the olbcOFDM field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the olbcOFDM field's value will be returned.
 *
 * @return The current value of the olbcOFDM field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_olbc_ofdm_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the olbcOFDM field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] olbcofdm - The value to set the field to.
 */
__INLINE void nxmac_enable_olbc_ofdm_setf(uint8_t olbcofdm)
{
    ASSERT_ERR((((uint32_t)olbcofdm << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)olbcofdm << 9));
}

/**
 * @brief Returns the current value of the rxFIFOOverFlow field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the rxFIFOOverFlow field's value will be returned.
 *
 * @return The current value of the rxFIFOOverFlow field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_rx_fifo_over_flow_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the rxFIFOOverFlow field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxfifooverflow - The value to set the field to.
 */
__INLINE void nxmac_enable_rx_fifo_over_flow_setf(uint8_t rxfifooverflow)
{
    ASSERT_ERR((((uint32_t)rxfifooverflow << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)rxfifooverflow << 8));
}

/**
 * @brief Returns the current value of the rxDMAEmpty field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the rxDMAEmpty field's value will be returned.
 *
 * @return The current value of the rxDMAEmpty field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_rx_dma_empty_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the rxDMAEmpty field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxdmaempty - The value to set the field to.
 */
__INLINE void nxmac_enable_rx_dma_empty_setf(uint8_t rxdmaempty)
{
    ASSERT_ERR((((uint32_t)rxdmaempty << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)rxdmaempty << 7));
}

/**
 * @brief Returns the current value of the macPHYIFOverflow field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the macPHYIFOverflow field's value will be returned.
 *
 * @return The current value of the macPHYIFOverflow field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_mac_phyif_overflow_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the macPHYIFOverflow field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macphyifoverflow - The value to set the field to.
 */
__INLINE void nxmac_enable_mac_phyif_overflow_setf(uint8_t macphyifoverflow)
{
    ASSERT_ERR((((uint32_t)macphyifoverflow << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)macphyifoverflow << 6));
}

/**
 * @brief Returns the current value of the coexEvent field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the coexEvent field's value will be returned.
 *
 * @return The current value of the coexEvent field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_coex_event_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the coexEvent field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexevent - The value to set the field to.
 */
__INLINE void nxmac_enable_coex_event_setf(uint8_t coexevent)
{
    ASSERT_ERR((((uint32_t)coexevent << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)coexevent << 5));
}

/**
 * @brief Returns the current value of the absGenTimers field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the absGenTimers field's value will be returned.
 *
 * @return The current value of the absGenTimers field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_abs_gen_timers_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the absGenTimers field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] absgentimers - The value to set the field to.
 */
__INLINE void nxmac_enable_abs_gen_timers_setf(uint8_t absgentimers)
{
    ASSERT_ERR((((uint32_t)absgentimers << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)absgentimers << 3));
}

/**
 * @brief Returns the current value of the idleInterrupt field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the idleInterrupt field's value will be returned.
 *
 * @return The current value of the idleInterrupt field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_idle_interrupt_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the idleInterrupt field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] idleinterrupt - The value to set the field to.
 */
__INLINE void nxmac_enable_idle_interrupt_setf(uint8_t idleinterrupt)
{
    ASSERT_ERR((((uint32_t)idleinterrupt << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)idleinterrupt << 2));
}

/**
 * @brief Returns the current value of the impSecTBTT field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the impSecTBTT field's value will be returned.
 *
 * @return The current value of the impSecTBTT field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_imp_sec_tbtt_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the impSecTBTT field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] impsectbtt - The value to set the field to.
 */
__INLINE void nxmac_enable_imp_sec_tbtt_setf(uint8_t impsectbtt)
{
    ASSERT_ERR((((uint32_t)impsectbtt << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)impsectbtt << 1));
}

/**
 * @brief Returns the current value of the impPriTBTT field in the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read and the impPriTBTT field's value will be returned.
 *
 * @return The current value of the impPriTBTT field in the GEN_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_imp_pri_tbtt_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the impPriTBTT field of the GEN_INT_ENABLE register.
 *
 * The GEN_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] imppritbtt - The value to set the field to.
 */
__INLINE void nxmac_enable_imp_pri_tbtt_setf(uint8_t imppritbtt)
{
    ASSERT_ERR((((uint32_t)imppritbtt << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_GEN_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_GEN_INT_ENABLE_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)imppritbtt << 0));
}

/// @}

/**
 * @name TX_RX_INT_STATUS register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     28      bcnTxBufTrigger   0
 *     27      ac3TxBufTrigger   0
 *     26      ac2TxBufTrigger   0
 *     25      ac1TxBufTrigger   0
 *     24      ac0TxBufTrigger   0
 *     23     ac3BWDropTrigger   0
 *     22     ac2BWDropTrigger   0
 *     21     ac1BWDropTrigger   0
 *     20     ac0BWDropTrigger   0
 *     19     counterRxTrigger   0
 *     18          baRxTrigger   0
 *     17       timerRxTrigger   0
 *     16            rxTrigger   0
 *     15     secUserTxTrigger   0
 *     14       timerTxTrigger   0
 *     13         txopComplete   0
 *     12          rdTxTrigger   0
 *     11        hccaTxTrigger   0
 *     10         bcnTxTrigger   0
 *     09         ac3TxTrigger   0
 *     08         ac2TxTrigger   0
 *     07         ac1TxTrigger   0
 *     06         ac0TxTrigger   0
 *     05        rdProtTrigger   0
 *     04      hccaProtTrigger   0
 *     03       ac3ProtTrigger   0
 *     02       ac2ProtTrigger   0
 *     01       ac1ProtTrigger   0
 *     00       ac0ProtTrigger   0
 * </pre>
 *
 * @{
 */

/// Address of the TX_RX_INT_STATUS register
#define NXMAC_TX_RX_INT_STATUS_ADDR   0xC0008078
/// Offset of the TX_RX_INT_STATUS register from the base address
#define NXMAC_TX_RX_INT_STATUS_OFFSET 0x00000078
/// Index of the TX_RX_INT_STATUS register
#define NXMAC_TX_RX_INT_STATUS_INDEX  0x0000001E
/// Reset value of the TX_RX_INT_STATUS register
#define NXMAC_TX_RX_INT_STATUS_RESET  0x00000000

/**
 * @brief Returns the current value of the TX_RX_INT_STATUS register.
 * The TX_RX_INT_STATUS register will be read and its value returned.
 * @return The current value of the TX_RX_INT_STATUS register.
 */
__INLINE uint32_t nxmac_tx_rx_int_status_get(void)
{
    return REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
}

/**
 * @brief Sets the TX_RX_INT_STATUS register to a value.
 * The TX_RX_INT_STATUS register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_tx_rx_int_status_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, value);
}

// fields defined in symmetrical set/clear register
/**
 * @brief Constructs a value for the TX_RX_INT_STATUS register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] bcntxbuftrigger - The value to use for the bcnTxBufTrigger field.
 * @param[in] ac3txbuftrigger - The value to use for the ac3TxBufTrigger field.
 * @param[in] ac2txbuftrigger - The value to use for the ac2TxBufTrigger field.
 * @param[in] ac1txbuftrigger - The value to use for the ac1TxBufTrigger field.
 * @param[in] ac0txbuftrigger - The value to use for the ac0TxBufTrigger field.
 * @param[in] ac3bwdroptrigger - The value to use for the ac3BWDropTrigger field.
 * @param[in] ac2bwdroptrigger - The value to use for the ac2BWDropTrigger field.
 * @param[in] ac1bwdroptrigger - The value to use for the ac1BWDropTrigger field.
 * @param[in] ac0bwdroptrigger - The value to use for the ac0BWDropTrigger field.
 * @param[in] counterrxtrigger - The value to use for the counterRxTrigger field.
 * @param[in] barxtrigger - The value to use for the baRxTrigger field.
 * @param[in] timerrxtrigger - The value to use for the timerRxTrigger field.
 * @param[in] rxtrigger - The value to use for the rxTrigger field.
 * @param[in] timertxtrigger - The value to use for the timerTxTrigger field.
 * @param[in] txopcomplete - The value to use for the txopComplete field.
 * @param[in] rdtxtrigger - The value to use for the rdTxTrigger field.
 * @param[in] hccatxtrigger - The value to use for the hccaTxTrigger field.
 * @param[in] bcntxtrigger - The value to use for the bcnTxTrigger field.
 * @param[in] ac3txtrigger - The value to use for the ac3TxTrigger field.
 * @param[in] ac2txtrigger - The value to use for the ac2TxTrigger field.
 * @param[in] ac1txtrigger - The value to use for the ac1TxTrigger field.
 * @param[in] ac0txtrigger - The value to use for the ac0TxTrigger field.
 * @param[in] rdprottrigger - The value to use for the rdProtTrigger field.
 * @param[in] hccaprottrigger - The value to use for the hccaProtTrigger field.
 * @param[in] ac3prottrigger - The value to use for the ac3ProtTrigger field.
 * @param[in] ac2prottrigger - The value to use for the ac2ProtTrigger field.
 * @param[in] ac1prottrigger - The value to use for the ac1ProtTrigger field.
 * @param[in] ac0prottrigger - The value to use for the ac0ProtTrigger field.
 */
__INLINE void nxmac_tx_rx_int_status_pack(uint8_t bcntxbuftrigger, uint8_t ac3txbuftrigger, uint8_t ac2txbuftrigger, uint8_t ac1txbuftrigger, uint8_t ac0txbuftrigger, uint8_t ac3bwdroptrigger, uint8_t ac2bwdroptrigger, uint8_t ac1bwdroptrigger, uint8_t ac0bwdroptrigger, uint8_t counterrxtrigger, uint8_t barxtrigger, uint8_t timerrxtrigger, uint8_t rxtrigger, uint8_t timertxtrigger, uint8_t txopcomplete, uint8_t rdtxtrigger, uint8_t hccatxtrigger, uint8_t bcntxtrigger, uint8_t ac3txtrigger, uint8_t ac2txtrigger, uint8_t ac1txtrigger, uint8_t ac0txtrigger, uint8_t rdprottrigger, uint8_t hccaprottrigger, uint8_t ac3prottrigger, uint8_t ac2prottrigger, uint8_t ac1prottrigger, uint8_t ac0prottrigger)
{
    ASSERT_ERR((((uint32_t)bcntxbuftrigger << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)ac3txbuftrigger << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)ac2txbuftrigger << 26) & ~((uint32_t)0x04000000)) == 0);
    ASSERT_ERR((((uint32_t)ac1txbuftrigger << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)ac0txbuftrigger << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)ac3bwdroptrigger << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)ac2bwdroptrigger << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)ac1bwdroptrigger << 21) & ~((uint32_t)0x00200000)) == 0);
    ASSERT_ERR((((uint32_t)ac0bwdroptrigger << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)counterrxtrigger << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)barxtrigger << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)timerrxtrigger << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)rxtrigger << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)timertxtrigger << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)txopcomplete << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)rdtxtrigger << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)hccatxtrigger << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)bcntxtrigger << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)ac3txtrigger << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)ac2txtrigger << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)ac1txtrigger << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)ac0txtrigger << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)rdprottrigger << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)hccaprottrigger << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)ac3prottrigger << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)ac2prottrigger << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)ac1prottrigger << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)ac0prottrigger << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR,  ((uint32_t)bcntxbuftrigger << 28) | ((uint32_t)ac3txbuftrigger << 27) | ((uint32_t)ac2txbuftrigger << 26) | ((uint32_t)ac1txbuftrigger << 25) | ((uint32_t)ac0txbuftrigger << 24) | ((uint32_t)ac3bwdroptrigger << 23) | ((uint32_t)ac2bwdroptrigger << 22) | ((uint32_t)ac1bwdroptrigger << 21) | ((uint32_t)ac0bwdroptrigger << 20) | ((uint32_t)counterrxtrigger << 19) | ((uint32_t)barxtrigger << 18) | ((uint32_t)timerrxtrigger << 17) | ((uint32_t)rxtrigger << 16) | ((uint32_t)timertxtrigger << 14) | ((uint32_t)txopcomplete << 13) | ((uint32_t)rdtxtrigger << 12) | ((uint32_t)hccatxtrigger << 11) | ((uint32_t)bcntxtrigger << 10) | ((uint32_t)ac3txtrigger << 9) | ((uint32_t)ac2txtrigger << 8) | ((uint32_t)ac1txtrigger << 7) | ((uint32_t)ac0txtrigger << 6) | ((uint32_t)rdprottrigger << 5) | ((uint32_t)hccaprottrigger << 4) | ((uint32_t)ac3prottrigger << 3) | ((uint32_t)ac2prottrigger << 2) | ((uint32_t)ac1prottrigger << 1) | ((uint32_t)ac0prottrigger << 0));
}

/**
 * @brief Unpacks TX_RX_INT_STATUS's fields from current value of the TX_RX_INT_STATUS register.
 *
 * Reads the TX_RX_INT_STATUS register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] bcntxbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac3txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac2txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac1txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac0txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac3bwdroptrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac2bwdroptrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac1bwdroptrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac0bwdroptrigger - Will be populated with the current value of this field from the register.
 * @param[out] counterrxtrigger - Will be populated with the current value of this field from the register.
 * @param[out] barxtrigger - Will be populated with the current value of this field from the register.
 * @param[out] timerrxtrigger - Will be populated with the current value of this field from the register.
 * @param[out] rxtrigger - Will be populated with the current value of this field from the register.
 * @param[out] secusertxtrigger - Will be populated with the current value of this field from the register.
 * @param[out] timertxtrigger - Will be populated with the current value of this field from the register.
 * @param[out] txopcomplete - Will be populated with the current value of this field from the register.
 * @param[out] rdtxtrigger - Will be populated with the current value of this field from the register.
 * @param[out] hccatxtrigger - Will be populated with the current value of this field from the register.
 * @param[out] bcntxtrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac3txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac2txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac1txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac0txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] rdprottrigger - Will be populated with the current value of this field from the register.
 * @param[out] hccaprottrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac3prottrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac2prottrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac1prottrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac0prottrigger - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_tx_rx_int_status_unpack(uint8_t *bcntxbuftrigger, uint8_t *ac3txbuftrigger, uint8_t *ac2txbuftrigger, uint8_t *ac1txbuftrigger, uint8_t *ac0txbuftrigger, uint8_t *ac3bwdroptrigger, uint8_t *ac2bwdroptrigger, uint8_t *ac1bwdroptrigger, uint8_t *ac0bwdroptrigger, uint8_t *counterrxtrigger, uint8_t *barxtrigger, uint8_t *timerrxtrigger, uint8_t *rxtrigger, uint8_t *secusertxtrigger, uint8_t *timertxtrigger, uint8_t *txopcomplete, uint8_t *rdtxtrigger, uint8_t *hccatxtrigger, uint8_t *bcntxtrigger, uint8_t *ac3txtrigger, uint8_t *ac2txtrigger, uint8_t *ac1txtrigger, uint8_t *ac0txtrigger, uint8_t *rdprottrigger, uint8_t *hccaprottrigger, uint8_t *ac3prottrigger, uint8_t *ac2prottrigger, uint8_t *ac1prottrigger, uint8_t *ac0prottrigger)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);

    *bcntxbuftrigger = (localVal & ((uint32_t)0x10000000)) >> 28;
    *ac3txbuftrigger = (localVal & ((uint32_t)0x08000000)) >> 27;
    *ac2txbuftrigger = (localVal & ((uint32_t)0x04000000)) >> 26;
    *ac1txbuftrigger = (localVal & ((uint32_t)0x02000000)) >> 25;
    *ac0txbuftrigger = (localVal & ((uint32_t)0x01000000)) >> 24;
    *ac3bwdroptrigger = (localVal & ((uint32_t)0x00800000)) >> 23;
    *ac2bwdroptrigger = (localVal & ((uint32_t)0x00400000)) >> 22;
    *ac1bwdroptrigger = (localVal & ((uint32_t)0x00200000)) >> 21;
    *ac0bwdroptrigger = (localVal & ((uint32_t)0x00100000)) >> 20;
    *counterrxtrigger = (localVal & ((uint32_t)0x00080000)) >> 19;
    *barxtrigger = (localVal & ((uint32_t)0x00040000)) >> 18;
    *timerrxtrigger = (localVal & ((uint32_t)0x00020000)) >> 17;
    *rxtrigger = (localVal & ((uint32_t)0x00010000)) >> 16;
    *secusertxtrigger = (localVal & ((uint32_t)0x00008000)) >> 15;
    *timertxtrigger = (localVal & ((uint32_t)0x00004000)) >> 14;
    *txopcomplete = (localVal & ((uint32_t)0x00002000)) >> 13;
    *rdtxtrigger = (localVal & ((uint32_t)0x00001000)) >> 12;
    *hccatxtrigger = (localVal & ((uint32_t)0x00000800)) >> 11;
    *bcntxtrigger = (localVal & ((uint32_t)0x00000400)) >> 10;
    *ac3txtrigger = (localVal & ((uint32_t)0x00000200)) >> 9;
    *ac2txtrigger = (localVal & ((uint32_t)0x00000100)) >> 8;
    *ac1txtrigger = (localVal & ((uint32_t)0x00000080)) >> 7;
    *ac0txtrigger = (localVal & ((uint32_t)0x00000040)) >> 6;
    *rdprottrigger = (localVal & ((uint32_t)0x00000020)) >> 5;
    *hccaprottrigger = (localVal & ((uint32_t)0x00000010)) >> 4;
    *ac3prottrigger = (localVal & ((uint32_t)0x00000008)) >> 3;
    *ac2prottrigger = (localVal & ((uint32_t)0x00000004)) >> 2;
    *ac1prottrigger = (localVal & ((uint32_t)0x00000002)) >> 1;
    *ac0prottrigger = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the bcnTxBufTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the bcnTxBufTrigger field's value will be returned.
 *
 * @return The current value of the bcnTxBufTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_bcn_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the bcnTxBufTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bcntxbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_status_bcn_tx_buf_trigger_setf(uint8_t bcntxbuftrigger)
{
    ASSERT_ERR((((uint32_t)bcntxbuftrigger << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)bcntxbuftrigger << 28);
}

/**
 * @brief Returns the current value of the ac3TxBufTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the ac3TxBufTrigger field's value will be returned.
 *
 * @return The current value of the ac3TxBufTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_3_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Sets the ac3TxBufTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac3txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_status_ac_3_tx_buf_trigger_setf(uint8_t ac3txbuftrigger)
{
    ASSERT_ERR((((uint32_t)ac3txbuftrigger << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)ac3txbuftrigger << 27);
}

/**
 * @brief Returns the current value of the ac2TxBufTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the ac2TxBufTrigger field's value will be returned.
 *
 * @return The current value of the ac2TxBufTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_2_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

/**
 * @brief Sets the ac2TxBufTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac2txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_status_ac_2_tx_buf_trigger_setf(uint8_t ac2txbuftrigger)
{
    ASSERT_ERR((((uint32_t)ac2txbuftrigger << 26) & ~((uint32_t)0x04000000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)ac2txbuftrigger << 26);
}

/**
 * @brief Returns the current value of the ac1TxBufTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the ac1TxBufTrigger field's value will be returned.
 *
 * @return The current value of the ac1TxBufTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_1_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

/**
 * @brief Sets the ac1TxBufTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac1txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_status_ac_1_tx_buf_trigger_setf(uint8_t ac1txbuftrigger)
{
    ASSERT_ERR((((uint32_t)ac1txbuftrigger << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)ac1txbuftrigger << 25);
}

/**
 * @brief Returns the current value of the ac0TxBufTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the ac0TxBufTrigger field's value will be returned.
 *
 * @return The current value of the ac0TxBufTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_0_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the ac0TxBufTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac0txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_status_ac_0_tx_buf_trigger_setf(uint8_t ac0txbuftrigger)
{
    ASSERT_ERR((((uint32_t)ac0txbuftrigger << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)ac0txbuftrigger << 24);
}

/**
 * @brief Returns the current value of the ac3BWDropTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the ac3BWDropTrigger field's value will be returned.
 *
 * @return The current value of the ac3BWDropTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_3bw_drop_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Sets the ac3BWDropTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac3bwdroptrigger - The value to set the field to.
 */
__INLINE void nxmac_status_ac_3bw_drop_trigger_setf(uint8_t ac3bwdroptrigger)
{
    ASSERT_ERR((((uint32_t)ac3bwdroptrigger << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)ac3bwdroptrigger << 23);
}

/**
 * @brief Returns the current value of the ac2BWDropTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the ac2BWDropTrigger field's value will be returned.
 *
 * @return The current value of the ac2BWDropTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_2bw_drop_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Sets the ac2BWDropTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac2bwdroptrigger - The value to set the field to.
 */
__INLINE void nxmac_status_ac_2bw_drop_trigger_setf(uint8_t ac2bwdroptrigger)
{
    ASSERT_ERR((((uint32_t)ac2bwdroptrigger << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)ac2bwdroptrigger << 22);
}

/**
 * @brief Returns the current value of the ac1BWDropTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the ac1BWDropTrigger field's value will be returned.
 *
 * @return The current value of the ac1BWDropTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_1bw_drop_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Sets the ac1BWDropTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac1bwdroptrigger - The value to set the field to.
 */
__INLINE void nxmac_status_ac_1bw_drop_trigger_setf(uint8_t ac1bwdroptrigger)
{
    ASSERT_ERR((((uint32_t)ac1bwdroptrigger << 21) & ~((uint32_t)0x00200000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)ac1bwdroptrigger << 21);
}

/**
 * @brief Returns the current value of the ac0BWDropTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the ac0BWDropTrigger field's value will be returned.
 *
 * @return The current value of the ac0BWDropTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_0bw_drop_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Sets the ac0BWDropTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac0bwdroptrigger - The value to set the field to.
 */
__INLINE void nxmac_status_ac_0bw_drop_trigger_setf(uint8_t ac0bwdroptrigger)
{
    ASSERT_ERR((((uint32_t)ac0bwdroptrigger << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)ac0bwdroptrigger << 20);
}

/**
 * @brief Returns the current value of the counterRxTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the counterRxTrigger field's value will be returned.
 *
 * @return The current value of the counterRxTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_counter_rx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Sets the counterRxTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] counterrxtrigger - The value to set the field to.
 */
__INLINE void nxmac_status_counter_rx_trigger_setf(uint8_t counterrxtrigger)
{
    ASSERT_ERR((((uint32_t)counterrxtrigger << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)counterrxtrigger << 19);
}

/**
 * @brief Returns the current value of the baRxTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the baRxTrigger field's value will be returned.
 *
 * @return The current value of the baRxTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ba_rx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Sets the baRxTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] barxtrigger - The value to set the field to.
 */
__INLINE void nxmac_status_ba_rx_trigger_setf(uint8_t barxtrigger)
{
    ASSERT_ERR((((uint32_t)barxtrigger << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)barxtrigger << 18);
}

/**
 * @brief Returns the current value of the timerRxTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the timerRxTrigger field's value will be returned.
 *
 * @return The current value of the timerRxTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_timer_rx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Sets the timerRxTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] timerrxtrigger - The value to set the field to.
 */
__INLINE void nxmac_status_timer_rx_trigger_setf(uint8_t timerrxtrigger)
{
    ASSERT_ERR((((uint32_t)timerrxtrigger << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)timerrxtrigger << 17);
}

/**
 * @brief Returns the current value of the rxTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the rxTrigger field's value will be returned.
 *
 * @return The current value of the rxTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_rx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the rxTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxtrigger - The value to set the field to.
 */
__INLINE void nxmac_status_rx_trigger_setf(uint8_t rxtrigger)
{
    ASSERT_ERR((((uint32_t)rxtrigger << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)rxtrigger << 16);
}

/**
 * @brief Returns the current value of the secUserTxTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the secUserTxTrigger field's value will be returned.
 *
 * @return The current value of the secUserTxTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_sec_user_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

/**
 * @brief Returns the current value of the timerTxTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the timerTxTrigger field's value will be returned.
 *
 * @return The current value of the timerTxTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_timer_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Sets the timerTxTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] timertxtrigger - The value to set the field to.
 */
__INLINE void nxmac_status_timer_tx_trigger_setf(uint8_t timertxtrigger)
{
    ASSERT_ERR((((uint32_t)timertxtrigger << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)timertxtrigger << 14);
}

/**
 * @brief Returns the current value of the txopComplete field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the txopComplete field's value will be returned.
 *
 * @return The current value of the txopComplete field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_txop_complete_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the txopComplete field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txopcomplete - The value to set the field to.
 */
__INLINE void nxmac_status_txop_complete_setf(uint8_t txopcomplete)
{
    ASSERT_ERR((((uint32_t)txopcomplete << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)txopcomplete << 13);
}

/**
 * @brief Returns the current value of the rdTxTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the rdTxTrigger field's value will be returned.
 *
 * @return The current value of the rdTxTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_rd_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the rdTxTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rdtxtrigger - The value to set the field to.
 */
__INLINE void nxmac_status_rd_tx_trigger_setf(uint8_t rdtxtrigger)
{
    ASSERT_ERR((((uint32_t)rdtxtrigger << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)rdtxtrigger << 12);
}

/**
 * @brief Returns the current value of the hccaTxTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the hccaTxTrigger field's value will be returned.
 *
 * @return The current value of the hccaTxTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_hcca_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the hccaTxTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] hccatxtrigger - The value to set the field to.
 */
__INLINE void nxmac_status_hcca_tx_trigger_setf(uint8_t hccatxtrigger)
{
    ASSERT_ERR((((uint32_t)hccatxtrigger << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)hccatxtrigger << 11);
}

/**
 * @brief Returns the current value of the bcnTxTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the bcnTxTrigger field's value will be returned.
 *
 * @return The current value of the bcnTxTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_bcn_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the bcnTxTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bcntxtrigger - The value to set the field to.
 */
__INLINE void nxmac_status_bcn_tx_trigger_setf(uint8_t bcntxtrigger)
{
    ASSERT_ERR((((uint32_t)bcntxtrigger << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)bcntxtrigger << 10);
}

/**
 * @brief Returns the current value of the ac3TxTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the ac3TxTrigger field's value will be returned.
 *
 * @return The current value of the ac3TxTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_3_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the ac3TxTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac3txtrigger - The value to set the field to.
 */
__INLINE void nxmac_status_ac_3_tx_trigger_setf(uint8_t ac3txtrigger)
{
    ASSERT_ERR((((uint32_t)ac3txtrigger << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)ac3txtrigger << 9);
}

/**
 * @brief Returns the current value of the ac2TxTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the ac2TxTrigger field's value will be returned.
 *
 * @return The current value of the ac2TxTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_2_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the ac2TxTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac2txtrigger - The value to set the field to.
 */
__INLINE void nxmac_status_ac_2_tx_trigger_setf(uint8_t ac2txtrigger)
{
    ASSERT_ERR((((uint32_t)ac2txtrigger << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)ac2txtrigger << 8);
}

/**
 * @brief Returns the current value of the ac1TxTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the ac1TxTrigger field's value will be returned.
 *
 * @return The current value of the ac1TxTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_1_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the ac1TxTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac1txtrigger - The value to set the field to.
 */
__INLINE void nxmac_status_ac_1_tx_trigger_setf(uint8_t ac1txtrigger)
{
    ASSERT_ERR((((uint32_t)ac1txtrigger << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)ac1txtrigger << 7);
}

/**
 * @brief Returns the current value of the ac0TxTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the ac0TxTrigger field's value will be returned.
 *
 * @return The current value of the ac0TxTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_0_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the ac0TxTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac0txtrigger - The value to set the field to.
 */
__INLINE void nxmac_status_ac_0_tx_trigger_setf(uint8_t ac0txtrigger)
{
    ASSERT_ERR((((uint32_t)ac0txtrigger << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)ac0txtrigger << 6);
}

/**
 * @brief Returns the current value of the rdProtTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the rdProtTrigger field's value will be returned.
 *
 * @return The current value of the rdProtTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_rd_prot_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the rdProtTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rdprottrigger - The value to set the field to.
 */
__INLINE void nxmac_status_rd_prot_trigger_setf(uint8_t rdprottrigger)
{
    ASSERT_ERR((((uint32_t)rdprottrigger << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)rdprottrigger << 5);
}

/**
 * @brief Returns the current value of the hccaProtTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the hccaProtTrigger field's value will be returned.
 *
 * @return The current value of the hccaProtTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_hcca_prot_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the hccaProtTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] hccaprottrigger - The value to set the field to.
 */
__INLINE void nxmac_status_hcca_prot_trigger_setf(uint8_t hccaprottrigger)
{
    ASSERT_ERR((((uint32_t)hccaprottrigger << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)hccaprottrigger << 4);
}

/**
 * @brief Returns the current value of the ac3ProtTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the ac3ProtTrigger field's value will be returned.
 *
 * @return The current value of the ac3ProtTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_3_prot_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the ac3ProtTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac3prottrigger - The value to set the field to.
 */
__INLINE void nxmac_status_ac_3_prot_trigger_setf(uint8_t ac3prottrigger)
{
    ASSERT_ERR((((uint32_t)ac3prottrigger << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)ac3prottrigger << 3);
}

/**
 * @brief Returns the current value of the ac2ProtTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the ac2ProtTrigger field's value will be returned.
 *
 * @return The current value of the ac2ProtTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_2_prot_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the ac2ProtTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac2prottrigger - The value to set the field to.
 */
__INLINE void nxmac_status_ac_2_prot_trigger_setf(uint8_t ac2prottrigger)
{
    ASSERT_ERR((((uint32_t)ac2prottrigger << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)ac2prottrigger << 2);
}

/**
 * @brief Returns the current value of the ac1ProtTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the ac1ProtTrigger field's value will be returned.
 *
 * @return The current value of the ac1ProtTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_1_prot_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the ac1ProtTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac1prottrigger - The value to set the field to.
 */
__INLINE void nxmac_status_ac_1_prot_trigger_setf(uint8_t ac1prottrigger)
{
    ASSERT_ERR((((uint32_t)ac1prottrigger << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)ac1prottrigger << 1);
}

/**
 * @brief Returns the current value of the ac0ProtTrigger field in the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read and the ac0ProtTrigger field's value will be returned.
 *
 * @return The current value of the ac0ProtTrigger field in the TX_RX_INT_STATUS register.
 */
__INLINE uint8_t nxmac_status_ac_0_prot_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the ac0ProtTrigger field of the TX_RX_INT_STATUS register.
 *
 * The TX_RX_INT_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac0prottrigger - The value to set the field to.
 */
__INLINE void nxmac_status_ac_0_prot_trigger_setf(uint8_t ac0prottrigger)
{
    ASSERT_ERR((((uint32_t)ac0prottrigger << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_STATUS_ADDR, (uint32_t)ac0prottrigger << 0);
}

/// @}

/**
 * @name TX_RX_INT_ACK register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     28      bcnTxBufTrigger   0
 *     27      ac3TxBufTrigger   0
 *     26      ac2TxBufTrigger   0
 *     25      ac1TxBufTrigger   0
 *     24      ac0TxBufTrigger   0
 *     23     ac3BWDropTrigger   0
 *     22     ac2BWDropTrigger   0
 *     21     ac1BWDropTrigger   0
 *     20     ac0BWDropTrigger   0
 *     19     counterRxTrigger   0
 *     18          baRxTrigger   0
 *     17       timerRxTrigger   0
 *     16            rxTrigger   0
 *     14       timerTxTrigger   0
 *     13         txopComplete   0
 *     12          rdTxTrigger   0
 *     11        hccaTxTrigger   0
 *     10         bcnTxTrigger   0
 *     09         ac3TxTrigger   0
 *     08         ac2TxTrigger   0
 *     07         ac1TxTrigger   0
 *     06         ac0TxTrigger   0
 *     05        rdProtTrigger   0
 *     04      hccaProtTrigger   0
 *     03       ac3ProtTrigger   0
 *     02       ac2ProtTrigger   0
 *     01       ac1ProtTrigger   0
 *     00       ac0ProtTrigger   0
 * </pre>
 *
 * @{
 */

/// Address of the TX_RX_INT_ACK register
#define NXMAC_TX_RX_INT_ACK_ADDR   0xC000807C
/// Offset of the TX_RX_INT_ACK register from the base address
#define NXMAC_TX_RX_INT_ACK_OFFSET 0x0000007C
/// Index of the TX_RX_INT_ACK register
#define NXMAC_TX_RX_INT_ACK_INDEX  0x0000001F
/// Reset value of the TX_RX_INT_ACK register
#define NXMAC_TX_RX_INT_ACK_RESET  0x00000000

/**
 * @brief Sets the TX_RX_INT_ACK register to a value.
 * The TX_RX_INT_ACK register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_tx_rx_int_ack_clear(uint32_t value)
{
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, value);
}

// fields defined in symmetrical set/clear register
/**
 * @brief Constructs a value for the TX_RX_INT_ACK register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] bcntxbuftrigger - The value to use for the bcnTxBufTrigger field.
 * @param[in] ac3txbuftrigger - The value to use for the ac3TxBufTrigger field.
 * @param[in] ac2txbuftrigger - The value to use for the ac2TxBufTrigger field.
 * @param[in] ac1txbuftrigger - The value to use for the ac1TxBufTrigger field.
 * @param[in] ac0txbuftrigger - The value to use for the ac0TxBufTrigger field.
 * @param[in] ac3bwdroptrigger - The value to use for the ac3BWDropTrigger field.
 * @param[in] ac2bwdroptrigger - The value to use for the ac2BWDropTrigger field.
 * @param[in] ac1bwdroptrigger - The value to use for the ac1BWDropTrigger field.
 * @param[in] ac0bwdroptrigger - The value to use for the ac0BWDropTrigger field.
 * @param[in] counterrxtrigger - The value to use for the counterRxTrigger field.
 * @param[in] barxtrigger - The value to use for the baRxTrigger field.
 * @param[in] timerrxtrigger - The value to use for the timerRxTrigger field.
 * @param[in] rxtrigger - The value to use for the rxTrigger field.
 * @param[in] timertxtrigger - The value to use for the timerTxTrigger field.
 * @param[in] txopcomplete - The value to use for the txopComplete field.
 * @param[in] rdtxtrigger - The value to use for the rdTxTrigger field.
 * @param[in] hccatxtrigger - The value to use for the hccaTxTrigger field.
 * @param[in] bcntxtrigger - The value to use for the bcnTxTrigger field.
 * @param[in] ac3txtrigger - The value to use for the ac3TxTrigger field.
 * @param[in] ac2txtrigger - The value to use for the ac2TxTrigger field.
 * @param[in] ac1txtrigger - The value to use for the ac1TxTrigger field.
 * @param[in] ac0txtrigger - The value to use for the ac0TxTrigger field.
 * @param[in] rdprottrigger - The value to use for the rdProtTrigger field.
 * @param[in] hccaprottrigger - The value to use for the hccaProtTrigger field.
 * @param[in] ac3prottrigger - The value to use for the ac3ProtTrigger field.
 * @param[in] ac2prottrigger - The value to use for the ac2ProtTrigger field.
 * @param[in] ac1prottrigger - The value to use for the ac1ProtTrigger field.
 * @param[in] ac0prottrigger - The value to use for the ac0ProtTrigger field.
 */
__INLINE void nxmac_tx_rx_int_ack_pack(uint8_t bcntxbuftrigger, uint8_t ac3txbuftrigger, uint8_t ac2txbuftrigger, uint8_t ac1txbuftrigger, uint8_t ac0txbuftrigger, uint8_t ac3bwdroptrigger, uint8_t ac2bwdroptrigger, uint8_t ac1bwdroptrigger, uint8_t ac0bwdroptrigger, uint8_t counterrxtrigger, uint8_t barxtrigger, uint8_t timerrxtrigger, uint8_t rxtrigger, uint8_t timertxtrigger, uint8_t txopcomplete, uint8_t rdtxtrigger, uint8_t hccatxtrigger, uint8_t bcntxtrigger, uint8_t ac3txtrigger, uint8_t ac2txtrigger, uint8_t ac1txtrigger, uint8_t ac0txtrigger, uint8_t rdprottrigger, uint8_t hccaprottrigger, uint8_t ac3prottrigger, uint8_t ac2prottrigger, uint8_t ac1prottrigger, uint8_t ac0prottrigger)
{
    ASSERT_ERR((((uint32_t)bcntxbuftrigger << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)ac3txbuftrigger << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)ac2txbuftrigger << 26) & ~((uint32_t)0x04000000)) == 0);
    ASSERT_ERR((((uint32_t)ac1txbuftrigger << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)ac0txbuftrigger << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)ac3bwdroptrigger << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)ac2bwdroptrigger << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)ac1bwdroptrigger << 21) & ~((uint32_t)0x00200000)) == 0);
    ASSERT_ERR((((uint32_t)ac0bwdroptrigger << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)counterrxtrigger << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)barxtrigger << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)timerrxtrigger << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)rxtrigger << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)timertxtrigger << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)txopcomplete << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)rdtxtrigger << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)hccatxtrigger << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)bcntxtrigger << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)ac3txtrigger << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)ac2txtrigger << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)ac1txtrigger << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)ac0txtrigger << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)rdprottrigger << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)hccaprottrigger << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)ac3prottrigger << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)ac2prottrigger << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)ac1prottrigger << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)ac0prottrigger << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR,  ((uint32_t)bcntxbuftrigger << 28) | ((uint32_t)ac3txbuftrigger << 27) | ((uint32_t)ac2txbuftrigger << 26) | ((uint32_t)ac1txbuftrigger << 25) | ((uint32_t)ac0txbuftrigger << 24) | ((uint32_t)ac3bwdroptrigger << 23) | ((uint32_t)ac2bwdroptrigger << 22) | ((uint32_t)ac1bwdroptrigger << 21) | ((uint32_t)ac0bwdroptrigger << 20) | ((uint32_t)counterrxtrigger << 19) | ((uint32_t)barxtrigger << 18) | ((uint32_t)timerrxtrigger << 17) | ((uint32_t)rxtrigger << 16) | ((uint32_t)timertxtrigger << 14) | ((uint32_t)txopcomplete << 13) | ((uint32_t)rdtxtrigger << 12) | ((uint32_t)hccatxtrigger << 11) | ((uint32_t)bcntxtrigger << 10) | ((uint32_t)ac3txtrigger << 9) | ((uint32_t)ac2txtrigger << 8) | ((uint32_t)ac1txtrigger << 7) | ((uint32_t)ac0txtrigger << 6) | ((uint32_t)rdprottrigger << 5) | ((uint32_t)hccaprottrigger << 4) | ((uint32_t)ac3prottrigger << 3) | ((uint32_t)ac2prottrigger << 2) | ((uint32_t)ac1prottrigger << 1) | ((uint32_t)ac0prottrigger << 0));
}

/**
 * @brief Sets the bcnTxBufTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bcntxbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_bcn_tx_buf_trigger_clearf(uint8_t bcntxbuftrigger)
{
    ASSERT_ERR((((uint32_t)bcntxbuftrigger << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)bcntxbuftrigger << 28);
}

/**
 * @brief Sets the ac3TxBufTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac3txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_3_tx_buf_trigger_clearf(uint8_t ac3txbuftrigger)
{
    ASSERT_ERR((((uint32_t)ac3txbuftrigger << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)ac3txbuftrigger << 27);
}

/**
 * @brief Sets the ac2TxBufTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac2txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_2_tx_buf_trigger_clearf(uint8_t ac2txbuftrigger)
{
    ASSERT_ERR((((uint32_t)ac2txbuftrigger << 26) & ~((uint32_t)0x04000000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)ac2txbuftrigger << 26);
}

/**
 * @brief Sets the ac1TxBufTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac1txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_1_tx_buf_trigger_clearf(uint8_t ac1txbuftrigger)
{
    ASSERT_ERR((((uint32_t)ac1txbuftrigger << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)ac1txbuftrigger << 25);
}

/**
 * @brief Sets the ac0TxBufTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac0txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_0_tx_buf_trigger_clearf(uint8_t ac0txbuftrigger)
{
    ASSERT_ERR((((uint32_t)ac0txbuftrigger << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)ac0txbuftrigger << 24);
}

/**
 * @brief Sets the ac3BWDropTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac3bwdroptrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_3bw_drop_trigger_clearf(uint8_t ac3bwdroptrigger)
{
    ASSERT_ERR((((uint32_t)ac3bwdroptrigger << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)ac3bwdroptrigger << 23);
}

/**
 * @brief Sets the ac2BWDropTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac2bwdroptrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_2bw_drop_trigger_clearf(uint8_t ac2bwdroptrigger)
{
    ASSERT_ERR((((uint32_t)ac2bwdroptrigger << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)ac2bwdroptrigger << 22);
}

/**
 * @brief Sets the ac1BWDropTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac1bwdroptrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_1bw_drop_trigger_clearf(uint8_t ac1bwdroptrigger)
{
    ASSERT_ERR((((uint32_t)ac1bwdroptrigger << 21) & ~((uint32_t)0x00200000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)ac1bwdroptrigger << 21);
}

/**
 * @brief Sets the ac0BWDropTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac0bwdroptrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_0bw_drop_trigger_clearf(uint8_t ac0bwdroptrigger)
{
    ASSERT_ERR((((uint32_t)ac0bwdroptrigger << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)ac0bwdroptrigger << 20);
}

/**
 * @brief Sets the counterRxTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] counterrxtrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_counter_rx_trigger_clearf(uint8_t counterrxtrigger)
{
    ASSERT_ERR((((uint32_t)counterrxtrigger << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)counterrxtrigger << 19);
}

/**
 * @brief Sets the baRxTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] barxtrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_ba_rx_trigger_clearf(uint8_t barxtrigger)
{
    ASSERT_ERR((((uint32_t)barxtrigger << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)barxtrigger << 18);
}

/**
 * @brief Sets the timerRxTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] timerrxtrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_timer_rx_trigger_clearf(uint8_t timerrxtrigger)
{
    ASSERT_ERR((((uint32_t)timerrxtrigger << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)timerrxtrigger << 17);
}

/**
 * @brief Sets the rxTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxtrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_rx_trigger_clearf(uint8_t rxtrigger)
{
    ASSERT_ERR((((uint32_t)rxtrigger << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)rxtrigger << 16);
}

/**
 * @brief Sets the timerTxTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] timertxtrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_timer_tx_trigger_clearf(uint8_t timertxtrigger)
{
    ASSERT_ERR((((uint32_t)timertxtrigger << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)timertxtrigger << 14);
}

/**
 * @brief Sets the txopComplete field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txopcomplete - The value to set the field to.
 */
__INLINE void nxmac_ack_txop_complete_clearf(uint8_t txopcomplete)
{
    ASSERT_ERR((((uint32_t)txopcomplete << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)txopcomplete << 13);
}

/**
 * @brief Sets the rdTxTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rdtxtrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_rd_tx_trigger_clearf(uint8_t rdtxtrigger)
{
    ASSERT_ERR((((uint32_t)rdtxtrigger << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)rdtxtrigger << 12);
}

/**
 * @brief Sets the hccaTxTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] hccatxtrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_hcca_tx_trigger_clearf(uint8_t hccatxtrigger)
{
    ASSERT_ERR((((uint32_t)hccatxtrigger << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)hccatxtrigger << 11);
}

/**
 * @brief Sets the bcnTxTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bcntxtrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_bcn_tx_trigger_clearf(uint8_t bcntxtrigger)
{
    ASSERT_ERR((((uint32_t)bcntxtrigger << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)bcntxtrigger << 10);
}

/**
 * @brief Sets the ac3TxTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac3txtrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_3_tx_trigger_clearf(uint8_t ac3txtrigger)
{
    ASSERT_ERR((((uint32_t)ac3txtrigger << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)ac3txtrigger << 9);
}

/**
 * @brief Sets the ac2TxTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac2txtrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_2_tx_trigger_clearf(uint8_t ac2txtrigger)
{
    ASSERT_ERR((((uint32_t)ac2txtrigger << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)ac2txtrigger << 8);
}

/**
 * @brief Sets the ac1TxTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac1txtrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_1_tx_trigger_clearf(uint8_t ac1txtrigger)
{
    ASSERT_ERR((((uint32_t)ac1txtrigger << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)ac1txtrigger << 7);
}

/**
 * @brief Sets the ac0TxTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac0txtrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_0_tx_trigger_clearf(uint8_t ac0txtrigger)
{
    ASSERT_ERR((((uint32_t)ac0txtrigger << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)ac0txtrigger << 6);
}

/**
 * @brief Sets the rdProtTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rdprottrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_rd_prot_trigger_clearf(uint8_t rdprottrigger)
{
    ASSERT_ERR((((uint32_t)rdprottrigger << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)rdprottrigger << 5);
}

/**
 * @brief Sets the hccaProtTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] hccaprottrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_hcca_prot_trigger_clearf(uint8_t hccaprottrigger)
{
    ASSERT_ERR((((uint32_t)hccaprottrigger << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)hccaprottrigger << 4);
}

/**
 * @brief Sets the ac3ProtTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac3prottrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_3_prot_trigger_clearf(uint8_t ac3prottrigger)
{
    ASSERT_ERR((((uint32_t)ac3prottrigger << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)ac3prottrigger << 3);
}

/**
 * @brief Sets the ac2ProtTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac2prottrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_2_prot_trigger_clearf(uint8_t ac2prottrigger)
{
    ASSERT_ERR((((uint32_t)ac2prottrigger << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)ac2prottrigger << 2);
}

/**
 * @brief Sets the ac1ProtTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac1prottrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_1_prot_trigger_clearf(uint8_t ac1prottrigger)
{
    ASSERT_ERR((((uint32_t)ac1prottrigger << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)ac1prottrigger << 1);
}

/**
 * @brief Sets the ac0ProtTrigger field of the TX_RX_INT_ACK register.
 *
 * The TX_RX_INT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac0prottrigger - The value to set the field to.
 */
__INLINE void nxmac_ack_ac_0_prot_trigger_clearf(uint8_t ac0prottrigger)
{
    ASSERT_ERR((((uint32_t)ac0prottrigger << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ACK_ADDR, (uint32_t)ac0prottrigger << 0);
}

/// @}

/**
 * @name TX_RX_INT_ENABLE register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31      masterTxRxIntEn   0
 *     28      bcnTxBufTrigger   0
 *     27      ac3TxBufTrigger   0
 *     26      ac2TxBufTrigger   0
 *     25      ac1TxBufTrigger   0
 *     24      ac0TxBufTrigger   0
 *     23     ac3BWDropTrigger   0
 *     22     ac2BWDropTrigger   0
 *     21     ac1BWDropTrigger   0
 *     20     ac0BWDropTrigger   0
 *     19     counterRxTrigger   0
 *     18          baRxTrigger   0
 *     17       timerRxTrigger   0
 *     16            rxTrigger   0
 *     15     secUserTxTrigger   0
 *     14       timerTxTrigger   0
 *     13         txopComplete   0
 *     12          rdTxTrigger   0
 *     11        hccaTxTrigger   0
 *     10         bcnTxTrigger   0
 *     09         ac3TxTrigger   0
 *     08         ac2TxTrigger   0
 *     07         ac1TxTrigger   0
 *     06         ac0TxTrigger   0
 *     05        rdProtTrigger   0
 *     04      hccaProtTrigger   0
 *     03       ac3ProtTrigger   0
 *     02       ac2ProtTrigger   0
 *     01       ac1ProtTrigger   0
 *     00       ac0ProtTrigger   0
 * </pre>
 *
 * @{
 */

/// Address of the TX_RX_INT_ENABLE register
#define NXMAC_TX_RX_INT_ENABLE_ADDR   0xC0008080
/// Offset of the TX_RX_INT_ENABLE register from the base address
#define NXMAC_TX_RX_INT_ENABLE_OFFSET 0x00000080
/// Index of the TX_RX_INT_ENABLE register
#define NXMAC_TX_RX_INT_ENABLE_INDEX  0x00000020
/// Reset value of the TX_RX_INT_ENABLE register
#define NXMAC_TX_RX_INT_ENABLE_RESET  0x00000000

/**
 * @brief Returns the current value of the TX_RX_INT_ENABLE register.
 * The TX_RX_INT_ENABLE register will be read and its value returned.
 * @return The current value of the TX_RX_INT_ENABLE register.
 */
__INLINE uint32_t nxmac_tx_rx_int_enable_get(void)
{
    return REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
}

/**
 * @brief Sets the TX_RX_INT_ENABLE register to a value.
 * The TX_RX_INT_ENABLE register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_tx_rx_int_enable_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, value);
}

// field definitions
/// MASTER_TX_RX_INT_EN field bit
#define NXMAC_MASTER_TX_RX_INT_EN_BIT    ((uint32_t)0x80000000)
/// MASTER_TX_RX_INT_EN field position
#define NXMAC_MASTER_TX_RX_INT_EN_POS    31
/// BCN_TX_BUF_TRIGGER field bit
#define NXMAC_BCN_TX_BUF_TRIGGER_BIT     ((uint32_t)0x10000000)
/// BCN_TX_BUF_TRIGGER field position
#define NXMAC_BCN_TX_BUF_TRIGGER_POS     28
/// AC_3_TX_BUF_TRIGGER field bit
#define NXMAC_AC_3_TX_BUF_TRIGGER_BIT    ((uint32_t)0x08000000)
/// AC_3_TX_BUF_TRIGGER field position
#define NXMAC_AC_3_TX_BUF_TRIGGER_POS    27
/// AC_2_TX_BUF_TRIGGER field bit
#define NXMAC_AC_2_TX_BUF_TRIGGER_BIT    ((uint32_t)0x04000000)
/// AC_2_TX_BUF_TRIGGER field position
#define NXMAC_AC_2_TX_BUF_TRIGGER_POS    26
/// AC_1_TX_BUF_TRIGGER field bit
#define NXMAC_AC_1_TX_BUF_TRIGGER_BIT    ((uint32_t)0x02000000)
/// AC_1_TX_BUF_TRIGGER field position
#define NXMAC_AC_1_TX_BUF_TRIGGER_POS    25
/// AC_0_TX_BUF_TRIGGER field bit
#define NXMAC_AC_0_TX_BUF_TRIGGER_BIT    ((uint32_t)0x01000000)
/// AC_0_TX_BUF_TRIGGER field position
#define NXMAC_AC_0_TX_BUF_TRIGGER_POS    24
/// AC_3BW_DROP_TRIGGER field bit
#define NXMAC_AC_3BW_DROP_TRIGGER_BIT    ((uint32_t)0x00800000)
/// AC_3BW_DROP_TRIGGER field position
#define NXMAC_AC_3BW_DROP_TRIGGER_POS    23
/// AC_2BW_DROP_TRIGGER field bit
#define NXMAC_AC_2BW_DROP_TRIGGER_BIT    ((uint32_t)0x00400000)
/// AC_2BW_DROP_TRIGGER field position
#define NXMAC_AC_2BW_DROP_TRIGGER_POS    22
/// AC_1BW_DROP_TRIGGER field bit
#define NXMAC_AC_1BW_DROP_TRIGGER_BIT    ((uint32_t)0x00200000)
/// AC_1BW_DROP_TRIGGER field position
#define NXMAC_AC_1BW_DROP_TRIGGER_POS    21
/// AC_0BW_DROP_TRIGGER field bit
#define NXMAC_AC_0BW_DROP_TRIGGER_BIT    ((uint32_t)0x00100000)
/// AC_0BW_DROP_TRIGGER field position
#define NXMAC_AC_0BW_DROP_TRIGGER_POS    20
/// COUNTER_RX_TRIGGER field bit
#define NXMAC_COUNTER_RX_TRIGGER_BIT     ((uint32_t)0x00080000)
/// COUNTER_RX_TRIGGER field position
#define NXMAC_COUNTER_RX_TRIGGER_POS     19
/// BA_RX_TRIGGER field bit
#define NXMAC_BA_RX_TRIGGER_BIT          ((uint32_t)0x00040000)
/// BA_RX_TRIGGER field position
#define NXMAC_BA_RX_TRIGGER_POS          18
/// TIMER_RX_TRIGGER field bit
#define NXMAC_TIMER_RX_TRIGGER_BIT       ((uint32_t)0x00020000)
/// TIMER_RX_TRIGGER field position
#define NXMAC_TIMER_RX_TRIGGER_POS       17
/// RX_TRIGGER field bit
#define NXMAC_RX_TRIGGER_BIT             ((uint32_t)0x00010000)
/// RX_TRIGGER field position
#define NXMAC_RX_TRIGGER_POS             16
/// SEC_USER_TX_TRIGGER field bit
#define NXMAC_SEC_USER_TX_TRIGGER_BIT    ((uint32_t)0x00008000)
/// SEC_USER_TX_TRIGGER field position
#define NXMAC_SEC_USER_TX_TRIGGER_POS    15
/// TIMER_TX_TRIGGER field bit
#define NXMAC_TIMER_TX_TRIGGER_BIT       ((uint32_t)0x00004000)
/// TIMER_TX_TRIGGER field position
#define NXMAC_TIMER_TX_TRIGGER_POS       14
/// TXOP_COMPLETE field bit
#define NXMAC_TXOP_COMPLETE_BIT          ((uint32_t)0x00002000)
/// TXOP_COMPLETE field position
#define NXMAC_TXOP_COMPLETE_POS          13
/// RD_TX_TRIGGER field bit
#define NXMAC_RD_TX_TRIGGER_BIT          ((uint32_t)0x00001000)
/// RD_TX_TRIGGER field position
#define NXMAC_RD_TX_TRIGGER_POS          12
/// HCCA_TX_TRIGGER field bit
#define NXMAC_HCCA_TX_TRIGGER_BIT        ((uint32_t)0x00000800)
/// HCCA_TX_TRIGGER field position
#define NXMAC_HCCA_TX_TRIGGER_POS        11
/// BCN_TX_TRIGGER field bit
#define NXMAC_BCN_TX_TRIGGER_BIT         ((uint32_t)0x00000400)
/// BCN_TX_TRIGGER field position
#define NXMAC_BCN_TX_TRIGGER_POS         10
/// AC_3_TX_TRIGGER field bit
#define NXMAC_AC_3_TX_TRIGGER_BIT        ((uint32_t)0x00000200)
/// AC_3_TX_TRIGGER field position
#define NXMAC_AC_3_TX_TRIGGER_POS        9
/// AC_2_TX_TRIGGER field bit
#define NXMAC_AC_2_TX_TRIGGER_BIT        ((uint32_t)0x00000100)
/// AC_2_TX_TRIGGER field position
#define NXMAC_AC_2_TX_TRIGGER_POS        8
/// AC_1_TX_TRIGGER field bit
#define NXMAC_AC_1_TX_TRIGGER_BIT        ((uint32_t)0x00000080)
/// AC_1_TX_TRIGGER field position
#define NXMAC_AC_1_TX_TRIGGER_POS        7
/// AC_0_TX_TRIGGER field bit
#define NXMAC_AC_0_TX_TRIGGER_BIT        ((uint32_t)0x00000040)
/// AC_0_TX_TRIGGER field position
#define NXMAC_AC_0_TX_TRIGGER_POS        6
/// RD_PROT_TRIGGER field bit
#define NXMAC_RD_PROT_TRIGGER_BIT        ((uint32_t)0x00000020)
/// RD_PROT_TRIGGER field position
#define NXMAC_RD_PROT_TRIGGER_POS        5
/// HCCA_PROT_TRIGGER field bit
#define NXMAC_HCCA_PROT_TRIGGER_BIT      ((uint32_t)0x00000010)
/// HCCA_PROT_TRIGGER field position
#define NXMAC_HCCA_PROT_TRIGGER_POS      4
/// AC_3_PROT_TRIGGER field bit
#define NXMAC_AC_3_PROT_TRIGGER_BIT      ((uint32_t)0x00000008)
/// AC_3_PROT_TRIGGER field position
#define NXMAC_AC_3_PROT_TRIGGER_POS      3
/// AC_2_PROT_TRIGGER field bit
#define NXMAC_AC_2_PROT_TRIGGER_BIT      ((uint32_t)0x00000004)
/// AC_2_PROT_TRIGGER field position
#define NXMAC_AC_2_PROT_TRIGGER_POS      2
/// AC_1_PROT_TRIGGER field bit
#define NXMAC_AC_1_PROT_TRIGGER_BIT      ((uint32_t)0x00000002)
/// AC_1_PROT_TRIGGER field position
#define NXMAC_AC_1_PROT_TRIGGER_POS      1
/// AC_0_PROT_TRIGGER field bit
#define NXMAC_AC_0_PROT_TRIGGER_BIT      ((uint32_t)0x00000001)
/// AC_0_PROT_TRIGGER field position
#define NXMAC_AC_0_PROT_TRIGGER_POS      0

/// MASTER_TX_RX_INT_EN field reset value
#define NXMAC_MASTER_TX_RX_INT_EN_RST    0x0
/// BCN_TX_BUF_TRIGGER field reset value
#define NXMAC_BCN_TX_BUF_TRIGGER_RST     0x0
/// AC_3_TX_BUF_TRIGGER field reset value
#define NXMAC_AC_3_TX_BUF_TRIGGER_RST    0x0
/// AC_2_TX_BUF_TRIGGER field reset value
#define NXMAC_AC_2_TX_BUF_TRIGGER_RST    0x0
/// AC_1_TX_BUF_TRIGGER field reset value
#define NXMAC_AC_1_TX_BUF_TRIGGER_RST    0x0
/// AC_0_TX_BUF_TRIGGER field reset value
#define NXMAC_AC_0_TX_BUF_TRIGGER_RST    0x0
/// AC_3BW_DROP_TRIGGER field reset value
#define NXMAC_AC_3BW_DROP_TRIGGER_RST    0x0
/// AC_2BW_DROP_TRIGGER field reset value
#define NXMAC_AC_2BW_DROP_TRIGGER_RST    0x0
/// AC_1BW_DROP_TRIGGER field reset value
#define NXMAC_AC_1BW_DROP_TRIGGER_RST    0x0
/// AC_0BW_DROP_TRIGGER field reset value
#define NXMAC_AC_0BW_DROP_TRIGGER_RST    0x0
/// COUNTER_RX_TRIGGER field reset value
#define NXMAC_COUNTER_RX_TRIGGER_RST     0x0
/// BA_RX_TRIGGER field reset value
#define NXMAC_BA_RX_TRIGGER_RST          0x0
/// TIMER_RX_TRIGGER field reset value
#define NXMAC_TIMER_RX_TRIGGER_RST       0x0
/// RX_TRIGGER field reset value
#define NXMAC_RX_TRIGGER_RST             0x0
/// SEC_USER_TX_TRIGGER field reset value
#define NXMAC_SEC_USER_TX_TRIGGER_RST    0x0
/// TIMER_TX_TRIGGER field reset value
#define NXMAC_TIMER_TX_TRIGGER_RST       0x0
/// TXOP_COMPLETE field reset value
#define NXMAC_TXOP_COMPLETE_RST          0x0
/// RD_TX_TRIGGER field reset value
#define NXMAC_RD_TX_TRIGGER_RST          0x0
/// HCCA_TX_TRIGGER field reset value
#define NXMAC_HCCA_TX_TRIGGER_RST        0x0
/// BCN_TX_TRIGGER field reset value
#define NXMAC_BCN_TX_TRIGGER_RST         0x0
/// AC_3_TX_TRIGGER field reset value
#define NXMAC_AC_3_TX_TRIGGER_RST        0x0
/// AC_2_TX_TRIGGER field reset value
#define NXMAC_AC_2_TX_TRIGGER_RST        0x0
/// AC_1_TX_TRIGGER field reset value
#define NXMAC_AC_1_TX_TRIGGER_RST        0x0
/// AC_0_TX_TRIGGER field reset value
#define NXMAC_AC_0_TX_TRIGGER_RST        0x0
/// RD_PROT_TRIGGER field reset value
#define NXMAC_RD_PROT_TRIGGER_RST        0x0
/// HCCA_PROT_TRIGGER field reset value
#define NXMAC_HCCA_PROT_TRIGGER_RST      0x0
/// AC_3_PROT_TRIGGER field reset value
#define NXMAC_AC_3_PROT_TRIGGER_RST      0x0
/// AC_2_PROT_TRIGGER field reset value
#define NXMAC_AC_2_PROT_TRIGGER_RST      0x0
/// AC_1_PROT_TRIGGER field reset value
#define NXMAC_AC_1_PROT_TRIGGER_RST      0x0
/// AC_0_PROT_TRIGGER field reset value
#define NXMAC_AC_0_PROT_TRIGGER_RST      0x0

/**
 * @brief Constructs a value for the TX_RX_INT_ENABLE register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] mastertxrxinten - The value to use for the masterTxRxIntEn field.
 * @param[in] bcntxbuftrigger - The value to use for the bcnTxBufTrigger field.
 * @param[in] ac3txbuftrigger - The value to use for the ac3TxBufTrigger field.
 * @param[in] ac2txbuftrigger - The value to use for the ac2TxBufTrigger field.
 * @param[in] ac1txbuftrigger - The value to use for the ac1TxBufTrigger field.
 * @param[in] ac0txbuftrigger - The value to use for the ac0TxBufTrigger field.
 * @param[in] ac3bwdroptrigger - The value to use for the ac3BWDropTrigger field.
 * @param[in] ac2bwdroptrigger - The value to use for the ac2BWDropTrigger field.
 * @param[in] ac1bwdroptrigger - The value to use for the ac1BWDropTrigger field.
 * @param[in] ac0bwdroptrigger - The value to use for the ac0BWDropTrigger field.
 * @param[in] counterrxtrigger - The value to use for the counterRxTrigger field.
 * @param[in] barxtrigger - The value to use for the baRxTrigger field.
 * @param[in] timerrxtrigger - The value to use for the timerRxTrigger field.
 * @param[in] rxtrigger - The value to use for the rxTrigger field.
 * @param[in] secusertxtrigger - The value to use for the secUserTxTrigger field.
 * @param[in] timertxtrigger - The value to use for the timerTxTrigger field.
 * @param[in] txopcomplete - The value to use for the txopComplete field.
 * @param[in] rdtxtrigger - The value to use for the rdTxTrigger field.
 * @param[in] hccatxtrigger - The value to use for the hccaTxTrigger field.
 * @param[in] bcntxtrigger - The value to use for the bcnTxTrigger field.
 * @param[in] ac3txtrigger - The value to use for the ac3TxTrigger field.
 * @param[in] ac2txtrigger - The value to use for the ac2TxTrigger field.
 * @param[in] ac1txtrigger - The value to use for the ac1TxTrigger field.
 * @param[in] ac0txtrigger - The value to use for the ac0TxTrigger field.
 * @param[in] rdprottrigger - The value to use for the rdProtTrigger field.
 * @param[in] hccaprottrigger - The value to use for the hccaProtTrigger field.
 * @param[in] ac3prottrigger - The value to use for the ac3ProtTrigger field.
 * @param[in] ac2prottrigger - The value to use for the ac2ProtTrigger field.
 * @param[in] ac1prottrigger - The value to use for the ac1ProtTrigger field.
 * @param[in] ac0prottrigger - The value to use for the ac0ProtTrigger field.
 */
__INLINE void nxmac_tx_rx_int_enable_pack(uint8_t mastertxrxinten, uint8_t bcntxbuftrigger, uint8_t ac3txbuftrigger, uint8_t ac2txbuftrigger, uint8_t ac1txbuftrigger, uint8_t ac0txbuftrigger, uint8_t ac3bwdroptrigger, uint8_t ac2bwdroptrigger, uint8_t ac1bwdroptrigger, uint8_t ac0bwdroptrigger, uint8_t counterrxtrigger, uint8_t barxtrigger, uint8_t timerrxtrigger, uint8_t rxtrigger, uint8_t secusertxtrigger, uint8_t timertxtrigger, uint8_t txopcomplete, uint8_t rdtxtrigger, uint8_t hccatxtrigger, uint8_t bcntxtrigger, uint8_t ac3txtrigger, uint8_t ac2txtrigger, uint8_t ac1txtrigger, uint8_t ac0txtrigger, uint8_t rdprottrigger, uint8_t hccaprottrigger, uint8_t ac3prottrigger, uint8_t ac2prottrigger, uint8_t ac1prottrigger, uint8_t ac0prottrigger)
{
    ASSERT_ERR((((uint32_t)mastertxrxinten << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)bcntxbuftrigger << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)ac3txbuftrigger << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)ac2txbuftrigger << 26) & ~((uint32_t)0x04000000)) == 0);
    ASSERT_ERR((((uint32_t)ac1txbuftrigger << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)ac0txbuftrigger << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)ac3bwdroptrigger << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)ac2bwdroptrigger << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)ac1bwdroptrigger << 21) & ~((uint32_t)0x00200000)) == 0);
    ASSERT_ERR((((uint32_t)ac0bwdroptrigger << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)counterrxtrigger << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)barxtrigger << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)timerrxtrigger << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)rxtrigger << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)secusertxtrigger << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)timertxtrigger << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)txopcomplete << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)rdtxtrigger << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)hccatxtrigger << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)bcntxtrigger << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)ac3txtrigger << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)ac2txtrigger << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)ac1txtrigger << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)ac0txtrigger << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)rdprottrigger << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)hccaprottrigger << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)ac3prottrigger << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)ac2prottrigger << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)ac1prottrigger << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)ac0prottrigger << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR,  ((uint32_t)mastertxrxinten << 31) | ((uint32_t)bcntxbuftrigger << 28) | ((uint32_t)ac3txbuftrigger << 27) | ((uint32_t)ac2txbuftrigger << 26) | ((uint32_t)ac1txbuftrigger << 25) | ((uint32_t)ac0txbuftrigger << 24) | ((uint32_t)ac3bwdroptrigger << 23) | ((uint32_t)ac2bwdroptrigger << 22) | ((uint32_t)ac1bwdroptrigger << 21) | ((uint32_t)ac0bwdroptrigger << 20) | ((uint32_t)counterrxtrigger << 19) | ((uint32_t)barxtrigger << 18) | ((uint32_t)timerrxtrigger << 17) | ((uint32_t)rxtrigger << 16) | ((uint32_t)secusertxtrigger << 15) | ((uint32_t)timertxtrigger << 14) | ((uint32_t)txopcomplete << 13) | ((uint32_t)rdtxtrigger << 12) | ((uint32_t)hccatxtrigger << 11) | ((uint32_t)bcntxtrigger << 10) | ((uint32_t)ac3txtrigger << 9) | ((uint32_t)ac2txtrigger << 8) | ((uint32_t)ac1txtrigger << 7) | ((uint32_t)ac0txtrigger << 6) | ((uint32_t)rdprottrigger << 5) | ((uint32_t)hccaprottrigger << 4) | ((uint32_t)ac3prottrigger << 3) | ((uint32_t)ac2prottrigger << 2) | ((uint32_t)ac1prottrigger << 1) | ((uint32_t)ac0prottrigger << 0));
}

/**
 * @brief Unpacks TX_RX_INT_ENABLE's fields from current value of the TX_RX_INT_ENABLE register.
 *
 * Reads the TX_RX_INT_ENABLE register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] mastertxrxinten - Will be populated with the current value of this field from the register.
 * @param[out] bcntxbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac3txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac2txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac1txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac0txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac3bwdroptrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac2bwdroptrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac1bwdroptrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac0bwdroptrigger - Will be populated with the current value of this field from the register.
 * @param[out] counterrxtrigger - Will be populated with the current value of this field from the register.
 * @param[out] barxtrigger - Will be populated with the current value of this field from the register.
 * @param[out] timerrxtrigger - Will be populated with the current value of this field from the register.
 * @param[out] rxtrigger - Will be populated with the current value of this field from the register.
 * @param[out] secusertxtrigger - Will be populated with the current value of this field from the register.
 * @param[out] timertxtrigger - Will be populated with the current value of this field from the register.
 * @param[out] txopcomplete - Will be populated with the current value of this field from the register.
 * @param[out] rdtxtrigger - Will be populated with the current value of this field from the register.
 * @param[out] hccatxtrigger - Will be populated with the current value of this field from the register.
 * @param[out] bcntxtrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac3txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac2txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac1txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac0txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] rdprottrigger - Will be populated with the current value of this field from the register.
 * @param[out] hccaprottrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac3prottrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac2prottrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac1prottrigger - Will be populated with the current value of this field from the register.
 * @param[out] ac0prottrigger - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_tx_rx_int_enable_unpack(uint8_t *mastertxrxinten, uint8_t *bcntxbuftrigger, uint8_t *ac3txbuftrigger, uint8_t *ac2txbuftrigger, uint8_t *ac1txbuftrigger, uint8_t *ac0txbuftrigger, uint8_t *ac3bwdroptrigger, uint8_t *ac2bwdroptrigger, uint8_t *ac1bwdroptrigger, uint8_t *ac0bwdroptrigger, uint8_t *counterrxtrigger, uint8_t *barxtrigger, uint8_t *timerrxtrigger, uint8_t *rxtrigger, uint8_t *secusertxtrigger, uint8_t *timertxtrigger, uint8_t *txopcomplete, uint8_t *rdtxtrigger, uint8_t *hccatxtrigger, uint8_t *bcntxtrigger, uint8_t *ac3txtrigger, uint8_t *ac2txtrigger, uint8_t *ac1txtrigger, uint8_t *ac0txtrigger, uint8_t *rdprottrigger, uint8_t *hccaprottrigger, uint8_t *ac3prottrigger, uint8_t *ac2prottrigger, uint8_t *ac1prottrigger, uint8_t *ac0prottrigger)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);

    *mastertxrxinten = (localVal & ((uint32_t)0x80000000)) >> 31;
    *bcntxbuftrigger = (localVal & ((uint32_t)0x10000000)) >> 28;
    *ac3txbuftrigger = (localVal & ((uint32_t)0x08000000)) >> 27;
    *ac2txbuftrigger = (localVal & ((uint32_t)0x04000000)) >> 26;
    *ac1txbuftrigger = (localVal & ((uint32_t)0x02000000)) >> 25;
    *ac0txbuftrigger = (localVal & ((uint32_t)0x01000000)) >> 24;
    *ac3bwdroptrigger = (localVal & ((uint32_t)0x00800000)) >> 23;
    *ac2bwdroptrigger = (localVal & ((uint32_t)0x00400000)) >> 22;
    *ac1bwdroptrigger = (localVal & ((uint32_t)0x00200000)) >> 21;
    *ac0bwdroptrigger = (localVal & ((uint32_t)0x00100000)) >> 20;
    *counterrxtrigger = (localVal & ((uint32_t)0x00080000)) >> 19;
    *barxtrigger = (localVal & ((uint32_t)0x00040000)) >> 18;
    *timerrxtrigger = (localVal & ((uint32_t)0x00020000)) >> 17;
    *rxtrigger = (localVal & ((uint32_t)0x00010000)) >> 16;
    *secusertxtrigger = (localVal & ((uint32_t)0x00008000)) >> 15;
    *timertxtrigger = (localVal & ((uint32_t)0x00004000)) >> 14;
    *txopcomplete = (localVal & ((uint32_t)0x00002000)) >> 13;
    *rdtxtrigger = (localVal & ((uint32_t)0x00001000)) >> 12;
    *hccatxtrigger = (localVal & ((uint32_t)0x00000800)) >> 11;
    *bcntxtrigger = (localVal & ((uint32_t)0x00000400)) >> 10;
    *ac3txtrigger = (localVal & ((uint32_t)0x00000200)) >> 9;
    *ac2txtrigger = (localVal & ((uint32_t)0x00000100)) >> 8;
    *ac1txtrigger = (localVal & ((uint32_t)0x00000080)) >> 7;
    *ac0txtrigger = (localVal & ((uint32_t)0x00000040)) >> 6;
    *rdprottrigger = (localVal & ((uint32_t)0x00000020)) >> 5;
    *hccaprottrigger = (localVal & ((uint32_t)0x00000010)) >> 4;
    *ac3prottrigger = (localVal & ((uint32_t)0x00000008)) >> 3;
    *ac2prottrigger = (localVal & ((uint32_t)0x00000004)) >> 2;
    *ac1prottrigger = (localVal & ((uint32_t)0x00000002)) >> 1;
    *ac0prottrigger = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the masterTxRxIntEn field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the masterTxRxIntEn field's value will be returned.
 *
 * @return The current value of the masterTxRxIntEn field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_master_tx_rx_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the masterTxRxIntEn field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mastertxrxinten - The value to set the field to.
 */
__INLINE void nxmac_enable_master_tx_rx_int_en_setf(uint8_t mastertxrxinten)
{
    ASSERT_ERR((((uint32_t)mastertxrxinten << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)mastertxrxinten << 31));
}

/**
 * @brief Returns the current value of the bcnTxBufTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the bcnTxBufTrigger field's value will be returned.
 *
 * @return The current value of the bcnTxBufTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_bcn_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the bcnTxBufTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bcntxbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_bcn_tx_buf_trigger_setf(uint8_t bcntxbuftrigger)
{
    ASSERT_ERR((((uint32_t)bcntxbuftrigger << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)bcntxbuftrigger << 28));
}

/**
 * @brief Returns the current value of the ac3TxBufTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the ac3TxBufTrigger field's value will be returned.
 *
 * @return The current value of the ac3TxBufTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_3_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Sets the ac3TxBufTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac3txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_3_tx_buf_trigger_setf(uint8_t ac3txbuftrigger)
{
    ASSERT_ERR((((uint32_t)ac3txbuftrigger << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x08000000)) | ((uint32_t)ac3txbuftrigger << 27));
}

/**
 * @brief Returns the current value of the ac2TxBufTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the ac2TxBufTrigger field's value will be returned.
 *
 * @return The current value of the ac2TxBufTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_2_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

/**
 * @brief Sets the ac2TxBufTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac2txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_2_tx_buf_trigger_setf(uint8_t ac2txbuftrigger)
{
    ASSERT_ERR((((uint32_t)ac2txbuftrigger << 26) & ~((uint32_t)0x04000000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x04000000)) | ((uint32_t)ac2txbuftrigger << 26));
}

/**
 * @brief Returns the current value of the ac1TxBufTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the ac1TxBufTrigger field's value will be returned.
 *
 * @return The current value of the ac1TxBufTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_1_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

/**
 * @brief Sets the ac1TxBufTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac1txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_1_tx_buf_trigger_setf(uint8_t ac1txbuftrigger)
{
    ASSERT_ERR((((uint32_t)ac1txbuftrigger << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x02000000)) | ((uint32_t)ac1txbuftrigger << 25));
}

/**
 * @brief Returns the current value of the ac0TxBufTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the ac0TxBufTrigger field's value will be returned.
 *
 * @return The current value of the ac0TxBufTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_0_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the ac0TxBufTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac0txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_0_tx_buf_trigger_setf(uint8_t ac0txbuftrigger)
{
    ASSERT_ERR((((uint32_t)ac0txbuftrigger << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)ac0txbuftrigger << 24));
}

/**
 * @brief Returns the current value of the ac3BWDropTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the ac3BWDropTrigger field's value will be returned.
 *
 * @return The current value of the ac3BWDropTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_3bw_drop_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Sets the ac3BWDropTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac3bwdroptrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_3bw_drop_trigger_setf(uint8_t ac3bwdroptrigger)
{
    ASSERT_ERR((((uint32_t)ac3bwdroptrigger << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00800000)) | ((uint32_t)ac3bwdroptrigger << 23));
}

/**
 * @brief Returns the current value of the ac2BWDropTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the ac2BWDropTrigger field's value will be returned.
 *
 * @return The current value of the ac2BWDropTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_2bw_drop_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Sets the ac2BWDropTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac2bwdroptrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_2bw_drop_trigger_setf(uint8_t ac2bwdroptrigger)
{
    ASSERT_ERR((((uint32_t)ac2bwdroptrigger << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00400000)) | ((uint32_t)ac2bwdroptrigger << 22));
}

/**
 * @brief Returns the current value of the ac1BWDropTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the ac1BWDropTrigger field's value will be returned.
 *
 * @return The current value of the ac1BWDropTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_1bw_drop_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Sets the ac1BWDropTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac1bwdroptrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_1bw_drop_trigger_setf(uint8_t ac1bwdroptrigger)
{
    ASSERT_ERR((((uint32_t)ac1bwdroptrigger << 21) & ~((uint32_t)0x00200000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00200000)) | ((uint32_t)ac1bwdroptrigger << 21));
}

/**
 * @brief Returns the current value of the ac0BWDropTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the ac0BWDropTrigger field's value will be returned.
 *
 * @return The current value of the ac0BWDropTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_0bw_drop_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Sets the ac0BWDropTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac0bwdroptrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_0bw_drop_trigger_setf(uint8_t ac0bwdroptrigger)
{
    ASSERT_ERR((((uint32_t)ac0bwdroptrigger << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00100000)) | ((uint32_t)ac0bwdroptrigger << 20));
}

/**
 * @brief Returns the current value of the counterRxTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the counterRxTrigger field's value will be returned.
 *
 * @return The current value of the counterRxTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_counter_rx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Sets the counterRxTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] counterrxtrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_counter_rx_trigger_setf(uint8_t counterrxtrigger)
{
    ASSERT_ERR((((uint32_t)counterrxtrigger << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00080000)) | ((uint32_t)counterrxtrigger << 19));
}

/**
 * @brief Returns the current value of the baRxTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the baRxTrigger field's value will be returned.
 *
 * @return The current value of the baRxTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ba_rx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Sets the baRxTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] barxtrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_ba_rx_trigger_setf(uint8_t barxtrigger)
{
    ASSERT_ERR((((uint32_t)barxtrigger << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00040000)) | ((uint32_t)barxtrigger << 18));
}

/**
 * @brief Returns the current value of the timerRxTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the timerRxTrigger field's value will be returned.
 *
 * @return The current value of the timerRxTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_timer_rx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Sets the timerRxTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] timerrxtrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_timer_rx_trigger_setf(uint8_t timerrxtrigger)
{
    ASSERT_ERR((((uint32_t)timerrxtrigger << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00020000)) | ((uint32_t)timerrxtrigger << 17));
}

/**
 * @brief Returns the current value of the rxTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the rxTrigger field's value will be returned.
 *
 * @return The current value of the rxTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_rx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the rxTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxtrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_rx_trigger_setf(uint8_t rxtrigger)
{
    ASSERT_ERR((((uint32_t)rxtrigger << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)rxtrigger << 16));
}

/**
 * @brief Returns the current value of the secUserTxTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the secUserTxTrigger field's value will be returned.
 *
 * @return The current value of the secUserTxTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_sec_user_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

/**
 * @brief Sets the secUserTxTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secusertxtrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_sec_user_tx_trigger_setf(uint8_t secusertxtrigger)
{
    ASSERT_ERR((((uint32_t)secusertxtrigger << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00008000)) | ((uint32_t)secusertxtrigger << 15));
}

/**
 * @brief Returns the current value of the timerTxTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the timerTxTrigger field's value will be returned.
 *
 * @return The current value of the timerTxTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_timer_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Sets the timerTxTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] timertxtrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_timer_tx_trigger_setf(uint8_t timertxtrigger)
{
    ASSERT_ERR((((uint32_t)timertxtrigger << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00004000)) | ((uint32_t)timertxtrigger << 14));
}

/**
 * @brief Returns the current value of the txopComplete field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the txopComplete field's value will be returned.
 *
 * @return The current value of the txopComplete field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_txop_complete_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the txopComplete field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txopcomplete - The value to set the field to.
 */
__INLINE void nxmac_enable_txop_complete_setf(uint8_t txopcomplete)
{
    ASSERT_ERR((((uint32_t)txopcomplete << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)txopcomplete << 13));
}

/**
 * @brief Returns the current value of the rdTxTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the rdTxTrigger field's value will be returned.
 *
 * @return The current value of the rdTxTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_rd_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the rdTxTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rdtxtrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_rd_tx_trigger_setf(uint8_t rdtxtrigger)
{
    ASSERT_ERR((((uint32_t)rdtxtrigger << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)rdtxtrigger << 12));
}

/**
 * @brief Returns the current value of the hccaTxTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the hccaTxTrigger field's value will be returned.
 *
 * @return The current value of the hccaTxTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_hcca_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the hccaTxTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] hccatxtrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_hcca_tx_trigger_setf(uint8_t hccatxtrigger)
{
    ASSERT_ERR((((uint32_t)hccatxtrigger << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00000800)) | ((uint32_t)hccatxtrigger << 11));
}

/**
 * @brief Returns the current value of the bcnTxTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the bcnTxTrigger field's value will be returned.
 *
 * @return The current value of the bcnTxTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_bcn_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the bcnTxTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bcntxtrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_bcn_tx_trigger_setf(uint8_t bcntxtrigger)
{
    ASSERT_ERR((((uint32_t)bcntxtrigger << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00000400)) | ((uint32_t)bcntxtrigger << 10));
}

/**
 * @brief Returns the current value of the ac3TxTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the ac3TxTrigger field's value will be returned.
 *
 * @return The current value of the ac3TxTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_3_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the ac3TxTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac3txtrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_3_tx_trigger_setf(uint8_t ac3txtrigger)
{
    ASSERT_ERR((((uint32_t)ac3txtrigger << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)ac3txtrigger << 9));
}

/**
 * @brief Returns the current value of the ac2TxTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the ac2TxTrigger field's value will be returned.
 *
 * @return The current value of the ac2TxTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_2_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the ac2TxTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac2txtrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_2_tx_trigger_setf(uint8_t ac2txtrigger)
{
    ASSERT_ERR((((uint32_t)ac2txtrigger << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)ac2txtrigger << 8));
}

/**
 * @brief Returns the current value of the ac1TxTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the ac1TxTrigger field's value will be returned.
 *
 * @return The current value of the ac1TxTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_1_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the ac1TxTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac1txtrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_1_tx_trigger_setf(uint8_t ac1txtrigger)
{
    ASSERT_ERR((((uint32_t)ac1txtrigger << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)ac1txtrigger << 7));
}

/**
 * @brief Returns the current value of the ac0TxTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the ac0TxTrigger field's value will be returned.
 *
 * @return The current value of the ac0TxTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_0_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the ac0TxTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac0txtrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_0_tx_trigger_setf(uint8_t ac0txtrigger)
{
    ASSERT_ERR((((uint32_t)ac0txtrigger << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)ac0txtrigger << 6));
}

/**
 * @brief Returns the current value of the rdProtTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the rdProtTrigger field's value will be returned.
 *
 * @return The current value of the rdProtTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_rd_prot_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the rdProtTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rdprottrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_rd_prot_trigger_setf(uint8_t rdprottrigger)
{
    ASSERT_ERR((((uint32_t)rdprottrigger << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)rdprottrigger << 5));
}

/**
 * @brief Returns the current value of the hccaProtTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the hccaProtTrigger field's value will be returned.
 *
 * @return The current value of the hccaProtTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_hcca_prot_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the hccaProtTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] hccaprottrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_hcca_prot_trigger_setf(uint8_t hccaprottrigger)
{
    ASSERT_ERR((((uint32_t)hccaprottrigger << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)hccaprottrigger << 4));
}

/**
 * @brief Returns the current value of the ac3ProtTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the ac3ProtTrigger field's value will be returned.
 *
 * @return The current value of the ac3ProtTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_3_prot_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the ac3ProtTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac3prottrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_3_prot_trigger_setf(uint8_t ac3prottrigger)
{
    ASSERT_ERR((((uint32_t)ac3prottrigger << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)ac3prottrigger << 3));
}

/**
 * @brief Returns the current value of the ac2ProtTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the ac2ProtTrigger field's value will be returned.
 *
 * @return The current value of the ac2ProtTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_2_prot_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the ac2ProtTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac2prottrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_2_prot_trigger_setf(uint8_t ac2prottrigger)
{
    ASSERT_ERR((((uint32_t)ac2prottrigger << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)ac2prottrigger << 2));
}

/**
 * @brief Returns the current value of the ac1ProtTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the ac1ProtTrigger field's value will be returned.
 *
 * @return The current value of the ac1ProtTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_1_prot_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the ac1ProtTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac1prottrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_1_prot_trigger_setf(uint8_t ac1prottrigger)
{
    ASSERT_ERR((((uint32_t)ac1prottrigger << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)ac1prottrigger << 1));
}

/**
 * @brief Returns the current value of the ac0ProtTrigger field in the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read and the ac0ProtTrigger field's value will be returned.
 *
 * @return The current value of the ac0ProtTrigger field in the TX_RX_INT_ENABLE register.
 */
__INLINE uint8_t nxmac_enable_ac_0_prot_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the ac0ProtTrigger field of the TX_RX_INT_ENABLE register.
 *
 * The TX_RX_INT_ENABLE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac0prottrigger - The value to set the field to.
 */
__INLINE void nxmac_enable_ac_0_prot_trigger_setf(uint8_t ac0prottrigger)
{
    ASSERT_ERR((((uint32_t)ac0prottrigger << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_TX_RX_INT_ENABLE_ADDR, (REG_PL_RD(NXMAC_TX_RX_INT_ENABLE_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)ac0prottrigger << 0));
}

/// @}

/**
 * @name TIMERS_INT_EVENT_SET register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     09           absTimers9   0
 *     08           absTimers8   0
 *     07           absTimers7   0
 *     06           absTimers6   0
 *     05           absTimers5   0
 *     04           absTimers4   0
 *     03           absTimers3   0
 *     02           absTimers2   0
 *     01           absTimers1   0
 *     00           absTimers0   0
 * </pre>
 *
 * @{
 */

/// Address of the TIMERS_INT_EVENT_SET register
#define NXMAC_TIMERS_INT_EVENT_SET_ADDR   0xC0008084
/// Offset of the TIMERS_INT_EVENT_SET register from the base address
#define NXMAC_TIMERS_INT_EVENT_SET_OFFSET 0x00000084
/// Index of the TIMERS_INT_EVENT_SET register
#define NXMAC_TIMERS_INT_EVENT_SET_INDEX  0x00000021
/// Reset value of the TIMERS_INT_EVENT_SET register
#define NXMAC_TIMERS_INT_EVENT_SET_RESET  0x00000000

/**
 * @brief Returns the current value of the TIMERS_INT_EVENT_SET register.
 * The TIMERS_INT_EVENT_SET register will be read and its value returned.
 * @return The current value of the TIMERS_INT_EVENT_SET register.
 */
__INLINE uint32_t nxmac_timers_int_event_get(void)
{
    return REG_PL_RD(NXMAC_TIMERS_INT_EVENT_SET_ADDR);
}

/**
 * @brief Sets the TIMERS_INT_EVENT_SET register to a value.
 * The TIMERS_INT_EVENT_SET register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_timers_int_event_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_SET_ADDR, value);
}

// field definitions
/// ABS_TIMERS_9 field bit
#define NXMAC_ABS_TIMERS_9_BIT    ((uint32_t)0x00000200)
/// ABS_TIMERS_9 field position
#define NXMAC_ABS_TIMERS_9_POS    9
/// ABS_TIMERS_8 field bit
#define NXMAC_ABS_TIMERS_8_BIT    ((uint32_t)0x00000100)
/// ABS_TIMERS_8 field position
#define NXMAC_ABS_TIMERS_8_POS    8
/// ABS_TIMERS_7 field bit
#define NXMAC_ABS_TIMERS_7_BIT    ((uint32_t)0x00000080)
/// ABS_TIMERS_7 field position
#define NXMAC_ABS_TIMERS_7_POS    7
/// ABS_TIMERS_6 field bit
#define NXMAC_ABS_TIMERS_6_BIT    ((uint32_t)0x00000040)
/// ABS_TIMERS_6 field position
#define NXMAC_ABS_TIMERS_6_POS    6
/// ABS_TIMERS_5 field bit
#define NXMAC_ABS_TIMERS_5_BIT    ((uint32_t)0x00000020)
/// ABS_TIMERS_5 field position
#define NXMAC_ABS_TIMERS_5_POS    5
/// ABS_TIMERS_4 field bit
#define NXMAC_ABS_TIMERS_4_BIT    ((uint32_t)0x00000010)
/// ABS_TIMERS_4 field position
#define NXMAC_ABS_TIMERS_4_POS    4
/// ABS_TIMERS_3 field bit
#define NXMAC_ABS_TIMERS_3_BIT    ((uint32_t)0x00000008)
/// ABS_TIMERS_3 field position
#define NXMAC_ABS_TIMERS_3_POS    3
/// ABS_TIMERS_2 field bit
#define NXMAC_ABS_TIMERS_2_BIT    ((uint32_t)0x00000004)
/// ABS_TIMERS_2 field position
#define NXMAC_ABS_TIMERS_2_POS    2
/// ABS_TIMERS_1 field bit
#define NXMAC_ABS_TIMERS_1_BIT    ((uint32_t)0x00000002)
/// ABS_TIMERS_1 field position
#define NXMAC_ABS_TIMERS_1_POS    1
/// ABS_TIMERS_0 field bit
#define NXMAC_ABS_TIMERS_0_BIT    ((uint32_t)0x00000001)
/// ABS_TIMERS_0 field position
#define NXMAC_ABS_TIMERS_0_POS    0

/// ABS_TIMERS_9 field reset value
#define NXMAC_ABS_TIMERS_9_RST    0x0
/// ABS_TIMERS_8 field reset value
#define NXMAC_ABS_TIMERS_8_RST    0x0
/// ABS_TIMERS_7 field reset value
#define NXMAC_ABS_TIMERS_7_RST    0x0
/// ABS_TIMERS_6 field reset value
#define NXMAC_ABS_TIMERS_6_RST    0x0
/// ABS_TIMERS_5 field reset value
#define NXMAC_ABS_TIMERS_5_RST    0x0
/// ABS_TIMERS_4 field reset value
#define NXMAC_ABS_TIMERS_4_RST    0x0
/// ABS_TIMERS_3 field reset value
#define NXMAC_ABS_TIMERS_3_RST    0x0
/// ABS_TIMERS_2 field reset value
#define NXMAC_ABS_TIMERS_2_RST    0x0
/// ABS_TIMERS_1 field reset value
#define NXMAC_ABS_TIMERS_1_RST    0x0
/// ABS_TIMERS_0 field reset value
#define NXMAC_ABS_TIMERS_0_RST    0x0

/**
 * @brief Constructs a value for the TIMERS_INT_EVENT_SET register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] abstimers9 - The value to use for the absTimers9 field.
 * @param[in] abstimers8 - The value to use for the absTimers8 field.
 * @param[in] abstimers7 - The value to use for the absTimers7 field.
 * @param[in] abstimers6 - The value to use for the absTimers6 field.
 * @param[in] abstimers5 - The value to use for the absTimers5 field.
 * @param[in] abstimers4 - The value to use for the absTimers4 field.
 * @param[in] abstimers3 - The value to use for the absTimers3 field.
 * @param[in] abstimers2 - The value to use for the absTimers2 field.
 * @param[in] abstimers1 - The value to use for the absTimers1 field.
 * @param[in] abstimers0 - The value to use for the absTimers0 field.
 */
__INLINE void nxmac_timers_int_event_set_pack(uint8_t abstimers9, uint8_t abstimers8, uint8_t abstimers7, uint8_t abstimers6, uint8_t abstimers5, uint8_t abstimers4, uint8_t abstimers3, uint8_t abstimers2, uint8_t abstimers1, uint8_t abstimers0)
{
    ASSERT_ERR((((uint32_t)abstimers9 << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)abstimers8 << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)abstimers7 << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)abstimers6 << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)abstimers5 << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)abstimers4 << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)abstimers3 << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)abstimers2 << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)abstimers1 << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)abstimers0 << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_SET_ADDR,  ((uint32_t)abstimers9 << 9) | ((uint32_t)abstimers8 << 8) | ((uint32_t)abstimers7 << 7) | ((uint32_t)abstimers6 << 6) | ((uint32_t)abstimers5 << 5) | ((uint32_t)abstimers4 << 4) | ((uint32_t)abstimers3 << 3) | ((uint32_t)abstimers2 << 2) | ((uint32_t)abstimers1 << 1) | ((uint32_t)abstimers0 << 0));
}

/**
 * @brief Unpacks TIMERS_INT_EVENT_SET's fields from current value of the TIMERS_INT_EVENT_SET register.
 *
 * Reads the TIMERS_INT_EVENT_SET register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] abstimers9 - Will be populated with the current value of this field from the register.
 * @param[out] abstimers8 - Will be populated with the current value of this field from the register.
 * @param[out] abstimers7 - Will be populated with the current value of this field from the register.
 * @param[out] abstimers6 - Will be populated with the current value of this field from the register.
 * @param[out] abstimers5 - Will be populated with the current value of this field from the register.
 * @param[out] abstimers4 - Will be populated with the current value of this field from the register.
 * @param[out] abstimers3 - Will be populated with the current value of this field from the register.
 * @param[out] abstimers2 - Will be populated with the current value of this field from the register.
 * @param[out] abstimers1 - Will be populated with the current value of this field from the register.
 * @param[out] abstimers0 - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_timers_int_event_unpack(uint8_t *abstimers9, uint8_t *abstimers8, uint8_t *abstimers7, uint8_t *abstimers6, uint8_t *abstimers5, uint8_t *abstimers4, uint8_t *abstimers3, uint8_t *abstimers2, uint8_t *abstimers1, uint8_t *abstimers0)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_EVENT_SET_ADDR);

    *abstimers9 = (localVal & ((uint32_t)0x00000200)) >> 9;
    *abstimers8 = (localVal & ((uint32_t)0x00000100)) >> 8;
    *abstimers7 = (localVal & ((uint32_t)0x00000080)) >> 7;
    *abstimers6 = (localVal & ((uint32_t)0x00000040)) >> 6;
    *abstimers5 = (localVal & ((uint32_t)0x00000020)) >> 5;
    *abstimers4 = (localVal & ((uint32_t)0x00000010)) >> 4;
    *abstimers3 = (localVal & ((uint32_t)0x00000008)) >> 3;
    *abstimers2 = (localVal & ((uint32_t)0x00000004)) >> 2;
    *abstimers1 = (localVal & ((uint32_t)0x00000002)) >> 1;
    *abstimers0 = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the absTimers9 field in the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read and the absTimers9 field's value will be returned.
 *
 * @return The current value of the absTimers9 field in the TIMERS_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_abs_timers_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the absTimers9 field of the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers9 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_9_setf(uint8_t abstimers9)
{
    ASSERT_ERR((((uint32_t)abstimers9 << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_SET_ADDR, (uint32_t)abstimers9 << 9);
}

/**
 * @brief Returns the current value of the absTimers8 field in the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read and the absTimers8 field's value will be returned.
 *
 * @return The current value of the absTimers8 field in the TIMERS_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_abs_timers_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the absTimers8 field of the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers8 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_8_setf(uint8_t abstimers8)
{
    ASSERT_ERR((((uint32_t)abstimers8 << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_SET_ADDR, (uint32_t)abstimers8 << 8);
}

/**
 * @brief Returns the current value of the absTimers7 field in the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read and the absTimers7 field's value will be returned.
 *
 * @return The current value of the absTimers7 field in the TIMERS_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_abs_timers_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the absTimers7 field of the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers7 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_7_setf(uint8_t abstimers7)
{
    ASSERT_ERR((((uint32_t)abstimers7 << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_SET_ADDR, (uint32_t)abstimers7 << 7);
}

/**
 * @brief Returns the current value of the absTimers6 field in the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read and the absTimers6 field's value will be returned.
 *
 * @return The current value of the absTimers6 field in the TIMERS_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_abs_timers_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the absTimers6 field of the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers6 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_6_setf(uint8_t abstimers6)
{
    ASSERT_ERR((((uint32_t)abstimers6 << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_SET_ADDR, (uint32_t)abstimers6 << 6);
}

/**
 * @brief Returns the current value of the absTimers5 field in the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read and the absTimers5 field's value will be returned.
 *
 * @return The current value of the absTimers5 field in the TIMERS_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_abs_timers_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the absTimers5 field of the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers5 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_5_setf(uint8_t abstimers5)
{
    ASSERT_ERR((((uint32_t)abstimers5 << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_SET_ADDR, (uint32_t)abstimers5 << 5);
}

/**
 * @brief Returns the current value of the absTimers4 field in the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read and the absTimers4 field's value will be returned.
 *
 * @return The current value of the absTimers4 field in the TIMERS_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_abs_timers_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the absTimers4 field of the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers4 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_4_setf(uint8_t abstimers4)
{
    ASSERT_ERR((((uint32_t)abstimers4 << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_SET_ADDR, (uint32_t)abstimers4 << 4);
}

/**
 * @brief Returns the current value of the absTimers3 field in the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read and the absTimers3 field's value will be returned.
 *
 * @return The current value of the absTimers3 field in the TIMERS_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_abs_timers_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the absTimers3 field of the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers3 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_3_setf(uint8_t abstimers3)
{
    ASSERT_ERR((((uint32_t)abstimers3 << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_SET_ADDR, (uint32_t)abstimers3 << 3);
}

/**
 * @brief Returns the current value of the absTimers2 field in the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read and the absTimers2 field's value will be returned.
 *
 * @return The current value of the absTimers2 field in the TIMERS_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_abs_timers_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the absTimers2 field of the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers2 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_2_setf(uint8_t abstimers2)
{
    ASSERT_ERR((((uint32_t)abstimers2 << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_SET_ADDR, (uint32_t)abstimers2 << 2);
}

/**
 * @brief Returns the current value of the absTimers1 field in the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read and the absTimers1 field's value will be returned.
 *
 * @return The current value of the absTimers1 field in the TIMERS_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_abs_timers_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the absTimers1 field of the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers1 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_1_setf(uint8_t abstimers1)
{
    ASSERT_ERR((((uint32_t)abstimers1 << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_SET_ADDR, (uint32_t)abstimers1 << 1);
}

/**
 * @brief Returns the current value of the absTimers0 field in the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read and the absTimers0 field's value will be returned.
 *
 * @return The current value of the absTimers0 field in the TIMERS_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_abs_timers_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the absTimers0 field of the TIMERS_INT_EVENT_SET register.
 *
 * The TIMERS_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers0 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_0_setf(uint8_t abstimers0)
{
    ASSERT_ERR((((uint32_t)abstimers0 << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_SET_ADDR, (uint32_t)abstimers0 << 0);
}

/// @}

/**
 * @name TIMERS_INT_EVENT_CLEAR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     09           absTimers9   0
 *     08           absTimers8   0
 *     07           absTimers7   0
 *     06           absTimers6   0
 *     05           absTimers5   0
 *     04           absTimers4   0
 *     03           absTimers3   0
 *     02           absTimers2   0
 *     01           absTimers1   0
 *     00           absTimers0   0
 * </pre>
 *
 * @{
 */

/// Address of the TIMERS_INT_EVENT_CLEAR register
#define NXMAC_TIMERS_INT_EVENT_CLEAR_ADDR   0xC0008088
/// Offset of the TIMERS_INT_EVENT_CLEAR register from the base address
#define NXMAC_TIMERS_INT_EVENT_CLEAR_OFFSET 0x00000088
/// Index of the TIMERS_INT_EVENT_CLEAR register
#define NXMAC_TIMERS_INT_EVENT_CLEAR_INDEX  0x00000022
/// Reset value of the TIMERS_INT_EVENT_CLEAR register
#define NXMAC_TIMERS_INT_EVENT_CLEAR_RESET  0x00000000

/**
 * @brief Sets the TIMERS_INT_EVENT_CLEAR register to a value.
 * The TIMERS_INT_EVENT_CLEAR register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_timers_int_event_clear(uint32_t value)
{
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_CLEAR_ADDR, value);
}

// fields defined in symmetrical set/clear register
/**
 * @brief Constructs a value for the TIMERS_INT_EVENT_CLEAR register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] abstimers9 - The value to use for the absTimers9 field.
 * @param[in] abstimers8 - The value to use for the absTimers8 field.
 * @param[in] abstimers7 - The value to use for the absTimers7 field.
 * @param[in] abstimers6 - The value to use for the absTimers6 field.
 * @param[in] abstimers5 - The value to use for the absTimers5 field.
 * @param[in] abstimers4 - The value to use for the absTimers4 field.
 * @param[in] abstimers3 - The value to use for the absTimers3 field.
 * @param[in] abstimers2 - The value to use for the absTimers2 field.
 * @param[in] abstimers1 - The value to use for the absTimers1 field.
 * @param[in] abstimers0 - The value to use for the absTimers0 field.
 */
__INLINE void nxmac_timers_int_event_clear_pack(uint8_t abstimers9, uint8_t abstimers8, uint8_t abstimers7, uint8_t abstimers6, uint8_t abstimers5, uint8_t abstimers4, uint8_t abstimers3, uint8_t abstimers2, uint8_t abstimers1, uint8_t abstimers0)
{
    ASSERT_ERR((((uint32_t)abstimers9 << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)abstimers8 << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)abstimers7 << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)abstimers6 << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)abstimers5 << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)abstimers4 << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)abstimers3 << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)abstimers2 << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)abstimers1 << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)abstimers0 << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_CLEAR_ADDR,  ((uint32_t)abstimers9 << 9) | ((uint32_t)abstimers8 << 8) | ((uint32_t)abstimers7 << 7) | ((uint32_t)abstimers6 << 6) | ((uint32_t)abstimers5 << 5) | ((uint32_t)abstimers4 << 4) | ((uint32_t)abstimers3 << 3) | ((uint32_t)abstimers2 << 2) | ((uint32_t)abstimers1 << 1) | ((uint32_t)abstimers0 << 0));
}

/**
 * @brief Sets the absTimers9 field of the TIMERS_INT_EVENT_CLEAR register.
 *
 * The TIMERS_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers9 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_9_clearf(uint8_t abstimers9)
{
    ASSERT_ERR((((uint32_t)abstimers9 << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_CLEAR_ADDR, (uint32_t)abstimers9 << 9);
}

/**
 * @brief Sets the absTimers8 field of the TIMERS_INT_EVENT_CLEAR register.
 *
 * The TIMERS_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers8 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_8_clearf(uint8_t abstimers8)
{
    ASSERT_ERR((((uint32_t)abstimers8 << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_CLEAR_ADDR, (uint32_t)abstimers8 << 8);
}

/**
 * @brief Sets the absTimers7 field of the TIMERS_INT_EVENT_CLEAR register.
 *
 * The TIMERS_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers7 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_7_clearf(uint8_t abstimers7)
{
    ASSERT_ERR((((uint32_t)abstimers7 << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_CLEAR_ADDR, (uint32_t)abstimers7 << 7);
}

/**
 * @brief Sets the absTimers6 field of the TIMERS_INT_EVENT_CLEAR register.
 *
 * The TIMERS_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers6 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_6_clearf(uint8_t abstimers6)
{
    ASSERT_ERR((((uint32_t)abstimers6 << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_CLEAR_ADDR, (uint32_t)abstimers6 << 6);
}

/**
 * @brief Sets the absTimers5 field of the TIMERS_INT_EVENT_CLEAR register.
 *
 * The TIMERS_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers5 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_5_clearf(uint8_t abstimers5)
{
    ASSERT_ERR((((uint32_t)abstimers5 << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_CLEAR_ADDR, (uint32_t)abstimers5 << 5);
}

/**
 * @brief Sets the absTimers4 field of the TIMERS_INT_EVENT_CLEAR register.
 *
 * The TIMERS_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers4 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_4_clearf(uint8_t abstimers4)
{
    ASSERT_ERR((((uint32_t)abstimers4 << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_CLEAR_ADDR, (uint32_t)abstimers4 << 4);
}

/**
 * @brief Sets the absTimers3 field of the TIMERS_INT_EVENT_CLEAR register.
 *
 * The TIMERS_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers3 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_3_clearf(uint8_t abstimers3)
{
    ASSERT_ERR((((uint32_t)abstimers3 << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_CLEAR_ADDR, (uint32_t)abstimers3 << 3);
}

/**
 * @brief Sets the absTimers2 field of the TIMERS_INT_EVENT_CLEAR register.
 *
 * The TIMERS_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers2 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_2_clearf(uint8_t abstimers2)
{
    ASSERT_ERR((((uint32_t)abstimers2 << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_CLEAR_ADDR, (uint32_t)abstimers2 << 2);
}

/**
 * @brief Sets the absTimers1 field of the TIMERS_INT_EVENT_CLEAR register.
 *
 * The TIMERS_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers1 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_1_clearf(uint8_t abstimers1)
{
    ASSERT_ERR((((uint32_t)abstimers1 << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_CLEAR_ADDR, (uint32_t)abstimers1 << 1);
}

/**
 * @brief Sets the absTimers0 field of the TIMERS_INT_EVENT_CLEAR register.
 *
 * The TIMERS_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abstimers0 - The value to set the field to.
 */
__INLINE void nxmac_abs_timers_0_clearf(uint8_t abstimers0)
{
    ASSERT_ERR((((uint32_t)abstimers0 << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_EVENT_CLEAR_ADDR, (uint32_t)abstimers0 << 0);
}

/// @}

/**
 * @name TIMERS_INT_UN_MASK register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     09       maskabsTimers9   0
 *     08       maskabsTimers8   0
 *     07       maskabsTimers7   0
 *     06       maskabsTimers6   0
 *     05       maskabsTimers5   0
 *     04       maskabsTimers4   0
 *     03       maskabsTimers3   0
 *     02       maskabsTimers2   0
 *     01       maskabsTimers1   0
 *     00       maskabsTimers0   0
 * </pre>
 *
 * @{
 */

/// Address of the TIMERS_INT_UN_MASK register
#define NXMAC_TIMERS_INT_UN_MASK_ADDR   0xC000808C
/// Offset of the TIMERS_INT_UN_MASK register from the base address
#define NXMAC_TIMERS_INT_UN_MASK_OFFSET 0x0000008C
/// Index of the TIMERS_INT_UN_MASK register
#define NXMAC_TIMERS_INT_UN_MASK_INDEX  0x00000023
/// Reset value of the TIMERS_INT_UN_MASK register
#define NXMAC_TIMERS_INT_UN_MASK_RESET  0x00000000

/**
 * @brief Returns the current value of the TIMERS_INT_UN_MASK register.
 * The TIMERS_INT_UN_MASK register will be read and its value returned.
 * @return The current value of the TIMERS_INT_UN_MASK register.
 */
__INLINE uint32_t nxmac_timers_int_un_mask_get(void)
{
    return REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR);
}

/**
 * @brief Sets the TIMERS_INT_UN_MASK register to a value.
 * The TIMERS_INT_UN_MASK register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_timers_int_un_mask_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TIMERS_INT_UN_MASK_ADDR, value);
}

// field definitions
/// MASKABS_TIMERS_9 field bit
#define NXMAC_MASKABS_TIMERS_9_BIT    ((uint32_t)0x00000200)
/// MASKABS_TIMERS_9 field position
#define NXMAC_MASKABS_TIMERS_9_POS    9
/// MASKABS_TIMERS_8 field bit
#define NXMAC_MASKABS_TIMERS_8_BIT    ((uint32_t)0x00000100)
/// MASKABS_TIMERS_8 field position
#define NXMAC_MASKABS_TIMERS_8_POS    8
/// MASKABS_TIMERS_7 field bit
#define NXMAC_MASKABS_TIMERS_7_BIT    ((uint32_t)0x00000080)
/// MASKABS_TIMERS_7 field position
#define NXMAC_MASKABS_TIMERS_7_POS    7
/// MASKABS_TIMERS_6 field bit
#define NXMAC_MASKABS_TIMERS_6_BIT    ((uint32_t)0x00000040)
/// MASKABS_TIMERS_6 field position
#define NXMAC_MASKABS_TIMERS_6_POS    6
/// MASKABS_TIMERS_5 field bit
#define NXMAC_MASKABS_TIMERS_5_BIT    ((uint32_t)0x00000020)
/// MASKABS_TIMERS_5 field position
#define NXMAC_MASKABS_TIMERS_5_POS    5
/// MASKABS_TIMERS_4 field bit
#define NXMAC_MASKABS_TIMERS_4_BIT    ((uint32_t)0x00000010)
/// MASKABS_TIMERS_4 field position
#define NXMAC_MASKABS_TIMERS_4_POS    4
/// MASKABS_TIMERS_3 field bit
#define NXMAC_MASKABS_TIMERS_3_BIT    ((uint32_t)0x00000008)
/// MASKABS_TIMERS_3 field position
#define NXMAC_MASKABS_TIMERS_3_POS    3
/// MASKABS_TIMERS_2 field bit
#define NXMAC_MASKABS_TIMERS_2_BIT    ((uint32_t)0x00000004)
/// MASKABS_TIMERS_2 field position
#define NXMAC_MASKABS_TIMERS_2_POS    2
/// MASKABS_TIMERS_1 field bit
#define NXMAC_MASKABS_TIMERS_1_BIT    ((uint32_t)0x00000002)
/// MASKABS_TIMERS_1 field position
#define NXMAC_MASKABS_TIMERS_1_POS    1
/// MASKABS_TIMERS_0 field bit
#define NXMAC_MASKABS_TIMERS_0_BIT    ((uint32_t)0x00000001)
/// MASKABS_TIMERS_0 field position
#define NXMAC_MASKABS_TIMERS_0_POS    0

/// MASKABS_TIMERS_9 field reset value
#define NXMAC_MASKABS_TIMERS_9_RST    0x0
/// MASKABS_TIMERS_8 field reset value
#define NXMAC_MASKABS_TIMERS_8_RST    0x0
/// MASKABS_TIMERS_7 field reset value
#define NXMAC_MASKABS_TIMERS_7_RST    0x0
/// MASKABS_TIMERS_6 field reset value
#define NXMAC_MASKABS_TIMERS_6_RST    0x0
/// MASKABS_TIMERS_5 field reset value
#define NXMAC_MASKABS_TIMERS_5_RST    0x0
/// MASKABS_TIMERS_4 field reset value
#define NXMAC_MASKABS_TIMERS_4_RST    0x0
/// MASKABS_TIMERS_3 field reset value
#define NXMAC_MASKABS_TIMERS_3_RST    0x0
/// MASKABS_TIMERS_2 field reset value
#define NXMAC_MASKABS_TIMERS_2_RST    0x0
/// MASKABS_TIMERS_1 field reset value
#define NXMAC_MASKABS_TIMERS_1_RST    0x0
/// MASKABS_TIMERS_0 field reset value
#define NXMAC_MASKABS_TIMERS_0_RST    0x0

/**
 * @brief Constructs a value for the TIMERS_INT_UN_MASK register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] maskabstimers9 - The value to use for the maskabsTimers9 field.
 * @param[in] maskabstimers8 - The value to use for the maskabsTimers8 field.
 * @param[in] maskabstimers7 - The value to use for the maskabsTimers7 field.
 * @param[in] maskabstimers6 - The value to use for the maskabsTimers6 field.
 * @param[in] maskabstimers5 - The value to use for the maskabsTimers5 field.
 * @param[in] maskabstimers4 - The value to use for the maskabsTimers4 field.
 * @param[in] maskabstimers3 - The value to use for the maskabsTimers3 field.
 * @param[in] maskabstimers2 - The value to use for the maskabsTimers2 field.
 * @param[in] maskabstimers1 - The value to use for the maskabsTimers1 field.
 * @param[in] maskabstimers0 - The value to use for the maskabsTimers0 field.
 */
__INLINE void nxmac_timers_int_un_mask_pack(uint8_t maskabstimers9, uint8_t maskabstimers8, uint8_t maskabstimers7, uint8_t maskabstimers6, uint8_t maskabstimers5, uint8_t maskabstimers4, uint8_t maskabstimers3, uint8_t maskabstimers2, uint8_t maskabstimers1, uint8_t maskabstimers0)
{
    ASSERT_ERR((((uint32_t)maskabstimers9 << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)maskabstimers8 << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)maskabstimers7 << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)maskabstimers6 << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)maskabstimers5 << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)maskabstimers4 << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)maskabstimers3 << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)maskabstimers2 << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)maskabstimers1 << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)maskabstimers0 << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_UN_MASK_ADDR,  ((uint32_t)maskabstimers9 << 9) | ((uint32_t)maskabstimers8 << 8) | ((uint32_t)maskabstimers7 << 7) | ((uint32_t)maskabstimers6 << 6) | ((uint32_t)maskabstimers5 << 5) | ((uint32_t)maskabstimers4 << 4) | ((uint32_t)maskabstimers3 << 3) | ((uint32_t)maskabstimers2 << 2) | ((uint32_t)maskabstimers1 << 1) | ((uint32_t)maskabstimers0 << 0));
}

/**
 * @brief Unpacks TIMERS_INT_UN_MASK's fields from current value of the TIMERS_INT_UN_MASK register.
 *
 * Reads the TIMERS_INT_UN_MASK register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] maskabstimers9 - Will be populated with the current value of this field from the register.
 * @param[out] maskabstimers8 - Will be populated with the current value of this field from the register.
 * @param[out] maskabstimers7 - Will be populated with the current value of this field from the register.
 * @param[out] maskabstimers6 - Will be populated with the current value of this field from the register.
 * @param[out] maskabstimers5 - Will be populated with the current value of this field from the register.
 * @param[out] maskabstimers4 - Will be populated with the current value of this field from the register.
 * @param[out] maskabstimers3 - Will be populated with the current value of this field from the register.
 * @param[out] maskabstimers2 - Will be populated with the current value of this field from the register.
 * @param[out] maskabstimers1 - Will be populated with the current value of this field from the register.
 * @param[out] maskabstimers0 - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_timers_int_un_mask_unpack(uint8_t *maskabstimers9, uint8_t *maskabstimers8, uint8_t *maskabstimers7, uint8_t *maskabstimers6, uint8_t *maskabstimers5, uint8_t *maskabstimers4, uint8_t *maskabstimers3, uint8_t *maskabstimers2, uint8_t *maskabstimers1, uint8_t *maskabstimers0)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR);

    *maskabstimers9 = (localVal & ((uint32_t)0x00000200)) >> 9;
    *maskabstimers8 = (localVal & ((uint32_t)0x00000100)) >> 8;
    *maskabstimers7 = (localVal & ((uint32_t)0x00000080)) >> 7;
    *maskabstimers6 = (localVal & ((uint32_t)0x00000040)) >> 6;
    *maskabstimers5 = (localVal & ((uint32_t)0x00000020)) >> 5;
    *maskabstimers4 = (localVal & ((uint32_t)0x00000010)) >> 4;
    *maskabstimers3 = (localVal & ((uint32_t)0x00000008)) >> 3;
    *maskabstimers2 = (localVal & ((uint32_t)0x00000004)) >> 2;
    *maskabstimers1 = (localVal & ((uint32_t)0x00000002)) >> 1;
    *maskabstimers0 = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the maskabsTimers9 field in the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read and the maskabsTimers9 field's value will be returned.
 *
 * @return The current value of the maskabsTimers9 field in the TIMERS_INT_UN_MASK register.
 */
__INLINE uint8_t nxmac_maskabs_timers_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the maskabsTimers9 field of the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maskabstimers9 - The value to set the field to.
 */
__INLINE void nxmac_maskabs_timers_9_setf(uint8_t maskabstimers9)
{
    ASSERT_ERR((((uint32_t)maskabstimers9 << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)maskabstimers9 << 9));
}

/**
 * @brief Returns the current value of the maskabsTimers8 field in the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read and the maskabsTimers8 field's value will be returned.
 *
 * @return The current value of the maskabsTimers8 field in the TIMERS_INT_UN_MASK register.
 */
__INLINE uint8_t nxmac_maskabs_timers_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the maskabsTimers8 field of the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maskabstimers8 - The value to set the field to.
 */
__INLINE void nxmac_maskabs_timers_8_setf(uint8_t maskabstimers8)
{
    ASSERT_ERR((((uint32_t)maskabstimers8 << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)maskabstimers8 << 8));
}

/**
 * @brief Returns the current value of the maskabsTimers7 field in the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read and the maskabsTimers7 field's value will be returned.
 *
 * @return The current value of the maskabsTimers7 field in the TIMERS_INT_UN_MASK register.
 */
__INLINE uint8_t nxmac_maskabs_timers_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the maskabsTimers7 field of the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maskabstimers7 - The value to set the field to.
 */
__INLINE void nxmac_maskabs_timers_7_setf(uint8_t maskabstimers7)
{
    ASSERT_ERR((((uint32_t)maskabstimers7 << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)maskabstimers7 << 7));
}

/**
 * @brief Returns the current value of the maskabsTimers6 field in the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read and the maskabsTimers6 field's value will be returned.
 *
 * @return The current value of the maskabsTimers6 field in the TIMERS_INT_UN_MASK register.
 */
__INLINE uint8_t nxmac_maskabs_timers_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the maskabsTimers6 field of the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maskabstimers6 - The value to set the field to.
 */
__INLINE void nxmac_maskabs_timers_6_setf(uint8_t maskabstimers6)
{
    ASSERT_ERR((((uint32_t)maskabstimers6 << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)maskabstimers6 << 6));
}

/**
 * @brief Returns the current value of the maskabsTimers5 field in the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read and the maskabsTimers5 field's value will be returned.
 *
 * @return The current value of the maskabsTimers5 field in the TIMERS_INT_UN_MASK register.
 */
__INLINE uint8_t nxmac_maskabs_timers_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the maskabsTimers5 field of the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maskabstimers5 - The value to set the field to.
 */
__INLINE void nxmac_maskabs_timers_5_setf(uint8_t maskabstimers5)
{
    ASSERT_ERR((((uint32_t)maskabstimers5 << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)maskabstimers5 << 5));
}

/**
 * @brief Returns the current value of the maskabsTimers4 field in the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read and the maskabsTimers4 field's value will be returned.
 *
 * @return The current value of the maskabsTimers4 field in the TIMERS_INT_UN_MASK register.
 */
__INLINE uint8_t nxmac_maskabs_timers_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the maskabsTimers4 field of the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maskabstimers4 - The value to set the field to.
 */
__INLINE void nxmac_maskabs_timers_4_setf(uint8_t maskabstimers4)
{
    ASSERT_ERR((((uint32_t)maskabstimers4 << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)maskabstimers4 << 4));
}

/**
 * @brief Returns the current value of the maskabsTimers3 field in the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read and the maskabsTimers3 field's value will be returned.
 *
 * @return The current value of the maskabsTimers3 field in the TIMERS_INT_UN_MASK register.
 */
__INLINE uint8_t nxmac_maskabs_timers_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the maskabsTimers3 field of the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maskabstimers3 - The value to set the field to.
 */
__INLINE void nxmac_maskabs_timers_3_setf(uint8_t maskabstimers3)
{
    ASSERT_ERR((((uint32_t)maskabstimers3 << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)maskabstimers3 << 3));
}

/**
 * @brief Returns the current value of the maskabsTimers2 field in the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read and the maskabsTimers2 field's value will be returned.
 *
 * @return The current value of the maskabsTimers2 field in the TIMERS_INT_UN_MASK register.
 */
__INLINE uint8_t nxmac_maskabs_timers_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the maskabsTimers2 field of the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maskabstimers2 - The value to set the field to.
 */
__INLINE void nxmac_maskabs_timers_2_setf(uint8_t maskabstimers2)
{
    ASSERT_ERR((((uint32_t)maskabstimers2 << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)maskabstimers2 << 2));
}

/**
 * @brief Returns the current value of the maskabsTimers1 field in the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read and the maskabsTimers1 field's value will be returned.
 *
 * @return The current value of the maskabsTimers1 field in the TIMERS_INT_UN_MASK register.
 */
__INLINE uint8_t nxmac_maskabs_timers_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the maskabsTimers1 field of the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maskabstimers1 - The value to set the field to.
 */
__INLINE void nxmac_maskabs_timers_1_setf(uint8_t maskabstimers1)
{
    ASSERT_ERR((((uint32_t)maskabstimers1 << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)maskabstimers1 << 1));
}

/**
 * @brief Returns the current value of the maskabsTimers0 field in the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read and the maskabsTimers0 field's value will be returned.
 *
 * @return The current value of the maskabsTimers0 field in the TIMERS_INT_UN_MASK register.
 */
__INLINE uint8_t nxmac_maskabs_timers_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the maskabsTimers0 field of the TIMERS_INT_UN_MASK register.
 *
 * The TIMERS_INT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maskabstimers0 - The value to set the field to.
 */
__INLINE void nxmac_maskabs_timers_0_setf(uint8_t maskabstimers0)
{
    ASSERT_ERR((((uint32_t)maskabstimers0 << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_TIMERS_INT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_TIMERS_INT_UN_MASK_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)maskabstimers0 << 0));
}

/// @}

/**
 * @name TSF_LO register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00          tsfTimerLow   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TSF_LO register
#define NXMAC_TSF_LO_ADDR   0xC00080A4
/// Offset of the TSF_LO register from the base address
#define NXMAC_TSF_LO_OFFSET 0x000000A4
/// Index of the TSF_LO register
#define NXMAC_TSF_LO_INDEX  0x00000029
/// Reset value of the TSF_LO register
#define NXMAC_TSF_LO_RESET  0x00000000

/**
 * @brief Returns the current value of the TSF_LO register.
 * The TSF_LO register will be read and its value returned.
 * @return The current value of the TSF_LO register.
 */
__INLINE uint32_t nxmac_tsf_lo_get(void)
{
    return REG_PL_RD(NXMAC_TSF_LO_ADDR);
}

/**
 * @brief Sets the TSF_LO register to a value.
 * The TSF_LO register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_tsf_lo_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TSF_LO_ADDR, value);
}

// field definitions
/// TSF_TIMER_LOW field mask
#define NXMAC_TSF_TIMER_LOW_MASK   ((uint32_t)0xFFFFFFFF)
/// TSF_TIMER_LOW field LSB position
#define NXMAC_TSF_TIMER_LOW_LSB    0
/// TSF_TIMER_LOW field width
#define NXMAC_TSF_TIMER_LOW_WIDTH  ((uint32_t)0x00000020)

/// TSF_TIMER_LOW field reset value
#define NXMAC_TSF_TIMER_LOW_RST    0x0

/**
 * @brief Returns the current value of the tsfTimerLow field in the TSF_LO register.
 *
 * The TSF_LO register will be read and the tsfTimerLow field's value will be returned.
 *
 * @return The current value of the tsfTimerLow field in the TSF_LO register.
 */
__INLINE uint32_t nxmac_tsf_timer_low_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TSF_LO_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the tsfTimerLow field of the TSF_LO register.
 *
 * The TSF_LO register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tsftimerlow - The value to set the field to.
 */
__INLINE void nxmac_tsf_timer_low_setf(uint32_t tsftimerlow)
{
    ASSERT_ERR((((uint32_t)tsftimerlow << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_TSF_LO_ADDR, (uint32_t)tsftimerlow << 0);
}

/// @}

/**
 * @name TSF_HI register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00         tsfTimerHigh   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TSF_HI register
#define NXMAC_TSF_HI_ADDR   0xC00080A8
/// Offset of the TSF_HI register from the base address
#define NXMAC_TSF_HI_OFFSET 0x000000A8
/// Index of the TSF_HI register
#define NXMAC_TSF_HI_INDEX  0x0000002A
/// Reset value of the TSF_HI register
#define NXMAC_TSF_HI_RESET  0x00000000

/**
 * @brief Returns the current value of the TSF_HI register.
 * The TSF_HI register will be read and its value returned.
 * @return The current value of the TSF_HI register.
 */
__INLINE uint32_t nxmac_tsf_hi_get(void)
{
    return REG_PL_RD(NXMAC_TSF_HI_ADDR);
}

/**
 * @brief Sets the TSF_HI register to a value.
 * The TSF_HI register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_tsf_hi_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TSF_HI_ADDR, value);
}

// field definitions
/// TSF_TIMER_HIGH field mask
#define NXMAC_TSF_TIMER_HIGH_MASK   ((uint32_t)0xFFFFFFFF)
/// TSF_TIMER_HIGH field LSB position
#define NXMAC_TSF_TIMER_HIGH_LSB    0
/// TSF_TIMER_HIGH field width
#define NXMAC_TSF_TIMER_HIGH_WIDTH  ((uint32_t)0x00000020)

/// TSF_TIMER_HIGH field reset value
#define NXMAC_TSF_TIMER_HIGH_RST    0x0

/**
 * @brief Returns the current value of the tsfTimerHigh field in the TSF_HI register.
 *
 * The TSF_HI register will be read and the tsfTimerHigh field's value will be returned.
 *
 * @return The current value of the tsfTimerHigh field in the TSF_HI register.
 */
__INLINE uint32_t nxmac_tsf_timer_high_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TSF_HI_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the tsfTimerHigh field of the TSF_HI register.
 *
 * The TSF_HI register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tsftimerhigh - The value to set the field to.
 */
__INLINE void nxmac_tsf_timer_high_setf(uint32_t tsftimerhigh)
{
    ASSERT_ERR((((uint32_t)tsftimerhigh << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_TSF_HI_ADDR, (uint32_t)tsftimerhigh << 0);
}

/// @}

/**
 * @name TIME_ON_AIR_PARAM_1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:30             ppduSTBC   0x0
 *  29:28        ppduNumExtnSS   0x0
 *     27          ppduShortGI   0
 *  26:24          ppduPreType   0x0
 *  23:22               ppduBW   0x0
 *  19:00           ppduLength   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TIME_ON_AIR_PARAM_1 register
#define NXMAC_TIME_ON_AIR_PARAM_1_ADDR   0xC0008160
/// Offset of the TIME_ON_AIR_PARAM_1 register from the base address
#define NXMAC_TIME_ON_AIR_PARAM_1_OFFSET 0x00000160
/// Index of the TIME_ON_AIR_PARAM_1 register
#define NXMAC_TIME_ON_AIR_PARAM_1_INDEX  0x00000058
/// Reset value of the TIME_ON_AIR_PARAM_1 register
#define NXMAC_TIME_ON_AIR_PARAM_1_RESET  0x00000000

/**
 * @brief Returns the current value of the TIME_ON_AIR_PARAM_1 register.
 * The TIME_ON_AIR_PARAM_1 register will be read and its value returned.
 * @return The current value of the TIME_ON_AIR_PARAM_1 register.
 */
__INLINE uint32_t nxmac_time_on_air_param_1_get(void)
{
    return REG_PL_RD(NXMAC_TIME_ON_AIR_PARAM_1_ADDR);
}

/**
 * @brief Sets the TIME_ON_AIR_PARAM_1 register to a value.
 * The TIME_ON_AIR_PARAM_1 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_time_on_air_param_1_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TIME_ON_AIR_PARAM_1_ADDR, value);
}

// field definitions
/// PPDU_STBC field mask
#define NXMAC_PPDU_STBC_MASK          ((uint32_t)0xC0000000)
/// PPDU_STBC field LSB position
#define NXMAC_PPDU_STBC_LSB           30
/// PPDU_STBC field width
#define NXMAC_PPDU_STBC_WIDTH         ((uint32_t)0x00000002)
/// PPDU_NUM_EXTN_SS field mask
#define NXMAC_PPDU_NUM_EXTN_SS_MASK   ((uint32_t)0x30000000)
/// PPDU_NUM_EXTN_SS field LSB position
#define NXMAC_PPDU_NUM_EXTN_SS_LSB    28
/// PPDU_NUM_EXTN_SS field width
#define NXMAC_PPDU_NUM_EXTN_SS_WIDTH  ((uint32_t)0x00000002)
/// PPDU_SHORT_GI field bit
#define NXMAC_PPDU_SHORT_GI_BIT       ((uint32_t)0x08000000)
/// PPDU_SHORT_GI field position
#define NXMAC_PPDU_SHORT_GI_POS       27
/// PPDU_PRE_TYPE field mask
#define NXMAC_PPDU_PRE_TYPE_MASK      ((uint32_t)0x07000000)
/// PPDU_PRE_TYPE field LSB position
#define NXMAC_PPDU_PRE_TYPE_LSB       24
/// PPDU_PRE_TYPE field width
#define NXMAC_PPDU_PRE_TYPE_WIDTH     ((uint32_t)0x00000003)
/// PPDU_BW field mask
#define NXMAC_PPDU_BW_MASK            ((uint32_t)0x00C00000)
/// PPDU_BW field LSB position
#define NXMAC_PPDU_BW_LSB             22
/// PPDU_BW field width
#define NXMAC_PPDU_BW_WIDTH           ((uint32_t)0x00000002)
/// PPDU_LENGTH field mask
#define NXMAC_PPDU_LENGTH_MASK        ((uint32_t)0x000FFFFF)
/// PPDU_LENGTH field LSB position
#define NXMAC_PPDU_LENGTH_LSB         0
/// PPDU_LENGTH field width
#define NXMAC_PPDU_LENGTH_WIDTH       ((uint32_t)0x00000014)

/// PPDU_STBC field reset value
#define NXMAC_PPDU_STBC_RST           0x0
/// PPDU_NUM_EXTN_SS field reset value
#define NXMAC_PPDU_NUM_EXTN_SS_RST    0x0
/// PPDU_SHORT_GI field reset value
#define NXMAC_PPDU_SHORT_GI_RST       0x0
/// PPDU_PRE_TYPE field reset value
#define NXMAC_PPDU_PRE_TYPE_RST       0x0
/// PPDU_BW field reset value
#define NXMAC_PPDU_BW_RST             0x0
/// PPDU_LENGTH field reset value
#define NXMAC_PPDU_LENGTH_RST         0x0

/**
 * @brief Constructs a value for the TIME_ON_AIR_PARAM_1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] ppdustbc - The value to use for the ppduSTBC field.
 * @param[in] ppdunumextnss - The value to use for the ppduNumExtnSS field.
 * @param[in] ppdushortgi - The value to use for the ppduShortGI field.
 * @param[in] ppdupretype - The value to use for the ppduPreType field.
 * @param[in] ppdubw - The value to use for the ppduBW field.
 * @param[in] ppdulength - The value to use for the ppduLength field.
 */
__INLINE void nxmac_time_on_air_param_1_pack(uint8_t ppdustbc, uint8_t ppdunumextnss, uint8_t ppdushortgi, uint8_t ppdupretype, uint8_t ppdubw, uint32_t ppdulength)
{
    ASSERT_ERR((((uint32_t)ppdustbc << 30) & ~((uint32_t)0xC0000000)) == 0);
    ASSERT_ERR((((uint32_t)ppdunumextnss << 28) & ~((uint32_t)0x30000000)) == 0);
    ASSERT_ERR((((uint32_t)ppdushortgi << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)ppdupretype << 24) & ~((uint32_t)0x07000000)) == 0);
    ASSERT_ERR((((uint32_t)ppdubw << 22) & ~((uint32_t)0x00C00000)) == 0);
    ASSERT_ERR((((uint32_t)ppdulength << 0) & ~((uint32_t)0x000FFFFF)) == 0);
    REG_PL_WR(NXMAC_TIME_ON_AIR_PARAM_1_ADDR,  ((uint32_t)ppdustbc << 30) | ((uint32_t)ppdunumextnss << 28) | ((uint32_t)ppdushortgi << 27) | ((uint32_t)ppdupretype << 24) | ((uint32_t)ppdubw << 22) | ((uint32_t)ppdulength << 0));
}

/**
 * @brief Unpacks TIME_ON_AIR_PARAM_1's fields from current value of the TIME_ON_AIR_PARAM_1 register.
 *
 * Reads the TIME_ON_AIR_PARAM_1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] ppdustbc - Will be populated with the current value of this field from the register.
 * @param[out] ppdunumextnss - Will be populated with the current value of this field from the register.
 * @param[out] ppdushortgi - Will be populated with the current value of this field from the register.
 * @param[out] ppdupretype - Will be populated with the current value of this field from the register.
 * @param[out] ppdubw - Will be populated with the current value of this field from the register.
 * @param[out] ppdulength - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_time_on_air_param_1_unpack(uint8_t *ppdustbc, uint8_t *ppdunumextnss, uint8_t *ppdushortgi, uint8_t *ppdupretype, uint8_t *ppdubw, uint32_t *ppdulength)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIME_ON_AIR_PARAM_1_ADDR);

    *ppdustbc = (localVal & ((uint32_t)0xC0000000)) >> 30;
    *ppdunumextnss = (localVal & ((uint32_t)0x30000000)) >> 28;
    *ppdushortgi = (localVal & ((uint32_t)0x08000000)) >> 27;
    *ppdupretype = (localVal & ((uint32_t)0x07000000)) >> 24;
    *ppdubw = (localVal & ((uint32_t)0x00C00000)) >> 22;
    *ppdulength = (localVal & ((uint32_t)0x000FFFFF)) >> 0;
}

/**
 * @brief Returns the current value of the ppduSTBC field in the TIME_ON_AIR_PARAM_1 register.
 *
 * The TIME_ON_AIR_PARAM_1 register will be read and the ppduSTBC field's value will be returned.
 *
 * @return The current value of the ppduSTBC field in the TIME_ON_AIR_PARAM_1 register.
 */
__INLINE uint8_t nxmac_ppdu_stbc_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIME_ON_AIR_PARAM_1_ADDR);
    return ((localVal & ((uint32_t)0xC0000000)) >> 30);
}

/**
 * @brief Sets the ppduSTBC field of the TIME_ON_AIR_PARAM_1 register.
 *
 * The TIME_ON_AIR_PARAM_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ppdustbc - The value to set the field to.
 */
__INLINE void nxmac_ppdu_stbc_setf(uint8_t ppdustbc)
{
    ASSERT_ERR((((uint32_t)ppdustbc << 30) & ~((uint32_t)0xC0000000)) == 0);
    REG_PL_WR(NXMAC_TIME_ON_AIR_PARAM_1_ADDR, (REG_PL_RD(NXMAC_TIME_ON_AIR_PARAM_1_ADDR) & ~((uint32_t)0xC0000000)) | ((uint32_t)ppdustbc << 30));
}

/**
 * @brief Returns the current value of the ppduNumExtnSS field in the TIME_ON_AIR_PARAM_1 register.
 *
 * The TIME_ON_AIR_PARAM_1 register will be read and the ppduNumExtnSS field's value will be returned.
 *
 * @return The current value of the ppduNumExtnSS field in the TIME_ON_AIR_PARAM_1 register.
 */
__INLINE uint8_t nxmac_ppdu_num_extn_ss_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIME_ON_AIR_PARAM_1_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

/**
 * @brief Sets the ppduNumExtnSS field of the TIME_ON_AIR_PARAM_1 register.
 *
 * The TIME_ON_AIR_PARAM_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ppdunumextnss - The value to set the field to.
 */
__INLINE void nxmac_ppdu_num_extn_ss_setf(uint8_t ppdunumextnss)
{
    ASSERT_ERR((((uint32_t)ppdunumextnss << 28) & ~((uint32_t)0x30000000)) == 0);
    REG_PL_WR(NXMAC_TIME_ON_AIR_PARAM_1_ADDR, (REG_PL_RD(NXMAC_TIME_ON_AIR_PARAM_1_ADDR) & ~((uint32_t)0x30000000)) | ((uint32_t)ppdunumextnss << 28));
}

/**
 * @brief Returns the current value of the ppduShortGI field in the TIME_ON_AIR_PARAM_1 register.
 *
 * The TIME_ON_AIR_PARAM_1 register will be read and the ppduShortGI field's value will be returned.
 *
 * @return The current value of the ppduShortGI field in the TIME_ON_AIR_PARAM_1 register.
 */
__INLINE uint8_t nxmac_ppdu_short_gi_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIME_ON_AIR_PARAM_1_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Sets the ppduShortGI field of the TIME_ON_AIR_PARAM_1 register.
 *
 * The TIME_ON_AIR_PARAM_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ppdushortgi - The value to set the field to.
 */
__INLINE void nxmac_ppdu_short_gi_setf(uint8_t ppdushortgi)
{
    ASSERT_ERR((((uint32_t)ppdushortgi << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(NXMAC_TIME_ON_AIR_PARAM_1_ADDR, (REG_PL_RD(NXMAC_TIME_ON_AIR_PARAM_1_ADDR) & ~((uint32_t)0x08000000)) | ((uint32_t)ppdushortgi << 27));
}

/**
 * @brief Returns the current value of the ppduPreType field in the TIME_ON_AIR_PARAM_1 register.
 *
 * The TIME_ON_AIR_PARAM_1 register will be read and the ppduPreType field's value will be returned.
 *
 * @return The current value of the ppduPreType field in the TIME_ON_AIR_PARAM_1 register.
 */
__INLINE uint8_t nxmac_ppdu_pre_type_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIME_ON_AIR_PARAM_1_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

/**
 * @brief Sets the ppduPreType field of the TIME_ON_AIR_PARAM_1 register.
 *
 * The TIME_ON_AIR_PARAM_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ppdupretype - The value to set the field to.
 */
__INLINE void nxmac_ppdu_pre_type_setf(uint8_t ppdupretype)
{
    ASSERT_ERR((((uint32_t)ppdupretype << 24) & ~((uint32_t)0x07000000)) == 0);
    REG_PL_WR(NXMAC_TIME_ON_AIR_PARAM_1_ADDR, (REG_PL_RD(NXMAC_TIME_ON_AIR_PARAM_1_ADDR) & ~((uint32_t)0x07000000)) | ((uint32_t)ppdupretype << 24));
}

/**
 * @brief Returns the current value of the ppduBW field in the TIME_ON_AIR_PARAM_1 register.
 *
 * The TIME_ON_AIR_PARAM_1 register will be read and the ppduBW field's value will be returned.
 *
 * @return The current value of the ppduBW field in the TIME_ON_AIR_PARAM_1 register.
 */
__INLINE uint8_t nxmac_ppdu_bw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIME_ON_AIR_PARAM_1_ADDR);
    return ((localVal & ((uint32_t)0x00C00000)) >> 22);
}

/**
 * @brief Sets the ppduBW field of the TIME_ON_AIR_PARAM_1 register.
 *
 * The TIME_ON_AIR_PARAM_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ppdubw - The value to set the field to.
 */
__INLINE void nxmac_ppdu_bw_setf(uint8_t ppdubw)
{
    ASSERT_ERR((((uint32_t)ppdubw << 22) & ~((uint32_t)0x00C00000)) == 0);
    REG_PL_WR(NXMAC_TIME_ON_AIR_PARAM_1_ADDR, (REG_PL_RD(NXMAC_TIME_ON_AIR_PARAM_1_ADDR) & ~((uint32_t)0x00C00000)) | ((uint32_t)ppdubw << 22));
}

/**
 * @brief Returns the current value of the ppduLength field in the TIME_ON_AIR_PARAM_1 register.
 *
 * The TIME_ON_AIR_PARAM_1 register will be read and the ppduLength field's value will be returned.
 *
 * @return The current value of the ppduLength field in the TIME_ON_AIR_PARAM_1 register.
 */
__INLINE uint32_t nxmac_ppdu_length_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIME_ON_AIR_PARAM_1_ADDR);
    return ((localVal & ((uint32_t)0x000FFFFF)) >> 0);
}

/**
 * @brief Sets the ppduLength field of the TIME_ON_AIR_PARAM_1 register.
 *
 * The TIME_ON_AIR_PARAM_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ppdulength - The value to set the field to.
 */
__INLINE void nxmac_ppdu_length_setf(uint32_t ppdulength)
{
    ASSERT_ERR((((uint32_t)ppdulength << 0) & ~((uint32_t)0x000FFFFF)) == 0);
    REG_PL_WR(NXMAC_TIME_ON_AIR_PARAM_1_ADDR, (REG_PL_RD(NXMAC_TIME_ON_AIR_PARAM_1_ADDR) & ~((uint32_t)0x000FFFFF)) | ((uint32_t)ppdulength << 0));
}

/// @}

/**
 * @name TIME_ON_AIR_PARAM_2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  06:00         ppduMCSIndex   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TIME_ON_AIR_PARAM_2 register
#define NXMAC_TIME_ON_AIR_PARAM_2_ADDR   0xC0008164
/// Offset of the TIME_ON_AIR_PARAM_2 register from the base address
#define NXMAC_TIME_ON_AIR_PARAM_2_OFFSET 0x00000164
/// Index of the TIME_ON_AIR_PARAM_2 register
#define NXMAC_TIME_ON_AIR_PARAM_2_INDEX  0x00000059
/// Reset value of the TIME_ON_AIR_PARAM_2 register
#define NXMAC_TIME_ON_AIR_PARAM_2_RESET  0x00000000

/**
 * @brief Returns the current value of the TIME_ON_AIR_PARAM_2 register.
 * The TIME_ON_AIR_PARAM_2 register will be read and its value returned.
 * @return The current value of the TIME_ON_AIR_PARAM_2 register.
 */
__INLINE uint32_t nxmac_time_on_air_param_2_get(void)
{
    return REG_PL_RD(NXMAC_TIME_ON_AIR_PARAM_2_ADDR);
}

/**
 * @brief Sets the TIME_ON_AIR_PARAM_2 register to a value.
 * The TIME_ON_AIR_PARAM_2 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_time_on_air_param_2_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TIME_ON_AIR_PARAM_2_ADDR, value);
}

// field definitions
/// PPDU_MCS_INDEX field mask
#define NXMAC_PPDU_MCS_INDEX_MASK   ((uint32_t)0x0000007F)
/// PPDU_MCS_INDEX field LSB position
#define NXMAC_PPDU_MCS_INDEX_LSB    0
/// PPDU_MCS_INDEX field width
#define NXMAC_PPDU_MCS_INDEX_WIDTH  ((uint32_t)0x00000007)

/// PPDU_MCS_INDEX field reset value
#define NXMAC_PPDU_MCS_INDEX_RST    0x0

/**
 * @brief Returns the current value of the ppduMCSIndex field in the TIME_ON_AIR_PARAM_2 register.
 *
 * The TIME_ON_AIR_PARAM_2 register will be read and the ppduMCSIndex field's value will be returned.
 *
 * @return The current value of the ppduMCSIndex field in the TIME_ON_AIR_PARAM_2 register.
 */
__INLINE uint8_t nxmac_ppdu_mcs_index_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIME_ON_AIR_PARAM_2_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000007F)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the ppduMCSIndex field of the TIME_ON_AIR_PARAM_2 register.
 *
 * The TIME_ON_AIR_PARAM_2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ppdumcsindex - The value to set the field to.
 */
__INLINE void nxmac_ppdu_mcs_index_setf(uint8_t ppdumcsindex)
{
    ASSERT_ERR((((uint32_t)ppdumcsindex << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(NXMAC_TIME_ON_AIR_PARAM_2_ADDR, (uint32_t)ppdumcsindex << 0);
}

/// @}

/**
 * @name TIME_ON_AIR_VALUE register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31      computeDuration   0
 *     30       timeOnAirValid   0
 *  15:00            timeOnAir   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TIME_ON_AIR_VALUE register
#define NXMAC_TIME_ON_AIR_VALUE_ADDR   0xC0008168
/// Offset of the TIME_ON_AIR_VALUE register from the base address
#define NXMAC_TIME_ON_AIR_VALUE_OFFSET 0x00000168
/// Index of the TIME_ON_AIR_VALUE register
#define NXMAC_TIME_ON_AIR_VALUE_INDEX  0x0000005A
/// Reset value of the TIME_ON_AIR_VALUE register
#define NXMAC_TIME_ON_AIR_VALUE_RESET  0x00000000

/**
 * @brief Returns the current value of the TIME_ON_AIR_VALUE register.
 * The TIME_ON_AIR_VALUE register will be read and its value returned.
 * @return The current value of the TIME_ON_AIR_VALUE register.
 */
__INLINE uint32_t nxmac_time_on_air_value_get(void)
{
    return REG_PL_RD(NXMAC_TIME_ON_AIR_VALUE_ADDR);
}

/**
 * @brief Sets the TIME_ON_AIR_VALUE register to a value.
 * The TIME_ON_AIR_VALUE register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_time_on_air_value_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TIME_ON_AIR_VALUE_ADDR, value);
}

// field definitions
/// COMPUTE_DURATION field bit
#define NXMAC_COMPUTE_DURATION_BIT     ((uint32_t)0x80000000)
/// COMPUTE_DURATION field position
#define NXMAC_COMPUTE_DURATION_POS     31
/// TIME_ON_AIR_VALID field bit
#define NXMAC_TIME_ON_AIR_VALID_BIT    ((uint32_t)0x40000000)
/// TIME_ON_AIR_VALID field position
#define NXMAC_TIME_ON_AIR_VALID_POS    30
/// TIME_ON_AIR field mask
#define NXMAC_TIME_ON_AIR_MASK         ((uint32_t)0x0000FFFF)
/// TIME_ON_AIR field LSB position
#define NXMAC_TIME_ON_AIR_LSB          0
/// TIME_ON_AIR field width
#define NXMAC_TIME_ON_AIR_WIDTH        ((uint32_t)0x00000010)

/// COMPUTE_DURATION field reset value
#define NXMAC_COMPUTE_DURATION_RST     0x0
/// TIME_ON_AIR_VALID field reset value
#define NXMAC_TIME_ON_AIR_VALID_RST    0x0
/// TIME_ON_AIR field reset value
#define NXMAC_TIME_ON_AIR_RST          0x0

/**
 * @brief Unpacks TIME_ON_AIR_VALUE's fields from current value of the TIME_ON_AIR_VALUE register.
 *
 * Reads the TIME_ON_AIR_VALUE register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] computeduration - Will be populated with the current value of this field from the register.
 * @param[out] timeonairvalid - Will be populated with the current value of this field from the register.
 * @param[out] timeonair - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_time_on_air_value_unpack(uint8_t *computeduration, uint8_t *timeonairvalid, uint16_t *timeonair)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIME_ON_AIR_VALUE_ADDR);

    *computeduration = (localVal & ((uint32_t)0x80000000)) >> 31;
    *timeonairvalid = (localVal & ((uint32_t)0x40000000)) >> 30;
    *timeonair = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the computeDuration field in the TIME_ON_AIR_VALUE register.
 *
 * The TIME_ON_AIR_VALUE register will be read and the computeDuration field's value will be returned.
 *
 * @return The current value of the computeDuration field in the TIME_ON_AIR_VALUE register.
 */
__INLINE uint8_t nxmac_compute_duration_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIME_ON_AIR_VALUE_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the computeDuration field of the TIME_ON_AIR_VALUE register.
 *
 * The TIME_ON_AIR_VALUE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] computeduration - The value to set the field to.
 */
__INLINE void nxmac_compute_duration_setf(uint8_t computeduration)
{
    ASSERT_ERR((((uint32_t)computeduration << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(NXMAC_TIME_ON_AIR_VALUE_ADDR, (uint32_t)computeduration << 31);
}

/**
 * @brief Returns the current value of the timeOnAirValid field in the TIME_ON_AIR_VALUE register.
 *
 * The TIME_ON_AIR_VALUE register will be read and the timeOnAirValid field's value will be returned.
 *
 * @return The current value of the timeOnAirValid field in the TIME_ON_AIR_VALUE register.
 */
__INLINE uint8_t nxmac_time_on_air_valid_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIME_ON_AIR_VALUE_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

/**
 * @brief Returns the current value of the timeOnAir field in the TIME_ON_AIR_VALUE register.
 *
 * The TIME_ON_AIR_VALUE register will be read and the timeOnAir field's value will be returned.
 *
 * @return The current value of the timeOnAir field in the TIME_ON_AIR_VALUE register.
 */
__INLINE uint16_t nxmac_time_on_air_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIME_ON_AIR_VALUE_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/// @}

/**
 * @name DMA_CNTRL_SET register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     27     rxPayloadNewHead   0
 *     26      rxHeaderNewHead   0
 *     25     rxPayloadNewTail   0
 *     24      rxHeaderNewTail   0
 *     19     haltAC3AfterTXOP   0
 *     18     haltAC2AfterTXOP   0
 *     17     haltAC1AfterTXOP   0
 *     16     haltAC0AfterTXOP   0
 *     15     haltBcnAfterTXOP   0
 *     12         txAC3NewHead   0
 *     11         txAC2NewHead   0
 *     10         txAC1NewHead   0
 *     09         txAC0NewHead   0
 *     08         txBcnNewHead   0
 *     04         txAC3NewTail   0
 *     03         txAC2NewTail   0
 *     02         txAC1NewTail   0
 *     01         txAC0NewTail   0
 *     00         txBcnNewTail   0
 * </pre>
 *
 * @{
 */

/// Address of the DMA_CNTRL_SET register
#define NXMAC_DMA_CNTRL_SET_ADDR   0xC0008180
/// Offset of the DMA_CNTRL_SET register from the base address
#define NXMAC_DMA_CNTRL_SET_OFFSET 0x00000180
/// Index of the DMA_CNTRL_SET register
#define NXMAC_DMA_CNTRL_SET_INDEX  0x00000060
/// Reset value of the DMA_CNTRL_SET register
#define NXMAC_DMA_CNTRL_SET_RESET  0x00000000

/**
 * @brief Returns the current value of the DMA_CNTRL_SET register.
 * The DMA_CNTRL_SET register will be read and its value returned.
 * @return The current value of the DMA_CNTRL_SET register.
 */
__INLINE uint32_t nxmac_dma_cntrl_get(void)
{
    return REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
}

/**
 * @brief Sets the DMA_CNTRL_SET register to a value.
 * The DMA_CNTRL_SET register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_dma_cntrl_set(uint32_t value)
{
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, value);
}

// field definitions
/// RX_PAYLOAD_NEW_HEAD field bit
#define NXMAC_RX_PAYLOAD_NEW_HEAD_BIT     ((uint32_t)0x08000000)
/// RX_PAYLOAD_NEW_HEAD field position
#define NXMAC_RX_PAYLOAD_NEW_HEAD_POS     27
/// RX_HEADER_NEW_HEAD field bit
#define NXMAC_RX_HEADER_NEW_HEAD_BIT      ((uint32_t)0x04000000)
/// RX_HEADER_NEW_HEAD field position
#define NXMAC_RX_HEADER_NEW_HEAD_POS      26
/// RX_PAYLOAD_NEW_TAIL field bit
#define NXMAC_RX_PAYLOAD_NEW_TAIL_BIT     ((uint32_t)0x02000000)
/// RX_PAYLOAD_NEW_TAIL field position
#define NXMAC_RX_PAYLOAD_NEW_TAIL_POS     25
/// RX_HEADER_NEW_TAIL field bit
#define NXMAC_RX_HEADER_NEW_TAIL_BIT      ((uint32_t)0x01000000)
/// RX_HEADER_NEW_TAIL field position
#define NXMAC_RX_HEADER_NEW_TAIL_POS      24
/// HALT_AC_3_AFTER_TXOP field bit
#define NXMAC_HALT_AC_3_AFTER_TXOP_BIT    ((uint32_t)0x00080000)
/// HALT_AC_3_AFTER_TXOP field position
#define NXMAC_HALT_AC_3_AFTER_TXOP_POS    19
/// HALT_AC_2_AFTER_TXOP field bit
#define NXMAC_HALT_AC_2_AFTER_TXOP_BIT    ((uint32_t)0x00040000)
/// HALT_AC_2_AFTER_TXOP field position
#define NXMAC_HALT_AC_2_AFTER_TXOP_POS    18
/// HALT_AC_1_AFTER_TXOP field bit
#define NXMAC_HALT_AC_1_AFTER_TXOP_BIT    ((uint32_t)0x00020000)
/// HALT_AC_1_AFTER_TXOP field position
#define NXMAC_HALT_AC_1_AFTER_TXOP_POS    17
/// HALT_AC_0_AFTER_TXOP field bit
#define NXMAC_HALT_AC_0_AFTER_TXOP_BIT    ((uint32_t)0x00010000)
/// HALT_AC_0_AFTER_TXOP field position
#define NXMAC_HALT_AC_0_AFTER_TXOP_POS    16
/// HALT_BCN_AFTER_TXOP field bit
#define NXMAC_HALT_BCN_AFTER_TXOP_BIT     ((uint32_t)0x00008000)
/// HALT_BCN_AFTER_TXOP field position
#define NXMAC_HALT_BCN_AFTER_TXOP_POS     15
/// TX_AC_3_NEW_HEAD field bit
#define NXMAC_TX_AC_3_NEW_HEAD_BIT        ((uint32_t)0x00001000)
/// TX_AC_3_NEW_HEAD field position
#define NXMAC_TX_AC_3_NEW_HEAD_POS        12
/// TX_AC_2_NEW_HEAD field bit
#define NXMAC_TX_AC_2_NEW_HEAD_BIT        ((uint32_t)0x00000800)
/// TX_AC_2_NEW_HEAD field position
#define NXMAC_TX_AC_2_NEW_HEAD_POS        11
/// TX_AC_1_NEW_HEAD field bit
#define NXMAC_TX_AC_1_NEW_HEAD_BIT        ((uint32_t)0x00000400)
/// TX_AC_1_NEW_HEAD field position
#define NXMAC_TX_AC_1_NEW_HEAD_POS        10
/// TX_AC_0_NEW_HEAD field bit
#define NXMAC_TX_AC_0_NEW_HEAD_BIT        ((uint32_t)0x00000200)
/// TX_AC_0_NEW_HEAD field position
#define NXMAC_TX_AC_0_NEW_HEAD_POS        9
/// TX_BCN_NEW_HEAD field bit
#define NXMAC_TX_BCN_NEW_HEAD_BIT         ((uint32_t)0x00000100)
/// TX_BCN_NEW_HEAD field position
#define NXMAC_TX_BCN_NEW_HEAD_POS         8
/// TX_AC_3_NEW_TAIL field bit
#define NXMAC_TX_AC_3_NEW_TAIL_BIT        ((uint32_t)0x00000010)
/// TX_AC_3_NEW_TAIL field position
#define NXMAC_TX_AC_3_NEW_TAIL_POS        4
/// TX_AC_2_NEW_TAIL field bit
#define NXMAC_TX_AC_2_NEW_TAIL_BIT        ((uint32_t)0x00000008)
/// TX_AC_2_NEW_TAIL field position
#define NXMAC_TX_AC_2_NEW_TAIL_POS        3
/// TX_AC_1_NEW_TAIL field bit
#define NXMAC_TX_AC_1_NEW_TAIL_BIT        ((uint32_t)0x00000004)
/// TX_AC_1_NEW_TAIL field position
#define NXMAC_TX_AC_1_NEW_TAIL_POS        2
/// TX_AC_0_NEW_TAIL field bit
#define NXMAC_TX_AC_0_NEW_TAIL_BIT        ((uint32_t)0x00000002)
/// TX_AC_0_NEW_TAIL field position
#define NXMAC_TX_AC_0_NEW_TAIL_POS        1
/// TX_BCN_NEW_TAIL field bit
#define NXMAC_TX_BCN_NEW_TAIL_BIT         ((uint32_t)0x00000001)
/// TX_BCN_NEW_TAIL field position
#define NXMAC_TX_BCN_NEW_TAIL_POS         0

/// RX_PAYLOAD_NEW_HEAD field reset value
#define NXMAC_RX_PAYLOAD_NEW_HEAD_RST     0x0
/// RX_HEADER_NEW_HEAD field reset value
#define NXMAC_RX_HEADER_NEW_HEAD_RST      0x0
/// RX_PAYLOAD_NEW_TAIL field reset value
#define NXMAC_RX_PAYLOAD_NEW_TAIL_RST     0x0
/// RX_HEADER_NEW_TAIL field reset value
#define NXMAC_RX_HEADER_NEW_TAIL_RST      0x0
/// HALT_AC_3_AFTER_TXOP field reset value
#define NXMAC_HALT_AC_3_AFTER_TXOP_RST    0x0
/// HALT_AC_2_AFTER_TXOP field reset value
#define NXMAC_HALT_AC_2_AFTER_TXOP_RST    0x0
/// HALT_AC_1_AFTER_TXOP field reset value
#define NXMAC_HALT_AC_1_AFTER_TXOP_RST    0x0
/// HALT_AC_0_AFTER_TXOP field reset value
#define NXMAC_HALT_AC_0_AFTER_TXOP_RST    0x0
/// HALT_BCN_AFTER_TXOP field reset value
#define NXMAC_HALT_BCN_AFTER_TXOP_RST     0x0
/// TX_AC_3_NEW_HEAD field reset value
#define NXMAC_TX_AC_3_NEW_HEAD_RST        0x0
/// TX_AC_2_NEW_HEAD field reset value
#define NXMAC_TX_AC_2_NEW_HEAD_RST        0x0
/// TX_AC_1_NEW_HEAD field reset value
#define NXMAC_TX_AC_1_NEW_HEAD_RST        0x0
/// TX_AC_0_NEW_HEAD field reset value
#define NXMAC_TX_AC_0_NEW_HEAD_RST        0x0
/// TX_BCN_NEW_HEAD field reset value
#define NXMAC_TX_BCN_NEW_HEAD_RST         0x0
/// TX_AC_3_NEW_TAIL field reset value
#define NXMAC_TX_AC_3_NEW_TAIL_RST        0x0
/// TX_AC_2_NEW_TAIL field reset value
#define NXMAC_TX_AC_2_NEW_TAIL_RST        0x0
/// TX_AC_1_NEW_TAIL field reset value
#define NXMAC_TX_AC_1_NEW_TAIL_RST        0x0
/// TX_AC_0_NEW_TAIL field reset value
#define NXMAC_TX_AC_0_NEW_TAIL_RST        0x0
/// TX_BCN_NEW_TAIL field reset value
#define NXMAC_TX_BCN_NEW_TAIL_RST         0x0

/**
 * @brief Constructs a value for the DMA_CNTRL_SET register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxpayloadnewhead - The value to use for the rxPayloadNewHead field.
 * @param[in] rxheadernewhead - The value to use for the rxHeaderNewHead field.
 * @param[in] rxpayloadnewtail - The value to use for the rxPayloadNewTail field.
 * @param[in] rxheadernewtail - The value to use for the rxHeaderNewTail field.
 * @param[in] haltac3aftertxop - The value to use for the haltAC3AfterTXOP field.
 * @param[in] haltac2aftertxop - The value to use for the haltAC2AfterTXOP field.
 * @param[in] haltac1aftertxop - The value to use for the haltAC1AfterTXOP field.
 * @param[in] haltac0aftertxop - The value to use for the haltAC0AfterTXOP field.
 * @param[in] haltbcnaftertxop - The value to use for the haltBcnAfterTXOP field.
 * @param[in] txac3newhead - The value to use for the txAC3NewHead field.
 * @param[in] txac2newhead - The value to use for the txAC2NewHead field.
 * @param[in] txac1newhead - The value to use for the txAC1NewHead field.
 * @param[in] txac0newhead - The value to use for the txAC0NewHead field.
 * @param[in] txbcnnewhead - The value to use for the txBcnNewHead field.
 * @param[in] txac3newtail - The value to use for the txAC3NewTail field.
 * @param[in] txac2newtail - The value to use for the txAC2NewTail field.
 * @param[in] txac1newtail - The value to use for the txAC1NewTail field.
 * @param[in] txac0newtail - The value to use for the txAC0NewTail field.
 * @param[in] txbcnnewtail - The value to use for the txBcnNewTail field.
 */
__INLINE void nxmac_dma_cntrl_set_pack(uint8_t rxpayloadnewhead, uint8_t rxheadernewhead, uint8_t rxpayloadnewtail, uint8_t rxheadernewtail, uint8_t haltac3aftertxop, uint8_t haltac2aftertxop, uint8_t haltac1aftertxop, uint8_t haltac0aftertxop, uint8_t haltbcnaftertxop, uint8_t txac3newhead, uint8_t txac2newhead, uint8_t txac1newhead, uint8_t txac0newhead, uint8_t txbcnnewhead, uint8_t txac3newtail, uint8_t txac2newtail, uint8_t txac1newtail, uint8_t txac0newtail, uint8_t txbcnnewtail)
{
    ASSERT_ERR((((uint32_t)rxpayloadnewhead << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)rxheadernewhead << 26) & ~((uint32_t)0x04000000)) == 0);
    ASSERT_ERR((((uint32_t)rxpayloadnewtail << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)rxheadernewtail << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)haltac3aftertxop << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)haltac2aftertxop << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)haltac1aftertxop << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)haltac0aftertxop << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)haltbcnaftertxop << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)txac3newhead << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)txac2newhead << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)txac1newhead << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)txac0newhead << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)txbcnnewhead << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)txac3newtail << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)txac2newtail << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)txac1newtail << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)txac0newtail << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)txbcnnewtail << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR,  ((uint32_t)rxpayloadnewhead << 27) | ((uint32_t)rxheadernewhead << 26) | ((uint32_t)rxpayloadnewtail << 25) | ((uint32_t)rxheadernewtail << 24) | ((uint32_t)haltac3aftertxop << 19) | ((uint32_t)haltac2aftertxop << 18) | ((uint32_t)haltac1aftertxop << 17) | ((uint32_t)haltac0aftertxop << 16) | ((uint32_t)haltbcnaftertxop << 15) | ((uint32_t)txac3newhead << 12) | ((uint32_t)txac2newhead << 11) | ((uint32_t)txac1newhead << 10) | ((uint32_t)txac0newhead << 9) | ((uint32_t)txbcnnewhead << 8) | ((uint32_t)txac3newtail << 4) | ((uint32_t)txac2newtail << 3) | ((uint32_t)txac1newtail << 2) | ((uint32_t)txac0newtail << 1) | ((uint32_t)txbcnnewtail << 0));
}

/**
 * @brief Unpacks DMA_CNTRL_SET's fields from current value of the DMA_CNTRL_SET register.
 *
 * Reads the DMA_CNTRL_SET register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxpayloadnewhead - Will be populated with the current value of this field from the register.
 * @param[out] rxheadernewhead - Will be populated with the current value of this field from the register.
 * @param[out] rxpayloadnewtail - Will be populated with the current value of this field from the register.
 * @param[out] rxheadernewtail - Will be populated with the current value of this field from the register.
 * @param[out] haltac3aftertxop - Will be populated with the current value of this field from the register.
 * @param[out] haltac2aftertxop - Will be populated with the current value of this field from the register.
 * @param[out] haltac1aftertxop - Will be populated with the current value of this field from the register.
 * @param[out] haltac0aftertxop - Will be populated with the current value of this field from the register.
 * @param[out] haltbcnaftertxop - Will be populated with the current value of this field from the register.
 * @param[out] txac3newhead - Will be populated with the current value of this field from the register.
 * @param[out] txac2newhead - Will be populated with the current value of this field from the register.
 * @param[out] txac1newhead - Will be populated with the current value of this field from the register.
 * @param[out] txac0newhead - Will be populated with the current value of this field from the register.
 * @param[out] txbcnnewhead - Will be populated with the current value of this field from the register.
 * @param[out] txac3newtail - Will be populated with the current value of this field from the register.
 * @param[out] txac2newtail - Will be populated with the current value of this field from the register.
 * @param[out] txac1newtail - Will be populated with the current value of this field from the register.
 * @param[out] txac0newtail - Will be populated with the current value of this field from the register.
 * @param[out] txbcnnewtail - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_dma_cntrl_unpack(uint8_t *rxpayloadnewhead, uint8_t *rxheadernewhead, uint8_t *rxpayloadnewtail, uint8_t *rxheadernewtail, uint8_t *haltac3aftertxop, uint8_t *haltac2aftertxop, uint8_t *haltac1aftertxop, uint8_t *haltac0aftertxop, uint8_t *haltbcnaftertxop, uint8_t *txac3newhead, uint8_t *txac2newhead, uint8_t *txac1newhead, uint8_t *txac0newhead, uint8_t *txbcnnewhead, uint8_t *txac3newtail, uint8_t *txac2newtail, uint8_t *txac1newtail, uint8_t *txac0newtail, uint8_t *txbcnnewtail)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);

    *rxpayloadnewhead = (localVal & ((uint32_t)0x08000000)) >> 27;
    *rxheadernewhead = (localVal & ((uint32_t)0x04000000)) >> 26;
    *rxpayloadnewtail = (localVal & ((uint32_t)0x02000000)) >> 25;
    *rxheadernewtail = (localVal & ((uint32_t)0x01000000)) >> 24;
    *haltac3aftertxop = (localVal & ((uint32_t)0x00080000)) >> 19;
    *haltac2aftertxop = (localVal & ((uint32_t)0x00040000)) >> 18;
    *haltac1aftertxop = (localVal & ((uint32_t)0x00020000)) >> 17;
    *haltac0aftertxop = (localVal & ((uint32_t)0x00010000)) >> 16;
    *haltbcnaftertxop = (localVal & ((uint32_t)0x00008000)) >> 15;
    *txac3newhead = (localVal & ((uint32_t)0x00001000)) >> 12;
    *txac2newhead = (localVal & ((uint32_t)0x00000800)) >> 11;
    *txac1newhead = (localVal & ((uint32_t)0x00000400)) >> 10;
    *txac0newhead = (localVal & ((uint32_t)0x00000200)) >> 9;
    *txbcnnewhead = (localVal & ((uint32_t)0x00000100)) >> 8;
    *txac3newtail = (localVal & ((uint32_t)0x00000010)) >> 4;
    *txac2newtail = (localVal & ((uint32_t)0x00000008)) >> 3;
    *txac1newtail = (localVal & ((uint32_t)0x00000004)) >> 2;
    *txac0newtail = (localVal & ((uint32_t)0x00000002)) >> 1;
    *txbcnnewtail = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the rxPayloadNewHead field in the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read and the rxPayloadNewHead field's value will be returned.
 *
 * @return The current value of the rxPayloadNewHead field in the DMA_CNTRL_SET register.
 */
__INLINE uint8_t nxmac_rx_payload_new_head_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Sets the rxPayloadNewHead field of the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxpayloadnewhead - The value to set the field to.
 */
__INLINE void nxmac_rx_payload_new_head_setf(uint8_t rxpayloadnewhead)
{
    ASSERT_ERR((((uint32_t)rxpayloadnewhead << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, (uint32_t)rxpayloadnewhead << 27);
}

/**
 * @brief Returns the current value of the rxHeaderNewHead field in the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read and the rxHeaderNewHead field's value will be returned.
 *
 * @return The current value of the rxHeaderNewHead field in the DMA_CNTRL_SET register.
 */
__INLINE uint8_t nxmac_rx_header_new_head_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

/**
 * @brief Sets the rxHeaderNewHead field of the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxheadernewhead - The value to set the field to.
 */
__INLINE void nxmac_rx_header_new_head_setf(uint8_t rxheadernewhead)
{
    ASSERT_ERR((((uint32_t)rxheadernewhead << 26) & ~((uint32_t)0x04000000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, (uint32_t)rxheadernewhead << 26);
}

/**
 * @brief Returns the current value of the rxPayloadNewTail field in the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read and the rxPayloadNewTail field's value will be returned.
 *
 * @return The current value of the rxPayloadNewTail field in the DMA_CNTRL_SET register.
 */
__INLINE uint8_t nxmac_rx_payload_new_tail_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

/**
 * @brief Sets the rxPayloadNewTail field of the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxpayloadnewtail - The value to set the field to.
 */
__INLINE void nxmac_rx_payload_new_tail_setf(uint8_t rxpayloadnewtail)
{
    ASSERT_ERR((((uint32_t)rxpayloadnewtail << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, (uint32_t)rxpayloadnewtail << 25);
}

/**
 * @brief Returns the current value of the rxHeaderNewTail field in the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read and the rxHeaderNewTail field's value will be returned.
 *
 * @return The current value of the rxHeaderNewTail field in the DMA_CNTRL_SET register.
 */
__INLINE uint8_t nxmac_rx_header_new_tail_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the rxHeaderNewTail field of the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxheadernewtail - The value to set the field to.
 */
__INLINE void nxmac_rx_header_new_tail_setf(uint8_t rxheadernewtail)
{
    ASSERT_ERR((((uint32_t)rxheadernewtail << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, (uint32_t)rxheadernewtail << 24);
}

/**
 * @brief Returns the current value of the haltAC3AfterTXOP field in the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read and the haltAC3AfterTXOP field's value will be returned.
 *
 * @return The current value of the haltAC3AfterTXOP field in the DMA_CNTRL_SET register.
 */
__INLINE uint8_t nxmac_halt_ac_3_after_txop_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Sets the haltAC3AfterTXOP field of the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] haltac3aftertxop - The value to set the field to.
 */
__INLINE void nxmac_halt_ac_3_after_txop_setf(uint8_t haltac3aftertxop)
{
    ASSERT_ERR((((uint32_t)haltac3aftertxop << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, (uint32_t)haltac3aftertxop << 19);
}

/**
 * @brief Returns the current value of the haltAC2AfterTXOP field in the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read and the haltAC2AfterTXOP field's value will be returned.
 *
 * @return The current value of the haltAC2AfterTXOP field in the DMA_CNTRL_SET register.
 */
__INLINE uint8_t nxmac_halt_ac_2_after_txop_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Sets the haltAC2AfterTXOP field of the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] haltac2aftertxop - The value to set the field to.
 */
__INLINE void nxmac_halt_ac_2_after_txop_setf(uint8_t haltac2aftertxop)
{
    ASSERT_ERR((((uint32_t)haltac2aftertxop << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, (uint32_t)haltac2aftertxop << 18);
}

/**
 * @brief Returns the current value of the haltAC1AfterTXOP field in the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read and the haltAC1AfterTXOP field's value will be returned.
 *
 * @return The current value of the haltAC1AfterTXOP field in the DMA_CNTRL_SET register.
 */
__INLINE uint8_t nxmac_halt_ac_1_after_txop_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Sets the haltAC1AfterTXOP field of the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] haltac1aftertxop - The value to set the field to.
 */
__INLINE void nxmac_halt_ac_1_after_txop_setf(uint8_t haltac1aftertxop)
{
    ASSERT_ERR((((uint32_t)haltac1aftertxop << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, (uint32_t)haltac1aftertxop << 17);
}

/**
 * @brief Returns the current value of the haltAC0AfterTXOP field in the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read and the haltAC0AfterTXOP field's value will be returned.
 *
 * @return The current value of the haltAC0AfterTXOP field in the DMA_CNTRL_SET register.
 */
__INLINE uint8_t nxmac_halt_ac_0_after_txop_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the haltAC0AfterTXOP field of the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] haltac0aftertxop - The value to set the field to.
 */
__INLINE void nxmac_halt_ac_0_after_txop_setf(uint8_t haltac0aftertxop)
{
    ASSERT_ERR((((uint32_t)haltac0aftertxop << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, (uint32_t)haltac0aftertxop << 16);
}

/**
 * @brief Returns the current value of the haltBcnAfterTXOP field in the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read and the haltBcnAfterTXOP field's value will be returned.
 *
 * @return The current value of the haltBcnAfterTXOP field in the DMA_CNTRL_SET register.
 */
__INLINE uint8_t nxmac_halt_bcn_after_txop_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

/**
 * @brief Sets the haltBcnAfterTXOP field of the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] haltbcnaftertxop - The value to set the field to.
 */
__INLINE void nxmac_halt_bcn_after_txop_setf(uint8_t haltbcnaftertxop)
{
    ASSERT_ERR((((uint32_t)haltbcnaftertxop << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, (uint32_t)haltbcnaftertxop << 15);
}

/**
 * @brief Returns the current value of the txAC3NewHead field in the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read and the txAC3NewHead field's value will be returned.
 *
 * @return The current value of the txAC3NewHead field in the DMA_CNTRL_SET register.
 */
__INLINE uint8_t nxmac_tx_ac_3_new_head_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the txAC3NewHead field of the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac3newhead - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_3_new_head_setf(uint8_t txac3newhead)
{
    ASSERT_ERR((((uint32_t)txac3newhead << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, (uint32_t)txac3newhead << 12);
}

/**
 * @brief Returns the current value of the txAC2NewHead field in the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read and the txAC2NewHead field's value will be returned.
 *
 * @return The current value of the txAC2NewHead field in the DMA_CNTRL_SET register.
 */
__INLINE uint8_t nxmac_tx_ac_2_new_head_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the txAC2NewHead field of the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac2newhead - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_2_new_head_setf(uint8_t txac2newhead)
{
    ASSERT_ERR((((uint32_t)txac2newhead << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, (uint32_t)txac2newhead << 11);
}

/**
 * @brief Returns the current value of the txAC1NewHead field in the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read and the txAC1NewHead field's value will be returned.
 *
 * @return The current value of the txAC1NewHead field in the DMA_CNTRL_SET register.
 */
__INLINE uint8_t nxmac_tx_ac_1_new_head_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the txAC1NewHead field of the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac1newhead - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_1_new_head_setf(uint8_t txac1newhead)
{
    ASSERT_ERR((((uint32_t)txac1newhead << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, (uint32_t)txac1newhead << 10);
}

/**
 * @brief Returns the current value of the txAC0NewHead field in the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read and the txAC0NewHead field's value will be returned.
 *
 * @return The current value of the txAC0NewHead field in the DMA_CNTRL_SET register.
 */
__INLINE uint8_t nxmac_tx_ac_0_new_head_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the txAC0NewHead field of the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac0newhead - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_0_new_head_setf(uint8_t txac0newhead)
{
    ASSERT_ERR((((uint32_t)txac0newhead << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, (uint32_t)txac0newhead << 9);
}

/**
 * @brief Returns the current value of the txBcnNewHead field in the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read and the txBcnNewHead field's value will be returned.
 *
 * @return The current value of the txBcnNewHead field in the DMA_CNTRL_SET register.
 */
__INLINE uint8_t nxmac_tx_bcn_new_head_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the txBcnNewHead field of the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txbcnnewhead - The value to set the field to.
 */
__INLINE void nxmac_tx_bcn_new_head_setf(uint8_t txbcnnewhead)
{
    ASSERT_ERR((((uint32_t)txbcnnewhead << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, (uint32_t)txbcnnewhead << 8);
}

/**
 * @brief Returns the current value of the txAC3NewTail field in the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read and the txAC3NewTail field's value will be returned.
 *
 * @return The current value of the txAC3NewTail field in the DMA_CNTRL_SET register.
 */
__INLINE uint8_t nxmac_tx_ac_3_new_tail_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the txAC3NewTail field of the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac3newtail - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_3_new_tail_setf(uint8_t txac3newtail)
{
    ASSERT_ERR((((uint32_t)txac3newtail << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, (uint32_t)txac3newtail << 4);
}

/**
 * @brief Returns the current value of the txAC2NewTail field in the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read and the txAC2NewTail field's value will be returned.
 *
 * @return The current value of the txAC2NewTail field in the DMA_CNTRL_SET register.
 */
__INLINE uint8_t nxmac_tx_ac_2_new_tail_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the txAC2NewTail field of the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac2newtail - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_2_new_tail_setf(uint8_t txac2newtail)
{
    ASSERT_ERR((((uint32_t)txac2newtail << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, (uint32_t)txac2newtail << 3);
}

/**
 * @brief Returns the current value of the txAC1NewTail field in the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read and the txAC1NewTail field's value will be returned.
 *
 * @return The current value of the txAC1NewTail field in the DMA_CNTRL_SET register.
 */
__INLINE uint8_t nxmac_tx_ac_1_new_tail_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the txAC1NewTail field of the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac1newtail - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_1_new_tail_setf(uint8_t txac1newtail)
{
    ASSERT_ERR((((uint32_t)txac1newtail << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, (uint32_t)txac1newtail << 2);
}

/**
 * @brief Returns the current value of the txAC0NewTail field in the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read and the txAC0NewTail field's value will be returned.
 *
 * @return The current value of the txAC0NewTail field in the DMA_CNTRL_SET register.
 */
__INLINE uint8_t nxmac_tx_ac_0_new_tail_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the txAC0NewTail field of the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac0newtail - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_0_new_tail_setf(uint8_t txac0newtail)
{
    ASSERT_ERR((((uint32_t)txac0newtail << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, (uint32_t)txac0newtail << 1);
}

/**
 * @brief Returns the current value of the txBcnNewTail field in the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read and the txBcnNewTail field's value will be returned.
 *
 * @return The current value of the txBcnNewTail field in the DMA_CNTRL_SET register.
 */
__INLINE uint8_t nxmac_tx_bcn_new_tail_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_CNTRL_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the txBcnNewTail field of the DMA_CNTRL_SET register.
 *
 * The DMA_CNTRL_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txbcnnewtail - The value to set the field to.
 */
__INLINE void nxmac_tx_bcn_new_tail_setf(uint8_t txbcnnewtail)
{
    ASSERT_ERR((((uint32_t)txbcnnewtail << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_SET_ADDR, (uint32_t)txbcnnewtail << 0);
}

/// @}

/**
 * @name DMA_CNTRL_CLEAR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     27     rxPayloadNewHead   0
 *     26      rxHeaderNewHead   0
 *     25     rxPayloadNewTail   0
 *     24      rxHeaderNewTail   0
 *     19     haltAC3AfterTXOP   0
 *     18     haltAC2AfterTXOP   0
 *     17     haltAC1AfterTXOP   0
 *     16     haltAC0AfterTXOP   0
 *     15     haltBcnAfterTXOP   0
 *     12         txAC3NewHead   0
 *     11         txAC2NewHead   0
 *     10         txAC1NewHead   0
 *     09         txAC0NewHead   0
 *     08         txBcnNewHead   0
 *     04         txAC3NewTail   0
 *     03         txAC2NewTail   0
 *     02         txAC1NewTail   0
 *     01         txAC0NewTail   0
 *     00         txBcnNewTail   0
 * </pre>
 *
 * @{
 */

/// Address of the DMA_CNTRL_CLEAR register
#define NXMAC_DMA_CNTRL_CLEAR_ADDR   0xC0008184
/// Offset of the DMA_CNTRL_CLEAR register from the base address
#define NXMAC_DMA_CNTRL_CLEAR_OFFSET 0x00000184
/// Index of the DMA_CNTRL_CLEAR register
#define NXMAC_DMA_CNTRL_CLEAR_INDEX  0x00000061
/// Reset value of the DMA_CNTRL_CLEAR register
#define NXMAC_DMA_CNTRL_CLEAR_RESET  0x00000000

/**
 * @brief Sets the DMA_CNTRL_CLEAR register to a value.
 * The DMA_CNTRL_CLEAR register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_dma_cntrl_clear(uint32_t value)
{
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, value);
}

// fields defined in symmetrical set/clear register
/**
 * @brief Constructs a value for the DMA_CNTRL_CLEAR register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxpayloadnewhead - The value to use for the rxPayloadNewHead field.
 * @param[in] rxheadernewhead - The value to use for the rxHeaderNewHead field.
 * @param[in] rxpayloadnewtail - The value to use for the rxPayloadNewTail field.
 * @param[in] rxheadernewtail - The value to use for the rxHeaderNewTail field.
 * @param[in] haltac3aftertxop - The value to use for the haltAC3AfterTXOP field.
 * @param[in] haltac2aftertxop - The value to use for the haltAC2AfterTXOP field.
 * @param[in] haltac1aftertxop - The value to use for the haltAC1AfterTXOP field.
 * @param[in] haltac0aftertxop - The value to use for the haltAC0AfterTXOP field.
 * @param[in] haltbcnaftertxop - The value to use for the haltBcnAfterTXOP field.
 * @param[in] txac3newhead - The value to use for the txAC3NewHead field.
 * @param[in] txac2newhead - The value to use for the txAC2NewHead field.
 * @param[in] txac1newhead - The value to use for the txAC1NewHead field.
 * @param[in] txac0newhead - The value to use for the txAC0NewHead field.
 * @param[in] txbcnnewhead - The value to use for the txBcnNewHead field.
 * @param[in] txac3newtail - The value to use for the txAC3NewTail field.
 * @param[in] txac2newtail - The value to use for the txAC2NewTail field.
 * @param[in] txac1newtail - The value to use for the txAC1NewTail field.
 * @param[in] txac0newtail - The value to use for the txAC0NewTail field.
 * @param[in] txbcnnewtail - The value to use for the txBcnNewTail field.
 */
__INLINE void nxmac_dma_cntrl_clear_pack(uint8_t rxpayloadnewhead, uint8_t rxheadernewhead, uint8_t rxpayloadnewtail, uint8_t rxheadernewtail, uint8_t haltac3aftertxop, uint8_t haltac2aftertxop, uint8_t haltac1aftertxop, uint8_t haltac0aftertxop, uint8_t haltbcnaftertxop, uint8_t txac3newhead, uint8_t txac2newhead, uint8_t txac1newhead, uint8_t txac0newhead, uint8_t txbcnnewhead, uint8_t txac3newtail, uint8_t txac2newtail, uint8_t txac1newtail, uint8_t txac0newtail, uint8_t txbcnnewtail)
{
    ASSERT_ERR((((uint32_t)rxpayloadnewhead << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)rxheadernewhead << 26) & ~((uint32_t)0x04000000)) == 0);
    ASSERT_ERR((((uint32_t)rxpayloadnewtail << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)rxheadernewtail << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)haltac3aftertxop << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)haltac2aftertxop << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)haltac1aftertxop << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)haltac0aftertxop << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)haltbcnaftertxop << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)txac3newhead << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)txac2newhead << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)txac1newhead << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)txac0newhead << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)txbcnnewhead << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)txac3newtail << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)txac2newtail << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)txac1newtail << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)txac0newtail << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)txbcnnewtail << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR,  ((uint32_t)rxpayloadnewhead << 27) | ((uint32_t)rxheadernewhead << 26) | ((uint32_t)rxpayloadnewtail << 25) | ((uint32_t)rxheadernewtail << 24) | ((uint32_t)haltac3aftertxop << 19) | ((uint32_t)haltac2aftertxop << 18) | ((uint32_t)haltac1aftertxop << 17) | ((uint32_t)haltac0aftertxop << 16) | ((uint32_t)haltbcnaftertxop << 15) | ((uint32_t)txac3newhead << 12) | ((uint32_t)txac2newhead << 11) | ((uint32_t)txac1newhead << 10) | ((uint32_t)txac0newhead << 9) | ((uint32_t)txbcnnewhead << 8) | ((uint32_t)txac3newtail << 4) | ((uint32_t)txac2newtail << 3) | ((uint32_t)txac1newtail << 2) | ((uint32_t)txac0newtail << 1) | ((uint32_t)txbcnnewtail << 0));
}

/**
 * @brief Sets the rxPayloadNewHead field of the DMA_CNTRL_CLEAR register.
 *
 * The DMA_CNTRL_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxpayloadnewhead - The value to set the field to.
 */
__INLINE void nxmac_rx_payload_new_head_clearf(uint8_t rxpayloadnewhead)
{
    ASSERT_ERR((((uint32_t)rxpayloadnewhead << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, (uint32_t)rxpayloadnewhead << 27);
}

/**
 * @brief Sets the rxHeaderNewHead field of the DMA_CNTRL_CLEAR register.
 *
 * The DMA_CNTRL_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxheadernewhead - The value to set the field to.
 */
__INLINE void nxmac_rx_header_new_head_clearf(uint8_t rxheadernewhead)
{
    ASSERT_ERR((((uint32_t)rxheadernewhead << 26) & ~((uint32_t)0x04000000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, (uint32_t)rxheadernewhead << 26);
}

/**
 * @brief Sets the rxPayloadNewTail field of the DMA_CNTRL_CLEAR register.
 *
 * The DMA_CNTRL_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxpayloadnewtail - The value to set the field to.
 */
__INLINE void nxmac_rx_payload_new_tail_clearf(uint8_t rxpayloadnewtail)
{
    ASSERT_ERR((((uint32_t)rxpayloadnewtail << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, (uint32_t)rxpayloadnewtail << 25);
}

/**
 * @brief Sets the rxHeaderNewTail field of the DMA_CNTRL_CLEAR register.
 *
 * The DMA_CNTRL_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxheadernewtail - The value to set the field to.
 */
__INLINE void nxmac_rx_header_new_tail_clearf(uint8_t rxheadernewtail)
{
    ASSERT_ERR((((uint32_t)rxheadernewtail << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, (uint32_t)rxheadernewtail << 24);
}

/**
 * @brief Sets the haltAC3AfterTXOP field of the DMA_CNTRL_CLEAR register.
 *
 * The DMA_CNTRL_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] haltac3aftertxop - The value to set the field to.
 */
__INLINE void nxmac_halt_ac_3_after_txop_clearf(uint8_t haltac3aftertxop)
{
    ASSERT_ERR((((uint32_t)haltac3aftertxop << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, (uint32_t)haltac3aftertxop << 19);
}

/**
 * @brief Sets the haltAC2AfterTXOP field of the DMA_CNTRL_CLEAR register.
 *
 * The DMA_CNTRL_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] haltac2aftertxop - The value to set the field to.
 */
__INLINE void nxmac_halt_ac_2_after_txop_clearf(uint8_t haltac2aftertxop)
{
    ASSERT_ERR((((uint32_t)haltac2aftertxop << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, (uint32_t)haltac2aftertxop << 18);
}

/**
 * @brief Sets the haltAC1AfterTXOP field of the DMA_CNTRL_CLEAR register.
 *
 * The DMA_CNTRL_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] haltac1aftertxop - The value to set the field to.
 */
__INLINE void nxmac_halt_ac_1_after_txop_clearf(uint8_t haltac1aftertxop)
{
    ASSERT_ERR((((uint32_t)haltac1aftertxop << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, (uint32_t)haltac1aftertxop << 17);
}

/**
 * @brief Sets the haltAC0AfterTXOP field of the DMA_CNTRL_CLEAR register.
 *
 * The DMA_CNTRL_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] haltac0aftertxop - The value to set the field to.
 */
__INLINE void nxmac_halt_ac_0_after_txop_clearf(uint8_t haltac0aftertxop)
{
    ASSERT_ERR((((uint32_t)haltac0aftertxop << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, (uint32_t)haltac0aftertxop << 16);
}

/**
 * @brief Sets the haltBcnAfterTXOP field of the DMA_CNTRL_CLEAR register.
 *
 * The DMA_CNTRL_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] haltbcnaftertxop - The value to set the field to.
 */
__INLINE void nxmac_halt_bcn_after_txop_clearf(uint8_t haltbcnaftertxop)
{
    ASSERT_ERR((((uint32_t)haltbcnaftertxop << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, (uint32_t)haltbcnaftertxop << 15);
}

/**
 * @brief Sets the txAC3NewHead field of the DMA_CNTRL_CLEAR register.
 *
 * The DMA_CNTRL_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac3newhead - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_3_new_head_clearf(uint8_t txac3newhead)
{
    ASSERT_ERR((((uint32_t)txac3newhead << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, (uint32_t)txac3newhead << 12);
}

/**
 * @brief Sets the txAC2NewHead field of the DMA_CNTRL_CLEAR register.
 *
 * The DMA_CNTRL_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac2newhead - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_2_new_head_clearf(uint8_t txac2newhead)
{
    ASSERT_ERR((((uint32_t)txac2newhead << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, (uint32_t)txac2newhead << 11);
}

/**
 * @brief Sets the txAC1NewHead field of the DMA_CNTRL_CLEAR register.
 *
 * The DMA_CNTRL_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac1newhead - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_1_new_head_clearf(uint8_t txac1newhead)
{
    ASSERT_ERR((((uint32_t)txac1newhead << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, (uint32_t)txac1newhead << 10);
}

/**
 * @brief Sets the txAC0NewHead field of the DMA_CNTRL_CLEAR register.
 *
 * The DMA_CNTRL_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac0newhead - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_0_new_head_clearf(uint8_t txac0newhead)
{
    ASSERT_ERR((((uint32_t)txac0newhead << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, (uint32_t)txac0newhead << 9);
}

/**
 * @brief Sets the txBcnNewHead field of the DMA_CNTRL_CLEAR register.
 *
 * The DMA_CNTRL_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txbcnnewhead - The value to set the field to.
 */
__INLINE void nxmac_tx_bcn_new_head_clearf(uint8_t txbcnnewhead)
{
    ASSERT_ERR((((uint32_t)txbcnnewhead << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, (uint32_t)txbcnnewhead << 8);
}

/**
 * @brief Sets the txAC3NewTail field of the DMA_CNTRL_CLEAR register.
 *
 * The DMA_CNTRL_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac3newtail - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_3_new_tail_clearf(uint8_t txac3newtail)
{
    ASSERT_ERR((((uint32_t)txac3newtail << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, (uint32_t)txac3newtail << 4);
}

/**
 * @brief Sets the txAC2NewTail field of the DMA_CNTRL_CLEAR register.
 *
 * The DMA_CNTRL_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac2newtail - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_2_new_tail_clearf(uint8_t txac2newtail)
{
    ASSERT_ERR((((uint32_t)txac2newtail << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, (uint32_t)txac2newtail << 3);
}

/**
 * @brief Sets the txAC1NewTail field of the DMA_CNTRL_CLEAR register.
 *
 * The DMA_CNTRL_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac1newtail - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_1_new_tail_clearf(uint8_t txac1newtail)
{
    ASSERT_ERR((((uint32_t)txac1newtail << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, (uint32_t)txac1newtail << 2);
}

/**
 * @brief Sets the txAC0NewTail field of the DMA_CNTRL_CLEAR register.
 *
 * The DMA_CNTRL_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac0newtail - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_0_new_tail_clearf(uint8_t txac0newtail)
{
    ASSERT_ERR((((uint32_t)txac0newtail << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, (uint32_t)txac0newtail << 1);
}

/**
 * @brief Sets the txBcnNewTail field of the DMA_CNTRL_CLEAR register.
 *
 * The DMA_CNTRL_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txbcnnewtail - The value to set the field to.
 */
__INLINE void nxmac_tx_bcn_new_tail_clearf(uint8_t txbcnnewtail)
{
    ASSERT_ERR((((uint32_t)txbcnnewtail << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_DMA_CNTRL_CLEAR_ADDR, (uint32_t)txbcnnewtail << 0);
}

/// @}

/**
 * @name DMA_STATUS_1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:28       rxPayloadState   0x0
 *  25:24        rxHeaderState   0x0
 *  17:16           txAC3State   0x0
 *  13:12           txAC2State   0x0
 *  09:08           txAC1State   0x0
 *  05:04           txAC0State   0x0
 *  01:00           txBcnState   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DMA_STATUS_1 register
#define NXMAC_DMA_STATUS_1_ADDR   0xC0008188
/// Offset of the DMA_STATUS_1 register from the base address
#define NXMAC_DMA_STATUS_1_OFFSET 0x00000188
/// Index of the DMA_STATUS_1 register
#define NXMAC_DMA_STATUS_1_INDEX  0x00000062
/// Reset value of the DMA_STATUS_1 register
#define NXMAC_DMA_STATUS_1_RESET  0x00000000

/**
 * @brief Returns the current value of the DMA_STATUS_1 register.
 * The DMA_STATUS_1 register will be read and its value returned.
 * @return The current value of the DMA_STATUS_1 register.
 */
__INLINE uint32_t nxmac_dma_status_1_get(void)
{
    return REG_PL_RD(NXMAC_DMA_STATUS_1_ADDR);
}

// field definitions
/// RX_PAYLOAD_STATE field mask
#define NXMAC_RX_PAYLOAD_STATE_MASK   ((uint32_t)0x30000000)
/// RX_PAYLOAD_STATE field LSB position
#define NXMAC_RX_PAYLOAD_STATE_LSB    28
/// RX_PAYLOAD_STATE field width
#define NXMAC_RX_PAYLOAD_STATE_WIDTH  ((uint32_t)0x00000002)
/// RX_HEADER_STATE field mask
#define NXMAC_RX_HEADER_STATE_MASK    ((uint32_t)0x03000000)
/// RX_HEADER_STATE field LSB position
#define NXMAC_RX_HEADER_STATE_LSB     24
/// RX_HEADER_STATE field width
#define NXMAC_RX_HEADER_STATE_WIDTH   ((uint32_t)0x00000002)
/// TX_AC_3_STATE field mask
#define NXMAC_TX_AC_3_STATE_MASK      ((uint32_t)0x00030000)
/// TX_AC_3_STATE field LSB position
#define NXMAC_TX_AC_3_STATE_LSB       16
/// TX_AC_3_STATE field width
#define NXMAC_TX_AC_3_STATE_WIDTH     ((uint32_t)0x00000002)
/// TX_AC_2_STATE field mask
#define NXMAC_TX_AC_2_STATE_MASK      ((uint32_t)0x00003000)
/// TX_AC_2_STATE field LSB position
#define NXMAC_TX_AC_2_STATE_LSB       12
/// TX_AC_2_STATE field width
#define NXMAC_TX_AC_2_STATE_WIDTH     ((uint32_t)0x00000002)
/// TX_AC_1_STATE field mask
#define NXMAC_TX_AC_1_STATE_MASK      ((uint32_t)0x00000300)
/// TX_AC_1_STATE field LSB position
#define NXMAC_TX_AC_1_STATE_LSB       8
/// TX_AC_1_STATE field width
#define NXMAC_TX_AC_1_STATE_WIDTH     ((uint32_t)0x00000002)
/// TX_AC_0_STATE field mask
#define NXMAC_TX_AC_0_STATE_MASK      ((uint32_t)0x00000030)
/// TX_AC_0_STATE field LSB position
#define NXMAC_TX_AC_0_STATE_LSB       4
/// TX_AC_0_STATE field width
#define NXMAC_TX_AC_0_STATE_WIDTH     ((uint32_t)0x00000002)
/// TX_BCN_STATE field mask
#define NXMAC_TX_BCN_STATE_MASK       ((uint32_t)0x00000003)
/// TX_BCN_STATE field LSB position
#define NXMAC_TX_BCN_STATE_LSB        0
/// TX_BCN_STATE field width
#define NXMAC_TX_BCN_STATE_WIDTH      ((uint32_t)0x00000002)

/// RX_PAYLOAD_STATE field reset value
#define NXMAC_RX_PAYLOAD_STATE_RST    0x0
/// RX_HEADER_STATE field reset value
#define NXMAC_RX_HEADER_STATE_RST     0x0
/// TX_AC_3_STATE field reset value
#define NXMAC_TX_AC_3_STATE_RST       0x0
/// TX_AC_2_STATE field reset value
#define NXMAC_TX_AC_2_STATE_RST       0x0
/// TX_AC_1_STATE field reset value
#define NXMAC_TX_AC_1_STATE_RST       0x0
/// TX_AC_0_STATE field reset value
#define NXMAC_TX_AC_0_STATE_RST       0x0
/// TX_BCN_STATE field reset value
#define NXMAC_TX_BCN_STATE_RST        0x0

/**
 * @brief Unpacks DMA_STATUS_1's fields from current value of the DMA_STATUS_1 register.
 *
 * Reads the DMA_STATUS_1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxpayloadstate - Will be populated with the current value of this field from the register.
 * @param[out] rxheaderstate - Will be populated with the current value of this field from the register.
 * @param[out] txac3state - Will be populated with the current value of this field from the register.
 * @param[out] txac2state - Will be populated with the current value of this field from the register.
 * @param[out] txac1state - Will be populated with the current value of this field from the register.
 * @param[out] txac0state - Will be populated with the current value of this field from the register.
 * @param[out] txbcnstate - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_dma_status_1_unpack(uint8_t *rxpayloadstate, uint8_t *rxheaderstate, uint8_t *txac3state, uint8_t *txac2state, uint8_t *txac1state, uint8_t *txac0state, uint8_t *txbcnstate)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_1_ADDR);

    *rxpayloadstate = (localVal & ((uint32_t)0x30000000)) >> 28;
    *rxheaderstate = (localVal & ((uint32_t)0x03000000)) >> 24;
    *txac3state = (localVal & ((uint32_t)0x00030000)) >> 16;
    *txac2state = (localVal & ((uint32_t)0x00003000)) >> 12;
    *txac1state = (localVal & ((uint32_t)0x00000300)) >> 8;
    *txac0state = (localVal & ((uint32_t)0x00000030)) >> 4;
    *txbcnstate = (localVal & ((uint32_t)0x00000003)) >> 0;
}

/**
 * @brief Returns the current value of the rxPayloadState field in the DMA_STATUS_1 register.
 *
 * The DMA_STATUS_1 register will be read and the rxPayloadState field's value will be returned.
 *
 * @return The current value of the rxPayloadState field in the DMA_STATUS_1 register.
 */
__INLINE uint8_t nxmac_rx_payload_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_1_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

/**
 * @brief Returns the current value of the rxHeaderState field in the DMA_STATUS_1 register.
 *
 * The DMA_STATUS_1 register will be read and the rxHeaderState field's value will be returned.
 *
 * @return The current value of the rxHeaderState field in the DMA_STATUS_1 register.
 */
__INLINE uint8_t nxmac_rx_header_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_1_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

/**
 * @brief Returns the current value of the txAC3State field in the DMA_STATUS_1 register.
 *
 * The DMA_STATUS_1 register will be read and the txAC3State field's value will be returned.
 *
 * @return The current value of the txAC3State field in the DMA_STATUS_1 register.
 */
__INLINE uint8_t nxmac_tx_ac_3_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_1_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

/**
 * @brief Returns the current value of the txAC2State field in the DMA_STATUS_1 register.
 *
 * The DMA_STATUS_1 register will be read and the txAC2State field's value will be returned.
 *
 * @return The current value of the txAC2State field in the DMA_STATUS_1 register.
 */
__INLINE uint8_t nxmac_tx_ac_2_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_1_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

/**
 * @brief Returns the current value of the txAC1State field in the DMA_STATUS_1 register.
 *
 * The DMA_STATUS_1 register will be read and the txAC1State field's value will be returned.
 *
 * @return The current value of the txAC1State field in the DMA_STATUS_1 register.
 */
__INLINE uint8_t nxmac_tx_ac_1_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_1_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

/**
 * @brief Returns the current value of the txAC0State field in the DMA_STATUS_1 register.
 *
 * The DMA_STATUS_1 register will be read and the txAC0State field's value will be returned.
 *
 * @return The current value of the txAC0State field in the DMA_STATUS_1 register.
 */
__INLINE uint8_t nxmac_tx_ac_0_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_1_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

/**
 * @brief Returns the current value of the txBcnState field in the DMA_STATUS_1 register.
 *
 * The DMA_STATUS_1 register will be read and the txBcnState field's value will be returned.
 *
 * @return The current value of the txBcnState field in the DMA_STATUS_1 register.
 */
__INLINE uint8_t nxmac_tx_bcn_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_1_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

/// @}

/**
 * @name DMA_STATUS_2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     29      txAC3NewHeadErr   0
 *     28      txAC2NewHeadErr   0
 *     27      txAC1NewHeadErr   0
 *     26      txAC0NewHeadErr   0
 *     25      txBcnNewHeadErr   0
 *     24          txAC3BusErr   0
 *     23          txAC2BusErr   0
 *     22          txAC1BusErr   0
 *     21          txAC0BusErr   0
 *     20          txBcnBusErr   0
 *     19    txAC3PTAddressErr   0
 *     18    txAC2PTAddressErr   0
 *     17    txAC1PTAddressErr   0
 *     16    txAC0PTAddressErr   0
 *     15    txBcnPTAddressErr   0
 *     14   txAC3NextPointerErr   0
 *     13   txAC2NextPointerErr   0
 *     12   txAC1NextPointerErr   0
 *     11   txAC0NextPointerErr   0
 *     10   txBcnNextPointerErr   0
 *     09     txAC3UPatternErr   0
 *     08     txAC2UPatternErr   0
 *     07     txAC1UPatternErr   0
 *     06     txAC0UPatternErr   0
 *     05     txBcnUPatternErr   0
 *     04     txAC3LenMismatch   0
 *     03     txAC2LenMismatch   0
 *     02     txAC1LenMismatch   0
 *     01     txAC0LenMismatch   0
 *     00     txBcnLenMismatch   0
 * </pre>
 *
 * @{
 */

/// Address of the DMA_STATUS_2 register
#define NXMAC_DMA_STATUS_2_ADDR   0xC000818C
/// Offset of the DMA_STATUS_2 register from the base address
#define NXMAC_DMA_STATUS_2_OFFSET 0x0000018C
/// Index of the DMA_STATUS_2 register
#define NXMAC_DMA_STATUS_2_INDEX  0x00000063
/// Reset value of the DMA_STATUS_2 register
#define NXMAC_DMA_STATUS_2_RESET  0x00000000

/**
 * @brief Returns the current value of the DMA_STATUS_2 register.
 * The DMA_STATUS_2 register will be read and its value returned.
 * @return The current value of the DMA_STATUS_2 register.
 */
__INLINE uint32_t nxmac_dma_status_2_get(void)
{
    return REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
}

// field definitions
/// TX_AC_3_NEW_HEAD_ERR field bit
#define NXMAC_TX_AC_3_NEW_HEAD_ERR_BIT        ((uint32_t)0x20000000)
/// TX_AC_3_NEW_HEAD_ERR field position
#define NXMAC_TX_AC_3_NEW_HEAD_ERR_POS        29
/// TX_AC_2_NEW_HEAD_ERR field bit
#define NXMAC_TX_AC_2_NEW_HEAD_ERR_BIT        ((uint32_t)0x10000000)
/// TX_AC_2_NEW_HEAD_ERR field position
#define NXMAC_TX_AC_2_NEW_HEAD_ERR_POS        28
/// TX_AC_1_NEW_HEAD_ERR field bit
#define NXMAC_TX_AC_1_NEW_HEAD_ERR_BIT        ((uint32_t)0x08000000)
/// TX_AC_1_NEW_HEAD_ERR field position
#define NXMAC_TX_AC_1_NEW_HEAD_ERR_POS        27
/// TX_AC_0_NEW_HEAD_ERR field bit
#define NXMAC_TX_AC_0_NEW_HEAD_ERR_BIT        ((uint32_t)0x04000000)
/// TX_AC_0_NEW_HEAD_ERR field position
#define NXMAC_TX_AC_0_NEW_HEAD_ERR_POS        26
/// TX_BCN_NEW_HEAD_ERR field bit
#define NXMAC_TX_BCN_NEW_HEAD_ERR_BIT         ((uint32_t)0x02000000)
/// TX_BCN_NEW_HEAD_ERR field position
#define NXMAC_TX_BCN_NEW_HEAD_ERR_POS         25
/// TX_AC_3_BUS_ERR field bit
#define NXMAC_TX_AC_3_BUS_ERR_BIT             ((uint32_t)0x01000000)
/// TX_AC_3_BUS_ERR field position
#define NXMAC_TX_AC_3_BUS_ERR_POS             24
/// TX_AC_2_BUS_ERR field bit
#define NXMAC_TX_AC_2_BUS_ERR_BIT             ((uint32_t)0x00800000)
/// TX_AC_2_BUS_ERR field position
#define NXMAC_TX_AC_2_BUS_ERR_POS             23
/// TX_AC_1_BUS_ERR field bit
#define NXMAC_TX_AC_1_BUS_ERR_BIT             ((uint32_t)0x00400000)
/// TX_AC_1_BUS_ERR field position
#define NXMAC_TX_AC_1_BUS_ERR_POS             22
/// TX_AC_0_BUS_ERR field bit
#define NXMAC_TX_AC_0_BUS_ERR_BIT             ((uint32_t)0x00200000)
/// TX_AC_0_BUS_ERR field position
#define NXMAC_TX_AC_0_BUS_ERR_POS             21
/// TX_BCN_BUS_ERR field bit
#define NXMAC_TX_BCN_BUS_ERR_BIT              ((uint32_t)0x00100000)
/// TX_BCN_BUS_ERR field position
#define NXMAC_TX_BCN_BUS_ERR_POS              20
/// TX_AC_3PT_ADDRESS_ERR field bit
#define NXMAC_TX_AC_3PT_ADDRESS_ERR_BIT       ((uint32_t)0x00080000)
/// TX_AC_3PT_ADDRESS_ERR field position
#define NXMAC_TX_AC_3PT_ADDRESS_ERR_POS       19
/// TX_AC_2PT_ADDRESS_ERR field bit
#define NXMAC_TX_AC_2PT_ADDRESS_ERR_BIT       ((uint32_t)0x00040000)
/// TX_AC_2PT_ADDRESS_ERR field position
#define NXMAC_TX_AC_2PT_ADDRESS_ERR_POS       18
/// TX_AC_1PT_ADDRESS_ERR field bit
#define NXMAC_TX_AC_1PT_ADDRESS_ERR_BIT       ((uint32_t)0x00020000)
/// TX_AC_1PT_ADDRESS_ERR field position
#define NXMAC_TX_AC_1PT_ADDRESS_ERR_POS       17
/// TX_AC_0PT_ADDRESS_ERR field bit
#define NXMAC_TX_AC_0PT_ADDRESS_ERR_BIT       ((uint32_t)0x00010000)
/// TX_AC_0PT_ADDRESS_ERR field position
#define NXMAC_TX_AC_0PT_ADDRESS_ERR_POS       16
/// TX_BCN_PT_ADDRESS_ERR field bit
#define NXMAC_TX_BCN_PT_ADDRESS_ERR_BIT       ((uint32_t)0x00008000)
/// TX_BCN_PT_ADDRESS_ERR field position
#define NXMAC_TX_BCN_PT_ADDRESS_ERR_POS       15
/// TX_AC_3_NEXT_POINTER_ERR field bit
#define NXMAC_TX_AC_3_NEXT_POINTER_ERR_BIT    ((uint32_t)0x00004000)
/// TX_AC_3_NEXT_POINTER_ERR field position
#define NXMAC_TX_AC_3_NEXT_POINTER_ERR_POS    14
/// TX_AC_2_NEXT_POINTER_ERR field bit
#define NXMAC_TX_AC_2_NEXT_POINTER_ERR_BIT    ((uint32_t)0x00002000)
/// TX_AC_2_NEXT_POINTER_ERR field position
#define NXMAC_TX_AC_2_NEXT_POINTER_ERR_POS    13
/// TX_AC_1_NEXT_POINTER_ERR field bit
#define NXMAC_TX_AC_1_NEXT_POINTER_ERR_BIT    ((uint32_t)0x00001000)
/// TX_AC_1_NEXT_POINTER_ERR field position
#define NXMAC_TX_AC_1_NEXT_POINTER_ERR_POS    12
/// TX_AC_0_NEXT_POINTER_ERR field bit
#define NXMAC_TX_AC_0_NEXT_POINTER_ERR_BIT    ((uint32_t)0x00000800)
/// TX_AC_0_NEXT_POINTER_ERR field position
#define NXMAC_TX_AC_0_NEXT_POINTER_ERR_POS    11
/// TX_BCN_NEXT_POINTER_ERR field bit
#define NXMAC_TX_BCN_NEXT_POINTER_ERR_BIT     ((uint32_t)0x00000400)
/// TX_BCN_NEXT_POINTER_ERR field position
#define NXMAC_TX_BCN_NEXT_POINTER_ERR_POS     10
/// TX_AC_3U_PATTERN_ERR field bit
#define NXMAC_TX_AC_3U_PATTERN_ERR_BIT        ((uint32_t)0x00000200)
/// TX_AC_3U_PATTERN_ERR field position
#define NXMAC_TX_AC_3U_PATTERN_ERR_POS        9
/// TX_AC_2U_PATTERN_ERR field bit
#define NXMAC_TX_AC_2U_PATTERN_ERR_BIT        ((uint32_t)0x00000100)
/// TX_AC_2U_PATTERN_ERR field position
#define NXMAC_TX_AC_2U_PATTERN_ERR_POS        8
/// TX_AC_1U_PATTERN_ERR field bit
#define NXMAC_TX_AC_1U_PATTERN_ERR_BIT        ((uint32_t)0x00000080)
/// TX_AC_1U_PATTERN_ERR field position
#define NXMAC_TX_AC_1U_PATTERN_ERR_POS        7
/// TX_AC_0U_PATTERN_ERR field bit
#define NXMAC_TX_AC_0U_PATTERN_ERR_BIT        ((uint32_t)0x00000040)
/// TX_AC_0U_PATTERN_ERR field position
#define NXMAC_TX_AC_0U_PATTERN_ERR_POS        6
/// TX_BCN_U_PATTERN_ERR field bit
#define NXMAC_TX_BCN_U_PATTERN_ERR_BIT        ((uint32_t)0x00000020)
/// TX_BCN_U_PATTERN_ERR field position
#define NXMAC_TX_BCN_U_PATTERN_ERR_POS        5
/// TX_AC_3_LEN_MISMATCH field bit
#define NXMAC_TX_AC_3_LEN_MISMATCH_BIT        ((uint32_t)0x00000010)
/// TX_AC_3_LEN_MISMATCH field position
#define NXMAC_TX_AC_3_LEN_MISMATCH_POS        4
/// TX_AC_2_LEN_MISMATCH field bit
#define NXMAC_TX_AC_2_LEN_MISMATCH_BIT        ((uint32_t)0x00000008)
/// TX_AC_2_LEN_MISMATCH field position
#define NXMAC_TX_AC_2_LEN_MISMATCH_POS        3
/// TX_AC_1_LEN_MISMATCH field bit
#define NXMAC_TX_AC_1_LEN_MISMATCH_BIT        ((uint32_t)0x00000004)
/// TX_AC_1_LEN_MISMATCH field position
#define NXMAC_TX_AC_1_LEN_MISMATCH_POS        2
/// TX_AC_0_LEN_MISMATCH field bit
#define NXMAC_TX_AC_0_LEN_MISMATCH_BIT        ((uint32_t)0x00000002)
/// TX_AC_0_LEN_MISMATCH field position
#define NXMAC_TX_AC_0_LEN_MISMATCH_POS        1
/// TX_BCN_LEN_MISMATCH field bit
#define NXMAC_TX_BCN_LEN_MISMATCH_BIT         ((uint32_t)0x00000001)
/// TX_BCN_LEN_MISMATCH field position
#define NXMAC_TX_BCN_LEN_MISMATCH_POS         0

/// TX_AC_3_NEW_HEAD_ERR field reset value
#define NXMAC_TX_AC_3_NEW_HEAD_ERR_RST        0x0
/// TX_AC_2_NEW_HEAD_ERR field reset value
#define NXMAC_TX_AC_2_NEW_HEAD_ERR_RST        0x0
/// TX_AC_1_NEW_HEAD_ERR field reset value
#define NXMAC_TX_AC_1_NEW_HEAD_ERR_RST        0x0
/// TX_AC_0_NEW_HEAD_ERR field reset value
#define NXMAC_TX_AC_0_NEW_HEAD_ERR_RST        0x0
/// TX_BCN_NEW_HEAD_ERR field reset value
#define NXMAC_TX_BCN_NEW_HEAD_ERR_RST         0x0
/// TX_AC_3_BUS_ERR field reset value
#define NXMAC_TX_AC_3_BUS_ERR_RST             0x0
/// TX_AC_2_BUS_ERR field reset value
#define NXMAC_TX_AC_2_BUS_ERR_RST             0x0
/// TX_AC_1_BUS_ERR field reset value
#define NXMAC_TX_AC_1_BUS_ERR_RST             0x0
/// TX_AC_0_BUS_ERR field reset value
#define NXMAC_TX_AC_0_BUS_ERR_RST             0x0
/// TX_BCN_BUS_ERR field reset value
#define NXMAC_TX_BCN_BUS_ERR_RST              0x0
/// TX_AC_3PT_ADDRESS_ERR field reset value
#define NXMAC_TX_AC_3PT_ADDRESS_ERR_RST       0x0
/// TX_AC_2PT_ADDRESS_ERR field reset value
#define NXMAC_TX_AC_2PT_ADDRESS_ERR_RST       0x0
/// TX_AC_1PT_ADDRESS_ERR field reset value
#define NXMAC_TX_AC_1PT_ADDRESS_ERR_RST       0x0
/// TX_AC_0PT_ADDRESS_ERR field reset value
#define NXMAC_TX_AC_0PT_ADDRESS_ERR_RST       0x0
/// TX_BCN_PT_ADDRESS_ERR field reset value
#define NXMAC_TX_BCN_PT_ADDRESS_ERR_RST       0x0
/// TX_AC_3_NEXT_POINTER_ERR field reset value
#define NXMAC_TX_AC_3_NEXT_POINTER_ERR_RST    0x0
/// TX_AC_2_NEXT_POINTER_ERR field reset value
#define NXMAC_TX_AC_2_NEXT_POINTER_ERR_RST    0x0
/// TX_AC_1_NEXT_POINTER_ERR field reset value
#define NXMAC_TX_AC_1_NEXT_POINTER_ERR_RST    0x0
/// TX_AC_0_NEXT_POINTER_ERR field reset value
#define NXMAC_TX_AC_0_NEXT_POINTER_ERR_RST    0x0
/// TX_BCN_NEXT_POINTER_ERR field reset value
#define NXMAC_TX_BCN_NEXT_POINTER_ERR_RST     0x0
/// TX_AC_3U_PATTERN_ERR field reset value
#define NXMAC_TX_AC_3U_PATTERN_ERR_RST        0x0
/// TX_AC_2U_PATTERN_ERR field reset value
#define NXMAC_TX_AC_2U_PATTERN_ERR_RST        0x0
/// TX_AC_1U_PATTERN_ERR field reset value
#define NXMAC_TX_AC_1U_PATTERN_ERR_RST        0x0
/// TX_AC_0U_PATTERN_ERR field reset value
#define NXMAC_TX_AC_0U_PATTERN_ERR_RST        0x0
/// TX_BCN_U_PATTERN_ERR field reset value
#define NXMAC_TX_BCN_U_PATTERN_ERR_RST        0x0
/// TX_AC_3_LEN_MISMATCH field reset value
#define NXMAC_TX_AC_3_LEN_MISMATCH_RST        0x0
/// TX_AC_2_LEN_MISMATCH field reset value
#define NXMAC_TX_AC_2_LEN_MISMATCH_RST        0x0
/// TX_AC_1_LEN_MISMATCH field reset value
#define NXMAC_TX_AC_1_LEN_MISMATCH_RST        0x0
/// TX_AC_0_LEN_MISMATCH field reset value
#define NXMAC_TX_AC_0_LEN_MISMATCH_RST        0x0
/// TX_BCN_LEN_MISMATCH field reset value
#define NXMAC_TX_BCN_LEN_MISMATCH_RST         0x0

/**
 * @brief Unpacks DMA_STATUS_2's fields from current value of the DMA_STATUS_2 register.
 *
 * Reads the DMA_STATUS_2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txac3newheaderr - Will be populated with the current value of this field from the register.
 * @param[out] txac2newheaderr - Will be populated with the current value of this field from the register.
 * @param[out] txac1newheaderr - Will be populated with the current value of this field from the register.
 * @param[out] txac0newheaderr - Will be populated with the current value of this field from the register.
 * @param[out] txbcnnewheaderr - Will be populated with the current value of this field from the register.
 * @param[out] txac3buserr - Will be populated with the current value of this field from the register.
 * @param[out] txac2buserr - Will be populated with the current value of this field from the register.
 * @param[out] txac1buserr - Will be populated with the current value of this field from the register.
 * @param[out] txac0buserr - Will be populated with the current value of this field from the register.
 * @param[out] txbcnbuserr - Will be populated with the current value of this field from the register.
 * @param[out] txac3ptaddresserr - Will be populated with the current value of this field from the register.
 * @param[out] txac2ptaddresserr - Will be populated with the current value of this field from the register.
 * @param[out] txac1ptaddresserr - Will be populated with the current value of this field from the register.
 * @param[out] txac0ptaddresserr - Will be populated with the current value of this field from the register.
 * @param[out] txbcnptaddresserr - Will be populated with the current value of this field from the register.
 * @param[out] txac3nextpointererr - Will be populated with the current value of this field from the register.
 * @param[out] txac2nextpointererr - Will be populated with the current value of this field from the register.
 * @param[out] txac1nextpointererr - Will be populated with the current value of this field from the register.
 * @param[out] txac0nextpointererr - Will be populated with the current value of this field from the register.
 * @param[out] txbcnnextpointererr - Will be populated with the current value of this field from the register.
 * @param[out] txac3upatternerr - Will be populated with the current value of this field from the register.
 * @param[out] txac2upatternerr - Will be populated with the current value of this field from the register.
 * @param[out] txac1upatternerr - Will be populated with the current value of this field from the register.
 * @param[out] txac0upatternerr - Will be populated with the current value of this field from the register.
 * @param[out] txbcnupatternerr - Will be populated with the current value of this field from the register.
 * @param[out] txac3lenmismatch - Will be populated with the current value of this field from the register.
 * @param[out] txac2lenmismatch - Will be populated with the current value of this field from the register.
 * @param[out] txac1lenmismatch - Will be populated with the current value of this field from the register.
 * @param[out] txac0lenmismatch - Will be populated with the current value of this field from the register.
 * @param[out] txbcnlenmismatch - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_dma_status_2_unpack(uint8_t *txac3newheaderr, uint8_t *txac2newheaderr, uint8_t *txac1newheaderr, uint8_t *txac0newheaderr, uint8_t *txbcnnewheaderr, uint8_t *txac3buserr, uint8_t *txac2buserr, uint8_t *txac1buserr, uint8_t *txac0buserr, uint8_t *txbcnbuserr, uint8_t *txac3ptaddresserr, uint8_t *txac2ptaddresserr, uint8_t *txac1ptaddresserr, uint8_t *txac0ptaddresserr, uint8_t *txbcnptaddresserr, uint8_t *txac3nextpointererr, uint8_t *txac2nextpointererr, uint8_t *txac1nextpointererr, uint8_t *txac0nextpointererr, uint8_t *txbcnnextpointererr, uint8_t *txac3upatternerr, uint8_t *txac2upatternerr, uint8_t *txac1upatternerr, uint8_t *txac0upatternerr, uint8_t *txbcnupatternerr, uint8_t *txac3lenmismatch, uint8_t *txac2lenmismatch, uint8_t *txac1lenmismatch, uint8_t *txac0lenmismatch, uint8_t *txbcnlenmismatch)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);

    *txac3newheaderr = (localVal & ((uint32_t)0x20000000)) >> 29;
    *txac2newheaderr = (localVal & ((uint32_t)0x10000000)) >> 28;
    *txac1newheaderr = (localVal & ((uint32_t)0x08000000)) >> 27;
    *txac0newheaderr = (localVal & ((uint32_t)0x04000000)) >> 26;
    *txbcnnewheaderr = (localVal & ((uint32_t)0x02000000)) >> 25;
    *txac3buserr = (localVal & ((uint32_t)0x01000000)) >> 24;
    *txac2buserr = (localVal & ((uint32_t)0x00800000)) >> 23;
    *txac1buserr = (localVal & ((uint32_t)0x00400000)) >> 22;
    *txac0buserr = (localVal & ((uint32_t)0x00200000)) >> 21;
    *txbcnbuserr = (localVal & ((uint32_t)0x00100000)) >> 20;
    *txac3ptaddresserr = (localVal & ((uint32_t)0x00080000)) >> 19;
    *txac2ptaddresserr = (localVal & ((uint32_t)0x00040000)) >> 18;
    *txac1ptaddresserr = (localVal & ((uint32_t)0x00020000)) >> 17;
    *txac0ptaddresserr = (localVal & ((uint32_t)0x00010000)) >> 16;
    *txbcnptaddresserr = (localVal & ((uint32_t)0x00008000)) >> 15;
    *txac3nextpointererr = (localVal & ((uint32_t)0x00004000)) >> 14;
    *txac2nextpointererr = (localVal & ((uint32_t)0x00002000)) >> 13;
    *txac1nextpointererr = (localVal & ((uint32_t)0x00001000)) >> 12;
    *txac0nextpointererr = (localVal & ((uint32_t)0x00000800)) >> 11;
    *txbcnnextpointererr = (localVal & ((uint32_t)0x00000400)) >> 10;
    *txac3upatternerr = (localVal & ((uint32_t)0x00000200)) >> 9;
    *txac2upatternerr = (localVal & ((uint32_t)0x00000100)) >> 8;
    *txac1upatternerr = (localVal & ((uint32_t)0x00000080)) >> 7;
    *txac0upatternerr = (localVal & ((uint32_t)0x00000040)) >> 6;
    *txbcnupatternerr = (localVal & ((uint32_t)0x00000020)) >> 5;
    *txac3lenmismatch = (localVal & ((uint32_t)0x00000010)) >> 4;
    *txac2lenmismatch = (localVal & ((uint32_t)0x00000008)) >> 3;
    *txac1lenmismatch = (localVal & ((uint32_t)0x00000004)) >> 2;
    *txac0lenmismatch = (localVal & ((uint32_t)0x00000002)) >> 1;
    *txbcnlenmismatch = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the txAC3NewHeadErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC3NewHeadErr field's value will be returned.
 *
 * @return The current value of the txAC3NewHeadErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_3_new_head_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

/**
 * @brief Returns the current value of the txAC2NewHeadErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC2NewHeadErr field's value will be returned.
 *
 * @return The current value of the txAC2NewHeadErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_2_new_head_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Returns the current value of the txAC1NewHeadErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC1NewHeadErr field's value will be returned.
 *
 * @return The current value of the txAC1NewHeadErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_1_new_head_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Returns the current value of the txAC0NewHeadErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC0NewHeadErr field's value will be returned.
 *
 * @return The current value of the txAC0NewHeadErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_0_new_head_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

/**
 * @brief Returns the current value of the txBcnNewHeadErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txBcnNewHeadErr field's value will be returned.
 *
 * @return The current value of the txBcnNewHeadErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_bcn_new_head_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

/**
 * @brief Returns the current value of the txAC3BusErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC3BusErr field's value will be returned.
 *
 * @return The current value of the txAC3BusErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_3_bus_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Returns the current value of the txAC2BusErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC2BusErr field's value will be returned.
 *
 * @return The current value of the txAC2BusErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_2_bus_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Returns the current value of the txAC1BusErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC1BusErr field's value will be returned.
 *
 * @return The current value of the txAC1BusErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_1_bus_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Returns the current value of the txAC0BusErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC0BusErr field's value will be returned.
 *
 * @return The current value of the txAC0BusErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_0_bus_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Returns the current value of the txBcnBusErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txBcnBusErr field's value will be returned.
 *
 * @return The current value of the txBcnBusErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_bcn_bus_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Returns the current value of the txAC3PTAddressErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC3PTAddressErr field's value will be returned.
 *
 * @return The current value of the txAC3PTAddressErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_3pt_address_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Returns the current value of the txAC2PTAddressErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC2PTAddressErr field's value will be returned.
 *
 * @return The current value of the txAC2PTAddressErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_2pt_address_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Returns the current value of the txAC1PTAddressErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC1PTAddressErr field's value will be returned.
 *
 * @return The current value of the txAC1PTAddressErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_1pt_address_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Returns the current value of the txAC0PTAddressErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC0PTAddressErr field's value will be returned.
 *
 * @return The current value of the txAC0PTAddressErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_0pt_address_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Returns the current value of the txBcnPTAddressErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txBcnPTAddressErr field's value will be returned.
 *
 * @return The current value of the txBcnPTAddressErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_bcn_pt_address_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

/**
 * @brief Returns the current value of the txAC3NextPointerErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC3NextPointerErr field's value will be returned.
 *
 * @return The current value of the txAC3NextPointerErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_3_next_pointer_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Returns the current value of the txAC2NextPointerErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC2NextPointerErr field's value will be returned.
 *
 * @return The current value of the txAC2NextPointerErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_2_next_pointer_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Returns the current value of the txAC1NextPointerErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC1NextPointerErr field's value will be returned.
 *
 * @return The current value of the txAC1NextPointerErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_1_next_pointer_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Returns the current value of the txAC0NextPointerErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC0NextPointerErr field's value will be returned.
 *
 * @return The current value of the txAC0NextPointerErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_0_next_pointer_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Returns the current value of the txBcnNextPointerErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txBcnNextPointerErr field's value will be returned.
 *
 * @return The current value of the txBcnNextPointerErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_bcn_next_pointer_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Returns the current value of the txAC3UPatternErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC3UPatternErr field's value will be returned.
 *
 * @return The current value of the txAC3UPatternErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_3u_pattern_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Returns the current value of the txAC2UPatternErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC2UPatternErr field's value will be returned.
 *
 * @return The current value of the txAC2UPatternErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_2u_pattern_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Returns the current value of the txAC1UPatternErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC1UPatternErr field's value will be returned.
 *
 * @return The current value of the txAC1UPatternErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_1u_pattern_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Returns the current value of the txAC0UPatternErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC0UPatternErr field's value will be returned.
 *
 * @return The current value of the txAC0UPatternErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_0u_pattern_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Returns the current value of the txBcnUPatternErr field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txBcnUPatternErr field's value will be returned.
 *
 * @return The current value of the txBcnUPatternErr field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_bcn_u_pattern_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Returns the current value of the txAC3LenMismatch field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC3LenMismatch field's value will be returned.
 *
 * @return The current value of the txAC3LenMismatch field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_3_len_mismatch_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Returns the current value of the txAC2LenMismatch field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC2LenMismatch field's value will be returned.
 *
 * @return The current value of the txAC2LenMismatch field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_2_len_mismatch_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Returns the current value of the txAC1LenMismatch field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC1LenMismatch field's value will be returned.
 *
 * @return The current value of the txAC1LenMismatch field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_1_len_mismatch_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Returns the current value of the txAC0LenMismatch field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txAC0LenMismatch field's value will be returned.
 *
 * @return The current value of the txAC0LenMismatch field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_ac_0_len_mismatch_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Returns the current value of the txBcnLenMismatch field in the DMA_STATUS_2 register.
 *
 * The DMA_STATUS_2 register will be read and the txBcnLenMismatch field's value will be returned.
 *
 * @return The current value of the txBcnLenMismatch field in the DMA_STATUS_2 register.
 */
__INLINE uint8_t nxmac_tx_bcn_len_mismatch_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/// @}

/**
 * @name DMA_STATUS_3 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     07      rxPayNewHeadErr   0
 *     06      rxHdrNewHeadErr   0
 *     05          rxPayBusErr   0
 *     04          rxHdrBusErr   0
 *     03   rxPayNextPointerErr   0
 *     02   rxHdrNextPointerErr   0
 *     01     rxPayUPatternErr   0
 *     00     rxHdrUPatternErr   0
 * </pre>
 *
 * @{
 */

/// Address of the DMA_STATUS_3 register
#define NXMAC_DMA_STATUS_3_ADDR   0xC0008190
/// Offset of the DMA_STATUS_3 register from the base address
#define NXMAC_DMA_STATUS_3_OFFSET 0x00000190
/// Index of the DMA_STATUS_3 register
#define NXMAC_DMA_STATUS_3_INDEX  0x00000064
/// Reset value of the DMA_STATUS_3 register
#define NXMAC_DMA_STATUS_3_RESET  0x00000000

/**
 * @brief Returns the current value of the DMA_STATUS_3 register.
 * The DMA_STATUS_3 register will be read and its value returned.
 * @return The current value of the DMA_STATUS_3 register.
 */
__INLINE uint32_t nxmac_dma_status_3_get(void)
{
    return REG_PL_RD(NXMAC_DMA_STATUS_3_ADDR);
}

// field definitions
/// RX_PAY_NEW_HEAD_ERR field bit
#define NXMAC_RX_PAY_NEW_HEAD_ERR_BIT        ((uint32_t)0x00000080)
/// RX_PAY_NEW_HEAD_ERR field position
#define NXMAC_RX_PAY_NEW_HEAD_ERR_POS        7
/// RX_HDR_NEW_HEAD_ERR field bit
#define NXMAC_RX_HDR_NEW_HEAD_ERR_BIT        ((uint32_t)0x00000040)
/// RX_HDR_NEW_HEAD_ERR field position
#define NXMAC_RX_HDR_NEW_HEAD_ERR_POS        6
/// RX_PAY_BUS_ERR field bit
#define NXMAC_RX_PAY_BUS_ERR_BIT             ((uint32_t)0x00000020)
/// RX_PAY_BUS_ERR field position
#define NXMAC_RX_PAY_BUS_ERR_POS             5
/// RX_HDR_BUS_ERR field bit
#define NXMAC_RX_HDR_BUS_ERR_BIT             ((uint32_t)0x00000010)
/// RX_HDR_BUS_ERR field position
#define NXMAC_RX_HDR_BUS_ERR_POS             4
/// RX_PAY_NEXT_POINTER_ERR field bit
#define NXMAC_RX_PAY_NEXT_POINTER_ERR_BIT    ((uint32_t)0x00000008)
/// RX_PAY_NEXT_POINTER_ERR field position
#define NXMAC_RX_PAY_NEXT_POINTER_ERR_POS    3
/// RX_HDR_NEXT_POINTER_ERR field bit
#define NXMAC_RX_HDR_NEXT_POINTER_ERR_BIT    ((uint32_t)0x00000004)
/// RX_HDR_NEXT_POINTER_ERR field position
#define NXMAC_RX_HDR_NEXT_POINTER_ERR_POS    2
/// RX_PAY_U_PATTERN_ERR field bit
#define NXMAC_RX_PAY_U_PATTERN_ERR_BIT       ((uint32_t)0x00000002)
/// RX_PAY_U_PATTERN_ERR field position
#define NXMAC_RX_PAY_U_PATTERN_ERR_POS       1
/// RX_HDR_U_PATTERN_ERR field bit
#define NXMAC_RX_HDR_U_PATTERN_ERR_BIT       ((uint32_t)0x00000001)
/// RX_HDR_U_PATTERN_ERR field position
#define NXMAC_RX_HDR_U_PATTERN_ERR_POS       0

/// RX_PAY_NEW_HEAD_ERR field reset value
#define NXMAC_RX_PAY_NEW_HEAD_ERR_RST        0x0
/// RX_HDR_NEW_HEAD_ERR field reset value
#define NXMAC_RX_HDR_NEW_HEAD_ERR_RST        0x0
/// RX_PAY_BUS_ERR field reset value
#define NXMAC_RX_PAY_BUS_ERR_RST             0x0
/// RX_HDR_BUS_ERR field reset value
#define NXMAC_RX_HDR_BUS_ERR_RST             0x0
/// RX_PAY_NEXT_POINTER_ERR field reset value
#define NXMAC_RX_PAY_NEXT_POINTER_ERR_RST    0x0
/// RX_HDR_NEXT_POINTER_ERR field reset value
#define NXMAC_RX_HDR_NEXT_POINTER_ERR_RST    0x0
/// RX_PAY_U_PATTERN_ERR field reset value
#define NXMAC_RX_PAY_U_PATTERN_ERR_RST       0x0
/// RX_HDR_U_PATTERN_ERR field reset value
#define NXMAC_RX_HDR_U_PATTERN_ERR_RST       0x0

/**
 * @brief Unpacks DMA_STATUS_3's fields from current value of the DMA_STATUS_3 register.
 *
 * Reads the DMA_STATUS_3 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxpaynewheaderr - Will be populated with the current value of this field from the register.
 * @param[out] rxhdrnewheaderr - Will be populated with the current value of this field from the register.
 * @param[out] rxpaybuserr - Will be populated with the current value of this field from the register.
 * @param[out] rxhdrbuserr - Will be populated with the current value of this field from the register.
 * @param[out] rxpaynextpointererr - Will be populated with the current value of this field from the register.
 * @param[out] rxhdrnextpointererr - Will be populated with the current value of this field from the register.
 * @param[out] rxpayupatternerr - Will be populated with the current value of this field from the register.
 * @param[out] rxhdrupatternerr - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_dma_status_3_unpack(uint8_t *rxpaynewheaderr, uint8_t *rxhdrnewheaderr, uint8_t *rxpaybuserr, uint8_t *rxhdrbuserr, uint8_t *rxpaynextpointererr, uint8_t *rxhdrnextpointererr, uint8_t *rxpayupatternerr, uint8_t *rxhdrupatternerr)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_3_ADDR);

    *rxpaynewheaderr = (localVal & ((uint32_t)0x00000080)) >> 7;
    *rxhdrnewheaderr = (localVal & ((uint32_t)0x00000040)) >> 6;
    *rxpaybuserr = (localVal & ((uint32_t)0x00000020)) >> 5;
    *rxhdrbuserr = (localVal & ((uint32_t)0x00000010)) >> 4;
    *rxpaynextpointererr = (localVal & ((uint32_t)0x00000008)) >> 3;
    *rxhdrnextpointererr = (localVal & ((uint32_t)0x00000004)) >> 2;
    *rxpayupatternerr = (localVal & ((uint32_t)0x00000002)) >> 1;
    *rxhdrupatternerr = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the rxPayNewHeadErr field in the DMA_STATUS_3 register.
 *
 * The DMA_STATUS_3 register will be read and the rxPayNewHeadErr field's value will be returned.
 *
 * @return The current value of the rxPayNewHeadErr field in the DMA_STATUS_3 register.
 */
__INLINE uint8_t nxmac_rx_pay_new_head_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_3_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Returns the current value of the rxHdrNewHeadErr field in the DMA_STATUS_3 register.
 *
 * The DMA_STATUS_3 register will be read and the rxHdrNewHeadErr field's value will be returned.
 *
 * @return The current value of the rxHdrNewHeadErr field in the DMA_STATUS_3 register.
 */
__INLINE uint8_t nxmac_rx_hdr_new_head_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_3_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Returns the current value of the rxPayBusErr field in the DMA_STATUS_3 register.
 *
 * The DMA_STATUS_3 register will be read and the rxPayBusErr field's value will be returned.
 *
 * @return The current value of the rxPayBusErr field in the DMA_STATUS_3 register.
 */
__INLINE uint8_t nxmac_rx_pay_bus_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_3_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Returns the current value of the rxHdrBusErr field in the DMA_STATUS_3 register.
 *
 * The DMA_STATUS_3 register will be read and the rxHdrBusErr field's value will be returned.
 *
 * @return The current value of the rxHdrBusErr field in the DMA_STATUS_3 register.
 */
__INLINE uint8_t nxmac_rx_hdr_bus_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_3_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Returns the current value of the rxPayNextPointerErr field in the DMA_STATUS_3 register.
 *
 * The DMA_STATUS_3 register will be read and the rxPayNextPointerErr field's value will be returned.
 *
 * @return The current value of the rxPayNextPointerErr field in the DMA_STATUS_3 register.
 */
__INLINE uint8_t nxmac_rx_pay_next_pointer_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_3_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Returns the current value of the rxHdrNextPointerErr field in the DMA_STATUS_3 register.
 *
 * The DMA_STATUS_3 register will be read and the rxHdrNextPointerErr field's value will be returned.
 *
 * @return The current value of the rxHdrNextPointerErr field in the DMA_STATUS_3 register.
 */
__INLINE uint8_t nxmac_rx_hdr_next_pointer_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_3_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Returns the current value of the rxPayUPatternErr field in the DMA_STATUS_3 register.
 *
 * The DMA_STATUS_3 register will be read and the rxPayUPatternErr field's value will be returned.
 *
 * @return The current value of the rxPayUPatternErr field in the DMA_STATUS_3 register.
 */
__INLINE uint8_t nxmac_rx_pay_u_pattern_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_3_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Returns the current value of the rxHdrUPatternErr field in the DMA_STATUS_3 register.
 *
 * The DMA_STATUS_3 register will be read and the rxHdrUPatternErr field's value will be returned.
 *
 * @return The current value of the rxHdrUPatternErr field in the DMA_STATUS_3 register.
 */
__INLINE uint8_t nxmac_rx_hdr_u_pattern_err_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_3_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/// @}

/**
 * @name DMA_STATUS_4 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     14   txAC3HaltAfterTXOP   0
 *     13   txAC2HaltAfterTXOP   0
 *     12   txAC1HaltAfterTXOP   0
 *     11   txAC0HaltAfterTXOP   0
 *     10   txBcnHaltAfterTXOP   0
 *     09            txAC3EndQ   0
 *     08            txAC2EndQ   0
 *     07            txAC1EndQ   0
 *     06            txAC0EndQ   0
 *     05            txBcnEndQ   0
 *     04         txAC3Startup   1
 *     03         txAC2Startup   1
 *     02         txAC1Startup   1
 *     01         txAC0Startup   1
 *     00         txBcnStartup   1
 * </pre>
 *
 * @{
 */

/// Address of the DMA_STATUS_4 register
#define NXMAC_DMA_STATUS_4_ADDR   0xC0008194
/// Offset of the DMA_STATUS_4 register from the base address
#define NXMAC_DMA_STATUS_4_OFFSET 0x00000194
/// Index of the DMA_STATUS_4 register
#define NXMAC_DMA_STATUS_4_INDEX  0x00000065
/// Reset value of the DMA_STATUS_4 register
#define NXMAC_DMA_STATUS_4_RESET  0x0000001F

/**
 * @brief Returns the current value of the DMA_STATUS_4 register.
 * The DMA_STATUS_4 register will be read and its value returned.
 * @return The current value of the DMA_STATUS_4 register.
 */
__INLINE uint32_t nxmac_dma_status_4_get(void)
{
    return REG_PL_RD(NXMAC_DMA_STATUS_4_ADDR);
}

// field definitions
/// TX_AC_3_HALT_AFTER_TXOP field bit
#define NXMAC_TX_AC_3_HALT_AFTER_TXOP_BIT    ((uint32_t)0x00004000)
/// TX_AC_3_HALT_AFTER_TXOP field position
#define NXMAC_TX_AC_3_HALT_AFTER_TXOP_POS    14
/// TX_AC_2_HALT_AFTER_TXOP field bit
#define NXMAC_TX_AC_2_HALT_AFTER_TXOP_BIT    ((uint32_t)0x00002000)
/// TX_AC_2_HALT_AFTER_TXOP field position
#define NXMAC_TX_AC_2_HALT_AFTER_TXOP_POS    13
/// TX_AC_1_HALT_AFTER_TXOP field bit
#define NXMAC_TX_AC_1_HALT_AFTER_TXOP_BIT    ((uint32_t)0x00001000)
/// TX_AC_1_HALT_AFTER_TXOP field position
#define NXMAC_TX_AC_1_HALT_AFTER_TXOP_POS    12
/// TX_AC_0_HALT_AFTER_TXOP field bit
#define NXMAC_TX_AC_0_HALT_AFTER_TXOP_BIT    ((uint32_t)0x00000800)
/// TX_AC_0_HALT_AFTER_TXOP field position
#define NXMAC_TX_AC_0_HALT_AFTER_TXOP_POS    11
/// TX_BCN_HALT_AFTER_TXOP field bit
#define NXMAC_TX_BCN_HALT_AFTER_TXOP_BIT     ((uint32_t)0x00000400)
/// TX_BCN_HALT_AFTER_TXOP field position
#define NXMAC_TX_BCN_HALT_AFTER_TXOP_POS     10
/// TX_AC_3_END_Q field bit
#define NXMAC_TX_AC_3_END_Q_BIT              ((uint32_t)0x00000200)
/// TX_AC_3_END_Q field position
#define NXMAC_TX_AC_3_END_Q_POS              9
/// TX_AC_2_END_Q field bit
#define NXMAC_TX_AC_2_END_Q_BIT              ((uint32_t)0x00000100)
/// TX_AC_2_END_Q field position
#define NXMAC_TX_AC_2_END_Q_POS              8
/// TX_AC_1_END_Q field bit
#define NXMAC_TX_AC_1_END_Q_BIT              ((uint32_t)0x00000080)
/// TX_AC_1_END_Q field position
#define NXMAC_TX_AC_1_END_Q_POS              7
/// TX_AC_0_END_Q field bit
#define NXMAC_TX_AC_0_END_Q_BIT              ((uint32_t)0x00000040)
/// TX_AC_0_END_Q field position
#define NXMAC_TX_AC_0_END_Q_POS              6
/// TX_BCN_END_Q field bit
#define NXMAC_TX_BCN_END_Q_BIT               ((uint32_t)0x00000020)
/// TX_BCN_END_Q field position
#define NXMAC_TX_BCN_END_Q_POS               5
/// TX_AC_3_STARTUP field bit
#define NXMAC_TX_AC_3_STARTUP_BIT            ((uint32_t)0x00000010)
/// TX_AC_3_STARTUP field position
#define NXMAC_TX_AC_3_STARTUP_POS            4
/// TX_AC_2_STARTUP field bit
#define NXMAC_TX_AC_2_STARTUP_BIT            ((uint32_t)0x00000008)
/// TX_AC_2_STARTUP field position
#define NXMAC_TX_AC_2_STARTUP_POS            3
/// TX_AC_1_STARTUP field bit
#define NXMAC_TX_AC_1_STARTUP_BIT            ((uint32_t)0x00000004)
/// TX_AC_1_STARTUP field position
#define NXMAC_TX_AC_1_STARTUP_POS            2
/// TX_AC_0_STARTUP field bit
#define NXMAC_TX_AC_0_STARTUP_BIT            ((uint32_t)0x00000002)
/// TX_AC_0_STARTUP field position
#define NXMAC_TX_AC_0_STARTUP_POS            1
/// TX_BCN_STARTUP field bit
#define NXMAC_TX_BCN_STARTUP_BIT             ((uint32_t)0x00000001)
/// TX_BCN_STARTUP field position
#define NXMAC_TX_BCN_STARTUP_POS             0

/// TX_AC_3_HALT_AFTER_TXOP field reset value
#define NXMAC_TX_AC_3_HALT_AFTER_TXOP_RST    0x0
/// TX_AC_2_HALT_AFTER_TXOP field reset value
#define NXMAC_TX_AC_2_HALT_AFTER_TXOP_RST    0x0
/// TX_AC_1_HALT_AFTER_TXOP field reset value
#define NXMAC_TX_AC_1_HALT_AFTER_TXOP_RST    0x0
/// TX_AC_0_HALT_AFTER_TXOP field reset value
#define NXMAC_TX_AC_0_HALT_AFTER_TXOP_RST    0x0
/// TX_BCN_HALT_AFTER_TXOP field reset value
#define NXMAC_TX_BCN_HALT_AFTER_TXOP_RST     0x0
/// TX_AC_3_END_Q field reset value
#define NXMAC_TX_AC_3_END_Q_RST              0x0
/// TX_AC_2_END_Q field reset value
#define NXMAC_TX_AC_2_END_Q_RST              0x0
/// TX_AC_1_END_Q field reset value
#define NXMAC_TX_AC_1_END_Q_RST              0x0
/// TX_AC_0_END_Q field reset value
#define NXMAC_TX_AC_0_END_Q_RST              0x0
/// TX_BCN_END_Q field reset value
#define NXMAC_TX_BCN_END_Q_RST               0x0
/// TX_AC_3_STARTUP field reset value
#define NXMAC_TX_AC_3_STARTUP_RST            0x1
/// TX_AC_2_STARTUP field reset value
#define NXMAC_TX_AC_2_STARTUP_RST            0x1
/// TX_AC_1_STARTUP field reset value
#define NXMAC_TX_AC_1_STARTUP_RST            0x1
/// TX_AC_0_STARTUP field reset value
#define NXMAC_TX_AC_0_STARTUP_RST            0x1
/// TX_BCN_STARTUP field reset value
#define NXMAC_TX_BCN_STARTUP_RST             0x1

/**
 * @brief Unpacks DMA_STATUS_4's fields from current value of the DMA_STATUS_4 register.
 *
 * Reads the DMA_STATUS_4 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txac3haltaftertxop - Will be populated with the current value of this field from the register.
 * @param[out] txac2haltaftertxop - Will be populated with the current value of this field from the register.
 * @param[out] txac1haltaftertxop - Will be populated with the current value of this field from the register.
 * @param[out] txac0haltaftertxop - Will be populated with the current value of this field from the register.
 * @param[out] txbcnhaltaftertxop - Will be populated with the current value of this field from the register.
 * @param[out] txac3endq - Will be populated with the current value of this field from the register.
 * @param[out] txac2endq - Will be populated with the current value of this field from the register.
 * @param[out] txac1endq - Will be populated with the current value of this field from the register.
 * @param[out] txac0endq - Will be populated with the current value of this field from the register.
 * @param[out] txbcnendq - Will be populated with the current value of this field from the register.
 * @param[out] txac3startup - Will be populated with the current value of this field from the register.
 * @param[out] txac2startup - Will be populated with the current value of this field from the register.
 * @param[out] txac1startup - Will be populated with the current value of this field from the register.
 * @param[out] txac0startup - Will be populated with the current value of this field from the register.
 * @param[out] txbcnstartup - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_dma_status_4_unpack(uint8_t *txac3haltaftertxop, uint8_t *txac2haltaftertxop, uint8_t *txac1haltaftertxop, uint8_t *txac0haltaftertxop, uint8_t *txbcnhaltaftertxop, uint8_t *txac3endq, uint8_t *txac2endq, uint8_t *txac1endq, uint8_t *txac0endq, uint8_t *txbcnendq, uint8_t *txac3startup, uint8_t *txac2startup, uint8_t *txac1startup, uint8_t *txac0startup, uint8_t *txbcnstartup)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_4_ADDR);

    *txac3haltaftertxop = (localVal & ((uint32_t)0x00004000)) >> 14;
    *txac2haltaftertxop = (localVal & ((uint32_t)0x00002000)) >> 13;
    *txac1haltaftertxop = (localVal & ((uint32_t)0x00001000)) >> 12;
    *txac0haltaftertxop = (localVal & ((uint32_t)0x00000800)) >> 11;
    *txbcnhaltaftertxop = (localVal & ((uint32_t)0x00000400)) >> 10;
    *txac3endq = (localVal & ((uint32_t)0x00000200)) >> 9;
    *txac2endq = (localVal & ((uint32_t)0x00000100)) >> 8;
    *txac1endq = (localVal & ((uint32_t)0x00000080)) >> 7;
    *txac0endq = (localVal & ((uint32_t)0x00000040)) >> 6;
    *txbcnendq = (localVal & ((uint32_t)0x00000020)) >> 5;
    *txac3startup = (localVal & ((uint32_t)0x00000010)) >> 4;
    *txac2startup = (localVal & ((uint32_t)0x00000008)) >> 3;
    *txac1startup = (localVal & ((uint32_t)0x00000004)) >> 2;
    *txac0startup = (localVal & ((uint32_t)0x00000002)) >> 1;
    *txbcnstartup = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the txAC3HaltAfterTXOP field in the DMA_STATUS_4 register.
 *
 * The DMA_STATUS_4 register will be read and the txAC3HaltAfterTXOP field's value will be returned.
 *
 * @return The current value of the txAC3HaltAfterTXOP field in the DMA_STATUS_4 register.
 */
__INLINE uint8_t nxmac_tx_ac_3_halt_after_txop_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_4_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Returns the current value of the txAC2HaltAfterTXOP field in the DMA_STATUS_4 register.
 *
 * The DMA_STATUS_4 register will be read and the txAC2HaltAfterTXOP field's value will be returned.
 *
 * @return The current value of the txAC2HaltAfterTXOP field in the DMA_STATUS_4 register.
 */
__INLINE uint8_t nxmac_tx_ac_2_halt_after_txop_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_4_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Returns the current value of the txAC1HaltAfterTXOP field in the DMA_STATUS_4 register.
 *
 * The DMA_STATUS_4 register will be read and the txAC1HaltAfterTXOP field's value will be returned.
 *
 * @return The current value of the txAC1HaltAfterTXOP field in the DMA_STATUS_4 register.
 */
__INLINE uint8_t nxmac_tx_ac_1_halt_after_txop_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_4_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Returns the current value of the txAC0HaltAfterTXOP field in the DMA_STATUS_4 register.
 *
 * The DMA_STATUS_4 register will be read and the txAC0HaltAfterTXOP field's value will be returned.
 *
 * @return The current value of the txAC0HaltAfterTXOP field in the DMA_STATUS_4 register.
 */
__INLINE uint8_t nxmac_tx_ac_0_halt_after_txop_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_4_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Returns the current value of the txBcnHaltAfterTXOP field in the DMA_STATUS_4 register.
 *
 * The DMA_STATUS_4 register will be read and the txBcnHaltAfterTXOP field's value will be returned.
 *
 * @return The current value of the txBcnHaltAfterTXOP field in the DMA_STATUS_4 register.
 */
__INLINE uint8_t nxmac_tx_bcn_halt_after_txop_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_4_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Returns the current value of the txAC3EndQ field in the DMA_STATUS_4 register.
 *
 * The DMA_STATUS_4 register will be read and the txAC3EndQ field's value will be returned.
 *
 * @return The current value of the txAC3EndQ field in the DMA_STATUS_4 register.
 */
__INLINE uint8_t nxmac_tx_ac_3_end_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_4_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Returns the current value of the txAC2EndQ field in the DMA_STATUS_4 register.
 *
 * The DMA_STATUS_4 register will be read and the txAC2EndQ field's value will be returned.
 *
 * @return The current value of the txAC2EndQ field in the DMA_STATUS_4 register.
 */
__INLINE uint8_t nxmac_tx_ac_2_end_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_4_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Returns the current value of the txAC1EndQ field in the DMA_STATUS_4 register.
 *
 * The DMA_STATUS_4 register will be read and the txAC1EndQ field's value will be returned.
 *
 * @return The current value of the txAC1EndQ field in the DMA_STATUS_4 register.
 */
__INLINE uint8_t nxmac_tx_ac_1_end_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_4_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Returns the current value of the txAC0EndQ field in the DMA_STATUS_4 register.
 *
 * The DMA_STATUS_4 register will be read and the txAC0EndQ field's value will be returned.
 *
 * @return The current value of the txAC0EndQ field in the DMA_STATUS_4 register.
 */
__INLINE uint8_t nxmac_tx_ac_0_end_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_4_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Returns the current value of the txBcnEndQ field in the DMA_STATUS_4 register.
 *
 * The DMA_STATUS_4 register will be read and the txBcnEndQ field's value will be returned.
 *
 * @return The current value of the txBcnEndQ field in the DMA_STATUS_4 register.
 */
__INLINE uint8_t nxmac_tx_bcn_end_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_4_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Returns the current value of the txAC3Startup field in the DMA_STATUS_4 register.
 *
 * The DMA_STATUS_4 register will be read and the txAC3Startup field's value will be returned.
 *
 * @return The current value of the txAC3Startup field in the DMA_STATUS_4 register.
 */
__INLINE uint8_t nxmac_tx_ac_3_startup_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_4_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Returns the current value of the txAC2Startup field in the DMA_STATUS_4 register.
 *
 * The DMA_STATUS_4 register will be read and the txAC2Startup field's value will be returned.
 *
 * @return The current value of the txAC2Startup field in the DMA_STATUS_4 register.
 */
__INLINE uint8_t nxmac_tx_ac_2_startup_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_4_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Returns the current value of the txAC1Startup field in the DMA_STATUS_4 register.
 *
 * The DMA_STATUS_4 register will be read and the txAC1Startup field's value will be returned.
 *
 * @return The current value of the txAC1Startup field in the DMA_STATUS_4 register.
 */
__INLINE uint8_t nxmac_tx_ac_1_startup_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_4_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Returns the current value of the txAC0Startup field in the DMA_STATUS_4 register.
 *
 * The DMA_STATUS_4 register will be read and the txAC0Startup field's value will be returned.
 *
 * @return The current value of the txAC0Startup field in the DMA_STATUS_4 register.
 */
__INLINE uint8_t nxmac_tx_ac_0_startup_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_4_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Returns the current value of the txBcnStartup field in the DMA_STATUS_4 register.
 *
 * The DMA_STATUS_4 register will be read and the txBcnStartup field's value will be returned.
 *
 * @return The current value of the txBcnStartup field in the DMA_STATUS_4 register.
 */
__INLINE uint8_t nxmac_tx_bcn_startup_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_STATUS_4_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/// @}

/**
 * @name TX_BCN_HEAD_PTR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00         txBcnHeadPtr   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TX_BCN_HEAD_PTR register
#define NXMAC_TX_BCN_HEAD_PTR_ADDR   0xC0008198
/// Offset of the TX_BCN_HEAD_PTR register from the base address
#define NXMAC_TX_BCN_HEAD_PTR_OFFSET 0x00000198
/// Index of the TX_BCN_HEAD_PTR register
#define NXMAC_TX_BCN_HEAD_PTR_INDEX  0x00000066
/// Reset value of the TX_BCN_HEAD_PTR register
#define NXMAC_TX_BCN_HEAD_PTR_RESET  0x00000000

/**
 * @brief Returns the current value of the TX_BCN_HEAD_PTR register.
 * The TX_BCN_HEAD_PTR register will be read and its value returned.
 * @return The current value of the TX_BCN_HEAD_PTR register.
 */
__INLINE uint32_t nxmac_tx_bcn_head_ptr_get(void)
{
    return REG_PL_RD(NXMAC_TX_BCN_HEAD_PTR_ADDR);
}

/**
 * @brief Sets the TX_BCN_HEAD_PTR register to a value.
 * The TX_BCN_HEAD_PTR register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_tx_bcn_head_ptr_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TX_BCN_HEAD_PTR_ADDR, value);
}

// field definitions
/// TX_BCN_HEAD_PTR field mask
#define NXMAC_TX_BCN_HEAD_PTR_MASK   ((uint32_t)0xFFFFFFFF)
/// TX_BCN_HEAD_PTR field LSB position
#define NXMAC_TX_BCN_HEAD_PTR_LSB    0
/// TX_BCN_HEAD_PTR field width
#define NXMAC_TX_BCN_HEAD_PTR_WIDTH  ((uint32_t)0x00000020)

/// TX_BCN_HEAD_PTR field reset value
#define NXMAC_TX_BCN_HEAD_PTR_RST    0x0

/**
 * @brief Returns the current value of the txBcnHeadPtr field in the TX_BCN_HEAD_PTR register.
 *
 * The TX_BCN_HEAD_PTR register will be read and the txBcnHeadPtr field's value will be returned.
 *
 * @return The current value of the txBcnHeadPtr field in the TX_BCN_HEAD_PTR register.
 */
__INLINE uint32_t nxmac_tx_bcn_head_ptr_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_BCN_HEAD_PTR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the txBcnHeadPtr field of the TX_BCN_HEAD_PTR register.
 *
 * The TX_BCN_HEAD_PTR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txbcnheadptr - The value to set the field to.
 */
__INLINE void nxmac_tx_bcn_head_ptr_setf(uint32_t txbcnheadptr)
{
    ASSERT_ERR((((uint32_t)txbcnheadptr << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_TX_BCN_HEAD_PTR_ADDR, (uint32_t)txbcnheadptr << 0);
}

/// @}

/**
 * @name TX_AC_0_HEAD_PTR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00         txAC0HeadPtr   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TX_AC_0_HEAD_PTR register
#define NXMAC_TX_AC_0_HEAD_PTR_ADDR   0xC000819C
/// Offset of the TX_AC_0_HEAD_PTR register from the base address
#define NXMAC_TX_AC_0_HEAD_PTR_OFFSET 0x0000019C
/// Index of the TX_AC_0_HEAD_PTR register
#define NXMAC_TX_AC_0_HEAD_PTR_INDEX  0x00000067
/// Reset value of the TX_AC_0_HEAD_PTR register
#define NXMAC_TX_AC_0_HEAD_PTR_RESET  0x00000000

/**
 * @brief Returns the current value of the TX_AC_0_HEAD_PTR register.
 * The TX_AC_0_HEAD_PTR register will be read and its value returned.
 * @return The current value of the TX_AC_0_HEAD_PTR register.
 */
__INLINE uint32_t nxmac_tx_ac_0_head_ptr_get(void)
{
    return REG_PL_RD(NXMAC_TX_AC_0_HEAD_PTR_ADDR);
}

/**
 * @brief Sets the TX_AC_0_HEAD_PTR register to a value.
 * The TX_AC_0_HEAD_PTR register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_tx_ac_0_head_ptr_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TX_AC_0_HEAD_PTR_ADDR, value);
}

// field definitions
/// TX_AC_0_HEAD_PTR field mask
#define NXMAC_TX_AC_0_HEAD_PTR_MASK   ((uint32_t)0xFFFFFFFF)
/// TX_AC_0_HEAD_PTR field LSB position
#define NXMAC_TX_AC_0_HEAD_PTR_LSB    0
/// TX_AC_0_HEAD_PTR field width
#define NXMAC_TX_AC_0_HEAD_PTR_WIDTH  ((uint32_t)0x00000020)

/// TX_AC_0_HEAD_PTR field reset value
#define NXMAC_TX_AC_0_HEAD_PTR_RST    0x0

/**
 * @brief Returns the current value of the txAC0HeadPtr field in the TX_AC_0_HEAD_PTR register.
 *
 * The TX_AC_0_HEAD_PTR register will be read and the txAC0HeadPtr field's value will be returned.
 *
 * @return The current value of the txAC0HeadPtr field in the TX_AC_0_HEAD_PTR register.
 */
__INLINE uint32_t nxmac_tx_ac_0_head_ptr_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_AC_0_HEAD_PTR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the txAC0HeadPtr field of the TX_AC_0_HEAD_PTR register.
 *
 * The TX_AC_0_HEAD_PTR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac0headptr - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_0_head_ptr_setf(uint32_t txac0headptr)
{
    ASSERT_ERR((((uint32_t)txac0headptr << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_TX_AC_0_HEAD_PTR_ADDR, (uint32_t)txac0headptr << 0);
}

/// @}

/**
 * @name TX_AC_1_HEAD_PTR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00         txAC1HeadPtr   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TX_AC_1_HEAD_PTR register
#define NXMAC_TX_AC_1_HEAD_PTR_ADDR   0xC00081A0
/// Offset of the TX_AC_1_HEAD_PTR register from the base address
#define NXMAC_TX_AC_1_HEAD_PTR_OFFSET 0x000001A0
/// Index of the TX_AC_1_HEAD_PTR register
#define NXMAC_TX_AC_1_HEAD_PTR_INDEX  0x00000068
/// Reset value of the TX_AC_1_HEAD_PTR register
#define NXMAC_TX_AC_1_HEAD_PTR_RESET  0x00000000

/**
 * @brief Returns the current value of the TX_AC_1_HEAD_PTR register.
 * The TX_AC_1_HEAD_PTR register will be read and its value returned.
 * @return The current value of the TX_AC_1_HEAD_PTR register.
 */
__INLINE uint32_t nxmac_tx_ac_1_head_ptr_get(void)
{
    return REG_PL_RD(NXMAC_TX_AC_1_HEAD_PTR_ADDR);
}

/**
 * @brief Sets the TX_AC_1_HEAD_PTR register to a value.
 * The TX_AC_1_HEAD_PTR register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_tx_ac_1_head_ptr_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TX_AC_1_HEAD_PTR_ADDR, value);
}

// field definitions
/// TX_AC_1_HEAD_PTR field mask
#define NXMAC_TX_AC_1_HEAD_PTR_MASK   ((uint32_t)0xFFFFFFFF)
/// TX_AC_1_HEAD_PTR field LSB position
#define NXMAC_TX_AC_1_HEAD_PTR_LSB    0
/// TX_AC_1_HEAD_PTR field width
#define NXMAC_TX_AC_1_HEAD_PTR_WIDTH  ((uint32_t)0x00000020)

/// TX_AC_1_HEAD_PTR field reset value
#define NXMAC_TX_AC_1_HEAD_PTR_RST    0x0

/**
 * @brief Returns the current value of the txAC1HeadPtr field in the TX_AC_1_HEAD_PTR register.
 *
 * The TX_AC_1_HEAD_PTR register will be read and the txAC1HeadPtr field's value will be returned.
 *
 * @return The current value of the txAC1HeadPtr field in the TX_AC_1_HEAD_PTR register.
 */
__INLINE uint32_t nxmac_tx_ac_1_head_ptr_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_AC_1_HEAD_PTR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the txAC1HeadPtr field of the TX_AC_1_HEAD_PTR register.
 *
 * The TX_AC_1_HEAD_PTR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac1headptr - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_1_head_ptr_setf(uint32_t txac1headptr)
{
    ASSERT_ERR((((uint32_t)txac1headptr << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_TX_AC_1_HEAD_PTR_ADDR, (uint32_t)txac1headptr << 0);
}

/// @}

/**
 * @name TX_AC_2_HEAD_PTR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00         txAC2HeadPtr   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TX_AC_2_HEAD_PTR register
#define NXMAC_TX_AC_2_HEAD_PTR_ADDR   0xC00081A4
/// Offset of the TX_AC_2_HEAD_PTR register from the base address
#define NXMAC_TX_AC_2_HEAD_PTR_OFFSET 0x000001A4
/// Index of the TX_AC_2_HEAD_PTR register
#define NXMAC_TX_AC_2_HEAD_PTR_INDEX  0x00000069
/// Reset value of the TX_AC_2_HEAD_PTR register
#define NXMAC_TX_AC_2_HEAD_PTR_RESET  0x00000000

/**
 * @brief Returns the current value of the TX_AC_2_HEAD_PTR register.
 * The TX_AC_2_HEAD_PTR register will be read and its value returned.
 * @return The current value of the TX_AC_2_HEAD_PTR register.
 */
__INLINE uint32_t nxmac_tx_ac_2_head_ptr_get(void)
{
    return REG_PL_RD(NXMAC_TX_AC_2_HEAD_PTR_ADDR);
}

/**
 * @brief Sets the TX_AC_2_HEAD_PTR register to a value.
 * The TX_AC_2_HEAD_PTR register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_tx_ac_2_head_ptr_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TX_AC_2_HEAD_PTR_ADDR, value);
}

// field definitions
/// TX_AC_2_HEAD_PTR field mask
#define NXMAC_TX_AC_2_HEAD_PTR_MASK   ((uint32_t)0xFFFFFFFF)
/// TX_AC_2_HEAD_PTR field LSB position
#define NXMAC_TX_AC_2_HEAD_PTR_LSB    0
/// TX_AC_2_HEAD_PTR field width
#define NXMAC_TX_AC_2_HEAD_PTR_WIDTH  ((uint32_t)0x00000020)

/// TX_AC_2_HEAD_PTR field reset value
#define NXMAC_TX_AC_2_HEAD_PTR_RST    0x0

/**
 * @brief Returns the current value of the txAC2HeadPtr field in the TX_AC_2_HEAD_PTR register.
 *
 * The TX_AC_2_HEAD_PTR register will be read and the txAC2HeadPtr field's value will be returned.
 *
 * @return The current value of the txAC2HeadPtr field in the TX_AC_2_HEAD_PTR register.
 */
__INLINE uint32_t nxmac_tx_ac_2_head_ptr_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_AC_2_HEAD_PTR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the txAC2HeadPtr field of the TX_AC_2_HEAD_PTR register.
 *
 * The TX_AC_2_HEAD_PTR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac2headptr - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_2_head_ptr_setf(uint32_t txac2headptr)
{
    ASSERT_ERR((((uint32_t)txac2headptr << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_TX_AC_2_HEAD_PTR_ADDR, (uint32_t)txac2headptr << 0);
}

/// @}

/**
 * @name TX_AC_3_HEAD_PTR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00         txAC3HeadPtr   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TX_AC_3_HEAD_PTR register
#define NXMAC_TX_AC_3_HEAD_PTR_ADDR   0xC00081A8
/// Offset of the TX_AC_3_HEAD_PTR register from the base address
#define NXMAC_TX_AC_3_HEAD_PTR_OFFSET 0x000001A8
/// Index of the TX_AC_3_HEAD_PTR register
#define NXMAC_TX_AC_3_HEAD_PTR_INDEX  0x0000006A
/// Reset value of the TX_AC_3_HEAD_PTR register
#define NXMAC_TX_AC_3_HEAD_PTR_RESET  0x00000000

/**
 * @brief Returns the current value of the TX_AC_3_HEAD_PTR register.
 * The TX_AC_3_HEAD_PTR register will be read and its value returned.
 * @return The current value of the TX_AC_3_HEAD_PTR register.
 */
__INLINE uint32_t nxmac_tx_ac_3_head_ptr_get(void)
{
    return REG_PL_RD(NXMAC_TX_AC_3_HEAD_PTR_ADDR);
}

/**
 * @brief Sets the TX_AC_3_HEAD_PTR register to a value.
 * The TX_AC_3_HEAD_PTR register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_tx_ac_3_head_ptr_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TX_AC_3_HEAD_PTR_ADDR, value);
}

// field definitions
/// TX_AC_3_HEAD_PTR field mask
#define NXMAC_TX_AC_3_HEAD_PTR_MASK   ((uint32_t)0xFFFFFFFF)
/// TX_AC_3_HEAD_PTR field LSB position
#define NXMAC_TX_AC_3_HEAD_PTR_LSB    0
/// TX_AC_3_HEAD_PTR field width
#define NXMAC_TX_AC_3_HEAD_PTR_WIDTH  ((uint32_t)0x00000020)

/// TX_AC_3_HEAD_PTR field reset value
#define NXMAC_TX_AC_3_HEAD_PTR_RST    0x0

/**
 * @brief Returns the current value of the txAC3HeadPtr field in the TX_AC_3_HEAD_PTR register.
 *
 * The TX_AC_3_HEAD_PTR register will be read and the txAC3HeadPtr field's value will be returned.
 *
 * @return The current value of the txAC3HeadPtr field in the TX_AC_3_HEAD_PTR register.
 */
__INLINE uint32_t nxmac_tx_ac_3_head_ptr_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_AC_3_HEAD_PTR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the txAC3HeadPtr field of the TX_AC_3_HEAD_PTR register.
 *
 * The TX_AC_3_HEAD_PTR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txac3headptr - The value to set the field to.
 */
__INLINE void nxmac_tx_ac_3_head_ptr_setf(uint32_t txac3headptr)
{
    ASSERT_ERR((((uint32_t)txac3headptr << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_TX_AC_3_HEAD_PTR_ADDR, (uint32_t)txac3headptr << 0);
}

/// @}

/**
 * @name TX_STRUCT_SIZES register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:24           dmaRBDSize   0x0
 *  23:18           dmaRHDSize   0x0
 *  17:12           dmaTBDSize   0x0
 *  11:06           dmaTHDSize   0x0
 *  05:00          ptEntrySize   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TX_STRUCT_SIZES register
#define NXMAC_TX_STRUCT_SIZES_ADDR   0xC00081AC
/// Offset of the TX_STRUCT_SIZES register from the base address
#define NXMAC_TX_STRUCT_SIZES_OFFSET 0x000001AC
/// Index of the TX_STRUCT_SIZES register
#define NXMAC_TX_STRUCT_SIZES_INDEX  0x0000006B
/// Reset value of the TX_STRUCT_SIZES register
#define NXMAC_TX_STRUCT_SIZES_RESET  0x00000000

/**
 * @brief Returns the current value of the TX_STRUCT_SIZES register.
 * The TX_STRUCT_SIZES register will be read and its value returned.
 * @return The current value of the TX_STRUCT_SIZES register.
 */
__INLINE uint32_t nxmac_tx_struct_sizes_get(void)
{
    return REG_PL_RD(NXMAC_TX_STRUCT_SIZES_ADDR);
}

/**
 * @brief Sets the TX_STRUCT_SIZES register to a value.
 * The TX_STRUCT_SIZES register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_tx_struct_sizes_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TX_STRUCT_SIZES_ADDR, value);
}

// field definitions
/// DMA_RBD_SIZE field mask
#define NXMAC_DMA_RBD_SIZE_MASK    ((uint32_t)0x3F000000)
/// DMA_RBD_SIZE field LSB position
#define NXMAC_DMA_RBD_SIZE_LSB     24
/// DMA_RBD_SIZE field width
#define NXMAC_DMA_RBD_SIZE_WIDTH   ((uint32_t)0x00000006)
/// DMA_RHD_SIZE field mask
#define NXMAC_DMA_RHD_SIZE_MASK    ((uint32_t)0x00FC0000)
/// DMA_RHD_SIZE field LSB position
#define NXMAC_DMA_RHD_SIZE_LSB     18
/// DMA_RHD_SIZE field width
#define NXMAC_DMA_RHD_SIZE_WIDTH   ((uint32_t)0x00000006)
/// DMA_TBD_SIZE field mask
#define NXMAC_DMA_TBD_SIZE_MASK    ((uint32_t)0x0003F000)
/// DMA_TBD_SIZE field LSB position
#define NXMAC_DMA_TBD_SIZE_LSB     12
/// DMA_TBD_SIZE field width
#define NXMAC_DMA_TBD_SIZE_WIDTH   ((uint32_t)0x00000006)
/// DMA_THD_SIZE field mask
#define NXMAC_DMA_THD_SIZE_MASK    ((uint32_t)0x00000FC0)
/// DMA_THD_SIZE field LSB position
#define NXMAC_DMA_THD_SIZE_LSB     6
/// DMA_THD_SIZE field width
#define NXMAC_DMA_THD_SIZE_WIDTH   ((uint32_t)0x00000006)
/// PT_ENTRY_SIZE field mask
#define NXMAC_PT_ENTRY_SIZE_MASK   ((uint32_t)0x0000003F)
/// PT_ENTRY_SIZE field LSB position
#define NXMAC_PT_ENTRY_SIZE_LSB    0
/// PT_ENTRY_SIZE field width
#define NXMAC_PT_ENTRY_SIZE_WIDTH  ((uint32_t)0x00000006)

/// DMA_RBD_SIZE field reset value
#define NXMAC_DMA_RBD_SIZE_RST     0x0
/// DMA_RHD_SIZE field reset value
#define NXMAC_DMA_RHD_SIZE_RST     0x0
/// DMA_TBD_SIZE field reset value
#define NXMAC_DMA_TBD_SIZE_RST     0x0
/// DMA_THD_SIZE field reset value
#define NXMAC_DMA_THD_SIZE_RST     0x0
/// PT_ENTRY_SIZE field reset value
#define NXMAC_PT_ENTRY_SIZE_RST    0x0

/**
 * @brief Constructs a value for the TX_STRUCT_SIZES register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] dmarbdsize - The value to use for the dmaRBDSize field.
 * @param[in] dmarhdsize - The value to use for the dmaRHDSize field.
 * @param[in] dmatbdsize - The value to use for the dmaTBDSize field.
 * @param[in] dmathdsize - The value to use for the dmaTHDSize field.
 * @param[in] ptentrysize - The value to use for the ptEntrySize field.
 */
__INLINE void nxmac_tx_struct_sizes_pack(uint8_t dmarbdsize, uint8_t dmarhdsize, uint8_t dmatbdsize, uint8_t dmathdsize, uint8_t ptentrysize)
{
    ASSERT_ERR((((uint32_t)dmarbdsize << 24) & ~((uint32_t)0x3F000000)) == 0);
    ASSERT_ERR((((uint32_t)dmarhdsize << 18) & ~((uint32_t)0x00FC0000)) == 0);
    ASSERT_ERR((((uint32_t)dmatbdsize << 12) & ~((uint32_t)0x0003F000)) == 0);
    ASSERT_ERR((((uint32_t)dmathdsize << 6) & ~((uint32_t)0x00000FC0)) == 0);
    ASSERT_ERR((((uint32_t)ptentrysize << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(NXMAC_TX_STRUCT_SIZES_ADDR,  ((uint32_t)dmarbdsize << 24) | ((uint32_t)dmarhdsize << 18) | ((uint32_t)dmatbdsize << 12) | ((uint32_t)dmathdsize << 6) | ((uint32_t)ptentrysize << 0));
}

/**
 * @brief Unpacks TX_STRUCT_SIZES's fields from current value of the TX_STRUCT_SIZES register.
 *
 * Reads the TX_STRUCT_SIZES register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] dmarbdsize - Will be populated with the current value of this field from the register.
 * @param[out] dmarhdsize - Will be populated with the current value of this field from the register.
 * @param[out] dmatbdsize - Will be populated with the current value of this field from the register.
 * @param[out] dmathdsize - Will be populated with the current value of this field from the register.
 * @param[out] ptentrysize - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_tx_struct_sizes_unpack(uint8_t *dmarbdsize, uint8_t *dmarhdsize, uint8_t *dmatbdsize, uint8_t *dmathdsize, uint8_t *ptentrysize)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_STRUCT_SIZES_ADDR);

    *dmarbdsize = (localVal & ((uint32_t)0x3F000000)) >> 24;
    *dmarhdsize = (localVal & ((uint32_t)0x00FC0000)) >> 18;
    *dmatbdsize = (localVal & ((uint32_t)0x0003F000)) >> 12;
    *dmathdsize = (localVal & ((uint32_t)0x00000FC0)) >> 6;
    *ptentrysize = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the dmaRBDSize field in the TX_STRUCT_SIZES register.
 *
 * The TX_STRUCT_SIZES register will be read and the dmaRBDSize field's value will be returned.
 *
 * @return The current value of the dmaRBDSize field in the TX_STRUCT_SIZES register.
 */
__INLINE uint8_t nxmac_dma_rbd_size_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_STRUCT_SIZES_ADDR);
    return ((localVal & ((uint32_t)0x3F000000)) >> 24);
}

/**
 * @brief Sets the dmaRBDSize field of the TX_STRUCT_SIZES register.
 *
 * The TX_STRUCT_SIZES register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dmarbdsize - The value to set the field to.
 */
__INLINE void nxmac_dma_rbd_size_setf(uint8_t dmarbdsize)
{
    ASSERT_ERR((((uint32_t)dmarbdsize << 24) & ~((uint32_t)0x3F000000)) == 0);
    REG_PL_WR(NXMAC_TX_STRUCT_SIZES_ADDR, (REG_PL_RD(NXMAC_TX_STRUCT_SIZES_ADDR) & ~((uint32_t)0x3F000000)) | ((uint32_t)dmarbdsize << 24));
}

/**
 * @brief Returns the current value of the dmaRHDSize field in the TX_STRUCT_SIZES register.
 *
 * The TX_STRUCT_SIZES register will be read and the dmaRHDSize field's value will be returned.
 *
 * @return The current value of the dmaRHDSize field in the TX_STRUCT_SIZES register.
 */
__INLINE uint8_t nxmac_dma_rhd_size_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_STRUCT_SIZES_ADDR);
    return ((localVal & ((uint32_t)0x00FC0000)) >> 18);
}

/**
 * @brief Sets the dmaRHDSize field of the TX_STRUCT_SIZES register.
 *
 * The TX_STRUCT_SIZES register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dmarhdsize - The value to set the field to.
 */
__INLINE void nxmac_dma_rhd_size_setf(uint8_t dmarhdsize)
{
    ASSERT_ERR((((uint32_t)dmarhdsize << 18) & ~((uint32_t)0x00FC0000)) == 0);
    REG_PL_WR(NXMAC_TX_STRUCT_SIZES_ADDR, (REG_PL_RD(NXMAC_TX_STRUCT_SIZES_ADDR) & ~((uint32_t)0x00FC0000)) | ((uint32_t)dmarhdsize << 18));
}

/**
 * @brief Returns the current value of the dmaTBDSize field in the TX_STRUCT_SIZES register.
 *
 * The TX_STRUCT_SIZES register will be read and the dmaTBDSize field's value will be returned.
 *
 * @return The current value of the dmaTBDSize field in the TX_STRUCT_SIZES register.
 */
__INLINE uint8_t nxmac_dma_tbd_size_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_STRUCT_SIZES_ADDR);
    return ((localVal & ((uint32_t)0x0003F000)) >> 12);
}

/**
 * @brief Sets the dmaTBDSize field of the TX_STRUCT_SIZES register.
 *
 * The TX_STRUCT_SIZES register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dmatbdsize - The value to set the field to.
 */
__INLINE void nxmac_dma_tbd_size_setf(uint8_t dmatbdsize)
{
    ASSERT_ERR((((uint32_t)dmatbdsize << 12) & ~((uint32_t)0x0003F000)) == 0);
    REG_PL_WR(NXMAC_TX_STRUCT_SIZES_ADDR, (REG_PL_RD(NXMAC_TX_STRUCT_SIZES_ADDR) & ~((uint32_t)0x0003F000)) | ((uint32_t)dmatbdsize << 12));
}

/**
 * @brief Returns the current value of the dmaTHDSize field in the TX_STRUCT_SIZES register.
 *
 * The TX_STRUCT_SIZES register will be read and the dmaTHDSize field's value will be returned.
 *
 * @return The current value of the dmaTHDSize field in the TX_STRUCT_SIZES register.
 */
__INLINE uint8_t nxmac_dma_thd_size_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_STRUCT_SIZES_ADDR);
    return ((localVal & ((uint32_t)0x00000FC0)) >> 6);
}

/**
 * @brief Sets the dmaTHDSize field of the TX_STRUCT_SIZES register.
 *
 * The TX_STRUCT_SIZES register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dmathdsize - The value to set the field to.
 */
__INLINE void nxmac_dma_thd_size_setf(uint8_t dmathdsize)
{
    ASSERT_ERR((((uint32_t)dmathdsize << 6) & ~((uint32_t)0x00000FC0)) == 0);
    REG_PL_WR(NXMAC_TX_STRUCT_SIZES_ADDR, (REG_PL_RD(NXMAC_TX_STRUCT_SIZES_ADDR) & ~((uint32_t)0x00000FC0)) | ((uint32_t)dmathdsize << 6));
}

/**
 * @brief Returns the current value of the ptEntrySize field in the TX_STRUCT_SIZES register.
 *
 * The TX_STRUCT_SIZES register will be read and the ptEntrySize field's value will be returned.
 *
 * @return The current value of the ptEntrySize field in the TX_STRUCT_SIZES register.
 */
__INLINE uint8_t nxmac_pt_entry_size_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_STRUCT_SIZES_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the ptEntrySize field of the TX_STRUCT_SIZES register.
 *
 * The TX_STRUCT_SIZES register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ptentrysize - The value to set the field to.
 */
__INLINE void nxmac_pt_entry_size_setf(uint8_t ptentrysize)
{
    ASSERT_ERR((((uint32_t)ptentrysize << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(NXMAC_TX_STRUCT_SIZES_ADDR, (REG_PL_RD(NXMAC_TX_STRUCT_SIZES_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)ptentrysize << 0));
}

/// @}

/**
 * @name RX_HEADER_HEAD_PTR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:02      rxHeaderHeadPtr   0x0
 *     00   rxHeaderHeadPtrValid   0
 * </pre>
 *
 * @{
 */

/// Address of the RX_HEADER_HEAD_PTR register
#define NXMAC_RX_HEADER_HEAD_PTR_ADDR   0xC00081B8
/// Offset of the RX_HEADER_HEAD_PTR register from the base address
#define NXMAC_RX_HEADER_HEAD_PTR_OFFSET 0x000001B8
/// Index of the RX_HEADER_HEAD_PTR register
#define NXMAC_RX_HEADER_HEAD_PTR_INDEX  0x0000006E
/// Reset value of the RX_HEADER_HEAD_PTR register
#define NXMAC_RX_HEADER_HEAD_PTR_RESET  0x00000000

/**
 * @brief Returns the current value of the RX_HEADER_HEAD_PTR register.
 * The RX_HEADER_HEAD_PTR register will be read and its value returned.
 * @return The current value of the RX_HEADER_HEAD_PTR register.
 */
__INLINE uint32_t nxmac_rx_header_head_ptr_get(void)
{
    return REG_PL_RD(NXMAC_RX_HEADER_HEAD_PTR_ADDR);
}

/**
 * @brief Sets the RX_HEADER_HEAD_PTR register to a value.
 * The RX_HEADER_HEAD_PTR register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_rx_header_head_ptr_set(uint32_t value)
{
    REG_PL_WR(NXMAC_RX_HEADER_HEAD_PTR_ADDR, value);
}

// field definitions
/// RX_HEADER_HEAD_PTR field mask
#define NXMAC_RX_HEADER_HEAD_PTR_MASK         ((uint32_t)0xFFFFFFFC)
/// RX_HEADER_HEAD_PTR field LSB position
#define NXMAC_RX_HEADER_HEAD_PTR_LSB          2
/// RX_HEADER_HEAD_PTR field width
#define NXMAC_RX_HEADER_HEAD_PTR_WIDTH        ((uint32_t)0x0000001E)
/// RX_HEADER_HEAD_PTR_VALID field bit
#define NXMAC_RX_HEADER_HEAD_PTR_VALID_BIT    ((uint32_t)0x00000001)
/// RX_HEADER_HEAD_PTR_VALID field position
#define NXMAC_RX_HEADER_HEAD_PTR_VALID_POS    0

/// RX_HEADER_HEAD_PTR field reset value
#define NXMAC_RX_HEADER_HEAD_PTR_RST          0x0
/// RX_HEADER_HEAD_PTR_VALID field reset value
#define NXMAC_RX_HEADER_HEAD_PTR_VALID_RST    0x0

/**
 * @brief Constructs a value for the RX_HEADER_HEAD_PTR register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxheaderheadptr - The value to use for the rxHeaderHeadPtr field.
 * @param[in] rxheaderheadptrvalid - The value to use for the rxHeaderHeadPtrValid field.
 */
__INLINE void nxmac_rx_header_head_ptr_pack(uint32_t rxheaderheadptr, uint8_t rxheaderheadptrvalid)
{
    ASSERT_ERR((((uint32_t)rxheaderheadptr << 2) & ~((uint32_t)0xFFFFFFFC)) == 0);
    ASSERT_ERR((((uint32_t)rxheaderheadptrvalid << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_RX_HEADER_HEAD_PTR_ADDR,  ((uint32_t)rxheaderheadptr << 2) | ((uint32_t)rxheaderheadptrvalid << 0));
}

/**
 * @brief Unpacks RX_HEADER_HEAD_PTR's fields from current value of the RX_HEADER_HEAD_PTR register.
 *
 * Reads the RX_HEADER_HEAD_PTR register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxheaderheadptr - Will be populated with the current value of this field from the register.
 * @param[out] rxheaderheadptrvalid - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_rx_header_head_ptr_unpack(uint32_t *rxheaderheadptr, uint8_t *rxheaderheadptrvalid)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_HEADER_HEAD_PTR_ADDR);

    *rxheaderheadptr = (localVal & ((uint32_t)0xFFFFFFFC)) >> 2;
    *rxheaderheadptrvalid = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the rxHeaderHeadPtr field in the RX_HEADER_HEAD_PTR register.
 *
 * The RX_HEADER_HEAD_PTR register will be read and the rxHeaderHeadPtr field's value will be returned.
 *
 * @return The current value of the rxHeaderHeadPtr field in the RX_HEADER_HEAD_PTR register.
 */
__INLINE uint32_t nxmac_rx_header_head_ptr_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_HEADER_HEAD_PTR_ADDR);
    return ((localVal & ((uint32_t)0xFFFFFFFC)) >> 2);
}

/**
 * @brief Sets the rxHeaderHeadPtr field of the RX_HEADER_HEAD_PTR register.
 *
 * The RX_HEADER_HEAD_PTR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxheaderheadptr - The value to set the field to.
 */
__INLINE void nxmac_rx_header_head_ptr_setf(uint32_t rxheaderheadptr)
{
    ASSERT_ERR((((uint32_t)rxheaderheadptr << 2) & ~((uint32_t)0xFFFFFFFC)) == 0);
    REG_PL_WR(NXMAC_RX_HEADER_HEAD_PTR_ADDR, (REG_PL_RD(NXMAC_RX_HEADER_HEAD_PTR_ADDR) & ~((uint32_t)0xFFFFFFFC)) | ((uint32_t)rxheaderheadptr << 2));
}

/**
 * @brief Returns the current value of the rxHeaderHeadPtrValid field in the RX_HEADER_HEAD_PTR register.
 *
 * The RX_HEADER_HEAD_PTR register will be read and the rxHeaderHeadPtrValid field's value will be returned.
 *
 * @return The current value of the rxHeaderHeadPtrValid field in the RX_HEADER_HEAD_PTR register.
 */
__INLINE uint8_t nxmac_rx_header_head_ptr_valid_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_HEADER_HEAD_PTR_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the rxHeaderHeadPtrValid field of the RX_HEADER_HEAD_PTR register.
 *
 * The RX_HEADER_HEAD_PTR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxheaderheadptrvalid - The value to set the field to.
 */
__INLINE void nxmac_rx_header_head_ptr_valid_setf(uint8_t rxheaderheadptrvalid)
{
    ASSERT_ERR((((uint32_t)rxheaderheadptrvalid << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_RX_HEADER_HEAD_PTR_ADDR, (REG_PL_RD(NXMAC_RX_HEADER_HEAD_PTR_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)rxheaderheadptrvalid << 0));
}

/// @}

/**
 * @name RX_PAYLOAD_HEAD_PTR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:02     rxPayloadHeadPtr   0x0
 *     00   rxPayloadHeadPtrValid   0
 * </pre>
 *
 * @{
 */

/// Address of the RX_PAYLOAD_HEAD_PTR register
#define NXMAC_RX_PAYLOAD_HEAD_PTR_ADDR   0xC00081BC
/// Offset of the RX_PAYLOAD_HEAD_PTR register from the base address
#define NXMAC_RX_PAYLOAD_HEAD_PTR_OFFSET 0x000001BC
/// Index of the RX_PAYLOAD_HEAD_PTR register
#define NXMAC_RX_PAYLOAD_HEAD_PTR_INDEX  0x0000006F
/// Reset value of the RX_PAYLOAD_HEAD_PTR register
#define NXMAC_RX_PAYLOAD_HEAD_PTR_RESET  0x00000000

/**
 * @brief Returns the current value of the RX_PAYLOAD_HEAD_PTR register.
 * The RX_PAYLOAD_HEAD_PTR register will be read and its value returned.
 * @return The current value of the RX_PAYLOAD_HEAD_PTR register.
 */
__INLINE uint32_t nxmac_rx_payload_head_ptr_get(void)
{
    return REG_PL_RD(NXMAC_RX_PAYLOAD_HEAD_PTR_ADDR);
}

/**
 * @brief Sets the RX_PAYLOAD_HEAD_PTR register to a value.
 * The RX_PAYLOAD_HEAD_PTR register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_rx_payload_head_ptr_set(uint32_t value)
{
    REG_PL_WR(NXMAC_RX_PAYLOAD_HEAD_PTR_ADDR, value);
}

// field definitions
/// RX_PAYLOAD_HEAD_PTR field mask
#define NXMAC_RX_PAYLOAD_HEAD_PTR_MASK         ((uint32_t)0xFFFFFFFC)
/// RX_PAYLOAD_HEAD_PTR field LSB position
#define NXMAC_RX_PAYLOAD_HEAD_PTR_LSB          2
/// RX_PAYLOAD_HEAD_PTR field width
#define NXMAC_RX_PAYLOAD_HEAD_PTR_WIDTH        ((uint32_t)0x0000001E)
/// RX_PAYLOAD_HEAD_PTR_VALID field bit
#define NXMAC_RX_PAYLOAD_HEAD_PTR_VALID_BIT    ((uint32_t)0x00000001)
/// RX_PAYLOAD_HEAD_PTR_VALID field position
#define NXMAC_RX_PAYLOAD_HEAD_PTR_VALID_POS    0

/// RX_PAYLOAD_HEAD_PTR field reset value
#define NXMAC_RX_PAYLOAD_HEAD_PTR_RST          0x0
/// RX_PAYLOAD_HEAD_PTR_VALID field reset value
#define NXMAC_RX_PAYLOAD_HEAD_PTR_VALID_RST    0x0

/**
 * @brief Constructs a value for the RX_PAYLOAD_HEAD_PTR register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxpayloadheadptr - The value to use for the rxPayloadHeadPtr field.
 * @param[in] rxpayloadheadptrvalid - The value to use for the rxPayloadHeadPtrValid field.
 */
__INLINE void nxmac_rx_payload_head_ptr_pack(uint32_t rxpayloadheadptr, uint8_t rxpayloadheadptrvalid)
{
    ASSERT_ERR((((uint32_t)rxpayloadheadptr << 2) & ~((uint32_t)0xFFFFFFFC)) == 0);
    ASSERT_ERR((((uint32_t)rxpayloadheadptrvalid << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_RX_PAYLOAD_HEAD_PTR_ADDR,  ((uint32_t)rxpayloadheadptr << 2) | ((uint32_t)rxpayloadheadptrvalid << 0));
}

/**
 * @brief Unpacks RX_PAYLOAD_HEAD_PTR's fields from current value of the RX_PAYLOAD_HEAD_PTR register.
 *
 * Reads the RX_PAYLOAD_HEAD_PTR register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxpayloadheadptr - Will be populated with the current value of this field from the register.
 * @param[out] rxpayloadheadptrvalid - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_rx_payload_head_ptr_unpack(uint32_t *rxpayloadheadptr, uint8_t *rxpayloadheadptrvalid)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_PAYLOAD_HEAD_PTR_ADDR);

    *rxpayloadheadptr = (localVal & ((uint32_t)0xFFFFFFFC)) >> 2;
    *rxpayloadheadptrvalid = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the rxPayloadHeadPtr field in the RX_PAYLOAD_HEAD_PTR register.
 *
 * The RX_PAYLOAD_HEAD_PTR register will be read and the rxPayloadHeadPtr field's value will be returned.
 *
 * @return The current value of the rxPayloadHeadPtr field in the RX_PAYLOAD_HEAD_PTR register.
 */
__INLINE uint32_t nxmac_rx_payload_head_ptr_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_PAYLOAD_HEAD_PTR_ADDR);
    return ((localVal & ((uint32_t)0xFFFFFFFC)) >> 2);
}

/**
 * @brief Sets the rxPayloadHeadPtr field of the RX_PAYLOAD_HEAD_PTR register.
 *
 * The RX_PAYLOAD_HEAD_PTR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxpayloadheadptr - The value to set the field to.
 */
__INLINE void nxmac_rx_payload_head_ptr_setf(uint32_t rxpayloadheadptr)
{
    ASSERT_ERR((((uint32_t)rxpayloadheadptr << 2) & ~((uint32_t)0xFFFFFFFC)) == 0);
    REG_PL_WR(NXMAC_RX_PAYLOAD_HEAD_PTR_ADDR, (REG_PL_RD(NXMAC_RX_PAYLOAD_HEAD_PTR_ADDR) & ~((uint32_t)0xFFFFFFFC)) | ((uint32_t)rxpayloadheadptr << 2));
}

/**
 * @brief Returns the current value of the rxPayloadHeadPtrValid field in the RX_PAYLOAD_HEAD_PTR register.
 *
 * The RX_PAYLOAD_HEAD_PTR register will be read and the rxPayloadHeadPtrValid field's value will be returned.
 *
 * @return The current value of the rxPayloadHeadPtrValid field in the RX_PAYLOAD_HEAD_PTR register.
 */
__INLINE uint8_t nxmac_rx_payload_head_ptr_valid_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_PAYLOAD_HEAD_PTR_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the rxPayloadHeadPtrValid field of the RX_PAYLOAD_HEAD_PTR register.
 *
 * The RX_PAYLOAD_HEAD_PTR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxpayloadheadptrvalid - The value to set the field to.
 */
__INLINE void nxmac_rx_payload_head_ptr_valid_setf(uint8_t rxpayloadheadptrvalid)
{
    ASSERT_ERR((((uint32_t)rxpayloadheadptrvalid << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_RX_PAYLOAD_HEAD_PTR_ADDR, (REG_PL_RD(NXMAC_RX_PAYLOAD_HEAD_PTR_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)rxpayloadheadptrvalid << 0));
}

/// @}

/**
 * @name DMA_THRESHOLD register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  23:16      rxFIFOThreshold   0x10
 *  07:00      txFIFOThreshold   0x10
 * </pre>
 *
 * @{
 */

/// Address of the DMA_THRESHOLD register
#define NXMAC_DMA_THRESHOLD_ADDR   0xC00081C0
/// Offset of the DMA_THRESHOLD register from the base address
#define NXMAC_DMA_THRESHOLD_OFFSET 0x000001C0
/// Index of the DMA_THRESHOLD register
#define NXMAC_DMA_THRESHOLD_INDEX  0x00000070
/// Reset value of the DMA_THRESHOLD register
#define NXMAC_DMA_THRESHOLD_RESET  0x00100010

/**
 * @brief Returns the current value of the DMA_THRESHOLD register.
 * The DMA_THRESHOLD register will be read and its value returned.
 * @return The current value of the DMA_THRESHOLD register.
 */
__INLINE uint32_t nxmac_dma_threshold_get(void)
{
    return REG_PL_RD(NXMAC_DMA_THRESHOLD_ADDR);
}

/**
 * @brief Sets the DMA_THRESHOLD register to a value.
 * The DMA_THRESHOLD register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_dma_threshold_set(uint32_t value)
{
    REG_PL_WR(NXMAC_DMA_THRESHOLD_ADDR, value);
}

// field definitions
/// RX_FIFO_THRESHOLD field mask
#define NXMAC_RX_FIFO_THRESHOLD_MASK   ((uint32_t)0x00FF0000)
/// RX_FIFO_THRESHOLD field LSB position
#define NXMAC_RX_FIFO_THRESHOLD_LSB    16
/// RX_FIFO_THRESHOLD field width
#define NXMAC_RX_FIFO_THRESHOLD_WIDTH  ((uint32_t)0x00000008)
/// TX_FIFO_THRESHOLD field mask
#define NXMAC_TX_FIFO_THRESHOLD_MASK   ((uint32_t)0x000000FF)
/// TX_FIFO_THRESHOLD field LSB position
#define NXMAC_TX_FIFO_THRESHOLD_LSB    0
/// TX_FIFO_THRESHOLD field width
#define NXMAC_TX_FIFO_THRESHOLD_WIDTH  ((uint32_t)0x00000008)

/// RX_FIFO_THRESHOLD field reset value
#define NXMAC_RX_FIFO_THRESHOLD_RST    0x10
/// TX_FIFO_THRESHOLD field reset value
#define NXMAC_TX_FIFO_THRESHOLD_RST    0x10

/**
 * @brief Constructs a value for the DMA_THRESHOLD register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxfifothreshold - The value to use for the rxFIFOThreshold field.
 * @param[in] txfifothreshold - The value to use for the txFIFOThreshold field.
 */
__INLINE void nxmac_dma_threshold_pack(uint8_t rxfifothreshold, uint8_t txfifothreshold)
{
    ASSERT_ERR((((uint32_t)rxfifothreshold << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)txfifothreshold << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_DMA_THRESHOLD_ADDR,  ((uint32_t)rxfifothreshold << 16) | ((uint32_t)txfifothreshold << 0));
}

/**
 * @brief Unpacks DMA_THRESHOLD's fields from current value of the DMA_THRESHOLD register.
 *
 * Reads the DMA_THRESHOLD register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxfifothreshold - Will be populated with the current value of this field from the register.
 * @param[out] txfifothreshold - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_dma_threshold_unpack(uint8_t *rxfifothreshold, uint8_t *txfifothreshold)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_THRESHOLD_ADDR);

    *rxfifothreshold = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *txfifothreshold = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the rxFIFOThreshold field in the DMA_THRESHOLD register.
 *
 * The DMA_THRESHOLD register will be read and the rxFIFOThreshold field's value will be returned.
 *
 * @return The current value of the rxFIFOThreshold field in the DMA_THRESHOLD register.
 */
__INLINE uint8_t nxmac_rx_fifo_threshold_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_THRESHOLD_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the rxFIFOThreshold field of the DMA_THRESHOLD register.
 *
 * The DMA_THRESHOLD register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxfifothreshold - The value to set the field to.
 */
__INLINE void nxmac_rx_fifo_threshold_setf(uint8_t rxfifothreshold)
{
    ASSERT_ERR((((uint32_t)rxfifothreshold << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(NXMAC_DMA_THRESHOLD_ADDR, (REG_PL_RD(NXMAC_DMA_THRESHOLD_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)rxfifothreshold << 16));
}

/**
 * @brief Returns the current value of the txFIFOThreshold field in the DMA_THRESHOLD register.
 *
 * The DMA_THRESHOLD register will be read and the txFIFOThreshold field's value will be returned.
 *
 * @return The current value of the txFIFOThreshold field in the DMA_THRESHOLD register.
 */
__INLINE uint8_t nxmac_tx_fifo_threshold_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DMA_THRESHOLD_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the txFIFOThreshold field of the DMA_THRESHOLD register.
 *
 * The DMA_THRESHOLD register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txfifothreshold - The value to set the field to.
 */
__INLINE void nxmac_tx_fifo_threshold_setf(uint8_t txfifothreshold)
{
    ASSERT_ERR((((uint32_t)txfifothreshold << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_DMA_THRESHOLD_ADDR, (REG_PL_RD(NXMAC_DMA_THRESHOLD_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)txfifothreshold << 0));
}

/// @}

/**
 * @name EDCA_AC_HAS_DATA_SET register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     03           ac3HasData   0
 *     02           ac2HasData   0
 *     01           ac1HasData   0
 *     00           ac0HasData   0
 * </pre>
 *
 * @{
 */

/// Address of the EDCA_AC_HAS_DATA_SET register
#define NXMAC_EDCA_AC_HAS_DATA_SET_ADDR   0xC0008210
/// Offset of the EDCA_AC_HAS_DATA_SET register from the base address
#define NXMAC_EDCA_AC_HAS_DATA_SET_OFFSET 0x00000210
/// Index of the EDCA_AC_HAS_DATA_SET register
#define NXMAC_EDCA_AC_HAS_DATA_SET_INDEX  0x00000084
/// Reset value of the EDCA_AC_HAS_DATA_SET register
#define NXMAC_EDCA_AC_HAS_DATA_SET_RESET  0x00000000

/**
 * @brief Returns the current value of the EDCA_AC_HAS_DATA_SET register.
 * The EDCA_AC_HAS_DATA_SET register will be read and its value returned.
 * @return The current value of the EDCA_AC_HAS_DATA_SET register.
 */
__INLINE uint32_t nxmac_edca_ac_has_data_get(void)
{
    return REG_PL_RD(NXMAC_EDCA_AC_HAS_DATA_SET_ADDR);
}

/**
 * @brief Sets the EDCA_AC_HAS_DATA_SET register to a value.
 * The EDCA_AC_HAS_DATA_SET register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_edca_ac_has_data_set(uint32_t value)
{
    REG_PL_WR(NXMAC_EDCA_AC_HAS_DATA_SET_ADDR, value);
}

// field definitions
/// AC_3_HAS_DATA field bit
#define NXMAC_AC_3_HAS_DATA_BIT    ((uint32_t)0x00000008)
/// AC_3_HAS_DATA field position
#define NXMAC_AC_3_HAS_DATA_POS    3
/// AC_2_HAS_DATA field bit
#define NXMAC_AC_2_HAS_DATA_BIT    ((uint32_t)0x00000004)
/// AC_2_HAS_DATA field position
#define NXMAC_AC_2_HAS_DATA_POS    2
/// AC_1_HAS_DATA field bit
#define NXMAC_AC_1_HAS_DATA_BIT    ((uint32_t)0x00000002)
/// AC_1_HAS_DATA field position
#define NXMAC_AC_1_HAS_DATA_POS    1
/// AC_0_HAS_DATA field bit
#define NXMAC_AC_0_HAS_DATA_BIT    ((uint32_t)0x00000001)
/// AC_0_HAS_DATA field position
#define NXMAC_AC_0_HAS_DATA_POS    0

/// AC_3_HAS_DATA field reset value
#define NXMAC_AC_3_HAS_DATA_RST    0x0
/// AC_2_HAS_DATA field reset value
#define NXMAC_AC_2_HAS_DATA_RST    0x0
/// AC_1_HAS_DATA field reset value
#define NXMAC_AC_1_HAS_DATA_RST    0x0
/// AC_0_HAS_DATA field reset value
#define NXMAC_AC_0_HAS_DATA_RST    0x0

/**
 * @brief Constructs a value for the EDCA_AC_HAS_DATA_SET register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] ac3hasdata - The value to use for the ac3HasData field.
 * @param[in] ac2hasdata - The value to use for the ac2HasData field.
 * @param[in] ac1hasdata - The value to use for the ac1HasData field.
 * @param[in] ac0hasdata - The value to use for the ac0HasData field.
 */
__INLINE void nxmac_edca_ac_has_data_set_pack(uint8_t ac3hasdata, uint8_t ac2hasdata, uint8_t ac1hasdata, uint8_t ac0hasdata)
{
    ASSERT_ERR((((uint32_t)ac3hasdata << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)ac2hasdata << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)ac1hasdata << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)ac0hasdata << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_HAS_DATA_SET_ADDR,  ((uint32_t)ac3hasdata << 3) | ((uint32_t)ac2hasdata << 2) | ((uint32_t)ac1hasdata << 1) | ((uint32_t)ac0hasdata << 0));
}

/**
 * @brief Unpacks EDCA_AC_HAS_DATA_SET's fields from current value of the EDCA_AC_HAS_DATA_SET register.
 *
 * Reads the EDCA_AC_HAS_DATA_SET register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] ac3hasdata - Will be populated with the current value of this field from the register.
 * @param[out] ac2hasdata - Will be populated with the current value of this field from the register.
 * @param[out] ac1hasdata - Will be populated with the current value of this field from the register.
 * @param[out] ac0hasdata - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_edca_ac_has_data_unpack(uint8_t *ac3hasdata, uint8_t *ac2hasdata, uint8_t *ac1hasdata, uint8_t *ac0hasdata)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_HAS_DATA_SET_ADDR);

    *ac3hasdata = (localVal & ((uint32_t)0x00000008)) >> 3;
    *ac2hasdata = (localVal & ((uint32_t)0x00000004)) >> 2;
    *ac1hasdata = (localVal & ((uint32_t)0x00000002)) >> 1;
    *ac0hasdata = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the ac3HasData field in the EDCA_AC_HAS_DATA_SET register.
 *
 * The EDCA_AC_HAS_DATA_SET register will be read and the ac3HasData field's value will be returned.
 *
 * @return The current value of the ac3HasData field in the EDCA_AC_HAS_DATA_SET register.
 */
__INLINE uint8_t nxmac_ac_3_has_data_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_HAS_DATA_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the ac3HasData field of the EDCA_AC_HAS_DATA_SET register.
 *
 * The EDCA_AC_HAS_DATA_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac3hasdata - The value to set the field to.
 */
__INLINE void nxmac_ac_3_has_data_setf(uint8_t ac3hasdata)
{
    ASSERT_ERR((((uint32_t)ac3hasdata << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_HAS_DATA_SET_ADDR, (uint32_t)ac3hasdata << 3);
}

/**
 * @brief Returns the current value of the ac2HasData field in the EDCA_AC_HAS_DATA_SET register.
 *
 * The EDCA_AC_HAS_DATA_SET register will be read and the ac2HasData field's value will be returned.
 *
 * @return The current value of the ac2HasData field in the EDCA_AC_HAS_DATA_SET register.
 */
__INLINE uint8_t nxmac_ac_2_has_data_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_HAS_DATA_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the ac2HasData field of the EDCA_AC_HAS_DATA_SET register.
 *
 * The EDCA_AC_HAS_DATA_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac2hasdata - The value to set the field to.
 */
__INLINE void nxmac_ac_2_has_data_setf(uint8_t ac2hasdata)
{
    ASSERT_ERR((((uint32_t)ac2hasdata << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_HAS_DATA_SET_ADDR, (uint32_t)ac2hasdata << 2);
}

/**
 * @brief Returns the current value of the ac1HasData field in the EDCA_AC_HAS_DATA_SET register.
 *
 * The EDCA_AC_HAS_DATA_SET register will be read and the ac1HasData field's value will be returned.
 *
 * @return The current value of the ac1HasData field in the EDCA_AC_HAS_DATA_SET register.
 */
__INLINE uint8_t nxmac_ac_1_has_data_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_HAS_DATA_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the ac1HasData field of the EDCA_AC_HAS_DATA_SET register.
 *
 * The EDCA_AC_HAS_DATA_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac1hasdata - The value to set the field to.
 */
__INLINE void nxmac_ac_1_has_data_setf(uint8_t ac1hasdata)
{
    ASSERT_ERR((((uint32_t)ac1hasdata << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_HAS_DATA_SET_ADDR, (uint32_t)ac1hasdata << 1);
}

/**
 * @brief Returns the current value of the ac0HasData field in the EDCA_AC_HAS_DATA_SET register.
 *
 * The EDCA_AC_HAS_DATA_SET register will be read and the ac0HasData field's value will be returned.
 *
 * @return The current value of the ac0HasData field in the EDCA_AC_HAS_DATA_SET register.
 */
__INLINE uint8_t nxmac_ac_0_has_data_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_HAS_DATA_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the ac0HasData field of the EDCA_AC_HAS_DATA_SET register.
 *
 * The EDCA_AC_HAS_DATA_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac0hasdata - The value to set the field to.
 */
__INLINE void nxmac_ac_0_has_data_setf(uint8_t ac0hasdata)
{
    ASSERT_ERR((((uint32_t)ac0hasdata << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_HAS_DATA_SET_ADDR, (uint32_t)ac0hasdata << 0);
}

/// @}

/**
 * @name EDCA_AC_HAS_DATA_CLEAR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     03           ac3HasData   0
 *     02           ac2HasData   0
 *     01           ac1HasData   0
 *     00           ac0HasData   0
 * </pre>
 *
 * @{
 */

/// Address of the EDCA_AC_HAS_DATA_CLEAR register
#define NXMAC_EDCA_AC_HAS_DATA_CLEAR_ADDR   0xC0008214
/// Offset of the EDCA_AC_HAS_DATA_CLEAR register from the base address
#define NXMAC_EDCA_AC_HAS_DATA_CLEAR_OFFSET 0x00000214
/// Index of the EDCA_AC_HAS_DATA_CLEAR register
#define NXMAC_EDCA_AC_HAS_DATA_CLEAR_INDEX  0x00000085
/// Reset value of the EDCA_AC_HAS_DATA_CLEAR register
#define NXMAC_EDCA_AC_HAS_DATA_CLEAR_RESET  0x00000000

/**
 * @brief Sets the EDCA_AC_HAS_DATA_CLEAR register to a value.
 * The EDCA_AC_HAS_DATA_CLEAR register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_edca_ac_has_data_clear(uint32_t value)
{
    REG_PL_WR(NXMAC_EDCA_AC_HAS_DATA_CLEAR_ADDR, value);
}

// fields defined in symmetrical set/clear register
/**
 * @brief Constructs a value for the EDCA_AC_HAS_DATA_CLEAR register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] ac3hasdata - The value to use for the ac3HasData field.
 * @param[in] ac2hasdata - The value to use for the ac2HasData field.
 * @param[in] ac1hasdata - The value to use for the ac1HasData field.
 * @param[in] ac0hasdata - The value to use for the ac0HasData field.
 */
__INLINE void nxmac_edca_ac_has_data_clear_pack(uint8_t ac3hasdata, uint8_t ac2hasdata, uint8_t ac1hasdata, uint8_t ac0hasdata)
{
    ASSERT_ERR((((uint32_t)ac3hasdata << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)ac2hasdata << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)ac1hasdata << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)ac0hasdata << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_HAS_DATA_CLEAR_ADDR,  ((uint32_t)ac3hasdata << 3) | ((uint32_t)ac2hasdata << 2) | ((uint32_t)ac1hasdata << 1) | ((uint32_t)ac0hasdata << 0));
}

/**
 * @brief Sets the ac3HasData field of the EDCA_AC_HAS_DATA_CLEAR register.
 *
 * The EDCA_AC_HAS_DATA_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac3hasdata - The value to set the field to.
 */
__INLINE void nxmac_ac_3_has_data_clearf(uint8_t ac3hasdata)
{
    ASSERT_ERR((((uint32_t)ac3hasdata << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_HAS_DATA_CLEAR_ADDR, (uint32_t)ac3hasdata << 3);
}

/**
 * @brief Sets the ac2HasData field of the EDCA_AC_HAS_DATA_CLEAR register.
 *
 * The EDCA_AC_HAS_DATA_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac2hasdata - The value to set the field to.
 */
__INLINE void nxmac_ac_2_has_data_clearf(uint8_t ac2hasdata)
{
    ASSERT_ERR((((uint32_t)ac2hasdata << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_HAS_DATA_CLEAR_ADDR, (uint32_t)ac2hasdata << 2);
}

/**
 * @brief Sets the ac1HasData field of the EDCA_AC_HAS_DATA_CLEAR register.
 *
 * The EDCA_AC_HAS_DATA_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac1hasdata - The value to set the field to.
 */
__INLINE void nxmac_ac_1_has_data_clearf(uint8_t ac1hasdata)
{
    ASSERT_ERR((((uint32_t)ac1hasdata << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_HAS_DATA_CLEAR_ADDR, (uint32_t)ac1hasdata << 1);
}

/**
 * @brief Sets the ac0HasData field of the EDCA_AC_HAS_DATA_CLEAR register.
 *
 * The EDCA_AC_HAS_DATA_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ac0hasdata - The value to set the field to.
 */
__INLINE void nxmac_ac_0_has_data_clearf(uint8_t ac0hasdata)
{
    ASSERT_ERR((((uint32_t)ac0hasdata << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_HAS_DATA_CLEAR_ADDR, (uint32_t)ac0hasdata << 0);
}

/// @}

/**
 * @name MOT_1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:16               ac1MOT   0x0
 *  15:00               ac0MOT   0x0
 * </pre>
 *
 * @{
 */

/// Address of the MOT_1 register
#define NXMAC_MOT_1_ADDR   0xC0008228
/// Offset of the MOT_1 register from the base address
#define NXMAC_MOT_1_OFFSET 0x00000228
/// Index of the MOT_1 register
#define NXMAC_MOT_1_INDEX  0x0000008A
/// Reset value of the MOT_1 register
#define NXMAC_MOT_1_RESET  0x00000000

/**
 * @brief Returns the current value of the MOT_1 register.
 * The MOT_1 register will be read and its value returned.
 * @return The current value of the MOT_1 register.
 */
__INLINE uint32_t nxmac_mot_1_get(void)
{
    return REG_PL_RD(NXMAC_MOT_1_ADDR);
}

// field definitions
/// AC_1MOT field mask
#define NXMAC_AC_1MOT_MASK   ((uint32_t)0xFFFF0000)
/// AC_1MOT field LSB position
#define NXMAC_AC_1MOT_LSB    16
/// AC_1MOT field width
#define NXMAC_AC_1MOT_WIDTH  ((uint32_t)0x00000010)
/// AC_0MOT field mask
#define NXMAC_AC_0MOT_MASK   ((uint32_t)0x0000FFFF)
/// AC_0MOT field LSB position
#define NXMAC_AC_0MOT_LSB    0
/// AC_0MOT field width
#define NXMAC_AC_0MOT_WIDTH  ((uint32_t)0x00000010)

/// AC_1MOT field reset value
#define NXMAC_AC_1MOT_RST    0x0
/// AC_0MOT field reset value
#define NXMAC_AC_0MOT_RST    0x0

/**
 * @brief Unpacks MOT_1's fields from current value of the MOT_1 register.
 *
 * Reads the MOT_1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] ac1mot - Will be populated with the current value of this field from the register.
 * @param[out] ac0mot - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_mot_1_unpack(uint16_t *ac1mot, uint16_t *ac0mot)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MOT_1_ADDR);

    *ac1mot = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *ac0mot = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the ac1MOT field in the MOT_1 register.
 *
 * The MOT_1 register will be read and the ac1MOT field's value will be returned.
 *
 * @return The current value of the ac1MOT field in the MOT_1 register.
 */
__INLINE uint16_t nxmac_ac_1mot_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MOT_1_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Returns the current value of the ac0MOT field in the MOT_1 register.
 *
 * The MOT_1 register will be read and the ac0MOT field's value will be returned.
 *
 * @return The current value of the ac0MOT field in the MOT_1 register.
 */
__INLINE uint16_t nxmac_ac_0mot_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MOT_1_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/// @}

/**
 * @name MOT_2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:16               ac3MOT   0x2F
 *  15:00               ac2MOT   0x5E
 * </pre>
 *
 * @{
 */

/// Address of the MOT_2 register
#define NXMAC_MOT_2_ADDR   0xC000822C
/// Offset of the MOT_2 register from the base address
#define NXMAC_MOT_2_OFFSET 0x0000022C
/// Index of the MOT_2 register
#define NXMAC_MOT_2_INDEX  0x0000008B
/// Reset value of the MOT_2 register
#define NXMAC_MOT_2_RESET  0x002F005E

/**
 * @brief Returns the current value of the MOT_2 register.
 * The MOT_2 register will be read and its value returned.
 * @return The current value of the MOT_2 register.
 */
__INLINE uint32_t nxmac_mot_2_get(void)
{
    return REG_PL_RD(NXMAC_MOT_2_ADDR);
}

// field definitions
/// AC_3MOT field mask
#define NXMAC_AC_3MOT_MASK   ((uint32_t)0xFFFF0000)
/// AC_3MOT field LSB position
#define NXMAC_AC_3MOT_LSB    16
/// AC_3MOT field width
#define NXMAC_AC_3MOT_WIDTH  ((uint32_t)0x00000010)
/// AC_2MOT field mask
#define NXMAC_AC_2MOT_MASK   ((uint32_t)0x0000FFFF)
/// AC_2MOT field LSB position
#define NXMAC_AC_2MOT_LSB    0
/// AC_2MOT field width
#define NXMAC_AC_2MOT_WIDTH  ((uint32_t)0x00000010)

/// AC_3MOT field reset value
#define NXMAC_AC_3MOT_RST    0x2F
/// AC_2MOT field reset value
#define NXMAC_AC_2MOT_RST    0x5E

/**
 * @brief Unpacks MOT_2's fields from current value of the MOT_2 register.
 *
 * Reads the MOT_2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] ac3mot - Will be populated with the current value of this field from the register.
 * @param[out] ac2mot - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_mot_2_unpack(uint16_t *ac3mot, uint16_t *ac2mot)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MOT_2_ADDR);

    *ac3mot = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *ac2mot = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the ac3MOT field in the MOT_2 register.
 *
 * The MOT_2 register will be read and the ac3MOT field's value will be returned.
 *
 * @return The current value of the ac3MOT field in the MOT_2 register.
 */
__INLINE uint16_t nxmac_ac_3mot_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MOT_2_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Returns the current value of the ac2MOT field in the MOT_2 register.
 *
 * The MOT_2 register will be read and the ac2MOT field's value will be returned.
 *
 * @return The current value of the ac2MOT field in the MOT_2 register.
 */
__INLINE uint16_t nxmac_ac_2mot_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MOT_2_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/// @}

/**
 * @name MOT_3 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:16              miscMOT   0x0
 *  15:00           hccaQAPMOT   0x0
 * </pre>
 *
 * @{
 */

/// Address of the MOT_3 register
#define NXMAC_MOT_3_ADDR   0xC0008230
/// Offset of the MOT_3 register from the base address
#define NXMAC_MOT_3_OFFSET 0x00000230
/// Index of the MOT_3 register
#define NXMAC_MOT_3_INDEX  0x0000008C
/// Reset value of the MOT_3 register
#define NXMAC_MOT_3_RESET  0x00000000

/**
 * @brief Returns the current value of the MOT_3 register.
 * The MOT_3 register will be read and its value returned.
 * @return The current value of the MOT_3 register.
 */
__INLINE uint32_t nxmac_mot_3_get(void)
{
    return REG_PL_RD(NXMAC_MOT_3_ADDR);
}

// field definitions
/// MISC_MOT field mask
#define NXMAC_MISC_MOT_MASK      ((uint32_t)0xFFFF0000)
/// MISC_MOT field LSB position
#define NXMAC_MISC_MOT_LSB       16
/// MISC_MOT field width
#define NXMAC_MISC_MOT_WIDTH     ((uint32_t)0x00000010)
/// HCCA_QAPMOT field mask
#define NXMAC_HCCA_QAPMOT_MASK   ((uint32_t)0x0000FFFF)
/// HCCA_QAPMOT field LSB position
#define NXMAC_HCCA_QAPMOT_LSB    0
/// HCCA_QAPMOT field width
#define NXMAC_HCCA_QAPMOT_WIDTH  ((uint32_t)0x00000010)

/// MISC_MOT field reset value
#define NXMAC_MISC_MOT_RST       0x0
/// HCCA_QAPMOT field reset value
#define NXMAC_HCCA_QAPMOT_RST    0x0

/**
 * @brief Unpacks MOT_3's fields from current value of the MOT_3 register.
 *
 * Reads the MOT_3 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] miscmot - Will be populated with the current value of this field from the register.
 * @param[out] hccaqapmot - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_mot_3_unpack(uint16_t *miscmot, uint16_t *hccaqapmot)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MOT_3_ADDR);

    *miscmot = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *hccaqapmot = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the miscMOT field in the MOT_3 register.
 *
 * The MOT_3 register will be read and the miscMOT field's value will be returned.
 *
 * @return The current value of the miscMOT field in the MOT_3 register.
 */
__INLINE uint16_t nxmac_misc_mot_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MOT_3_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Returns the current value of the hccaQAPMOT field in the MOT_3 register.
 *
 * The MOT_3 register will be read and the hccaQAPMOT field's value will be returned.
 *
 * @return The current value of the hccaQAPMOT field in the MOT_3 register.
 */
__INLINE uint16_t nxmac_hcca_qapmot_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MOT_3_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/// @}

/**
 * @name TX_BW_DROP_INFO register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  01:00        txBWAfterDrop   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TX_BW_DROP_INFO register
#define NXMAC_TX_BW_DROP_INFO_ADDR   0xC0008330
/// Offset of the TX_BW_DROP_INFO register from the base address
#define NXMAC_TX_BW_DROP_INFO_OFFSET 0x00000330
/// Index of the TX_BW_DROP_INFO register
#define NXMAC_TX_BW_DROP_INFO_INDEX  0x000000CC
/// Reset value of the TX_BW_DROP_INFO register
#define NXMAC_TX_BW_DROP_INFO_RESET  0x00000000

/**
 * @brief Returns the current value of the TX_BW_DROP_INFO register.
 * The TX_BW_DROP_INFO register will be read and its value returned.
 * @return The current value of the TX_BW_DROP_INFO register.
 */
__INLINE uint32_t nxmac_tx_bw_drop_info_get(void)
{
    return REG_PL_RD(NXMAC_TX_BW_DROP_INFO_ADDR);
}

// field definitions
/// TX_BW_AFTER_DROP field mask
#define NXMAC_TX_BW_AFTER_DROP_MASK   ((uint32_t)0x00000003)
/// TX_BW_AFTER_DROP field LSB position
#define NXMAC_TX_BW_AFTER_DROP_LSB    0
/// TX_BW_AFTER_DROP field width
#define NXMAC_TX_BW_AFTER_DROP_WIDTH  ((uint32_t)0x00000002)

/// TX_BW_AFTER_DROP field reset value
#define NXMAC_TX_BW_AFTER_DROP_RST    0x0

/**
 * @brief Returns the current value of the txBWAfterDrop field in the TX_BW_DROP_INFO register.
 *
 * The TX_BW_DROP_INFO register will be read and the txBWAfterDrop field's value will be returned.
 *
 * @return The current value of the txBWAfterDrop field in the TX_BW_DROP_INFO register.
 */
__INLINE uint8_t nxmac_tx_bw_after_drop_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_BW_DROP_INFO_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000003)) == 0);
    return (localVal >> 0);
}

/// @}

#if RW_MUMIMO_TX_EN
/**
 * @name SEC_USERS_TX_INT_EVENT_SET register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     23   secU3ac3TxBufTrigger   0
 *     22   secU3ac2TxBufTrigger   0
 *     21   secU3ac1TxBufTrigger   0
 *     20   secU3ac0TxBufTrigger   0
 *     19    secU3ac3TxTrigger   0
 *     18    secU3ac2TxTrigger   0
 *     17    secU3ac1TxTrigger   0
 *     16    secU3ac0TxTrigger   0
 *     15   secU2ac3TxBufTrigger   0
 *     14   secU2ac2TxBufTrigger   0
 *     13   secU2ac1TxBufTrigger   0
 *     12   secU2ac0TxBufTrigger   0
 *     11    secU2ac3TxTrigger   0
 *     10    secU2ac2TxTrigger   0
 *     09    secU2ac1TxTrigger   0
 *     08    secU2ac0TxTrigger   0
 *     07   secU1ac3TxBufTrigger   0
 *     06   secU1ac2TxBufTrigger   0
 *     05   secU1ac1TxBufTrigger   0
 *     04   secU1ac0TxBufTrigger   0
 *     03    secU1ac3TxTrigger   0
 *     02    secU1ac2TxTrigger   0
 *     01    secU1ac1TxTrigger   0
 *     00    secU1ac0TxTrigger   0
 * </pre>
 *
 * @{
 */

/// Address of the SEC_USERS_TX_INT_EVENT_SET register
#define NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR   0xC0008364
/// Offset of the SEC_USERS_TX_INT_EVENT_SET register from the base address
#define NXMAC_SEC_USERS_TX_INT_EVENT_SET_OFFSET 0x00000364
/// Index of the SEC_USERS_TX_INT_EVENT_SET register
#define NXMAC_SEC_USERS_TX_INT_EVENT_SET_INDEX  0x000000D9
/// Reset value of the SEC_USERS_TX_INT_EVENT_SET register
#define NXMAC_SEC_USERS_TX_INT_EVENT_SET_RESET  0x00000000

/**
 * @brief Returns the current value of the SEC_USERS_TX_INT_EVENT_SET register.
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and its value returned.
 * @return The current value of the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint32_t nxmac_sec_users_tx_int_event_get(void)
{
    return REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
}

/**
 * @brief Sets the SEC_USERS_TX_INT_EVENT_SET register to a value.
 * The SEC_USERS_TX_INT_EVENT_SET register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_sec_users_tx_int_event_set(uint32_t value)
{
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, value);
}

// field definitions
/// SEC_U_3AC_3_TX_BUF_TRIGGER field bit
#define NXMAC_SEC_U_3AC_3_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00800000)
/// SEC_U_3AC_3_TX_BUF_TRIGGER field position
#define NXMAC_SEC_U_3AC_3_TX_BUF_TRIGGER_POS    23
/// SEC_U_3AC_2_TX_BUF_TRIGGER field bit
#define NXMAC_SEC_U_3AC_2_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00400000)
/// SEC_U_3AC_2_TX_BUF_TRIGGER field position
#define NXMAC_SEC_U_3AC_2_TX_BUF_TRIGGER_POS    22
/// SEC_U_3AC_1_TX_BUF_TRIGGER field bit
#define NXMAC_SEC_U_3AC_1_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00200000)
/// SEC_U_3AC_1_TX_BUF_TRIGGER field position
#define NXMAC_SEC_U_3AC_1_TX_BUF_TRIGGER_POS    21
/// SEC_U_3AC_0_TX_BUF_TRIGGER field bit
#define NXMAC_SEC_U_3AC_0_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00100000)
/// SEC_U_3AC_0_TX_BUF_TRIGGER field position
#define NXMAC_SEC_U_3AC_0_TX_BUF_TRIGGER_POS    20
/// SEC_U_3AC_3_TX_TRIGGER field bit
#define NXMAC_SEC_U_3AC_3_TX_TRIGGER_BIT        ((uint32_t)0x00080000)
/// SEC_U_3AC_3_TX_TRIGGER field position
#define NXMAC_SEC_U_3AC_3_TX_TRIGGER_POS        19
/// SEC_U_3AC_2_TX_TRIGGER field bit
#define NXMAC_SEC_U_3AC_2_TX_TRIGGER_BIT        ((uint32_t)0x00040000)
/// SEC_U_3AC_2_TX_TRIGGER field position
#define NXMAC_SEC_U_3AC_2_TX_TRIGGER_POS        18
/// SEC_U_3AC_1_TX_TRIGGER field bit
#define NXMAC_SEC_U_3AC_1_TX_TRIGGER_BIT        ((uint32_t)0x00020000)
/// SEC_U_3AC_1_TX_TRIGGER field position
#define NXMAC_SEC_U_3AC_1_TX_TRIGGER_POS        17
/// SEC_U_3AC_0_TX_TRIGGER field bit
#define NXMAC_SEC_U_3AC_0_TX_TRIGGER_BIT        ((uint32_t)0x00010000)
/// SEC_U_3AC_0_TX_TRIGGER field position
#define NXMAC_SEC_U_3AC_0_TX_TRIGGER_POS        16
/// SEC_U_2AC_3_TX_BUF_TRIGGER field bit
#define NXMAC_SEC_U_2AC_3_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00008000)
/// SEC_U_2AC_3_TX_BUF_TRIGGER field position
#define NXMAC_SEC_U_2AC_3_TX_BUF_TRIGGER_POS    15
/// SEC_U_2AC_2_TX_BUF_TRIGGER field bit
#define NXMAC_SEC_U_2AC_2_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00004000)
/// SEC_U_2AC_2_TX_BUF_TRIGGER field position
#define NXMAC_SEC_U_2AC_2_TX_BUF_TRIGGER_POS    14
/// SEC_U_2AC_1_TX_BUF_TRIGGER field bit
#define NXMAC_SEC_U_2AC_1_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00002000)
/// SEC_U_2AC_1_TX_BUF_TRIGGER field position
#define NXMAC_SEC_U_2AC_1_TX_BUF_TRIGGER_POS    13
/// SEC_U_2AC_0_TX_BUF_TRIGGER field bit
#define NXMAC_SEC_U_2AC_0_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00001000)
/// SEC_U_2AC_0_TX_BUF_TRIGGER field position
#define NXMAC_SEC_U_2AC_0_TX_BUF_TRIGGER_POS    12
/// SEC_U_2AC_3_TX_TRIGGER field bit
#define NXMAC_SEC_U_2AC_3_TX_TRIGGER_BIT        ((uint32_t)0x00000800)
/// SEC_U_2AC_3_TX_TRIGGER field position
#define NXMAC_SEC_U_2AC_3_TX_TRIGGER_POS        11
/// SEC_U_2AC_2_TX_TRIGGER field bit
#define NXMAC_SEC_U_2AC_2_TX_TRIGGER_BIT        ((uint32_t)0x00000400)
/// SEC_U_2AC_2_TX_TRIGGER field position
#define NXMAC_SEC_U_2AC_2_TX_TRIGGER_POS        10
/// SEC_U_2AC_1_TX_TRIGGER field bit
#define NXMAC_SEC_U_2AC_1_TX_TRIGGER_BIT        ((uint32_t)0x00000200)
/// SEC_U_2AC_1_TX_TRIGGER field position
#define NXMAC_SEC_U_2AC_1_TX_TRIGGER_POS        9
/// SEC_U_2AC_0_TX_TRIGGER field bit
#define NXMAC_SEC_U_2AC_0_TX_TRIGGER_BIT        ((uint32_t)0x00000100)
/// SEC_U_2AC_0_TX_TRIGGER field position
#define NXMAC_SEC_U_2AC_0_TX_TRIGGER_POS        8
/// SEC_U_1AC_3_TX_BUF_TRIGGER field bit
#define NXMAC_SEC_U_1AC_3_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00000080)
/// SEC_U_1AC_3_TX_BUF_TRIGGER field position
#define NXMAC_SEC_U_1AC_3_TX_BUF_TRIGGER_POS    7
/// SEC_U_1AC_2_TX_BUF_TRIGGER field bit
#define NXMAC_SEC_U_1AC_2_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00000040)
/// SEC_U_1AC_2_TX_BUF_TRIGGER field position
#define NXMAC_SEC_U_1AC_2_TX_BUF_TRIGGER_POS    6
/// SEC_U_1AC_1_TX_BUF_TRIGGER field bit
#define NXMAC_SEC_U_1AC_1_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00000020)
/// SEC_U_1AC_1_TX_BUF_TRIGGER field position
#define NXMAC_SEC_U_1AC_1_TX_BUF_TRIGGER_POS    5
/// SEC_U_1AC_0_TX_BUF_TRIGGER field bit
#define NXMAC_SEC_U_1AC_0_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00000010)
/// SEC_U_1AC_0_TX_BUF_TRIGGER field position
#define NXMAC_SEC_U_1AC_0_TX_BUF_TRIGGER_POS    4
/// SEC_U_1AC_3_TX_TRIGGER field bit
#define NXMAC_SEC_U_1AC_3_TX_TRIGGER_BIT        ((uint32_t)0x00000008)
/// SEC_U_1AC_3_TX_TRIGGER field position
#define NXMAC_SEC_U_1AC_3_TX_TRIGGER_POS        3
/// SEC_U_1AC_2_TX_TRIGGER field bit
#define NXMAC_SEC_U_1AC_2_TX_TRIGGER_BIT        ((uint32_t)0x00000004)
/// SEC_U_1AC_2_TX_TRIGGER field position
#define NXMAC_SEC_U_1AC_2_TX_TRIGGER_POS        2
/// SEC_U_1AC_1_TX_TRIGGER field bit
#define NXMAC_SEC_U_1AC_1_TX_TRIGGER_BIT        ((uint32_t)0x00000002)
/// SEC_U_1AC_1_TX_TRIGGER field position
#define NXMAC_SEC_U_1AC_1_TX_TRIGGER_POS        1
/// SEC_U_1AC_0_TX_TRIGGER field bit
#define NXMAC_SEC_U_1AC_0_TX_TRIGGER_BIT        ((uint32_t)0x00000001)
/// SEC_U_1AC_0_TX_TRIGGER field position
#define NXMAC_SEC_U_1AC_0_TX_TRIGGER_POS        0

/// SEC_U_3AC_3_TX_BUF_TRIGGER field reset value
#define NXMAC_SEC_U_3AC_3_TX_BUF_TRIGGER_RST    0x0
/// SEC_U_3AC_2_TX_BUF_TRIGGER field reset value
#define NXMAC_SEC_U_3AC_2_TX_BUF_TRIGGER_RST    0x0
/// SEC_U_3AC_1_TX_BUF_TRIGGER field reset value
#define NXMAC_SEC_U_3AC_1_TX_BUF_TRIGGER_RST    0x0
/// SEC_U_3AC_0_TX_BUF_TRIGGER field reset value
#define NXMAC_SEC_U_3AC_0_TX_BUF_TRIGGER_RST    0x0
/// SEC_U_3AC_3_TX_TRIGGER field reset value
#define NXMAC_SEC_U_3AC_3_TX_TRIGGER_RST        0x0
/// SEC_U_3AC_2_TX_TRIGGER field reset value
#define NXMAC_SEC_U_3AC_2_TX_TRIGGER_RST        0x0
/// SEC_U_3AC_1_TX_TRIGGER field reset value
#define NXMAC_SEC_U_3AC_1_TX_TRIGGER_RST        0x0
/// SEC_U_3AC_0_TX_TRIGGER field reset value
#define NXMAC_SEC_U_3AC_0_TX_TRIGGER_RST        0x0
/// SEC_U_2AC_3_TX_BUF_TRIGGER field reset value
#define NXMAC_SEC_U_2AC_3_TX_BUF_TRIGGER_RST    0x0
/// SEC_U_2AC_2_TX_BUF_TRIGGER field reset value
#define NXMAC_SEC_U_2AC_2_TX_BUF_TRIGGER_RST    0x0
/// SEC_U_2AC_1_TX_BUF_TRIGGER field reset value
#define NXMAC_SEC_U_2AC_1_TX_BUF_TRIGGER_RST    0x0
/// SEC_U_2AC_0_TX_BUF_TRIGGER field reset value
#define NXMAC_SEC_U_2AC_0_TX_BUF_TRIGGER_RST    0x0
/// SEC_U_2AC_3_TX_TRIGGER field reset value
#define NXMAC_SEC_U_2AC_3_TX_TRIGGER_RST        0x0
/// SEC_U_2AC_2_TX_TRIGGER field reset value
#define NXMAC_SEC_U_2AC_2_TX_TRIGGER_RST        0x0
/// SEC_U_2AC_1_TX_TRIGGER field reset value
#define NXMAC_SEC_U_2AC_1_TX_TRIGGER_RST        0x0
/// SEC_U_2AC_0_TX_TRIGGER field reset value
#define NXMAC_SEC_U_2AC_0_TX_TRIGGER_RST        0x0
/// SEC_U_1AC_3_TX_BUF_TRIGGER field reset value
#define NXMAC_SEC_U_1AC_3_TX_BUF_TRIGGER_RST    0x0
/// SEC_U_1AC_2_TX_BUF_TRIGGER field reset value
#define NXMAC_SEC_U_1AC_2_TX_BUF_TRIGGER_RST    0x0
/// SEC_U_1AC_1_TX_BUF_TRIGGER field reset value
#define NXMAC_SEC_U_1AC_1_TX_BUF_TRIGGER_RST    0x0
/// SEC_U_1AC_0_TX_BUF_TRIGGER field reset value
#define NXMAC_SEC_U_1AC_0_TX_BUF_TRIGGER_RST    0x0
/// SEC_U_1AC_3_TX_TRIGGER field reset value
#define NXMAC_SEC_U_1AC_3_TX_TRIGGER_RST        0x0
/// SEC_U_1AC_2_TX_TRIGGER field reset value
#define NXMAC_SEC_U_1AC_2_TX_TRIGGER_RST        0x0
/// SEC_U_1AC_1_TX_TRIGGER field reset value
#define NXMAC_SEC_U_1AC_1_TX_TRIGGER_RST        0x0
/// SEC_U_1AC_0_TX_TRIGGER field reset value
#define NXMAC_SEC_U_1AC_0_TX_TRIGGER_RST        0x0

/**
 * @brief Constructs a value for the SEC_USERS_TX_INT_EVENT_SET register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] secu3ac3txbuftrigger - The value to use for the secU3ac3TxBufTrigger field.
 * @param[in] secu3ac2txbuftrigger - The value to use for the secU3ac2TxBufTrigger field.
 * @param[in] secu3ac1txbuftrigger - The value to use for the secU3ac1TxBufTrigger field.
 * @param[in] secu3ac0txbuftrigger - The value to use for the secU3ac0TxBufTrigger field.
 * @param[in] secu3ac3txtrigger - The value to use for the secU3ac3TxTrigger field.
 * @param[in] secu3ac2txtrigger - The value to use for the secU3ac2TxTrigger field.
 * @param[in] secu3ac1txtrigger - The value to use for the secU3ac1TxTrigger field.
 * @param[in] secu3ac0txtrigger - The value to use for the secU3ac0TxTrigger field.
 * @param[in] secu2ac3txbuftrigger - The value to use for the secU2ac3TxBufTrigger field.
 * @param[in] secu2ac2txbuftrigger - The value to use for the secU2ac2TxBufTrigger field.
 * @param[in] secu2ac1txbuftrigger - The value to use for the secU2ac1TxBufTrigger field.
 * @param[in] secu2ac0txbuftrigger - The value to use for the secU2ac0TxBufTrigger field.
 * @param[in] secu2ac3txtrigger - The value to use for the secU2ac3TxTrigger field.
 * @param[in] secu2ac2txtrigger - The value to use for the secU2ac2TxTrigger field.
 * @param[in] secu2ac1txtrigger - The value to use for the secU2ac1TxTrigger field.
 * @param[in] secu2ac0txtrigger - The value to use for the secU2ac0TxTrigger field.
 * @param[in] secu1ac3txbuftrigger - The value to use for the secU1ac3TxBufTrigger field.
 * @param[in] secu1ac2txbuftrigger - The value to use for the secU1ac2TxBufTrigger field.
 * @param[in] secu1ac1txbuftrigger - The value to use for the secU1ac1TxBufTrigger field.
 * @param[in] secu1ac0txbuftrigger - The value to use for the secU1ac0TxBufTrigger field.
 * @param[in] secu1ac3txtrigger - The value to use for the secU1ac3TxTrigger field.
 * @param[in] secu1ac2txtrigger - The value to use for the secU1ac2TxTrigger field.
 * @param[in] secu1ac1txtrigger - The value to use for the secU1ac1TxTrigger field.
 * @param[in] secu1ac0txtrigger - The value to use for the secU1ac0TxTrigger field.
 */
__INLINE void nxmac_sec_users_tx_int_event_set_pack(uint8_t secu3ac3txbuftrigger, uint8_t secu3ac2txbuftrigger, uint8_t secu3ac1txbuftrigger, uint8_t secu3ac0txbuftrigger, uint8_t secu3ac3txtrigger, uint8_t secu3ac2txtrigger, uint8_t secu3ac1txtrigger, uint8_t secu3ac0txtrigger, uint8_t secu2ac3txbuftrigger, uint8_t secu2ac2txbuftrigger, uint8_t secu2ac1txbuftrigger, uint8_t secu2ac0txbuftrigger, uint8_t secu2ac3txtrigger, uint8_t secu2ac2txtrigger, uint8_t secu2ac1txtrigger, uint8_t secu2ac0txtrigger, uint8_t secu1ac3txbuftrigger, uint8_t secu1ac2txbuftrigger, uint8_t secu1ac1txbuftrigger, uint8_t secu1ac0txbuftrigger, uint8_t secu1ac3txtrigger, uint8_t secu1ac2txtrigger, uint8_t secu1ac1txtrigger, uint8_t secu1ac0txtrigger)
{
    ASSERT_ERR((((uint32_t)secu3ac3txbuftrigger << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)secu3ac2txbuftrigger << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)secu3ac1txbuftrigger << 21) & ~((uint32_t)0x00200000)) == 0);
    ASSERT_ERR((((uint32_t)secu3ac0txbuftrigger << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)secu3ac3txtrigger << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)secu3ac2txtrigger << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)secu3ac1txtrigger << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)secu3ac0txtrigger << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)secu2ac3txbuftrigger << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)secu2ac2txbuftrigger << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)secu2ac1txbuftrigger << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)secu2ac0txbuftrigger << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)secu2ac3txtrigger << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)secu2ac2txtrigger << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)secu2ac1txtrigger << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)secu2ac0txtrigger << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)secu1ac3txbuftrigger << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)secu1ac2txbuftrigger << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)secu1ac1txbuftrigger << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)secu1ac0txbuftrigger << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)secu1ac3txtrigger << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)secu1ac2txtrigger << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)secu1ac1txtrigger << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)secu1ac0txtrigger << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR,  ((uint32_t)secu3ac3txbuftrigger << 23) | ((uint32_t)secu3ac2txbuftrigger << 22) | ((uint32_t)secu3ac1txbuftrigger << 21) | ((uint32_t)secu3ac0txbuftrigger << 20) | ((uint32_t)secu3ac3txtrigger << 19) | ((uint32_t)secu3ac2txtrigger << 18) | ((uint32_t)secu3ac1txtrigger << 17) | ((uint32_t)secu3ac0txtrigger << 16) | ((uint32_t)secu2ac3txbuftrigger << 15) | ((uint32_t)secu2ac2txbuftrigger << 14) | ((uint32_t)secu2ac1txbuftrigger << 13) | ((uint32_t)secu2ac0txbuftrigger << 12) | ((uint32_t)secu2ac3txtrigger << 11) | ((uint32_t)secu2ac2txtrigger << 10) | ((uint32_t)secu2ac1txtrigger << 9) | ((uint32_t)secu2ac0txtrigger << 8) | ((uint32_t)secu1ac3txbuftrigger << 7) | ((uint32_t)secu1ac2txbuftrigger << 6) | ((uint32_t)secu1ac1txbuftrigger << 5) | ((uint32_t)secu1ac0txbuftrigger << 4) | ((uint32_t)secu1ac3txtrigger << 3) | ((uint32_t)secu1ac2txtrigger << 2) | ((uint32_t)secu1ac1txtrigger << 1) | ((uint32_t)secu1ac0txtrigger << 0));
}

/**
 * @brief Unpacks SEC_USERS_TX_INT_EVENT_SET's fields from current value of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * Reads the SEC_USERS_TX_INT_EVENT_SET register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] secu3ac3txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu3ac2txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu3ac1txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu3ac0txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu3ac3txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu3ac2txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu3ac1txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu3ac0txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu2ac3txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu2ac2txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu2ac1txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu2ac0txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu2ac3txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu2ac2txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu2ac1txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu2ac0txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu1ac3txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu1ac2txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu1ac1txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu1ac0txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu1ac3txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu1ac2txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu1ac1txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] secu1ac0txtrigger - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_sec_users_tx_int_event_unpack(uint8_t *secu3ac3txbuftrigger, uint8_t *secu3ac2txbuftrigger, uint8_t *secu3ac1txbuftrigger, uint8_t *secu3ac0txbuftrigger, uint8_t *secu3ac3txtrigger, uint8_t *secu3ac2txtrigger, uint8_t *secu3ac1txtrigger, uint8_t *secu3ac0txtrigger, uint8_t *secu2ac3txbuftrigger, uint8_t *secu2ac2txbuftrigger, uint8_t *secu2ac1txbuftrigger, uint8_t *secu2ac0txbuftrigger, uint8_t *secu2ac3txtrigger, uint8_t *secu2ac2txtrigger, uint8_t *secu2ac1txtrigger, uint8_t *secu2ac0txtrigger, uint8_t *secu1ac3txbuftrigger, uint8_t *secu1ac2txbuftrigger, uint8_t *secu1ac1txbuftrigger, uint8_t *secu1ac0txbuftrigger, uint8_t *secu1ac3txtrigger, uint8_t *secu1ac2txtrigger, uint8_t *secu1ac1txtrigger, uint8_t *secu1ac0txtrigger)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);

    *secu3ac3txbuftrigger = (localVal & ((uint32_t)0x00800000)) >> 23;
    *secu3ac2txbuftrigger = (localVal & ((uint32_t)0x00400000)) >> 22;
    *secu3ac1txbuftrigger = (localVal & ((uint32_t)0x00200000)) >> 21;
    *secu3ac0txbuftrigger = (localVal & ((uint32_t)0x00100000)) >> 20;
    *secu3ac3txtrigger = (localVal & ((uint32_t)0x00080000)) >> 19;
    *secu3ac2txtrigger = (localVal & ((uint32_t)0x00040000)) >> 18;
    *secu3ac1txtrigger = (localVal & ((uint32_t)0x00020000)) >> 17;
    *secu3ac0txtrigger = (localVal & ((uint32_t)0x00010000)) >> 16;
    *secu2ac3txbuftrigger = (localVal & ((uint32_t)0x00008000)) >> 15;
    *secu2ac2txbuftrigger = (localVal & ((uint32_t)0x00004000)) >> 14;
    *secu2ac1txbuftrigger = (localVal & ((uint32_t)0x00002000)) >> 13;
    *secu2ac0txbuftrigger = (localVal & ((uint32_t)0x00001000)) >> 12;
    *secu2ac3txtrigger = (localVal & ((uint32_t)0x00000800)) >> 11;
    *secu2ac2txtrigger = (localVal & ((uint32_t)0x00000400)) >> 10;
    *secu2ac1txtrigger = (localVal & ((uint32_t)0x00000200)) >> 9;
    *secu2ac0txtrigger = (localVal & ((uint32_t)0x00000100)) >> 8;
    *secu1ac3txbuftrigger = (localVal & ((uint32_t)0x00000080)) >> 7;
    *secu1ac2txbuftrigger = (localVal & ((uint32_t)0x00000040)) >> 6;
    *secu1ac1txbuftrigger = (localVal & ((uint32_t)0x00000020)) >> 5;
    *secu1ac0txbuftrigger = (localVal & ((uint32_t)0x00000010)) >> 4;
    *secu1ac3txtrigger = (localVal & ((uint32_t)0x00000008)) >> 3;
    *secu1ac2txtrigger = (localVal & ((uint32_t)0x00000004)) >> 2;
    *secu1ac1txtrigger = (localVal & ((uint32_t)0x00000002)) >> 1;
    *secu1ac0txtrigger = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the secU3ac3TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU3ac3TxBufTrigger field's value will be returned.
 *
 * @return The current value of the secU3ac3TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_3ac_3_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Sets the secU3ac3TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu3ac3txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_3ac_3_tx_buf_trigger_setf(uint8_t secu3ac3txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu3ac3txbuftrigger << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu3ac3txbuftrigger << 23);
}

/**
 * @brief Returns the current value of the secU3ac2TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU3ac2TxBufTrigger field's value will be returned.
 *
 * @return The current value of the secU3ac2TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_3ac_2_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Sets the secU3ac2TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu3ac2txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_3ac_2_tx_buf_trigger_setf(uint8_t secu3ac2txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu3ac2txbuftrigger << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu3ac2txbuftrigger << 22);
}

/**
 * @brief Returns the current value of the secU3ac1TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU3ac1TxBufTrigger field's value will be returned.
 *
 * @return The current value of the secU3ac1TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_3ac_1_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Sets the secU3ac1TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu3ac1txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_3ac_1_tx_buf_trigger_setf(uint8_t secu3ac1txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu3ac1txbuftrigger << 21) & ~((uint32_t)0x00200000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu3ac1txbuftrigger << 21);
}

/**
 * @brief Returns the current value of the secU3ac0TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU3ac0TxBufTrigger field's value will be returned.
 *
 * @return The current value of the secU3ac0TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_3ac_0_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Sets the secU3ac0TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu3ac0txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_3ac_0_tx_buf_trigger_setf(uint8_t secu3ac0txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu3ac0txbuftrigger << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu3ac0txbuftrigger << 20);
}

/**
 * @brief Returns the current value of the secU3ac3TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU3ac3TxTrigger field's value will be returned.
 *
 * @return The current value of the secU3ac3TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_3ac_3_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Sets the secU3ac3TxTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu3ac3txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_3ac_3_tx_trigger_setf(uint8_t secu3ac3txtrigger)
{
    ASSERT_ERR((((uint32_t)secu3ac3txtrigger << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu3ac3txtrigger << 19);
}

/**
 * @brief Returns the current value of the secU3ac2TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU3ac2TxTrigger field's value will be returned.
 *
 * @return The current value of the secU3ac2TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_3ac_2_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Sets the secU3ac2TxTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu3ac2txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_3ac_2_tx_trigger_setf(uint8_t secu3ac2txtrigger)
{
    ASSERT_ERR((((uint32_t)secu3ac2txtrigger << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu3ac2txtrigger << 18);
}

/**
 * @brief Returns the current value of the secU3ac1TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU3ac1TxTrigger field's value will be returned.
 *
 * @return The current value of the secU3ac1TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_3ac_1_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Sets the secU3ac1TxTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu3ac1txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_3ac_1_tx_trigger_setf(uint8_t secu3ac1txtrigger)
{
    ASSERT_ERR((((uint32_t)secu3ac1txtrigger << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu3ac1txtrigger << 17);
}

/**
 * @brief Returns the current value of the secU3ac0TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU3ac0TxTrigger field's value will be returned.
 *
 * @return The current value of the secU3ac0TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_3ac_0_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the secU3ac0TxTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu3ac0txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_3ac_0_tx_trigger_setf(uint8_t secu3ac0txtrigger)
{
    ASSERT_ERR((((uint32_t)secu3ac0txtrigger << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu3ac0txtrigger << 16);
}

/**
 * @brief Returns the current value of the secU2ac3TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU2ac3TxBufTrigger field's value will be returned.
 *
 * @return The current value of the secU2ac3TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_2ac_3_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

/**
 * @brief Sets the secU2ac3TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu2ac3txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_2ac_3_tx_buf_trigger_setf(uint8_t secu2ac3txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu2ac3txbuftrigger << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu2ac3txbuftrigger << 15);
}

/**
 * @brief Returns the current value of the secU2ac2TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU2ac2TxBufTrigger field's value will be returned.
 *
 * @return The current value of the secU2ac2TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_2ac_2_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Sets the secU2ac2TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu2ac2txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_2ac_2_tx_buf_trigger_setf(uint8_t secu2ac2txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu2ac2txbuftrigger << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu2ac2txbuftrigger << 14);
}

/**
 * @brief Returns the current value of the secU2ac1TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU2ac1TxBufTrigger field's value will be returned.
 *
 * @return The current value of the secU2ac1TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_2ac_1_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the secU2ac1TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu2ac1txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_2ac_1_tx_buf_trigger_setf(uint8_t secu2ac1txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu2ac1txbuftrigger << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu2ac1txbuftrigger << 13);
}

/**
 * @brief Returns the current value of the secU2ac0TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU2ac0TxBufTrigger field's value will be returned.
 *
 * @return The current value of the secU2ac0TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_2ac_0_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the secU2ac0TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu2ac0txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_2ac_0_tx_buf_trigger_setf(uint8_t secu2ac0txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu2ac0txbuftrigger << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu2ac0txbuftrigger << 12);
}

/**
 * @brief Returns the current value of the secU2ac3TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU2ac3TxTrigger field's value will be returned.
 *
 * @return The current value of the secU2ac3TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_2ac_3_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the secU2ac3TxTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu2ac3txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_2ac_3_tx_trigger_setf(uint8_t secu2ac3txtrigger)
{
    ASSERT_ERR((((uint32_t)secu2ac3txtrigger << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu2ac3txtrigger << 11);
}

/**
 * @brief Returns the current value of the secU2ac2TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU2ac2TxTrigger field's value will be returned.
 *
 * @return The current value of the secU2ac2TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_2ac_2_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the secU2ac2TxTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu2ac2txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_2ac_2_tx_trigger_setf(uint8_t secu2ac2txtrigger)
{
    ASSERT_ERR((((uint32_t)secu2ac2txtrigger << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu2ac2txtrigger << 10);
}

/**
 * @brief Returns the current value of the secU2ac1TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU2ac1TxTrigger field's value will be returned.
 *
 * @return The current value of the secU2ac1TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_2ac_1_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the secU2ac1TxTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu2ac1txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_2ac_1_tx_trigger_setf(uint8_t secu2ac1txtrigger)
{
    ASSERT_ERR((((uint32_t)secu2ac1txtrigger << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu2ac1txtrigger << 9);
}

/**
 * @brief Returns the current value of the secU2ac0TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU2ac0TxTrigger field's value will be returned.
 *
 * @return The current value of the secU2ac0TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_2ac_0_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the secU2ac0TxTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu2ac0txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_2ac_0_tx_trigger_setf(uint8_t secu2ac0txtrigger)
{
    ASSERT_ERR((((uint32_t)secu2ac0txtrigger << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu2ac0txtrigger << 8);
}

/**
 * @brief Returns the current value of the secU1ac3TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU1ac3TxBufTrigger field's value will be returned.
 *
 * @return The current value of the secU1ac3TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_1ac_3_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the secU1ac3TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu1ac3txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_1ac_3_tx_buf_trigger_setf(uint8_t secu1ac3txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu1ac3txbuftrigger << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu1ac3txbuftrigger << 7);
}

/**
 * @brief Returns the current value of the secU1ac2TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU1ac2TxBufTrigger field's value will be returned.
 *
 * @return The current value of the secU1ac2TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_1ac_2_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the secU1ac2TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu1ac2txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_1ac_2_tx_buf_trigger_setf(uint8_t secu1ac2txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu1ac2txbuftrigger << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu1ac2txbuftrigger << 6);
}

/**
 * @brief Returns the current value of the secU1ac1TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU1ac1TxBufTrigger field's value will be returned.
 *
 * @return The current value of the secU1ac1TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_1ac_1_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the secU1ac1TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu1ac1txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_1ac_1_tx_buf_trigger_setf(uint8_t secu1ac1txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu1ac1txbuftrigger << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu1ac1txbuftrigger << 5);
}

/**
 * @brief Returns the current value of the secU1ac0TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU1ac0TxBufTrigger field's value will be returned.
 *
 * @return The current value of the secU1ac0TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_1ac_0_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the secU1ac0TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu1ac0txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_1ac_0_tx_buf_trigger_setf(uint8_t secu1ac0txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu1ac0txbuftrigger << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu1ac0txbuftrigger << 4);
}

/**
 * @brief Returns the current value of the secU1ac3TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU1ac3TxTrigger field's value will be returned.
 *
 * @return The current value of the secU1ac3TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_1ac_3_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the secU1ac3TxTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu1ac3txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_1ac_3_tx_trigger_setf(uint8_t secu1ac3txtrigger)
{
    ASSERT_ERR((((uint32_t)secu1ac3txtrigger << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu1ac3txtrigger << 3);
}

/**
 * @brief Returns the current value of the secU1ac2TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU1ac2TxTrigger field's value will be returned.
 *
 * @return The current value of the secU1ac2TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_1ac_2_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the secU1ac2TxTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu1ac2txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_1ac_2_tx_trigger_setf(uint8_t secu1ac2txtrigger)
{
    ASSERT_ERR((((uint32_t)secu1ac2txtrigger << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu1ac2txtrigger << 2);
}

/**
 * @brief Returns the current value of the secU1ac1TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU1ac1TxTrigger field's value will be returned.
 *
 * @return The current value of the secU1ac1TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_1ac_1_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the secU1ac1TxTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu1ac1txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_1ac_1_tx_trigger_setf(uint8_t secu1ac1txtrigger)
{
    ASSERT_ERR((((uint32_t)secu1ac1txtrigger << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu1ac1txtrigger << 1);
}

/**
 * @brief Returns the current value of the secU1ac0TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read and the secU1ac0TxTrigger field's value will be returned.
 *
 * @return The current value of the secU1ac0TxTrigger field in the SEC_USERS_TX_INT_EVENT_SET register.
 */
__INLINE uint8_t nxmac_sec_u_1ac_0_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the secU1ac0TxTrigger field of the SEC_USERS_TX_INT_EVENT_SET register.
 *
 * The SEC_USERS_TX_INT_EVENT_SET register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu1ac0txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_1ac_0_tx_trigger_setf(uint8_t secu1ac0txtrigger)
{
    ASSERT_ERR((((uint32_t)secu1ac0txtrigger << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_SET_ADDR, (uint32_t)secu1ac0txtrigger << 0);
}

#endif // RW_MUMIMO_TX_EN
/// @}

#if RW_MUMIMO_TX_EN
/**
 * @name SEC_USERS_TX_INT_EVENT_CLEAR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     23   secU3ac3TxBufTrigger   0
 *     22   secU3ac2TxBufTrigger   0
 *     21   secU3ac1TxBufTrigger   0
 *     20   secU3ac0TxBufTrigger   0
 *     19    secU3ac3TxTrigger   0
 *     18    secU3ac2TxTrigger   0
 *     17    secU3ac1TxTrigger   0
 *     16    secU3ac0TxTrigger   0
 *     15   secU2ac3TxBufTrigger   0
 *     14   secU2ac2TxBufTrigger   0
 *     13   secU2ac1TxBufTrigger   0
 *     12   secU2ac0TxBufTrigger   0
 *     11    secU2ac3TxTrigger   0
 *     10    secU2ac2TxTrigger   0
 *     09    secU2ac1TxTrigger   0
 *     08    secU2ac0TxTrigger   0
 *     07   secU1ac3TxBufTrigger   0
 *     06   secU1ac2TxBufTrigger   0
 *     05   secU1ac1TxBufTrigger   0
 *     04   secU1ac0TxBufTrigger   0
 *     03    secU1ac3TxTrigger   0
 *     02    secU1ac2TxTrigger   0
 *     01    secU1ac1TxTrigger   0
 *     00    secU1ac0TxTrigger   0
 * </pre>
 *
 * @{
 */

/// Address of the SEC_USERS_TX_INT_EVENT_CLEAR register
#define NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR   0xC0008368
/// Offset of the SEC_USERS_TX_INT_EVENT_CLEAR register from the base address
#define NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_OFFSET 0x00000368
/// Index of the SEC_USERS_TX_INT_EVENT_CLEAR register
#define NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_INDEX  0x000000DA
/// Reset value of the SEC_USERS_TX_INT_EVENT_CLEAR register
#define NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_RESET  0x00000000

/**
 * @brief Sets the SEC_USERS_TX_INT_EVENT_CLEAR register to a value.
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_sec_users_tx_int_event_clear(uint32_t value)
{
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, value);
}

// fields defined in symmetrical set/clear register
/**
 * @brief Constructs a value for the SEC_USERS_TX_INT_EVENT_CLEAR register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] secu3ac3txbuftrigger - The value to use for the secU3ac3TxBufTrigger field.
 * @param[in] secu3ac2txbuftrigger - The value to use for the secU3ac2TxBufTrigger field.
 * @param[in] secu3ac1txbuftrigger - The value to use for the secU3ac1TxBufTrigger field.
 * @param[in] secu3ac0txbuftrigger - The value to use for the secU3ac0TxBufTrigger field.
 * @param[in] secu3ac3txtrigger - The value to use for the secU3ac3TxTrigger field.
 * @param[in] secu3ac2txtrigger - The value to use for the secU3ac2TxTrigger field.
 * @param[in] secu3ac1txtrigger - The value to use for the secU3ac1TxTrigger field.
 * @param[in] secu3ac0txtrigger - The value to use for the secU3ac0TxTrigger field.
 * @param[in] secu2ac3txbuftrigger - The value to use for the secU2ac3TxBufTrigger field.
 * @param[in] secu2ac2txbuftrigger - The value to use for the secU2ac2TxBufTrigger field.
 * @param[in] secu2ac1txbuftrigger - The value to use for the secU2ac1TxBufTrigger field.
 * @param[in] secu2ac0txbuftrigger - The value to use for the secU2ac0TxBufTrigger field.
 * @param[in] secu2ac3txtrigger - The value to use for the secU2ac3TxTrigger field.
 * @param[in] secu2ac2txtrigger - The value to use for the secU2ac2TxTrigger field.
 * @param[in] secu2ac1txtrigger - The value to use for the secU2ac1TxTrigger field.
 * @param[in] secu2ac0txtrigger - The value to use for the secU2ac0TxTrigger field.
 * @param[in] secu1ac3txbuftrigger - The value to use for the secU1ac3TxBufTrigger field.
 * @param[in] secu1ac2txbuftrigger - The value to use for the secU1ac2TxBufTrigger field.
 * @param[in] secu1ac1txbuftrigger - The value to use for the secU1ac1TxBufTrigger field.
 * @param[in] secu1ac0txbuftrigger - The value to use for the secU1ac0TxBufTrigger field.
 * @param[in] secu1ac3txtrigger - The value to use for the secU1ac3TxTrigger field.
 * @param[in] secu1ac2txtrigger - The value to use for the secU1ac2TxTrigger field.
 * @param[in] secu1ac1txtrigger - The value to use for the secU1ac1TxTrigger field.
 * @param[in] secu1ac0txtrigger - The value to use for the secU1ac0TxTrigger field.
 */
__INLINE void nxmac_sec_users_tx_int_event_clear_pack(uint8_t secu3ac3txbuftrigger, uint8_t secu3ac2txbuftrigger, uint8_t secu3ac1txbuftrigger, uint8_t secu3ac0txbuftrigger, uint8_t secu3ac3txtrigger, uint8_t secu3ac2txtrigger, uint8_t secu3ac1txtrigger, uint8_t secu3ac0txtrigger, uint8_t secu2ac3txbuftrigger, uint8_t secu2ac2txbuftrigger, uint8_t secu2ac1txbuftrigger, uint8_t secu2ac0txbuftrigger, uint8_t secu2ac3txtrigger, uint8_t secu2ac2txtrigger, uint8_t secu2ac1txtrigger, uint8_t secu2ac0txtrigger, uint8_t secu1ac3txbuftrigger, uint8_t secu1ac2txbuftrigger, uint8_t secu1ac1txbuftrigger, uint8_t secu1ac0txbuftrigger, uint8_t secu1ac3txtrigger, uint8_t secu1ac2txtrigger, uint8_t secu1ac1txtrigger, uint8_t secu1ac0txtrigger)
{
    ASSERT_ERR((((uint32_t)secu3ac3txbuftrigger << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)secu3ac2txbuftrigger << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)secu3ac1txbuftrigger << 21) & ~((uint32_t)0x00200000)) == 0);
    ASSERT_ERR((((uint32_t)secu3ac0txbuftrigger << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)secu3ac3txtrigger << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)secu3ac2txtrigger << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)secu3ac1txtrigger << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)secu3ac0txtrigger << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)secu2ac3txbuftrigger << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)secu2ac2txbuftrigger << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)secu2ac1txbuftrigger << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)secu2ac0txbuftrigger << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)secu2ac3txtrigger << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)secu2ac2txtrigger << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)secu2ac1txtrigger << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)secu2ac0txtrigger << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)secu1ac3txbuftrigger << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)secu1ac2txbuftrigger << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)secu1ac1txbuftrigger << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)secu1ac0txbuftrigger << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)secu1ac3txtrigger << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)secu1ac2txtrigger << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)secu1ac1txtrigger << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)secu1ac0txtrigger << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR,  ((uint32_t)secu3ac3txbuftrigger << 23) | ((uint32_t)secu3ac2txbuftrigger << 22) | ((uint32_t)secu3ac1txbuftrigger << 21) | ((uint32_t)secu3ac0txbuftrigger << 20) | ((uint32_t)secu3ac3txtrigger << 19) | ((uint32_t)secu3ac2txtrigger << 18) | ((uint32_t)secu3ac1txtrigger << 17) | ((uint32_t)secu3ac0txtrigger << 16) | ((uint32_t)secu2ac3txbuftrigger << 15) | ((uint32_t)secu2ac2txbuftrigger << 14) | ((uint32_t)secu2ac1txbuftrigger << 13) | ((uint32_t)secu2ac0txbuftrigger << 12) | ((uint32_t)secu2ac3txtrigger << 11) | ((uint32_t)secu2ac2txtrigger << 10) | ((uint32_t)secu2ac1txtrigger << 9) | ((uint32_t)secu2ac0txtrigger << 8) | ((uint32_t)secu1ac3txbuftrigger << 7) | ((uint32_t)secu1ac2txbuftrigger << 6) | ((uint32_t)secu1ac1txbuftrigger << 5) | ((uint32_t)secu1ac0txbuftrigger << 4) | ((uint32_t)secu1ac3txtrigger << 3) | ((uint32_t)secu1ac2txtrigger << 2) | ((uint32_t)secu1ac1txtrigger << 1) | ((uint32_t)secu1ac0txtrigger << 0));
}

/**
 * @brief Sets the secU3ac3TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu3ac3txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_3ac_3_tx_buf_trigger_clearf(uint8_t secu3ac3txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu3ac3txbuftrigger << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu3ac3txbuftrigger << 23);
}

/**
 * @brief Sets the secU3ac2TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu3ac2txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_3ac_2_tx_buf_trigger_clearf(uint8_t secu3ac2txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu3ac2txbuftrigger << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu3ac2txbuftrigger << 22);
}

/**
 * @brief Sets the secU3ac1TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu3ac1txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_3ac_1_tx_buf_trigger_clearf(uint8_t secu3ac1txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu3ac1txbuftrigger << 21) & ~((uint32_t)0x00200000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu3ac1txbuftrigger << 21);
}

/**
 * @brief Sets the secU3ac0TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu3ac0txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_3ac_0_tx_buf_trigger_clearf(uint8_t secu3ac0txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu3ac0txbuftrigger << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu3ac0txbuftrigger << 20);
}

/**
 * @brief Sets the secU3ac3TxTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu3ac3txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_3ac_3_tx_trigger_clearf(uint8_t secu3ac3txtrigger)
{
    ASSERT_ERR((((uint32_t)secu3ac3txtrigger << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu3ac3txtrigger << 19);
}

/**
 * @brief Sets the secU3ac2TxTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu3ac2txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_3ac_2_tx_trigger_clearf(uint8_t secu3ac2txtrigger)
{
    ASSERT_ERR((((uint32_t)secu3ac2txtrigger << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu3ac2txtrigger << 18);
}

/**
 * @brief Sets the secU3ac1TxTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu3ac1txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_3ac_1_tx_trigger_clearf(uint8_t secu3ac1txtrigger)
{
    ASSERT_ERR((((uint32_t)secu3ac1txtrigger << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu3ac1txtrigger << 17);
}

/**
 * @brief Sets the secU3ac0TxTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu3ac0txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_3ac_0_tx_trigger_clearf(uint8_t secu3ac0txtrigger)
{
    ASSERT_ERR((((uint32_t)secu3ac0txtrigger << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu3ac0txtrigger << 16);
}

/**
 * @brief Sets the secU2ac3TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu2ac3txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_2ac_3_tx_buf_trigger_clearf(uint8_t secu2ac3txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu2ac3txbuftrigger << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu2ac3txbuftrigger << 15);
}

/**
 * @brief Sets the secU2ac2TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu2ac2txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_2ac_2_tx_buf_trigger_clearf(uint8_t secu2ac2txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu2ac2txbuftrigger << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu2ac2txbuftrigger << 14);
}

/**
 * @brief Sets the secU2ac1TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu2ac1txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_2ac_1_tx_buf_trigger_clearf(uint8_t secu2ac1txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu2ac1txbuftrigger << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu2ac1txbuftrigger << 13);
}

/**
 * @brief Sets the secU2ac0TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu2ac0txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_2ac_0_tx_buf_trigger_clearf(uint8_t secu2ac0txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu2ac0txbuftrigger << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu2ac0txbuftrigger << 12);
}

/**
 * @brief Sets the secU2ac3TxTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu2ac3txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_2ac_3_tx_trigger_clearf(uint8_t secu2ac3txtrigger)
{
    ASSERT_ERR((((uint32_t)secu2ac3txtrigger << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu2ac3txtrigger << 11);
}

/**
 * @brief Sets the secU2ac2TxTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu2ac2txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_2ac_2_tx_trigger_clearf(uint8_t secu2ac2txtrigger)
{
    ASSERT_ERR((((uint32_t)secu2ac2txtrigger << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu2ac2txtrigger << 10);
}

/**
 * @brief Sets the secU2ac1TxTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu2ac1txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_2ac_1_tx_trigger_clearf(uint8_t secu2ac1txtrigger)
{
    ASSERT_ERR((((uint32_t)secu2ac1txtrigger << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu2ac1txtrigger << 9);
}

/**
 * @brief Sets the secU2ac0TxTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu2ac0txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_2ac_0_tx_trigger_clearf(uint8_t secu2ac0txtrigger)
{
    ASSERT_ERR((((uint32_t)secu2ac0txtrigger << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu2ac0txtrigger << 8);
}

/**
 * @brief Sets the secU1ac3TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu1ac3txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_1ac_3_tx_buf_trigger_clearf(uint8_t secu1ac3txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu1ac3txbuftrigger << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu1ac3txbuftrigger << 7);
}

/**
 * @brief Sets the secU1ac2TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu1ac2txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_1ac_2_tx_buf_trigger_clearf(uint8_t secu1ac2txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu1ac2txbuftrigger << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu1ac2txbuftrigger << 6);
}

/**
 * @brief Sets the secU1ac1TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu1ac1txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_1ac_1_tx_buf_trigger_clearf(uint8_t secu1ac1txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu1ac1txbuftrigger << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu1ac1txbuftrigger << 5);
}

/**
 * @brief Sets the secU1ac0TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu1ac0txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_1ac_0_tx_buf_trigger_clearf(uint8_t secu1ac0txbuftrigger)
{
    ASSERT_ERR((((uint32_t)secu1ac0txbuftrigger << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu1ac0txbuftrigger << 4);
}

/**
 * @brief Sets the secU1ac3TxTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu1ac3txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_1ac_3_tx_trigger_clearf(uint8_t secu1ac3txtrigger)
{
    ASSERT_ERR((((uint32_t)secu1ac3txtrigger << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu1ac3txtrigger << 3);
}

/**
 * @brief Sets the secU1ac2TxTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu1ac2txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_1ac_2_tx_trigger_clearf(uint8_t secu1ac2txtrigger)
{
    ASSERT_ERR((((uint32_t)secu1ac2txtrigger << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu1ac2txtrigger << 2);
}

/**
 * @brief Sets the secU1ac1TxTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu1ac1txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_1ac_1_tx_trigger_clearf(uint8_t secu1ac1txtrigger)
{
    ASSERT_ERR((((uint32_t)secu1ac1txtrigger << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu1ac1txtrigger << 1);
}

/**
 * @brief Sets the secU1ac0TxTrigger field of the SEC_USERS_TX_INT_EVENT_CLEAR register.
 *
 * The SEC_USERS_TX_INT_EVENT_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] secu1ac0txtrigger - The value to set the field to.
 */
__INLINE void nxmac_sec_u_1ac_0_tx_trigger_clearf(uint8_t secu1ac0txtrigger)
{
    ASSERT_ERR((((uint32_t)secu1ac0txtrigger << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_CLEAR_ADDR, (uint32_t)secu1ac0txtrigger << 0);
}

#endif // RW_MUMIMO_TX_EN
/// @}

#if RW_MUMIMO_TX_EN
/**
 * @name SEC_USERS_TX_INT_EVENT_UN_MASK register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31   masterSecUsersTxIntEn   0
 *     23   masksecU3ac3TxBufTrigger   0
 *     22   masksecU3ac2TxBufTrigger   0
 *     21   masksecU3ac1TxBufTrigger   0
 *     20   masksecU3ac0TxBufTrigger   0
 *     19   masksecU3ac3TxTrigger   0
 *     18   masksecU3ac2TxTrigger   0
 *     17   masksecU3ac1TxTrigger   0
 *     16   masksecU3ac0TxTrigger   0
 *     15   masksecU2ac3TxBufTrigger   0
 *     14   masksecU2ac2TxBufTrigger   0
 *     13   masksecU2ac1TxBufTrigger   0
 *     12   masksecU2ac0TxBufTrigger   0
 *     11   masksecU2ac3TxTrigger   0
 *     10   masksecU2ac2TxTrigger   0
 *     09   masksecU2ac1TxTrigger   0
 *     08   masksecU2ac0TxTrigger   0
 *     07   masksecU1ac3TxBufTrigger   0
 *     06   masksecU1ac2TxBufTrigger   0
 *     05   masksecU1ac1TxBufTrigger   0
 *     04   masksecU1ac0TxBufTrigger   0
 *     03   masksecU1ac3TxTrigger   0
 *     02   masksecU1ac2TxTrigger   0
 *     01   masksecU1ac1TxTrigger   0
 *     00   masksecU1ac0TxTrigger   0
 * </pre>
 *
 * @{
 */

/// Address of the SEC_USERS_TX_INT_EVENT_UN_MASK register
#define NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR   0xC000836C
/// Offset of the SEC_USERS_TX_INT_EVENT_UN_MASK register from the base address
#define NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_OFFSET 0x0000036C
/// Index of the SEC_USERS_TX_INT_EVENT_UN_MASK register
#define NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_INDEX  0x000000DB
/// Reset value of the SEC_USERS_TX_INT_EVENT_UN_MASK register
#define NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_RESET  0x00000000

/**
 * @brief Returns the current value of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and its value returned.
 * @return The current value of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint32_t nxmac_sec_users_tx_int_event_un_mask_get(void)
{
    return REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
}

/**
 * @brief Sets the SEC_USERS_TX_INT_EVENT_UN_MASK register to a value.
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_sec_users_tx_int_event_un_mask_set(uint32_t value)
{
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, value);
}

// field definitions
/// MASTER_SEC_USERS_TX_INT_EN field bit
#define NXMAC_MASTER_SEC_USERS_TX_INT_EN_BIT        ((uint32_t)0x80000000)
/// MASTER_SEC_USERS_TX_INT_EN field position
#define NXMAC_MASTER_SEC_USERS_TX_INT_EN_POS        31
/// MASKSEC_U_3AC_3_TX_BUF_TRIGGER field bit
#define NXMAC_MASKSEC_U_3AC_3_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00800000)
/// MASKSEC_U_3AC_3_TX_BUF_TRIGGER field position
#define NXMAC_MASKSEC_U_3AC_3_TX_BUF_TRIGGER_POS    23
/// MASKSEC_U_3AC_2_TX_BUF_TRIGGER field bit
#define NXMAC_MASKSEC_U_3AC_2_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00400000)
/// MASKSEC_U_3AC_2_TX_BUF_TRIGGER field position
#define NXMAC_MASKSEC_U_3AC_2_TX_BUF_TRIGGER_POS    22
/// MASKSEC_U_3AC_1_TX_BUF_TRIGGER field bit
#define NXMAC_MASKSEC_U_3AC_1_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00200000)
/// MASKSEC_U_3AC_1_TX_BUF_TRIGGER field position
#define NXMAC_MASKSEC_U_3AC_1_TX_BUF_TRIGGER_POS    21
/// MASKSEC_U_3AC_0_TX_BUF_TRIGGER field bit
#define NXMAC_MASKSEC_U_3AC_0_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00100000)
/// MASKSEC_U_3AC_0_TX_BUF_TRIGGER field position
#define NXMAC_MASKSEC_U_3AC_0_TX_BUF_TRIGGER_POS    20
/// MASKSEC_U_3AC_3_TX_TRIGGER field bit
#define NXMAC_MASKSEC_U_3AC_3_TX_TRIGGER_BIT        ((uint32_t)0x00080000)
/// MASKSEC_U_3AC_3_TX_TRIGGER field position
#define NXMAC_MASKSEC_U_3AC_3_TX_TRIGGER_POS        19
/// MASKSEC_U_3AC_2_TX_TRIGGER field bit
#define NXMAC_MASKSEC_U_3AC_2_TX_TRIGGER_BIT        ((uint32_t)0x00040000)
/// MASKSEC_U_3AC_2_TX_TRIGGER field position
#define NXMAC_MASKSEC_U_3AC_2_TX_TRIGGER_POS        18
/// MASKSEC_U_3AC_1_TX_TRIGGER field bit
#define NXMAC_MASKSEC_U_3AC_1_TX_TRIGGER_BIT        ((uint32_t)0x00020000)
/// MASKSEC_U_3AC_1_TX_TRIGGER field position
#define NXMAC_MASKSEC_U_3AC_1_TX_TRIGGER_POS        17
/// MASKSEC_U_3AC_0_TX_TRIGGER field bit
#define NXMAC_MASKSEC_U_3AC_0_TX_TRIGGER_BIT        ((uint32_t)0x00010000)
/// MASKSEC_U_3AC_0_TX_TRIGGER field position
#define NXMAC_MASKSEC_U_3AC_0_TX_TRIGGER_POS        16
/// MASKSEC_U_2AC_3_TX_BUF_TRIGGER field bit
#define NXMAC_MASKSEC_U_2AC_3_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00008000)
/// MASKSEC_U_2AC_3_TX_BUF_TRIGGER field position
#define NXMAC_MASKSEC_U_2AC_3_TX_BUF_TRIGGER_POS    15
/// MASKSEC_U_2AC_2_TX_BUF_TRIGGER field bit
#define NXMAC_MASKSEC_U_2AC_2_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00004000)
/// MASKSEC_U_2AC_2_TX_BUF_TRIGGER field position
#define NXMAC_MASKSEC_U_2AC_2_TX_BUF_TRIGGER_POS    14
/// MASKSEC_U_2AC_1_TX_BUF_TRIGGER field bit
#define NXMAC_MASKSEC_U_2AC_1_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00002000)
/// MASKSEC_U_2AC_1_TX_BUF_TRIGGER field position
#define NXMAC_MASKSEC_U_2AC_1_TX_BUF_TRIGGER_POS    13
/// MASKSEC_U_2AC_0_TX_BUF_TRIGGER field bit
#define NXMAC_MASKSEC_U_2AC_0_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00001000)
/// MASKSEC_U_2AC_0_TX_BUF_TRIGGER field position
#define NXMAC_MASKSEC_U_2AC_0_TX_BUF_TRIGGER_POS    12
/// MASKSEC_U_2AC_3_TX_TRIGGER field bit
#define NXMAC_MASKSEC_U_2AC_3_TX_TRIGGER_BIT        ((uint32_t)0x00000800)
/// MASKSEC_U_2AC_3_TX_TRIGGER field position
#define NXMAC_MASKSEC_U_2AC_3_TX_TRIGGER_POS        11
/// MASKSEC_U_2AC_2_TX_TRIGGER field bit
#define NXMAC_MASKSEC_U_2AC_2_TX_TRIGGER_BIT        ((uint32_t)0x00000400)
/// MASKSEC_U_2AC_2_TX_TRIGGER field position
#define NXMAC_MASKSEC_U_2AC_2_TX_TRIGGER_POS        10
/// MASKSEC_U_2AC_1_TX_TRIGGER field bit
#define NXMAC_MASKSEC_U_2AC_1_TX_TRIGGER_BIT        ((uint32_t)0x00000200)
/// MASKSEC_U_2AC_1_TX_TRIGGER field position
#define NXMAC_MASKSEC_U_2AC_1_TX_TRIGGER_POS        9
/// MASKSEC_U_2AC_0_TX_TRIGGER field bit
#define NXMAC_MASKSEC_U_2AC_0_TX_TRIGGER_BIT        ((uint32_t)0x00000100)
/// MASKSEC_U_2AC_0_TX_TRIGGER field position
#define NXMAC_MASKSEC_U_2AC_0_TX_TRIGGER_POS        8
/// MASKSEC_U_1AC_3_TX_BUF_TRIGGER field bit
#define NXMAC_MASKSEC_U_1AC_3_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00000080)
/// MASKSEC_U_1AC_3_TX_BUF_TRIGGER field position
#define NXMAC_MASKSEC_U_1AC_3_TX_BUF_TRIGGER_POS    7
/// MASKSEC_U_1AC_2_TX_BUF_TRIGGER field bit
#define NXMAC_MASKSEC_U_1AC_2_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00000040)
/// MASKSEC_U_1AC_2_TX_BUF_TRIGGER field position
#define NXMAC_MASKSEC_U_1AC_2_TX_BUF_TRIGGER_POS    6
/// MASKSEC_U_1AC_1_TX_BUF_TRIGGER field bit
#define NXMAC_MASKSEC_U_1AC_1_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00000020)
/// MASKSEC_U_1AC_1_TX_BUF_TRIGGER field position
#define NXMAC_MASKSEC_U_1AC_1_TX_BUF_TRIGGER_POS    5
/// MASKSEC_U_1AC_0_TX_BUF_TRIGGER field bit
#define NXMAC_MASKSEC_U_1AC_0_TX_BUF_TRIGGER_BIT    ((uint32_t)0x00000010)
/// MASKSEC_U_1AC_0_TX_BUF_TRIGGER field position
#define NXMAC_MASKSEC_U_1AC_0_TX_BUF_TRIGGER_POS    4
/// MASKSEC_U_1AC_3_TX_TRIGGER field bit
#define NXMAC_MASKSEC_U_1AC_3_TX_TRIGGER_BIT        ((uint32_t)0x00000008)
/// MASKSEC_U_1AC_3_TX_TRIGGER field position
#define NXMAC_MASKSEC_U_1AC_3_TX_TRIGGER_POS        3
/// MASKSEC_U_1AC_2_TX_TRIGGER field bit
#define NXMAC_MASKSEC_U_1AC_2_TX_TRIGGER_BIT        ((uint32_t)0x00000004)
/// MASKSEC_U_1AC_2_TX_TRIGGER field position
#define NXMAC_MASKSEC_U_1AC_2_TX_TRIGGER_POS        2
/// MASKSEC_U_1AC_1_TX_TRIGGER field bit
#define NXMAC_MASKSEC_U_1AC_1_TX_TRIGGER_BIT        ((uint32_t)0x00000002)
/// MASKSEC_U_1AC_1_TX_TRIGGER field position
#define NXMAC_MASKSEC_U_1AC_1_TX_TRIGGER_POS        1
/// MASKSEC_U_1AC_0_TX_TRIGGER field bit
#define NXMAC_MASKSEC_U_1AC_0_TX_TRIGGER_BIT        ((uint32_t)0x00000001)
/// MASKSEC_U_1AC_0_TX_TRIGGER field position
#define NXMAC_MASKSEC_U_1AC_0_TX_TRIGGER_POS        0

/// MASTER_SEC_USERS_TX_INT_EN field reset value
#define NXMAC_MASTER_SEC_USERS_TX_INT_EN_RST        0x0
/// MASKSEC_U_3AC_3_TX_BUF_TRIGGER field reset value
#define NXMAC_MASKSEC_U_3AC_3_TX_BUF_TRIGGER_RST    0x0
/// MASKSEC_U_3AC_2_TX_BUF_TRIGGER field reset value
#define NXMAC_MASKSEC_U_3AC_2_TX_BUF_TRIGGER_RST    0x0
/// MASKSEC_U_3AC_1_TX_BUF_TRIGGER field reset value
#define NXMAC_MASKSEC_U_3AC_1_TX_BUF_TRIGGER_RST    0x0
/// MASKSEC_U_3AC_0_TX_BUF_TRIGGER field reset value
#define NXMAC_MASKSEC_U_3AC_0_TX_BUF_TRIGGER_RST    0x0
/// MASKSEC_U_3AC_3_TX_TRIGGER field reset value
#define NXMAC_MASKSEC_U_3AC_3_TX_TRIGGER_RST        0x0
/// MASKSEC_U_3AC_2_TX_TRIGGER field reset value
#define NXMAC_MASKSEC_U_3AC_2_TX_TRIGGER_RST        0x0
/// MASKSEC_U_3AC_1_TX_TRIGGER field reset value
#define NXMAC_MASKSEC_U_3AC_1_TX_TRIGGER_RST        0x0
/// MASKSEC_U_3AC_0_TX_TRIGGER field reset value
#define NXMAC_MASKSEC_U_3AC_0_TX_TRIGGER_RST        0x0
/// MASKSEC_U_2AC_3_TX_BUF_TRIGGER field reset value
#define NXMAC_MASKSEC_U_2AC_3_TX_BUF_TRIGGER_RST    0x0
/// MASKSEC_U_2AC_2_TX_BUF_TRIGGER field reset value
#define NXMAC_MASKSEC_U_2AC_2_TX_BUF_TRIGGER_RST    0x0
/// MASKSEC_U_2AC_1_TX_BUF_TRIGGER field reset value
#define NXMAC_MASKSEC_U_2AC_1_TX_BUF_TRIGGER_RST    0x0
/// MASKSEC_U_2AC_0_TX_BUF_TRIGGER field reset value
#define NXMAC_MASKSEC_U_2AC_0_TX_BUF_TRIGGER_RST    0x0
/// MASKSEC_U_2AC_3_TX_TRIGGER field reset value
#define NXMAC_MASKSEC_U_2AC_3_TX_TRIGGER_RST        0x0
/// MASKSEC_U_2AC_2_TX_TRIGGER field reset value
#define NXMAC_MASKSEC_U_2AC_2_TX_TRIGGER_RST        0x0
/// MASKSEC_U_2AC_1_TX_TRIGGER field reset value
#define NXMAC_MASKSEC_U_2AC_1_TX_TRIGGER_RST        0x0
/// MASKSEC_U_2AC_0_TX_TRIGGER field reset value
#define NXMAC_MASKSEC_U_2AC_0_TX_TRIGGER_RST        0x0
/// MASKSEC_U_1AC_3_TX_BUF_TRIGGER field reset value
#define NXMAC_MASKSEC_U_1AC_3_TX_BUF_TRIGGER_RST    0x0
/// MASKSEC_U_1AC_2_TX_BUF_TRIGGER field reset value
#define NXMAC_MASKSEC_U_1AC_2_TX_BUF_TRIGGER_RST    0x0
/// MASKSEC_U_1AC_1_TX_BUF_TRIGGER field reset value
#define NXMAC_MASKSEC_U_1AC_1_TX_BUF_TRIGGER_RST    0x0
/// MASKSEC_U_1AC_0_TX_BUF_TRIGGER field reset value
#define NXMAC_MASKSEC_U_1AC_0_TX_BUF_TRIGGER_RST    0x0
/// MASKSEC_U_1AC_3_TX_TRIGGER field reset value
#define NXMAC_MASKSEC_U_1AC_3_TX_TRIGGER_RST        0x0
/// MASKSEC_U_1AC_2_TX_TRIGGER field reset value
#define NXMAC_MASKSEC_U_1AC_2_TX_TRIGGER_RST        0x0
/// MASKSEC_U_1AC_1_TX_TRIGGER field reset value
#define NXMAC_MASKSEC_U_1AC_1_TX_TRIGGER_RST        0x0
/// MASKSEC_U_1AC_0_TX_TRIGGER field reset value
#define NXMAC_MASKSEC_U_1AC_0_TX_TRIGGER_RST        0x0

/**
 * @brief Constructs a value for the SEC_USERS_TX_INT_EVENT_UN_MASK register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] mastersecuserstxinten - The value to use for the masterSecUsersTxIntEn field.
 * @param[in] masksecu3ac3txbuftrigger - The value to use for the masksecU3ac3TxBufTrigger field.
 * @param[in] masksecu3ac2txbuftrigger - The value to use for the masksecU3ac2TxBufTrigger field.
 * @param[in] masksecu3ac1txbuftrigger - The value to use for the masksecU3ac1TxBufTrigger field.
 * @param[in] masksecu3ac0txbuftrigger - The value to use for the masksecU3ac0TxBufTrigger field.
 * @param[in] masksecu3ac3txtrigger - The value to use for the masksecU3ac3TxTrigger field.
 * @param[in] masksecu3ac2txtrigger - The value to use for the masksecU3ac2TxTrigger field.
 * @param[in] masksecu3ac1txtrigger - The value to use for the masksecU3ac1TxTrigger field.
 * @param[in] masksecu3ac0txtrigger - The value to use for the masksecU3ac0TxTrigger field.
 * @param[in] masksecu2ac3txbuftrigger - The value to use for the masksecU2ac3TxBufTrigger field.
 * @param[in] masksecu2ac2txbuftrigger - The value to use for the masksecU2ac2TxBufTrigger field.
 * @param[in] masksecu2ac1txbuftrigger - The value to use for the masksecU2ac1TxBufTrigger field.
 * @param[in] masksecu2ac0txbuftrigger - The value to use for the masksecU2ac0TxBufTrigger field.
 * @param[in] masksecu2ac3txtrigger - The value to use for the masksecU2ac3TxTrigger field.
 * @param[in] masksecu2ac2txtrigger - The value to use for the masksecU2ac2TxTrigger field.
 * @param[in] masksecu2ac1txtrigger - The value to use for the masksecU2ac1TxTrigger field.
 * @param[in] masksecu2ac0txtrigger - The value to use for the masksecU2ac0TxTrigger field.
 * @param[in] masksecu1ac3txbuftrigger - The value to use for the masksecU1ac3TxBufTrigger field.
 * @param[in] masksecu1ac2txbuftrigger - The value to use for the masksecU1ac2TxBufTrigger field.
 * @param[in] masksecu1ac1txbuftrigger - The value to use for the masksecU1ac1TxBufTrigger field.
 * @param[in] masksecu1ac0txbuftrigger - The value to use for the masksecU1ac0TxBufTrigger field.
 * @param[in] masksecu1ac3txtrigger - The value to use for the masksecU1ac3TxTrigger field.
 * @param[in] masksecu1ac2txtrigger - The value to use for the masksecU1ac2TxTrigger field.
 * @param[in] masksecu1ac1txtrigger - The value to use for the masksecU1ac1TxTrigger field.
 * @param[in] masksecu1ac0txtrigger - The value to use for the masksecU1ac0TxTrigger field.
 */
__INLINE void nxmac_sec_users_tx_int_event_un_mask_pack(uint8_t mastersecuserstxinten, uint8_t masksecu3ac3txbuftrigger, uint8_t masksecu3ac2txbuftrigger, uint8_t masksecu3ac1txbuftrigger, uint8_t masksecu3ac0txbuftrigger, uint8_t masksecu3ac3txtrigger, uint8_t masksecu3ac2txtrigger, uint8_t masksecu3ac1txtrigger, uint8_t masksecu3ac0txtrigger, uint8_t masksecu2ac3txbuftrigger, uint8_t masksecu2ac2txbuftrigger, uint8_t masksecu2ac1txbuftrigger, uint8_t masksecu2ac0txbuftrigger, uint8_t masksecu2ac3txtrigger, uint8_t masksecu2ac2txtrigger, uint8_t masksecu2ac1txtrigger, uint8_t masksecu2ac0txtrigger, uint8_t masksecu1ac3txbuftrigger, uint8_t masksecu1ac2txbuftrigger, uint8_t masksecu1ac1txbuftrigger, uint8_t masksecu1ac0txbuftrigger, uint8_t masksecu1ac3txtrigger, uint8_t masksecu1ac2txtrigger, uint8_t masksecu1ac1txtrigger, uint8_t masksecu1ac0txtrigger)
{
    ASSERT_ERR((((uint32_t)mastersecuserstxinten << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)masksecu3ac3txbuftrigger << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)masksecu3ac2txbuftrigger << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)masksecu3ac1txbuftrigger << 21) & ~((uint32_t)0x00200000)) == 0);
    ASSERT_ERR((((uint32_t)masksecu3ac0txbuftrigger << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)masksecu3ac3txtrigger << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)masksecu3ac2txtrigger << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)masksecu3ac1txtrigger << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)masksecu3ac0txtrigger << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)masksecu2ac3txbuftrigger << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)masksecu2ac2txbuftrigger << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)masksecu2ac1txbuftrigger << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)masksecu2ac0txbuftrigger << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)masksecu2ac3txtrigger << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)masksecu2ac2txtrigger << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)masksecu2ac1txtrigger << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)masksecu2ac0txtrigger << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)masksecu1ac3txbuftrigger << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)masksecu1ac2txbuftrigger << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)masksecu1ac1txbuftrigger << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)masksecu1ac0txbuftrigger << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)masksecu1ac3txtrigger << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)masksecu1ac2txtrigger << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)masksecu1ac1txtrigger << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)masksecu1ac0txtrigger << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR,  ((uint32_t)mastersecuserstxinten << 31) | ((uint32_t)masksecu3ac3txbuftrigger << 23) | ((uint32_t)masksecu3ac2txbuftrigger << 22) | ((uint32_t)masksecu3ac1txbuftrigger << 21) | ((uint32_t)masksecu3ac0txbuftrigger << 20) | ((uint32_t)masksecu3ac3txtrigger << 19) | ((uint32_t)masksecu3ac2txtrigger << 18) | ((uint32_t)masksecu3ac1txtrigger << 17) | ((uint32_t)masksecu3ac0txtrigger << 16) | ((uint32_t)masksecu2ac3txbuftrigger << 15) | ((uint32_t)masksecu2ac2txbuftrigger << 14) | ((uint32_t)masksecu2ac1txbuftrigger << 13) | ((uint32_t)masksecu2ac0txbuftrigger << 12) | ((uint32_t)masksecu2ac3txtrigger << 11) | ((uint32_t)masksecu2ac2txtrigger << 10) | ((uint32_t)masksecu2ac1txtrigger << 9) | ((uint32_t)masksecu2ac0txtrigger << 8) | ((uint32_t)masksecu1ac3txbuftrigger << 7) | ((uint32_t)masksecu1ac2txbuftrigger << 6) | ((uint32_t)masksecu1ac1txbuftrigger << 5) | ((uint32_t)masksecu1ac0txbuftrigger << 4) | ((uint32_t)masksecu1ac3txtrigger << 3) | ((uint32_t)masksecu1ac2txtrigger << 2) | ((uint32_t)masksecu1ac1txtrigger << 1) | ((uint32_t)masksecu1ac0txtrigger << 0));
}

/**
 * @brief Unpacks SEC_USERS_TX_INT_EVENT_UN_MASK's fields from current value of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * Reads the SEC_USERS_TX_INT_EVENT_UN_MASK register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] mastersecuserstxinten - Will be populated with the current value of this field from the register.
 * @param[out] masksecu3ac3txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu3ac2txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu3ac1txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu3ac0txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu3ac3txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu3ac2txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu3ac1txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu3ac0txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu2ac3txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu2ac2txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu2ac1txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu2ac0txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu2ac3txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu2ac2txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu2ac1txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu2ac0txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu1ac3txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu1ac2txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu1ac1txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu1ac0txbuftrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu1ac3txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu1ac2txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu1ac1txtrigger - Will be populated with the current value of this field from the register.
 * @param[out] masksecu1ac0txtrigger - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_sec_users_tx_int_event_un_mask_unpack(uint8_t *mastersecuserstxinten, uint8_t *masksecu3ac3txbuftrigger, uint8_t *masksecu3ac2txbuftrigger, uint8_t *masksecu3ac1txbuftrigger, uint8_t *masksecu3ac0txbuftrigger, uint8_t *masksecu3ac3txtrigger, uint8_t *masksecu3ac2txtrigger, uint8_t *masksecu3ac1txtrigger, uint8_t *masksecu3ac0txtrigger, uint8_t *masksecu2ac3txbuftrigger, uint8_t *masksecu2ac2txbuftrigger, uint8_t *masksecu2ac1txbuftrigger, uint8_t *masksecu2ac0txbuftrigger, uint8_t *masksecu2ac3txtrigger, uint8_t *masksecu2ac2txtrigger, uint8_t *masksecu2ac1txtrigger, uint8_t *masksecu2ac0txtrigger, uint8_t *masksecu1ac3txbuftrigger, uint8_t *masksecu1ac2txbuftrigger, uint8_t *masksecu1ac1txbuftrigger, uint8_t *masksecu1ac0txbuftrigger, uint8_t *masksecu1ac3txtrigger, uint8_t *masksecu1ac2txtrigger, uint8_t *masksecu1ac1txtrigger, uint8_t *masksecu1ac0txtrigger)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);

    *mastersecuserstxinten = (localVal & ((uint32_t)0x80000000)) >> 31;
    *masksecu3ac3txbuftrigger = (localVal & ((uint32_t)0x00800000)) >> 23;
    *masksecu3ac2txbuftrigger = (localVal & ((uint32_t)0x00400000)) >> 22;
    *masksecu3ac1txbuftrigger = (localVal & ((uint32_t)0x00200000)) >> 21;
    *masksecu3ac0txbuftrigger = (localVal & ((uint32_t)0x00100000)) >> 20;
    *masksecu3ac3txtrigger = (localVal & ((uint32_t)0x00080000)) >> 19;
    *masksecu3ac2txtrigger = (localVal & ((uint32_t)0x00040000)) >> 18;
    *masksecu3ac1txtrigger = (localVal & ((uint32_t)0x00020000)) >> 17;
    *masksecu3ac0txtrigger = (localVal & ((uint32_t)0x00010000)) >> 16;
    *masksecu2ac3txbuftrigger = (localVal & ((uint32_t)0x00008000)) >> 15;
    *masksecu2ac2txbuftrigger = (localVal & ((uint32_t)0x00004000)) >> 14;
    *masksecu2ac1txbuftrigger = (localVal & ((uint32_t)0x00002000)) >> 13;
    *masksecu2ac0txbuftrigger = (localVal & ((uint32_t)0x00001000)) >> 12;
    *masksecu2ac3txtrigger = (localVal & ((uint32_t)0x00000800)) >> 11;
    *masksecu2ac2txtrigger = (localVal & ((uint32_t)0x00000400)) >> 10;
    *masksecu2ac1txtrigger = (localVal & ((uint32_t)0x00000200)) >> 9;
    *masksecu2ac0txtrigger = (localVal & ((uint32_t)0x00000100)) >> 8;
    *masksecu1ac3txbuftrigger = (localVal & ((uint32_t)0x00000080)) >> 7;
    *masksecu1ac2txbuftrigger = (localVal & ((uint32_t)0x00000040)) >> 6;
    *masksecu1ac1txbuftrigger = (localVal & ((uint32_t)0x00000020)) >> 5;
    *masksecu1ac0txbuftrigger = (localVal & ((uint32_t)0x00000010)) >> 4;
    *masksecu1ac3txtrigger = (localVal & ((uint32_t)0x00000008)) >> 3;
    *masksecu1ac2txtrigger = (localVal & ((uint32_t)0x00000004)) >> 2;
    *masksecu1ac1txtrigger = (localVal & ((uint32_t)0x00000002)) >> 1;
    *masksecu1ac0txtrigger = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the masterSecUsersTxIntEn field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masterSecUsersTxIntEn field's value will be returned.
 *
 * @return The current value of the masterSecUsersTxIntEn field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_master_sec_users_tx_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the masterSecUsersTxIntEn field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mastersecuserstxinten - The value to set the field to.
 */
__INLINE void nxmac_master_sec_users_tx_int_en_setf(uint8_t mastersecuserstxinten)
{
    ASSERT_ERR((((uint32_t)mastersecuserstxinten << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)mastersecuserstxinten << 31));
}

/**
 * @brief Returns the current value of the masksecU3ac3TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU3ac3TxBufTrigger field's value will be returned.
 *
 * @return The current value of the masksecU3ac3TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_3ac_3_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Sets the masksecU3ac3TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu3ac3txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_3ac_3_tx_buf_trigger_setf(uint8_t masksecu3ac3txbuftrigger)
{
    ASSERT_ERR((((uint32_t)masksecu3ac3txbuftrigger << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00800000)) | ((uint32_t)masksecu3ac3txbuftrigger << 23));
}

/**
 * @brief Returns the current value of the masksecU3ac2TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU3ac2TxBufTrigger field's value will be returned.
 *
 * @return The current value of the masksecU3ac2TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_3ac_2_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Sets the masksecU3ac2TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu3ac2txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_3ac_2_tx_buf_trigger_setf(uint8_t masksecu3ac2txbuftrigger)
{
    ASSERT_ERR((((uint32_t)masksecu3ac2txbuftrigger << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00400000)) | ((uint32_t)masksecu3ac2txbuftrigger << 22));
}

/**
 * @brief Returns the current value of the masksecU3ac1TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU3ac1TxBufTrigger field's value will be returned.
 *
 * @return The current value of the masksecU3ac1TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_3ac_1_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Sets the masksecU3ac1TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu3ac1txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_3ac_1_tx_buf_trigger_setf(uint8_t masksecu3ac1txbuftrigger)
{
    ASSERT_ERR((((uint32_t)masksecu3ac1txbuftrigger << 21) & ~((uint32_t)0x00200000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00200000)) | ((uint32_t)masksecu3ac1txbuftrigger << 21));
}

/**
 * @brief Returns the current value of the masksecU3ac0TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU3ac0TxBufTrigger field's value will be returned.
 *
 * @return The current value of the masksecU3ac0TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_3ac_0_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Sets the masksecU3ac0TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu3ac0txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_3ac_0_tx_buf_trigger_setf(uint8_t masksecu3ac0txbuftrigger)
{
    ASSERT_ERR((((uint32_t)masksecu3ac0txbuftrigger << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00100000)) | ((uint32_t)masksecu3ac0txbuftrigger << 20));
}

/**
 * @brief Returns the current value of the masksecU3ac3TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU3ac3TxTrigger field's value will be returned.
 *
 * @return The current value of the masksecU3ac3TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_3ac_3_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Sets the masksecU3ac3TxTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu3ac3txtrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_3ac_3_tx_trigger_setf(uint8_t masksecu3ac3txtrigger)
{
    ASSERT_ERR((((uint32_t)masksecu3ac3txtrigger << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00080000)) | ((uint32_t)masksecu3ac3txtrigger << 19));
}

/**
 * @brief Returns the current value of the masksecU3ac2TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU3ac2TxTrigger field's value will be returned.
 *
 * @return The current value of the masksecU3ac2TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_3ac_2_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Sets the masksecU3ac2TxTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu3ac2txtrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_3ac_2_tx_trigger_setf(uint8_t masksecu3ac2txtrigger)
{
    ASSERT_ERR((((uint32_t)masksecu3ac2txtrigger << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00040000)) | ((uint32_t)masksecu3ac2txtrigger << 18));
}

/**
 * @brief Returns the current value of the masksecU3ac1TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU3ac1TxTrigger field's value will be returned.
 *
 * @return The current value of the masksecU3ac1TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_3ac_1_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Sets the masksecU3ac1TxTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu3ac1txtrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_3ac_1_tx_trigger_setf(uint8_t masksecu3ac1txtrigger)
{
    ASSERT_ERR((((uint32_t)masksecu3ac1txtrigger << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00020000)) | ((uint32_t)masksecu3ac1txtrigger << 17));
}

/**
 * @brief Returns the current value of the masksecU3ac0TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU3ac0TxTrigger field's value will be returned.
 *
 * @return The current value of the masksecU3ac0TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_3ac_0_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the masksecU3ac0TxTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu3ac0txtrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_3ac_0_tx_trigger_setf(uint8_t masksecu3ac0txtrigger)
{
    ASSERT_ERR((((uint32_t)masksecu3ac0txtrigger << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)masksecu3ac0txtrigger << 16));
}

/**
 * @brief Returns the current value of the masksecU2ac3TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU2ac3TxBufTrigger field's value will be returned.
 *
 * @return The current value of the masksecU2ac3TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_2ac_3_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

/**
 * @brief Sets the masksecU2ac3TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu2ac3txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_2ac_3_tx_buf_trigger_setf(uint8_t masksecu2ac3txbuftrigger)
{
    ASSERT_ERR((((uint32_t)masksecu2ac3txbuftrigger << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00008000)) | ((uint32_t)masksecu2ac3txbuftrigger << 15));
}

/**
 * @brief Returns the current value of the masksecU2ac2TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU2ac2TxBufTrigger field's value will be returned.
 *
 * @return The current value of the masksecU2ac2TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_2ac_2_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Sets the masksecU2ac2TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu2ac2txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_2ac_2_tx_buf_trigger_setf(uint8_t masksecu2ac2txbuftrigger)
{
    ASSERT_ERR((((uint32_t)masksecu2ac2txbuftrigger << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00004000)) | ((uint32_t)masksecu2ac2txbuftrigger << 14));
}

/**
 * @brief Returns the current value of the masksecU2ac1TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU2ac1TxBufTrigger field's value will be returned.
 *
 * @return The current value of the masksecU2ac1TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_2ac_1_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the masksecU2ac1TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu2ac1txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_2ac_1_tx_buf_trigger_setf(uint8_t masksecu2ac1txbuftrigger)
{
    ASSERT_ERR((((uint32_t)masksecu2ac1txbuftrigger << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)masksecu2ac1txbuftrigger << 13));
}

/**
 * @brief Returns the current value of the masksecU2ac0TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU2ac0TxBufTrigger field's value will be returned.
 *
 * @return The current value of the masksecU2ac0TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_2ac_0_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the masksecU2ac0TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu2ac0txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_2ac_0_tx_buf_trigger_setf(uint8_t masksecu2ac0txbuftrigger)
{
    ASSERT_ERR((((uint32_t)masksecu2ac0txbuftrigger << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)masksecu2ac0txbuftrigger << 12));
}

/**
 * @brief Returns the current value of the masksecU2ac3TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU2ac3TxTrigger field's value will be returned.
 *
 * @return The current value of the masksecU2ac3TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_2ac_3_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the masksecU2ac3TxTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu2ac3txtrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_2ac_3_tx_trigger_setf(uint8_t masksecu2ac3txtrigger)
{
    ASSERT_ERR((((uint32_t)masksecu2ac3txtrigger << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00000800)) | ((uint32_t)masksecu2ac3txtrigger << 11));
}

/**
 * @brief Returns the current value of the masksecU2ac2TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU2ac2TxTrigger field's value will be returned.
 *
 * @return The current value of the masksecU2ac2TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_2ac_2_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the masksecU2ac2TxTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu2ac2txtrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_2ac_2_tx_trigger_setf(uint8_t masksecu2ac2txtrigger)
{
    ASSERT_ERR((((uint32_t)masksecu2ac2txtrigger << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00000400)) | ((uint32_t)masksecu2ac2txtrigger << 10));
}

/**
 * @brief Returns the current value of the masksecU2ac1TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU2ac1TxTrigger field's value will be returned.
 *
 * @return The current value of the masksecU2ac1TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_2ac_1_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the masksecU2ac1TxTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu2ac1txtrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_2ac_1_tx_trigger_setf(uint8_t masksecu2ac1txtrigger)
{
    ASSERT_ERR((((uint32_t)masksecu2ac1txtrigger << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)masksecu2ac1txtrigger << 9));
}

/**
 * @brief Returns the current value of the masksecU2ac0TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU2ac0TxTrigger field's value will be returned.
 *
 * @return The current value of the masksecU2ac0TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_2ac_0_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the masksecU2ac0TxTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu2ac0txtrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_2ac_0_tx_trigger_setf(uint8_t masksecu2ac0txtrigger)
{
    ASSERT_ERR((((uint32_t)masksecu2ac0txtrigger << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)masksecu2ac0txtrigger << 8));
}

/**
 * @brief Returns the current value of the masksecU1ac3TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU1ac3TxBufTrigger field's value will be returned.
 *
 * @return The current value of the masksecU1ac3TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_1ac_3_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the masksecU1ac3TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu1ac3txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_1ac_3_tx_buf_trigger_setf(uint8_t masksecu1ac3txbuftrigger)
{
    ASSERT_ERR((((uint32_t)masksecu1ac3txbuftrigger << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)masksecu1ac3txbuftrigger << 7));
}

/**
 * @brief Returns the current value of the masksecU1ac2TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU1ac2TxBufTrigger field's value will be returned.
 *
 * @return The current value of the masksecU1ac2TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_1ac_2_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the masksecU1ac2TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu1ac2txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_1ac_2_tx_buf_trigger_setf(uint8_t masksecu1ac2txbuftrigger)
{
    ASSERT_ERR((((uint32_t)masksecu1ac2txbuftrigger << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)masksecu1ac2txbuftrigger << 6));
}

/**
 * @brief Returns the current value of the masksecU1ac1TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU1ac1TxBufTrigger field's value will be returned.
 *
 * @return The current value of the masksecU1ac1TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_1ac_1_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the masksecU1ac1TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu1ac1txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_1ac_1_tx_buf_trigger_setf(uint8_t masksecu1ac1txbuftrigger)
{
    ASSERT_ERR((((uint32_t)masksecu1ac1txbuftrigger << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)masksecu1ac1txbuftrigger << 5));
}

/**
 * @brief Returns the current value of the masksecU1ac0TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU1ac0TxBufTrigger field's value will be returned.
 *
 * @return The current value of the masksecU1ac0TxBufTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_1ac_0_tx_buf_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the masksecU1ac0TxBufTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu1ac0txbuftrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_1ac_0_tx_buf_trigger_setf(uint8_t masksecu1ac0txbuftrigger)
{
    ASSERT_ERR((((uint32_t)masksecu1ac0txbuftrigger << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)masksecu1ac0txbuftrigger << 4));
}

/**
 * @brief Returns the current value of the masksecU1ac3TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU1ac3TxTrigger field's value will be returned.
 *
 * @return The current value of the masksecU1ac3TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_1ac_3_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the masksecU1ac3TxTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu1ac3txtrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_1ac_3_tx_trigger_setf(uint8_t masksecu1ac3txtrigger)
{
    ASSERT_ERR((((uint32_t)masksecu1ac3txtrigger << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)masksecu1ac3txtrigger << 3));
}

/**
 * @brief Returns the current value of the masksecU1ac2TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU1ac2TxTrigger field's value will be returned.
 *
 * @return The current value of the masksecU1ac2TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_1ac_2_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the masksecU1ac2TxTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu1ac2txtrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_1ac_2_tx_trigger_setf(uint8_t masksecu1ac2txtrigger)
{
    ASSERT_ERR((((uint32_t)masksecu1ac2txtrigger << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)masksecu1ac2txtrigger << 2));
}

/**
 * @brief Returns the current value of the masksecU1ac1TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU1ac1TxTrigger field's value will be returned.
 *
 * @return The current value of the masksecU1ac1TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_1ac_1_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the masksecU1ac1TxTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu1ac1txtrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_1ac_1_tx_trigger_setf(uint8_t masksecu1ac1txtrigger)
{
    ASSERT_ERR((((uint32_t)masksecu1ac1txtrigger << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)masksecu1ac1txtrigger << 1));
}

/**
 * @brief Returns the current value of the masksecU1ac0TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read and the masksecU1ac0TxTrigger field's value will be returned.
 *
 * @return The current value of the masksecU1ac0TxTrigger field in the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 */
__INLINE uint8_t nxmac_masksec_u_1ac_0_tx_trigger_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the masksecU1ac0TxTrigger field of the SEC_USERS_TX_INT_EVENT_UN_MASK register.
 *
 * The SEC_USERS_TX_INT_EVENT_UN_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] masksecu1ac0txtrigger - The value to set the field to.
 */
__INLINE void nxmac_masksec_u_1ac_0_tx_trigger_setf(uint8_t masksecu1ac0txtrigger)
{
    ASSERT_ERR((((uint32_t)masksecu1ac0txtrigger << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR, (REG_PL_RD(NXMAC_SEC_USERS_TX_INT_EVENT_UN_MASK_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)masksecu1ac0txtrigger << 0));
}

#endif // RW_MUMIMO_TX_EN
/// @}

/**
 * @name DEBUG_BCN_S_PTR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00     bcnStatusPointer   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_BCN_S_PTR register
#define NXMAC_DEBUG_BCN_S_PTR_ADDR   0xC0008524
/// Offset of the DEBUG_BCN_S_PTR register from the base address
#define NXMAC_DEBUG_BCN_S_PTR_OFFSET 0x00000524
/// Index of the DEBUG_BCN_S_PTR register
#define NXMAC_DEBUG_BCN_S_PTR_INDEX  0x00000149
/// Reset value of the DEBUG_BCN_S_PTR register
#define NXMAC_DEBUG_BCN_S_PTR_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_BCN_S_PTR register.
 * The DEBUG_BCN_S_PTR register will be read and its value returned.
 * @return The current value of the DEBUG_BCN_S_PTR register.
 */
__INLINE uint32_t nxmac_debug_bcn_s_ptr_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_BCN_S_PTR_ADDR);
}

// field definitions
/// BCN_STATUS_POINTER field mask
#define NXMAC_BCN_STATUS_POINTER_MASK   ((uint32_t)0xFFFFFFFF)
/// BCN_STATUS_POINTER field LSB position
#define NXMAC_BCN_STATUS_POINTER_LSB    0
/// BCN_STATUS_POINTER field width
#define NXMAC_BCN_STATUS_POINTER_WIDTH  ((uint32_t)0x00000020)

/// BCN_STATUS_POINTER field reset value
#define NXMAC_BCN_STATUS_POINTER_RST    0x0

/**
 * @brief Returns the current value of the bcnStatusPointer field in the DEBUG_BCN_S_PTR register.
 *
 * The DEBUG_BCN_S_PTR register will be read and the bcnStatusPointer field's value will be returned.
 *
 * @return The current value of the bcnStatusPointer field in the DEBUG_BCN_S_PTR register.
 */
__INLINE uint32_t nxmac_bcn_status_pointer_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_BCN_S_PTR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name DEBUG_AC_0S_PTR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00     ac0StatusPointer   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_AC_0S_PTR register
#define NXMAC_DEBUG_AC_0S_PTR_ADDR   0xC0008528
/// Offset of the DEBUG_AC_0S_PTR register from the base address
#define NXMAC_DEBUG_AC_0S_PTR_OFFSET 0x00000528
/// Index of the DEBUG_AC_0S_PTR register
#define NXMAC_DEBUG_AC_0S_PTR_INDEX  0x0000014A
/// Reset value of the DEBUG_AC_0S_PTR register
#define NXMAC_DEBUG_AC_0S_PTR_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_AC_0S_PTR register.
 * The DEBUG_AC_0S_PTR register will be read and its value returned.
 * @return The current value of the DEBUG_AC_0S_PTR register.
 */
__INLINE uint32_t nxmac_debug_ac_0s_ptr_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_AC_0S_PTR_ADDR);
}

// field definitions
/// AC_0_STATUS_POINTER field mask
#define NXMAC_AC_0_STATUS_POINTER_MASK   ((uint32_t)0xFFFFFFFF)
/// AC_0_STATUS_POINTER field LSB position
#define NXMAC_AC_0_STATUS_POINTER_LSB    0
/// AC_0_STATUS_POINTER field width
#define NXMAC_AC_0_STATUS_POINTER_WIDTH  ((uint32_t)0x00000020)

/// AC_0_STATUS_POINTER field reset value
#define NXMAC_AC_0_STATUS_POINTER_RST    0x0

/**
 * @brief Returns the current value of the ac0StatusPointer field in the DEBUG_AC_0S_PTR register.
 *
 * The DEBUG_AC_0S_PTR register will be read and the ac0StatusPointer field's value will be returned.
 *
 * @return The current value of the ac0StatusPointer field in the DEBUG_AC_0S_PTR register.
 */
__INLINE uint32_t nxmac_ac_0_status_pointer_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_AC_0S_PTR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name DEBUG_AC_1S_PTR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00     ac1StatusPointer   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_AC_1S_PTR register
#define NXMAC_DEBUG_AC_1S_PTR_ADDR   0xC000852C
/// Offset of the DEBUG_AC_1S_PTR register from the base address
#define NXMAC_DEBUG_AC_1S_PTR_OFFSET 0x0000052C
/// Index of the DEBUG_AC_1S_PTR register
#define NXMAC_DEBUG_AC_1S_PTR_INDEX  0x0000014B
/// Reset value of the DEBUG_AC_1S_PTR register
#define NXMAC_DEBUG_AC_1S_PTR_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_AC_1S_PTR register.
 * The DEBUG_AC_1S_PTR register will be read and its value returned.
 * @return The current value of the DEBUG_AC_1S_PTR register.
 */
__INLINE uint32_t nxmac_debug_ac_1s_ptr_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_AC_1S_PTR_ADDR);
}

// field definitions
/// AC_1_STATUS_POINTER field mask
#define NXMAC_AC_1_STATUS_POINTER_MASK   ((uint32_t)0xFFFFFFFF)
/// AC_1_STATUS_POINTER field LSB position
#define NXMAC_AC_1_STATUS_POINTER_LSB    0
/// AC_1_STATUS_POINTER field width
#define NXMAC_AC_1_STATUS_POINTER_WIDTH  ((uint32_t)0x00000020)

/// AC_1_STATUS_POINTER field reset value
#define NXMAC_AC_1_STATUS_POINTER_RST    0x0

/**
 * @brief Returns the current value of the ac1StatusPointer field in the DEBUG_AC_1S_PTR register.
 *
 * The DEBUG_AC_1S_PTR register will be read and the ac1StatusPointer field's value will be returned.
 *
 * @return The current value of the ac1StatusPointer field in the DEBUG_AC_1S_PTR register.
 */
__INLINE uint32_t nxmac_ac_1_status_pointer_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_AC_1S_PTR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name DEBUG_AC_2S_PTR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00     ac2StatusPointer   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_AC_2S_PTR register
#define NXMAC_DEBUG_AC_2S_PTR_ADDR   0xC0008530
/// Offset of the DEBUG_AC_2S_PTR register from the base address
#define NXMAC_DEBUG_AC_2S_PTR_OFFSET 0x00000530
/// Index of the DEBUG_AC_2S_PTR register
#define NXMAC_DEBUG_AC_2S_PTR_INDEX  0x0000014C
/// Reset value of the DEBUG_AC_2S_PTR register
#define NXMAC_DEBUG_AC_2S_PTR_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_AC_2S_PTR register.
 * The DEBUG_AC_2S_PTR register will be read and its value returned.
 * @return The current value of the DEBUG_AC_2S_PTR register.
 */
__INLINE uint32_t nxmac_debug_ac_2s_ptr_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_AC_2S_PTR_ADDR);
}

// field definitions
/// AC_2_STATUS_POINTER field mask
#define NXMAC_AC_2_STATUS_POINTER_MASK   ((uint32_t)0xFFFFFFFF)
/// AC_2_STATUS_POINTER field LSB position
#define NXMAC_AC_2_STATUS_POINTER_LSB    0
/// AC_2_STATUS_POINTER field width
#define NXMAC_AC_2_STATUS_POINTER_WIDTH  ((uint32_t)0x00000020)

/// AC_2_STATUS_POINTER field reset value
#define NXMAC_AC_2_STATUS_POINTER_RST    0x0

/**
 * @brief Returns the current value of the ac2StatusPointer field in the DEBUG_AC_2S_PTR register.
 *
 * The DEBUG_AC_2S_PTR register will be read and the ac2StatusPointer field's value will be returned.
 *
 * @return The current value of the ac2StatusPointer field in the DEBUG_AC_2S_PTR register.
 */
__INLINE uint32_t nxmac_ac_2_status_pointer_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_AC_2S_PTR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name DEBUG_AC_3S_PTR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00     ac3StatusPointer   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_AC_3S_PTR register
#define NXMAC_DEBUG_AC_3S_PTR_ADDR   0xC0008534
/// Offset of the DEBUG_AC_3S_PTR register from the base address
#define NXMAC_DEBUG_AC_3S_PTR_OFFSET 0x00000534
/// Index of the DEBUG_AC_3S_PTR register
#define NXMAC_DEBUG_AC_3S_PTR_INDEX  0x0000014D
/// Reset value of the DEBUG_AC_3S_PTR register
#define NXMAC_DEBUG_AC_3S_PTR_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_AC_3S_PTR register.
 * The DEBUG_AC_3S_PTR register will be read and its value returned.
 * @return The current value of the DEBUG_AC_3S_PTR register.
 */
__INLINE uint32_t nxmac_debug_ac_3s_ptr_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_AC_3S_PTR_ADDR);
}

// field definitions
/// AC_3_STATUS_POINTER field mask
#define NXMAC_AC_3_STATUS_POINTER_MASK   ((uint32_t)0xFFFFFFFF)
/// AC_3_STATUS_POINTER field LSB position
#define NXMAC_AC_3_STATUS_POINTER_LSB    0
/// AC_3_STATUS_POINTER field width
#define NXMAC_AC_3_STATUS_POINTER_WIDTH  ((uint32_t)0x00000020)

/// AC_3_STATUS_POINTER field reset value
#define NXMAC_AC_3_STATUS_POINTER_RST    0x0

/**
 * @brief Returns the current value of the ac3StatusPointer field in the DEBUG_AC_3S_PTR register.
 *
 * The DEBUG_AC_3S_PTR register will be read and the ac3StatusPointer field's value will be returned.
 *
 * @return The current value of the ac3StatusPointer field in the DEBUG_AC_3S_PTR register.
 */
__INLINE uint32_t nxmac_ac_3_status_pointer_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_AC_3S_PTR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name DEBUG_TX_C_PTR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00     txCurrentPointer   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_TX_C_PTR register
#define NXMAC_DEBUG_TX_C_PTR_ADDR   0xC0008540
/// Offset of the DEBUG_TX_C_PTR register from the base address
#define NXMAC_DEBUG_TX_C_PTR_OFFSET 0x00000540
/// Index of the DEBUG_TX_C_PTR register
#define NXMAC_DEBUG_TX_C_PTR_INDEX  0x00000150
/// Reset value of the DEBUG_TX_C_PTR register
#define NXMAC_DEBUG_TX_C_PTR_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_TX_C_PTR register.
 * The DEBUG_TX_C_PTR register will be read and its value returned.
 * @return The current value of the DEBUG_TX_C_PTR register.
 */
__INLINE uint32_t nxmac_debug_tx_c_ptr_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_TX_C_PTR_ADDR);
}

// field definitions
/// TX_CURRENT_POINTER field mask
#define NXMAC_TX_CURRENT_POINTER_MASK   ((uint32_t)0xFFFFFFFF)
/// TX_CURRENT_POINTER field LSB position
#define NXMAC_TX_CURRENT_POINTER_LSB    0
/// TX_CURRENT_POINTER field width
#define NXMAC_TX_CURRENT_POINTER_WIDTH  ((uint32_t)0x00000020)

/// TX_CURRENT_POINTER field reset value
#define NXMAC_TX_CURRENT_POINTER_RST    0x0

/**
 * @brief Returns the current value of the txCurrentPointer field in the DEBUG_TX_C_PTR register.
 *
 * The DEBUG_TX_C_PTR register will be read and the txCurrentPointer field's value will be returned.
 *
 * @return The current value of the txCurrentPointer field in the DEBUG_TX_C_PTR register.
 */
__INLINE uint32_t nxmac_tx_current_pointer_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_TX_C_PTR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name DEBUG_RX_PAY_S_PTR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00     rxPayStatPointer   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_RX_PAY_S_PTR register
#define NXMAC_DEBUG_RX_PAY_S_PTR_ADDR   0xC0008544
/// Offset of the DEBUG_RX_PAY_S_PTR register from the base address
#define NXMAC_DEBUG_RX_PAY_S_PTR_OFFSET 0x00000544
/// Index of the DEBUG_RX_PAY_S_PTR register
#define NXMAC_DEBUG_RX_PAY_S_PTR_INDEX  0x00000151
/// Reset value of the DEBUG_RX_PAY_S_PTR register
#define NXMAC_DEBUG_RX_PAY_S_PTR_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_RX_PAY_S_PTR register.
 * The DEBUG_RX_PAY_S_PTR register will be read and its value returned.
 * @return The current value of the DEBUG_RX_PAY_S_PTR register.
 */
__INLINE uint32_t nxmac_debug_rx_pay_s_ptr_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_RX_PAY_S_PTR_ADDR);
}

// field definitions
/// RX_PAY_STAT_POINTER field mask
#define NXMAC_RX_PAY_STAT_POINTER_MASK   ((uint32_t)0xFFFFFFFF)
/// RX_PAY_STAT_POINTER field LSB position
#define NXMAC_RX_PAY_STAT_POINTER_LSB    0
/// RX_PAY_STAT_POINTER field width
#define NXMAC_RX_PAY_STAT_POINTER_WIDTH  ((uint32_t)0x00000020)

/// RX_PAY_STAT_POINTER field reset value
#define NXMAC_RX_PAY_STAT_POINTER_RST    0x0

/**
 * @brief Returns the current value of the rxPayStatPointer field in the DEBUG_RX_PAY_S_PTR register.
 *
 * The DEBUG_RX_PAY_S_PTR register will be read and the rxPayStatPointer field's value will be returned.
 *
 * @return The current value of the rxPayStatPointer field in the DEBUG_RX_PAY_S_PTR register.
 */
__INLINE uint32_t nxmac_rx_pay_stat_pointer_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_RX_PAY_S_PTR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name DEBUG_RX_HDR_C_PTR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00     rxHdrStatPointer   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_RX_HDR_C_PTR register
#define NXMAC_DEBUG_RX_HDR_C_PTR_ADDR   0xC0008548
/// Offset of the DEBUG_RX_HDR_C_PTR register from the base address
#define NXMAC_DEBUG_RX_HDR_C_PTR_OFFSET 0x00000548
/// Index of the DEBUG_RX_HDR_C_PTR register
#define NXMAC_DEBUG_RX_HDR_C_PTR_INDEX  0x00000152
/// Reset value of the DEBUG_RX_HDR_C_PTR register
#define NXMAC_DEBUG_RX_HDR_C_PTR_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_RX_HDR_C_PTR register.
 * The DEBUG_RX_HDR_C_PTR register will be read and its value returned.
 * @return The current value of the DEBUG_RX_HDR_C_PTR register.
 */
__INLINE uint32_t nxmac_debug_rx_hdr_c_ptr_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_RX_HDR_C_PTR_ADDR);
}

// field definitions
/// RX_HDR_STAT_POINTER field mask
#define NXMAC_RX_HDR_STAT_POINTER_MASK   ((uint32_t)0xFFFFFFFF)
/// RX_HDR_STAT_POINTER field LSB position
#define NXMAC_RX_HDR_STAT_POINTER_LSB    0
/// RX_HDR_STAT_POINTER field width
#define NXMAC_RX_HDR_STAT_POINTER_WIDTH  ((uint32_t)0x00000020)

/// RX_HDR_STAT_POINTER field reset value
#define NXMAC_RX_HDR_STAT_POINTER_RST    0x0

/**
 * @brief Returns the current value of the rxHdrStatPointer field in the DEBUG_RX_HDR_C_PTR register.
 *
 * The DEBUG_RX_HDR_C_PTR register will be read and the rxHdrStatPointer field's value will be returned.
 *
 * @return The current value of the rxHdrStatPointer field in the DEBUG_RX_HDR_C_PTR register.
 */
__INLINE uint32_t nxmac_rx_hdr_stat_pointer_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_RX_HDR_C_PTR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name DEBUG_RX_PAY_C_PTR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00   rxPayCurrentPointer   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_RX_PAY_C_PTR register
#define NXMAC_DEBUG_RX_PAY_C_PTR_ADDR   0xC000854C
/// Offset of the DEBUG_RX_PAY_C_PTR register from the base address
#define NXMAC_DEBUG_RX_PAY_C_PTR_OFFSET 0x0000054C
/// Index of the DEBUG_RX_PAY_C_PTR register
#define NXMAC_DEBUG_RX_PAY_C_PTR_INDEX  0x00000153
/// Reset value of the DEBUG_RX_PAY_C_PTR register
#define NXMAC_DEBUG_RX_PAY_C_PTR_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_RX_PAY_C_PTR register.
 * The DEBUG_RX_PAY_C_PTR register will be read and its value returned.
 * @return The current value of the DEBUG_RX_PAY_C_PTR register.
 */
__INLINE uint32_t nxmac_debug_rx_pay_c_ptr_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_RX_PAY_C_PTR_ADDR);
}

// field definitions
/// RX_PAY_CURRENT_POINTER field mask
#define NXMAC_RX_PAY_CURRENT_POINTER_MASK   ((uint32_t)0xFFFFFFFF)
/// RX_PAY_CURRENT_POINTER field LSB position
#define NXMAC_RX_PAY_CURRENT_POINTER_LSB    0
/// RX_PAY_CURRENT_POINTER field width
#define NXMAC_RX_PAY_CURRENT_POINTER_WIDTH  ((uint32_t)0x00000020)

/// RX_PAY_CURRENT_POINTER field reset value
#define NXMAC_RX_PAY_CURRENT_POINTER_RST    0x0

/**
 * @brief Returns the current value of the rxPayCurrentPointer field in the DEBUG_RX_PAY_C_PTR register.
 *
 * The DEBUG_RX_PAY_C_PTR register will be read and the rxPayCurrentPointer field's value will be returned.
 *
 * @return The current value of the rxPayCurrentPointer field in the DEBUG_RX_PAY_C_PTR register.
 */
__INLINE uint32_t nxmac_rx_pay_current_pointer_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_RX_PAY_C_PTR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name SW_PROFILING register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31             swProf31   0
 *     30             swProf30   0
 *     29             swProf29   0
 *     28             swProf28   0
 *     27             swProf27   0
 *     26             swProf26   0
 *     25             swProf25   0
 *     24             swProf24   0
 *     23             swProf23   0
 *     22             swProf22   0
 *     21             swProf21   0
 *     20             swProf20   0
 *     19             swProf19   0
 *     18             swProf18   0
 *     17             swProf17   0
 *     16             swProf16   0
 *     15             swProf15   0
 *     14             swProf14   0
 *     13             swProf13   0
 *     12             swProf12   0
 *     11             swProf11   0
 *     10             swProf10   0
 *     09              swProf9   0
 *     08              swProf8   0
 *     07              swProf7   0
 *     06              swProf6   0
 *     05              swProf5   0
 *     04              swProf4   0
 *     03              swProf3   0
 *     02              swProf2   0
 *     01              swProf1   0
 *     00              swProf0   0
 * </pre>
 *
 * @{
 */

/// Address of the SW_PROFILING register
#define NXMAC_SW_PROFILING_ADDR   0xC0008560
/// Offset of the SW_PROFILING register from the base address
#define NXMAC_SW_PROFILING_OFFSET 0x00000560
/// Index of the SW_PROFILING register
#define NXMAC_SW_PROFILING_INDEX  0x00000158
/// Reset value of the SW_PROFILING register
#define NXMAC_SW_PROFILING_RESET  0x00000000

/**
 * @brief Returns the current value of the SW_PROFILING register.
 * The SW_PROFILING register will be read and its value returned.
 * @return The current value of the SW_PROFILING register.
 */
__INLINE uint32_t nxmac_sw_profiling_get(void)
{
    return REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
}

/**
 * @brief Sets the SW_PROFILING register to a value.
 * The SW_PROFILING register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_sw_profiling_set(uint32_t value)
{
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, value);
}

// field definitions
/// SW_PROF_31 field bit
#define NXMAC_SW_PROF_31_BIT    ((uint32_t)0x80000000)
/// SW_PROF_31 field position
#define NXMAC_SW_PROF_31_POS    31
/// SW_PROF_30 field bit
#define NXMAC_SW_PROF_30_BIT    ((uint32_t)0x40000000)
/// SW_PROF_30 field position
#define NXMAC_SW_PROF_30_POS    30
/// SW_PROF_29 field bit
#define NXMAC_SW_PROF_29_BIT    ((uint32_t)0x20000000)
/// SW_PROF_29 field position
#define NXMAC_SW_PROF_29_POS    29
/// SW_PROF_28 field bit
#define NXMAC_SW_PROF_28_BIT    ((uint32_t)0x10000000)
/// SW_PROF_28 field position
#define NXMAC_SW_PROF_28_POS    28
/// SW_PROF_27 field bit
#define NXMAC_SW_PROF_27_BIT    ((uint32_t)0x08000000)
/// SW_PROF_27 field position
#define NXMAC_SW_PROF_27_POS    27
/// SW_PROF_26 field bit
#define NXMAC_SW_PROF_26_BIT    ((uint32_t)0x04000000)
/// SW_PROF_26 field position
#define NXMAC_SW_PROF_26_POS    26
/// SW_PROF_25 field bit
#define NXMAC_SW_PROF_25_BIT    ((uint32_t)0x02000000)
/// SW_PROF_25 field position
#define NXMAC_SW_PROF_25_POS    25
/// SW_PROF_24 field bit
#define NXMAC_SW_PROF_24_BIT    ((uint32_t)0x01000000)
/// SW_PROF_24 field position
#define NXMAC_SW_PROF_24_POS    24
/// SW_PROF_23 field bit
#define NXMAC_SW_PROF_23_BIT    ((uint32_t)0x00800000)
/// SW_PROF_23 field position
#define NXMAC_SW_PROF_23_POS    23
/// SW_PROF_22 field bit
#define NXMAC_SW_PROF_22_BIT    ((uint32_t)0x00400000)
/// SW_PROF_22 field position
#define NXMAC_SW_PROF_22_POS    22
/// SW_PROF_21 field bit
#define NXMAC_SW_PROF_21_BIT    ((uint32_t)0x00200000)
/// SW_PROF_21 field position
#define NXMAC_SW_PROF_21_POS    21
/// SW_PROF_20 field bit
#define NXMAC_SW_PROF_20_BIT    ((uint32_t)0x00100000)
/// SW_PROF_20 field position
#define NXMAC_SW_PROF_20_POS    20
/// SW_PROF_19 field bit
#define NXMAC_SW_PROF_19_BIT    ((uint32_t)0x00080000)
/// SW_PROF_19 field position
#define NXMAC_SW_PROF_19_POS    19
/// SW_PROF_18 field bit
#define NXMAC_SW_PROF_18_BIT    ((uint32_t)0x00040000)
/// SW_PROF_18 field position
#define NXMAC_SW_PROF_18_POS    18
/// SW_PROF_17 field bit
#define NXMAC_SW_PROF_17_BIT    ((uint32_t)0x00020000)
/// SW_PROF_17 field position
#define NXMAC_SW_PROF_17_POS    17
/// SW_PROF_16 field bit
#define NXMAC_SW_PROF_16_BIT    ((uint32_t)0x00010000)
/// SW_PROF_16 field position
#define NXMAC_SW_PROF_16_POS    16
/// SW_PROF_15 field bit
#define NXMAC_SW_PROF_15_BIT    ((uint32_t)0x00008000)
/// SW_PROF_15 field position
#define NXMAC_SW_PROF_15_POS    15
/// SW_PROF_14 field bit
#define NXMAC_SW_PROF_14_BIT    ((uint32_t)0x00004000)
/// SW_PROF_14 field position
#define NXMAC_SW_PROF_14_POS    14
/// SW_PROF_13 field bit
#define NXMAC_SW_PROF_13_BIT    ((uint32_t)0x00002000)
/// SW_PROF_13 field position
#define NXMAC_SW_PROF_13_POS    13
/// SW_PROF_12 field bit
#define NXMAC_SW_PROF_12_BIT    ((uint32_t)0x00001000)
/// SW_PROF_12 field position
#define NXMAC_SW_PROF_12_POS    12
/// SW_PROF_11 field bit
#define NXMAC_SW_PROF_11_BIT    ((uint32_t)0x00000800)
/// SW_PROF_11 field position
#define NXMAC_SW_PROF_11_POS    11
/// SW_PROF_10 field bit
#define NXMAC_SW_PROF_10_BIT    ((uint32_t)0x00000400)
/// SW_PROF_10 field position
#define NXMAC_SW_PROF_10_POS    10
/// SW_PROF_9 field bit
#define NXMAC_SW_PROF_9_BIT     ((uint32_t)0x00000200)
/// SW_PROF_9 field position
#define NXMAC_SW_PROF_9_POS     9
/// SW_PROF_8 field bit
#define NXMAC_SW_PROF_8_BIT     ((uint32_t)0x00000100)
/// SW_PROF_8 field position
#define NXMAC_SW_PROF_8_POS     8
/// SW_PROF_7 field bit
#define NXMAC_SW_PROF_7_BIT     ((uint32_t)0x00000080)
/// SW_PROF_7 field position
#define NXMAC_SW_PROF_7_POS     7
/// SW_PROF_6 field bit
#define NXMAC_SW_PROF_6_BIT     ((uint32_t)0x00000040)
/// SW_PROF_6 field position
#define NXMAC_SW_PROF_6_POS     6
/// SW_PROF_5 field bit
#define NXMAC_SW_PROF_5_BIT     ((uint32_t)0x00000020)
/// SW_PROF_5 field position
#define NXMAC_SW_PROF_5_POS     5
/// SW_PROF_4 field bit
#define NXMAC_SW_PROF_4_BIT     ((uint32_t)0x00000010)
/// SW_PROF_4 field position
#define NXMAC_SW_PROF_4_POS     4
/// SW_PROF_3 field bit
#define NXMAC_SW_PROF_3_BIT     ((uint32_t)0x00000008)
/// SW_PROF_3 field position
#define NXMAC_SW_PROF_3_POS     3
/// SW_PROF_2 field bit
#define NXMAC_SW_PROF_2_BIT     ((uint32_t)0x00000004)
/// SW_PROF_2 field position
#define NXMAC_SW_PROF_2_POS     2
/// SW_PROF_1 field bit
#define NXMAC_SW_PROF_1_BIT     ((uint32_t)0x00000002)
/// SW_PROF_1 field position
#define NXMAC_SW_PROF_1_POS     1
/// SW_PROF_0 field bit
#define NXMAC_SW_PROF_0_BIT     ((uint32_t)0x00000001)
/// SW_PROF_0 field position
#define NXMAC_SW_PROF_0_POS     0

/// SW_PROF_31 field reset value
#define NXMAC_SW_PROF_31_RST    0x0
/// SW_PROF_30 field reset value
#define NXMAC_SW_PROF_30_RST    0x0
/// SW_PROF_29 field reset value
#define NXMAC_SW_PROF_29_RST    0x0
/// SW_PROF_28 field reset value
#define NXMAC_SW_PROF_28_RST    0x0
/// SW_PROF_27 field reset value
#define NXMAC_SW_PROF_27_RST    0x0
/// SW_PROF_26 field reset value
#define NXMAC_SW_PROF_26_RST    0x0
/// SW_PROF_25 field reset value
#define NXMAC_SW_PROF_25_RST    0x0
/// SW_PROF_24 field reset value
#define NXMAC_SW_PROF_24_RST    0x0
/// SW_PROF_23 field reset value
#define NXMAC_SW_PROF_23_RST    0x0
/// SW_PROF_22 field reset value
#define NXMAC_SW_PROF_22_RST    0x0
/// SW_PROF_21 field reset value
#define NXMAC_SW_PROF_21_RST    0x0
/// SW_PROF_20 field reset value
#define NXMAC_SW_PROF_20_RST    0x0
/// SW_PROF_19 field reset value
#define NXMAC_SW_PROF_19_RST    0x0
/// SW_PROF_18 field reset value
#define NXMAC_SW_PROF_18_RST    0x0
/// SW_PROF_17 field reset value
#define NXMAC_SW_PROF_17_RST    0x0
/// SW_PROF_16 field reset value
#define NXMAC_SW_PROF_16_RST    0x0
/// SW_PROF_15 field reset value
#define NXMAC_SW_PROF_15_RST    0x0
/// SW_PROF_14 field reset value
#define NXMAC_SW_PROF_14_RST    0x0
/// SW_PROF_13 field reset value
#define NXMAC_SW_PROF_13_RST    0x0
/// SW_PROF_12 field reset value
#define NXMAC_SW_PROF_12_RST    0x0
/// SW_PROF_11 field reset value
#define NXMAC_SW_PROF_11_RST    0x0
/// SW_PROF_10 field reset value
#define NXMAC_SW_PROF_10_RST    0x0
/// SW_PROF_9 field reset value
#define NXMAC_SW_PROF_9_RST     0x0
/// SW_PROF_8 field reset value
#define NXMAC_SW_PROF_8_RST     0x0
/// SW_PROF_7 field reset value
#define NXMAC_SW_PROF_7_RST     0x0
/// SW_PROF_6 field reset value
#define NXMAC_SW_PROF_6_RST     0x0
/// SW_PROF_5 field reset value
#define NXMAC_SW_PROF_5_RST     0x0
/// SW_PROF_4 field reset value
#define NXMAC_SW_PROF_4_RST     0x0
/// SW_PROF_3 field reset value
#define NXMAC_SW_PROF_3_RST     0x0
/// SW_PROF_2 field reset value
#define NXMAC_SW_PROF_2_RST     0x0
/// SW_PROF_1 field reset value
#define NXMAC_SW_PROF_1_RST     0x0
/// SW_PROF_0 field reset value
#define NXMAC_SW_PROF_0_RST     0x0

/**
 * @brief Constructs a value for the SW_PROFILING register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] swprof31 - The value to use for the swProf31 field.
 * @param[in] swprof30 - The value to use for the swProf30 field.
 * @param[in] swprof29 - The value to use for the swProf29 field.
 * @param[in] swprof28 - The value to use for the swProf28 field.
 * @param[in] swprof27 - The value to use for the swProf27 field.
 * @param[in] swprof26 - The value to use for the swProf26 field.
 * @param[in] swprof25 - The value to use for the swProf25 field.
 * @param[in] swprof24 - The value to use for the swProf24 field.
 * @param[in] swprof23 - The value to use for the swProf23 field.
 * @param[in] swprof22 - The value to use for the swProf22 field.
 * @param[in] swprof21 - The value to use for the swProf21 field.
 * @param[in] swprof20 - The value to use for the swProf20 field.
 * @param[in] swprof19 - The value to use for the swProf19 field.
 * @param[in] swprof18 - The value to use for the swProf18 field.
 * @param[in] swprof17 - The value to use for the swProf17 field.
 * @param[in] swprof16 - The value to use for the swProf16 field.
 * @param[in] swprof15 - The value to use for the swProf15 field.
 * @param[in] swprof14 - The value to use for the swProf14 field.
 * @param[in] swprof13 - The value to use for the swProf13 field.
 * @param[in] swprof12 - The value to use for the swProf12 field.
 * @param[in] swprof11 - The value to use for the swProf11 field.
 * @param[in] swprof10 - The value to use for the swProf10 field.
 * @param[in] swprof9 - The value to use for the swProf9 field.
 * @param[in] swprof8 - The value to use for the swProf8 field.
 * @param[in] swprof7 - The value to use for the swProf7 field.
 * @param[in] swprof6 - The value to use for the swProf6 field.
 * @param[in] swprof5 - The value to use for the swProf5 field.
 * @param[in] swprof4 - The value to use for the swProf4 field.
 * @param[in] swprof3 - The value to use for the swProf3 field.
 * @param[in] swprof2 - The value to use for the swProf2 field.
 * @param[in] swprof1 - The value to use for the swProf1 field.
 * @param[in] swprof0 - The value to use for the swProf0 field.
 */
__INLINE void nxmac_sw_profiling_pack(uint8_t swprof31, uint8_t swprof30, uint8_t swprof29, uint8_t swprof28, uint8_t swprof27, uint8_t swprof26, uint8_t swprof25, uint8_t swprof24, uint8_t swprof23, uint8_t swprof22, uint8_t swprof21, uint8_t swprof20, uint8_t swprof19, uint8_t swprof18, uint8_t swprof17, uint8_t swprof16, uint8_t swprof15, uint8_t swprof14, uint8_t swprof13, uint8_t swprof12, uint8_t swprof11, uint8_t swprof10, uint8_t swprof9, uint8_t swprof8, uint8_t swprof7, uint8_t swprof6, uint8_t swprof5, uint8_t swprof4, uint8_t swprof3, uint8_t swprof2, uint8_t swprof1, uint8_t swprof0)
{
    ASSERT_ERR((((uint32_t)swprof31 << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)swprof30 << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)swprof29 << 29) & ~((uint32_t)0x20000000)) == 0);
    ASSERT_ERR((((uint32_t)swprof28 << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)swprof27 << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)swprof26 << 26) & ~((uint32_t)0x04000000)) == 0);
    ASSERT_ERR((((uint32_t)swprof25 << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)swprof24 << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)swprof23 << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)swprof22 << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)swprof21 << 21) & ~((uint32_t)0x00200000)) == 0);
    ASSERT_ERR((((uint32_t)swprof20 << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)swprof19 << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)swprof18 << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)swprof17 << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)swprof16 << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)swprof15 << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)swprof14 << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)swprof13 << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)swprof12 << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)swprof11 << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)swprof10 << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)swprof9 << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)swprof8 << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)swprof7 << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)swprof6 << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)swprof5 << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)swprof4 << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)swprof3 << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)swprof2 << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)swprof1 << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)swprof0 << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR,  ((uint32_t)swprof31 << 31) | ((uint32_t)swprof30 << 30) | ((uint32_t)swprof29 << 29) | ((uint32_t)swprof28 << 28) | ((uint32_t)swprof27 << 27) | ((uint32_t)swprof26 << 26) | ((uint32_t)swprof25 << 25) | ((uint32_t)swprof24 << 24) | ((uint32_t)swprof23 << 23) | ((uint32_t)swprof22 << 22) | ((uint32_t)swprof21 << 21) | ((uint32_t)swprof20 << 20) | ((uint32_t)swprof19 << 19) | ((uint32_t)swprof18 << 18) | ((uint32_t)swprof17 << 17) | ((uint32_t)swprof16 << 16) | ((uint32_t)swprof15 << 15) | ((uint32_t)swprof14 << 14) | ((uint32_t)swprof13 << 13) | ((uint32_t)swprof12 << 12) | ((uint32_t)swprof11 << 11) | ((uint32_t)swprof10 << 10) | ((uint32_t)swprof9 << 9) | ((uint32_t)swprof8 << 8) | ((uint32_t)swprof7 << 7) | ((uint32_t)swprof6 << 6) | ((uint32_t)swprof5 << 5) | ((uint32_t)swprof4 << 4) | ((uint32_t)swprof3 << 3) | ((uint32_t)swprof2 << 2) | ((uint32_t)swprof1 << 1) | ((uint32_t)swprof0 << 0));
}

/**
 * @brief Unpacks SW_PROFILING's fields from current value of the SW_PROFILING register.
 *
 * Reads the SW_PROFILING register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] swprof31 - Will be populated with the current value of this field from the register.
 * @param[out] swprof30 - Will be populated with the current value of this field from the register.
 * @param[out] swprof29 - Will be populated with the current value of this field from the register.
 * @param[out] swprof28 - Will be populated with the current value of this field from the register.
 * @param[out] swprof27 - Will be populated with the current value of this field from the register.
 * @param[out] swprof26 - Will be populated with the current value of this field from the register.
 * @param[out] swprof25 - Will be populated with the current value of this field from the register.
 * @param[out] swprof24 - Will be populated with the current value of this field from the register.
 * @param[out] swprof23 - Will be populated with the current value of this field from the register.
 * @param[out] swprof22 - Will be populated with the current value of this field from the register.
 * @param[out] swprof21 - Will be populated with the current value of this field from the register.
 * @param[out] swprof20 - Will be populated with the current value of this field from the register.
 * @param[out] swprof19 - Will be populated with the current value of this field from the register.
 * @param[out] swprof18 - Will be populated with the current value of this field from the register.
 * @param[out] swprof17 - Will be populated with the current value of this field from the register.
 * @param[out] swprof16 - Will be populated with the current value of this field from the register.
 * @param[out] swprof15 - Will be populated with the current value of this field from the register.
 * @param[out] swprof14 - Will be populated with the current value of this field from the register.
 * @param[out] swprof13 - Will be populated with the current value of this field from the register.
 * @param[out] swprof12 - Will be populated with the current value of this field from the register.
 * @param[out] swprof11 - Will be populated with the current value of this field from the register.
 * @param[out] swprof10 - Will be populated with the current value of this field from the register.
 * @param[out] swprof9 - Will be populated with the current value of this field from the register.
 * @param[out] swprof8 - Will be populated with the current value of this field from the register.
 * @param[out] swprof7 - Will be populated with the current value of this field from the register.
 * @param[out] swprof6 - Will be populated with the current value of this field from the register.
 * @param[out] swprof5 - Will be populated with the current value of this field from the register.
 * @param[out] swprof4 - Will be populated with the current value of this field from the register.
 * @param[out] swprof3 - Will be populated with the current value of this field from the register.
 * @param[out] swprof2 - Will be populated with the current value of this field from the register.
 * @param[out] swprof1 - Will be populated with the current value of this field from the register.
 * @param[out] swprof0 - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_sw_profiling_unpack(uint8_t *swprof31, uint8_t *swprof30, uint8_t *swprof29, uint8_t *swprof28, uint8_t *swprof27, uint8_t *swprof26, uint8_t *swprof25, uint8_t *swprof24, uint8_t *swprof23, uint8_t *swprof22, uint8_t *swprof21, uint8_t *swprof20, uint8_t *swprof19, uint8_t *swprof18, uint8_t *swprof17, uint8_t *swprof16, uint8_t *swprof15, uint8_t *swprof14, uint8_t *swprof13, uint8_t *swprof12, uint8_t *swprof11, uint8_t *swprof10, uint8_t *swprof9, uint8_t *swprof8, uint8_t *swprof7, uint8_t *swprof6, uint8_t *swprof5, uint8_t *swprof4, uint8_t *swprof3, uint8_t *swprof2, uint8_t *swprof1, uint8_t *swprof0)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);

    *swprof31 = (localVal & ((uint32_t)0x80000000)) >> 31;
    *swprof30 = (localVal & ((uint32_t)0x40000000)) >> 30;
    *swprof29 = (localVal & ((uint32_t)0x20000000)) >> 29;
    *swprof28 = (localVal & ((uint32_t)0x10000000)) >> 28;
    *swprof27 = (localVal & ((uint32_t)0x08000000)) >> 27;
    *swprof26 = (localVal & ((uint32_t)0x04000000)) >> 26;
    *swprof25 = (localVal & ((uint32_t)0x02000000)) >> 25;
    *swprof24 = (localVal & ((uint32_t)0x01000000)) >> 24;
    *swprof23 = (localVal & ((uint32_t)0x00800000)) >> 23;
    *swprof22 = (localVal & ((uint32_t)0x00400000)) >> 22;
    *swprof21 = (localVal & ((uint32_t)0x00200000)) >> 21;
    *swprof20 = (localVal & ((uint32_t)0x00100000)) >> 20;
    *swprof19 = (localVal & ((uint32_t)0x00080000)) >> 19;
    *swprof18 = (localVal & ((uint32_t)0x00040000)) >> 18;
    *swprof17 = (localVal & ((uint32_t)0x00020000)) >> 17;
    *swprof16 = (localVal & ((uint32_t)0x00010000)) >> 16;
    *swprof15 = (localVal & ((uint32_t)0x00008000)) >> 15;
    *swprof14 = (localVal & ((uint32_t)0x00004000)) >> 14;
    *swprof13 = (localVal & ((uint32_t)0x00002000)) >> 13;
    *swprof12 = (localVal & ((uint32_t)0x00001000)) >> 12;
    *swprof11 = (localVal & ((uint32_t)0x00000800)) >> 11;
    *swprof10 = (localVal & ((uint32_t)0x00000400)) >> 10;
    *swprof9 = (localVal & ((uint32_t)0x00000200)) >> 9;
    *swprof8 = (localVal & ((uint32_t)0x00000100)) >> 8;
    *swprof7 = (localVal & ((uint32_t)0x00000080)) >> 7;
    *swprof6 = (localVal & ((uint32_t)0x00000040)) >> 6;
    *swprof5 = (localVal & ((uint32_t)0x00000020)) >> 5;
    *swprof4 = (localVal & ((uint32_t)0x00000010)) >> 4;
    *swprof3 = (localVal & ((uint32_t)0x00000008)) >> 3;
    *swprof2 = (localVal & ((uint32_t)0x00000004)) >> 2;
    *swprof1 = (localVal & ((uint32_t)0x00000002)) >> 1;
    *swprof0 = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the swProf31 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf31 field's value will be returned.
 *
 * @return The current value of the swProf31 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_31_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the swProf31 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof31 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_31_setf(uint8_t swprof31)
{
    ASSERT_ERR((((uint32_t)swprof31 << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)swprof31 << 31));
}

/**
 * @brief Returns the current value of the swProf30 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf30 field's value will be returned.
 *
 * @return The current value of the swProf30 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_30_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

/**
 * @brief Sets the swProf30 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof30 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_30_setf(uint8_t swprof30)
{
    ASSERT_ERR((((uint32_t)swprof30 << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)swprof30 << 30));
}

/**
 * @brief Returns the current value of the swProf29 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf29 field's value will be returned.
 *
 * @return The current value of the swProf29 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_29_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

/**
 * @brief Sets the swProf29 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof29 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_29_setf(uint8_t swprof29)
{
    ASSERT_ERR((((uint32_t)swprof29 << 29) & ~((uint32_t)0x20000000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x20000000)) | ((uint32_t)swprof29 << 29));
}

/**
 * @brief Returns the current value of the swProf28 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf28 field's value will be returned.
 *
 * @return The current value of the swProf28 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_28_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the swProf28 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof28 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_28_setf(uint8_t swprof28)
{
    ASSERT_ERR((((uint32_t)swprof28 << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)swprof28 << 28));
}

/**
 * @brief Returns the current value of the swProf27 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf27 field's value will be returned.
 *
 * @return The current value of the swProf27 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_27_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Sets the swProf27 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof27 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_27_setf(uint8_t swprof27)
{
    ASSERT_ERR((((uint32_t)swprof27 << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x08000000)) | ((uint32_t)swprof27 << 27));
}

/**
 * @brief Returns the current value of the swProf26 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf26 field's value will be returned.
 *
 * @return The current value of the swProf26 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_26_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

/**
 * @brief Sets the swProf26 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof26 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_26_setf(uint8_t swprof26)
{
    ASSERT_ERR((((uint32_t)swprof26 << 26) & ~((uint32_t)0x04000000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x04000000)) | ((uint32_t)swprof26 << 26));
}

/**
 * @brief Returns the current value of the swProf25 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf25 field's value will be returned.
 *
 * @return The current value of the swProf25 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_25_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

/**
 * @brief Sets the swProf25 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof25 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_25_setf(uint8_t swprof25)
{
    ASSERT_ERR((((uint32_t)swprof25 << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x02000000)) | ((uint32_t)swprof25 << 25));
}

/**
 * @brief Returns the current value of the swProf24 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf24 field's value will be returned.
 *
 * @return The current value of the swProf24 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_24_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the swProf24 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof24 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_24_setf(uint8_t swprof24)
{
    ASSERT_ERR((((uint32_t)swprof24 << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)swprof24 << 24));
}

/**
 * @brief Returns the current value of the swProf23 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf23 field's value will be returned.
 *
 * @return The current value of the swProf23 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_23_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Sets the swProf23 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof23 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_23_setf(uint8_t swprof23)
{
    ASSERT_ERR((((uint32_t)swprof23 << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00800000)) | ((uint32_t)swprof23 << 23));
}

/**
 * @brief Returns the current value of the swProf22 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf22 field's value will be returned.
 *
 * @return The current value of the swProf22 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_22_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Sets the swProf22 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof22 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_22_setf(uint8_t swprof22)
{
    ASSERT_ERR((((uint32_t)swprof22 << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00400000)) | ((uint32_t)swprof22 << 22));
}

/**
 * @brief Returns the current value of the swProf21 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf21 field's value will be returned.
 *
 * @return The current value of the swProf21 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_21_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Sets the swProf21 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof21 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_21_setf(uint8_t swprof21)
{
    ASSERT_ERR((((uint32_t)swprof21 << 21) & ~((uint32_t)0x00200000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00200000)) | ((uint32_t)swprof21 << 21));
}

/**
 * @brief Returns the current value of the swProf20 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf20 field's value will be returned.
 *
 * @return The current value of the swProf20 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_20_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Sets the swProf20 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof20 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_20_setf(uint8_t swprof20)
{
    ASSERT_ERR((((uint32_t)swprof20 << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00100000)) | ((uint32_t)swprof20 << 20));
}

/**
 * @brief Returns the current value of the swProf19 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf19 field's value will be returned.
 *
 * @return The current value of the swProf19 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_19_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Sets the swProf19 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof19 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_19_setf(uint8_t swprof19)
{
    ASSERT_ERR((((uint32_t)swprof19 << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00080000)) | ((uint32_t)swprof19 << 19));
}

/**
 * @brief Returns the current value of the swProf18 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf18 field's value will be returned.
 *
 * @return The current value of the swProf18 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_18_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Sets the swProf18 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof18 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_18_setf(uint8_t swprof18)
{
    ASSERT_ERR((((uint32_t)swprof18 << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00040000)) | ((uint32_t)swprof18 << 18));
}

/**
 * @brief Returns the current value of the swProf17 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf17 field's value will be returned.
 *
 * @return The current value of the swProf17 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_17_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Sets the swProf17 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof17 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_17_setf(uint8_t swprof17)
{
    ASSERT_ERR((((uint32_t)swprof17 << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00020000)) | ((uint32_t)swprof17 << 17));
}

/**
 * @brief Returns the current value of the swProf16 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf16 field's value will be returned.
 *
 * @return The current value of the swProf16 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_16_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the swProf16 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof16 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_16_setf(uint8_t swprof16)
{
    ASSERT_ERR((((uint32_t)swprof16 << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)swprof16 << 16));
}

/**
 * @brief Returns the current value of the swProf15 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf15 field's value will be returned.
 *
 * @return The current value of the swProf15 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_15_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

/**
 * @brief Sets the swProf15 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof15 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_15_setf(uint8_t swprof15)
{
    ASSERT_ERR((((uint32_t)swprof15 << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00008000)) | ((uint32_t)swprof15 << 15));
}

/**
 * @brief Returns the current value of the swProf14 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf14 field's value will be returned.
 *
 * @return The current value of the swProf14 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_14_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Sets the swProf14 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof14 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_14_setf(uint8_t swprof14)
{
    ASSERT_ERR((((uint32_t)swprof14 << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00004000)) | ((uint32_t)swprof14 << 14));
}

/**
 * @brief Returns the current value of the swProf13 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf13 field's value will be returned.
 *
 * @return The current value of the swProf13 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_13_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the swProf13 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof13 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_13_setf(uint8_t swprof13)
{
    ASSERT_ERR((((uint32_t)swprof13 << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)swprof13 << 13));
}

/**
 * @brief Returns the current value of the swProf12 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf12 field's value will be returned.
 *
 * @return The current value of the swProf12 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the swProf12 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof12 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_12_setf(uint8_t swprof12)
{
    ASSERT_ERR((((uint32_t)swprof12 << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)swprof12 << 12));
}

/**
 * @brief Returns the current value of the swProf11 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf11 field's value will be returned.
 *
 * @return The current value of the swProf11 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the swProf11 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof11 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_11_setf(uint8_t swprof11)
{
    ASSERT_ERR((((uint32_t)swprof11 << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00000800)) | ((uint32_t)swprof11 << 11));
}

/**
 * @brief Returns the current value of the swProf10 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf10 field's value will be returned.
 *
 * @return The current value of the swProf10 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the swProf10 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof10 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_10_setf(uint8_t swprof10)
{
    ASSERT_ERR((((uint32_t)swprof10 << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00000400)) | ((uint32_t)swprof10 << 10));
}

/**
 * @brief Returns the current value of the swProf9 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf9 field's value will be returned.
 *
 * @return The current value of the swProf9 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the swProf9 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof9 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_9_setf(uint8_t swprof9)
{
    ASSERT_ERR((((uint32_t)swprof9 << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)swprof9 << 9));
}

/**
 * @brief Returns the current value of the swProf8 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf8 field's value will be returned.
 *
 * @return The current value of the swProf8 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the swProf8 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof8 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_8_setf(uint8_t swprof8)
{
    ASSERT_ERR((((uint32_t)swprof8 << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)swprof8 << 8));
}

/**
 * @brief Returns the current value of the swProf7 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf7 field's value will be returned.
 *
 * @return The current value of the swProf7 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the swProf7 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof7 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_7_setf(uint8_t swprof7)
{
    ASSERT_ERR((((uint32_t)swprof7 << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)swprof7 << 7));
}

/**
 * @brief Returns the current value of the swProf6 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf6 field's value will be returned.
 *
 * @return The current value of the swProf6 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the swProf6 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof6 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_6_setf(uint8_t swprof6)
{
    ASSERT_ERR((((uint32_t)swprof6 << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)swprof6 << 6));
}

/**
 * @brief Returns the current value of the swProf5 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf5 field's value will be returned.
 *
 * @return The current value of the swProf5 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the swProf5 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof5 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_5_setf(uint8_t swprof5)
{
    ASSERT_ERR((((uint32_t)swprof5 << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)swprof5 << 5));
}

/**
 * @brief Returns the current value of the swProf4 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf4 field's value will be returned.
 *
 * @return The current value of the swProf4 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the swProf4 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof4 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_4_setf(uint8_t swprof4)
{
    ASSERT_ERR((((uint32_t)swprof4 << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)swprof4 << 4));
}

/**
 * @brief Returns the current value of the swProf3 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf3 field's value will be returned.
 *
 * @return The current value of the swProf3 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the swProf3 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof3 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_3_setf(uint8_t swprof3)
{
    ASSERT_ERR((((uint32_t)swprof3 << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)swprof3 << 3));
}

/**
 * @brief Returns the current value of the swProf2 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf2 field's value will be returned.
 *
 * @return The current value of the swProf2 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the swProf2 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof2 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_2_setf(uint8_t swprof2)
{
    ASSERT_ERR((((uint32_t)swprof2 << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)swprof2 << 2));
}

/**
 * @brief Returns the current value of the swProf1 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf1 field's value will be returned.
 *
 * @return The current value of the swProf1 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the swProf1 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof1 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_1_setf(uint8_t swprof1)
{
    ASSERT_ERR((((uint32_t)swprof1 << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)swprof1 << 1));
}

/**
 * @brief Returns the current value of the swProf0 field in the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read and the swProf0 field's value will be returned.
 *
 * @return The current value of the swProf0 field in the SW_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_prof_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the swProf0 field of the SW_PROFILING register.
 *
 * The SW_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swprof0 - The value to set the field to.
 */
__INLINE void nxmac_sw_prof_0_setf(uint8_t swprof0)
{
    ASSERT_ERR((((uint32_t)swprof0 << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_SW_PROFILING_ADDR, (REG_PL_RD(NXMAC_SW_PROFILING_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)swprof0 << 0));
}

/// @}

/**
 * @name SW_SET_PROFILING register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31          swSetProf31   0
 *     30          swSetProf30   0
 *     29          swSetProf29   0
 *     28          swSetProf28   0
 *     27          swSetProf27   0
 *     26          swSetProf26   0
 *     25          swSetProf25   0
 *     24          swSetProf24   0
 *     23          swSetProf23   0
 *     22          swSetProf22   0
 *     21          swSetProf21   0
 *     20          swSetProf20   0
 *     19          swSetProf19   0
 *     18          swSetProf18   0
 *     17          swSetProf17   0
 *     16          swSetProf16   0
 *     15          swSetProf15   0
 *     14          swSetProf14   0
 *     13          swSetProf13   0
 *     12          swSetProf12   0
 *     11          swSetProf11   0
 *     10          swSetProf10   0
 *     09           swSetProf9   0
 *     08           swSetProf8   0
 *     07           swSetProf7   0
 *     06           swSetProf6   0
 *     05           swSetProf5   0
 *     04           swSetProf4   0
 *     03           swSetProf3   0
 *     02           swSetProf2   0
 *     01           swSetProf1   0
 *     00           swSetProf0   0
 * </pre>
 *
 * @{
 */

/// Address of the SW_SET_PROFILING register
#define NXMAC_SW_SET_PROFILING_ADDR   0xC0008564
/// Offset of the SW_SET_PROFILING register from the base address
#define NXMAC_SW_SET_PROFILING_OFFSET 0x00000564
/// Index of the SW_SET_PROFILING register
#define NXMAC_SW_SET_PROFILING_INDEX  0x00000159
/// Reset value of the SW_SET_PROFILING register
#define NXMAC_SW_SET_PROFILING_RESET  0x00000000

/**
 * @brief Returns the current value of the SW_SET_PROFILING register.
 * The SW_SET_PROFILING register will be read and its value returned.
 * @return The current value of the SW_SET_PROFILING register.
 */
__INLINE uint32_t nxmac_sw_set_profiling_get(void)
{
    return REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
}

/**
 * @brief Sets the SW_SET_PROFILING register to a value.
 * The SW_SET_PROFILING register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_sw_set_profiling_set(uint32_t value)
{
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, value);
}

// field definitions
/// SW_SET_PROF_31 field bit
#define NXMAC_SW_SET_PROF_31_BIT    ((uint32_t)0x80000000)
/// SW_SET_PROF_31 field position
#define NXMAC_SW_SET_PROF_31_POS    31
/// SW_SET_PROF_30 field bit
#define NXMAC_SW_SET_PROF_30_BIT    ((uint32_t)0x40000000)
/// SW_SET_PROF_30 field position
#define NXMAC_SW_SET_PROF_30_POS    30
/// SW_SET_PROF_29 field bit
#define NXMAC_SW_SET_PROF_29_BIT    ((uint32_t)0x20000000)
/// SW_SET_PROF_29 field position
#define NXMAC_SW_SET_PROF_29_POS    29
/// SW_SET_PROF_28 field bit
#define NXMAC_SW_SET_PROF_28_BIT    ((uint32_t)0x10000000)
/// SW_SET_PROF_28 field position
#define NXMAC_SW_SET_PROF_28_POS    28
/// SW_SET_PROF_27 field bit
#define NXMAC_SW_SET_PROF_27_BIT    ((uint32_t)0x08000000)
/// SW_SET_PROF_27 field position
#define NXMAC_SW_SET_PROF_27_POS    27
/// SW_SET_PROF_26 field bit
#define NXMAC_SW_SET_PROF_26_BIT    ((uint32_t)0x04000000)
/// SW_SET_PROF_26 field position
#define NXMAC_SW_SET_PROF_26_POS    26
/// SW_SET_PROF_25 field bit
#define NXMAC_SW_SET_PROF_25_BIT    ((uint32_t)0x02000000)
/// SW_SET_PROF_25 field position
#define NXMAC_SW_SET_PROF_25_POS    25
/// SW_SET_PROF_24 field bit
#define NXMAC_SW_SET_PROF_24_BIT    ((uint32_t)0x01000000)
/// SW_SET_PROF_24 field position
#define NXMAC_SW_SET_PROF_24_POS    24
/// SW_SET_PROF_23 field bit
#define NXMAC_SW_SET_PROF_23_BIT    ((uint32_t)0x00800000)
/// SW_SET_PROF_23 field position
#define NXMAC_SW_SET_PROF_23_POS    23
/// SW_SET_PROF_22 field bit
#define NXMAC_SW_SET_PROF_22_BIT    ((uint32_t)0x00400000)
/// SW_SET_PROF_22 field position
#define NXMAC_SW_SET_PROF_22_POS    22
/// SW_SET_PROF_21 field bit
#define NXMAC_SW_SET_PROF_21_BIT    ((uint32_t)0x00200000)
/// SW_SET_PROF_21 field position
#define NXMAC_SW_SET_PROF_21_POS    21
/// SW_SET_PROF_20 field bit
#define NXMAC_SW_SET_PROF_20_BIT    ((uint32_t)0x00100000)
/// SW_SET_PROF_20 field position
#define NXMAC_SW_SET_PROF_20_POS    20
/// SW_SET_PROF_19 field bit
#define NXMAC_SW_SET_PROF_19_BIT    ((uint32_t)0x00080000)
/// SW_SET_PROF_19 field position
#define NXMAC_SW_SET_PROF_19_POS    19
/// SW_SET_PROF_18 field bit
#define NXMAC_SW_SET_PROF_18_BIT    ((uint32_t)0x00040000)
/// SW_SET_PROF_18 field position
#define NXMAC_SW_SET_PROF_18_POS    18
/// SW_SET_PROF_17 field bit
#define NXMAC_SW_SET_PROF_17_BIT    ((uint32_t)0x00020000)
/// SW_SET_PROF_17 field position
#define NXMAC_SW_SET_PROF_17_POS    17
/// SW_SET_PROF_16 field bit
#define NXMAC_SW_SET_PROF_16_BIT    ((uint32_t)0x00010000)
/// SW_SET_PROF_16 field position
#define NXMAC_SW_SET_PROF_16_POS    16
/// SW_SET_PROF_15 field bit
#define NXMAC_SW_SET_PROF_15_BIT    ((uint32_t)0x00008000)
/// SW_SET_PROF_15 field position
#define NXMAC_SW_SET_PROF_15_POS    15
/// SW_SET_PROF_14 field bit
#define NXMAC_SW_SET_PROF_14_BIT    ((uint32_t)0x00004000)
/// SW_SET_PROF_14 field position
#define NXMAC_SW_SET_PROF_14_POS    14
/// SW_SET_PROF_13 field bit
#define NXMAC_SW_SET_PROF_13_BIT    ((uint32_t)0x00002000)
/// SW_SET_PROF_13 field position
#define NXMAC_SW_SET_PROF_13_POS    13
/// SW_SET_PROF_12 field bit
#define NXMAC_SW_SET_PROF_12_BIT    ((uint32_t)0x00001000)
/// SW_SET_PROF_12 field position
#define NXMAC_SW_SET_PROF_12_POS    12
/// SW_SET_PROF_11 field bit
#define NXMAC_SW_SET_PROF_11_BIT    ((uint32_t)0x00000800)
/// SW_SET_PROF_11 field position
#define NXMAC_SW_SET_PROF_11_POS    11
/// SW_SET_PROF_10 field bit
#define NXMAC_SW_SET_PROF_10_BIT    ((uint32_t)0x00000400)
/// SW_SET_PROF_10 field position
#define NXMAC_SW_SET_PROF_10_POS    10
/// SW_SET_PROF_9 field bit
#define NXMAC_SW_SET_PROF_9_BIT     ((uint32_t)0x00000200)
/// SW_SET_PROF_9 field position
#define NXMAC_SW_SET_PROF_9_POS     9
/// SW_SET_PROF_8 field bit
#define NXMAC_SW_SET_PROF_8_BIT     ((uint32_t)0x00000100)
/// SW_SET_PROF_8 field position
#define NXMAC_SW_SET_PROF_8_POS     8
/// SW_SET_PROF_7 field bit
#define NXMAC_SW_SET_PROF_7_BIT     ((uint32_t)0x00000080)
/// SW_SET_PROF_7 field position
#define NXMAC_SW_SET_PROF_7_POS     7
/// SW_SET_PROF_6 field bit
#define NXMAC_SW_SET_PROF_6_BIT     ((uint32_t)0x00000040)
/// SW_SET_PROF_6 field position
#define NXMAC_SW_SET_PROF_6_POS     6
/// SW_SET_PROF_5 field bit
#define NXMAC_SW_SET_PROF_5_BIT     ((uint32_t)0x00000020)
/// SW_SET_PROF_5 field position
#define NXMAC_SW_SET_PROF_5_POS     5
/// SW_SET_PROF_4 field bit
#define NXMAC_SW_SET_PROF_4_BIT     ((uint32_t)0x00000010)
/// SW_SET_PROF_4 field position
#define NXMAC_SW_SET_PROF_4_POS     4
/// SW_SET_PROF_3 field bit
#define NXMAC_SW_SET_PROF_3_BIT     ((uint32_t)0x00000008)
/// SW_SET_PROF_3 field position
#define NXMAC_SW_SET_PROF_3_POS     3
/// SW_SET_PROF_2 field bit
#define NXMAC_SW_SET_PROF_2_BIT     ((uint32_t)0x00000004)
/// SW_SET_PROF_2 field position
#define NXMAC_SW_SET_PROF_2_POS     2
/// SW_SET_PROF_1 field bit
#define NXMAC_SW_SET_PROF_1_BIT     ((uint32_t)0x00000002)
/// SW_SET_PROF_1 field position
#define NXMAC_SW_SET_PROF_1_POS     1
/// SW_SET_PROF_0 field bit
#define NXMAC_SW_SET_PROF_0_BIT     ((uint32_t)0x00000001)
/// SW_SET_PROF_0 field position
#define NXMAC_SW_SET_PROF_0_POS     0

/// SW_SET_PROF_31 field reset value
#define NXMAC_SW_SET_PROF_31_RST    0x0
/// SW_SET_PROF_30 field reset value
#define NXMAC_SW_SET_PROF_30_RST    0x0
/// SW_SET_PROF_29 field reset value
#define NXMAC_SW_SET_PROF_29_RST    0x0
/// SW_SET_PROF_28 field reset value
#define NXMAC_SW_SET_PROF_28_RST    0x0
/// SW_SET_PROF_27 field reset value
#define NXMAC_SW_SET_PROF_27_RST    0x0
/// SW_SET_PROF_26 field reset value
#define NXMAC_SW_SET_PROF_26_RST    0x0
/// SW_SET_PROF_25 field reset value
#define NXMAC_SW_SET_PROF_25_RST    0x0
/// SW_SET_PROF_24 field reset value
#define NXMAC_SW_SET_PROF_24_RST    0x0
/// SW_SET_PROF_23 field reset value
#define NXMAC_SW_SET_PROF_23_RST    0x0
/// SW_SET_PROF_22 field reset value
#define NXMAC_SW_SET_PROF_22_RST    0x0
/// SW_SET_PROF_21 field reset value
#define NXMAC_SW_SET_PROF_21_RST    0x0
/// SW_SET_PROF_20 field reset value
#define NXMAC_SW_SET_PROF_20_RST    0x0
/// SW_SET_PROF_19 field reset value
#define NXMAC_SW_SET_PROF_19_RST    0x0
/// SW_SET_PROF_18 field reset value
#define NXMAC_SW_SET_PROF_18_RST    0x0
/// SW_SET_PROF_17 field reset value
#define NXMAC_SW_SET_PROF_17_RST    0x0
/// SW_SET_PROF_16 field reset value
#define NXMAC_SW_SET_PROF_16_RST    0x0
/// SW_SET_PROF_15 field reset value
#define NXMAC_SW_SET_PROF_15_RST    0x0
/// SW_SET_PROF_14 field reset value
#define NXMAC_SW_SET_PROF_14_RST    0x0
/// SW_SET_PROF_13 field reset value
#define NXMAC_SW_SET_PROF_13_RST    0x0
/// SW_SET_PROF_12 field reset value
#define NXMAC_SW_SET_PROF_12_RST    0x0
/// SW_SET_PROF_11 field reset value
#define NXMAC_SW_SET_PROF_11_RST    0x0
/// SW_SET_PROF_10 field reset value
#define NXMAC_SW_SET_PROF_10_RST    0x0
/// SW_SET_PROF_9 field reset value
#define NXMAC_SW_SET_PROF_9_RST     0x0
/// SW_SET_PROF_8 field reset value
#define NXMAC_SW_SET_PROF_8_RST     0x0
/// SW_SET_PROF_7 field reset value
#define NXMAC_SW_SET_PROF_7_RST     0x0
/// SW_SET_PROF_6 field reset value
#define NXMAC_SW_SET_PROF_6_RST     0x0
/// SW_SET_PROF_5 field reset value
#define NXMAC_SW_SET_PROF_5_RST     0x0
/// SW_SET_PROF_4 field reset value
#define NXMAC_SW_SET_PROF_4_RST     0x0
/// SW_SET_PROF_3 field reset value
#define NXMAC_SW_SET_PROF_3_RST     0x0
/// SW_SET_PROF_2 field reset value
#define NXMAC_SW_SET_PROF_2_RST     0x0
/// SW_SET_PROF_1 field reset value
#define NXMAC_SW_SET_PROF_1_RST     0x0
/// SW_SET_PROF_0 field reset value
#define NXMAC_SW_SET_PROF_0_RST     0x0

/**
 * @brief Constructs a value for the SW_SET_PROFILING register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] swsetprof31 - The value to use for the swSetProf31 field.
 * @param[in] swsetprof30 - The value to use for the swSetProf30 field.
 * @param[in] swsetprof29 - The value to use for the swSetProf29 field.
 * @param[in] swsetprof28 - The value to use for the swSetProf28 field.
 * @param[in] swsetprof27 - The value to use for the swSetProf27 field.
 * @param[in] swsetprof26 - The value to use for the swSetProf26 field.
 * @param[in] swsetprof25 - The value to use for the swSetProf25 field.
 * @param[in] swsetprof24 - The value to use for the swSetProf24 field.
 * @param[in] swsetprof23 - The value to use for the swSetProf23 field.
 * @param[in] swsetprof22 - The value to use for the swSetProf22 field.
 * @param[in] swsetprof21 - The value to use for the swSetProf21 field.
 * @param[in] swsetprof20 - The value to use for the swSetProf20 field.
 * @param[in] swsetprof19 - The value to use for the swSetProf19 field.
 * @param[in] swsetprof18 - The value to use for the swSetProf18 field.
 * @param[in] swsetprof17 - The value to use for the swSetProf17 field.
 * @param[in] swsetprof16 - The value to use for the swSetProf16 field.
 * @param[in] swsetprof15 - The value to use for the swSetProf15 field.
 * @param[in] swsetprof14 - The value to use for the swSetProf14 field.
 * @param[in] swsetprof13 - The value to use for the swSetProf13 field.
 * @param[in] swsetprof12 - The value to use for the swSetProf12 field.
 * @param[in] swsetprof11 - The value to use for the swSetProf11 field.
 * @param[in] swsetprof10 - The value to use for the swSetProf10 field.
 * @param[in] swsetprof9 - The value to use for the swSetProf9 field.
 * @param[in] swsetprof8 - The value to use for the swSetProf8 field.
 * @param[in] swsetprof7 - The value to use for the swSetProf7 field.
 * @param[in] swsetprof6 - The value to use for the swSetProf6 field.
 * @param[in] swsetprof5 - The value to use for the swSetProf5 field.
 * @param[in] swsetprof4 - The value to use for the swSetProf4 field.
 * @param[in] swsetprof3 - The value to use for the swSetProf3 field.
 * @param[in] swsetprof2 - The value to use for the swSetProf2 field.
 * @param[in] swsetprof1 - The value to use for the swSetProf1 field.
 * @param[in] swsetprof0 - The value to use for the swSetProf0 field.
 */
__INLINE void nxmac_sw_set_profiling_pack(uint8_t swsetprof31, uint8_t swsetprof30, uint8_t swsetprof29, uint8_t swsetprof28, uint8_t swsetprof27, uint8_t swsetprof26, uint8_t swsetprof25, uint8_t swsetprof24, uint8_t swsetprof23, uint8_t swsetprof22, uint8_t swsetprof21, uint8_t swsetprof20, uint8_t swsetprof19, uint8_t swsetprof18, uint8_t swsetprof17, uint8_t swsetprof16, uint8_t swsetprof15, uint8_t swsetprof14, uint8_t swsetprof13, uint8_t swsetprof12, uint8_t swsetprof11, uint8_t swsetprof10, uint8_t swsetprof9, uint8_t swsetprof8, uint8_t swsetprof7, uint8_t swsetprof6, uint8_t swsetprof5, uint8_t swsetprof4, uint8_t swsetprof3, uint8_t swsetprof2, uint8_t swsetprof1, uint8_t swsetprof0)
{
    ASSERT_ERR((((uint32_t)swsetprof31 << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof30 << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof29 << 29) & ~((uint32_t)0x20000000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof28 << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof27 << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof26 << 26) & ~((uint32_t)0x04000000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof25 << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof24 << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof23 << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof22 << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof21 << 21) & ~((uint32_t)0x00200000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof20 << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof19 << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof18 << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof17 << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof16 << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof15 << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof14 << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof13 << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof12 << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof11 << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof10 << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof9 << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof8 << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof7 << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof6 << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof5 << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof4 << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof3 << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof2 << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof1 << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)swsetprof0 << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR,  ((uint32_t)swsetprof31 << 31) | ((uint32_t)swsetprof30 << 30) | ((uint32_t)swsetprof29 << 29) | ((uint32_t)swsetprof28 << 28) | ((uint32_t)swsetprof27 << 27) | ((uint32_t)swsetprof26 << 26) | ((uint32_t)swsetprof25 << 25) | ((uint32_t)swsetprof24 << 24) | ((uint32_t)swsetprof23 << 23) | ((uint32_t)swsetprof22 << 22) | ((uint32_t)swsetprof21 << 21) | ((uint32_t)swsetprof20 << 20) | ((uint32_t)swsetprof19 << 19) | ((uint32_t)swsetprof18 << 18) | ((uint32_t)swsetprof17 << 17) | ((uint32_t)swsetprof16 << 16) | ((uint32_t)swsetprof15 << 15) | ((uint32_t)swsetprof14 << 14) | ((uint32_t)swsetprof13 << 13) | ((uint32_t)swsetprof12 << 12) | ((uint32_t)swsetprof11 << 11) | ((uint32_t)swsetprof10 << 10) | ((uint32_t)swsetprof9 << 9) | ((uint32_t)swsetprof8 << 8) | ((uint32_t)swsetprof7 << 7) | ((uint32_t)swsetprof6 << 6) | ((uint32_t)swsetprof5 << 5) | ((uint32_t)swsetprof4 << 4) | ((uint32_t)swsetprof3 << 3) | ((uint32_t)swsetprof2 << 2) | ((uint32_t)swsetprof1 << 1) | ((uint32_t)swsetprof0 << 0));
}

/**
 * @brief Unpacks SW_SET_PROFILING's fields from current value of the SW_SET_PROFILING register.
 *
 * Reads the SW_SET_PROFILING register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] swsetprof31 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof30 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof29 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof28 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof27 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof26 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof25 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof24 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof23 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof22 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof21 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof20 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof19 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof18 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof17 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof16 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof15 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof14 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof13 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof12 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof11 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof10 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof9 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof8 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof7 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof6 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof5 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof4 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof3 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof2 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof1 - Will be populated with the current value of this field from the register.
 * @param[out] swsetprof0 - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_sw_set_profiling_unpack(uint8_t *swsetprof31, uint8_t *swsetprof30, uint8_t *swsetprof29, uint8_t *swsetprof28, uint8_t *swsetprof27, uint8_t *swsetprof26, uint8_t *swsetprof25, uint8_t *swsetprof24, uint8_t *swsetprof23, uint8_t *swsetprof22, uint8_t *swsetprof21, uint8_t *swsetprof20, uint8_t *swsetprof19, uint8_t *swsetprof18, uint8_t *swsetprof17, uint8_t *swsetprof16, uint8_t *swsetprof15, uint8_t *swsetprof14, uint8_t *swsetprof13, uint8_t *swsetprof12, uint8_t *swsetprof11, uint8_t *swsetprof10, uint8_t *swsetprof9, uint8_t *swsetprof8, uint8_t *swsetprof7, uint8_t *swsetprof6, uint8_t *swsetprof5, uint8_t *swsetprof4, uint8_t *swsetprof3, uint8_t *swsetprof2, uint8_t *swsetprof1, uint8_t *swsetprof0)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);

    *swsetprof31 = (localVal & ((uint32_t)0x80000000)) >> 31;
    *swsetprof30 = (localVal & ((uint32_t)0x40000000)) >> 30;
    *swsetprof29 = (localVal & ((uint32_t)0x20000000)) >> 29;
    *swsetprof28 = (localVal & ((uint32_t)0x10000000)) >> 28;
    *swsetprof27 = (localVal & ((uint32_t)0x08000000)) >> 27;
    *swsetprof26 = (localVal & ((uint32_t)0x04000000)) >> 26;
    *swsetprof25 = (localVal & ((uint32_t)0x02000000)) >> 25;
    *swsetprof24 = (localVal & ((uint32_t)0x01000000)) >> 24;
    *swsetprof23 = (localVal & ((uint32_t)0x00800000)) >> 23;
    *swsetprof22 = (localVal & ((uint32_t)0x00400000)) >> 22;
    *swsetprof21 = (localVal & ((uint32_t)0x00200000)) >> 21;
    *swsetprof20 = (localVal & ((uint32_t)0x00100000)) >> 20;
    *swsetprof19 = (localVal & ((uint32_t)0x00080000)) >> 19;
    *swsetprof18 = (localVal & ((uint32_t)0x00040000)) >> 18;
    *swsetprof17 = (localVal & ((uint32_t)0x00020000)) >> 17;
    *swsetprof16 = (localVal & ((uint32_t)0x00010000)) >> 16;
    *swsetprof15 = (localVal & ((uint32_t)0x00008000)) >> 15;
    *swsetprof14 = (localVal & ((uint32_t)0x00004000)) >> 14;
    *swsetprof13 = (localVal & ((uint32_t)0x00002000)) >> 13;
    *swsetprof12 = (localVal & ((uint32_t)0x00001000)) >> 12;
    *swsetprof11 = (localVal & ((uint32_t)0x00000800)) >> 11;
    *swsetprof10 = (localVal & ((uint32_t)0x00000400)) >> 10;
    *swsetprof9 = (localVal & ((uint32_t)0x00000200)) >> 9;
    *swsetprof8 = (localVal & ((uint32_t)0x00000100)) >> 8;
    *swsetprof7 = (localVal & ((uint32_t)0x00000080)) >> 7;
    *swsetprof6 = (localVal & ((uint32_t)0x00000040)) >> 6;
    *swsetprof5 = (localVal & ((uint32_t)0x00000020)) >> 5;
    *swsetprof4 = (localVal & ((uint32_t)0x00000010)) >> 4;
    *swsetprof3 = (localVal & ((uint32_t)0x00000008)) >> 3;
    *swsetprof2 = (localVal & ((uint32_t)0x00000004)) >> 2;
    *swsetprof1 = (localVal & ((uint32_t)0x00000002)) >> 1;
    *swsetprof0 = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the swSetProf31 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf31 field's value will be returned.
 *
 * @return The current value of the swSetProf31 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_31_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the swSetProf31 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof31 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_31_setf(uint8_t swsetprof31)
{
    ASSERT_ERR((((uint32_t)swsetprof31 << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof31 << 31);
}

/**
 * @brief Returns the current value of the swSetProf30 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf30 field's value will be returned.
 *
 * @return The current value of the swSetProf30 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_30_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

/**
 * @brief Sets the swSetProf30 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof30 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_30_setf(uint8_t swsetprof30)
{
    ASSERT_ERR((((uint32_t)swsetprof30 << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof30 << 30);
}

/**
 * @brief Returns the current value of the swSetProf29 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf29 field's value will be returned.
 *
 * @return The current value of the swSetProf29 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_29_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

/**
 * @brief Sets the swSetProf29 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof29 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_29_setf(uint8_t swsetprof29)
{
    ASSERT_ERR((((uint32_t)swsetprof29 << 29) & ~((uint32_t)0x20000000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof29 << 29);
}

/**
 * @brief Returns the current value of the swSetProf28 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf28 field's value will be returned.
 *
 * @return The current value of the swSetProf28 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_28_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the swSetProf28 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof28 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_28_setf(uint8_t swsetprof28)
{
    ASSERT_ERR((((uint32_t)swsetprof28 << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof28 << 28);
}

/**
 * @brief Returns the current value of the swSetProf27 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf27 field's value will be returned.
 *
 * @return The current value of the swSetProf27 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_27_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Sets the swSetProf27 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof27 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_27_setf(uint8_t swsetprof27)
{
    ASSERT_ERR((((uint32_t)swsetprof27 << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof27 << 27);
}

/**
 * @brief Returns the current value of the swSetProf26 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf26 field's value will be returned.
 *
 * @return The current value of the swSetProf26 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_26_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

/**
 * @brief Sets the swSetProf26 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof26 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_26_setf(uint8_t swsetprof26)
{
    ASSERT_ERR((((uint32_t)swsetprof26 << 26) & ~((uint32_t)0x04000000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof26 << 26);
}

/**
 * @brief Returns the current value of the swSetProf25 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf25 field's value will be returned.
 *
 * @return The current value of the swSetProf25 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_25_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

/**
 * @brief Sets the swSetProf25 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof25 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_25_setf(uint8_t swsetprof25)
{
    ASSERT_ERR((((uint32_t)swsetprof25 << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof25 << 25);
}

/**
 * @brief Returns the current value of the swSetProf24 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf24 field's value will be returned.
 *
 * @return The current value of the swSetProf24 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_24_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the swSetProf24 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof24 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_24_setf(uint8_t swsetprof24)
{
    ASSERT_ERR((((uint32_t)swsetprof24 << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof24 << 24);
}

/**
 * @brief Returns the current value of the swSetProf23 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf23 field's value will be returned.
 *
 * @return The current value of the swSetProf23 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_23_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Sets the swSetProf23 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof23 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_23_setf(uint8_t swsetprof23)
{
    ASSERT_ERR((((uint32_t)swsetprof23 << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof23 << 23);
}

/**
 * @brief Returns the current value of the swSetProf22 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf22 field's value will be returned.
 *
 * @return The current value of the swSetProf22 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_22_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Sets the swSetProf22 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof22 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_22_setf(uint8_t swsetprof22)
{
    ASSERT_ERR((((uint32_t)swsetprof22 << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof22 << 22);
}

/**
 * @brief Returns the current value of the swSetProf21 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf21 field's value will be returned.
 *
 * @return The current value of the swSetProf21 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_21_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Sets the swSetProf21 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof21 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_21_setf(uint8_t swsetprof21)
{
    ASSERT_ERR((((uint32_t)swsetprof21 << 21) & ~((uint32_t)0x00200000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof21 << 21);
}

/**
 * @brief Returns the current value of the swSetProf20 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf20 field's value will be returned.
 *
 * @return The current value of the swSetProf20 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_20_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Sets the swSetProf20 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof20 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_20_setf(uint8_t swsetprof20)
{
    ASSERT_ERR((((uint32_t)swsetprof20 << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof20 << 20);
}

/**
 * @brief Returns the current value of the swSetProf19 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf19 field's value will be returned.
 *
 * @return The current value of the swSetProf19 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_19_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Sets the swSetProf19 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof19 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_19_setf(uint8_t swsetprof19)
{
    ASSERT_ERR((((uint32_t)swsetprof19 << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof19 << 19);
}

/**
 * @brief Returns the current value of the swSetProf18 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf18 field's value will be returned.
 *
 * @return The current value of the swSetProf18 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_18_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Sets the swSetProf18 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof18 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_18_setf(uint8_t swsetprof18)
{
    ASSERT_ERR((((uint32_t)swsetprof18 << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof18 << 18);
}

/**
 * @brief Returns the current value of the swSetProf17 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf17 field's value will be returned.
 *
 * @return The current value of the swSetProf17 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_17_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Sets the swSetProf17 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof17 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_17_setf(uint8_t swsetprof17)
{
    ASSERT_ERR((((uint32_t)swsetprof17 << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof17 << 17);
}

/**
 * @brief Returns the current value of the swSetProf16 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf16 field's value will be returned.
 *
 * @return The current value of the swSetProf16 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_16_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the swSetProf16 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof16 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_16_setf(uint8_t swsetprof16)
{
    ASSERT_ERR((((uint32_t)swsetprof16 << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof16 << 16);
}

/**
 * @brief Returns the current value of the swSetProf15 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf15 field's value will be returned.
 *
 * @return The current value of the swSetProf15 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_15_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

/**
 * @brief Sets the swSetProf15 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof15 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_15_setf(uint8_t swsetprof15)
{
    ASSERT_ERR((((uint32_t)swsetprof15 << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof15 << 15);
}

/**
 * @brief Returns the current value of the swSetProf14 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf14 field's value will be returned.
 *
 * @return The current value of the swSetProf14 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_14_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Sets the swSetProf14 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof14 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_14_setf(uint8_t swsetprof14)
{
    ASSERT_ERR((((uint32_t)swsetprof14 << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof14 << 14);
}

/**
 * @brief Returns the current value of the swSetProf13 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf13 field's value will be returned.
 *
 * @return The current value of the swSetProf13 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_13_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the swSetProf13 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof13 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_13_setf(uint8_t swsetprof13)
{
    ASSERT_ERR((((uint32_t)swsetprof13 << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof13 << 13);
}

/**
 * @brief Returns the current value of the swSetProf12 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf12 field's value will be returned.
 *
 * @return The current value of the swSetProf12 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the swSetProf12 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof12 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_12_setf(uint8_t swsetprof12)
{
    ASSERT_ERR((((uint32_t)swsetprof12 << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof12 << 12);
}

/**
 * @brief Returns the current value of the swSetProf11 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf11 field's value will be returned.
 *
 * @return The current value of the swSetProf11 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the swSetProf11 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof11 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_11_setf(uint8_t swsetprof11)
{
    ASSERT_ERR((((uint32_t)swsetprof11 << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof11 << 11);
}

/**
 * @brief Returns the current value of the swSetProf10 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf10 field's value will be returned.
 *
 * @return The current value of the swSetProf10 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the swSetProf10 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof10 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_10_setf(uint8_t swsetprof10)
{
    ASSERT_ERR((((uint32_t)swsetprof10 << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof10 << 10);
}

/**
 * @brief Returns the current value of the swSetProf9 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf9 field's value will be returned.
 *
 * @return The current value of the swSetProf9 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the swSetProf9 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof9 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_9_setf(uint8_t swsetprof9)
{
    ASSERT_ERR((((uint32_t)swsetprof9 << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof9 << 9);
}

/**
 * @brief Returns the current value of the swSetProf8 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf8 field's value will be returned.
 *
 * @return The current value of the swSetProf8 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the swSetProf8 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof8 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_8_setf(uint8_t swsetprof8)
{
    ASSERT_ERR((((uint32_t)swsetprof8 << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof8 << 8);
}

/**
 * @brief Returns the current value of the swSetProf7 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf7 field's value will be returned.
 *
 * @return The current value of the swSetProf7 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the swSetProf7 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof7 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_7_setf(uint8_t swsetprof7)
{
    ASSERT_ERR((((uint32_t)swsetprof7 << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof7 << 7);
}

/**
 * @brief Returns the current value of the swSetProf6 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf6 field's value will be returned.
 *
 * @return The current value of the swSetProf6 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the swSetProf6 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof6 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_6_setf(uint8_t swsetprof6)
{
    ASSERT_ERR((((uint32_t)swsetprof6 << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof6 << 6);
}

/**
 * @brief Returns the current value of the swSetProf5 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf5 field's value will be returned.
 *
 * @return The current value of the swSetProf5 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the swSetProf5 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof5 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_5_setf(uint8_t swsetprof5)
{
    ASSERT_ERR((((uint32_t)swsetprof5 << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof5 << 5);
}

/**
 * @brief Returns the current value of the swSetProf4 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf4 field's value will be returned.
 *
 * @return The current value of the swSetProf4 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the swSetProf4 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof4 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_4_setf(uint8_t swsetprof4)
{
    ASSERT_ERR((((uint32_t)swsetprof4 << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof4 << 4);
}

/**
 * @brief Returns the current value of the swSetProf3 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf3 field's value will be returned.
 *
 * @return The current value of the swSetProf3 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the swSetProf3 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof3 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_3_setf(uint8_t swsetprof3)
{
    ASSERT_ERR((((uint32_t)swsetprof3 << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof3 << 3);
}

/**
 * @brief Returns the current value of the swSetProf2 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf2 field's value will be returned.
 *
 * @return The current value of the swSetProf2 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the swSetProf2 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof2 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_2_setf(uint8_t swsetprof2)
{
    ASSERT_ERR((((uint32_t)swsetprof2 << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof2 << 2);
}

/**
 * @brief Returns the current value of the swSetProf1 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf1 field's value will be returned.
 *
 * @return The current value of the swSetProf1 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the swSetProf1 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof1 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_1_setf(uint8_t swsetprof1)
{
    ASSERT_ERR((((uint32_t)swsetprof1 << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof1 << 1);
}

/**
 * @brief Returns the current value of the swSetProf0 field in the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read and the swSetProf0 field's value will be returned.
 *
 * @return The current value of the swSetProf0 field in the SW_SET_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_set_prof_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_SET_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the swSetProf0 field of the SW_SET_PROFILING register.
 *
 * The SW_SET_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swsetprof0 - The value to set the field to.
 */
__INLINE void nxmac_sw_set_prof_0_setf(uint8_t swsetprof0)
{
    ASSERT_ERR((((uint32_t)swsetprof0 << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_SW_SET_PROFILING_ADDR, (uint32_t)swsetprof0 << 0);
}

/// @}

/**
 * @name SW_CLEAR_PROFILING register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31        swClearProf31   0
 *     30        swClearProf30   0
 *     29        swClearProf29   0
 *     28        swClearProf28   0
 *     27        swClearProf27   0
 *     26        swClearProf26   0
 *     25        swClearProf25   0
 *     24        swClearProf24   0
 *     23        swClearProf23   0
 *     22        swClearProf22   0
 *     21        swClearProf21   0
 *     20        swClearProf20   0
 *     19        swClearProf19   0
 *     18        swClearProf18   0
 *     17        swClearProf17   0
 *     16        swClearProf16   0
 *     15        swClearProf15   0
 *     14        swClearProf14   0
 *     13        swClearProf13   0
 *     12        swClearProf12   0
 *     11        swClearProf11   0
 *     10        swClearProf10   0
 *     09         swClearProf9   0
 *     08         swClearProf8   0
 *     07         swClearProf7   0
 *     06         swClearProf6   0
 *     05         swClearProf5   0
 *     04         swClearProf4   0
 *     03         swClearProf3   0
 *     02         swClearProf2   0
 *     01         swClearProf1   0
 *     00         swClearProf0   0
 * </pre>
 *
 * @{
 */

/// Address of the SW_CLEAR_PROFILING register
#define NXMAC_SW_CLEAR_PROFILING_ADDR   0xC0008568
/// Offset of the SW_CLEAR_PROFILING register from the base address
#define NXMAC_SW_CLEAR_PROFILING_OFFSET 0x00000568
/// Index of the SW_CLEAR_PROFILING register
#define NXMAC_SW_CLEAR_PROFILING_INDEX  0x0000015A
/// Reset value of the SW_CLEAR_PROFILING register
#define NXMAC_SW_CLEAR_PROFILING_RESET  0x00000000

/**
 * @brief Returns the current value of the SW_CLEAR_PROFILING register.
 * The SW_CLEAR_PROFILING register will be read and its value returned.
 * @return The current value of the SW_CLEAR_PROFILING register.
 */
__INLINE uint32_t nxmac_sw_clear_profiling_get(void)
{
    return REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
}

/**
 * @brief Sets the SW_CLEAR_PROFILING register to a value.
 * The SW_CLEAR_PROFILING register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_sw_clear_profiling_clear(uint32_t value)
{
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, value);
}

// field definitions
/// SW_CLEAR_PROF_31 field bit
#define NXMAC_SW_CLEAR_PROF_31_BIT    ((uint32_t)0x80000000)
/// SW_CLEAR_PROF_31 field position
#define NXMAC_SW_CLEAR_PROF_31_POS    31
/// SW_CLEAR_PROF_30 field bit
#define NXMAC_SW_CLEAR_PROF_30_BIT    ((uint32_t)0x40000000)
/// SW_CLEAR_PROF_30 field position
#define NXMAC_SW_CLEAR_PROF_30_POS    30
/// SW_CLEAR_PROF_29 field bit
#define NXMAC_SW_CLEAR_PROF_29_BIT    ((uint32_t)0x20000000)
/// SW_CLEAR_PROF_29 field position
#define NXMAC_SW_CLEAR_PROF_29_POS    29
/// SW_CLEAR_PROF_28 field bit
#define NXMAC_SW_CLEAR_PROF_28_BIT    ((uint32_t)0x10000000)
/// SW_CLEAR_PROF_28 field position
#define NXMAC_SW_CLEAR_PROF_28_POS    28
/// SW_CLEAR_PROF_27 field bit
#define NXMAC_SW_CLEAR_PROF_27_BIT    ((uint32_t)0x08000000)
/// SW_CLEAR_PROF_27 field position
#define NXMAC_SW_CLEAR_PROF_27_POS    27
/// SW_CLEAR_PROF_26 field bit
#define NXMAC_SW_CLEAR_PROF_26_BIT    ((uint32_t)0x04000000)
/// SW_CLEAR_PROF_26 field position
#define NXMAC_SW_CLEAR_PROF_26_POS    26
/// SW_CLEAR_PROF_25 field bit
#define NXMAC_SW_CLEAR_PROF_25_BIT    ((uint32_t)0x02000000)
/// SW_CLEAR_PROF_25 field position
#define NXMAC_SW_CLEAR_PROF_25_POS    25
/// SW_CLEAR_PROF_24 field bit
#define NXMAC_SW_CLEAR_PROF_24_BIT    ((uint32_t)0x01000000)
/// SW_CLEAR_PROF_24 field position
#define NXMAC_SW_CLEAR_PROF_24_POS    24
/// SW_CLEAR_PROF_23 field bit
#define NXMAC_SW_CLEAR_PROF_23_BIT    ((uint32_t)0x00800000)
/// SW_CLEAR_PROF_23 field position
#define NXMAC_SW_CLEAR_PROF_23_POS    23
/// SW_CLEAR_PROF_22 field bit
#define NXMAC_SW_CLEAR_PROF_22_BIT    ((uint32_t)0x00400000)
/// SW_CLEAR_PROF_22 field position
#define NXMAC_SW_CLEAR_PROF_22_POS    22
/// SW_CLEAR_PROF_21 field bit
#define NXMAC_SW_CLEAR_PROF_21_BIT    ((uint32_t)0x00200000)
/// SW_CLEAR_PROF_21 field position
#define NXMAC_SW_CLEAR_PROF_21_POS    21
/// SW_CLEAR_PROF_20 field bit
#define NXMAC_SW_CLEAR_PROF_20_BIT    ((uint32_t)0x00100000)
/// SW_CLEAR_PROF_20 field position
#define NXMAC_SW_CLEAR_PROF_20_POS    20
/// SW_CLEAR_PROF_19 field bit
#define NXMAC_SW_CLEAR_PROF_19_BIT    ((uint32_t)0x00080000)
/// SW_CLEAR_PROF_19 field position
#define NXMAC_SW_CLEAR_PROF_19_POS    19
/// SW_CLEAR_PROF_18 field bit
#define NXMAC_SW_CLEAR_PROF_18_BIT    ((uint32_t)0x00040000)
/// SW_CLEAR_PROF_18 field position
#define NXMAC_SW_CLEAR_PROF_18_POS    18
/// SW_CLEAR_PROF_17 field bit
#define NXMAC_SW_CLEAR_PROF_17_BIT    ((uint32_t)0x00020000)
/// SW_CLEAR_PROF_17 field position
#define NXMAC_SW_CLEAR_PROF_17_POS    17
/// SW_CLEAR_PROF_16 field bit
#define NXMAC_SW_CLEAR_PROF_16_BIT    ((uint32_t)0x00010000)
/// SW_CLEAR_PROF_16 field position
#define NXMAC_SW_CLEAR_PROF_16_POS    16
/// SW_CLEAR_PROF_15 field bit
#define NXMAC_SW_CLEAR_PROF_15_BIT    ((uint32_t)0x00008000)
/// SW_CLEAR_PROF_15 field position
#define NXMAC_SW_CLEAR_PROF_15_POS    15
/// SW_CLEAR_PROF_14 field bit
#define NXMAC_SW_CLEAR_PROF_14_BIT    ((uint32_t)0x00004000)
/// SW_CLEAR_PROF_14 field position
#define NXMAC_SW_CLEAR_PROF_14_POS    14
/// SW_CLEAR_PROF_13 field bit
#define NXMAC_SW_CLEAR_PROF_13_BIT    ((uint32_t)0x00002000)
/// SW_CLEAR_PROF_13 field position
#define NXMAC_SW_CLEAR_PROF_13_POS    13
/// SW_CLEAR_PROF_12 field bit
#define NXMAC_SW_CLEAR_PROF_12_BIT    ((uint32_t)0x00001000)
/// SW_CLEAR_PROF_12 field position
#define NXMAC_SW_CLEAR_PROF_12_POS    12
/// SW_CLEAR_PROF_11 field bit
#define NXMAC_SW_CLEAR_PROF_11_BIT    ((uint32_t)0x00000800)
/// SW_CLEAR_PROF_11 field position
#define NXMAC_SW_CLEAR_PROF_11_POS    11
/// SW_CLEAR_PROF_10 field bit
#define NXMAC_SW_CLEAR_PROF_10_BIT    ((uint32_t)0x00000400)
/// SW_CLEAR_PROF_10 field position
#define NXMAC_SW_CLEAR_PROF_10_POS    10
/// SW_CLEAR_PROF_9 field bit
#define NXMAC_SW_CLEAR_PROF_9_BIT     ((uint32_t)0x00000200)
/// SW_CLEAR_PROF_9 field position
#define NXMAC_SW_CLEAR_PROF_9_POS     9
/// SW_CLEAR_PROF_8 field bit
#define NXMAC_SW_CLEAR_PROF_8_BIT     ((uint32_t)0x00000100)
/// SW_CLEAR_PROF_8 field position
#define NXMAC_SW_CLEAR_PROF_8_POS     8
/// SW_CLEAR_PROF_7 field bit
#define NXMAC_SW_CLEAR_PROF_7_BIT     ((uint32_t)0x00000080)
/// SW_CLEAR_PROF_7 field position
#define NXMAC_SW_CLEAR_PROF_7_POS     7
/// SW_CLEAR_PROF_6 field bit
#define NXMAC_SW_CLEAR_PROF_6_BIT     ((uint32_t)0x00000040)
/// SW_CLEAR_PROF_6 field position
#define NXMAC_SW_CLEAR_PROF_6_POS     6
/// SW_CLEAR_PROF_5 field bit
#define NXMAC_SW_CLEAR_PROF_5_BIT     ((uint32_t)0x00000020)
/// SW_CLEAR_PROF_5 field position
#define NXMAC_SW_CLEAR_PROF_5_POS     5
/// SW_CLEAR_PROF_4 field bit
#define NXMAC_SW_CLEAR_PROF_4_BIT     ((uint32_t)0x00000010)
/// SW_CLEAR_PROF_4 field position
#define NXMAC_SW_CLEAR_PROF_4_POS     4
/// SW_CLEAR_PROF_3 field bit
#define NXMAC_SW_CLEAR_PROF_3_BIT     ((uint32_t)0x00000008)
/// SW_CLEAR_PROF_3 field position
#define NXMAC_SW_CLEAR_PROF_3_POS     3
/// SW_CLEAR_PROF_2 field bit
#define NXMAC_SW_CLEAR_PROF_2_BIT     ((uint32_t)0x00000004)
/// SW_CLEAR_PROF_2 field position
#define NXMAC_SW_CLEAR_PROF_2_POS     2
/// SW_CLEAR_PROF_1 field bit
#define NXMAC_SW_CLEAR_PROF_1_BIT     ((uint32_t)0x00000002)
/// SW_CLEAR_PROF_1 field position
#define NXMAC_SW_CLEAR_PROF_1_POS     1
/// SW_CLEAR_PROF_0 field bit
#define NXMAC_SW_CLEAR_PROF_0_BIT     ((uint32_t)0x00000001)
/// SW_CLEAR_PROF_0 field position
#define NXMAC_SW_CLEAR_PROF_0_POS     0

/// SW_CLEAR_PROF_31 field reset value
#define NXMAC_SW_CLEAR_PROF_31_RST    0x0
/// SW_CLEAR_PROF_30 field reset value
#define NXMAC_SW_CLEAR_PROF_30_RST    0x0
/// SW_CLEAR_PROF_29 field reset value
#define NXMAC_SW_CLEAR_PROF_29_RST    0x0
/// SW_CLEAR_PROF_28 field reset value
#define NXMAC_SW_CLEAR_PROF_28_RST    0x0
/// SW_CLEAR_PROF_27 field reset value
#define NXMAC_SW_CLEAR_PROF_27_RST    0x0
/// SW_CLEAR_PROF_26 field reset value
#define NXMAC_SW_CLEAR_PROF_26_RST    0x0
/// SW_CLEAR_PROF_25 field reset value
#define NXMAC_SW_CLEAR_PROF_25_RST    0x0
/// SW_CLEAR_PROF_24 field reset value
#define NXMAC_SW_CLEAR_PROF_24_RST    0x0
/// SW_CLEAR_PROF_23 field reset value
#define NXMAC_SW_CLEAR_PROF_23_RST    0x0
/// SW_CLEAR_PROF_22 field reset value
#define NXMAC_SW_CLEAR_PROF_22_RST    0x0
/// SW_CLEAR_PROF_21 field reset value
#define NXMAC_SW_CLEAR_PROF_21_RST    0x0
/// SW_CLEAR_PROF_20 field reset value
#define NXMAC_SW_CLEAR_PROF_20_RST    0x0
/// SW_CLEAR_PROF_19 field reset value
#define NXMAC_SW_CLEAR_PROF_19_RST    0x0
/// SW_CLEAR_PROF_18 field reset value
#define NXMAC_SW_CLEAR_PROF_18_RST    0x0
/// SW_CLEAR_PROF_17 field reset value
#define NXMAC_SW_CLEAR_PROF_17_RST    0x0
/// SW_CLEAR_PROF_16 field reset value
#define NXMAC_SW_CLEAR_PROF_16_RST    0x0
/// SW_CLEAR_PROF_15 field reset value
#define NXMAC_SW_CLEAR_PROF_15_RST    0x0
/// SW_CLEAR_PROF_14 field reset value
#define NXMAC_SW_CLEAR_PROF_14_RST    0x0
/// SW_CLEAR_PROF_13 field reset value
#define NXMAC_SW_CLEAR_PROF_13_RST    0x0
/// SW_CLEAR_PROF_12 field reset value
#define NXMAC_SW_CLEAR_PROF_12_RST    0x0
/// SW_CLEAR_PROF_11 field reset value
#define NXMAC_SW_CLEAR_PROF_11_RST    0x0
/// SW_CLEAR_PROF_10 field reset value
#define NXMAC_SW_CLEAR_PROF_10_RST    0x0
/// SW_CLEAR_PROF_9 field reset value
#define NXMAC_SW_CLEAR_PROF_9_RST     0x0
/// SW_CLEAR_PROF_8 field reset value
#define NXMAC_SW_CLEAR_PROF_8_RST     0x0
/// SW_CLEAR_PROF_7 field reset value
#define NXMAC_SW_CLEAR_PROF_7_RST     0x0
/// SW_CLEAR_PROF_6 field reset value
#define NXMAC_SW_CLEAR_PROF_6_RST     0x0
/// SW_CLEAR_PROF_5 field reset value
#define NXMAC_SW_CLEAR_PROF_5_RST     0x0
/// SW_CLEAR_PROF_4 field reset value
#define NXMAC_SW_CLEAR_PROF_4_RST     0x0
/// SW_CLEAR_PROF_3 field reset value
#define NXMAC_SW_CLEAR_PROF_3_RST     0x0
/// SW_CLEAR_PROF_2 field reset value
#define NXMAC_SW_CLEAR_PROF_2_RST     0x0
/// SW_CLEAR_PROF_1 field reset value
#define NXMAC_SW_CLEAR_PROF_1_RST     0x0
/// SW_CLEAR_PROF_0 field reset value
#define NXMAC_SW_CLEAR_PROF_0_RST     0x0

/**
 * @brief Constructs a value for the SW_CLEAR_PROFILING register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] swclearprof31 - The value to use for the swClearProf31 field.
 * @param[in] swclearprof30 - The value to use for the swClearProf30 field.
 * @param[in] swclearprof29 - The value to use for the swClearProf29 field.
 * @param[in] swclearprof28 - The value to use for the swClearProf28 field.
 * @param[in] swclearprof27 - The value to use for the swClearProf27 field.
 * @param[in] swclearprof26 - The value to use for the swClearProf26 field.
 * @param[in] swclearprof25 - The value to use for the swClearProf25 field.
 * @param[in] swclearprof24 - The value to use for the swClearProf24 field.
 * @param[in] swclearprof23 - The value to use for the swClearProf23 field.
 * @param[in] swclearprof22 - The value to use for the swClearProf22 field.
 * @param[in] swclearprof21 - The value to use for the swClearProf21 field.
 * @param[in] swclearprof20 - The value to use for the swClearProf20 field.
 * @param[in] swclearprof19 - The value to use for the swClearProf19 field.
 * @param[in] swclearprof18 - The value to use for the swClearProf18 field.
 * @param[in] swclearprof17 - The value to use for the swClearProf17 field.
 * @param[in] swclearprof16 - The value to use for the swClearProf16 field.
 * @param[in] swclearprof15 - The value to use for the swClearProf15 field.
 * @param[in] swclearprof14 - The value to use for the swClearProf14 field.
 * @param[in] swclearprof13 - The value to use for the swClearProf13 field.
 * @param[in] swclearprof12 - The value to use for the swClearProf12 field.
 * @param[in] swclearprof11 - The value to use for the swClearProf11 field.
 * @param[in] swclearprof10 - The value to use for the swClearProf10 field.
 * @param[in] swclearprof9 - The value to use for the swClearProf9 field.
 * @param[in] swclearprof8 - The value to use for the swClearProf8 field.
 * @param[in] swclearprof7 - The value to use for the swClearProf7 field.
 * @param[in] swclearprof6 - The value to use for the swClearProf6 field.
 * @param[in] swclearprof5 - The value to use for the swClearProf5 field.
 * @param[in] swclearprof4 - The value to use for the swClearProf4 field.
 * @param[in] swclearprof3 - The value to use for the swClearProf3 field.
 * @param[in] swclearprof2 - The value to use for the swClearProf2 field.
 * @param[in] swclearprof1 - The value to use for the swClearProf1 field.
 * @param[in] swclearprof0 - The value to use for the swClearProf0 field.
 */
__INLINE void nxmac_sw_clear_profiling_pack(uint8_t swclearprof31, uint8_t swclearprof30, uint8_t swclearprof29, uint8_t swclearprof28, uint8_t swclearprof27, uint8_t swclearprof26, uint8_t swclearprof25, uint8_t swclearprof24, uint8_t swclearprof23, uint8_t swclearprof22, uint8_t swclearprof21, uint8_t swclearprof20, uint8_t swclearprof19, uint8_t swclearprof18, uint8_t swclearprof17, uint8_t swclearprof16, uint8_t swclearprof15, uint8_t swclearprof14, uint8_t swclearprof13, uint8_t swclearprof12, uint8_t swclearprof11, uint8_t swclearprof10, uint8_t swclearprof9, uint8_t swclearprof8, uint8_t swclearprof7, uint8_t swclearprof6, uint8_t swclearprof5, uint8_t swclearprof4, uint8_t swclearprof3, uint8_t swclearprof2, uint8_t swclearprof1, uint8_t swclearprof0)
{
    ASSERT_ERR((((uint32_t)swclearprof31 << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof30 << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof29 << 29) & ~((uint32_t)0x20000000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof28 << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof27 << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof26 << 26) & ~((uint32_t)0x04000000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof25 << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof24 << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof23 << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof22 << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof21 << 21) & ~((uint32_t)0x00200000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof20 << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof19 << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof18 << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof17 << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof16 << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof15 << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof14 << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof13 << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof12 << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof11 << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof10 << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof9 << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof8 << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof7 << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof6 << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof5 << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof4 << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof3 << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof2 << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof1 << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)swclearprof0 << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR,  ((uint32_t)swclearprof31 << 31) | ((uint32_t)swclearprof30 << 30) | ((uint32_t)swclearprof29 << 29) | ((uint32_t)swclearprof28 << 28) | ((uint32_t)swclearprof27 << 27) | ((uint32_t)swclearprof26 << 26) | ((uint32_t)swclearprof25 << 25) | ((uint32_t)swclearprof24 << 24) | ((uint32_t)swclearprof23 << 23) | ((uint32_t)swclearprof22 << 22) | ((uint32_t)swclearprof21 << 21) | ((uint32_t)swclearprof20 << 20) | ((uint32_t)swclearprof19 << 19) | ((uint32_t)swclearprof18 << 18) | ((uint32_t)swclearprof17 << 17) | ((uint32_t)swclearprof16 << 16) | ((uint32_t)swclearprof15 << 15) | ((uint32_t)swclearprof14 << 14) | ((uint32_t)swclearprof13 << 13) | ((uint32_t)swclearprof12 << 12) | ((uint32_t)swclearprof11 << 11) | ((uint32_t)swclearprof10 << 10) | ((uint32_t)swclearprof9 << 9) | ((uint32_t)swclearprof8 << 8) | ((uint32_t)swclearprof7 << 7) | ((uint32_t)swclearprof6 << 6) | ((uint32_t)swclearprof5 << 5) | ((uint32_t)swclearprof4 << 4) | ((uint32_t)swclearprof3 << 3) | ((uint32_t)swclearprof2 << 2) | ((uint32_t)swclearprof1 << 1) | ((uint32_t)swclearprof0 << 0));
}

/**
 * @brief Unpacks SW_CLEAR_PROFILING's fields from current value of the SW_CLEAR_PROFILING register.
 *
 * Reads the SW_CLEAR_PROFILING register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] swclearprof31 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof30 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof29 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof28 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof27 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof26 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof25 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof24 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof23 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof22 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof21 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof20 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof19 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof18 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof17 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof16 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof15 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof14 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof13 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof12 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof11 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof10 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof9 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof8 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof7 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof6 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof5 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof4 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof3 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof2 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof1 - Will be populated with the current value of this field from the register.
 * @param[out] swclearprof0 - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_sw_clear_profiling_unpack(uint8_t *swclearprof31, uint8_t *swclearprof30, uint8_t *swclearprof29, uint8_t *swclearprof28, uint8_t *swclearprof27, uint8_t *swclearprof26, uint8_t *swclearprof25, uint8_t *swclearprof24, uint8_t *swclearprof23, uint8_t *swclearprof22, uint8_t *swclearprof21, uint8_t *swclearprof20, uint8_t *swclearprof19, uint8_t *swclearprof18, uint8_t *swclearprof17, uint8_t *swclearprof16, uint8_t *swclearprof15, uint8_t *swclearprof14, uint8_t *swclearprof13, uint8_t *swclearprof12, uint8_t *swclearprof11, uint8_t *swclearprof10, uint8_t *swclearprof9, uint8_t *swclearprof8, uint8_t *swclearprof7, uint8_t *swclearprof6, uint8_t *swclearprof5, uint8_t *swclearprof4, uint8_t *swclearprof3, uint8_t *swclearprof2, uint8_t *swclearprof1, uint8_t *swclearprof0)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);

    *swclearprof31 = (localVal & ((uint32_t)0x80000000)) >> 31;
    *swclearprof30 = (localVal & ((uint32_t)0x40000000)) >> 30;
    *swclearprof29 = (localVal & ((uint32_t)0x20000000)) >> 29;
    *swclearprof28 = (localVal & ((uint32_t)0x10000000)) >> 28;
    *swclearprof27 = (localVal & ((uint32_t)0x08000000)) >> 27;
    *swclearprof26 = (localVal & ((uint32_t)0x04000000)) >> 26;
    *swclearprof25 = (localVal & ((uint32_t)0x02000000)) >> 25;
    *swclearprof24 = (localVal & ((uint32_t)0x01000000)) >> 24;
    *swclearprof23 = (localVal & ((uint32_t)0x00800000)) >> 23;
    *swclearprof22 = (localVal & ((uint32_t)0x00400000)) >> 22;
    *swclearprof21 = (localVal & ((uint32_t)0x00200000)) >> 21;
    *swclearprof20 = (localVal & ((uint32_t)0x00100000)) >> 20;
    *swclearprof19 = (localVal & ((uint32_t)0x00080000)) >> 19;
    *swclearprof18 = (localVal & ((uint32_t)0x00040000)) >> 18;
    *swclearprof17 = (localVal & ((uint32_t)0x00020000)) >> 17;
    *swclearprof16 = (localVal & ((uint32_t)0x00010000)) >> 16;
    *swclearprof15 = (localVal & ((uint32_t)0x00008000)) >> 15;
    *swclearprof14 = (localVal & ((uint32_t)0x00004000)) >> 14;
    *swclearprof13 = (localVal & ((uint32_t)0x00002000)) >> 13;
    *swclearprof12 = (localVal & ((uint32_t)0x00001000)) >> 12;
    *swclearprof11 = (localVal & ((uint32_t)0x00000800)) >> 11;
    *swclearprof10 = (localVal & ((uint32_t)0x00000400)) >> 10;
    *swclearprof9 = (localVal & ((uint32_t)0x00000200)) >> 9;
    *swclearprof8 = (localVal & ((uint32_t)0x00000100)) >> 8;
    *swclearprof7 = (localVal & ((uint32_t)0x00000080)) >> 7;
    *swclearprof6 = (localVal & ((uint32_t)0x00000040)) >> 6;
    *swclearprof5 = (localVal & ((uint32_t)0x00000020)) >> 5;
    *swclearprof4 = (localVal & ((uint32_t)0x00000010)) >> 4;
    *swclearprof3 = (localVal & ((uint32_t)0x00000008)) >> 3;
    *swclearprof2 = (localVal & ((uint32_t)0x00000004)) >> 2;
    *swclearprof1 = (localVal & ((uint32_t)0x00000002)) >> 1;
    *swclearprof0 = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the swClearProf31 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf31 field's value will be returned.
 *
 * @return The current value of the swClearProf31 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_31_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the swClearProf31 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof31 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_31_clearf(uint8_t swclearprof31)
{
    ASSERT_ERR((((uint32_t)swclearprof31 << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof31 << 31);
}

/**
 * @brief Returns the current value of the swClearProf30 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf30 field's value will be returned.
 *
 * @return The current value of the swClearProf30 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_30_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

/**
 * @brief Sets the swClearProf30 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof30 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_30_clearf(uint8_t swclearprof30)
{
    ASSERT_ERR((((uint32_t)swclearprof30 << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof30 << 30);
}

/**
 * @brief Returns the current value of the swClearProf29 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf29 field's value will be returned.
 *
 * @return The current value of the swClearProf29 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_29_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

/**
 * @brief Sets the swClearProf29 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof29 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_29_clearf(uint8_t swclearprof29)
{
    ASSERT_ERR((((uint32_t)swclearprof29 << 29) & ~((uint32_t)0x20000000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof29 << 29);
}

/**
 * @brief Returns the current value of the swClearProf28 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf28 field's value will be returned.
 *
 * @return The current value of the swClearProf28 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_28_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the swClearProf28 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof28 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_28_clearf(uint8_t swclearprof28)
{
    ASSERT_ERR((((uint32_t)swclearprof28 << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof28 << 28);
}

/**
 * @brief Returns the current value of the swClearProf27 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf27 field's value will be returned.
 *
 * @return The current value of the swClearProf27 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_27_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Sets the swClearProf27 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof27 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_27_clearf(uint8_t swclearprof27)
{
    ASSERT_ERR((((uint32_t)swclearprof27 << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof27 << 27);
}

/**
 * @brief Returns the current value of the swClearProf26 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf26 field's value will be returned.
 *
 * @return The current value of the swClearProf26 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_26_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

/**
 * @brief Sets the swClearProf26 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof26 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_26_clearf(uint8_t swclearprof26)
{
    ASSERT_ERR((((uint32_t)swclearprof26 << 26) & ~((uint32_t)0x04000000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof26 << 26);
}

/**
 * @brief Returns the current value of the swClearProf25 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf25 field's value will be returned.
 *
 * @return The current value of the swClearProf25 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_25_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

/**
 * @brief Sets the swClearProf25 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof25 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_25_clearf(uint8_t swclearprof25)
{
    ASSERT_ERR((((uint32_t)swclearprof25 << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof25 << 25);
}

/**
 * @brief Returns the current value of the swClearProf24 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf24 field's value will be returned.
 *
 * @return The current value of the swClearProf24 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_24_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the swClearProf24 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof24 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_24_clearf(uint8_t swclearprof24)
{
    ASSERT_ERR((((uint32_t)swclearprof24 << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof24 << 24);
}

/**
 * @brief Returns the current value of the swClearProf23 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf23 field's value will be returned.
 *
 * @return The current value of the swClearProf23 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_23_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Sets the swClearProf23 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof23 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_23_clearf(uint8_t swclearprof23)
{
    ASSERT_ERR((((uint32_t)swclearprof23 << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof23 << 23);
}

/**
 * @brief Returns the current value of the swClearProf22 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf22 field's value will be returned.
 *
 * @return The current value of the swClearProf22 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_22_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Sets the swClearProf22 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof22 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_22_clearf(uint8_t swclearprof22)
{
    ASSERT_ERR((((uint32_t)swclearprof22 << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof22 << 22);
}

/**
 * @brief Returns the current value of the swClearProf21 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf21 field's value will be returned.
 *
 * @return The current value of the swClearProf21 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_21_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Sets the swClearProf21 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof21 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_21_clearf(uint8_t swclearprof21)
{
    ASSERT_ERR((((uint32_t)swclearprof21 << 21) & ~((uint32_t)0x00200000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof21 << 21);
}

/**
 * @brief Returns the current value of the swClearProf20 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf20 field's value will be returned.
 *
 * @return The current value of the swClearProf20 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_20_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Sets the swClearProf20 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof20 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_20_clearf(uint8_t swclearprof20)
{
    ASSERT_ERR((((uint32_t)swclearprof20 << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof20 << 20);
}

/**
 * @brief Returns the current value of the swClearProf19 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf19 field's value will be returned.
 *
 * @return The current value of the swClearProf19 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_19_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Sets the swClearProf19 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof19 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_19_clearf(uint8_t swclearprof19)
{
    ASSERT_ERR((((uint32_t)swclearprof19 << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof19 << 19);
}

/**
 * @brief Returns the current value of the swClearProf18 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf18 field's value will be returned.
 *
 * @return The current value of the swClearProf18 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_18_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Sets the swClearProf18 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof18 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_18_clearf(uint8_t swclearprof18)
{
    ASSERT_ERR((((uint32_t)swclearprof18 << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof18 << 18);
}

/**
 * @brief Returns the current value of the swClearProf17 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf17 field's value will be returned.
 *
 * @return The current value of the swClearProf17 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_17_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Sets the swClearProf17 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof17 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_17_clearf(uint8_t swclearprof17)
{
    ASSERT_ERR((((uint32_t)swclearprof17 << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof17 << 17);
}

/**
 * @brief Returns the current value of the swClearProf16 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf16 field's value will be returned.
 *
 * @return The current value of the swClearProf16 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_16_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the swClearProf16 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof16 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_16_clearf(uint8_t swclearprof16)
{
    ASSERT_ERR((((uint32_t)swclearprof16 << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof16 << 16);
}

/**
 * @brief Returns the current value of the swClearProf15 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf15 field's value will be returned.
 *
 * @return The current value of the swClearProf15 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_15_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

/**
 * @brief Sets the swClearProf15 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof15 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_15_clearf(uint8_t swclearprof15)
{
    ASSERT_ERR((((uint32_t)swclearprof15 << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof15 << 15);
}

/**
 * @brief Returns the current value of the swClearProf14 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf14 field's value will be returned.
 *
 * @return The current value of the swClearProf14 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_14_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Sets the swClearProf14 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof14 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_14_clearf(uint8_t swclearprof14)
{
    ASSERT_ERR((((uint32_t)swclearprof14 << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof14 << 14);
}

/**
 * @brief Returns the current value of the swClearProf13 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf13 field's value will be returned.
 *
 * @return The current value of the swClearProf13 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_13_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the swClearProf13 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof13 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_13_clearf(uint8_t swclearprof13)
{
    ASSERT_ERR((((uint32_t)swclearprof13 << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof13 << 13);
}

/**
 * @brief Returns the current value of the swClearProf12 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf12 field's value will be returned.
 *
 * @return The current value of the swClearProf12 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the swClearProf12 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof12 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_12_clearf(uint8_t swclearprof12)
{
    ASSERT_ERR((((uint32_t)swclearprof12 << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof12 << 12);
}

/**
 * @brief Returns the current value of the swClearProf11 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf11 field's value will be returned.
 *
 * @return The current value of the swClearProf11 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the swClearProf11 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof11 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_11_clearf(uint8_t swclearprof11)
{
    ASSERT_ERR((((uint32_t)swclearprof11 << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof11 << 11);
}

/**
 * @brief Returns the current value of the swClearProf10 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf10 field's value will be returned.
 *
 * @return The current value of the swClearProf10 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the swClearProf10 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof10 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_10_clearf(uint8_t swclearprof10)
{
    ASSERT_ERR((((uint32_t)swclearprof10 << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof10 << 10);
}

/**
 * @brief Returns the current value of the swClearProf9 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf9 field's value will be returned.
 *
 * @return The current value of the swClearProf9 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the swClearProf9 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof9 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_9_clearf(uint8_t swclearprof9)
{
    ASSERT_ERR((((uint32_t)swclearprof9 << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof9 << 9);
}

/**
 * @brief Returns the current value of the swClearProf8 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf8 field's value will be returned.
 *
 * @return The current value of the swClearProf8 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the swClearProf8 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof8 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_8_clearf(uint8_t swclearprof8)
{
    ASSERT_ERR((((uint32_t)swclearprof8 << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof8 << 8);
}

/**
 * @brief Returns the current value of the swClearProf7 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf7 field's value will be returned.
 *
 * @return The current value of the swClearProf7 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the swClearProf7 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof7 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_7_clearf(uint8_t swclearprof7)
{
    ASSERT_ERR((((uint32_t)swclearprof7 << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof7 << 7);
}

/**
 * @brief Returns the current value of the swClearProf6 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf6 field's value will be returned.
 *
 * @return The current value of the swClearProf6 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the swClearProf6 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof6 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_6_clearf(uint8_t swclearprof6)
{
    ASSERT_ERR((((uint32_t)swclearprof6 << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof6 << 6);
}

/**
 * @brief Returns the current value of the swClearProf5 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf5 field's value will be returned.
 *
 * @return The current value of the swClearProf5 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the swClearProf5 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof5 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_5_clearf(uint8_t swclearprof5)
{
    ASSERT_ERR((((uint32_t)swclearprof5 << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof5 << 5);
}

/**
 * @brief Returns the current value of the swClearProf4 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf4 field's value will be returned.
 *
 * @return The current value of the swClearProf4 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the swClearProf4 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof4 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_4_clearf(uint8_t swclearprof4)
{
    ASSERT_ERR((((uint32_t)swclearprof4 << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof4 << 4);
}

/**
 * @brief Returns the current value of the swClearProf3 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf3 field's value will be returned.
 *
 * @return The current value of the swClearProf3 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the swClearProf3 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof3 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_3_clearf(uint8_t swclearprof3)
{
    ASSERT_ERR((((uint32_t)swclearprof3 << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof3 << 3);
}

/**
 * @brief Returns the current value of the swClearProf2 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf2 field's value will be returned.
 *
 * @return The current value of the swClearProf2 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the swClearProf2 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof2 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_2_clearf(uint8_t swclearprof2)
{
    ASSERT_ERR((((uint32_t)swclearprof2 << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof2 << 2);
}

/**
 * @brief Returns the current value of the swClearProf1 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf1 field's value will be returned.
 *
 * @return The current value of the swClearProf1 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the swClearProf1 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof1 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_1_clearf(uint8_t swclearprof1)
{
    ASSERT_ERR((((uint32_t)swclearprof1 << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof1 << 1);
}

/**
 * @brief Returns the current value of the swClearProf0 field in the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read and the swClearProf0 field's value will be returned.
 *
 * @return The current value of the swClearProf0 field in the SW_CLEAR_PROFILING register.
 */
__INLINE uint8_t nxmac_sw_clear_prof_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SW_CLEAR_PROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the swClearProf0 field of the SW_CLEAR_PROFILING register.
 *
 * The SW_CLEAR_PROFILING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] swclearprof0 - The value to set the field to.
 */
__INLINE void nxmac_sw_clear_prof_0_clearf(uint8_t swclearprof0)
{
    ASSERT_ERR((((uint32_t)swclearprof0 << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_SW_CLEAR_PROFILING_ADDR, (uint32_t)swclearprof0 << 0);
}

/// @}

#if RW_MUMIMO_SEC_USER1_EN
/**
 * @name DEBUG_SEC_U_1_TX_C_PTR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00   secU1TxCurrentPointer   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_SEC_U_1_TX_C_PTR register
#define NXMAC_DEBUG_SEC_U_1_TX_C_PTR_ADDR   0xC0008570
/// Offset of the DEBUG_SEC_U_1_TX_C_PTR register from the base address
#define NXMAC_DEBUG_SEC_U_1_TX_C_PTR_OFFSET 0x00000570
/// Index of the DEBUG_SEC_U_1_TX_C_PTR register
#define NXMAC_DEBUG_SEC_U_1_TX_C_PTR_INDEX  0x0000015C
/// Reset value of the DEBUG_SEC_U_1_TX_C_PTR register
#define NXMAC_DEBUG_SEC_U_1_TX_C_PTR_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_SEC_U_1_TX_C_PTR register.
 * The DEBUG_SEC_U_1_TX_C_PTR register will be read and its value returned.
 * @return The current value of the DEBUG_SEC_U_1_TX_C_PTR register.
 */
__INLINE uint32_t nxmac_debug_sec_u_1_tx_c_ptr_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_SEC_U_1_TX_C_PTR_ADDR);
}

// field definitions
/// SEC_U_1_TX_CURRENT_POINTER field mask
#define NXMAC_SEC_U_1_TX_CURRENT_POINTER_MASK   ((uint32_t)0xFFFFFFFF)
/// SEC_U_1_TX_CURRENT_POINTER field LSB position
#define NXMAC_SEC_U_1_TX_CURRENT_POINTER_LSB    0
/// SEC_U_1_TX_CURRENT_POINTER field width
#define NXMAC_SEC_U_1_TX_CURRENT_POINTER_WIDTH  ((uint32_t)0x00000020)

/// SEC_U_1_TX_CURRENT_POINTER field reset value
#define NXMAC_SEC_U_1_TX_CURRENT_POINTER_RST    0x0

/**
 * @brief Returns the current value of the secU1TxCurrentPointer field in the DEBUG_SEC_U_1_TX_C_PTR register.
 *
 * The DEBUG_SEC_U_1_TX_C_PTR register will be read and the secU1TxCurrentPointer field's value will be returned.
 *
 * @return The current value of the secU1TxCurrentPointer field in the DEBUG_SEC_U_1_TX_C_PTR register.
 */
__INLINE uint32_t nxmac_sec_u_1_tx_current_pointer_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_SEC_U_1_TX_C_PTR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

#endif // RW_MUMIMO_SEC_USER1_EN 
/// @}

#if RW_MUMIMO_SEC_USER2_EN
/**
 * @name DEBUG_SEC_U_2_TX_C_PTR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00   secU2TxCurrentPointer   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_SEC_U_2_TX_C_PTR register
#define NXMAC_DEBUG_SEC_U_2_TX_C_PTR_ADDR   0xC0008574
/// Offset of the DEBUG_SEC_U_2_TX_C_PTR register from the base address
#define NXMAC_DEBUG_SEC_U_2_TX_C_PTR_OFFSET 0x00000574
/// Index of the DEBUG_SEC_U_2_TX_C_PTR register
#define NXMAC_DEBUG_SEC_U_2_TX_C_PTR_INDEX  0x0000015D
/// Reset value of the DEBUG_SEC_U_2_TX_C_PTR register
#define NXMAC_DEBUG_SEC_U_2_TX_C_PTR_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_SEC_U_2_TX_C_PTR register.
 * The DEBUG_SEC_U_2_TX_C_PTR register will be read and its value returned.
 * @return The current value of the DEBUG_SEC_U_2_TX_C_PTR register.
 */
__INLINE uint32_t nxmac_debug_sec_u_2_tx_c_ptr_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_SEC_U_2_TX_C_PTR_ADDR);
}

// field definitions
/// SEC_U_2_TX_CURRENT_POINTER field mask
#define NXMAC_SEC_U_2_TX_CURRENT_POINTER_MASK   ((uint32_t)0xFFFFFFFF)
/// SEC_U_2_TX_CURRENT_POINTER field LSB position
#define NXMAC_SEC_U_2_TX_CURRENT_POINTER_LSB    0
/// SEC_U_2_TX_CURRENT_POINTER field width
#define NXMAC_SEC_U_2_TX_CURRENT_POINTER_WIDTH  ((uint32_t)0x00000020)

/// SEC_U_2_TX_CURRENT_POINTER field reset value
#define NXMAC_SEC_U_2_TX_CURRENT_POINTER_RST    0x0

/**
 * @brief Returns the current value of the secU2TxCurrentPointer field in the DEBUG_SEC_U_2_TX_C_PTR register.
 *
 * The DEBUG_SEC_U_2_TX_C_PTR register will be read and the secU2TxCurrentPointer field's value will be returned.
 *
 * @return The current value of the secU2TxCurrentPointer field in the DEBUG_SEC_U_2_TX_C_PTR register.
 */
__INLINE uint32_t nxmac_sec_u_2_tx_current_pointer_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_SEC_U_2_TX_C_PTR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

#endif // RW_MUMIMO_SEC_USER2_EN 
/// @}

#if RW_MUMIMO_SEC_USER3_EN
/**
 * @name DEBUG_SEC_U_3_TX_C_PTR register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00   secU3TxCurrentPointer   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_SEC_U_3_TX_C_PTR register
#define NXMAC_DEBUG_SEC_U_3_TX_C_PTR_ADDR   0xC0008578
/// Offset of the DEBUG_SEC_U_3_TX_C_PTR register from the base address
#define NXMAC_DEBUG_SEC_U_3_TX_C_PTR_OFFSET 0x00000578
/// Index of the DEBUG_SEC_U_3_TX_C_PTR register
#define NXMAC_DEBUG_SEC_U_3_TX_C_PTR_INDEX  0x0000015E
/// Reset value of the DEBUG_SEC_U_3_TX_C_PTR register
#define NXMAC_DEBUG_SEC_U_3_TX_C_PTR_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_SEC_U_3_TX_C_PTR register.
 * The DEBUG_SEC_U_3_TX_C_PTR register will be read and its value returned.
 * @return The current value of the DEBUG_SEC_U_3_TX_C_PTR register.
 */
__INLINE uint32_t nxmac_debug_sec_u_3_tx_c_ptr_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_SEC_U_3_TX_C_PTR_ADDR);
}

// field definitions
/// SEC_U_3_TX_CURRENT_POINTER field mask
#define NXMAC_SEC_U_3_TX_CURRENT_POINTER_MASK   ((uint32_t)0xFFFFFFFF)
/// SEC_U_3_TX_CURRENT_POINTER field LSB position
#define NXMAC_SEC_U_3_TX_CURRENT_POINTER_LSB    0
/// SEC_U_3_TX_CURRENT_POINTER field width
#define NXMAC_SEC_U_3_TX_CURRENT_POINTER_WIDTH  ((uint32_t)0x00000020)

/// SEC_U_3_TX_CURRENT_POINTER field reset value
#define NXMAC_SEC_U_3_TX_CURRENT_POINTER_RST    0x0

/**
 * @brief Returns the current value of the secU3TxCurrentPointer field in the DEBUG_SEC_U_3_TX_C_PTR register.
 *
 * The DEBUG_SEC_U_3_TX_C_PTR register will be read and the secU3TxCurrentPointer field's value will be returned.
 *
 * @return The current value of the secU3TxCurrentPointer field in the DEBUG_SEC_U_3_TX_C_PTR register.
 */
__INLINE uint32_t nxmac_sec_u_3_tx_current_pointer_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_SEC_U_3_TX_C_PTR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

#endif // RW_MUMIMO_SEC_USER3_EN 
/// @}
/*lint +e91*/

#endif // _REG_MAC_PL_H_

/// @}

