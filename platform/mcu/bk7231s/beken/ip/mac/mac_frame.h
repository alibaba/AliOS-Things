/**
 ****************************************************************************************
 *
 * @file mac_frame.h
 *
 * @brief Main Frame Formation and Reception definitions.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _MAC_FRAME_H_
#define _MAC_FRAME_H_

/**
 ****************************************************************************************
 * @defgroup CO_FRAME CO_FRAME
 * @ingroup COMMON
 * @brief Definition of the mac_frame module API.
 * @{
 ****************************************************************************************
 */

/**
 * INCLUDE FILES
 ****************************************************************************************
 */
// for the CO_BIT macro
#include "co_math.h"
#include "mac.h"

/**
 * MAC FRAME LENGTH DEFINITIONS
 ****************************************************************************************
 */

/// MIC Header Length -> DA (6 bytes) + SA (6 bytes) + Priority (4 bytes)
#define MIC_HDR_LEN     (16)
/// IV Length
#define IV_LEN          (4)
/// EIV Length
#define EIV_LEN         (4)
/// MIC Length
#define MIC_LEN         (8)
/// ICV Length
#define ICV_LEN         (4)
// WPI IV length
#define WPI_IV_LEN      (18)
// WPI MIC length
#define WPI_MIC_LEN     (16)

/// Word 1 of CCMP IV
#define EIV_PRESENT     (0x2000)

/**
 * MAC HEADER LENGTH DEFINITIONS
 ****************************************************************************************
 */
/// Long MAC Header length (with QoS control field and HT control field)
#define MAC_LONG_QOS_HTC_MAC_HDR_LEN   36
/// Long MAC Header length (with QoS control field)
#define MAC_LONG_QOS_MAC_HDR_LEN       32
/// Long MAC Header length (without QoS control field)
#define MAC_LONG_MAC_HDR_LEN           30
/// Short MAC Header length (with QoS control field and HT control field)
#define MAC_SHORT_QOS_HTC_MAC_HDR_LEN  30
/// Short MAC Header length (with QoS control field)
#define MAC_SHORT_QOS_MAC_HDR_LEN      26
/// Short MAC Header length (without QoS control field)
#define MAC_SHORT_MAC_HDR_LEN          24

/// QoS Control Field Length
#define MAC_HDR_QOS_CTRL_LEN           2

/// Longest possible MAC Header
#define MAC_HDR_LEN_MAX                MAC_LONG_QOS_HTC_MAC_HDR_LEN

/// Long control frame header length
#define MAC_LONG_CTRLFRAME_LEN         16
/// Short control frame header length (ACK/CTS)
#define MAC_SHORT_CTRLFRAME_LEN        10

/*
 * MAC HEADER definitions and OFFSETS
 ****************************************************************************************
 */
#define MAC_HEAD_FCTRL_OFT              0
#define MAC_HEAD_DURATION_OFT           2
#define MAC_HEAD_DURATION_CFP      0x8000
#define MAC_HEAD_ADDR1_OFT              4
#define MAC_HEAD_ADDR2_OFT             10
#define MAC_HEAD_ADDR3_OFT             16
#define MAC_HEAD_CTRL_OFT              22
#define MAC_HEAD_ADDR4_OFT             24
#define MAC_HEAD_SHORT_QOS_OFT         24
#define MAC_HEAD_LONG_QOS_OFT          30
#define MAC_ORIGINAL_ETHTYPE_OFT       36

/// FRAME CONTROL MASK
/* The type information in the header of a frame consists of the Type and Subtype fields
 * When using the term "frame type" in the code, we refer to the type field and not to
 * the combined type+subtype information.
 */
#define MAC_FCTRL_LEN                   2

#define MAC_FRAME_CTRL_BEACON           0x0080

#define MAC_FCTRL_PROTOCOLVERSION_MASK  0x0003
#define MAC_FCTRL_TYPE_MASK             0x000C
#define MAC_FCTRL_MGT_T                 0x0000
#define MAC_FCTRL_CTRL_T                0x0004
#define MAC_FCTRL_DATA_T                0x0008
#define MAC_FCTRL_RSV_T                 0x000c

#define MAC_FCTRL_SUBT_MASK             0x00F0
/// Management SubType
#define MAC_FCTRL_ASSOCREQ_ST           0x0000
#define MAC_FCTRL_ASSOCRSP_ST           0x0010
#define MAC_FCTRL_REASSOCREQ_ST         0x0020
#define MAC_FCTRL_REASSOCRSP_ST         0x0030
#define MAC_FCTRL_PROBEREQ_ST           0x0040
#define MAC_FCTRL_PROBERSP_ST           0x0050
#define MAC_FCTRL_BEACON_ST             0x0080
#define MAC_FCTRL_ATIM_ST               0x0090
#define MAC_FCTRL_DISASSOC_ST           0x00A0
#define MAC_FCTRL_AUTHENT_ST            0x00B0
#define MAC_FCTRL_DEAUTHENT_ST          0x00C0
#define MAC_FCTRL_ACTION_ST             0x00D0
#define MAC_FCTRL_ACTION_NO_ACK_ST      0x00E0
/// Control SubTypes
#define MAC_FCTRL_BFM_REPORT_POLL_ST    0x0040
#define MAC_FCTRL_VHT_NDPA_ST           0x0050
#define MAC_FCTRL_CTRL_WRAPPER_ST       0x0070
#define MAC_FCTRL_BAR_ST                0x0080
#define MAC_FCTRL_BA_ST                 0x0090
#define MAC_FCTRL_PSPOLL_ST             0x00A0
#define MAC_FCTRL_RTS_ST                0x00B0
#define MAC_FCTRL_CTS_ST                0x00C0
#define MAC_FCTRL_ACK_ST                0x00D0
#define MAC_FCTRL_CFEND_ST              0x00E0
#define MAC_FCTRL_CFEND_CFACK_ST        0x00F0

/// Data SubTypes
/* Decoding the subtypes of data type frames can take advantage of the fact that
 * each subtype field bit position is used to indicate a specific modification of
 * the basic data frame (subtype 0). Frame control bit 4 is set to 1 in data
 * subtypes which include +CF-Ack, bit 5 is set to 1 in data subtypes which include
 * +CF-Poll, bit 6 is set to 1 in data subtypes that contain no Frame Body,
 * and bit 7 is set to 1 in the QoS data subtypes, which have QoS Control fields in
 * their MAC headers.
 *
 * Usage: check FrameT and FrameSubT individually. If the FrameT is MAC_FCTRL_DATA_T,
 * check the following bits of the FrameSubT
 */
#define MAC_CFACK_ST_BIT                CO_BIT(4)
#define MAC_CFPOLL_ST_BIT               CO_BIT(5)
#define MAC_NODATA_ST_BIT               CO_BIT(6)
#define MAC_QOS_ST_BIT                  CO_BIT(7)

#define MAC_FCTRL_DATACFACKPOLL_ST      (MAC_CFACK_ST_BIT | MAC_CFPOLL_ST_BIT)
#define MAC_FCTRL_TODS                  0x0100
#define MAC_FCTRL_FROMDS                0x0200
#define MAC_FCTRL_MOREFRAG              0x0400
#define MAC_FCTRL_RETRY                 0x0800
#define MAC_FCTRL_PWRMGT                0x1000
#define MAC_FCTRL_MOREDATA              0x2000
#define MAC_FCTRL_PROTECTEDFRAME        0x4000
#define MAC_FCTRL_ORDER                 0x8000

#define MAC_FCTRL_TODS_FROMDS          (MAC_FCTRL_TODS | MAC_FCTRL_FROMDS)

/// FRAME CONTROL : Type information including Type and SubType
#define MAC_FCTRL_TYPESUBTYPE_MASK      (MAC_FCTRL_TYPE_MASK | MAC_FCTRL_SUBT_MASK)
#define MAC_FCTRL_ASSOCREQ              (MAC_FCTRL_MGT_T     | MAC_FCTRL_ASSOCREQ_ST)
#define MAC_FCTRL_ASSOCRSP              (MAC_FCTRL_MGT_T     | MAC_FCTRL_ASSOCRSP_ST)
#define MAC_FCTRL_REASSOCREQ            (MAC_FCTRL_MGT_T     | MAC_FCTRL_REASSOCREQ_ST)
#define MAC_FCTRL_REASSOCRSP            (MAC_FCTRL_MGT_T     | MAC_FCTRL_REASSOCRSP_ST)
#define MAC_FCTRL_PROBEREQ              (MAC_FCTRL_MGT_T     | MAC_FCTRL_PROBEREQ_ST)
#define MAC_FCTRL_PROBERSP              (MAC_FCTRL_MGT_T     | MAC_FCTRL_PROBERSP_ST)
#define MAC_FCTRL_BEACON                (MAC_FCTRL_MGT_T     | MAC_FCTRL_BEACON_ST)
#define MAC_FCTRL_ATIM                  (MAC_FCTRL_MGT_T     | MAC_FCTRL_ATIM_ST)
#define MAC_FCTRL_DISASSOC              (MAC_FCTRL_MGT_T     | MAC_FCTRL_DISASSOC_ST)
#define MAC_FCTRL_AUTHENT               (MAC_FCTRL_MGT_T     | MAC_FCTRL_AUTHENT_ST)
#define MAC_FCTRL_DEAUTHENT             (MAC_FCTRL_MGT_T     | MAC_FCTRL_DEAUTHENT_ST)
#define MAC_FCTRL_ACTION                (MAC_FCTRL_MGT_T     | MAC_FCTRL_ACTION_ST)
#define MAC_FCTRL_ACTION_NO_ACK         (MAC_FCTRL_MGT_T     | MAC_FCTRL_ACTION_NO_ACK_ST)
#define MAC_FCTRL_BFM_REPORT_POLL       (MAC_FCTRL_CTRL_T    | MAC_FCTRL_BFM_REPORT_POLL_ST)
#define MAC_FCTRL_VHT_NDPA              (MAC_FCTRL_CTRL_T    | MAC_FCTRL_VHT_NDPA_ST)
#define MAC_FCTRL_BA                    (MAC_FCTRL_CTRL_T    | MAC_FCTRL_BA_ST)
#define MAC_FCTRL_BAR                   (MAC_FCTRL_CTRL_T    | MAC_FCTRL_BAR_ST)
#define MAC_FCTRL_PSPOLL                (MAC_FCTRL_CTRL_T    | MAC_FCTRL_PSPOLL_ST)
#define MAC_FCTRL_RTS                   (MAC_FCTRL_CTRL_T    | MAC_FCTRL_RTS_ST)
#define MAC_FCTRL_CTS                   (MAC_FCTRL_CTRL_T    | MAC_FCTRL_CTS_ST)
#define MAC_FCTRL_ACK                   (MAC_FCTRL_CTRL_T    | MAC_FCTRL_ACK_ST)
#define MAC_FCTRL_CFEND                 (MAC_FCTRL_CTRL_T    | MAC_FCTRL_CFEND_ST)
#define MAC_FCTRL_CFEND_CFACK           (MAC_FCTRL_CFEND     | MAC_CFACK_ST_BIT)
#define MAC_FCTRL_DATA_CFACK            (MAC_FCTRL_DATA_T    | MAC_CFACK_ST_BIT)
#define MAC_FCTRL_DATA_CFPOLL           (MAC_FCTRL_DATA_T    | MAC_CFPOLL_ST_BIT)
#define MAC_FCTRL_DATA_CFACKPOLL        (MAC_FCTRL_DATA_T    | MAC_FCTRL_DATACFACKPOLL_ST)
#define MAC_FCTRL_NULL_FUNCTION         (MAC_FCTRL_DATA_T    | MAC_NODATA_ST_BIT)
#define MAC_FCTRL_NULL_CFACK            (MAC_FCTRL_NULL_FUNCTION  | MAC_CFACK_ST_BIT)
#define MAC_FCTRL_NULL_CFPOLL           (MAC_FCTRL_NULL_FUNCTION  | MAC_CFPOLL_ST_BIT)
#define MAC_FCTRL_NULL_CFACKPOLL        (MAC_FCTRL_NULL_FUNCTION  | MAC_FCTRL_DATACFACKPOLL_ST)
#define MAC_FCTRL_QOS_DATA              (MAC_FCTRL_DATA_T    | MAC_QOS_ST_BIT)
#define MAC_FCTRL_QOS_DATA_CFACK        (MAC_FCTRL_QOS_DATA  | MAC_CFACK_ST_BIT)
#define MAC_FCTRL_QOS_DATA_CFPOLL       (MAC_FCTRL_QOS_DATA  | MAC_CFPOLL_ST_BIT)
#define MAC_FCTRL_QOS_DATA_CFACKPOLL    (MAC_FCTRL_QOS_DATA  | MAC_FCTRL_DATACFACKPOLL_ST)
#define MAC_FCTRL_QOS_NULL              (MAC_FCTRL_QOS_DATA  | MAC_NODATA_ST_BIT)
#define MAC_FCTRL_QOS_NULL_CFACK        (MAC_FCTRL_QOS_DATA  | MAC_FCTRL_NULL_CFACK)
#define MAC_FCTRL_QOS_NULL_CFPOLL       (MAC_FCTRL_QOS_DATA  | MAC_FCTRL_NULL_CFPOLL)
#define MAC_FCTRL_QOS_NULL_CFACKPOLL    (MAC_FCTRL_QOS_DATA  | MAC_FCTRL_NULL_CFACKPOLL)

