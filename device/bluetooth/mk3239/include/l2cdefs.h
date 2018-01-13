/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/****************************************************************************
**
**  Name:       l2cdefs.h
**
**   Function:   This file contains L2CAP protocol definitions
**
**
*****************************************************************************/

#ifndef L2CDEFS_H
#define L2CDEFS_H

/* L2CAP command codes
*/
#define L2CAP_CMD_REJECT                    0x01
#define L2CAP_CMD_CONN_REQ                  0x02
#define L2CAP_CMD_CONN_RSP                  0x03
#define L2CAP_CMD_CONFIG_REQ                0x04
#define L2CAP_CMD_CONFIG_RSP                0x05
#define L2CAP_CMD_DISC_REQ                  0x06
#define L2CAP_CMD_DISC_RSP                  0x07
#define L2CAP_CMD_ECHO_REQ                  0x08
#define L2CAP_CMD_ECHO_RSP                  0x09
#define L2CAP_CMD_INFO_REQ                  0x0A
#define L2CAP_CMD_INFO_RSP                  0x0B
#define L2CAP_CMD_AMP_CONN_REQ              0x0C
#define L2CAP_CMD_AMP_CONN_RSP              0x0D
#define L2CAP_CMD_AMP_MOVE_REQ              0x0E
#define L2CAP_CMD_AMP_MOVE_RSP              0x0F
#define L2CAP_CMD_AMP_MOVE_CFM              0x10
#define L2CAP_CMD_AMP_MOVE_CFM_RSP          0x11
#define L2CAP_CMD_BLE_UPDATE_REQ            0x12
#define L2CAP_CMD_BLE_UPDATE_RSP            0x13
#define L2CAP_CMD_BLE_CONN_REQ              0x14
#define L2CAP_CMD_BLE_CONN_RSP              0x15
#define L2CAP_CMD_BLE_CREDIT                0x16


/* Define some packet and header lengths
*/
#define L2CAP_PKT_OVERHEAD      4           /* Length and CID                       */
#define L2CAP_CMD_OVERHEAD      4           /* Cmd code, Id and length              */
#define L2CAP_CMD_REJECT_LEN    2           /* Reason (data is optional)            */
#define L2CAP_CONN_REQ_LEN      4           /* PSM and source CID                   */
#define L2CAP_CONN_RSP_LEN      8           /* Dest CID, source CID, reason, status */
#define L2CAP_CONFIG_REQ_LEN    4           /* Dest CID, flags (data is optional)   */
#define L2CAP_CONFIG_RSP_LEN    6           /* Dest CID, flags, result,data optional*/
#define L2CAP_DISC_REQ_LEN      4           /* Dest CID, source CID                 */
#define L2CAP_DISC_RSP_LEN      4           /* Dest CID, source CID                 */
#define L2CAP_ECHO_REQ_LEN      0           /* Data is optional                     */
#define L2CAP_ECHO_RSP_LEN      0           /* Data is optional                     */
#define L2CAP_INFO_REQ_LEN      2           /* Info type                            */
#define L2CAP_INFO_RSP_LEN      4           /* Info type, result (data is optional) */
#define L2CAP_BCST_OVERHEAD     2           /* Additional broadcast packet overhead */
#define L2CAP_UCD_OVERHEAD      2           /* Additional connectionless packet overhead */

#define L2CAP_AMP_CONN_REQ_LEN  5           /* PSM, CID, and remote controller ID   */
#define L2CAP_AMP_MOVE_REQ_LEN  3           /* CID and remote controller ID         */
#define L2CAP_AMP_MOVE_RSP_LEN  4           /* CID and result                       */
#define L2CAP_AMP_MOVE_CFM_LEN  4           /* CID and result                       */
#define L2CAP_AMP_MOVE_CFM_RSP_LEN  2       /* CID                                  */

#define L2CAP_CMD_BLE_UPD_REQ_LEN   8       /* Min and max interval, latency, tout  */
#define L2CAP_CMD_BLE_UPD_RSP_LEN   2       /* Result                               */

