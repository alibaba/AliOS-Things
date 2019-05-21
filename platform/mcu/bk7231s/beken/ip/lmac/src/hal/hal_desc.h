/**
 ****************************************************************************************
 *
 * @file hal_desc.h
 *
 * @brief File containing the definition of HW descriptors.
 *
 * File containing the definition of the structure and API function used to initialise the pool.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _HAL_DESC_H_
#define _HAL_DESC_H_

/**
 *****************************************************************************************
 * @defgroup HWDESC HWDESC
 * @ingroup HAL
 * @brief HW descriptor definitions
 * @{
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"
// for mac_addr
#include "mac.h"
// for frame length
#include "mac_common.h"
// for mac frame definitions
#include "mac_frame.h"
// for dma_desc
#include "dma.h"
// for CO_BIT
#include "co_math.h"
// for co_list_hdr
#include "co_list.h"
// for phy_channel_info
#include "phy.h"
// for DBG info format
#if (NX_RADAR_DETECT)
// for hal_dma_desc
#include "hal_dma.h"
#endif //(NX_RADAR_DETECT)

/*
 * DEFINES
 ****************************************************************************************
 */
/// Number of pulses in a radar event structure
#define RADAR_PULSE_MAX   4
/// Number of radar event structures
#define RADAR_EVENT_MAX   10

/// Minimum number of RX descriptors
#define HAL_MIN_RX_DESC_CNT           (NX_RXDESC_CNT + 2)

/// Number of Descriptors for IPC transfers between the embedded and the host
#if NX_REORD && ((2 * NX_REORD_BUF_SIZE) > HAL_MIN_RX_DESC_CNT)
	#define HAL_RXDESC_CNT            (2 * NX_REORD_BUF_SIZE)
#else
	#define HAL_RXDESC_CNT            HAL_MIN_RX_DESC_CNT
#endif

/// uPattern for TX header descriptor.
#define TX_HEADER_DESC_PATTERN 0xCAFEBABE
/// uPattern for TX buffer descriptor
#define TX_PAYLOAD_DESC_PATTERN 0xCAFEFADE
/// uPattern for RX header descriptor.
#define RX_HEADER_DESC_PATTERN 0xBAADF00D
/// uPattern for RX payload descriptor.
#define RX_PAYLOAD_DESC_PATTERN 0xC0DEDBAD

//----------------------------------------------------------------------------------------
// TBD: BUF CONTROL INFO definitions
//----------------------------------------------------------------------------------------
/// Flag indicating if HW handled the buffer
#define TBD_DONE_HW             CO_BIT(31)
/// Bit allowing to request HW to generate an interrupt upon a payload buffer transmission
#define TBD_INTERRUPT_EN        CO_BIT(0)

//----------------------------------------------------------------------------------------
// THD: PHY CONTROL INFO definitions
//----------------------------------------------------------------------------------------
/// Sounding of PPDU Frame Transmission (Bit 0)
#define SOUNDING_TX_BIT         CO_BIT(0)
/// Smoothing for PPDU frames (Bit 1)
#define SMTHN_TX_BIT            CO_BIT(1)
/// Smoothing for Control frames (Bit 2)
#define SMTHN_PROT_BIT          CO_BIT(2)
/// Use BW signaling bit
#define USE_BW_SIG_TX_BIT       CO_BIT(3)
/// Dynamic BW
#define DYN_BW_TX_BIT           CO_BIT(4)
/// Doze allowed by AP during TXOP
#define DOZE_ALLOWED_TX_BIT     CO_BIT(5)
/// Continuous transmit
#define CONT_TX_BIT             CO_BIT(6)
/// User Position field offset
#define USER_POS_OFT            12
/// User Position field mask
#define USER_POS_MASK           (0x3 << USER_POS_OFT)
/// Use RIFS for Transmission (Bit 14)
#define USE_RIFS_TX_BIT         CO_BIT(14)
/// Use MU-MIMO for Transmission (Bit 15)
#define USE_MUMIMO_TX_BIT       CO_BIT(15)
/// GroupId field offset
#define GID_TX_OFT              16
/// GroupId field mask
#define GID_TX_MASK             (0x3F << GID_TX_OFT)
/// Partial AID field offset
#define PAID_TX_OFT             22
/// Partial AID field mask
#define PAID_TX_MASK            (0x1FF << PAID_TX_OFT)

//----------------------------------------------------------------------------------------
// THD: MAC CONTROL INFO 1 definitions
//----------------------------------------------------------------------------------------
/// Protection Frame Duration offset
#define PROT_FRM_DURATION_OFT             16
/// Protection Frame Duration mask
#define PROT_FRM_DURATION_MSK             (0xFFFF << PROT_FRM_DURATION_OFT)

/// Set if ACK has to be passed to SW
#define WRITE_ACK                         CO_BIT(14)
/// Set if lower rates have to be used for retries
#define LOW_RATE_RETRY                    CO_BIT(13)
/// L-SIG TXOP Protection for protection frame
#define LSTP_PROT                         CO_BIT(12)
/// L-SIG TXOP Protection
#define LSTP                              CO_BIT(11)

// Expected Acknowledgment
/// Expected Acknowledgment offset
#define EXPECTED_ACK_OFT                  9
/// Expected Acknowledgment mask
#define EXPECTED_ACK_MSK                  (0x3 << EXPECTED_ACK_OFT)
/// No acknowledgment
#define EXPECTED_ACK_NO_ACK               (0x0 << EXPECTED_ACK_OFT)
/// Normal acknowledgment
#define EXPECTED_ACK_NORMAL_ACK           (0x1 << EXPECTED_ACK_OFT)
/// Uncompressed block acknowledgment
#define EXPECTED_ACK_BLOCK_ACK            (0x2 << EXPECTED_ACK_OFT)
/// Compressed block acknowledgment
#define EXPECTED_ACK_COMPRESSED_BLOCK_ACK (0x3 << EXPECTED_ACK_OFT)

//----------------------------------------------------------------------------------------
// THD MACCTRLINFO2 fields
//----------------------------------------------------------------------------------------
/// Type and Subtype fields Valid bit
#define TS_VALID_BIT                      CO_BIT(0)