#define MAC_FCTRL_IS(fc, type) (((fc) & MAC_FCTRL_TYPESUBTYPE_MASK) == MAC_FCTRL_##type)

/*
 * FIELD DESCRIPTION
 ****************************************************************************************
 */
/// DURATION FIELD
#define MAC_DURATION_LEN                2

/// ASSOCIATION ID FIELD
#define MAC_AID_MSK                     0x3FFF

/// SEQUENCE CONTROL FIELD
#define MAC_SEQCTRL_LEN                 2
#define MAC_SEQCTRL_NUM_OFT             4
#define MAC_SEQCTRL_NUM_MSK             0xFFF0
#define MAC_SEQCTRL_NUM_MAX             (MAC_SEQCTRL_NUM_MSK >> MAC_SEQCTRL_NUM_OFT)
#define MAC_SEQCTRL_FRAG_OFT            0
#define MAC_SEQCTRL_FRAG_MSK            0x000F

// SECURITY
#define MAC_HDR_TSC_LEN                 6
#define MAC_TKIP_TSC1_OFT               0
#define MAC_TKIP_TSC0_OFT               2
#define MAC_TKIP_KEYID_OFT              3
#define MAC_CCMP_PN0_OFT                0
#define MAC_CCMP_PN1_OFT                1
#define MAC_CCMP_RSV_OFT                2
#define MAC_CCMP_PN2BIT_OFT             8
#define MAC_IV_EXTIV                    0x20000000

/// QOS CONTROL FIELD
#define MAC_QOSCTRL_LEN                 2
#define MAC_QOSCTRL_UP_OFT              0
#define MAC_QOSCTRL_UP_MSK              0x0007
#define MAC_QOSCTRL_EOSP_OFT            4
#define MAC_QOSCTRL_EOSP                0x10
#define MAC_QOSCTRL_QUEUE_SIZE_OFT      8
#define MAC_QOSCTRL_QUEUE_SIZE_NEGLECTED 0xFF
#define MAC_QOSCTRL_ACK_OFT             5
#define MAC_QOSCTRL_ACK_NORMAL          0x0000
#define MAC_QOSCTRL_ACK_NOACK           0x0020
#define MAC_QOSCTRL_ACK_NOEXPL          0x0040
#define MAC_QOSCTRL_ACK_BLOCK           0x0060
#define MAC_QOSCTRL_AMSDU_OFT           7
#define MAC_QOSCTRL_AMSDU_PRESENT       CO_BIT(7)
#define MAC_QOSCTRL_MESH_CTRL_PRESENT   CO_BIT(8)
#define MAC_QOSCTRL_MESH_POWER_SAVE_LVL CO_BIT(9)
#define MAC_QOSCTRL_MESH_RSPI           CO_BIT(10)

/// HT CONTROL FIELD
#define MAC_HTCTRL_LEN                  4

// SUPPORTED RATES MAXIMUM LENGTH
#define MAC_SUPPORTED_RATES_LEN   8

// CAPABILITY INFORMATION FIELD
#define MAC_CAPA_ESS                    CO_BIT(0)
#define MAC_CAPA_IBSS                   CO_BIT(1)
#define MAC_CAPA_CFPOLL                 CO_BIT(2)
#define MAC_CAPA_CFREQ                  CO_BIT(3)
#define MAC_CAPA_PRIVA                  CO_BIT(4)
#define MAC_CAPA_SHORT_PREAMBLE         CO_BIT(5)
#define MAC_CAPA_PBCB                   CO_BIT(6)
#define MAC_CAPA_AGILITY                CO_BIT(7)
#define MAC_CAPA_SPECTRUM               CO_BIT(8)
#define MAC_CAPA_QOS                    CO_BIT(9)
#define MAC_CAPA_SHORT_SLOT             CO_BIT(10)
//#define MAC_CAPA_ROBUST_SECURITY       CO_BIT(11)
#define MAC_CAPA_APSD                   CO_BIT(11)
#define MAC_CAPA_DSSS_OFDM              CO_BIT(13)
#define MAC_CAPA_DELAYED_BA             CO_BIT(14)
#define MAC_CAPA_IMMEDIATE_BA           CO_BIT(15)
#define MAC_CAPA_BA_POLICY              (MAC_CAPA_DELAYED_BA | MAC_CAPA_IMMEDIATE_BA)

/// HT CAPABILITY INFORMATION FIELD
#define MAC_HTCAPA_LDPC                 CO_BIT(0)
#define MAC_HTCAPA_40_MHZ               CO_BIT(1)
#define MAC_HTCAPA_SMPS_OFT             2
#define MAC_HTCAPA_SMPS_MSK             (0x03 << MAC_HTCAPA_SMPS_OFT)
#define MAC_HTCAPA_SMPS_STATIC          (0x00 << MAC_HTCAPA_SMPS_OFT)
#define MAC_HTCAPA_SMPS_DYNAMIC         (0x01 << MAC_HTCAPA_SMPS_OFT)
#define MAC_HTCAPA_SMPS_DISABLE         (0x03 << MAC_HTCAPA_SMPS_OFT)
#define MAC_HTCAPA_SHORTGI_20           CO_BIT(5)
#define MAC_HTCAPA_SHORTGI_40           CO_BIT(6)
#define MAC_HTCAPA_TX_STBC              CO_BIT(7)
#define MAC_HTCAPA_RX_STBC_OFT          8
#define MAC_HTCAPA_RX_STBC_MSK          (0x03 << MAC_HTCAPA_RX_STBC_OFT)
#define MAC_HTCAPA_AMSDU                CO_BIT(11)
#define MAC_HTCAPA_DSSS_CCK_40          CO_BIT(12)
#define MAC_HTCAPA_40_INTOLERANT        CO_BIT(14)
#define MAC_HTCAPA_LSIG                 CO_BIT(15)

/// HT A-MPDU PARAMETERS INFORMATION FIELD
#define MAC_AMPDU_LEN_EXP_OFT           0
#define MAC_AMPDU_LEN_EXP_MSK           (0x03 << MAC_AMPDU_LEN_EXP_OFT)
#define MAC_AMPDU_MIN_SPACING_OFT       2
#define MAC_AMPDU_MIN_SPACING_MSK       (0x07 << MAC_AMPDU_MIN_SPACING_OFT)

#define MAC_HT_MAX_AMPDU_FACTOR         13

/// HT EXTENDED CAPABILITY
#define MAC_HTXCAPA_MFB_UNSOLICIT       CO_BIT(9)


// VHT CAPABILITIES
#define MAC_VHTCAPA_MAX_MPDU_LENGTH_OFT        0
#define MAC_VHTCAPA_MAX_MPDU_LENGTH_MSK        (0x03 << MAC_VHTCAPA_MAX_MPDU_LENGTH_OFT)
#define MAC_VHTCAPA_MAX_MPDU_LENGTH_3895       (0x00 << MAC_VHTCAPA_MAX_MPDU_LENGTH_OFT)
#define MAC_VHTCAPA_MAX_MPDU_LENGTH_7991       (0x01 << MAC_VHTCAPA_MAX_MPDU_LENGTH_OFT)
#define MAC_VHTCAPA_MAX_MPDU_LENGTH_11454      (0x02 << MAC_VHTCAPA_MAX_MPDU_LENGTH_OFT)
#define MAC_VHTCAPA_SUPP_CHAN_WIDTH_160MHZ     CO_BIT(2)
#define MAC_VHTCAPA_SUPP_CHAN_WIDTH_160_80P80  CO_BIT(3)
#define MAC_VHTCAPA_SUPP_CHAN_WIDTH_OFT        2
#define MAC_VHTCAPA_SUPP_CHAN_WIDTH_MSK        (0x03 << MAC_VHTCAPA_SUPP_CHAN_WIDTH_OFT)
#define MAC_VHTCAPA_RXLDPC                     CO_BIT(4)
#define MAC_VHTCAPA_SHORT_GI_80                CO_BIT(5)
#define MAC_VHTCAPA_SHORT_GI_160               CO_BIT(6)
#define MAC_VHTCAPA_TXSTBC                     CO_BIT(7)
#define MAC_VHTCAPA_RXSTBC_OFT                 8
#define MAC_VHTCAPA_RXSTBC_MSK                 (0x07 << MAC_VHTCAPA_RXSTBC_OFT)
#define MAC_VHTCAPA_RXSTBC_1                   (0x01 << MAC_VHTCAPA_RXSTBC_OFT)
#define MAC_VHTCAPA_RXSTBC_2                   (0x02 << MAC_VHTCAPA_RXSTBC_OFT)
#define MAC_VHTCAPA_RXSTBC_3                   (0x03 << MAC_VHTCAPA_RXSTBC_OFT)
#define MAC_VHTCAPA_RXSTBC_4                   (0x04 << MAC_VHTCAPA_RXSTBC_OFT)
#define MAC_VHTCAPA_SU_BEAMFORMER_CAPABLE      CO_BIT(11)
#define MAC_VHTCAPA_SU_BEAMFORMEE_CAPABLE      CO_BIT(12)
#define MAC_VHTCAPA_BEAMFORMEE_STS_OFT         13
#define MAC_VHTCAPA_BEAMFORMEE_STS_MSK         (0x07 << MAC_VHTCAPA_BEAMFORMEE_STS_OFT)
#define MAC_VHTCAPA_SOUNDING_DIMENSIONS_OFT    16
#define MAC_VHTCAPA_SOUNDING_DIMENSIONS_MSK    (0x07 << MAC_VHTCAPA_SOUNDING_DIMENSIONS_OFT)
#define MAC_VHTCAPA_MU_BEAMFORMER_CAPABLE      CO_BIT(19)
#define MAC_VHTCAPA_MU_BEAMFORMEE_CAPABLE      CO_BIT(20)
#define MAC_VHTCAPA_VHT_TXOP_PS                CO_BIT(21)
#define MAC_VHTCAPA_HTC_VHT                    CO_BIT(22)
#define MAC_VHTCAPA_MAX_A_MPDU_LENGTH_EXP_OFT  23
#define MAC_VHTCAPA_MAX_A_MPDU_LENGTH_EXP_MSK  (0x07 << MAC_VHTCAPA_MAX_A_MPDU_LENGTH_EXP_OFT)
#define MAC_VHTCAPA_VHT_LA_VHT_UNSOL_MFB       0x08000000
#define MAC_VHTCAPA_VHT_LA_VHT_MRQ_MFB         0x0c000000
#define MAC_VHTCAPA_RX_ANTENNA_PATTERN         CO_BIT(28)
#define MAC_VHTCAPA_TX_ANTENNA_PATTERN         CO_BIT(29)

#define MAC_VHT_MCS_MAP_MSK                    0x03
#define MAC_VHT_MCS_MAP_0_7                    0x00
#define MAC_VHT_MCS_MAP_0_8                    0x01
#define MAC_VHT_MCS_MAP_0_9                    0x02
#define MAC_VHT_MCS_MAP_NONE                   0x03


/****************************************************************************************
 * MAC FRAME OFFSETS
 ****************************************************************************************
 */
// Offset for the length frame in any element ID
#define MAC_LEN_OFT                             1

// SSID FIELD
#define MAC_SSID_ID_OFT                         0
#define MAC_SSID_LEN_OFT                        1
#define MAC_SSID_SSID_OFT                       2

// SUPPORTED RATES FIELD
#define MAC_RATES_ID_OFT                        0
#define MAC_RATES_LEN_OFT                       1
#define MAC_RATES_RATES_OFT                     2
#define MAC_RATES_LEN                           8

// FH PARAM SET FIELD
#define MAC_FH_ID_OFT                           0
#define MAC_FH_LEN_OFT                          1
#define MAC_FH_DWELL_OFT                        2
#define MAC_FH_HOP_SET_OFT                      4
#define MAC_FH_HOP_PAT_OFT                      5
#define MAC_FH_HOP_IDX_OFT                      6
#define MAC_FH_PARAM_LEN                        7

// DS PARAM SET
#define MAC_DS_ID_OFT                           0
#define MAC_DS_LEN_OFT                          1
#define MAC_DS_CHANNEL_OFT                      2
#define MAC_DS_PARAM_LEN                        3

// CF PARAM SET
#define MAC_CF_ID_OFT                           0
#define MAC_CF_LEN_OFT                          1
#define MAC_CF_COUNT_OFT                        2
#define MAC_CF_PERIOD_OFT                       3
#define MAC_CF_MAX_DUR_OFT                      4
#define MAC_CF_REM_DUR_OFT                      6
#define MAC_CF_PARAM_LEN                        8

// TIM
#define MAC_TIM_ID_OFT                          0
#define MAC_TIM_LEN_OFT                         1
#define MAC_TIM_CNT_OFT                         2
#define MAC_TIM_PERIOD_OFT                      3
#define MAC_TIM_BMPC_OFT                        4
#define MAC_TIM_BMP_OFT                         5
#define MAC_TIM_IE_LEN                         (5 + MAC_TIM_SIZE)
#define MAC_TIM_BCMC_PRESENT                    CO_BIT(0)