#define L2CAP_BLE_CONN_REQ_LEN      10      /* LE_PSM, src CID, MTU, MPS, init credits  */
#define L2CAP_BLE_CONN_RSP_LEN      10      /* Dest CID, MTU, MPS, init credits, result */
#define L2CAP_BLE_CONN_CRED_LEN     4       /* Local CID, connection credits to add                */


/* Define the packet boundary flags
*/
#if (L2CAP_NON_FLUSHABLE_PB_INCLUDED == TRUE)
#define L2CAP_PKT_START_FLUSHABLE       2
#define L2CAP_PKT_START_NON_FLUSHABLE   0
#endif
#define L2CAP_COMPLETE_AMP_PKT          3       /* complete L2CAP packet on AMP HCI */
#define L2CAP_PKT_START                 2
#define L2CAP_PKT_CONTINUE              1
#define L2CAP_MASK_FLAG            0x0FFF
#define L2CAP_PKT_TYPE_SHIFT            12
#define L2CAP_PKT_TYPE_MASK             3

#define L2CAP_BLE_CONN_MIN_MPS_SIZE           23
#define L2CAP_BLE_CONN_MAX_MPS_SIZE           65533
#define L2CAP_BLE_CONN_MIN_MTU_SIZE           23
#define L2CAP_BLE_CONN_SDU_LEN_FIELD_SIZE     2


/* Define the L2CAP connection result codes
*/
#define L2CAP_CONN_OK                0
#define L2CAP_CONN_PENDING           1
#define L2CAP_CONN_NO_PSM            2
#define L2CAP_CONN_SECURITY_BLOCK    3
#define L2CAP_CONN_NO_RESOURCES      4
#define L2CAP_CONN_BAD_CTLR_ID       5              /* AMP related */
#define L2CAP_BLE_CONN_BAD_AUTHENT   5              /* BLE L2CAP connections related */
#define L2CAP_BLE_CONN_BAD_AUTHORIZ  6
#define L2CAP_BLE_CONN_BAD_KEY_SIZE  7
#define L2CAP_BLE_CONN_BAD_ENCRYPT   8
/* Add a couple of our own for internal use */
#define L2CAP_CONN_TIMEOUT           0xEEEE
#define L2CAP_BLE_CMD_REJ            253
#define L2CAP_CONN_AMP_FAILED        254
#define L2CAP_CONN_NO_LINK           255
#define L2CAP_CONN_CANCEL            256        /* L2CAP connection cancelled */


/* Define L2CAP Move Channel Response result codes
*/
#define L2CAP_MOVE_OK                   0
#define L2CAP_MOVE_PENDING              1
#define L2CAP_MOVE_CTRL_ID_NOT_SUPPORT  2
#define L2CAP_MOVE_SAME_CTRLR_ID        3
#define L2CAP_MOVE_CONFIG_NOT_SUPPORTED 4
#define L2CAP_MOVE_CHAN_COLLISION       5
#define L2CAP_MOVE_NOT_ALLOWED          6


/* Define L2CAP Move Channel Confirmation result codes
*/
#define L2CAP_MOVE_CFM_OK               0
#define L2CAP_MOVE_CFM_REFUSED          1


/* Define the L2CAP command reject reason codes
*/
#define L2CAP_CMD_REJ_NOT_UNDERSTOOD    0
#define L2CAP_CMD_REJ_MTU_EXCEEDED      1
#define L2CAP_CMD_REJ_INVALID_CID       2


/* L2CAP Predefined CIDs
*/
#define L2CAP_SIGNALLING_CID            1
#define L2CAP_CONNECTIONLESS_CID        2
#define L2CAP_AMP_CID                   3
#define L2CAP_ATT_CID                   4
#define L2CAP_BLE_SIGNALLING_CID        5
#define L2CAP_SMP_CID                   6
#define L2CAP_SMP_BR_CID                7
#define L2CAP_AMP_TEST_CID              0x003F
#define L2CAP_BASE_APPL_CID             0x0040
#define L2CAP_BLE_CONN_MAX_CID          0x007F