/// Type field offset
#define FRM_TYPE_OFT                      1
/// Type field mask
#define FRM_TYPE_MSK                      (0x3<<FRM_TYPE_OFT)

/// Management type
#define FRM_TYPE_MNG                      (0<<FRM_TYPE_OFT)
/// Control type
#define FRM_TYPE_CNTL                     (1<<FRM_TYPE_OFT)
/// Data type
#define FRM_TYPE_DATA                     (2<<FRM_TYPE_OFT)

/// Subtype field offset
#define FRM_SUBTYPE_OFT                   3
/// Subtype field mask
#define FRM_SUBTYPE_MSK                   (0xF<<FRM_SUBTYPE_OFT)

/// BAR subtype
#define FRM_CNTL_SUBTYPE_BAR              (8<<FRM_SUBTYPE_OFT)
/// BA subtype
#define FRM_CNTL_SUBTYPE_BA               (9<<FRM_SUBTYPE_OFT)

/// Bit indicating if an interrupt has to be set or not once packet is transmitted
#define INTERRUPT_EN_TX                   CO_BIT(8)

/// Offset of Number of Blank delimiters
#define NB_BLANK_DELIM_OFT                9
/// Mask of Number of Blank delimiters
#define NB_BLANK_DELIM_MSK                (0x3FF << NB_BLANK_DELIM_OFT)

/// WhichDescriptor definition - contains aMPDU bit and position value
/// Offset of WhichDescriptor field in the MAC CONTROL INFO 2 word
#define WHICHDESC_OFT                     19
/// Mask of the WhichDescriptor field
#define WHICHDESC_MSK                     (0x07 << WHICHDESC_OFT)
/// Only 1 THD possible, describing an unfragmented MSDU
#define WHICHDESC_UNFRAGMENTED_MSDU       (0x00 << WHICHDESC_OFT)
/// THD describing the first MPDU of a fragmented MSDU
#define WHICHDESC_FRAGMENTED_MSDU_FIRST   (0x01 << WHICHDESC_OFT)
/// THD describing intermediate MPDUs of a fragmented MSDU
#define WHICHDESC_FRAGMENTED_MSDU_INT     (0x02 << WHICHDESC_OFT)
/// THD describing the last MPDU of a fragmented MSDU
#define WHICHDESC_FRAGMENTED_MSDU_LAST    (0x03 << WHICHDESC_OFT)
/// THD for extra descriptor starting an AMPDU
#define WHICHDESC_AMPDU_EXTRA             (0x04 << WHICHDESC_OFT)
/// THD describing the first MPDU of an A-MPDU
#define WHICHDESC_AMPDU_FIRST             (0x05 << WHICHDESC_OFT)
/// THD describing intermediate MPDUs of an A-MPDU
#define WHICHDESC_AMPDU_INT               (0x06 << WHICHDESC_OFT)
/// THD describing the last MPDU of an A-MPDU
#define WHICHDESC_AMPDU_LAST              (0x07 << WHICHDESC_OFT)

/// aMPDU bit offset
#define AMPDU_OFT                         21
/// aMPDU bit
#define AMPDU_BIT                         CO_BIT(AMPDU_OFT)

/// Under BA setup bit
#define UNDER_BA_SETUP_BIT                CO_BIT(22)

/// Don't touch duration bit
#define DONT_TOUCH_DUR                    CO_BIT(28)


//----------------------------------------------------------------------------------------
//THD STATINFO fields
//----------------------------------------------------------------------------------------
/// Number of RTS frame retries offset
#define NUM_RTS_RETRIES_OFT                0
/// Number of RTS frame retries mask
#define NUM_RTS_RETRIES_MSK               (0xFF << NUM_RTS_RETRIES_OFT)
/// Number of MPDU frame retries offset
#define NUM_MPDU_RETRIES_OFT               8
/// Number of MPDU frame retries mask
#define NUM_MPDU_RETRIES_MSK              (0xFF << NUM_MPDU_RETRIES_OFT)
/// Retry limit reached: frame unsuccessful
#define RETRY_LIMIT_REACHED_BIT            CO_BIT(16)
/// Frame lifetime expired: frame unsuccessful
#define LIFETIME_EXPIRED_BIT               CO_BIT(17)
/// BA frame not received - valid only for MPDUs part of AMPDU
#define BA_FRAME_RECEIVED_BIT              CO_BIT(18)
/// Frame successful by TX DMA: Ack received successfully
#define FRAME_SUCCESSFUL_TX_BIT            CO_BIT(23)
/// Last MPDU of an A-MPDU
#define A_MPDU_LAST                        (0x0F << 26)
/// Transmission bandwidth offset
#define BW_TX_OFT                          24
/// Transmission bandwidth mask
#define BW_TX_MSK                          (0x3 << BW_TX_OFT)
/// Transmission bandwidth - 20MHz
#define BW_20MHZ_TX                        (0x0 << BW_TX_OFT)
/// Transmission bandwidth - 40MHz
#define BW_40MHZ_TX                        (0x1 << BW_TX_OFT)
/// Transmission bandwidth - 80MHz
#define BW_80MHZ_TX                        (0x2 << BW_TX_OFT)
/// Transmission bandwidth - 160MHz
#define BW_160MHZ_TX                       (0x3 << BW_TX_OFT)
/// Descriptor done bit: Set by HW for TX DMA
#define DESC_DONE_TX_BIT                   CO_BIT(31)
/// Descriptor done bit: Set by SW for TX DMA
#define DESC_DONE_SW_TX_BIT                CO_BIT(30)

/// @name Policy table definitions
/// @{

/// Number of rate control steps in the policy table
#define RATE_CONTROL_STEPS      4

/// uPattern for Policy Table
#define POLICY_TABLE_PATTERN    0xBADCAB1E