// IBSS PARAM SET
#define MAC_IBSS_ID_OFT                         0
#define MAC_IBSS_LEN_OFT                        1
#define MAC_IBSSATIM_OFT                        2
#define MAC_IBSS_PARAM_LEN                      4

// Country
#define MAC_COUNTRY_ID_OFT                      0
#define MAC_COUNTRY_LEN_OFT                     1
#define MAC_COUNTRY_STRING_OFT                  2
#define MAC_COUNTRY_STRING_LEN                  3
#define MAC_COUNTRY_FIRST_CHAN_OFT              0
#define MAC_COUNTRY_NB_CHAN_OFT                 1
#define MAC_COUNTRY_PWR_LEVEL_OFT               2
#define MAC_COUNTRY_TRIPLET_LEN                 3

// CHALLENGE PARAM SET
#define MAC_CHALLENGE_ID_OFT                    0
#define MAC_CHALLENGE_LEN_OFT                   1
#define MAC_CHALLENGE_TEXT_OFT                  2

// ERP
#define MAC_ERP_ID_OFT                          0
#define MAC_ERP_LEN_OFT                         1
#define MAC_ERP_PARAM_OFT                       2
// ERP BIT FIELD
#define MAC_ERP_NON_ERP_PRESENT                 CO_BIT(0)
#define MAC_ERP_USE_PROTECTION                  CO_BIT(1)
#define MAC_ERP_BARKER_PREAMBLE_MODE            CO_BIT(2)
#define MAC_ERP_IE_PRESENT                      CO_BIT(7)   // not standard, used internally

// RSN INFORMATION FIELD (WPA Extention)
#define MAC_RSNIE_ID_OFT                         0
#define MAC_RSNIE_LEN_OFT                        1
#define MAC_RSNIE_INFO_OFT                       2
#define MAC_RSNIE_VERSION_OFT                    2
#define MAC_RSNIE_GROUP_CIPHER_OFT               4
#define MAC_RSNIE_PAIRWISE_CIPHER_SUITE_CNT_OFT  8
#define MAC_RSNIE_PAIRWISE_CIPHER_SUITE_LIST_OFT 10
#define MAC_RSNIE_KEY_MANAGEMENT_SUITE_CNT_OFT   14
#define MAC_RSNIE_KEY_MANAGEMENT_SUITE_LIST_OFT  16
#define MAC_RSNIE_RSN_CAPABILITIES_OFT           20
#define MAC_RSNIE_RSN_PMKID_COUNT_OFT            22
#define MAC_RSNIE_RSN_PMKID_COUNT_LIST_OFT       24

// BSS Load IE
#define MAC_BSS_LOAD_ID_OFT                     0
#define MAC_BSS_LOAD_LEN_OFT                    1
#define MAC_BSS_LOAD_STA_CNT_OFT                2
#define MAC_BSS_LOAD_CH_UTILIZATION_OFT         4
#define MAC_BSS_LOAD_AVAIL_ADM_CAPA             5
#define MAC_BSS_LOAD_LEN                        7

// EDCA Parameter Set IE
#define MAC_EDCA_PARAM_ID_OFT                   0
#define MAC_EDCA_PARAM_LEN_OFT                  1
#define MAC_EDCA_PARAM_QOS_INFO_OFT             2
#define MAC_EDCA_PARAM_RESERVED_OFT             3
#define MAC_EDCA_PARAM_BE_PARAM_OFT             4
#define MAC_EDCA_PARAM_BK_PARAM_OFT             8
#define MAC_EDCA_PARAM_VI_PARAM_OFT             12
#define MAC_EDCA_PARAM_VO_PARAM_OFT             16
#define MAC_EDCA_PARAM_LEN                      20

// QoS Capability IE
#define MAC_QOS_CAPA_ID_OFT                     0
#define MAC_QOS_CAPA_LEN_OFT                    1
#define MAC_QOS_CAPA_INFO_OFT                   2
#define MAC_QOS_CAPA_LEN                        3

// HT Capability IE
#define MAC_HT_CAPA_ID_OFT                      0
#define MAC_HT_CAPA_LEN_OFT                     1
#define MAC_HT_CAPA_INFO_OFT                    2
#define MAC_HT_CAPA_AMPDU_PARAM_OFT             4
#define MAC_HT_CAPA_SUPPORTED_MCS_SET_OFT       5
#define MAC_HT_CAPA_EXTENDED_CAPA_OFT           21
#define MAC_HT_CAPA_TX_BEAM_FORMING_CAPA_OFT    23
#define MAC_HT_CAPA_ASEL_CAPA_OFT               27
#define MAC_HT_CAPA_ELMT_LEN                    26
#define MAC_HT_CAPA_ELMT_LEN_WD                (CO_ALIGN4_HI(MAC_HT_CAPA_ELMT_LEN) / 4)
#define MAC_HT_CAPA_LEN                         28

// HT Operation IE
#define MAC_HT_OPER_ID_OFT                      0
#define MAC_HT_OPER_LEN_OFT                     1
#define MAC_HT_OPER_PRIM_CH_OFT                 2
#define MAC_HT_OPER_INFO_OFT                    3
#define MAC_HT_OPER_INFO_SUBSET2_OFT            4
#define MAC_HT_OPER_OP_MODE_MASK                0x0003
#define MAC_HT_OPER_NONGF_MASK                  0x0004
#define MAC_HT_OPER_OBSS_MASK                   0x0010
#define MAC_HT_OPER_INFO_SUBSET3_OFT            6
#define MAC_HT_OPER_BASIC_MSC_SET_OFT           8
#define MAC_HT_OPER_ELMT_LEN                    22
#define MAC_HT_OPER_LEN                         24

// HT Protection values
enum mac_ht_oper_prot_val
{
    MAC_HT_OPER_PROT_NO_PROT   = 0,
    MAC_HT_OPER_PROT_NONMEMBER,
    MAC_HT_OPER_PROT_20MHZ,
    MAC_HT_OPER_PROP_NON_HT_MIXED
};

// VHT Capability IE
#define MAC_VHT_CAPA_ELMT_LEN                   12
#define MAC_VHT_CAPA_ELMT_LEN_WD               (CO_ALIGN4_HI(MAC_VHT_CAPA_ELMT_LEN) / 4)
#define MAC_VHT_CAPA_LEN                        14
#define MAC_VHT_CAPA_ID_OFT                     0
#define MAC_VHT_CAPA_LEN_OFT                    1
#define MAC_VHT_CAPA_INFO_OFT                   2
#define MAC_VHT_RX_MCS_MAP_OFT                  6
#define MAC_VHT_RX_HIGHEST_RATE_OFT             8
#define MAC_VHT_TX_MCS_MAP_OFT                  10
#define MAC_VHT_TX_HIGHEST_RATE_OFT             12

// VHT Operation IE
#define MAC_VHT_OPER_ELMT_LEN                   5
#define MAC_VHT_OPER_ID_OFT                     0
#define MAC_VHT_OPER_LEN_OFT                    1
#define MAC_VHT_CHAN_WIDTH_OFT                  2
#define MAC_VHT_CENTER_FREQ0_OFT                3
#define MAC_VHT_CENTER_FREQ1_OFT                4
#define MAC_VHT_BASIC_MCS_OFT                   5

// 20/40 Coexistence IE
#define MAC_20_40_COEXISTENCE_ID_OFT                0
#define MAC_20_40_COEXISTENCE_LEN_OFT               1
#define MAC_20_40_COEXISTENCE_INFO_OFT              2
#define MAC_20_40_COEXISTENCE_ELMT_LEN              1
#define MAC_20_40_COEXISTENCE_LEN                   3
#define MAC_20_40_COEXISTENCE_INFO_REQ_MASK         0x01
#define MAC_20_40_COEXISTENCE_40_INTOLERANT_MASK    0x02
#define MAC_20_40_COEXISTENCE_20_BSS_WIDTH_REQ_MASK 0x04
#define MAC_20_40_COEXISTENCE_OBSS_SCAN_REQ_MASK    0x08
#define MAC_20_40_COEXISTENCE_OBSS_SCAN_GRANT_MASK  0x10

// Mgmt MIC IE
#define MAC_MGMT_MIC_ID_OFT    0
#define MAC_MGMT_MIC_LEN_OFT   1
#define MAC_MGMT_MIC_KEYID_OFT 2
#define MAC_MGMT_MIC_IPN_OFT   4
#define MAC_MGMT_MIC_IPN_LEN   6
#define MAC_MGMT_MIC_MIC_OFT   10
#define MAC_MGMT_MIC_MIC_LEN   8
#define MAC_MGMT_MIC_LEN       18

// QOS INFORMATION FIELD
#define MAC_QINFO_OFT                           6

// QOS INFORMATION FIELD received from the AP
#define MAC_QINFO_SET_COUNT_OFT                 8
#define MAC_QINFO_SET_UAPSD_OFT                 8

// Bit position for QOS INFORMATION FEILD received from the AP
#define MAC_QINFO_SET_COUNT                     0x07   // Bits 0,1,2
#define MAC_QINFO_SET_UAPSD                     0x80   // Bit 7

#define SUP_MCS_RX_HIGHEST_RATE_OFT             10 // 10,11th Byte from Start of MCS.
#define SUP_MCS_RX_HIGHEST_RATE_SIZE            10 // 10 Bits
#define SUP_TX_MCS_SET_OFT                      12 // 12th Byte from Start of MCS.

/*
* MANAGEMENT FRAME DESCRIPTION
****************************************************************************************
*/
#define MAC_BEACON_MAX_LEN              333
#define MAC_BEACONINFO_SIZE              78   // size without TIM
#define MAC_TIM_SIZE                    251   // max TIM size
// Attention, in a non-AP STA the IBSS
// parameter set is stored in place of
// the TIM -> min size 4

#define MAC_BEACON_HDR_LEN               36

/*
 * Beacon Frame offset (Table 5 p46)
 */
#define MAC_BEACON_TIMESTAMP_OFT          MAC_SHORT_MAC_HDR_LEN   // Order 1
#define MAC_BEACON_INTERVAL_OFT          (MAC_SHORT_MAC_HDR_LEN + 8)   // Order 2
#define MAC_BEACON_CAPA_OFT              (MAC_SHORT_MAC_HDR_LEN + 10)   // Order 3
#define MAC_BEACON_VARIABLE_PART_OFT     (MAC_SHORT_MAC_HDR_LEN + 12)   // Order 4

#define BEACON_CAP_ESS	    0x0001
#define BEACON_CAP_IBSS	    0x0002
#define BEACON_CAP_PRIVACY	0x0010
#define BEACON_CAP_RRM	    0x1000


/*
 * Probe Request Frame offset
 */
#define MAC_PROBEREQ_SSID_OFT             0   // Order 1

/*
 * Probe Response Frame offset (Table 12 p49)
 */
#define MAC_PROBE_TIMESTAMP_OFT           0   // Order 1
#define MAC_PROBE_INTERVAL_OFT            8   // Order 2
#define MAC_PROBE_CAPA_OFT               10   // Order 3
#define MAC_PROBE_SSID_OFT               12   // Order 4

/*
 * Authentication Frame offset
 */
#define MAC_AUTH_ALGONBR_OFT              0   // Order 1
#define MAC_AUTH_SEQNBR_OFT               2   // Order 2
#define MAC_AUTH_STATUS_OFT               4   // Order 3
#define MAC_AUTH_CHALLENGE_OFT            6   // Order 4
#define MAC_AUTH_CHALLENGE_LEN          128
//challenge text IE size
#define CHALLENGE_TEXT_SIZE             130

/*
 * Association Req Frame offset
 */
#define MAC_ASSO_REQ_CAPA_OFT             0   // Order 1
#define MAC_ASSO_REQ_LISTEN_OFT           2   // Order 2
#define MAC_ASSO_REQ_SSID_OFT             4   // Order 3

/*
 * Association Rsp Frame offset
 */
#define MAC_ASSO_RSP_CAPA_OFT             0   // Order 1
#define MAC_ASSO_RSP_STATUS_OFT           2   // Order 2
#define MAC_ASSO_RSP_AID_OFT              4   // Order 3
#define MAC_ASSO_RSP_RATES_OFT            6   // Order 4

/*
 * Re association Req Frame offset
 */
#define MAC_REASSO_REQ_CAPA_OFT           0   // Order 1
#define MAC_REASSO_REQ_LISTEN_OFT         2   // Order 2
#define MAC_REASSO_REQ_AP_ADDR_OFT        4   // Order 3
#define MAC_REASSO_REQ_SSID_OFT          10   // Order 4

/*
 * Deauthentication Req Frame offset
 */

#define MAC_DEAUTH_REASON_OFT             0

/*
 * De association Req Frame offset
 */

#define MAC_DISASSOC_REASON_OFT           0
#define MAC_DISASSOC_REASON_LEN           2

/*
 * Category and action for all action frames
 */
