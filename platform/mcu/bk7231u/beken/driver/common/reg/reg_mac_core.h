/**
 * @file reg_mac_core.h
 * @brief Definitions of the NXMAC HW block registers and register access functions.
 *
 * @defgroup REG_MAC_CORE REG_MAC_CORE
 * @ingroup REG
 * @{
 *
 * @brief Definitions of the NXMAC HW block registers and register access functions.
 */
#ifndef _REG_MAC_CORE_H_
#define _REG_MAC_CORE_H_

#include "co_int.h"
#include "_reg_mac_core.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"

/*lint -e91 */
/** @brief Number of registers in the REG_MAC_CORE peripheral.
 */
#define REG_MAC_CORE_COUNT 344

/** @brief Decoding mask of the REG_MAC_CORE peripheral registers from the CPU point of view.
 */
#define REG_MAC_CORE_DECODING_MASK 0x000007FF

/**
 * @name SIGNATURE register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            signature   0x0
 * </pre>
 *
 * @{
 */

/// Address of the SIGNATURE register
#define NXMAC_SIGNATURE_ADDR   0xC0000000
/// Offset of the SIGNATURE register from the base address
#define NXMAC_SIGNATURE_OFFSET 0x00000000
/// Index of the SIGNATURE register
#define NXMAC_SIGNATURE_INDEX  0x00000000
/// Reset value of the SIGNATURE register
#define NXMAC_SIGNATURE_RESET  0x00000000

/**
 * @brief Returns the current value of the SIGNATURE register.
 * The SIGNATURE register will be read and its value returned.
 * @return The current value of the SIGNATURE register.
 */
__INLINE uint32_t nxmac_signature_get(void)
{
    return REG_PL_RD(NXMAC_SIGNATURE_ADDR);
}

// field definitions
/// SIGNATURE field mask
#define NXMAC_SIGNATURE_MASK   ((uint32_t)0xFFFFFFFF)
/// SIGNATURE field LSB position
#define NXMAC_SIGNATURE_LSB    0
/// SIGNATURE field width
#define NXMAC_SIGNATURE_WIDTH  ((uint32_t)0x00000020)

/// SIGNATURE field reset value
#define NXMAC_SIGNATURE_RST    0x0

/**
 * @brief Returns the current value of the signature field in the SIGNATURE register.
 *
 * The SIGNATURE register will be read and the signature field's value will be returned.
 *
 * @return The current value of the signature field in the SIGNATURE register.
 */
__INLINE uint32_t nxmac_signature_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SIGNATURE_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name VERSION_1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     19             muMIMOTx   0
 *     18                bfmer   0
 *     17                bfmee   0
 *     16     mac80211MHFormat   0
 *     14                 coex   0
 *     13                 wapi   0
 *     12                  tpc   1
 *     11                  vht   1
 *     10                   ht   1
 *     08                  rce   0
 *     07                 ccmp   0
 *     06                 tkip   0
 *     05                  wep   0
 *     04             security   0
 *     03                  sme   0
 *     02                 hcca   0
 *     01                 edca   1
 *     00                  qos   1
 * </pre>
 *
 * @{
 */

/// Address of the VERSION_1 register
#define NXMAC_VERSION_1_ADDR   0xC0000004
/// Offset of the VERSION_1 register from the base address
#define NXMAC_VERSION_1_OFFSET 0x00000004
/// Index of the VERSION_1 register
#define NXMAC_VERSION_1_INDEX  0x00000001
/// Reset value of the VERSION_1 register
#define NXMAC_VERSION_1_RESET  0x00001C03

/**
 * @brief Returns the current value of the VERSION_1 register.
 * The VERSION_1 register will be read and its value returned.
 * @return The current value of the VERSION_1 register.
 */
__INLINE uint32_t nxmac_version_1_get(void)
{
    return REG_PL_RD(NXMAC_VERSION_1_ADDR);
}

// field definitions
/// MU_MIMO_TX field bit
#define NXMAC_MU_MIMO_TX_BIT            ((uint32_t)0x00080000)
/// MU_MIMO_TX field position
#define NXMAC_MU_MIMO_TX_POS            19
/// BFMER field bit
#define NXMAC_BFMER_BIT                 ((uint32_t)0x00040000)
/// BFMER field position
#define NXMAC_BFMER_POS                 18
/// BFMEE field bit
#define NXMAC_BFMEE_BIT                 ((uint32_t)0x00020000)
/// BFMEE field position
#define NXMAC_BFMEE_POS                 17
/// MAC_80211MH_FORMAT field bit
#define NXMAC_MAC_80211MH_FORMAT_BIT    ((uint32_t)0x00010000)
/// MAC_80211MH_FORMAT field position
#define NXMAC_MAC_80211MH_FORMAT_POS    16
/// COEX field bit
#define NXMAC_COEX_BIT                  ((uint32_t)0x00004000)
/// COEX field position
#define NXMAC_COEX_POS                  14
/// WAPI field bit
#define NXMAC_WAPI_BIT                  ((uint32_t)0x00002000)
/// WAPI field position
#define NXMAC_WAPI_POS                  13
/// TPC field bit
#define NXMAC_TPC_BIT                   ((uint32_t)0x00001000)
/// TPC field position
#define NXMAC_TPC_POS                   12
/// VHT field bit
#define NXMAC_VHT_BIT                   ((uint32_t)0x00000800)
/// VHT field position
#define NXMAC_VHT_POS                   11
/// HT field bit
#define NXMAC_HT_BIT                    ((uint32_t)0x00000400)
/// HT field position
#define NXMAC_HT_POS                    10
/// RCE field bit
#define NXMAC_RCE_BIT                   ((uint32_t)0x00000100)
/// RCE field position
#define NXMAC_RCE_POS                   8
/// CCMP field bit
#define NXMAC_CCMP_BIT                  ((uint32_t)0x00000080)
/// CCMP field position
#define NXMAC_CCMP_POS                  7
/// TKIP field bit
#define NXMAC_TKIP_BIT                  ((uint32_t)0x00000040)
/// TKIP field position
#define NXMAC_TKIP_POS                  6
/// WEP field bit
#define NXMAC_WEP_BIT                   ((uint32_t)0x00000020)
/// WEP field position
#define NXMAC_WEP_POS                   5
/// SECURITY field bit
#define NXMAC_SECURITY_BIT              ((uint32_t)0x00000010)
/// SECURITY field position
#define NXMAC_SECURITY_POS              4
/// SME field bit
#define NXMAC_SME_BIT                   ((uint32_t)0x00000008)
/// SME field position
#define NXMAC_SME_POS                   3
/// HCCA field bit
#define NXMAC_HCCA_BIT                  ((uint32_t)0x00000004)
/// HCCA field position
#define NXMAC_HCCA_POS                  2
/// EDCA field bit
#define NXMAC_EDCA_BIT                  ((uint32_t)0x00000002)
/// EDCA field position
#define NXMAC_EDCA_POS                  1
/// QOS field bit
#define NXMAC_QOS_BIT                   ((uint32_t)0x00000001)
/// QOS field position
#define NXMAC_QOS_POS                   0

/// MU_MIMO_TX field reset value
#define NXMAC_MU_MIMO_TX_RST            0x0
/// BFMER field reset value
#define NXMAC_BFMER_RST                 0x0
/// BFMEE field reset value
#define NXMAC_BFMEE_RST                 0x0
/// MAC_80211MH_FORMAT field reset value
#define NXMAC_MAC_80211MH_FORMAT_RST    0x0
/// COEX field reset value
#define NXMAC_COEX_RST                  0x0
/// WAPI field reset value
#define NXMAC_WAPI_RST                  0x0
/// TPC field reset value
#define NXMAC_TPC_RST                   0x1
/// VHT field reset value
#define NXMAC_VHT_RST                   0x1
/// HT field reset value
#define NXMAC_HT_RST                    0x1
/// RCE field reset value
#define NXMAC_RCE_RST                   0x0
/// CCMP field reset value
#define NXMAC_CCMP_RST                  0x0
/// TKIP field reset value
#define NXMAC_TKIP_RST                  0x0
/// WEP field reset value
#define NXMAC_WEP_RST                   0x0
/// SECURITY field reset value
#define NXMAC_SECURITY_RST              0x0
/// SME field reset value
#define NXMAC_SME_RST                   0x0
/// HCCA field reset value
#define NXMAC_HCCA_RST                  0x0
/// EDCA field reset value
#define NXMAC_EDCA_RST                  0x1
/// QOS field reset value
#define NXMAC_QOS_RST                   0x1

/**
 * @brief Unpacks VERSION_1's fields from current value of the VERSION_1 register.
 *
 * Reads the VERSION_1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] mumimotx - Will be populated with the current value of this field from the register.
 * @param[out] bfmer - Will be populated with the current value of this field from the register.
 * @param[out] bfmee - Will be populated with the current value of this field from the register.
 * @param[out] mac80211mhformat - Will be populated with the current value of this field from the register.
 * @param[out] coex - Will be populated with the current value of this field from the register.
 * @param[out] wapi - Will be populated with the current value of this field from the register.
 * @param[out] tpc - Will be populated with the current value of this field from the register.
 * @param[out] vht - Will be populated with the current value of this field from the register.
 * @param[out] ht - Will be populated with the current value of this field from the register.
 * @param[out] rce - Will be populated with the current value of this field from the register.
 * @param[out] ccmp - Will be populated with the current value of this field from the register.
 * @param[out] tkip - Will be populated with the current value of this field from the register.
 * @param[out] wep - Will be populated with the current value of this field from the register.
 * @param[out] security - Will be populated with the current value of this field from the register.
 * @param[out] sme - Will be populated with the current value of this field from the register.
 * @param[out] hcca - Will be populated with the current value of this field from the register.
 * @param[out] edca - Will be populated with the current value of this field from the register.
 * @param[out] qos - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_version_1_unpack(uint8_t *mumimotx, uint8_t *bfmer, uint8_t *bfmee, uint8_t *mac80211mhformat, uint8_t *coex, uint8_t *wapi, uint8_t *tpc, uint8_t *vht, uint8_t *ht, uint8_t *rce, uint8_t *ccmp, uint8_t *tkip, uint8_t *wep, uint8_t *security, uint8_t *sme, uint8_t *hcca, uint8_t *edca, uint8_t *qos)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_1_ADDR);

    *mumimotx = (localVal & ((uint32_t)0x00080000)) >> 19;
    *bfmer = (localVal & ((uint32_t)0x00040000)) >> 18;
    *bfmee = (localVal & ((uint32_t)0x00020000)) >> 17;
    *mac80211mhformat = (localVal & ((uint32_t)0x00010000)) >> 16;
    *coex = (localVal & ((uint32_t)0x00004000)) >> 14;
    *wapi = (localVal & ((uint32_t)0x00002000)) >> 13;
    *tpc = (localVal & ((uint32_t)0x00001000)) >> 12;
    *vht = (localVal & ((uint32_t)0x00000800)) >> 11;
    *ht = (localVal & ((uint32_t)0x00000400)) >> 10;
    *rce = (localVal & ((uint32_t)0x00000100)) >> 8;
    *ccmp = (localVal & ((uint32_t)0x00000080)) >> 7;
    *tkip = (localVal & ((uint32_t)0x00000040)) >> 6;
    *wep = (localVal & ((uint32_t)0x00000020)) >> 5;
    *security = (localVal & ((uint32_t)0x00000010)) >> 4;
    *sme = (localVal & ((uint32_t)0x00000008)) >> 3;
    *hcca = (localVal & ((uint32_t)0x00000004)) >> 2;
    *edca = (localVal & ((uint32_t)0x00000002)) >> 1;
    *qos = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the muMIMOTx field in the VERSION_1 register.
 *
 * The VERSION_1 register will be read and the muMIMOTx field's value will be returned.
 *
 * @return The current value of the muMIMOTx field in the VERSION_1 register.
 */
__INLINE uint8_t nxmac_mu_mimo_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_1_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Returns the current value of the bfmer field in the VERSION_1 register.
 *
 * The VERSION_1 register will be read and the bfmer field's value will be returned.
 *
 * @return The current value of the bfmer field in the VERSION_1 register.
 */
__INLINE uint8_t nxmac_bfmer_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_1_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Returns the current value of the bfmee field in the VERSION_1 register.
 *
 * The VERSION_1 register will be read and the bfmee field's value will be returned.
 *
 * @return The current value of the bfmee field in the VERSION_1 register.
 */
__INLINE uint8_t nxmac_bfmee_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_1_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Returns the current value of the mac80211MHFormat field in the VERSION_1 register.
 *
 * The VERSION_1 register will be read and the mac80211MHFormat field's value will be returned.
 *
 * @return The current value of the mac80211MHFormat field in the VERSION_1 register.
 */
__INLINE uint8_t nxmac_mac_80211mh_format_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_1_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Returns the current value of the coex field in the VERSION_1 register.
 *
 * The VERSION_1 register will be read and the coex field's value will be returned.
 *
 * @return The current value of the coex field in the VERSION_1 register.
 */
__INLINE uint8_t nxmac_coex_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_1_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Returns the current value of the wapi field in the VERSION_1 register.
 *
 * The VERSION_1 register will be read and the wapi field's value will be returned.
 *
 * @return The current value of the wapi field in the VERSION_1 register.
 */
__INLINE uint8_t nxmac_wapi_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_1_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Returns the current value of the tpc field in the VERSION_1 register.
 *
 * The VERSION_1 register will be read and the tpc field's value will be returned.
 *
 * @return The current value of the tpc field in the VERSION_1 register.
 */
__INLINE uint8_t nxmac_tpc_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_1_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Returns the current value of the vht field in the VERSION_1 register.
 *
 * The VERSION_1 register will be read and the vht field's value will be returned.
 *
 * @return The current value of the vht field in the VERSION_1 register.
 */
__INLINE uint8_t nxmac_vht_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_1_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Returns the current value of the ht field in the VERSION_1 register.
 *
 * The VERSION_1 register will be read and the ht field's value will be returned.
 *
 * @return The current value of the ht field in the VERSION_1 register.
 */
__INLINE uint8_t nxmac_ht_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_1_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Returns the current value of the rce field in the VERSION_1 register.
 *
 * The VERSION_1 register will be read and the rce field's value will be returned.
 *
 * @return The current value of the rce field in the VERSION_1 register.
 */
__INLINE uint8_t nxmac_rce_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_1_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Returns the current value of the ccmp field in the VERSION_1 register.
 *
 * The VERSION_1 register will be read and the ccmp field's value will be returned.
 *
 * @return The current value of the ccmp field in the VERSION_1 register.
 */
__INLINE uint8_t nxmac_ccmp_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_1_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Returns the current value of the tkip field in the VERSION_1 register.
 *
 * The VERSION_1 register will be read and the tkip field's value will be returned.
 *
 * @return The current value of the tkip field in the VERSION_1 register.
 */
__INLINE uint8_t nxmac_tkip_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_1_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Returns the current value of the wep field in the VERSION_1 register.
 *
 * The VERSION_1 register will be read and the wep field's value will be returned.
 *
 * @return The current value of the wep field in the VERSION_1 register.
 */
__INLINE uint8_t nxmac_wep_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_1_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Returns the current value of the security field in the VERSION_1 register.
 *
 * The VERSION_1 register will be read and the security field's value will be returned.
 *
 * @return The current value of the security field in the VERSION_1 register.
 */
__INLINE uint8_t nxmac_security_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_1_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Returns the current value of the sme field in the VERSION_1 register.
 *
 * The VERSION_1 register will be read and the sme field's value will be returned.
 *
 * @return The current value of the sme field in the VERSION_1 register.
 */
__INLINE uint8_t nxmac_sme_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_1_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Returns the current value of the hcca field in the VERSION_1 register.
 *
 * The VERSION_1 register will be read and the hcca field's value will be returned.
 *
 * @return The current value of the hcca field in the VERSION_1 register.
 */
__INLINE uint8_t nxmac_hcca_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_1_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Returns the current value of the edca field in the VERSION_1 register.
 *
 * The VERSION_1 register will be read and the edca field's value will be returned.
 *
 * @return The current value of the edca field in the VERSION_1 register.
 */
__INLINE uint8_t nxmac_edca_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_1_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Returns the current value of the qos field in the VERSION_1 register.
 *
 * The VERSION_1 register will be read and the qos field's value will be returned.
 *
 * @return The current value of the qos field in the VERSION_1 register.
 */
__INLINE uint8_t nxmac_qos_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_1_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/// @}

/**
 * @name VERSION_2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  16:14          phaseNumber   0x0
 *  13:08        releaseNumber   0x0
 *     07            ieRelease   0
 *  06:00            umVersion   0x0
 * </pre>
 *
 * @{
 */

/// Address of the VERSION_2 register
#define NXMAC_VERSION_2_ADDR   0xC0000008
/// Offset of the VERSION_2 register from the base address
#define NXMAC_VERSION_2_OFFSET 0x00000008
/// Index of the VERSION_2 register
#define NXMAC_VERSION_2_INDEX  0x00000002
/// Reset value of the VERSION_2 register
#define NXMAC_VERSION_2_RESET  0x00000000

/**
 * @brief Returns the current value of the VERSION_2 register.
 * The VERSION_2 register will be read and its value returned.
 * @return The current value of the VERSION_2 register.
 */
__INLINE uint32_t nxmac_version_2_get(void)
{
    return REG_PL_RD(NXMAC_VERSION_2_ADDR);
}

// field definitions
/// PHASE_NUMBER field mask
#define NXMAC_PHASE_NUMBER_MASK     ((uint32_t)0x0001C000)
/// PHASE_NUMBER field LSB position
#define NXMAC_PHASE_NUMBER_LSB      14
/// PHASE_NUMBER field width
#define NXMAC_PHASE_NUMBER_WIDTH    ((uint32_t)0x00000003)
/// RELEASE_NUMBER field mask
#define NXMAC_RELEASE_NUMBER_MASK   ((uint32_t)0x00003F00)
/// RELEASE_NUMBER field LSB position
#define NXMAC_RELEASE_NUMBER_LSB    8
/// RELEASE_NUMBER field width
#define NXMAC_RELEASE_NUMBER_WIDTH  ((uint32_t)0x00000006)
/// IE_RELEASE field bit
#define NXMAC_IE_RELEASE_BIT        ((uint32_t)0x00000080)
/// IE_RELEASE field position
#define NXMAC_IE_RELEASE_POS        7
/// UM_VERSION field mask
#define NXMAC_UM_VERSION_MASK       ((uint32_t)0x0000007F)
/// UM_VERSION field LSB position
#define NXMAC_UM_VERSION_LSB        0
/// UM_VERSION field width
#define NXMAC_UM_VERSION_WIDTH      ((uint32_t)0x00000007)

/// PHASE_NUMBER field reset value
#define NXMAC_PHASE_NUMBER_RST      0x0
/// RELEASE_NUMBER field reset value
#define NXMAC_RELEASE_NUMBER_RST    0x0
/// IE_RELEASE field reset value
#define NXMAC_IE_RELEASE_RST        0x0
/// UM_VERSION field reset value
#define NXMAC_UM_VERSION_RST        0x0

/**
 * @brief Unpacks VERSION_2's fields from current value of the VERSION_2 register.
 *
 * Reads the VERSION_2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] phasenumber - Will be populated with the current value of this field from the register.
 * @param[out] releasenumber - Will be populated with the current value of this field from the register.
 * @param[out] ierelease - Will be populated with the current value of this field from the register.
 * @param[out] umversion - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_version_2_unpack(uint8_t *phasenumber, uint8_t *releasenumber, uint8_t *ierelease, uint8_t *umversion)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_2_ADDR);

    *phasenumber = (localVal & ((uint32_t)0x0001C000)) >> 14;
    *releasenumber = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *ierelease = (localVal & ((uint32_t)0x00000080)) >> 7;
    *umversion = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the phaseNumber field in the VERSION_2 register.
 *
 * The VERSION_2 register will be read and the phaseNumber field's value will be returned.
 *
 * @return The current value of the phaseNumber field in the VERSION_2 register.
 */
__INLINE uint8_t nxmac_phase_number_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_2_ADDR);
    return ((localVal & ((uint32_t)0x0001C000)) >> 14);
}

/**
 * @brief Returns the current value of the releaseNumber field in the VERSION_2 register.
 *
 * The VERSION_2 register will be read and the releaseNumber field's value will be returned.
 *
 * @return The current value of the releaseNumber field in the VERSION_2 register.
 */
__INLINE uint8_t nxmac_release_number_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_2_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Returns the current value of the ieRelease field in the VERSION_2 register.
 *
 * The VERSION_2 register will be read and the ieRelease field's value will be returned.
 *
 * @return The current value of the ieRelease field in the VERSION_2 register.
 */
__INLINE uint8_t nxmac_ie_release_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_2_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Returns the current value of the umVersion field in the VERSION_2 register.
 *
 * The VERSION_2 register will be read and the umVersion field's value will be returned.
 *
 * @return The current value of the umVersion field in the VERSION_2 register.
 */
__INLINE uint8_t nxmac_um_version_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VERSION_2_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/// @}

/**
 * @name BITMAP_CNT register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00            bitmapCnt   0x0
 * </pre>
 *
 * @{
 */

/// Address of the BITMAP_CNT register
#define NXMAC_BITMAP_CNT_ADDR   0xC000000C
/// Offset of the BITMAP_CNT register from the base address
#define NXMAC_BITMAP_CNT_OFFSET 0x0000000C
/// Index of the BITMAP_CNT register
#define NXMAC_BITMAP_CNT_INDEX  0x00000003
/// Reset value of the BITMAP_CNT register
#define NXMAC_BITMAP_CNT_RESET  0x00000000

/**
 * @brief Returns the current value of the BITMAP_CNT register.
 * The BITMAP_CNT register will be read and its value returned.
 * @return The current value of the BITMAP_CNT register.
 */
__INLINE uint32_t nxmac_bitmap_cnt_get(void)
{
    return REG_PL_RD(NXMAC_BITMAP_CNT_ADDR);
}

// field definitions
/// BITMAP_CNT field mask
#define NXMAC_BITMAP_CNT_MASK   ((uint32_t)0x0000FFFF)
/// BITMAP_CNT field LSB position
#define NXMAC_BITMAP_CNT_LSB    0
/// BITMAP_CNT field width
#define NXMAC_BITMAP_CNT_WIDTH  ((uint32_t)0x00000010)

/// BITMAP_CNT field reset value
#define NXMAC_BITMAP_CNT_RST    0x0

/**
 * @brief Returns the current value of the bitmapCnt field in the BITMAP_CNT register.
 *
 * The BITMAP_CNT register will be read and the bitmapCnt field's value will be returned.
 *
 * @return The current value of the bitmapCnt field in the BITMAP_CNT register.
 */
__INLINE uint16_t nxmac_bitmap_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BITMAP_CNT_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name MAC_ADDR_LOW register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00           macAddrLow   0x0
 * </pre>
 *
 * @{
 */

/// Address of the MAC_ADDR_LOW register
#define NXMAC_MAC_ADDR_LOW_ADDR   0xC0000010
/// Offset of the MAC_ADDR_LOW register from the base address
#define NXMAC_MAC_ADDR_LOW_OFFSET 0x00000010
/// Index of the MAC_ADDR_LOW register
#define NXMAC_MAC_ADDR_LOW_INDEX  0x00000004
/// Reset value of the MAC_ADDR_LOW register
#define NXMAC_MAC_ADDR_LOW_RESET  0x00000000

/**
 * @brief Returns the current value of the MAC_ADDR_LOW register.
 * The MAC_ADDR_LOW register will be read and its value returned.
 * @return The current value of the MAC_ADDR_LOW register.
 */
__INLINE uint32_t nxmac_mac_addr_low_get(void)
{
    return REG_PL_RD(NXMAC_MAC_ADDR_LOW_ADDR);
}

/**
 * @brief Sets the MAC_ADDR_LOW register to a value.
 * The MAC_ADDR_LOW register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_mac_addr_low_set(uint32_t value)
{
    REG_PL_WR(NXMAC_MAC_ADDR_LOW_ADDR, value);
}

// field definitions
/// MAC_ADDR_LOW field mask
#define NXMAC_MAC_ADDR_LOW_MASK   ((uint32_t)0xFFFFFFFF)
/// MAC_ADDR_LOW field LSB position
#define NXMAC_MAC_ADDR_LOW_LSB    0
/// MAC_ADDR_LOW field width
#define NXMAC_MAC_ADDR_LOW_WIDTH  ((uint32_t)0x00000020)

/// MAC_ADDR_LOW field reset value
#define NXMAC_MAC_ADDR_LOW_RST    0x0

/**
 * @brief Returns the current value of the macAddrLow field in the MAC_ADDR_LOW register.
 *
 * The MAC_ADDR_LOW register will be read and the macAddrLow field's value will be returned.
 *
 * @return The current value of the macAddrLow field in the MAC_ADDR_LOW register.
 */
__INLINE uint32_t nxmac_mac_addr_low_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ADDR_LOW_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the macAddrLow field of the MAC_ADDR_LOW register.
 *
 * The MAC_ADDR_LOW register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macaddrlow - The value to set the field to.
 */
__INLINE void nxmac_mac_addr_low_setf(uint32_t macaddrlow)
{
    ASSERT_ERR((((uint32_t)macaddrlow << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_MAC_ADDR_LOW_ADDR, (uint32_t)macaddrlow << 0);
}

/// @}

/**
 * @name MAC_ADDR_HI register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00          macAddrHigh   0x0
 * </pre>
 *
 * @{
 */

/// Address of the MAC_ADDR_HI register
#define NXMAC_MAC_ADDR_HI_ADDR   0xC0000014
/// Offset of the MAC_ADDR_HI register from the base address
#define NXMAC_MAC_ADDR_HI_OFFSET 0x00000014
/// Index of the MAC_ADDR_HI register
#define NXMAC_MAC_ADDR_HI_INDEX  0x00000005
/// Reset value of the MAC_ADDR_HI register
#define NXMAC_MAC_ADDR_HI_RESET  0x00000000

/**
 * @brief Returns the current value of the MAC_ADDR_HI register.
 * The MAC_ADDR_HI register will be read and its value returned.
 * @return The current value of the MAC_ADDR_HI register.
 */
__INLINE uint32_t nxmac_mac_addr_hi_get(void)
{
    return REG_PL_RD(NXMAC_MAC_ADDR_HI_ADDR);
}

/**
 * @brief Sets the MAC_ADDR_HI register to a value.
 * The MAC_ADDR_HI register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_mac_addr_hi_set(uint32_t value)
{
    REG_PL_WR(NXMAC_MAC_ADDR_HI_ADDR, value);
}

// field definitions
/// MAC_ADDR_HIGH field mask
#define NXMAC_MAC_ADDR_HIGH_MASK   ((uint32_t)0x0000FFFF)
/// MAC_ADDR_HIGH field LSB position
#define NXMAC_MAC_ADDR_HIGH_LSB    0
/// MAC_ADDR_HIGH field width
#define NXMAC_MAC_ADDR_HIGH_WIDTH  ((uint32_t)0x00000010)

/// MAC_ADDR_HIGH field reset value
#define NXMAC_MAC_ADDR_HIGH_RST    0x0

/**
 * @brief Returns the current value of the macAddrHigh field in the MAC_ADDR_HI register.
 *
 * The MAC_ADDR_HI register will be read and the macAddrHigh field's value will be returned.
 *
 * @return The current value of the macAddrHigh field in the MAC_ADDR_HI register.
 */
__INLINE uint16_t nxmac_mac_addr_high_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ADDR_HI_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the macAddrHigh field of the MAC_ADDR_HI register.
 *
 * The MAC_ADDR_HI register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macaddrhigh - The value to set the field to.
 */
__INLINE void nxmac_mac_addr_high_setf(uint16_t macaddrhigh)
{
    ASSERT_ERR((((uint32_t)macaddrhigh << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_MAC_ADDR_HI_ADDR, (uint32_t)macaddrhigh << 0);
}

/// @}

/**
 * @name MAC_ADDR_LOW_MASK register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00       macAddrLowMask   0x0
 * </pre>
 *
 * @{
 */

/// Address of the MAC_ADDR_LOW_MASK register
#define NXMAC_MAC_ADDR_LOW_MASK_ADDR   0xC0000018
/// Offset of the MAC_ADDR_LOW_MASK register from the base address
#define NXMAC_MAC_ADDR_LOW_MASK_OFFSET 0x00000018
/// Index of the MAC_ADDR_LOW_MASK register
#define NXMAC_MAC_ADDR_LOW_MASK_INDEX  0x00000006
/// Reset value of the MAC_ADDR_LOW_MASK register
#define NXMAC_MAC_ADDR_LOW_MASK_RESET  0x00000000

/**
 * @brief Returns the current value of the MAC_ADDR_LOW_MASK register.
 * The MAC_ADDR_LOW_MASK register will be read and its value returned.
 * @return The current value of the MAC_ADDR_LOW_MASK register.
 */
__INLINE uint32_t nxmac_mac_addr_low_mask_get(void)
{
    return REG_PL_RD(NXMAC_MAC_ADDR_LOW_MASK_ADDR);
}

/**
 * @brief Sets the MAC_ADDR_LOW_MASK register to a value.
 * The MAC_ADDR_LOW_MASK register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_mac_addr_low_mask_set(uint32_t value)
{
    REG_PL_WR(NXMAC_MAC_ADDR_LOW_MASK_ADDR, value);
}

// field definitions
/// MAC_ADDR_LOW_MASK field mask
#define NXMAC_MAC_ADDR_LOW_MASK_MASK   ((uint32_t)0xFFFFFFFF)
/// MAC_ADDR_LOW_MASK field LSB position
#define NXMAC_MAC_ADDR_LOW_MASK_LSB    0
/// MAC_ADDR_LOW_MASK field width
#define NXMAC_MAC_ADDR_LOW_MASK_WIDTH  ((uint32_t)0x00000020)

/// MAC_ADDR_LOW_MASK field reset value
#define NXMAC_MAC_ADDR_LOW_MASK_RST    0x0

/**
 * @brief Returns the current value of the macAddrLowMask field in the MAC_ADDR_LOW_MASK register.
 *
 * The MAC_ADDR_LOW_MASK register will be read and the macAddrLowMask field's value will be returned.
 *
 * @return The current value of the macAddrLowMask field in the MAC_ADDR_LOW_MASK register.
 */
__INLINE uint32_t nxmac_mac_addr_low_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ADDR_LOW_MASK_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the macAddrLowMask field of the MAC_ADDR_LOW_MASK register.
 *
 * The MAC_ADDR_LOW_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macaddrlowmask - The value to set the field to.
 */
__INLINE void nxmac_mac_addr_low_mask_setf(uint32_t macaddrlowmask)
{
    ASSERT_ERR((((uint32_t)macaddrlowmask << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_MAC_ADDR_LOW_MASK_ADDR, (uint32_t)macaddrlowmask << 0);
}

/// @}

/**
 * @name MAC_ADDR_HI_MASK register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00      macAddrHighMask   0x0
 * </pre>
 *
 * @{
 */

/// Address of the MAC_ADDR_HI_MASK register
#define NXMAC_MAC_ADDR_HI_MASK_ADDR   0xC000001C
/// Offset of the MAC_ADDR_HI_MASK register from the base address
#define NXMAC_MAC_ADDR_HI_MASK_OFFSET 0x0000001C
/// Index of the MAC_ADDR_HI_MASK register
#define NXMAC_MAC_ADDR_HI_MASK_INDEX  0x00000007
/// Reset value of the MAC_ADDR_HI_MASK register
#define NXMAC_MAC_ADDR_HI_MASK_RESET  0x00000000

/**
 * @brief Returns the current value of the MAC_ADDR_HI_MASK register.
 * The MAC_ADDR_HI_MASK register will be read and its value returned.
 * @return The current value of the MAC_ADDR_HI_MASK register.
 */
__INLINE uint32_t nxmac_mac_addr_hi_mask_get(void)
{
    return REG_PL_RD(NXMAC_MAC_ADDR_HI_MASK_ADDR);
}

/**
 * @brief Sets the MAC_ADDR_HI_MASK register to a value.
 * The MAC_ADDR_HI_MASK register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_mac_addr_hi_mask_set(uint32_t value)
{
    REG_PL_WR(NXMAC_MAC_ADDR_HI_MASK_ADDR, value);
}

// field definitions
/// MAC_ADDR_HIGH_MASK field mask
#define NXMAC_MAC_ADDR_HIGH_MASK_MASK   ((uint32_t)0x0000FFFF)
/// MAC_ADDR_HIGH_MASK field LSB position
#define NXMAC_MAC_ADDR_HIGH_MASK_LSB    0
/// MAC_ADDR_HIGH_MASK field width
#define NXMAC_MAC_ADDR_HIGH_MASK_WIDTH  ((uint32_t)0x00000010)

/// MAC_ADDR_HIGH_MASK field reset value
#define NXMAC_MAC_ADDR_HIGH_MASK_RST    0x0

/**
 * @brief Returns the current value of the macAddrHighMask field in the MAC_ADDR_HI_MASK register.
 *
 * The MAC_ADDR_HI_MASK register will be read and the macAddrHighMask field's value will be returned.
 *
 * @return The current value of the macAddrHighMask field in the MAC_ADDR_HI_MASK register.
 */
__INLINE uint16_t nxmac_mac_addr_high_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ADDR_HI_MASK_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the macAddrHighMask field of the MAC_ADDR_HI_MASK register.
 *
 * The MAC_ADDR_HI_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macaddrhighmask - The value to set the field to.
 */
__INLINE void nxmac_mac_addr_high_mask_setf(uint16_t macaddrhighmask)
{
    ASSERT_ERR((((uint32_t)macaddrhighmask << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_MAC_ADDR_HI_MASK_ADDR, (uint32_t)macaddrhighmask << 0);
}

/// @}

/**
 * @name BSS_ID_LOW register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00             bssIDLow   0x0
 * </pre>
 *
 * @{
 */

/// Address of the BSS_ID_LOW register
#define NXMAC_BSS_ID_LOW_ADDR   0xC0000020
/// Offset of the BSS_ID_LOW register from the base address
#define NXMAC_BSS_ID_LOW_OFFSET 0x00000020
/// Index of the BSS_ID_LOW register
#define NXMAC_BSS_ID_LOW_INDEX  0x00000008
/// Reset value of the BSS_ID_LOW register
#define NXMAC_BSS_ID_LOW_RESET  0x00000000

/**
 * @brief Returns the current value of the BSS_ID_LOW register.
 * The BSS_ID_LOW register will be read and its value returned.
 * @return The current value of the BSS_ID_LOW register.
 */
__INLINE uint32_t nxmac_bss_id_low_get(void)
{
    return REG_PL_RD(NXMAC_BSS_ID_LOW_ADDR);
}

/**
 * @brief Sets the BSS_ID_LOW register to a value.
 * The BSS_ID_LOW register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_bss_id_low_set(uint32_t value)
{
    REG_PL_WR(NXMAC_BSS_ID_LOW_ADDR, value);
}

// field definitions
/// BSS_ID_LOW field mask
#define NXMAC_BSS_ID_LOW_MASK   ((uint32_t)0xFFFFFFFF)
/// BSS_ID_LOW field LSB position
#define NXMAC_BSS_ID_LOW_LSB    0
/// BSS_ID_LOW field width
#define NXMAC_BSS_ID_LOW_WIDTH  ((uint32_t)0x00000020)

/// BSS_ID_LOW field reset value
#define NXMAC_BSS_ID_LOW_RST    0x0

/**
 * @brief Returns the current value of the bssIDLow field in the BSS_ID_LOW register.
 *
 * The BSS_ID_LOW register will be read and the bssIDLow field's value will be returned.
 *
 * @return The current value of the bssIDLow field in the BSS_ID_LOW register.
 */
__INLINE uint32_t nxmac_bss_id_low_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BSS_ID_LOW_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the bssIDLow field of the BSS_ID_LOW register.
 *
 * The BSS_ID_LOW register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bssidlow - The value to set the field to.
 */
__INLINE void nxmac_bss_id_low_setf(uint32_t bssidlow)
{
    ASSERT_ERR((((uint32_t)bssidlow << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_BSS_ID_LOW_ADDR, (uint32_t)bssidlow << 0);
}

/// @}

/**
 * @name BSS_ID_HI register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00            bssIDHigh   0x0
 * </pre>
 *
 * @{
 */

/// Address of the BSS_ID_HI register
#define NXMAC_BSS_ID_HI_ADDR   0xC0000024
/// Offset of the BSS_ID_HI register from the base address
#define NXMAC_BSS_ID_HI_OFFSET 0x00000024
/// Index of the BSS_ID_HI register
#define NXMAC_BSS_ID_HI_INDEX  0x00000009
/// Reset value of the BSS_ID_HI register
#define NXMAC_BSS_ID_HI_RESET  0x00000000

/**
 * @brief Returns the current value of the BSS_ID_HI register.
 * The BSS_ID_HI register will be read and its value returned.
 * @return The current value of the BSS_ID_HI register.
 */
__INLINE uint32_t nxmac_bss_id_hi_get(void)
{
    return REG_PL_RD(NXMAC_BSS_ID_HI_ADDR);
}

/**
 * @brief Sets the BSS_ID_HI register to a value.
 * The BSS_ID_HI register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_bss_id_hi_set(uint32_t value)
{
    REG_PL_WR(NXMAC_BSS_ID_HI_ADDR, value);
}

// field definitions
/// BSS_ID_HIGH field mask
#define NXMAC_BSS_ID_HIGH_MASK   ((uint32_t)0x0000FFFF)
/// BSS_ID_HIGH field LSB position
#define NXMAC_BSS_ID_HIGH_LSB    0
/// BSS_ID_HIGH field width
#define NXMAC_BSS_ID_HIGH_WIDTH  ((uint32_t)0x00000010)

/// BSS_ID_HIGH field reset value
#define NXMAC_BSS_ID_HIGH_RST    0x0

/**
 * @brief Returns the current value of the bssIDHigh field in the BSS_ID_HI register.
 *
 * The BSS_ID_HI register will be read and the bssIDHigh field's value will be returned.
 *
 * @return The current value of the bssIDHigh field in the BSS_ID_HI register.
 */
__INLINE uint16_t nxmac_bss_id_high_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BSS_ID_HI_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the bssIDHigh field of the BSS_ID_HI register.
 *
 * The BSS_ID_HI register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bssidhigh - The value to set the field to.
 */
__INLINE void nxmac_bss_id_high_setf(uint16_t bssidhigh)
{
    ASSERT_ERR((((uint32_t)bssidhigh << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_BSS_ID_HI_ADDR, (uint32_t)bssidhigh << 0);
}

/// @}

/**
 * @name BSS_ID_LOW_MASK register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00         bssIDLowMask   0x0
 * </pre>
 *
 * @{
 */

/// Address of the BSS_ID_LOW_MASK register
#define NXMAC_BSS_ID_LOW_MASK_ADDR   0xC0000028
/// Offset of the BSS_ID_LOW_MASK register from the base address
#define NXMAC_BSS_ID_LOW_MASK_OFFSET 0x00000028
/// Index of the BSS_ID_LOW_MASK register
#define NXMAC_BSS_ID_LOW_MASK_INDEX  0x0000000A
/// Reset value of the BSS_ID_LOW_MASK register
#define NXMAC_BSS_ID_LOW_MASK_RESET  0x00000000

/**
 * @brief Returns the current value of the BSS_ID_LOW_MASK register.
 * The BSS_ID_LOW_MASK register will be read and its value returned.
 * @return The current value of the BSS_ID_LOW_MASK register.
 */
__INLINE uint32_t nxmac_bss_id_low_mask_get(void)
{
    return REG_PL_RD(NXMAC_BSS_ID_LOW_MASK_ADDR);
}

/**
 * @brief Sets the BSS_ID_LOW_MASK register to a value.
 * The BSS_ID_LOW_MASK register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_bss_id_low_mask_set(uint32_t value)
{
    REG_PL_WR(NXMAC_BSS_ID_LOW_MASK_ADDR, value);
}

// field definitions
/// BSS_ID_LOW_MASK field mask
#define NXMAC_BSS_ID_LOW_MASK_MASK   ((uint32_t)0xFFFFFFFF)
/// BSS_ID_LOW_MASK field LSB position
#define NXMAC_BSS_ID_LOW_MASK_LSB    0
/// BSS_ID_LOW_MASK field width
#define NXMAC_BSS_ID_LOW_MASK_WIDTH  ((uint32_t)0x00000020)

/// BSS_ID_LOW_MASK field reset value
#define NXMAC_BSS_ID_LOW_MASK_RST    0x0

/**
 * @brief Returns the current value of the bssIDLowMask field in the BSS_ID_LOW_MASK register.
 *
 * The BSS_ID_LOW_MASK register will be read and the bssIDLowMask field's value will be returned.
 *
 * @return The current value of the bssIDLowMask field in the BSS_ID_LOW_MASK register.
 */
__INLINE uint32_t nxmac_bss_id_low_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BSS_ID_LOW_MASK_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the bssIDLowMask field of the BSS_ID_LOW_MASK register.
 *
 * The BSS_ID_LOW_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bssidlowmask - The value to set the field to.
 */
__INLINE void nxmac_bss_id_low_mask_setf(uint32_t bssidlowmask)
{
    ASSERT_ERR((((uint32_t)bssidlowmask << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_BSS_ID_LOW_MASK_ADDR, (uint32_t)bssidlowmask << 0);
}

/// @}

/**
 * @name BSS_ID_HI_MASK register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00        bssIDHighMask   0x0
 * </pre>
 *
 * @{
 */

/// Address of the BSS_ID_HI_MASK register
#define NXMAC_BSS_ID_HI_MASK_ADDR   0xC000002C
/// Offset of the BSS_ID_HI_MASK register from the base address
#define NXMAC_BSS_ID_HI_MASK_OFFSET 0x0000002C
/// Index of the BSS_ID_HI_MASK register
#define NXMAC_BSS_ID_HI_MASK_INDEX  0x0000000B
/// Reset value of the BSS_ID_HI_MASK register
#define NXMAC_BSS_ID_HI_MASK_RESET  0x00000000

/**
 * @brief Returns the current value of the BSS_ID_HI_MASK register.
 * The BSS_ID_HI_MASK register will be read and its value returned.
 * @return The current value of the BSS_ID_HI_MASK register.
 */
__INLINE uint32_t nxmac_bss_id_hi_mask_get(void)
{
    return REG_PL_RD(NXMAC_BSS_ID_HI_MASK_ADDR);
}

/**
 * @brief Sets the BSS_ID_HI_MASK register to a value.
 * The BSS_ID_HI_MASK register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_bss_id_hi_mask_set(uint32_t value)
{
    REG_PL_WR(NXMAC_BSS_ID_HI_MASK_ADDR, value);
}

// field definitions
/// BSS_ID_HIGH_MASK field mask
#define NXMAC_BSS_ID_HIGH_MASK_MASK   ((uint32_t)0x0000FFFF)
/// BSS_ID_HIGH_MASK field LSB position
#define NXMAC_BSS_ID_HIGH_MASK_LSB    0
/// BSS_ID_HIGH_MASK field width
#define NXMAC_BSS_ID_HIGH_MASK_WIDTH  ((uint32_t)0x00000010)

/// BSS_ID_HIGH_MASK field reset value
#define NXMAC_BSS_ID_HIGH_MASK_RST    0x0

/**
 * @brief Returns the current value of the bssIDHighMask field in the BSS_ID_HI_MASK register.
 *
 * The BSS_ID_HI_MASK register will be read and the bssIDHighMask field's value will be returned.
 *
 * @return The current value of the bssIDHighMask field in the BSS_ID_HI_MASK register.
 */
__INLINE uint16_t nxmac_bss_id_high_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BSS_ID_HI_MASK_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the bssIDHighMask field of the BSS_ID_HI_MASK register.
 *
 * The BSS_ID_HI_MASK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bssidhighmask - The value to set the field to.
 */
__INLINE void nxmac_bss_id_high_mask_setf(uint16_t bssidhighmask)
{
    ASSERT_ERR((((uint32_t)bssidhighmask << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_BSS_ID_HI_MASK_ADDR, (uint32_t)bssidhighmask << 0);
}

/// @}

/**
 * @name STATE_CNTRL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  07:04            nextState   0x0
 *  03:00         currentState   0x0
 * </pre>
 *
 * @{
 */

/// Address of the STATE_CNTRL register
#define NXMAC_STATE_CNTRL_ADDR   0xC0000038
/// Offset of the STATE_CNTRL register from the base address
#define NXMAC_STATE_CNTRL_OFFSET 0x00000038
/// Index of the STATE_CNTRL register
#define NXMAC_STATE_CNTRL_INDEX  0x0000000E
/// Reset value of the STATE_CNTRL register
#define NXMAC_STATE_CNTRL_RESET  0x00000000

/**
 * @brief Returns the current value of the STATE_CNTRL register.
 * The STATE_CNTRL register will be read and its value returned.
 * @return The current value of the STATE_CNTRL register.
 */
__INLINE uint32_t nxmac_state_cntrl_get(void)
{
    return REG_PL_RD(NXMAC_STATE_CNTRL_ADDR);
}

/**
 * @brief Sets the STATE_CNTRL register to a value.
 * The STATE_CNTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_state_cntrl_set(uint32_t value)
{
    REG_PL_WR(NXMAC_STATE_CNTRL_ADDR, value);
}

// field definitions
/// NEXT_STATE field mask
#define NXMAC_NEXT_STATE_MASK      ((uint32_t)0x000000F0)
/// NEXT_STATE field LSB position
#define NXMAC_NEXT_STATE_LSB       4
/// NEXT_STATE field width
#define NXMAC_NEXT_STATE_WIDTH     ((uint32_t)0x00000004)
/// CURRENT_STATE field mask
#define NXMAC_CURRENT_STATE_MASK   ((uint32_t)0x0000000F)
/// CURRENT_STATE field LSB position
#define NXMAC_CURRENT_STATE_LSB    0
/// CURRENT_STATE field width
#define NXMAC_CURRENT_STATE_WIDTH  ((uint32_t)0x00000004)

/// NEXT_STATE field reset value
#define NXMAC_NEXT_STATE_RST       0x0
/// CURRENT_STATE field reset value
#define NXMAC_CURRENT_STATE_RST    0x0

/**
 * @brief Unpacks STATE_CNTRL's fields from current value of the STATE_CNTRL register.
 *
 * Reads the STATE_CNTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] nextstate - Will be populated with the current value of this field from the register.
 * @param[out] currentstate - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_state_cntrl_unpack(uint8_t *nextstate, uint8_t *currentstate)
{
    uint32_t localVal = REG_PL_RD(NXMAC_STATE_CNTRL_ADDR);

    *nextstate = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *currentstate = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the nextState field in the STATE_CNTRL register.
 *
 * The STATE_CNTRL register will be read and the nextState field's value will be returned.
 *
 * @return The current value of the nextState field in the STATE_CNTRL register.
 */
__INLINE uint8_t nxmac_next_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_STATE_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the nextState field of the STATE_CNTRL register.
 *
 * The STATE_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nextstate - The value to set the field to.
 */
__INLINE void nxmac_next_state_setf(uint8_t nextstate)
{
    ASSERT_ERR((((uint32_t)nextstate << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(NXMAC_STATE_CNTRL_ADDR, (uint32_t)nextstate << 4);
}

/**
 * @brief Returns the current value of the currentState field in the STATE_CNTRL register.
 *
 * The STATE_CNTRL register will be read and the currentState field's value will be returned.
 *
 * @return The current value of the currentState field in the STATE_CNTRL register.
 */
__INLINE uint8_t nxmac_current_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_STATE_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/// @}

/**
 * @name SCAN_CNTRL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00           probeDelay   0x0
 * </pre>
 *
 * @{
 */

/// Address of the SCAN_CNTRL register
#define NXMAC_SCAN_CNTRL_ADDR   0xC000003C
/// Offset of the SCAN_CNTRL register from the base address
#define NXMAC_SCAN_CNTRL_OFFSET 0x0000003C
/// Index of the SCAN_CNTRL register
#define NXMAC_SCAN_CNTRL_INDEX  0x0000000F
/// Reset value of the SCAN_CNTRL register
#define NXMAC_SCAN_CNTRL_RESET  0x00000000

/**
 * @brief Returns the current value of the SCAN_CNTRL register.
 * The SCAN_CNTRL register will be read and its value returned.
 * @return The current value of the SCAN_CNTRL register.
 */
__INLINE uint32_t nxmac_scan_cntrl_get(void)
{
    return REG_PL_RD(NXMAC_SCAN_CNTRL_ADDR);
}

/**
 * @brief Sets the SCAN_CNTRL register to a value.
 * The SCAN_CNTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_scan_cntrl_set(uint32_t value)
{
    REG_PL_WR(NXMAC_SCAN_CNTRL_ADDR, value);
}

// field definitions
/// PROBE_DELAY field mask
#define NXMAC_PROBE_DELAY_MASK   ((uint32_t)0x0000FFFF)
/// PROBE_DELAY field LSB position
#define NXMAC_PROBE_DELAY_LSB    0
/// PROBE_DELAY field width
#define NXMAC_PROBE_DELAY_WIDTH  ((uint32_t)0x00000010)

/// PROBE_DELAY field reset value
#define NXMAC_PROBE_DELAY_RST    0x0

/**
 * @brief Returns the current value of the probeDelay field in the SCAN_CNTRL register.
 *
 * The SCAN_CNTRL register will be read and the probeDelay field's value will be returned.
 *
 * @return The current value of the probeDelay field in the SCAN_CNTRL register.
 */
__INLINE uint16_t nxmac_probe_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_SCAN_CNTRL_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the probeDelay field of the SCAN_CNTRL register.
 *
 * The SCAN_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] probedelay - The value to set the field to.
 */
__INLINE void nxmac_probe_delay_setf(uint16_t probedelay)
{
    ASSERT_ERR((((uint32_t)probedelay << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_SCAN_CNTRL_ADDR, (uint32_t)probedelay << 0);
}

/// @}

/**
 * @name DOZE_CNTRL_1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:17                atimW   0x0
 *     16           wakeupDTIM   0
 *  15:00       listenInterval   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DOZE_CNTRL_1 register
#define NXMAC_DOZE_CNTRL_1_ADDR   0xC0000044
/// Offset of the DOZE_CNTRL_1 register from the base address
#define NXMAC_DOZE_CNTRL_1_OFFSET 0x00000044
/// Index of the DOZE_CNTRL_1 register
#define NXMAC_DOZE_CNTRL_1_INDEX  0x00000011
/// Reset value of the DOZE_CNTRL_1 register
#define NXMAC_DOZE_CNTRL_1_RESET  0x00000000

/**
 * @brief Returns the current value of the DOZE_CNTRL_1 register.
 * The DOZE_CNTRL_1 register will be read and its value returned.
 * @return The current value of the DOZE_CNTRL_1 register.
 */
__INLINE uint32_t nxmac_doze_cntrl_1_get(void)
{
    return REG_PL_RD(NXMAC_DOZE_CNTRL_1_ADDR);
}

/**
 * @brief Sets the DOZE_CNTRL_1 register to a value.
 * The DOZE_CNTRL_1 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_doze_cntrl_1_set(uint32_t value)
{
    REG_PL_WR(NXMAC_DOZE_CNTRL_1_ADDR, value);
}

// field definitions
/// ATIM_W field mask
#define NXMAC_ATIM_W_MASK            ((uint32_t)0xFFFE0000)
/// ATIM_W field LSB position
#define NXMAC_ATIM_W_LSB             17
/// ATIM_W field width
#define NXMAC_ATIM_W_WIDTH           ((uint32_t)0x0000000F)
/// WAKEUP_DTIM field bit
#define NXMAC_WAKEUP_DTIM_BIT        ((uint32_t)0x00010000)
/// WAKEUP_DTIM field position
#define NXMAC_WAKEUP_DTIM_POS        16
/// LISTEN_INTERVAL field mask
#define NXMAC_LISTEN_INTERVAL_MASK   ((uint32_t)0x0000FFFF)
/// LISTEN_INTERVAL field LSB position
#define NXMAC_LISTEN_INTERVAL_LSB    0
/// LISTEN_INTERVAL field width
#define NXMAC_LISTEN_INTERVAL_WIDTH  ((uint32_t)0x00000010)

/// ATIM_W field reset value
#define NXMAC_ATIM_W_RST             0x0
/// WAKEUP_DTIM field reset value
#define NXMAC_WAKEUP_DTIM_RST        0x0
/// LISTEN_INTERVAL field reset value
#define NXMAC_LISTEN_INTERVAL_RST    0x0

/**
 * @brief Constructs a value for the DOZE_CNTRL_1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] atimw - The value to use for the atimW field.
 * @param[in] wakeupdtim - The value to use for the wakeupDTIM field.
 * @param[in] listeninterval - The value to use for the listenInterval field.
 */
__INLINE void nxmac_doze_cntrl_1_pack(uint16_t atimw, uint8_t wakeupdtim, uint16_t listeninterval)
{
    ASSERT_ERR((((uint32_t)atimw << 17) & ~((uint32_t)0xFFFE0000)) == 0);
    ASSERT_ERR((((uint32_t)wakeupdtim << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)listeninterval << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_DOZE_CNTRL_1_ADDR,  ((uint32_t)atimw << 17) | ((uint32_t)wakeupdtim << 16) | ((uint32_t)listeninterval << 0));
}

/**
 * @brief Unpacks DOZE_CNTRL_1's fields from current value of the DOZE_CNTRL_1 register.
 *
 * Reads the DOZE_CNTRL_1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] atimw - Will be populated with the current value of this field from the register.
 * @param[out] wakeupdtim - Will be populated with the current value of this field from the register.
 * @param[out] listeninterval - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_doze_cntrl_1_unpack(uint16_t *atimw, uint8_t *wakeupdtim, uint16_t *listeninterval)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DOZE_CNTRL_1_ADDR);

    *atimw = (localVal & ((uint32_t)0xFFFE0000)) >> 17;
    *wakeupdtim = (localVal & ((uint32_t)0x00010000)) >> 16;
    *listeninterval = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the atimW field in the DOZE_CNTRL_1 register.
 *
 * The DOZE_CNTRL_1 register will be read and the atimW field's value will be returned.
 *
 * @return The current value of the atimW field in the DOZE_CNTRL_1 register.
 */
__INLINE uint16_t nxmac_atim_w_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DOZE_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0xFFFE0000)) >> 17);
}

/**
 * @brief Sets the atimW field of the DOZE_CNTRL_1 register.
 *
 * The DOZE_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] atimw - The value to set the field to.
 */
__INLINE void nxmac_atim_w_setf(uint16_t atimw)
{
    ASSERT_ERR((((uint32_t)atimw << 17) & ~((uint32_t)0xFFFE0000)) == 0);
    REG_PL_WR(NXMAC_DOZE_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_DOZE_CNTRL_1_ADDR) & ~((uint32_t)0xFFFE0000)) | ((uint32_t)atimw << 17));
}

/**
 * @brief Returns the current value of the wakeupDTIM field in the DOZE_CNTRL_1 register.
 *
 * The DOZE_CNTRL_1 register will be read and the wakeupDTIM field's value will be returned.
 *
 * @return The current value of the wakeupDTIM field in the DOZE_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_wakeup_dtim_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DOZE_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the wakeupDTIM field of the DOZE_CNTRL_1 register.
 *
 * The DOZE_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] wakeupdtim - The value to set the field to.
 */
__INLINE void nxmac_wakeup_dtim_setf(uint8_t wakeupdtim)
{
    ASSERT_ERR((((uint32_t)wakeupdtim << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(NXMAC_DOZE_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_DOZE_CNTRL_1_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)wakeupdtim << 16));
}

/**
 * @brief Returns the current value of the listenInterval field in the DOZE_CNTRL_1 register.
 *
 * The DOZE_CNTRL_1 register will be read and the listenInterval field's value will be returned.
 *
 * @return The current value of the listenInterval field in the DOZE_CNTRL_1 register.
 */
__INLINE uint16_t nxmac_listen_interval_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DOZE_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the listenInterval field of the DOZE_CNTRL_1 register.
 *
 * The DOZE_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] listeninterval - The value to set the field to.
 */
__INLINE void nxmac_listen_interval_setf(uint16_t listeninterval)
{
    ASSERT_ERR((((uint32_t)listeninterval << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_DOZE_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_DOZE_CNTRL_1_ADDR) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)listeninterval << 0));
}

/// @}

/**
 * @name MAC_CNTRL_1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     26             rxRIFSEn   0
 *     25        tsfMgtDisable   0
 *     24       tsfUpdatedBySW   0
 *  16:14             abgnMode   0x3
 *     13       keyStoRAMReset   0
 *     12        mibTableReset   0
 *     11   rateControllerMPIF   1
 *     10        disableBAResp   0
 *     09       disableCTSResp   0
 *     08       disableACKResp   0
 *     07      activeClkGating   1
 *     06    enableLPClkSwitch   0
 *     05         lpClk32786Hz   0
 *     03             cfpAware   0
 *     02               pwrMgt   0
 *     01                   ap   0
 *     00              bssType   1
 * </pre>
 *
 * @{
 */

/// Address of the MAC_CNTRL_1 register
#define NXMAC_MAC_CNTRL_1_ADDR   0xC000004C
/// Offset of the MAC_CNTRL_1 register from the base address
#define NXMAC_MAC_CNTRL_1_OFFSET 0x0000004C
/// Index of the MAC_CNTRL_1 register
#define NXMAC_MAC_CNTRL_1_INDEX  0x00000013
/// Reset value of the MAC_CNTRL_1 register
#define NXMAC_MAC_CNTRL_1_RESET  0x0000C881

/**
 * @brief Returns the current value of the MAC_CNTRL_1 register.
 * The MAC_CNTRL_1 register will be read and its value returned.
 * @return The current value of the MAC_CNTRL_1 register.
 */
__INLINE uint32_t nxmac_mac_cntrl_1_get(void)
{
    return REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR);
}

/**
 * @brief Sets the MAC_CNTRL_1 register to a value.
 * The MAC_CNTRL_1 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_mac_cntrl_1_set(uint32_t value)
{
    REG_PL_WR(NXMAC_MAC_CNTRL_1_ADDR, value);
}

// field definitions
/// RX_RIFS_EN field bit
#define NXMAC_RX_RIFS_EN_BIT              ((uint32_t)0x04000000)
/// RX_RIFS_EN field position
#define NXMAC_RX_RIFS_EN_POS              26
/// TSF_MGT_DISABLE field bit
#define NXMAC_TSF_MGT_DISABLE_BIT         ((uint32_t)0x02000000)
/// TSF_MGT_DISABLE field position
#define NXMAC_TSF_MGT_DISABLE_POS         25
/// TSF_UPDATED_BY_SW field bit
#define NXMAC_TSF_UPDATED_BY_SW_BIT       ((uint32_t)0x01000000)
/// TSF_UPDATED_BY_SW field position
#define NXMAC_TSF_UPDATED_BY_SW_POS       24
/// ABGN_MODE field mask
#define NXMAC_ABGN_MODE_MASK              ((uint32_t)0x0001C000)
/// ABGN_MODE field LSB position
#define NXMAC_ABGN_MODE_LSB               14
/// ABGN_MODE field width
#define NXMAC_ABGN_MODE_WIDTH             ((uint32_t)0x00000003)
/// KEY_STO_RAM_RESET field bit
#define NXMAC_KEY_STO_RAM_RESET_BIT       ((uint32_t)0x00002000)
/// KEY_STO_RAM_RESET field position
#define NXMAC_KEY_STO_RAM_RESET_POS       13
/// MIB_TABLE_RESET field bit
#define NXMAC_MIB_TABLE_RESET_BIT         ((uint32_t)0x00001000)
/// MIB_TABLE_RESET field position
#define NXMAC_MIB_TABLE_RESET_POS         12
/// RATE_CONTROLLER_MPIF field bit
#define NXMAC_RATE_CONTROLLER_MPIF_BIT    ((uint32_t)0x00000800)
/// RATE_CONTROLLER_MPIF field position
#define NXMAC_RATE_CONTROLLER_MPIF_POS    11
/// DISABLE_BA_RESP field bit
#define NXMAC_DISABLE_BA_RESP_BIT         ((uint32_t)0x00000400)
/// DISABLE_BA_RESP field position
#define NXMAC_DISABLE_BA_RESP_POS         10
/// DISABLE_CTS_RESP field bit
#define NXMAC_DISABLE_CTS_RESP_BIT        ((uint32_t)0x00000200)
/// DISABLE_CTS_RESP field position
#define NXMAC_DISABLE_CTS_RESP_POS        9
/// DISABLE_ACK_RESP field bit
#define NXMAC_DISABLE_ACK_RESP_BIT        ((uint32_t)0x00000100)
/// DISABLE_ACK_RESP field position
#define NXMAC_DISABLE_ACK_RESP_POS        8
/// ACTIVE_CLK_GATING field bit
#define NXMAC_ACTIVE_CLK_GATING_BIT       ((uint32_t)0x00000080)
/// ACTIVE_CLK_GATING field position
#define NXMAC_ACTIVE_CLK_GATING_POS       7
/// ENABLE_LP_CLK_SWITCH field bit
#define NXMAC_ENABLE_LP_CLK_SWITCH_BIT    ((uint32_t)0x00000040)
/// ENABLE_LP_CLK_SWITCH field position
#define NXMAC_ENABLE_LP_CLK_SWITCH_POS    6
/// LP_CLK_32786_HZ field bit
#define NXMAC_LP_CLK_32786_HZ_BIT         ((uint32_t)0x00000020)
/// LP_CLK_32786_HZ field position
#define NXMAC_LP_CLK_32786_HZ_POS         5
/// CFP_AWARE field bit
#define NXMAC_CFP_AWARE_BIT               ((uint32_t)0x00000008)
/// CFP_AWARE field position
#define NXMAC_CFP_AWARE_POS               3
/// PWR_MGT field bit
#define NXMAC_PWR_MGT_BIT                 ((uint32_t)0x00000004)
/// PWR_MGT field position
#define NXMAC_PWR_MGT_POS                 2
/// AP field bit
#define NXMAC_AP_BIT                      ((uint32_t)0x00000002)
/// AP field position
#define NXMAC_AP_POS                      1
/// BSS_TYPE field bit
#define NXMAC_BSS_TYPE_BIT                ((uint32_t)0x00000001)
/// BSS_TYPE field position
#define NXMAC_BSS_TYPE_POS                0

/// RX_RIFS_EN field reset value
#define NXMAC_RX_RIFS_EN_RST              0x0
/// TSF_MGT_DISABLE field reset value
#define NXMAC_TSF_MGT_DISABLE_RST         0x0
/// TSF_UPDATED_BY_SW field reset value
#define NXMAC_TSF_UPDATED_BY_SW_RST       0x0
/// ABGN_MODE field reset value
#define NXMAC_ABGN_MODE_RST               0x3
/// KEY_STO_RAM_RESET field reset value
#define NXMAC_KEY_STO_RAM_RESET_RST       0x0
/// MIB_TABLE_RESET field reset value
#define NXMAC_MIB_TABLE_RESET_RST         0x0
/// RATE_CONTROLLER_MPIF field reset value
#define NXMAC_RATE_CONTROLLER_MPIF_RST    0x1
/// DISABLE_BA_RESP field reset value
#define NXMAC_DISABLE_BA_RESP_RST         0x0
/// DISABLE_CTS_RESP field reset value
#define NXMAC_DISABLE_CTS_RESP_RST        0x0
/// DISABLE_ACK_RESP field reset value
#define NXMAC_DISABLE_ACK_RESP_RST        0x0
/// ACTIVE_CLK_GATING field reset value
#define NXMAC_ACTIVE_CLK_GATING_RST       0x1
/// ENABLE_LP_CLK_SWITCH field reset value
#define NXMAC_ENABLE_LP_CLK_SWITCH_RST    0x0
/// LP_CLK_32786_HZ field reset value
#define NXMAC_LP_CLK_32786_HZ_RST         0x0
/// CFP_AWARE field reset value
#define NXMAC_CFP_AWARE_RST               0x0
/// PWR_MGT field reset value
#define NXMAC_PWR_MGT_RST                 0x0
/// AP field reset value
#define NXMAC_AP_RST                      0x0
/// BSS_TYPE field reset value
#define NXMAC_BSS_TYPE_RST                0x1

/**
 * @brief Constructs a value for the MAC_CNTRL_1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxrifsen - The value to use for the rxRIFSEn field.
 * @param[in] tsfmgtdisable - The value to use for the tsfMgtDisable field.
 * @param[in] tsfupdatedbysw - The value to use for the tsfUpdatedBySW field.
 * @param[in] abgnmode - The value to use for the abgnMode field.
 * @param[in] keystoramreset - The value to use for the keyStoRAMReset field.
 * @param[in] mibtablereset - The value to use for the mibTableReset field.
 * @param[in] ratecontrollermpif - The value to use for the rateControllerMPIF field.
 * @param[in] disablebaresp - The value to use for the disableBAResp field.
 * @param[in] disablectsresp - The value to use for the disableCTSResp field.
 * @param[in] disableackresp - The value to use for the disableACKResp field.
 * @param[in] activeclkgating - The value to use for the activeClkGating field.
 * @param[in] enablelpclkswitch - The value to use for the enableLPClkSwitch field.
 * @param[in] lpclk32786hz - The value to use for the lpClk32786Hz field.
 * @param[in] cfpaware - The value to use for the cfpAware field.
 * @param[in] pwrmgt - The value to use for the pwrMgt field.
 * @param[in] ap - The value to use for the ap field.
 * @param[in] bsstype - The value to use for the bssType field.
 */
__INLINE void nxmac_mac_cntrl_1_pack(uint8_t rxrifsen, uint8_t tsfmgtdisable, uint8_t tsfupdatedbysw, uint8_t abgnmode, uint8_t keystoramreset, uint8_t mibtablereset, uint8_t ratecontrollermpif, uint8_t disablebaresp, uint8_t disablectsresp, uint8_t disableackresp, uint8_t activeclkgating, uint8_t enablelpclkswitch, uint8_t lpclk32786hz, uint8_t cfpaware, uint8_t pwrmgt, uint8_t ap, uint8_t bsstype)
{
    ASSERT_ERR((((uint32_t)rxrifsen << 26) & ~((uint32_t)0x04000000)) == 0);
    ASSERT_ERR((((uint32_t)tsfmgtdisable << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)tsfupdatedbysw << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)abgnmode << 14) & ~((uint32_t)0x0001C000)) == 0);
    ASSERT_ERR((((uint32_t)keystoramreset << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)mibtablereset << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)ratecontrollermpif << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)disablebaresp << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)disablectsresp << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)disableackresp << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)activeclkgating << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)enablelpclkswitch << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)lpclk32786hz << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)cfpaware << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)pwrmgt << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)ap << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)bsstype << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_MAC_CNTRL_1_ADDR,  ((uint32_t)rxrifsen << 26) | ((uint32_t)tsfmgtdisable << 25) | ((uint32_t)tsfupdatedbysw << 24) | ((uint32_t)abgnmode << 14) | ((uint32_t)keystoramreset << 13) | ((uint32_t)mibtablereset << 12) | ((uint32_t)ratecontrollermpif << 11) | ((uint32_t)disablebaresp << 10) | ((uint32_t)disablectsresp << 9) | ((uint32_t)disableackresp << 8) | ((uint32_t)activeclkgating << 7) | ((uint32_t)enablelpclkswitch << 6) | ((uint32_t)lpclk32786hz << 5) | ((uint32_t)cfpaware << 3) | ((uint32_t)pwrmgt << 2) | ((uint32_t)ap << 1) | ((uint32_t)bsstype << 0));
}

/**
 * @brief Unpacks MAC_CNTRL_1's fields from current value of the MAC_CNTRL_1 register.
 *
 * Reads the MAC_CNTRL_1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxrifsen - Will be populated with the current value of this field from the register.
 * @param[out] tsfmgtdisable - Will be populated with the current value of this field from the register.
 * @param[out] tsfupdatedbysw - Will be populated with the current value of this field from the register.
 * @param[out] abgnmode - Will be populated with the current value of this field from the register.
 * @param[out] keystoramreset - Will be populated with the current value of this field from the register.
 * @param[out] mibtablereset - Will be populated with the current value of this field from the register.
 * @param[out] ratecontrollermpif - Will be populated with the current value of this field from the register.
 * @param[out] disablebaresp - Will be populated with the current value of this field from the register.
 * @param[out] disablectsresp - Will be populated with the current value of this field from the register.
 * @param[out] disableackresp - Will be populated with the current value of this field from the register.
 * @param[out] activeclkgating - Will be populated with the current value of this field from the register.
 * @param[out] enablelpclkswitch - Will be populated with the current value of this field from the register.
 * @param[out] lpclk32786hz - Will be populated with the current value of this field from the register.
 * @param[out] cfpaware - Will be populated with the current value of this field from the register.
 * @param[out] pwrmgt - Will be populated with the current value of this field from the register.
 * @param[out] ap - Will be populated with the current value of this field from the register.
 * @param[out] bsstype - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_mac_cntrl_1_unpack(uint8_t *rxrifsen, uint8_t *tsfmgtdisable, uint8_t *tsfupdatedbysw, uint8_t *abgnmode, uint8_t *keystoramreset, uint8_t *mibtablereset, uint8_t *ratecontrollermpif, uint8_t *disablebaresp, uint8_t *disablectsresp, uint8_t *disableackresp, uint8_t *activeclkgating, uint8_t *enablelpclkswitch, uint8_t *lpclk32786hz, uint8_t *cfpaware, uint8_t *pwrmgt, uint8_t *ap, uint8_t *bsstype)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR);

    *rxrifsen = (localVal & ((uint32_t)0x04000000)) >> 26;
    *tsfmgtdisable = (localVal & ((uint32_t)0x02000000)) >> 25;
    *tsfupdatedbysw = (localVal & ((uint32_t)0x01000000)) >> 24;
    *abgnmode = (localVal & ((uint32_t)0x0001C000)) >> 14;
    *keystoramreset = (localVal & ((uint32_t)0x00002000)) >> 13;
    *mibtablereset = (localVal & ((uint32_t)0x00001000)) >> 12;
    *ratecontrollermpif = (localVal & ((uint32_t)0x00000800)) >> 11;
    *disablebaresp = (localVal & ((uint32_t)0x00000400)) >> 10;
    *disablectsresp = (localVal & ((uint32_t)0x00000200)) >> 9;
    *disableackresp = (localVal & ((uint32_t)0x00000100)) >> 8;
    *activeclkgating = (localVal & ((uint32_t)0x00000080)) >> 7;
    *enablelpclkswitch = (localVal & ((uint32_t)0x00000040)) >> 6;
    *lpclk32786hz = (localVal & ((uint32_t)0x00000020)) >> 5;
    *cfpaware = (localVal & ((uint32_t)0x00000008)) >> 3;
    *pwrmgt = (localVal & ((uint32_t)0x00000004)) >> 2;
    *ap = (localVal & ((uint32_t)0x00000002)) >> 1;
    *bsstype = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the rxRIFSEn field in the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read and the rxRIFSEn field's value will be returned.
 *
 * @return The current value of the rxRIFSEn field in the MAC_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_rx_rifs_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

/**
 * @brief Sets the rxRIFSEn field of the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxrifsen - The value to set the field to.
 */
__INLINE void nxmac_rx_rifs_en_setf(uint8_t rxrifsen)
{
    ASSERT_ERR((((uint32_t)rxrifsen << 26) & ~((uint32_t)0x04000000)) == 0);
    REG_PL_WR(NXMAC_MAC_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR) & ~((uint32_t)0x04000000)) | ((uint32_t)rxrifsen << 26));
}

/**
 * @brief Returns the current value of the tsfMgtDisable field in the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read and the tsfMgtDisable field's value will be returned.
 *
 * @return The current value of the tsfMgtDisable field in the MAC_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_tsf_mgt_disable_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

/**
 * @brief Sets the tsfMgtDisable field of the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tsfmgtdisable - The value to set the field to.
 */
__INLINE void nxmac_tsf_mgt_disable_setf(uint8_t tsfmgtdisable)
{
    ASSERT_ERR((((uint32_t)tsfmgtdisable << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(NXMAC_MAC_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR) & ~((uint32_t)0x02000000)) | ((uint32_t)tsfmgtdisable << 25));
}

/**
 * @brief Returns the current value of the tsfUpdatedBySW field in the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read and the tsfUpdatedBySW field's value will be returned.
 *
 * @return The current value of the tsfUpdatedBySW field in the MAC_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_tsf_updated_by_sw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the tsfUpdatedBySW field of the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tsfupdatedbysw - The value to set the field to.
 */
__INLINE void nxmac_tsf_updated_by_sw_setf(uint8_t tsfupdatedbysw)
{
    ASSERT_ERR((((uint32_t)tsfupdatedbysw << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(NXMAC_MAC_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)tsfupdatedbysw << 24));
}

/**
 * @brief Returns the current value of the abgnMode field in the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read and the abgnMode field's value will be returned.
 *
 * @return The current value of the abgnMode field in the MAC_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_abgn_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x0001C000)) >> 14);
}

/**
 * @brief Sets the abgnMode field of the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] abgnmode - The value to set the field to.
 */
__INLINE void nxmac_abgn_mode_setf(uint8_t abgnmode)
{
    ASSERT_ERR((((uint32_t)abgnmode << 14) & ~((uint32_t)0x0001C000)) == 0);
    REG_PL_WR(NXMAC_MAC_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR) & ~((uint32_t)0x0001C000)) | ((uint32_t)abgnmode << 14));
}

/**
 * @brief Returns the current value of the keyStoRAMReset field in the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read and the keyStoRAMReset field's value will be returned.
 *
 * @return The current value of the keyStoRAMReset field in the MAC_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_key_sto_ram_reset_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the keyStoRAMReset field of the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] keystoramreset - The value to set the field to.
 */
__INLINE void nxmac_key_sto_ram_reset_setf(uint8_t keystoramreset)
{
    ASSERT_ERR((((uint32_t)keystoramreset << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(NXMAC_MAC_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)keystoramreset << 13));
}

/**
 * @brief Returns the current value of the mibTableReset field in the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read and the mibTableReset field's value will be returned.
 *
 * @return The current value of the mibTableReset field in the MAC_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_mib_table_reset_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the mibTableReset field of the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mibtablereset - The value to set the field to.
 */
__INLINE void nxmac_mib_table_reset_setf(uint8_t mibtablereset)
{
    ASSERT_ERR((((uint32_t)mibtablereset << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(NXMAC_MAC_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)mibtablereset << 12));
}

/**
 * @brief Returns the current value of the rateControllerMPIF field in the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read and the rateControllerMPIF field's value will be returned.
 *
 * @return The current value of the rateControllerMPIF field in the MAC_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_rate_controller_mpif_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the rateControllerMPIF field of the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ratecontrollermpif - The value to set the field to.
 */
__INLINE void nxmac_rate_controller_mpif_setf(uint8_t ratecontrollermpif)
{
    ASSERT_ERR((((uint32_t)ratecontrollermpif << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(NXMAC_MAC_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR) & ~((uint32_t)0x00000800)) | ((uint32_t)ratecontrollermpif << 11));
}

/**
 * @brief Returns the current value of the disableBAResp field in the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read and the disableBAResp field's value will be returned.
 *
 * @return The current value of the disableBAResp field in the MAC_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_disable_ba_resp_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the disableBAResp field of the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] disablebaresp - The value to set the field to.
 */
__INLINE void nxmac_disable_ba_resp_setf(uint8_t disablebaresp)
{
    ASSERT_ERR((((uint32_t)disablebaresp << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(NXMAC_MAC_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR) & ~((uint32_t)0x00000400)) | ((uint32_t)disablebaresp << 10));
}

/**
 * @brief Returns the current value of the disableCTSResp field in the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read and the disableCTSResp field's value will be returned.
 *
 * @return The current value of the disableCTSResp field in the MAC_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_disable_cts_resp_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the disableCTSResp field of the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] disablectsresp - The value to set the field to.
 */
__INLINE void nxmac_disable_cts_resp_setf(uint8_t disablectsresp)
{
    ASSERT_ERR((((uint32_t)disablectsresp << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(NXMAC_MAC_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)disablectsresp << 9));
}

/**
 * @brief Returns the current value of the disableACKResp field in the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read and the disableACKResp field's value will be returned.
 *
 * @return The current value of the disableACKResp field in the MAC_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_disable_ack_resp_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the disableACKResp field of the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] disableackresp - The value to set the field to.
 */
__INLINE void nxmac_disable_ack_resp_setf(uint8_t disableackresp)
{
    ASSERT_ERR((((uint32_t)disableackresp << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_MAC_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)disableackresp << 8));
}

/**
 * @brief Returns the current value of the activeClkGating field in the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read and the activeClkGating field's value will be returned.
 *
 * @return The current value of the activeClkGating field in the MAC_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_active_clk_gating_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the activeClkGating field of the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] activeclkgating - The value to set the field to.
 */
__INLINE void nxmac_active_clk_gating_setf(uint8_t activeclkgating)
{
    ASSERT_ERR((((uint32_t)activeclkgating << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(NXMAC_MAC_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)activeclkgating << 7));
}

/**
 * @brief Returns the current value of the enableLPClkSwitch field in the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read and the enableLPClkSwitch field's value will be returned.
 *
 * @return The current value of the enableLPClkSwitch field in the MAC_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_enable_lp_clk_switch_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the enableLPClkSwitch field of the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] enablelpclkswitch - The value to set the field to.
 */
__INLINE void nxmac_enable_lp_clk_switch_setf(uint8_t enablelpclkswitch)
{
    ASSERT_ERR((((uint32_t)enablelpclkswitch << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(NXMAC_MAC_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)enablelpclkswitch << 6));
}

/**
 * @brief Returns the current value of the lpClk32786Hz field in the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read and the lpClk32786Hz field's value will be returned.
 *
 * @return The current value of the lpClk32786Hz field in the MAC_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_lp_clk_32786_hz_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the lpClk32786Hz field of the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] lpclk32786hz - The value to set the field to.
 */
__INLINE void nxmac_lp_clk_32786_hz_setf(uint8_t lpclk32786hz)
{
    ASSERT_ERR((((uint32_t)lpclk32786hz << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_MAC_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)lpclk32786hz << 5));
}

/**
 * @brief Returns the current value of the cfpAware field in the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read and the cfpAware field's value will be returned.
 *
 * @return The current value of the cfpAware field in the MAC_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_cfp_aware_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the cfpAware field of the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cfpaware - The value to set the field to.
 */
__INLINE void nxmac_cfp_aware_setf(uint8_t cfpaware)
{
    ASSERT_ERR((((uint32_t)cfpaware << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_MAC_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)cfpaware << 3));
}

/**
 * @brief Returns the current value of the pwrMgt field in the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read and the pwrMgt field's value will be returned.
 *
 * @return The current value of the pwrMgt field in the MAC_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_pwr_mgt_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the pwrMgt field of the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pwrmgt - The value to set the field to.
 */
__INLINE void nxmac_pwr_mgt_setf(uint8_t pwrmgt)
{
    ASSERT_ERR((((uint32_t)pwrmgt << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_MAC_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)pwrmgt << 2));
}

/**
 * @brief Returns the current value of the ap field in the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read and the ap field's value will be returned.
 *
 * @return The current value of the ap field in the MAC_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_ap_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the ap field of the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ap - The value to set the field to.
 */
__INLINE void nxmac_ap_setf(uint8_t ap)
{
    ASSERT_ERR((((uint32_t)ap << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_MAC_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)ap << 1));
}

/**
 * @brief Returns the current value of the bssType field in the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read and the bssType field's value will be returned.
 *
 * @return The current value of the bssType field in the MAC_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_bss_type_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the bssType field of the MAC_CNTRL_1 register.
 *
 * The MAC_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bsstype - The value to set the field to.
 */
__INLINE void nxmac_bss_type_setf(uint8_t bsstype)
{
    ASSERT_ERR((((uint32_t)bsstype << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_MAC_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_MAC_CNTRL_1_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)bsstype << 0));
}

/// @}

/**
 * @name MAC_ERR_REC_CNTRL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     16        rxFlowCntrlEn   0
 *     07      baPSBitmapReset   0
 *     06      encrRxFIFOReset   0
 *     05    macPHYIFFIFOReset   0
 *     04          txFIFOReset   0
 *     03          rxFIFOReset   0
 *     02           hwFSMReset   0
 *     01            useErrDet   0
 *     00            useErrRec   0
 * </pre>
 *
 * @{
 */

/// Address of the MAC_ERR_REC_CNTRL register
#define NXMAC_MAC_ERR_REC_CNTRL_ADDR   0xC0000054
/// Offset of the MAC_ERR_REC_CNTRL register from the base address
#define NXMAC_MAC_ERR_REC_CNTRL_OFFSET 0x00000054
/// Index of the MAC_ERR_REC_CNTRL register
#define NXMAC_MAC_ERR_REC_CNTRL_INDEX  0x00000015
/// Reset value of the MAC_ERR_REC_CNTRL register
#define NXMAC_MAC_ERR_REC_CNTRL_RESET  0x00000000

/**
 * @brief Returns the current value of the MAC_ERR_REC_CNTRL register.
 * The MAC_ERR_REC_CNTRL register will be read and its value returned.
 * @return The current value of the MAC_ERR_REC_CNTRL register.
 */
__INLINE uint32_t nxmac_mac_err_rec_cntrl_get(void)
{
    return REG_PL_RD(NXMAC_MAC_ERR_REC_CNTRL_ADDR);
}

/**
 * @brief Sets the MAC_ERR_REC_CNTRL register to a value.
 * The MAC_ERR_REC_CNTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_mac_err_rec_cntrl_set(uint32_t value)
{
    REG_PL_WR(NXMAC_MAC_ERR_REC_CNTRL_ADDR, value);
}

// field definitions
/// RX_FLOW_CNTRL_EN field bit
#define NXMAC_RX_FLOW_CNTRL_EN_BIT       ((uint32_t)0x00010000)
/// RX_FLOW_CNTRL_EN field position
#define NXMAC_RX_FLOW_CNTRL_EN_POS       16
/// BA_PS_BITMAP_RESET field bit
#define NXMAC_BA_PS_BITMAP_RESET_BIT     ((uint32_t)0x00000080)
/// BA_PS_BITMAP_RESET field position
#define NXMAC_BA_PS_BITMAP_RESET_POS     7
/// ENCR_RX_FIFO_RESET field bit
#define NXMAC_ENCR_RX_FIFO_RESET_BIT     ((uint32_t)0x00000040)
/// ENCR_RX_FIFO_RESET field position
#define NXMAC_ENCR_RX_FIFO_RESET_POS     6
/// MAC_PHYIFFIFO_RESET field bit
#define NXMAC_MAC_PHYIFFIFO_RESET_BIT    ((uint32_t)0x00000020)
/// MAC_PHYIFFIFO_RESET field position
#define NXMAC_MAC_PHYIFFIFO_RESET_POS    5
/// TX_FIFO_RESET field bit
#define NXMAC_TX_FIFO_RESET_BIT          ((uint32_t)0x00000010)
/// TX_FIFO_RESET field position
#define NXMAC_TX_FIFO_RESET_POS          4
/// RX_FIFO_RESET field bit
#define NXMAC_RX_FIFO_RESET_BIT          ((uint32_t)0x00000008)
/// RX_FIFO_RESET field position
#define NXMAC_RX_FIFO_RESET_POS          3
/// HW_FSM_RESET field bit
#define NXMAC_HW_FSM_RESET_BIT           ((uint32_t)0x00000004)
/// HW_FSM_RESET field position
#define NXMAC_HW_FSM_RESET_POS           2
/// USE_ERR_DET field bit
#define NXMAC_USE_ERR_DET_BIT            ((uint32_t)0x00000002)
/// USE_ERR_DET field position
#define NXMAC_USE_ERR_DET_POS            1
/// USE_ERR_REC field bit
#define NXMAC_USE_ERR_REC_BIT            ((uint32_t)0x00000001)
/// USE_ERR_REC field position
#define NXMAC_USE_ERR_REC_POS            0

/// RX_FLOW_CNTRL_EN field reset value
#define NXMAC_RX_FLOW_CNTRL_EN_RST       0x0
/// BA_PS_BITMAP_RESET field reset value
#define NXMAC_BA_PS_BITMAP_RESET_RST     0x0
/// ENCR_RX_FIFO_RESET field reset value
#define NXMAC_ENCR_RX_FIFO_RESET_RST     0x0
/// MAC_PHYIFFIFO_RESET field reset value
#define NXMAC_MAC_PHYIFFIFO_RESET_RST    0x0
/// TX_FIFO_RESET field reset value
#define NXMAC_TX_FIFO_RESET_RST          0x0
/// RX_FIFO_RESET field reset value
#define NXMAC_RX_FIFO_RESET_RST          0x0
/// HW_FSM_RESET field reset value
#define NXMAC_HW_FSM_RESET_RST           0x0
/// USE_ERR_DET field reset value
#define NXMAC_USE_ERR_DET_RST            0x0
/// USE_ERR_REC field reset value
#define NXMAC_USE_ERR_REC_RST            0x0

/**
 * @brief Constructs a value for the MAC_ERR_REC_CNTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxflowcntrlen - The value to use for the rxFlowCntrlEn field.
 * @param[in] bapsbitmapreset - The value to use for the baPSBitmapReset field.
 * @param[in] encrrxfiforeset - The value to use for the encrRxFIFOReset field.
 * @param[in] macphyiffiforeset - The value to use for the macPHYIFFIFOReset field.
 * @param[in] txfiforeset - The value to use for the txFIFOReset field.
 * @param[in] rxfiforeset - The value to use for the rxFIFOReset field.
 * @param[in] hwfsmreset - The value to use for the hwFSMReset field.
 * @param[in] useerrdet - The value to use for the useErrDet field.
 */
__INLINE void nxmac_mac_err_rec_cntrl_pack(uint8_t rxflowcntrlen, uint8_t bapsbitmapreset, uint8_t encrrxfiforeset, uint8_t macphyiffiforeset, uint8_t txfiforeset, uint8_t rxfiforeset, uint8_t hwfsmreset, uint8_t useerrdet)
{
    ASSERT_ERR((((uint32_t)rxflowcntrlen << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)bapsbitmapreset << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)encrrxfiforeset << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)macphyiffiforeset << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)txfiforeset << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)rxfiforeset << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)hwfsmreset << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)useerrdet << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_MAC_ERR_REC_CNTRL_ADDR,  ((uint32_t)rxflowcntrlen << 16) | ((uint32_t)bapsbitmapreset << 7) | ((uint32_t)encrrxfiforeset << 6) | ((uint32_t)macphyiffiforeset << 5) | ((uint32_t)txfiforeset << 4) | ((uint32_t)rxfiforeset << 3) | ((uint32_t)hwfsmreset << 2) | ((uint32_t)useerrdet << 1));
}

/**
 * @brief Unpacks MAC_ERR_REC_CNTRL's fields from current value of the MAC_ERR_REC_CNTRL register.
 *
 * Reads the MAC_ERR_REC_CNTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxflowcntrlen - Will be populated with the current value of this field from the register.
 * @param[out] bapsbitmapreset - Will be populated with the current value of this field from the register.
 * @param[out] encrrxfiforeset - Will be populated with the current value of this field from the register.
 * @param[out] macphyiffiforeset - Will be populated with the current value of this field from the register.
 * @param[out] txfiforeset - Will be populated with the current value of this field from the register.
 * @param[out] rxfiforeset - Will be populated with the current value of this field from the register.
 * @param[out] hwfsmreset - Will be populated with the current value of this field from the register.
 * @param[out] useerrdet - Will be populated with the current value of this field from the register.
 * @param[out] useerrrec - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_mac_err_rec_cntrl_unpack(uint8_t *rxflowcntrlen, uint8_t *bapsbitmapreset, uint8_t *encrrxfiforeset, uint8_t *macphyiffiforeset, uint8_t *txfiforeset, uint8_t *rxfiforeset, uint8_t *hwfsmreset, uint8_t *useerrdet, uint8_t *useerrrec)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_REC_CNTRL_ADDR);

    *rxflowcntrlen = (localVal & ((uint32_t)0x00010000)) >> 16;
    *bapsbitmapreset = (localVal & ((uint32_t)0x00000080)) >> 7;
    *encrrxfiforeset = (localVal & ((uint32_t)0x00000040)) >> 6;
    *macphyiffiforeset = (localVal & ((uint32_t)0x00000020)) >> 5;
    *txfiforeset = (localVal & ((uint32_t)0x00000010)) >> 4;
    *rxfiforeset = (localVal & ((uint32_t)0x00000008)) >> 3;
    *hwfsmreset = (localVal & ((uint32_t)0x00000004)) >> 2;
    *useerrdet = (localVal & ((uint32_t)0x00000002)) >> 1;
    *useerrrec = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the rxFlowCntrlEn field in the MAC_ERR_REC_CNTRL register.
 *
 * The MAC_ERR_REC_CNTRL register will be read and the rxFlowCntrlEn field's value will be returned.
 *
 * @return The current value of the rxFlowCntrlEn field in the MAC_ERR_REC_CNTRL register.
 */
__INLINE uint8_t nxmac_rx_flow_cntrl_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_REC_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the rxFlowCntrlEn field of the MAC_ERR_REC_CNTRL register.
 *
 * The MAC_ERR_REC_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxflowcntrlen - The value to set the field to.
 */
__INLINE void nxmac_rx_flow_cntrl_en_setf(uint8_t rxflowcntrlen)
{
    ASSERT_ERR((((uint32_t)rxflowcntrlen << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(NXMAC_MAC_ERR_REC_CNTRL_ADDR, (REG_PL_RD(NXMAC_MAC_ERR_REC_CNTRL_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)rxflowcntrlen << 16));
}

/**
 * @brief Returns the current value of the baPSBitmapReset field in the MAC_ERR_REC_CNTRL register.
 *
 * The MAC_ERR_REC_CNTRL register will be read and the baPSBitmapReset field's value will be returned.
 *
 * @return The current value of the baPSBitmapReset field in the MAC_ERR_REC_CNTRL register.
 */
__INLINE uint8_t nxmac_ba_ps_bitmap_reset_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_REC_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the baPSBitmapReset field of the MAC_ERR_REC_CNTRL register.
 *
 * The MAC_ERR_REC_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bapsbitmapreset - The value to set the field to.
 */
__INLINE void nxmac_ba_ps_bitmap_reset_setf(uint8_t bapsbitmapreset)
{
    ASSERT_ERR((((uint32_t)bapsbitmapreset << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(NXMAC_MAC_ERR_REC_CNTRL_ADDR, (REG_PL_RD(NXMAC_MAC_ERR_REC_CNTRL_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)bapsbitmapreset << 7));
}

/**
 * @brief Returns the current value of the encrRxFIFOReset field in the MAC_ERR_REC_CNTRL register.
 *
 * The MAC_ERR_REC_CNTRL register will be read and the encrRxFIFOReset field's value will be returned.
 *
 * @return The current value of the encrRxFIFOReset field in the MAC_ERR_REC_CNTRL register.
 */
__INLINE uint8_t nxmac_encr_rx_fifo_reset_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_REC_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the encrRxFIFOReset field of the MAC_ERR_REC_CNTRL register.
 *
 * The MAC_ERR_REC_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] encrrxfiforeset - The value to set the field to.
 */
__INLINE void nxmac_encr_rx_fifo_reset_setf(uint8_t encrrxfiforeset)
{
    ASSERT_ERR((((uint32_t)encrrxfiforeset << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(NXMAC_MAC_ERR_REC_CNTRL_ADDR, (REG_PL_RD(NXMAC_MAC_ERR_REC_CNTRL_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)encrrxfiforeset << 6));
}

/**
 * @brief Returns the current value of the macPHYIFFIFOReset field in the MAC_ERR_REC_CNTRL register.
 *
 * The MAC_ERR_REC_CNTRL register will be read and the macPHYIFFIFOReset field's value will be returned.
 *
 * @return The current value of the macPHYIFFIFOReset field in the MAC_ERR_REC_CNTRL register.
 */
__INLINE uint8_t nxmac_mac_phyiffifo_reset_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_REC_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the macPHYIFFIFOReset field of the MAC_ERR_REC_CNTRL register.
 *
 * The MAC_ERR_REC_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macphyiffiforeset - The value to set the field to.
 */
__INLINE void nxmac_mac_phyiffifo_reset_setf(uint8_t macphyiffiforeset)
{
    ASSERT_ERR((((uint32_t)macphyiffiforeset << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_MAC_ERR_REC_CNTRL_ADDR, (REG_PL_RD(NXMAC_MAC_ERR_REC_CNTRL_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)macphyiffiforeset << 5));
}

/**
 * @brief Returns the current value of the txFIFOReset field in the MAC_ERR_REC_CNTRL register.
 *
 * The MAC_ERR_REC_CNTRL register will be read and the txFIFOReset field's value will be returned.
 *
 * @return The current value of the txFIFOReset field in the MAC_ERR_REC_CNTRL register.
 */
__INLINE uint8_t nxmac_tx_fifo_reset_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_REC_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the txFIFOReset field of the MAC_ERR_REC_CNTRL register.
 *
 * The MAC_ERR_REC_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txfiforeset - The value to set the field to.
 */
__INLINE void nxmac_tx_fifo_reset_setf(uint8_t txfiforeset)
{
    ASSERT_ERR((((uint32_t)txfiforeset << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(NXMAC_MAC_ERR_REC_CNTRL_ADDR, (REG_PL_RD(NXMAC_MAC_ERR_REC_CNTRL_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)txfiforeset << 4));
}

/**
 * @brief Returns the current value of the rxFIFOReset field in the MAC_ERR_REC_CNTRL register.
 *
 * The MAC_ERR_REC_CNTRL register will be read and the rxFIFOReset field's value will be returned.
 *
 * @return The current value of the rxFIFOReset field in the MAC_ERR_REC_CNTRL register.
 */
__INLINE uint8_t nxmac_rx_fifo_reset_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_REC_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the rxFIFOReset field of the MAC_ERR_REC_CNTRL register.
 *
 * The MAC_ERR_REC_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxfiforeset - The value to set the field to.
 */
__INLINE void nxmac_rx_fifo_reset_setf(uint8_t rxfiforeset)
{
    ASSERT_ERR((((uint32_t)rxfiforeset << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_MAC_ERR_REC_CNTRL_ADDR, (REG_PL_RD(NXMAC_MAC_ERR_REC_CNTRL_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)rxfiforeset << 3));
}

/**
 * @brief Returns the current value of the hwFSMReset field in the MAC_ERR_REC_CNTRL register.
 *
 * The MAC_ERR_REC_CNTRL register will be read and the hwFSMReset field's value will be returned.
 *
 * @return The current value of the hwFSMReset field in the MAC_ERR_REC_CNTRL register.
 */
__INLINE uint8_t nxmac_hw_fsm_reset_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_REC_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the hwFSMReset field of the MAC_ERR_REC_CNTRL register.
 *
 * The MAC_ERR_REC_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] hwfsmreset - The value to set the field to.
 */
__INLINE void nxmac_hw_fsm_reset_setf(uint8_t hwfsmreset)
{
    ASSERT_ERR((((uint32_t)hwfsmreset << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_MAC_ERR_REC_CNTRL_ADDR, (REG_PL_RD(NXMAC_MAC_ERR_REC_CNTRL_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)hwfsmreset << 2));
}

/**
 * @brief Returns the current value of the useErrDet field in the MAC_ERR_REC_CNTRL register.
 *
 * The MAC_ERR_REC_CNTRL register will be read and the useErrDet field's value will be returned.
 *
 * @return The current value of the useErrDet field in the MAC_ERR_REC_CNTRL register.
 */
__INLINE uint8_t nxmac_use_err_det_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_REC_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the useErrDet field of the MAC_ERR_REC_CNTRL register.
 *
 * The MAC_ERR_REC_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] useerrdet - The value to set the field to.
 */
__INLINE void nxmac_use_err_det_setf(uint8_t useerrdet)
{
    ASSERT_ERR((((uint32_t)useerrdet << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_MAC_ERR_REC_CNTRL_ADDR, (REG_PL_RD(NXMAC_MAC_ERR_REC_CNTRL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)useerrdet << 1));
}

/**
 * @brief Returns the current value of the useErrRec field in the MAC_ERR_REC_CNTRL register.
 *
 * The MAC_ERR_REC_CNTRL register will be read and the useErrRec field's value will be returned.
 *
 * @return The current value of the useErrRec field in the MAC_ERR_REC_CNTRL register.
 */
__INLINE uint8_t nxmac_use_err_rec_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_REC_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/// @}

/**
 * @name MAC_ERR_SET_STATUS register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     03        errInHWLevel3   0
 *     02      errInTxRxLevel2   0
 *     01        errInRxLevel1   0
 *     00        errInTxLevel1   0
 * </pre>
 *
 * @{
 */

/// Address of the MAC_ERR_SET_STATUS register
#define NXMAC_MAC_ERR_SET_STATUS_ADDR   0xC0000058
/// Offset of the MAC_ERR_SET_STATUS register from the base address
#define NXMAC_MAC_ERR_SET_STATUS_OFFSET 0x00000058
/// Index of the MAC_ERR_SET_STATUS register
#define NXMAC_MAC_ERR_SET_STATUS_INDEX  0x00000016
/// Reset value of the MAC_ERR_SET_STATUS register
#define NXMAC_MAC_ERR_SET_STATUS_RESET  0x00000000

/**
 * @brief Returns the current value of the MAC_ERR_SET_STATUS register.
 * The MAC_ERR_SET_STATUS register will be read and its value returned.
 * @return The current value of the MAC_ERR_SET_STATUS register.
 */
__INLINE uint32_t nxmac_mac_err_set_status_get(void)
{
    return REG_PL_RD(NXMAC_MAC_ERR_SET_STATUS_ADDR);
}

/**
 * @brief Sets the MAC_ERR_SET_STATUS register to a value.
 * The MAC_ERR_SET_STATUS register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_mac_err_set_status_set(uint32_t value)
{
    REG_PL_WR(NXMAC_MAC_ERR_SET_STATUS_ADDR, value);
}

// field definitions
/// ERR_IN_HW_LEVEL_3 field bit
#define NXMAC_ERR_IN_HW_LEVEL_3_BIT       ((uint32_t)0x00000008)
/// ERR_IN_HW_LEVEL_3 field position
#define NXMAC_ERR_IN_HW_LEVEL_3_POS       3
/// ERR_IN_TX_RX_LEVEL_2 field bit
#define NXMAC_ERR_IN_TX_RX_LEVEL_2_BIT    ((uint32_t)0x00000004)
/// ERR_IN_TX_RX_LEVEL_2 field position
#define NXMAC_ERR_IN_TX_RX_LEVEL_2_POS    2
/// ERR_IN_RX_LEVEL_1 field bit
#define NXMAC_ERR_IN_RX_LEVEL_1_BIT       ((uint32_t)0x00000002)
/// ERR_IN_RX_LEVEL_1 field position
#define NXMAC_ERR_IN_RX_LEVEL_1_POS       1
/// ERR_IN_TX_LEVEL_1 field bit
#define NXMAC_ERR_IN_TX_LEVEL_1_BIT       ((uint32_t)0x00000001)
/// ERR_IN_TX_LEVEL_1 field position
#define NXMAC_ERR_IN_TX_LEVEL_1_POS       0

/// ERR_IN_HW_LEVEL_3 field reset value
#define NXMAC_ERR_IN_HW_LEVEL_3_RST       0x0
/// ERR_IN_TX_RX_LEVEL_2 field reset value
#define NXMAC_ERR_IN_TX_RX_LEVEL_2_RST    0x0
/// ERR_IN_RX_LEVEL_1 field reset value
#define NXMAC_ERR_IN_RX_LEVEL_1_RST       0x0
/// ERR_IN_TX_LEVEL_1 field reset value
#define NXMAC_ERR_IN_TX_LEVEL_1_RST       0x0

/**
 * @brief Constructs a value for the MAC_ERR_SET_STATUS register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] errinhwlevel3 - The value to use for the errInHWLevel3 field.
 * @param[in] errintxrxlevel2 - The value to use for the errInTxRxLevel2 field.
 * @param[in] errinrxlevel1 - The value to use for the errInRxLevel1 field.
 * @param[in] errintxlevel1 - The value to use for the errInTxLevel1 field.
 */
__INLINE void nxmac_mac_err_set_status_pack(uint8_t errinhwlevel3, uint8_t errintxrxlevel2, uint8_t errinrxlevel1, uint8_t errintxlevel1)
{
    ASSERT_ERR((((uint32_t)errinhwlevel3 << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)errintxrxlevel2 << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)errinrxlevel1 << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)errintxlevel1 << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_MAC_ERR_SET_STATUS_ADDR,  ((uint32_t)errinhwlevel3 << 3) | ((uint32_t)errintxrxlevel2 << 2) | ((uint32_t)errinrxlevel1 << 1) | ((uint32_t)errintxlevel1 << 0));
}

/**
 * @brief Unpacks MAC_ERR_SET_STATUS's fields from current value of the MAC_ERR_SET_STATUS register.
 *
 * Reads the MAC_ERR_SET_STATUS register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] errinhwlevel3 - Will be populated with the current value of this field from the register.
 * @param[out] errintxrxlevel2 - Will be populated with the current value of this field from the register.
 * @param[out] errinrxlevel1 - Will be populated with the current value of this field from the register.
 * @param[out] errintxlevel1 - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_mac_err_set_status_unpack(uint8_t *errinhwlevel3, uint8_t *errintxrxlevel2, uint8_t *errinrxlevel1, uint8_t *errintxlevel1)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_SET_STATUS_ADDR);

    *errinhwlevel3 = (localVal & ((uint32_t)0x00000008)) >> 3;
    *errintxrxlevel2 = (localVal & ((uint32_t)0x00000004)) >> 2;
    *errinrxlevel1 = (localVal & ((uint32_t)0x00000002)) >> 1;
    *errintxlevel1 = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the errInHWLevel3 field in the MAC_ERR_SET_STATUS register.
 *
 * The MAC_ERR_SET_STATUS register will be read and the errInHWLevel3 field's value will be returned.
 *
 * @return The current value of the errInHWLevel3 field in the MAC_ERR_SET_STATUS register.
 */
__INLINE uint8_t nxmac_err_in_hw_level_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_SET_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the errInHWLevel3 field of the MAC_ERR_SET_STATUS register.
 *
 * The MAC_ERR_SET_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] errinhwlevel3 - The value to set the field to.
 */
__INLINE void nxmac_err_in_hw_level_3_setf(uint8_t errinhwlevel3)
{
    ASSERT_ERR((((uint32_t)errinhwlevel3 << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_MAC_ERR_SET_STATUS_ADDR, (uint32_t)errinhwlevel3 << 3);
}

/**
 * @brief Returns the current value of the errInTxRxLevel2 field in the MAC_ERR_SET_STATUS register.
 *
 * The MAC_ERR_SET_STATUS register will be read and the errInTxRxLevel2 field's value will be returned.
 *
 * @return The current value of the errInTxRxLevel2 field in the MAC_ERR_SET_STATUS register.
 */
__INLINE uint8_t nxmac_err_in_tx_rx_level_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_SET_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the errInTxRxLevel2 field of the MAC_ERR_SET_STATUS register.
 *
 * The MAC_ERR_SET_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] errintxrxlevel2 - The value to set the field to.
 */
__INLINE void nxmac_err_in_tx_rx_level_2_setf(uint8_t errintxrxlevel2)
{
    ASSERT_ERR((((uint32_t)errintxrxlevel2 << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_MAC_ERR_SET_STATUS_ADDR, (uint32_t)errintxrxlevel2 << 2);
}

/**
 * @brief Returns the current value of the errInRxLevel1 field in the MAC_ERR_SET_STATUS register.
 *
 * The MAC_ERR_SET_STATUS register will be read and the errInRxLevel1 field's value will be returned.
 *
 * @return The current value of the errInRxLevel1 field in the MAC_ERR_SET_STATUS register.
 */
__INLINE uint8_t nxmac_err_in_rx_level_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_SET_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the errInRxLevel1 field of the MAC_ERR_SET_STATUS register.
 *
 * The MAC_ERR_SET_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] errinrxlevel1 - The value to set the field to.
 */
__INLINE void nxmac_err_in_rx_level_1_setf(uint8_t errinrxlevel1)
{
    ASSERT_ERR((((uint32_t)errinrxlevel1 << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_MAC_ERR_SET_STATUS_ADDR, (uint32_t)errinrxlevel1 << 1);
}

/**
 * @brief Returns the current value of the errInTxLevel1 field in the MAC_ERR_SET_STATUS register.
 *
 * The MAC_ERR_SET_STATUS register will be read and the errInTxLevel1 field's value will be returned.
 *
 * @return The current value of the errInTxLevel1 field in the MAC_ERR_SET_STATUS register.
 */
__INLINE uint8_t nxmac_err_in_tx_level_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_SET_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the errInTxLevel1 field of the MAC_ERR_SET_STATUS register.
 *
 * The MAC_ERR_SET_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] errintxlevel1 - The value to set the field to.
 */
__INLINE void nxmac_err_in_tx_level_1_setf(uint8_t errintxlevel1)
{
    ASSERT_ERR((((uint32_t)errintxlevel1 << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_MAC_ERR_SET_STATUS_ADDR, (uint32_t)errintxlevel1 << 0);
}

/// @}

/**
 * @name MAC_ERR_CLEAR_STATUS register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     03   clearErrInHWLevel3   0
 *     02   clearErrInTxRxLevel2   0
 *     01   clearErrInRxLevel1   0
 *     00   clearErrInTxLevel1   0
 * </pre>
 *
 * @{
 */

/// Address of the MAC_ERR_CLEAR_STATUS register
#define NXMAC_MAC_ERR_CLEAR_STATUS_ADDR   0xC000005C
/// Offset of the MAC_ERR_CLEAR_STATUS register from the base address
#define NXMAC_MAC_ERR_CLEAR_STATUS_OFFSET 0x0000005C
/// Index of the MAC_ERR_CLEAR_STATUS register
#define NXMAC_MAC_ERR_CLEAR_STATUS_INDEX  0x00000017
/// Reset value of the MAC_ERR_CLEAR_STATUS register
#define NXMAC_MAC_ERR_CLEAR_STATUS_RESET  0x00000000

/**
 * @brief Returns the current value of the MAC_ERR_CLEAR_STATUS register.
 * The MAC_ERR_CLEAR_STATUS register will be read and its value returned.
 * @return The current value of the MAC_ERR_CLEAR_STATUS register.
 */
__INLINE uint32_t nxmac_mac_err_clear_status_get(void)
{
    return REG_PL_RD(NXMAC_MAC_ERR_CLEAR_STATUS_ADDR);
}

/**
 * @brief Sets the MAC_ERR_CLEAR_STATUS register to a value.
 * The MAC_ERR_CLEAR_STATUS register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_mac_err_clear_status_clear(uint32_t value)
{
    REG_PL_WR(NXMAC_MAC_ERR_CLEAR_STATUS_ADDR, value);
}

// field definitions
/// CLEAR_ERR_IN_HW_LEVEL_3 field bit
#define NXMAC_CLEAR_ERR_IN_HW_LEVEL_3_BIT       ((uint32_t)0x00000008)
/// CLEAR_ERR_IN_HW_LEVEL_3 field position
#define NXMAC_CLEAR_ERR_IN_HW_LEVEL_3_POS       3
/// CLEAR_ERR_IN_TX_RX_LEVEL_2 field bit
#define NXMAC_CLEAR_ERR_IN_TX_RX_LEVEL_2_BIT    ((uint32_t)0x00000004)
/// CLEAR_ERR_IN_TX_RX_LEVEL_2 field position
#define NXMAC_CLEAR_ERR_IN_TX_RX_LEVEL_2_POS    2
/// CLEAR_ERR_IN_RX_LEVEL_1 field bit
#define NXMAC_CLEAR_ERR_IN_RX_LEVEL_1_BIT       ((uint32_t)0x00000002)
/// CLEAR_ERR_IN_RX_LEVEL_1 field position
#define NXMAC_CLEAR_ERR_IN_RX_LEVEL_1_POS       1
/// CLEAR_ERR_IN_TX_LEVEL_1 field bit
#define NXMAC_CLEAR_ERR_IN_TX_LEVEL_1_BIT       ((uint32_t)0x00000001)
/// CLEAR_ERR_IN_TX_LEVEL_1 field position
#define NXMAC_CLEAR_ERR_IN_TX_LEVEL_1_POS       0

/// CLEAR_ERR_IN_HW_LEVEL_3 field reset value
#define NXMAC_CLEAR_ERR_IN_HW_LEVEL_3_RST       0x0
/// CLEAR_ERR_IN_TX_RX_LEVEL_2 field reset value
#define NXMAC_CLEAR_ERR_IN_TX_RX_LEVEL_2_RST    0x0
/// CLEAR_ERR_IN_RX_LEVEL_1 field reset value
#define NXMAC_CLEAR_ERR_IN_RX_LEVEL_1_RST       0x0
/// CLEAR_ERR_IN_TX_LEVEL_1 field reset value
#define NXMAC_CLEAR_ERR_IN_TX_LEVEL_1_RST       0x0

/**
 * @brief Constructs a value for the MAC_ERR_CLEAR_STATUS register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] clearerrinhwlevel3 - The value to use for the clearErrInHWLevel3 field.
 * @param[in] clearerrintxrxlevel2 - The value to use for the clearErrInTxRxLevel2 field.
 * @param[in] clearerrinrxlevel1 - The value to use for the clearErrInRxLevel1 field.
 * @param[in] clearerrintxlevel1 - The value to use for the clearErrInTxLevel1 field.
 */
__INLINE void nxmac_mac_err_clear_status_pack(uint8_t clearerrinhwlevel3, uint8_t clearerrintxrxlevel2, uint8_t clearerrinrxlevel1, uint8_t clearerrintxlevel1)
{
    ASSERT_ERR((((uint32_t)clearerrinhwlevel3 << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)clearerrintxrxlevel2 << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)clearerrinrxlevel1 << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)clearerrintxlevel1 << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_MAC_ERR_CLEAR_STATUS_ADDR,  ((uint32_t)clearerrinhwlevel3 << 3) | ((uint32_t)clearerrintxrxlevel2 << 2) | ((uint32_t)clearerrinrxlevel1 << 1) | ((uint32_t)clearerrintxlevel1 << 0));
}

/**
 * @brief Unpacks MAC_ERR_CLEAR_STATUS's fields from current value of the MAC_ERR_CLEAR_STATUS register.
 *
 * Reads the MAC_ERR_CLEAR_STATUS register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] clearerrinhwlevel3 - Will be populated with the current value of this field from the register.
 * @param[out] clearerrintxrxlevel2 - Will be populated with the current value of this field from the register.
 * @param[out] clearerrinrxlevel1 - Will be populated with the current value of this field from the register.
 * @param[out] clearerrintxlevel1 - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_mac_err_clear_status_unpack(uint8_t *clearerrinhwlevel3, uint8_t *clearerrintxrxlevel2, uint8_t *clearerrinrxlevel1, uint8_t *clearerrintxlevel1)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_CLEAR_STATUS_ADDR);

    *clearerrinhwlevel3 = (localVal & ((uint32_t)0x00000008)) >> 3;
    *clearerrintxrxlevel2 = (localVal & ((uint32_t)0x00000004)) >> 2;
    *clearerrinrxlevel1 = (localVal & ((uint32_t)0x00000002)) >> 1;
    *clearerrintxlevel1 = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the clearErrInHWLevel3 field in the MAC_ERR_CLEAR_STATUS register.
 *
 * The MAC_ERR_CLEAR_STATUS register will be read and the clearErrInHWLevel3 field's value will be returned.
 *
 * @return The current value of the clearErrInHWLevel3 field in the MAC_ERR_CLEAR_STATUS register.
 */
__INLINE uint8_t nxmac_clear_err_in_hw_level_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_CLEAR_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the clearErrInHWLevel3 field of the MAC_ERR_CLEAR_STATUS register.
 *
 * The MAC_ERR_CLEAR_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] clearerrinhwlevel3 - The value to set the field to.
 */
__INLINE void nxmac_clear_err_in_hw_level_3_clearf(uint8_t clearerrinhwlevel3)
{
    ASSERT_ERR((((uint32_t)clearerrinhwlevel3 << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_MAC_ERR_CLEAR_STATUS_ADDR, (uint32_t)clearerrinhwlevel3 << 3);
}

/**
 * @brief Returns the current value of the clearErrInTxRxLevel2 field in the MAC_ERR_CLEAR_STATUS register.
 *
 * The MAC_ERR_CLEAR_STATUS register will be read and the clearErrInTxRxLevel2 field's value will be returned.
 *
 * @return The current value of the clearErrInTxRxLevel2 field in the MAC_ERR_CLEAR_STATUS register.
 */
__INLINE uint8_t nxmac_clear_err_in_tx_rx_level_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_CLEAR_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the clearErrInTxRxLevel2 field of the MAC_ERR_CLEAR_STATUS register.
 *
 * The MAC_ERR_CLEAR_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] clearerrintxrxlevel2 - The value to set the field to.
 */
__INLINE void nxmac_clear_err_in_tx_rx_level_2_clearf(uint8_t clearerrintxrxlevel2)
{
    ASSERT_ERR((((uint32_t)clearerrintxrxlevel2 << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_MAC_ERR_CLEAR_STATUS_ADDR, (uint32_t)clearerrintxrxlevel2 << 2);
}

/**
 * @brief Returns the current value of the clearErrInRxLevel1 field in the MAC_ERR_CLEAR_STATUS register.
 *
 * The MAC_ERR_CLEAR_STATUS register will be read and the clearErrInRxLevel1 field's value will be returned.
 *
 * @return The current value of the clearErrInRxLevel1 field in the MAC_ERR_CLEAR_STATUS register.
 */
__INLINE uint8_t nxmac_clear_err_in_rx_level_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_CLEAR_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the clearErrInRxLevel1 field of the MAC_ERR_CLEAR_STATUS register.
 *
 * The MAC_ERR_CLEAR_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] clearerrinrxlevel1 - The value to set the field to.
 */
__INLINE void nxmac_clear_err_in_rx_level_1_clearf(uint8_t clearerrinrxlevel1)
{
    ASSERT_ERR((((uint32_t)clearerrinrxlevel1 << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_MAC_ERR_CLEAR_STATUS_ADDR, (uint32_t)clearerrinrxlevel1 << 1);
}

/**
 * @brief Returns the current value of the clearErrInTxLevel1 field in the MAC_ERR_CLEAR_STATUS register.
 *
 * The MAC_ERR_CLEAR_STATUS register will be read and the clearErrInTxLevel1 field's value will be returned.
 *
 * @return The current value of the clearErrInTxLevel1 field in the MAC_ERR_CLEAR_STATUS register.
 */
__INLINE uint8_t nxmac_clear_err_in_tx_level_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAC_ERR_CLEAR_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the clearErrInTxLevel1 field of the MAC_ERR_CLEAR_STATUS register.
 *
 * The MAC_ERR_CLEAR_STATUS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] clearerrintxlevel1 - The value to set the field to.
 */
__INLINE void nxmac_clear_err_in_tx_level_1_clearf(uint8_t clearerrintxlevel1)
{
    ASSERT_ERR((((uint32_t)clearerrintxlevel1 << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_MAC_ERR_CLEAR_STATUS_ADDR, (uint32_t)clearerrintxlevel1 << 0);
}

/// @}

/**
 * @name RX_CNTRL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31   enDuplicateDetection   0
 *     30        acceptUnknown   0
 *     29   acceptOtherDataFrames   0
 *     28        acceptQoSNull   1
 *     27      acceptQCFWOData   0
 *     26          acceptQData   1
 *     25       acceptCFWOData   0
 *     24           acceptData   1
 *     23   acceptOtherCntrlFrames   0
 *     22          acceptCFEnd   0
 *     21            acceptACK   0
 *     20            acceptCTS   0
 *     19            acceptRTS   0
 *     18         acceptPSPoll   1
 *     17             acceptBA   1
 *     16            acceptBAR   1
 *     15   acceptOtherMgmtFrames   1
 *     14    acceptBfmeeFrames   0
 *     13      acceptAllBeacon   0
 *     12   acceptNotExpectedBA   0
 *     11   acceptDecryptErrorFrames   0
 *     10         acceptBeacon   1
 *     09      acceptProbeResp   1
 *     08       acceptProbeReq   1
 *     07      acceptMyUnicast   1
 *     06        acceptUnicast   0
 *     05    acceptErrorFrames   0
 *     04     acceptOtherBSSID   0
 *     03      acceptBroadcast   1
 *     02      acceptMulticast   0
 *     01          dontDecrypt   0
 *     00       excUnencrypted   0
 * </pre>
 *
 * @{
 */

/// Address of the RX_CNTRL register
#define NXMAC_RX_CNTRL_ADDR   0xC0000060
/// Offset of the RX_CNTRL register from the base address
#define NXMAC_RX_CNTRL_OFFSET 0x00000060
/// Index of the RX_CNTRL register
#define NXMAC_RX_CNTRL_INDEX  0x00000018
/// Reset value of the RX_CNTRL register
#define NXMAC_RX_CNTRL_RESET  0x15078788

/**
 * @brief Returns the current value of the RX_CNTRL register.
 * The RX_CNTRL register will be read and its value returned.
 * @return The current value of the RX_CNTRL register.
 */
__INLINE uint32_t nxmac_rx_cntrl_get(void)
{
    return REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
}

/**
 * @brief Sets the RX_CNTRL register to a value.
 * The RX_CNTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_rx_cntrl_set(uint32_t value)
{
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, value);
}

// field definitions
/// EN_DUPLICATE_DETECTION field bit
#define NXMAC_EN_DUPLICATE_DETECTION_BIT         ((uint32_t)0x80000000)
/// EN_DUPLICATE_DETECTION field position
#define NXMAC_EN_DUPLICATE_DETECTION_POS         31
/// ACCEPT_UNKNOWN field bit
#define NXMAC_ACCEPT_UNKNOWN_BIT                 ((uint32_t)0x40000000)
/// ACCEPT_UNKNOWN field position
#define NXMAC_ACCEPT_UNKNOWN_POS                 30
/// ACCEPT_OTHER_DATA_FRAMES field bit
#define NXMAC_ACCEPT_OTHER_DATA_FRAMES_BIT       ((uint32_t)0x20000000)
/// ACCEPT_OTHER_DATA_FRAMES field position
#define NXMAC_ACCEPT_OTHER_DATA_FRAMES_POS       29
/// ACCEPT_QO_S_NULL field bit
#define NXMAC_ACCEPT_QO_S_NULL_BIT               ((uint32_t)0x10000000)
/// ACCEPT_QO_S_NULL field position
#define NXMAC_ACCEPT_QO_S_NULL_POS               28
/// ACCEPT_QCFWO_DATA field bit
#define NXMAC_ACCEPT_QCFWO_DATA_BIT              ((uint32_t)0x08000000)
/// ACCEPT_QCFWO_DATA field position
#define NXMAC_ACCEPT_QCFWO_DATA_POS              27
/// ACCEPT_Q_DATA field bit
#define NXMAC_ACCEPT_Q_DATA_BIT                  ((uint32_t)0x04000000)
/// ACCEPT_Q_DATA field position
#define NXMAC_ACCEPT_Q_DATA_POS                  26
/// ACCEPT_CFWO_DATA field bit
#define NXMAC_ACCEPT_CFWO_DATA_BIT               ((uint32_t)0x02000000)
/// ACCEPT_CFWO_DATA field position
#define NXMAC_ACCEPT_CFWO_DATA_POS               25
/// ACCEPT_DATA field bit
#define NXMAC_ACCEPT_DATA_BIT                    ((uint32_t)0x01000000)
/// ACCEPT_DATA field position
#define NXMAC_ACCEPT_DATA_POS                    24
/// ACCEPT_OTHER_CNTRL_FRAMES field bit
#define NXMAC_ACCEPT_OTHER_CNTRL_FRAMES_BIT      ((uint32_t)0x00800000)
/// ACCEPT_OTHER_CNTRL_FRAMES field position
#define NXMAC_ACCEPT_OTHER_CNTRL_FRAMES_POS      23
/// ACCEPT_CF_END field bit
#define NXMAC_ACCEPT_CF_END_BIT                  ((uint32_t)0x00400000)
/// ACCEPT_CF_END field position
#define NXMAC_ACCEPT_CF_END_POS                  22
/// ACCEPT_ACK field bit
#define NXMAC_ACCEPT_ACK_BIT                     ((uint32_t)0x00200000)
/// ACCEPT_ACK field position
#define NXMAC_ACCEPT_ACK_POS                     21
/// ACCEPT_CTS field bit
#define NXMAC_ACCEPT_CTS_BIT                     ((uint32_t)0x00100000)
/// ACCEPT_CTS field position
#define NXMAC_ACCEPT_CTS_POS                     20
/// ACCEPT_RTS field bit
#define NXMAC_ACCEPT_RTS_BIT                     ((uint32_t)0x00080000)
/// ACCEPT_RTS field position
#define NXMAC_ACCEPT_RTS_POS                     19
/// ACCEPT_PS_POLL field bit
#define NXMAC_ACCEPT_PS_POLL_BIT                 ((uint32_t)0x00040000)
/// ACCEPT_PS_POLL field position
#define NXMAC_ACCEPT_PS_POLL_POS                 18
/// ACCEPT_BA field bit
#define NXMAC_ACCEPT_BA_BIT                      ((uint32_t)0x00020000)
/// ACCEPT_BA field position
#define NXMAC_ACCEPT_BA_POS                      17
/// ACCEPT_BAR field bit
#define NXMAC_ACCEPT_BAR_BIT                     ((uint32_t)0x00010000)
/// ACCEPT_BAR field position
#define NXMAC_ACCEPT_BAR_POS                     16
/// ACCEPT_OTHER_MGMT_FRAMES field bit
#define NXMAC_ACCEPT_OTHER_MGMT_FRAMES_BIT       ((uint32_t)0x00008000)
/// ACCEPT_OTHER_MGMT_FRAMES field position
#define NXMAC_ACCEPT_OTHER_MGMT_FRAMES_POS       15
/// ACCEPT_BFMEE_FRAMES field bit
#define NXMAC_ACCEPT_BFMEE_FRAMES_BIT            ((uint32_t)0x00004000)
/// ACCEPT_BFMEE_FRAMES field position
#define NXMAC_ACCEPT_BFMEE_FRAMES_POS            14
/// ACCEPT_ALL_BEACON field bit
#define NXMAC_ACCEPT_ALL_BEACON_BIT              ((uint32_t)0x00002000)
/// ACCEPT_ALL_BEACON field position
#define NXMAC_ACCEPT_ALL_BEACON_POS              13
/// ACCEPT_NOT_EXPECTED_BA field bit
#define NXMAC_ACCEPT_NOT_EXPECTED_BA_BIT         ((uint32_t)0x00001000)
/// ACCEPT_NOT_EXPECTED_BA field position
#define NXMAC_ACCEPT_NOT_EXPECTED_BA_POS         12
/// ACCEPT_DECRYPT_ERROR_FRAMES field bit
#define NXMAC_ACCEPT_DECRYPT_ERROR_FRAMES_BIT    ((uint32_t)0x00000800)
/// ACCEPT_DECRYPT_ERROR_FRAMES field position
#define NXMAC_ACCEPT_DECRYPT_ERROR_FRAMES_POS    11
/// ACCEPT_BEACON field bit
#define NXMAC_ACCEPT_BEACON_BIT                  ((uint32_t)0x00000400)
/// ACCEPT_BEACON field position
#define NXMAC_ACCEPT_BEACON_POS                  10
/// ACCEPT_PROBE_RESP field bit
#define NXMAC_ACCEPT_PROBE_RESP_BIT              ((uint32_t)0x00000200)
/// ACCEPT_PROBE_RESP field position
#define NXMAC_ACCEPT_PROBE_RESP_POS              9
/// ACCEPT_PROBE_REQ field bit
#define NXMAC_ACCEPT_PROBE_REQ_BIT               ((uint32_t)0x00000100)
/// ACCEPT_PROBE_REQ field position
#define NXMAC_ACCEPT_PROBE_REQ_POS               8
/// ACCEPT_MY_UNICAST field bit
#define NXMAC_ACCEPT_MY_UNICAST_BIT              ((uint32_t)0x00000080)
/// ACCEPT_MY_UNICAST field position
#define NXMAC_ACCEPT_MY_UNICAST_POS              7
/// ACCEPT_UNICAST field bit
#define NXMAC_ACCEPT_UNICAST_BIT                 ((uint32_t)0x00000040)
/// ACCEPT_UNICAST field position
#define NXMAC_ACCEPT_UNICAST_POS                 6
/// ACCEPT_ERROR_FRAMES field bit
#define NXMAC_ACCEPT_ERROR_FRAMES_BIT            ((uint32_t)0x00000020)
/// ACCEPT_ERROR_FRAMES field position
#define NXMAC_ACCEPT_ERROR_FRAMES_POS            5
/// ACCEPT_OTHER_BSSID field bit
#define NXMAC_ACCEPT_OTHER_BSSID_BIT             ((uint32_t)0x00000010)
/// ACCEPT_OTHER_BSSID field position
#define NXMAC_ACCEPT_OTHER_BSSID_POS             4
/// ACCEPT_BROADCAST field bit
#define NXMAC_ACCEPT_BROADCAST_BIT               ((uint32_t)0x00000008)
/// ACCEPT_BROADCAST field position
#define NXMAC_ACCEPT_BROADCAST_POS               3
/// ACCEPT_MULTICAST field bit
#define NXMAC_ACCEPT_MULTICAST_BIT               ((uint32_t)0x00000004)
/// ACCEPT_MULTICAST field position
#define NXMAC_ACCEPT_MULTICAST_POS               2
/// DONT_DECRYPT field bit
#define NXMAC_DONT_DECRYPT_BIT                   ((uint32_t)0x00000002)
/// DONT_DECRYPT field position
#define NXMAC_DONT_DECRYPT_POS                   1
/// EXC_UNENCRYPTED field bit
#define NXMAC_EXC_UNENCRYPTED_BIT                ((uint32_t)0x00000001)
/// EXC_UNENCRYPTED field position
#define NXMAC_EXC_UNENCRYPTED_POS                0

/// EN_DUPLICATE_DETECTION field reset value
#define NXMAC_EN_DUPLICATE_DETECTION_RST         0x0
/// ACCEPT_UNKNOWN field reset value
#define NXMAC_ACCEPT_UNKNOWN_RST                 0x0
/// ACCEPT_OTHER_DATA_FRAMES field reset value
#define NXMAC_ACCEPT_OTHER_DATA_FRAMES_RST       0x0
/// ACCEPT_QO_S_NULL field reset value
#define NXMAC_ACCEPT_QO_S_NULL_RST               0x1
/// ACCEPT_QCFWO_DATA field reset value
#define NXMAC_ACCEPT_QCFWO_DATA_RST              0x0
/// ACCEPT_Q_DATA field reset value
#define NXMAC_ACCEPT_Q_DATA_RST                  0x1
/// ACCEPT_CFWO_DATA field reset value
#define NXMAC_ACCEPT_CFWO_DATA_RST               0x0
/// ACCEPT_DATA field reset value
#define NXMAC_ACCEPT_DATA_RST                    0x1
/// ACCEPT_OTHER_CNTRL_FRAMES field reset value
#define NXMAC_ACCEPT_OTHER_CNTRL_FRAMES_RST      0x0
/// ACCEPT_CF_END field reset value
#define NXMAC_ACCEPT_CF_END_RST                  0x0
/// ACCEPT_ACK field reset value
#define NXMAC_ACCEPT_ACK_RST                     0x0
/// ACCEPT_CTS field reset value
#define NXMAC_ACCEPT_CTS_RST                     0x0
/// ACCEPT_RTS field reset value
#define NXMAC_ACCEPT_RTS_RST                     0x0
/// ACCEPT_PS_POLL field reset value
#define NXMAC_ACCEPT_PS_POLL_RST                 0x1
/// ACCEPT_BA field reset value
#define NXMAC_ACCEPT_BA_RST                      0x1
/// ACCEPT_BAR field reset value
#define NXMAC_ACCEPT_BAR_RST                     0x1
/// ACCEPT_OTHER_MGMT_FRAMES field reset value
#define NXMAC_ACCEPT_OTHER_MGMT_FRAMES_RST       0x1
/// ACCEPT_BFMEE_FRAMES field reset value
#define NXMAC_ACCEPT_BFMEE_FRAMES_RST            0x0
/// ACCEPT_ALL_BEACON field reset value
#define NXMAC_ACCEPT_ALL_BEACON_RST              0x0
/// ACCEPT_NOT_EXPECTED_BA field reset value
#define NXMAC_ACCEPT_NOT_EXPECTED_BA_RST         0x0
/// ACCEPT_DECRYPT_ERROR_FRAMES field reset value
#define NXMAC_ACCEPT_DECRYPT_ERROR_FRAMES_RST    0x0
/// ACCEPT_BEACON field reset value
#define NXMAC_ACCEPT_BEACON_RST                  0x1
/// ACCEPT_PROBE_RESP field reset value
#define NXMAC_ACCEPT_PROBE_RESP_RST              0x1
/// ACCEPT_PROBE_REQ field reset value
#define NXMAC_ACCEPT_PROBE_REQ_RST               0x1
/// ACCEPT_MY_UNICAST field reset value
#define NXMAC_ACCEPT_MY_UNICAST_RST              0x1
/// ACCEPT_UNICAST field reset value
#define NXMAC_ACCEPT_UNICAST_RST                 0x0
/// ACCEPT_ERROR_FRAMES field reset value
#define NXMAC_ACCEPT_ERROR_FRAMES_RST            0x0
/// ACCEPT_OTHER_BSSID field reset value
#define NXMAC_ACCEPT_OTHER_BSSID_RST             0x0
/// ACCEPT_BROADCAST field reset value
#define NXMAC_ACCEPT_BROADCAST_RST               0x1
/// ACCEPT_MULTICAST field reset value
#define NXMAC_ACCEPT_MULTICAST_RST               0x0
/// DONT_DECRYPT field reset value
#define NXMAC_DONT_DECRYPT_RST                   0x0
/// EXC_UNENCRYPTED field reset value
#define NXMAC_EXC_UNENCRYPTED_RST                0x0

/**
 * @brief Constructs a value for the RX_CNTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] acceptunknown - The value to use for the acceptUnknown field.
 * @param[in] acceptotherdataframes - The value to use for the acceptOtherDataFrames field.
 * @param[in] acceptqosnull - The value to use for the acceptQoSNull field.
 * @param[in] acceptqcfwodata - The value to use for the acceptQCFWOData field.
 * @param[in] acceptqdata - The value to use for the acceptQData field.
 * @param[in] acceptcfwodata - The value to use for the acceptCFWOData field.
 * @param[in] acceptdata - The value to use for the acceptData field.
 * @param[in] acceptothercntrlframes - The value to use for the acceptOtherCntrlFrames field.
 * @param[in] acceptcfend - The value to use for the acceptCFEnd field.
 * @param[in] acceptack - The value to use for the acceptACK field.
 * @param[in] acceptcts - The value to use for the acceptCTS field.
 * @param[in] acceptrts - The value to use for the acceptRTS field.
 * @param[in] acceptpspoll - The value to use for the acceptPSPoll field.
 * @param[in] acceptba - The value to use for the acceptBA field.
 * @param[in] acceptbar - The value to use for the acceptBAR field.
 * @param[in] acceptothermgmtframes - The value to use for the acceptOtherMgmtFrames field.
 * @param[in] acceptbfmeeframes - The value to use for the acceptBfmeeFrames field.
 * @param[in] acceptallbeacon - The value to use for the acceptAllBeacon field.
 * @param[in] acceptnotexpectedba - The value to use for the acceptNotExpectedBA field.
 * @param[in] acceptdecrypterrorframes - The value to use for the acceptDecryptErrorFrames field.
 * @param[in] acceptbeacon - The value to use for the acceptBeacon field.
 * @param[in] acceptproberesp - The value to use for the acceptProbeResp field.
 * @param[in] acceptprobereq - The value to use for the acceptProbeReq field.
 * @param[in] acceptmyunicast - The value to use for the acceptMyUnicast field.
 * @param[in] acceptunicast - The value to use for the acceptUnicast field.
 * @param[in] accepterrorframes - The value to use for the acceptErrorFrames field.
 * @param[in] acceptotherbssid - The value to use for the acceptOtherBSSID field.
 * @param[in] acceptbroadcast - The value to use for the acceptBroadcast field.
 * @param[in] acceptmulticast - The value to use for the acceptMulticast field.
 * @param[in] dontdecrypt - The value to use for the dontDecrypt field.
 * @param[in] excunencrypted - The value to use for the excUnencrypted field.
 */
__INLINE void nxmac_rx_cntrl_pack(uint8_t acceptunknown, uint8_t acceptotherdataframes, uint8_t acceptqosnull, uint8_t acceptqcfwodata, uint8_t acceptqdata, uint8_t acceptcfwodata, uint8_t acceptdata, uint8_t acceptothercntrlframes, uint8_t acceptcfend, uint8_t acceptack, uint8_t acceptcts, uint8_t acceptrts, uint8_t acceptpspoll, uint8_t acceptba, uint8_t acceptbar, uint8_t acceptothermgmtframes, uint8_t acceptbfmeeframes, uint8_t acceptallbeacon, uint8_t acceptnotexpectedba, uint8_t acceptdecrypterrorframes, uint8_t acceptbeacon, uint8_t acceptproberesp, uint8_t acceptprobereq, uint8_t acceptmyunicast, uint8_t acceptunicast, uint8_t accepterrorframes, uint8_t acceptotherbssid, uint8_t acceptbroadcast, uint8_t acceptmulticast, uint8_t dontdecrypt, uint8_t excunencrypted)
{
    ASSERT_ERR((((uint32_t)acceptunknown << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)acceptotherdataframes << 29) & ~((uint32_t)0x20000000)) == 0);
    ASSERT_ERR((((uint32_t)acceptqosnull << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)acceptqcfwodata << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)acceptqdata << 26) & ~((uint32_t)0x04000000)) == 0);
    ASSERT_ERR((((uint32_t)acceptcfwodata << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)acceptdata << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)acceptothercntrlframes << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)acceptcfend << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)acceptack << 21) & ~((uint32_t)0x00200000)) == 0);
    ASSERT_ERR((((uint32_t)acceptcts << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)acceptrts << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)acceptpspoll << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)acceptba << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)acceptbar << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)acceptothermgmtframes << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)acceptbfmeeframes << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)acceptallbeacon << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)acceptnotexpectedba << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)acceptdecrypterrorframes << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)acceptbeacon << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)acceptproberesp << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)acceptprobereq << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)acceptmyunicast << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)acceptunicast << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)accepterrorframes << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)acceptotherbssid << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)acceptbroadcast << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)acceptmulticast << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)dontdecrypt << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)excunencrypted << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR,  ((uint32_t)acceptunknown << 30) | ((uint32_t)acceptotherdataframes << 29) | ((uint32_t)acceptqosnull << 28) | ((uint32_t)acceptqcfwodata << 27) | ((uint32_t)acceptqdata << 26) | ((uint32_t)acceptcfwodata << 25) | ((uint32_t)acceptdata << 24) | ((uint32_t)acceptothercntrlframes << 23) | ((uint32_t)acceptcfend << 22) | ((uint32_t)acceptack << 21) | ((uint32_t)acceptcts << 20) | ((uint32_t)acceptrts << 19) | ((uint32_t)acceptpspoll << 18) | ((uint32_t)acceptba << 17) | ((uint32_t)acceptbar << 16) | ((uint32_t)acceptothermgmtframes << 15) | ((uint32_t)acceptbfmeeframes << 14) | ((uint32_t)acceptallbeacon << 13) | ((uint32_t)acceptnotexpectedba << 12) | ((uint32_t)acceptdecrypterrorframes << 11) | ((uint32_t)acceptbeacon << 10) | ((uint32_t)acceptproberesp << 9) | ((uint32_t)acceptprobereq << 8) | ((uint32_t)acceptmyunicast << 7) | ((uint32_t)acceptunicast << 6) | ((uint32_t)accepterrorframes << 5) | ((uint32_t)acceptotherbssid << 4) | ((uint32_t)acceptbroadcast << 3) | ((uint32_t)acceptmulticast << 2) | ((uint32_t)dontdecrypt << 1) | ((uint32_t)excunencrypted << 0));
}

/**
 * @brief Unpacks RX_CNTRL's fields from current value of the RX_CNTRL register.
 *
 * Reads the RX_CNTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] enduplicatedetection - Will be populated with the current value of this field from the register.
 * @param[out] acceptunknown - Will be populated with the current value of this field from the register.
 * @param[out] acceptotherdataframes - Will be populated with the current value of this field from the register.
 * @param[out] acceptqosnull - Will be populated with the current value of this field from the register.
 * @param[out] acceptqcfwodata - Will be populated with the current value of this field from the register.
 * @param[out] acceptqdata - Will be populated with the current value of this field from the register.
 * @param[out] acceptcfwodata - Will be populated with the current value of this field from the register.
 * @param[out] acceptdata - Will be populated with the current value of this field from the register.
 * @param[out] acceptothercntrlframes - Will be populated with the current value of this field from the register.
 * @param[out] acceptcfend - Will be populated with the current value of this field from the register.
 * @param[out] acceptack - Will be populated with the current value of this field from the register.
 * @param[out] acceptcts - Will be populated with the current value of this field from the register.
 * @param[out] acceptrts - Will be populated with the current value of this field from the register.
 * @param[out] acceptpspoll - Will be populated with the current value of this field from the register.
 * @param[out] acceptba - Will be populated with the current value of this field from the register.
 * @param[out] acceptbar - Will be populated with the current value of this field from the register.
 * @param[out] acceptothermgmtframes - Will be populated with the current value of this field from the register.
 * @param[out] acceptbfmeeframes - Will be populated with the current value of this field from the register.
 * @param[out] acceptallbeacon - Will be populated with the current value of this field from the register.
 * @param[out] acceptnotexpectedba - Will be populated with the current value of this field from the register.
 * @param[out] acceptdecrypterrorframes - Will be populated with the current value of this field from the register.
 * @param[out] acceptbeacon - Will be populated with the current value of this field from the register.
 * @param[out] acceptproberesp - Will be populated with the current value of this field from the register.
 * @param[out] acceptprobereq - Will be populated with the current value of this field from the register.
 * @param[out] acceptmyunicast - Will be populated with the current value of this field from the register.
 * @param[out] acceptunicast - Will be populated with the current value of this field from the register.
 * @param[out] accepterrorframes - Will be populated with the current value of this field from the register.
 * @param[out] acceptotherbssid - Will be populated with the current value of this field from the register.
 * @param[out] acceptbroadcast - Will be populated with the current value of this field from the register.
 * @param[out] acceptmulticast - Will be populated with the current value of this field from the register.
 * @param[out] dontdecrypt - Will be populated with the current value of this field from the register.
 * @param[out] excunencrypted - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_rx_cntrl_unpack(uint8_t *enduplicatedetection, uint8_t *acceptunknown, uint8_t *acceptotherdataframes, uint8_t *acceptqosnull, uint8_t *acceptqcfwodata, uint8_t *acceptqdata, uint8_t *acceptcfwodata, uint8_t *acceptdata, uint8_t *acceptothercntrlframes, uint8_t *acceptcfend, uint8_t *acceptack, uint8_t *acceptcts, uint8_t *acceptrts, uint8_t *acceptpspoll, uint8_t *acceptba, uint8_t *acceptbar, uint8_t *acceptothermgmtframes, uint8_t *acceptbfmeeframes, uint8_t *acceptallbeacon, uint8_t *acceptnotexpectedba, uint8_t *acceptdecrypterrorframes, uint8_t *acceptbeacon, uint8_t *acceptproberesp, uint8_t *acceptprobereq, uint8_t *acceptmyunicast, uint8_t *acceptunicast, uint8_t *accepterrorframes, uint8_t *acceptotherbssid, uint8_t *acceptbroadcast, uint8_t *acceptmulticast, uint8_t *dontdecrypt, uint8_t *excunencrypted)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);

    *enduplicatedetection = (localVal & ((uint32_t)0x80000000)) >> 31;
    *acceptunknown = (localVal & ((uint32_t)0x40000000)) >> 30;
    *acceptotherdataframes = (localVal & ((uint32_t)0x20000000)) >> 29;
    *acceptqosnull = (localVal & ((uint32_t)0x10000000)) >> 28;
    *acceptqcfwodata = (localVal & ((uint32_t)0x08000000)) >> 27;
    *acceptqdata = (localVal & ((uint32_t)0x04000000)) >> 26;
    *acceptcfwodata = (localVal & ((uint32_t)0x02000000)) >> 25;
    *acceptdata = (localVal & ((uint32_t)0x01000000)) >> 24;
    *acceptothercntrlframes = (localVal & ((uint32_t)0x00800000)) >> 23;
    *acceptcfend = (localVal & ((uint32_t)0x00400000)) >> 22;
    *acceptack = (localVal & ((uint32_t)0x00200000)) >> 21;
    *acceptcts = (localVal & ((uint32_t)0x00100000)) >> 20;
    *acceptrts = (localVal & ((uint32_t)0x00080000)) >> 19;
    *acceptpspoll = (localVal & ((uint32_t)0x00040000)) >> 18;
    *acceptba = (localVal & ((uint32_t)0x00020000)) >> 17;
    *acceptbar = (localVal & ((uint32_t)0x00010000)) >> 16;
    *acceptothermgmtframes = (localVal & ((uint32_t)0x00008000)) >> 15;
    *acceptbfmeeframes = (localVal & ((uint32_t)0x00004000)) >> 14;
    *acceptallbeacon = (localVal & ((uint32_t)0x00002000)) >> 13;
    *acceptnotexpectedba = (localVal & ((uint32_t)0x00001000)) >> 12;
    *acceptdecrypterrorframes = (localVal & ((uint32_t)0x00000800)) >> 11;
    *acceptbeacon = (localVal & ((uint32_t)0x00000400)) >> 10;
    *acceptproberesp = (localVal & ((uint32_t)0x00000200)) >> 9;
    *acceptprobereq = (localVal & ((uint32_t)0x00000100)) >> 8;
    *acceptmyunicast = (localVal & ((uint32_t)0x00000080)) >> 7;
    *acceptunicast = (localVal & ((uint32_t)0x00000040)) >> 6;
    *accepterrorframes = (localVal & ((uint32_t)0x00000020)) >> 5;
    *acceptotherbssid = (localVal & ((uint32_t)0x00000010)) >> 4;
    *acceptbroadcast = (localVal & ((uint32_t)0x00000008)) >> 3;
    *acceptmulticast = (localVal & ((uint32_t)0x00000004)) >> 2;
    *dontdecrypt = (localVal & ((uint32_t)0x00000002)) >> 1;
    *excunencrypted = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the enDuplicateDetection field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the enDuplicateDetection field's value will be returned.
 *
 * @return The current value of the enDuplicateDetection field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_en_duplicate_detection_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Returns the current value of the acceptUnknown field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptUnknown field's value will be returned.
 *
 * @return The current value of the acceptUnknown field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_unknown_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

/**
 * @brief Sets the acceptUnknown field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptunknown - The value to set the field to.
 */
__INLINE void nxmac_accept_unknown_setf(uint8_t acceptunknown)
{
    ASSERT_ERR((((uint32_t)acceptunknown << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)acceptunknown << 30));
}

/**
 * @brief Returns the current value of the acceptOtherDataFrames field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptOtherDataFrames field's value will be returned.
 *
 * @return The current value of the acceptOtherDataFrames field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_other_data_frames_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

/**
 * @brief Sets the acceptOtherDataFrames field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptotherdataframes - The value to set the field to.
 */
__INLINE void nxmac_accept_other_data_frames_setf(uint8_t acceptotherdataframes)
{
    ASSERT_ERR((((uint32_t)acceptotherdataframes << 29) & ~((uint32_t)0x20000000)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x20000000)) | ((uint32_t)acceptotherdataframes << 29));
}

/**
 * @brief Returns the current value of the acceptQoSNull field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptQoSNull field's value will be returned.
 *
 * @return The current value of the acceptQoSNull field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_qo_s_null_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the acceptQoSNull field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptqosnull - The value to set the field to.
 */
__INLINE void nxmac_accept_qo_s_null_setf(uint8_t acceptqosnull)
{
    ASSERT_ERR((((uint32_t)acceptqosnull << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)acceptqosnull << 28));
}

/**
 * @brief Returns the current value of the acceptQCFWOData field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptQCFWOData field's value will be returned.
 *
 * @return The current value of the acceptQCFWOData field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_qcfwo_data_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Sets the acceptQCFWOData field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptqcfwodata - The value to set the field to.
 */
__INLINE void nxmac_accept_qcfwo_data_setf(uint8_t acceptqcfwodata)
{
    ASSERT_ERR((((uint32_t)acceptqcfwodata << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x08000000)) | ((uint32_t)acceptqcfwodata << 27));
}

/**
 * @brief Returns the current value of the acceptQData field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptQData field's value will be returned.
 *
 * @return The current value of the acceptQData field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_q_data_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

/**
 * @brief Sets the acceptQData field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptqdata - The value to set the field to.
 */
__INLINE void nxmac_accept_q_data_setf(uint8_t acceptqdata)
{
    ASSERT_ERR((((uint32_t)acceptqdata << 26) & ~((uint32_t)0x04000000)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x04000000)) | ((uint32_t)acceptqdata << 26));
}

/**
 * @brief Returns the current value of the acceptCFWOData field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptCFWOData field's value will be returned.
 *
 * @return The current value of the acceptCFWOData field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_cfwo_data_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

/**
 * @brief Sets the acceptCFWOData field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptcfwodata - The value to set the field to.
 */
__INLINE void nxmac_accept_cfwo_data_setf(uint8_t acceptcfwodata)
{
    ASSERT_ERR((((uint32_t)acceptcfwodata << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x02000000)) | ((uint32_t)acceptcfwodata << 25));
}

/**
 * @brief Returns the current value of the acceptData field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptData field's value will be returned.
 *
 * @return The current value of the acceptData field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_data_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the acceptData field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptdata - The value to set the field to.
 */
__INLINE void nxmac_accept_data_setf(uint8_t acceptdata)
{
    ASSERT_ERR((((uint32_t)acceptdata << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)acceptdata << 24));
}

/**
 * @brief Returns the current value of the acceptOtherCntrlFrames field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptOtherCntrlFrames field's value will be returned.
 *
 * @return The current value of the acceptOtherCntrlFrames field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_other_cntrl_frames_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Sets the acceptOtherCntrlFrames field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptothercntrlframes - The value to set the field to.
 */
__INLINE void nxmac_accept_other_cntrl_frames_setf(uint8_t acceptothercntrlframes)
{
    ASSERT_ERR((((uint32_t)acceptothercntrlframes << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00800000)) | ((uint32_t)acceptothercntrlframes << 23));
}

/**
 * @brief Returns the current value of the acceptCFEnd field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptCFEnd field's value will be returned.
 *
 * @return The current value of the acceptCFEnd field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_cf_end_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Sets the acceptCFEnd field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptcfend - The value to set the field to.
 */
__INLINE void nxmac_accept_cf_end_setf(uint8_t acceptcfend)
{
    ASSERT_ERR((((uint32_t)acceptcfend << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00400000)) | ((uint32_t)acceptcfend << 22));
}

/**
 * @brief Returns the current value of the acceptACK field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptACK field's value will be returned.
 *
 * @return The current value of the acceptACK field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_ack_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Sets the acceptACK field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptack - The value to set the field to.
 */
__INLINE void nxmac_accept_ack_setf(uint8_t acceptack)
{
    ASSERT_ERR((((uint32_t)acceptack << 21) & ~((uint32_t)0x00200000)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00200000)) | ((uint32_t)acceptack << 21));
}

/**
 * @brief Returns the current value of the acceptCTS field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptCTS field's value will be returned.
 *
 * @return The current value of the acceptCTS field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_cts_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Sets the acceptCTS field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptcts - The value to set the field to.
 */
__INLINE void nxmac_accept_cts_setf(uint8_t acceptcts)
{
    ASSERT_ERR((((uint32_t)acceptcts << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00100000)) | ((uint32_t)acceptcts << 20));
}

/**
 * @brief Returns the current value of the acceptRTS field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptRTS field's value will be returned.
 *
 * @return The current value of the acceptRTS field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_rts_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Sets the acceptRTS field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptrts - The value to set the field to.
 */
__INLINE void nxmac_accept_rts_setf(uint8_t acceptrts)
{
    ASSERT_ERR((((uint32_t)acceptrts << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00080000)) | ((uint32_t)acceptrts << 19));
}

/**
 * @brief Returns the current value of the acceptPSPoll field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptPSPoll field's value will be returned.
 *
 * @return The current value of the acceptPSPoll field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_ps_poll_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Sets the acceptPSPoll field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptpspoll - The value to set the field to.
 */
__INLINE void nxmac_accept_ps_poll_setf(uint8_t acceptpspoll)
{
    ASSERT_ERR((((uint32_t)acceptpspoll << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00040000)) | ((uint32_t)acceptpspoll << 18));
}

/**
 * @brief Returns the current value of the acceptBA field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptBA field's value will be returned.
 *
 * @return The current value of the acceptBA field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_ba_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Sets the acceptBA field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptba - The value to set the field to.
 */
__INLINE void nxmac_accept_ba_setf(uint8_t acceptba)
{
    ASSERT_ERR((((uint32_t)acceptba << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00020000)) | ((uint32_t)acceptba << 17));
}

/**
 * @brief Returns the current value of the acceptBAR field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptBAR field's value will be returned.
 *
 * @return The current value of the acceptBAR field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_bar_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the acceptBAR field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptbar - The value to set the field to.
 */
__INLINE void nxmac_accept_bar_setf(uint8_t acceptbar)
{
    ASSERT_ERR((((uint32_t)acceptbar << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)acceptbar << 16));
}

/**
 * @brief Returns the current value of the acceptOtherMgmtFrames field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptOtherMgmtFrames field's value will be returned.
 *
 * @return The current value of the acceptOtherMgmtFrames field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_other_mgmt_frames_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

/**
 * @brief Sets the acceptOtherMgmtFrames field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptothermgmtframes - The value to set the field to.
 */
__INLINE void nxmac_accept_other_mgmt_frames_setf(uint8_t acceptothermgmtframes)
{
    ASSERT_ERR((((uint32_t)acceptothermgmtframes << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00008000)) | ((uint32_t)acceptothermgmtframes << 15));
}

/**
 * @brief Returns the current value of the acceptBfmeeFrames field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptBfmeeFrames field's value will be returned.
 *
 * @return The current value of the acceptBfmeeFrames field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_bfmee_frames_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Sets the acceptBfmeeFrames field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptbfmeeframes - The value to set the field to.
 */
__INLINE void nxmac_accept_bfmee_frames_setf(uint8_t acceptbfmeeframes)
{
    ASSERT_ERR((((uint32_t)acceptbfmeeframes << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00004000)) | ((uint32_t)acceptbfmeeframes << 14));
}

/**
 * @brief Returns the current value of the acceptAllBeacon field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptAllBeacon field's value will be returned.
 *
 * @return The current value of the acceptAllBeacon field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_all_beacon_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the acceptAllBeacon field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptallbeacon - The value to set the field to.
 */
__INLINE void nxmac_accept_all_beacon_setf(uint8_t acceptallbeacon)
{
    ASSERT_ERR((((uint32_t)acceptallbeacon << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)acceptallbeacon << 13));
}

/**
 * @brief Returns the current value of the acceptNotExpectedBA field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptNotExpectedBA field's value will be returned.
 *
 * @return The current value of the acceptNotExpectedBA field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_not_expected_ba_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the acceptNotExpectedBA field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptnotexpectedba - The value to set the field to.
 */
__INLINE void nxmac_accept_not_expected_ba_setf(uint8_t acceptnotexpectedba)
{
    ASSERT_ERR((((uint32_t)acceptnotexpectedba << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)acceptnotexpectedba << 12));
}

/**
 * @brief Returns the current value of the acceptDecryptErrorFrames field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptDecryptErrorFrames field's value will be returned.
 *
 * @return The current value of the acceptDecryptErrorFrames field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_decrypt_error_frames_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the acceptDecryptErrorFrames field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptdecrypterrorframes - The value to set the field to.
 */
__INLINE void nxmac_accept_decrypt_error_frames_setf(uint8_t acceptdecrypterrorframes)
{
    ASSERT_ERR((((uint32_t)acceptdecrypterrorframes << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00000800)) | ((uint32_t)acceptdecrypterrorframes << 11));
}

/**
 * @brief Returns the current value of the acceptBeacon field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptBeacon field's value will be returned.
 *
 * @return The current value of the acceptBeacon field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_beacon_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the acceptBeacon field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptbeacon - The value to set the field to.
 */
__INLINE void nxmac_accept_beacon_setf(uint8_t acceptbeacon)
{
    ASSERT_ERR((((uint32_t)acceptbeacon << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00000400)) | ((uint32_t)acceptbeacon << 10));
}

/**
 * @brief Returns the current value of the acceptProbeResp field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptProbeResp field's value will be returned.
 *
 * @return The current value of the acceptProbeResp field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_probe_resp_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the acceptProbeResp field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptproberesp - The value to set the field to.
 */
__INLINE void nxmac_accept_probe_resp_setf(uint8_t acceptproberesp)
{
    ASSERT_ERR((((uint32_t)acceptproberesp << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)acceptproberesp << 9));
}

/**
 * @brief Returns the current value of the acceptProbeReq field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptProbeReq field's value will be returned.
 *
 * @return The current value of the acceptProbeReq field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_probe_req_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the acceptProbeReq field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptprobereq - The value to set the field to.
 */
__INLINE void nxmac_accept_probe_req_setf(uint8_t acceptprobereq)
{
    ASSERT_ERR((((uint32_t)acceptprobereq << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)acceptprobereq << 8));
}

/**
 * @brief Returns the current value of the acceptMyUnicast field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptMyUnicast field's value will be returned.
 *
 * @return The current value of the acceptMyUnicast field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_my_unicast_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the acceptMyUnicast field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptmyunicast - The value to set the field to.
 */
__INLINE void nxmac_accept_my_unicast_setf(uint8_t acceptmyunicast)
{
    ASSERT_ERR((((uint32_t)acceptmyunicast << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)acceptmyunicast << 7));
}

/**
 * @brief Returns the current value of the acceptUnicast field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptUnicast field's value will be returned.
 *
 * @return The current value of the acceptUnicast field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_unicast_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the acceptUnicast field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptunicast - The value to set the field to.
 */
__INLINE void nxmac_accept_unicast_setf(uint8_t acceptunicast)
{
    ASSERT_ERR((((uint32_t)acceptunicast << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)acceptunicast << 6));
}

/**
 * @brief Returns the current value of the acceptErrorFrames field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptErrorFrames field's value will be returned.
 *
 * @return The current value of the acceptErrorFrames field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_error_frames_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the acceptErrorFrames field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] accepterrorframes - The value to set the field to.
 */
__INLINE void nxmac_accept_error_frames_setf(uint8_t accepterrorframes)
{
    ASSERT_ERR((((uint32_t)accepterrorframes << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)accepterrorframes << 5));
}

/**
 * @brief Returns the current value of the acceptOtherBSSID field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptOtherBSSID field's value will be returned.
 *
 * @return The current value of the acceptOtherBSSID field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_other_bssid_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the acceptOtherBSSID field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptotherbssid - The value to set the field to.
 */
__INLINE void nxmac_accept_other_bssid_setf(uint8_t acceptotherbssid)
{
    ASSERT_ERR((((uint32_t)acceptotherbssid << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)acceptotherbssid << 4));
}

/**
 * @brief Returns the current value of the acceptBroadcast field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptBroadcast field's value will be returned.
 *
 * @return The current value of the acceptBroadcast field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_broadcast_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the acceptBroadcast field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptbroadcast - The value to set the field to.
 */
__INLINE void nxmac_accept_broadcast_setf(uint8_t acceptbroadcast)
{
    ASSERT_ERR((((uint32_t)acceptbroadcast << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)acceptbroadcast << 3));
}

/**
 * @brief Returns the current value of the acceptMulticast field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the acceptMulticast field's value will be returned.
 *
 * @return The current value of the acceptMulticast field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_accept_multicast_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the acceptMulticast field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] acceptmulticast - The value to set the field to.
 */
__INLINE void nxmac_accept_multicast_setf(uint8_t acceptmulticast)
{
    ASSERT_ERR((((uint32_t)acceptmulticast << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)acceptmulticast << 2));
}

/**
 * @brief Returns the current value of the dontDecrypt field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the dontDecrypt field's value will be returned.
 *
 * @return The current value of the dontDecrypt field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_dont_decrypt_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the dontDecrypt field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dontdecrypt - The value to set the field to.
 */
__INLINE void nxmac_dont_decrypt_setf(uint8_t dontdecrypt)
{
    ASSERT_ERR((((uint32_t)dontdecrypt << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)dontdecrypt << 1));
}

/**
 * @brief Returns the current value of the excUnencrypted field in the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read and the excUnencrypted field's value will be returned.
 *
 * @return The current value of the excUnencrypted field in the RX_CNTRL register.
 */
__INLINE uint8_t nxmac_exc_unencrypted_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the excUnencrypted field of the RX_CNTRL register.
 *
 * The RX_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] excunencrypted - The value to set the field to.
 */
__INLINE void nxmac_exc_unencrypted_setf(uint8_t excunencrypted)
{
    ASSERT_ERR((((uint32_t)excunencrypted << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_RX_CNTRL_ADDR, (REG_PL_RD(NXMAC_RX_CNTRL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)excunencrypted << 0));
}

/// @}

/**
 * @name BCN_CNTRL_1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:24          noBcnTxTime   0x0
 *     23       impTBTTIn128Us   0
 *  22:16        impTBTTPeriod   0x0
 *  15:00            beaconInt   0x0
 * </pre>
 *
 * @{
 */

/// Address of the BCN_CNTRL_1 register
#define NXMAC_BCN_CNTRL_1_ADDR   0xC0000064
/// Offset of the BCN_CNTRL_1 register from the base address
#define NXMAC_BCN_CNTRL_1_OFFSET 0x00000064
/// Index of the BCN_CNTRL_1 register
#define NXMAC_BCN_CNTRL_1_INDEX  0x00000019
/// Reset value of the BCN_CNTRL_1 register
#define NXMAC_BCN_CNTRL_1_RESET  0x00000000

/**
 * @brief Returns the current value of the BCN_CNTRL_1 register.
 * The BCN_CNTRL_1 register will be read and its value returned.
 * @return The current value of the BCN_CNTRL_1 register.
 */
__INLINE uint32_t nxmac_bcn_cntrl_1_get(void)
{
    return REG_PL_RD(NXMAC_BCN_CNTRL_1_ADDR);
}

/**
 * @brief Sets the BCN_CNTRL_1 register to a value.
 * The BCN_CNTRL_1 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_bcn_cntrl_1_set(uint32_t value)
{
    REG_PL_WR(NXMAC_BCN_CNTRL_1_ADDR, value);
}

// field definitions
/// NO_BCN_TX_TIME field mask
#define NXMAC_NO_BCN_TX_TIME_MASK       ((uint32_t)0xFF000000)
/// NO_BCN_TX_TIME field LSB position
#define NXMAC_NO_BCN_TX_TIME_LSB        24
/// NO_BCN_TX_TIME field width
#define NXMAC_NO_BCN_TX_TIME_WIDTH      ((uint32_t)0x00000008)
/// IMP_TBTT_IN_128_US field bit
#define NXMAC_IMP_TBTT_IN_128_US_BIT    ((uint32_t)0x00800000)
/// IMP_TBTT_IN_128_US field position
#define NXMAC_IMP_TBTT_IN_128_US_POS    23
/// IMP_TBTT_PERIOD field mask
#define NXMAC_IMP_TBTT_PERIOD_MASK      ((uint32_t)0x007F0000)
/// IMP_TBTT_PERIOD field LSB position
#define NXMAC_IMP_TBTT_PERIOD_LSB       16
/// IMP_TBTT_PERIOD field width
#define NXMAC_IMP_TBTT_PERIOD_WIDTH     ((uint32_t)0x00000007)
/// BEACON_INT field mask
#define NXMAC_BEACON_INT_MASK           ((uint32_t)0x0000FFFF)
/// BEACON_INT field LSB position
#define NXMAC_BEACON_INT_LSB            0
/// BEACON_INT field width
#define NXMAC_BEACON_INT_WIDTH          ((uint32_t)0x00000010)

/// NO_BCN_TX_TIME field reset value
#define NXMAC_NO_BCN_TX_TIME_RST        0x0
/// IMP_TBTT_IN_128_US field reset value
#define NXMAC_IMP_TBTT_IN_128_US_RST    0x0
/// IMP_TBTT_PERIOD field reset value
#define NXMAC_IMP_TBTT_PERIOD_RST       0x0
/// BEACON_INT field reset value
#define NXMAC_BEACON_INT_RST            0x0

/**
 * @brief Constructs a value for the BCN_CNTRL_1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] nobcntxtime - The value to use for the noBcnTxTime field.
 * @param[in] imptbttin128us - The value to use for the impTBTTIn128Us field.
 * @param[in] imptbttperiod - The value to use for the impTBTTPeriod field.
 * @param[in] beaconint - The value to use for the beaconInt field.
 */
__INLINE void nxmac_bcn_cntrl_1_pack(uint8_t nobcntxtime, uint8_t imptbttin128us, uint8_t imptbttperiod, uint16_t beaconint)
{
    ASSERT_ERR((((uint32_t)nobcntxtime << 24) & ~((uint32_t)0xFF000000)) == 0);
    ASSERT_ERR((((uint32_t)imptbttin128us << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)imptbttperiod << 16) & ~((uint32_t)0x007F0000)) == 0);
    ASSERT_ERR((((uint32_t)beaconint << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_BCN_CNTRL_1_ADDR,  ((uint32_t)nobcntxtime << 24) | ((uint32_t)imptbttin128us << 23) | ((uint32_t)imptbttperiod << 16) | ((uint32_t)beaconint << 0));
}

/**
 * @brief Unpacks BCN_CNTRL_1's fields from current value of the BCN_CNTRL_1 register.
 *
 * Reads the BCN_CNTRL_1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] nobcntxtime - Will be populated with the current value of this field from the register.
 * @param[out] imptbttin128us - Will be populated with the current value of this field from the register.
 * @param[out] imptbttperiod - Will be populated with the current value of this field from the register.
 * @param[out] beaconint - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_bcn_cntrl_1_unpack(uint8_t *nobcntxtime, uint8_t *imptbttin128us, uint8_t *imptbttperiod, uint16_t *beaconint)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BCN_CNTRL_1_ADDR);

    *nobcntxtime = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *imptbttin128us = (localVal & ((uint32_t)0x00800000)) >> 23;
    *imptbttperiod = (localVal & ((uint32_t)0x007F0000)) >> 16;
    *beaconint = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the noBcnTxTime field in the BCN_CNTRL_1 register.
 *
 * The BCN_CNTRL_1 register will be read and the noBcnTxTime field's value will be returned.
 *
 * @return The current value of the noBcnTxTime field in the BCN_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_no_bcn_tx_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BCN_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Sets the noBcnTxTime field of the BCN_CNTRL_1 register.
 *
 * The BCN_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nobcntxtime - The value to set the field to.
 */
__INLINE void nxmac_no_bcn_tx_time_setf(uint8_t nobcntxtime)
{
    ASSERT_ERR((((uint32_t)nobcntxtime << 24) & ~((uint32_t)0xFF000000)) == 0);
    REG_PL_WR(NXMAC_BCN_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_BCN_CNTRL_1_ADDR) & ~((uint32_t)0xFF000000)) | ((uint32_t)nobcntxtime << 24));
}

/**
 * @brief Returns the current value of the impTBTTIn128Us field in the BCN_CNTRL_1 register.
 *
 * The BCN_CNTRL_1 register will be read and the impTBTTIn128Us field's value will be returned.
 *
 * @return The current value of the impTBTTIn128Us field in the BCN_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_imp_tbtt_in_128_us_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BCN_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Sets the impTBTTIn128Us field of the BCN_CNTRL_1 register.
 *
 * The BCN_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] imptbttin128us - The value to set the field to.
 */
__INLINE void nxmac_imp_tbtt_in_128_us_setf(uint8_t imptbttin128us)
{
    ASSERT_ERR((((uint32_t)imptbttin128us << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(NXMAC_BCN_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_BCN_CNTRL_1_ADDR) & ~((uint32_t)0x00800000)) | ((uint32_t)imptbttin128us << 23));
}

/**
 * @brief Returns the current value of the impTBTTPeriod field in the BCN_CNTRL_1 register.
 *
 * The BCN_CNTRL_1 register will be read and the impTBTTPeriod field's value will be returned.
 *
 * @return The current value of the impTBTTPeriod field in the BCN_CNTRL_1 register.
 */
__INLINE uint8_t nxmac_imp_tbtt_period_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BCN_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x007F0000)) >> 16);
}

/**
 * @brief Sets the impTBTTPeriod field of the BCN_CNTRL_1 register.
 *
 * The BCN_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] imptbttperiod - The value to set the field to.
 */
__INLINE void nxmac_imp_tbtt_period_setf(uint8_t imptbttperiod)
{
    ASSERT_ERR((((uint32_t)imptbttperiod << 16) & ~((uint32_t)0x007F0000)) == 0);
    REG_PL_WR(NXMAC_BCN_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_BCN_CNTRL_1_ADDR) & ~((uint32_t)0x007F0000)) | ((uint32_t)imptbttperiod << 16));
}

/**
 * @brief Returns the current value of the beaconInt field in the BCN_CNTRL_1 register.
 *
 * The BCN_CNTRL_1 register will be read and the beaconInt field's value will be returned.
 *
 * @return The current value of the beaconInt field in the BCN_CNTRL_1 register.
 */
__INLINE uint16_t nxmac_beacon_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BCN_CNTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the beaconInt field of the BCN_CNTRL_1 register.
 *
 * The BCN_CNTRL_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] beaconint - The value to set the field to.
 */
__INLINE void nxmac_beacon_int_setf(uint16_t beaconint)
{
    ASSERT_ERR((((uint32_t)beaconint << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_BCN_CNTRL_1_ADDR, (REG_PL_RD(NXMAC_BCN_CNTRL_1_ADDR) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)beaconint << 0));
}

/// @}

/**
 * @name BCN_CNTRL_2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:16                  aid   0x0
 *  15:08            timOffset   0x0
 *  07:00      bcnUpdateOffset   0x0
 * </pre>
 *
 * @{
 */

/// Address of the BCN_CNTRL_2 register
#define NXMAC_BCN_CNTRL_2_ADDR   0xC0000068
/// Offset of the BCN_CNTRL_2 register from the base address
#define NXMAC_BCN_CNTRL_2_OFFSET 0x00000068
/// Index of the BCN_CNTRL_2 register
#define NXMAC_BCN_CNTRL_2_INDEX  0x0000001A
/// Reset value of the BCN_CNTRL_2 register
#define NXMAC_BCN_CNTRL_2_RESET  0x00000000

/**
 * @brief Returns the current value of the BCN_CNTRL_2 register.
 * The BCN_CNTRL_2 register will be read and its value returned.
 * @return The current value of the BCN_CNTRL_2 register.
 */
__INLINE uint32_t nxmac_bcn_cntrl_2_get(void)
{
    return REG_PL_RD(NXMAC_BCN_CNTRL_2_ADDR);
}

/**
 * @brief Sets the BCN_CNTRL_2 register to a value.
 * The BCN_CNTRL_2 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_bcn_cntrl_2_set(uint32_t value)
{
    REG_PL_WR(NXMAC_BCN_CNTRL_2_ADDR, value);
}

// field definitions
/// AID field mask
#define NXMAC_AID_MASK                 ((uint32_t)0xFFFF0000)
/// AID field LSB position
#define NXMAC_AID_LSB                  16
/// AID field width
#define NXMAC_AID_WIDTH                ((uint32_t)0x00000010)
/// TIM_OFFSET field mask
#define NXMAC_TIM_OFFSET_MASK          ((uint32_t)0x0000FF00)
/// TIM_OFFSET field LSB position
#define NXMAC_TIM_OFFSET_LSB           8
/// TIM_OFFSET field width
#define NXMAC_TIM_OFFSET_WIDTH         ((uint32_t)0x00000008)
/// BCN_UPDATE_OFFSET field mask
#define NXMAC_BCN_UPDATE_OFFSET_MASK   ((uint32_t)0x000000FF)
/// BCN_UPDATE_OFFSET field LSB position
#define NXMAC_BCN_UPDATE_OFFSET_LSB    0
/// BCN_UPDATE_OFFSET field width
#define NXMAC_BCN_UPDATE_OFFSET_WIDTH  ((uint32_t)0x00000008)

/// AID field reset value
#define NXMAC_AID_RST                  0x0
/// TIM_OFFSET field reset value
#define NXMAC_TIM_OFFSET_RST           0x0
/// BCN_UPDATE_OFFSET field reset value
#define NXMAC_BCN_UPDATE_OFFSET_RST    0x0

/**
 * @brief Constructs a value for the BCN_CNTRL_2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] aid - The value to use for the aid field.
 * @param[in] timoffset - The value to use for the timOffset field.
 * @param[in] bcnupdateoffset - The value to use for the bcnUpdateOffset field.
 */
__INLINE void nxmac_bcn_cntrl_2_pack(uint16_t aid, uint8_t timoffset, uint8_t bcnupdateoffset)
{
    ASSERT_ERR((((uint32_t)aid << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    ASSERT_ERR((((uint32_t)timoffset << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)bcnupdateoffset << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_BCN_CNTRL_2_ADDR,  ((uint32_t)aid << 16) | ((uint32_t)timoffset << 8) | ((uint32_t)bcnupdateoffset << 0));
}

/**
 * @brief Unpacks BCN_CNTRL_2's fields from current value of the BCN_CNTRL_2 register.
 *
 * Reads the BCN_CNTRL_2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] aid - Will be populated with the current value of this field from the register.
 * @param[out] timoffset - Will be populated with the current value of this field from the register.
 * @param[out] bcnupdateoffset - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_bcn_cntrl_2_unpack(uint16_t *aid, uint8_t *timoffset, uint8_t *bcnupdateoffset)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BCN_CNTRL_2_ADDR);

    *aid = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *timoffset = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *bcnupdateoffset = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the aid field in the BCN_CNTRL_2 register.
 *
 * The BCN_CNTRL_2 register will be read and the aid field's value will be returned.
 *
 * @return The current value of the aid field in the BCN_CNTRL_2 register.
 */
__INLINE uint16_t nxmac_aid_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BCN_CNTRL_2_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Sets the aid field of the BCN_CNTRL_2 register.
 *
 * The BCN_CNTRL_2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aid - The value to set the field to.
 */
__INLINE void nxmac_aid_setf(uint16_t aid)
{
    ASSERT_ERR((((uint32_t)aid << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    REG_PL_WR(NXMAC_BCN_CNTRL_2_ADDR, (REG_PL_RD(NXMAC_BCN_CNTRL_2_ADDR) & ~((uint32_t)0xFFFF0000)) | ((uint32_t)aid << 16));
}

/**
 * @brief Returns the current value of the timOffset field in the BCN_CNTRL_2 register.
 *
 * The BCN_CNTRL_2 register will be read and the timOffset field's value will be returned.
 *
 * @return The current value of the timOffset field in the BCN_CNTRL_2 register.
 */
__INLINE uint8_t nxmac_tim_offset_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BCN_CNTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the timOffset field of the BCN_CNTRL_2 register.
 *
 * The BCN_CNTRL_2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] timoffset - The value to set the field to.
 */
__INLINE void nxmac_tim_offset_setf(uint8_t timoffset)
{
    ASSERT_ERR((((uint32_t)timoffset << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(NXMAC_BCN_CNTRL_2_ADDR, (REG_PL_RD(NXMAC_BCN_CNTRL_2_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)timoffset << 8));
}

/**
 * @brief Returns the current value of the bcnUpdateOffset field in the BCN_CNTRL_2 register.
 *
 * The BCN_CNTRL_2 register will be read and the bcnUpdateOffset field's value will be returned.
 *
 * @return The current value of the bcnUpdateOffset field in the BCN_CNTRL_2 register.
 */
__INLINE uint8_t nxmac_bcn_update_offset_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BCN_CNTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the bcnUpdateOffset field of the BCN_CNTRL_2 register.
 *
 * The BCN_CNTRL_2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bcnupdateoffset - The value to set the field to.
 */
__INLINE void nxmac_bcn_update_offset_setf(uint8_t bcnupdateoffset)
{
    ASSERT_ERR((((uint32_t)bcnupdateoffset << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_BCN_CNTRL_2_ADDR, (REG_PL_RD(NXMAC_BCN_CNTRL_2_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)bcnupdateoffset << 0));
}

/// @}

/**
 * @name DTIM_CFP_1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31      dtimUpdatedBySW   0
 *  15:08            cfpPeriod   0x0
 *  07:00           dtimPeriod   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DTIM_CFP_1 register
#define NXMAC_DTIM_CFP_1_ADDR   0xC0000090
/// Offset of the DTIM_CFP_1 register from the base address
#define NXMAC_DTIM_CFP_1_OFFSET 0x00000090
/// Index of the DTIM_CFP_1 register
#define NXMAC_DTIM_CFP_1_INDEX  0x00000024
/// Reset value of the DTIM_CFP_1 register
#define NXMAC_DTIM_CFP_1_RESET  0x00000000

/**
 * @brief Returns the current value of the DTIM_CFP_1 register.
 * The DTIM_CFP_1 register will be read and its value returned.
 * @return The current value of the DTIM_CFP_1 register.
 */
__INLINE uint32_t nxmac_dtim_cfp_1_get(void)
{
    return REG_PL_RD(NXMAC_DTIM_CFP_1_ADDR);
}

/**
 * @brief Sets the DTIM_CFP_1 register to a value.
 * The DTIM_CFP_1 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_dtim_cfp_1_set(uint32_t value)
{
    REG_PL_WR(NXMAC_DTIM_CFP_1_ADDR, value);
}

// field definitions
/// DTIM_UPDATED_BY_SW field bit
#define NXMAC_DTIM_UPDATED_BY_SW_BIT    ((uint32_t)0x80000000)
/// DTIM_UPDATED_BY_SW field position
#define NXMAC_DTIM_UPDATED_BY_SW_POS    31
/// CFP_PERIOD field mask
#define NXMAC_CFP_PERIOD_MASK           ((uint32_t)0x0000FF00)
/// CFP_PERIOD field LSB position
#define NXMAC_CFP_PERIOD_LSB            8
/// CFP_PERIOD field width
#define NXMAC_CFP_PERIOD_WIDTH          ((uint32_t)0x00000008)
/// DTIM_PERIOD field mask
#define NXMAC_DTIM_PERIOD_MASK          ((uint32_t)0x000000FF)
/// DTIM_PERIOD field LSB position
#define NXMAC_DTIM_PERIOD_LSB           0
/// DTIM_PERIOD field width
#define NXMAC_DTIM_PERIOD_WIDTH         ((uint32_t)0x00000008)

/// DTIM_UPDATED_BY_SW field reset value
#define NXMAC_DTIM_UPDATED_BY_SW_RST    0x0
/// CFP_PERIOD field reset value
#define NXMAC_CFP_PERIOD_RST            0x0
/// DTIM_PERIOD field reset value
#define NXMAC_DTIM_PERIOD_RST           0x0

/**
 * @brief Constructs a value for the DTIM_CFP_1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] dtimupdatedbysw - The value to use for the dtimUpdatedBySW field.
 * @param[in] cfpperiod - The value to use for the cfpPeriod field.
 * @param[in] dtimperiod - The value to use for the dtimPeriod field.
 */
__INLINE void nxmac_dtim_cfp_1_pack(uint8_t dtimupdatedbysw, uint8_t cfpperiod, uint8_t dtimperiod)
{
    ASSERT_ERR((((uint32_t)dtimupdatedbysw << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)cfpperiod << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)dtimperiod << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_DTIM_CFP_1_ADDR,  ((uint32_t)dtimupdatedbysw << 31) | ((uint32_t)cfpperiod << 8) | ((uint32_t)dtimperiod << 0));
}

/**
 * @brief Unpacks DTIM_CFP_1's fields from current value of the DTIM_CFP_1 register.
 *
 * Reads the DTIM_CFP_1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] dtimupdatedbysw - Will be populated with the current value of this field from the register.
 * @param[out] cfpperiod - Will be populated with the current value of this field from the register.
 * @param[out] dtimperiod - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_dtim_cfp_1_unpack(uint8_t *dtimupdatedbysw, uint8_t *cfpperiod, uint8_t *dtimperiod)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DTIM_CFP_1_ADDR);

    *dtimupdatedbysw = (localVal & ((uint32_t)0x80000000)) >> 31;
    *cfpperiod = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *dtimperiod = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the dtimUpdatedBySW field in the DTIM_CFP_1 register.
 *
 * The DTIM_CFP_1 register will be read and the dtimUpdatedBySW field's value will be returned.
 *
 * @return The current value of the dtimUpdatedBySW field in the DTIM_CFP_1 register.
 */
__INLINE uint8_t nxmac_dtim_updated_by_sw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DTIM_CFP_1_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the dtimUpdatedBySW field of the DTIM_CFP_1 register.
 *
 * The DTIM_CFP_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dtimupdatedbysw - The value to set the field to.
 */
__INLINE void nxmac_dtim_updated_by_sw_setf(uint8_t dtimupdatedbysw)
{
    ASSERT_ERR((((uint32_t)dtimupdatedbysw << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(NXMAC_DTIM_CFP_1_ADDR, (REG_PL_RD(NXMAC_DTIM_CFP_1_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)dtimupdatedbysw << 31));
}

/**
 * @brief Returns the current value of the cfpPeriod field in the DTIM_CFP_1 register.
 *
 * The DTIM_CFP_1 register will be read and the cfpPeriod field's value will be returned.
 *
 * @return The current value of the cfpPeriod field in the DTIM_CFP_1 register.
 */
__INLINE uint8_t nxmac_cfp_period_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DTIM_CFP_1_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the cfpPeriod field of the DTIM_CFP_1 register.
 *
 * The DTIM_CFP_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cfpperiod - The value to set the field to.
 */
__INLINE void nxmac_cfp_period_setf(uint8_t cfpperiod)
{
    ASSERT_ERR((((uint32_t)cfpperiod << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(NXMAC_DTIM_CFP_1_ADDR, (REG_PL_RD(NXMAC_DTIM_CFP_1_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)cfpperiod << 8));
}

/**
 * @brief Returns the current value of the dtimPeriod field in the DTIM_CFP_1 register.
 *
 * The DTIM_CFP_1 register will be read and the dtimPeriod field's value will be returned.
 *
 * @return The current value of the dtimPeriod field in the DTIM_CFP_1 register.
 */
__INLINE uint8_t nxmac_dtim_period_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DTIM_CFP_1_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the dtimPeriod field of the DTIM_CFP_1 register.
 *
 * The DTIM_CFP_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dtimperiod - The value to set the field to.
 */
__INLINE void nxmac_dtim_period_setf(uint8_t dtimperiod)
{
    ASSERT_ERR((((uint32_t)dtimperiod << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_DTIM_CFP_1_ADDR, (REG_PL_RD(NXMAC_DTIM_CFP_1_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)dtimperiod << 0));
}

/// @}

/**
 * @name DTIM_CFP_2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00       cfpMaxDuration   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DTIM_CFP_2 register
#define NXMAC_DTIM_CFP_2_ADDR   0xC0000094
/// Offset of the DTIM_CFP_2 register from the base address
#define NXMAC_DTIM_CFP_2_OFFSET 0x00000094
/// Index of the DTIM_CFP_2 register
#define NXMAC_DTIM_CFP_2_INDEX  0x00000025
/// Reset value of the DTIM_CFP_2 register
#define NXMAC_DTIM_CFP_2_RESET  0x00000000

/**
 * @brief Returns the current value of the DTIM_CFP_2 register.
 * The DTIM_CFP_2 register will be read and its value returned.
 * @return The current value of the DTIM_CFP_2 register.
 */
__INLINE uint32_t nxmac_dtim_cfp_2_get(void)
{
    return REG_PL_RD(NXMAC_DTIM_CFP_2_ADDR);
}

/**
 * @brief Sets the DTIM_CFP_2 register to a value.
 * The DTIM_CFP_2 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_dtim_cfp_2_set(uint32_t value)
{
    REG_PL_WR(NXMAC_DTIM_CFP_2_ADDR, value);
}

// field definitions
/// CFP_MAX_DURATION field mask
#define NXMAC_CFP_MAX_DURATION_MASK   ((uint32_t)0x0000FFFF)
/// CFP_MAX_DURATION field LSB position
#define NXMAC_CFP_MAX_DURATION_LSB    0
/// CFP_MAX_DURATION field width
#define NXMAC_CFP_MAX_DURATION_WIDTH  ((uint32_t)0x00000010)

/// CFP_MAX_DURATION field reset value
#define NXMAC_CFP_MAX_DURATION_RST    0x0

/**
 * @brief Returns the current value of the cfpMaxDuration field in the DTIM_CFP_2 register.
 *
 * The DTIM_CFP_2 register will be read and the cfpMaxDuration field's value will be returned.
 *
 * @return The current value of the cfpMaxDuration field in the DTIM_CFP_2 register.
 */
__INLINE uint16_t nxmac_cfp_max_duration_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DTIM_CFP_2_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the cfpMaxDuration field of the DTIM_CFP_2 register.
 *
 * The DTIM_CFP_2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cfpmaxduration - The value to set the field to.
 */
__INLINE void nxmac_cfp_max_duration_setf(uint16_t cfpmaxduration)
{
    ASSERT_ERR((((uint32_t)cfpmaxduration << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_DTIM_CFP_2_ADDR, (uint32_t)cfpmaxduration << 0);
}

/// @}

/**
 * @name RETRY_LIMITS register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:08   dot11LongRetryLimit   0x4
 *  07:00   dot11ShortRetryLimit   0x7
 * </pre>
 *
 * @{
 */

/// Address of the RETRY_LIMITS register
#define NXMAC_RETRY_LIMITS_ADDR   0xC0000098
/// Offset of the RETRY_LIMITS register from the base address
#define NXMAC_RETRY_LIMITS_OFFSET 0x00000098
/// Index of the RETRY_LIMITS register
#define NXMAC_RETRY_LIMITS_INDEX  0x00000026
/// Reset value of the RETRY_LIMITS register
#define NXMAC_RETRY_LIMITS_RESET  0x00000407

/**
 * @brief Returns the current value of the RETRY_LIMITS register.
 * The RETRY_LIMITS register will be read and its value returned.
 * @return The current value of the RETRY_LIMITS register.
 */
__INLINE uint32_t nxmac_retry_limits_get(void)
{
    return REG_PL_RD(NXMAC_RETRY_LIMITS_ADDR);
}

/**
 * @brief Sets the RETRY_LIMITS register to a value.
 * The RETRY_LIMITS register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_retry_limits_set(uint32_t value)
{
    REG_PL_WR(NXMAC_RETRY_LIMITS_ADDR, value);
}

// field definitions
/// DOT_11_LONG_RETRY_LIMIT field mask
#define NXMAC_DOT_11_LONG_RETRY_LIMIT_MASK    ((uint32_t)0x0000FF00)
/// DOT_11_LONG_RETRY_LIMIT field LSB position
#define NXMAC_DOT_11_LONG_RETRY_LIMIT_LSB     8
/// DOT_11_LONG_RETRY_LIMIT field width
#define NXMAC_DOT_11_LONG_RETRY_LIMIT_WIDTH   ((uint32_t)0x00000008)
/// DOT_11_SHORT_RETRY_LIMIT field mask
#define NXMAC_DOT_11_SHORT_RETRY_LIMIT_MASK   ((uint32_t)0x000000FF)
/// DOT_11_SHORT_RETRY_LIMIT field LSB position
#define NXMAC_DOT_11_SHORT_RETRY_LIMIT_LSB    0
/// DOT_11_SHORT_RETRY_LIMIT field width
#define NXMAC_DOT_11_SHORT_RETRY_LIMIT_WIDTH  ((uint32_t)0x00000008)

/// DOT_11_LONG_RETRY_LIMIT field reset value
#define NXMAC_DOT_11_LONG_RETRY_LIMIT_RST     0x4
/// DOT_11_SHORT_RETRY_LIMIT field reset value
#define NXMAC_DOT_11_SHORT_RETRY_LIMIT_RST    0x7

/**
 * @brief Constructs a value for the RETRY_LIMITS register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] dot11longretrylimit - The value to use for the dot11LongRetryLimit field.
 * @param[in] dot11shortretrylimit - The value to use for the dot11ShortRetryLimit field.
 */
__INLINE void nxmac_retry_limits_pack(uint8_t dot11longretrylimit, uint8_t dot11shortretrylimit)
{
    ASSERT_ERR((((uint32_t)dot11longretrylimit << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)dot11shortretrylimit << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_RETRY_LIMITS_ADDR,  ((uint32_t)dot11longretrylimit << 8) | ((uint32_t)dot11shortretrylimit << 0));
}

/**
 * @brief Unpacks RETRY_LIMITS's fields from current value of the RETRY_LIMITS register.
 *
 * Reads the RETRY_LIMITS register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] dot11longretrylimit - Will be populated with the current value of this field from the register.
 * @param[out] dot11shortretrylimit - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_retry_limits_unpack(uint8_t *dot11longretrylimit, uint8_t *dot11shortretrylimit)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RETRY_LIMITS_ADDR);

    *dot11longretrylimit = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *dot11shortretrylimit = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the dot11LongRetryLimit field in the RETRY_LIMITS register.
 *
 * The RETRY_LIMITS register will be read and the dot11LongRetryLimit field's value will be returned.
 *
 * @return The current value of the dot11LongRetryLimit field in the RETRY_LIMITS register.
 */
__INLINE uint8_t nxmac_dot_11_long_retry_limit_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RETRY_LIMITS_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the dot11LongRetryLimit field of the RETRY_LIMITS register.
 *
 * The RETRY_LIMITS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dot11longretrylimit - The value to set the field to.
 */
__INLINE void nxmac_dot_11_long_retry_limit_setf(uint8_t dot11longretrylimit)
{
    ASSERT_ERR((((uint32_t)dot11longretrylimit << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(NXMAC_RETRY_LIMITS_ADDR, (REG_PL_RD(NXMAC_RETRY_LIMITS_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)dot11longretrylimit << 8));
}

/**
 * @brief Returns the current value of the dot11ShortRetryLimit field in the RETRY_LIMITS register.
 *
 * The RETRY_LIMITS register will be read and the dot11ShortRetryLimit field's value will be returned.
 *
 * @return The current value of the dot11ShortRetryLimit field in the RETRY_LIMITS register.
 */
__INLINE uint8_t nxmac_dot_11_short_retry_limit_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RETRY_LIMITS_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the dot11ShortRetryLimit field of the RETRY_LIMITS register.
 *
 * The RETRY_LIMITS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dot11shortretrylimit - The value to set the field to.
 */
__INLINE void nxmac_dot_11_short_retry_limit_setf(uint8_t dot11shortretrylimit)
{
    ASSERT_ERR((((uint32_t)dot11shortretrylimit << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_RETRY_LIMITS_ADDR, (REG_PL_RD(NXMAC_RETRY_LIMITS_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)dot11shortretrylimit << 0));
}

/// @}

/**
 * @name BB_SERVICE register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  28:26            maxPHYNtx   0x2
 *  23:16           bbServiceB   0x0
 *  15:00           bbServiceA   0x0
 * </pre>
 *
 * @{
 */

/// Address of the BB_SERVICE register
#define NXMAC_BB_SERVICE_ADDR   0xC000009C
/// Offset of the BB_SERVICE register from the base address
#define NXMAC_BB_SERVICE_OFFSET 0x0000009C
/// Index of the BB_SERVICE register
#define NXMAC_BB_SERVICE_INDEX  0x00000027
/// Reset value of the BB_SERVICE register
#define NXMAC_BB_SERVICE_RESET  0x08000000

/**
 * @brief Returns the current value of the BB_SERVICE register.
 * The BB_SERVICE register will be read and its value returned.
 * @return The current value of the BB_SERVICE register.
 */
__INLINE uint32_t nxmac_bb_service_get(void)
{
    return REG_PL_RD(NXMAC_BB_SERVICE_ADDR);
}

/**
 * @brief Sets the BB_SERVICE register to a value.
 * The BB_SERVICE register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_bb_service_set(uint32_t value)
{
    REG_PL_WR(NXMAC_BB_SERVICE_ADDR, value);
}

// field definitions
/// MAX_PHY_NTX field mask
#define NXMAC_MAX_PHY_NTX_MASK    ((uint32_t)0x1C000000)
/// MAX_PHY_NTX field LSB position
#define NXMAC_MAX_PHY_NTX_LSB     26
/// MAX_PHY_NTX field width
#define NXMAC_MAX_PHY_NTX_WIDTH   ((uint32_t)0x00000003)
/// BB_SERVICE_B field mask
#define NXMAC_BB_SERVICE_B_MASK   ((uint32_t)0x00FF0000)
/// BB_SERVICE_B field LSB position
#define NXMAC_BB_SERVICE_B_LSB    16
/// BB_SERVICE_B field width
#define NXMAC_BB_SERVICE_B_WIDTH  ((uint32_t)0x00000008)
/// BB_SERVICE_A field mask
#define NXMAC_BB_SERVICE_A_MASK   ((uint32_t)0x0000FFFF)
/// BB_SERVICE_A field LSB position
#define NXMAC_BB_SERVICE_A_LSB    0
/// BB_SERVICE_A field width
#define NXMAC_BB_SERVICE_A_WIDTH  ((uint32_t)0x00000010)

/// MAX_PHY_NTX field reset value
#define NXMAC_MAX_PHY_NTX_RST     0x2
/// BB_SERVICE_B field reset value
#define NXMAC_BB_SERVICE_B_RST    0x0
/// BB_SERVICE_A field reset value
#define NXMAC_BB_SERVICE_A_RST    0x0

/**
 * @brief Constructs a value for the BB_SERVICE register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] maxphyntx - The value to use for the maxPHYNtx field.
 * @param[in] bbserviceb - The value to use for the bbServiceB field.
 * @param[in] bbservicea - The value to use for the bbServiceA field.
 */
__INLINE void nxmac_bb_service_pack(uint8_t maxphyntx, uint8_t bbserviceb, uint16_t bbservicea)
{
    ASSERT_ERR((((uint32_t)maxphyntx << 26) & ~((uint32_t)0x1C000000)) == 0);
    ASSERT_ERR((((uint32_t)bbserviceb << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)bbservicea << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_BB_SERVICE_ADDR,  ((uint32_t)maxphyntx << 26) | ((uint32_t)bbserviceb << 16) | ((uint32_t)bbservicea << 0));
}

/**
 * @brief Unpacks BB_SERVICE's fields from current value of the BB_SERVICE register.
 *
 * Reads the BB_SERVICE register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] maxphyntx - Will be populated with the current value of this field from the register.
 * @param[out] bbserviceb - Will be populated with the current value of this field from the register.
 * @param[out] bbservicea - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_bb_service_unpack(uint8_t *maxphyntx, uint8_t *bbserviceb, uint16_t *bbservicea)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BB_SERVICE_ADDR);

    *maxphyntx = (localVal & ((uint32_t)0x1C000000)) >> 26;
    *bbserviceb = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *bbservicea = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the maxPHYNtx field in the BB_SERVICE register.
 *
 * The BB_SERVICE register will be read and the maxPHYNtx field's value will be returned.
 *
 * @return The current value of the maxPHYNtx field in the BB_SERVICE register.
 */
__INLINE uint8_t nxmac_max_phy_ntx_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BB_SERVICE_ADDR);
    return ((localVal & ((uint32_t)0x1C000000)) >> 26);
}

/**
 * @brief Sets the maxPHYNtx field of the BB_SERVICE register.
 *
 * The BB_SERVICE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maxphyntx - The value to set the field to.
 */
__INLINE void nxmac_max_phy_ntx_setf(uint8_t maxphyntx)
{
    ASSERT_ERR((((uint32_t)maxphyntx << 26) & ~((uint32_t)0x1C000000)) == 0);
    REG_PL_WR(NXMAC_BB_SERVICE_ADDR, (REG_PL_RD(NXMAC_BB_SERVICE_ADDR) & ~((uint32_t)0x1C000000)) | ((uint32_t)maxphyntx << 26));
}

/**
 * @brief Returns the current value of the bbServiceB field in the BB_SERVICE register.
 *
 * The BB_SERVICE register will be read and the bbServiceB field's value will be returned.
 *
 * @return The current value of the bbServiceB field in the BB_SERVICE register.
 */
__INLINE uint8_t nxmac_bb_service_b_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BB_SERVICE_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the bbServiceB field of the BB_SERVICE register.
 *
 * The BB_SERVICE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bbserviceb - The value to set the field to.
 */
__INLINE void nxmac_bb_service_b_setf(uint8_t bbserviceb)
{
    ASSERT_ERR((((uint32_t)bbserviceb << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(NXMAC_BB_SERVICE_ADDR, (REG_PL_RD(NXMAC_BB_SERVICE_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)bbserviceb << 16));
}

/**
 * @brief Returns the current value of the bbServiceA field in the BB_SERVICE register.
 *
 * The BB_SERVICE register will be read and the bbServiceA field's value will be returned.
 *
 * @return The current value of the bbServiceA field in the BB_SERVICE register.
 */
__INLINE uint16_t nxmac_bb_service_a_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BB_SERVICE_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the bbServiceA field of the BB_SERVICE register.
 *
 * The BB_SERVICE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bbservicea - The value to set the field to.
 */
__INLINE void nxmac_bb_service_a_setf(uint16_t bbservicea)
{
    ASSERT_ERR((((uint32_t)bbservicea << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_BB_SERVICE_ADDR, (REG_PL_RD(NXMAC_BB_SERVICE_ADDR) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)bbservicea << 0));
}

/// @}

/**
 * @name MAX_POWER_LEVEL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:08      dsssMaxPwrLevel   0x4
 *  07:00      ofdmMaxPwrLevel   0x4
 * </pre>
 *
 * @{
 */

/// Address of the MAX_POWER_LEVEL register
#define NXMAC_MAX_POWER_LEVEL_ADDR   0xC00000A0
/// Offset of the MAX_POWER_LEVEL register from the base address
#define NXMAC_MAX_POWER_LEVEL_OFFSET 0x000000A0
/// Index of the MAX_POWER_LEVEL register
#define NXMAC_MAX_POWER_LEVEL_INDEX  0x00000028
/// Reset value of the MAX_POWER_LEVEL register
#define NXMAC_MAX_POWER_LEVEL_RESET  0x00000404

/**
 * @brief Returns the current value of the MAX_POWER_LEVEL register.
 * The MAX_POWER_LEVEL register will be read and its value returned.
 * @return The current value of the MAX_POWER_LEVEL register.
 */
__INLINE uint32_t nxmac_max_power_level_get(void)
{
    return REG_PL_RD(NXMAC_MAX_POWER_LEVEL_ADDR);
}

/**
 * @brief Sets the MAX_POWER_LEVEL register to a value.
 * The MAX_POWER_LEVEL register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_max_power_level_set(uint32_t value)
{
    REG_PL_WR(NXMAC_MAX_POWER_LEVEL_ADDR, value);
}

// field definitions
/// DSSS_MAX_PWR_LEVEL field mask
#define NXMAC_DSSS_MAX_PWR_LEVEL_MASK   ((uint32_t)0x0000FF00)
/// DSSS_MAX_PWR_LEVEL field LSB position
#define NXMAC_DSSS_MAX_PWR_LEVEL_LSB    8
/// DSSS_MAX_PWR_LEVEL field width
#define NXMAC_DSSS_MAX_PWR_LEVEL_WIDTH  ((uint32_t)0x00000008)
/// OFDM_MAX_PWR_LEVEL field mask
#define NXMAC_OFDM_MAX_PWR_LEVEL_MASK   ((uint32_t)0x000000FF)
/// OFDM_MAX_PWR_LEVEL field LSB position
#define NXMAC_OFDM_MAX_PWR_LEVEL_LSB    0
/// OFDM_MAX_PWR_LEVEL field width
#define NXMAC_OFDM_MAX_PWR_LEVEL_WIDTH  ((uint32_t)0x00000008)

/// DSSS_MAX_PWR_LEVEL field reset value
#define NXMAC_DSSS_MAX_PWR_LEVEL_RST    0x4
/// OFDM_MAX_PWR_LEVEL field reset value
#define NXMAC_OFDM_MAX_PWR_LEVEL_RST    0x4

/**
 * @brief Constructs a value for the MAX_POWER_LEVEL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] dsssmaxpwrlevel - The value to use for the dsssMaxPwrLevel field.
 * @param[in] ofdmmaxpwrlevel - The value to use for the ofdmMaxPwrLevel field.
 */
__INLINE void nxmac_max_power_level_pack(uint8_t dsssmaxpwrlevel, uint8_t ofdmmaxpwrlevel)
{
    ASSERT_ERR((((uint32_t)dsssmaxpwrlevel << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)ofdmmaxpwrlevel << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_MAX_POWER_LEVEL_ADDR,  ((uint32_t)dsssmaxpwrlevel << 8) | ((uint32_t)ofdmmaxpwrlevel << 0));
}

/**
 * @brief Unpacks MAX_POWER_LEVEL's fields from current value of the MAX_POWER_LEVEL register.
 *
 * Reads the MAX_POWER_LEVEL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] dsssmaxpwrlevel - Will be populated with the current value of this field from the register.
 * @param[out] ofdmmaxpwrlevel - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_max_power_level_unpack(uint8_t *dsssmaxpwrlevel, uint8_t *ofdmmaxpwrlevel)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAX_POWER_LEVEL_ADDR);

    *dsssmaxpwrlevel = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *ofdmmaxpwrlevel = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the dsssMaxPwrLevel field in the MAX_POWER_LEVEL register.
 *
 * The MAX_POWER_LEVEL register will be read and the dsssMaxPwrLevel field's value will be returned.
 *
 * @return The current value of the dsssMaxPwrLevel field in the MAX_POWER_LEVEL register.
 */
__INLINE uint8_t nxmac_dsss_max_pwr_level_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAX_POWER_LEVEL_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the dsssMaxPwrLevel field of the MAX_POWER_LEVEL register.
 *
 * The MAX_POWER_LEVEL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dsssmaxpwrlevel - The value to set the field to.
 */
__INLINE void nxmac_dsss_max_pwr_level_setf(uint8_t dsssmaxpwrlevel)
{
    ASSERT_ERR((((uint32_t)dsssmaxpwrlevel << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(NXMAC_MAX_POWER_LEVEL_ADDR, (REG_PL_RD(NXMAC_MAX_POWER_LEVEL_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)dsssmaxpwrlevel << 8));
}

/**
 * @brief Returns the current value of the ofdmMaxPwrLevel field in the MAX_POWER_LEVEL register.
 *
 * The MAX_POWER_LEVEL register will be read and the ofdmMaxPwrLevel field's value will be returned.
 *
 * @return The current value of the ofdmMaxPwrLevel field in the MAX_POWER_LEVEL register.
 */
__INLINE uint8_t nxmac_ofdm_max_pwr_level_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAX_POWER_LEVEL_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the ofdmMaxPwrLevel field of the MAX_POWER_LEVEL register.
 *
 * The MAX_POWER_LEVEL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ofdmmaxpwrlevel - The value to set the field to.
 */
__INLINE void nxmac_ofdm_max_pwr_level_setf(uint8_t ofdmmaxpwrlevel)
{
    ASSERT_ERR((((uint32_t)ofdmmaxpwrlevel << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_MAX_POWER_LEVEL_ADDR, (REG_PL_RD(NXMAC_MAX_POWER_LEVEL_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)ofdmmaxpwrlevel << 0));
}

/// @}

/**
 * @name ENCR_KEY_0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00      encrKeyRAMWord0   0x0
 * </pre>
 *
 * @{
 */

/// Address of the ENCR_KEY_0 register
#define NXMAC_ENCR_KEY_0_ADDR   0xC00000AC
/// Offset of the ENCR_KEY_0 register from the base address
#define NXMAC_ENCR_KEY_0_OFFSET 0x000000AC
/// Index of the ENCR_KEY_0 register
#define NXMAC_ENCR_KEY_0_INDEX  0x0000002B
/// Reset value of the ENCR_KEY_0 register
#define NXMAC_ENCR_KEY_0_RESET  0x00000000

/**
 * @brief Returns the current value of the ENCR_KEY_0 register.
 * The ENCR_KEY_0 register will be read and its value returned.
 * @return The current value of the ENCR_KEY_0 register.
 */
__INLINE uint32_t nxmac_encr_key_0_get(void)
{
    return REG_PL_RD(NXMAC_ENCR_KEY_0_ADDR);
}

/**
 * @brief Sets the ENCR_KEY_0 register to a value.
 * The ENCR_KEY_0 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_encr_key_0_set(uint32_t value)
{
    REG_PL_WR(NXMAC_ENCR_KEY_0_ADDR, value);
}

// field definitions
/// ENCR_KEY_RAM_WORD_0 field mask
#define NXMAC_ENCR_KEY_RAM_WORD_0_MASK   ((uint32_t)0xFFFFFFFF)
/// ENCR_KEY_RAM_WORD_0 field LSB position
#define NXMAC_ENCR_KEY_RAM_WORD_0_LSB    0
/// ENCR_KEY_RAM_WORD_0 field width
#define NXMAC_ENCR_KEY_RAM_WORD_0_WIDTH  ((uint32_t)0x00000020)

/// ENCR_KEY_RAM_WORD_0 field reset value
#define NXMAC_ENCR_KEY_RAM_WORD_0_RST    0x0

/**
 * @brief Returns the current value of the encrKeyRAMWord0 field in the ENCR_KEY_0 register.
 *
 * The ENCR_KEY_0 register will be read and the encrKeyRAMWord0 field's value will be returned.
 *
 * @return The current value of the encrKeyRAMWord0 field in the ENCR_KEY_0 register.
 */
__INLINE uint32_t nxmac_encr_key_ram_word_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_KEY_0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the encrKeyRAMWord0 field of the ENCR_KEY_0 register.
 *
 * The ENCR_KEY_0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] encrkeyramword0 - The value to set the field to.
 */
__INLINE void nxmac_encr_key_ram_word_0_setf(uint32_t encrkeyramword0)
{
    ASSERT_ERR((((uint32_t)encrkeyramword0 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_ENCR_KEY_0_ADDR, (uint32_t)encrkeyramword0 << 0);
}

/// @}

/**
 * @name ENCR_KEY_1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00      encrKeyRAMWord1   0x0
 * </pre>
 *
 * @{
 */

/// Address of the ENCR_KEY_1 register
#define NXMAC_ENCR_KEY_1_ADDR   0xC00000B0
/// Offset of the ENCR_KEY_1 register from the base address
#define NXMAC_ENCR_KEY_1_OFFSET 0x000000B0
/// Index of the ENCR_KEY_1 register
#define NXMAC_ENCR_KEY_1_INDEX  0x0000002C
/// Reset value of the ENCR_KEY_1 register
#define NXMAC_ENCR_KEY_1_RESET  0x00000000

/**
 * @brief Returns the current value of the ENCR_KEY_1 register.
 * The ENCR_KEY_1 register will be read and its value returned.
 * @return The current value of the ENCR_KEY_1 register.
 */
__INLINE uint32_t nxmac_encr_key_1_get(void)
{
    return REG_PL_RD(NXMAC_ENCR_KEY_1_ADDR);
}

/**
 * @brief Sets the ENCR_KEY_1 register to a value.
 * The ENCR_KEY_1 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_encr_key_1_set(uint32_t value)
{
    REG_PL_WR(NXMAC_ENCR_KEY_1_ADDR, value);
}

// field definitions
/// ENCR_KEY_RAM_WORD_1 field mask
#define NXMAC_ENCR_KEY_RAM_WORD_1_MASK   ((uint32_t)0xFFFFFFFF)
/// ENCR_KEY_RAM_WORD_1 field LSB position
#define NXMAC_ENCR_KEY_RAM_WORD_1_LSB    0
/// ENCR_KEY_RAM_WORD_1 field width
#define NXMAC_ENCR_KEY_RAM_WORD_1_WIDTH  ((uint32_t)0x00000020)

/// ENCR_KEY_RAM_WORD_1 field reset value
#define NXMAC_ENCR_KEY_RAM_WORD_1_RST    0x0

/**
 * @brief Returns the current value of the encrKeyRAMWord1 field in the ENCR_KEY_1 register.
 *
 * The ENCR_KEY_1 register will be read and the encrKeyRAMWord1 field's value will be returned.
 *
 * @return The current value of the encrKeyRAMWord1 field in the ENCR_KEY_1 register.
 */
__INLINE uint32_t nxmac_encr_key_ram_word_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_KEY_1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the encrKeyRAMWord1 field of the ENCR_KEY_1 register.
 *
 * The ENCR_KEY_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] encrkeyramword1 - The value to set the field to.
 */
__INLINE void nxmac_encr_key_ram_word_1_setf(uint32_t encrkeyramword1)
{
    ASSERT_ERR((((uint32_t)encrkeyramword1 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_ENCR_KEY_1_ADDR, (uint32_t)encrkeyramword1 << 0);
}

/// @}

/**
 * @name ENCR_KEY_2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00      encrKeyRAMWord2   0x0
 * </pre>
 *
 * @{
 */

/// Address of the ENCR_KEY_2 register
#define NXMAC_ENCR_KEY_2_ADDR   0xC00000B4
/// Offset of the ENCR_KEY_2 register from the base address
#define NXMAC_ENCR_KEY_2_OFFSET 0x000000B4
/// Index of the ENCR_KEY_2 register
#define NXMAC_ENCR_KEY_2_INDEX  0x0000002D
/// Reset value of the ENCR_KEY_2 register
#define NXMAC_ENCR_KEY_2_RESET  0x00000000

/**
 * @brief Returns the current value of the ENCR_KEY_2 register.
 * The ENCR_KEY_2 register will be read and its value returned.
 * @return The current value of the ENCR_KEY_2 register.
 */
__INLINE uint32_t nxmac_encr_key_2_get(void)
{
    return REG_PL_RD(NXMAC_ENCR_KEY_2_ADDR);
}

/**
 * @brief Sets the ENCR_KEY_2 register to a value.
 * The ENCR_KEY_2 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_encr_key_2_set(uint32_t value)
{
    REG_PL_WR(NXMAC_ENCR_KEY_2_ADDR, value);
}

// field definitions
/// ENCR_KEY_RAM_WORD_2 field mask
#define NXMAC_ENCR_KEY_RAM_WORD_2_MASK   ((uint32_t)0xFFFFFFFF)
/// ENCR_KEY_RAM_WORD_2 field LSB position
#define NXMAC_ENCR_KEY_RAM_WORD_2_LSB    0
/// ENCR_KEY_RAM_WORD_2 field width
#define NXMAC_ENCR_KEY_RAM_WORD_2_WIDTH  ((uint32_t)0x00000020)

/// ENCR_KEY_RAM_WORD_2 field reset value
#define NXMAC_ENCR_KEY_RAM_WORD_2_RST    0x0

/**
 * @brief Returns the current value of the encrKeyRAMWord2 field in the ENCR_KEY_2 register.
 *
 * The ENCR_KEY_2 register will be read and the encrKeyRAMWord2 field's value will be returned.
 *
 * @return The current value of the encrKeyRAMWord2 field in the ENCR_KEY_2 register.
 */
__INLINE uint32_t nxmac_encr_key_ram_word_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_KEY_2_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the encrKeyRAMWord2 field of the ENCR_KEY_2 register.
 *
 * The ENCR_KEY_2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] encrkeyramword2 - The value to set the field to.
 */
__INLINE void nxmac_encr_key_ram_word_2_setf(uint32_t encrkeyramword2)
{
    ASSERT_ERR((((uint32_t)encrkeyramword2 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_ENCR_KEY_2_ADDR, (uint32_t)encrkeyramword2 << 0);
}

/// @}

/**
 * @name ENCR_KEY_3 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00      encrKeyRAMWord3   0x0
 * </pre>
 *
 * @{
 */

/// Address of the ENCR_KEY_3 register
#define NXMAC_ENCR_KEY_3_ADDR   0xC00000B8
/// Offset of the ENCR_KEY_3 register from the base address
#define NXMAC_ENCR_KEY_3_OFFSET 0x000000B8
/// Index of the ENCR_KEY_3 register
#define NXMAC_ENCR_KEY_3_INDEX  0x0000002E
/// Reset value of the ENCR_KEY_3 register
#define NXMAC_ENCR_KEY_3_RESET  0x00000000

/**
 * @brief Returns the current value of the ENCR_KEY_3 register.
 * The ENCR_KEY_3 register will be read and its value returned.
 * @return The current value of the ENCR_KEY_3 register.
 */
__INLINE uint32_t nxmac_encr_key_3_get(void)
{
    return REG_PL_RD(NXMAC_ENCR_KEY_3_ADDR);
}

/**
 * @brief Sets the ENCR_KEY_3 register to a value.
 * The ENCR_KEY_3 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_encr_key_3_set(uint32_t value)
{
    REG_PL_WR(NXMAC_ENCR_KEY_3_ADDR, value);
}

// field definitions
/// ENCR_KEY_RAM_WORD_3 field mask
#define NXMAC_ENCR_KEY_RAM_WORD_3_MASK   ((uint32_t)0xFFFFFFFF)
/// ENCR_KEY_RAM_WORD_3 field LSB position
#define NXMAC_ENCR_KEY_RAM_WORD_3_LSB    0
/// ENCR_KEY_RAM_WORD_3 field width
#define NXMAC_ENCR_KEY_RAM_WORD_3_WIDTH  ((uint32_t)0x00000020)

/// ENCR_KEY_RAM_WORD_3 field reset value
#define NXMAC_ENCR_KEY_RAM_WORD_3_RST    0x0

/**
 * @brief Returns the current value of the encrKeyRAMWord3 field in the ENCR_KEY_3 register.
 *
 * The ENCR_KEY_3 register will be read and the encrKeyRAMWord3 field's value will be returned.
 *
 * @return The current value of the encrKeyRAMWord3 field in the ENCR_KEY_3 register.
 */
__INLINE uint32_t nxmac_encr_key_ram_word_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_KEY_3_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the encrKeyRAMWord3 field of the ENCR_KEY_3 register.
 *
 * The ENCR_KEY_3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] encrkeyramword3 - The value to set the field to.
 */
__INLINE void nxmac_encr_key_ram_word_3_setf(uint32_t encrkeyramword3)
{
    ASSERT_ERR((((uint32_t)encrkeyramword3 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_ENCR_KEY_3_ADDR, (uint32_t)encrkeyramword3 << 0);
}

/// @}

/**
 * @name ENCR_MAC_ADDR_LOW register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00        macAddrRAMLow   0xFFFFFFFF
 * </pre>
 *
 * @{
 */

/// Address of the ENCR_MAC_ADDR_LOW register
#define NXMAC_ENCR_MAC_ADDR_LOW_ADDR   0xC00000BC
/// Offset of the ENCR_MAC_ADDR_LOW register from the base address
#define NXMAC_ENCR_MAC_ADDR_LOW_OFFSET 0x000000BC
/// Index of the ENCR_MAC_ADDR_LOW register
#define NXMAC_ENCR_MAC_ADDR_LOW_INDEX  0x0000002F
/// Reset value of the ENCR_MAC_ADDR_LOW register
#define NXMAC_ENCR_MAC_ADDR_LOW_RESET  0xFFFFFFFF

/**
 * @brief Returns the current value of the ENCR_MAC_ADDR_LOW register.
 * The ENCR_MAC_ADDR_LOW register will be read and its value returned.
 * @return The current value of the ENCR_MAC_ADDR_LOW register.
 */
__INLINE uint32_t nxmac_encr_mac_addr_low_get(void)
{
    return REG_PL_RD(NXMAC_ENCR_MAC_ADDR_LOW_ADDR);
}

/**
 * @brief Sets the ENCR_MAC_ADDR_LOW register to a value.
 * The ENCR_MAC_ADDR_LOW register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_encr_mac_addr_low_set(uint32_t value)
{
    REG_PL_WR(NXMAC_ENCR_MAC_ADDR_LOW_ADDR, value);
}

// field definitions
/// MAC_ADDR_RAM_LOW field mask
#define NXMAC_MAC_ADDR_RAM_LOW_MASK   ((uint32_t)0xFFFFFFFF)
/// MAC_ADDR_RAM_LOW field LSB position
#define NXMAC_MAC_ADDR_RAM_LOW_LSB    0
/// MAC_ADDR_RAM_LOW field width
#define NXMAC_MAC_ADDR_RAM_LOW_WIDTH  ((uint32_t)0x00000020)

/// MAC_ADDR_RAM_LOW field reset value
#define NXMAC_MAC_ADDR_RAM_LOW_RST    0xFFFFFFFF

/**
 * @brief Returns the current value of the macAddrRAMLow field in the ENCR_MAC_ADDR_LOW register.
 *
 * The ENCR_MAC_ADDR_LOW register will be read and the macAddrRAMLow field's value will be returned.
 *
 * @return The current value of the macAddrRAMLow field in the ENCR_MAC_ADDR_LOW register.
 */
__INLINE uint32_t nxmac_mac_addr_ram_low_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_MAC_ADDR_LOW_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the macAddrRAMLow field of the ENCR_MAC_ADDR_LOW register.
 *
 * The ENCR_MAC_ADDR_LOW register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macaddrramlow - The value to set the field to.
 */
__INLINE void nxmac_mac_addr_ram_low_setf(uint32_t macaddrramlow)
{
    ASSERT_ERR((((uint32_t)macaddrramlow << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_ENCR_MAC_ADDR_LOW_ADDR, (uint32_t)macaddrramlow << 0);
}

/// @}

/**
 * @name ENCR_MAC_ADDR_HIGH register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00       macAddrRAMHigh   0xFFFF
 * </pre>
 *
 * @{
 */

/// Address of the ENCR_MAC_ADDR_HIGH register
#define NXMAC_ENCR_MAC_ADDR_HIGH_ADDR   0xC00000C0
/// Offset of the ENCR_MAC_ADDR_HIGH register from the base address
#define NXMAC_ENCR_MAC_ADDR_HIGH_OFFSET 0x000000C0
/// Index of the ENCR_MAC_ADDR_HIGH register
#define NXMAC_ENCR_MAC_ADDR_HIGH_INDEX  0x00000030
/// Reset value of the ENCR_MAC_ADDR_HIGH register
#define NXMAC_ENCR_MAC_ADDR_HIGH_RESET  0x0000FFFF

/**
 * @brief Returns the current value of the ENCR_MAC_ADDR_HIGH register.
 * The ENCR_MAC_ADDR_HIGH register will be read and its value returned.
 * @return The current value of the ENCR_MAC_ADDR_HIGH register.
 */
__INLINE uint32_t nxmac_encr_mac_addr_high_get(void)
{
    return REG_PL_RD(NXMAC_ENCR_MAC_ADDR_HIGH_ADDR);
}

/**
 * @brief Sets the ENCR_MAC_ADDR_HIGH register to a value.
 * The ENCR_MAC_ADDR_HIGH register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_encr_mac_addr_high_set(uint32_t value)
{
    REG_PL_WR(NXMAC_ENCR_MAC_ADDR_HIGH_ADDR, value);
}

// field definitions
/// MAC_ADDR_RAM_HIGH field mask
#define NXMAC_MAC_ADDR_RAM_HIGH_MASK   ((uint32_t)0x0000FFFF)
/// MAC_ADDR_RAM_HIGH field LSB position
#define NXMAC_MAC_ADDR_RAM_HIGH_LSB    0
/// MAC_ADDR_RAM_HIGH field width
#define NXMAC_MAC_ADDR_RAM_HIGH_WIDTH  ((uint32_t)0x00000010)

/// MAC_ADDR_RAM_HIGH field reset value
#define NXMAC_MAC_ADDR_RAM_HIGH_RST    0xFFFF

/**
 * @brief Returns the current value of the macAddrRAMHigh field in the ENCR_MAC_ADDR_HIGH register.
 *
 * The ENCR_MAC_ADDR_HIGH register will be read and the macAddrRAMHigh field's value will be returned.
 *
 * @return The current value of the macAddrRAMHigh field in the ENCR_MAC_ADDR_HIGH register.
 */
__INLINE uint16_t nxmac_mac_addr_ram_high_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_MAC_ADDR_HIGH_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the macAddrRAMHigh field of the ENCR_MAC_ADDR_HIGH register.
 *
 * The ENCR_MAC_ADDR_HIGH register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macaddrramhigh - The value to set the field to.
 */
__INLINE void nxmac_mac_addr_ram_high_setf(uint16_t macaddrramhigh)
{
    ASSERT_ERR((((uint32_t)macaddrramhigh << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_ENCR_MAC_ADDR_HIGH_ADDR, (uint32_t)macaddrramhigh << 0);
}

/// @}

/**
 * @name ENCR_CNTRL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31              newRead   0
 *     30             newWrite   0
 *     29            newSearch   0
 *     28          searchError   0
 *  25:16          keyIndexRAM   0x0
 *  10:08             cTypeRAM   0x0
 *  07:04            vlanIDRAM   0x0
 *  03:02               sppRAM   0x0
 *     01         useDefKeyRAM   0
 *     00              cLenRAM   0
 * </pre>
 *
 * @{
 */
enum
{
	CTYPERAM_NULL_KEY = 0,
	CTYPERAM_WEP,
	CTYPERAM_TKIP,
	CTYPERAM_CCMP,
};

/// Address of the ENCR_CNTRL register
#define NXMAC_ENCR_CNTRL_ADDR   0xC00000C4
/// Offset of the ENCR_CNTRL register from the base address
#define NXMAC_ENCR_CNTRL_OFFSET 0x000000C4
/// Index of the ENCR_CNTRL register
#define NXMAC_ENCR_CNTRL_INDEX  0x00000031
/// Reset value of the ENCR_CNTRL register
#define NXMAC_ENCR_CNTRL_RESET  0x00000000

/**
 * @brief Returns the current value of the ENCR_CNTRL register.
 * The ENCR_CNTRL register will be read and its value returned.
 * @return The current value of the ENCR_CNTRL register.
 */
__INLINE uint32_t nxmac_encr_cntrl_get(void)
{
    return REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR);
}

/**
 * @brief Sets the ENCR_CNTRL register to a value.
 * The ENCR_CNTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_encr_cntrl_set(uint32_t value)
{
    REG_PL_WR(NXMAC_ENCR_CNTRL_ADDR, value);
}

// field definitions
/// NEW_READ field bit
#define NXMAC_NEW_READ_BIT           ((uint32_t)0x80000000)
/// NEW_READ field position
#define NXMAC_NEW_READ_POS           31
/// NEW_WRITE field bit
#define NXMAC_NEW_WRITE_BIT          ((uint32_t)0x40000000)
/// NEW_WRITE field position
#define NXMAC_NEW_WRITE_POS          30
/// NEW_SEARCH field bit
#define NXMAC_NEW_SEARCH_BIT         ((uint32_t)0x20000000)
/// NEW_SEARCH field position
#define NXMAC_NEW_SEARCH_POS         29
/// SEARCH_ERROR field bit
#define NXMAC_SEARCH_ERROR_BIT       ((uint32_t)0x10000000)
/// SEARCH_ERROR field position
#define NXMAC_SEARCH_ERROR_POS       28
/// KEY_INDEX_RAM field mask
#define NXMAC_KEY_INDEX_RAM_MASK     ((uint32_t)0x03FF0000)
/// KEY_INDEX_RAM field LSB position
#define NXMAC_KEY_INDEX_RAM_LSB      16
/// KEY_INDEX_RAM field width
#define NXMAC_KEY_INDEX_RAM_WIDTH    ((uint32_t)0x0000000A)
/// C_TYPE_RAM field mask
#define NXMAC_C_TYPE_RAM_MASK        ((uint32_t)0x00000700)
/// C_TYPE_RAM field LSB position
#define NXMAC_C_TYPE_RAM_LSB         8
/// C_TYPE_RAM field width
#define NXMAC_C_TYPE_RAM_WIDTH       ((uint32_t)0x00000003)
/// VLAN_IDRAM field mask
#define NXMAC_VLAN_IDRAM_MASK        ((uint32_t)0x000000F0)
/// VLAN_IDRAM field LSB position
#define NXMAC_VLAN_IDRAM_LSB         4
/// VLAN_IDRAM field width
#define NXMAC_VLAN_IDRAM_WIDTH       ((uint32_t)0x00000004)
/// SPP_RAM field mask
#define NXMAC_SPP_RAM_MASK           ((uint32_t)0x0000000C)
/// SPP_RAM field LSB position
#define NXMAC_SPP_RAM_LSB            2
/// SPP_RAM field width
#define NXMAC_SPP_RAM_WIDTH          ((uint32_t)0x00000002)
/// USE_DEF_KEY_RAM field bit
#define NXMAC_USE_DEF_KEY_RAM_BIT    ((uint32_t)0x00000002)
/// USE_DEF_KEY_RAM field position
#define NXMAC_USE_DEF_KEY_RAM_POS    1
/// C_LEN_RAM field bit
#define NXMAC_C_LEN_RAM_BIT          ((uint32_t)0x00000001)
/// C_LEN_RAM field position
#define NXMAC_C_LEN_RAM_POS          0

/// NEW_READ field reset value
#define NXMAC_NEW_READ_RST           0x0
/// NEW_WRITE field reset value
#define NXMAC_NEW_WRITE_RST          0x0
/// NEW_SEARCH field reset value
#define NXMAC_NEW_SEARCH_RST         0x0
/// SEARCH_ERROR field reset value
#define NXMAC_SEARCH_ERROR_RST       0x0
/// KEY_INDEX_RAM field reset value
#define NXMAC_KEY_INDEX_RAM_RST      0x0
/// C_TYPE_RAM field reset value
#define NXMAC_C_TYPE_RAM_RST         0x0
/// VLAN_IDRAM field reset value
#define NXMAC_VLAN_IDRAM_RST         0x0
/// SPP_RAM field reset value
#define NXMAC_SPP_RAM_RST            0x0
/// USE_DEF_KEY_RAM field reset value
#define NXMAC_USE_DEF_KEY_RAM_RST    0x0
/// C_LEN_RAM field reset value
#define NXMAC_C_LEN_RAM_RST          0x0

/**
 * @brief Constructs a value for the ENCR_CNTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] newread - The value to use for the newRead field.
 * @param[in] newwrite - The value to use for the newWrite field.
 * @param[in] newsearch - The value to use for the newSearch field.
 * @param[in] searcherror - The value to use for the searchError field.
 * @param[in] keyindexram - The value to use for the keyIndexRAM field.
 * @param[in] ctyperam - The value to use for the cTypeRAM field.
 * @param[in] vlanidram - The value to use for the vlanIDRAM field.
 * @param[in] sppram - The value to use for the sppRAM field.
 * @param[in] usedefkeyram - The value to use for the useDefKeyRAM field.
 * @param[in] clenram - The value to use for the cLenRAM field.
 */
__INLINE void nxmac_encr_cntrl_pack(uint8_t newread, uint8_t newwrite, uint8_t newsearch, uint8_t searcherror, uint16_t keyindexram, uint8_t ctyperam, uint8_t vlanidram, uint8_t sppram, uint8_t usedefkeyram, uint8_t clenram)
{
    ASSERT_ERR((((uint32_t)newread << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)newwrite << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)newsearch << 29) & ~((uint32_t)0x20000000)) == 0);
    ASSERT_ERR((((uint32_t)searcherror << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)keyindexram << 16) & ~((uint32_t)0x03FF0000)) == 0);
    ASSERT_ERR((((uint32_t)ctyperam << 8) & ~((uint32_t)0x00000700)) == 0);
    ASSERT_ERR((((uint32_t)vlanidram << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)sppram << 2) & ~((uint32_t)0x0000000C)) == 0);
    ASSERT_ERR((((uint32_t)usedefkeyram << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)clenram << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_ENCR_CNTRL_ADDR,  ((uint32_t)newread << 31) | ((uint32_t)newwrite << 30) | ((uint32_t)newsearch << 29) | ((uint32_t)searcherror << 28) | ((uint32_t)keyindexram << 16) | ((uint32_t)ctyperam << 8) | ((uint32_t)vlanidram << 4) | ((uint32_t)sppram << 2) | ((uint32_t)usedefkeyram << 1) | ((uint32_t)clenram << 0));
}

/**
 * @brief Unpacks ENCR_CNTRL's fields from current value of the ENCR_CNTRL register.
 *
 * Reads the ENCR_CNTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] newread - Will be populated with the current value of this field from the register.
 * @param[out] newwrite - Will be populated with the current value of this field from the register.
 * @param[out] newsearch - Will be populated with the current value of this field from the register.
 * @param[out] searcherror - Will be populated with the current value of this field from the register.
 * @param[out] keyindexram - Will be populated with the current value of this field from the register.
 * @param[out] ctyperam - Will be populated with the current value of this field from the register.
 * @param[out] vlanidram - Will be populated with the current value of this field from the register.
 * @param[out] sppram - Will be populated with the current value of this field from the register.
 * @param[out] usedefkeyram - Will be populated with the current value of this field from the register.
 * @param[out] clenram - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_encr_cntrl_unpack(uint8_t *newread, uint8_t *newwrite, uint8_t *newsearch, uint8_t *searcherror, uint16_t *keyindexram, uint8_t *ctyperam, uint8_t *vlanidram, uint8_t *sppram, uint8_t *usedefkeyram, uint8_t *clenram)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR);

    *newread = (localVal & ((uint32_t)0x80000000)) >> 31;
    *newwrite = (localVal & ((uint32_t)0x40000000)) >> 30;
    *newsearch = (localVal & ((uint32_t)0x20000000)) >> 29;
    *searcherror = (localVal & ((uint32_t)0x10000000)) >> 28;
    *keyindexram = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *ctyperam = (localVal & ((uint32_t)0x00000700)) >> 8;
    *vlanidram = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *sppram = (localVal & ((uint32_t)0x0000000C)) >> 2;
    *usedefkeyram = (localVal & ((uint32_t)0x00000002)) >> 1;
    *clenram = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the newRead field in the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read and the newRead field's value will be returned.
 *
 * @return The current value of the newRead field in the ENCR_CNTRL register.
 */
__INLINE uint8_t nxmac_new_read_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the newRead field of the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] newread - The value to set the field to.
 */
__INLINE void nxmac_new_read_setf(uint8_t newread)
{
    ASSERT_ERR((((uint32_t)newread << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(NXMAC_ENCR_CNTRL_ADDR, (REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)newread << 31));
}

/**
 * @brief Returns the current value of the newWrite field in the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read and the newWrite field's value will be returned.
 *
 * @return The current value of the newWrite field in the ENCR_CNTRL register.
 */
__INLINE uint8_t nxmac_new_write_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

/**
 * @brief Sets the newWrite field of the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] newwrite - The value to set the field to.
 */
__INLINE void nxmac_new_write_setf(uint8_t newwrite)
{
    ASSERT_ERR((((uint32_t)newwrite << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_PL_WR(NXMAC_ENCR_CNTRL_ADDR, (REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)newwrite << 30));
}

/**
 * @brief Returns the current value of the newSearch field in the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read and the newSearch field's value will be returned.
 *
 * @return The current value of the newSearch field in the ENCR_CNTRL register.
 */
__INLINE uint8_t nxmac_new_search_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

/**
 * @brief Sets the newSearch field of the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] newsearch - The value to set the field to.
 */
__INLINE void nxmac_new_search_setf(uint8_t newsearch)
{
    ASSERT_ERR((((uint32_t)newsearch << 29) & ~((uint32_t)0x20000000)) == 0);
    REG_PL_WR(NXMAC_ENCR_CNTRL_ADDR, (REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR) & ~((uint32_t)0x20000000)) | ((uint32_t)newsearch << 29));
}

/**
 * @brief Returns the current value of the searchError field in the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read and the searchError field's value will be returned.
 *
 * @return The current value of the searchError field in the ENCR_CNTRL register.
 */
__INLINE uint8_t nxmac_search_error_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the searchError field of the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] searcherror - The value to set the field to.
 */
__INLINE void nxmac_search_error_setf(uint8_t searcherror)
{
    ASSERT_ERR((((uint32_t)searcherror << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(NXMAC_ENCR_CNTRL_ADDR, (REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)searcherror << 28));
}

/**
 * @brief Returns the current value of the keyIndexRAM field in the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read and the keyIndexRAM field's value will be returned.
 *
 * @return The current value of the keyIndexRAM field in the ENCR_CNTRL register.
 */
__INLINE uint16_t nxmac_key_index_ram_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

/**
 * @brief Sets the keyIndexRAM field of the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] keyindexram - The value to set the field to.
 */
__INLINE void nxmac_key_index_ram_setf(uint16_t keyindexram)
{
    ASSERT_ERR((((uint32_t)keyindexram << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_PL_WR(NXMAC_ENCR_CNTRL_ADDR, (REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR) & ~((uint32_t)0x03FF0000)) | ((uint32_t)keyindexram << 16));
}

/**
 * @brief Returns the current value of the cTypeRAM field in the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read and the cTypeRAM field's value will be returned.
 *
 * @return The current value of the cTypeRAM field in the ENCR_CNTRL register.
 */
__INLINE uint8_t nxmac_c_type_ram_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

/**
 * @brief Sets the cTypeRAM field of the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ctyperam - The value to set the field to.
 */
__INLINE void nxmac_c_type_ram_setf(uint8_t ctyperam)
{
    ASSERT_ERR((((uint32_t)ctyperam << 8) & ~((uint32_t)0x00000700)) == 0);
    REG_PL_WR(NXMAC_ENCR_CNTRL_ADDR, (REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR) & ~((uint32_t)0x00000700)) | ((uint32_t)ctyperam << 8));
}

/**
 * @brief Returns the current value of the vlanIDRAM field in the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read and the vlanIDRAM field's value will be returned.
 *
 * @return The current value of the vlanIDRAM field in the ENCR_CNTRL register.
 */
__INLINE uint8_t nxmac_vlan_idram_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the vlanIDRAM field of the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] vlanidram - The value to set the field to.
 */
__INLINE void nxmac_vlan_idram_setf(uint8_t vlanidram)
{
    ASSERT_ERR((((uint32_t)vlanidram << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(NXMAC_ENCR_CNTRL_ADDR, (REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)vlanidram << 4));
}

/**
 * @brief Returns the current value of the sppRAM field in the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read and the sppRAM field's value will be returned.
 *
 * @return The current value of the sppRAM field in the ENCR_CNTRL register.
 */
__INLINE uint8_t nxmac_spp_ram_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x0000000C)) >> 2);
}

/**
 * @brief Sets the sppRAM field of the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] sppram - The value to set the field to.
 */
__INLINE void nxmac_spp_ram_setf(uint8_t sppram)
{
    ASSERT_ERR((((uint32_t)sppram << 2) & ~((uint32_t)0x0000000C)) == 0);
    REG_PL_WR(NXMAC_ENCR_CNTRL_ADDR, (REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR) & ~((uint32_t)0x0000000C)) | ((uint32_t)sppram << 2));
}

/**
 * @brief Returns the current value of the useDefKeyRAM field in the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read and the useDefKeyRAM field's value will be returned.
 *
 * @return The current value of the useDefKeyRAM field in the ENCR_CNTRL register.
 */
__INLINE uint8_t nxmac_use_def_key_ram_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the useDefKeyRAM field of the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] usedefkeyram - The value to set the field to.
 */
__INLINE void nxmac_use_def_key_ram_setf(uint8_t usedefkeyram)
{
    ASSERT_ERR((((uint32_t)usedefkeyram << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_ENCR_CNTRL_ADDR, (REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)usedefkeyram << 1));
}

/**
 * @brief Returns the current value of the cLenRAM field in the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read and the cLenRAM field's value will be returned.
 *
 * @return The current value of the cLenRAM field in the ENCR_CNTRL register.
 */
__INLINE uint8_t nxmac_c_len_ram_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the cLenRAM field of the ENCR_CNTRL register.
 *
 * The ENCR_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] clenram - The value to set the field to.
 */
__INLINE void nxmac_c_len_ram_setf(uint8_t clenram)
{
    ASSERT_ERR((((uint32_t)clenram << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_ENCR_CNTRL_ADDR, (REG_PL_RD(NXMAC_ENCR_CNTRL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)clenram << 0));
}

/// @}

#if RW_WAPI_EN
/**
 * @name ENCR_WPI_INT_KEY_0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00   encrIntKeyRAMWord0   0x0
 * </pre>
 *
 * @{
 */

/// Address of the ENCR_WPI_INT_KEY_0 register
#define NXMAC_ENCR_WPI_INT_KEY_0_ADDR   0xC00000C8
/// Offset of the ENCR_WPI_INT_KEY_0 register from the base address
#define NXMAC_ENCR_WPI_INT_KEY_0_OFFSET 0x000000C8
/// Index of the ENCR_WPI_INT_KEY_0 register
#define NXMAC_ENCR_WPI_INT_KEY_0_INDEX  0x00000032
/// Reset value of the ENCR_WPI_INT_KEY_0 register
#define NXMAC_ENCR_WPI_INT_KEY_0_RESET  0x00000000

/**
 * @brief Returns the current value of the ENCR_WPI_INT_KEY_0 register.
 * The ENCR_WPI_INT_KEY_0 register will be read and its value returned.
 * @return The current value of the ENCR_WPI_INT_KEY_0 register.
 */
__INLINE uint32_t nxmac_encr_wpi_int_key_0_get(void)
{
    return REG_PL_RD(NXMAC_ENCR_WPI_INT_KEY_0_ADDR);
}

/**
 * @brief Sets the ENCR_WPI_INT_KEY_0 register to a value.
 * The ENCR_WPI_INT_KEY_0 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_encr_wpi_int_key_0_set(uint32_t value)
{
    REG_PL_WR(NXMAC_ENCR_WPI_INT_KEY_0_ADDR, value);
}

// field definitions
/// ENCR_INT_KEY_RAM_WORD_0 field mask
#define NXMAC_ENCR_INT_KEY_RAM_WORD_0_MASK   ((uint32_t)0xFFFFFFFF)
/// ENCR_INT_KEY_RAM_WORD_0 field LSB position
#define NXMAC_ENCR_INT_KEY_RAM_WORD_0_LSB    0
/// ENCR_INT_KEY_RAM_WORD_0 field width
#define NXMAC_ENCR_INT_KEY_RAM_WORD_0_WIDTH  ((uint32_t)0x00000020)

/// ENCR_INT_KEY_RAM_WORD_0 field reset value
#define NXMAC_ENCR_INT_KEY_RAM_WORD_0_RST    0x0

/**
 * @brief Returns the current value of the encrIntKeyRAMWord0 field in the ENCR_WPI_INT_KEY_0 register.
 *
 * The ENCR_WPI_INT_KEY_0 register will be read and the encrIntKeyRAMWord0 field's value will be returned.
 *
 * @return The current value of the encrIntKeyRAMWord0 field in the ENCR_WPI_INT_KEY_0 register.
 */
__INLINE uint32_t nxmac_encr_int_key_ram_word_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_WPI_INT_KEY_0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the encrIntKeyRAMWord0 field of the ENCR_WPI_INT_KEY_0 register.
 *
 * The ENCR_WPI_INT_KEY_0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] encrintkeyramword0 - The value to set the field to.
 */
__INLINE void nxmac_encr_int_key_ram_word_0_setf(uint32_t encrintkeyramword0)
{
    ASSERT_ERR((((uint32_t)encrintkeyramword0 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_ENCR_WPI_INT_KEY_0_ADDR, (uint32_t)encrintkeyramword0 << 0);
}

#endif // RW_WAPI_EN
/// @}

#if RW_WAPI_EN
/**
 * @name ENCR_WPI_INT_KEY_1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00   encrIntKeyRAMWord1   0x0
 * </pre>
 *
 * @{
 */

/// Address of the ENCR_WPI_INT_KEY_1 register
#define NXMAC_ENCR_WPI_INT_KEY_1_ADDR   0xC00000CC
/// Offset of the ENCR_WPI_INT_KEY_1 register from the base address
#define NXMAC_ENCR_WPI_INT_KEY_1_OFFSET 0x000000CC
/// Index of the ENCR_WPI_INT_KEY_1 register
#define NXMAC_ENCR_WPI_INT_KEY_1_INDEX  0x00000033
/// Reset value of the ENCR_WPI_INT_KEY_1 register
#define NXMAC_ENCR_WPI_INT_KEY_1_RESET  0x00000000

/**
 * @brief Returns the current value of the ENCR_WPI_INT_KEY_1 register.
 * The ENCR_WPI_INT_KEY_1 register will be read and its value returned.
 * @return The current value of the ENCR_WPI_INT_KEY_1 register.
 */
__INLINE uint32_t nxmac_encr_wpi_int_key_1_get(void)
{
    return REG_PL_RD(NXMAC_ENCR_WPI_INT_KEY_1_ADDR);
}

/**
 * @brief Sets the ENCR_WPI_INT_KEY_1 register to a value.
 * The ENCR_WPI_INT_KEY_1 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_encr_wpi_int_key_1_set(uint32_t value)
{
    REG_PL_WR(NXMAC_ENCR_WPI_INT_KEY_1_ADDR, value);
}

// field definitions
/// ENCR_INT_KEY_RAM_WORD_1 field mask
#define NXMAC_ENCR_INT_KEY_RAM_WORD_1_MASK   ((uint32_t)0xFFFFFFFF)
/// ENCR_INT_KEY_RAM_WORD_1 field LSB position
#define NXMAC_ENCR_INT_KEY_RAM_WORD_1_LSB    0
/// ENCR_INT_KEY_RAM_WORD_1 field width
#define NXMAC_ENCR_INT_KEY_RAM_WORD_1_WIDTH  ((uint32_t)0x00000020)

/// ENCR_INT_KEY_RAM_WORD_1 field reset value
#define NXMAC_ENCR_INT_KEY_RAM_WORD_1_RST    0x0

/**
 * @brief Returns the current value of the encrIntKeyRAMWord1 field in the ENCR_WPI_INT_KEY_1 register.
 *
 * The ENCR_WPI_INT_KEY_1 register will be read and the encrIntKeyRAMWord1 field's value will be returned.
 *
 * @return The current value of the encrIntKeyRAMWord1 field in the ENCR_WPI_INT_KEY_1 register.
 */
__INLINE uint32_t nxmac_encr_int_key_ram_word_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_WPI_INT_KEY_1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the encrIntKeyRAMWord1 field of the ENCR_WPI_INT_KEY_1 register.
 *
 * The ENCR_WPI_INT_KEY_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] encrintkeyramword1 - The value to set the field to.
 */
__INLINE void nxmac_encr_int_key_ram_word_1_setf(uint32_t encrintkeyramword1)
{
    ASSERT_ERR((((uint32_t)encrintkeyramword1 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_ENCR_WPI_INT_KEY_1_ADDR, (uint32_t)encrintkeyramword1 << 0);
}

#endif // RW_WAPI_EN
/// @}

#if RW_WAPI_EN
/**
 * @name ENCR_WPI_INT_KEY_2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00   encrIntKeyRAMWord2   0x0
 * </pre>
 *
 * @{
 */

/// Address of the ENCR_WPI_INT_KEY_2 register
#define NXMAC_ENCR_WPI_INT_KEY_2_ADDR   0xC00000D0
/// Offset of the ENCR_WPI_INT_KEY_2 register from the base address
#define NXMAC_ENCR_WPI_INT_KEY_2_OFFSET 0x000000D0
/// Index of the ENCR_WPI_INT_KEY_2 register
#define NXMAC_ENCR_WPI_INT_KEY_2_INDEX  0x00000034
/// Reset value of the ENCR_WPI_INT_KEY_2 register
#define NXMAC_ENCR_WPI_INT_KEY_2_RESET  0x00000000

/**
 * @brief Returns the current value of the ENCR_WPI_INT_KEY_2 register.
 * The ENCR_WPI_INT_KEY_2 register will be read and its value returned.
 * @return The current value of the ENCR_WPI_INT_KEY_2 register.
 */
__INLINE uint32_t nxmac_encr_wpi_int_key_2_get(void)
{
    return REG_PL_RD(NXMAC_ENCR_WPI_INT_KEY_2_ADDR);
}

/**
 * @brief Sets the ENCR_WPI_INT_KEY_2 register to a value.
 * The ENCR_WPI_INT_KEY_2 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_encr_wpi_int_key_2_set(uint32_t value)
{
    REG_PL_WR(NXMAC_ENCR_WPI_INT_KEY_2_ADDR, value);
}

// field definitions
/// ENCR_INT_KEY_RAM_WORD_2 field mask
#define NXMAC_ENCR_INT_KEY_RAM_WORD_2_MASK   ((uint32_t)0xFFFFFFFF)
/// ENCR_INT_KEY_RAM_WORD_2 field LSB position
#define NXMAC_ENCR_INT_KEY_RAM_WORD_2_LSB    0
/// ENCR_INT_KEY_RAM_WORD_2 field width
#define NXMAC_ENCR_INT_KEY_RAM_WORD_2_WIDTH  ((uint32_t)0x00000020)

/// ENCR_INT_KEY_RAM_WORD_2 field reset value
#define NXMAC_ENCR_INT_KEY_RAM_WORD_2_RST    0x0

/**
 * @brief Returns the current value of the encrIntKeyRAMWord2 field in the ENCR_WPI_INT_KEY_2 register.
 *
 * The ENCR_WPI_INT_KEY_2 register will be read and the encrIntKeyRAMWord2 field's value will be returned.
 *
 * @return The current value of the encrIntKeyRAMWord2 field in the ENCR_WPI_INT_KEY_2 register.
 */
__INLINE uint32_t nxmac_encr_int_key_ram_word_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_WPI_INT_KEY_2_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the encrIntKeyRAMWord2 field of the ENCR_WPI_INT_KEY_2 register.
 *
 * The ENCR_WPI_INT_KEY_2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] encrintkeyramword2 - The value to set the field to.
 */
__INLINE void nxmac_encr_int_key_ram_word_2_setf(uint32_t encrintkeyramword2)
{
    ASSERT_ERR((((uint32_t)encrintkeyramword2 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_ENCR_WPI_INT_KEY_2_ADDR, (uint32_t)encrintkeyramword2 << 0);
}

#endif // RW_WAPI_EN
/// @}

#if RW_WAPI_EN
/**
 * @name ENCR_WPI_INT_KEY_3 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00   encrIntKeyRAMWord3   0x0
 * </pre>
 *
 * @{
 */

/// Address of the ENCR_WPI_INT_KEY_3 register
#define NXMAC_ENCR_WPI_INT_KEY_3_ADDR   0xC00000D4
/// Offset of the ENCR_WPI_INT_KEY_3 register from the base address
#define NXMAC_ENCR_WPI_INT_KEY_3_OFFSET 0x000000D4
/// Index of the ENCR_WPI_INT_KEY_3 register
#define NXMAC_ENCR_WPI_INT_KEY_3_INDEX  0x00000035
/// Reset value of the ENCR_WPI_INT_KEY_3 register
#define NXMAC_ENCR_WPI_INT_KEY_3_RESET  0x00000000

/**
 * @brief Returns the current value of the ENCR_WPI_INT_KEY_3 register.
 * The ENCR_WPI_INT_KEY_3 register will be read and its value returned.
 * @return The current value of the ENCR_WPI_INT_KEY_3 register.
 */
__INLINE uint32_t nxmac_encr_wpi_int_key_3_get(void)
{
    return REG_PL_RD(NXMAC_ENCR_WPI_INT_KEY_3_ADDR);
}

/**
 * @brief Sets the ENCR_WPI_INT_KEY_3 register to a value.
 * The ENCR_WPI_INT_KEY_3 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_encr_wpi_int_key_3_set(uint32_t value)
{
    REG_PL_WR(NXMAC_ENCR_WPI_INT_KEY_3_ADDR, value);
}

// field definitions
/// ENCR_INT_KEY_RAM_WORD_3 field mask
#define NXMAC_ENCR_INT_KEY_RAM_WORD_3_MASK   ((uint32_t)0xFFFFFFFF)
/// ENCR_INT_KEY_RAM_WORD_3 field LSB position
#define NXMAC_ENCR_INT_KEY_RAM_WORD_3_LSB    0
/// ENCR_INT_KEY_RAM_WORD_3 field width
#define NXMAC_ENCR_INT_KEY_RAM_WORD_3_WIDTH  ((uint32_t)0x00000020)

/// ENCR_INT_KEY_RAM_WORD_3 field reset value
#define NXMAC_ENCR_INT_KEY_RAM_WORD_3_RST    0x0

/**
 * @brief Returns the current value of the encrIntKeyRAMWord3 field in the ENCR_WPI_INT_KEY_3 register.
 *
 * The ENCR_WPI_INT_KEY_3 register will be read and the encrIntKeyRAMWord3 field's value will be returned.
 *
 * @return The current value of the encrIntKeyRAMWord3 field in the ENCR_WPI_INT_KEY_3 register.
 */
__INLINE uint32_t nxmac_encr_int_key_ram_word_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_WPI_INT_KEY_3_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the encrIntKeyRAMWord3 field of the ENCR_WPI_INT_KEY_3 register.
 *
 * The ENCR_WPI_INT_KEY_3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] encrintkeyramword3 - The value to set the field to.
 */
__INLINE void nxmac_encr_int_key_ram_word_3_setf(uint32_t encrintkeyramword3)
{
    ASSERT_ERR((((uint32_t)encrintkeyramword3 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_ENCR_WPI_INT_KEY_3_ADDR, (uint32_t)encrintkeyramword3 << 0);
}

#endif // RW_WAPI_EN
/// @}

/**
 * @name ENCR_RAM_CONFIG register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:24       staKeyMaxIndex   0x0
 *  19:16                 nVAP   0x0
 *  15:08       staKeyEndIndex   0x0
 *  07:00     staKeyStartIndex   0x0
 * </pre>
 *
 * @{
 */

/// Address of the ENCR_RAM_CONFIG register
#define NXMAC_ENCR_RAM_CONFIG_ADDR   0xC00000D8
/// Offset of the ENCR_RAM_CONFIG register from the base address
#define NXMAC_ENCR_RAM_CONFIG_OFFSET 0x000000D8
/// Index of the ENCR_RAM_CONFIG register
#define NXMAC_ENCR_RAM_CONFIG_INDEX  0x00000036
/// Reset value of the ENCR_RAM_CONFIG register
#define NXMAC_ENCR_RAM_CONFIG_RESET  0x00000000

/**
 * @brief Returns the current value of the ENCR_RAM_CONFIG register.
 * The ENCR_RAM_CONFIG register will be read and its value returned.
 * @return The current value of the ENCR_RAM_CONFIG register.
 */
__INLINE uint32_t nxmac_encr_ram_config_get(void)
{
    return REG_PL_RD(NXMAC_ENCR_RAM_CONFIG_ADDR);
}

/**
 * @brief Sets the ENCR_RAM_CONFIG register to a value.
 * The ENCR_RAM_CONFIG register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_encr_ram_config_set(uint32_t value)
{
    REG_PL_WR(NXMAC_ENCR_RAM_CONFIG_ADDR, value);
}

// field definitions
/// STA_KEY_MAX_INDEX field mask
#define NXMAC_STA_KEY_MAX_INDEX_MASK     ((uint32_t)0xFF000000)
/// STA_KEY_MAX_INDEX field LSB position
#define NXMAC_STA_KEY_MAX_INDEX_LSB      24
/// STA_KEY_MAX_INDEX field width
#define NXMAC_STA_KEY_MAX_INDEX_WIDTH    ((uint32_t)0x00000008)
/// N_VAP field mask
#define NXMAC_N_VAP_MASK                 ((uint32_t)0x000F0000)
/// N_VAP field LSB position
#define NXMAC_N_VAP_LSB                  16
/// N_VAP field width
#define NXMAC_N_VAP_WIDTH                ((uint32_t)0x00000004)
/// STA_KEY_END_INDEX field mask
#define NXMAC_STA_KEY_END_INDEX_MASK     ((uint32_t)0x0000FF00)
/// STA_KEY_END_INDEX field LSB position
#define NXMAC_STA_KEY_END_INDEX_LSB      8
/// STA_KEY_END_INDEX field width
#define NXMAC_STA_KEY_END_INDEX_WIDTH    ((uint32_t)0x00000008)
/// STA_KEY_START_INDEX field mask
#define NXMAC_STA_KEY_START_INDEX_MASK   ((uint32_t)0x000000FF)
/// STA_KEY_START_INDEX field LSB position
#define NXMAC_STA_KEY_START_INDEX_LSB    0
/// STA_KEY_START_INDEX field width
#define NXMAC_STA_KEY_START_INDEX_WIDTH  ((uint32_t)0x00000008)

/// STA_KEY_MAX_INDEX field reset value
#define NXMAC_STA_KEY_MAX_INDEX_RST      0x0
/// N_VAP field reset value
#define NXMAC_N_VAP_RST                  0x0
/// STA_KEY_END_INDEX field reset value
#define NXMAC_STA_KEY_END_INDEX_RST      0x0
/// STA_KEY_START_INDEX field reset value
#define NXMAC_STA_KEY_START_INDEX_RST    0x0

/**
 * @brief Constructs a value for the ENCR_RAM_CONFIG register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] nvap - The value to use for the nVAP field.
 * @param[in] stakeyendindex - The value to use for the staKeyEndIndex field.
 * @param[in] stakeystartindex - The value to use for the staKeyStartIndex field.
 */
__INLINE void nxmac_encr_ram_config_pack(uint8_t nvap, uint8_t stakeyendindex, uint8_t stakeystartindex)
{
    ASSERT_ERR((((uint32_t)nvap << 16) & ~((uint32_t)0x000F0000)) == 0);
    ASSERT_ERR((((uint32_t)stakeyendindex << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)stakeystartindex << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_ENCR_RAM_CONFIG_ADDR,  ((uint32_t)nvap << 16) | ((uint32_t)stakeyendindex << 8) | ((uint32_t)stakeystartindex << 0));
}

/**
 * @brief Unpacks ENCR_RAM_CONFIG's fields from current value of the ENCR_RAM_CONFIG register.
 *
 * Reads the ENCR_RAM_CONFIG register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] stakeymaxindex - Will be populated with the current value of this field from the register.
 * @param[out] nvap - Will be populated with the current value of this field from the register.
 * @param[out] stakeyendindex - Will be populated with the current value of this field from the register.
 * @param[out] stakeystartindex - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_encr_ram_config_unpack(uint8_t *stakeymaxindex, uint8_t *nvap, uint8_t *stakeyendindex, uint8_t *stakeystartindex)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_RAM_CONFIG_ADDR);

    *stakeymaxindex = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *nvap = (localVal & ((uint32_t)0x000F0000)) >> 16;
    *stakeyendindex = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *stakeystartindex = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the staKeyMaxIndex field in the ENCR_RAM_CONFIG register.
 *
 * The ENCR_RAM_CONFIG register will be read and the staKeyMaxIndex field's value will be returned.
 *
 * @return The current value of the staKeyMaxIndex field in the ENCR_RAM_CONFIG register.
 */
__INLINE uint8_t nxmac_sta_key_max_index_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_RAM_CONFIG_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Returns the current value of the nVAP field in the ENCR_RAM_CONFIG register.
 *
 * The ENCR_RAM_CONFIG register will be read and the nVAP field's value will be returned.
 *
 * @return The current value of the nVAP field in the ENCR_RAM_CONFIG register.
 */
__INLINE uint8_t nxmac_n_vap_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_RAM_CONFIG_ADDR);
    return ((localVal & ((uint32_t)0x000F0000)) >> 16);
}

/**
 * @brief Sets the nVAP field of the ENCR_RAM_CONFIG register.
 *
 * The ENCR_RAM_CONFIG register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nvap - The value to set the field to.
 */
__INLINE void nxmac_n_vap_setf(uint8_t nvap)
{
    ASSERT_ERR((((uint32_t)nvap << 16) & ~((uint32_t)0x000F0000)) == 0);
    REG_PL_WR(NXMAC_ENCR_RAM_CONFIG_ADDR, (REG_PL_RD(NXMAC_ENCR_RAM_CONFIG_ADDR) & ~((uint32_t)0x000F0000)) | ((uint32_t)nvap << 16));
}

/**
 * @brief Returns the current value of the staKeyEndIndex field in the ENCR_RAM_CONFIG register.
 *
 * The ENCR_RAM_CONFIG register will be read and the staKeyEndIndex field's value will be returned.
 *
 * @return The current value of the staKeyEndIndex field in the ENCR_RAM_CONFIG register.
 */
__INLINE uint8_t nxmac_sta_key_end_index_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_RAM_CONFIG_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the staKeyEndIndex field of the ENCR_RAM_CONFIG register.
 *
 * The ENCR_RAM_CONFIG register will be read, modified to contain the new field value, and written.
 *
 * @param[in] stakeyendindex - The value to set the field to.
 */
__INLINE void nxmac_sta_key_end_index_setf(uint8_t stakeyendindex)
{
    ASSERT_ERR((((uint32_t)stakeyendindex << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(NXMAC_ENCR_RAM_CONFIG_ADDR, (REG_PL_RD(NXMAC_ENCR_RAM_CONFIG_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)stakeyendindex << 8));
}

/**
 * @brief Returns the current value of the staKeyStartIndex field in the ENCR_RAM_CONFIG register.
 *
 * The ENCR_RAM_CONFIG register will be read and the staKeyStartIndex field's value will be returned.
 *
 * @return The current value of the staKeyStartIndex field in the ENCR_RAM_CONFIG register.
 */
__INLINE uint8_t nxmac_sta_key_start_index_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ENCR_RAM_CONFIG_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the staKeyStartIndex field of the ENCR_RAM_CONFIG register.
 *
 * The ENCR_RAM_CONFIG register will be read, modified to contain the new field value, and written.
 *
 * @param[in] stakeystartindex - The value to set the field to.
 */
__INLINE void nxmac_sta_key_start_index_setf(uint8_t stakeystartindex)
{
    ASSERT_ERR((((uint32_t)stakeystartindex << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_ENCR_RAM_CONFIG_ADDR, (REG_PL_RD(NXMAC_ENCR_RAM_CONFIG_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)stakeystartindex << 0));
}

/// @}

/**
 * @name RATES register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  11:00      bssBasicRateSet   0x150
 * </pre>
 *
 * @{
 */

/// Address of the RATES register
#define NXMAC_RATES_ADDR   0xC00000DC
/// Offset of the RATES register from the base address
#define NXMAC_RATES_OFFSET 0x000000DC
/// Index of the RATES register
#define NXMAC_RATES_INDEX  0x00000037
/// Reset value of the RATES register
#define NXMAC_RATES_RESET  0x00000150

/**
 * @brief Returns the current value of the RATES register.
 * The RATES register will be read and its value returned.
 * @return The current value of the RATES register.
 */
__INLINE uint32_t nxmac_rates_get(void)
{
    return REG_PL_RD(NXMAC_RATES_ADDR);
}

/**
 * @brief Sets the RATES register to a value.
 * The RATES register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_rates_set(uint32_t value)
{
    REG_PL_WR(NXMAC_RATES_ADDR, value);
}

// field definitions
/// BSS_BASIC_RATE_SET field mask
#define NXMAC_BSS_BASIC_RATE_SET_MASK   ((uint32_t)0x00000FFF)
/// BSS_BASIC_RATE_SET field LSB position
#define NXMAC_BSS_BASIC_RATE_SET_LSB    0
/// BSS_BASIC_RATE_SET field width
#define NXMAC_BSS_BASIC_RATE_SET_WIDTH  ((uint32_t)0x0000000C)

/// BSS_BASIC_RATE_SET field reset value
#define NXMAC_BSS_BASIC_RATE_SET_RST    0x150

/**
 * @brief Returns the current value of the bssBasicRateSet field in the RATES register.
 *
 * The RATES register will be read and the bssBasicRateSet field's value will be returned.
 *
 * @return The current value of the bssBasicRateSet field in the RATES register.
 */
__INLINE uint16_t nxmac_bss_basic_rate_set_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RATES_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000FFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the bssBasicRateSet field of the RATES register.
 *
 * The RATES register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bssbasicrateset - The value to set the field to.
 */
__INLINE void nxmac_bss_basic_rate_set_setf(uint16_t bssbasicrateset)
{
    ASSERT_ERR((((uint32_t)bssbasicrateset << 0) & ~((uint32_t)0x00000FFF)) == 0);
    REG_PL_WR(NXMAC_RATES_ADDR, (uint32_t)bssbasicrateset << 0);
}

/// @}

/**
 * @name OLBC register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:24            dsssCount   0x0
 *  23:16            ofdmCount   0x0
 *  15:00            olbcTimer   0x0
 * </pre>
 *
 * @{
 */

/// Address of the OLBC register
#define NXMAC_OLBC_ADDR   0xC00000E0
/// Offset of the OLBC register from the base address
#define NXMAC_OLBC_OFFSET 0x000000E0
/// Index of the OLBC register
#define NXMAC_OLBC_INDEX  0x00000038
/// Reset value of the OLBC register
#define NXMAC_OLBC_RESET  0x00000000

/**
 * @brief Returns the current value of the OLBC register.
 * The OLBC register will be read and its value returned.
 * @return The current value of the OLBC register.
 */
__INLINE uint32_t nxmac_olbc_get(void)
{
    return REG_PL_RD(NXMAC_OLBC_ADDR);
}

/**
 * @brief Sets the OLBC register to a value.
 * The OLBC register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_olbc_set(uint32_t value)
{
    REG_PL_WR(NXMAC_OLBC_ADDR, value);
}

// field definitions
/// DSSS_COUNT field mask
#define NXMAC_DSSS_COUNT_MASK   ((uint32_t)0xFF000000)
/// DSSS_COUNT field LSB position
#define NXMAC_DSSS_COUNT_LSB    24
/// DSSS_COUNT field width
#define NXMAC_DSSS_COUNT_WIDTH  ((uint32_t)0x00000008)
/// OFDM_COUNT field mask
#define NXMAC_OFDM_COUNT_MASK   ((uint32_t)0x00FF0000)
/// OFDM_COUNT field LSB position
#define NXMAC_OFDM_COUNT_LSB    16
/// OFDM_COUNT field width
#define NXMAC_OFDM_COUNT_WIDTH  ((uint32_t)0x00000008)
/// OLBC_TIMER field mask
#define NXMAC_OLBC_TIMER_MASK   ((uint32_t)0x0000FFFF)
/// OLBC_TIMER field LSB position
#define NXMAC_OLBC_TIMER_LSB    0
/// OLBC_TIMER field width
#define NXMAC_OLBC_TIMER_WIDTH  ((uint32_t)0x00000010)

/// DSSS_COUNT field reset value
#define NXMAC_DSSS_COUNT_RST    0x0
/// OFDM_COUNT field reset value
#define NXMAC_OFDM_COUNT_RST    0x0
/// OLBC_TIMER field reset value
#define NXMAC_OLBC_TIMER_RST    0x0

/**
 * @brief Constructs a value for the OLBC register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] dssscount - The value to use for the dsssCount field.
 * @param[in] ofdmcount - The value to use for the ofdmCount field.
 * @param[in] olbctimer - The value to use for the olbcTimer field.
 */
__INLINE void nxmac_olbc_pack(uint8_t dssscount, uint8_t ofdmcount, uint16_t olbctimer)
{
    ASSERT_ERR((((uint32_t)dssscount << 24) & ~((uint32_t)0xFF000000)) == 0);
    ASSERT_ERR((((uint32_t)ofdmcount << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)olbctimer << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_OLBC_ADDR,  ((uint32_t)dssscount << 24) | ((uint32_t)ofdmcount << 16) | ((uint32_t)olbctimer << 0));
}

/**
 * @brief Unpacks OLBC's fields from current value of the OLBC register.
 *
 * Reads the OLBC register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] dssscount - Will be populated with the current value of this field from the register.
 * @param[out] ofdmcount - Will be populated with the current value of this field from the register.
 * @param[out] olbctimer - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_olbc_unpack(uint8_t *dssscount, uint8_t *ofdmcount, uint16_t *olbctimer)
{
    uint32_t localVal = REG_PL_RD(NXMAC_OLBC_ADDR);

    *dssscount = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *ofdmcount = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *olbctimer = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the dsssCount field in the OLBC register.
 *
 * The OLBC register will be read and the dsssCount field's value will be returned.
 *
 * @return The current value of the dsssCount field in the OLBC register.
 */
__INLINE uint8_t nxmac_dsss_count_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_OLBC_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Sets the dsssCount field of the OLBC register.
 *
 * The OLBC register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dssscount - The value to set the field to.
 */
__INLINE void nxmac_dsss_count_setf(uint8_t dssscount)
{
    ASSERT_ERR((((uint32_t)dssscount << 24) & ~((uint32_t)0xFF000000)) == 0);
    REG_PL_WR(NXMAC_OLBC_ADDR, (REG_PL_RD(NXMAC_OLBC_ADDR) & ~((uint32_t)0xFF000000)) | ((uint32_t)dssscount << 24));
}

/**
 * @brief Returns the current value of the ofdmCount field in the OLBC register.
 *
 * The OLBC register will be read and the ofdmCount field's value will be returned.
 *
 * @return The current value of the ofdmCount field in the OLBC register.
 */
__INLINE uint8_t nxmac_ofdm_count_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_OLBC_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the ofdmCount field of the OLBC register.
 *
 * The OLBC register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ofdmcount - The value to set the field to.
 */
__INLINE void nxmac_ofdm_count_setf(uint8_t ofdmcount)
{
    ASSERT_ERR((((uint32_t)ofdmcount << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(NXMAC_OLBC_ADDR, (REG_PL_RD(NXMAC_OLBC_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)ofdmcount << 16));
}

/**
 * @brief Returns the current value of the olbcTimer field in the OLBC register.
 *
 * The OLBC register will be read and the olbcTimer field's value will be returned.
 *
 * @return The current value of the olbcTimer field in the OLBC register.
 */
__INLINE uint16_t nxmac_olbc_timer_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_OLBC_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the olbcTimer field of the OLBC register.
 *
 * The OLBC register will be read, modified to contain the new field value, and written.
 *
 * @param[in] olbctimer - The value to set the field to.
 */
__INLINE void nxmac_olbc_timer_setf(uint16_t olbctimer)
{
    ASSERT_ERR((((uint32_t)olbctimer << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_OLBC_ADDR, (REG_PL_RD(NXMAC_OLBC_ADDR) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)olbctimer << 0));
}

/// @}

/**
 * @name TIMINGS_1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  27:18   txChainDelayInMACClk   0x0
 *  17:08    txRFDelayInMACClk   0x0
 *  07:00       macCoreClkFreq   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TIMINGS_1 register
#define NXMAC_TIMINGS_1_ADDR   0xC00000E4
/// Offset of the TIMINGS_1 register from the base address
#define NXMAC_TIMINGS_1_OFFSET 0x000000E4
/// Index of the TIMINGS_1 register
#define NXMAC_TIMINGS_1_INDEX  0x00000039
/// Reset value of the TIMINGS_1 register
#define NXMAC_TIMINGS_1_RESET  0x00000000

/**
 * @brief Returns the current value of the TIMINGS_1 register.
 * The TIMINGS_1 register will be read and its value returned.
 * @return The current value of the TIMINGS_1 register.
 */
__INLINE uint32_t nxmac_timings_1_get(void)
{
    return REG_PL_RD(NXMAC_TIMINGS_1_ADDR);
}

/**
 * @brief Sets the TIMINGS_1 register to a value.
 * The TIMINGS_1 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_timings_1_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TIMINGS_1_ADDR, value);
}

// field definitions
/// TX_CHAIN_DELAY_IN_MAC_CLK field mask
#define NXMAC_TX_CHAIN_DELAY_IN_MAC_CLK_MASK   ((uint32_t)0x0FFC0000)
/// TX_CHAIN_DELAY_IN_MAC_CLK field LSB position
#define NXMAC_TX_CHAIN_DELAY_IN_MAC_CLK_LSB    18
/// TX_CHAIN_DELAY_IN_MAC_CLK field width
#define NXMAC_TX_CHAIN_DELAY_IN_MAC_CLK_WIDTH  ((uint32_t)0x0000000A)
/// TX_RF_DELAY_IN_MAC_CLK field mask
#define NXMAC_TX_RF_DELAY_IN_MAC_CLK_MASK      ((uint32_t)0x0003FF00)
/// TX_RF_DELAY_IN_MAC_CLK field LSB position
#define NXMAC_TX_RF_DELAY_IN_MAC_CLK_LSB       8
/// TX_RF_DELAY_IN_MAC_CLK field width
#define NXMAC_TX_RF_DELAY_IN_MAC_CLK_WIDTH     ((uint32_t)0x0000000A)
/// MAC_CORE_CLK_FREQ field mask
#define NXMAC_MAC_CORE_CLK_FREQ_MASK           ((uint32_t)0x000000FF)
/// MAC_CORE_CLK_FREQ field LSB position
#define NXMAC_MAC_CORE_CLK_FREQ_LSB            0
/// MAC_CORE_CLK_FREQ field width
#define NXMAC_MAC_CORE_CLK_FREQ_WIDTH          ((uint32_t)0x00000008)

/// TX_CHAIN_DELAY_IN_MAC_CLK field reset value
#define NXMAC_TX_CHAIN_DELAY_IN_MAC_CLK_RST    0x0
/// TX_RF_DELAY_IN_MAC_CLK field reset value
#define NXMAC_TX_RF_DELAY_IN_MAC_CLK_RST       0x0
/// MAC_CORE_CLK_FREQ field reset value
#define NXMAC_MAC_CORE_CLK_FREQ_RST            0x0

/**
 * @brief Constructs a value for the TIMINGS_1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] txchaindelayinmacclk - The value to use for the txChainDelayInMACClk field.
 * @param[in] txrfdelayinmacclk - The value to use for the txRFDelayInMACClk field.
 * @param[in] maccoreclkfreq - The value to use for the macCoreClkFreq field.
 */
__INLINE void nxmac_timings_1_pack(uint16_t txchaindelayinmacclk, uint16_t txrfdelayinmacclk, uint8_t maccoreclkfreq)
{
    ASSERT_ERR((((uint32_t)txchaindelayinmacclk << 18) & ~((uint32_t)0x0FFC0000)) == 0);
    ASSERT_ERR((((uint32_t)txrfdelayinmacclk << 8) & ~((uint32_t)0x0003FF00)) == 0);
    ASSERT_ERR((((uint32_t)maccoreclkfreq << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_1_ADDR,  ((uint32_t)txchaindelayinmacclk << 18) | ((uint32_t)txrfdelayinmacclk << 8) | ((uint32_t)maccoreclkfreq << 0));
}

/**
 * @brief Unpacks TIMINGS_1's fields from current value of the TIMINGS_1 register.
 *
 * Reads the TIMINGS_1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txchaindelayinmacclk - Will be populated with the current value of this field from the register.
 * @param[out] txrfdelayinmacclk - Will be populated with the current value of this field from the register.
 * @param[out] maccoreclkfreq - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_timings_1_unpack(uint16_t *txchaindelayinmacclk, uint16_t *txrfdelayinmacclk, uint8_t *maccoreclkfreq)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_1_ADDR);

    *txchaindelayinmacclk = (localVal & ((uint32_t)0x0FFC0000)) >> 18;
    *txrfdelayinmacclk = (localVal & ((uint32_t)0x0003FF00)) >> 8;
    *maccoreclkfreq = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the txChainDelayInMACClk field in the TIMINGS_1 register.
 *
 * The TIMINGS_1 register will be read and the txChainDelayInMACClk field's value will be returned.
 *
 * @return The current value of the txChainDelayInMACClk field in the TIMINGS_1 register.
 */
__INLINE uint16_t nxmac_tx_chain_delay_in_mac_clk_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_1_ADDR);
    return ((localVal & ((uint32_t)0x0FFC0000)) >> 18);
}

/**
 * @brief Sets the txChainDelayInMACClk field of the TIMINGS_1 register.
 *
 * The TIMINGS_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txchaindelayinmacclk - The value to set the field to.
 */
__INLINE void nxmac_tx_chain_delay_in_mac_clk_setf(uint16_t txchaindelayinmacclk)
{
    ASSERT_ERR((((uint32_t)txchaindelayinmacclk << 18) & ~((uint32_t)0x0FFC0000)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_1_ADDR, (REG_PL_RD(NXMAC_TIMINGS_1_ADDR) & ~((uint32_t)0x0FFC0000)) | ((uint32_t)txchaindelayinmacclk << 18));
}

/**
 * @brief Returns the current value of the txRFDelayInMACClk field in the TIMINGS_1 register.
 *
 * The TIMINGS_1 register will be read and the txRFDelayInMACClk field's value will be returned.
 *
 * @return The current value of the txRFDelayInMACClk field in the TIMINGS_1 register.
 */
__INLINE uint16_t nxmac_tx_rf_delay_in_mac_clk_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_1_ADDR);
    return ((localVal & ((uint32_t)0x0003FF00)) >> 8);
}

/**
 * @brief Sets the txRFDelayInMACClk field of the TIMINGS_1 register.
 *
 * The TIMINGS_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txrfdelayinmacclk - The value to set the field to.
 */
__INLINE void nxmac_tx_rf_delay_in_mac_clk_setf(uint16_t txrfdelayinmacclk)
{
    ASSERT_ERR((((uint32_t)txrfdelayinmacclk << 8) & ~((uint32_t)0x0003FF00)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_1_ADDR, (REG_PL_RD(NXMAC_TIMINGS_1_ADDR) & ~((uint32_t)0x0003FF00)) | ((uint32_t)txrfdelayinmacclk << 8));
}

/**
 * @brief Returns the current value of the macCoreClkFreq field in the TIMINGS_1 register.
 *
 * The TIMINGS_1 register will be read and the macCoreClkFreq field's value will be returned.
 *
 * @return The current value of the macCoreClkFreq field in the TIMINGS_1 register.
 */
__INLINE uint8_t nxmac_mac_core_clk_freq_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_1_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the macCoreClkFreq field of the TIMINGS_1 register.
 *
 * The TIMINGS_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maccoreclkfreq - The value to set the field to.
 */
__INLINE void nxmac_mac_core_clk_freq_setf(uint8_t maccoreclkfreq)
{
    ASSERT_ERR((((uint32_t)maccoreclkfreq << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_1_ADDR, (REG_PL_RD(NXMAC_TIMINGS_1_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)maccoreclkfreq << 0));
}

/// @}

/**
 * @name TIMINGS_2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  23:08     slotTimeInMACClk   0x0
 *  07:00             slotTime   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TIMINGS_2 register
#define NXMAC_TIMINGS_2_ADDR   0xC00000E8
/// Offset of the TIMINGS_2 register from the base address
#define NXMAC_TIMINGS_2_OFFSET 0x000000E8
/// Index of the TIMINGS_2 register
#define NXMAC_TIMINGS_2_INDEX  0x0000003A
/// Reset value of the TIMINGS_2 register
#define NXMAC_TIMINGS_2_RESET  0x00000000

/**
 * @brief Returns the current value of the TIMINGS_2 register.
 * The TIMINGS_2 register will be read and its value returned.
 * @return The current value of the TIMINGS_2 register.
 */
__INLINE uint32_t nxmac_timings_2_get(void)
{
    return REG_PL_RD(NXMAC_TIMINGS_2_ADDR);
}

/**
 * @brief Sets the TIMINGS_2 register to a value.
 * The TIMINGS_2 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_timings_2_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TIMINGS_2_ADDR, value);
}

// field definitions
/// SLOT_TIME_IN_MAC_CLK field mask
#define NXMAC_SLOT_TIME_IN_MAC_CLK_MASK   ((uint32_t)0x00FFFF00)
/// SLOT_TIME_IN_MAC_CLK field LSB position
#define NXMAC_SLOT_TIME_IN_MAC_CLK_LSB    8
/// SLOT_TIME_IN_MAC_CLK field width
#define NXMAC_SLOT_TIME_IN_MAC_CLK_WIDTH  ((uint32_t)0x00000010)
/// SLOT_TIME field mask
#define NXMAC_SLOT_TIME_MASK              ((uint32_t)0x000000FF)
/// SLOT_TIME field LSB position
#define NXMAC_SLOT_TIME_LSB               0
/// SLOT_TIME field width
#define NXMAC_SLOT_TIME_WIDTH             ((uint32_t)0x00000008)

/// SLOT_TIME_IN_MAC_CLK field reset value
#define NXMAC_SLOT_TIME_IN_MAC_CLK_RST    0x0
/// SLOT_TIME field reset value
#define NXMAC_SLOT_TIME_RST               0x0

/**
 * @brief Constructs a value for the TIMINGS_2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] slottimeinmacclk - The value to use for the slotTimeInMACClk field.
 * @param[in] slottime - The value to use for the slotTime field.
 */
__INLINE void nxmac_timings_2_pack(uint16_t slottimeinmacclk, uint8_t slottime)
{
    ASSERT_ERR((((uint32_t)slottimeinmacclk << 8) & ~((uint32_t)0x00FFFF00)) == 0);
    ASSERT_ERR((((uint32_t)slottime << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_2_ADDR,  ((uint32_t)slottimeinmacclk << 8) | ((uint32_t)slottime << 0));
}

/**
 * @brief Unpacks TIMINGS_2's fields from current value of the TIMINGS_2 register.
 *
 * Reads the TIMINGS_2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] slottimeinmacclk - Will be populated with the current value of this field from the register.
 * @param[out] slottime - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_timings_2_unpack(uint16_t *slottimeinmacclk, uint8_t *slottime)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_2_ADDR);

    *slottimeinmacclk = (localVal & ((uint32_t)0x00FFFF00)) >> 8;
    *slottime = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the slotTimeInMACClk field in the TIMINGS_2 register.
 *
 * The TIMINGS_2 register will be read and the slotTimeInMACClk field's value will be returned.
 *
 * @return The current value of the slotTimeInMACClk field in the TIMINGS_2 register.
 */
__INLINE uint16_t nxmac_slot_time_in_mac_clk_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_2_ADDR);
    return ((localVal & ((uint32_t)0x00FFFF00)) >> 8);
}

/**
 * @brief Sets the slotTimeInMACClk field of the TIMINGS_2 register.
 *
 * The TIMINGS_2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] slottimeinmacclk - The value to set the field to.
 */
__INLINE void nxmac_slot_time_in_mac_clk_setf(uint16_t slottimeinmacclk)
{
    ASSERT_ERR((((uint32_t)slottimeinmacclk << 8) & ~((uint32_t)0x00FFFF00)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_2_ADDR, (REG_PL_RD(NXMAC_TIMINGS_2_ADDR) & ~((uint32_t)0x00FFFF00)) | ((uint32_t)slottimeinmacclk << 8));
}

/**
 * @brief Returns the current value of the slotTime field in the TIMINGS_2 register.
 *
 * The TIMINGS_2 register will be read and the slotTime field's value will be returned.
 *
 * @return The current value of the slotTime field in the TIMINGS_2 register.
 */
__INLINE uint8_t nxmac_slot_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_2_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the slotTime field of the TIMINGS_2 register.
 *
 * The TIMINGS_2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] slottime - The value to set the field to.
 */
__INLINE void nxmac_slot_time_setf(uint8_t slottime)
{
    ASSERT_ERR((((uint32_t)slottime << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_2_ADDR, (REG_PL_RD(NXMAC_TIMINGS_2_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)slottime << 0));
}

/// @}

/**
 * @name TIMINGS_3 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:20    rxRFDelayInMACClk   0x0
 *  19:10   txDelayRFOnInMACClk   0x0
 *  09:00   macProcDelayInMACClk   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TIMINGS_3 register
#define NXMAC_TIMINGS_3_ADDR   0xC00000EC
/// Offset of the TIMINGS_3 register from the base address
#define NXMAC_TIMINGS_3_OFFSET 0x000000EC
/// Index of the TIMINGS_3 register
#define NXMAC_TIMINGS_3_INDEX  0x0000003B
/// Reset value of the TIMINGS_3 register
#define NXMAC_TIMINGS_3_RESET  0x00000000

/**
 * @brief Returns the current value of the TIMINGS_3 register.
 * The TIMINGS_3 register will be read and its value returned.
 * @return The current value of the TIMINGS_3 register.
 */
__INLINE uint32_t nxmac_timings_3_get(void)
{
    return REG_PL_RD(NXMAC_TIMINGS_3_ADDR);
}

/**
 * @brief Sets the TIMINGS_3 register to a value.
 * The TIMINGS_3 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_timings_3_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TIMINGS_3_ADDR, value);
}

// field definitions
/// RX_RF_DELAY_IN_MAC_CLK field mask
#define NXMAC_RX_RF_DELAY_IN_MAC_CLK_MASK      ((uint32_t)0x3FF00000)
/// RX_RF_DELAY_IN_MAC_CLK field LSB position
#define NXMAC_RX_RF_DELAY_IN_MAC_CLK_LSB       20
/// RX_RF_DELAY_IN_MAC_CLK field width
#define NXMAC_RX_RF_DELAY_IN_MAC_CLK_WIDTH     ((uint32_t)0x0000000A)
/// TX_DELAY_RF_ON_IN_MAC_CLK field mask
#define NXMAC_TX_DELAY_RF_ON_IN_MAC_CLK_MASK   ((uint32_t)0x000FFC00)
/// TX_DELAY_RF_ON_IN_MAC_CLK field LSB position
#define NXMAC_TX_DELAY_RF_ON_IN_MAC_CLK_LSB    10
/// TX_DELAY_RF_ON_IN_MAC_CLK field width
#define NXMAC_TX_DELAY_RF_ON_IN_MAC_CLK_WIDTH  ((uint32_t)0x0000000A)
/// MAC_PROC_DELAY_IN_MAC_CLK field mask
#define NXMAC_MAC_PROC_DELAY_IN_MAC_CLK_MASK   ((uint32_t)0x000003FF)
/// MAC_PROC_DELAY_IN_MAC_CLK field LSB position
#define NXMAC_MAC_PROC_DELAY_IN_MAC_CLK_LSB    0
/// MAC_PROC_DELAY_IN_MAC_CLK field width
#define NXMAC_MAC_PROC_DELAY_IN_MAC_CLK_WIDTH  ((uint32_t)0x0000000A)

/// RX_RF_DELAY_IN_MAC_CLK field reset value
#define NXMAC_RX_RF_DELAY_IN_MAC_CLK_RST       0x0
/// TX_DELAY_RF_ON_IN_MAC_CLK field reset value
#define NXMAC_TX_DELAY_RF_ON_IN_MAC_CLK_RST    0x0
/// MAC_PROC_DELAY_IN_MAC_CLK field reset value
#define NXMAC_MAC_PROC_DELAY_IN_MAC_CLK_RST    0x0

/**
 * @brief Constructs a value for the TIMINGS_3 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxrfdelayinmacclk - The value to use for the rxRFDelayInMACClk field.
 * @param[in] txdelayrfoninmacclk - The value to use for the txDelayRFOnInMACClk field.
 * @param[in] macprocdelayinmacclk - The value to use for the macProcDelayInMACClk field.
 */
__INLINE void nxmac_timings_3_pack(uint16_t rxrfdelayinmacclk, uint16_t txdelayrfoninmacclk, uint16_t macprocdelayinmacclk)
{
    ASSERT_ERR((((uint32_t)rxrfdelayinmacclk << 20) & ~((uint32_t)0x3FF00000)) == 0);
    ASSERT_ERR((((uint32_t)txdelayrfoninmacclk << 10) & ~((uint32_t)0x000FFC00)) == 0);
    ASSERT_ERR((((uint32_t)macprocdelayinmacclk << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_3_ADDR,  ((uint32_t)rxrfdelayinmacclk << 20) | ((uint32_t)txdelayrfoninmacclk << 10) | ((uint32_t)macprocdelayinmacclk << 0));
}

/**
 * @brief Unpacks TIMINGS_3's fields from current value of the TIMINGS_3 register.
 *
 * Reads the TIMINGS_3 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxrfdelayinmacclk - Will be populated with the current value of this field from the register.
 * @param[out] txdelayrfoninmacclk - Will be populated with the current value of this field from the register.
 * @param[out] macprocdelayinmacclk - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_timings_3_unpack(uint16_t *rxrfdelayinmacclk, uint16_t *txdelayrfoninmacclk, uint16_t *macprocdelayinmacclk)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_3_ADDR);

    *rxrfdelayinmacclk = (localVal & ((uint32_t)0x3FF00000)) >> 20;
    *txdelayrfoninmacclk = (localVal & ((uint32_t)0x000FFC00)) >> 10;
    *macprocdelayinmacclk = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the rxRFDelayInMACClk field in the TIMINGS_3 register.
 *
 * The TIMINGS_3 register will be read and the rxRFDelayInMACClk field's value will be returned.
 *
 * @return The current value of the rxRFDelayInMACClk field in the TIMINGS_3 register.
 */
__INLINE uint16_t nxmac_rx_rf_delay_in_mac_clk_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_3_ADDR);
    return ((localVal & ((uint32_t)0x3FF00000)) >> 20);
}

/**
 * @brief Sets the rxRFDelayInMACClk field of the TIMINGS_3 register.
 *
 * The TIMINGS_3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxrfdelayinmacclk - The value to set the field to.
 */
__INLINE void nxmac_rx_rf_delay_in_mac_clk_setf(uint16_t rxrfdelayinmacclk)
{
    ASSERT_ERR((((uint32_t)rxrfdelayinmacclk << 20) & ~((uint32_t)0x3FF00000)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_3_ADDR, (REG_PL_RD(NXMAC_TIMINGS_3_ADDR) & ~((uint32_t)0x3FF00000)) | ((uint32_t)rxrfdelayinmacclk << 20));
}

/**
 * @brief Returns the current value of the txDelayRFOnInMACClk field in the TIMINGS_3 register.
 *
 * The TIMINGS_3 register will be read and the txDelayRFOnInMACClk field's value will be returned.
 *
 * @return The current value of the txDelayRFOnInMACClk field in the TIMINGS_3 register.
 */
__INLINE uint16_t nxmac_tx_delay_rf_on_in_mac_clk_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_3_ADDR);
    return ((localVal & ((uint32_t)0x000FFC00)) >> 10);
}

/**
 * @brief Sets the txDelayRFOnInMACClk field of the TIMINGS_3 register.
 *
 * The TIMINGS_3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txdelayrfoninmacclk - The value to set the field to.
 */
__INLINE void nxmac_tx_delay_rf_on_in_mac_clk_setf(uint16_t txdelayrfoninmacclk)
{
    ASSERT_ERR((((uint32_t)txdelayrfoninmacclk << 10) & ~((uint32_t)0x000FFC00)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_3_ADDR, (REG_PL_RD(NXMAC_TIMINGS_3_ADDR) & ~((uint32_t)0x000FFC00)) | ((uint32_t)txdelayrfoninmacclk << 10));
}

/**
 * @brief Returns the current value of the macProcDelayInMACClk field in the TIMINGS_3 register.
 *
 * The TIMINGS_3 register will be read and the macProcDelayInMACClk field's value will be returned.
 *
 * @return The current value of the macProcDelayInMACClk field in the TIMINGS_3 register.
 */
__INLINE uint16_t nxmac_mac_proc_delay_in_mac_clk_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_3_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the macProcDelayInMACClk field of the TIMINGS_3 register.
 *
 * The TIMINGS_3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macprocdelayinmacclk - The value to set the field to.
 */
__INLINE void nxmac_mac_proc_delay_in_mac_clk_setf(uint16_t macprocdelayinmacclk)
{
    ASSERT_ERR((((uint32_t)macprocdelayinmacclk << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_3_ADDR, (REG_PL_RD(NXMAC_TIMINGS_3_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)macprocdelayinmacclk << 0));
}

/// @}

/**
 * @name TIMINGS_4 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:22      radioWakeUpTime   0x0
 *  21:12       radioChirpTime   0x0
 *  01:00     wt2CryptClkRatio   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TIMINGS_4 register
#define NXMAC_TIMINGS_4_ADDR   0xC00000F0
/// Offset of the TIMINGS_4 register from the base address
#define NXMAC_TIMINGS_4_OFFSET 0x000000F0
/// Index of the TIMINGS_4 register
#define NXMAC_TIMINGS_4_INDEX  0x0000003C
/// Reset value of the TIMINGS_4 register
#define NXMAC_TIMINGS_4_RESET  0x00000000

/**
 * @brief Returns the current value of the TIMINGS_4 register.
 * The TIMINGS_4 register will be read and its value returned.
 * @return The current value of the TIMINGS_4 register.
 */
__INLINE uint32_t nxmac_timings_4_get(void)
{
    return REG_PL_RD(NXMAC_TIMINGS_4_ADDR);
}

/**
 * @brief Sets the TIMINGS_4 register to a value.
 * The TIMINGS_4 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_timings_4_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TIMINGS_4_ADDR, value);
}

// field definitions
/// RADIO_WAKE_UP_TIME field mask
#define NXMAC_RADIO_WAKE_UP_TIME_MASK     ((uint32_t)0xFFC00000)
/// RADIO_WAKE_UP_TIME field LSB position
#define NXMAC_RADIO_WAKE_UP_TIME_LSB      22
/// RADIO_WAKE_UP_TIME field width
#define NXMAC_RADIO_WAKE_UP_TIME_WIDTH    ((uint32_t)0x0000000A)
/// RADIO_CHIRP_TIME field mask
#define NXMAC_RADIO_CHIRP_TIME_MASK       ((uint32_t)0x003FF000)
/// RADIO_CHIRP_TIME field LSB position
#define NXMAC_RADIO_CHIRP_TIME_LSB        12
/// RADIO_CHIRP_TIME field width
#define NXMAC_RADIO_CHIRP_TIME_WIDTH      ((uint32_t)0x0000000A)
/// WT_2_CRYPT_CLK_RATIO field mask
#define NXMAC_WT_2_CRYPT_CLK_RATIO_MASK   ((uint32_t)0x00000003)
/// WT_2_CRYPT_CLK_RATIO field LSB position
#define NXMAC_WT_2_CRYPT_CLK_RATIO_LSB    0
/// WT_2_CRYPT_CLK_RATIO field width
#define NXMAC_WT_2_CRYPT_CLK_RATIO_WIDTH  ((uint32_t)0x00000002)

/// RADIO_WAKE_UP_TIME field reset value
#define NXMAC_RADIO_WAKE_UP_TIME_RST      0x0
/// RADIO_CHIRP_TIME field reset value
#define NXMAC_RADIO_CHIRP_TIME_RST        0x0
/// WT_2_CRYPT_CLK_RATIO field reset value
#define NXMAC_WT_2_CRYPT_CLK_RATIO_RST    0x0

/**
 * @brief Constructs a value for the TIMINGS_4 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] radiowakeuptime - The value to use for the radioWakeUpTime field.
 * @param[in] radiochirptime - The value to use for the radioChirpTime field.
 * @param[in] wt2cryptclkratio - The value to use for the wt2CryptClkRatio field.
 */
__INLINE void nxmac_timings_4_pack(uint16_t radiowakeuptime, uint16_t radiochirptime, uint8_t wt2cryptclkratio)
{
    ASSERT_ERR((((uint32_t)radiowakeuptime << 22) & ~((uint32_t)0xFFC00000)) == 0);
    ASSERT_ERR((((uint32_t)radiochirptime << 12) & ~((uint32_t)0x003FF000)) == 0);
    ASSERT_ERR((((uint32_t)wt2cryptclkratio << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_4_ADDR,  ((uint32_t)radiowakeuptime << 22) | ((uint32_t)radiochirptime << 12) | ((uint32_t)wt2cryptclkratio << 0));
}

/**
 * @brief Unpacks TIMINGS_4's fields from current value of the TIMINGS_4 register.
 *
 * Reads the TIMINGS_4 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] radiowakeuptime - Will be populated with the current value of this field from the register.
 * @param[out] radiochirptime - Will be populated with the current value of this field from the register.
 * @param[out] wt2cryptclkratio - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_timings_4_unpack(uint16_t *radiowakeuptime, uint16_t *radiochirptime, uint8_t *wt2cryptclkratio)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_4_ADDR);

    *radiowakeuptime = (localVal & ((uint32_t)0xFFC00000)) >> 22;
    *radiochirptime = (localVal & ((uint32_t)0x003FF000)) >> 12;
    *wt2cryptclkratio = (localVal & ((uint32_t)0x00000003)) >> 0;
}

/**
 * @brief Returns the current value of the radioWakeUpTime field in the TIMINGS_4 register.
 *
 * The TIMINGS_4 register will be read and the radioWakeUpTime field's value will be returned.
 *
 * @return The current value of the radioWakeUpTime field in the TIMINGS_4 register.
 */
__INLINE uint16_t nxmac_radio_wake_up_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_4_ADDR);
    return ((localVal & ((uint32_t)0xFFC00000)) >> 22);
}

/**
 * @brief Sets the radioWakeUpTime field of the TIMINGS_4 register.
 *
 * The TIMINGS_4 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] radiowakeuptime - The value to set the field to.
 */
__INLINE void nxmac_radio_wake_up_time_setf(uint16_t radiowakeuptime)
{
    ASSERT_ERR((((uint32_t)radiowakeuptime << 22) & ~((uint32_t)0xFFC00000)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_4_ADDR, (REG_PL_RD(NXMAC_TIMINGS_4_ADDR) & ~((uint32_t)0xFFC00000)) | ((uint32_t)radiowakeuptime << 22));
}

/**
 * @brief Returns the current value of the radioChirpTime field in the TIMINGS_4 register.
 *
 * The TIMINGS_4 register will be read and the radioChirpTime field's value will be returned.
 *
 * @return The current value of the radioChirpTime field in the TIMINGS_4 register.
 */
__INLINE uint16_t nxmac_radio_chirp_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_4_ADDR);
    return ((localVal & ((uint32_t)0x003FF000)) >> 12);
}

/**
 * @brief Sets the radioChirpTime field of the TIMINGS_4 register.
 *
 * The TIMINGS_4 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] radiochirptime - The value to set the field to.
 */
__INLINE void nxmac_radio_chirp_time_setf(uint16_t radiochirptime)
{
    ASSERT_ERR((((uint32_t)radiochirptime << 12) & ~((uint32_t)0x003FF000)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_4_ADDR, (REG_PL_RD(NXMAC_TIMINGS_4_ADDR) & ~((uint32_t)0x003FF000)) | ((uint32_t)radiochirptime << 12));
}

/**
 * @brief Returns the current value of the wt2CryptClkRatio field in the TIMINGS_4 register.
 *
 * The TIMINGS_4 register will be read and the wt2CryptClkRatio field's value will be returned.
 *
 * @return The current value of the wt2CryptClkRatio field in the TIMINGS_4 register.
 */
__INLINE uint8_t nxmac_wt_2_crypt_clk_ratio_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_4_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

/**
 * @brief Sets the wt2CryptClkRatio field of the TIMINGS_4 register.
 *
 * The TIMINGS_4 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] wt2cryptclkratio - The value to set the field to.
 */
__INLINE void nxmac_wt_2_crypt_clk_ratio_setf(uint8_t wt2cryptclkratio)
{
    ASSERT_ERR((((uint32_t)wt2cryptclkratio << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_4_ADDR, (REG_PL_RD(NXMAC_TIMINGS_4_ADDR) & ~((uint32_t)0x00000003)) | ((uint32_t)wt2cryptclkratio << 0));
}

/// @}

/**
 * @name TIMINGS_5 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  23:08        sifsBInMACClk   0x0
 *  07:00                sifsB   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TIMINGS_5 register
#define NXMAC_TIMINGS_5_ADDR   0xC00000F4
/// Offset of the TIMINGS_5 register from the base address
#define NXMAC_TIMINGS_5_OFFSET 0x000000F4
/// Index of the TIMINGS_5 register
#define NXMAC_TIMINGS_5_INDEX  0x0000003D
/// Reset value of the TIMINGS_5 register
#define NXMAC_TIMINGS_5_RESET  0x00000000

/**
 * @brief Returns the current value of the TIMINGS_5 register.
 * The TIMINGS_5 register will be read and its value returned.
 * @return The current value of the TIMINGS_5 register.
 */
__INLINE uint32_t nxmac_timings_5_get(void)
{
    return REG_PL_RD(NXMAC_TIMINGS_5_ADDR);
}

/**
 * @brief Sets the TIMINGS_5 register to a value.
 * The TIMINGS_5 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_timings_5_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TIMINGS_5_ADDR, value);
}

// field definitions
/// SIFS_B_IN_MAC_CLK field mask
#define NXMAC_SIFS_B_IN_MAC_CLK_MASK   ((uint32_t)0x00FFFF00)
/// SIFS_B_IN_MAC_CLK field LSB position
#define NXMAC_SIFS_B_IN_MAC_CLK_LSB    8
/// SIFS_B_IN_MAC_CLK field width
#define NXMAC_SIFS_B_IN_MAC_CLK_WIDTH  ((uint32_t)0x00000010)
/// SIFS_B field mask
#define NXMAC_SIFS_B_MASK              ((uint32_t)0x000000FF)
/// SIFS_B field LSB position
#define NXMAC_SIFS_B_LSB               0
/// SIFS_B field width
#define NXMAC_SIFS_B_WIDTH             ((uint32_t)0x00000008)

/// SIFS_B_IN_MAC_CLK field reset value
#define NXMAC_SIFS_B_IN_MAC_CLK_RST    0x0
/// SIFS_B field reset value
#define NXMAC_SIFS_B_RST               0x0

/**
 * @brief Constructs a value for the TIMINGS_5 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] sifsbinmacclk - The value to use for the sifsBInMACClk field.
 * @param[in] sifsb - The value to use for the sifsB field.
 */
__INLINE void nxmac_timings_5_pack(uint16_t sifsbinmacclk, uint8_t sifsb)
{
    ASSERT_ERR((((uint32_t)sifsbinmacclk << 8) & ~((uint32_t)0x00FFFF00)) == 0);
    ASSERT_ERR((((uint32_t)sifsb << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_5_ADDR,  ((uint32_t)sifsbinmacclk << 8) | ((uint32_t)sifsb << 0));
}

/**
 * @brief Unpacks TIMINGS_5's fields from current value of the TIMINGS_5 register.
 *
 * Reads the TIMINGS_5 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] sifsbinmacclk - Will be populated with the current value of this field from the register.
 * @param[out] sifsb - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_timings_5_unpack(uint16_t *sifsbinmacclk, uint8_t *sifsb)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_5_ADDR);

    *sifsbinmacclk = (localVal & ((uint32_t)0x00FFFF00)) >> 8;
    *sifsb = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the sifsBInMACClk field in the TIMINGS_5 register.
 *
 * The TIMINGS_5 register will be read and the sifsBInMACClk field's value will be returned.
 *
 * @return The current value of the sifsBInMACClk field in the TIMINGS_5 register.
 */
__INLINE uint16_t nxmac_sifs_b_in_mac_clk_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_5_ADDR);
    return ((localVal & ((uint32_t)0x00FFFF00)) >> 8);
}

/**
 * @brief Sets the sifsBInMACClk field of the TIMINGS_5 register.
 *
 * The TIMINGS_5 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] sifsbinmacclk - The value to set the field to.
 */
__INLINE void nxmac_sifs_b_in_mac_clk_setf(uint16_t sifsbinmacclk)
{
    ASSERT_ERR((((uint32_t)sifsbinmacclk << 8) & ~((uint32_t)0x00FFFF00)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_5_ADDR, (REG_PL_RD(NXMAC_TIMINGS_5_ADDR) & ~((uint32_t)0x00FFFF00)) | ((uint32_t)sifsbinmacclk << 8));
}

/**
 * @brief Returns the current value of the sifsB field in the TIMINGS_5 register.
 *
 * The TIMINGS_5 register will be read and the sifsB field's value will be returned.
 *
 * @return The current value of the sifsB field in the TIMINGS_5 register.
 */
__INLINE uint8_t nxmac_sifs_b_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_5_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the sifsB field of the TIMINGS_5 register.
 *
 * The TIMINGS_5 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] sifsb - The value to set the field to.
 */
__INLINE void nxmac_sifs_b_setf(uint8_t sifsb)
{
    ASSERT_ERR((((uint32_t)sifsb << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_5_ADDR, (REG_PL_RD(NXMAC_TIMINGS_5_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)sifsb << 0));
}

/// @}

/**
 * @name TIMINGS_6 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  23:08        sifsAInMACClk   0x0
 *  07:00                sifsA   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TIMINGS_6 register
#define NXMAC_TIMINGS_6_ADDR   0xC00000F8
/// Offset of the TIMINGS_6 register from the base address
#define NXMAC_TIMINGS_6_OFFSET 0x000000F8
/// Index of the TIMINGS_6 register
#define NXMAC_TIMINGS_6_INDEX  0x0000003E
/// Reset value of the TIMINGS_6 register
#define NXMAC_TIMINGS_6_RESET  0x00000000

/**
 * @brief Returns the current value of the TIMINGS_6 register.
 * The TIMINGS_6 register will be read and its value returned.
 * @return The current value of the TIMINGS_6 register.
 */
__INLINE uint32_t nxmac_timings_6_get(void)
{
    return REG_PL_RD(NXMAC_TIMINGS_6_ADDR);
}

/**
 * @brief Sets the TIMINGS_6 register to a value.
 * The TIMINGS_6 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_timings_6_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TIMINGS_6_ADDR, value);
}

// field definitions
/// SIFS_A_IN_MAC_CLK field mask
#define NXMAC_SIFS_A_IN_MAC_CLK_MASK   ((uint32_t)0x00FFFF00)
/// SIFS_A_IN_MAC_CLK field LSB position
#define NXMAC_SIFS_A_IN_MAC_CLK_LSB    8
/// SIFS_A_IN_MAC_CLK field width
#define NXMAC_SIFS_A_IN_MAC_CLK_WIDTH  ((uint32_t)0x00000010)
/// SIFS_A field mask
#define NXMAC_SIFS_A_MASK              ((uint32_t)0x000000FF)
/// SIFS_A field LSB position
#define NXMAC_SIFS_A_LSB               0
/// SIFS_A field width
#define NXMAC_SIFS_A_WIDTH             ((uint32_t)0x00000008)

/// SIFS_A_IN_MAC_CLK field reset value
#define NXMAC_SIFS_A_IN_MAC_CLK_RST    0x0
/// SIFS_A field reset value
#define NXMAC_SIFS_A_RST               0x0

/**
 * @brief Constructs a value for the TIMINGS_6 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] sifsainmacclk - The value to use for the sifsAInMACClk field.
 * @param[in] sifsa - The value to use for the sifsA field.
 */
__INLINE void nxmac_timings_6_pack(uint16_t sifsainmacclk, uint8_t sifsa)
{
    ASSERT_ERR((((uint32_t)sifsainmacclk << 8) & ~((uint32_t)0x00FFFF00)) == 0);
    ASSERT_ERR((((uint32_t)sifsa << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_6_ADDR,  ((uint32_t)sifsainmacclk << 8) | ((uint32_t)sifsa << 0));
}

/**
 * @brief Unpacks TIMINGS_6's fields from current value of the TIMINGS_6 register.
 *
 * Reads the TIMINGS_6 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] sifsainmacclk - Will be populated with the current value of this field from the register.
 * @param[out] sifsa - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_timings_6_unpack(uint16_t *sifsainmacclk, uint8_t *sifsa)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_6_ADDR);

    *sifsainmacclk = (localVal & ((uint32_t)0x00FFFF00)) >> 8;
    *sifsa = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the sifsAInMACClk field in the TIMINGS_6 register.
 *
 * The TIMINGS_6 register will be read and the sifsAInMACClk field's value will be returned.
 *
 * @return The current value of the sifsAInMACClk field in the TIMINGS_6 register.
 */
__INLINE uint16_t nxmac_sifs_a_in_mac_clk_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_6_ADDR);
    return ((localVal & ((uint32_t)0x00FFFF00)) >> 8);
}

/**
 * @brief Sets the sifsAInMACClk field of the TIMINGS_6 register.
 *
 * The TIMINGS_6 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] sifsainmacclk - The value to set the field to.
 */
__INLINE void nxmac_sifs_a_in_mac_clk_setf(uint16_t sifsainmacclk)
{
    ASSERT_ERR((((uint32_t)sifsainmacclk << 8) & ~((uint32_t)0x00FFFF00)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_6_ADDR, (REG_PL_RD(NXMAC_TIMINGS_6_ADDR) & ~((uint32_t)0x00FFFF00)) | ((uint32_t)sifsainmacclk << 8));
}

/**
 * @brief Returns the current value of the sifsA field in the TIMINGS_6 register.
 *
 * The TIMINGS_6 register will be read and the sifsA field's value will be returned.
 *
 * @return The current value of the sifsA field in the TIMINGS_6 register.
 */
__INLINE uint8_t nxmac_sifs_a_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_6_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the sifsA field of the TIMINGS_6 register.
 *
 * The TIMINGS_6 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] sifsa - The value to set the field to.
 */
__INLINE void nxmac_sifs_a_setf(uint8_t sifsa)
{
    ASSERT_ERR((((uint32_t)sifsa << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_6_ADDR, (REG_PL_RD(NXMAC_TIMINGS_6_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)sifsa << 0));
}

/// @}

/**
 * @name TIMINGS_7 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  11:08           rxCCADelay   0x0
 *  07:00                 rifs   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TIMINGS_7 register
#define NXMAC_TIMINGS_7_ADDR   0xC00000FC
/// Offset of the TIMINGS_7 register from the base address
#define NXMAC_TIMINGS_7_OFFSET 0x000000FC
/// Index of the TIMINGS_7 register
#define NXMAC_TIMINGS_7_INDEX  0x0000003F
/// Reset value of the TIMINGS_7 register
#define NXMAC_TIMINGS_7_RESET  0x00000000

/**
 * @brief Returns the current value of the TIMINGS_7 register.
 * The TIMINGS_7 register will be read and its value returned.
 * @return The current value of the TIMINGS_7 register.
 */
__INLINE uint32_t nxmac_timings_7_get(void)
{
    return REG_PL_RD(NXMAC_TIMINGS_7_ADDR);
}

/**
 * @brief Sets the TIMINGS_7 register to a value.
 * The TIMINGS_7 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_timings_7_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TIMINGS_7_ADDR, value);
}

// field definitions
/// RX_CCA_DELAY field mask
#define NXMAC_RX_CCA_DELAY_MASK   ((uint32_t)0x00000F00)
/// RX_CCA_DELAY field LSB position
#define NXMAC_RX_CCA_DELAY_LSB    8
/// RX_CCA_DELAY field width
#define NXMAC_RX_CCA_DELAY_WIDTH  ((uint32_t)0x00000004)
/// RIFS field mask
#define NXMAC_RIFS_MASK           ((uint32_t)0x000000FF)
/// RIFS field LSB position
#define NXMAC_RIFS_LSB            0
/// RIFS field width
#define NXMAC_RIFS_WIDTH          ((uint32_t)0x00000008)

/// RX_CCA_DELAY field reset value
#define NXMAC_RX_CCA_DELAY_RST    0x0
/// RIFS field reset value
#define NXMAC_RIFS_RST            0x0

/**
 * @brief Constructs a value for the TIMINGS_7 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxccadelay - The value to use for the rxCCADelay field.
 * @param[in] rifs - The value to use for the rifs field.
 */
__INLINE void nxmac_timings_7_pack(uint8_t rxccadelay, uint8_t rifs)
{
    ASSERT_ERR((((uint32_t)rxccadelay << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)rifs << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_7_ADDR,  ((uint32_t)rxccadelay << 8) | ((uint32_t)rifs << 0));
}

/**
 * @brief Unpacks TIMINGS_7's fields from current value of the TIMINGS_7 register.
 *
 * Reads the TIMINGS_7 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxccadelay - Will be populated with the current value of this field from the register.
 * @param[out] rifs - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_timings_7_unpack(uint8_t *rxccadelay, uint8_t *rifs)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_7_ADDR);

    *rxccadelay = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *rifs = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the rxCCADelay field in the TIMINGS_7 register.
 *
 * The TIMINGS_7 register will be read and the rxCCADelay field's value will be returned.
 *
 * @return The current value of the rxCCADelay field in the TIMINGS_7 register.
 */
__INLINE uint8_t nxmac_rx_cca_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_7_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the rxCCADelay field of the TIMINGS_7 register.
 *
 * The TIMINGS_7 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxccadelay - The value to set the field to.
 */
__INLINE void nxmac_rx_cca_delay_setf(uint8_t rxccadelay)
{
    ASSERT_ERR((((uint32_t)rxccadelay << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_7_ADDR, (REG_PL_RD(NXMAC_TIMINGS_7_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)rxccadelay << 8));
}

/**
 * @brief Returns the current value of the rifs field in the TIMINGS_7 register.
 *
 * The TIMINGS_7 register will be read and the rifs field's value will be returned.
 *
 * @return The current value of the rifs field in the TIMINGS_7 register.
 */
__INLINE uint8_t nxmac_rifs_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_7_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the rifs field of the TIMINGS_7 register.
 *
 * The TIMINGS_7 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rifs - The value to set the field to.
 */
__INLINE void nxmac_rifs_setf(uint8_t rifs)
{
    ASSERT_ERR((((uint32_t)rifs << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_7_ADDR, (REG_PL_RD(NXMAC_TIMINGS_7_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)rifs << 0));
}

/// @}

/**
 * @name TIMINGS_8 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:24     rxStartDelayMIMO   0x21
 *  23:16    rxStartDelayShort   0x66
 *  15:08     rxStartDelayLong   0xC6
 *  07:00     rxStartDelayOFDM   0x21
 * </pre>
 *
 * @{
 */

/// Address of the TIMINGS_8 register
#define NXMAC_TIMINGS_8_ADDR   0xC0000100
/// Offset of the TIMINGS_8 register from the base address
#define NXMAC_TIMINGS_8_OFFSET 0x00000100
/// Index of the TIMINGS_8 register
#define NXMAC_TIMINGS_8_INDEX  0x00000040
/// Reset value of the TIMINGS_8 register
#define NXMAC_TIMINGS_8_RESET  0x2166C621

/**
 * @brief Returns the current value of the TIMINGS_8 register.
 * The TIMINGS_8 register will be read and its value returned.
 * @return The current value of the TIMINGS_8 register.
 */
__INLINE uint32_t nxmac_timings_8_get(void)
{
    return REG_PL_RD(NXMAC_TIMINGS_8_ADDR);
}

/**
 * @brief Sets the TIMINGS_8 register to a value.
 * The TIMINGS_8 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_timings_8_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TIMINGS_8_ADDR, value);
}

// field definitions
/// RX_START_DELAY_MIMO field mask
#define NXMAC_RX_START_DELAY_MIMO_MASK    ((uint32_t)0xFF000000)
/// RX_START_DELAY_MIMO field LSB position
#define NXMAC_RX_START_DELAY_MIMO_LSB     24
/// RX_START_DELAY_MIMO field width
#define NXMAC_RX_START_DELAY_MIMO_WIDTH   ((uint32_t)0x00000008)
/// RX_START_DELAY_SHORT field mask
#define NXMAC_RX_START_DELAY_SHORT_MASK   ((uint32_t)0x00FF0000)
/// RX_START_DELAY_SHORT field LSB position
#define NXMAC_RX_START_DELAY_SHORT_LSB    16
/// RX_START_DELAY_SHORT field width
#define NXMAC_RX_START_DELAY_SHORT_WIDTH  ((uint32_t)0x00000008)
/// RX_START_DELAY_LONG field mask
#define NXMAC_RX_START_DELAY_LONG_MASK    ((uint32_t)0x0000FF00)
/// RX_START_DELAY_LONG field LSB position
#define NXMAC_RX_START_DELAY_LONG_LSB     8
/// RX_START_DELAY_LONG field width
#define NXMAC_RX_START_DELAY_LONG_WIDTH   ((uint32_t)0x00000008)
/// RX_START_DELAY_OFDM field mask
#define NXMAC_RX_START_DELAY_OFDM_MASK    ((uint32_t)0x000000FF)
/// RX_START_DELAY_OFDM field LSB position
#define NXMAC_RX_START_DELAY_OFDM_LSB     0
/// RX_START_DELAY_OFDM field width
#define NXMAC_RX_START_DELAY_OFDM_WIDTH   ((uint32_t)0x00000008)

/// RX_START_DELAY_MIMO field reset value
#define NXMAC_RX_START_DELAY_MIMO_RST     0x21
/// RX_START_DELAY_SHORT field reset value
#define NXMAC_RX_START_DELAY_SHORT_RST    0x66
/// RX_START_DELAY_LONG field reset value
#define NXMAC_RX_START_DELAY_LONG_RST     0xC6
/// RX_START_DELAY_OFDM field reset value
#define NXMAC_RX_START_DELAY_OFDM_RST     0x21

/**
 * @brief Constructs a value for the TIMINGS_8 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxstartdelaymimo - The value to use for the rxStartDelayMIMO field.
 * @param[in] rxstartdelayshort - The value to use for the rxStartDelayShort field.
 * @param[in] rxstartdelaylong - The value to use for the rxStartDelayLong field.
 * @param[in] rxstartdelayofdm - The value to use for the rxStartDelayOFDM field.
 */
__INLINE void nxmac_timings_8_pack(uint8_t rxstartdelaymimo, uint8_t rxstartdelayshort, uint8_t rxstartdelaylong, uint8_t rxstartdelayofdm)
{
    ASSERT_ERR((((uint32_t)rxstartdelaymimo << 24) & ~((uint32_t)0xFF000000)) == 0);
    ASSERT_ERR((((uint32_t)rxstartdelayshort << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)rxstartdelaylong << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)rxstartdelayofdm << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_8_ADDR,  ((uint32_t)rxstartdelaymimo << 24) | ((uint32_t)rxstartdelayshort << 16) | ((uint32_t)rxstartdelaylong << 8) | ((uint32_t)rxstartdelayofdm << 0));
}

/**
 * @brief Unpacks TIMINGS_8's fields from current value of the TIMINGS_8 register.
 *
 * Reads the TIMINGS_8 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxstartdelaymimo - Will be populated with the current value of this field from the register.
 * @param[out] rxstartdelayshort - Will be populated with the current value of this field from the register.
 * @param[out] rxstartdelaylong - Will be populated with the current value of this field from the register.
 * @param[out] rxstartdelayofdm - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_timings_8_unpack(uint8_t *rxstartdelaymimo, uint8_t *rxstartdelayshort, uint8_t *rxstartdelaylong, uint8_t *rxstartdelayofdm)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_8_ADDR);

    *rxstartdelaymimo = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *rxstartdelayshort = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *rxstartdelaylong = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *rxstartdelayofdm = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the rxStartDelayMIMO field in the TIMINGS_8 register.
 *
 * The TIMINGS_8 register will be read and the rxStartDelayMIMO field's value will be returned.
 *
 * @return The current value of the rxStartDelayMIMO field in the TIMINGS_8 register.
 */
__INLINE uint8_t nxmac_rx_start_delay_mimo_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_8_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Sets the rxStartDelayMIMO field of the TIMINGS_8 register.
 *
 * The TIMINGS_8 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxstartdelaymimo - The value to set the field to.
 */
__INLINE void nxmac_rx_start_delay_mimo_setf(uint8_t rxstartdelaymimo)
{
    ASSERT_ERR((((uint32_t)rxstartdelaymimo << 24) & ~((uint32_t)0xFF000000)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_8_ADDR, (REG_PL_RD(NXMAC_TIMINGS_8_ADDR) & ~((uint32_t)0xFF000000)) | ((uint32_t)rxstartdelaymimo << 24));
}

/**
 * @brief Returns the current value of the rxStartDelayShort field in the TIMINGS_8 register.
 *
 * The TIMINGS_8 register will be read and the rxStartDelayShort field's value will be returned.
 *
 * @return The current value of the rxStartDelayShort field in the TIMINGS_8 register.
 */
__INLINE uint8_t nxmac_rx_start_delay_short_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_8_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the rxStartDelayShort field of the TIMINGS_8 register.
 *
 * The TIMINGS_8 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxstartdelayshort - The value to set the field to.
 */
__INLINE void nxmac_rx_start_delay_short_setf(uint8_t rxstartdelayshort)
{
    ASSERT_ERR((((uint32_t)rxstartdelayshort << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_8_ADDR, (REG_PL_RD(NXMAC_TIMINGS_8_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)rxstartdelayshort << 16));
}

/**
 * @brief Returns the current value of the rxStartDelayLong field in the TIMINGS_8 register.
 *
 * The TIMINGS_8 register will be read and the rxStartDelayLong field's value will be returned.
 *
 * @return The current value of the rxStartDelayLong field in the TIMINGS_8 register.
 */
__INLINE uint8_t nxmac_rx_start_delay_long_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_8_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the rxStartDelayLong field of the TIMINGS_8 register.
 *
 * The TIMINGS_8 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxstartdelaylong - The value to set the field to.
 */
__INLINE void nxmac_rx_start_delay_long_setf(uint8_t rxstartdelaylong)
{
    ASSERT_ERR((((uint32_t)rxstartdelaylong << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_8_ADDR, (REG_PL_RD(NXMAC_TIMINGS_8_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)rxstartdelaylong << 8));
}

/**
 * @brief Returns the current value of the rxStartDelayOFDM field in the TIMINGS_8 register.
 *
 * The TIMINGS_8 register will be read and the rxStartDelayOFDM field's value will be returned.
 *
 * @return The current value of the rxStartDelayOFDM field in the TIMINGS_8 register.
 */
__INLINE uint8_t nxmac_rx_start_delay_ofdm_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_8_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the rxStartDelayOFDM field of the TIMINGS_8 register.
 *
 * The TIMINGS_8 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxstartdelayofdm - The value to set the field to.
 */
__INLINE void nxmac_rx_start_delay_ofdm_setf(uint8_t rxstartdelayofdm)
{
    ASSERT_ERR((((uint32_t)rxstartdelayofdm << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_8_ADDR, (REG_PL_RD(NXMAC_TIMINGS_8_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)rxstartdelayofdm << 0));
}

/// @}

/**
 * @name TIMINGS_9 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:20       rifsTOInMACClk   0x0
 *  19:10         rifsInMACClk   0x0
 *  09:00   txDMAProcDlyInMACClk   0x0
 * </pre>
 *
 * @{
 */

/// Address of the TIMINGS_9 register
#define NXMAC_TIMINGS_9_ADDR   0xC0000104
/// Offset of the TIMINGS_9 register from the base address
#define NXMAC_TIMINGS_9_OFFSET 0x00000104
/// Index of the TIMINGS_9 register
#define NXMAC_TIMINGS_9_INDEX  0x00000041
/// Reset value of the TIMINGS_9 register
#define NXMAC_TIMINGS_9_RESET  0x00000000

/**
 * @brief Returns the current value of the TIMINGS_9 register.
 * The TIMINGS_9 register will be read and its value returned.
 * @return The current value of the TIMINGS_9 register.
 */
__INLINE uint32_t nxmac_timings_9_get(void)
{
    return REG_PL_RD(NXMAC_TIMINGS_9_ADDR);
}

/**
 * @brief Sets the TIMINGS_9 register to a value.
 * The TIMINGS_9 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_timings_9_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TIMINGS_9_ADDR, value);
}

// field definitions
/// RIFS_TO_IN_MAC_CLK field mask
#define NXMAC_RIFS_TO_IN_MAC_CLK_MASK           ((uint32_t)0x3FF00000)
/// RIFS_TO_IN_MAC_CLK field LSB position
#define NXMAC_RIFS_TO_IN_MAC_CLK_LSB            20
/// RIFS_TO_IN_MAC_CLK field width
#define NXMAC_RIFS_TO_IN_MAC_CLK_WIDTH          ((uint32_t)0x0000000A)
/// RIFS_IN_MAC_CLK field mask
#define NXMAC_RIFS_IN_MAC_CLK_MASK              ((uint32_t)0x000FFC00)
/// RIFS_IN_MAC_CLK field LSB position
#define NXMAC_RIFS_IN_MAC_CLK_LSB               10
/// RIFS_IN_MAC_CLK field width
#define NXMAC_RIFS_IN_MAC_CLK_WIDTH             ((uint32_t)0x0000000A)
/// TX_DMA_PROC_DLY_IN_MAC_CLK field mask
#define NXMAC_TX_DMA_PROC_DLY_IN_MAC_CLK_MASK   ((uint32_t)0x000003FF)
/// TX_DMA_PROC_DLY_IN_MAC_CLK field LSB position
#define NXMAC_TX_DMA_PROC_DLY_IN_MAC_CLK_LSB    0
/// TX_DMA_PROC_DLY_IN_MAC_CLK field width
#define NXMAC_TX_DMA_PROC_DLY_IN_MAC_CLK_WIDTH  ((uint32_t)0x0000000A)

/// RIFS_TO_IN_MAC_CLK field reset value
#define NXMAC_RIFS_TO_IN_MAC_CLK_RST            0x0
/// RIFS_IN_MAC_CLK field reset value
#define NXMAC_RIFS_IN_MAC_CLK_RST               0x0
/// TX_DMA_PROC_DLY_IN_MAC_CLK field reset value
#define NXMAC_TX_DMA_PROC_DLY_IN_MAC_CLK_RST    0x0

/**
 * @brief Constructs a value for the TIMINGS_9 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rifstoinmacclk - The value to use for the rifsTOInMACClk field.
 * @param[in] rifsinmacclk - The value to use for the rifsInMACClk field.
 * @param[in] txdmaprocdlyinmacclk - The value to use for the txDMAProcDlyInMACClk field.
 */
__INLINE void nxmac_timings_9_pack(uint16_t rifstoinmacclk, uint16_t rifsinmacclk, uint16_t txdmaprocdlyinmacclk)
{
    ASSERT_ERR((((uint32_t)rifstoinmacclk << 20) & ~((uint32_t)0x3FF00000)) == 0);
    ASSERT_ERR((((uint32_t)rifsinmacclk << 10) & ~((uint32_t)0x000FFC00)) == 0);
    ASSERT_ERR((((uint32_t)txdmaprocdlyinmacclk << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_9_ADDR,  ((uint32_t)rifstoinmacclk << 20) | ((uint32_t)rifsinmacclk << 10) | ((uint32_t)txdmaprocdlyinmacclk << 0));
}

/**
 * @brief Unpacks TIMINGS_9's fields from current value of the TIMINGS_9 register.
 *
 * Reads the TIMINGS_9 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rifstoinmacclk - Will be populated with the current value of this field from the register.
 * @param[out] rifsinmacclk - Will be populated with the current value of this field from the register.
 * @param[out] txdmaprocdlyinmacclk - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_timings_9_unpack(uint16_t *rifstoinmacclk, uint16_t *rifsinmacclk, uint16_t *txdmaprocdlyinmacclk)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_9_ADDR);

    *rifstoinmacclk = (localVal & ((uint32_t)0x3FF00000)) >> 20;
    *rifsinmacclk = (localVal & ((uint32_t)0x000FFC00)) >> 10;
    *txdmaprocdlyinmacclk = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the rifsTOInMACClk field in the TIMINGS_9 register.
 *
 * The TIMINGS_9 register will be read and the rifsTOInMACClk field's value will be returned.
 *
 * @return The current value of the rifsTOInMACClk field in the TIMINGS_9 register.
 */
__INLINE uint16_t nxmac_rifs_to_in_mac_clk_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_9_ADDR);
    return ((localVal & ((uint32_t)0x3FF00000)) >> 20);
}

/**
 * @brief Sets the rifsTOInMACClk field of the TIMINGS_9 register.
 *
 * The TIMINGS_9 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rifstoinmacclk - The value to set the field to.
 */
__INLINE void nxmac_rifs_to_in_mac_clk_setf(uint16_t rifstoinmacclk)
{
    ASSERT_ERR((((uint32_t)rifstoinmacclk << 20) & ~((uint32_t)0x3FF00000)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_9_ADDR, (REG_PL_RD(NXMAC_TIMINGS_9_ADDR) & ~((uint32_t)0x3FF00000)) | ((uint32_t)rifstoinmacclk << 20));
}

/**
 * @brief Returns the current value of the rifsInMACClk field in the TIMINGS_9 register.
 *
 * The TIMINGS_9 register will be read and the rifsInMACClk field's value will be returned.
 *
 * @return The current value of the rifsInMACClk field in the TIMINGS_9 register.
 */
__INLINE uint16_t nxmac_rifs_in_mac_clk_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_9_ADDR);
    return ((localVal & ((uint32_t)0x000FFC00)) >> 10);
}

/**
 * @brief Sets the rifsInMACClk field of the TIMINGS_9 register.
 *
 * The TIMINGS_9 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rifsinmacclk - The value to set the field to.
 */
__INLINE void nxmac_rifs_in_mac_clk_setf(uint16_t rifsinmacclk)
{
    ASSERT_ERR((((uint32_t)rifsinmacclk << 10) & ~((uint32_t)0x000FFC00)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_9_ADDR, (REG_PL_RD(NXMAC_TIMINGS_9_ADDR) & ~((uint32_t)0x000FFC00)) | ((uint32_t)rifsinmacclk << 10));
}

/**
 * @brief Returns the current value of the txDMAProcDlyInMACClk field in the TIMINGS_9 register.
 *
 * The TIMINGS_9 register will be read and the txDMAProcDlyInMACClk field's value will be returned.
 *
 * @return The current value of the txDMAProcDlyInMACClk field in the TIMINGS_9 register.
 */
__INLINE uint16_t nxmac_tx_dma_proc_dly_in_mac_clk_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TIMINGS_9_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the txDMAProcDlyInMACClk field of the TIMINGS_9 register.
 *
 * The TIMINGS_9 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txdmaprocdlyinmacclk - The value to set the field to.
 */
__INLINE void nxmac_tx_dma_proc_dly_in_mac_clk_setf(uint16_t txdmaprocdlyinmacclk)
{
    ASSERT_ERR((((uint32_t)txdmaprocdlyinmacclk << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(NXMAC_TIMINGS_9_ADDR, (REG_PL_RD(NXMAC_TIMINGS_9_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)txdmaprocdlyinmacclk << 0));
}

/// @}

/**
 * @name PROT_TRIG_TIMER register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:08     hccaTriggerTimer   0x0
 *  07:00     edcaTriggerTimer   0x9
 * </pre>
 *
 * @{
 */

/// Address of the PROT_TRIG_TIMER register
#define NXMAC_PROT_TRIG_TIMER_ADDR   0xC000010C
/// Offset of the PROT_TRIG_TIMER register from the base address
#define NXMAC_PROT_TRIG_TIMER_OFFSET 0x0000010C
/// Index of the PROT_TRIG_TIMER register
#define NXMAC_PROT_TRIG_TIMER_INDEX  0x00000043
/// Reset value of the PROT_TRIG_TIMER register
#define NXMAC_PROT_TRIG_TIMER_RESET  0x00000009

/**
 * @brief Returns the current value of the PROT_TRIG_TIMER register.
 * The PROT_TRIG_TIMER register will be read and its value returned.
 * @return The current value of the PROT_TRIG_TIMER register.
 */
__INLINE uint32_t nxmac_prot_trig_timer_get(void)
{
    return REG_PL_RD(NXMAC_PROT_TRIG_TIMER_ADDR);
}

/**
 * @brief Sets the PROT_TRIG_TIMER register to a value.
 * The PROT_TRIG_TIMER register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_prot_trig_timer_set(uint32_t value)
{
    REG_PL_WR(NXMAC_PROT_TRIG_TIMER_ADDR, value);
}

// field definitions
/// HCCA_TRIGGER_TIMER field mask
#define NXMAC_HCCA_TRIGGER_TIMER_MASK   ((uint32_t)0x0000FF00)
/// HCCA_TRIGGER_TIMER field LSB position
#define NXMAC_HCCA_TRIGGER_TIMER_LSB    8
/// HCCA_TRIGGER_TIMER field width
#define NXMAC_HCCA_TRIGGER_TIMER_WIDTH  ((uint32_t)0x00000008)
/// EDCA_TRIGGER_TIMER field mask
#define NXMAC_EDCA_TRIGGER_TIMER_MASK   ((uint32_t)0x000000FF)
/// EDCA_TRIGGER_TIMER field LSB position
#define NXMAC_EDCA_TRIGGER_TIMER_LSB    0
/// EDCA_TRIGGER_TIMER field width
#define NXMAC_EDCA_TRIGGER_TIMER_WIDTH  ((uint32_t)0x00000008)

/// HCCA_TRIGGER_TIMER field reset value
#define NXMAC_HCCA_TRIGGER_TIMER_RST    0x0
/// EDCA_TRIGGER_TIMER field reset value
#define NXMAC_EDCA_TRIGGER_TIMER_RST    0x9

/**
 * @brief Unpacks PROT_TRIG_TIMER's fields from current value of the PROT_TRIG_TIMER register.
 *
 * Reads the PROT_TRIG_TIMER register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] hccatriggertimer - Will be populated with the current value of this field from the register.
 * @param[out] edcatriggertimer - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_prot_trig_timer_unpack(uint8_t *hccatriggertimer, uint8_t *edcatriggertimer)
{
    uint32_t localVal = REG_PL_RD(NXMAC_PROT_TRIG_TIMER_ADDR);

    *hccatriggertimer = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *edcatriggertimer = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the hccaTriggerTimer field in the PROT_TRIG_TIMER register.
 *
 * The PROT_TRIG_TIMER register will be read and the hccaTriggerTimer field's value will be returned.
 *
 * @return The current value of the hccaTriggerTimer field in the PROT_TRIG_TIMER register.
 */
__INLINE uint8_t nxmac_hcca_trigger_timer_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_PROT_TRIG_TIMER_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Returns the current value of the edcaTriggerTimer field in the PROT_TRIG_TIMER register.
 *
 * The PROT_TRIG_TIMER register will be read and the edcaTriggerTimer field's value will be returned.
 *
 * @return The current value of the edcaTriggerTimer field in the PROT_TRIG_TIMER register.
 */
__INLINE uint8_t nxmac_edca_trigger_timer_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_PROT_TRIG_TIMER_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the edcaTriggerTimer field of the PROT_TRIG_TIMER register.
 *
 * The PROT_TRIG_TIMER register will be read, modified to contain the new field value, and written.
 *
 * @param[in] edcatriggertimer - The value to set the field to.
 */
__INLINE void nxmac_edca_trigger_timer_setf(uint8_t edcatriggertimer)
{
    ASSERT_ERR((((uint32_t)edcatriggertimer << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_PROT_TRIG_TIMER_ADDR, (uint32_t)edcatriggertimer << 0);
}

/// @}

/**
 * @name TX_TRIGGER_TIMER register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:08      txPacketTimeout   0xF
 *  07:00    txAbsoluteTimeout   0x9C
 * </pre>
 *
 * @{
 */

/// Address of the TX_TRIGGER_TIMER register
#define NXMAC_TX_TRIGGER_TIMER_ADDR   0xC0000110
/// Offset of the TX_TRIGGER_TIMER register from the base address
#define NXMAC_TX_TRIGGER_TIMER_OFFSET 0x00000110
/// Index of the TX_TRIGGER_TIMER register
#define NXMAC_TX_TRIGGER_TIMER_INDEX  0x00000044
/// Reset value of the TX_TRIGGER_TIMER register
#define NXMAC_TX_TRIGGER_TIMER_RESET  0x00000F9C

/**
 * @brief Returns the current value of the TX_TRIGGER_TIMER register.
 * The TX_TRIGGER_TIMER register will be read and its value returned.
 * @return The current value of the TX_TRIGGER_TIMER register.
 */
__INLINE uint32_t nxmac_tx_trigger_timer_get(void)
{
    return REG_PL_RD(NXMAC_TX_TRIGGER_TIMER_ADDR);
}

/**
 * @brief Sets the TX_TRIGGER_TIMER register to a value.
 * The TX_TRIGGER_TIMER register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_tx_trigger_timer_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TX_TRIGGER_TIMER_ADDR, value);
}

// field definitions
/// TX_PACKET_TIMEOUT field mask
#define NXMAC_TX_PACKET_TIMEOUT_MASK     ((uint32_t)0x0000FF00)
/// TX_PACKET_TIMEOUT field LSB position
#define NXMAC_TX_PACKET_TIMEOUT_LSB      8
/// TX_PACKET_TIMEOUT field width
#define NXMAC_TX_PACKET_TIMEOUT_WIDTH    ((uint32_t)0x00000008)
/// TX_ABSOLUTE_TIMEOUT field mask
#define NXMAC_TX_ABSOLUTE_TIMEOUT_MASK   ((uint32_t)0x000000FF)
/// TX_ABSOLUTE_TIMEOUT field LSB position
#define NXMAC_TX_ABSOLUTE_TIMEOUT_LSB    0
/// TX_ABSOLUTE_TIMEOUT field width
#define NXMAC_TX_ABSOLUTE_TIMEOUT_WIDTH  ((uint32_t)0x00000008)

/// TX_PACKET_TIMEOUT field reset value
#define NXMAC_TX_PACKET_TIMEOUT_RST      0xF
/// TX_ABSOLUTE_TIMEOUT field reset value
#define NXMAC_TX_ABSOLUTE_TIMEOUT_RST    0x9C

/**
 * @brief Constructs a value for the TX_TRIGGER_TIMER register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] txpackettimeout - The value to use for the txPacketTimeout field.
 * @param[in] txabsolutetimeout - The value to use for the txAbsoluteTimeout field.
 */
__INLINE void nxmac_tx_trigger_timer_pack(uint8_t txpackettimeout, uint8_t txabsolutetimeout)
{
    ASSERT_ERR((((uint32_t)txpackettimeout << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)txabsolutetimeout << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_TX_TRIGGER_TIMER_ADDR,  ((uint32_t)txpackettimeout << 8) | ((uint32_t)txabsolutetimeout << 0));
}

/**
 * @brief Unpacks TX_TRIGGER_TIMER's fields from current value of the TX_TRIGGER_TIMER register.
 *
 * Reads the TX_TRIGGER_TIMER register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txpackettimeout - Will be populated with the current value of this field from the register.
 * @param[out] txabsolutetimeout - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_tx_trigger_timer_unpack(uint8_t *txpackettimeout, uint8_t *txabsolutetimeout)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_TRIGGER_TIMER_ADDR);

    *txpackettimeout = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *txabsolutetimeout = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the txPacketTimeout field in the TX_TRIGGER_TIMER register.
 *
 * The TX_TRIGGER_TIMER register will be read and the txPacketTimeout field's value will be returned.
 *
 * @return The current value of the txPacketTimeout field in the TX_TRIGGER_TIMER register.
 */
__INLINE uint8_t nxmac_tx_packet_timeout_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_TRIGGER_TIMER_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the txPacketTimeout field of the TX_TRIGGER_TIMER register.
 *
 * The TX_TRIGGER_TIMER register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txpackettimeout - The value to set the field to.
 */
__INLINE void nxmac_tx_packet_timeout_setf(uint8_t txpackettimeout)
{
    ASSERT_ERR((((uint32_t)txpackettimeout << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(NXMAC_TX_TRIGGER_TIMER_ADDR, (REG_PL_RD(NXMAC_TX_TRIGGER_TIMER_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)txpackettimeout << 8));
}

/**
 * @brief Returns the current value of the txAbsoluteTimeout field in the TX_TRIGGER_TIMER register.
 *
 * The TX_TRIGGER_TIMER register will be read and the txAbsoluteTimeout field's value will be returned.
 *
 * @return The current value of the txAbsoluteTimeout field in the TX_TRIGGER_TIMER register.
 */
__INLINE uint8_t nxmac_tx_absolute_timeout_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_TRIGGER_TIMER_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the txAbsoluteTimeout field of the TX_TRIGGER_TIMER register.
 *
 * The TX_TRIGGER_TIMER register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txabsolutetimeout - The value to set the field to.
 */
__INLINE void nxmac_tx_absolute_timeout_setf(uint8_t txabsolutetimeout)
{
    ASSERT_ERR((((uint32_t)txabsolutetimeout << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_TX_TRIGGER_TIMER_ADDR, (REG_PL_RD(NXMAC_TX_TRIGGER_TIMER_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)txabsolutetimeout << 0));
}

/// @}

/**
 * @name RX_TRIGGER_TIMER register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  23:16   rxPayloadUsedCount   0xF
 *  15:08      rxPacketTimeout   0xF
 *  07:00    rxAbsoluteTimeout   0x9C
 * </pre>
 *
 * @{
 */

/// Address of the RX_TRIGGER_TIMER register
#define NXMAC_RX_TRIGGER_TIMER_ADDR   0xC0000114
/// Offset of the RX_TRIGGER_TIMER register from the base address
#define NXMAC_RX_TRIGGER_TIMER_OFFSET 0x00000114
/// Index of the RX_TRIGGER_TIMER register
#define NXMAC_RX_TRIGGER_TIMER_INDEX  0x00000045
/// Reset value of the RX_TRIGGER_TIMER register
#define NXMAC_RX_TRIGGER_TIMER_RESET  0x000F0F9C

/**
 * @brief Returns the current value of the RX_TRIGGER_TIMER register.
 * The RX_TRIGGER_TIMER register will be read and its value returned.
 * @return The current value of the RX_TRIGGER_TIMER register.
 */
__INLINE uint32_t nxmac_rx_trigger_timer_get(void)
{
    return REG_PL_RD(NXMAC_RX_TRIGGER_TIMER_ADDR);
}

/**
 * @brief Sets the RX_TRIGGER_TIMER register to a value.
 * The RX_TRIGGER_TIMER register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_rx_trigger_timer_set(uint32_t value)
{
    REG_PL_WR(NXMAC_RX_TRIGGER_TIMER_ADDR, value);
}

// field definitions
/// RX_PAYLOAD_USED_COUNT field mask
#define NXMAC_RX_PAYLOAD_USED_COUNT_MASK   ((uint32_t)0x00FF0000)
/// RX_PAYLOAD_USED_COUNT field LSB position
#define NXMAC_RX_PAYLOAD_USED_COUNT_LSB    16
/// RX_PAYLOAD_USED_COUNT field width
#define NXMAC_RX_PAYLOAD_USED_COUNT_WIDTH  ((uint32_t)0x00000008)
/// RX_PACKET_TIMEOUT field mask
#define NXMAC_RX_PACKET_TIMEOUT_MASK       ((uint32_t)0x0000FF00)
/// RX_PACKET_TIMEOUT field LSB position
#define NXMAC_RX_PACKET_TIMEOUT_LSB        8
/// RX_PACKET_TIMEOUT field width
#define NXMAC_RX_PACKET_TIMEOUT_WIDTH      ((uint32_t)0x00000008)
/// RX_ABSOLUTE_TIMEOUT field mask
#define NXMAC_RX_ABSOLUTE_TIMEOUT_MASK     ((uint32_t)0x000000FF)
/// RX_ABSOLUTE_TIMEOUT field LSB position
#define NXMAC_RX_ABSOLUTE_TIMEOUT_LSB      0
/// RX_ABSOLUTE_TIMEOUT field width
#define NXMAC_RX_ABSOLUTE_TIMEOUT_WIDTH    ((uint32_t)0x00000008)

/// RX_PAYLOAD_USED_COUNT field reset value
#define NXMAC_RX_PAYLOAD_USED_COUNT_RST    0xF
/// RX_PACKET_TIMEOUT field reset value
#define NXMAC_RX_PACKET_TIMEOUT_RST        0xF
/// RX_ABSOLUTE_TIMEOUT field reset value
#define NXMAC_RX_ABSOLUTE_TIMEOUT_RST      0x9C

/**
 * @brief Constructs a value for the RX_TRIGGER_TIMER register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxpayloadusedcount - The value to use for the rxPayloadUsedCount field.
 * @param[in] rxpackettimeout - The value to use for the rxPacketTimeout field.
 * @param[in] rxabsolutetimeout - The value to use for the rxAbsoluteTimeout field.
 */
__INLINE void nxmac_rx_trigger_timer_pack(uint8_t rxpayloadusedcount, uint8_t rxpackettimeout, uint8_t rxabsolutetimeout)
{
    ASSERT_ERR((((uint32_t)rxpayloadusedcount << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)rxpackettimeout << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)rxabsolutetimeout << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_RX_TRIGGER_TIMER_ADDR,  ((uint32_t)rxpayloadusedcount << 16) | ((uint32_t)rxpackettimeout << 8) | ((uint32_t)rxabsolutetimeout << 0));
}

/**
 * @brief Unpacks RX_TRIGGER_TIMER's fields from current value of the RX_TRIGGER_TIMER register.
 *
 * Reads the RX_TRIGGER_TIMER register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxpayloadusedcount - Will be populated with the current value of this field from the register.
 * @param[out] rxpackettimeout - Will be populated with the current value of this field from the register.
 * @param[out] rxabsolutetimeout - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_rx_trigger_timer_unpack(uint8_t *rxpayloadusedcount, uint8_t *rxpackettimeout, uint8_t *rxabsolutetimeout)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_TRIGGER_TIMER_ADDR);

    *rxpayloadusedcount = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *rxpackettimeout = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *rxabsolutetimeout = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the rxPayloadUsedCount field in the RX_TRIGGER_TIMER register.
 *
 * The RX_TRIGGER_TIMER register will be read and the rxPayloadUsedCount field's value will be returned.
 *
 * @return The current value of the rxPayloadUsedCount field in the RX_TRIGGER_TIMER register.
 */
__INLINE uint8_t nxmac_rx_payload_used_count_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_TRIGGER_TIMER_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the rxPayloadUsedCount field of the RX_TRIGGER_TIMER register.
 *
 * The RX_TRIGGER_TIMER register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxpayloadusedcount - The value to set the field to.
 */
__INLINE void nxmac_rx_payload_used_count_setf(uint8_t rxpayloadusedcount)
{
    ASSERT_ERR((((uint32_t)rxpayloadusedcount << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(NXMAC_RX_TRIGGER_TIMER_ADDR, (REG_PL_RD(NXMAC_RX_TRIGGER_TIMER_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)rxpayloadusedcount << 16));
}

/**
 * @brief Returns the current value of the rxPacketTimeout field in the RX_TRIGGER_TIMER register.
 *
 * The RX_TRIGGER_TIMER register will be read and the rxPacketTimeout field's value will be returned.
 *
 * @return The current value of the rxPacketTimeout field in the RX_TRIGGER_TIMER register.
 */
__INLINE uint8_t nxmac_rx_packet_timeout_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_TRIGGER_TIMER_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the rxPacketTimeout field of the RX_TRIGGER_TIMER register.
 *
 * The RX_TRIGGER_TIMER register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxpackettimeout - The value to set the field to.
 */
__INLINE void nxmac_rx_packet_timeout_setf(uint8_t rxpackettimeout)
{
    ASSERT_ERR((((uint32_t)rxpackettimeout << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(NXMAC_RX_TRIGGER_TIMER_ADDR, (REG_PL_RD(NXMAC_RX_TRIGGER_TIMER_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)rxpackettimeout << 8));
}

/**
 * @brief Returns the current value of the rxAbsoluteTimeout field in the RX_TRIGGER_TIMER register.
 *
 * The RX_TRIGGER_TIMER register will be read and the rxAbsoluteTimeout field's value will be returned.
 *
 * @return The current value of the rxAbsoluteTimeout field in the RX_TRIGGER_TIMER register.
 */
__INLINE uint8_t nxmac_rx_absolute_timeout_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_RX_TRIGGER_TIMER_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the rxAbsoluteTimeout field of the RX_TRIGGER_TIMER register.
 *
 * The RX_TRIGGER_TIMER register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxabsolutetimeout - The value to set the field to.
 */
__INLINE void nxmac_rx_absolute_timeout_setf(uint8_t rxabsolutetimeout)
{
    ASSERT_ERR((((uint32_t)rxabsolutetimeout << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_RX_TRIGGER_TIMER_ADDR, (REG_PL_RD(NXMAC_RX_TRIGGER_TIMER_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)rxabsolutetimeout << 0));
}

/// @}

/**
 * @name MIB_TABLE_WRITE register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:16             mibValue   0x0
 *     15             mibWrite   0
 *     14     mibIncrementMode   1
 *  09:00        mibTableIndex   0x0
 * </pre>
 *
 * @{
 */

/// Address of the MIB_TABLE_WRITE register
#define NXMAC_MIB_TABLE_WRITE_ADDR   0xC0000118
/// Offset of the MIB_TABLE_WRITE register from the base address
#define NXMAC_MIB_TABLE_WRITE_OFFSET 0x00000118
/// Index of the MIB_TABLE_WRITE register
#define NXMAC_MIB_TABLE_WRITE_INDEX  0x00000046
/// Reset value of the MIB_TABLE_WRITE register
#define NXMAC_MIB_TABLE_WRITE_RESET  0x00004000

/**
 * @brief Returns the current value of the MIB_TABLE_WRITE register.
 * The MIB_TABLE_WRITE register will be read and its value returned.
 * @return The current value of the MIB_TABLE_WRITE register.
 */
__INLINE uint32_t nxmac_mib_table_write_get(void)
{
    return REG_PL_RD(NXMAC_MIB_TABLE_WRITE_ADDR);
}

/**
 * @brief Sets the MIB_TABLE_WRITE register to a value.
 * The MIB_TABLE_WRITE register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_mib_table_write_set(uint32_t value)
{
    REG_PL_WR(NXMAC_MIB_TABLE_WRITE_ADDR, value);
}

// field definitions
/// MIB_VALUE field mask
#define NXMAC_MIB_VALUE_MASK            ((uint32_t)0xFFFF0000)
/// MIB_VALUE field LSB position
#define NXMAC_MIB_VALUE_LSB             16
/// MIB_VALUE field width
#define NXMAC_MIB_VALUE_WIDTH           ((uint32_t)0x00000010)
/// MIB_WRITE field bit
#define NXMAC_MIB_WRITE_BIT             ((uint32_t)0x00008000)
/// MIB_WRITE field position
#define NXMAC_MIB_WRITE_POS             15
/// MIB_INCREMENT_MODE field bit
#define NXMAC_MIB_INCREMENT_MODE_BIT    ((uint32_t)0x00004000)
/// MIB_INCREMENT_MODE field position
#define NXMAC_MIB_INCREMENT_MODE_POS    14
/// MIB_TABLE_INDEX field mask
#define NXMAC_MIB_TABLE_INDEX_MASK      ((uint32_t)0x000003FF)
/// MIB_TABLE_INDEX field LSB position
#define NXMAC_MIB_TABLE_INDEX_LSB       0
/// MIB_TABLE_INDEX field width
#define NXMAC_MIB_TABLE_INDEX_WIDTH     ((uint32_t)0x0000000A)

/// MIB_VALUE field reset value
#define NXMAC_MIB_VALUE_RST             0x0
/// MIB_WRITE field reset value
#define NXMAC_MIB_WRITE_RST             0x0
/// MIB_INCREMENT_MODE field reset value
#define NXMAC_MIB_INCREMENT_MODE_RST    0x1
/// MIB_TABLE_INDEX field reset value
#define NXMAC_MIB_TABLE_INDEX_RST       0x0

/**
 * @brief Constructs a value for the MIB_TABLE_WRITE register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] mibvalue - The value to use for the mibValue field.
 * @param[in] mibwrite - The value to use for the mibWrite field.
 * @param[in] mibincrementmode - The value to use for the mibIncrementMode field.
 * @param[in] mibtableindex - The value to use for the mibTableIndex field.
 */
__INLINE void nxmac_mib_table_write_pack(uint16_t mibvalue, uint8_t mibwrite, uint8_t mibincrementmode, uint16_t mibtableindex)
{
    ASSERT_ERR((((uint32_t)mibvalue << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    ASSERT_ERR((((uint32_t)mibwrite << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)mibincrementmode << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)mibtableindex << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(NXMAC_MIB_TABLE_WRITE_ADDR,  ((uint32_t)mibvalue << 16) | ((uint32_t)mibwrite << 15) | ((uint32_t)mibincrementmode << 14) | ((uint32_t)mibtableindex << 0));
}

/**
 * @brief Unpacks MIB_TABLE_WRITE's fields from current value of the MIB_TABLE_WRITE register.
 *
 * Reads the MIB_TABLE_WRITE register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] mibvalue - Will be populated with the current value of this field from the register.
 * @param[out] mibwrite - Will be populated with the current value of this field from the register.
 * @param[out] mibincrementmode - Will be populated with the current value of this field from the register.
 * @param[out] mibtableindex - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_mib_table_write_unpack(uint16_t *mibvalue, uint8_t *mibwrite, uint8_t *mibincrementmode, uint16_t *mibtableindex)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MIB_TABLE_WRITE_ADDR);

    *mibvalue = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *mibwrite = (localVal & ((uint32_t)0x00008000)) >> 15;
    *mibincrementmode = (localVal & ((uint32_t)0x00004000)) >> 14;
    *mibtableindex = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the mibValue field in the MIB_TABLE_WRITE register.
 *
 * The MIB_TABLE_WRITE register will be read and the mibValue field's value will be returned.
 *
 * @return The current value of the mibValue field in the MIB_TABLE_WRITE register.
 */
__INLINE uint16_t nxmac_mib_value_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MIB_TABLE_WRITE_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Sets the mibValue field of the MIB_TABLE_WRITE register.
 *
 * The MIB_TABLE_WRITE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mibvalue - The value to set the field to.
 */
__INLINE void nxmac_mib_value_setf(uint16_t mibvalue)
{
    ASSERT_ERR((((uint32_t)mibvalue << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    REG_PL_WR(NXMAC_MIB_TABLE_WRITE_ADDR, (REG_PL_RD(NXMAC_MIB_TABLE_WRITE_ADDR) & ~((uint32_t)0xFFFF0000)) | ((uint32_t)mibvalue << 16));
}

/**
 * @brief Returns the current value of the mibWrite field in the MIB_TABLE_WRITE register.
 *
 * The MIB_TABLE_WRITE register will be read and the mibWrite field's value will be returned.
 *
 * @return The current value of the mibWrite field in the MIB_TABLE_WRITE register.
 */
__INLINE uint8_t nxmac_mib_write_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MIB_TABLE_WRITE_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

/**
 * @brief Sets the mibWrite field of the MIB_TABLE_WRITE register.
 *
 * The MIB_TABLE_WRITE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mibwrite - The value to set the field to.
 */
__INLINE void nxmac_mib_write_setf(uint8_t mibwrite)
{
    ASSERT_ERR((((uint32_t)mibwrite << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_PL_WR(NXMAC_MIB_TABLE_WRITE_ADDR, (REG_PL_RD(NXMAC_MIB_TABLE_WRITE_ADDR) & ~((uint32_t)0x00008000)) | ((uint32_t)mibwrite << 15));
}

/**
 * @brief Returns the current value of the mibIncrementMode field in the MIB_TABLE_WRITE register.
 *
 * The MIB_TABLE_WRITE register will be read and the mibIncrementMode field's value will be returned.
 *
 * @return The current value of the mibIncrementMode field in the MIB_TABLE_WRITE register.
 */
__INLINE uint8_t nxmac_mib_increment_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MIB_TABLE_WRITE_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Sets the mibIncrementMode field of the MIB_TABLE_WRITE register.
 *
 * The MIB_TABLE_WRITE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mibincrementmode - The value to set the field to.
 */
__INLINE void nxmac_mib_increment_mode_setf(uint8_t mibincrementmode)
{
    ASSERT_ERR((((uint32_t)mibincrementmode << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(NXMAC_MIB_TABLE_WRITE_ADDR, (REG_PL_RD(NXMAC_MIB_TABLE_WRITE_ADDR) & ~((uint32_t)0x00004000)) | ((uint32_t)mibincrementmode << 14));
}

/**
 * @brief Returns the current value of the mibTableIndex field in the MIB_TABLE_WRITE register.
 *
 * The MIB_TABLE_WRITE register will be read and the mibTableIndex field's value will be returned.
 *
 * @return The current value of the mibTableIndex field in the MIB_TABLE_WRITE register.
 */
__INLINE uint16_t nxmac_mib_table_index_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MIB_TABLE_WRITE_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the mibTableIndex field of the MIB_TABLE_WRITE register.
 *
 * The MIB_TABLE_WRITE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mibtableindex - The value to set the field to.
 */
__INLINE void nxmac_mib_table_index_setf(uint16_t mibtableindex)
{
    ASSERT_ERR((((uint32_t)mibtableindex << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(NXMAC_MIB_TABLE_WRITE_ADDR, (REG_PL_RD(NXMAC_MIB_TABLE_WRITE_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)mibtableindex << 0));
}

/// @}

/**
 * @name MONOTONIC_COUNTER_1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00    monotonicCounter1   0x0
 * </pre>
 *
 * @{
 */

/// Address of the MONOTONIC_COUNTER_1 register
#define NXMAC_MONOTONIC_COUNTER_1_ADDR   0xC000011C
/// Offset of the MONOTONIC_COUNTER_1 register from the base address
#define NXMAC_MONOTONIC_COUNTER_1_OFFSET 0x0000011C
/// Index of the MONOTONIC_COUNTER_1 register
#define NXMAC_MONOTONIC_COUNTER_1_INDEX  0x00000047
/// Reset value of the MONOTONIC_COUNTER_1 register
#define NXMAC_MONOTONIC_COUNTER_1_RESET  0x00000000

/**
 * @brief Returns the current value of the MONOTONIC_COUNTER_1 register.
 * The MONOTONIC_COUNTER_1 register will be read and its value returned.
 * @return The current value of the MONOTONIC_COUNTER_1 register.
 */
__INLINE uint32_t nxmac_monotonic_counter_1_get(void)
{
    return REG_PL_RD(NXMAC_MONOTONIC_COUNTER_1_ADDR);
}

// field definitions
/// MONOTONIC_COUNTER_1 field mask
#define NXMAC_MONOTONIC_COUNTER_1_MASK   ((uint32_t)0xFFFFFFFF)
/// MONOTONIC_COUNTER_1 field LSB position
#define NXMAC_MONOTONIC_COUNTER_1_LSB    0
/// MONOTONIC_COUNTER_1 field width
#define NXMAC_MONOTONIC_COUNTER_1_WIDTH  ((uint32_t)0x00000020)

/// MONOTONIC_COUNTER_1 field reset value
#define NXMAC_MONOTONIC_COUNTER_1_RST    0x0

/**
 * @brief Returns the current value of the monotonicCounter1 field in the MONOTONIC_COUNTER_1 register.
 *
 * The MONOTONIC_COUNTER_1 register will be read and the monotonicCounter1 field's value will be returned.
 *
 * @return The current value of the monotonicCounter1 field in the MONOTONIC_COUNTER_1 register.
 */
__INLINE uint32_t nxmac_monotonic_counter_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MONOTONIC_COUNTER_1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name MONOTONIC_COUNTER_2_LO register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00   monotonicCounterLow2   0x0
 * </pre>
 *
 * @{
 */

/// Address of the MONOTONIC_COUNTER_2_LO register
#define NXMAC_MONOTONIC_COUNTER_2_LO_ADDR   0xC0000120
/// Offset of the MONOTONIC_COUNTER_2_LO register from the base address
#define NXMAC_MONOTONIC_COUNTER_2_LO_OFFSET 0x00000120
/// Index of the MONOTONIC_COUNTER_2_LO register
#define NXMAC_MONOTONIC_COUNTER_2_LO_INDEX  0x00000048
/// Reset value of the MONOTONIC_COUNTER_2_LO register
#define NXMAC_MONOTONIC_COUNTER_2_LO_RESET  0x00000000

/**
 * @brief Returns the current value of the MONOTONIC_COUNTER_2_LO register.
 * The MONOTONIC_COUNTER_2_LO register will be read and its value returned.
 * @return The current value of the MONOTONIC_COUNTER_2_LO register.
 */
__INLINE uint32_t nxmac_monotonic_counter_2_lo_get(void)
{
    return REG_PL_RD(NXMAC_MONOTONIC_COUNTER_2_LO_ADDR);
}

/**
 * @brief Sets the MONOTONIC_COUNTER_2_LO register to a value.
 * The MONOTONIC_COUNTER_2_LO register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_monotonic_counter_2_lo_set(uint32_t value)
{
    REG_PL_WR(NXMAC_MONOTONIC_COUNTER_2_LO_ADDR, value);
}

// field definitions
/// MONOTONIC_COUNTER_LOW_2 field mask
#define NXMAC_MONOTONIC_COUNTER_LOW_2_MASK   ((uint32_t)0xFFFFFFFF)
/// MONOTONIC_COUNTER_LOW_2 field LSB position
#define NXMAC_MONOTONIC_COUNTER_LOW_2_LSB    0
/// MONOTONIC_COUNTER_LOW_2 field width
#define NXMAC_MONOTONIC_COUNTER_LOW_2_WIDTH  ((uint32_t)0x00000020)

/// MONOTONIC_COUNTER_LOW_2 field reset value
#define NXMAC_MONOTONIC_COUNTER_LOW_2_RST    0x0

/**
 * @brief Returns the current value of the monotonicCounterLow2 field in the MONOTONIC_COUNTER_2_LO register.
 *
 * The MONOTONIC_COUNTER_2_LO register will be read and the monotonicCounterLow2 field's value will be returned.
 *
 * @return The current value of the monotonicCounterLow2 field in the MONOTONIC_COUNTER_2_LO register.
 */
__INLINE uint32_t nxmac_monotonic_counter_low_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MONOTONIC_COUNTER_2_LO_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the monotonicCounterLow2 field of the MONOTONIC_COUNTER_2_LO register.
 *
 * The MONOTONIC_COUNTER_2_LO register will be read, modified to contain the new field value, and written.
 *
 * @param[in] monotoniccounterlow2 - The value to set the field to.
 */
__INLINE void nxmac_monotonic_counter_low_2_setf(uint32_t monotoniccounterlow2)
{
    ASSERT_ERR((((uint32_t)monotoniccounterlow2 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_MONOTONIC_COUNTER_2_LO_ADDR, (uint32_t)monotoniccounterlow2 << 0);
}

/// @}

/**
 * @name MONOTONIC_COUNTER_2_HI register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31   monotonicCounter2SWUpdate   0
 *  15:00   monotonicCounterHigh2   0x0
 * </pre>
 *
 * @{
 */

/// Address of the MONOTONIC_COUNTER_2_HI register
#define NXMAC_MONOTONIC_COUNTER_2_HI_ADDR   0xC0000124
/// Offset of the MONOTONIC_COUNTER_2_HI register from the base address
#define NXMAC_MONOTONIC_COUNTER_2_HI_OFFSET 0x00000124
/// Index of the MONOTONIC_COUNTER_2_HI register
#define NXMAC_MONOTONIC_COUNTER_2_HI_INDEX  0x00000049
/// Reset value of the MONOTONIC_COUNTER_2_HI register
#define NXMAC_MONOTONIC_COUNTER_2_HI_RESET  0x00000000

/**
 * @brief Returns the current value of the MONOTONIC_COUNTER_2_HI register.
 * The MONOTONIC_COUNTER_2_HI register will be read and its value returned.
 * @return The current value of the MONOTONIC_COUNTER_2_HI register.
 */
__INLINE uint32_t nxmac_monotonic_counter_2_hi_get(void)
{
    return REG_PL_RD(NXMAC_MONOTONIC_COUNTER_2_HI_ADDR);
}

/**
 * @brief Sets the MONOTONIC_COUNTER_2_HI register to a value.
 * The MONOTONIC_COUNTER_2_HI register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_monotonic_counter_2_hi_set(uint32_t value)
{
    REG_PL_WR(NXMAC_MONOTONIC_COUNTER_2_HI_ADDR, value);
}

// field definitions
/// MONOTONIC_COUNTER_2SW_UPDATE field bit
#define NXMAC_MONOTONIC_COUNTER_2SW_UPDATE_BIT    ((uint32_t)0x80000000)
/// MONOTONIC_COUNTER_2SW_UPDATE field position
#define NXMAC_MONOTONIC_COUNTER_2SW_UPDATE_POS    31
/// MONOTONIC_COUNTER_HIGH_2 field mask
#define NXMAC_MONOTONIC_COUNTER_HIGH_2_MASK       ((uint32_t)0x0000FFFF)
/// MONOTONIC_COUNTER_HIGH_2 field LSB position
#define NXMAC_MONOTONIC_COUNTER_HIGH_2_LSB        0
/// MONOTONIC_COUNTER_HIGH_2 field width
#define NXMAC_MONOTONIC_COUNTER_HIGH_2_WIDTH      ((uint32_t)0x00000010)

/// MONOTONIC_COUNTER_2SW_UPDATE field reset value
#define NXMAC_MONOTONIC_COUNTER_2SW_UPDATE_RST    0x0
/// MONOTONIC_COUNTER_HIGH_2 field reset value
#define NXMAC_MONOTONIC_COUNTER_HIGH_2_RST        0x0

/**
 * @brief Constructs a value for the MONOTONIC_COUNTER_2_HI register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] monotoniccounter2swupdate - The value to use for the monotonicCounter2SWUpdate field.
 * @param[in] monotoniccounterhigh2 - The value to use for the monotonicCounterHigh2 field.
 */
__INLINE void nxmac_monotonic_counter_2_hi_pack(uint8_t monotoniccounter2swupdate, uint16_t monotoniccounterhigh2)
{
    ASSERT_ERR((((uint32_t)monotoniccounter2swupdate << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)monotoniccounterhigh2 << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_MONOTONIC_COUNTER_2_HI_ADDR,  ((uint32_t)monotoniccounter2swupdate << 31) | ((uint32_t)monotoniccounterhigh2 << 0));
}

/**
 * @brief Unpacks MONOTONIC_COUNTER_2_HI's fields from current value of the MONOTONIC_COUNTER_2_HI register.
 *
 * Reads the MONOTONIC_COUNTER_2_HI register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] monotoniccounter2swupdate - Will be populated with the current value of this field from the register.
 * @param[out] monotoniccounterhigh2 - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_monotonic_counter_2_hi_unpack(uint8_t *monotoniccounter2swupdate, uint16_t *monotoniccounterhigh2)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MONOTONIC_COUNTER_2_HI_ADDR);

    *monotoniccounter2swupdate = (localVal & ((uint32_t)0x80000000)) >> 31;
    *monotoniccounterhigh2 = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the monotonicCounter2SWUpdate field in the MONOTONIC_COUNTER_2_HI register.
 *
 * The MONOTONIC_COUNTER_2_HI register will be read and the monotonicCounter2SWUpdate field's value will be returned.
 *
 * @return The current value of the monotonicCounter2SWUpdate field in the MONOTONIC_COUNTER_2_HI register.
 */
__INLINE uint8_t nxmac_monotonic_counter_2sw_update_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MONOTONIC_COUNTER_2_HI_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the monotonicCounter2SWUpdate field of the MONOTONIC_COUNTER_2_HI register.
 *
 * The MONOTONIC_COUNTER_2_HI register will be read, modified to contain the new field value, and written.
 *
 * @param[in] monotoniccounter2swupdate - The value to set the field to.
 */
__INLINE void nxmac_monotonic_counter_2sw_update_setf(uint8_t monotoniccounter2swupdate)
{
    ASSERT_ERR((((uint32_t)monotoniccounter2swupdate << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(NXMAC_MONOTONIC_COUNTER_2_HI_ADDR, (REG_PL_RD(NXMAC_MONOTONIC_COUNTER_2_HI_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)monotoniccounter2swupdate << 31));
}

/**
 * @brief Returns the current value of the monotonicCounterHigh2 field in the MONOTONIC_COUNTER_2_HI register.
 *
 * The MONOTONIC_COUNTER_2_HI register will be read and the monotonicCounterHigh2 field's value will be returned.
 *
 * @return The current value of the monotonicCounterHigh2 field in the MONOTONIC_COUNTER_2_HI register.
 */
__INLINE uint16_t nxmac_monotonic_counter_high_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MONOTONIC_COUNTER_2_HI_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the monotonicCounterHigh2 field of the MONOTONIC_COUNTER_2_HI register.
 *
 * The MONOTONIC_COUNTER_2_HI register will be read, modified to contain the new field value, and written.
 *
 * @param[in] monotoniccounterhigh2 - The value to set the field to.
 */
__INLINE void nxmac_monotonic_counter_high_2_setf(uint16_t monotoniccounterhigh2)
{
    ASSERT_ERR((((uint32_t)monotoniccounterhigh2 << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_MONOTONIC_COUNTER_2_HI_ADDR, (REG_PL_RD(NXMAC_MONOTONIC_COUNTER_2_HI_ADDR) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)monotoniccounterhigh2 << 0));
}

/// @}

/**
 * @name ABS_TIMER register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00        absTimerValue   0x0
 * </pre>
 *
 * @{
 */

/// Address of the ABS_TIMER register
#define NXMAC_ABS_TIMER_ADDR   0xC0000128
/// Offset of the ABS_TIMER register from the base address
#define NXMAC_ABS_TIMER_OFFSET 0x00000128
/// Index of the ABS_TIMER register
#define NXMAC_ABS_TIMER_INDEX  0x0000004A
/// Reset value of the ABS_TIMER register
#define NXMAC_ABS_TIMER_RESET  0x00000000
/// Number of elements of the ABS_TIMER register array
#define NXMAC_ABS_TIMER_COUNT  10

/**
 * @brief Returns the current value of the ABS_TIMER register.
 * The ABS_TIMER register will be read and its value returned.
 * @param[in] reg_idx Index of the register
 * @return The current value of the ABS_TIMER register.
 */
__INLINE uint32_t nxmac_abs_timer_get(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 9);
    return REG_PL_RD(NXMAC_ABS_TIMER_ADDR + reg_idx * 4);
}

/**
 * @brief Sets the ABS_TIMER register to a value.
 * The ABS_TIMER register will be written.
 * @param[in] reg_idx Index of the register
 * @param value - The value to write.
 */
__INLINE void nxmac_abs_timer_set(int reg_idx, uint32_t value)
{
    ASSERT_ERR(reg_idx <= 9);
    REG_PL_WR(NXMAC_ABS_TIMER_ADDR + reg_idx * 4, value);
}

// field definitions
/// ABS_TIMER_VALUE field mask
#define NXMAC_ABS_TIMER_VALUE_MASK   ((uint32_t)0xFFFFFFFF)
/// ABS_TIMER_VALUE field LSB position
#define NXMAC_ABS_TIMER_VALUE_LSB    0
/// ABS_TIMER_VALUE field width
#define NXMAC_ABS_TIMER_VALUE_WIDTH  ((uint32_t)0x00000020)

/// ABS_TIMER_VALUE field reset value
#define NXMAC_ABS_TIMER_VALUE_RST    0x0

/**
 * @brief Returns the current value of the absTimerValue field in the ABS_TIMER register.
 *
 * The ABS_TIMER register will be read and the absTimerValue field's value will be returned.
 *
 * @param[in] reg_idx Index of the register
 * @return The current value of the absTimerValue field in the ABS_TIMER register.
 */
__INLINE uint32_t nxmac_abs_timer_value_getf(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 9);

    uint32_t localVal = REG_PL_RD(NXMAC_ABS_TIMER_ADDR + reg_idx * 4);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);

    return (localVal >> 0);
}

/**
 * @brief Sets the absTimerValue field of the ABS_TIMER register.
 *
 * The ABS_TIMER register will be read, modified to contain the new field value, and written.
 *
 * @param[in] reg_idx Index of the register
 * @param[in] abstimervalue - The value to set the field to.
 */
__INLINE void nxmac_abs_timer_value_setf(int reg_idx, uint32_t abstimervalue)
{
    ASSERT_ERR(reg_idx <= 9);
    ASSERT_ERR((((uint32_t)abstimervalue << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_ABS_TIMER_ADDR + reg_idx * 4, (uint32_t)abstimervalue << 0);
}

/// @}

/**
 * @name MAX_RX_LENGTH register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  19:00     maxAllowedLength   0xFFFF
 * </pre>
 *
 * @{
 */

/// Address of the MAX_RX_LENGTH register
#define NXMAC_MAX_RX_LENGTH_ADDR   0xC0000150
/// Offset of the MAX_RX_LENGTH register from the base address
#define NXMAC_MAX_RX_LENGTH_OFFSET 0x00000150
/// Index of the MAX_RX_LENGTH register
#define NXMAC_MAX_RX_LENGTH_INDEX  0x00000054
/// Reset value of the MAX_RX_LENGTH register
#define NXMAC_MAX_RX_LENGTH_RESET  0x0000FFFF

/**
 * @brief Returns the current value of the MAX_RX_LENGTH register.
 * The MAX_RX_LENGTH register will be read and its value returned.
 * @return The current value of the MAX_RX_LENGTH register.
 */
__INLINE uint32_t nxmac_max_rx_length_get(void)
{
    return REG_PL_RD(NXMAC_MAX_RX_LENGTH_ADDR);
}

/**
 * @brief Sets the MAX_RX_LENGTH register to a value.
 * The MAX_RX_LENGTH register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_max_rx_length_set(uint32_t value)
{
    REG_PL_WR(NXMAC_MAX_RX_LENGTH_ADDR, value);
}

// field definitions
/// MAX_ALLOWED_LENGTH field mask
#define NXMAC_MAX_ALLOWED_LENGTH_MASK   ((uint32_t)0x000FFFFF)
/// MAX_ALLOWED_LENGTH field LSB position
#define NXMAC_MAX_ALLOWED_LENGTH_LSB    0
/// MAX_ALLOWED_LENGTH field width
#define NXMAC_MAX_ALLOWED_LENGTH_WIDTH  ((uint32_t)0x00000014)

/// MAX_ALLOWED_LENGTH field reset value
#define NXMAC_MAX_ALLOWED_LENGTH_RST    0xFFFF

/**
 * @brief Returns the current value of the maxAllowedLength field in the MAX_RX_LENGTH register.
 *
 * The MAX_RX_LENGTH register will be read and the maxAllowedLength field's value will be returned.
 *
 * @return The current value of the maxAllowedLength field in the MAX_RX_LENGTH register.
 */
__INLINE uint32_t nxmac_max_allowed_length_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_MAX_RX_LENGTH_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000FFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the maxAllowedLength field of the MAX_RX_LENGTH register.
 *
 * The MAX_RX_LENGTH register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maxallowedlength - The value to set the field to.
 */
__INLINE void nxmac_max_allowed_length_setf(uint32_t maxallowedlength)
{
    ASSERT_ERR((((uint32_t)maxallowedlength << 0) & ~((uint32_t)0x000FFFFF)) == 0);
    REG_PL_WR(NXMAC_MAX_RX_LENGTH_ADDR, (uint32_t)maxallowedlength << 0);
}

/// @}

/**
 * @name EDCA_AC_0 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  27:12           txOpLimit0   0x0
 *  11:08               cwMax0   0xA
 *  07:04               cwMin0   0x4
 *  03:00               aifsn0   0x7
 * </pre>
 *
 * @{
 */

/// Address of the EDCA_AC_0 register
#define NXMAC_EDCA_AC_0_ADDR   0xC0000200
/// Offset of the EDCA_AC_0 register from the base address
#define NXMAC_EDCA_AC_0_OFFSET 0x00000200
/// Index of the EDCA_AC_0 register
#define NXMAC_EDCA_AC_0_INDEX  0x00000080
/// Reset value of the EDCA_AC_0 register
#define NXMAC_EDCA_AC_0_RESET  0x00000A47

/**
 * @brief Returns the current value of the EDCA_AC_0 register.
 * The EDCA_AC_0 register will be read and its value returned.
 * @return The current value of the EDCA_AC_0 register.
 */
__INLINE uint32_t nxmac_edca_ac_0_get(void)
{
    return REG_PL_RD(NXMAC_EDCA_AC_0_ADDR);
}

/**
 * @brief Sets the EDCA_AC_0 register to a value.
 * The EDCA_AC_0 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_edca_ac_0_set(uint32_t value)
{
    REG_PL_WR(NXMAC_EDCA_AC_0_ADDR, value);
}

// field definitions
/// TX_OP_LIMIT_0 field mask
#define NXMAC_TX_OP_LIMIT_0_MASK   ((uint32_t)0x0FFFF000)
/// TX_OP_LIMIT_0 field LSB position
#define NXMAC_TX_OP_LIMIT_0_LSB    12
/// TX_OP_LIMIT_0 field width
#define NXMAC_TX_OP_LIMIT_0_WIDTH  ((uint32_t)0x00000010)
/// CW_MAX_0 field mask
#define NXMAC_CW_MAX_0_MASK        ((uint32_t)0x00000F00)
/// CW_MAX_0 field LSB position
#define NXMAC_CW_MAX_0_LSB         8
/// CW_MAX_0 field width
#define NXMAC_CW_MAX_0_WIDTH       ((uint32_t)0x00000004)
/// CW_MIN_0 field mask
#define NXMAC_CW_MIN_0_MASK        ((uint32_t)0x000000F0)
/// CW_MIN_0 field LSB position
#define NXMAC_CW_MIN_0_LSB         4
/// CW_MIN_0 field width
#define NXMAC_CW_MIN_0_WIDTH       ((uint32_t)0x00000004)
/// AIFSN_0 field mask
#define NXMAC_AIFSN_0_MASK         ((uint32_t)0x0000000F)
/// AIFSN_0 field LSB position
#define NXMAC_AIFSN_0_LSB          0
/// AIFSN_0 field width
#define NXMAC_AIFSN_0_WIDTH        ((uint32_t)0x00000004)

/// TX_OP_LIMIT_0 field reset value
#define NXMAC_TX_OP_LIMIT_0_RST    0x0
/// CW_MAX_0 field reset value
#define NXMAC_CW_MAX_0_RST         0xA
/// CW_MIN_0 field reset value
#define NXMAC_CW_MIN_0_RST         0x4
/// AIFSN_0 field reset value
#define NXMAC_AIFSN_0_RST          0x7

/**
 * @brief Constructs a value for the EDCA_AC_0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] txoplimit0 - The value to use for the txOpLimit0 field.
 * @param[in] cwmax0 - The value to use for the cwMax0 field.
 * @param[in] cwmin0 - The value to use for the cwMin0 field.
 * @param[in] aifsn0 - The value to use for the aifsn0 field.
 */
__INLINE void nxmac_edca_ac_0_pack(uint16_t txoplimit0, uint8_t cwmax0, uint8_t cwmin0, uint8_t aifsn0)
{
    ASSERT_ERR((((uint32_t)txoplimit0 << 12) & ~((uint32_t)0x0FFFF000)) == 0);
    ASSERT_ERR((((uint32_t)cwmax0 << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)cwmin0 << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)aifsn0 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_0_ADDR,  ((uint32_t)txoplimit0 << 12) | ((uint32_t)cwmax0 << 8) | ((uint32_t)cwmin0 << 4) | ((uint32_t)aifsn0 << 0));
}

/**
 * @brief Unpacks EDCA_AC_0's fields from current value of the EDCA_AC_0 register.
 *
 * Reads the EDCA_AC_0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txoplimit0 - Will be populated with the current value of this field from the register.
 * @param[out] cwmax0 - Will be populated with the current value of this field from the register.
 * @param[out] cwmin0 - Will be populated with the current value of this field from the register.
 * @param[out] aifsn0 - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_edca_ac_0_unpack(uint16_t *txoplimit0, uint8_t *cwmax0, uint8_t *cwmin0, uint8_t *aifsn0)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_0_ADDR);

    *txoplimit0 = (localVal & ((uint32_t)0x0FFFF000)) >> 12;
    *cwmax0 = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *cwmin0 = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *aifsn0 = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the txOpLimit0 field in the EDCA_AC_0 register.
 *
 * The EDCA_AC_0 register will be read and the txOpLimit0 field's value will be returned.
 *
 * @return The current value of the txOpLimit0 field in the EDCA_AC_0 register.
 */
__INLINE uint16_t nxmac_tx_op_limit_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_0_ADDR);
    return ((localVal & ((uint32_t)0x0FFFF000)) >> 12);
}

/**
 * @brief Sets the txOpLimit0 field of the EDCA_AC_0 register.
 *
 * The EDCA_AC_0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txoplimit0 - The value to set the field to.
 */
__INLINE void nxmac_tx_op_limit_0_setf(uint16_t txoplimit0)
{
    ASSERT_ERR((((uint32_t)txoplimit0 << 12) & ~((uint32_t)0x0FFFF000)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_0_ADDR, (REG_PL_RD(NXMAC_EDCA_AC_0_ADDR) & ~((uint32_t)0x0FFFF000)) | ((uint32_t)txoplimit0 << 12));
}

/**
 * @brief Returns the current value of the cwMax0 field in the EDCA_AC_0 register.
 *
 * The EDCA_AC_0 register will be read and the cwMax0 field's value will be returned.
 *
 * @return The current value of the cwMax0 field in the EDCA_AC_0 register.
 */
__INLINE uint8_t nxmac_cw_max_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_0_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the cwMax0 field of the EDCA_AC_0 register.
 *
 * The EDCA_AC_0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cwmax0 - The value to set the field to.
 */
__INLINE void nxmac_cw_max_0_setf(uint8_t cwmax0)
{
    ASSERT_ERR((((uint32_t)cwmax0 << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_0_ADDR, (REG_PL_RD(NXMAC_EDCA_AC_0_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)cwmax0 << 8));
}

/**
 * @brief Returns the current value of the cwMin0 field in the EDCA_AC_0 register.
 *
 * The EDCA_AC_0 register will be read and the cwMin0 field's value will be returned.
 *
 * @return The current value of the cwMin0 field in the EDCA_AC_0 register.
 */
__INLINE uint8_t nxmac_cw_min_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_0_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the cwMin0 field of the EDCA_AC_0 register.
 *
 * The EDCA_AC_0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cwmin0 - The value to set the field to.
 */
__INLINE void nxmac_cw_min_0_setf(uint8_t cwmin0)
{
    ASSERT_ERR((((uint32_t)cwmin0 << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_0_ADDR, (REG_PL_RD(NXMAC_EDCA_AC_0_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)cwmin0 << 4));
}

/**
 * @brief Returns the current value of the aifsn0 field in the EDCA_AC_0 register.
 *
 * The EDCA_AC_0 register will be read and the aifsn0 field's value will be returned.
 *
 * @return The current value of the aifsn0 field in the EDCA_AC_0 register.
 */
__INLINE uint8_t nxmac_aifsn_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_0_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the aifsn0 field of the EDCA_AC_0 register.
 *
 * The EDCA_AC_0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aifsn0 - The value to set the field to.
 */
__INLINE void nxmac_aifsn_0_setf(uint8_t aifsn0)
{
    ASSERT_ERR((((uint32_t)aifsn0 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_0_ADDR, (REG_PL_RD(NXMAC_EDCA_AC_0_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)aifsn0 << 0));
}

/// @}

/**
 * @name EDCA_AC_1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  27:12           txOpLimit1   0x0
 *  11:08               cwMax1   0xA
 *  07:04               cwMin1   0x4
 *  03:00               aifsn1   0x3
 * </pre>
 *
 * @{
 */

/// Address of the EDCA_AC_1 register
#define NXMAC_EDCA_AC_1_ADDR   0xC0000204
/// Offset of the EDCA_AC_1 register from the base address
#define NXMAC_EDCA_AC_1_OFFSET 0x00000204
/// Index of the EDCA_AC_1 register
#define NXMAC_EDCA_AC_1_INDEX  0x00000081
/// Reset value of the EDCA_AC_1 register
#define NXMAC_EDCA_AC_1_RESET  0x00000A43

/**
 * @brief Returns the current value of the EDCA_AC_1 register.
 * The EDCA_AC_1 register will be read and its value returned.
 * @return The current value of the EDCA_AC_1 register.
 */
__INLINE uint32_t nxmac_edca_ac_1_get(void)
{
    return REG_PL_RD(NXMAC_EDCA_AC_1_ADDR);
}

/**
 * @brief Sets the EDCA_AC_1 register to a value.
 * The EDCA_AC_1 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_edca_ac_1_set(uint32_t value)
{
    REG_PL_WR(NXMAC_EDCA_AC_1_ADDR, value);
}

// field definitions
/// TX_OP_LIMIT_1 field mask
#define NXMAC_TX_OP_LIMIT_1_MASK   ((uint32_t)0x0FFFF000)
/// TX_OP_LIMIT_1 field LSB position
#define NXMAC_TX_OP_LIMIT_1_LSB    12
/// TX_OP_LIMIT_1 field width
#define NXMAC_TX_OP_LIMIT_1_WIDTH  ((uint32_t)0x00000010)
/// CW_MAX_1 field mask
#define NXMAC_CW_MAX_1_MASK        ((uint32_t)0x00000F00)
/// CW_MAX_1 field LSB position
#define NXMAC_CW_MAX_1_LSB         8
/// CW_MAX_1 field width
#define NXMAC_CW_MAX_1_WIDTH       ((uint32_t)0x00000004)
/// CW_MIN_1 field mask
#define NXMAC_CW_MIN_1_MASK        ((uint32_t)0x000000F0)
/// CW_MIN_1 field LSB position
#define NXMAC_CW_MIN_1_LSB         4
/// CW_MIN_1 field width
#define NXMAC_CW_MIN_1_WIDTH       ((uint32_t)0x00000004)
/// AIFSN_1 field mask
#define NXMAC_AIFSN_1_MASK         ((uint32_t)0x0000000F)
/// AIFSN_1 field LSB position
#define NXMAC_AIFSN_1_LSB          0
/// AIFSN_1 field width
#define NXMAC_AIFSN_1_WIDTH        ((uint32_t)0x00000004)

/// TX_OP_LIMIT_1 field reset value
#define NXMAC_TX_OP_LIMIT_1_RST    0x0
/// CW_MAX_1 field reset value
#define NXMAC_CW_MAX_1_RST         0xA
/// CW_MIN_1 field reset value
#define NXMAC_CW_MIN_1_RST         0x4
/// AIFSN_1 field reset value
#define NXMAC_AIFSN_1_RST          0x3

/**
 * @brief Constructs a value for the EDCA_AC_1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] txoplimit1 - The value to use for the txOpLimit1 field.
 * @param[in] cwmax1 - The value to use for the cwMax1 field.
 * @param[in] cwmin1 - The value to use for the cwMin1 field.
 * @param[in] aifsn1 - The value to use for the aifsn1 field.
 */
__INLINE void nxmac_edca_ac_1_pack(uint16_t txoplimit1, uint8_t cwmax1, uint8_t cwmin1, uint8_t aifsn1)
{
    ASSERT_ERR((((uint32_t)txoplimit1 << 12) & ~((uint32_t)0x0FFFF000)) == 0);
    ASSERT_ERR((((uint32_t)cwmax1 << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)cwmin1 << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)aifsn1 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_1_ADDR,  ((uint32_t)txoplimit1 << 12) | ((uint32_t)cwmax1 << 8) | ((uint32_t)cwmin1 << 4) | ((uint32_t)aifsn1 << 0));
}

/**
 * @brief Unpacks EDCA_AC_1's fields from current value of the EDCA_AC_1 register.
 *
 * Reads the EDCA_AC_1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txoplimit1 - Will be populated with the current value of this field from the register.
 * @param[out] cwmax1 - Will be populated with the current value of this field from the register.
 * @param[out] cwmin1 - Will be populated with the current value of this field from the register.
 * @param[out] aifsn1 - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_edca_ac_1_unpack(uint16_t *txoplimit1, uint8_t *cwmax1, uint8_t *cwmin1, uint8_t *aifsn1)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_1_ADDR);

    *txoplimit1 = (localVal & ((uint32_t)0x0FFFF000)) >> 12;
    *cwmax1 = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *cwmin1 = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *aifsn1 = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the txOpLimit1 field in the EDCA_AC_1 register.
 *
 * The EDCA_AC_1 register will be read and the txOpLimit1 field's value will be returned.
 *
 * @return The current value of the txOpLimit1 field in the EDCA_AC_1 register.
 */
__INLINE uint16_t nxmac_tx_op_limit_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_1_ADDR);
    return ((localVal & ((uint32_t)0x0FFFF000)) >> 12);
}

/**
 * @brief Sets the txOpLimit1 field of the EDCA_AC_1 register.
 *
 * The EDCA_AC_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txoplimit1 - The value to set the field to.
 */
__INLINE void nxmac_tx_op_limit_1_setf(uint16_t txoplimit1)
{
    ASSERT_ERR((((uint32_t)txoplimit1 << 12) & ~((uint32_t)0x0FFFF000)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_1_ADDR, (REG_PL_RD(NXMAC_EDCA_AC_1_ADDR) & ~((uint32_t)0x0FFFF000)) | ((uint32_t)txoplimit1 << 12));
}

/**
 * @brief Returns the current value of the cwMax1 field in the EDCA_AC_1 register.
 *
 * The EDCA_AC_1 register will be read and the cwMax1 field's value will be returned.
 *
 * @return The current value of the cwMax1 field in the EDCA_AC_1 register.
 */
__INLINE uint8_t nxmac_cw_max_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_1_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the cwMax1 field of the EDCA_AC_1 register.
 *
 * The EDCA_AC_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cwmax1 - The value to set the field to.
 */
__INLINE void nxmac_cw_max_1_setf(uint8_t cwmax1)
{
    ASSERT_ERR((((uint32_t)cwmax1 << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_1_ADDR, (REG_PL_RD(NXMAC_EDCA_AC_1_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)cwmax1 << 8));
}

/**
 * @brief Returns the current value of the cwMin1 field in the EDCA_AC_1 register.
 *
 * The EDCA_AC_1 register will be read and the cwMin1 field's value will be returned.
 *
 * @return The current value of the cwMin1 field in the EDCA_AC_1 register.
 */
__INLINE uint8_t nxmac_cw_min_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_1_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the cwMin1 field of the EDCA_AC_1 register.
 *
 * The EDCA_AC_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cwmin1 - The value to set the field to.
 */
__INLINE void nxmac_cw_min_1_setf(uint8_t cwmin1)
{
    ASSERT_ERR((((uint32_t)cwmin1 << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_1_ADDR, (REG_PL_RD(NXMAC_EDCA_AC_1_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)cwmin1 << 4));
}

/**
 * @brief Returns the current value of the aifsn1 field in the EDCA_AC_1 register.
 *
 * The EDCA_AC_1 register will be read and the aifsn1 field's value will be returned.
 *
 * @return The current value of the aifsn1 field in the EDCA_AC_1 register.
 */
__INLINE uint8_t nxmac_aifsn_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_1_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the aifsn1 field of the EDCA_AC_1 register.
 *
 * The EDCA_AC_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aifsn1 - The value to set the field to.
 */
__INLINE void nxmac_aifsn_1_setf(uint8_t aifsn1)
{
    ASSERT_ERR((((uint32_t)aifsn1 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_1_ADDR, (REG_PL_RD(NXMAC_EDCA_AC_1_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)aifsn1 << 0));
}

/// @}

/**
 * @name EDCA_AC_2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  27:12           txOpLimit2   0x5E
 *  11:08               cwMax2   0x4
 *  07:04               cwMin2   0x3
 *  03:00               aifsn2   0x2
 * </pre>
 *
 * @{
 */

/// Address of the EDCA_AC_2 register
#define NXMAC_EDCA_AC_2_ADDR   0xC0000208
/// Offset of the EDCA_AC_2 register from the base address
#define NXMAC_EDCA_AC_2_OFFSET 0x00000208
/// Index of the EDCA_AC_2 register
#define NXMAC_EDCA_AC_2_INDEX  0x00000082
/// Reset value of the EDCA_AC_2 register
#define NXMAC_EDCA_AC_2_RESET  0x0005E432

/**
 * @brief Returns the current value of the EDCA_AC_2 register.
 * The EDCA_AC_2 register will be read and its value returned.
 * @return The current value of the EDCA_AC_2 register.
 */
__INLINE uint32_t nxmac_edca_ac_2_get(void)
{
    return REG_PL_RD(NXMAC_EDCA_AC_2_ADDR);
}

/**
 * @brief Sets the EDCA_AC_2 register to a value.
 * The EDCA_AC_2 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_edca_ac_2_set(uint32_t value)
{
    REG_PL_WR(NXMAC_EDCA_AC_2_ADDR, value);
}

// field definitions
/// TX_OP_LIMIT_2 field mask
#define NXMAC_TX_OP_LIMIT_2_MASK   ((uint32_t)0x0FFFF000)
/// TX_OP_LIMIT_2 field LSB position
#define NXMAC_TX_OP_LIMIT_2_LSB    12
/// TX_OP_LIMIT_2 field width
#define NXMAC_TX_OP_LIMIT_2_WIDTH  ((uint32_t)0x00000010)
/// CW_MAX_2 field mask
#define NXMAC_CW_MAX_2_MASK        ((uint32_t)0x00000F00)
/// CW_MAX_2 field LSB position
#define NXMAC_CW_MAX_2_LSB         8
/// CW_MAX_2 field width
#define NXMAC_CW_MAX_2_WIDTH       ((uint32_t)0x00000004)
/// CW_MIN_2 field mask
#define NXMAC_CW_MIN_2_MASK        ((uint32_t)0x000000F0)
/// CW_MIN_2 field LSB position
#define NXMAC_CW_MIN_2_LSB         4
/// CW_MIN_2 field width
#define NXMAC_CW_MIN_2_WIDTH       ((uint32_t)0x00000004)
/// AIFSN_2 field mask
#define NXMAC_AIFSN_2_MASK         ((uint32_t)0x0000000F)
/// AIFSN_2 field LSB position
#define NXMAC_AIFSN_2_LSB          0
/// AIFSN_2 field width
#define NXMAC_AIFSN_2_WIDTH        ((uint32_t)0x00000004)

/// TX_OP_LIMIT_2 field reset value
#define NXMAC_TX_OP_LIMIT_2_RST    0x5E
/// CW_MAX_2 field reset value
#define NXMAC_CW_MAX_2_RST         0x4
/// CW_MIN_2 field reset value
#define NXMAC_CW_MIN_2_RST         0x3
/// AIFSN_2 field reset value
#define NXMAC_AIFSN_2_RST          0x2

/**
 * @brief Constructs a value for the EDCA_AC_2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] txoplimit2 - The value to use for the txOpLimit2 field.
 * @param[in] cwmax2 - The value to use for the cwMax2 field.
 * @param[in] cwmin2 - The value to use for the cwMin2 field.
 * @param[in] aifsn2 - The value to use for the aifsn2 field.
 */
__INLINE void nxmac_edca_ac_2_pack(uint16_t txoplimit2, uint8_t cwmax2, uint8_t cwmin2, uint8_t aifsn2)
{
    ASSERT_ERR((((uint32_t)txoplimit2 << 12) & ~((uint32_t)0x0FFFF000)) == 0);
    ASSERT_ERR((((uint32_t)cwmax2 << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)cwmin2 << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)aifsn2 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_2_ADDR,  ((uint32_t)txoplimit2 << 12) | ((uint32_t)cwmax2 << 8) | ((uint32_t)cwmin2 << 4) | ((uint32_t)aifsn2 << 0));
}

/**
 * @brief Unpacks EDCA_AC_2's fields from current value of the EDCA_AC_2 register.
 *
 * Reads the EDCA_AC_2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txoplimit2 - Will be populated with the current value of this field from the register.
 * @param[out] cwmax2 - Will be populated with the current value of this field from the register.
 * @param[out] cwmin2 - Will be populated with the current value of this field from the register.
 * @param[out] aifsn2 - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_edca_ac_2_unpack(uint16_t *txoplimit2, uint8_t *cwmax2, uint8_t *cwmin2, uint8_t *aifsn2)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_2_ADDR);

    *txoplimit2 = (localVal & ((uint32_t)0x0FFFF000)) >> 12;
    *cwmax2 = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *cwmin2 = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *aifsn2 = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the txOpLimit2 field in the EDCA_AC_2 register.
 *
 * The EDCA_AC_2 register will be read and the txOpLimit2 field's value will be returned.
 *
 * @return The current value of the txOpLimit2 field in the EDCA_AC_2 register.
 */
__INLINE uint16_t nxmac_tx_op_limit_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_2_ADDR);
    return ((localVal & ((uint32_t)0x0FFFF000)) >> 12);
}

/**
 * @brief Sets the txOpLimit2 field of the EDCA_AC_2 register.
 *
 * The EDCA_AC_2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txoplimit2 - The value to set the field to.
 */
__INLINE void nxmac_tx_op_limit_2_setf(uint16_t txoplimit2)
{
    ASSERT_ERR((((uint32_t)txoplimit2 << 12) & ~((uint32_t)0x0FFFF000)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_2_ADDR, (REG_PL_RD(NXMAC_EDCA_AC_2_ADDR) & ~((uint32_t)0x0FFFF000)) | ((uint32_t)txoplimit2 << 12));
}

/**
 * @brief Returns the current value of the cwMax2 field in the EDCA_AC_2 register.
 *
 * The EDCA_AC_2 register will be read and the cwMax2 field's value will be returned.
 *
 * @return The current value of the cwMax2 field in the EDCA_AC_2 register.
 */
__INLINE uint8_t nxmac_cw_max_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_2_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the cwMax2 field of the EDCA_AC_2 register.
 *
 * The EDCA_AC_2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cwmax2 - The value to set the field to.
 */
__INLINE void nxmac_cw_max_2_setf(uint8_t cwmax2)
{
    ASSERT_ERR((((uint32_t)cwmax2 << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_2_ADDR, (REG_PL_RD(NXMAC_EDCA_AC_2_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)cwmax2 << 8));
}

/**
 * @brief Returns the current value of the cwMin2 field in the EDCA_AC_2 register.
 *
 * The EDCA_AC_2 register will be read and the cwMin2 field's value will be returned.
 *
 * @return The current value of the cwMin2 field in the EDCA_AC_2 register.
 */
__INLINE uint8_t nxmac_cw_min_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_2_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the cwMin2 field of the EDCA_AC_2 register.
 *
 * The EDCA_AC_2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cwmin2 - The value to set the field to.
 */
__INLINE void nxmac_cw_min_2_setf(uint8_t cwmin2)
{
    ASSERT_ERR((((uint32_t)cwmin2 << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_2_ADDR, (REG_PL_RD(NXMAC_EDCA_AC_2_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)cwmin2 << 4));
}

/**
 * @brief Returns the current value of the aifsn2 field in the EDCA_AC_2 register.
 *
 * The EDCA_AC_2 register will be read and the aifsn2 field's value will be returned.
 *
 * @return The current value of the aifsn2 field in the EDCA_AC_2 register.
 */
__INLINE uint8_t nxmac_aifsn_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_2_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the aifsn2 field of the EDCA_AC_2 register.
 *
 * The EDCA_AC_2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aifsn2 - The value to set the field to.
 */
__INLINE void nxmac_aifsn_2_setf(uint8_t aifsn2)
{
    ASSERT_ERR((((uint32_t)aifsn2 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_2_ADDR, (REG_PL_RD(NXMAC_EDCA_AC_2_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)aifsn2 << 0));
}

/// @}

/**
 * @name EDCA_AC_3 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  27:12           txOpLimit3   0x2F
 *  11:08               cwMax3   0x3
 *  07:04               cwMin3   0x2
 *  03:00               aifsn3   0x2
 * </pre>
 *
 * @{
 */

/// Address of the EDCA_AC_3 register
#define NXMAC_EDCA_AC_3_ADDR   0xC000020C
/// Offset of the EDCA_AC_3 register from the base address
#define NXMAC_EDCA_AC_3_OFFSET 0x0000020C
/// Index of the EDCA_AC_3 register
#define NXMAC_EDCA_AC_3_INDEX  0x00000083
/// Reset value of the EDCA_AC_3 register
#define NXMAC_EDCA_AC_3_RESET  0x0002F322

/**
 * @brief Returns the current value of the EDCA_AC_3 register.
 * The EDCA_AC_3 register will be read and its value returned.
 * @return The current value of the EDCA_AC_3 register.
 */
__INLINE uint32_t nxmac_edca_ac_3_get(void)
{
    return REG_PL_RD(NXMAC_EDCA_AC_3_ADDR);
}

/**
 * @brief Sets the EDCA_AC_3 register to a value.
 * The EDCA_AC_3 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_edca_ac_3_set(uint32_t value)
{
    REG_PL_WR(NXMAC_EDCA_AC_3_ADDR, value);
}

// field definitions
/// TX_OP_LIMIT_3 field mask
#define NXMAC_TX_OP_LIMIT_3_MASK   ((uint32_t)0x0FFFF000)
/// TX_OP_LIMIT_3 field LSB position
#define NXMAC_TX_OP_LIMIT_3_LSB    12
/// TX_OP_LIMIT_3 field width
#define NXMAC_TX_OP_LIMIT_3_WIDTH  ((uint32_t)0x00000010)
/// CW_MAX_3 field mask
#define NXMAC_CW_MAX_3_MASK        ((uint32_t)0x00000F00)
/// CW_MAX_3 field LSB position
#define NXMAC_CW_MAX_3_LSB         8
/// CW_MAX_3 field width
#define NXMAC_CW_MAX_3_WIDTH       ((uint32_t)0x00000004)
/// CW_MIN_3 field mask
#define NXMAC_CW_MIN_3_MASK        ((uint32_t)0x000000F0)
/// CW_MIN_3 field LSB position
#define NXMAC_CW_MIN_3_LSB         4
/// CW_MIN_3 field width
#define NXMAC_CW_MIN_3_WIDTH       ((uint32_t)0x00000004)
/// AIFSN_3 field mask
#define NXMAC_AIFSN_3_MASK         ((uint32_t)0x0000000F)
/// AIFSN_3 field LSB position
#define NXMAC_AIFSN_3_LSB          0
/// AIFSN_3 field width
#define NXMAC_AIFSN_3_WIDTH        ((uint32_t)0x00000004)

/// TX_OP_LIMIT_3 field reset value
#define NXMAC_TX_OP_LIMIT_3_RST    0x2F
/// CW_MAX_3 field reset value
#define NXMAC_CW_MAX_3_RST         0x3
/// CW_MIN_3 field reset value
#define NXMAC_CW_MIN_3_RST         0x2
/// AIFSN_3 field reset value
#define NXMAC_AIFSN_3_RST          0x2

/**
 * @brief Constructs a value for the EDCA_AC_3 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] txoplimit3 - The value to use for the txOpLimit3 field.
 * @param[in] cwmax3 - The value to use for the cwMax3 field.
 * @param[in] cwmin3 - The value to use for the cwMin3 field.
 * @param[in] aifsn3 - The value to use for the aifsn3 field.
 */
__INLINE void nxmac_edca_ac_3_pack(uint16_t txoplimit3, uint8_t cwmax3, uint8_t cwmin3, uint8_t aifsn3)
{
    ASSERT_ERR((((uint32_t)txoplimit3 << 12) & ~((uint32_t)0x0FFFF000)) == 0);
    ASSERT_ERR((((uint32_t)cwmax3 << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)cwmin3 << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)aifsn3 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_3_ADDR,  ((uint32_t)txoplimit3 << 12) | ((uint32_t)cwmax3 << 8) | ((uint32_t)cwmin3 << 4) | ((uint32_t)aifsn3 << 0));
}

/**
 * @brief Unpacks EDCA_AC_3's fields from current value of the EDCA_AC_3 register.
 *
 * Reads the EDCA_AC_3 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txoplimit3 - Will be populated with the current value of this field from the register.
 * @param[out] cwmax3 - Will be populated with the current value of this field from the register.
 * @param[out] cwmin3 - Will be populated with the current value of this field from the register.
 * @param[out] aifsn3 - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_edca_ac_3_unpack(uint16_t *txoplimit3, uint8_t *cwmax3, uint8_t *cwmin3, uint8_t *aifsn3)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_3_ADDR);

    *txoplimit3 = (localVal & ((uint32_t)0x0FFFF000)) >> 12;
    *cwmax3 = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *cwmin3 = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *aifsn3 = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the txOpLimit3 field in the EDCA_AC_3 register.
 *
 * The EDCA_AC_3 register will be read and the txOpLimit3 field's value will be returned.
 *
 * @return The current value of the txOpLimit3 field in the EDCA_AC_3 register.
 */
__INLINE uint16_t nxmac_tx_op_limit_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_3_ADDR);
    return ((localVal & ((uint32_t)0x0FFFF000)) >> 12);
}

/**
 * @brief Sets the txOpLimit3 field of the EDCA_AC_3 register.
 *
 * The EDCA_AC_3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txoplimit3 - The value to set the field to.
 */
__INLINE void nxmac_tx_op_limit_3_setf(uint16_t txoplimit3)
{
    ASSERT_ERR((((uint32_t)txoplimit3 << 12) & ~((uint32_t)0x0FFFF000)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_3_ADDR, (REG_PL_RD(NXMAC_EDCA_AC_3_ADDR) & ~((uint32_t)0x0FFFF000)) | ((uint32_t)txoplimit3 << 12));
}

/**
 * @brief Returns the current value of the cwMax3 field in the EDCA_AC_3 register.
 *
 * The EDCA_AC_3 register will be read and the cwMax3 field's value will be returned.
 *
 * @return The current value of the cwMax3 field in the EDCA_AC_3 register.
 */
__INLINE uint8_t nxmac_cw_max_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_3_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the cwMax3 field of the EDCA_AC_3 register.
 *
 * The EDCA_AC_3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cwmax3 - The value to set the field to.
 */
__INLINE void nxmac_cw_max_3_setf(uint8_t cwmax3)
{
    ASSERT_ERR((((uint32_t)cwmax3 << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_3_ADDR, (REG_PL_RD(NXMAC_EDCA_AC_3_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)cwmax3 << 8));
}

/**
 * @brief Returns the current value of the cwMin3 field in the EDCA_AC_3 register.
 *
 * The EDCA_AC_3 register will be read and the cwMin3 field's value will be returned.
 *
 * @return The current value of the cwMin3 field in the EDCA_AC_3 register.
 */
__INLINE uint8_t nxmac_cw_min_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_3_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the cwMin3 field of the EDCA_AC_3 register.
 *
 * The EDCA_AC_3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cwmin3 - The value to set the field to.
 */
__INLINE void nxmac_cw_min_3_setf(uint8_t cwmin3)
{
    ASSERT_ERR((((uint32_t)cwmin3 << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_3_ADDR, (REG_PL_RD(NXMAC_EDCA_AC_3_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)cwmin3 << 4));
}

/**
 * @brief Returns the current value of the aifsn3 field in the EDCA_AC_3 register.
 *
 * The EDCA_AC_3 register will be read and the aifsn3 field's value will be returned.
 *
 * @return The current value of the aifsn3 field in the EDCA_AC_3 register.
 */
__INLINE uint8_t nxmac_aifsn_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_AC_3_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the aifsn3 field of the EDCA_AC_3 register.
 *
 * The EDCA_AC_3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] aifsn3 - The value to set the field to.
 */
__INLINE void nxmac_aifsn_3_setf(uint8_t aifsn3)
{
    ASSERT_ERR((((uint32_t)aifsn3 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(NXMAC_EDCA_AC_3_ADDR, (REG_PL_RD(NXMAC_EDCA_AC_3_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)aifsn3 << 0));
}

/// @}

/**
 * @name EDCA_CCA_BUSY register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00           ccaBusyDur   0x0
 * </pre>
 *
 * @{
 */

/// Address of the EDCA_CCA_BUSY register
#define NXMAC_EDCA_CCA_BUSY_ADDR   0xC0000220
/// Offset of the EDCA_CCA_BUSY register from the base address
#define NXMAC_EDCA_CCA_BUSY_OFFSET 0x00000220
/// Index of the EDCA_CCA_BUSY register
#define NXMAC_EDCA_CCA_BUSY_INDEX  0x00000088
/// Reset value of the EDCA_CCA_BUSY register
#define NXMAC_EDCA_CCA_BUSY_RESET  0x00000000

/**
 * @brief Returns the current value of the EDCA_CCA_BUSY register.
 * The EDCA_CCA_BUSY register will be read and its value returned.
 * @return The current value of the EDCA_CCA_BUSY register.
 */
__INLINE uint32_t nxmac_edca_cca_busy_get(void)
{
    return REG_PL_RD(NXMAC_EDCA_CCA_BUSY_ADDR);
}

/**
 * @brief Sets the EDCA_CCA_BUSY register to a value.
 * The EDCA_CCA_BUSY register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_edca_cca_busy_set(uint32_t value)
{
    REG_PL_WR(NXMAC_EDCA_CCA_BUSY_ADDR, value);
}

// field definitions
/// CCA_BUSY_DUR field mask
#define NXMAC_CCA_BUSY_DUR_MASK   ((uint32_t)0xFFFFFFFF)
/// CCA_BUSY_DUR field LSB position
#define NXMAC_CCA_BUSY_DUR_LSB    0
/// CCA_BUSY_DUR field width
#define NXMAC_CCA_BUSY_DUR_WIDTH  ((uint32_t)0x00000020)

/// CCA_BUSY_DUR field reset value
#define NXMAC_CCA_BUSY_DUR_RST    0x0

/**
 * @brief Returns the current value of the ccaBusyDur field in the EDCA_CCA_BUSY register.
 *
 * The EDCA_CCA_BUSY register will be read and the ccaBusyDur field's value will be returned.
 *
 * @return The current value of the ccaBusyDur field in the EDCA_CCA_BUSY register.
 */
__INLINE uint32_t nxmac_cca_busy_dur_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_CCA_BUSY_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the ccaBusyDur field of the EDCA_CCA_BUSY register.
 *
 * The EDCA_CCA_BUSY register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ccabusydur - The value to set the field to.
 */
__INLINE void nxmac_cca_busy_dur_setf(uint32_t ccabusydur)
{
    ASSERT_ERR((((uint32_t)ccabusydur << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_EDCA_CCA_BUSY_ADDR, (uint32_t)ccabusydur << 0);
}

/// @}

/**
 * @name EDCA_CNTRL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     05         keepTXOPOpen   0
 *     04    remTXOPInDurField   0
 *     01            sendCFEnd   0
 *     00         sendCFEndNow   0
 * </pre>
 *
 * @{
 */

/// Address of the EDCA_CNTRL register
#define NXMAC_EDCA_CNTRL_ADDR   0xC0000224
/// Offset of the EDCA_CNTRL register from the base address
#define NXMAC_EDCA_CNTRL_OFFSET 0x00000224
/// Index of the EDCA_CNTRL register
#define NXMAC_EDCA_CNTRL_INDEX  0x00000089
/// Reset value of the EDCA_CNTRL register
#define NXMAC_EDCA_CNTRL_RESET  0x00000000

/**
 * @brief Returns the current value of the EDCA_CNTRL register.
 * The EDCA_CNTRL register will be read and its value returned.
 * @return The current value of the EDCA_CNTRL register.
 */
__INLINE uint32_t nxmac_edca_cntrl_get(void)
{
    return REG_PL_RD(NXMAC_EDCA_CNTRL_ADDR);
}

/**
 * @brief Sets the EDCA_CNTRL register to a value.
 * The EDCA_CNTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_edca_cntrl_set(uint32_t value)
{
    REG_PL_WR(NXMAC_EDCA_CNTRL_ADDR, value);
}

// field definitions
/// KEEP_TXOP_OPEN field bit
#define NXMAC_KEEP_TXOP_OPEN_BIT           ((uint32_t)0x00000020)
/// KEEP_TXOP_OPEN field position
#define NXMAC_KEEP_TXOP_OPEN_POS           5
/// REM_TXOP_IN_DUR_FIELD field bit
#define NXMAC_REM_TXOP_IN_DUR_FIELD_BIT    ((uint32_t)0x00000010)
/// REM_TXOP_IN_DUR_FIELD field position
#define NXMAC_REM_TXOP_IN_DUR_FIELD_POS    4
/// SEND_CF_END field bit
#define NXMAC_SEND_CF_END_BIT              ((uint32_t)0x00000002)
/// SEND_CF_END field position
#define NXMAC_SEND_CF_END_POS              1
/// SEND_CF_END_NOW field bit
#define NXMAC_SEND_CF_END_NOW_BIT          ((uint32_t)0x00000001)
/// SEND_CF_END_NOW field position
#define NXMAC_SEND_CF_END_NOW_POS          0

/// KEEP_TXOP_OPEN field reset value
#define NXMAC_KEEP_TXOP_OPEN_RST           0x0
/// REM_TXOP_IN_DUR_FIELD field reset value
#define NXMAC_REM_TXOP_IN_DUR_FIELD_RST    0x0
/// SEND_CF_END field reset value
#define NXMAC_SEND_CF_END_RST              0x0
/// SEND_CF_END_NOW field reset value
#define NXMAC_SEND_CF_END_NOW_RST          0x0

/**
 * @brief Constructs a value for the EDCA_CNTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] keeptxopopen - The value to use for the keepTXOPOpen field.
 * @param[in] remtxopindurfield - The value to use for the remTXOPInDurField field.
 * @param[in] sendcfend - The value to use for the sendCFEnd field.
 * @param[in] sendcfendnow - The value to use for the sendCFEndNow field.
 */
__INLINE void nxmac_edca_cntrl_pack(uint8_t keeptxopopen, uint8_t remtxopindurfield, uint8_t sendcfend, uint8_t sendcfendnow)
{
    ASSERT_ERR((((uint32_t)keeptxopopen << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)remtxopindurfield << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)sendcfend << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)sendcfendnow << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_EDCA_CNTRL_ADDR,  ((uint32_t)keeptxopopen << 5) | ((uint32_t)remtxopindurfield << 4) | ((uint32_t)sendcfend << 1) | ((uint32_t)sendcfendnow << 0));
}

/**
 * @brief Unpacks EDCA_CNTRL's fields from current value of the EDCA_CNTRL register.
 *
 * Reads the EDCA_CNTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] keeptxopopen - Will be populated with the current value of this field from the register.
 * @param[out] remtxopindurfield - Will be populated with the current value of this field from the register.
 * @param[out] sendcfend - Will be populated with the current value of this field from the register.
 * @param[out] sendcfendnow - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_edca_cntrl_unpack(uint8_t *keeptxopopen, uint8_t *remtxopindurfield, uint8_t *sendcfend, uint8_t *sendcfendnow)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_CNTRL_ADDR);

    *keeptxopopen = (localVal & ((uint32_t)0x00000020)) >> 5;
    *remtxopindurfield = (localVal & ((uint32_t)0x00000010)) >> 4;
    *sendcfend = (localVal & ((uint32_t)0x00000002)) >> 1;
    *sendcfendnow = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the keepTXOPOpen field in the EDCA_CNTRL register.
 *
 * The EDCA_CNTRL register will be read and the keepTXOPOpen field's value will be returned.
 *
 * @return The current value of the keepTXOPOpen field in the EDCA_CNTRL register.
 */
__INLINE uint8_t nxmac_keep_txop_open_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the keepTXOPOpen field of the EDCA_CNTRL register.
 *
 * The EDCA_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] keeptxopopen - The value to set the field to.
 */
__INLINE void nxmac_keep_txop_open_setf(uint8_t keeptxopopen)
{
    ASSERT_ERR((((uint32_t)keeptxopopen << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_EDCA_CNTRL_ADDR, (REG_PL_RD(NXMAC_EDCA_CNTRL_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)keeptxopopen << 5));
}

/**
 * @brief Returns the current value of the remTXOPInDurField field in the EDCA_CNTRL register.
 *
 * The EDCA_CNTRL register will be read and the remTXOPInDurField field's value will be returned.
 *
 * @return The current value of the remTXOPInDurField field in the EDCA_CNTRL register.
 */
__INLINE uint8_t nxmac_rem_txop_in_dur_field_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the remTXOPInDurField field of the EDCA_CNTRL register.
 *
 * The EDCA_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] remtxopindurfield - The value to set the field to.
 */
__INLINE void nxmac_rem_txop_in_dur_field_setf(uint8_t remtxopindurfield)
{
    ASSERT_ERR((((uint32_t)remtxopindurfield << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(NXMAC_EDCA_CNTRL_ADDR, (REG_PL_RD(NXMAC_EDCA_CNTRL_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)remtxopindurfield << 4));
}

/**
 * @brief Returns the current value of the sendCFEnd field in the EDCA_CNTRL register.
 *
 * The EDCA_CNTRL register will be read and the sendCFEnd field's value will be returned.
 *
 * @return The current value of the sendCFEnd field in the EDCA_CNTRL register.
 */
__INLINE uint8_t nxmac_send_cf_end_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the sendCFEnd field of the EDCA_CNTRL register.
 *
 * The EDCA_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] sendcfend - The value to set the field to.
 */
__INLINE void nxmac_send_cf_end_setf(uint8_t sendcfend)
{
    ASSERT_ERR((((uint32_t)sendcfend << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_EDCA_CNTRL_ADDR, (REG_PL_RD(NXMAC_EDCA_CNTRL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)sendcfend << 1));
}

/**
 * @brief Returns the current value of the sendCFEndNow field in the EDCA_CNTRL register.
 *
 * The EDCA_CNTRL register will be read and the sendCFEndNow field's value will be returned.
 *
 * @return The current value of the sendCFEndNow field in the EDCA_CNTRL register.
 */
__INLINE uint8_t nxmac_send_cf_end_now_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_EDCA_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the sendCFEndNow field of the EDCA_CNTRL register.
 *
 * The EDCA_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] sendcfendnow - The value to set the field to.
 */
__INLINE void nxmac_send_cf_end_now_setf(uint8_t sendcfendnow)
{
    ASSERT_ERR((((uint32_t)sendcfendnow << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_EDCA_CNTRL_ADDR, (REG_PL_RD(NXMAC_EDCA_CNTRL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)sendcfendnow << 0));
}

/// @}

/**
 * @name QUIET_ELEMENT_1A register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:16       quietDuration1   0x0
 *  15:08         quietPeriod1   0x0
 *  07:00          quietCount1   0x0
 * </pre>
 *
 * @{
 */

/// Address of the QUIET_ELEMENT_1A register
#define NXMAC_QUIET_ELEMENT_1A_ADDR   0xC0000280
/// Offset of the QUIET_ELEMENT_1A register from the base address
#define NXMAC_QUIET_ELEMENT_1A_OFFSET 0x00000280
/// Index of the QUIET_ELEMENT_1A register
#define NXMAC_QUIET_ELEMENT_1A_INDEX  0x000000A0
/// Reset value of the QUIET_ELEMENT_1A register
#define NXMAC_QUIET_ELEMENT_1A_RESET  0x00000000

/**
 * @brief Returns the current value of the QUIET_ELEMENT_1A register.
 * The QUIET_ELEMENT_1A register will be read and its value returned.
 * @return The current value of the QUIET_ELEMENT_1A register.
 */
__INLINE uint32_t nxmac_quiet_element_1a_get(void)
{
    return REG_PL_RD(NXMAC_QUIET_ELEMENT_1A_ADDR);
}

/**
 * @brief Sets the QUIET_ELEMENT_1A register to a value.
 * The QUIET_ELEMENT_1A register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_quiet_element_1a_set(uint32_t value)
{
    REG_PL_WR(NXMAC_QUIET_ELEMENT_1A_ADDR, value);
}

// field definitions
/// QUIET_DURATION_1 field mask
#define NXMAC_QUIET_DURATION_1_MASK   ((uint32_t)0xFFFF0000)
/// QUIET_DURATION_1 field LSB position
#define NXMAC_QUIET_DURATION_1_LSB    16
/// QUIET_DURATION_1 field width
#define NXMAC_QUIET_DURATION_1_WIDTH  ((uint32_t)0x00000010)
/// QUIET_PERIOD_1 field mask
#define NXMAC_QUIET_PERIOD_1_MASK     ((uint32_t)0x0000FF00)
/// QUIET_PERIOD_1 field LSB position
#define NXMAC_QUIET_PERIOD_1_LSB      8
/// QUIET_PERIOD_1 field width
#define NXMAC_QUIET_PERIOD_1_WIDTH    ((uint32_t)0x00000008)
/// QUIET_COUNT_1 field mask
#define NXMAC_QUIET_COUNT_1_MASK      ((uint32_t)0x000000FF)
/// QUIET_COUNT_1 field LSB position
#define NXMAC_QUIET_COUNT_1_LSB       0
/// QUIET_COUNT_1 field width
#define NXMAC_QUIET_COUNT_1_WIDTH     ((uint32_t)0x00000008)

/// QUIET_DURATION_1 field reset value
#define NXMAC_QUIET_DURATION_1_RST    0x0
/// QUIET_PERIOD_1 field reset value
#define NXMAC_QUIET_PERIOD_1_RST      0x0
/// QUIET_COUNT_1 field reset value
#define NXMAC_QUIET_COUNT_1_RST       0x0

/**
 * @brief Constructs a value for the QUIET_ELEMENT_1A register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] quietduration1 - The value to use for the quietDuration1 field.
 * @param[in] quietperiod1 - The value to use for the quietPeriod1 field.
 * @param[in] quietcount1 - The value to use for the quietCount1 field.
 */
__INLINE void nxmac_quiet_element_1a_pack(uint16_t quietduration1, uint8_t quietperiod1, uint8_t quietcount1)
{
    ASSERT_ERR((((uint32_t)quietduration1 << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    ASSERT_ERR((((uint32_t)quietperiod1 << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)quietcount1 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_QUIET_ELEMENT_1A_ADDR,  ((uint32_t)quietduration1 << 16) | ((uint32_t)quietperiod1 << 8) | ((uint32_t)quietcount1 << 0));
}

/**
 * @brief Unpacks QUIET_ELEMENT_1A's fields from current value of the QUIET_ELEMENT_1A register.
 *
 * Reads the QUIET_ELEMENT_1A register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] quietduration1 - Will be populated with the current value of this field from the register.
 * @param[out] quietperiod1 - Will be populated with the current value of this field from the register.
 * @param[out] quietcount1 - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_quiet_element_1a_unpack(uint16_t *quietduration1, uint8_t *quietperiod1, uint8_t *quietcount1)
{
    uint32_t localVal = REG_PL_RD(NXMAC_QUIET_ELEMENT_1A_ADDR);

    *quietduration1 = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *quietperiod1 = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *quietcount1 = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the quietDuration1 field in the QUIET_ELEMENT_1A register.
 *
 * The QUIET_ELEMENT_1A register will be read and the quietDuration1 field's value will be returned.
 *
 * @return The current value of the quietDuration1 field in the QUIET_ELEMENT_1A register.
 */
__INLINE uint16_t nxmac_quiet_duration_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_QUIET_ELEMENT_1A_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Sets the quietDuration1 field of the QUIET_ELEMENT_1A register.
 *
 * The QUIET_ELEMENT_1A register will be read, modified to contain the new field value, and written.
 *
 * @param[in] quietduration1 - The value to set the field to.
 */
__INLINE void nxmac_quiet_duration_1_setf(uint16_t quietduration1)
{
    ASSERT_ERR((((uint32_t)quietduration1 << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    REG_PL_WR(NXMAC_QUIET_ELEMENT_1A_ADDR, (REG_PL_RD(NXMAC_QUIET_ELEMENT_1A_ADDR) & ~((uint32_t)0xFFFF0000)) | ((uint32_t)quietduration1 << 16));
}

/**
 * @brief Returns the current value of the quietPeriod1 field in the QUIET_ELEMENT_1A register.
 *
 * The QUIET_ELEMENT_1A register will be read and the quietPeriod1 field's value will be returned.
 *
 * @return The current value of the quietPeriod1 field in the QUIET_ELEMENT_1A register.
 */
__INLINE uint8_t nxmac_quiet_period_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_QUIET_ELEMENT_1A_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the quietPeriod1 field of the QUIET_ELEMENT_1A register.
 *
 * The QUIET_ELEMENT_1A register will be read, modified to contain the new field value, and written.
 *
 * @param[in] quietperiod1 - The value to set the field to.
 */
__INLINE void nxmac_quiet_period_1_setf(uint8_t quietperiod1)
{
    ASSERT_ERR((((uint32_t)quietperiod1 << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(NXMAC_QUIET_ELEMENT_1A_ADDR, (REG_PL_RD(NXMAC_QUIET_ELEMENT_1A_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)quietperiod1 << 8));
}

/**
 * @brief Returns the current value of the quietCount1 field in the QUIET_ELEMENT_1A register.
 *
 * The QUIET_ELEMENT_1A register will be read and the quietCount1 field's value will be returned.
 *
 * @return The current value of the quietCount1 field in the QUIET_ELEMENT_1A register.
 */
__INLINE uint8_t nxmac_quiet_count_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_QUIET_ELEMENT_1A_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the quietCount1 field of the QUIET_ELEMENT_1A register.
 *
 * The QUIET_ELEMENT_1A register will be read, modified to contain the new field value, and written.
 *
 * @param[in] quietcount1 - The value to set the field to.
 */
__INLINE void nxmac_quiet_count_1_setf(uint8_t quietcount1)
{
    ASSERT_ERR((((uint32_t)quietcount1 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_QUIET_ELEMENT_1A_ADDR, (REG_PL_RD(NXMAC_QUIET_ELEMENT_1A_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)quietcount1 << 0));
}

/// @}

/**
 * @name QUIET_ELEMENT_1B register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00         quietOffset1   0x0
 * </pre>
 *
 * @{
 */

/// Address of the QUIET_ELEMENT_1B register
#define NXMAC_QUIET_ELEMENT_1B_ADDR   0xC0000284
/// Offset of the QUIET_ELEMENT_1B register from the base address
#define NXMAC_QUIET_ELEMENT_1B_OFFSET 0x00000284
/// Index of the QUIET_ELEMENT_1B register
#define NXMAC_QUIET_ELEMENT_1B_INDEX  0x000000A1
/// Reset value of the QUIET_ELEMENT_1B register
#define NXMAC_QUIET_ELEMENT_1B_RESET  0x00000000

/**
 * @brief Returns the current value of the QUIET_ELEMENT_1B register.
 * The QUIET_ELEMENT_1B register will be read and its value returned.
 * @return The current value of the QUIET_ELEMENT_1B register.
 */
__INLINE uint32_t nxmac_quiet_element_1b_get(void)
{
    return REG_PL_RD(NXMAC_QUIET_ELEMENT_1B_ADDR);
}

/**
 * @brief Sets the QUIET_ELEMENT_1B register to a value.
 * The QUIET_ELEMENT_1B register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_quiet_element_1b_set(uint32_t value)
{
    REG_PL_WR(NXMAC_QUIET_ELEMENT_1B_ADDR, value);
}

// field definitions
/// QUIET_OFFSET_1 field mask
#define NXMAC_QUIET_OFFSET_1_MASK   ((uint32_t)0x0000FFFF)
/// QUIET_OFFSET_1 field LSB position
#define NXMAC_QUIET_OFFSET_1_LSB    0
/// QUIET_OFFSET_1 field width
#define NXMAC_QUIET_OFFSET_1_WIDTH  ((uint32_t)0x00000010)

/// QUIET_OFFSET_1 field reset value
#define NXMAC_QUIET_OFFSET_1_RST    0x0

/**
 * @brief Returns the current value of the quietOffset1 field in the QUIET_ELEMENT_1B register.
 *
 * The QUIET_ELEMENT_1B register will be read and the quietOffset1 field's value will be returned.
 *
 * @return The current value of the quietOffset1 field in the QUIET_ELEMENT_1B register.
 */
__INLINE uint16_t nxmac_quiet_offset_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_QUIET_ELEMENT_1B_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the quietOffset1 field of the QUIET_ELEMENT_1B register.
 *
 * The QUIET_ELEMENT_1B register will be read, modified to contain the new field value, and written.
 *
 * @param[in] quietoffset1 - The value to set the field to.
 */
__INLINE void nxmac_quiet_offset_1_setf(uint16_t quietoffset1)
{
    ASSERT_ERR((((uint32_t)quietoffset1 << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_QUIET_ELEMENT_1B_ADDR, (uint32_t)quietoffset1 << 0);
}

/// @}

/**
 * @name QUIET_ELEMENT_2A register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:16       quietDuration2   0x0
 *  15:08         quietPeriod2   0x0
 *  07:00          quietCount2   0x0
 * </pre>
 *
 * @{
 */

/// Address of the QUIET_ELEMENT_2A register
#define NXMAC_QUIET_ELEMENT_2A_ADDR   0xC0000288
/// Offset of the QUIET_ELEMENT_2A register from the base address
#define NXMAC_QUIET_ELEMENT_2A_OFFSET 0x00000288
/// Index of the QUIET_ELEMENT_2A register
#define NXMAC_QUIET_ELEMENT_2A_INDEX  0x000000A2
/// Reset value of the QUIET_ELEMENT_2A register
#define NXMAC_QUIET_ELEMENT_2A_RESET  0x00000000

/**
 * @brief Returns the current value of the QUIET_ELEMENT_2A register.
 * The QUIET_ELEMENT_2A register will be read and its value returned.
 * @return The current value of the QUIET_ELEMENT_2A register.
 */
__INLINE uint32_t nxmac_quiet_element_2a_get(void)
{
    return REG_PL_RD(NXMAC_QUIET_ELEMENT_2A_ADDR);
}

/**
 * @brief Sets the QUIET_ELEMENT_2A register to a value.
 * The QUIET_ELEMENT_2A register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_quiet_element_2a_set(uint32_t value)
{
    REG_PL_WR(NXMAC_QUIET_ELEMENT_2A_ADDR, value);
}

// field definitions
/// QUIET_DURATION_2 field mask
#define NXMAC_QUIET_DURATION_2_MASK   ((uint32_t)0xFFFF0000)
/// QUIET_DURATION_2 field LSB position
#define NXMAC_QUIET_DURATION_2_LSB    16
/// QUIET_DURATION_2 field width
#define NXMAC_QUIET_DURATION_2_WIDTH  ((uint32_t)0x00000010)
/// QUIET_PERIOD_2 field mask
#define NXMAC_QUIET_PERIOD_2_MASK     ((uint32_t)0x0000FF00)
/// QUIET_PERIOD_2 field LSB position
#define NXMAC_QUIET_PERIOD_2_LSB      8
/// QUIET_PERIOD_2 field width
#define NXMAC_QUIET_PERIOD_2_WIDTH    ((uint32_t)0x00000008)
/// QUIET_COUNT_2 field mask
#define NXMAC_QUIET_COUNT_2_MASK      ((uint32_t)0x000000FF)
/// QUIET_COUNT_2 field LSB position
#define NXMAC_QUIET_COUNT_2_LSB       0
/// QUIET_COUNT_2 field width
#define NXMAC_QUIET_COUNT_2_WIDTH     ((uint32_t)0x00000008)

/// QUIET_DURATION_2 field reset value
#define NXMAC_QUIET_DURATION_2_RST    0x0
/// QUIET_PERIOD_2 field reset value
#define NXMAC_QUIET_PERIOD_2_RST      0x0
/// QUIET_COUNT_2 field reset value
#define NXMAC_QUIET_COUNT_2_RST       0x0

/**
 * @brief Constructs a value for the QUIET_ELEMENT_2A register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] quietduration2 - The value to use for the quietDuration2 field.
 * @param[in] quietperiod2 - The value to use for the quietPeriod2 field.
 * @param[in] quietcount2 - The value to use for the quietCount2 field.
 */
__INLINE void nxmac_quiet_element_2a_pack(uint16_t quietduration2, uint8_t quietperiod2, uint8_t quietcount2)
{
    ASSERT_ERR((((uint32_t)quietduration2 << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    ASSERT_ERR((((uint32_t)quietperiod2 << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)quietcount2 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_QUIET_ELEMENT_2A_ADDR,  ((uint32_t)quietduration2 << 16) | ((uint32_t)quietperiod2 << 8) | ((uint32_t)quietcount2 << 0));
}

/**
 * @brief Unpacks QUIET_ELEMENT_2A's fields from current value of the QUIET_ELEMENT_2A register.
 *
 * Reads the QUIET_ELEMENT_2A register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] quietduration2 - Will be populated with the current value of this field from the register.
 * @param[out] quietperiod2 - Will be populated with the current value of this field from the register.
 * @param[out] quietcount2 - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_quiet_element_2a_unpack(uint16_t *quietduration2, uint8_t *quietperiod2, uint8_t *quietcount2)
{
    uint32_t localVal = REG_PL_RD(NXMAC_QUIET_ELEMENT_2A_ADDR);

    *quietduration2 = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *quietperiod2 = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *quietcount2 = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the quietDuration2 field in the QUIET_ELEMENT_2A register.
 *
 * The QUIET_ELEMENT_2A register will be read and the quietDuration2 field's value will be returned.
 *
 * @return The current value of the quietDuration2 field in the QUIET_ELEMENT_2A register.
 */
__INLINE uint16_t nxmac_quiet_duration_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_QUIET_ELEMENT_2A_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Sets the quietDuration2 field of the QUIET_ELEMENT_2A register.
 *
 * The QUIET_ELEMENT_2A register will be read, modified to contain the new field value, and written.
 *
 * @param[in] quietduration2 - The value to set the field to.
 */
__INLINE void nxmac_quiet_duration_2_setf(uint16_t quietduration2)
{
    ASSERT_ERR((((uint32_t)quietduration2 << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    REG_PL_WR(NXMAC_QUIET_ELEMENT_2A_ADDR, (REG_PL_RD(NXMAC_QUIET_ELEMENT_2A_ADDR) & ~((uint32_t)0xFFFF0000)) | ((uint32_t)quietduration2 << 16));
}

/**
 * @brief Returns the current value of the quietPeriod2 field in the QUIET_ELEMENT_2A register.
 *
 * The QUIET_ELEMENT_2A register will be read and the quietPeriod2 field's value will be returned.
 *
 * @return The current value of the quietPeriod2 field in the QUIET_ELEMENT_2A register.
 */
__INLINE uint8_t nxmac_quiet_period_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_QUIET_ELEMENT_2A_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the quietPeriod2 field of the QUIET_ELEMENT_2A register.
 *
 * The QUIET_ELEMENT_2A register will be read, modified to contain the new field value, and written.
 *
 * @param[in] quietperiod2 - The value to set the field to.
 */
__INLINE void nxmac_quiet_period_2_setf(uint8_t quietperiod2)
{
    ASSERT_ERR((((uint32_t)quietperiod2 << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(NXMAC_QUIET_ELEMENT_2A_ADDR, (REG_PL_RD(NXMAC_QUIET_ELEMENT_2A_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)quietperiod2 << 8));
}

/**
 * @brief Returns the current value of the quietCount2 field in the QUIET_ELEMENT_2A register.
 *
 * The QUIET_ELEMENT_2A register will be read and the quietCount2 field's value will be returned.
 *
 * @return The current value of the quietCount2 field in the QUIET_ELEMENT_2A register.
 */
__INLINE uint8_t nxmac_quiet_count_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_QUIET_ELEMENT_2A_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the quietCount2 field of the QUIET_ELEMENT_2A register.
 *
 * The QUIET_ELEMENT_2A register will be read, modified to contain the new field value, and written.
 *
 * @param[in] quietcount2 - The value to set the field to.
 */
__INLINE void nxmac_quiet_count_2_setf(uint8_t quietcount2)
{
    ASSERT_ERR((((uint32_t)quietcount2 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_QUIET_ELEMENT_2A_ADDR, (REG_PL_RD(NXMAC_QUIET_ELEMENT_2A_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)quietcount2 << 0));
}

/// @}

/**
 * @name QUIET_ELEMENT_2B register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00         quietOffset2   0x0
 * </pre>
 *
 * @{
 */

/// Address of the QUIET_ELEMENT_2B register
#define NXMAC_QUIET_ELEMENT_2B_ADDR   0xC000028C
/// Offset of the QUIET_ELEMENT_2B register from the base address
#define NXMAC_QUIET_ELEMENT_2B_OFFSET 0x0000028C
/// Index of the QUIET_ELEMENT_2B register
#define NXMAC_QUIET_ELEMENT_2B_INDEX  0x000000A3
/// Reset value of the QUIET_ELEMENT_2B register
#define NXMAC_QUIET_ELEMENT_2B_RESET  0x00000000

/**
 * @brief Returns the current value of the QUIET_ELEMENT_2B register.
 * The QUIET_ELEMENT_2B register will be read and its value returned.
 * @return The current value of the QUIET_ELEMENT_2B register.
 */
__INLINE uint32_t nxmac_quiet_element_2b_get(void)
{
    return REG_PL_RD(NXMAC_QUIET_ELEMENT_2B_ADDR);
}

/**
 * @brief Sets the QUIET_ELEMENT_2B register to a value.
 * The QUIET_ELEMENT_2B register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_quiet_element_2b_set(uint32_t value)
{
    REG_PL_WR(NXMAC_QUIET_ELEMENT_2B_ADDR, value);
}

// field definitions
/// QUIET_OFFSET_2 field mask
#define NXMAC_QUIET_OFFSET_2_MASK   ((uint32_t)0x0000FFFF)
/// QUIET_OFFSET_2 field LSB position
#define NXMAC_QUIET_OFFSET_2_LSB    0
/// QUIET_OFFSET_2 field width
#define NXMAC_QUIET_OFFSET_2_WIDTH  ((uint32_t)0x00000010)

/// QUIET_OFFSET_2 field reset value
#define NXMAC_QUIET_OFFSET_2_RST    0x0

/**
 * @brief Returns the current value of the quietOffset2 field in the QUIET_ELEMENT_2B register.
 *
 * The QUIET_ELEMENT_2B register will be read and the quietOffset2 field's value will be returned.
 *
 * @return The current value of the quietOffset2 field in the QUIET_ELEMENT_2B register.
 */
__INLINE uint16_t nxmac_quiet_offset_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_QUIET_ELEMENT_2B_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the quietOffset2 field of the QUIET_ELEMENT_2B register.
 *
 * The QUIET_ELEMENT_2B register will be read, modified to contain the new field value, and written.
 *
 * @param[in] quietoffset2 - The value to set the field to.
 */
__INLINE void nxmac_quiet_offset_2_setf(uint16_t quietoffset2)
{
    ASSERT_ERR((((uint32_t)quietoffset2 << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_QUIET_ELEMENT_2B_ADDR, (uint32_t)quietoffset2 << 0);
}

/// @}

/**
 * @name ADD_CCA_BUSY_SEC_20 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00      ccaBusyDurSec20   0x0
 * </pre>
 *
 * @{
 */

/// Address of the ADD_CCA_BUSY_SEC_20 register
#define NXMAC_ADD_CCA_BUSY_SEC_20_ADDR   0xC0000290
/// Offset of the ADD_CCA_BUSY_SEC_20 register from the base address
#define NXMAC_ADD_CCA_BUSY_SEC_20_OFFSET 0x00000290
/// Index of the ADD_CCA_BUSY_SEC_20 register
#define NXMAC_ADD_CCA_BUSY_SEC_20_INDEX  0x000000A4
/// Reset value of the ADD_CCA_BUSY_SEC_20 register
#define NXMAC_ADD_CCA_BUSY_SEC_20_RESET  0x00000000

/**
 * @brief Returns the current value of the ADD_CCA_BUSY_SEC_20 register.
 * The ADD_CCA_BUSY_SEC_20 register will be read and its value returned.
 * @return The current value of the ADD_CCA_BUSY_SEC_20 register.
 */
__INLINE uint32_t nxmac_add_cca_busy_sec_20_get(void)
{
    return REG_PL_RD(NXMAC_ADD_CCA_BUSY_SEC_20_ADDR);
}

/**
 * @brief Sets the ADD_CCA_BUSY_SEC_20 register to a value.
 * The ADD_CCA_BUSY_SEC_20 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_add_cca_busy_sec_20_set(uint32_t value)
{
    REG_PL_WR(NXMAC_ADD_CCA_BUSY_SEC_20_ADDR, value);
}

// field definitions
/// CCA_BUSY_DUR_SEC_20 field mask
#define NXMAC_CCA_BUSY_DUR_SEC_20_MASK   ((uint32_t)0xFFFFFFFF)
/// CCA_BUSY_DUR_SEC_20 field LSB position
#define NXMAC_CCA_BUSY_DUR_SEC_20_LSB    0
/// CCA_BUSY_DUR_SEC_20 field width
#define NXMAC_CCA_BUSY_DUR_SEC_20_WIDTH  ((uint32_t)0x00000020)

/// CCA_BUSY_DUR_SEC_20 field reset value
#define NXMAC_CCA_BUSY_DUR_SEC_20_RST    0x0

/**
 * @brief Returns the current value of the ccaBusyDurSec20 field in the ADD_CCA_BUSY_SEC_20 register.
 *
 * The ADD_CCA_BUSY_SEC_20 register will be read and the ccaBusyDurSec20 field's value will be returned.
 *
 * @return The current value of the ccaBusyDurSec20 field in the ADD_CCA_BUSY_SEC_20 register.
 */
__INLINE uint32_t nxmac_cca_busy_dur_sec_20_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ADD_CCA_BUSY_SEC_20_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the ccaBusyDurSec20 field of the ADD_CCA_BUSY_SEC_20 register.
 *
 * The ADD_CCA_BUSY_SEC_20 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ccabusydursec20 - The value to set the field to.
 */
__INLINE void nxmac_cca_busy_dur_sec_20_setf(uint32_t ccabusydursec20)
{
    ASSERT_ERR((((uint32_t)ccabusydursec20 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_ADD_CCA_BUSY_SEC_20_ADDR, (uint32_t)ccabusydursec20 << 0);
}

/// @}

/**
 * @name ADD_CCA_BUSY_SEC_40 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00      ccaBusyDurSec40   0x0
 * </pre>
 *
 * @{
 */

/// Address of the ADD_CCA_BUSY_SEC_40 register
#define NXMAC_ADD_CCA_BUSY_SEC_40_ADDR   0xC0000294
/// Offset of the ADD_CCA_BUSY_SEC_40 register from the base address
#define NXMAC_ADD_CCA_BUSY_SEC_40_OFFSET 0x00000294
/// Index of the ADD_CCA_BUSY_SEC_40 register
#define NXMAC_ADD_CCA_BUSY_SEC_40_INDEX  0x000000A5
/// Reset value of the ADD_CCA_BUSY_SEC_40 register
#define NXMAC_ADD_CCA_BUSY_SEC_40_RESET  0x00000000

/**
 * @brief Returns the current value of the ADD_CCA_BUSY_SEC_40 register.
 * The ADD_CCA_BUSY_SEC_40 register will be read and its value returned.
 * @return The current value of the ADD_CCA_BUSY_SEC_40 register.
 */
__INLINE uint32_t nxmac_add_cca_busy_sec_40_get(void)
{
    return REG_PL_RD(NXMAC_ADD_CCA_BUSY_SEC_40_ADDR);
}

/**
 * @brief Sets the ADD_CCA_BUSY_SEC_40 register to a value.
 * The ADD_CCA_BUSY_SEC_40 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_add_cca_busy_sec_40_set(uint32_t value)
{
    REG_PL_WR(NXMAC_ADD_CCA_BUSY_SEC_40_ADDR, value);
}

// field definitions
/// CCA_BUSY_DUR_SEC_40 field mask
#define NXMAC_CCA_BUSY_DUR_SEC_40_MASK   ((uint32_t)0xFFFFFFFF)
/// CCA_BUSY_DUR_SEC_40 field LSB position
#define NXMAC_CCA_BUSY_DUR_SEC_40_LSB    0
/// CCA_BUSY_DUR_SEC_40 field width
#define NXMAC_CCA_BUSY_DUR_SEC_40_WIDTH  ((uint32_t)0x00000020)

/// CCA_BUSY_DUR_SEC_40 field reset value
#define NXMAC_CCA_BUSY_DUR_SEC_40_RST    0x0

/**
 * @brief Returns the current value of the ccaBusyDurSec40 field in the ADD_CCA_BUSY_SEC_40 register.
 *
 * The ADD_CCA_BUSY_SEC_40 register will be read and the ccaBusyDurSec40 field's value will be returned.
 *
 * @return The current value of the ccaBusyDurSec40 field in the ADD_CCA_BUSY_SEC_40 register.
 */
__INLINE uint32_t nxmac_cca_busy_dur_sec_40_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ADD_CCA_BUSY_SEC_40_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the ccaBusyDurSec40 field of the ADD_CCA_BUSY_SEC_40 register.
 *
 * The ADD_CCA_BUSY_SEC_40 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ccabusydursec40 - The value to set the field to.
 */
__INLINE void nxmac_cca_busy_dur_sec_40_setf(uint32_t ccabusydursec40)
{
    ASSERT_ERR((((uint32_t)ccabusydursec40 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_ADD_CCA_BUSY_SEC_40_ADDR, (uint32_t)ccabusydursec40 << 0);
}

/// @}

/**
 * @name ADD_CCA_BUSY_SEC_80 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00      ccaBusyDurSec80   0x0
 * </pre>
 *
 * @{
 */

/// Address of the ADD_CCA_BUSY_SEC_80 register
#define NXMAC_ADD_CCA_BUSY_SEC_80_ADDR   0xC0000298
/// Offset of the ADD_CCA_BUSY_SEC_80 register from the base address
#define NXMAC_ADD_CCA_BUSY_SEC_80_OFFSET 0x00000298
/// Index of the ADD_CCA_BUSY_SEC_80 register
#define NXMAC_ADD_CCA_BUSY_SEC_80_INDEX  0x000000A6
/// Reset value of the ADD_CCA_BUSY_SEC_80 register
#define NXMAC_ADD_CCA_BUSY_SEC_80_RESET  0x00000000

/**
 * @brief Returns the current value of the ADD_CCA_BUSY_SEC_80 register.
 * The ADD_CCA_BUSY_SEC_80 register will be read and its value returned.
 * @return The current value of the ADD_CCA_BUSY_SEC_80 register.
 */
__INLINE uint32_t nxmac_add_cca_busy_sec_80_get(void)
{
    return REG_PL_RD(NXMAC_ADD_CCA_BUSY_SEC_80_ADDR);
}

/**
 * @brief Sets the ADD_CCA_BUSY_SEC_80 register to a value.
 * The ADD_CCA_BUSY_SEC_80 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_add_cca_busy_sec_80_set(uint32_t value)
{
    REG_PL_WR(NXMAC_ADD_CCA_BUSY_SEC_80_ADDR, value);
}

// field definitions
/// CCA_BUSY_DUR_SEC_80 field mask
#define NXMAC_CCA_BUSY_DUR_SEC_80_MASK   ((uint32_t)0xFFFFFFFF)
/// CCA_BUSY_DUR_SEC_80 field LSB position
#define NXMAC_CCA_BUSY_DUR_SEC_80_LSB    0
/// CCA_BUSY_DUR_SEC_80 field width
#define NXMAC_CCA_BUSY_DUR_SEC_80_WIDTH  ((uint32_t)0x00000020)

/// CCA_BUSY_DUR_SEC_80 field reset value
#define NXMAC_CCA_BUSY_DUR_SEC_80_RST    0x0

/**
 * @brief Returns the current value of the ccaBusyDurSec80 field in the ADD_CCA_BUSY_SEC_80 register.
 *
 * The ADD_CCA_BUSY_SEC_80 register will be read and the ccaBusyDurSec80 field's value will be returned.
 *
 * @return The current value of the ccaBusyDurSec80 field in the ADD_CCA_BUSY_SEC_80 register.
 */
__INLINE uint32_t nxmac_cca_busy_dur_sec_80_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_ADD_CCA_BUSY_SEC_80_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the ccaBusyDurSec80 field of the ADD_CCA_BUSY_SEC_80 register.
 *
 * The ADD_CCA_BUSY_SEC_80 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ccabusydursec80 - The value to set the field to.
 */
__INLINE void nxmac_cca_busy_dur_sec_80_setf(uint32_t ccabusydursec80)
{
    ASSERT_ERR((((uint32_t)ccabusydursec80 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(NXMAC_ADD_CCA_BUSY_SEC_80_ADDR, (uint32_t)ccabusydursec80 << 0);
}

/// @}

/**
 * @name STBC_CNTRL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:25         basicSTBCMCS   0x0
 *     24          dualCTSProt   0
 *  23:16           ctsSTBCDur   0x0
 *  15:00         cfEndSTBCDur   0x0
 * </pre>
 *
 * @{
 */

/// Address of the STBC_CNTRL register
#define NXMAC_STBC_CNTRL_ADDR   0xC0000300
/// Offset of the STBC_CNTRL register from the base address
#define NXMAC_STBC_CNTRL_OFFSET 0x00000300
/// Index of the STBC_CNTRL register
#define NXMAC_STBC_CNTRL_INDEX  0x000000C0
/// Reset value of the STBC_CNTRL register
#define NXMAC_STBC_CNTRL_RESET  0x00000000

/**
 * @brief Returns the current value of the STBC_CNTRL register.
 * The STBC_CNTRL register will be read and its value returned.
 * @return The current value of the STBC_CNTRL register.
 */
__INLINE uint32_t nxmac_stbc_cntrl_get(void)
{
    return REG_PL_RD(NXMAC_STBC_CNTRL_ADDR);
}

/**
 * @brief Sets the STBC_CNTRL register to a value.
 * The STBC_CNTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_stbc_cntrl_set(uint32_t value)
{
    REG_PL_WR(NXMAC_STBC_CNTRL_ADDR, value);
}

// field definitions
/// BASIC_STBCMCS field mask
#define NXMAC_BASIC_STBCMCS_MASK     ((uint32_t)0xFE000000)
/// BASIC_STBCMCS field LSB position
#define NXMAC_BASIC_STBCMCS_LSB      25
/// BASIC_STBCMCS field width
#define NXMAC_BASIC_STBCMCS_WIDTH    ((uint32_t)0x00000007)
/// DUAL_CTS_PROT field bit
#define NXMAC_DUAL_CTS_PROT_BIT      ((uint32_t)0x01000000)
/// DUAL_CTS_PROT field position
#define NXMAC_DUAL_CTS_PROT_POS      24
/// CTS_STBC_DUR field mask
#define NXMAC_CTS_STBC_DUR_MASK      ((uint32_t)0x00FF0000)
/// CTS_STBC_DUR field LSB position
#define NXMAC_CTS_STBC_DUR_LSB       16
/// CTS_STBC_DUR field width
#define NXMAC_CTS_STBC_DUR_WIDTH     ((uint32_t)0x00000008)
/// CF_END_STBC_DUR field mask
#define NXMAC_CF_END_STBC_DUR_MASK   ((uint32_t)0x0000FFFF)
/// CF_END_STBC_DUR field LSB position
#define NXMAC_CF_END_STBC_DUR_LSB    0
/// CF_END_STBC_DUR field width
#define NXMAC_CF_END_STBC_DUR_WIDTH  ((uint32_t)0x00000010)

/// BASIC_STBCMCS field reset value
#define NXMAC_BASIC_STBCMCS_RST      0x0
/// DUAL_CTS_PROT field reset value
#define NXMAC_DUAL_CTS_PROT_RST      0x0
/// CTS_STBC_DUR field reset value
#define NXMAC_CTS_STBC_DUR_RST       0x0
/// CF_END_STBC_DUR field reset value
#define NXMAC_CF_END_STBC_DUR_RST    0x0

/**
 * @brief Constructs a value for the STBC_CNTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] basicstbcmcs - The value to use for the basicSTBCMCS field.
 * @param[in] dualctsprot - The value to use for the dualCTSProt field.
 * @param[in] ctsstbcdur - The value to use for the ctsSTBCDur field.
 * @param[in] cfendstbcdur - The value to use for the cfEndSTBCDur field.
 */
__INLINE void nxmac_stbc_cntrl_pack(uint8_t basicstbcmcs, uint8_t dualctsprot, uint8_t ctsstbcdur, uint16_t cfendstbcdur)
{
    ASSERT_ERR((((uint32_t)basicstbcmcs << 25) & ~((uint32_t)0xFE000000)) == 0);
    ASSERT_ERR((((uint32_t)dualctsprot << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)ctsstbcdur << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)cfendstbcdur << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_STBC_CNTRL_ADDR,  ((uint32_t)basicstbcmcs << 25) | ((uint32_t)dualctsprot << 24) | ((uint32_t)ctsstbcdur << 16) | ((uint32_t)cfendstbcdur << 0));
}

/**
 * @brief Unpacks STBC_CNTRL's fields from current value of the STBC_CNTRL register.
 *
 * Reads the STBC_CNTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] basicstbcmcs - Will be populated with the current value of this field from the register.
 * @param[out] dualctsprot - Will be populated with the current value of this field from the register.
 * @param[out] ctsstbcdur - Will be populated with the current value of this field from the register.
 * @param[out] cfendstbcdur - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_stbc_cntrl_unpack(uint8_t *basicstbcmcs, uint8_t *dualctsprot, uint8_t *ctsstbcdur, uint16_t *cfendstbcdur)
{
    uint32_t localVal = REG_PL_RD(NXMAC_STBC_CNTRL_ADDR);

    *basicstbcmcs = (localVal & ((uint32_t)0xFE000000)) >> 25;
    *dualctsprot = (localVal & ((uint32_t)0x01000000)) >> 24;
    *ctsstbcdur = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *cfendstbcdur = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the basicSTBCMCS field in the STBC_CNTRL register.
 *
 * The STBC_CNTRL register will be read and the basicSTBCMCS field's value will be returned.
 *
 * @return The current value of the basicSTBCMCS field in the STBC_CNTRL register.
 */
__INLINE uint8_t nxmac_basic_stbcmcs_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_STBC_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0xFE000000)) >> 25);
}

/**
 * @brief Sets the basicSTBCMCS field of the STBC_CNTRL register.
 *
 * The STBC_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] basicstbcmcs - The value to set the field to.
 */
__INLINE void nxmac_basic_stbcmcs_setf(uint8_t basicstbcmcs)
{
    ASSERT_ERR((((uint32_t)basicstbcmcs << 25) & ~((uint32_t)0xFE000000)) == 0);
    REG_PL_WR(NXMAC_STBC_CNTRL_ADDR, (REG_PL_RD(NXMAC_STBC_CNTRL_ADDR) & ~((uint32_t)0xFE000000)) | ((uint32_t)basicstbcmcs << 25));
}

/**
 * @brief Returns the current value of the dualCTSProt field in the STBC_CNTRL register.
 *
 * The STBC_CNTRL register will be read and the dualCTSProt field's value will be returned.
 *
 * @return The current value of the dualCTSProt field in the STBC_CNTRL register.
 */
__INLINE uint8_t nxmac_dual_cts_prot_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_STBC_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the dualCTSProt field of the STBC_CNTRL register.
 *
 * The STBC_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dualctsprot - The value to set the field to.
 */
__INLINE void nxmac_dual_cts_prot_setf(uint8_t dualctsprot)
{
    ASSERT_ERR((((uint32_t)dualctsprot << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(NXMAC_STBC_CNTRL_ADDR, (REG_PL_RD(NXMAC_STBC_CNTRL_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)dualctsprot << 24));
}

/**
 * @brief Returns the current value of the ctsSTBCDur field in the STBC_CNTRL register.
 *
 * The STBC_CNTRL register will be read and the ctsSTBCDur field's value will be returned.
 *
 * @return The current value of the ctsSTBCDur field in the STBC_CNTRL register.
 */
__INLINE uint8_t nxmac_cts_stbc_dur_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_STBC_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the ctsSTBCDur field of the STBC_CNTRL register.
 *
 * The STBC_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ctsstbcdur - The value to set the field to.
 */
__INLINE void nxmac_cts_stbc_dur_setf(uint8_t ctsstbcdur)
{
    ASSERT_ERR((((uint32_t)ctsstbcdur << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(NXMAC_STBC_CNTRL_ADDR, (REG_PL_RD(NXMAC_STBC_CNTRL_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)ctsstbcdur << 16));
}

/**
 * @brief Returns the current value of the cfEndSTBCDur field in the STBC_CNTRL register.
 *
 * The STBC_CNTRL register will be read and the cfEndSTBCDur field's value will be returned.
 *
 * @return The current value of the cfEndSTBCDur field in the STBC_CNTRL register.
 */
__INLINE uint16_t nxmac_cf_end_stbc_dur_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_STBC_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the cfEndSTBCDur field of the STBC_CNTRL register.
 *
 * The STBC_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cfendstbcdur - The value to set the field to.
 */
__INLINE void nxmac_cf_end_stbc_dur_setf(uint16_t cfendstbcdur)
{
    ASSERT_ERR((((uint32_t)cfendstbcdur << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_STBC_CNTRL_ADDR, (REG_PL_RD(NXMAC_STBC_CNTRL_ADDR) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)cfendstbcdur << 0));
}

/// @}

/**
 * @name START_TX_1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:28            startTxBW   0x0
 *     27       startTxPreType   0
 *  26:24     startTxFormatMod   0x0
 *  23:16     startTxMCSIndex0   0x0
 *  15:06      startTxKSRIndex   0x0
 *  04:03            startTxAC   0x0
 *  02:01     startTxFrmExType   0x0
 *     00       startTxFrameEx   0
 * </pre>
 *
 * @{
 */

/// Address of the START_TX_1 register
#define NXMAC_START_TX_1_ADDR   0xC0000304
/// Offset of the START_TX_1 register from the base address
#define NXMAC_START_TX_1_OFFSET 0x00000304
/// Index of the START_TX_1 register
#define NXMAC_START_TX_1_INDEX  0x000000C1
/// Reset value of the START_TX_1 register
#define NXMAC_START_TX_1_RESET  0x00000000

/**
 * @brief Returns the current value of the START_TX_1 register.
 * The START_TX_1 register will be read and its value returned.
 * @return The current value of the START_TX_1 register.
 */
__INLINE uint32_t nxmac_start_tx_1_get(void)
{
    return REG_PL_RD(NXMAC_START_TX_1_ADDR);
}

/**
 * @brief Sets the START_TX_1 register to a value.
 * The START_TX_1 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_start_tx_1_set(uint32_t value)
{
    REG_PL_WR(NXMAC_START_TX_1_ADDR, value);
}

// field definitions
/// START_TX_BW field mask
#define NXMAC_START_TX_BW_MASK            ((uint32_t)0x30000000)
/// START_TX_BW field LSB position
#define NXMAC_START_TX_BW_LSB             28
/// START_TX_BW field width
#define NXMAC_START_TX_BW_WIDTH           ((uint32_t)0x00000002)
/// START_TX_PRE_TYPE field bit
#define NXMAC_START_TX_PRE_TYPE_BIT       ((uint32_t)0x08000000)
/// START_TX_PRE_TYPE field position
#define NXMAC_START_TX_PRE_TYPE_POS       27
/// START_TX_FORMAT_MOD field mask
#define NXMAC_START_TX_FORMAT_MOD_MASK    ((uint32_t)0x07000000)
/// START_TX_FORMAT_MOD field LSB position
#define NXMAC_START_TX_FORMAT_MOD_LSB     24
/// START_TX_FORMAT_MOD field width
#define NXMAC_START_TX_FORMAT_MOD_WIDTH   ((uint32_t)0x00000003)
/// START_TX_MCS_INDEX_0 field mask
#define NXMAC_START_TX_MCS_INDEX_0_MASK   ((uint32_t)0x00FF0000)
/// START_TX_MCS_INDEX_0 field LSB position
#define NXMAC_START_TX_MCS_INDEX_0_LSB    16
/// START_TX_MCS_INDEX_0 field width
#define NXMAC_START_TX_MCS_INDEX_0_WIDTH  ((uint32_t)0x00000008)
/// START_TX_KSR_INDEX field mask
#define NXMAC_START_TX_KSR_INDEX_MASK     ((uint32_t)0x0000FFC0)
/// START_TX_KSR_INDEX field LSB position
#define NXMAC_START_TX_KSR_INDEX_LSB      6
/// START_TX_KSR_INDEX field width
#define NXMAC_START_TX_KSR_INDEX_WIDTH    ((uint32_t)0x0000000A)
/// START_TX_AC field mask
#define NXMAC_START_TX_AC_MASK            ((uint32_t)0x00000018)
/// START_TX_AC field LSB position
#define NXMAC_START_TX_AC_LSB             3
/// START_TX_AC field width
#define NXMAC_START_TX_AC_WIDTH           ((uint32_t)0x00000002)
/// START_TX_FRM_EX_TYPE field mask
#define NXMAC_START_TX_FRM_EX_TYPE_MASK   ((uint32_t)0x00000006)
/// START_TX_FRM_EX_TYPE field LSB position
#define NXMAC_START_TX_FRM_EX_TYPE_LSB    1
/// START_TX_FRM_EX_TYPE field width
#define NXMAC_START_TX_FRM_EX_TYPE_WIDTH  ((uint32_t)0x00000002)
/// START_TX_FRAME_EX field bit
#define NXMAC_START_TX_FRAME_EX_BIT       ((uint32_t)0x00000001)
/// START_TX_FRAME_EX field position
#define NXMAC_START_TX_FRAME_EX_POS       0

/// START_TX_BW field reset value
#define NXMAC_START_TX_BW_RST             0x0
/// START_TX_PRE_TYPE field reset value
#define NXMAC_START_TX_PRE_TYPE_RST       0x0
/// START_TX_FORMAT_MOD field reset value
#define NXMAC_START_TX_FORMAT_MOD_RST     0x0
/// START_TX_MCS_INDEX_0 field reset value
#define NXMAC_START_TX_MCS_INDEX_0_RST    0x0
/// START_TX_KSR_INDEX field reset value
#define NXMAC_START_TX_KSR_INDEX_RST      0x0
/// START_TX_AC field reset value
#define NXMAC_START_TX_AC_RST             0x0
/// START_TX_FRM_EX_TYPE field reset value
#define NXMAC_START_TX_FRM_EX_TYPE_RST    0x0
/// START_TX_FRAME_EX field reset value
#define NXMAC_START_TX_FRAME_EX_RST       0x0

/**
 * @brief Constructs a value for the START_TX_1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] starttxbw - The value to use for the startTxBW field.
 * @param[in] starttxpretype - The value to use for the startTxPreType field.
 * @param[in] starttxformatmod - The value to use for the startTxFormatMod field.
 * @param[in] starttxmcsindex0 - The value to use for the startTxMCSIndex0 field.
 * @param[in] starttxksrindex - The value to use for the startTxKSRIndex field.
 * @param[in] starttxac - The value to use for the startTxAC field.
 * @param[in] starttxfrmextype - The value to use for the startTxFrmExType field.
 * @param[in] starttxframeex - The value to use for the startTxFrameEx field.
 */
__INLINE void nxmac_start_tx_1_pack(uint8_t starttxbw, uint8_t starttxpretype, uint8_t starttxformatmod, uint8_t starttxmcsindex0, uint16_t starttxksrindex, uint8_t starttxac, uint8_t starttxfrmextype, uint8_t starttxframeex)
{
    ASSERT_ERR((((uint32_t)starttxbw << 28) & ~((uint32_t)0x30000000)) == 0);
    ASSERT_ERR((((uint32_t)starttxpretype << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)starttxformatmod << 24) & ~((uint32_t)0x07000000)) == 0);
    ASSERT_ERR((((uint32_t)starttxmcsindex0 << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)starttxksrindex << 6) & ~((uint32_t)0x0000FFC0)) == 0);
    ASSERT_ERR((((uint32_t)starttxac << 3) & ~((uint32_t)0x00000018)) == 0);
    ASSERT_ERR((((uint32_t)starttxfrmextype << 1) & ~((uint32_t)0x00000006)) == 0);
    ASSERT_ERR((((uint32_t)starttxframeex << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_START_TX_1_ADDR,  ((uint32_t)starttxbw << 28) | ((uint32_t)starttxpretype << 27) | ((uint32_t)starttxformatmod << 24) | ((uint32_t)starttxmcsindex0 << 16) | ((uint32_t)starttxksrindex << 6) | ((uint32_t)starttxac << 3) | ((uint32_t)starttxfrmextype << 1) | ((uint32_t)starttxframeex << 0));
}

/**
 * @brief Unpacks START_TX_1's fields from current value of the START_TX_1 register.
 *
 * Reads the START_TX_1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] starttxbw - Will be populated with the current value of this field from the register.
 * @param[out] starttxpretype - Will be populated with the current value of this field from the register.
 * @param[out] starttxformatmod - Will be populated with the current value of this field from the register.
 * @param[out] starttxmcsindex0 - Will be populated with the current value of this field from the register.
 * @param[out] starttxksrindex - Will be populated with the current value of this field from the register.
 * @param[out] starttxac - Will be populated with the current value of this field from the register.
 * @param[out] starttxfrmextype - Will be populated with the current value of this field from the register.
 * @param[out] starttxframeex - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_start_tx_1_unpack(uint8_t *starttxbw, uint8_t *starttxpretype, uint8_t *starttxformatmod, uint8_t *starttxmcsindex0, uint16_t *starttxksrindex, uint8_t *starttxac, uint8_t *starttxfrmextype, uint8_t *starttxframeex)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_1_ADDR);

    *starttxbw = (localVal & ((uint32_t)0x30000000)) >> 28;
    *starttxpretype = (localVal & ((uint32_t)0x08000000)) >> 27;
    *starttxformatmod = (localVal & ((uint32_t)0x07000000)) >> 24;
    *starttxmcsindex0 = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *starttxksrindex = (localVal & ((uint32_t)0x0000FFC0)) >> 6;
    *starttxac = (localVal & ((uint32_t)0x00000018)) >> 3;
    *starttxfrmextype = (localVal & ((uint32_t)0x00000006)) >> 1;
    *starttxframeex = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the startTxBW field in the START_TX_1 register.
 *
 * The START_TX_1 register will be read and the startTxBW field's value will be returned.
 *
 * @return The current value of the startTxBW field in the START_TX_1 register.
 */
__INLINE uint8_t nxmac_start_tx_bw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_1_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

/**
 * @brief Sets the startTxBW field of the START_TX_1 register.
 *
 * The START_TX_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] starttxbw - The value to set the field to.
 */
__INLINE void nxmac_start_tx_bw_setf(uint8_t starttxbw)
{
    ASSERT_ERR((((uint32_t)starttxbw << 28) & ~((uint32_t)0x30000000)) == 0);
    REG_PL_WR(NXMAC_START_TX_1_ADDR, (REG_PL_RD(NXMAC_START_TX_1_ADDR) & ~((uint32_t)0x30000000)) | ((uint32_t)starttxbw << 28));
}

/**
 * @brief Returns the current value of the startTxPreType field in the START_TX_1 register.
 *
 * The START_TX_1 register will be read and the startTxPreType field's value will be returned.
 *
 * @return The current value of the startTxPreType field in the START_TX_1 register.
 */
__INLINE uint8_t nxmac_start_tx_pre_type_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_1_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Sets the startTxPreType field of the START_TX_1 register.
 *
 * The START_TX_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] starttxpretype - The value to set the field to.
 */
__INLINE void nxmac_start_tx_pre_type_setf(uint8_t starttxpretype)
{
    ASSERT_ERR((((uint32_t)starttxpretype << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(NXMAC_START_TX_1_ADDR, (REG_PL_RD(NXMAC_START_TX_1_ADDR) & ~((uint32_t)0x08000000)) | ((uint32_t)starttxpretype << 27));
}

/**
 * @brief Returns the current value of the startTxFormatMod field in the START_TX_1 register.
 *
 * The START_TX_1 register will be read and the startTxFormatMod field's value will be returned.
 *
 * @return The current value of the startTxFormatMod field in the START_TX_1 register.
 */
__INLINE uint8_t nxmac_start_tx_format_mod_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_1_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

/**
 * @brief Sets the startTxFormatMod field of the START_TX_1 register.
 *
 * The START_TX_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] starttxformatmod - The value to set the field to.
 */
__INLINE void nxmac_start_tx_format_mod_setf(uint8_t starttxformatmod)
{
    ASSERT_ERR((((uint32_t)starttxformatmod << 24) & ~((uint32_t)0x07000000)) == 0);
    REG_PL_WR(NXMAC_START_TX_1_ADDR, (REG_PL_RD(NXMAC_START_TX_1_ADDR) & ~((uint32_t)0x07000000)) | ((uint32_t)starttxformatmod << 24));
}

/**
 * @brief Returns the current value of the startTxMCSIndex0 field in the START_TX_1 register.
 *
 * The START_TX_1 register will be read and the startTxMCSIndex0 field's value will be returned.
 *
 * @return The current value of the startTxMCSIndex0 field in the START_TX_1 register.
 */
__INLINE uint8_t nxmac_start_tx_mcs_index_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_1_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the startTxMCSIndex0 field of the START_TX_1 register.
 *
 * The START_TX_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] starttxmcsindex0 - The value to set the field to.
 */
__INLINE void nxmac_start_tx_mcs_index_0_setf(uint8_t starttxmcsindex0)
{
    ASSERT_ERR((((uint32_t)starttxmcsindex0 << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(NXMAC_START_TX_1_ADDR, (REG_PL_RD(NXMAC_START_TX_1_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)starttxmcsindex0 << 16));
}

/**
 * @brief Returns the current value of the startTxKSRIndex field in the START_TX_1 register.
 *
 * The START_TX_1 register will be read and the startTxKSRIndex field's value will be returned.
 *
 * @return The current value of the startTxKSRIndex field in the START_TX_1 register.
 */
__INLINE uint16_t nxmac_start_tx_ksr_index_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_1_ADDR);
    return ((localVal & ((uint32_t)0x0000FFC0)) >> 6);
}

/**
 * @brief Sets the startTxKSRIndex field of the START_TX_1 register.
 *
 * The START_TX_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] starttxksrindex - The value to set the field to.
 */
__INLINE void nxmac_start_tx_ksr_index_setf(uint16_t starttxksrindex)
{
    ASSERT_ERR((((uint32_t)starttxksrindex << 6) & ~((uint32_t)0x0000FFC0)) == 0);
    REG_PL_WR(NXMAC_START_TX_1_ADDR, (REG_PL_RD(NXMAC_START_TX_1_ADDR) & ~((uint32_t)0x0000FFC0)) | ((uint32_t)starttxksrindex << 6));
}

/**
 * @brief Returns the current value of the startTxAC field in the START_TX_1 register.
 *
 * The START_TX_1 register will be read and the startTxAC field's value will be returned.
 *
 * @return The current value of the startTxAC field in the START_TX_1 register.
 */
__INLINE uint8_t nxmac_start_tx_ac_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_1_ADDR);
    return ((localVal & ((uint32_t)0x00000018)) >> 3);
}

/**
 * @brief Sets the startTxAC field of the START_TX_1 register.
 *
 * The START_TX_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] starttxac - The value to set the field to.
 */
__INLINE void nxmac_start_tx_ac_setf(uint8_t starttxac)
{
    ASSERT_ERR((((uint32_t)starttxac << 3) & ~((uint32_t)0x00000018)) == 0);
    REG_PL_WR(NXMAC_START_TX_1_ADDR, (REG_PL_RD(NXMAC_START_TX_1_ADDR) & ~((uint32_t)0x00000018)) | ((uint32_t)starttxac << 3));
}

/**
 * @brief Returns the current value of the startTxFrmExType field in the START_TX_1 register.
 *
 * The START_TX_1 register will be read and the startTxFrmExType field's value will be returned.
 *
 * @return The current value of the startTxFrmExType field in the START_TX_1 register.
 */
__INLINE uint8_t nxmac_start_tx_frm_ex_type_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_1_ADDR);
    return ((localVal & ((uint32_t)0x00000006)) >> 1);
}

/**
 * @brief Sets the startTxFrmExType field of the START_TX_1 register.
 *
 * The START_TX_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] starttxfrmextype - The value to set the field to.
 */
__INLINE void nxmac_start_tx_frm_ex_type_setf(uint8_t starttxfrmextype)
{
    ASSERT_ERR((((uint32_t)starttxfrmextype << 1) & ~((uint32_t)0x00000006)) == 0);
    REG_PL_WR(NXMAC_START_TX_1_ADDR, (REG_PL_RD(NXMAC_START_TX_1_ADDR) & ~((uint32_t)0x00000006)) | ((uint32_t)starttxfrmextype << 1));
}

/**
 * @brief Returns the current value of the startTxFrameEx field in the START_TX_1 register.
 *
 * The START_TX_1 register will be read and the startTxFrameEx field's value will be returned.
 *
 * @return The current value of the startTxFrameEx field in the START_TX_1 register.
 */
__INLINE uint8_t nxmac_start_tx_frame_ex_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_1_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the startTxFrameEx field of the START_TX_1 register.
 *
 * The START_TX_1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] starttxframeex - The value to set the field to.
 */
__INLINE void nxmac_start_tx_frame_ex_setf(uint8_t starttxframeex)
{
    ASSERT_ERR((((uint32_t)starttxframeex << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_START_TX_1_ADDR, (REG_PL_RD(NXMAC_START_TX_1_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)starttxframeex << 0));
}

/// @}

/**
 * @name START_TX_2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00        durControlFrm   0x0
 * </pre>
 *
 * @{
 */

/// Address of the START_TX_2 register
#define NXMAC_START_TX_2_ADDR   0xC0000308
/// Offset of the START_TX_2 register from the base address
#define NXMAC_START_TX_2_OFFSET 0x00000308
/// Index of the START_TX_2 register
#define NXMAC_START_TX_2_INDEX  0x000000C2
/// Reset value of the START_TX_2 register
#define NXMAC_START_TX_2_RESET  0x00000000

/**
 * @brief Returns the current value of the START_TX_2 register.
 * The START_TX_2 register will be read and its value returned.
 * @return The current value of the START_TX_2 register.
 */
__INLINE uint32_t nxmac_start_tx_2_get(void)
{
    return REG_PL_RD(NXMAC_START_TX_2_ADDR);
}

/**
 * @brief Sets the START_TX_2 register to a value.
 * The START_TX_2 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_start_tx_2_set(uint32_t value)
{
    REG_PL_WR(NXMAC_START_TX_2_ADDR, value);
}

// field definitions
/// DUR_CONTROL_FRM field mask
#define NXMAC_DUR_CONTROL_FRM_MASK   ((uint32_t)0x0000FFFF)
/// DUR_CONTROL_FRM field LSB position
#define NXMAC_DUR_CONTROL_FRM_LSB    0
/// DUR_CONTROL_FRM field width
#define NXMAC_DUR_CONTROL_FRM_WIDTH  ((uint32_t)0x00000010)

/// DUR_CONTROL_FRM field reset value
#define NXMAC_DUR_CONTROL_FRM_RST    0x0

/**
 * @brief Returns the current value of the durControlFrm field in the START_TX_2 register.
 *
 * The START_TX_2 register will be read and the durControlFrm field's value will be returned.
 *
 * @return The current value of the durControlFrm field in the START_TX_2 register.
 */
__INLINE uint16_t nxmac_dur_control_frm_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_2_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the durControlFrm field of the START_TX_2 register.
 *
 * The START_TX_2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] durcontrolfrm - The value to set the field to.
 */
__INLINE void nxmac_dur_control_frm_setf(uint16_t durcontrolfrm)
{
    ASSERT_ERR((((uint32_t)durcontrolfrm << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_START_TX_2_ADDR, (uint32_t)durcontrolfrm << 0);
}

/// @}

/**
 * @name START_TX_3 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:24      startTxSMMIndex   0x0
 *  23:16    startTxAntennaSet   0x0
 *     13          startTxLSTP   0
 *     12     startTxSmoothing   0
 *     08       startTxShortGI   0
 *  07:05           startTxNTx   0x0
 *     04     startTxFecCoding   0
 *  03:02          startTxSTBC   0x0
 *  01:00     startTxNumExtnSS   0x0
 * </pre>
 *
 * @{
 */

/// Address of the START_TX_3 register
#define NXMAC_START_TX_3_ADDR   0xC000030C
/// Offset of the START_TX_3 register from the base address
#define NXMAC_START_TX_3_OFFSET 0x0000030C
/// Index of the START_TX_3 register
#define NXMAC_START_TX_3_INDEX  0x000000C3
/// Reset value of the START_TX_3 register
#define NXMAC_START_TX_3_RESET  0x00000000

/**
 * @brief Returns the current value of the START_TX_3 register.
 * The START_TX_3 register will be read and its value returned.
 * @return The current value of the START_TX_3 register.
 */
__INLINE uint32_t nxmac_start_tx_3_get(void)
{
    return REG_PL_RD(NXMAC_START_TX_3_ADDR);
}

/**
 * @brief Sets the START_TX_3 register to a value.
 * The START_TX_3 register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_start_tx_3_set(uint32_t value)
{
    REG_PL_WR(NXMAC_START_TX_3_ADDR, value);
}

// field definitions
/// START_TX_SMM_INDEX field mask
#define NXMAC_START_TX_SMM_INDEX_MASK     ((uint32_t)0xFF000000)
/// START_TX_SMM_INDEX field LSB position
#define NXMAC_START_TX_SMM_INDEX_LSB      24
/// START_TX_SMM_INDEX field width
#define NXMAC_START_TX_SMM_INDEX_WIDTH    ((uint32_t)0x00000008)
/// START_TX_ANTENNA_SET field mask
#define NXMAC_START_TX_ANTENNA_SET_MASK   ((uint32_t)0x00FF0000)
/// START_TX_ANTENNA_SET field LSB position
#define NXMAC_START_TX_ANTENNA_SET_LSB    16
/// START_TX_ANTENNA_SET field width
#define NXMAC_START_TX_ANTENNA_SET_WIDTH  ((uint32_t)0x00000008)
/// START_TX_LSTP field bit
#define NXMAC_START_TX_LSTP_BIT           ((uint32_t)0x00002000)
/// START_TX_LSTP field position
#define NXMAC_START_TX_LSTP_POS           13
/// START_TX_SMOOTHING field bit
#define NXMAC_START_TX_SMOOTHING_BIT      ((uint32_t)0x00001000)
/// START_TX_SMOOTHING field position
#define NXMAC_START_TX_SMOOTHING_POS      12
/// START_TX_SHORT_GI field bit
#define NXMAC_START_TX_SHORT_GI_BIT       ((uint32_t)0x00000100)
/// START_TX_SHORT_GI field position
#define NXMAC_START_TX_SHORT_GI_POS       8
/// START_TX_N_TX field mask
#define NXMAC_START_TX_N_TX_MASK          ((uint32_t)0x000000E0)
/// START_TX_N_TX field LSB position
#define NXMAC_START_TX_N_TX_LSB           5
/// START_TX_N_TX field width
#define NXMAC_START_TX_N_TX_WIDTH         ((uint32_t)0x00000003)
/// START_TX_FEC_CODING field bit
#define NXMAC_START_TX_FEC_CODING_BIT     ((uint32_t)0x00000010)
/// START_TX_FEC_CODING field position
#define NXMAC_START_TX_FEC_CODING_POS     4
/// START_TX_STBC field mask
#define NXMAC_START_TX_STBC_MASK          ((uint32_t)0x0000000C)
/// START_TX_STBC field LSB position
#define NXMAC_START_TX_STBC_LSB           2
/// START_TX_STBC field width
#define NXMAC_START_TX_STBC_WIDTH         ((uint32_t)0x00000002)
/// START_TX_NUM_EXTN_SS field mask
#define NXMAC_START_TX_NUM_EXTN_SS_MASK   ((uint32_t)0x00000003)
/// START_TX_NUM_EXTN_SS field LSB position
#define NXMAC_START_TX_NUM_EXTN_SS_LSB    0
/// START_TX_NUM_EXTN_SS field width
#define NXMAC_START_TX_NUM_EXTN_SS_WIDTH  ((uint32_t)0x00000002)

/// START_TX_SMM_INDEX field reset value
#define NXMAC_START_TX_SMM_INDEX_RST      0x0
/// START_TX_ANTENNA_SET field reset value
#define NXMAC_START_TX_ANTENNA_SET_RST    0x0
/// START_TX_LSTP field reset value
#define NXMAC_START_TX_LSTP_RST           0x0
/// START_TX_SMOOTHING field reset value
#define NXMAC_START_TX_SMOOTHING_RST      0x0
/// START_TX_SHORT_GI field reset value
#define NXMAC_START_TX_SHORT_GI_RST       0x0
/// START_TX_N_TX field reset value
#define NXMAC_START_TX_N_TX_RST           0x0
/// START_TX_FEC_CODING field reset value
#define NXMAC_START_TX_FEC_CODING_RST     0x0
/// START_TX_STBC field reset value
#define NXMAC_START_TX_STBC_RST           0x0
/// START_TX_NUM_EXTN_SS field reset value
#define NXMAC_START_TX_NUM_EXTN_SS_RST    0x0

/**
 * @brief Constructs a value for the START_TX_3 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] starttxsmmindex - The value to use for the startTxSMMIndex field.
 * @param[in] starttxantennaset - The value to use for the startTxAntennaSet field.
 * @param[in] starttxlstp - The value to use for the startTxLSTP field.
 * @param[in] starttxsmoothing - The value to use for the startTxSmoothing field.
 * @param[in] starttxshortgi - The value to use for the startTxShortGI field.
 * @param[in] starttxntx - The value to use for the startTxNTx field.
 * @param[in] starttxfeccoding - The value to use for the startTxFecCoding field.
 * @param[in] starttxstbc - The value to use for the startTxSTBC field.
 * @param[in] starttxnumextnss - The value to use for the startTxNumExtnSS field.
 */
__INLINE void nxmac_start_tx_3_pack(uint8_t starttxsmmindex, uint8_t starttxantennaset, uint8_t starttxlstp, uint8_t starttxsmoothing, uint8_t starttxshortgi, uint8_t starttxntx, uint8_t starttxfeccoding, uint8_t starttxstbc, uint8_t starttxnumextnss)
{
    ASSERT_ERR((((uint32_t)starttxsmmindex << 24) & ~((uint32_t)0xFF000000)) == 0);
    ASSERT_ERR((((uint32_t)starttxantennaset << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)starttxlstp << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)starttxsmoothing << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)starttxshortgi << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)starttxntx << 5) & ~((uint32_t)0x000000E0)) == 0);
    ASSERT_ERR((((uint32_t)starttxfeccoding << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)starttxstbc << 2) & ~((uint32_t)0x0000000C)) == 0);
    ASSERT_ERR((((uint32_t)starttxnumextnss << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(NXMAC_START_TX_3_ADDR,  ((uint32_t)starttxsmmindex << 24) | ((uint32_t)starttxantennaset << 16) | ((uint32_t)starttxlstp << 13) | ((uint32_t)starttxsmoothing << 12) | ((uint32_t)starttxshortgi << 8) | ((uint32_t)starttxntx << 5) | ((uint32_t)starttxfeccoding << 4) | ((uint32_t)starttxstbc << 2) | ((uint32_t)starttxnumextnss << 0));
}

/**
 * @brief Unpacks START_TX_3's fields from current value of the START_TX_3 register.
 *
 * Reads the START_TX_3 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] starttxsmmindex - Will be populated with the current value of this field from the register.
 * @param[out] starttxantennaset - Will be populated with the current value of this field from the register.
 * @param[out] starttxlstp - Will be populated with the current value of this field from the register.
 * @param[out] starttxsmoothing - Will be populated with the current value of this field from the register.
 * @param[out] starttxshortgi - Will be populated with the current value of this field from the register.
 * @param[out] starttxntx - Will be populated with the current value of this field from the register.
 * @param[out] starttxfeccoding - Will be populated with the current value of this field from the register.
 * @param[out] starttxstbc - Will be populated with the current value of this field from the register.
 * @param[out] starttxnumextnss - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_start_tx_3_unpack(uint8_t *starttxsmmindex, uint8_t *starttxantennaset, uint8_t *starttxlstp, uint8_t *starttxsmoothing, uint8_t *starttxshortgi, uint8_t *starttxntx, uint8_t *starttxfeccoding, uint8_t *starttxstbc, uint8_t *starttxnumextnss)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_3_ADDR);

    *starttxsmmindex = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *starttxantennaset = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *starttxlstp = (localVal & ((uint32_t)0x00002000)) >> 13;
    *starttxsmoothing = (localVal & ((uint32_t)0x00001000)) >> 12;
    *starttxshortgi = (localVal & ((uint32_t)0x00000100)) >> 8;
    *starttxntx = (localVal & ((uint32_t)0x000000E0)) >> 5;
    *starttxfeccoding = (localVal & ((uint32_t)0x00000010)) >> 4;
    *starttxstbc = (localVal & ((uint32_t)0x0000000C)) >> 2;
    *starttxnumextnss = (localVal & ((uint32_t)0x00000003)) >> 0;
}

/**
 * @brief Returns the current value of the startTxSMMIndex field in the START_TX_3 register.
 *
 * The START_TX_3 register will be read and the startTxSMMIndex field's value will be returned.
 *
 * @return The current value of the startTxSMMIndex field in the START_TX_3 register.
 */
__INLINE uint8_t nxmac_start_tx_smm_index_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_3_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Sets the startTxSMMIndex field of the START_TX_3 register.
 *
 * The START_TX_3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] starttxsmmindex - The value to set the field to.
 */
__INLINE void nxmac_start_tx_smm_index_setf(uint8_t starttxsmmindex)
{
    ASSERT_ERR((((uint32_t)starttxsmmindex << 24) & ~((uint32_t)0xFF000000)) == 0);
    REG_PL_WR(NXMAC_START_TX_3_ADDR, (REG_PL_RD(NXMAC_START_TX_3_ADDR) & ~((uint32_t)0xFF000000)) | ((uint32_t)starttxsmmindex << 24));
}

/**
 * @brief Returns the current value of the startTxAntennaSet field in the START_TX_3 register.
 *
 * The START_TX_3 register will be read and the startTxAntennaSet field's value will be returned.
 *
 * @return The current value of the startTxAntennaSet field in the START_TX_3 register.
 */
__INLINE uint8_t nxmac_start_tx_antenna_set_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_3_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the startTxAntennaSet field of the START_TX_3 register.
 *
 * The START_TX_3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] starttxantennaset - The value to set the field to.
 */
__INLINE void nxmac_start_tx_antenna_set_setf(uint8_t starttxantennaset)
{
    ASSERT_ERR((((uint32_t)starttxantennaset << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(NXMAC_START_TX_3_ADDR, (REG_PL_RD(NXMAC_START_TX_3_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)starttxantennaset << 16));
}

/**
 * @brief Returns the current value of the startTxLSTP field in the START_TX_3 register.
 *
 * The START_TX_3 register will be read and the startTxLSTP field's value will be returned.
 *
 * @return The current value of the startTxLSTP field in the START_TX_3 register.
 */
__INLINE uint8_t nxmac_start_tx_lstp_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_3_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the startTxLSTP field of the START_TX_3 register.
 *
 * The START_TX_3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] starttxlstp - The value to set the field to.
 */
__INLINE void nxmac_start_tx_lstp_setf(uint8_t starttxlstp)
{
    ASSERT_ERR((((uint32_t)starttxlstp << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(NXMAC_START_TX_3_ADDR, (REG_PL_RD(NXMAC_START_TX_3_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)starttxlstp << 13));
}

/**
 * @brief Returns the current value of the startTxSmoothing field in the START_TX_3 register.
 *
 * The START_TX_3 register will be read and the startTxSmoothing field's value will be returned.
 *
 * @return The current value of the startTxSmoothing field in the START_TX_3 register.
 */
__INLINE uint8_t nxmac_start_tx_smoothing_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_3_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the startTxSmoothing field of the START_TX_3 register.
 *
 * The START_TX_3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] starttxsmoothing - The value to set the field to.
 */
__INLINE void nxmac_start_tx_smoothing_setf(uint8_t starttxsmoothing)
{
    ASSERT_ERR((((uint32_t)starttxsmoothing << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(NXMAC_START_TX_3_ADDR, (REG_PL_RD(NXMAC_START_TX_3_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)starttxsmoothing << 12));
}

/**
 * @brief Returns the current value of the startTxShortGI field in the START_TX_3 register.
 *
 * The START_TX_3 register will be read and the startTxShortGI field's value will be returned.
 *
 * @return The current value of the startTxShortGI field in the START_TX_3 register.
 */
__INLINE uint8_t nxmac_start_tx_short_gi_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_3_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the startTxShortGI field of the START_TX_3 register.
 *
 * The START_TX_3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] starttxshortgi - The value to set the field to.
 */
__INLINE void nxmac_start_tx_short_gi_setf(uint8_t starttxshortgi)
{
    ASSERT_ERR((((uint32_t)starttxshortgi << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(NXMAC_START_TX_3_ADDR, (REG_PL_RD(NXMAC_START_TX_3_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)starttxshortgi << 8));
}

/**
 * @brief Returns the current value of the startTxNTx field in the START_TX_3 register.
 *
 * The START_TX_3 register will be read and the startTxNTx field's value will be returned.
 *
 * @return The current value of the startTxNTx field in the START_TX_3 register.
 */
__INLINE uint8_t nxmac_start_tx_n_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_3_ADDR);
    return ((localVal & ((uint32_t)0x000000E0)) >> 5);
}

/**
 * @brief Sets the startTxNTx field of the START_TX_3 register.
 *
 * The START_TX_3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] starttxntx - The value to set the field to.
 */
__INLINE void nxmac_start_tx_n_tx_setf(uint8_t starttxntx)
{
    ASSERT_ERR((((uint32_t)starttxntx << 5) & ~((uint32_t)0x000000E0)) == 0);
    REG_PL_WR(NXMAC_START_TX_3_ADDR, (REG_PL_RD(NXMAC_START_TX_3_ADDR) & ~((uint32_t)0x000000E0)) | ((uint32_t)starttxntx << 5));
}

/**
 * @brief Returns the current value of the startTxFecCoding field in the START_TX_3 register.
 *
 * The START_TX_3 register will be read and the startTxFecCoding field's value will be returned.
 *
 * @return The current value of the startTxFecCoding field in the START_TX_3 register.
 */
__INLINE uint8_t nxmac_start_tx_fec_coding_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_3_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the startTxFecCoding field of the START_TX_3 register.
 *
 * The START_TX_3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] starttxfeccoding - The value to set the field to.
 */
__INLINE void nxmac_start_tx_fec_coding_setf(uint8_t starttxfeccoding)
{
    ASSERT_ERR((((uint32_t)starttxfeccoding << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(NXMAC_START_TX_3_ADDR, (REG_PL_RD(NXMAC_START_TX_3_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)starttxfeccoding << 4));
}

/**
 * @brief Returns the current value of the startTxSTBC field in the START_TX_3 register.
 *
 * The START_TX_3 register will be read and the startTxSTBC field's value will be returned.
 *
 * @return The current value of the startTxSTBC field in the START_TX_3 register.
 */
__INLINE uint8_t nxmac_start_tx_stbc_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_3_ADDR);
    return ((localVal & ((uint32_t)0x0000000C)) >> 2);
}

/**
 * @brief Sets the startTxSTBC field of the START_TX_3 register.
 *
 * The START_TX_3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] starttxstbc - The value to set the field to.
 */
__INLINE void nxmac_start_tx_stbc_setf(uint8_t starttxstbc)
{
    ASSERT_ERR((((uint32_t)starttxstbc << 2) & ~((uint32_t)0x0000000C)) == 0);
    REG_PL_WR(NXMAC_START_TX_3_ADDR, (REG_PL_RD(NXMAC_START_TX_3_ADDR) & ~((uint32_t)0x0000000C)) | ((uint32_t)starttxstbc << 2));
}

/**
 * @brief Returns the current value of the startTxNumExtnSS field in the START_TX_3 register.
 *
 * The START_TX_3 register will be read and the startTxNumExtnSS field's value will be returned.
 *
 * @return The current value of the startTxNumExtnSS field in the START_TX_3 register.
 */
__INLINE uint8_t nxmac_start_tx_num_extn_ss_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_START_TX_3_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

/**
 * @brief Sets the startTxNumExtnSS field of the START_TX_3 register.
 *
 * The START_TX_3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] starttxnumextnss - The value to set the field to.
 */
__INLINE void nxmac_start_tx_num_extn_ss_setf(uint8_t starttxnumextnss)
{
    ASSERT_ERR((((uint32_t)starttxnumextnss << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(NXMAC_START_TX_3_ADDR, (REG_PL_RD(NXMAC_START_TX_3_ADDR) & ~((uint32_t)0x00000003)) | ((uint32_t)starttxnumextnss << 0));
}

/// @}

/**
 * @name TX_BW_CNTRL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  17:16       maxSupportedBW   0x2
 *  15:08         aPPDUMaxTime   0xA
 *     07              dynBWEn   0
 *  06:04   numTryBWAcquisition   0x1
 *     03        dropToLowerBW   1
 *  02:01        defaultBWTXOP   0x0
 *     00       defaultBWTXOPV   0
 * </pre>
 *
 * @{
 */

/// Address of the TX_BW_CNTRL register
#define NXMAC_TX_BW_CNTRL_ADDR   0xC0000310
/// Offset of the TX_BW_CNTRL register from the base address
#define NXMAC_TX_BW_CNTRL_OFFSET 0x00000310
/// Index of the TX_BW_CNTRL register
#define NXMAC_TX_BW_CNTRL_INDEX  0x000000C4
/// Reset value of the TX_BW_CNTRL register
#define NXMAC_TX_BW_CNTRL_RESET  0x00020A18

/**
 * @brief Returns the current value of the TX_BW_CNTRL register.
 * The TX_BW_CNTRL register will be read and its value returned.
 * @return The current value of the TX_BW_CNTRL register.
 */
__INLINE uint32_t nxmac_tx_bw_cntrl_get(void)
{
    return REG_PL_RD(NXMAC_TX_BW_CNTRL_ADDR);
}

/**
 * @brief Sets the TX_BW_CNTRL register to a value.
 * The TX_BW_CNTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_tx_bw_cntrl_set(uint32_t value)
{
    REG_PL_WR(NXMAC_TX_BW_CNTRL_ADDR, value);
}

// field definitions
/// MAX_SUPPORTED_BW field mask
#define NXMAC_MAX_SUPPORTED_BW_MASK         ((uint32_t)0x00030000)
/// MAX_SUPPORTED_BW field LSB position
#define NXMAC_MAX_SUPPORTED_BW_LSB          16
/// MAX_SUPPORTED_BW field width
#define NXMAC_MAX_SUPPORTED_BW_WIDTH        ((uint32_t)0x00000002)
/// A_PPDU_MAX_TIME field mask
#define NXMAC_A_PPDU_MAX_TIME_MASK          ((uint32_t)0x0000FF00)
/// A_PPDU_MAX_TIME field LSB position
#define NXMAC_A_PPDU_MAX_TIME_LSB           8
/// A_PPDU_MAX_TIME field width
#define NXMAC_A_PPDU_MAX_TIME_WIDTH         ((uint32_t)0x00000008)
/// DYN_BW_EN field bit
#define NXMAC_DYN_BW_EN_BIT                 ((uint32_t)0x00000080)
/// DYN_BW_EN field position
#define NXMAC_DYN_BW_EN_POS                 7
/// NUM_TRY_BW_ACQUISITION field mask
#define NXMAC_NUM_TRY_BW_ACQUISITION_MASK   ((uint32_t)0x00000070)
/// NUM_TRY_BW_ACQUISITION field LSB position
#define NXMAC_NUM_TRY_BW_ACQUISITION_LSB    4
/// NUM_TRY_BW_ACQUISITION field width
#define NXMAC_NUM_TRY_BW_ACQUISITION_WIDTH  ((uint32_t)0x00000003)
/// DROP_TO_LOWER_BW field bit
#define NXMAC_DROP_TO_LOWER_BW_BIT          ((uint32_t)0x00000008)
/// DROP_TO_LOWER_BW field position
#define NXMAC_DROP_TO_LOWER_BW_POS          3
/// DEFAULT_BWTXOP field mask
#define NXMAC_DEFAULT_BWTXOP_MASK           ((uint32_t)0x00000006)
/// DEFAULT_BWTXOP field LSB position
#define NXMAC_DEFAULT_BWTXOP_LSB            1
/// DEFAULT_BWTXOP field width
#define NXMAC_DEFAULT_BWTXOP_WIDTH          ((uint32_t)0x00000002)
/// DEFAULT_BWTXOPV field bit
#define NXMAC_DEFAULT_BWTXOPV_BIT           ((uint32_t)0x00000001)
/// DEFAULT_BWTXOPV field position
#define NXMAC_DEFAULT_BWTXOPV_POS           0

/// MAX_SUPPORTED_BW field reset value
#define NXMAC_MAX_SUPPORTED_BW_RST          0x2
/// A_PPDU_MAX_TIME field reset value
#define NXMAC_A_PPDU_MAX_TIME_RST           0xA
/// DYN_BW_EN field reset value
#define NXMAC_DYN_BW_EN_RST                 0x0
/// NUM_TRY_BW_ACQUISITION field reset value
#define NXMAC_NUM_TRY_BW_ACQUISITION_RST    0x1
/// DROP_TO_LOWER_BW field reset value
#define NXMAC_DROP_TO_LOWER_BW_RST          0x1
/// DEFAULT_BWTXOP field reset value
#define NXMAC_DEFAULT_BWTXOP_RST            0x0
/// DEFAULT_BWTXOPV field reset value
#define NXMAC_DEFAULT_BWTXOPV_RST           0x0

/**
 * @brief Constructs a value for the TX_BW_CNTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] maxsupportedbw - The value to use for the maxSupportedBW field.
 * @param[in] appdumaxtime - The value to use for the aPPDUMaxTime field.
 * @param[in] dynbwen - The value to use for the dynBWEn field.
 * @param[in] numtrybwacquisition - The value to use for the numTryBWAcquisition field.
 * @param[in] droptolowerbw - The value to use for the dropToLowerBW field.
 * @param[in] defaultbwtxop - The value to use for the defaultBWTXOP field.
 * @param[in] defaultbwtxopv - The value to use for the defaultBWTXOPV field.
 */
__INLINE void nxmac_tx_bw_cntrl_pack(uint8_t maxsupportedbw, uint8_t appdumaxtime, uint8_t dynbwen, uint8_t numtrybwacquisition, uint8_t droptolowerbw, uint8_t defaultbwtxop, uint8_t defaultbwtxopv)
{
    ASSERT_ERR((((uint32_t)maxsupportedbw << 16) & ~((uint32_t)0x00030000)) == 0);
    ASSERT_ERR((((uint32_t)appdumaxtime << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)dynbwen << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)numtrybwacquisition << 4) & ~((uint32_t)0x00000070)) == 0);
    ASSERT_ERR((((uint32_t)droptolowerbw << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)defaultbwtxop << 1) & ~((uint32_t)0x00000006)) == 0);
    ASSERT_ERR((((uint32_t)defaultbwtxopv << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_TX_BW_CNTRL_ADDR,  ((uint32_t)maxsupportedbw << 16) | ((uint32_t)appdumaxtime << 8) | ((uint32_t)dynbwen << 7) | ((uint32_t)numtrybwacquisition << 4) | ((uint32_t)droptolowerbw << 3) | ((uint32_t)defaultbwtxop << 1) | ((uint32_t)defaultbwtxopv << 0));
}

/**
 * @brief Unpacks TX_BW_CNTRL's fields from current value of the TX_BW_CNTRL register.
 *
 * Reads the TX_BW_CNTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] maxsupportedbw - Will be populated with the current value of this field from the register.
 * @param[out] appdumaxtime - Will be populated with the current value of this field from the register.
 * @param[out] dynbwen - Will be populated with the current value of this field from the register.
 * @param[out] numtrybwacquisition - Will be populated with the current value of this field from the register.
 * @param[out] droptolowerbw - Will be populated with the current value of this field from the register.
 * @param[out] defaultbwtxop - Will be populated with the current value of this field from the register.
 * @param[out] defaultbwtxopv - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_tx_bw_cntrl_unpack(uint8_t *maxsupportedbw, uint8_t *appdumaxtime, uint8_t *dynbwen, uint8_t *numtrybwacquisition, uint8_t *droptolowerbw, uint8_t *defaultbwtxop, uint8_t *defaultbwtxopv)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_BW_CNTRL_ADDR);

    *maxsupportedbw = (localVal & ((uint32_t)0x00030000)) >> 16;
    *appdumaxtime = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *dynbwen = (localVal & ((uint32_t)0x00000080)) >> 7;
    *numtrybwacquisition = (localVal & ((uint32_t)0x00000070)) >> 4;
    *droptolowerbw = (localVal & ((uint32_t)0x00000008)) >> 3;
    *defaultbwtxop = (localVal & ((uint32_t)0x00000006)) >> 1;
    *defaultbwtxopv = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the maxSupportedBW field in the TX_BW_CNTRL register.
 *
 * The TX_BW_CNTRL register will be read and the maxSupportedBW field's value will be returned.
 *
 * @return The current value of the maxSupportedBW field in the TX_BW_CNTRL register.
 */
__INLINE uint8_t nxmac_max_supported_bw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_BW_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

/**
 * @brief Sets the maxSupportedBW field of the TX_BW_CNTRL register.
 *
 * The TX_BW_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maxsupportedbw - The value to set the field to.
 */
__INLINE void nxmac_max_supported_bw_setf(uint8_t maxsupportedbw)
{
    ASSERT_ERR((((uint32_t)maxsupportedbw << 16) & ~((uint32_t)0x00030000)) == 0);
    REG_PL_WR(NXMAC_TX_BW_CNTRL_ADDR, (REG_PL_RD(NXMAC_TX_BW_CNTRL_ADDR) & ~((uint32_t)0x00030000)) | ((uint32_t)maxsupportedbw << 16));
}

/**
 * @brief Returns the current value of the aPPDUMaxTime field in the TX_BW_CNTRL register.
 *
 * The TX_BW_CNTRL register will be read and the aPPDUMaxTime field's value will be returned.
 *
 * @return The current value of the aPPDUMaxTime field in the TX_BW_CNTRL register.
 */
__INLINE uint8_t nxmac_a_ppdu_max_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_BW_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the aPPDUMaxTime field of the TX_BW_CNTRL register.
 *
 * The TX_BW_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] appdumaxtime - The value to set the field to.
 */
__INLINE void nxmac_a_ppdu_max_time_setf(uint8_t appdumaxtime)
{
    ASSERT_ERR((((uint32_t)appdumaxtime << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(NXMAC_TX_BW_CNTRL_ADDR, (REG_PL_RD(NXMAC_TX_BW_CNTRL_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)appdumaxtime << 8));
}

/**
 * @brief Returns the current value of the dynBWEn field in the TX_BW_CNTRL register.
 *
 * The TX_BW_CNTRL register will be read and the dynBWEn field's value will be returned.
 *
 * @return The current value of the dynBWEn field in the TX_BW_CNTRL register.
 */
__INLINE uint8_t nxmac_dyn_bw_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_BW_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the dynBWEn field of the TX_BW_CNTRL register.
 *
 * The TX_BW_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dynbwen - The value to set the field to.
 */
__INLINE void nxmac_dyn_bw_en_setf(uint8_t dynbwen)
{
    ASSERT_ERR((((uint32_t)dynbwen << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(NXMAC_TX_BW_CNTRL_ADDR, (REG_PL_RD(NXMAC_TX_BW_CNTRL_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)dynbwen << 7));
}

/**
 * @brief Returns the current value of the numTryBWAcquisition field in the TX_BW_CNTRL register.
 *
 * The TX_BW_CNTRL register will be read and the numTryBWAcquisition field's value will be returned.
 *
 * @return The current value of the numTryBWAcquisition field in the TX_BW_CNTRL register.
 */
__INLINE uint8_t nxmac_num_try_bw_acquisition_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_BW_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

/**
 * @brief Sets the numTryBWAcquisition field of the TX_BW_CNTRL register.
 *
 * The TX_BW_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] numtrybwacquisition - The value to set the field to.
 */
__INLINE void nxmac_num_try_bw_acquisition_setf(uint8_t numtrybwacquisition)
{
    ASSERT_ERR((((uint32_t)numtrybwacquisition << 4) & ~((uint32_t)0x00000070)) == 0);
    REG_PL_WR(NXMAC_TX_BW_CNTRL_ADDR, (REG_PL_RD(NXMAC_TX_BW_CNTRL_ADDR) & ~((uint32_t)0x00000070)) | ((uint32_t)numtrybwacquisition << 4));
}

/**
 * @brief Returns the current value of the dropToLowerBW field in the TX_BW_CNTRL register.
 *
 * The TX_BW_CNTRL register will be read and the dropToLowerBW field's value will be returned.
 *
 * @return The current value of the dropToLowerBW field in the TX_BW_CNTRL register.
 */
__INLINE uint8_t nxmac_drop_to_lower_bw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_BW_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the dropToLowerBW field of the TX_BW_CNTRL register.
 *
 * The TX_BW_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] droptolowerbw - The value to set the field to.
 */
__INLINE void nxmac_drop_to_lower_bw_setf(uint8_t droptolowerbw)
{
    ASSERT_ERR((((uint32_t)droptolowerbw << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_TX_BW_CNTRL_ADDR, (REG_PL_RD(NXMAC_TX_BW_CNTRL_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)droptolowerbw << 3));
}

/**
 * @brief Returns the current value of the defaultBWTXOP field in the TX_BW_CNTRL register.
 *
 * The TX_BW_CNTRL register will be read and the defaultBWTXOP field's value will be returned.
 *
 * @return The current value of the defaultBWTXOP field in the TX_BW_CNTRL register.
 */
__INLINE uint8_t nxmac_default_bwtxop_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_BW_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000006)) >> 1);
}

/**
 * @brief Sets the defaultBWTXOP field of the TX_BW_CNTRL register.
 *
 * The TX_BW_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] defaultbwtxop - The value to set the field to.
 */
__INLINE void nxmac_default_bwtxop_setf(uint8_t defaultbwtxop)
{
    ASSERT_ERR((((uint32_t)defaultbwtxop << 1) & ~((uint32_t)0x00000006)) == 0);
    REG_PL_WR(NXMAC_TX_BW_CNTRL_ADDR, (REG_PL_RD(NXMAC_TX_BW_CNTRL_ADDR) & ~((uint32_t)0x00000006)) | ((uint32_t)defaultbwtxop << 1));
}

/**
 * @brief Returns the current value of the defaultBWTXOPV field in the TX_BW_CNTRL register.
 *
 * The TX_BW_CNTRL register will be read and the defaultBWTXOPV field's value will be returned.
 *
 * @return The current value of the defaultBWTXOPV field in the TX_BW_CNTRL register.
 */
__INLINE uint8_t nxmac_default_bwtxopv_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_TX_BW_CNTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the defaultBWTXOPV field of the TX_BW_CNTRL register.
 *
 * The TX_BW_CNTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] defaultbwtxopv - The value to set the field to.
 */
__INLINE void nxmac_default_bwtxopv_setf(uint8_t defaultbwtxopv)
{
    ASSERT_ERR((((uint32_t)defaultbwtxopv << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_TX_BW_CNTRL_ADDR, (REG_PL_RD(NXMAC_TX_BW_CNTRL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)defaultbwtxopv << 0));
}

/// @}

/**
 * @name HTMCS register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  21:16   bssBasicHTMCSSetUM   0x0
 *  15:00   bssBasicHTMCSSetEM   0xFFFF
 * </pre>
 *
 * @{
 */

/// Address of the HTMCS register
#define NXMAC_HTMCS_ADDR   0xC0000314
/// Offset of the HTMCS register from the base address
#define NXMAC_HTMCS_OFFSET 0x00000314
/// Index of the HTMCS register
#define NXMAC_HTMCS_INDEX  0x000000C5
/// Reset value of the HTMCS register
#define NXMAC_HTMCS_RESET  0x0000FFFF

/**
 * @brief Returns the current value of the HTMCS register.
 * The HTMCS register will be read and its value returned.
 * @return The current value of the HTMCS register.
 */
__INLINE uint32_t nxmac_htmcs_get(void)
{
    return REG_PL_RD(NXMAC_HTMCS_ADDR);
}

/**
 * @brief Sets the HTMCS register to a value.
 * The HTMCS register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_htmcs_set(uint32_t value)
{
    REG_PL_WR(NXMAC_HTMCS_ADDR, value);
}

// field definitions
/// BSS_BASIC_HTMCS_SET_UM field mask
#define NXMAC_BSS_BASIC_HTMCS_SET_UM_MASK   ((uint32_t)0x003F0000)
/// BSS_BASIC_HTMCS_SET_UM field LSB position
#define NXMAC_BSS_BASIC_HTMCS_SET_UM_LSB    16
/// BSS_BASIC_HTMCS_SET_UM field width
#define NXMAC_BSS_BASIC_HTMCS_SET_UM_WIDTH  ((uint32_t)0x00000006)
/// BSS_BASIC_HTMCS_SET_EM field mask
#define NXMAC_BSS_BASIC_HTMCS_SET_EM_MASK   ((uint32_t)0x0000FFFF)
/// BSS_BASIC_HTMCS_SET_EM field LSB position
#define NXMAC_BSS_BASIC_HTMCS_SET_EM_LSB    0
/// BSS_BASIC_HTMCS_SET_EM field width
#define NXMAC_BSS_BASIC_HTMCS_SET_EM_WIDTH  ((uint32_t)0x00000010)

/// BSS_BASIC_HTMCS_SET_UM field reset value
#define NXMAC_BSS_BASIC_HTMCS_SET_UM_RST    0x0
/// BSS_BASIC_HTMCS_SET_EM field reset value
#define NXMAC_BSS_BASIC_HTMCS_SET_EM_RST    0xFFFF

/**
 * @brief Constructs a value for the HTMCS register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] bssbasichtmcssetum - The value to use for the bssBasicHTMCSSetUM field.
 * @param[in] bssbasichtmcssetem - The value to use for the bssBasicHTMCSSetEM field.
 */
__INLINE void nxmac_htmcs_pack(uint8_t bssbasichtmcssetum, uint16_t bssbasichtmcssetem)
{
    ASSERT_ERR((((uint32_t)bssbasichtmcssetum << 16) & ~((uint32_t)0x003F0000)) == 0);
    ASSERT_ERR((((uint32_t)bssbasichtmcssetem << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_HTMCS_ADDR,  ((uint32_t)bssbasichtmcssetum << 16) | ((uint32_t)bssbasichtmcssetem << 0));
}

/**
 * @brief Unpacks HTMCS's fields from current value of the HTMCS register.
 *
 * Reads the HTMCS register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] bssbasichtmcssetum - Will be populated with the current value of this field from the register.
 * @param[out] bssbasichtmcssetem - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_htmcs_unpack(uint8_t *bssbasichtmcssetum, uint16_t *bssbasichtmcssetem)
{
    uint32_t localVal = REG_PL_RD(NXMAC_HTMCS_ADDR);

    *bssbasichtmcssetum = (localVal & ((uint32_t)0x003F0000)) >> 16;
    *bssbasichtmcssetem = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the bssBasicHTMCSSetUM field in the HTMCS register.
 *
 * The HTMCS register will be read and the bssBasicHTMCSSetUM field's value will be returned.
 *
 * @return The current value of the bssBasicHTMCSSetUM field in the HTMCS register.
 */
__INLINE uint8_t nxmac_bss_basic_htmcs_set_um_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_HTMCS_ADDR);
    return ((localVal & ((uint32_t)0x003F0000)) >> 16);
}

/**
 * @brief Sets the bssBasicHTMCSSetUM field of the HTMCS register.
 *
 * The HTMCS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bssbasichtmcssetum - The value to set the field to.
 */
__INLINE void nxmac_bss_basic_htmcs_set_um_setf(uint8_t bssbasichtmcssetum)
{
    ASSERT_ERR((((uint32_t)bssbasichtmcssetum << 16) & ~((uint32_t)0x003F0000)) == 0);
    REG_PL_WR(NXMAC_HTMCS_ADDR, (REG_PL_RD(NXMAC_HTMCS_ADDR) & ~((uint32_t)0x003F0000)) | ((uint32_t)bssbasichtmcssetum << 16));
}

/**
 * @brief Returns the current value of the bssBasicHTMCSSetEM field in the HTMCS register.
 *
 * The HTMCS register will be read and the bssBasicHTMCSSetEM field's value will be returned.
 *
 * @return The current value of the bssBasicHTMCSSetEM field in the HTMCS register.
 */
__INLINE uint16_t nxmac_bss_basic_htmcs_set_em_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_HTMCS_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the bssBasicHTMCSSetEM field of the HTMCS register.
 *
 * The HTMCS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bssbasichtmcssetem - The value to set the field to.
 */
__INLINE void nxmac_bss_basic_htmcs_set_em_setf(uint16_t bssbasichtmcssetem)
{
    ASSERT_ERR((((uint32_t)bssbasichtmcssetem << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_HTMCS_ADDR, (REG_PL_RD(NXMAC_HTMCS_ADDR) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)bssbasichtmcssetem << 0));
}

/// @}

/**
 * @name VHTMCS register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00    bssBasicVHTMCSSet   0xFFFF
 * </pre>
 *
 * @{
 */

/// Address of the VHTMCS register
#define NXMAC_VHTMCS_ADDR   0xC000031C
/// Offset of the VHTMCS register from the base address
#define NXMAC_VHTMCS_OFFSET 0x0000031C
/// Index of the VHTMCS register
#define NXMAC_VHTMCS_INDEX  0x000000C7
/// Reset value of the VHTMCS register
#define NXMAC_VHTMCS_RESET  0x0000FFFF

/**
 * @brief Returns the current value of the VHTMCS register.
 * The VHTMCS register will be read and its value returned.
 * @return The current value of the VHTMCS register.
 */
__INLINE uint32_t nxmac_vhtmcs_get(void)
{
    return REG_PL_RD(NXMAC_VHTMCS_ADDR);
}

/**
 * @brief Sets the VHTMCS register to a value.
 * The VHTMCS register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_vhtmcs_set(uint32_t value)
{
    REG_PL_WR(NXMAC_VHTMCS_ADDR, value);
}

// field definitions
/// BSS_BASIC_VHTMCS_SET field mask
#define NXMAC_BSS_BASIC_VHTMCS_SET_MASK   ((uint32_t)0x0000FFFF)
/// BSS_BASIC_VHTMCS_SET field LSB position
#define NXMAC_BSS_BASIC_VHTMCS_SET_LSB    0
/// BSS_BASIC_VHTMCS_SET field width
#define NXMAC_BSS_BASIC_VHTMCS_SET_WIDTH  ((uint32_t)0x00000010)

/// BSS_BASIC_VHTMCS_SET field reset value
#define NXMAC_BSS_BASIC_VHTMCS_SET_RST    0xFFFF

/**
 * @brief Returns the current value of the bssBasicVHTMCSSet field in the VHTMCS register.
 *
 * The VHTMCS register will be read and the bssBasicVHTMCSSet field's value will be returned.
 *
 * @return The current value of the bssBasicVHTMCSSet field in the VHTMCS register.
 */
__INLINE uint16_t nxmac_bss_basic_vhtmcs_set_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_VHTMCS_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the bssBasicVHTMCSSet field of the VHTMCS register.
 *
 * The VHTMCS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bssbasicvhtmcsset - The value to set the field to.
 */
__INLINE void nxmac_bss_basic_vhtmcs_set_setf(uint16_t bssbasicvhtmcsset)
{
    ASSERT_ERR((((uint32_t)bssbasicvhtmcsset << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(NXMAC_VHTMCS_ADDR, (uint32_t)bssbasicvhtmcsset << 0);
}

/// @}

/**
 * @name LSTP register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     00          supportLSTP   0
 * </pre>
 *
 * @{
 */

/// Address of the LSTP register
#define NXMAC_LSTP_ADDR   0xC0000320
/// Offset of the LSTP register from the base address
#define NXMAC_LSTP_OFFSET 0x00000320
/// Index of the LSTP register
#define NXMAC_LSTP_INDEX  0x000000C8
/// Reset value of the LSTP register
#define NXMAC_LSTP_RESET  0x00000000

/**
 * @brief Returns the current value of the LSTP register.
 * The LSTP register will be read and its value returned.
 * @return The current value of the LSTP register.
 */
__INLINE uint32_t nxmac_lstp_get(void)
{
    return REG_PL_RD(NXMAC_LSTP_ADDR);
}

/**
 * @brief Sets the LSTP register to a value.
 * The LSTP register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_lstp_set(uint32_t value)
{
    REG_PL_WR(NXMAC_LSTP_ADDR, value);
}

// field definitions
/// SUPPORT_LSTP field bit
#define NXMAC_SUPPORT_LSTP_BIT    ((uint32_t)0x00000001)
/// SUPPORT_LSTP field position
#define NXMAC_SUPPORT_LSTP_POS    0

/// SUPPORT_LSTP field reset value
#define NXMAC_SUPPORT_LSTP_RST    0x0

/**
 * @brief Returns the current value of the supportLSTP field in the LSTP register.
 *
 * The LSTP register will be read and the supportLSTP field's value will be returned.
 *
 * @return The current value of the supportLSTP field in the LSTP register.
 */
__INLINE uint8_t nxmac_support_lstp_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_LSTP_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000001)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the supportLSTP field of the LSTP register.
 *
 * The LSTP register will be read, modified to contain the new field value, and written.
 *
 * @param[in] supportlstp - The value to set the field to.
 */
__INLINE void nxmac_support_lstp_setf(uint8_t supportlstp)
{
    ASSERT_ERR((((uint32_t)supportlstp << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_LSTP_ADDR, (uint32_t)supportlstp << 0);
}

/// @}

#if RW_BFMEE_EN
/**
 * @name BFMEE_CONTROL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  27:25         bfrFormatMod   0x4
 *     24           bfrShortGI   0
 *  23:16               bfrMCS   0x84
 *  10:08              bfmeeNc   0x1
 *  07:05              bfmeeNr   0x1
 *  04:03        bfmeeGrouping   0x0
 *     02        bfmeeCodebook   1
 *     01       bfmeeMUSupport   0
 *     00          bfmeeEnable   0
 * </pre>
 *
 * @{
 */

/// Address of the BFMEE_CONTROL register
#define NXMAC_BFMEE_CONTROL_ADDR   0xC0000350
/// Offset of the BFMEE_CONTROL register from the base address
#define NXMAC_BFMEE_CONTROL_OFFSET 0x00000350
/// Index of the BFMEE_CONTROL register
#define NXMAC_BFMEE_CONTROL_INDEX  0x000000D4
/// Reset value of the BFMEE_CONTROL register
#define NXMAC_BFMEE_CONTROL_RESET  0x08840124

/**
 * @brief Returns the current value of the BFMEE_CONTROL register.
 * The BFMEE_CONTROL register will be read and its value returned.
 * @return The current value of the BFMEE_CONTROL register.
 */
__INLINE uint32_t nxmac_bfmee_control_get(void)
{
    return REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR);
}

/**
 * @brief Sets the BFMEE_CONTROL register to a value.
 * The BFMEE_CONTROL register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_bfmee_control_set(uint32_t value)
{
    REG_PL_WR(NXMAC_BFMEE_CONTROL_ADDR, value);
}

// field definitions
/// BFR_FORMAT_MOD field mask
#define NXMAC_BFR_FORMAT_MOD_MASK     ((uint32_t)0x0E000000)
/// BFR_FORMAT_MOD field LSB position
#define NXMAC_BFR_FORMAT_MOD_LSB      25
/// BFR_FORMAT_MOD field width
#define NXMAC_BFR_FORMAT_MOD_WIDTH    ((uint32_t)0x00000003)
/// BFR_SHORT_GI field bit
#define NXMAC_BFR_SHORT_GI_BIT        ((uint32_t)0x01000000)
/// BFR_SHORT_GI field position
#define NXMAC_BFR_SHORT_GI_POS        24
/// BFR_MCS field mask
#define NXMAC_BFR_MCS_MASK            ((uint32_t)0x00FF0000)
/// BFR_MCS field LSB position
#define NXMAC_BFR_MCS_LSB             16
/// BFR_MCS field width
#define NXMAC_BFR_MCS_WIDTH           ((uint32_t)0x00000008)
/// BFMEE_NC field mask
#define NXMAC_BFMEE_NC_MASK           ((uint32_t)0x00000700)
/// BFMEE_NC field LSB position
#define NXMAC_BFMEE_NC_LSB            8
/// BFMEE_NC field width
#define NXMAC_BFMEE_NC_WIDTH          ((uint32_t)0x00000003)
/// BFMEE_NR field mask
#define NXMAC_BFMEE_NR_MASK           ((uint32_t)0x000000E0)
/// BFMEE_NR field LSB position
#define NXMAC_BFMEE_NR_LSB            5
/// BFMEE_NR field width
#define NXMAC_BFMEE_NR_WIDTH          ((uint32_t)0x00000003)
/// BFMEE_GROUPING field mask
#define NXMAC_BFMEE_GROUPING_MASK     ((uint32_t)0x00000018)
/// BFMEE_GROUPING field LSB position
#define NXMAC_BFMEE_GROUPING_LSB      3
/// BFMEE_GROUPING field width
#define NXMAC_BFMEE_GROUPING_WIDTH    ((uint32_t)0x00000002)
/// BFMEE_CODEBOOK field bit
#define NXMAC_BFMEE_CODEBOOK_BIT      ((uint32_t)0x00000004)
/// BFMEE_CODEBOOK field position
#define NXMAC_BFMEE_CODEBOOK_POS      2
/// BFMEE_MU_SUPPORT field bit
#define NXMAC_BFMEE_MU_SUPPORT_BIT    ((uint32_t)0x00000002)
/// BFMEE_MU_SUPPORT field position
#define NXMAC_BFMEE_MU_SUPPORT_POS    1
/// BFMEE_ENABLE field bit
#define NXMAC_BFMEE_ENABLE_BIT        ((uint32_t)0x00000001)
/// BFMEE_ENABLE field position
#define NXMAC_BFMEE_ENABLE_POS        0

/// BFR_FORMAT_MOD field reset value
#define NXMAC_BFR_FORMAT_MOD_RST      0x4
/// BFR_SHORT_GI field reset value
#define NXMAC_BFR_SHORT_GI_RST        0x0
/// BFR_MCS field reset value
#define NXMAC_BFR_MCS_RST             0x84
/// BFMEE_NC field reset value
#define NXMAC_BFMEE_NC_RST            0x1
/// BFMEE_NR field reset value
#define NXMAC_BFMEE_NR_RST            0x1
/// BFMEE_GROUPING field reset value
#define NXMAC_BFMEE_GROUPING_RST      0x0
/// BFMEE_CODEBOOK field reset value
#define NXMAC_BFMEE_CODEBOOK_RST      0x1
/// BFMEE_MU_SUPPORT field reset value
#define NXMAC_BFMEE_MU_SUPPORT_RST    0x0
/// BFMEE_ENABLE field reset value
#define NXMAC_BFMEE_ENABLE_RST        0x0

/**
 * @brief Constructs a value for the BFMEE_CONTROL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] bfrformatmod - The value to use for the bfrFormatMod field.
 * @param[in] bfrshortgi - The value to use for the bfrShortGI field.
 * @param[in] bfrmcs - The value to use for the bfrMCS field.
 * @param[in] bfmeenc - The value to use for the bfmeeNc field.
 * @param[in] bfmeenr - The value to use for the bfmeeNr field.
 * @param[in] bfmeegrouping - The value to use for the bfmeeGrouping field.
 * @param[in] bfmeecodebook - The value to use for the bfmeeCodebook field.
 * @param[in] bfmeemusupport - The value to use for the bfmeeMUSupport field.
 * @param[in] bfmeeenable - The value to use for the bfmeeEnable field.
 */
__INLINE void nxmac_bfmee_control_pack(uint8_t bfrformatmod, uint8_t bfrshortgi, uint8_t bfrmcs, uint8_t bfmeenc, uint8_t bfmeenr, uint8_t bfmeegrouping, uint8_t bfmeecodebook, uint8_t bfmeemusupport, uint8_t bfmeeenable)
{
    ASSERT_ERR((((uint32_t)bfrformatmod << 25) & ~((uint32_t)0x0E000000)) == 0);
    ASSERT_ERR((((uint32_t)bfrshortgi << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)bfrmcs << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)bfmeenc << 8) & ~((uint32_t)0x00000700)) == 0);
    ASSERT_ERR((((uint32_t)bfmeenr << 5) & ~((uint32_t)0x000000E0)) == 0);
    ASSERT_ERR((((uint32_t)bfmeegrouping << 3) & ~((uint32_t)0x00000018)) == 0);
    ASSERT_ERR((((uint32_t)bfmeecodebook << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)bfmeemusupport << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)bfmeeenable << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_BFMEE_CONTROL_ADDR,  ((uint32_t)bfrformatmod << 25) | ((uint32_t)bfrshortgi << 24) | ((uint32_t)bfrmcs << 16) | ((uint32_t)bfmeenc << 8) | ((uint32_t)bfmeenr << 5) | ((uint32_t)bfmeegrouping << 3) | ((uint32_t)bfmeecodebook << 2) | ((uint32_t)bfmeemusupport << 1) | ((uint32_t)bfmeeenable << 0));
}

/**
 * @brief Unpacks BFMEE_CONTROL's fields from current value of the BFMEE_CONTROL register.
 *
 * Reads the BFMEE_CONTROL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] bfrformatmod - Will be populated with the current value of this field from the register.
 * @param[out] bfrshortgi - Will be populated with the current value of this field from the register.
 * @param[out] bfrmcs - Will be populated with the current value of this field from the register.
 * @param[out] bfmeenc - Will be populated with the current value of this field from the register.
 * @param[out] bfmeenr - Will be populated with the current value of this field from the register.
 * @param[out] bfmeegrouping - Will be populated with the current value of this field from the register.
 * @param[out] bfmeecodebook - Will be populated with the current value of this field from the register.
 * @param[out] bfmeemusupport - Will be populated with the current value of this field from the register.
 * @param[out] bfmeeenable - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_bfmee_control_unpack(uint8_t *bfrformatmod, uint8_t *bfrshortgi, uint8_t *bfrmcs, uint8_t *bfmeenc, uint8_t *bfmeenr, uint8_t *bfmeegrouping, uint8_t *bfmeecodebook, uint8_t *bfmeemusupport, uint8_t *bfmeeenable)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR);

    *bfrformatmod = (localVal & ((uint32_t)0x0E000000)) >> 25;
    *bfrshortgi = (localVal & ((uint32_t)0x01000000)) >> 24;
    *bfrmcs = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *bfmeenc = (localVal & ((uint32_t)0x00000700)) >> 8;
    *bfmeenr = (localVal & ((uint32_t)0x000000E0)) >> 5;
    *bfmeegrouping = (localVal & ((uint32_t)0x00000018)) >> 3;
    *bfmeecodebook = (localVal & ((uint32_t)0x00000004)) >> 2;
    *bfmeemusupport = (localVal & ((uint32_t)0x00000002)) >> 1;
    *bfmeeenable = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the bfrFormatMod field in the BFMEE_CONTROL register.
 *
 * The BFMEE_CONTROL register will be read and the bfrFormatMod field's value will be returned.
 *
 * @return The current value of the bfrFormatMod field in the BFMEE_CONTROL register.
 */
__INLINE uint8_t nxmac_bfr_format_mod_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x0E000000)) >> 25);
}

/**
 * @brief Sets the bfrFormatMod field of the BFMEE_CONTROL register.
 *
 * The BFMEE_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bfrformatmod - The value to set the field to.
 */
__INLINE void nxmac_bfr_format_mod_setf(uint8_t bfrformatmod)
{
    ASSERT_ERR((((uint32_t)bfrformatmod << 25) & ~((uint32_t)0x0E000000)) == 0);
    REG_PL_WR(NXMAC_BFMEE_CONTROL_ADDR, (REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR) & ~((uint32_t)0x0E000000)) | ((uint32_t)bfrformatmod << 25));
}

/**
 * @brief Returns the current value of the bfrShortGI field in the BFMEE_CONTROL register.
 *
 * The BFMEE_CONTROL register will be read and the bfrShortGI field's value will be returned.
 *
 * @return The current value of the bfrShortGI field in the BFMEE_CONTROL register.
 */
__INLINE uint8_t nxmac_bfr_short_gi_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the bfrShortGI field of the BFMEE_CONTROL register.
 *
 * The BFMEE_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bfrshortgi - The value to set the field to.
 */
__INLINE void nxmac_bfr_short_gi_setf(uint8_t bfrshortgi)
{
    ASSERT_ERR((((uint32_t)bfrshortgi << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(NXMAC_BFMEE_CONTROL_ADDR, (REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)bfrshortgi << 24));
}

/**
 * @brief Returns the current value of the bfrMCS field in the BFMEE_CONTROL register.
 *
 * The BFMEE_CONTROL register will be read and the bfrMCS field's value will be returned.
 *
 * @return The current value of the bfrMCS field in the BFMEE_CONTROL register.
 */
__INLINE uint8_t nxmac_bfr_mcs_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the bfrMCS field of the BFMEE_CONTROL register.
 *
 * The BFMEE_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bfrmcs - The value to set the field to.
 */
__INLINE void nxmac_bfr_mcs_setf(uint8_t bfrmcs)
{
    ASSERT_ERR((((uint32_t)bfrmcs << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(NXMAC_BFMEE_CONTROL_ADDR, (REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)bfrmcs << 16));
}

/**
 * @brief Returns the current value of the bfmeeNc field in the BFMEE_CONTROL register.
 *
 * The BFMEE_CONTROL register will be read and the bfmeeNc field's value will be returned.
 *
 * @return The current value of the bfmeeNc field in the BFMEE_CONTROL register.
 */
__INLINE uint8_t nxmac_bfmee_nc_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

/**
 * @brief Sets the bfmeeNc field of the BFMEE_CONTROL register.
 *
 * The BFMEE_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bfmeenc - The value to set the field to.
 */
__INLINE void nxmac_bfmee_nc_setf(uint8_t bfmeenc)
{
    ASSERT_ERR((((uint32_t)bfmeenc << 8) & ~((uint32_t)0x00000700)) == 0);
    REG_PL_WR(NXMAC_BFMEE_CONTROL_ADDR, (REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR) & ~((uint32_t)0x00000700)) | ((uint32_t)bfmeenc << 8));
}

/**
 * @brief Returns the current value of the bfmeeNr field in the BFMEE_CONTROL register.
 *
 * The BFMEE_CONTROL register will be read and the bfmeeNr field's value will be returned.
 *
 * @return The current value of the bfmeeNr field in the BFMEE_CONTROL register.
 */
__INLINE uint8_t nxmac_bfmee_nr_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x000000E0)) >> 5);
}

/**
 * @brief Sets the bfmeeNr field of the BFMEE_CONTROL register.
 *
 * The BFMEE_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bfmeenr - The value to set the field to.
 */
__INLINE void nxmac_bfmee_nr_setf(uint8_t bfmeenr)
{
    ASSERT_ERR((((uint32_t)bfmeenr << 5) & ~((uint32_t)0x000000E0)) == 0);
    REG_PL_WR(NXMAC_BFMEE_CONTROL_ADDR, (REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR) & ~((uint32_t)0x000000E0)) | ((uint32_t)bfmeenr << 5));
}

/**
 * @brief Returns the current value of the bfmeeGrouping field in the BFMEE_CONTROL register.
 *
 * The BFMEE_CONTROL register will be read and the bfmeeGrouping field's value will be returned.
 *
 * @return The current value of the bfmeeGrouping field in the BFMEE_CONTROL register.
 */
__INLINE uint8_t nxmac_bfmee_grouping_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00000018)) >> 3);
}

/**
 * @brief Sets the bfmeeGrouping field of the BFMEE_CONTROL register.
 *
 * The BFMEE_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bfmeegrouping - The value to set the field to.
 */
__INLINE void nxmac_bfmee_grouping_setf(uint8_t bfmeegrouping)
{
    ASSERT_ERR((((uint32_t)bfmeegrouping << 3) & ~((uint32_t)0x00000018)) == 0);
    REG_PL_WR(NXMAC_BFMEE_CONTROL_ADDR, (REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR) & ~((uint32_t)0x00000018)) | ((uint32_t)bfmeegrouping << 3));
}

/**
 * @brief Returns the current value of the bfmeeCodebook field in the BFMEE_CONTROL register.
 *
 * The BFMEE_CONTROL register will be read and the bfmeeCodebook field's value will be returned.
 *
 * @return The current value of the bfmeeCodebook field in the BFMEE_CONTROL register.
 */
__INLINE uint8_t nxmac_bfmee_codebook_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the bfmeeCodebook field of the BFMEE_CONTROL register.
 *
 * The BFMEE_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bfmeecodebook - The value to set the field to.
 */
__INLINE void nxmac_bfmee_codebook_setf(uint8_t bfmeecodebook)
{
    ASSERT_ERR((((uint32_t)bfmeecodebook << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_BFMEE_CONTROL_ADDR, (REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)bfmeecodebook << 2));
}

/**
 * @brief Returns the current value of the bfmeeMUSupport field in the BFMEE_CONTROL register.
 *
 * The BFMEE_CONTROL register will be read and the bfmeeMUSupport field's value will be returned.
 *
 * @return The current value of the bfmeeMUSupport field in the BFMEE_CONTROL register.
 */
__INLINE uint8_t nxmac_bfmee_mu_support_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the bfmeeMUSupport field of the BFMEE_CONTROL register.
 *
 * The BFMEE_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bfmeemusupport - The value to set the field to.
 */
__INLINE void nxmac_bfmee_mu_support_setf(uint8_t bfmeemusupport)
{
    ASSERT_ERR((((uint32_t)bfmeemusupport << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_BFMEE_CONTROL_ADDR, (REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)bfmeemusupport << 1));
}

/**
 * @brief Returns the current value of the bfmeeEnable field in the BFMEE_CONTROL register.
 *
 * The BFMEE_CONTROL register will be read and the bfmeeEnable field's value will be returned.
 *
 * @return The current value of the bfmeeEnable field in the BFMEE_CONTROL register.
 */
__INLINE uint8_t nxmac_bfmee_enable_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the bfmeeEnable field of the BFMEE_CONTROL register.
 *
 * The BFMEE_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bfmeeenable - The value to set the field to.
 */
__INLINE void nxmac_bfmee_enable_setf(uint8_t bfmeeenable)
{
    ASSERT_ERR((((uint32_t)bfmeeenable << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_BFMEE_CONTROL_ADDR, (REG_PL_RD(NXMAC_BFMEE_CONTROL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)bfmeeenable << 0));
}

#endif // RW_BFMEE_EN
/// @}

#if RW_WLAN_COEX_EN
/**
 * @name COEX_CONTROL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:28     coexForceWlanPTI   0x0
 *     27   coexForceWlanPTIToggle   0
 *     26   coexForceWlanChanBw   0
 *     25      coexForceWlanRx   0
 *     24      coexForceWlanTx   0
 *  22:16     coexWlanChanFreq   0x0
 *     12   coexWlanChanOffset   0
 *  07:06   coexAutoPTIAdjIncr   0x1
 *     05   coexAutoPTIAdjEnable   1
 *     04      coexForceEnable   0
 *     03   coexPostponeTxEnable   1
 *     02   coexPHYRxAbortEnable   0
 *     01   coexPHYTxAbortEnable   0
 *     00           coexEnable   0
 * </pre>
 *
 * @{
 */

/// Address of the COEX_CONTROL register
#define NXMAC_COEX_CONTROL_ADDR   0xC0000400
/// Offset of the COEX_CONTROL register from the base address
#define NXMAC_COEX_CONTROL_OFFSET 0x00000400
/// Index of the COEX_CONTROL register
#define NXMAC_COEX_CONTROL_INDEX  0x00000100
/// Reset value of the COEX_CONTROL register
#define NXMAC_COEX_CONTROL_RESET  0x00000068

/**
 * @brief Returns the current value of the COEX_CONTROL register.
 * The COEX_CONTROL register will be read and its value returned.
 * @return The current value of the COEX_CONTROL register.
 */
__INLINE uint32_t nxmac_coex_control_get(void)
{
    return REG_PL_RD(NXMAC_COEX_CONTROL_ADDR);
}

/**
 * @brief Sets the COEX_CONTROL register to a value.
 * The COEX_CONTROL register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_coex_control_set(uint32_t value)
{
    REG_PL_WR(NXMAC_COEX_CONTROL_ADDR, value);
}

// field definitions
/// COEX_FORCE_WLAN_PTI field mask
#define NXMAC_COEX_FORCE_WLAN_PTI_MASK          ((uint32_t)0xF0000000)
/// COEX_FORCE_WLAN_PTI field LSB position
#define NXMAC_COEX_FORCE_WLAN_PTI_LSB           28
/// COEX_FORCE_WLAN_PTI field width
#define NXMAC_COEX_FORCE_WLAN_PTI_WIDTH         ((uint32_t)0x00000004)
/// COEX_FORCE_WLAN_PTI_TOGGLE field bit
#define NXMAC_COEX_FORCE_WLAN_PTI_TOGGLE_BIT    ((uint32_t)0x08000000)
/// COEX_FORCE_WLAN_PTI_TOGGLE field position
#define NXMAC_COEX_FORCE_WLAN_PTI_TOGGLE_POS    27
/// COEX_FORCE_WLAN_CHAN_BW field bit
#define NXMAC_COEX_FORCE_WLAN_CHAN_BW_BIT       ((uint32_t)0x04000000)
/// COEX_FORCE_WLAN_CHAN_BW field position
#define NXMAC_COEX_FORCE_WLAN_CHAN_BW_POS       26
/// COEX_FORCE_WLAN_RX field bit
#define NXMAC_COEX_FORCE_WLAN_RX_BIT            ((uint32_t)0x02000000)
/// COEX_FORCE_WLAN_RX field position
#define NXMAC_COEX_FORCE_WLAN_RX_POS            25
/// COEX_FORCE_WLAN_TX field bit
#define NXMAC_COEX_FORCE_WLAN_TX_BIT            ((uint32_t)0x01000000)
/// COEX_FORCE_WLAN_TX field position
#define NXMAC_COEX_FORCE_WLAN_TX_POS            24
/// COEX_WLAN_CHAN_FREQ field mask
#define NXMAC_COEX_WLAN_CHAN_FREQ_MASK          ((uint32_t)0x007F0000)
/// COEX_WLAN_CHAN_FREQ field LSB position
#define NXMAC_COEX_WLAN_CHAN_FREQ_LSB           16
/// COEX_WLAN_CHAN_FREQ field width
#define NXMAC_COEX_WLAN_CHAN_FREQ_WIDTH         ((uint32_t)0x00000007)
/// COEX_WLAN_CHAN_OFFSET field bit
#define NXMAC_COEX_WLAN_CHAN_OFFSET_BIT         ((uint32_t)0x00001000)
/// COEX_WLAN_CHAN_OFFSET field position
#define NXMAC_COEX_WLAN_CHAN_OFFSET_POS         12
/// COEX_AUTO_PTI_ADJ_INCR field mask
#define NXMAC_COEX_AUTO_PTI_ADJ_INCR_MASK       ((uint32_t)0x000000C0)
/// COEX_AUTO_PTI_ADJ_INCR field LSB position
#define NXMAC_COEX_AUTO_PTI_ADJ_INCR_LSB        6
/// COEX_AUTO_PTI_ADJ_INCR field width
#define NXMAC_COEX_AUTO_PTI_ADJ_INCR_WIDTH      ((uint32_t)0x00000002)
/// COEX_AUTO_PTI_ADJ_ENABLE field bit
#define NXMAC_COEX_AUTO_PTI_ADJ_ENABLE_BIT      ((uint32_t)0x00000020)
/// COEX_AUTO_PTI_ADJ_ENABLE field position
#define NXMAC_COEX_AUTO_PTI_ADJ_ENABLE_POS      5
/// COEX_FORCE_ENABLE field bit
#define NXMAC_COEX_FORCE_ENABLE_BIT             ((uint32_t)0x00000010)
/// COEX_FORCE_ENABLE field position
#define NXMAC_COEX_FORCE_ENABLE_POS             4
/// COEX_POSTPONE_TX_ENABLE field bit
#define NXMAC_COEX_POSTPONE_TX_ENABLE_BIT       ((uint32_t)0x00000008)
/// COEX_POSTPONE_TX_ENABLE field position
#define NXMAC_COEX_POSTPONE_TX_ENABLE_POS       3
/// COEX_PHY_RX_ABORT_ENABLE field bit
#define NXMAC_COEX_PHY_RX_ABORT_ENABLE_BIT      ((uint32_t)0x00000004)
/// COEX_PHY_RX_ABORT_ENABLE field position
#define NXMAC_COEX_PHY_RX_ABORT_ENABLE_POS      2
/// COEX_PHY_TX_ABORT_ENABLE field bit
#define NXMAC_COEX_PHY_TX_ABORT_ENABLE_BIT      ((uint32_t)0x00000002)
/// COEX_PHY_TX_ABORT_ENABLE field position
#define NXMAC_COEX_PHY_TX_ABORT_ENABLE_POS      1
/// COEX_ENABLE field bit
#define NXMAC_COEX_ENABLE_BIT                   ((uint32_t)0x00000001)
/// COEX_ENABLE field position
#define NXMAC_COEX_ENABLE_POS                   0

/// COEX_FORCE_WLAN_PTI field reset value
#define NXMAC_COEX_FORCE_WLAN_PTI_RST           0x0
/// COEX_FORCE_WLAN_PTI_TOGGLE field reset value
#define NXMAC_COEX_FORCE_WLAN_PTI_TOGGLE_RST    0x0
/// COEX_FORCE_WLAN_CHAN_BW field reset value
#define NXMAC_COEX_FORCE_WLAN_CHAN_BW_RST       0x0
/// COEX_FORCE_WLAN_RX field reset value
#define NXMAC_COEX_FORCE_WLAN_RX_RST            0x0
/// COEX_FORCE_WLAN_TX field reset value
#define NXMAC_COEX_FORCE_WLAN_TX_RST            0x0
/// COEX_WLAN_CHAN_FREQ field reset value
#define NXMAC_COEX_WLAN_CHAN_FREQ_RST           0x0
/// COEX_WLAN_CHAN_OFFSET field reset value
#define NXMAC_COEX_WLAN_CHAN_OFFSET_RST         0x0
/// COEX_AUTO_PTI_ADJ_INCR field reset value
#define NXMAC_COEX_AUTO_PTI_ADJ_INCR_RST        0x1
/// COEX_AUTO_PTI_ADJ_ENABLE field reset value
#define NXMAC_COEX_AUTO_PTI_ADJ_ENABLE_RST      0x1
/// COEX_FORCE_ENABLE field reset value
#define NXMAC_COEX_FORCE_ENABLE_RST             0x0
/// COEX_POSTPONE_TX_ENABLE field reset value
#define NXMAC_COEX_POSTPONE_TX_ENABLE_RST       0x1
/// COEX_PHY_RX_ABORT_ENABLE field reset value
#define NXMAC_COEX_PHY_RX_ABORT_ENABLE_RST      0x0
/// COEX_PHY_TX_ABORT_ENABLE field reset value
#define NXMAC_COEX_PHY_TX_ABORT_ENABLE_RST      0x0
/// COEX_ENABLE field reset value
#define NXMAC_COEX_ENABLE_RST                   0x0

/**
 * @brief Constructs a value for the COEX_CONTROL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] coexforcewlanpti - The value to use for the coexForceWlanPTI field.
 * @param[in] coexforcewlanptitoggle - The value to use for the coexForceWlanPTIToggle field.
 * @param[in] coexforcewlanchanbw - The value to use for the coexForceWlanChanBw field.
 * @param[in] coexforcewlanrx - The value to use for the coexForceWlanRx field.
 * @param[in] coexforcewlantx - The value to use for the coexForceWlanTx field.
 * @param[in] coexwlanchanfreq - The value to use for the coexWlanChanFreq field.
 * @param[in] coexwlanchanoffset - The value to use for the coexWlanChanOffset field.
 * @param[in] coexautoptiadjincr - The value to use for the coexAutoPTIAdjIncr field.
 * @param[in] coexautoptiadjenable - The value to use for the coexAutoPTIAdjEnable field.
 * @param[in] coexforceenable - The value to use for the coexForceEnable field.
 * @param[in] coexpostponetxenable - The value to use for the coexPostponeTxEnable field.
 * @param[in] coexphyrxabortenable - The value to use for the coexPHYRxAbortEnable field.
 * @param[in] coexphytxabortenable - The value to use for the coexPHYTxAbortEnable field.
 * @param[in] coexenable - The value to use for the coexEnable field.
 */
__INLINE void nxmac_coex_control_pack(uint8_t coexforcewlanpti, uint8_t coexforcewlanptitoggle, uint8_t coexforcewlanchanbw, uint8_t coexforcewlanrx, uint8_t coexforcewlantx, uint8_t coexwlanchanfreq, uint8_t coexwlanchanoffset, uint8_t coexautoptiadjincr, uint8_t coexautoptiadjenable, uint8_t coexforceenable, uint8_t coexpostponetxenable, uint8_t coexphyrxabortenable, uint8_t coexphytxabortenable, uint8_t coexenable)
{
    ASSERT_ERR((((uint32_t)coexforcewlanpti << 28) & ~((uint32_t)0xF0000000)) == 0);
    ASSERT_ERR((((uint32_t)coexforcewlanptitoggle << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)coexforcewlanchanbw << 26) & ~((uint32_t)0x04000000)) == 0);
    ASSERT_ERR((((uint32_t)coexforcewlanrx << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)coexforcewlantx << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)coexwlanchanfreq << 16) & ~((uint32_t)0x007F0000)) == 0);
    ASSERT_ERR((((uint32_t)coexwlanchanoffset << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)coexautoptiadjincr << 6) & ~((uint32_t)0x000000C0)) == 0);
    ASSERT_ERR((((uint32_t)coexautoptiadjenable << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)coexforceenable << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)coexpostponetxenable << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)coexphyrxabortenable << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)coexphytxabortenable << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)coexenable << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_COEX_CONTROL_ADDR,  ((uint32_t)coexforcewlanpti << 28) | ((uint32_t)coexforcewlanptitoggle << 27) | ((uint32_t)coexforcewlanchanbw << 26) | ((uint32_t)coexforcewlanrx << 25) | ((uint32_t)coexforcewlantx << 24) | ((uint32_t)coexwlanchanfreq << 16) | ((uint32_t)coexwlanchanoffset << 12) | ((uint32_t)coexautoptiadjincr << 6) | ((uint32_t)coexautoptiadjenable << 5) | ((uint32_t)coexforceenable << 4) | ((uint32_t)coexpostponetxenable << 3) | ((uint32_t)coexphyrxabortenable << 2) | ((uint32_t)coexphytxabortenable << 1) | ((uint32_t)coexenable << 0));
}

/**
 * @brief Unpacks COEX_CONTROL's fields from current value of the COEX_CONTROL register.
 *
 * Reads the COEX_CONTROL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] coexforcewlanpti - Will be populated with the current value of this field from the register.
 * @param[out] coexforcewlanptitoggle - Will be populated with the current value of this field from the register.
 * @param[out] coexforcewlanchanbw - Will be populated with the current value of this field from the register.
 * @param[out] coexforcewlanrx - Will be populated with the current value of this field from the register.
 * @param[out] coexforcewlantx - Will be populated with the current value of this field from the register.
 * @param[out] coexwlanchanfreq - Will be populated with the current value of this field from the register.
 * @param[out] coexwlanchanoffset - Will be populated with the current value of this field from the register.
 * @param[out] coexautoptiadjincr - Will be populated with the current value of this field from the register.
 * @param[out] coexautoptiadjenable - Will be populated with the current value of this field from the register.
 * @param[out] coexforceenable - Will be populated with the current value of this field from the register.
 * @param[out] coexpostponetxenable - Will be populated with the current value of this field from the register.
 * @param[out] coexphyrxabortenable - Will be populated with the current value of this field from the register.
 * @param[out] coexphytxabortenable - Will be populated with the current value of this field from the register.
 * @param[out] coexenable - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_coex_control_unpack(uint8_t *coexforcewlanpti, uint8_t *coexforcewlanptitoggle, uint8_t *coexforcewlanchanbw, uint8_t *coexforcewlanrx, uint8_t *coexforcewlantx, uint8_t *coexwlanchanfreq, uint8_t *coexwlanchanoffset, uint8_t *coexautoptiadjincr, uint8_t *coexautoptiadjenable, uint8_t *coexforceenable, uint8_t *coexpostponetxenable, uint8_t *coexphyrxabortenable, uint8_t *coexphytxabortenable, uint8_t *coexenable)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_CONTROL_ADDR);

    *coexforcewlanpti = (localVal & ((uint32_t)0xF0000000)) >> 28;
    *coexforcewlanptitoggle = (localVal & ((uint32_t)0x08000000)) >> 27;
    *coexforcewlanchanbw = (localVal & ((uint32_t)0x04000000)) >> 26;
    *coexforcewlanrx = (localVal & ((uint32_t)0x02000000)) >> 25;
    *coexforcewlantx = (localVal & ((uint32_t)0x01000000)) >> 24;
    *coexwlanchanfreq = (localVal & ((uint32_t)0x007F0000)) >> 16;
    *coexwlanchanoffset = (localVal & ((uint32_t)0x00001000)) >> 12;
    *coexautoptiadjincr = (localVal & ((uint32_t)0x000000C0)) >> 6;
    *coexautoptiadjenable = (localVal & ((uint32_t)0x00000020)) >> 5;
    *coexforceenable = (localVal & ((uint32_t)0x00000010)) >> 4;
    *coexpostponetxenable = (localVal & ((uint32_t)0x00000008)) >> 3;
    *coexphyrxabortenable = (localVal & ((uint32_t)0x00000004)) >> 2;
    *coexphytxabortenable = (localVal & ((uint32_t)0x00000002)) >> 1;
    *coexenable = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the coexForceWlanPTI field in the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read and the coexForceWlanPTI field's value will be returned.
 *
 * @return The current value of the coexForceWlanPTI field in the COEX_CONTROL register.
 */
__INLINE uint8_t nxmac_coex_force_wlan_pti_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0xF0000000)) >> 28);
}

/**
 * @brief Sets the coexForceWlanPTI field of the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexforcewlanpti - The value to set the field to.
 */
__INLINE void nxmac_coex_force_wlan_pti_setf(uint8_t coexforcewlanpti)
{
    ASSERT_ERR((((uint32_t)coexforcewlanpti << 28) & ~((uint32_t)0xF0000000)) == 0);
    REG_PL_WR(NXMAC_COEX_CONTROL_ADDR, (REG_PL_RD(NXMAC_COEX_CONTROL_ADDR) & ~((uint32_t)0xF0000000)) | ((uint32_t)coexforcewlanpti << 28));
}

/**
 * @brief Returns the current value of the coexForceWlanPTIToggle field in the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read and the coexForceWlanPTIToggle field's value will be returned.
 *
 * @return The current value of the coexForceWlanPTIToggle field in the COEX_CONTROL register.
 */
__INLINE uint8_t nxmac_coex_force_wlan_pti_toggle_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Sets the coexForceWlanPTIToggle field of the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexforcewlanptitoggle - The value to set the field to.
 */
__INLINE void nxmac_coex_force_wlan_pti_toggle_setf(uint8_t coexforcewlanptitoggle)
{
    ASSERT_ERR((((uint32_t)coexforcewlanptitoggle << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(NXMAC_COEX_CONTROL_ADDR, (REG_PL_RD(NXMAC_COEX_CONTROL_ADDR) & ~((uint32_t)0x08000000)) | ((uint32_t)coexforcewlanptitoggle << 27));
}

/**
 * @brief Returns the current value of the coexForceWlanChanBw field in the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read and the coexForceWlanChanBw field's value will be returned.
 *
 * @return The current value of the coexForceWlanChanBw field in the COEX_CONTROL register.
 */
__INLINE uint8_t nxmac_coex_force_wlan_chan_bw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

/**
 * @brief Sets the coexForceWlanChanBw field of the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexforcewlanchanbw - The value to set the field to.
 */
__INLINE void nxmac_coex_force_wlan_chan_bw_setf(uint8_t coexforcewlanchanbw)
{
    ASSERT_ERR((((uint32_t)coexforcewlanchanbw << 26) & ~((uint32_t)0x04000000)) == 0);
    REG_PL_WR(NXMAC_COEX_CONTROL_ADDR, (REG_PL_RD(NXMAC_COEX_CONTROL_ADDR) & ~((uint32_t)0x04000000)) | ((uint32_t)coexforcewlanchanbw << 26));
}

/**
 * @brief Returns the current value of the coexForceWlanRx field in the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read and the coexForceWlanRx field's value will be returned.
 *
 * @return The current value of the coexForceWlanRx field in the COEX_CONTROL register.
 */
__INLINE uint8_t nxmac_coex_force_wlan_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

/**
 * @brief Sets the coexForceWlanRx field of the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexforcewlanrx - The value to set the field to.
 */
__INLINE void nxmac_coex_force_wlan_rx_setf(uint8_t coexforcewlanrx)
{
    ASSERT_ERR((((uint32_t)coexforcewlanrx << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(NXMAC_COEX_CONTROL_ADDR, (REG_PL_RD(NXMAC_COEX_CONTROL_ADDR) & ~((uint32_t)0x02000000)) | ((uint32_t)coexforcewlanrx << 25));
}

/**
 * @brief Returns the current value of the coexForceWlanTx field in the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read and the coexForceWlanTx field's value will be returned.
 *
 * @return The current value of the coexForceWlanTx field in the COEX_CONTROL register.
 */
__INLINE uint8_t nxmac_coex_force_wlan_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the coexForceWlanTx field of the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexforcewlantx - The value to set the field to.
 */
__INLINE void nxmac_coex_force_wlan_tx_setf(uint8_t coexforcewlantx)
{
    ASSERT_ERR((((uint32_t)coexforcewlantx << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(NXMAC_COEX_CONTROL_ADDR, (REG_PL_RD(NXMAC_COEX_CONTROL_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)coexforcewlantx << 24));
}

/**
 * @brief Returns the current value of the coexWlanChanFreq field in the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read and the coexWlanChanFreq field's value will be returned.
 *
 * @return The current value of the coexWlanChanFreq field in the COEX_CONTROL register.
 */
__INLINE uint8_t nxmac_coex_wlan_chan_freq_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x007F0000)) >> 16);
}

/**
 * @brief Sets the coexWlanChanFreq field of the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexwlanchanfreq - The value to set the field to.
 */
__INLINE void nxmac_coex_wlan_chan_freq_setf(uint8_t coexwlanchanfreq)
{
    ASSERT_ERR((((uint32_t)coexwlanchanfreq << 16) & ~((uint32_t)0x007F0000)) == 0);
    REG_PL_WR(NXMAC_COEX_CONTROL_ADDR, (REG_PL_RD(NXMAC_COEX_CONTROL_ADDR) & ~((uint32_t)0x007F0000)) | ((uint32_t)coexwlanchanfreq << 16));
}

/**
 * @brief Returns the current value of the coexWlanChanOffset field in the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read and the coexWlanChanOffset field's value will be returned.
 *
 * @return The current value of the coexWlanChanOffset field in the COEX_CONTROL register.
 */
__INLINE uint8_t nxmac_coex_wlan_chan_offset_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the coexWlanChanOffset field of the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexwlanchanoffset - The value to set the field to.
 */
__INLINE void nxmac_coex_wlan_chan_offset_setf(uint8_t coexwlanchanoffset)
{
    ASSERT_ERR((((uint32_t)coexwlanchanoffset << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(NXMAC_COEX_CONTROL_ADDR, (REG_PL_RD(NXMAC_COEX_CONTROL_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)coexwlanchanoffset << 12));
}

/**
 * @brief Returns the current value of the coexAutoPTIAdjIncr field in the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read and the coexAutoPTIAdjIncr field's value will be returned.
 *
 * @return The current value of the coexAutoPTIAdjIncr field in the COEX_CONTROL register.
 */
__INLINE uint8_t nxmac_coex_auto_pti_adj_incr_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x000000C0)) >> 6);
}

/**
 * @brief Sets the coexAutoPTIAdjIncr field of the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexautoptiadjincr - The value to set the field to.
 */
__INLINE void nxmac_coex_auto_pti_adj_incr_setf(uint8_t coexautoptiadjincr)
{
    ASSERT_ERR((((uint32_t)coexautoptiadjincr << 6) & ~((uint32_t)0x000000C0)) == 0);
    REG_PL_WR(NXMAC_COEX_CONTROL_ADDR, (REG_PL_RD(NXMAC_COEX_CONTROL_ADDR) & ~((uint32_t)0x000000C0)) | ((uint32_t)coexautoptiadjincr << 6));
}

/**
 * @brief Returns the current value of the coexAutoPTIAdjEnable field in the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read and the coexAutoPTIAdjEnable field's value will be returned.
 *
 * @return The current value of the coexAutoPTIAdjEnable field in the COEX_CONTROL register.
 */
__INLINE uint8_t nxmac_coex_auto_pti_adj_enable_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the coexAutoPTIAdjEnable field of the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexautoptiadjenable - The value to set the field to.
 */
__INLINE void nxmac_coex_auto_pti_adj_enable_setf(uint8_t coexautoptiadjenable)
{
    ASSERT_ERR((((uint32_t)coexautoptiadjenable << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(NXMAC_COEX_CONTROL_ADDR, (REG_PL_RD(NXMAC_COEX_CONTROL_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)coexautoptiadjenable << 5));
}

/**
 * @brief Returns the current value of the coexForceEnable field in the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read and the coexForceEnable field's value will be returned.
 *
 * @return The current value of the coexForceEnable field in the COEX_CONTROL register.
 */
__INLINE uint8_t nxmac_coex_force_enable_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the coexForceEnable field of the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexforceenable - The value to set the field to.
 */
__INLINE void nxmac_coex_force_enable_setf(uint8_t coexforceenable)
{
    ASSERT_ERR((((uint32_t)coexforceenable << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(NXMAC_COEX_CONTROL_ADDR, (REG_PL_RD(NXMAC_COEX_CONTROL_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)coexforceenable << 4));
}

/**
 * @brief Returns the current value of the coexPostponeTxEnable field in the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read and the coexPostponeTxEnable field's value will be returned.
 *
 * @return The current value of the coexPostponeTxEnable field in the COEX_CONTROL register.
 */
__INLINE uint8_t nxmac_coex_postpone_tx_enable_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the coexPostponeTxEnable field of the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexpostponetxenable - The value to set the field to.
 */
__INLINE void nxmac_coex_postpone_tx_enable_setf(uint8_t coexpostponetxenable)
{
    ASSERT_ERR((((uint32_t)coexpostponetxenable << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_COEX_CONTROL_ADDR, (REG_PL_RD(NXMAC_COEX_CONTROL_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)coexpostponetxenable << 3));
}

/**
 * @brief Returns the current value of the coexPHYRxAbortEnable field in the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read and the coexPHYRxAbortEnable field's value will be returned.
 *
 * @return The current value of the coexPHYRxAbortEnable field in the COEX_CONTROL register.
 */
__INLINE uint8_t nxmac_coex_phy_rx_abort_enable_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the coexPHYRxAbortEnable field of the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexphyrxabortenable - The value to set the field to.
 */
__INLINE void nxmac_coex_phy_rx_abort_enable_setf(uint8_t coexphyrxabortenable)
{
    ASSERT_ERR((((uint32_t)coexphyrxabortenable << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_COEX_CONTROL_ADDR, (REG_PL_RD(NXMAC_COEX_CONTROL_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)coexphyrxabortenable << 2));
}

/**
 * @brief Returns the current value of the coexPHYTxAbortEnable field in the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read and the coexPHYTxAbortEnable field's value will be returned.
 *
 * @return The current value of the coexPHYTxAbortEnable field in the COEX_CONTROL register.
 */
__INLINE uint8_t nxmac_coex_phy_tx_abort_enable_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the coexPHYTxAbortEnable field of the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexphytxabortenable - The value to set the field to.
 */
__INLINE void nxmac_coex_phy_tx_abort_enable_setf(uint8_t coexphytxabortenable)
{
    ASSERT_ERR((((uint32_t)coexphytxabortenable << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_COEX_CONTROL_ADDR, (REG_PL_RD(NXMAC_COEX_CONTROL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)coexphytxabortenable << 1));
}

/**
 * @brief Returns the current value of the coexEnable field in the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read and the coexEnable field's value will be returned.
 *
 * @return The current value of the coexEnable field in the COEX_CONTROL register.
 */
__INLINE uint8_t nxmac_coex_enable_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the coexEnable field of the COEX_CONTROL register.
 *
 * The COEX_CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexenable - The value to set the field to.
 */
__INLINE void nxmac_coex_enable_setf(uint8_t coexenable)
{
    ASSERT_ERR((((uint32_t)coexenable << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_COEX_CONTROL_ADDR, (REG_PL_RD(NXMAC_COEX_CONTROL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)coexenable << 0));
}

#endif // RW_WLAN_COEX_EN
/// @}

#if RW_WLAN_COEX_EN
/**
 * @name COEX_PTI register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:28       coexPTIBcnData   0x0
 *  27:24        coexPTIBKData   0x0
 *  23:20        coexPTIBEData   0x2
 *  19:16        coexPTIVIData   0x4
 *  15:12        coexPTIVOData   0x6
 *  11:08           coexPTIMgt   0x6
 *  07:04          coexPTICntl   0x3
 *  03:00           coexPTIAck   0x7
 * </pre>
 *
 * @{
 */

/// Address of the COEX_PTI register
#define NXMAC_COEX_PTI_ADDR   0xC0000404
/// Offset of the COEX_PTI register from the base address
#define NXMAC_COEX_PTI_OFFSET 0x00000404
/// Index of the COEX_PTI register
#define NXMAC_COEX_PTI_INDEX  0x00000101
/// Reset value of the COEX_PTI register
#define NXMAC_COEX_PTI_RESET  0x00246637

/**
 * @brief Returns the current value of the COEX_PTI register.
 * The COEX_PTI register will be read and its value returned.
 * @return The current value of the COEX_PTI register.
 */
__INLINE uint32_t nxmac_coex_pti_get(void)
{
    return REG_PL_RD(NXMAC_COEX_PTI_ADDR);
}

/**
 * @brief Sets the COEX_PTI register to a value.
 * The COEX_PTI register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_coex_pti_set(uint32_t value)
{
    REG_PL_WR(NXMAC_COEX_PTI_ADDR, value);
}

// field definitions
/// COEX_PTI_BCN_DATA field mask
#define NXMAC_COEX_PTI_BCN_DATA_MASK   ((uint32_t)0xF0000000)
/// COEX_PTI_BCN_DATA field LSB position
#define NXMAC_COEX_PTI_BCN_DATA_LSB    28
/// COEX_PTI_BCN_DATA field width
#define NXMAC_COEX_PTI_BCN_DATA_WIDTH  ((uint32_t)0x00000004)
/// COEX_PTIBK_DATA field mask
#define NXMAC_COEX_PTIBK_DATA_MASK     ((uint32_t)0x0F000000)
/// COEX_PTIBK_DATA field LSB position
#define NXMAC_COEX_PTIBK_DATA_LSB      24
/// COEX_PTIBK_DATA field width
#define NXMAC_COEX_PTIBK_DATA_WIDTH    ((uint32_t)0x00000004)
/// COEX_PTIBE_DATA field mask
#define NXMAC_COEX_PTIBE_DATA_MASK     ((uint32_t)0x00F00000)
/// COEX_PTIBE_DATA field LSB position
#define NXMAC_COEX_PTIBE_DATA_LSB      20
/// COEX_PTIBE_DATA field width
#define NXMAC_COEX_PTIBE_DATA_WIDTH    ((uint32_t)0x00000004)
/// COEX_PTIVI_DATA field mask
#define NXMAC_COEX_PTIVI_DATA_MASK     ((uint32_t)0x000F0000)
/// COEX_PTIVI_DATA field LSB position
#define NXMAC_COEX_PTIVI_DATA_LSB      16
/// COEX_PTIVI_DATA field width
#define NXMAC_COEX_PTIVI_DATA_WIDTH    ((uint32_t)0x00000004)
/// COEX_PTIVO_DATA field mask
#define NXMAC_COEX_PTIVO_DATA_MASK     ((uint32_t)0x0000F000)
/// COEX_PTIVO_DATA field LSB position
#define NXMAC_COEX_PTIVO_DATA_LSB      12
/// COEX_PTIVO_DATA field width
#define NXMAC_COEX_PTIVO_DATA_WIDTH    ((uint32_t)0x00000004)
/// COEX_PTI_MGT field mask
#define NXMAC_COEX_PTI_MGT_MASK        ((uint32_t)0x00000F00)
/// COEX_PTI_MGT field LSB position
#define NXMAC_COEX_PTI_MGT_LSB         8
/// COEX_PTI_MGT field width
#define NXMAC_COEX_PTI_MGT_WIDTH       ((uint32_t)0x00000004)
/// COEX_PTI_CNTL field mask
#define NXMAC_COEX_PTI_CNTL_MASK       ((uint32_t)0x000000F0)
/// COEX_PTI_CNTL field LSB position
#define NXMAC_COEX_PTI_CNTL_LSB        4
/// COEX_PTI_CNTL field width
#define NXMAC_COEX_PTI_CNTL_WIDTH      ((uint32_t)0x00000004)
/// COEX_PTI_ACK field mask
#define NXMAC_COEX_PTI_ACK_MASK        ((uint32_t)0x0000000F)
/// COEX_PTI_ACK field LSB position
#define NXMAC_COEX_PTI_ACK_LSB         0
/// COEX_PTI_ACK field width
#define NXMAC_COEX_PTI_ACK_WIDTH       ((uint32_t)0x00000004)

/// COEX_PTI_BCN_DATA field reset value
#define NXMAC_COEX_PTI_BCN_DATA_RST    0x0
/// COEX_PTIBK_DATA field reset value
#define NXMAC_COEX_PTIBK_DATA_RST      0x0
/// COEX_PTIBE_DATA field reset value
#define NXMAC_COEX_PTIBE_DATA_RST      0x2
/// COEX_PTIVI_DATA field reset value
#define NXMAC_COEX_PTIVI_DATA_RST      0x4
/// COEX_PTIVO_DATA field reset value
#define NXMAC_COEX_PTIVO_DATA_RST      0x6
/// COEX_PTI_MGT field reset value
#define NXMAC_COEX_PTI_MGT_RST         0x6
/// COEX_PTI_CNTL field reset value
#define NXMAC_COEX_PTI_CNTL_RST        0x3
/// COEX_PTI_ACK field reset value
#define NXMAC_COEX_PTI_ACK_RST         0x7

/**
 * @brief Constructs a value for the COEX_PTI register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] coexptibcndata - The value to use for the coexPTIBcnData field.
 * @param[in] coexptibkdata - The value to use for the coexPTIBKData field.
 * @param[in] coexptibedata - The value to use for the coexPTIBEData field.
 * @param[in] coexptividata - The value to use for the coexPTIVIData field.
 * @param[in] coexptivodata - The value to use for the coexPTIVOData field.
 * @param[in] coexptimgt - The value to use for the coexPTIMgt field.
 * @param[in] coexpticntl - The value to use for the coexPTICntl field.
 * @param[in] coexptiack - The value to use for the coexPTIAck field.
 */
__INLINE void nxmac_coex_pti_pack(uint8_t coexptibcndata, uint8_t coexptibkdata, uint8_t coexptibedata, uint8_t coexptividata, uint8_t coexptivodata, uint8_t coexptimgt, uint8_t coexpticntl, uint8_t coexptiack)
{
    ASSERT_ERR((((uint32_t)coexptibcndata << 28) & ~((uint32_t)0xF0000000)) == 0);
    ASSERT_ERR((((uint32_t)coexptibkdata << 24) & ~((uint32_t)0x0F000000)) == 0);
    ASSERT_ERR((((uint32_t)coexptibedata << 20) & ~((uint32_t)0x00F00000)) == 0);
    ASSERT_ERR((((uint32_t)coexptividata << 16) & ~((uint32_t)0x000F0000)) == 0);
    ASSERT_ERR((((uint32_t)coexptivodata << 12) & ~((uint32_t)0x0000F000)) == 0);
    ASSERT_ERR((((uint32_t)coexptimgt << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)coexpticntl << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)coexptiack << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(NXMAC_COEX_PTI_ADDR,  ((uint32_t)coexptibcndata << 28) | ((uint32_t)coexptibkdata << 24) | ((uint32_t)coexptibedata << 20) | ((uint32_t)coexptividata << 16) | ((uint32_t)coexptivodata << 12) | ((uint32_t)coexptimgt << 8) | ((uint32_t)coexpticntl << 4) | ((uint32_t)coexptiack << 0));
}

/**
 * @brief Unpacks COEX_PTI's fields from current value of the COEX_PTI register.
 *
 * Reads the COEX_PTI register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] coexptibcndata - Will be populated with the current value of this field from the register.
 * @param[out] coexptibkdata - Will be populated with the current value of this field from the register.
 * @param[out] coexptibedata - Will be populated with the current value of this field from the register.
 * @param[out] coexptividata - Will be populated with the current value of this field from the register.
 * @param[out] coexptivodata - Will be populated with the current value of this field from the register.
 * @param[out] coexptimgt - Will be populated with the current value of this field from the register.
 * @param[out] coexpticntl - Will be populated with the current value of this field from the register.
 * @param[out] coexptiack - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_coex_pti_unpack(uint8_t *coexptibcndata, uint8_t *coexptibkdata, uint8_t *coexptibedata, uint8_t *coexptividata, uint8_t *coexptivodata, uint8_t *coexptimgt, uint8_t *coexpticntl, uint8_t *coexptiack)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_PTI_ADDR);

    *coexptibcndata = (localVal & ((uint32_t)0xF0000000)) >> 28;
    *coexptibkdata = (localVal & ((uint32_t)0x0F000000)) >> 24;
    *coexptibedata = (localVal & ((uint32_t)0x00F00000)) >> 20;
    *coexptividata = (localVal & ((uint32_t)0x000F0000)) >> 16;
    *coexptivodata = (localVal & ((uint32_t)0x0000F000)) >> 12;
    *coexptimgt = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *coexpticntl = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *coexptiack = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the coexPTIBcnData field in the COEX_PTI register.
 *
 * The COEX_PTI register will be read and the coexPTIBcnData field's value will be returned.
 *
 * @return The current value of the coexPTIBcnData field in the COEX_PTI register.
 */
__INLINE uint8_t nxmac_coex_pti_bcn_data_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_PTI_ADDR);
    return ((localVal & ((uint32_t)0xF0000000)) >> 28);
}

/**
 * @brief Sets the coexPTIBcnData field of the COEX_PTI register.
 *
 * The COEX_PTI register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexptibcndata - The value to set the field to.
 */
__INLINE void nxmac_coex_pti_bcn_data_setf(uint8_t coexptibcndata)
{
    ASSERT_ERR((((uint32_t)coexptibcndata << 28) & ~((uint32_t)0xF0000000)) == 0);
    REG_PL_WR(NXMAC_COEX_PTI_ADDR, (REG_PL_RD(NXMAC_COEX_PTI_ADDR) & ~((uint32_t)0xF0000000)) | ((uint32_t)coexptibcndata << 28));
}

/**
 * @brief Returns the current value of the coexPTIBKData field in the COEX_PTI register.
 *
 * The COEX_PTI register will be read and the coexPTIBKData field's value will be returned.
 *
 * @return The current value of the coexPTIBKData field in the COEX_PTI register.
 */
__INLINE uint8_t nxmac_coex_ptibk_data_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_PTI_ADDR);
    return ((localVal & ((uint32_t)0x0F000000)) >> 24);
}

/**
 * @brief Sets the coexPTIBKData field of the COEX_PTI register.
 *
 * The COEX_PTI register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexptibkdata - The value to set the field to.
 */
__INLINE void nxmac_coex_ptibk_data_setf(uint8_t coexptibkdata)
{
    ASSERT_ERR((((uint32_t)coexptibkdata << 24) & ~((uint32_t)0x0F000000)) == 0);
    REG_PL_WR(NXMAC_COEX_PTI_ADDR, (REG_PL_RD(NXMAC_COEX_PTI_ADDR) & ~((uint32_t)0x0F000000)) | ((uint32_t)coexptibkdata << 24));
}

/**
 * @brief Returns the current value of the coexPTIBEData field in the COEX_PTI register.
 *
 * The COEX_PTI register will be read and the coexPTIBEData field's value will be returned.
 *
 * @return The current value of the coexPTIBEData field in the COEX_PTI register.
 */
__INLINE uint8_t nxmac_coex_ptibe_data_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_PTI_ADDR);
    return ((localVal & ((uint32_t)0x00F00000)) >> 20);
}

/**
 * @brief Sets the coexPTIBEData field of the COEX_PTI register.
 *
 * The COEX_PTI register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexptibedata - The value to set the field to.
 */
__INLINE void nxmac_coex_ptibe_data_setf(uint8_t coexptibedata)
{
    ASSERT_ERR((((uint32_t)coexptibedata << 20) & ~((uint32_t)0x00F00000)) == 0);
    REG_PL_WR(NXMAC_COEX_PTI_ADDR, (REG_PL_RD(NXMAC_COEX_PTI_ADDR) & ~((uint32_t)0x00F00000)) | ((uint32_t)coexptibedata << 20));
}

/**
 * @brief Returns the current value of the coexPTIVIData field in the COEX_PTI register.
 *
 * The COEX_PTI register will be read and the coexPTIVIData field's value will be returned.
 *
 * @return The current value of the coexPTIVIData field in the COEX_PTI register.
 */
__INLINE uint8_t nxmac_coex_ptivi_data_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_PTI_ADDR);
    return ((localVal & ((uint32_t)0x000F0000)) >> 16);
}

/**
 * @brief Sets the coexPTIVIData field of the COEX_PTI register.
 *
 * The COEX_PTI register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexptividata - The value to set the field to.
 */
__INLINE void nxmac_coex_ptivi_data_setf(uint8_t coexptividata)
{
    ASSERT_ERR((((uint32_t)coexptividata << 16) & ~((uint32_t)0x000F0000)) == 0);
    REG_PL_WR(NXMAC_COEX_PTI_ADDR, (REG_PL_RD(NXMAC_COEX_PTI_ADDR) & ~((uint32_t)0x000F0000)) | ((uint32_t)coexptividata << 16));
}

/**
 * @brief Returns the current value of the coexPTIVOData field in the COEX_PTI register.
 *
 * The COEX_PTI register will be read and the coexPTIVOData field's value will be returned.
 *
 * @return The current value of the coexPTIVOData field in the COEX_PTI register.
 */
__INLINE uint8_t nxmac_coex_ptivo_data_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_PTI_ADDR);
    return ((localVal & ((uint32_t)0x0000F000)) >> 12);
}

/**
 * @brief Sets the coexPTIVOData field of the COEX_PTI register.
 *
 * The COEX_PTI register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexptivodata - The value to set the field to.
 */
__INLINE void nxmac_coex_ptivo_data_setf(uint8_t coexptivodata)
{
    ASSERT_ERR((((uint32_t)coexptivodata << 12) & ~((uint32_t)0x0000F000)) == 0);
    REG_PL_WR(NXMAC_COEX_PTI_ADDR, (REG_PL_RD(NXMAC_COEX_PTI_ADDR) & ~((uint32_t)0x0000F000)) | ((uint32_t)coexptivodata << 12));
}

/**
 * @brief Returns the current value of the coexPTIMgt field in the COEX_PTI register.
 *
 * The COEX_PTI register will be read and the coexPTIMgt field's value will be returned.
 *
 * @return The current value of the coexPTIMgt field in the COEX_PTI register.
 */
__INLINE uint8_t nxmac_coex_pti_mgt_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_PTI_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the coexPTIMgt field of the COEX_PTI register.
 *
 * The COEX_PTI register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexptimgt - The value to set the field to.
 */
__INLINE void nxmac_coex_pti_mgt_setf(uint8_t coexptimgt)
{
    ASSERT_ERR((((uint32_t)coexptimgt << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(NXMAC_COEX_PTI_ADDR, (REG_PL_RD(NXMAC_COEX_PTI_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)coexptimgt << 8));
}

/**
 * @brief Returns the current value of the coexPTICntl field in the COEX_PTI register.
 *
 * The COEX_PTI register will be read and the coexPTICntl field's value will be returned.
 *
 * @return The current value of the coexPTICntl field in the COEX_PTI register.
 */
__INLINE uint8_t nxmac_coex_pti_cntl_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_PTI_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the coexPTICntl field of the COEX_PTI register.
 *
 * The COEX_PTI register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexpticntl - The value to set the field to.
 */
__INLINE void nxmac_coex_pti_cntl_setf(uint8_t coexpticntl)
{
    ASSERT_ERR((((uint32_t)coexpticntl << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(NXMAC_COEX_PTI_ADDR, (REG_PL_RD(NXMAC_COEX_PTI_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)coexpticntl << 4));
}

/**
 * @brief Returns the current value of the coexPTIAck field in the COEX_PTI register.
 *
 * The COEX_PTI register will be read and the coexPTIAck field's value will be returned.
 *
 * @return The current value of the coexPTIAck field in the COEX_PTI register.
 */
__INLINE uint8_t nxmac_coex_pti_ack_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_PTI_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the coexPTIAck field of the COEX_PTI register.
 *
 * The COEX_PTI register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexptiack - The value to set the field to.
 */
__INLINE void nxmac_coex_pti_ack_setf(uint8_t coexptiack)
{
    ASSERT_ERR((((uint32_t)coexptiack << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(NXMAC_COEX_PTI_ADDR, (REG_PL_RD(NXMAC_COEX_PTI_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)coexptiack << 0));
}

#endif // RW_WLAN_COEX_EN
/// @}

#if RW_WLAN_COEX_EN
/**
 * @name COEX_STAT register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  11:08     coexWlanPTIState   0x0
 *     05   coexWlanPTIToggleState   0
 *     04   coexWlanChanBwState   0
 *     03      coexWlanRxState   0
 *     02      coexWlanTxState   0
 *     01   coexWlanRxAbortState   0
 *     00   coexWlanTxAbortState   0
 * </pre>
 *
 * @{
 */

/// Address of the COEX_STAT register
#define NXMAC_COEX_STAT_ADDR   0xC0000408
/// Offset of the COEX_STAT register from the base address
#define NXMAC_COEX_STAT_OFFSET 0x00000408
/// Index of the COEX_STAT register
#define NXMAC_COEX_STAT_INDEX  0x00000102
/// Reset value of the COEX_STAT register
#define NXMAC_COEX_STAT_RESET  0x00000000

/**
 * @brief Returns the current value of the COEX_STAT register.
 * The COEX_STAT register will be read and its value returned.
 * @return The current value of the COEX_STAT register.
 */
__INLINE uint32_t nxmac_coex_stat_get(void)
{
    return REG_PL_RD(NXMAC_COEX_STAT_ADDR);
}

/**
 * @brief Sets the COEX_STAT register to a value.
 * The COEX_STAT register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_coex_stat_set(uint32_t value)
{
    REG_PL_WR(NXMAC_COEX_STAT_ADDR, value);
}

// field definitions
/// COEX_WLAN_PTI_STATE field mask
#define NXMAC_COEX_WLAN_PTI_STATE_MASK          ((uint32_t)0x00000F00)
/// COEX_WLAN_PTI_STATE field LSB position
#define NXMAC_COEX_WLAN_PTI_STATE_LSB           8
/// COEX_WLAN_PTI_STATE field width
#define NXMAC_COEX_WLAN_PTI_STATE_WIDTH         ((uint32_t)0x00000004)
/// COEX_WLAN_PTI_TOGGLE_STATE field bit
#define NXMAC_COEX_WLAN_PTI_TOGGLE_STATE_BIT    ((uint32_t)0x00000020)
/// COEX_WLAN_PTI_TOGGLE_STATE field position
#define NXMAC_COEX_WLAN_PTI_TOGGLE_STATE_POS    5
/// COEX_WLAN_CHAN_BW_STATE field bit
#define NXMAC_COEX_WLAN_CHAN_BW_STATE_BIT       ((uint32_t)0x00000010)
/// COEX_WLAN_CHAN_BW_STATE field position
#define NXMAC_COEX_WLAN_CHAN_BW_STATE_POS       4
/// COEX_WLAN_RX_STATE field bit
#define NXMAC_COEX_WLAN_RX_STATE_BIT            ((uint32_t)0x00000008)
/// COEX_WLAN_RX_STATE field position
#define NXMAC_COEX_WLAN_RX_STATE_POS            3
/// COEX_WLAN_TX_STATE field bit
#define NXMAC_COEX_WLAN_TX_STATE_BIT            ((uint32_t)0x00000004)
/// COEX_WLAN_TX_STATE field position
#define NXMAC_COEX_WLAN_TX_STATE_POS            2
/// COEX_WLAN_RX_ABORT_STATE field bit
#define NXMAC_COEX_WLAN_RX_ABORT_STATE_BIT      ((uint32_t)0x00000002)
/// COEX_WLAN_RX_ABORT_STATE field position
#define NXMAC_COEX_WLAN_RX_ABORT_STATE_POS      1
/// COEX_WLAN_TX_ABORT_STATE field bit
#define NXMAC_COEX_WLAN_TX_ABORT_STATE_BIT      ((uint32_t)0x00000001)
/// COEX_WLAN_TX_ABORT_STATE field position
#define NXMAC_COEX_WLAN_TX_ABORT_STATE_POS      0

/// COEX_WLAN_PTI_STATE field reset value
#define NXMAC_COEX_WLAN_PTI_STATE_RST           0x0
/// COEX_WLAN_PTI_TOGGLE_STATE field reset value
#define NXMAC_COEX_WLAN_PTI_TOGGLE_STATE_RST    0x0
/// COEX_WLAN_CHAN_BW_STATE field reset value
#define NXMAC_COEX_WLAN_CHAN_BW_STATE_RST       0x0
/// COEX_WLAN_RX_STATE field reset value
#define NXMAC_COEX_WLAN_RX_STATE_RST            0x0
/// COEX_WLAN_TX_STATE field reset value
#define NXMAC_COEX_WLAN_TX_STATE_RST            0x0
/// COEX_WLAN_RX_ABORT_STATE field reset value
#define NXMAC_COEX_WLAN_RX_ABORT_STATE_RST      0x0
/// COEX_WLAN_TX_ABORT_STATE field reset value
#define NXMAC_COEX_WLAN_TX_ABORT_STATE_RST      0x0

/**
 * @brief Unpacks COEX_STAT's fields from current value of the COEX_STAT register.
 *
 * Reads the COEX_STAT register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] coexwlanptistate - Will be populated with the current value of this field from the register.
 * @param[out] coexwlanptitogglestate - Will be populated with the current value of this field from the register.
 * @param[out] coexwlanchanbwstate - Will be populated with the current value of this field from the register.
 * @param[out] coexwlanrxstate - Will be populated with the current value of this field from the register.
 * @param[out] coexwlantxstate - Will be populated with the current value of this field from the register.
 * @param[out] coexwlanrxabortstate - Will be populated with the current value of this field from the register.
 * @param[out] coexwlantxabortstate - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_coex_stat_unpack(uint8_t *coexwlanptistate, uint8_t *coexwlanptitogglestate, uint8_t *coexwlanchanbwstate, uint8_t *coexwlanrxstate, uint8_t *coexwlantxstate, uint8_t *coexwlanrxabortstate, uint8_t *coexwlantxabortstate)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_STAT_ADDR);

    *coexwlanptistate = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *coexwlanptitogglestate = (localVal & ((uint32_t)0x00000020)) >> 5;
    *coexwlanchanbwstate = (localVal & ((uint32_t)0x00000010)) >> 4;
    *coexwlanrxstate = (localVal & ((uint32_t)0x00000008)) >> 3;
    *coexwlantxstate = (localVal & ((uint32_t)0x00000004)) >> 2;
    *coexwlanrxabortstate = (localVal & ((uint32_t)0x00000002)) >> 1;
    *coexwlantxabortstate = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the coexWlanPTIState field in the COEX_STAT register.
 *
 * The COEX_STAT register will be read and the coexWlanPTIState field's value will be returned.
 *
 * @return The current value of the coexWlanPTIState field in the COEX_STAT register.
 */
__INLINE uint8_t nxmac_coex_wlan_pti_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Returns the current value of the coexWlanPTIToggleState field in the COEX_STAT register.
 *
 * The COEX_STAT register will be read and the coexWlanPTIToggleState field's value will be returned.
 *
 * @return The current value of the coexWlanPTIToggleState field in the COEX_STAT register.
 */
__INLINE uint8_t nxmac_coex_wlan_pti_toggle_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Returns the current value of the coexWlanChanBwState field in the COEX_STAT register.
 *
 * The COEX_STAT register will be read and the coexWlanChanBwState field's value will be returned.
 *
 * @return The current value of the coexWlanChanBwState field in the COEX_STAT register.
 */
__INLINE uint8_t nxmac_coex_wlan_chan_bw_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Returns the current value of the coexWlanRxState field in the COEX_STAT register.
 *
 * The COEX_STAT register will be read and the coexWlanRxState field's value will be returned.
 *
 * @return The current value of the coexWlanRxState field in the COEX_STAT register.
 */
__INLINE uint8_t nxmac_coex_wlan_rx_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Returns the current value of the coexWlanTxState field in the COEX_STAT register.
 *
 * The COEX_STAT register will be read and the coexWlanTxState field's value will be returned.
 *
 * @return The current value of the coexWlanTxState field in the COEX_STAT register.
 */
__INLINE uint8_t nxmac_coex_wlan_tx_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Returns the current value of the coexWlanRxAbortState field in the COEX_STAT register.
 *
 * The COEX_STAT register will be read and the coexWlanRxAbortState field's value will be returned.
 *
 * @return The current value of the coexWlanRxAbortState field in the COEX_STAT register.
 */
__INLINE uint8_t nxmac_coex_wlan_rx_abort_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Returns the current value of the coexWlanTxAbortState field in the COEX_STAT register.
 *
 * The COEX_STAT register will be read and the coexWlanTxAbortState field's value will be returned.
 *
 * @return The current value of the coexWlanTxAbortState field in the COEX_STAT register.
 */
__INLINE uint8_t nxmac_coex_wlan_tx_abort_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

#endif // RW_WLAN_COEX_EN
/// @}

#if RW_WLAN_COEX_EN
/**
 * @name COEX_INT register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     03   coexWlanRxAbortFallEn   0
 *     02   coexWlanRxAbortRiseEn   0
 *     01   coexWlanTxAbortFallEn   0
 *     00   coexWlanTxAbortRiseEn   0
 * </pre>
 *
 * @{
 */

/// Address of the COEX_INT register
#define NXMAC_COEX_INT_ADDR   0xC000040C
/// Offset of the COEX_INT register from the base address
#define NXMAC_COEX_INT_OFFSET 0x0000040C
/// Index of the COEX_INT register
#define NXMAC_COEX_INT_INDEX  0x00000103
/// Reset value of the COEX_INT register
#define NXMAC_COEX_INT_RESET  0x00000000

/**
 * @brief Returns the current value of the COEX_INT register.
 * The COEX_INT register will be read and its value returned.
 * @return The current value of the COEX_INT register.
 */
__INLINE uint32_t nxmac_coex_int_get(void)
{
    return REG_PL_RD(NXMAC_COEX_INT_ADDR);
}

/**
 * @brief Sets the COEX_INT register to a value.
 * The COEX_INT register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_coex_int_set(uint32_t value)
{
    REG_PL_WR(NXMAC_COEX_INT_ADDR, value);
}

// field definitions
/// COEX_WLAN_RX_ABORT_FALL_EN field bit
#define NXMAC_COEX_WLAN_RX_ABORT_FALL_EN_BIT    ((uint32_t)0x00000008)
/// COEX_WLAN_RX_ABORT_FALL_EN field position
#define NXMAC_COEX_WLAN_RX_ABORT_FALL_EN_POS    3
/// COEX_WLAN_RX_ABORT_RISE_EN field bit
#define NXMAC_COEX_WLAN_RX_ABORT_RISE_EN_BIT    ((uint32_t)0x00000004)
/// COEX_WLAN_RX_ABORT_RISE_EN field position
#define NXMAC_COEX_WLAN_RX_ABORT_RISE_EN_POS    2
/// COEX_WLAN_TX_ABORT_FALL_EN field bit
#define NXMAC_COEX_WLAN_TX_ABORT_FALL_EN_BIT    ((uint32_t)0x00000002)
/// COEX_WLAN_TX_ABORT_FALL_EN field position
#define NXMAC_COEX_WLAN_TX_ABORT_FALL_EN_POS    1
/// COEX_WLAN_TX_ABORT_RISE_EN field bit
#define NXMAC_COEX_WLAN_TX_ABORT_RISE_EN_BIT    ((uint32_t)0x00000001)
/// COEX_WLAN_TX_ABORT_RISE_EN field position
#define NXMAC_COEX_WLAN_TX_ABORT_RISE_EN_POS    0

/// COEX_WLAN_RX_ABORT_FALL_EN field reset value
#define NXMAC_COEX_WLAN_RX_ABORT_FALL_EN_RST    0x0
/// COEX_WLAN_RX_ABORT_RISE_EN field reset value
#define NXMAC_COEX_WLAN_RX_ABORT_RISE_EN_RST    0x0
/// COEX_WLAN_TX_ABORT_FALL_EN field reset value
#define NXMAC_COEX_WLAN_TX_ABORT_FALL_EN_RST    0x0
/// COEX_WLAN_TX_ABORT_RISE_EN field reset value
#define NXMAC_COEX_WLAN_TX_ABORT_RISE_EN_RST    0x0

/**
 * @brief Constructs a value for the COEX_INT register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] coexwlanrxabortfallen - The value to use for the coexWlanRxAbortFallEn field.
 * @param[in] coexwlanrxabortriseen - The value to use for the coexWlanRxAbortRiseEn field.
 * @param[in] coexwlantxabortfallen - The value to use for the coexWlanTxAbortFallEn field.
 * @param[in] coexwlantxabortriseen - The value to use for the coexWlanTxAbortRiseEn field.
 */
__INLINE void nxmac_coex_int_pack(uint8_t coexwlanrxabortfallen, uint8_t coexwlanrxabortriseen, uint8_t coexwlantxabortfallen, uint8_t coexwlantxabortriseen)
{
    ASSERT_ERR((((uint32_t)coexwlanrxabortfallen << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)coexwlanrxabortriseen << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)coexwlantxabortfallen << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)coexwlantxabortriseen << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_COEX_INT_ADDR,  ((uint32_t)coexwlanrxabortfallen << 3) | ((uint32_t)coexwlanrxabortriseen << 2) | ((uint32_t)coexwlantxabortfallen << 1) | ((uint32_t)coexwlantxabortriseen << 0));
}

/**
 * @brief Unpacks COEX_INT's fields from current value of the COEX_INT register.
 *
 * Reads the COEX_INT register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] coexwlanrxabortfallen - Will be populated with the current value of this field from the register.
 * @param[out] coexwlanrxabortriseen - Will be populated with the current value of this field from the register.
 * @param[out] coexwlantxabortfallen - Will be populated with the current value of this field from the register.
 * @param[out] coexwlantxabortriseen - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_coex_int_unpack(uint8_t *coexwlanrxabortfallen, uint8_t *coexwlanrxabortriseen, uint8_t *coexwlantxabortfallen, uint8_t *coexwlantxabortriseen)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_INT_ADDR);

    *coexwlanrxabortfallen = (localVal & ((uint32_t)0x00000008)) >> 3;
    *coexwlanrxabortriseen = (localVal & ((uint32_t)0x00000004)) >> 2;
    *coexwlantxabortfallen = (localVal & ((uint32_t)0x00000002)) >> 1;
    *coexwlantxabortriseen = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the coexWlanRxAbortFallEn field in the COEX_INT register.
 *
 * The COEX_INT register will be read and the coexWlanRxAbortFallEn field's value will be returned.
 *
 * @return The current value of the coexWlanRxAbortFallEn field in the COEX_INT register.
 */
__INLINE uint8_t nxmac_coex_wlan_rx_abort_fall_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_INT_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the coexWlanRxAbortFallEn field of the COEX_INT register.
 *
 * The COEX_INT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexwlanrxabortfallen - The value to set the field to.
 */
__INLINE void nxmac_coex_wlan_rx_abort_fall_en_setf(uint8_t coexwlanrxabortfallen)
{
    ASSERT_ERR((((uint32_t)coexwlanrxabortfallen << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(NXMAC_COEX_INT_ADDR, (REG_PL_RD(NXMAC_COEX_INT_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)coexwlanrxabortfallen << 3));
}

/**
 * @brief Returns the current value of the coexWlanRxAbortRiseEn field in the COEX_INT register.
 *
 * The COEX_INT register will be read and the coexWlanRxAbortRiseEn field's value will be returned.
 *
 * @return The current value of the coexWlanRxAbortRiseEn field in the COEX_INT register.
 */
__INLINE uint8_t nxmac_coex_wlan_rx_abort_rise_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_INT_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the coexWlanRxAbortRiseEn field of the COEX_INT register.
 *
 * The COEX_INT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexwlanrxabortriseen - The value to set the field to.
 */
__INLINE void nxmac_coex_wlan_rx_abort_rise_en_setf(uint8_t coexwlanrxabortriseen)
{
    ASSERT_ERR((((uint32_t)coexwlanrxabortriseen << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(NXMAC_COEX_INT_ADDR, (REG_PL_RD(NXMAC_COEX_INT_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)coexwlanrxabortriseen << 2));
}

/**
 * @brief Returns the current value of the coexWlanTxAbortFallEn field in the COEX_INT register.
 *
 * The COEX_INT register will be read and the coexWlanTxAbortFallEn field's value will be returned.
 *
 * @return The current value of the coexWlanTxAbortFallEn field in the COEX_INT register.
 */
__INLINE uint8_t nxmac_coex_wlan_tx_abort_fall_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_INT_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the coexWlanTxAbortFallEn field of the COEX_INT register.
 *
 * The COEX_INT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexwlantxabortfallen - The value to set the field to.
 */
__INLINE void nxmac_coex_wlan_tx_abort_fall_en_setf(uint8_t coexwlantxabortfallen)
{
    ASSERT_ERR((((uint32_t)coexwlantxabortfallen << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_COEX_INT_ADDR, (REG_PL_RD(NXMAC_COEX_INT_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)coexwlantxabortfallen << 1));
}

/**
 * @brief Returns the current value of the coexWlanTxAbortRiseEn field in the COEX_INT register.
 *
 * The COEX_INT register will be read and the coexWlanTxAbortRiseEn field's value will be returned.
 *
 * @return The current value of the coexWlanTxAbortRiseEn field in the COEX_INT register.
 */
__INLINE uint8_t nxmac_coex_wlan_tx_abort_rise_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_COEX_INT_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the coexWlanTxAbortRiseEn field of the COEX_INT register.
 *
 * The COEX_INT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] coexwlantxabortriseen - The value to set the field to.
 */
__INLINE void nxmac_coex_wlan_tx_abort_rise_en_setf(uint8_t coexwlantxabortriseen)
{
    ASSERT_ERR((((uint32_t)coexwlantxabortriseen << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_COEX_INT_ADDR, (REG_PL_RD(NXMAC_COEX_INT_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)coexwlantxabortriseen << 0));
}

#endif // RW_WLAN_COEX_EN
/// @}

/**
 * @name DEBUG_HWSM_1 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:24         macControlLs   0x0
 *  16:08          txControlLs   0x0
 *  05:00          rxControlLs   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_HWSM_1 register
#define NXMAC_DEBUG_HWSM_1_ADDR   0xC0000500
/// Offset of the DEBUG_HWSM_1 register from the base address
#define NXMAC_DEBUG_HWSM_1_OFFSET 0x00000500
/// Index of the DEBUG_HWSM_1 register
#define NXMAC_DEBUG_HWSM_1_INDEX  0x00000140
/// Reset value of the DEBUG_HWSM_1 register
#define NXMAC_DEBUG_HWSM_1_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_HWSM_1 register.
 * The DEBUG_HWSM_1 register will be read and its value returned.
 * @return The current value of the DEBUG_HWSM_1 register.
 */
__INLINE uint32_t nxmac_debug_hwsm_1_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_HWSM_1_ADDR);
}

// field definitions
/// MAC_CONTROL_LS field mask
#define NXMAC_MAC_CONTROL_LS_MASK   ((uint32_t)0xFF000000)
/// MAC_CONTROL_LS field LSB position
#define NXMAC_MAC_CONTROL_LS_LSB    24
/// MAC_CONTROL_LS field width
#define NXMAC_MAC_CONTROL_LS_WIDTH  ((uint32_t)0x00000008)
/// TX_CONTROL_LS field mask
#define NXMAC_TX_CONTROL_LS_MASK    ((uint32_t)0x0001FF00)
/// TX_CONTROL_LS field LSB position
#define NXMAC_TX_CONTROL_LS_LSB     8
/// TX_CONTROL_LS field width
#define NXMAC_TX_CONTROL_LS_WIDTH   ((uint32_t)0x00000009)
/// RX_CONTROL_LS field mask
#define NXMAC_RX_CONTROL_LS_MASK    ((uint32_t)0x0000003F)
/// RX_CONTROL_LS field LSB position
#define NXMAC_RX_CONTROL_LS_LSB     0
/// RX_CONTROL_LS field width
#define NXMAC_RX_CONTROL_LS_WIDTH   ((uint32_t)0x00000006)

/// MAC_CONTROL_LS field reset value
#define NXMAC_MAC_CONTROL_LS_RST    0x0
/// TX_CONTROL_LS field reset value
#define NXMAC_TX_CONTROL_LS_RST     0x0
/// RX_CONTROL_LS field reset value
#define NXMAC_RX_CONTROL_LS_RST     0x0

/**
 * @brief Unpacks DEBUG_HWSM_1's fields from current value of the DEBUG_HWSM_1 register.
 *
 * Reads the DEBUG_HWSM_1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] maccontrolls - Will be populated with the current value of this field from the register.
 * @param[out] txcontrolls - Will be populated with the current value of this field from the register.
 * @param[out] rxcontrolls - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_debug_hwsm_1_unpack(uint8_t *maccontrolls, uint16_t *txcontrolls, uint8_t *rxcontrolls)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_HWSM_1_ADDR);

    *maccontrolls = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *txcontrolls = (localVal & ((uint32_t)0x0001FF00)) >> 8;
    *rxcontrolls = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the macControlLs field in the DEBUG_HWSM_1 register.
 *
 * The DEBUG_HWSM_1 register will be read and the macControlLs field's value will be returned.
 *
 * @return The current value of the macControlLs field in the DEBUG_HWSM_1 register.
 */
__INLINE uint8_t nxmac_mac_control_ls_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_HWSM_1_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Returns the current value of the txControlLs field in the DEBUG_HWSM_1 register.
 *
 * The DEBUG_HWSM_1 register will be read and the txControlLs field's value will be returned.
 *
 * @return The current value of the txControlLs field in the DEBUG_HWSM_1 register.
 */
__INLINE uint16_t nxmac_tx_control_ls_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_HWSM_1_ADDR);
    return ((localVal & ((uint32_t)0x0001FF00)) >> 8);
}

/**
 * @brief Returns the current value of the rxControlLs field in the DEBUG_HWSM_1 register.
 *
 * The DEBUG_HWSM_1 register will be read and the rxControlLs field's value will be returned.
 *
 * @return The current value of the rxControlLs field in the DEBUG_HWSM_1 register.
 */
__INLINE uint8_t nxmac_rx_control_ls_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_HWSM_1_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/// @}

/**
 * @name DEBUG_HWSM_2 register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:24         macControlCs   0x0
 *  16:08          txControlCs   0x0
 *  05:00          rxControlCs   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_HWSM_2 register
#define NXMAC_DEBUG_HWSM_2_ADDR   0xC0000504
/// Offset of the DEBUG_HWSM_2 register from the base address
#define NXMAC_DEBUG_HWSM_2_OFFSET 0x00000504
/// Index of the DEBUG_HWSM_2 register
#define NXMAC_DEBUG_HWSM_2_INDEX  0x00000141
/// Reset value of the DEBUG_HWSM_2 register
#define NXMAC_DEBUG_HWSM_2_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_HWSM_2 register.
 * The DEBUG_HWSM_2 register will be read and its value returned.
 * @return The current value of the DEBUG_HWSM_2 register.
 */
__INLINE uint32_t nxmac_debug_hwsm_2_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_HWSM_2_ADDR);
}

// field definitions
/// MAC_CONTROL_CS field mask
#define NXMAC_MAC_CONTROL_CS_MASK   ((uint32_t)0xFF000000)
/// MAC_CONTROL_CS field LSB position
#define NXMAC_MAC_CONTROL_CS_LSB    24
/// MAC_CONTROL_CS field width
#define NXMAC_MAC_CONTROL_CS_WIDTH  ((uint32_t)0x00000008)
/// TX_CONTROL_CS field mask
#define NXMAC_TX_CONTROL_CS_MASK    ((uint32_t)0x0001FF00)
/// TX_CONTROL_CS field LSB position
#define NXMAC_TX_CONTROL_CS_LSB     8
/// TX_CONTROL_CS field width
#define NXMAC_TX_CONTROL_CS_WIDTH   ((uint32_t)0x00000009)
/// RX_CONTROL_CS field mask
#define NXMAC_RX_CONTROL_CS_MASK    ((uint32_t)0x0000003F)
/// RX_CONTROL_CS field LSB position
#define NXMAC_RX_CONTROL_CS_LSB     0
/// RX_CONTROL_CS field width
#define NXMAC_RX_CONTROL_CS_WIDTH   ((uint32_t)0x00000006)

/// MAC_CONTROL_CS field reset value
#define NXMAC_MAC_CONTROL_CS_RST    0x0
/// TX_CONTROL_CS field reset value
#define NXMAC_TX_CONTROL_CS_RST     0x0
/// RX_CONTROL_CS field reset value
#define NXMAC_RX_CONTROL_CS_RST     0x0

/**
 * @brief Unpacks DEBUG_HWSM_2's fields from current value of the DEBUG_HWSM_2 register.
 *
 * Reads the DEBUG_HWSM_2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] maccontrolcs - Will be populated with the current value of this field from the register.
 * @param[out] txcontrolcs - Will be populated with the current value of this field from the register.
 * @param[out] rxcontrolcs - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_debug_hwsm_2_unpack(uint8_t *maccontrolcs, uint16_t *txcontrolcs, uint8_t *rxcontrolcs)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_HWSM_2_ADDR);

    *maccontrolcs = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *txcontrolcs = (localVal & ((uint32_t)0x0001FF00)) >> 8;
    *rxcontrolcs = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the macControlCs field in the DEBUG_HWSM_2 register.
 *
 * The DEBUG_HWSM_2 register will be read and the macControlCs field's value will be returned.
 *
 * @return The current value of the macControlCs field in the DEBUG_HWSM_2 register.
 */
__INLINE uint8_t nxmac_mac_control_cs_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_HWSM_2_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Returns the current value of the txControlCs field in the DEBUG_HWSM_2 register.
 *
 * The DEBUG_HWSM_2 register will be read and the txControlCs field's value will be returned.
 *
 * @return The current value of the txControlCs field in the DEBUG_HWSM_2 register.
 */
__INLINE uint16_t nxmac_tx_control_cs_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_HWSM_2_ADDR);
    return ((localVal & ((uint32_t)0x0001FF00)) >> 8);
}

/**
 * @brief Returns the current value of the rxControlCs field in the DEBUG_HWSM_2 register.
 *
 * The DEBUG_HWSM_2 register will be read and the rxControlCs field's value will be returned.
 *
 * @return The current value of the rxControlCs field in the DEBUG_HWSM_2 register.
 */
__INLINE uint8_t nxmac_rx_control_cs_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_HWSM_2_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/// @}

/**
 * @name DEBUG_PORT_VALUE register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00        debugPortRead   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_PORT_VALUE register
#define NXMAC_DEBUG_PORT_VALUE_ADDR   0xC000050C
/// Offset of the DEBUG_PORT_VALUE register from the base address
#define NXMAC_DEBUG_PORT_VALUE_OFFSET 0x0000050C
/// Index of the DEBUG_PORT_VALUE register
#define NXMAC_DEBUG_PORT_VALUE_INDEX  0x00000143
/// Reset value of the DEBUG_PORT_VALUE register
#define NXMAC_DEBUG_PORT_VALUE_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_PORT_VALUE register.
 * The DEBUG_PORT_VALUE register will be read and its value returned.
 * @return The current value of the DEBUG_PORT_VALUE register.
 */
__INLINE uint32_t nxmac_debug_port_value_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_PORT_VALUE_ADDR);
}

// field definitions
/// DEBUG_PORT_READ field mask
#define NXMAC_DEBUG_PORT_READ_MASK   ((uint32_t)0xFFFFFFFF)
/// DEBUG_PORT_READ field LSB position
#define NXMAC_DEBUG_PORT_READ_LSB    0
/// DEBUG_PORT_READ field width
#define NXMAC_DEBUG_PORT_READ_WIDTH  ((uint32_t)0x00000020)

/// DEBUG_PORT_READ field reset value
#define NXMAC_DEBUG_PORT_READ_RST    0x0

/**
 * @brief Returns the current value of the debugPortRead field in the DEBUG_PORT_VALUE register.
 *
 * The DEBUG_PORT_VALUE register will be read and the debugPortRead field's value will be returned.
 *
 * @return The current value of the debugPortRead field in the DEBUG_PORT_VALUE register.
 */
__INLINE uint32_t nxmac_debug_port_read_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_PORT_VALUE_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name DEBUG_PORT_SEL register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:08        debugPortSel2   0x0
 *  07:00        debugPortSel1   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_PORT_SEL register
#define NXMAC_DEBUG_PORT_SEL_ADDR   0xC0000510
/// Offset of the DEBUG_PORT_SEL register from the base address
#define NXMAC_DEBUG_PORT_SEL_OFFSET 0x00000510
/// Index of the DEBUG_PORT_SEL register
#define NXMAC_DEBUG_PORT_SEL_INDEX  0x00000144
/// Reset value of the DEBUG_PORT_SEL register
#define NXMAC_DEBUG_PORT_SEL_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_PORT_SEL register.
 * The DEBUG_PORT_SEL register will be read and its value returned.
 * @return The current value of the DEBUG_PORT_SEL register.
 */
__INLINE uint32_t nxmac_debug_port_sel_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_PORT_SEL_ADDR);
}

/**
 * @brief Sets the DEBUG_PORT_SEL register to a value.
 * The DEBUG_PORT_SEL register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_debug_port_sel_set(uint32_t value)
{
    REG_PL_WR(NXMAC_DEBUG_PORT_SEL_ADDR, value);
}

// field definitions
/// DEBUG_PORT_SEL_2 field mask
#define NXMAC_DEBUG_PORT_SEL_2_MASK   ((uint32_t)0x0000FF00)
/// DEBUG_PORT_SEL_2 field LSB position
#define NXMAC_DEBUG_PORT_SEL_2_LSB    8
/// DEBUG_PORT_SEL_2 field width
#define NXMAC_DEBUG_PORT_SEL_2_WIDTH  ((uint32_t)0x00000008)
/// DEBUG_PORT_SEL_1 field mask
#define NXMAC_DEBUG_PORT_SEL_1_MASK   ((uint32_t)0x000000FF)
/// DEBUG_PORT_SEL_1 field LSB position
#define NXMAC_DEBUG_PORT_SEL_1_LSB    0
/// DEBUG_PORT_SEL_1 field width
#define NXMAC_DEBUG_PORT_SEL_1_WIDTH  ((uint32_t)0x00000008)

/// DEBUG_PORT_SEL_2 field reset value
#define NXMAC_DEBUG_PORT_SEL_2_RST    0x0
/// DEBUG_PORT_SEL_1 field reset value
#define NXMAC_DEBUG_PORT_SEL_1_RST    0x0

/**
 * @brief Constructs a value for the DEBUG_PORT_SEL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] debugportsel2 - The value to use for the debugPortSel2 field.
 * @param[in] debugportsel1 - The value to use for the debugPortSel1 field.
 */
__INLINE void nxmac_debug_port_sel_pack(uint8_t debugportsel2, uint8_t debugportsel1)
{
    ASSERT_ERR((((uint32_t)debugportsel2 << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)debugportsel1 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_DEBUG_PORT_SEL_ADDR,  ((uint32_t)debugportsel2 << 8) | ((uint32_t)debugportsel1 << 0));
}

/**
 * @brief Unpacks DEBUG_PORT_SEL's fields from current value of the DEBUG_PORT_SEL register.
 *
 * Reads the DEBUG_PORT_SEL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] debugportsel2 - Will be populated with the current value of this field from the register.
 * @param[out] debugportsel1 - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_debug_port_sel_unpack(uint8_t *debugportsel2, uint8_t *debugportsel1)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_PORT_SEL_ADDR);

    *debugportsel2 = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *debugportsel1 = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the debugPortSel2 field in the DEBUG_PORT_SEL register.
 *
 * The DEBUG_PORT_SEL register will be read and the debugPortSel2 field's value will be returned.
 *
 * @return The current value of the debugPortSel2 field in the DEBUG_PORT_SEL register.
 */
__INLINE uint8_t nxmac_debug_port_sel_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_PORT_SEL_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the debugPortSel2 field of the DEBUG_PORT_SEL register.
 *
 * The DEBUG_PORT_SEL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] debugportsel2 - The value to set the field to.
 */
__INLINE void nxmac_debug_port_sel_2_setf(uint8_t debugportsel2)
{
    ASSERT_ERR((((uint32_t)debugportsel2 << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(NXMAC_DEBUG_PORT_SEL_ADDR, (REG_PL_RD(NXMAC_DEBUG_PORT_SEL_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)debugportsel2 << 8));
}

/**
 * @brief Returns the current value of the debugPortSel1 field in the DEBUG_PORT_SEL register.
 *
 * The DEBUG_PORT_SEL register will be read and the debugPortSel1 field's value will be returned.
 *
 * @return The current value of the debugPortSel1 field in the DEBUG_PORT_SEL register.
 */
__INLINE uint8_t nxmac_debug_port_sel_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_PORT_SEL_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the debugPortSel1 field of the DEBUG_PORT_SEL register.
 *
 * The DEBUG_PORT_SEL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] debugportsel1 - The value to set the field to.
 */
__INLINE void nxmac_debug_port_sel_1_setf(uint8_t debugportsel1)
{
    ASSERT_ERR((((uint32_t)debugportsel1 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(NXMAC_DEBUG_PORT_SEL_ADDR, (REG_PL_RD(NXMAC_DEBUG_PORT_SEL_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)debugportsel1 << 0));
}

/// @}

/**
 * @name DEBUG_NAV register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  25:00           navCounter   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_NAV register
#define NXMAC_DEBUG_NAV_ADDR   0xC0000514
/// Offset of the DEBUG_NAV register from the base address
#define NXMAC_DEBUG_NAV_OFFSET 0x00000514
/// Index of the DEBUG_NAV register
#define NXMAC_DEBUG_NAV_INDEX  0x00000145
/// Reset value of the DEBUG_NAV register
#define NXMAC_DEBUG_NAV_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_NAV register.
 * The DEBUG_NAV register will be read and its value returned.
 * @return The current value of the DEBUG_NAV register.
 */
__INLINE uint32_t nxmac_debug_nav_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_NAV_ADDR);
}

/**
 * @brief Sets the DEBUG_NAV register to a value.
 * The DEBUG_NAV register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_debug_nav_set(uint32_t value)
{
    REG_PL_WR(NXMAC_DEBUG_NAV_ADDR, value);
}

// field definitions
/// NAV_COUNTER field mask
#define NXMAC_NAV_COUNTER_MASK   ((uint32_t)0x03FFFFFF)
/// NAV_COUNTER field LSB position
#define NXMAC_NAV_COUNTER_LSB    0
/// NAV_COUNTER field width
#define NXMAC_NAV_COUNTER_WIDTH  ((uint32_t)0x0000001A)

/// NAV_COUNTER field reset value
#define NXMAC_NAV_COUNTER_RST    0x0

/**
 * @brief Returns the current value of the navCounter field in the DEBUG_NAV register.
 *
 * The DEBUG_NAV register will be read and the navCounter field's value will be returned.
 *
 * @return The current value of the navCounter field in the DEBUG_NAV register.
 */
__INLINE uint32_t nxmac_nav_counter_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_NAV_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x03FFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the navCounter field of the DEBUG_NAV register.
 *
 * The DEBUG_NAV register will be read, modified to contain the new field value, and written.
 *
 * @param[in] navcounter - The value to set the field to.
 */
__INLINE void nxmac_nav_counter_setf(uint32_t navcounter)
{
    ASSERT_ERR((((uint32_t)navcounter << 0) & ~((uint32_t)0x03FFFFFF)) == 0);
    REG_PL_WR(NXMAC_DEBUG_NAV_ADDR, (uint32_t)navcounter << 0);
}

/// @}

/**
 * @name DEBUG_CW register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  25:24        backoffOffset   0x0
 *  17:16             activeAC   0x0
 *  15:12           currentCW3   0x2
 *  11:08           currentCW2   0x3
 *  07:04           currentCW1   0x4
 *  03:00           currentCW0   0x4
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_CW register
#define NXMAC_DEBUG_CW_ADDR   0xC0000518
/// Offset of the DEBUG_CW register from the base address
#define NXMAC_DEBUG_CW_OFFSET 0x00000518
/// Index of the DEBUG_CW register
#define NXMAC_DEBUG_CW_INDEX  0x00000146
/// Reset value of the DEBUG_CW register
#define NXMAC_DEBUG_CW_RESET  0x00002344

/**
 * @brief Returns the current value of the DEBUG_CW register.
 * The DEBUG_CW register will be read and its value returned.
 * @return The current value of the DEBUG_CW register.
 */
__INLINE uint32_t nxmac_debug_cw_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_CW_ADDR);
}

/**
 * @brief Sets the DEBUG_CW register to a value.
 * The DEBUG_CW register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_debug_cw_set(uint32_t value)
{
    REG_PL_WR(NXMAC_DEBUG_CW_ADDR, value);
}

// field definitions
/// BACKOFF_OFFSET field mask
#define NXMAC_BACKOFF_OFFSET_MASK   ((uint32_t)0x03000000)
/// BACKOFF_OFFSET field LSB position
#define NXMAC_BACKOFF_OFFSET_LSB    24
/// BACKOFF_OFFSET field width
#define NXMAC_BACKOFF_OFFSET_WIDTH  ((uint32_t)0x00000002)
/// ACTIVE_AC field mask
#define NXMAC_ACTIVE_AC_MASK        ((uint32_t)0x00030000)
/// ACTIVE_AC field LSB position
#define NXMAC_ACTIVE_AC_LSB         16
/// ACTIVE_AC field width
#define NXMAC_ACTIVE_AC_WIDTH       ((uint32_t)0x00000002)
/// CURRENT_CW_3 field mask
#define NXMAC_CURRENT_CW_3_MASK     ((uint32_t)0x0000F000)
/// CURRENT_CW_3 field LSB position
#define NXMAC_CURRENT_CW_3_LSB      12
/// CURRENT_CW_3 field width
#define NXMAC_CURRENT_CW_3_WIDTH    ((uint32_t)0x00000004)
/// CURRENT_CW_2 field mask
#define NXMAC_CURRENT_CW_2_MASK     ((uint32_t)0x00000F00)
/// CURRENT_CW_2 field LSB position
#define NXMAC_CURRENT_CW_2_LSB      8
/// CURRENT_CW_2 field width
#define NXMAC_CURRENT_CW_2_WIDTH    ((uint32_t)0x00000004)
/// CURRENT_CW_1 field mask
#define NXMAC_CURRENT_CW_1_MASK     ((uint32_t)0x000000F0)
/// CURRENT_CW_1 field LSB position
#define NXMAC_CURRENT_CW_1_LSB      4
/// CURRENT_CW_1 field width
#define NXMAC_CURRENT_CW_1_WIDTH    ((uint32_t)0x00000004)
/// CURRENT_CW_0 field mask
#define NXMAC_CURRENT_CW_0_MASK     ((uint32_t)0x0000000F)
/// CURRENT_CW_0 field LSB position
#define NXMAC_CURRENT_CW_0_LSB      0
/// CURRENT_CW_0 field width
#define NXMAC_CURRENT_CW_0_WIDTH    ((uint32_t)0x00000004)

/// BACKOFF_OFFSET field reset value
#define NXMAC_BACKOFF_OFFSET_RST    0x0
/// ACTIVE_AC field reset value
#define NXMAC_ACTIVE_AC_RST         0x0
/// CURRENT_CW_3 field reset value
#define NXMAC_CURRENT_CW_3_RST      0x2
/// CURRENT_CW_2 field reset value
#define NXMAC_CURRENT_CW_2_RST      0x3
/// CURRENT_CW_1 field reset value
#define NXMAC_CURRENT_CW_1_RST      0x4
/// CURRENT_CW_0 field reset value
#define NXMAC_CURRENT_CW_0_RST      0x4

/**
 * @brief Unpacks DEBUG_CW's fields from current value of the DEBUG_CW register.
 *
 * Reads the DEBUG_CW register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] backoffoffset - Will be populated with the current value of this field from the register.
 * @param[out] activeac - Will be populated with the current value of this field from the register.
 * @param[out] currentcw3 - Will be populated with the current value of this field from the register.
 * @param[out] currentcw2 - Will be populated with the current value of this field from the register.
 * @param[out] currentcw1 - Will be populated with the current value of this field from the register.
 * @param[out] currentcw0 - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_debug_cw_unpack(uint8_t *backoffoffset, uint8_t *activeac, uint8_t *currentcw3, uint8_t *currentcw2, uint8_t *currentcw1, uint8_t *currentcw0)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_CW_ADDR);

    *backoffoffset = (localVal & ((uint32_t)0x03000000)) >> 24;
    *activeac = (localVal & ((uint32_t)0x00030000)) >> 16;
    *currentcw3 = (localVal & ((uint32_t)0x0000F000)) >> 12;
    *currentcw2 = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *currentcw1 = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *currentcw0 = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the backoffOffset field in the DEBUG_CW register.
 *
 * The DEBUG_CW register will be read and the backoffOffset field's value will be returned.
 *
 * @return The current value of the backoffOffset field in the DEBUG_CW register.
 */
__INLINE uint8_t nxmac_backoff_offset_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_CW_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

/**
 * @brief Sets the backoffOffset field of the DEBUG_CW register.
 *
 * The DEBUG_CW register will be read, modified to contain the new field value, and written.
 *
 * @param[in] backoffoffset - The value to set the field to.
 */
__INLINE void nxmac_backoff_offset_setf(uint8_t backoffoffset)
{
    ASSERT_ERR((((uint32_t)backoffoffset << 24) & ~((uint32_t)0x03000000)) == 0);
    REG_PL_WR(NXMAC_DEBUG_CW_ADDR, (uint32_t)backoffoffset << 24);
}

/**
 * @brief Returns the current value of the activeAC field in the DEBUG_CW register.
 *
 * The DEBUG_CW register will be read and the activeAC field's value will be returned.
 *
 * @return The current value of the activeAC field in the DEBUG_CW register.
 */
__INLINE uint8_t nxmac_active_ac_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_CW_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

/**
 * @brief Returns the current value of the currentCW3 field in the DEBUG_CW register.
 *
 * The DEBUG_CW register will be read and the currentCW3 field's value will be returned.
 *
 * @return The current value of the currentCW3 field in the DEBUG_CW register.
 */
__INLINE uint8_t nxmac_current_cw_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_CW_ADDR);
    return ((localVal & ((uint32_t)0x0000F000)) >> 12);
}

/**
 * @brief Returns the current value of the currentCW2 field in the DEBUG_CW register.
 *
 * The DEBUG_CW register will be read and the currentCW2 field's value will be returned.
 *
 * @return The current value of the currentCW2 field in the DEBUG_CW register.
 */
__INLINE uint8_t nxmac_current_cw_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_CW_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Returns the current value of the currentCW1 field in the DEBUG_CW register.
 *
 * The DEBUG_CW register will be read and the currentCW1 field's value will be returned.
 *
 * @return The current value of the currentCW1 field in the DEBUG_CW register.
 */
__INLINE uint8_t nxmac_current_cw_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_CW_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Returns the current value of the currentCW0 field in the DEBUG_CW register.
 *
 * The DEBUG_CW register will be read and the currentCW0 field's value will be returned.
 *
 * @return The current value of the currentCW0 field in the DEBUG_CW register.
 */
__INLINE uint8_t nxmac_current_cw_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_CW_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/// @}

/**
 * @name DEBUG_QSRC register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:24              ac3QSRC   0x0
 *  23:16              ac2QSRC   0x0
 *  15:08              ac1QSRC   0x0
 *  07:00              ac0QSRC   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_QSRC register
#define NXMAC_DEBUG_QSRC_ADDR   0xC000051C
/// Offset of the DEBUG_QSRC register from the base address
#define NXMAC_DEBUG_QSRC_OFFSET 0x0000051C
/// Index of the DEBUG_QSRC register
#define NXMAC_DEBUG_QSRC_INDEX  0x00000147
/// Reset value of the DEBUG_QSRC register
#define NXMAC_DEBUG_QSRC_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_QSRC register.
 * The DEBUG_QSRC register will be read and its value returned.
 * @return The current value of the DEBUG_QSRC register.
 */
__INLINE uint32_t nxmac_debug_qsrc_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_QSRC_ADDR);
}

// field definitions
/// AC_3QSRC field mask
#define NXMAC_AC_3QSRC_MASK   ((uint32_t)0xFF000000)
/// AC_3QSRC field LSB position
#define NXMAC_AC_3QSRC_LSB    24
/// AC_3QSRC field width
#define NXMAC_AC_3QSRC_WIDTH  ((uint32_t)0x00000008)
/// AC_2QSRC field mask
#define NXMAC_AC_2QSRC_MASK   ((uint32_t)0x00FF0000)
/// AC_2QSRC field LSB position
#define NXMAC_AC_2QSRC_LSB    16
/// AC_2QSRC field width
#define NXMAC_AC_2QSRC_WIDTH  ((uint32_t)0x00000008)
/// AC_1QSRC field mask
#define NXMAC_AC_1QSRC_MASK   ((uint32_t)0x0000FF00)
/// AC_1QSRC field LSB position
#define NXMAC_AC_1QSRC_LSB    8
/// AC_1QSRC field width
#define NXMAC_AC_1QSRC_WIDTH  ((uint32_t)0x00000008)
/// AC_0QSRC field mask
#define NXMAC_AC_0QSRC_MASK   ((uint32_t)0x000000FF)
/// AC_0QSRC field LSB position
#define NXMAC_AC_0QSRC_LSB    0
/// AC_0QSRC field width
#define NXMAC_AC_0QSRC_WIDTH  ((uint32_t)0x00000008)

/// AC_3QSRC field reset value
#define NXMAC_AC_3QSRC_RST    0x0
/// AC_2QSRC field reset value
#define NXMAC_AC_2QSRC_RST    0x0
/// AC_1QSRC field reset value
#define NXMAC_AC_1QSRC_RST    0x0
/// AC_0QSRC field reset value
#define NXMAC_AC_0QSRC_RST    0x0

/**
 * @brief Unpacks DEBUG_QSRC's fields from current value of the DEBUG_QSRC register.
 *
 * Reads the DEBUG_QSRC register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] ac3qsrc - Will be populated with the current value of this field from the register.
 * @param[out] ac2qsrc - Will be populated with the current value of this field from the register.
 * @param[out] ac1qsrc - Will be populated with the current value of this field from the register.
 * @param[out] ac0qsrc - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_debug_qsrc_unpack(uint8_t *ac3qsrc, uint8_t *ac2qsrc, uint8_t *ac1qsrc, uint8_t *ac0qsrc)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_QSRC_ADDR);

    *ac3qsrc = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *ac2qsrc = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *ac1qsrc = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *ac0qsrc = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the ac3QSRC field in the DEBUG_QSRC register.
 *
 * The DEBUG_QSRC register will be read and the ac3QSRC field's value will be returned.
 *
 * @return The current value of the ac3QSRC field in the DEBUG_QSRC register.
 */
__INLINE uint8_t nxmac_ac_3qsrc_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_QSRC_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Returns the current value of the ac2QSRC field in the DEBUG_QSRC register.
 *
 * The DEBUG_QSRC register will be read and the ac2QSRC field's value will be returned.
 *
 * @return The current value of the ac2QSRC field in the DEBUG_QSRC register.
 */
__INLINE uint8_t nxmac_ac_2qsrc_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_QSRC_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Returns the current value of the ac1QSRC field in the DEBUG_QSRC register.
 *
 * The DEBUG_QSRC register will be read and the ac1QSRC field's value will be returned.
 *
 * @return The current value of the ac1QSRC field in the DEBUG_QSRC register.
 */
__INLINE uint8_t nxmac_ac_1qsrc_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_QSRC_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Returns the current value of the ac0QSRC field in the DEBUG_QSRC register.
 *
 * The DEBUG_QSRC register will be read and the ac0QSRC field's value will be returned.
 *
 * @return The current value of the ac0QSRC field in the DEBUG_QSRC register.
 */
__INLINE uint8_t nxmac_ac_0qsrc_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_QSRC_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/// @}

/**
 * @name DEBUG_QLRC register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:24              ac3QLRC   0x0
 *  23:16              ac2QLRC   0x0
 *  15:08              ac1QLRC   0x0
 *  07:00              ac0QLRC   0x0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_QLRC register
#define NXMAC_DEBUG_QLRC_ADDR   0xC0000520
/// Offset of the DEBUG_QLRC register from the base address
#define NXMAC_DEBUG_QLRC_OFFSET 0x00000520
/// Index of the DEBUG_QLRC register
#define NXMAC_DEBUG_QLRC_INDEX  0x00000148
/// Reset value of the DEBUG_QLRC register
#define NXMAC_DEBUG_QLRC_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_QLRC register.
 * The DEBUG_QLRC register will be read and its value returned.
 * @return The current value of the DEBUG_QLRC register.
 */
__INLINE uint32_t nxmac_debug_qlrc_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_QLRC_ADDR);
}

// field definitions
/// AC_3QLRC field mask
#define NXMAC_AC_3QLRC_MASK   ((uint32_t)0xFF000000)
/// AC_3QLRC field LSB position
#define NXMAC_AC_3QLRC_LSB    24
/// AC_3QLRC field width
#define NXMAC_AC_3QLRC_WIDTH  ((uint32_t)0x00000008)
/// AC_2QLRC field mask
#define NXMAC_AC_2QLRC_MASK   ((uint32_t)0x00FF0000)
/// AC_2QLRC field LSB position
#define NXMAC_AC_2QLRC_LSB    16
/// AC_2QLRC field width
#define NXMAC_AC_2QLRC_WIDTH  ((uint32_t)0x00000008)
/// AC_1QLRC field mask
#define NXMAC_AC_1QLRC_MASK   ((uint32_t)0x0000FF00)
/// AC_1QLRC field LSB position
#define NXMAC_AC_1QLRC_LSB    8
/// AC_1QLRC field width
#define NXMAC_AC_1QLRC_WIDTH  ((uint32_t)0x00000008)
/// AC_0QLRC field mask
#define NXMAC_AC_0QLRC_MASK   ((uint32_t)0x000000FF)
/// AC_0QLRC field LSB position
#define NXMAC_AC_0QLRC_LSB    0
/// AC_0QLRC field width
#define NXMAC_AC_0QLRC_WIDTH  ((uint32_t)0x00000008)

/// AC_3QLRC field reset value
#define NXMAC_AC_3QLRC_RST    0x0
/// AC_2QLRC field reset value
#define NXMAC_AC_2QLRC_RST    0x0
/// AC_1QLRC field reset value
#define NXMAC_AC_1QLRC_RST    0x0
/// AC_0QLRC field reset value
#define NXMAC_AC_0QLRC_RST    0x0

/**
 * @brief Unpacks DEBUG_QLRC's fields from current value of the DEBUG_QLRC register.
 *
 * Reads the DEBUG_QLRC register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] ac3qlrc - Will be populated with the current value of this field from the register.
 * @param[out] ac2qlrc - Will be populated with the current value of this field from the register.
 * @param[out] ac1qlrc - Will be populated with the current value of this field from the register.
 * @param[out] ac0qlrc - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_debug_qlrc_unpack(uint8_t *ac3qlrc, uint8_t *ac2qlrc, uint8_t *ac1qlrc, uint8_t *ac0qlrc)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_QLRC_ADDR);

    *ac3qlrc = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *ac2qlrc = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *ac1qlrc = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *ac0qlrc = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the ac3QLRC field in the DEBUG_QLRC register.
 *
 * The DEBUG_QLRC register will be read and the ac3QLRC field's value will be returned.
 *
 * @return The current value of the ac3QLRC field in the DEBUG_QLRC register.
 */
__INLINE uint8_t nxmac_ac_3qlrc_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_QLRC_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Returns the current value of the ac2QLRC field in the DEBUG_QLRC register.
 *
 * The DEBUG_QLRC register will be read and the ac2QLRC field's value will be returned.
 *
 * @return The current value of the ac2QLRC field in the DEBUG_QLRC register.
 */
__INLINE uint8_t nxmac_ac_2qlrc_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_QLRC_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Returns the current value of the ac1QLRC field in the DEBUG_QLRC register.
 *
 * The DEBUG_QLRC register will be read and the ac1QLRC field's value will be returned.
 *
 * @return The current value of the ac1QLRC field in the DEBUG_QLRC register.
 */
__INLINE uint8_t nxmac_ac_1qlrc_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_QLRC_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Returns the current value of the ac0QLRC field in the DEBUG_QLRC register.
 *
 * The DEBUG_QLRC register will be read and the ac0QLRC field's value will be returned.
 *
 * @return The current value of the ac0QLRC field in the DEBUG_QLRC register.
 */
__INLINE uint8_t nxmac_ac_0qlrc_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_QLRC_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/// @}

/**
 * @name DEBUG_PHY register definitions
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     01   rxEndForTimingErrRec   0
 *     00   rxReqForceDeassertion   0
 * </pre>
 *
 * @{
 */

/// Address of the DEBUG_PHY register
#define NXMAC_DEBUG_PHY_ADDR   0xC000055C
/// Offset of the DEBUG_PHY register from the base address
#define NXMAC_DEBUG_PHY_OFFSET 0x0000055C
/// Index of the DEBUG_PHY register
#define NXMAC_DEBUG_PHY_INDEX  0x00000157
/// Reset value of the DEBUG_PHY register
#define NXMAC_DEBUG_PHY_RESET  0x00000000

/**
 * @brief Returns the current value of the DEBUG_PHY register.
 * The DEBUG_PHY register will be read and its value returned.
 * @return The current value of the DEBUG_PHY register.
 */
__INLINE uint32_t nxmac_debug_phy_get(void)
{
    return REG_PL_RD(NXMAC_DEBUG_PHY_ADDR);
}

/**
 * @brief Sets the DEBUG_PHY register to a value.
 * The DEBUG_PHY register will be written.
 * @param value - The value to write.
 */
__INLINE void nxmac_debug_phy_set(uint32_t value)
{
    REG_PL_WR(NXMAC_DEBUG_PHY_ADDR, value);
}

// field definitions
/// RX_END_FOR_TIMING_ERR_REC field bit
#define NXMAC_RX_END_FOR_TIMING_ERR_REC_BIT    ((uint32_t)0x00000002)
/// RX_END_FOR_TIMING_ERR_REC field position
#define NXMAC_RX_END_FOR_TIMING_ERR_REC_POS    1
/// RX_REQ_FORCE_DEASSERTION field bit
#define NXMAC_RX_REQ_FORCE_DEASSERTION_BIT     ((uint32_t)0x00000001)
/// RX_REQ_FORCE_DEASSERTION field position
#define NXMAC_RX_REQ_FORCE_DEASSERTION_POS     0

/// RX_END_FOR_TIMING_ERR_REC field reset value
#define NXMAC_RX_END_FOR_TIMING_ERR_REC_RST    0x0
/// RX_REQ_FORCE_DEASSERTION field reset value
#define NXMAC_RX_REQ_FORCE_DEASSERTION_RST     0x0

/**
 * @brief Constructs a value for the DEBUG_PHY register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxendfortimingerrrec - The value to use for the rxEndForTimingErrRec field.
 * @param[in] rxreqforcedeassertion - The value to use for the rxReqForceDeassertion field.
 */
__INLINE void nxmac_debug_phy_pack(uint8_t rxendfortimingerrrec, uint8_t rxreqforcedeassertion)
{
    ASSERT_ERR((((uint32_t)rxendfortimingerrrec << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)rxreqforcedeassertion << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_DEBUG_PHY_ADDR,  ((uint32_t)rxendfortimingerrrec << 1) | ((uint32_t)rxreqforcedeassertion << 0));
}

/**
 * @brief Unpacks DEBUG_PHY's fields from current value of the DEBUG_PHY register.
 *
 * Reads the DEBUG_PHY register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxendfortimingerrrec - Will be populated with the current value of this field from the register.
 * @param[out] rxreqforcedeassertion - Will be populated with the current value of this field from the register.
 */
__INLINE void nxmac_debug_phy_unpack(uint8_t *rxendfortimingerrrec, uint8_t *rxreqforcedeassertion)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_PHY_ADDR);

    *rxendfortimingerrrec = (localVal & ((uint32_t)0x00000002)) >> 1;
    *rxreqforcedeassertion = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the rxEndForTimingErrRec field in the DEBUG_PHY register.
 *
 * The DEBUG_PHY register will be read and the rxEndForTimingErrRec field's value will be returned.
 *
 * @return The current value of the rxEndForTimingErrRec field in the DEBUG_PHY register.
 */
__INLINE uint8_t nxmac_rx_end_for_timing_err_rec_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_PHY_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the rxEndForTimingErrRec field of the DEBUG_PHY register.
 *
 * The DEBUG_PHY register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxendfortimingerrrec - The value to set the field to.
 */
__INLINE void nxmac_rx_end_for_timing_err_rec_setf(uint8_t rxendfortimingerrrec)
{
    ASSERT_ERR((((uint32_t)rxendfortimingerrrec << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(NXMAC_DEBUG_PHY_ADDR, (REG_PL_RD(NXMAC_DEBUG_PHY_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)rxendfortimingerrrec << 1));
}

/**
 * @brief Returns the current value of the rxReqForceDeassertion field in the DEBUG_PHY register.
 *
 * The DEBUG_PHY register will be read and the rxReqForceDeassertion field's value will be returned.
 *
 * @return The current value of the rxReqForceDeassertion field in the DEBUG_PHY register.
 */
__INLINE uint8_t nxmac_rx_req_force_deassertion_getf(void)
{
    uint32_t localVal = REG_PL_RD(NXMAC_DEBUG_PHY_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the rxReqForceDeassertion field of the DEBUG_PHY register.
 *
 * The DEBUG_PHY register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxreqforcedeassertion - The value to set the field to.
 */
__INLINE void nxmac_rx_req_force_deassertion_setf(uint8_t rxreqforcedeassertion)
{
    ASSERT_ERR((((uint32_t)rxreqforcedeassertion << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(NXMAC_DEBUG_PHY_ADDR, (REG_PL_RD(NXMAC_DEBUG_PHY_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)rxreqforcedeassertion << 0));
}
/*lint +e91*/

/// @}

#endif // _REG_MAC_CORE_H_

/// @}