// Policy Table: PHY Control 1 Information field
/// Beam Forming Frame Exchange offset
#define BF_FORM_EXT_PT_OFT      3
/// Beam Forming Frame Exchange mask
#define BF_FORM_EXT_PT_MASK     (0x1 << BF_FORM_EXT_PT_OFT)
/// Number of Extension Spatial Streams offset
#define NO_EXTN_SS_PT_OFT       4
/// Number of Extension Spatial Streams mask
#define NO_EXTN_SS_PT_MASK      (0x3 << NO_EXTN_SS_PT_OFT)
/// FEC Coding offset
#define FEC_CODING_PT_OFT       6
/// FEC Coding bit
#define FEC_CODING_PT_BIT       CO_BIT(FEC_CODING_PT_OFT)
/// Space Time Block Coding offset
#define STBC_PT_OFT             7
/// Space Time Block Coding mask
#define STBC_PT_MASK            (0x3 << STBC_PT_OFT)
/// Number of Transmit Chains for PPDU offset
#define NX_TX_PT_OFT            14
/// Number of Transmit Chains for PPDU mask
#define NX_TX_PT_MASK           (0x7 << NX_TX_PT_OFT)
/// Number of Transmit Chains for Protection Frame offset
#define NX_TX_PROT_PT_OFT       17
/// Number of Transmit Chains for Protection Frame mask
#define NX_TX_PROT_PT_MASK      (0x7 << NX_TX_PROT_PT_OFT)

// Policy Table: PHY Control 2 Information field
/// Antenna Set offset
#define ANTENNA_SET_PT_OFT      0
/// Antenna Set mask
#define ANTENNA_SET_PT_MASK     0XFF
/// Spatial Map Matrix Index offset
#define SMM_INDEX_PT_OFT        8
/// Spatial Map Matrix Index mask
#define SMM_INDEX_PT_MASK       (0XFF << SMM_INDEX_PT_OFT)
/// Beamformed Offset
#define BMFED_OFT               16
/// Beamformed Bit
#define BMFED_BIT               CO_BIT(BMFED_OFT)

// Policy Table: MAC Control 1 Information field
/// Key Storage RAM Index offset
#define KEYSRAM_INDEX_OFT       0
/// Key Storage RAM Index mask
#define KEYSRAM_INDEX_MASK      (0X3FF << KEYSRAM_INDEX_OFT)
/// Key Storage RAM Index for Receiver Address offset
#define KEYSRAM_INDEX_RA_OFT    10
/// Key Storage RAM Index for Receiver Address mask
#define KEYSRAM_INDEX_RA_MASK   (0X3FF << KEYSRAM_INDEX_RA_OFT)

// Policy Table: MAC Control 2 Information field.
/// dot11LongRetryLimit MIB Value offset
#define LONG_RETRY_LIMIT_OFT    0
/// dot11LongRetryLimit MIB Value mask
#define LONG_RETRY_LIMIT_MASK   (0XFF << LONG_RETRY_LIMIT_OFT)
/// dot11ShortRetryLimit MIB Value offset
#define SHORT_RETRY_LIMIT_OFT   8
/// dot11ShortRetryLimit MIB Value mask
#define SHORT_RETRY_LIMIT_MASK  (0XFF << SHORT_RETRY_LIMIT_OFT)
/// dot11RtsThrshold MIB Value offset
#define RTS_THRSHOLD_OFT        16 // Bits 16-23
/// dot11RtsThrshold MIB Value mask
#define RTS_THRSHOLD_MASK       (0XFF << RTS_THRSHOLD_OFT)

// Policy Table: Rate Control Information field
/// MCS Index offset
#define MCS_INDEX_TX_RCX_OFT    0
/// MCS Index mask
#define MCS_INDEX_TX_RCX_MASK   (0X7FU << MCS_INDEX_TX_RCX_OFT)
/// Bandwidth for transmission offset
#define BW_TX_RCX_OFT           7
/// Bandwidth for transmission mask
#define BW_TX_RCX_MASK          (0X3U << BW_TX_RCX_OFT)
/// Short Guard Interval for Transmission offset
#define SHORT_GI_TX_RCX_OFT     9
/// Short Guard Interval for Transmission mask
#define SHORT_GI_TX_RCX_MASK    (0x1U << SHORT_GI_TX_RCX_OFT)
/// Preamble type for 11b Transmission offset
#define PRE_TYPE_TX_RCX_OFT     10
/// Preamble type for 11b Transmission mask
#define PRE_TYPE_TX_RCX_MASK    (0x1U << PRE_TYPE_TX_RCX_OFT)
/// Format of the modulation offset
#define FORMAT_MOD_TX_RCX_OFT   11
/// Format of the modulation mask
#define FORMAT_MOD_TX_RCX_MASK  (0X7U << FORMAT_MOD_TX_RCX_OFT)
/// Type of NAV protection frame exchange offset
#define PROT_FRM_EX_RCX_OFT     14
/// Type of NAV protection frame exchange mask
#define PROT_FRM_EX_RCX_MASK    (0X7 << PROT_FRM_EX_RCX_OFT)
/// No protection
#define PROT_NO_PROT            (0x0 << PROT_FRM_EX_RCX_OFT)
/// Self-CTS
#define PROT_SELF_CTS           (0x1 << PROT_FRM_EX_RCX_OFT)
/// RTS-CTS with intended receiver
#define PROT_RTS_CTS            (0x2 << PROT_FRM_EX_RCX_OFT)
/// RTS-CTS with QAP
#define PROT_RTS_CTS_WITH_QAP   (0x3 << PROT_FRM_EX_RCX_OFT)
/// STBC protection
#define PROT_STBC               (0x4 << PROT_FRM_EX_RCX_OFT)

/// MCS Index for protection frame offset
#define MCS_INDEX_PROT_TX_RCX_OFT  17
/// MCS Index for protection frame mask
#define MCS_INDEX_PROT_TX_RCX_MASK (0x7F << MCS_INDEX_PROT_TX_RCX_OFT)
/// Bandwidth for protection frame transmission offset
#define BW_PROT_TX_RCX_OFT      24
/// Bandwidth for protection frame transmission mask
#define BW_PROT_TX_RCX_MASK     (0x3 << BW_PROT_TX_RCX_OFT)
/// Format of the modulation for the protection frame offset
#define FORMAT_MOD_PROT_TX_RCX_OFT  26
/// Format of the modulation for the protection frame mask
#define FORMAT_MOD_PROT_TX_RCX_MASK (0x7 << FORMAT_MOD_PROT_TX_RCX_OFT)
/// Number of retries at this rate offset
#define N_RETRY_RCX_OFT         29
/// Number of retries at this rate mask
#define N_RETRY_RCX_MASK        (0x7U << N_RETRY_RCX_OFT)