#define MAC_ACTION_CATEGORY_OFT           0
#define MAC_ACTION_ACTION_OFT             1
#define MAC_ACTION_TOKEN_OFT              2
#define MAC_ACTION_P2P_ACTION_OFT         5
#define MAC_ACTION_P2P_TAGGED_OFT         7

#define MAC_SPECTRUM_ACTION_CATEGORY        0
#define MAC_WMMAC_QOS_ACTION_CATEGORY       1
#define MAC_DLS_ACTION_CATEGORY             2
#define MAC_BA_ACTION_CATEGORY              3
#define MAC_PUBLIC_ACTION_CATEGORY          4
#define MAC_RADIO_MEASURE_ACTION_CATEGORY   5
#define MAC_FAST_BSS_ACTION_CATEGORY        6
#define MAC_HT_ACTION_CATEGORY              7
#define MAC_SA_QUERY_ACTION_CATEGORY        8
#define MAC_PROT_PUBLIC_ACTION_CATEGORY     9
#define MAC_WNM_ACTION_CATEGORY            10
#define MAC_UNPROT_WNM_ACTION_CATEGORY     11
#define MAC_TDLS_ACTION_CATEGORY           12
#define MAC_MESH_ACTION_CATEGORY           13
#define MAC_MULTIHOP_ACTION_CATEGORY       14
#define MAC_SELF_PROT_ACTION_CATEGORY      15
#define MAC_VHT_ACTION_CATEGORY            21
#define MAC_VENDOR_PROT_ACTION_CATEGORY   126
#define MAC_VENDOR_ACTION_CATEGORY        127
#define MAC_ACTION_CATEGORY_ERROR         128

#define MAC_P2P_ACTION_NOA_SUBTYPE          0

#define MAC_SELF_PROT_ACTION_CAPA_OFT     2
#define MAC_SELF_PROT_ACTION_CAPA_LEN     2
#define MAC_SELF_PROT_ACTION_AID_OFT      4
#define MAC_SELF_PROT_ACTION_AID_LEN      2

/* See IEEE 802.1H for LLC/SNAP encapsulation/decapsulation */
/* Ethernet-II snap header (RFC1042 for most EtherTypes) */
#define FRAME_BODY_LLC_H 0x0003AAAA
#define FRAME_BODY_LLC_L 0x0D890000
#define PAYLOAD_TYPE_TDLS 0x02

#define MAC_ENCAPSULATED_LLC_H_OFT  0
#define MAC_ENCAPSULATED_LLC_L_OFT  4
#define MAC_ENCAPSULATED_PAYLOAD_TYPE_OFT  8
#define MAC_ENCAPSULATED_PAYLOAD_OFT  9

/*
****************************************************************************************
****************************************************************************************
*----------------------- WMM_AC FRAMES AND VALUES DESCRIPTIONS -------------------------
****************************************************************************************
****************************************************************************************
*/

/*
*---------------------------------------------------------------------------------------
*---------------------------------WMM-AC fields offsets---------------------------------
*---------------------------------------------------------------------------------------
*/

#define MAC_WMMAC_OUI_BYTE                              0x00
#define MAC_WMMAC_OUI_TYPE_BYTE                         0x50
#define MAC_WMMAC_OUI_SUBTYPE_BYTE                      0xF2

/* TSPEC field offsets
*--------------------------------------------------------------------------------------*/
#define MAC_WMMAC_TSPEC_HDR_OFT                         0
#define MAC_WMMAC_TSPEC_TSINFO_OFT                      8
#define MAC_WMMAC_TSPEC_NOM_MSDU_SIZE_OFT              11
#define MAC_WMMAC_TSPEC_MAX_MSDU_SIZE_OFT              13
#define MAC_WMMAC_TSPEC_MINSI_OFT                      15
#define MAC_WMMAC_TSPEC_MAXSI_OFT                      19
#define MAC_WMMAC_TSPEC_INACTIVITY_INTERVAL_OFT        23
#define MAC_WMMAC_TSPEC_SUSPENSION_INTERVAL_OFT        27
#define MAC_WMMAC_TSPEC_SERVICE_START_TIME_OFT         31
#define MAC_WMMAC_TSPEC_MIN_DATA_RATE_OFT              35
#define MAC_WMMAC_TSPEC_MEAN_DATA_RATE_OFT             39
#define MAC_WMMAC_TSPEC_PEAK_DATA_RATE_OFT             43
#define MAC_WMMAC_TSPEC_BURST_SIZE_OFT                 47
#define MAC_WMMAC_TSPEC_DELAY_BOUND_OFT                51
#define MAC_WMMAC_TSPEC_MIN_PHY_RATE_OFT               55
#define MAC_WMMAC_TSPEC_SURPLUS_BW_OFT                 59
#define MAC_WMMAC_TSPEC_MEDUIM_TIME_OFT                61

/* TSINFO field offsets
*--------------------------------------------------------------------------------------*/
#define MAC_WMMAC_TSINFO_HDR_OFT                        0
#define MAC_WMMAC_TSINFO_BODY_OFT                       8

/* TSDELAY field offsets
*--------------------------------------------------------------------------------------*/
#define MAC_WMMSA_TSDELAY_HDR_OFT                       0
#define MAC_WMMSA_TSDELAY_BODY_OFT                      8
/*
*---------------------------------------------------------------------------------------
*---------------------------------WMM-AC fields subtypes--------------------------------
*---------------------------------------------------------------------------------------
*/


#define MAC_WMMAC_OUI_SUBTYPE_TSPEC_LEN                 61


#define MAC_OUI_SUBTYPE_WMMAC_TSINFO_LEN                9

// Used by the unit test of the WMM-AC
#define MAC_OUI_SUBTYPE_WMMAC_TSDELAY_LEN              10

// Bit position for TSPEC info field
#define MAC_TSPEC_TRAFIC_TYPE_OFT                        0
#define MAC_TSPEC_TRAFIC_TYPE_MASK                       (CO_BIT(0))

#define MAC_TSPEC_TSID_OFT                               1
#define MAC_TSPEC_TSID_MASK                              (CO_BIT(1)|CO_BIT(2)|CO_BIT(3)|CO_BIT(4))

#define MAC_TSPEC_DIRECTION_OFT                          5
#define MAC_TSPEC_DIRECTION_MASK                         (CO_BIT(5)|CO_BIT(6))

#define MAC_TSPEC_ACCESS_POLICY_OFT                      7
#define MAC_TSPEC_ACCESS_POLICY_MASK                     (CO_BIT(7)|CO_BIT(8))

#define MAC_TSPEC_AGGREGATION_OFT                        9
#define MAC_TSPEC_AGGREGATION_MASK                       (CO_BIT(9))

#define MAC_TSPEC_APSD_OFT                               10
#define MAC_TSPEC_APSD_MASK                              (CO_BIT(10))

#define MAC_TSPEC_USER_PRIORITY_OFT                      11
#define MAC_TSPEC_USER_PRIORITY_MASK                     (CO_BIT(11)|CO_BIT(12)|CO_BIT(13))

#define MAC_TSPEC_ACK_POLICY_OFT                         14
#define MAC_TSPEC_ACK_POLICY_MASK                        (CO_BIT(14)|CO_BIT(15))

#define MAC_TSPEC_SCHEDULE_OFT                           16
#define MAC_TSPEC_SCHEDULE_MASK                          (CO_BIT(16))

/*
*---------------------------------------------------------------------------------------
*---------------------------------WMM-AC Status Codes-----------------------------------
*---------------------------------------------------------------------------------------
*/
#define MAC_WMMAC_ST_ADDTS_SUCESS                      0
#define MAC_WMMAC_ST_DELTS_POOR_CHANNEL_COND          34
#define MAC_WMMAC_ST_ADDTS_REQUEST_DECLINED           37
#define MAC_WMMAC_ST_ADDTS_INVALID_PARAMETERS         38
#define MAC_WMMAC_ST_ADDTS_REJ_W_SUG_CHANGES          39
#define MAC_WMMAC_ST_ADDTS_REJ_W_DELAY_PRD            47

/*
*---------------------------------------------------------------------------------------
*---------------------------------WMM-AC Reason Codes-----------------------------------
*---------------------------------------------------------------------------------------
*/
#define MAC_WMMAC_RS_DELTS_QSTA_LEAVING               36
#define MAC_WMMAC_RS_DELTS_END_TS                     37
#define MAC_WMMAC_RS_DELTS_UNKOWN_TS                  38
#define MAC_WMMAC_RS_DELTS_TIMEOUT                    39
#define MAC_WMMAC_RS_DELTS_UNSPECIFIED                32


/*
*---------------------------------------------------------------------------------------
*---------------------------------WMM-AC Action Frames----------------------------------
*---------------------------------------------------------------------------------------
*/


/* CATEGORY \ ACTION values
*--------------------------------------------------------------------------------------*/

#define MAC_WMMAC_ACTION_ADDTS_REQ        0
#define MAC_WMMAC_ACTION_ADDTS_RSP        1
#define MAC_WMMAC_ACTION_DELTS            2
#define MAC_WMMAC_ACTION_SCHEDULE         3


/* ADDTS_REQ offsets
*--------------------------------------------------------------------------------------*/

#define MAC_ADDTS_REQ_TSPEC_OFT           3
#define MAC_ADDTS_REQ_TCLASS_OFT         66


/* ADDTS_RSP offsets
*--------------------------------------------------------------------------------------*/

#define MAC_ADDTS_RSP_STATUS_OFT          3
#define MAC_ADDTS_RSP_TSDELAY_OFT         5
#define MAC_ADDTS_RSP_TSPEC_OFT          17
#define MAC_ADDTS_RSP_TCLASS_OFT         80

/* DELTS offsets
*--------------------------------------------------------------------------------------*/

#define MAC_DELTS_TSINFO_OFT              2
#define MAC_DELTS_REASON_OFT             13

/*
*--------------------------------------------------------------------------------------*
*-----------------------------------WMM-AC END------------------------------------------
*--------------------------------------------------------------------------------------*
*/

/*
****************************************************************************************
****************************************************************************************
*----------------------- Block Ack FRAMES AND VALUES DESCRIPTIONS -------------------------
****************************************************************************************
****************************************************************************************
*/

/*
*---------------------------------------------------------------------------------------
*---------------------------------Block Ack fields offsets---------------------------------
*---------------------------------------------------------------------------------------
*/

#define MAC_BARINFO_PERTIDINFO_OFT              0
#define MAC_BARINFO_PERTIDINFO_STEP             1
#define MAC_BARINFO_SEQ_OFT                     2
#define MAC_BARINFO_PERTIDINFO_TID_STEP         4
#define MAC_CAPA_BA_OFT                         14

// the length of fields
#define MAC_BA_PARAMSET_LEN                     2
#define MAC_BA_TIMEOUT_LEN                      2
#define MAC_BA_BARINFO_LEN                      4
#define MAC_BA_DELBA_REASON_LEN                 2

// value of the The Fragment Number
#define MAC_BARINFO_SEQ_FRAG                    0

// offset of The DELPARAMSET
#define MAC_DELPARAMSET_START_OFT               0
#define MAC_DELPARAMSET_STEP_OFT                1

// Bit position for Parameter Set field
#define MAC_BA_PARAMSET_AMSDU_OFT               0
#define MAC_BA_PARAMSET_AMSDU_MASK              CO_BIT(0)

#define MAC_BA_PARAMSET_POLICY_OFT              1
#define MAC_BA_PARAMSET_POLICY_MASK             CO_BIT(1)

#define MAC_BA_PARAMSET_TID_OFT                 2
#define MAC_BA_PARAMSET_TID_MASK                (CO_BIT(2)|CO_BIT(3)|CO_BIT(4)|CO_BIT(5))

#define MAC_BA_PARAMSET_BUFFER_OFT              6
#define MAC_BA_PARAMSET_BUFFER_MASK             0xFFC0

//Bit position for Per TID Info field
#define MAC_BA_BARINFO_PERTIDINFO_TID_OFT       12
#define MAC_BA_BARINFO_PERTIDINFO_TID_MASK  (CO_BIT(12)|CO_BIT(13))|(CO_BIT(14)|CO_BIT(15))

//Bit position for Starting Sequence Control filed
#define MAC_BA_BARINFO_SEQ_FRAG_OFT             0
#define MAC_BA_BARINFO_SEQ_FRAG_MASK        (CO_BIT(0)|CO_BIT(1))|(CO_BIT(2)|CO_BIT(3))

#define MAC_BA_BARINFO_SEQ_STARTSEQ_OFT         4
#define MAC_BA_BARINFO_SEQ_STARTSEQ_MASK        0x7FF0

//Bit position for DELBA Parameter Set field
#define MAC_BA_DELPARAMSET_INIT_OFT             3
#define MAC_BA_DELBARINFO_INIT_MASK             CO_BIT(3)

#define MAC_BA_DELPARAMSET_TID_OFT              4
#define MAC_BA_DELPARAMSET_TID_MASK         (CO_BIT(4)|CO_BIT(5)|CO_BIT(6)|CO_BIT(7))

/*
*---------------------------------------------------------------------------------------
*---------------------------------Block Ack Status Codes-----------------------------------
*---------------------------------------------------------------------------------------
*/
#define MAC_BA_ST_SUCCESS                     0
#define MAC_BA_ST_REQUEST_REFUSED             37
#define MAC_BA_ST_INVALID_PARAMETERS          38