/* Fixed Channels mask bits
*/
#define L2CAP_FIXED_CHNL_SIG_BIT        (1 << L2CAP_SIGNALLING_CID)     /* Signal Channel Supported (Mandatory) */
#define L2CAP_FIXED_CHNL_CNCTLESS_BIT   (1 << L2CAP_CONNECTIONLESS_CID) /* Connectionless Reception             */
#define L2CAP_FIXED_CHNL_AMP_BIT        (1 << L2CAP_AMP_CID)            /* AMP Manager Supported                */
#define L2CAP_FIXED_CHNL_ATT_BIT        (1 << L2CAP_ATT_CID)            /* Attribute protocol Supported         */
#define L2CAP_FIXED_CHNL_BLE_SIG_BIT    (1 << L2CAP_BLE_SIGNALLING_CID) /* BLE Signalling Supported             */
#define L2CAP_FIXED_CHNL_SMP_BIT        (1 << L2CAP_SMP_CID)            /* BLE Security Manager Supported       */
#define L2CAP_FIXED_CHNL_SMP_BR_BIT     (1 << L2CAP_SMP_BR_CID)         /* Security Manager over BR Supported   */



/* Define the L2CAP configuration result codes
*/
#define L2CAP_CFG_OK                    0
#define L2CAP_CFG_UNACCEPTABLE_PARAMS   1
#define L2CAP_CFG_FAILED_NO_REASON      2
#define L2CAP_CFG_UNKNOWN_OPTIONS       3
#define L2CAP_CFG_PENDING               4
#define L2CAP_CFG_FLOW_SPEC_REJECTED    5


/* Define the L2CAP configuration option types
*/
#define L2CAP_CFG_TYPE_MTU              0x01
#define L2CAP_CFG_TYPE_FLUSH_TOUT       0x02
#define L2CAP_CFG_TYPE_QOS              0x03
#define L2CAP_CFG_TYPE_FCR              0x04
#define L2CAP_CFG_TYPE_FCS              0x05
#define L2CAP_CFG_TYPE_EXT_FLOW         0x06
#define L2CAP_CFG_TYPE_EXT_WIN_SIZE     0x07

#define L2CAP_CFG_MTU_OPTION_LEN        2           /* MTU option length    */
#define L2CAP_CFG_FLUSH_OPTION_LEN      2           /* Flush option len     */
#define L2CAP_CFG_QOS_OPTION_LEN        22          /* QOS option length    */
#define L2CAP_CFG_FCR_OPTION_LEN        9           /* FCR option length    */
#define L2CAP_CFG_FCS_OPTION_LEN        1           /* FCR option length    */
#define L2CAP_CFG_EXT_FLOW_OPTION_LEN   16          /* Extended Flow Spec   */
#define L2CAP_CFG_EXT_WIN_SIZE_LEN      2           /* Ext window size length */
#define L2CAP_CFG_OPTION_OVERHEAD       2           /* Type and length      */

/* Configuration Cmd/Rsp Flags mask
*/
#define L2CAP_CFG_FLAGS_MASK_CONT       0x0001      /* Flags mask: Continuation */

/* FCS Check Option values
*/
#define L2CAP_CFG_FCS_BYPASS            0       /* Bypass the FCS in streaming or ERTM modes */
#define L2CAP_CFG_FCS_USE               1       /* Use the FCS in streaming or ERTM modes [default] */

/* Default values for configuration
*/
#define L2CAP_NO_AUTOMATIC_FLUSH        0xFFFF
#define L2CAP_NO_RETRANSMISSION         0x0001

#define L2CAP_DEFAULT_MTU               (672)
#define L2CAP_DEFAULT_FLUSH_TO          L2CAP_NO_AUTOMATIC_FLUSH
#define L2CAP_DEFAULT_SERV_TYPE         1
#define L2CAP_DEFAULT_TOKEN_RATE        0
#define L2CAP_DEFAULT_BUCKET_SIZE       0
#define L2CAP_DEFAULT_PEAK_BANDWIDTH    0
#define L2CAP_DEFAULT_LATENCY           0xFFFFFFFF
#define L2CAP_DEFAULT_DELAY             0xFFFFFFFF
#define L2CAP_DEFAULT_FCS               L2CAP_CFG_FCS_USE