// Values of the FORMAT_MOD fields
/// Non-HT format
#define FORMATMOD_NON_HT          0
/// Non-HT duplicate OFDM format
#define FORMATMOD_NON_HT_DUP_OFDM 1
/// HT mixed mode format
#define FORMATMOD_HT_MF           2
/// HT greenfield format
#define FORMATMOD_HT_GF           3
/// VHT format
#define FORMATMOD_VHT             4

/// 20 MHz bandwidth
#define BW_20MHZ                  0
/// 40 MHz bandwidth
#define BW_40MHZ                  1
/// 80 MHz bandwidth
#define BW_80MHZ                  2
/// 160 or 80+80 MHz bandwidth
#define BW_160MHZ                 3

// VHT specific NSS and MCS values
/// VHT Nss offset
#define VHT_NSS_OFT               4
/// VHT Nss mask
#define VHT_NSS_MASK              (0x7 << VHT_NSS_OFT)
/// VHT MCS offset
#define VHT_MCS_OFT               0
/// VHT MCS mask
#define VHT_MCS_MASK              (0xF << VHT_MCS_OFT)

// HT specific NSS and MCS values (valid only for MCS <= 31)
/// Offset of the NSS in a HT MCS value (valid for MCS <= 31)
#define HT_NSS_OFT               3
/// Mask of the NSS in a HT MCS value (valid for MCS <= 31)
#define HT_NSS_MASK              (0x3 << HT_NSS_OFT)
/// Offset of the MCS in a HT MCS value (valid for MCS <= 31)
#define HT_MCS_OFT               0
/// Mask of the MCS in a HT MCS value (valid for MCS <= 31)
#define HT_MCS_MASK              (0x7 << HT_MCS_OFT)

// Policy Table: Power Control Information field
/// Transmit Power Level for RCX offset
#define TX_PWR_LEVEL_PT_RCX_OFT         0
/// Transmit Power Level for RCX mask
#define TX_PWR_LEVEL_PT_RCX_MASK        (0xff << TX_PWR_LEVEL_PT_RCX_OFT)
/// Transmit Power Level of Protection for RCX offset
#define TX_PWR_LEVEL_PROT_PT_RCX_OFT    8
/// Transmit Power Level of Protection for RCX mask
#define TX_PWR_LEVEL_PROT_PT_RCX_MASK   (0xff << TX_PWR_LEVEL_PROT_PT_RCX_OFT)

/// @}

/// @name Compressed Policy table definitions
/// @{

// Compressed Policy Table: Secondary User Information field
/// MCS index offset
#define MCS_IDX_TX_CPT_OFT      0
/// MCS index mask
#define MCS_IDX_TX_CPT_MASK     (0x7F << MCS_IDX_TX_CPT_OFT)
/// FEC Coding offset
#define FEC_CODING_CPT_OFT      7
/// FEC Coding bit
#define FEC_CODING_CPT_BIT      CO_BIT(FEC_CODING_CPT_OFT)
/// Spatial Map Matrix Index offset
#define SMM_INDEX_CPT_OFT       8
/// Spatial Map Matrix Index mask
#define SMM_INDEX_CPT_MASK      (0XFF << SMM_INDEX_CPT_OFT)
/// Key Storage RAM Index offset
#define KEYSRAM_INDEX_CPT_OFT   16
/// Key Storage RAM Index mask
#define KEYSRAM_INDEX_CPT_MASK  (0X3FF << KEYSRAM_INDEX_OFT)

/// @}

/// @name RHD STATINFO
/// @{

/// Key index offset
#define KEY_IDX_OFT                       15
/// Key index mask
#define KEY_IDX_MSK                       (0x3FF << KEY_IDX_OFT)
/// Key index valid bit
#define KEY_IDX_VALID_BIT                 CO_BIT(25)
/// Immediate response access category offset
#define IMM_RSP_AC_OFT                    11
/// Immediate response access category mask
#define IMM_RSP_AC_MSK                    (0x03 << IMM_RSP_AC_OFT)

/// Last buffer of the MPDU
#define RX_PD_LASTBUF                     0x0001
/// Descriptor Done in HW
#define RX_PD_DONE                        0x0002

/// Storage RAM key index valid bit.
#define RX_HD_KEYIDV                      0x02000000
/// Storage RAM key index.
#define RX_HD_KEYID                       0x01FF8000
/// Lowest significant bit of the storage RAM key index.
#define RX_HD_KEYID_LSB                   15
/// Done bit.
#define RX_HD_DONE                        0x00004000
/// Frame successfully received bit.
#define RX_HD_SUCCESS                     0x00002000
/// Group Addressed frame bit.
#define RX_HD_GA_FRAME                    0x00000400
/// Address mismatch bit.
#define RX_HD_ADDRMIS                     0x00000200
/// FCS error bit.
#define RX_HD_FCSERR                      0x0100
/// PHY error bit.
#define RX_HD_PHYERR                      0x00000080
/// Undefined error bit.
#define RX_HD_UNDEFERR                    0x00000040
/// Decryption status mask.
#define RX_HD_DECRSTATUS                  0x0000001C
/// Is response frame bit.
#define RX_HD_RSP_FRM                     0x00000002
/// Vector 2 valid bit.
#define RX_HD_RXVEC2V                     0x00000001
/// Frame unencrypted.
#define RX_HD_DECR_UNENC                  0x0000
/// WEP/TKIP ICV failure.
#define RX_HD_DECR_ICVFAIL                0x0004
/// MAC CCMP failure.
#define RX_HD_DECR_CCMPFAIL               0x0008
/// MAC A-MSDU discarded at HW.
#define RX_HD_DECR_AMSDUDISCARD           0x000C
/// NULL key found.
#define RX_HD_DECR_NULLKEY                0x0010
/// MAC security type WEP.
#define RX_HD_DECR_WEPSUCCESS             0x0014
/// MAC security type TKIP.
#define RX_HD_DECR_TKIPSUCCESS            0x0018
/// MAC security type CCMP.
#define RX_HD_DECR_CCMPSUCCESS            0x001C
/// Macro to retrieve the storage RAM key index for the received frame.
/// @param[in] __s MPDU status information from the RX header descriptor.
#define RX_HD_KEYID_GET(__s) (((__s) & RX_HD_KEYID) >> RX_HD_KEYID_LSB)
/// Macro to retrieve the done bit of the received frame.
/// @param[in] __s MPDU status information from the RX header descriptor.
#define RX_HD_DONE_GET(__s) ((__s) & RX_HD_DONE)
/// Macro to retrieve the success bit of the received frame.
/// @param[in] __s MPDU status information from the RX header descriptor.
#define RX_HD_SUCCESS_GET(__s) ((__s) & (RX_HD_SUCCESS | RX_HD_FCSERR))