/*
*---------------------------------------------------------------------------------------
*---------------------------------Block Ack Reason Codes-----------------------------------
*---------------------------------------------------------------------------------------
*/
#define MAC_BA_RS_STA_LEAVING                 36
#define MAC_BA_RS_END_BA                      37
#define MAC_BA_RS_UNKNOWN_BA                  38
#define MAC_BA_RS_TIMEOUT                     39

/*
*---------------------------------------------------------------------------------------
*---------------------------------Block Ack Action Frames----------------------------------
*---------------------------------------------------------------------------------------
*/


/* CATEGORY \ ACTION values
*--------------------------------------------------------------------------------------*/

#define MAC_BA_ACTION_ADDBA_REQ         0
#define MAC_BA_ACTION_ADDBA_RSP         1
#define MAC_BA_ACTION_DELBA             2


/* ADDBA_REQ offsets
*--------------------------------------------------------------------------------------*/

#define MAC_ADDBA_REQ_PARAMSET_OFT      3
#define MAC_ADDBA_REQ_TIMEOUT_OFT       5
#define MAC_ADDBA_REQ_BARINFO_OFT       7


/* ADDBA_RSP offsets
*--------------------------------------------------------------------------------------*/

#define MAC_ADDBA_RSP_STATUS_OFT        3
#define MAC_ADDBA_RSP_PARAMSET_OFT      5
#define MAC_ADDBA_RSP_TIMEOUT_OFT       7


/* DELBA offsets
*--------------------------------------------------------------------------------------*/

#define MAC_DELBA_PARAMSET_OFT          2
#define MAC_DELBA_REASON_OFT            4

/*
*--------------------------------------------------------------------------------------*
*-----------------------------------Block Ack END------------------------------------------
*--------------------------------------------------------------------------------------*
*--------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------
*---------------------------------TDLS Action Frames----------------------------------
*---------------------------------------------------------------------------------------
*/


/* CATEGORY \ ACTION values
*--------------------------------------------------------------------------------------*/
#define MAC_TDLS_ACTION_SETUP_REQ           0
#define MAC_TDLS_ACTION_SETUP_RSP           1
#define MAC_TDLS_ACTION_SETUP_CFM           2
#define MAC_TDLS_ACTION_TEARDOWN            3
#define MAC_TDLS_ACTION_PEER_TRAFFIC_IND    4
#define MAC_TDLS_ACTION_CHANSW_REQ          5
#define MAC_TDLS_ACTION_CHANSW_RSP          6
#define MAC_TDLS_ACTION_PEER_TRAFFIC_RSP    9
#define MAC_TDLS_ACTION_DISCOVERY_REQ       10

#define TDLS_CHANSW_REQ_TARGET_CH_OFFSET    2
#define TDLS_CHANSW_REQ_OP_CLASS            3
#define TDLS_CHANSW_REQ_IES_OFFSET          4
#define TDLS_CHANSW_REQ_IE_SEC_CH_OFT_LEN   3
#define TDLS_CHANSW_REQ_IE_LINK_ID_LEN      20
#define TDLS_CHANSW_REQ_IE_CH_SWITCH_TIMING_LEN 6
#define TDLS_CHANSW_REQ_IE_WIDE_BW_CHAN_SWITCH_LEN 5

#define TDLS_CHANSW_RSP_STATUS_OFFSET       2
#define TDLS_CHANSW_RSP_IES_OFFSET          4

#define TDLS_CHANSW_REQUEST_ACCEPTED        0
#define TDLS_CHANSW_REQUEST_DECLINED        37

#define TDLS_PEER_TRAFFIC_IND_IES_OFFSET    3
#define TDLS_PEER_TRAFFIC_IND_IE_LINK_ID_LEN 20
#define TDLS_PEER_TRAFFIC_IND_IE_PTI_CTRL_LEN 5
#define TDLS_PEER_TRAFFIC_IND_IE_TPU_BUF_STATUS_LEN 3

#define TDLS_PEER_TRAFFIC_RSP_IES_OFFSET    3
#define TDLS_PEER_TRAFFIC_RSP_IE_LINK_ID_LEN 20

/*
*--------------------------------------------------------------------------------------*
*-----------------------------------TDLS END------------------------------------------
*--------------------------------------------------------------------------------------*
*--------------------------------------------------------------------------------------*/


/*
 * LINK Management Defines
 */

#define MAC_HEAD_HT_OFT                  32
#define MAC_MAI_MASK                     0x003C
#define MAC_MAI_OFT                      2
#define MAC_MAI_IND_ASELI                14
#define MAC_MFB_MASK                     0xFE00
#define MAC_MFB_OFT                      9
#define NO_FEEDBACK                      127
#define MAC_ADDR2_OFT                    10
#define MCS_IDX3_MASK                    0x0000FF
#define MCS_IDX2_MASK                    0x00FF00
#define MCS_IDX1_MASK                    0xFF0000
#define MCS_IDX2_OFT                     8
#define MCS_IDX1_OFT                     16


/* ------------------------------------------------------------------------------------ *
 * ------------------ Spectrum & Power Management Frame Offset ------------------------ *
 * ------------------------------------------------------------------------------------ */

#define MAC_ACTION_FRAME_INFO_OFT           3
#define MAC_ACTION_CH_SWITCH_FRAME_INFO_OFT 2
#define MAC_ACTION_FRAME_PRE_INFO_LEN       3

//Spectrum & Power Management Element Offset

// Power Constraint Element
#define MAC_INFOELT_POWER_CONSTRAINT_OFT     2

#define MAC_POWER_CONSTRAINT_ELT_LEN         3

// Power Capability Element
#define MAC_INFOELT_MIN_TX_POWER_OFT         2
#define MAC_INFOELT_MAX_TX_POWER_OFT         3

#define MAC_POWER_CAPABILITY_ELT_LEN         4
#define MAC_POWER_CAPABILITY_ELT_LEN_FIELD   2

// TPC Report Info Element sub-offsets
#define MAC_INFOELT_TX_POWER_OFT             2
#define MAC_INFOELT_LINK_MARGIN_OFT          3

// TPC Report and Request IE lengths
#define MAC_TPC_REQUEST_ELT_LEN              2
#define MAC_TPC_REPORT_ELT_LEN               4
// TPC Report and Request IE's length field values
#define MAC_INFOELT_LEN_VAL_TPC_REQ          0
#define MAC_INFOELT_LEN_VAL_TPC_REP          2
// TPC Report and Request Action frame lengths
#define MAC_ACTION_FRAME_TPC_REQ_LEN         5
#define MAC_ACTION_FRAME_TPC_REP_LEN         7


// Supported Channels Element
#define MAC_INFOELT_FIRST_CHANNEL_OFT        2
#define MAC_INFOELT_NUMBER_CHANNEL_OFT       3

// Channel Switch Announcement Element
#define MAC_INFOELT_SWITCH_MODE_OFT          2
#define MAC_INFOELT_SWITCH_NEW_CHAN_OFT      3
#define MAC_INFOELT_SWITCH_COUNT_OFT         4

#define MAC_CHANNEL_SWITCH_INFO_LEN          3
#define MAC_CHANNEL_SWITCH_ELT_LEN           5

// Extended Channel Switch Announcement Element
#define MAC_INFOELT_EXT_SWITCH_MODE_OFT      2
#define MAC_INFOELT_EXT_SWITCH_NEW_CLASS     3
#define MAC_INFOELT_EXT_SWITCH_NEW_CHAN_OFT  4
#define MAC_INFOELT_EXT_SWITCH_COUNT_OFT     5

#define MAC_EXT_CHANNEL_SWITCH_INFO_LEN      4
#define MAC_EXT_CHANNEL_SWITCH_ELT_LEN       6

// Secondary Channel Offset Element
#define MAC_INFOELT_SEC_CH_OFFSET_OFT        2

#define MAC_INFOELT_SEC_CH_OFFSET_INFO_LEN   1
#define MAC_INFOELT_SEC_CH_OFFSET_ELT_LEN    3

#define MAC_INFOELT_SEC_CH_OFFSET_SEC_CH_OFT 2
#define MAC_INFOELT_SEC_CH_OFFSET_NO_SEC_CH  0
#define MAC_INFOELT_SEC_CH_OFFSET_SEC_ABOVE  1
#define MAC_INFOELT_SEC_CH_OFFSET_SEC_BELOW  3

// Wide BAndwidth Channel switch
#define MAC_INFOELT_WIDE_BW_CHAN_SWITCH_NEW_CW_OFT 2
#define MAC_INFOELT_WIDE_BW_CHAN_SWITCH_NEW_CENTER1_OFT 3
#define MAC_INFOELT_WIDE_BW_CHAN_SWITCH_NEW_CENTER2_OFT 4
#define MAC_INFOELT_WIDE_BW_CHAN_SWITCH_INFO_LEN 3
#define MAC_INFOELT_WIDE_BW_CHAN_SWITCH_ELT_LEN  5

// Measurement Request/Report Element
#define MAC_INFOELT_MEASURE_TOKEN_OFT        2
#define MAC_INFOELT_MEASURE_MODE_OFT         3
#define MAC_INFOELT_MEASURE_TYPE_OFT         4
#define MAC_INFOELT_MEASURE_CH_NBR_OFT       5
#define MAC_INFOELT_MEASURE_START_TIME_OFT   6
#define MAC_INFOELT_MEASURE_DURATION_OFT     14

#define MAC_INFOELT_MEASURE_INFO_OFT        16

#define MAC_MEAS_REQ_CNF_ELT_LEN             5
#define MAC_MEAS_REQ_ELT_LEN                16
#define MAC_MEAS_REQ_FRAME_LEN              19
#define MAC_MEAS_REP_BASIC_ELT_LEN          17
#define MAC_MEAS_REP_BASIC_FRAME_LEN        20
#define MAC_MEAS_REP_CCA_ELT_LEN            17
#define MAC_MEAS_REP_CCA_FRAME_LEN          20
#define MAC_MEAS_REP_RPI_ELT_LEN            24
#define MAC_MEAS_REP_RPI_FRAME_LEN          27
#define MAC_MEAS_REP_ERR_ELT_LEN             5
#define MAC_MEAS_REP_ERR_FRAME_LEN           8

// Mobility Domain Element (MDE)
#define MAC_INFOELT_MDE_ID_OFT               0
#define MAC_INFOELT_MDE_LEN_OFT              1
#define MAC_INFOELT_MDE_MDID_OFT             2
#define MAC_INFOELT_MDE_FT_CAPA_POL_OFT      4
#define MAC_INFOELT_MDE_MDID_LEN             2
#define MAC_INFOELT_MDE_FT_CAPA_POL_LEN      1
#define MAC_INFOELT_MDE_ELMT_LEN             3
#define MAC_INFOELT_MDE_LEN                  5

// Link Identifier Element
#define MAC_INFOELT_LINK_ID_LEN              18
#define MAC_INFOELT_LINK_ID_BSSID_OFT        2
#define MAC_INFOELT_LINK_ID_INIT_STA_OFT     8
#define MAC_INFOELT_LINK_ID_RESP_STA_OFT     14

// Link Identifier Element
#define MAC_INFOELT_CH_SWITCH_TIMING_LEN     4
#define MAC_INFOELT_CH_SWITCH_TIMING_SWTIME_OFT 2
#define MAC_INFOELT_CH_SWITCH_TIMING_SWTOUT_OFT 4

// PTI Control Element
#define MAC_INFOELT_PTI_CONTROL_LEN          3
#define MAC_INFOELT_PTI_CONTROL_TID_OFT      2
#define MAC_INFOELT_PTI_CONTROL_SEQ_CTRL_OFT 3

// TPU Buffer Status Element
#define MAC_INFOELT_TPU_BUF_STATUS_LEN       1
#define MAC_INFOELT_TPU_BUF_STATUS_AC_STATUS 2

// Spectrum management frame values:
//----------------------------------

//TO_DO_MMH:Remove unused fields

//MAC Action Spectrum Categorie => Sub Action
#define MAC_ACTION_MEASURE_REQUEST        0
#define MAC_ACTION_MEASURE_REPORT         1
#define MAC_ACTION_TPC_REQUEST            2
#define MAC_ACTION_TPC_REPORT             3
#define MAC_ACTION_CHANNEL_SWITCH         4

// Channel Switch Annoncement Mode
#define MAC_SWITCH_MODE_TX_TRAFFIC_CONT   0
#define MAC_SWITCH_MODE_TX_TRAFFIC_STOP   1

// Measurement Request Mode
#define MAC_MEASURE_MODE_ENABLE           CO_BIT(1)
#define MAC_MEASURE_MODE_REQUEST          CO_BIT(2)
#define MAC_MEASURE_MODE_REPORT           CO_BIT(3)

// Measurement Report Mode
#define MAC_MEASURE_MODE_LATE             CO_BIT(0)
#define MAC_MEASURE_MODE_INCAPABLE        CO_BIT(1)
#define MAC_MEASURE_MODE_REFUSED          CO_BIT(2)