/* Define the L2CAP disconnect result codes
*/
#define L2CAP_DISC_OK                   0
#define L2CAP_DISC_TIMEOUT              0xEEEE

/* Define the L2CAP info resp result codes
*/
#define L2CAP_INFO_RESP_RESULT_SUCCESS          0
#define L2CAP_INFO_RESP_RESULT_NOT_SUPPORTED    1

/* Define the info-type fields of information request & response
*/
#define L2CAP_CONNLESS_MTU_INFO_TYPE      0x0001
#define L2CAP_EXTENDED_FEATURES_INFO_TYPE 0x0002    /* Used in Information Req/Response */
#define L2CAP_FIXED_CHANNELS_INFO_TYPE    0x0003    /* Used in AMP                      */

#define L2CAP_CONNLESS_MTU_INFO_SIZE           2    /* Connectionless MTU size          */
#define L2CAP_EXTENDED_FEATURES_ARRAY_SIZE     4    /* Extended features array size     */
#define L2CAP_FIXED_CHNL_ARRAY_SIZE            8    /* Fixed channel array size         */

/* Extended features mask bits
*/
#define L2CAP_EXTFEA_RTRANS         0x00000001    /* Retransmission Mode (Not Supported)    */
#define L2CAP_EXTFEA_FC             0x00000002    /* Flow Control Mode   (Not Supported)    */
#define L2CAP_EXTFEA_QOS            0x00000004
#define L2CAP_EXTFEA_ENH_RETRANS    0x00000008    /* Enhanced retransmission mode           */
#define L2CAP_EXTFEA_STREAM_MODE    0x00000010    /* Streaming Mode                         */
#define L2CAP_EXTFEA_NO_CRC         0x00000020    /* Optional FCS (if set No FCS desired)   */
#define L2CAP_EXTFEA_EXT_FLOW_SPEC  0x00000040    /* Extended flow spec                     */
#define L2CAP_EXTFEA_FIXED_CHNLS    0x00000080    /* Fixed channels                         */
#define L2CAP_EXTFEA_EXT_WINDOW     0x00000100    /* Extended Window Size                   */
#define L2CAP_EXTFEA_UCD_RECEPTION  0x00000200    /* Unicast Connectionless Data Reception  */

/* Mask for locally supported features used in Information Response (default to none) */
#ifndef L2CAP_EXTFEA_SUPPORTED_MASK
#define L2CAP_EXTFEA_SUPPORTED_MASK 0
#endif

/* Mask for LE supported features used in Information Response (default to none) */
#ifndef L2CAP_BLE_EXTFEA_MASK
#define L2CAP_BLE_EXTFEA_MASK 0
#endif

/* Define a value that tells L2CAP to use the default HCI ACL buffer pool */
#define L2CAP_DEFAULT_ERM_POOL_ID       0xFF
/* Define a value that tells L2CAP to use the default MPS */
#define L2CAP_DEFAULT_ERM_MPS           0x0000

#define L2CAP_FCR_OVERHEAD         2   /* Control word                 */
#define L2CAP_FCS_LEN              2   /* FCS takes 2 bytes */
#define L2CAP_SDU_LEN_OVERHEAD     2   /* SDU length field is 2 bytes */
#define L2CAP_SDU_LEN_OFFSET       2   /* SDU length offset is 2 bytes */
#define L2CAP_EXT_CONTROL_OVERHEAD 4   /* Extended Control Field       */
#define L2CAP_MAX_HEADER_FCS       (L2CAP_PKT_OVERHEAD + L2CAP_EXT_CONTROL_OVERHEAD + L2CAP_SDU_LEN_OVERHEAD + L2CAP_FCS_LEN)
/* length(2), channel(2), control(4), SDU length(2) FCS(2) */
/* Part of L2CAP_MIN_OFFSET that is not part of L2CAP
*/
#define L2CAP_OFFSET_WO_L2HDR  (L2CAP_MIN_OFFSET-(L2CAP_PKT_OVERHEAD+L2CAP_FCR_OVERHEAD))