/// @}

/// Length of the receive vectors
#define RXL_HWDESC_RXV_LEN    40



/// @name RX VECTOR related definitions.
/// @{

/// Macro retrieving the legacy length of the RX vector
/// @param[in] __x RX Vector 1A value.
#define RXVEC1A_LEGLENGTH(__x)   (((__x) & 0x00000FFF))
/// Macro retrieving the legacy rate of the RX vector
/// @param[in] __x RX Vector 1A value.
#define RXVEC1A_LEGRATE(__x)     (((__x) & 0x0000F000) >> 12)
/// Macro retrieving the HT length of the RX vector
/// @param[in] __x RX Vector 1A value.
#define RXVEC1A_HTLENGTH(__x)    (((__x) & 0xFFFF0000) >> 16)
/// Macro retrieving the HT/VHT MCS of the RX vector
/// @param[in] __x RX Vector 1B value.
#define RXVEC1B_MCS(__x)         (((__x) & 0x0000007F))
/// Macro retrieving the preamble type of the RX vector
/// @param[in] __x RX Vector 1B value.
#define RXVEC1B_PRETYPE(__x)     (((__x) & 0x00000080))
/// Macro retrieving the modulation format of the RX vector
/// @param[in] __x RX Vector 1B value.
#define RXVEC1B_FORMATMOD(__x)   (((__x) & 0x00000300) >> 8)
/// Macro retrieving the channel bandwidth of the RX vector
/// @param[in] __x RX Vector 1B value.
#define RXVEC1B_CHBW(__x)        (((__x) & 0x00000400))
/// Macro retrieving the GI mode of the RX vector
/// @param[in] __x RX Vector 1B value.
#define RXVEC1B_SHORTGI(__x)     (((__x) & 0x00000800))
/// Macro retrieving the smoothing of the RX vector
/// @param[in] __x RX Vector 1B value.
#define RXVEC1B_SMOOTHING(__x)   (((__x) & 0x00001000))
/// Macro retrieving the SIG valid of the RX vector
/// @param[in] __x RX Vector 1B value.
#define RXVEC1B_SIGVALID(__x)    (((__x) & 0x00002000))
/// Macro retrieving the STBC mode of the RX vector
/// @param[in] __x RX Vector 1B value.
#define RXVEC1B_STBC(__x)        (((__x) & 0x0000C000) >> 14)
/// Macro retrieving the sounding bit of the RX vector
/// @param[in] __x RX Vector 1B value.
#define RXVEC1B_SOUNDING(__x)    (((__x) & 0x00010000))
/// Macro retrieving the number of extension SS of the RX vector
/// @param[in] __x RX Vector 1B value.
#define RXVEC1B_NUMEXTNSS(__x)   (((__x) & 0x00060000) >> 17)
/// Macro retrieving the aggregation bit of the RX vector
/// @param[in] __x RX Vector 1B value.
#define RXVEC1B_AGGREGATION(__x) (((__x) & 0x00080000))
/// Macro retrieving the FEC coding bit of the RX vector
/// @param[in] __x RX Vector 1B value.
#define RXVEC1B_FECCODING(__x)   (((__x) & 0x00100000))
/// Macro retrieving the antenna set of the RX vector
/// @param[in] __x RX Vector 1B value.
#define RXVEC1B_ANTENNASET(__x)  (((__x) & 0xFF000000) >> 24)
/// Macro retrieving the 1st chain RSSI of the RX vector
/// @param[in] __x RX Vector 1C value.
#define RXVEC1C_RSSI1(__x)       (((__x) & 0x000000FF))
/// Macro retrieving the 2nd chain RSSI of the RX vector
/// @param[in] __x RX Vector 1C value.
#define RXVEC1C_RSSI2(__x)       (((__x) & 0x0000FF00) >> 8)
/// Macro retrieving the 3rd chain RSSI of the RX vector
/// @param[in] __x RX Vector 1C value.
#define RXVEC1C_RSSI3(__x)       (((__x) & 0x00FF0000) >> 16)
/// Macro retrieving the 4th chain RSSI of the RX vector
/// @param[in] __x RX Vector 1C value.
#define RXVEC1C_RSSI4(__x)       (((__x) & 0xFF000000) >> 24)

/// @}

/// legacy RATE definitions
typedef enum
{
    /// 1Mbps
    HW_RATE_1MBPS = 0,
    /// 2Mbps
    HW_RATE_2MBPS,
    /// 5.5Mbps
    HW_RATE_5_5MBPS,
    /// 11Mbps
    HW_RATE_11MBPS,
    /// 6Mbps
    HW_RATE_6MBPS,
    /// 9Mbps
    HW_RATE_9MBPS,
    /// 12Mbps
    HW_RATE_12MBPS,
    /// 18Mbps
    HW_RATE_18MBPS,
    /// 24Mbps
    HW_RATE_24MBPS,
    /// 36Mbps
    HW_RATE_36MBPS,
    /// 48Mbps
    HW_RATE_48MBPS,
    /// 54Mbps
    HW_RATE_54MBPS
}HW_RATE_E;

/// MAC header content as defined in the MAC HW User Manual transmit MPDU template.
struct machdr
{
    /// Reserved.
    uint16_t reserved;
    /// Frame Control field.
    uint16_t framectrl;
    /// Duration/ID field.
    uint16_t duration;
    /// MAC Address 1.
    struct mac_addr macaddr1;
    /// MAC Address 2.
    struct mac_addr macaddr2;
    /// MAC Address 3.
    struct mac_addr macaddr3;
    /// Sequence control field.
    uint16_t seq_ctrl;
    /// MAC Address 4.
    struct mac_addr macaddr4;
    /// QoS control field.
    uint16_t qos_ctrl;
    /// Carried Frame Control field.
    uint16_t carriedfr_ctrl;
    /// High Throughput Control field.
    uint32_t ht_ctrl;
    /// Security initialization vector.
    uint32_t iv;
    /// Security extended initialization vector.
    uint32_t ext_iv;
};