// Measurement Request Type
#define MAC_MEASURE_TYPE_BASIC            0
#define MAC_MEASURE_TYPE_CCA              1
#define MAC_MEASURE_TYPE_RPI              2

// Measurement Report Map Bits
#define MAC_MEASURE_MAP_FREE              0x00
#define MAC_MEASURE_MAP_BSS               CO_BIT(0)
#define MAC_MEASURE_MAP_OFDM              CO_BIT(1)
#define MAC_MEASURE_MAP_UNIDENTIFIED      CO_BIT(2)
#define MAC_MEASURE_MAP_RADAR             CO_BIT(3)
#define MAC_MEASURE_MAP_UNMEASURED        CO_BIT(4)
#define MAC_MEASURE_MAP_MASK              0x1F

/*
 * We set the default switch count to Zero which indicates that the switch shall occur at
 * any time after the frame containing the element is transmitted.
 * This is done to be able to give the STA time to know the new channel number in case the
 * channel switch fails and another new channel was decided
 */
#define MAC_DEFAULT_SWITCH_COUNT            0
#define MAC_DEFAULT_RADAR_CHECK_TIME        60

enum
{
    REMOTE_MEAS_REQ_LATE = 1,
    REMOTE_MEAS_REQ_INCAPABLE,
    REMOTE_MEAS_REQ_REFUSED
};

/* ------------------------------------------------------------------------------------ *
 * ----------------- Spectrum & Power Management Frame Offset End --------------------- *
 * ------------------------------------------------------------------------------------ */

/*
*---------------------------------------------------------------------------------------
*--------------------------------- HT Action Frames------------------------------------
*---------------------------------------------------------------------------------------
*/


/* HT Action Field values
*--------------------------------------------------------------------------------------*/

#define MAC_CHAN_WIDTH_HT_ACTION                0
#define MAC_SMPS_HT_ACTION                      1
#define MAC_PSMP_HT_ACTION                      2
#define MAC_SET_PCO_PHASE_HT_ACTION             3
#define MAC_CSI_HT_ACTION                       4
#define MAC_NON_COMP_BEAMFORMING_HT_ACTION      5
#define MAC_COMP_BEAMFORMING_HT_ACTION          6
#define MAC_ASEL_INDICES_FEEDBACK_HT_ACTION     7


/* HT channel width offsets
*--------------------------------------------------------------------------------------*/
#define MAC_CHAN_WIDTH_WIDTH_OFT                2

/* HT SM Power Control field offset
*--------------------------------------------------------------------------------------*/
#define MAC_SM_PRW_CTRL_OFT                     2
#define MAC_SMPS_ENABLE_BIT                     CO_BIT(0)
#define MAC_SMPS_DYNAMIC_BIT                    CO_BIT(1)


/*
*---------------------------------------------------------------------------------------
*--------------------------------- VHT Action Frames------------------------------------
*---------------------------------------------------------------------------------------
*/


/* VHT Action Field values
*--------------------------------------------------------------------------------------*/

#define MAC_COMP_BEAMFORMING_VHT_ACTION         0
#define MAC_GROUP_ID_MGMT_VHT_ACTION            1
#define MAC_OP_MODE_NOTIF_VHT_ACTION            2


/* GroupId management offsets
*--------------------------------------------------------------------------------------*/
#define MAC_GROUP_ID_MGT_PAYL_LENGTH            26
#define MAC_GROUP_ID_MGT_MEMBERSHIP_OFT         2
#define MAC_GROUP_ID_MGT_USER_POS_OFT           10


/* VHT operation mode notification offsets
*--------------------------------------------------------------------------------------*/
#define MAC_OP_MODE_NOTIF_OPMODE_OFT            2
#define MAC_OPMODE_BW_OFT                       0
#define MAC_OPMODE_BW_MSK                       (0x3 << MAC_OPMODE_BW_OFT)
#define MAC_OPMODE_RXNSS_OFT                    4
#define MAC_OPMODE_RXNSS_MSK                    (0x7 << MAC_OPMODE_RXNSS_OFT)
#define MAC_OPMODE_RXNSS_TYPE_BIT               CO_BIT(7)


/*
* ELEMENT ID DEFINITION (Table 20 page 55)
****************************************************************************************
*/
#define MAC_INFOELT_ID_OFT                0
#define MAC_INFOELT_LEN_OFT               1
#define MAC_INFOELT_INFO_OFT              2

#define MAC_ELTID_SSID                    0
#define MAC_ELTID_RATES                   1
#define MAC_ELTID_FH                      2
#define MAC_ELTID_DS                      3
#define MAC_ELTID_CF                      4
#define MAC_ELTID_TIM                     5
#define MAC_ELTID_IBSS                    6
#define MAC_ELTID_COUNTRY                 7             // optional
#define MAC_ELTID_HOP_PARAM               8             // optional
#define MAC_ELTID_HOP_TABLE               9             // optional
#define MAC_ELTID_REQUEST                10             // optional
#define MAC_ELTID_BSS_LOAD               11
#define MAC_ELTID_EDCA_PARAM             12

#define MAC_ELTID_CHALLENGE              16
#define MAC_ELTID_POWER_CONSTRAINT       32             // Spectrum & Power Management
#define MAC_ELTID_POWER_CAPABILITY       33             // Spectrum & Power Management
#define MAC_POWER_CAPABILITY_IE_LEN      2              // Spectrum & Power Management
#define MAC_ELTID_TPC_REQUEST            34             // Spectrum & Power Management
#define MAC_ELTID_TPC_REPORT             35             // Spectrum & Power Management
#define MAC_ELTID_SUPPORTED_CHANNELS     36             // Spectrum & Power Management
#define MAC_ELTID_CHANNEL_SWITCH         37             // Spectrum & Power Management
#define MAC_ELTID_MEASUREMENT_REQUEST    38             // Spectrum & Power Management
#define MAC_ELTID_MEASUREMENT_REPORT     39             // Spectrum & Power Management
#define MAC_ELTID_QUIET                  40             // Spectrum & Power Management
#define MAC_ELTID_IBSS_DFS               41             // Spectrum & Power Management

#define MAC_ELTID_ERP                    42
#define MAC_ELTID_HT_CAPA                45
#define MAC_ELTID_QOS_CAPA               46
#define MAC_ELTID_RSN_IEEE               48
#define MAC_ELTID_EXT_RATES              50
#define MAC_ELTID_MDE                    54  // Mobility Domain
#define MAC_ELTID_FTE                    55  // Fast BSS Transition
#define MAC_ELTID_SUPP_OPER_CLASS        59
#define MAC_ELTID_EXT_CHANNEL_SWITCH     60
#define MAC_ELTID_SEC_CH_OFFSET          62
#define MAC_ELTID_20_40_COEXISTENCE      72
#define MAC_ELTID_OBSS_SCAN_PARAM        74
#define MAC_ELTID_MGMT_MIC               76
#define MAC_ELTID_HT_OPERATION           61
#define MAC_ELTID_LINK_IDENTIFIER        101
#define MAC_ELTID_CHANNEL_SWITCH_TIMING  104
#define MAC_ELTID_PTI_CONTROL            105
#define MAC_ELTID_TPU_BUFFER_STATUS      106
#define MAC_ELTID_MESH_CONF              113
#define MAC_ELTID_MESH_ID                114
#define MAC_ELTID_MESH_LINK_METRIC_REP   115
#define MAC_ELTID_CONGESTION_NOTIF       116
#define MAC_ELTID_MESH_PEER_MGMT         117
#define MAC_ELTID_MESH_CHAN_SWITCH_PARAM 118
#define MAC_ELTID_MESH_AWAKE_WINDOW      119
#define MAC_ELTID_MESH_GANN              125
#define MAC_ELTID_MESH_RANN              126
#define MAC_ELTID_EXT_CAPA               127 // This number need to be confirmed.
#define MAC_ELTID_MESH_PREQ              130
#define MAC_ELTID_MESH_PREP              131
#define MAC_ELTID_MESH_PERR              132
#define MAC_ELTID_VHT_CAPA                   191
#define MAC_ELTID_VHT_OPERATION              192
#define MAC_ELTID_WIDE_BANDWIDTH_CHAN_SWITCH 194
#define MAC_ELTID_VHT_TRANSMIT_PWR_ENVELOP   195
#define MAC_ELTID_CHAN_SWITCH_WRP            196
#define MAC_ELTID_OUI                    221 // Proprietary   (0xDD)

#define MAC_OUI_TYPE_WPA                  1
#define MAC_OUI_TYPE_WME                  2
#define MAC_OUI_SUBTYPE_WME_INFORMATION   0
#define MAC_OUI_SUBTYPE_WME_PARAMETER     1
#define MAC_OUI_TYPE_WMMAC                2
#define MAC_OUI_SUBTYPE_WMMAC_TSPEC       2
#define MAC_OUI_SUBTYPE_WMMAC_TSDELAY     8
#define MAC_OUI_SUBTYPE_WMMAC_TSINFO      10

// WME IE OFFSET
#define MAC_OUI_PARAM_QOS_INFO_OFT        8
#define MAC_OUI_PARAM_RESERVED_OFT        9
#define MAC_OUI_PARAM_BK_PARAM_OFT       14
#define MAC_OUI_PARAM_BE_PARAM_OFT       10
#define MAC_OUI_PARAM_VI_PARAM_OFT       18
#define MAC_OUI_PARAM_VO_PARAM_OFT       22

/*
 * STATUS CODE
 ****************************************************************************************
 */
#define MAC_ST_SUCCESSFUL                   0
#define MAC_ST_FAILURE                      1
#define MAC_ST_RESERVED                     2
#define MAC_ST_CAPA_NOT_SUPPORTED           10
#define MAC_ST_REASSOC_NOT_ASSOC            11
#define MAC_ST_ASSOC_DENIED                 12
#define MAC_ST_AUTH_ALGO_NOT_SUPPORTED      13
#define MAC_ST_AUTH_FRAME_WRONG_SEQ         14
#define MAC_ST_AUTH_CHALLENGE_FAILED        15
#define MAC_ST_AUTH_TIMEOUT                 16
#define MAC_ST_ASSOC_TOO_MANY_STA           17
#define MAC_ST_ASSOC_RATES_NOT_SUPPORTED    18
#define MAC_ST_ASSOC_PREAMBLE_NOT_SUPPORTED 19

#define MAC_ST_ASSOC_SPECTRUM_REQUIRED   22
#define MAC_ST_ASSOC_POWER_CAPA          23
#define MAC_ST_ASSOC_SUPPORTED_CHANNEL   24
#define MAC_ST_ASSOC_SLOT_NOT_SUPPORTED  25

#define MAC_ST_REFUSED_TEMPORARILY       30
#define MAC_ST_INVALID_MFP_POLICY        31

#define MAC_ST_INVALID_IE                40             // draft 7.0 extention
#define MAC_ST_GROUP_CIPHER_INVALID      41             // draft 7.0 extention
#define MAC_ST_PAIRWISE_CIPHER_INVALID   42             // draft 7.0 extention
#define MAC_ST_AKMP_INVALID              43             // draft 7.0 extention
#define MAC_ST_UNSUPPORTED_RSNE_VERSION  44             // draft 7.0 extention
#define MAC_ST_INVALID_RSNE_CAPA         45             // draft 7.0 extention
#define MAC_ST_CIPHER_SUITE_REJECTED     46             // draft 7.0 extention


/*
* REASON CODE
****************************************************************************************
*/
#define MAC_RS_RESERVED                           0
#define MAC_RS_UNSPECIFIED                        1
#define MAC_RS_PREVIOUS_AUTH_NOT_VALID            2
#define MAC_RS_DEAUTH_SENDER_LEFT_IBSS_ESS        3
#define MAC_RS_DIASSOC_INACTIVITY                 4
#define MAC_RS_DIASSOC_TOO_MANY_STA               5
#define MAC_RS_CLASS_2_FORBIDDEN                  6
#define MAC_RS_CLASS_3_FORBIDDEN                  7
#define MAC_RS_DEAUTH_SENDER_LEFT_BSS             8
#define MAC_RS_STA_REQ_ASSOC_NOT_AUTH             9
#define MAC_RS_DIASSOC_POWER_CAPA                10
#define MAC_RS_DIASSOC_SUPPORTED_CHANNEL         11
#define MAC_RS_RESERVED_12                       12
#define MAC_RS_INVALID_IE                        13
#define MAC_RS_MIC_FAILURE                       14
#define MAC_RS_4WAY_HANDSHAKE_TIMEOUT            15
#define MAC_RS_GROUP_KEY_UPDATE_TIMEOUT          16
#define MAC_RS_BAD_RSNIE                         17
#define MAC_RS_GROUP_CIPHER_INVALID              18
#define MAC_RS_PAIRWISE_CIPHER_INVALID           19
#define MAC_RS_AKMP_INVALID                      20
#define MAC_RS_UNSUPPORTED_RSNE_VERSION          21
#define MAC_RS_INVALID_RSNE_CAPA                 22
#define MAC_RS_802_1X_AUTH_FAIL                  23
#define MAC_RS_CIPHER_SUITE_REJECTED             24
#define MAC_RS_TIMEOUT                           39