/* SAR bits in the control word
*/
#define L2CAP_FCR_UNSEG_SDU    0x0000   /* Control word to begin with for unsegmented PDU*/
#define L2CAP_FCR_START_SDU    0x4000   /* ...for Starting PDU of a semented SDU */
#define L2CAP_FCR_END_SDU      0x8000   /* ...for ending PDU of a segmented SDU */
#define L2CAP_FCR_CONT_SDU     0xc000   /* ...for continuation PDU of a segmented SDU */

/* Supervisory frame types
*/
#define L2CAP_FCR_SUP_RR        0x0000   /* Supervisory frame - RR                          */
#define L2CAP_FCR_SUP_REJ       0x0001   /* Supervisory frame - REJ                         */
#define L2CAP_FCR_SUP_RNR       0x0002   /* Supervisory frame - RNR                         */
#define L2CAP_FCR_SUP_SREJ      0x0003   /* Supervisory frame - SREJ                        */

#define L2CAP_FCR_SAR_BITS           0xC000    /* Mask to get the SAR bits from control word */
#define L2CAP_FCR_SAR_BITS_SHIFT     14        /* Bits to shift right to get the SAR bits from ctrl-word */

#define L2CAP_FCR_S_FRAME_BIT         0x0001   /* Mask to check if a PDU is S-frame */
#define L2CAP_FCR_REQ_SEQ_BITS        0x3F00   /* Mask to get the req-seq from control word */
#define L2CAP_FCR_REQ_SEQ_BITS_SHIFT  8        /* Bits to shift right to get the req-seq from ctrl-word */
#define L2CAP_FCR_TX_SEQ_BITS         0x007E   /* Mask on get the tx-seq from control word */
#define L2CAP_FCR_TX_SEQ_BITS_SHIFT   1        /* Bits to shift right to get the tx-seq from ctrl-word */

#define L2CAP_FCR_F_BIT               0x0080   /* F-bit in the control word (Sup and I frames)  */
#define L2CAP_FCR_P_BIT               0x0010   /* P-bit in the control word (Sup frames only)   */

#define L2CAP_FCR_F_BIT_SHIFT         7
#define L2CAP_FCR_P_BIT_SHIFT         4

#define L2CAP_FCR_SEG_BITS            0xC000   /* Mask to get the segmentation bits from ctrl-word */
#define L2CAP_FCR_SUP_SHIFT           2        /* Bits to shift right to get the S-bits from ctrl-word */
#define L2CAP_FCR_SUP_BITS            0x000C   /* Mask to get the supervisory bits from ctrl-word */

#define L2CAP_FCR_INIT_CRC            0   /* Initial state of the CRC register */
#define L2CAP_FCR_SEQ_MODULO          0x3F     /* Mask for sequence numbers (range 0 - 63) */

/*************************************************************************************************
** The following definitions are only used for internal testing of ERTM at the application level
*************************************************************************************************/
/* L2CA_SetupErtmTest() corruption test types */
#define L2CAP_FCR_TTYPE_CORR_IFRAME     0       /* Corrupt one or more I-frames, based on count   */
#define L2CAP_FCR_TTYPE_CORR_SFRAME     1       /* Corrupt an S-frame, (acknowledgement) */
#define L2CAP_FCR_TTYPE_STOP_TEST       2       /* Used when turning off a test */
#define L2CAP_FCR_TTYPE_GET_CID         3       /* Returns RFCOMM cid when '0' is passed in cid argument */

/* L2CA_SetupErtmTest() Freq */
#define L2CAP_FCR_FREQ_NORMAL           0       /* A single test is run */
#define L2CAP_FCR_FREQ_RANDOM           1       /* Randomly loses or corrupts a packet */

#endif