/// Policy Table Structure used to store Policy Table Information used by MAC HW to
/// prepare transmit vector to used by PHY.
struct tx_policy_tbl
{
    /// Unique Pattern at the start of Policy Table
    uint32_t upatterntx;
    /// PHY Control Information 1 used by MAC HW
    uint32_t phycntrlinfo1;
    /// PHY Control Information 2 used by MAC HW
    uint32_t phycntrlinfo2;
    /// MAC Control Information 1 used by MAC HW
    uint32_t maccntrlinfo1;
    /// MAC Control Information 2 used by MAC HW
    uint32_t maccntrlinfo2;
    /// Rate Control Information used by MAC HW
    uint32_t ratecntrlinfo[RATE_CONTROL_STEPS];
    /// Power Control Information used by MAC HW
    uint32_t powercntrlinfo[RATE_CONTROL_STEPS];
};

/// Compressed Policy Table Structure used to store Policy Table Information used by MAC HW
/// to get TX information for secondary users in case of MU-MIMO PPDU transmission.
struct tx_compressed_policy_tbl
{
    /// Unique Pattern at the start of Policy Table
    uint32_t upatterntx;
    /// Secondary User Control Information
    uint32_t sec_user_control;
};

/// Definition of a TX header descriptor.
struct tx_hd
{
    /// Unique pattern for transmit DMA.
    uint32_t             upatterntx;
    /// Starting descriptor of the next atomic frame exchange sequence.
    uint32_t             nextfrmexseq_ptr;
    /// Next MPDU in the frame exchange.
    uint32_t             nextmpdudesc_ptr;
	
    /// First payload buffer descriptor/Secondary user 1 THD address
		/*#pragma anon_unions :GCC allows you to define a structure or 
		union that contains, as fields, structures and unions without 
		names*/ 
    union {
        /// First payload buffer descriptor
        uint32_t             first_pbd_ptr;
        /// Secondary user 1 THD address
        uint32_t             sec_user1_ptr;
    };
        
    /// Data buffer start/Secondary user 2 THD address.
    union {
        /// Data buffer start
        uint32_t             datastartptr;
        /// Secondary user 2 THD address
        uint32_t             sec_user2_ptr;
    };
    
    /// Data buffer end /Secondary user 3 THD address.
    union {
        /// Data buffer end
        uint32_t             dataendptr;
        /// Secondary user 3 THD address
        uint32_t             sec_user3_ptr;
    };
    
    /// Total length of the frame on air.
    uint32_t             frmlen;
    /// MSDU lifetime parameter (for EDCA frame).
    uint32_t             frmlifetime;
    
    /// Valid only for A-MPDU header descriptor and for singleton MPDUs.
    uint32_t             phyctrlinfo;
    /// Valid only for A-MPDU header descriptor and for singleton MPDUs.
    uint32_t             policyentryaddr;
    /// Optional length fields in case of BW drop
    uint32_t             optlen[3];
    /// Valid only for A-MPDU header descriptor and for singleton MPDUs.
    uint32_t             macctrlinfo1;
    /// Valid only for MPDU header descriptor and for singleton MPDUs.
    uint32_t             macctrlinfo2;
    /// Valid only for A-MPDU header descriptor and for singleton MPDUs.
    uint32_t             statinfo;
    
    /// Medium time used.
    uint32_t             mediumtimeused;
};

/// Definition of a TX payload buffer descriptor.
struct tx_pbd
{
    /// Unique pattern for transmit buffer descriptor.
    uint32_t             upatterntx;
    /// Next buffer descriptor of the MPDU when the MPDU is split over several buffers.
    uint32_t             next;
    /// Data start in the buffer associated with this buffer descriptor.
    uint32_t             datastartptr;
    /// Data end in the buffer associated with this buffer descriptor (inclusive).
    uint32_t             dataendptr;
    /// Buffer control for transmit DMA.
    uint32_t             bufctrlinfo;
};

/// Definition of a TX DMA descriptor.
struct tx_dmadesc
{
    /// Tx header descriptor
    struct tx_hd header;
    /// Tx buffer descriptor
    struct tx_pbd buffer;
    /// Mac header buffer
    struct machdr macheader;
};

union rx_vector_1a
{
	uint32_t value;
	struct
	{
		uint32_t legacy_len:12;
		uint32_t legacy_rate:4;
		uint32_t ht_len1:16;
	}bits;
};

enum
{
	FMOD_NON_HT = 0,
	FMOD_NON_HT_DUP_OFDM,
	FMOD_HT_MF,
	FMOD_HT_GF,
	FMOD_VHT,
};

union rx_vector_1b
{
	uint32_t value;
	struct
	{
		uint32_t ht_len2:4;
		uint32_t short_gi:1;
		uint32_t stbc:2;
		uint32_t smoothing:1;
		uint32_t msc:7;
		uint32_t pre_type:1;
		uint32_t format_mod:3;
		uint32_t channel_bw:2;
		uint32_t nsts:3;
		uint32_t lsig_valid:1;
		uint32_t sounding:1;
		uint32_t num_ext_nss:2;
		uint32_t is_agg:1;
		uint32_t fec_coding:1;
		uint32_t syn_bw:1;
		uint32_t doze_not_allow:1;		
	}bits;
};

/// Element in the pool of RX header descriptor.
struct rx_hd
{
    /// Unique pattern for receive DMA.
    uint32_t            upatternrx;
    /// Pointer to the location of the next Header Descriptor
    uint32_t            next;
    /// Pointer to the first payload buffer descriptor
    uint32_t            first_pbd_ptr;
    /// Pointer to the SW descriptor associated with this HW descriptor
    struct rx_swdesc   *swdesc;
	