#define MAC_RAW_WME_INFO_ELMT_DEFAULT                                         \
{                                                                             \
    MAC_ELTID_OUI, 0x07,                                                      \
    0x00, 0x50, 0xF2, MAC_OUI_TYPE_WME,         /* additionnal WPA OUI     */ \
    MAC_OUI_SUBTYPE_WME_INFORMATION, 0x01,      /* OUI subtype and version */ \
    0x00                                        /* QoS field               */ \
}

/// EDCA parameter set values
// Best effort
#define MAC_ACI_BE                        0
// Background
#define MAC_ACI_BK                        1
// Video
#define MAC_ACI_VI                        2
// Voice
#define MAC_ACI_VO                        3

#define MAC_ACI_OFT                       5

#define MAC_ECWMIN                        4
#define MAC_ECWMAX                        10

#define MAC_ECW_BK_MIN                  MAC_ECWMIN
#define MAC_ECW_BE_MIN                  MAC_ECWMIN
//#define MAC_ECW_VI_MIN                  ((MAC_ECWMIN+1)/2 - 1)
//#define MAC_ECW_VO_MIN                  ((MAC_ECWMIN+1)/4 - 1)
#define MAC_ECW_VI_MIN                  ((MAC_ECWMIN) - 1)
#define MAC_ECW_VO_MIN                  ((MAC_ECWMIN)/2)

#define MAC_ECW_BK_MAX                  MAC_ECWMAX
#define MAC_ECW_BE_MAX                  MAC_ECWMAX
#define MAC_ECW_VI_MAX                  MAC_ECWMIN
//#define MAC_ECW_VO_MAX                  ((MAC_ECWMIN+1)/2 - 1)
#define MAC_ECW_VO_MAX                  ((MAC_ECWMIN) - 1)

#define MAC_AIFSN_BK                    7
#define MAC_AIFSN_BE                    3
#define MAC_AIFSN_VI                    2
#define MAC_AIFSN_VO                    2

#define MAC_AC_BE_TXOP                  0
#define MAC_AC_BK_TXOP                  0
#define MAC_AC_VI_TXOP_b                188    // 6016us / 32 us
#define MAC_AC_VI_TXOP_ag               94     // 3008 ms / 32 usec


#define MAC_AC_VO_TXOP_b                102    // 3264us / 32 us
#define MAC_AC_VO_TXOP_ag                47    // 1504us / 32 us


#define MAC_EDCA_DEFAULT_STA                                                            \
{                                                                                       \
 /*  TXOP (LSB.MSB), (MAC_ECW_XX_MAX<<4)|MAC_ECW_XX_MIN, (AIFSN)*/                      \
    (MAC_AC_BK_TXOP & 0xFF),(MAC_AC_BK_TXOP>>8), (MAC_ECW_BK_MAX<<4)|MAC_ECW_BK_MIN, (MAC_ACI_BK<<5)|MAC_AIFSN_BK,  \
    (MAC_AC_BE_TXOP & 0xFF),(MAC_AC_BE_TXOP>>8), (MAC_ECW_BE_MAX<<4)|MAC_ECW_BE_MIN, (MAC_ACI_BE<<5)|MAC_AIFSN_BE,  \
    (MAC_AC_VI_TXOP_ag & 0xFF),(MAC_AC_VI_TXOP_ag>>8), (MAC_ECW_VI_MAX<<4)|MAC_ECW_VI_MIN, (MAC_ACI_VI<<5)|MAC_AIFSN_VI, \
    (MAC_AC_VO_TXOP_ag & 0xFF),(MAC_AC_VO_TXOP_ag>>8), (MAC_ECW_VO_MAX<<4)|MAC_ECW_VO_MIN, (MAC_ACI_VO<<5)|MAC_AIFSN_VO  \
}

#define MAC_EDCA_DEFAULT_AP                                                             \
{                                                                                       \
 /*  TXOP (LSB.MSB), (MAC_ECW_XX_MAX<<4)|MAC_ECW_XX_MIN, (AIFSN)*/                      \
    (MAC_AC_BK_TXOP & 0xFF),(MAC_AC_BK_TXOP>>8), (10<<4)| 4, (MAC_ACI_BK<<5)|MAC_AIFSN_BK,  \
    (MAC_AC_BE_TXOP & 0xFF),(MAC_AC_BE_TXOP>>8), (6<<4)| 4, (MAC_ACI_BE<<5)|MAC_AIFSN_BE, \
    (MAC_AC_VI_TXOP_ag & 0xFF),(MAC_AC_VI_TXOP_ag>>8), (4<<4)| 3, (MAC_ACI_VI<<5)|((MAC_AIFSN_VI)-1), \
    (MAC_AC_VO_TXOP_ag & 0xFF),(MAC_AC_VO_TXOP_ag>>8), (3<<4)| 2, (MAC_ACI_VO<<5)|((MAC_AIFSN_VO)-1)  \
}

// WME Parameter Element
#define MAC_RAW_WME_PARAM_DEFAULT                                                        \
{                                                                                        \
    MAC_ELTID_OUI, 0x18,                                                               \
    0x00, 0x50, 0xF2, MAC_OUI_TYPE_WME,         /* additionnal WPA OUI                */ \
    MAC_OUI_SUBTYPE_WME_PARAMETER, 0x01,        /* OUI subtype and version            */ \
    0x00,                                       /* QoS field                          */ \
    0x00,                                       /* reserved                           */ \
 /*  TXOP (LSB.MSB), (MAC_ECW_XX_MAX<<4)|MAC_ECW_XX_MIN, (AIFSN)*/                       \
    (MAC_ACI_BE<<5)|MAC_AIFSN_BE, (MAC_ECW_BE_MAX<<4)|MAC_ECW_BE_MIN,  (MAC_AC_BE_TXOP & 0xFF),(MAC_AC_BE_TXOP>>8),  \
    (MAC_ACI_BK<<5)|MAC_AIFSN_BK, (MAC_ECW_BK_MAX<<4)|MAC_ECW_BK_MIN, (MAC_AC_BK_TXOP & 0xFF),(MAC_AC_BK_TXOP>>8),  \
    (MAC_ACI_VI<<5)|MAC_AIFSN_VI, (MAC_ECW_VI_MAX<<4)|MAC_ECW_VI_MIN, (MAC_AC_VI_TXOP_ag & 0xFF),(MAC_AC_VI_TXOP_ag>>8),\
    (MAC_ACI_VO<<5)|MAC_AIFSN_VO, (MAC_ECW_VO_MAX<<4)|MAC_ECW_VO_MIN, (MAC_AC_VO_TXOP_ag & 0xFF),(MAC_AC_VO_TXOP_ag>>8) \
}

#define MAC_WME_PARAM_AP_b              \
{                                       \
 /* (MAC_ACI_XX<<5)|MAC_AIFSN_XX, (MAC_ECW_XX_MIN<<4)|MAC_ECW_XX_MAX, (MAC_AC_XX_TXOP & 0xFF),(MAC_AC_XX_TXOP>>8)*/\
    (MAC_ACI_BE<<5)|MAC_AIFSN_BE, (MAC_ECW_BE_MIN<<4)|MAC_ECW_BE_MAX, (MAC_AC_BE_TXOP & 0xFF),(MAC_AC_BE_TXOP>>8),  \
    (MAC_ACI_BK<<5)|MAC_AIFSN_BK, (MAC_ECW_BK_MIN<<4)|MAC_ECW_BK_MAX, (MAC_AC_BK_TXOP & 0xFF),(MAC_AC_BK_TXOP>>8),  \
    (MAC_ACI_VI<<5)|MAC_AIFSN_VI, (MAC_ECW_VI_MIN<<4)|MAC_ECW_VI_MAX, (MAC_AC_VI_TXOP_b & 0xFF),(MAC_AC_VI_TXOP_b>>8), \
    (MAC_ACI_VO<<5)|MAC_AIFSN_VO, (MAC_ECW_VO_MIN<<4)|MAC_ECW_VO_MAX, (MAC_AC_VO_TXOP_b & 0xFF),(MAC_AC_VO_TXOP_b>>8)  \
}

#define MAC_WME_PARAM_AP_ag             \
{                                       \
 /* (MAC_ACI_XX<<5)|MAC_AIFSN_XX, (MAC_ECW_XX_MIN<<4)|MAC_ECW_XX_MAX, (MAC_AC_XX_TXOP & 0xFF),(MAC_AC_XX_TXOP>>8)*/\
    (MAC_ACI_BE<<5)|MAC_AIFSN_BE, (MAC_ECW_BE_MIN<<4)|MAC_ECW_BE_MAX, (MAC_AC_BE_TXOP & 0xFF),(MAC_AC_BE_TXOP>>8),  \
    (MAC_ACI_BK<<5)|MAC_AIFSN_BK, (MAC_ECW_BK_MIN<<4)|MAC_ECW_BK_MAX, (MAC_AC_BK_TXOP & 0xFF),(MAC_AC_BK_TXOP>>8),  \
    (MAC_ACI_VI<<5)|MAC_AIFSN_VI, (MAC_ECW_VI_MIN<<4)|MAC_ECW_VI_MAX, (MAC_AC_VI_TXOP_ag & 0xFF),(MAC_AC_VI_TXOP_ag>>8), \
    (MAC_ACI_VO<<5)|MAC_AIFSN_VO, (MAC_ECW_VO_MIN<<4)|MAC_ECW_VO_MAX, (MAC_AC_VO_TXOP_ag & 0xFF),(MAC_AC_VO_TXOP_ag>>8)  \
}

 // RSN Information element related fields
 #define MAC_RSNIE_WPA_OUI_LEN           4
 #define MAC_RSNIE_GROUP_KEY_OFT         7
 #define MAC_RSNIE_PAIRWISE_KEY_CNT_OFT  8
 #define MAC_RSNIE_PAIRWISE_KEY_OFT     13

 // Cipher suite selectors
 #define MAC_RSNIE_CIPHER_MASK    0x07
 #define MAC_RSNIE_CIPHER_WEP40   0x00
 #define MAC_RSNIE_CIPHER_TKIP    0x01
 #define MAC_RSNIE_CIPHER_CCMP    0x02
 #define MAC_RSNIE_CIPHER_WEP104  0x03
 #define MAC_RSNIE_CIPHER_WPI_SMS4 0x04
 #define MAC_RSNIE_CIPHER_AES_CMAC 0x05

 #define MAC_RSNIE_CIPHER_NOSEC   0xFE
 #define MAC_RSNIE_CIPHER_INVALID 0xFF

 // Authentication and key management suite selectors
 #define MAC_RSNIE_KEYMGT_8021X    1
 #define MAC_RSNIE_KEYMGT_PSK      2


// TKIP and CCMP OUI
#define MAC_RAW_ENC_WPA_TKIP                                                         \
{                                                                                    \
    MAC_ELTID_OUI, 0x16,                                                             \
    0x00, 0x50, 0xF2, MAC_OUI_TYPE_WPA,         /* additionnal WPA OUI                                                     */ \
    0x01, 0x00,                                 /* version                                                                 */ \
    0x00, 0x50, 0xF2, MAC_RSNIE_CIPHER_TKIP,    /* group key cipher    (00==none, 01=WEP40, 02==TKIP, 04==CCMP, 05==WEP104)*/ \
    0x01, 0x00,                                 /* pairwise key cipher number                                              */ \
    0x00, 0x50, 0xF2, MAC_RSNIE_CIPHER_TKIP,    /* pairwise key cipher (00==none, 01=WEP40, 02==TKIP, 04==CCMP, 05==WEP104)*/ \
    0x01, 0x00,                                 /* authentication key management number                                    */ \
    0x00, 0x50, 0xF2, MAC_RSNIE_KEYMGT_PSK      /* authentication key management (01==802.1X auth, 02==PSK)                */ \
}

#define MAC_RAW_ENC_WPA_CCMP                                                         \
{                                                                                   \
    MAC_ELTID_OUI, 0x16,                                                             \
    0x00, 0x50, 0xF2, MAC_OUI_TYPE_WPA,         /* additionnal WPA OUI                                                     */ \
    0x01, 0x00,                                 /* version                                                                 */ \
    0x00, 0x50, 0xF2, MAC_RSNIE_CIPHER_CCMP,    /* group key cipher    (00==none, 01=WEP40, 02==TKIP, 04==CCMP, 05==WEP104)*/ \
    0x01, 0x00,                                 /* pairwise key cipher number                                              */ \
    0x00, 0x50, 0xF2, MAC_RSNIE_CIPHER_CCMP,    /* pairwise key cipher (00==none, 01=WEP40, 02==TKIP, 04==CCMP, 05==WEP104)*/ \
    0x01, 0x00,                                 /* authentication key management number                                    */ \
    0x00, 0x50, 0xF2, MAC_RSNIE_KEYMGT_PSK      /* authentication key management (01==802.1X auth, 02==PSK)                */ \
}

#define MAC_RAW_ENC_WPA_MIXED                                                         \
{                                                                                    \
    MAC_ELTID_OUI, 0x1A,                                                              \
    0x00, 0x50, 0xF2, MAC_OUI_TYPE_WPA,         /* additionnal WPA OUI                                                     */ \
    0x01, 0x00,                                 /* version                                                                 */ \
    0x00, 0x50, 0xF2, MAC_RSNIE_CIPHER_TKIP,    /* group key cipher    (00==none, 01=WEP40, 02==TKIP, 04==CCMP, 05==WEP104)*/ \
    0x02, 0x00,                                 /* pairwise key cipher number                                              */ \
    0x00, 0x50, 0xF2, MAC_RSNIE_CIPHER_CCMP,    /* pairwise key cipher (00==none, 01=WEP40, 02==TKIP, 04==CCMP, 05==WEP104)*/ \
    0x00, 0x50, 0xF2, MAC_RSNIE_CIPHER_TKIP,    /* pairwise key cipher (00==none, 01=WEP40, 02==TKIP, 04==CCMP, 05==WEP104)*/ \
    0x01, 0x00,                                 /* authentication key management number                                    */ \
    0x00, 0x50, 0xF2, MAC_RSNIE_KEYMGT_PSK      /* authentication key management (01==802.1X auth, 02==PSK)                */ \
}

#define MAC_RAW_ENC_RSN_TKIP                                                         \
{{                                                                                       \
    MAC_ELTID_RSN_IEEE, 0x14,                                                            \
    0x01, 0x00,                                 /* version                                                                 */ \
    0x00, 0x0F, 0xAC, MAC_RSNIE_CIPHER_TKIP,    /* group key cipher    (00==none, 01=WEP40, 02==TKIP, 04==CCMP, 05==WEP104)*/ \
    0x01, 0x00,                                 /* pairwise key cipher number                                              */ \
    0x00, 0x0F, 0xAC, MAC_RSNIE_CIPHER_TKIP,    /* pairwise key cipher (00==none, 01=WEP40, 02==TKIP, 04==CCMP, 05==WEP104)*/ \
    0x01, 0x00,                                 /* authentication key management number                                    */ \
    0x00, 0x0F, 0xAC, MAC_RSNIE_KEYMGT_PSK,     /* authentication key management (01==802.1X auth, 02==PSK)                */ \
    0x00, 0x00                                  /* no pre-authentication capabilities   */                                    \
}}

#define MAC_RAW_ENC_RSN_CCMP                                                        \
{{                                                                                       \
    MAC_ELTID_RSN_IEEE, 0x14,                                                            \
    0x01, 0x00,                                 /* version                                                                 */ \
    0x00, 0x0F, 0xAC, MAC_RSNIE_CIPHER_CCMP,    /* group key cipher    (00==none, 01=WEP40, 02==TKIP, 04==CCMP, 05==WEP104)*/ \
    0x01, 0x00,                                 /* pairwise key cipher number                                              */ \
    0x00, 0x0F, 0xAC, MAC_RSNIE_CIPHER_CCMP,    /* pairwise key cipher (00==none, 01=WEP40, 02==TKIP, 04==CCMP, 05==WEP104)*/ \
    0x01, 0x00,                                 /* authentication key management number                                    */ \
    0x00, 0x0F, 0xAC, MAC_RSNIE_KEYMGT_PSK,     /* authentication key management (01==802.1X auth, 02==PSK)                */ \
    0x00, 0x00                                  /* no pre-authentication capabilities   */                                    \
}}


#define MAC_RAW_ENC_RSN_MIXED                                                            \
{{                                                                                       \
    MAC_ELTID_RSN_IEEE, 0x18,                                                            \
    0x01, 0x00,                                 /* version                                                                 */ \
    0x00, 0x0F, 0xAC, MAC_RSNIE_CIPHER_TKIP,    /* group key cipher    (00==none, 01=WEP40, 02==TKIP, 04==CCMP, 05==WEP104)*/ \
    0x02, 0x00,                                 /* pairwise key cipher number                                              */ \
    0x00, 0x0F, 0xAC, MAC_RSNIE_CIPHER_CCMP,    /* pairwise key cipher (00==none, 01=WEP40, 02==TKIP, 04==CCMP, 05==WEP104)*/ \
    0x00, 0x0F, 0xAC, MAC_RSNIE_CIPHER_TKIP,    /* pairwise key cipher (00==none, 01=WEP40, 02==TKIP, 04==CCMP, 05==WEP104)*/ \
    0x01, 0x00,                                 /* authentication key management number                                    */ \
    0x00, 0x0F, 0xAC, MAC_RSNIE_KEYMGT_PSK,     /* authentication key management (01==802.1X auth, 02==PSK)                */ \
    0x00, 0x00                                  /* no pre-authentication capabilities   */                                    \
}}


// Country IE data.
#define MAC_COUNTRY_2G4_FULL      {  3, {'x', 'x', ' '}, { 1, 14, 30}}
#define MAC_COUNTRY_2G4_USA       {  6, {'U', 'S', ' '}, { 1, 11, 30}} // X'10' FCC
#define MAC_COUNTRY_2G4_CANADA    {  6, {'C', 'A', ' '}, { 1, 11, 30}} // X'20' DOC/IC
#define MAC_COUNTRY_2G4_EUROPE    {  6, {'E', 'U', ' '}, { 1, 13, 30}} // X'30' ETSI
#define MAC_COUNTRY_2G4_SPAIN     {  6, {'S', 'P', ' '}, {10,  2, 30}} // X'31'
#define MAC_COUNTRY_2G4_FRANCE    {  6, {'F', 'R', ' '}, {10,  4, 20}} // X'32'
#define MAC_COUNTRY_2G4_JAPAN     {  6, {'J', 'P', ' '}, {14,  1, 30}} // X'40'
#define MAC_COUNTRY_2G4_CHINA     {  6, {'C', 'N', ' '}, { 1, 13, 30}} // X'50'

#define MAC_COUNTRY_5G_FULL       {  3, {'x', 'x', ' '}, {{ 36, 8, 23}}}
#define MAC_COUNTRY_5G_USA        { 18, {'U', 'S', ' '}, {{ 36, 4, 16}, /**/ {52, 4, 23}, /**/ {100, 11, 23}, /**/ {149, 4, 29}, /**/ {165, 1, 30}}}
#define MAC_COUNTRY_5G_FRANCE     { 12, {'F', 'R', ' '}, {{ 36, 4, 23}, /**/ {52, 4, 23}, /**/ {100, 11, 40}}}
#define MAC_COUNTRY_5G_JAPAN      { 12, {'J', 'P', ' '}, {{184, 4, 24}, /**/ { 8, 3, 24}, /**/ { 34,  4, 22}}}

/*
* AUTHENTICATION ALGORITHM
****************************************************************************************
*/
#define MAC_AUTH_ALGO_OPEN                0
#define MAC_AUTH_ALGO_SHARED              1
#define MAC_AUTH_ALGO_FT                  2

#define MAC_AUTH_FIRST_SEQ                1
#define MAC_AUTH_SECOND_SEQ               2
#define MAC_AUTH_THIRD_SEQ                3
#define MAC_AUTH_FOURTH_SEQ               4

// Defines for encryption status
#define MAC_ENC_NONE               0
#define MAC_ENC_WEP                0x01
#define MAC_ENC_WPA                0x02
#define MAC_ENC_TKIP               0x04
#define MAC_ENC_WPA2               0x10
#define MAC_ENC_CCMP               0x20

// MIC and FCS lengths
#define MAC_FCS_LEN     4
#define MAC_MIC_LEN     8

// Key Offsets
#define  MAC_ENC_KEY_OFFSET         0
#define  MAC_AUTHENTICATOR_KEY     16
#define  MAC_SUPPLICANT_KEY        24
#define  MAC_ENC_KEY_LEN           16
#define  MAC_MICHEAL_KEY_LEN        8

/// Maximum size of a beacon frame (default IE + TIM + remaining IE space)
#define MAC_BEACON_SIZE             (102 + MAC_TIM_SIZE + 128)
/// Maximum size of a probe request frame
#define MAC_PROBEREQ_SIZE           100
/// Maximum size of a probe response frame (default IE + remaining IE space)
#define MAC_PROBERSP_SIZE               (102 + 128)
/// Size of a NULL frame
#define MAC_NULL_FRAME_SIZE         MAC_SHORT_MAC_HDR_LEN
/// Size of a QoS-NULL frame
#define MAC_QOS_NULL_FRAME_SIZE     MAC_SHORT_QOS_MAC_HDR_LEN
/// Size of a PS poll
#define MAC_PSPOLL_FRAME_SIZE       MAC_LONG_CTRLFRAME_LEN

/// Structure of a short control frame MAC header
__PACKED struct mac_hdr_ctrl_short
{
    /// Frame control
    uint16_t fctl;
    /// Duration/ID
    uint16_t durid;
    /// Address 1
    struct mac_addr addr1;
} ;

/// Structure of a long control frame MAC header
__PACKED struct mac_hdr_ctrl
{
    /// Frame control
    uint16_t fctl;
    /// Duration/ID
    uint16_t durid;
    /// Address 1
    struct mac_addr addr1;
    /// Address 2
    struct mac_addr addr2;
};

/// Structure of a long control frame MAC header
__PACKED struct mac_hdr
{
    /// Frame control
    uint16_t fctl;
    /// Duration/ID
    uint16_t durid;
    /// Address 1
    struct mac_addr addr1;
    /// Address 2
    struct mac_addr addr2;
    /// Address 3
    struct mac_addr addr3;
    /// Sequence control
    uint16_t seq;
};

/// Structure of a long control frame MAC header
__PACKED struct mac_hdr_qos
{
    /// Frame control
    uint16_t fctl;
    /// Duration/ID
    uint16_t durid;
    /// Address 1
    struct mac_addr addr1;
    /// Address 2
    struct mac_addr addr2;
    /// Address 3
    struct mac_addr addr3;
    /// Sequence control
    uint16_t seq;
    /// QoS control
    uint16_t qos;
};

/// Structure of a long control frame MAC header
__PACKED struct mac_hdr_long
{
    /// Frame control
    uint16_t fctl;
    /// Duration/ID
    uint16_t durid;
    /// Address 1
    struct mac_addr addr1;
    /// Address 2
    struct mac_addr addr2;
    /// Address 3
    struct mac_addr addr3;
    /// Sequence control
    uint16_t seq;
    /// Address 4
    struct mac_addr addr4;
};

/// Structure of a long control frame MAC header
__PACKED struct mac_hdr_long_qos
{
    /// Frame control
    uint16_t fctl;
    /// Duration/ID
    uint16_t durid;
    /// Address 1
    struct mac_addr addr1;
    /// Address 2
    struct mac_addr addr2;
    /// Address 3
    struct mac_addr addr3;
    /// Sequence control
    uint16_t seq;
    /// Address 4
    struct mac_addr addr4;
    /// QoS control
    uint16_t qos;
};

/// Structure of Ethernet Header
__PACKED struct ethernet_hdr
{
    // Destination Address
    struct mac_addr da;
    // Source Address
    struct mac_addr sa;
    // Length / Type
    uint16_t len;
};

/// Structure of a Beacon or ProbeRsp frame
__PACKED struct bcn_frame
{
    /// MAC Header
    struct mac_hdr h;
    /// Timestamp
    uint64_t tsf;
    /// Beacon interval
    uint16_t bcnint;
    /// Capability information
    uint16_t capa;
    /// Rest of the payload
    uint8_t variable[];
};

/// Structure of a Probe Request frame
__PACKED struct preq_frame
{
    /// MAC Header
    struct mac_hdr h;
    /// Rest of the payload
    uint8_t payload[];
};

/// Structure of a Block Acknowledgment Request frame
struct bar_frame
{
    /// MAC Header
    struct mac_hdr_ctrl h;
    /// BAR control
    uint16_t bar_cntrl;
    /// BAR information
    uint16_t bar_information;
};

/// Structure of a Block Acknowledgment frame
__PACKED struct ba_frame
{
    /// MAC Header
    struct mac_hdr_ctrl h;
    /// BA control
    uint16_t ba_cntrl;
    /// BA Start Sequence Control
    uint16_t ssc;
    /// BA bitmap
    uint16_t bitmap[4];
};

/// Structure of A-MSDU Header
__PACKED struct amsdu_hdr
{
    // Destination Address
    struct mac_addr da;
    // Source Address
    struct mac_addr sa;
    // Length
    uint16_t len;
} ;

///Payload length of the BAR frame
#define BAR_PAYLOAD_LEN        (sizeof_b(struct bar_frame))
///Byte length of the BAR frame, including FCS
#define BAR_FRM_LEN_WITH_FCS   (BAR_PAYLOAD_LEN + MAC_FCS_LEN)

///BAR frame body BAR Control field
#define BAR_CNTL_ACK_TYPE_BIT    CO_BIT(0)
#define BAR_CNTL_MULTI_TID_BIT   CO_BIT(1)
#define BAR_CNTL_COMPRESSED_BIT  CO_BIT(2)

#define BAR_CNTL_TID_OFT         12
#define BAR_CNTL_TID_MSK         (0xF<<BAR_CNTL_TID_OFT)

/// Compressed BlockAck frame length
#define BA_FRM_LEN               (sizeof_b(struct ba_frame))





/// @} end of group
#endif // _MAC_FRAME_H_