    /// Pointer to the address in buffer where the hardware should start writing the data
    uint32_t            datastartptr;
    /// Pointer to the address in buffer where the hardware should stop writing data
    uint32_t            dataendptr;
    /// Header control information. Except for a single bit which is used for enabling the
    /// Interrupt for receive DMA rest of the fields are reserved
    uint32_t            headerctrlinfo;
    /// Total length of the received MPDU
    uint16_t            frmlen;
    /// AMPDU status information
    uint16_t            ampdu_stat_info;
	
    /// TSF Low
    uint32_t            tsflo;
    /// TSF High
    uint32_t            tsfhi;
    /// Contains the bytes 4 - 1 of Receive Vector 1
    uint32_t            recvec1a;
    /// Contains the bytes 8 - 5 of Receive Vector 1
    uint32_t            recvec1b;
	
    /// Contains the bytes 12 - 9 of Receive Vector 1
    uint32_t            recvec1c;
    /// Contains the bytes 16 - 13 of Receive Vector 1
    uint32_t            recvec1d;
    /// Contains the bytes 4 - 1 of Receive Vector 2
    uint32_t            recvec2a;
    ///  Contains the bytes 8 - 5 of Receive Vector 2
    uint32_t            recvec2b;
	
    /// MPDU status information
    uint32_t            statinfo;
};

/// Element in the pool of rx payload buffer descriptors.
struct rx_pbd
{
    /// Unique pattern
    uint32_t            upattern;
    /// Points to the next payload buffer descriptor of the MPDU when the MPDU is split
    /// over several buffers
    uint32_t            next;
    /// Points to the address in the buffer where the data starts
    uint32_t            datastartptr;
    /// Points to the address in the buffer where the data ends
    uint32_t            dataendptr;
    /// buffer status info for receive DMA.
    uint16_t            bufstatinfo;
    /// complete length of the buffer in memory
    uint16_t            reserved;
};

/// Definition of a Rx DMA header descriptor
struct rx_dmadesc
{
    /// Rx header descriptor (this element MUST be the first of the structure)
    struct rx_hd hd;
    /// Structure containing the information about the PHY channel that was used for this RX
    struct phy_channel_info phy_info;
    /// Word containing some SW flags about the RX packet
    uint32_t flags;
    /// Spare room for LMAC FW to write a pattern when last DMA is sent
    uint32_t pattern;
    /// IPC DMA control structure for MAC Header transfer
    struct dma_desc dma_desc;
};

/// Definition of a Rx DMA payload descriptor
struct rx_payloaddesc
{
    /// Mac header buffer (this element MUST be the first of the structure)
    struct rx_pbd pbd;
    /// IPC DMA control structure
    struct dma_desc dma_desc;
    /// Buffer containing the payload
    uint32_t buffer[NX_RX_PAYLOAD_LEN/4];
};

/// Structure indicating the status and other information about the transmission
struct tx_cfm_tag
{
    /// PN that was used for the transmission
    uint16_t pn[4];
    /// Sequence number of the packet
    uint16_t sn;
    /// Timestamp of first transmission of this MPDU
    uint16_t timestamp;
    /// Number of flow control credits allocated
    int8_t credits;
    
    #if NX_AMSDU_TX
    /// Size allowed for AMSDU
    uint16_t amsdu_size;
    #endif

    /// TX status
    uint32_t status;
};

/// Definition of a TX confirmation descriptor
struct tx_hw_desc_s
{
    /// Status of the transmission
    struct tx_cfm_tag cfm;
    /// DMA control structure for status transfer
    struct dma_desc dma_desc;
    /// TX header descriptor attached to the MPDU
    struct tx_hd    thd;
};

#if NX_AMPDU_TX
/// Minimum of A-MPDU descriptors per queue
#define TX_MIN_AMPDU_NB_PER_AC  (3 * RW_USER_MAX)
#define TX_AGG_DIVIDER          (8 / RW_USER_MAX)
/// Max number of AMPDU descriptors for the BK queue
#if (NX_TXDESC_CNT0 / TX_AGG_DIVIDER) >= TX_MIN_AMPDU_NB_PER_AC
#define TX_MAX_AMPDU_NB_PER_AC0  ((NX_TXDESC_CNT0 / TX_AGG_DIVIDER)
#else
#define TX_MAX_AMPDU_NB_PER_AC0  TX_MIN_AMPDU_NB_PER_AC
#endif
/// Max number of AMPDU descriptors for the BE queue
#if (NX_TXDESC_CNT1 / TX_AGG_DIVIDER) >= TX_MIN_AMPDU_NB_PER_AC
#define TX_MAX_AMPDU_NB_PER_AC1  (NX_TXDESC_CNT1 / TX_AGG_DIVIDER)
#else
#define TX_MAX_AMPDU_NB_PER_AC1  TX_MIN_AMPDU_NB_PER_AC
#endif
/// Max number of AMPDU descriptors for the VI queue
#if (NX_TXDESC_CNT2 / TX_AGG_DIVIDER) >= TX_MIN_AMPDU_NB_PER_AC
#define TX_MAX_AMPDU_NB_PER_AC2  (NX_TXDESC_CNT2 / TX_AGG_DIVIDER)
#else
#define TX_MAX_AMPDU_NB_PER_AC2  TX_MIN_AMPDU_NB_PER_AC
#endif
/// Max number of AMPDU descriptors for the VO queue
#if (NX_TXDESC_CNT3 / TX_AGG_DIVIDER) >= TX_MIN_AMPDU_NB_PER_AC
#define TX_MAX_AMPDU_NB_PER_AC3  (NX_TXDESC_CNT3 / TX_AGG_DIVIDER)
#else
#define TX_MAX_AMPDU_NB_PER_AC3  TX_MIN_AMPDU_NB_PER_AC
#endif
#if (NX_BEACONING)
/// Max number of AMPDU descriptors for the BCN queue
#if (NX_TXDESC_CNT4 / TX_AGG_DIVIDER) > TX_MIN_AMPDU_NB_PER_AC
#define TX_MAX_AMPDU_NB_PER_AC4  (NX_TXDESC_CNT4 / TX_AGG_DIVIDER)
#else
#define TX_MAX_AMPDU_NB_PER_AC4  TX_MIN_AMPDU_NB_PER_AC
#endif
#endif

///Aggregation descriptor, containing AMPDU THD, BAR descriptor, BAR payload and Policy Table
struct tx_agg_desc
{
    /// Pointer to the next element in the queue
    struct co_list_hdr list_hdr;
    /// Amount of data currently downloaded for this A-MPDU
    uint16_t available_len;
    /// Number of MPDUs downloaded for this A-MPDU
    uint8_t available_cnt;
    /// Current status of the aggregation process
    uint8_t status;
    ///STA IDX for which AMPDU is being built
    uint8_t sta_idx;
    ///TID for which AMPDU is being built
    uint8_t tid;
    ///Number of "users" currently sharing this descriptor
    ///The descriptor will be freed only when no more SW modules are using it
    uint8_t user_cnt;
    ///AMPDU THD (no payload)
    struct tx_hd a_thd;
    ///BAR THD
    struct tx_hd bar_thd;
    ///BAR payload space (no FCS)
    struct bar_frame bar_payl;
    ///Policy table for explicit BAR frame
    struct tx_policy_tbl bar_pol_tbl;
    ///Pointer to the BA frame
    struct rx_swdesc *ba_desc;
    
    #if NX_BW_LEN_ADAPT
    /// Pointer to TX descriptors finishing a BW step
    struct txdesc *txdesc[NX_BW_LEN_STEPS - 1];
    #endif
    
    #if RW_MUMIMO_TX_EN
    /// List containing the descriptors to be confirmed once BA is received for this
    /// secondary user
    struct co_list cfm;
    /// Pointer to the primary TX descriptor
    struct tx_agg_desc *prim_agg_desc;
    /// Pointer to the last BAR THD of the MU-MIMO frame exchange
    struct tx_hd *last_bar_thd;
    /// Bitfield indicating which user indexes have still data to download before being
    /// able to be chained to HW
    uint8_t download;
    #endif
};
#else

/// When aggregation is not enabled, empty structure
struct tx_agg_desc
{
    /// Reserved field
    uint8_t reserved;
};

#endif

#if NX_RADAR_DETECT
/// Definition of an array of radar pulses
struct radar_pulse_array_desc
{
    /// Buffer containing the radar pulses
    struct phy_radar_pulse pulse[RADAR_PULSE_MAX];
    /// Index of the radar detection chain that detected those pulses
    uint32_t idx;
    /// Number of valid pulses in the buffer
    uint32_t cnt;
};

/// Definition of a Rx DMA payload descriptor
struct radar_event_desc
{
    /// Pointer to the next element in the queue
    struct co_list_hdr list_hdr;
    /// IPC DMA control structure
    struct dma_desc dma_desc;
    /// General Purpose DMA Control Structure
    struct hal_dma_desc_tag gp_dma_desc;
    /// Buffer containing the radar pulses
    struct radar_pulse_array_desc pulse_array;
};
#endif

enum hal_host_rxdesc_status
{
    /// Descriptor is IDLE
    HAL_HOST_RXDESC_IDLE        = 0,
    /// Data linked to the descriptor has already been pushed in host memory
    HAL_HOST_RXDESC_DATA_XFER,
    /// Data linked to the descriptor has not been pushed in host memory
    HAL_HOST_RXDESC_DATA_NXFER,
};

struct hal_host_rxdesc_val
{
    /// Host Buffer Address
    uint32_t host_id;
    /// Length
    uint32_t frame_len;
    /// Status
    uint8_t status;
};

struct hal_host_rxdesc
{
    /// Pointer to the next element in the queue
    struct co_list_hdr list_hdr;
    /// IPC DMA control structure
    struct dma_desc dma_desc;
    /// RX Descriptor value
    struct hal_host_rxdesc_val rxdesc_val;
    /// Pointer to the associated SW Descriptor
    struct rx_swdesc *p_rx_swdesc;
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
#if NX_AMPDU_TX
/// Array of aggregation descriptors for the BK queue
extern struct tx_agg_desc tx_agg_desc_array0[TX_MAX_AMPDU_NB_PER_AC0];
/// Array of aggregation descriptors for the BE queue
extern struct tx_agg_desc tx_agg_desc_array1[TX_MAX_AMPDU_NB_PER_AC1];
/// Array of aggregation descriptors for the VI queue
extern struct tx_agg_desc tx_agg_desc_array2[TX_MAX_AMPDU_NB_PER_AC2];
/// Array of aggregation descriptors for the VO queue
extern struct tx_agg_desc tx_agg_desc_array3[TX_MAX_AMPDU_NB_PER_AC3];
#if (NX_BEACONING)
/// Array of aggregation descriptors for the BCN queue
extern struct tx_agg_desc tx_agg_desc_array4[TX_MAX_AMPDU_NB_PER_AC4];
#endif
#endif

#if NX_RADAR_DETECT
extern struct radar_event_desc radar_event_desc_array[RADAR_EVENT_MAX];
#endif

#if NX_BCN_AUTONOMOUS_TX
/// IPC DMA control structure for beacon download
extern struct dma_desc bcn_dwnld_desc;
#endif

/// Array of struct containing the RX header descriptors
extern struct rx_dmadesc       rx_dma_hdrdesc[];
/// Array of struct containing the RX buffer descriptors
extern struct rx_payloaddesc   rx_payload_desc[];
/// Array of HW descriptors for BK queue
extern struct tx_hw_desc_s       tx_hw_desc0[RW_USER_MAX * NX_TXDESC_CNT0];
/// Array of HW descriptors for BE queue
extern struct tx_hw_desc_s       tx_hw_desc1[RW_USER_MAX * NX_TXDESC_CNT1];
/// Array of HW descriptors for VI queue
extern struct tx_hw_desc_s       tx_hw_desc2[RW_USER_MAX * NX_TXDESC_CNT2];
/// Array of HW descriptors for VO queue
extern struct tx_hw_desc_s       tx_hw_desc3[RW_USER_MAX * NX_TXDESC_CNT3];

#if (NX_BEACONING)
/// Array of HW descriptors for BCN queue
extern struct tx_hw_desc_s       tx_hw_desc4[NX_TXDESC_CNT4];
#endif

extern struct hal_host_rxdesc  hal_host_rxdesc_pool[HAL_RXDESC_CNT];
extern struct dma_desc hal_me_dma_desc;

/// @}

#endif // _HWIF_DESC_H_
