#ifndef CO_BT_DEFINES_H_
#define CO_BT_DEFINES_H_


/**
 ****************************************************************************************
 * @addtogroup CO_BT_DEFINES Common Bluetooth defines
 * @ingroup CO_BT
 * @brief Common Bluetooth definitions and structures.
 *
 * @{
 ****************************************************************************************
 */


/*
 * DEFINES
 ****************************************************************************************
 */

/**
 * BD Address format (values in bytes)
 * |      3B        |  1B |    2B    |
 * |      LAP       | UAP |    NAP   |
 */
#define BD_ADDR_LEN         6
#define BD_ADDR_LAP_POS     0
#define BD_ADDR_LAP_LEN     3
#define BD_ADDR_UAP_POS     BD_ADDR_LAP_LEN
#define BD_ADDR_UAP_LEN     1
#define BD_ADDR_NAP_POS     BD_ADDR_UAP_LEN
#define BD_ADDR_NAP_LEN     2

///Length of fields in Bluetooth messages, in number of bytes
#define EVT_MASK_LEN        8
#define DEV_CLASS_LEN       3
#define ACO_LEN             12
#define SRES_LEN            0x04
#define ACCESS_ADDR_LEN     0x04
#define LE_PASSKEY_LEN      0x04
#define BD_NAME_SIZE        0xF8 // Was 0x20 for BLE HL
#define ADV_DATA_LEN        0x1F
#define EXT_ADV_DATA_MAX_LEN    229 // HCI:7.7.65.13
#define PER_ADV_DATA_MAX_LEN    248 // HCI:7.7.65.16
#define BLE_DATA_LEN        0x1B
#define SCAN_RSP_DATA_LEN   0x1F
#define CONNECT_REQ_DATA_LEN    0x16
#define LE_CHNL_MAP_LEN     0x05
#define CHNL_MAP_LEN        0x0A
#define KEY_LEN             0x10
#define PIN_CODE_MIN_LEN    0x01
#define PIN_CODE_MAX_LEN    0x10
#define PRIV_KEY_192_LEN    24
#define PUB_KEY_192_LEN     48
#define PRIV_KEY_256_LEN    32
#define PUB_KEY_256_LEN     64
#define CFM_LEN             0x10
#define ENC_DATA_LEN        0x10
#define RAND_VAL_LEN        0x10
#define RAND_NB_LEN         0x08
#define LE_FEATS_LEN        0x08
#define SUPP_CMDS_LEN       0x40
#define FEATS_LEN           0x08
#define NAME_VECT_SIZE      14
#define LMP_FEATS_LEN       0x08
#define LE_STATES_LEN       0x08
#define WHITE_LIST_LEN      0x0A
#define LE_FREQ_LEN         0x28
#define LE_DATA_FREQ_LEN    0x25
#define CRC_INIT_LEN        0x03
#define SESS_KEY_DIV_LEN    0x08
#define INIT_VECT_LEN       0x04
#define MIC_LEN             0x04
#define IV_LEN              0x08
#define SK_DIV_LEN          0x10

// Session Key Diversifier Master or slave
#define SKD_M_OFFSET        0x00
#define SKD_S_OFFSET        0x08
// Initialization Vector Master or slave
#define IV_M_OFFSET         0x00
#define IV_S_OFFSET         0x04


// BT 4.2 - Secure Connections
#define PUBLIC_KEY_P256_LEN 0x20
#define DHKEY_CHECK_LEN     0x10

#define DH_KEY_LEN          0x20

/// Maximum maskable event code
#define EVT_MASK_CODE_MAX   EVT_MASK_LEN * 8

/// Advertising and Data Channel Indices (chapter 6.B.1.4.1)
#define DATA_CHANNEL_MIN    0
#define DATA_CHANNEL_MAX    36
#define DATA_CHANNEL_NB     37
#define ADV_CHANNEL_37      37
#define ADV_CHANNEL_38      38
#define ADV_CHANNEL_39      39

/// Minimum number of used channel in the map (chapter 6.B.4.5.8.1)
#define DATA_CHANNEL_USED_NB_MIN    2

/// Advertising interval (in 625us slot) (chapter 2.E.7.8.5)
#define ADV_INTERVAL_MIN    0x0020 //(20 ms)
#define ADV_INTERVAL_MAX    0x4000 //(10.24 sec)
#define ADV_INTERVAL_DFT    0x0800 //(1.28 sec)

/// Scanning interval (in 625us slot) (chapter 2.E.7.8.10)
#define SCAN_INTERVAL_MIN   0x0004 //(2.5 ms)
#define SCAN_INTERVAL_MAX   0x4000 //(10.24 sec)
#define SCAN_INTERVAL_DFT   0x0010 //(10 ms)

/// Scanning window (in 625us slot) (chapter 2.E.7.8.10)
#define SCAN_WINDOW_MIN     0x0004 //(2.5 ms)
#define SCAN_WINDOW_MAX     0x4000 //(10.24 sec)
#define SCAN_WINDOW_DFT     0x0010 //(10 ms)

/// Connection interval (N*1.250ms) (chapter 2.E.7.8.12)
#define CON_INTERVAL_MIN    0x0006  //(7.5 msec)
#define CON_INTERVAL_MAX    0x0C80  //(4 sec)
/// Connection latency (N*cnx evt) (chapter 2.E.7.8.12)
#define CON_LATENCY_MIN     0x0000
#define CON_LATENCY_MAX     0x01F3  // (499)
/// Supervision TO (N*10ms) (chapter 2.E.7.8.12)
#define CON_SUP_TO_MIN      0x000A  //(100 msec)
#define CON_SUP_TO_MAX      0x0C80  //(32 sec)

/// Format of the Advertising packets
#define ADV_ADDR_OFFSET     0
#define ADV_ADDR_LEN        BD_ADDR_LEN
#define ADV_DATA_OFFSET    (ADV_ADDR_OFFSET + ADV_ADDR_LEN)

/// BLE supported features
//byte 0
#define BLE_ENC_FEAT                     0x01
#define BLE_CON_PARAM_REQ_PROC_FEAT      0x02
#define BLE_REJ_IND_EXT_FEAT             0x04
#define BLE_SLAVE_INIT_EXCHG_FEAT        0x08
#define BLE_PING_FEAT                    0x10
#define BLE_LENGTH_EXT_FEAT              0x20
#define BLE_LL_PRIVACY_FEAT              0x40
#define BLE_EXT_SCAN_POLICY_FEAT         0x80

//byte 1
#define BLE_2M_PHY_FEAT                  0x01
#define BLE_STABLE_MOD_IDX_TX_FEAT       0x02
#define BLE_STABLE_MOD_IDX_RX_FEAT       0x04
#define BLE_CODED_PHY_FEAT               0x08
#define BLE_EXT_ADV_FEAT                 0x10
#define BLE_PER_ADV_FEAT                 0x20
#define BLE_CHAN_SEL_ALGO_2_FEAT         0x40
#define BLEPWR_CLASS_1_FEAT              0x80

//byte 2
#define BLE_MIN_NUM_USED_CHAN_PROC          0x01

// List of supported BLE Features
enum ble_feature
{
    BLE_FEAT_ENC                       = (0),
    BLE_FEAT_CON_PARAM_REQ_PROC        = (1),
    BLE_FEAT_REJ_IND_EXT               = (2),
    BLE_FEAT_SLAVE_INIT_EXCHG          = (3),
    BLE_FEAT_PING                      = (4),
    BLE_FEAT_LENGTH_EXT                = (5),
    BLE_FEAT_LL_PRIVACY                = (6),
    BLE_FEAT_EXT_SCAN_POLICY           = (7),
    BLE_FEAT_2MBPS                     = (8),
    BLE_FEAT_STABLE_MOD                = (9),
};

/// BLE supported states
//byte 0
#define BLE_NON_CON_ADV_STATE                       0x01
#define BLE_DISC_ADV_STATE                          0x02
#define BLE_CON_ADV_STATE                           0x04
#define BLE_HDC_DIRECT_ADV_STATE                    0x08
#define BLE_PASS_SCAN_STATE                         0x10
#define BLE_ACTIV_SCAN_STATE                        0x20
#define BLE_INIT_MASTER_STATE                       0x40
#define BLE_CON_SLAVE_STATE                         0x80

//byte 1
#define BLE_NON_CON_ADV_PASS_SCAN_STATE             0x01
#define BLE_DISC_ADV_PASS_SCAN_STATE                0x02
#define BLE_CON_ADV_PASS_SCAN_STATE                 0x04
#define BLE_HDC_DIRECT_ADV_PASS_SCAN_STATE          0x08
#define BLE_NON_CON_ADV_ACTIV_SCAN_STATE            0x10
#define BLE_DISC_ADV_ACTIV_SCAN_STATE               0x20
#define BLE_CON_ADV_ACTIV_SCAN_STATE                0x40
#define BLE_HDC_DIRECT_ADV_ACTIV_SCAN_STATE         0x80

//byte 2
#define BLE_NON_CON_ADV_INIT_STATE                  0x01
#define BLE_DISC_ADV_INIT_STATE                     0x02
#define BLE_NON_CON_ADV_MASTER_STATE                0x04
#define BLE_DISC_ADV_MASTER_STATE                   0x08
#define BLE_NON_CON_ADV_SLAVE_STATE                 0x10
#define BLE_DISC_ADV_SLAVE_STATE                    0x20
#define BLE_PASS_SCAN_INIT_STATE                    0x40
#define BLE_ACTIV_SCAN_INIT_STATE                   0x80

//byte 3
#define BLE_PASS_SCAN_MASTER_STATE                  0x01
#define BLE_ACTIV_SCAN_MASTER_STATE                 0x02
#define BLE_PASS_SCAN_SLAVE_STATE                   0x04
#define BLE_ACTIV_SCAN_SLAVE_STATE                  0x08
#define BLE_INIT_MASTER_MASTER_STATE                0x10
#define BLE_LDC_DIRECT_ADV_STATE                    0x20
#define BLE_LDC_DIRECT_ADV_PASS_SCAN_STATE          0x40
#define BLE_LDC_DIRECT_ADV_ACTIV_SCAN_STATE         0x80

//byte 4
#define BLE_CON_ADV_INIT_MASTER_SLAVE_STATE         0x01
#define BLE_HDC_DIRECT_ADV_INIT_MASTER_SLAVE_STATE  0x02
#define BLE_LDC_DIRECT_ADV_INIT_MASTER_SLAVE_STATE  0x04
#define BLE_CON_ADV_MASTER_SLAVE_STATE              0x08
#define BLE_HDC_DIRECT_ADV_MASTER_SLAVE_STATE       0x10
#define BLE_LDC_DIRECT_ADV_MASTER_SLAVE_STATE       0x20
#define BLE_CON_ADV_SLAVE_SLAVE_STATE               0x40
#define BLE_HDC_DIRECT_ADV_SLAVE_SLAVE_STATE        0x80

//byte 5
#define BLE_LDC_DIRECT_ADV_SLAVE_SLAVE_STATE        0x01
#define BLE_INIT_MASTER_SLAVE_STATE                 0x02

/// BLE supported commands
//byte0
#define BLE_DISC_CMD                0x20
//byte2
#define BLE_RD_REM_VERS_CMD         0x80
//byte5
#define BLE_SET_EVT_MSK_CMD         0x40
#define BLE_RESET_CMD               0x80
//byte10
#define BLE_RD_TX_PWR_CMD            0x04
#define BLE_SET_CTRL_TO_HL_FCTRL_CMD 0x20
#define BLE_HL_BUF_SIZE_CMD          0x40
#define BLE_HL_NB_CMP_PKT_CMD        0x80
//byte14
#define BLE_RD_LOC_VERS_CMD         0x08
#define BLE_RD_LOC_SUP_FEAT_CMD     0x20
#define BLE_RD_BUF_SIZE_CMD         0x80
//byte15
#define BLE_RD_BD_ADDR_CMD          0x02
#define BLE_RD_RSSI_CMD             0x20
//byte22
#define BLE_SET_EVT_MSK_PG2_CMD     0x04
//byte25
#define BLE_LE_SET_EVT_MSK_CMD      0x01
#define BLE_LE_RD_BUF_SIZE_CMD      0x02
#define BLE_LE_RD_LOC_SUP_FEAT_CMD  0x04
#define BLE_LE_SET_RAND_ADDR_CMD    0x10
#define BLE_LE_SET_ADV_PARAM_CMD    0x20
#define BLE_LE_RD_ADV_TX_PWR_CMD    0x40
#define BLE_LE_SET_ADV_DATA_CMD     0x80
//byte26
#define BLE_LE_SET_SC_RSP_DATA_CMD  0x01
#define BLE_LE_SET_ADV_EN_CMD       0x02
#define BLE_LE_SET_SC_PARAM_CMD     0x04
#define BLE_LE_SET_SC_EN_CMD        0x08
#define BLE_LE_CREAT_CNX_CMD        0x10
#define BLE_LE_CREAT_CNX_CNL_CMD    0x20
#define BLE_LE_RD_WL_SIZE_CMD       0x40
#define BLE_LE_CLEAR_WL_CMD         0x80
//byte27
#define BLE_LE_ADD_DEV_WL_CMD       0x01
#define BLE_LE_REM_DEV_WL_CMD       0x02
#define BLE_LE_CNX_UPDATE_CMD       0x04
#define BLE_LE_SET_HL_CH_CLASS_CMD  0x08
#define BLE_LE_RD_CH_MAP_CMD        0x10
#define BLE_LE_RD_REM_FEAT_CMD      0x20
#define BLE_LE_ENCRYPT_CMD          0x40
#define BLE_LE_RAND_CMD             0x80
//byte28
#define BLE_LE_START_ENC_CMD        0x01
#define BLE_LE_LTK_REQ_RPLY_CMD     0x02
#define BLE_LE_LTK_REQ_NEG_RPLY_CMD 0x04
#define BLE_LE_RD_SUPP_STATES_CMD   0x08
#define BLE_LE_RX_TEST_CMD          0x10
#define BLE_LE_TX_TEST_CMD          0x20
#define BLE_LE_STOP_TEST_CMD        0x40

//byte32
#define BLE_RD_AUTH_PAYL_TO_CMD     0x10
#define BLE_WR_AUTH_PAYL_TO_CMD     0x20


//byte33
#define BLE_LE_REM_CON_PARA_REQ_RPLY_CMD        0x10
#define BLE_LE_REM_CON_PARA_REQ_NEG_RPLY_CMD    0x20
#define BLE_LE_SET_DATA_LEN_CMD                 0x40
#define BLE_LE_RD_SUGGTED_DFT_DATA_LEN_CMD      0x80

//byte34
#define BLE_LE_WR_SUGGTED_DFT_DATA_LEN_CMD      0x01
#define BLE_LE_RD_LOC_P256_PUB_KEY_CMD          0x02
#define BLE_LE_GEN_DH_KEY_CMD                   0x04
#define BLE_LE_ADD_DEV_TO_RESOLV_LIST_CMD       0x08
#define BLE_LE_REM_DEV_FROM_RESOLV_LIST_CMD     0x10
#define BLE_LE_CLEAR_RESOLV_LIST_CMD            0x20
#define BLE_LE_RD_RESOLV_LIST_SIZE_CMD          0x40
#define BLE_LE_RD_PEER_RESOLV_ADDR_CMD          0x80

//byte35
#define BLE_LE_RD_LOCAL_RESOLV_ADDR_CMD         0x01
#define BLE_LE_SET_ADDR_RESOL_CMD               0x02
#define BLE_LE_SET_RESOLV_PRIV_ADDR_TO_CMD      0x04
#define BLE_LE_RD_MAX_DATA_LEN_CMD              0x08

// Inquiry Length HCI:7.1.1
#define INQ_LEN_MIN      0x01
#define INQ_LEN_MAX      0x30

// Inquiry Length HCI:7.1.3
#define INQ_MIN_PER_LEN_MIN    0x0002
#define INQ_MIN_PER_LEN_MAX    0xFFFE
#define INQ_MAX_PER_LEN_MIN    0x0003
#define INQ_MAX_PER_LEN_MAX    0xFFFF

// IAC support
#define NB_IAC_MIN     0x01
#define NB_IAC_MAX     0x40


/// Logical Transport Adresses  BB:4.2
#define LT_ADDR_BCST            0x00
#define LT_ADDR_MIN             0x01
#define LT_ADDR_MAX             0x07

/// Link type             HCI:7.7.3
#define SCO_TYPE                0
#define ACL_TYPE                1
#define ESCO_TYPE               2
#define UNKNOWN_TYPE            3       // Used in LM
#define LE_TYPE                 4


/// Allow Role Switch     HCI:4.6.8
#define MASTER_ROLE             0
#define SLAVE_ROLE              1
#define UNKNOWN_ROLE            0xFF   //Used in LC to init the links role

/// Link policy HCI:4.6.9 and HCI:4.6.10
#define POLICY_SWITCH           0x0001
#define POLICY_HOLD             0x0002
#define POLICY_SNIFF            0x0004
#define POLICY_PARK             0x0008

/// Allow Role Switch     HCI:4.5.5
#define ROLE_SWITCH_NOT_ALLOWED 0
#define ROLE_SWITCH_ALLOWED     1

/// AcceptConnection Role     HCI:4.5.8
#define ACCEPT_SWITCH_TO_MASTER 0
#define ACCEPT_REMAIN_SLAVE     1

/// Packet Type Flags     HCI:7.1.14
#define PACKET_TYPE_EDR_MSK         0x330E
#define PACKET_TYPE_GFSK_MSK        0xCCF8
#define PACKET_TYPE_NO_2_DH1_FLAG   0x0002
#define PACKET_TYPE_NO_3_DH1_FLAG   0x0004
#define PACKET_TYPE_DM1_FLAG        0x0008
#define PACKET_TYPE_DH1_FLAG        0x0010
#define PACKET_TYPE_HV1_FLAG        0x0020
#define PACKET_TYPE_HV2_FLAG        0x0040
#define PACKET_TYPE_HV3_FLAG        0x0080
#define PACKET_TYPE_NO_2_DH3_FLAG   0x0100
#define PACKET_TYPE_NO_3_DH3_FLAG   0x0200
#define PACKET_TYPE_DM3_FLAG        0x0400
#define PACKET_TYPE_DH3_FLAG        0x0800
#define PACKET_TYPE_NO_2_DH5_FLAG   0x1000
#define PACKET_TYPE_NO_3_DH5_FLAG   0x2000
#define PACKET_TYPE_DM5_FLAG        0x4000
#define PACKET_TYPE_DH5_FLAG        0x8000

/// Synchronous Packet Types     HCI:7.1.14
#define SYNC_PACKET_TYPE_HV1_FLAG       0x0001
#define SYNC_PACKET_TYPE_HV2_FLAG       0x0002
#define SYNC_PACKET_TYPE_HV3_FLAG       0x0004
#define SYNC_PACKET_TYPE_EV3_FLAG       0x0008
#define SYNC_PACKET_TYPE_EV4_FLAG       0x0010
#define SYNC_PACKET_TYPE_EV5_FLAG       0x0020

#define SYNC_PACKET_TYPE_NO_EV3_2_FLAG  0x0040
#define SYNC_PACKET_TYPE_NO_EV3_3_FLAG  0x0080
#define SYNC_PACKET_TYPE_NO_EV5_2_FLAG  0x0100
#define SYNC_PACKET_TYPE_NO_EV5_3_FLAG  0x0200

#define SYNC_PACKET_TYPE_EV3_2_FLAG     0x0040
#define SYNC_PACKET_TYPE_EV3_3_FLAG     0x0080
#define SYNC_PACKET_TYPE_EV5_2_FLAG     0x0100
#define SYNC_PACKET_TYPE_EV5_3_FLAG     0x0200

/// RWBT 1.2
#define SYNC_EV3_PACKET_SIZE         30
#define SYNC_EV4_PACKET_SIZE        120
#define SYNC_EV5_PACKET_SIZE        180

/// Packet Boundary Flag   HCI:5.4.2
#define PBF_1ST_NF_HL_FRAG          0x00    // Non-flushable packets
#define PBF_CONT_HL_FRAG            0x01
#define PBF_1ST_HL_FRAG             0x02
#define PBF_CMP_PDU                 0x03
//#define PBF_MASK                    0x03

/// Broadcast Flag   HCI:5.4.2
#define BCF_P2P                     0x00
#define BCF_ACTIVE_SLV_BCST         0x01
#define BCF_PARK_SLV_BCST           0x02
#define BCF_MASK                    0x03

/// Synchronous Packet Status Flag   HCI:5.4.3
#define CORRECTLY_RX_FLAG       0x00
#define POSSIBLY_INVALID_FLAG   0x01
#define NO_RX_DATA_FLAG         0x02
#define PARTIALLY_LOST_FLAG     0x03

/// Park mode defines LMP:3.17
#define MACCESS_MSK             0x0F
#define ACCSCHEM_MSK            0xF0

/// Support 3 feature pages
#define FEATURE_PAGE_MAX   3

#define FEATURE_PAGE_0     0
#define FEATURE_PAGE_1     1
#define FEATURE_PAGE_2     2

/// Feature mask definition LMP:3.3
#define B0_3_SLOT_POS               0
#define B0_3_SLOT_MSK               0x01
#define B0_5_SLOT_POS               1
#define B0_5_SLOT_MSK               0x02
#define B0_ENC_POS                  2
#define B0_ENC_MSK                  0x04
#define B0_SLOT_OFF_POS             3
#define B0_SLOT_OFF_MSK             0x08
#define B0_TIMING_ACCU_POS          4
#define B0_TIMING_ACCU_MSK          0x10
#define B0_ROLE_SWITCH_POS          5
#define B0_ROLE_SWITCH_MSK          0x20
#define B0_HOLD_MODE_POS            6
#define B0_HOLD_MODE_MSK            0x40
#define B0_SNIFF_MODE_POS           7
#define B0_SNIFF_MODE_MSK           0x80

#define B1_PARK_POS                 0
#define B1_PARK_MSK                 0x01
#define B1_RSSI_POS                 1
#define B1_RSSI_MSK                 0x02
#define B1_CQDDR_POS                2
#define B1_CQDDR_MSK                0x04
#define B1_SCO_POS                  3
#define B1_SCO_MSK                  0x08
#define B1_HV2_POS                  4
#define B1_HV2_MSK                  0x10
#define B1_HV3_POS                  5
#define B1_HV3_MSK                  0x20
#define B1_MULAW_POS                6
#define B1_MULAW_MSK                0x40
#define B1_ALAW_POS                 7
#define B1_ALAW_MSK                 0x80

#define B2_CVSD_POS                 0
#define B2_CVSD_MSK                 0x01
#define B2_PAGING_PAR_NEGO_POS      1
#define B2_PAGING_PAR_NEGO_MSK      0x02
#define B2_PWR_CTRL_POS             2
#define B2_PWR_CTRL_MSK             0x04
#define B2_TRANSPARENT_SCO_POS      3
#define B2_TRANSPARENT_SCO_MSK      0x08
#define B2_FLOW_CTRL_LAG_POS        4
#define B2_FLOW_CTRL_LAG_MSK        0x70
#define B2_BCAST_ENC_POS            7
#define B2_BCAST_ENC_MSK            0x80

#define B3_EDR_2MBPS_ACL_POS        1
#define B3_EDR_2MBPS_ACL_MSK        0x02
#define B3_EDR_3MBPS_ACL_POS        2
#define B3_EDR_3MBPS_ACL_MSK        0x04
#define B3_ENH_INQSCAN_POS          3
#define B3_ENH_INQSCAN_MSK          0x08
#define B3_INT_INQSCAN_POS          4
#define B3_INT_INQSCAN_MSK          0x10
#define B3_INT_PAGESCAN_POS         5
#define B3_INT_PAGESCAN_MSK         0x20
#define B3_RSSI_INQ_RES_POS         6
#define B3_RSSI_INQ_RES_MSK         0x40
#define B3_ESCO_EV3_POS             7
#define B3_ESCO_EV3_MSK             0x80

#define B4_EV4_PKT_POS              0
#define B4_EV4_PKT_MSK              0x01
#define B4_EV5_PKT_POS              1
#define B4_EV5_PKT_MSK              0x02
#define B4_AFH_CAP_SLV_POS          3
#define B4_AFH_CAP_SLV_MSK          0x08
#define B4_AFH_CLASS_SLV_POS        4
#define B4_AFH_CLASS_SLV_MSK        0x10
#define B4_BR_EDR_NOT_SUPP_POS      5
#define B4_BR_EDR_NOT_SUPP_MSK      0x20
#define B4_LE_SUPP_POS              6
#define B4_LE_SUPP_MSK              0x40
#define B4_3_SLOT_EDR_ACL_POS       7
#define B4_3_SLOT_EDR_ACL_MSK       0x80

#define B5_5_SLOT_EDR_ACL_POS       0
#define B5_5_SLOT_EDR_ACL_MSK       0x01
#define B5_SSR_POS                  1
#define B5_SSR_MSK                  0x02
#define B5_PAUSE_ENC_POS            2
#define B5_PAUSE_ENC_MSK            0x04
#define B5_AFH_CAP_MST_POS          3
#define B5_AFH_CAP_MST_MSK          0x08
#define B5_AFH_CLASS_MST_POS        4
#define B5_AFH_CLASS_MST_MSK        0x10
#define B5_EDR_ESCO_2MBPS_POS       5
#define B5_EDR_ESCO_2MBPS_MSK       0x20
#define B5_EDR_ESCO_3MBPS_POS       6
#define B5_EDR_ESCO_3MBPS_MSK       0x40
#define B5_3_SLOT_EDR_ESCO_POS      7
#define B5_3_SLOT_EDR_ESCO_MSK      0x80

#define B6_EIR_POS                  0
#define B6_EIR_MSK                  0x01
#define B6_SIM_LE_BREDR_DEV_CAP_POS 1
#define B6_SIM_LE_BREDR_DEV_CAP_MSK 0x02
#define B6_SSP_POS                  3
#define B6_SSP_MSK                  0x08
#define B6_ENCAPS_PDU_POS           4
#define B6_ENCAPS_PDU_MSK           0x10
#define B6_ERR_DATA_REP_POS         5
#define B6_ERR_DATA_REP_MSK         0x20
#define B6_NONFLUSH_PBF_POS         6
#define B6_NONFLUSH_PBF_MSK         0x40

#define B7_LST_CHANGE_EVT_POS       0
#define B7_LST_CHANGE_EVT_MSK       0x01
#define B7_INQRES_TXPOW_POS         1
#define B7_INQRES_TXPOW_MSK         0x02
#define B7_ENH_PWR_CTRL_POS         2
#define B7_ENH_PWR_CTRL_MSK         0x04
#define B7_EXT_FEATS_POS            7
#define B7_EXT_FEATS_MSK            0x80

/// Extended feature mask definition page 1 LMP:3.3
#define B0_HOST_SSP_POS             0
#define B0_HOST_SSP_MSK             0x01
#define B0_HOST_LE_POS              1
#define B0_HOST_LE_MSK              0x02
#define B0_HOST_LE_BR_EDR_POS       2
#define B0_HOST_LE_BR_EDR_MSK       0x04
#define B0_HOST_SECURE_CON_POS      3
#define B0_HOST_SECURE_CON_MSK      0x08

/// Extended feature mask definition page 2 LMP:3.3
#define B0_CSB_MASTER_POS           0
#define B0_CSB_MASTER_MSK           0x01
#define B0_CSB_SLAVE_POS            1
#define B0_CSB_SLAVE_MSK            0x02
#define B0_SYNC_TRAIN_POS           2
#define B0_SYNC_TRAIN_MSK           0x04
#define B0_SYNC_SCAN_POS            3
#define B0_SYNC_SCAN_MSK            0x08
#define B0_INQ_RES_NOTIF_EVT_POS    4
#define B0_INQ_RES_NOTIF_EVT_MSK    0x10
#define B0_GEN_INTERL_SCAN_POS      5
#define B0_GEN_INTERL_SCAN_MSK      0x20
#define B0_COARSE_CLK_ADJ_POS       6
#define B0_COARSE_CLK_ADJ_MSK       0x40

#define B1_SEC_CON_CTRL_POS         0
#define B1_SEC_CON_CTRL_MSK         0x01
#define B1_PING_POS                 1
#define B1_PING_MSK                 0x02
#define B1_TRAIN_NUDGING_POS        3
#define B1_TRAIN_NUDGING_MSK        0x08

/// Features definitions
#define FEAT_3_SLOT_BIT_POS           0
#define FEAT_5_SLOT_BIT_POS           1
#define FEAT_ENC_BIT_POS              2
#define FEAT_SLOT_OFFSET_BIT_POS      3
#define FEAT_TIMING_ACC_BIT_POS       4
#define FEAT_SWITCH_BIT_POS           5
#define FEAT_HOLD_BIT_POS             6
#define FEAT_SNIFF_BIT_POS            7

#define FEAT_PARK_BIT_POS             8
#define FEAT_RSSI_BIT_POS             9
#define FEAT_QUALITY_BIT_POS          10
#define FEAT_SCO_BIT_POS              11
#define FEAT_HV2_BIT_POS              12
#define FEAT_HV3_BIT_POS              13
#define FEAT_ULAW_BIT_POS             14
#define FEAT_ALAW_BIT_POS             15

#define FEAT_CVSD_BIT_POS             16
#define FEAT_PAGING_BIT_POS           17
#define FEAT_POWER_BIT_POS            18
#define FEAT_TRANSP_SCO_BIT_POS       19
#define FEAT_BCAST_ENCRYPT_BIT_POS    23

#define FEAT_EDR_2MB_BIT_POS          25
#define FEAT_EDR_3MB_BIT_POS          26
#define FEAT_ENH_INQSCAN_BIT_POS      27
#define FEAT_INT_INQSCAN_BIT_POS      28
#define FEAT_INT_PAGESCAN_BIT_POS     29
#define FEAT_RSSI_INQRES_BIT_POS      30
#define FEAT_EV3_BIT_POS              31

#define FEAT_EV4_BIT_POS              32
#define FEAT_EV5_BIT_POS              33
#define FEAT_AFH_CAPABLE_S_BIT_POS    35
#define FEAT_AFH_CLASS_S_BIT_POS      36
#define FEAT_BR_EDR_NO_SUPP_BIT_POS   37
#define FEAT_LE_BIT_POS               38
#define FEAT_3_SLOT_EDR_BIT_POS       39
#define FEAT_5_SLOT_EDR_BIT_POS       40
#define FEAT_SNIFF_SUBRAT_BIT_POS     41
#define FEAT_PAUSE_ENCRYPT_BIT_POS    42
#define FEAT_AFH_CAPABLE_M_BIT_POS    43
#define FEAT_AFH_CLASS_M_BIT_POS      44
#define FEAT_EDR_ESCO_2MB_BIT_POS     45
#define FEAT_EDR_ESCO_3MB_BIT_POS     46
#define FEAT_3_SLOT_EDR_ESCO_BIT_POS  47
#define FEAT_EIR_BIT_POS              48
#define FEAT_LE_BR_EDR_BIT_POS        49
#define FEAT_SSP_BIT_POS              51
#define FEAT_ENCAP_PDU_BIT_POS        52
#define FEAT_ERRO_DATA_REP_BIT_POS    53
#define FEAT_NFLUSH_PBF_BIT_POS       54
#define FEAT_LSTO_CHG_EVT_BIT_POS     56
#define FEAT_INQ_TXPWR_BIT_POS        57
#define FEAT_EPC_BIT_POS              58
#define FEAT_EXT_FEATS_BIT_POS        63
#define FEAT_SSP_HOST_BIT_POS         64
#define FEAT_LE_HOST_BIT_POS          65
#define FEAT_LE_BR_EDR_HOST_BIT_POS   66
#define FEAT_SEC_CON_HOST_BIT_POS     67

#define FEAT_CSB_MASTER_BIT_POS        128
#define FEAT_CSB_SLAVE_BIT_POS         129
#define FEAT_SYNC_TRAIN_BIT_POS        130
#define FEAT_SYNC_SCAN_BIT_POS         131
#define FEAT_INQ_RES_NOTIF_EVT_BIT_POS 132
#define FEAT_GEN_INTERL_SCAN_BIT_POS   133
#define FEAT_COARSE_CLK_ADJ_BIT_POS    134
#define FEAT_SEC_CON_CTRL_BIT_POS      136
#define FEAT_PING_BIT_POS              137
#define FEAT_TRAIN_NUDGING_BIT_POS     139

/// Maximum number of feature bits per page (8 bytes x 8 bits)
#define MAX_FEAT_BITS_PER_PAGE         64

/// Poll interval defines LMP:5.2
#define POLL_INTERVAL_MIN       0x0006
#define POLL_INTERVAL_DFT       0x0028
#define POLL_INTERVAL_MAX       0x1000

/// Power Adjustment Request LMP:5.2
#define PWR_ADJ_REQ_DEC_1_STEP  0x00
#define PWR_ADJ_REQ_INC_1_STEP  0x01
#define PWR_ADJ_REQ_INC_MAX     0x02

/// Power Adjustment Response LMP:5.2
#define PWR_ADJ_RES_GFSK_POS    0
#define PWR_ADJ_RES_GFSK_MASK   0x03
#define PWR_ADJ_RES_DQPSK_POS   2
#define PWR_ADJ_RES_DQPSK_MASK  0x0C
#define PWR_ADJ_RES_8DPSK_POS   4
#define PWR_ADJ_RES_8DPSK_MASK  0x30

#define PWR_ADJ_RES_NOT_SUPP    0x00
#define PWR_ADJ_RES_CHG_1_STEP  0x01
#define PWR_ADJ_RES_MAX         0x02
#define PWR_ADJ_RES_MIN         0x03

/// Nb of Broadcast retransmissions defines
#define NB_BROADCAST_DFT        0x01

/// Nb of Broadcast CLK_ADJ PDU   Baseband:4.1.14.1
#define NB_BROADCAST_CLK_ADJ    0x06

/// Min PCA clk_adj_instant (in slots) LMP:4.1.14.1
#define PCA_INSTANT_MIN         12

/// Piconet Clock Adjustment clk_adj_mode LMP:4.1.14.1
#define CLK_ADJ_BEFORE_INSTANT   0
#define CLK_ADJ_AFTER_INSTANT    1


/// Different packet types BaseBand:6.7
/* Packet and buffer sizes. These sizes do not include payload header (except for FHS
 * packet where there is no payload header) since payload header is written or read by
 * the RWBT in a different control structure part (TX/RXPHDR)                          */
#define FHS_PACKET_SIZE         18
#define DM1_PACKET_SIZE         17
#define DH1_PACKET_SIZE         27
#define DH1_2_PACKET_SIZE       54
#define DH1_3_PACKET_SIZE       83
#define DV_ACL_PACKET_SIZE      9
#define DM3_PACKET_SIZE         121
#define DH3_PACKET_SIZE         183
#define DH3_2_PACKET_SIZE       367
#define DH3_3_PACKET_SIZE       552
#define DM5_PACKET_SIZE         224
#define DH5_PACKET_SIZE         339
#define DH5_2_PACKET_SIZE       679
#define DH5_3_PACKET_SIZE       1021
#define AUX1_PACKET_SIZE        29

#define HV1_PACKET_SIZE         10
#define HV2_PACKET_SIZE         20
#define HV3_PACKET_SIZE         30
#define EV3_PACKET_SIZE         30
#define EV3_2_PACKET_SIZE       60
#define EV3_3_PACKET_SIZE       90
#define EV4_PACKET_SIZE        120
#define EV5_PACKET_SIZE        180
#define EV5_2_PACKET_SIZE      360
#define EV5_3_PACKET_SIZE      540

/// SCO Packet coding LMP:5.2
#define SCO_PACKET_HV1          0x00
#define SCO_PACKET_HV2          0x01
#define SCO_PACKET_HV3          0x02

/// eSCO Packet coding LMP:5.2
#define ESCO_PACKET_NULL        0x00
#define ESCO_PACKET_EV3         0x07
#define ESCO_PACKET_EV4         0x0C
#define ESCO_PACKET_EV5         0x0D
#define ESCO_PACKET_EV3_2       0x26
#define ESCO_PACKET_EV3_3       0x37
#define ESCO_PACKET_EV5_2       0x2C
#define ESCO_PACKET_EV5_3       0x3D

/// Max number of HV packet BaseBand:4.4.2.1
#define MAX_NB_HV1              1
#define MAX_NB_HV2              2
#define MAX_NB_HV3              3

/// Tsco (ScoInterval) BaseBand:4.4.2.1
#define TSCO_HV1                2
#define TSCO_HV2                4
#define TSCO_HV3                6

/* Inquiry train repetition length , Baseband :Table 10.4
 *      - 256 repetitions if no SCO
 *      - 512 repetitions if 1 SCO
 *      - 768 repetitions if 2 SCO  */
#define INQ_TRAIN_LENGTH_NO_SCO 256
#define INQ_TRAIN_LENGTH_1_SCO  512
#define INQ_TRAIN_LENGTH_2_SCO  768

/* Counter for train length, Npage (N*16 slots) depends on the slave page scan mode and
 * the number of active SCO:
 *    | SR mode |  no SCO  |  one SCO  |  two SCO |
 *    |   R0    |   >=1    |    >=2    |    >=3   |
 *    |   R1    |   >=128  |    >=256  |    >=384 |
 *    |   R2    |   >=256  |    >=512  |    >=768 |   */
#define PAGE_TRAIN_LENGTH_R0    1
#define PAGE_TRAIN_LENGTH_R1    128
#define PAGE_TRAIN_LENGTH_R2    256

/// Synchronisation defines
#define NORMAL_SYNC_POS     (64 + 4)                // End of Synchro word at bit 68 (64 + 4)
#define SLOT_SIZE            625                    // A slot is 625 us
#define HALF_SLOT_SIZE       625                    // A half slot is 312.5 us (in half us)

/// Baseband timeout default value, Baseband timers: 1.1
#define PAGE_RESP_TO_DEF        8
#define INQ_RESP_TO_DEF         128
#define NEW_CONNECTION_TO       32

/// LMP Response Timeout (in sec)
#define LMP_RSP_TO             30
/// LLCP Response Timeout (in units of 10 ms)
#define LLCP_RSP_TO            3000 // 30 secs

/// Athenticated Payload Timeout (in units of 10 ms)
#define AUTH_PAYL_TO_DFT       0x0BB8  // 30 secs
#define AUTH_PAYL_TO_MIN       0x0001

/// Voice mute pattern defines
#define MU_LAW_MUTE             0xFF
#define ALAW_CVSD_MUTE          0x55
#define TRANSP_MUTE             0x00

/// Air Mode  LMP:5.2
#define MU_LAW_MODE             0
#define A_LAW_MODE              1
#define CVSD_MODE               2
#define TRANS_MODE              3

/// eSCO negotiation State LMP:5.2
#define ESCO_NEGO_INIT              0
#define ESCO_NEGO_LATEST_POSSIBLE   1
#define ESCO_NEGO_SLOT_VIOLATION    2
#define ESCO_NEGO_LAT_VIOLATION     3
#define ESCO_NEGO_UNSUPPORTED       4

#define SCO_BANDWIDTH               8000
#define SYNC_BANDWIDTH_DONT_CARE    0xFFFFFFFF

#define SYNC_MIN_LATENCY            0x0004
#define SYNC_MAX_LATENCY_ESCO_S1    0x0007
#define SYNC_MAX_LATENCY_ESCO_S2    0x0007
#define SYNC_MAX_LATENCY_ESCO_S3    0x000A
#define SYNC_DONT_CARE_LATENCY      0xFFFF

#define SYNC_NO_RE_TX           0x00
#define SYNC_RE_TX_POWER        0x01
#define SYNC_RE_TX_QUALITY      0x02
#define SYNC_RE_TX_DONT_CARE    0xFF

/// Timing Control Flags  LMP:5.2
#define TIM_CHANGE_FLAG         0x01
#define INIT2_FLAG              0x02
#define ACCESS_WIN_FLAG         0x04

/// Packet Type Table defines    LMP:4.1.11
#define PACKET_TABLE_1MBPS      0x00
#define PACKET_TABLE_2_3MBPS    0x01

/// Data Rate defines    LMP:5.2
#define FEC_RATE_MSK            0x01
#define USE_FEC_RATE            0x00
#define NO_FEC_RATE             0x01
#define PREF_PACK_MSK           0x06
#define NO_PREF_PACK_SIZE       0x00
#define USE_1_SLOT_PACKET       0x02
#define USE_3_SLOT_PACKET       0x04
#define USE_5_SLOT_PACKET       0x06
#define PREF_EDR_MSK            0x18
#define USE_DM1_ONLY            0x00
#define USE_2_MBPS_RATE         0x08
#define USE_3_MBPS_RATE         0x10
#define PREF_PACK_EDR_MSK       0x60
#define USE_1_SLOT_EDR_PKT      0x20
#define USE_3_SLOT_EDR_PKT      0x40
#define USE_5_SLOT_EDR_PKT      0x60

/// EIR Data Size HCI:6.24
#define EIR_DATA_SIZE         240

/// Voice setting HCI:4.7.29 & 4.7.30
#define INPUT_COD_LIN           0x0000
#define INPUT_COD_MULAW         0x0100
#define INPUT_COD_ALAW          0x0200
#define INPUT_COD_MSK           0x0300
#define INPUT_COD_OFF           8
#define INPUT_DATA_1COMP        0x0000
#define INPUT_DATA_2COMP        0x0040
#define INPUT_DATA_SMAG         0x0080
#define INPUT_DATA_UNSIGNED     0x00C0
#define INPUT_DATAFORM_MSK      0x00C0
#define INPUT_DATAFORM_OFF      6
#define INPUT_SAMP_8BIT         0x0000
#define INPUT_SAMP_16BIT        0x0020
#define INPUT_SAMPSIZE_MSK      0x0020
#define INPUT_SAMPSIZE_OFF      5
#define LIN_PCM_BIT_POS_MSK     0x001C
#define LIN_PCM_BIT_POS_OFF     2
#define AIR_COD_CVSD            0x0000
#define AIR_COD_MULAW           0x0001
#define AIR_COD_ALAW            0x0002
#define AIR_COD_TRANS           0x0003
#define AIR_COD_MSK             0x0003
#define AIR_COD_OFF             0

/// ScanEnable HCI:6.1
#define BOTH_SCAN_DISABLE       0
#define INQUIRY_SCAN_ENABLE     1
#define PAGE_SCAN_ENABLE        2
#define BOTH_SCAN_ENABLE        3

/// PageScanInterval HCI:6.8
#define PAGE_SCAN_INTV_MIN  0x0012
#define PAGE_SCAN_INTV_MAX  0x1000
#define PAGE_SCAN_INTV_DFT  0x0800

/// PageScanWindow HCI:6.9
#define PAGE_SCAN_WIN_MIN    0x0011
#define PAGE_SCAN_WIN_MAX    0x1000
#define PAGE_SCAN_WIN_DFT    0x0012

/// InquiryScanInterval HCI:6.2
#define INQ_SCAN_INTV_MIN  0x0012
#define INQ_SCAN_INTV_MAX  0x1000
#define INQ_SCAN_INTV_DFT  0x1000

/// InquiryScanWindow HCI:6.3
#define INQ_SCAN_WIN_MIN    0x0011
#define INQ_SCAN_WIN_MAX    0x1000
#define INQ_SCAN_WIN_DFT    0x0012

/// General/Unlimited Inquiry Access Code (GIAC)
#define GIAC_LAP_0              0x33
#define GIAC_LAP_1              0x8B
#define GIAC_LAP_2              0x9E

/// Limited Dedicated Inquiry Access Code (LIAC)
#define LIAC_LAP_0              0x00
#define LIAC_LAP_1              0x8B
#define LIAC_LAP_2              0x9E

/// Maximum Dedicated Inquiry Access Code (DIAC MAX)
#define DIAC_MAX_LAP_0          0x3F
#define DIAC_MAX_LAP_1          0x8B
#define DIAC_MAX_LAP_2          0x9E

/// PIN Type HCI:6.13
#define VARIABLE_PIN            0
#define FIXED_PIN               1

/// ConnectionAcceptTimeout HCI:6.7
#define CON_ACCEPT_TO_MIN  0x00A0
#define CON_ACCEPT_TO_MAX  0xB540
#define CON_ACCEPT_TO_DFT  0x1FA0

/// PageTimeout HCI:6.6
#define PAGE_TO_MIN        0x0016
#define PAGE_TO_MAX        0xFFFF
#define PAGE_TO_DFT        0x2000

/// Clock offset valid flag in clock offset field HCI:7.1.5/7.1.19
#define CLK_OFFSET_VALID_FLAG_POS    15
#define CLK_OFFSET_VALID_FLAG_MSK    0x8000

/// AuthenticationEnable HCI:4.7.24
#define AUTH_DISABLED           0x00        // Default
#define AUTH_ENABLED            0x01

/// EncryptionMode HCI:4.7.26
#define ENC_DISABLED            0x00        // Default
#define ENC_PP_ENABLED          0x01
#define ENC_PP_BC_ENABLED       0x02

/// AutomaticFlushTimeout HCI:4.7.32
#define AUTO_FLUSH_TIMEOUT_MAX  0x07FF
#define AUTO_FLUSH_TIMEOUT_OFF  0x0000
#define AUTO_FLUSH_TIMEOUT_DFT  AUTO_FLUSH_TIMEOUT_OFF      // Default (no automatic flush timeout)

/// Link Supervision Time Out (in slots) HCI:6.21
#define LSTO_OFF      0x0000
#define LSTO_MIN      0x0001
#define LSTO_DFT      0x7D00      // Default is 20 s
#define LSTO_MAX      0xFFFF

/// PageScanRepetitionMode HCI:4.5.5
#define R0                      0x00
#define R1                      0x01
#define R2                      0x02
#define PAGESCAN_REP_DEF        R1          // Default

/// PageScanPeriodMode HCI:4.7.49
#define P0                      0x00        // Default
#define P1                      0x01
#define P2                      0x02

/// PageScanMode HCI:4.7.51
#define MANDATORY_PAGE_SCAN_MODE 0x00       // Default

#define OPT_PAGE_SCAN_MODE_1    0x01
#define OPT_PAGE_SCAN_MODE_2    0x02
#define OPT_PAGE_SCAN_MODE_3    0x03

/// Encryption Enable HCI:4.5.17
#define ENCRYPTION_OFF          0x00
#define ENCRYPTION_ON           0x01

/// Country Code HCI:4.8.4
#define NORTH_AMERICA_EUROPE    0x00
#define FRANCE                  0x01
#define SPAIN                   0x02
#define JAPAN                   0x03

/// Loopback mode HCI:7.6.2
#define NO_LOOPBACK             0x00        // Default
#define LOCAL_LOOPBACK          0x01
#define REMOTE_LOOPBACK         0x02

/// Erroneous Data Reporting HCI:7.3.65
#define ERR_DATA_REP_DIS        0x00        // Default
#define ERR_DATA_REP_EN         0x01

/// LM modes HCI:5.2.20
#define LM_ACTIVE_MODE          0x00
#define LM_HOLD_MODE            0x01
#define LM_SNIFF_MODE           0x02
#define LM_PARK_MODE            0x03

/// Key Type HCI:5.2.24
#define COMB_KEY                0
//#define LOCAL_UNIT_KEY          1
//#define REMOTE_UNIT_KEY         2
#define DEBUG_COMB_KEY          3
#define UNAUTH_COMB_KEY_192     4
#define AUTH_COMB_KEY_192       5
#define CHANGED_COMB_KEY        6
#define UNAUTH_COMB_KEY_256     7
#define AUTH_COMB_KEY_256       8

/// Key Flag HCI:5.4.18
#define SEMI_PERMANENT_KEY      0x00
#define TEMPORARY_KEY           0x01

/// QOS Service Type HCI:4.6.6
#define QOS_NO_TRAFFIC          0x00
#define QOS_BEST_EFFORT         0x01
#define QOS_GUARANTEED          0x02
#define QOS_NOTSPECIFIED        0xFF

#define QOS_WILD_CARD           0xFFFFFFFF

/// RSSI golden range
#define RSSI_GOLDEN_RG        0x00

/// Inquiry TX power level (in dBm) HCI:7.3.62
#define INQ_TX_PWR_DBM_MIN    -70
#define INQ_TX_PWR_DBM_DFT    0
#define INQ_TX_PWR_DBM_MAX    +20

/// Bluetooth Test Mode defines    Bluetooth Test Mode: Table 3.2

#define PAUSE_MODE              0x00
#define TXTEST0_MODE            0x01
#define TXTEST1_MODE            0x02
#define TXTEST10_MODE           0x03
#define PRAND_MODE              0x04
#define ACLLOOP_MODE            0x05
#define SCOLOOP_MODE            0x06
#define ACLNOWHIT_MODE          0x07
#define SCONOWHIT_MODE          0x08
#define TXTEST1100_MODE         0x09
#define EXITTEST_MODE           0xFF

#define HOPSINGLE               0x00
#define HOPUSA                  0x01

#define FIXTXPOW                0x00
#define ADAPTTXPOW              0x01

/// Packet type parameter bit field of LMP_test_control
#define LMP_TEST_CTRL_PKT_TYPE_CODE_POS  0
#define LMP_TEST_CTRL_PKT_TYPE_CODE_MSK  0x0F
#define LMP_TEST_CTRL_PKT_TYPE_LINK_POS  4
#define LMP_TEST_CTRL_PKT_TYPE_LINK_MSK  0xF0
#define TEST_ACLSCO   0
#define TEST_ESCO     1
#define TEST_EDRACL   2
#define TEST_EDRESCO  3

/// LMP_encapsulated_header parameters LMP:5.3
#define LMP_ENCAPS_P192_MAJ_TYPE      1
#define LMP_ENCAPS_P192_MIN_TYPE      1
#define LMP_ENCAPS_P192_PAYL_LEN      48
#define LMP_ENCAPS_P192_PAYL_NB       3
#define LMP_ENCAPS_P256_MAJ_TYPE      1
#define LMP_ENCAPS_P256_MIN_TYPE      2
#define LMP_ENCAPS_P256_PAYL_LEN      64
#define LMP_ENCAPS_P256_PAYL_NB       4

/// Number of bits in the passkey code used during Secure Simple Pairing
#define SSP_PASSKEY_NB_BITS           20

// Event Filter HCI 4.7.3

/// Filter type
#define CLEAR_ALL_FILTER_TYPE   0x00
#define INQUIRY_FILTER_TYPE     0x01
#define CONNECTION_FILTER_TYPE  0x02

/// Filter size
#define CLEAR_ALL_FILTER_SIZE   0

/// Inquiry & Connection Setup Filter Condition Type
#define ALL_FILTER_CONDITION_TYPE       0x00
#define CLASS_FILTER_CONDITION_TYPE     0x01
#define BD_ADDR_FILTER_CONDITION_TYPE   0x02

/// Auto Accept Flag
#define DO_NOT_AUTO_ACCEPT_CONNECTION   0x01
#define ACCEPT_CONNECTION_SLAVE         0x02
#define ACCEPT_CONNECTION_MASTER        0x03

/// Event Mask HCI 4.7.1
#define NO_EVENTS_SPECIFIED_FILTER                            0x00000000
#define INQUIRY_COMPLETE_EVENT_FILTER                         0x00000001
#define INQUIRY_RESULT_EVENT_FILTER                           0x00000002
#define CONNECTION_COMPLETE_EVENT_FILTER                      0x00000004
#define CONNECTION_REQUEST_EVENT_FILTER                       0x00000008
#define DISCONNECTION_COMPLETE_EVENT_FILTER                   0x00000010
#define AUTHENTICATION_COMPLETE_EVENT_FILTER                  0x00000020
#define REMOTE_NAME_REQUEST_COMPLETE_EVENT_FILTER             0x00000040
#define ENCRYPTION_CHANGE_EVENT_FILTER                        0x00000080
#define CHANGE_CONNECTION_LINK_KEY_COMPLETE_EVENT_FILTER      0x00000100
#define MASTER_LINK_KEY_COMPLETE_EVENT_FILTER                 0x00000200
#define READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_EVENT_FILTER  0x00000400
#define READ_REMOTE_VERSION_INFORMATION_COMPLETE_EVENT_FILTER 0x00000800
#define QOS_SETUP_COMPLETE_EVENT_FILTER                       0x00001000
#define COMMAND_COMPLETE_EVENT_FILTER                         0x00002000   // Unchecked */
#define COMMAND_STATUS_EVENT_FILTER                           0x00004000   // Unchecked */
#define HARDWARE_ERROR_EVENT_FILTER                           0x00008000
#define FLUSH_OCCURRED_EVENT_FILTER                           0x00010000
#define ROLE_CHANGE_EVENT_FILTER                              0x00020000
#define NUMBER_OF_COMPLETED_PACKETS_EVENT_FILTER              0x00040000   // Unchecked */
#define MODE_CHANGE_EVENT_FILTER                              0x00080000
#define RETURN_LINK_KEYS_EVENT_FILTER                         0x00100000
#define PIN_CODE_REQUEST_EVENT_FILTER                         0x00200000
#define LINK_KEY_REQUEST_EVENT_FILTER                         0x00400000
#define LINK_KEY_NOTIFICATION_EVENT_FILTER                    0x00800000
#define LOOPBACK_COMMAND_EVENT_FILTER                         0x01000000   // Not implemented */
#define DATA_BUFFER_OVERFLOW_EVENT_FILTER                     0x02000000
#define MAX_SLOTS_CHANGE_EVENT_FILTER                         0x04000000
#define READ_CLOCK_OFFSET_COMPLETE_EVENT_FILTER               0x08000000
#define CONNECTION_PACKET_TYPE_CHANGED_EVENT_FILTER           0x10000000
#define QOS_VIOLATION_EVENT_FILTER                            0x20000000
#define PAGE_SCAN_MODE_CHANGE_EVENT_FILTER                    0x40000000   // Deprecated */
#define PAGE_SCAN_REPETITION_MODE_CHANGE_EVENT_FILTER         0x80000000

#define FLOW_SPECIFICATION_COMPLETE_EVENT_FILTER                0x00000001
#define INQUIRY_RESULT_WITH_RSSI_EVENT_FILTER                   0x00000002
#define READ_REMOTE_EXTENDED_FEATURES_COMPLETE_EVENT_FILTER     0x00000004
#define SYNCHRONOUS_CONNECTION_COMPLETE_EVENT_FILTER            0x00000800
#define SYNCHRONOUS_CONNECTION_CHANGE_EVENT_FILTER              0x00001000
#define SNIFF_SUBRATING_EVENT_FILTER                            0x00002000
#define EXTENDED_INQUIRY_RESULT_EVENT_FILTER                    0x00004000
#define ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT_FILTER            0x00008000
#define IO_CAPABILITY_REQUEST_EVENT_FILTER                      0x00010000
#define IO_CAPABILITY_REQUEST_REPLY_EVENT_FILTER                0x00020000
#define USER_CONFIRMATION_REQUEST_EVENT_FILTER                  0x00040000
#define USER_PASSKEY_REQUEST_EVENT_FILTER                       0x00080000
#define REMOTE_OOB_DATA_REQUEST_EVENT_FILTER                    0x00100000
#define SIMPLE_PAIRING_COMPLETE_EVENT_FILTER                    0x00200000
#define LINK_SUPERVISION_TIMEOUT_CHANGE_EVENT_FILTER            0x00800000
#define ENHANCED_FLUSH_COMPLETE_EVENT_FILTER                    0x01000000
#define USER_PASSKEY_NOTIFICATION_EVENT_FILTER                  0x04000000
#define KEYPRESS_NOTIFICATION_EVENT_FILTER                      0x08000000
#define REM_HOST_SUPPORTED_FEATURES_NOTIFICATION_EVENT_FILTER   0x10000000

/// HostControllerToHostFlowControl (ACL) HCI 7.3.40
#define FLOW_CONTROL_OFF                0x00
#define FLOW_CONTROL_ACL                0x01
#define FLOW_CONTROL_SCO                0x02
#define FLOW_CONTROL_ACL_SCO            0x03

/// SynchroinousFlowControlEnable (SCO) HCI 7.3.39
#define SYNC_FLOW_CONTROL_OFF           0x00
#define SYNC_FLOW_CONTROL_ON            0x01

/// Tx Power HCI:4.7.37
#define CURRENT_TX_POWER                0x00
#define MAX_TX_POWER                    0x01

/// Flow_direction HCI:7.2.13
#define FLOW_DIR_OUT                0x00
#define FLOW_DIR_IN                 0x01

/// Drift and Jitter default value LMP 5.2
#define DRIFT_BLE_DFT                   500
#define DRIFT_BT_DFT                    250
#define JITTER_DFT                      10
#define DRIFT_BT_ACTIVE_MAX             20 // BB:2.2.5

/// Read Stored Link Key HCI:4.7.8
#define LINK_KEY_BD_ADDR                0x00
#define LINK_KEY_ALL                    0x01

/// Read/Write Hold Mode Activity HCI:4.7.35 and 4.7.36
#define HOLD_MODE_ACTIV_DEFAULT                 0x00
#define HOLD_MODE_ACTIV_SUSP_PAGE_SCAN          0x01
#define HOLD_MODE_ACTIV_SUSP_INQUIRY_SCAN       0x02
#define HOLD_MODE_ACTIV_SUSP_PERIODIC_INQ       0x04
#define HOLD_MODE_ACTIV_NOT_MASK                0xF8

/// AFH Mode
#define AFH_DISABLED                  0x00
#define AFH_ENABLED                   0x01

/// AFH Reporting Mode
#define AFH_REPORTING_DISABLED        0x00
#define AFH_REPORTING_ENABLED         0x01

/// AFH channel assessment Mode
#define AFH_CH_ASS_DISABLED           0x00
#define AFH_CH_ASS_ENABLED            0x01

/// AFH MIn/Max interval, in BT slots (1s - 30s)
#define AFH_REPORT_INTERVAL_MIN       0x0640
#define AFH_REPORT_INTERVAL_MAX       0xBB80

/// Channel classification values for frequency pairs
#define AFH_CH_CLASS_UNKNOWN          0x0
#define AFH_CH_CLASS_GOOD             0x1
#define AFH_CH_CLASS_RESERVED         0x2
#define AFH_CH_CLASS_BAD              0x3

/// Maximum number of frequencies used in adapted channel hopping sequence
#define AFH_NB_CHANNEL_MIN            20
#define AFH_NB_CHANNEL_MAX            79

/// Maximum number of frequencies used in synchronization train BB:2.6.4.8
#define SYNC_TRAIN_CHANNEL_NB         3
/// Indices of frequencies used in synchronization train
#define SYNC_TRAIN_CHANNEL_0          0
#define SYNC_TRAIN_CHANNEL_1          24
#define SYNC_TRAIN_CHANNEL_2          78

/// Maximum delay in synchronization train (in slots) BB:2.7.2
#define SYNC_TRAIN_DELAY_MAX_DFT      16
/// Maximum delay in synchronization train for Coarse clock adjustment (in slots) BB:2.7.2
#define SYNC_TRAIN_DELAY_MAX_CLK_ADJ  4
/// Synchronization train interval for Coarse clock adjustment (in slots) BB:2.7.2
#define SYNC_TRAIN_INTV_CLK_ADJ       32

/// Future CSB instant value offset for Coarse clock adjustment (in slots) BB: 8.11.2
#define SYNC_TRAIN_CSB_INSTANT_OFFSET_CLK_ADJ  1600 

/// Minimum value for synchronization train interval (in slots) HCI:7.3.90
#define SYNC_TRAIN_INTV_MIN           0x20
/// Minimum value for synchronization train timeout (in slots) HCI:7.3.90
#define SYNC_TRAIN_TO_MIN             0x00000002
/// Maximum value for synchronization train timeout (in slots) HCI:7.3.90
#define SYNC_TRAIN_TO_MAX             0x07FFFFFE

/// Default value for synchronization train interval (in slots) HCI:6.36
#define SYNC_TRAIN_INTV_DEFAULT       0x80
/// Default value for synchronization train timeout (in slots) HCI:6.37
#define SYNC_TRAIN_TO_DEFAULT         0x0002EE00
/// Default value for synchronization train service data HCI:6.39
#define SYNC_TRAIN_SVC_DATA_DEFAULT   0x00

/// Minimum value for synchronization scan timeout (in slots) HCI:7.1.52
#define SYNC_SCAN_TO_MIN              0x22
/// Minimum value for synchronization scan window (in slots) HCI:7.1.52
#define SYNC_SCAN_WIN_MIN             0x22
/// Minimum value for synchronization scan interval (in slots) HCI:7.1.52
#define SYNC_SCAN_INTV_MIN            0x02

/// Default value for synchronization scan timeout (in slots) BB: Apppendix B
#define SYNC_SCAN_TO_DEFAULT          0x2000
/// Recommended value for synchronization scan window (91.25ms) GAP: Appendix A
#define SYNC_SCAN_WIN_DEFAULT         0x0092
/// Recommended value for synchronization scan interval (320 ms) GAP: Appendix A
#define SYNC_SCAN_INTV_DEFAULT        0x0200

/// CSB receive enable HCI:7.1.50
#define CSB_RX_MODE_DIS        0x00
#define CSB_RX_MODE_EN         0x01

///  CSB fragment HCI:7.2.88
#define CSB_CONTINUATION_FRAGMENT     0
#define CSB_STARTING_FRAGMENT         1
#define CSB_ENDING_FRAGMENT           2
#define CSB_NO_FRAGMENTATION          3

/// CSB max fragment size HCI:7.2.88
#define CSB_FRAGMENT_SIZE_MAX         0xFF

/// MWS Channel_Enable
#define MWS_CHANNEL_DISABLED             0x00
#define MWS_CHANNEL_ENABLED              0x01

/// MWS Channel_Type
#define MWS_TDD_CHANNEL_TYPE             0x00
#define MWS_FDD_CHANNEL_TYPE             0x01

/// MWS Transport_Layer
#define MWS_SIGNALING_ONLY               0x00
#define MWS_WCI_1                        0x01
#define MWS_WCI_2                        0x02
#define MWS_TRANSPORT_TYPE_MAX           0x02

/// MWS PATTERN Index
#define MWS_PATTERN_INDEX_MAX            2

/// MWS PATTERN IntervalType
#define MWS_PATTERN_NO_TXRX              0
#define MWS_PATTERN_TX_ALLOWED           1
#define MWS_PATTERN_RX_ALLOWED           2
#define MWS_PATTERN_TXRX_ALLOWED         3
#define MWS_PATTERN_EXT_FRAME            4
#define MWS_PATTERN_TYPE_MAX             4


/// MWS Ext_Num_Periods
#define MWS_EXT_NUM_PERIODS_MIN          0x01
#define MWS_EXT_NUM_PERIODS_MAX          0x32

/// MWS Period_Type
#define MWS_PERIOD_TYPE_DOWNLINK         0x00
#define MWS_PERIOD_TYPE_UPLINK           0x01
#define MWS_PERIOD_TYPE_BIDIRECTIONAL    0x02
#define MWS_PERIOD_TYPE_GUARD_PERIOD     0x03
#define MWS_PERIOD_TYPE_RESERVED         0x04

/// Simple pairing mode HCI:7.3.58/HCI:7.3.59
#define SP_MODE_DIS        0x00
#define SP_MODE_EN         0x01

/// Inquiry Scan Type and Page Scan Type HCI:6.4/HCI:6.11
#define STANDARD_SCAN           0x00
#define INTERLACED_SCAN         0x01

/// Default interlace offset used for frequency selection during interlaced inquiry/page scan  BB:8.3.1/8.4.1
#define INTERLACE_OFFSET_DFT    16

/// Inquiry Mode
#define STANDARD_INQUIRY        0x00
#define RSSI_INQUIRY            0x01
#define EXTENDED_INQUIRY        0x02

/// Maximum number of link keys Host can write via HCI Write Stored Link Key Command
#define NB_LINK_KEY             0x0B

/// LMP Version
#define BT_LMP_V1_0             0
#define BT_LMP_V1_1             1
#define BT_LMP_V1_2             2
#define BT_LMP_V2_0             3
#define BT_LMP_V2_1             4
#define BT_LMP_V3_0             5
#define BT_LMP_V4_0             6
#define BT_LMP_V4_1             7

/// WhichClock parameter
#define LOCAL_CLOCK             0
#define PICONET_CLOCK           1

/// Clock Accuracy parameter
#define CLOCK_ACCURACY_UNKNOWN  0xFFFF

#define SP_PASSKEY_STARTED          0x00
#define SP_PASSKEY_DIGIT_ENTERED    0x01
#define SP_PASSKEY_DIGIT_ERASED     0x02
#define SP_PASSKEY_CLEARED          0x03
#define SP_PASSKEY_COMPLETED        0x04

/// Low Power Mode
#define PARK_BEACON_MIN             0x000E

/// RWBT Device can be slave of 2 master at max
#define MAX_SLAVES_FOR_DIFFERENT_MASTERS    2
// Flags for ld_util_get_nb_acl function
/// Flag for master link
#define MASTER_FLAG       0x01
/// Flag for slave link
#define SLAVE_FLAG        0x02


/// BLE packet info in bytes
#define  BLE_PKT_HDR_LEN       (2)
#define  BLE_PKT_CRC_LEN       (3)
#define  BLE_PKT_ACCESS_LEN    (4)

/// IFS duration in us
#define  BLE_IFS_DUR           (150)


/// BLE event mask
enum le_evt_mask
{
    LE_EVT_MASK_CON_CMP_EVT_BIT                     = 0,     //!< LE_EVT_MASK_CON_CMP_EVT_BIT
    LE_EVT_MASK_CON_CMP_EVT_MSK                     = 0x0001,//!< LE_EVT_MASK_CON_CMP_EVT_MSK
    LE_EVT_MASK_ADV_REP_EVT_BIT                     = 1,     //!< LE_EVT_MASK_ADV_REP_EVT_BIT
    LE_EVT_MASK_ADV_REP_EVT_MSK                     = 0x0002,//!< LE_EVT_MASK_ADV_REP_EVT_MSK
    LE_EVT_MASK_CON_UPD_EVT_BIT                     = 2,     //!< LE_EVT_MASK_CON_UPD_EVT_BIT
    LE_EVT_MASK_CON_UPD_EVT_MSK                     = 0x0004,//!< LE_EVT_MASK_CON_UPD_EVT_MSK
    LE_EVT_MASK_CON_RD_REM_FEAT_EVT_BIT             = 3,     //!< LE_EVT_MASK_CON_RD_REM_FEAT_EVT_BIT
    LE_EVT_MASK_CON_RD_REM_FEAT_EVT_MSK             = 0x0008,//!< LE_EVT_MASK_CON_RD_REM_FEAT_EVT_MSK
    LE_EVT_MASK_LG_TR_KEY_REQ_EVT_BIT               = 4,     //!< LE_EVT_MASK_LG_TR_KEY_REQ_EVT_BIT
    LE_EVT_MASK_LG_TR_KEY_REQ_EVT_MSK               = 0x0010,//!< LE_EVT_MASK_LG_TR_KEY_REQ_EVT_MSK
    LE_EVT_MASK_REM_CON_PARA_REQ_EVT_BIT            = 5,     //!< LE_EVT_MASK_REM_CON_PARA_REQ_EVT_BIT
    LE_EVT_MASK_REM_CON_PARA_REQ_EVT_MSK            = 0x0020,//!< LE_EVT_MASK_REM_CON_PARA_REQ_EVT_MSK
    LE_EVT_MASK_DATA_LEN_CHG_EVT_BIT                = 6,     //!< LE_EVT_MASK_DATA_LEN_CHG_EVT_BIT
    LE_EVT_MASK_DATA_LEN_CHG_EVT_MSK                = 0x0040,//!< LE_EVT_MASK_DATA_LEN_CHG_EVT_MSK
    LE_EVT_MASK_RD_LOC_P256_PUB_KEY_CMP_EVT_BIT     = 7,     //!< LE_EVT_MASK_RD_LOC_P256_PUB_KEY_CMP_EVT_BIT
    LE_EVT_MASK_RD_LOC_P256_PUB_KEY_CMP_EVT_MSK     = 0x0080,//!< LE_EVT_MASK_RD_LOC_P256_PUB_KEY_CMP_EVT_MSK
    LE_EVT_MASK_GEN_DHKEY_CMP_EVT_BIT               = 8,     //!< LE_EVT_MASK_GEN_DHKEY_CMP_EVT_BIT
    LE_EVT_MASK_GEN_DHKEY_CMP_EVT_MSK               = 0x0100,//!< LE_EVT_MASK_GEN_DHKEY_CMP_EVT_MSK
    LE_EVT_MASK_ENH_CON_CMP_EVT_BIT                 = 9,     //!< LE_EVT_MASK_ENH_CON_CMP_EVT_BIT
    LE_EVT_MASK_ENH_CON_CMP_EVT_MSK                 = 0x0200,//!< LE_EVT_MASK_ENH_CON_CMP_EVT_MSK
    LE_EVT_MASK_DIR_ADV_REP_EVT_BIT                 = 10,    //!< LE_EVT_MASK_DIR_ADV_REP_EVT_BIT
    LE_EVT_MASK_DIR_ADV_REP_EVT_MSK                 = 0x0400,//!< LE_EVT_MASK_DIR_ADV_REP_EVT_MSK
    LE_EVT_MASK_PHY_UPD_CMP_EVT_BIT                 = 11,    //!< LE_EVT_MASK_PHY_UPD_CMP_EVT_BIT
    LE_EVT_MASK_PHY_UPD_CMP_EVT_MSK                 = 0x0800,//!< LE_EVT_MASK_PHY_UPD_CMP_EVT_MSK

    LE_EVT_MASK_DFT                                 = 0x001F,//!< LE_EVT_MASK_DFT
};

/// Enhanced Synchronous Connection HCI:7.1.41 & 7.1.42
#define    CODING_FORMAT_ULAW          0x00
#define    CODING_FORMAT_ALAW          0x01
#define    CODING_FORMAT_CVSD          0x02
#define    CODING_FORMAT_TRANSP        0x03
#define    CODING_FORMAT_LINPCM        0x04
#define    CODING_FORMAT_MSBC          0x05
#define    CODING_FORMAT_VENDSPEC      0xFF

#define    PCM_FORMAT_NA               0x00
#define    PCM_FORMAT_1SCOMP           0x01
#define    PCM_FORMAT_2SCOMP           0x02
#define    PCM_FORMAT_SIGNMAG          0x03
#define    PCM_FORMAT_UNSIGNED         0x04

#define    PCM_SAMPLE_SIZE_8BITS       8
#define    PCM_SAMPLE_SIZE_16BITS      16

#define    AUDIO_DATA_PATH_HCI               0
#define    AUDIO_DATA_PATH_PCM               1

/// Default maximum number of slots per packet
#define MAX_SLOT_DFT   1

/// Packet type code interpretation possibilities BB:6.5
#define ID_NUL_TYPE     0x0
#define POLL_TYPE       0x1
#define FHS_TYPE        0x2
#define DM1_TYPE        0x3
#define DH1_TYPE        0x4
#define DH1_2_TYPE      0x4
#define DH1_3_TYPE      0x8
#define HV1_TYPE        0x5
#define HV2_TYPE        0x6
#define EV3_2_TYPE      0x6
#define HV3_TYPE        0x7
#define EV3_TYPE        0x7
#define EV3_3_TYPE      0x7
#define DV_TYPE         0x8
#define AUX1_TYPE       0x9
#define DM3_TYPE        0xA
#define DH3_TYPE        0xB
#define DH3_2_TYPE      0xA
#define DH3_3_TYPE      0xB
#define EV4_TYPE        0xC
#define EV5_2_TYPE      0xC
#define EV5_TYPE        0xD
#define EV5_3_TYPE      0xD
#define DM5_TYPE        0xE
#define DH5_TYPE        0xF
#define DH5_2_TYPE      0xE
#define DH5_3_TYPE      0xF

/// Format of the FHS payload BB:6.5.1.4
#define FHS_PAR_BITS_POS        0
#define FHS_PAR_BITS_LEN        34
#define FHS_PAR_BITS_END        (FHS_PAR_BITS_POS + FHS_PAR_BITS_LEN)
#define FHS_LAP_POS             FHS_PAR_BITS_END
#define FHS_LAP_LEN             24
#define FHS_LAP_END             (FHS_LAP_POS + FHS_LAP_LEN)
#define FHS_EIR_POS             FHS_LAP_END
#define FHS_EIR_LEN             1
#define FHS_EIR_END             (FHS_EIR_POS + FHS_EIR_LEN)
#define FHS_UNDEF_POS           FHS_EIR_END
#define FHS_UNDEF_LEN           1
#define FHS_UNDEF_END           (FHS_UNDEF_POS + FHS_UNDEF_LEN)
#define FHS_SR_POS              FHS_UNDEF_END
#define FHS_SR_LEN              2
#define FHS_SR_END              (FHS_SR_POS + FHS_SR_LEN)
#define FHS_RSVD_POS            FHS_SR_END
#define FHS_RSVD_LEN            2
#define FHS_RSVD_END            (FHS_RSVD_POS + FHS_RSVD_LEN)
#define FHS_UAP_POS             FHS_RSVD_END
#define FHS_UAP_LEN             8
#define FHS_UAP_END             (FHS_UAP_POS + FHS_UAP_LEN)
#define FHS_NAP_POS             FHS_UAP_END
#define FHS_NAP_LEN             16
#define FHS_NAP_END             (FHS_NAP_POS + FHS_NAP_LEN)
#define FHS_CLASS_OF_DEV_POS    FHS_NAP_END
#define FHS_CLASS_OF_DEV_LEN    24
#define FHS_CLASS_OF_DEV_END    (FHS_CLASS_OF_DEV_POS + FHS_CLASS_OF_DEV_LEN)
#define FHS_LT_ADDR_POS         FHS_CLASS_OF_DEV_END
#define FHS_LT_ADDR_LEN         3
#define FHS_LT_ADDR_END         (FHS_LT_ADDR_POS + FHS_LT_ADDR_LEN)
#define FHS_CLK_POS             FHS_LT_ADDR_END
#define FHS_CLK_LEN             26
#define FHS_CLK_END             (FHS_CLK_POS + FHS_CLK_LEN)
#define FHS_PAGE_SCAN_MODE_POS  FHS_CLK_END
#define FHS_PAGE_SCAN_MODE_LEN  3
#define FHS_PAGE_SCAN_MODE_END  (FHS_PAGE_SCAN_MODE_POS + FHS_PAGE_SCAN_MODE_LEN)

/// Format of the STP payload BB:8.11.2
#define STP_CLK_POS             0
#define STP_CLK_LEN             4
#define STP_FUT_CSB_INST_POS    4
#define STP_FUT_CSB_INST_LEN    4
#define STP_AFH_CH_MAP_POS      8
#define STP_AFH_CH_MAP_LEN      10
#define STP_MST_BD_ADDR_POS     18
#define STP_MST_BD_ADDR_LEN     6
#define STP_CSB_INTV_POS        24
#define STP_CSB_INTV_LEN        2
#define STP_CSB_LT_ADDR_POS     26
#define STP_CSB_LT_ADDR_LEN     1
#define STP_SVC_DATA_POS        27
#define STP_SVC_DATA_LEN        1
#define STP_PACKET_SIZE         28

/// CSB Receive status HCI:7.7.69
#define CSB_RX_OK        0x00
#define CSB_RX_KO        0x01


/// HCI 7.8.18 LE Connection Update Command
/// Connection interval min (N*1.250ms)
#define LE_CNX_INTERVAL_MIN            6       //(0x06)
/// Connection interval Max (N*1.250ms)
#define LE_CNX_INTERVAL_MAX            3200    //(0xC80)
/// Connection latency min (N*cnx evt)
#define LE_CNX_LATENCY_MIN             0       //(0x00)
/// Connection latency Max (N*cnx evt
#define LE_CNX_LATENCY_MAX             500     //(0x1F4)
/// Supervision TO min (N*10ms)
#define LE_CNX_SUP_TO_MIN              10      //(0x0A)
/// Supervision TO Max (N*10ms)
#define LE_CNX_SUP_TO_MAX              3200    //(0xC80)
/// Connection event length min (N*0.625ms)
#define LE_CNX_CE_LGTH_MIN             0       //(0x00)
/// Connection event length  Max (N*0.625ms)
#define LE_CNX_CE_LGTH_MAX             65535   //(0xFFFF)'


/// HCI 7.8.33 LE Set Data Length Command
/// Preferred minimum number of payload octets
#define LE_MIN_OCTETS    (27)
/// Preferred minimum  number of microseconds
#define LE_MIN_TIME     (328)
/// Preferred maximum number of payload octets
#define LE_MAX_OCTETS   (251)
/// Preferred maximum number of microseconds
#define LE_MAX_TIME    (2120)

/// LE LL 2.1.2 Access Address
#define LE_ADV_CH_ACC_ADDR_H        0x8E89
#define LE_ADV_CH_ACC_ADDR_L        0xBED6

/// Resolvable private address timeout (in seconds) HCI:7.8.45
#define RPA_TO_MAX         0xA1B8 // approximately 11.5 hours
#define RPA_TO_DFT         0x0384 // 900 seconds or 15 minutes
#define RPA_TO_MIN         0x0001 // 1 second

/// Scan/Init PHYs (bit fields, as defined in HCI:7.8.64, HCI:7.8.66).
#define LE_1M_PHY_BIT_POS        0
#define LE_1M_PHY_BIT_MSK        0x01
#define LE_CODED_PHY_BIT_POS     2
#define LE_CODED_PHY_BIT_MSK     0x04

/// Max scanning PHYs which can be set HCI:7.8.64
#define MAX_SCAN_PHYS     2

/// Max initiatng PHYs which can be set HCI:7.8.66
#define MAX_INIT_PHYS     2   

/// Ext Scanning interval (in 625us slot) (chapter 2.E.7.8.64)
#define EXT_SCAN_INTERVAL_MIN     0x0004 //(2.5 ms)
#define EXT_SCAN_INTERVAL_MAX     0xFFFF //(40.96 sec)

/// Ext Scanning window (in 625us slot) (chapter 2.E.7.8.64)
#define EXT_SCAN_WINDOW_MIN     0x0004 //(2.5 ms)
#define EXT_SCAN_WINDOW_MAX     0xFFFF //(40.96 sec)

/// Advertiser PHY definitions for Ext Adv report Evts  HCI:7.7.65.13
#define HCI_ADV_PHY_1M              1
#define HCI_ADV_PHY_2M              2
#define HCI_ADV_PHY_CODED           3

/// Duration of 1MBPS PDU of specified payload length in microseconds (chapter 6.B.2.1)
#define PDU_1MBPS_LEN_US(n_bytes) ((8 + (2 + n_bytes))*8) // (1 + 4 + (2 + payload_len) + 3)*8

/// Duration of 2MBPS PDU of specified payload length in microseconds (chapter 6.B.2.1)
#define PDU_2MBPS_LEN_US(n_bytes) ((9 + (2 + n_bytes))*4) // (2 + 4 + (2 + payload_len) + 3)*4

/// Duration of 500KBPS PDU of specified payload length in microseconds (chapter 6.B.2.2)
#define PDU_500KBPS_LEN_US(n_bytes) (430 + (2 + n_bytes)*16) // 80 + 256 + 16 + 24 + (2 + payload_len)*8*2 + 24*2 + 3*2

/// Duration of 125KBPS PDU of specified payload length in microseconds (chapter 6.B.2.2)
#define PDU_125KBPS_LEN_US(n_bytes) (592 + (2 + n_bytes)*64) // 80 + 256 + 16 + 24 + (2 + payload_len)*8*8 + 24*8 + 3*8

/// Maximum length of BLE advertising channel PDU payloads ((chapter 6.B.2.3))
#define PDU_ADV_PAYLOAD_LEN_MAX     255

/// Ext Adv Report  - ADV SID - Define for no ADI filed in the PDU  HCI:7.7.65.13
#define REP_ADV_NO_ADI              0xFF

/// Ext Adv Report  - Tx Power & RSSI dBm - Define for information not available    HCI:7.7.65.13
#define REP_ADV_DBM_UNKNOWN         127

/// Periodic Advertising Report - Unused parameter - mandatory value HCI:7.7.65.15
#define PER_ADV_REPORT_TRAIL_BYTE  0xFF

/// Maximum advertising handle HCI:7.8.53
#define ADV_HDL_MAX     0xEF

/// Advertising_Tx_Power, Host has no preference HCI:7.8.53
#define ADV_TX_PWR_NO_PREF     127


/*
 * ENUMERATIONS
 ****************************************************************************************
 */
/// BLE 2MBPS HCI:7.8.x
enum le_phys_preference
{
    /// The Host has no preference among the transmitter PHYs supported by the Controller
    ALL_PHYS_TX_NO_PREF = (1 << 0),
    /// The Host has no preference among the receiver PHYs supported by the Controller
    ALL_PHYS_RX_NO_PREF = (1 << 1),
};

enum le_phy_value
{
    /// The Host prefers to use the LE 1M transmitter/receiver PHY (possibly among others)
    PHY_1MBPS      = (1<<0),
    /// The Host prefers to use the LE 2M transmitter/receiver PHY (possibly among others)
    PHY_2MBPS      = (1<<1),
    /// The Host prefers to use the LE Coded transmitter/receiver PHY (possibly among others)
    PHY_LE_CODED   = (1<<2),
    /// The Host prefers to use the LE Coded transmitter/receiver PHY (possibly among others)
    PHY_ALL        = (PHY_1MBPS | PHY_2MBPS | PHY_LE_CODED),
};

enum le_phy_opt
{
    /// The Host has no preferred coding when transmitting on the LE Coded PHY
    PHY_OPT_NO_LE_CODED_TX_PREF,
    /// The Host prefers that S=2 coding be used when transmitting on the LE Coded PHY
    PHY_OPT_S2_LE_CODED_TX_PREF,
    /// The Host prefers that S=8 coding be used when transmitting on the LE Coded PHY
    PHY_OPT_S8_LE_CODED_TX_PREF,
};


enum le_phy_mode
{
    PHYS_MOD_STANDARD ,
    PHYS_MOD_STABLE   ,
    PHYS_MOD_MAX,
};

///Transmit Power level types
enum
{
    ///Current Power Level
    TX_PW_LVL_CURRENT             = 0x00,
    ///Maximum power level
    TX_PW_LVL_MAX,
};

///Controller to Host flow control
enum
{
    /// C-> H flow control off
    FLOW_CTRL_OFF                 = 0x00,
    ///C->H ACL flow control on only
    FLOW_CTRL_ON_ACL_OFF_SYNC,
    ///C->H Sync flow control on only
    FLOW_CTRL_OFF_ACL_ON_SYNC,
    ///C->H ACL and Sync  flow control on
    FLOW_CTRL_ON_ACL_ON_SYNC,
};

///LE Supported Host enable
enum
{
    ///Disable LE supported Host
    LE_SUPP_HOST_DIS              = 0x00,
    ///Enable LE Supported Host
    LE_SUPP_HOST_EN,
};

///Simultaneous LE Host enable
enum
{
    ///Disable LE simultaneous Host disable
    SIMULT_LE_HOST_DIS            = 0x00,
    ///Enable LE simultaneous Host disable
    SIMULT_LE_HOST_EN,
};

///Advertising HCI Type
enum
{
    ///Connectable Undirected advertising
    ADV_CONN_UNDIR                = 0x00,
    ///Connectable high duty cycle directed advertising
    ADV_CONN_DIR,
    ///Discoverable undirected advertising
    ADV_DISC_UNDIR,
    ///Non-connectable undirected advertising
    ADV_NONCONN_UNDIR,
    ///Connectable low duty cycle directed advertising
    ADV_CONN_DIR_LDC,
};

///Advertising event properties values for legacy PDUs
enum
{
    ///Connectable and scannable undirected
    ADV_IND                   = 0x13,
    ///Connectable directed (low duty cycle)
    ADV_DIRECT_LO_IND         = 0x15,
    ///Connectable directed (high duty cycle)
    ADV_DIRECT_HI_IND         = 0x1D,
    ///Connectable directed (received)
    ADV_DIRECT_IND            = 0x15,
    ///Scannable undirected
    ADV_SCAN_IND              = 0x12,
    ///Non-connectable and non-scannable undirected
    ADV_NONCONN_IND           = 0x10,
    ///Scan response to an ADV_IND
    SCAN_RSP_TO_ADV_IND       = 0x1B,
    ///Scan response to an ADV_SCAN_IND
    SCAN_RSP_TO_ADV_SCAN_IND  = 0x1A,
};

///Advertising event properties masks
enum
{
    ///Connectable advertising
    ADV_CON                = 0x01,
    ///Scannable advertising
    ADV_SCAN               = 0x02,
    ///Directed advertising
    ADV_DIRECT             = 0x04,
    ///High duty cycle directed connectable advertising
    ADV_DIRECT_HI          = 0x08,
    ///Use legacy advertising PDUs
    ADV_LEGACY             = 0x10,
    ///Omit advertiser's address from all PDUs ("anonymous advertising")
    ADV_ANONYMOUS          = 0x20,
    ///Include TxPower in the extended header of the advertising PDU
    ADV_TX_PWR             = 0x40,
};

///Scanning HCI Type
enum
{
    ///Scan request
    SCAN_REQ,
    ///Scan response
    SCAN_RSP,
};

///BD address type
enum
{
    ///Public BD address
    ADDR_PUBLIC                   = 0x00,
    ///Random BD Address
    ADDR_RAND,
    /// Controller generates Resolvable Private Address based on the
    /// local IRK from resolving list. If resolving list contains no matching
    /// entry, use public address.
    ADDR_RPA_OR_PUBLIC,
    /// Controller generates Resolvable Private Address based on the
    /// local IRK from resolving list. If resolving list contains no matching
    /// entry, use random address.
    ADDR_RPA_OR_RAND,
    /// mask used to determine Address type in the air
    ADDR_MASK                     = 0x01,
    /// mask used to determine if an address is an RPA
    ADDR_RPA_MASK                 = 0x02,
    /// Random device address (controller unable to resolve)
    ADDR_RAND_UNRESOLVED          = 0xFE,
    /// No address provided (anonymous advertisement)
    ADDR_NONE                     = 0xFF,
};

///Privacy mode type
enum
{
    ///Network privacy mode
    PRIV_TYPE_NETWORK = 0x00,
    ///Device privacy mode
    PRIV_TYPE_DEVICE  = 0x01,
};

/// Random Address type (2 MSB of the LE BD Address)
enum rnd_addr_type
{
    /// Static random address           - 11 (MSB->LSB)
    RND_STATIC_ADDR     = 0xC0,
    /// Private non resolvable address  - 00 (MSB->LSB)
    RND_NON_RSLV_ADDR   = 0x00,
    /// Private resolvable address      - 01 (MSB->LSB)
    RND_RSLV_ADDR       = 0x40,
};

///Advertising channels enables
enum adv_channel_map
{
    ///Byte value for advertising channel map for channel 37 enable
    ADV_CHNL_37_EN                = 0x01,
    ///Byte value for advertising channel map for channel 38 enable
    ADV_CHNL_38_EN                = 0x02,
    ///Byte value for advertising channel map for channel 39 enable
    ADV_CHNL_39_EN                = 0x04,
    ///Byte value for advertising channel map for channel 37, 38 and 39 enable
    ADV_ALL_CHNLS_EN              = 0x07,
};

///Advertising filter policy
enum adv_filter_policy
{
    ///Allow both scan and connection requests from anyone
    ADV_ALLOW_SCAN_ANY_CON_ANY    = 0x00,
    ///Allow both scan req from White List devices only and connection req from anyone
    ADV_ALLOW_SCAN_WLST_CON_ANY,
    ///Allow both scan req from anyone and connection req from White List devices only
    ADV_ALLOW_SCAN_ANY_CON_WLST,
    ///Allow scan and connection requests from White List devices only
    ADV_ALLOW_SCAN_WLST_CON_WLST,
};

///Advertising enables
enum
{
    ///Disable advertising
    ADV_DIS                       = 0x00,
    ///Enable advertising
    ADV_EN,
};

///LE Scan type
enum
{
    ///Passive scan
    SCAN_PASSIVE                  = 0x00,
    ///Active scan
    SCAN_ACTIVE,
};

///Scan filter policy
enum scan_filter_policy
{
    ///Allow advertising packets from anyone
    SCAN_ALLOW_ADV_ALL            = 0x00,
    ///Allow advertising packets from White List devices only
    SCAN_ALLOW_ADV_WLST,
    ///Allow advertising packets from anyone and Direct adv using RPA in InitA
    SCAN_ALLOW_ADV_ALL_AND_INIT_RPA,
    ///Allow advertising packets from White List devices only and Direct adv using RPA in InitA
    SCAN_ALLOW_ADV_WLST_AND_INIT_RPA,
};

///Le Scan enables
enum
{
    ///Disable scan
    SCAN_DIS                      = 0x00,
    ///Enable scan
    SCAN_EN,
};

///Filter duplicates
enum scan_dup_filter_policy
{
    ///Disable filtering of duplicate packets
    SCAN_FILT_DUPLIC_DIS          = 0x00,
    ///Enable filtering of duplicate packets
    SCAN_FILT_DUPLIC_EN,
    ///Enable filtering of duplicate packets per scan period
    SCAN_FILT_DUPLIC_EN_PER_PERIOD,
};

///Initiator Filter policy
enum
{
    ///Initiator will ignore White List
    INIT_FILT_IGNORE_WLST         = 0x00,
    ///Initiator will use White List
    INIT_FILT_USE_WLST,
};

///Transmitter test Packet Payload Type
enum
{
    ///Pseudo-random 9 TX test payload type
    PAYL_PSEUDO_RAND_9            = 0x00,
    ///11110000 TX test payload type
    PAYL_11110000,
    ///10101010 TX test payload type
    PAYL_10101010,
    ///Pseudo-random 15 TX test payload type
    PAYL_PSEUDO_RAND_15,
    ///All 1s TX test payload type
    PAYL_ALL_1,
    ///All 0s TX test payload type
    PAYL_ALL_0,
    ///00001111 TX test payload type
    PAYL_00001111,
    ///01010101 TX test payload type
    PAYL_01010101,
};

/// Constant defining the role
enum
{
    ///Master role
    ROLE_MASTER,
    ///Slave role
    ROLE_SLAVE,
};

/// Constant clock accuracy
enum
{
    ///Clock accuracy at 500PPM
    SCA_500PPM,
    ///Clock accuracy at 250PPM
    SCA_250PPM,
    ///Clock accuracy at 150PPM
    SCA_150PPM,
    ///Clock accuracy at 100PPM
    SCA_100PPM,
    ///Clock accuracy at 75PPM
    SCA_75PPM,
    ///Clock accuracy at 50PPM
    SCA_50PPM,
    ///Clock accuracy at 30PPM
    SCA_30PPM,
    ///Clock accuracy at 20PPM
    SCA_20PPM
};

///Advertising pdu Type
enum ble_adv_type
{
    /// Undirected advertising
    BLE_ADV_IND                = 0x00,
    /// Directed advertising
    BLE_ADV_DIRECT_IND         = 0x01,
    /// Non Connectable advertising
    BLE_ADV_NONCONN_IND        = 0x02,
    /// Scan Request on primary channel
    BLE_SCAN_REQ               = 0x03,
    /// Scan Response on secondary channel
    BLE_AUX_SCAN_REQ           = 0x03,

    /// Scan Response
    BLE_SCAN_RSP               = 0x04,
    /// Connect Request on primary channel
    BLE_CONNECT_IND            = 0x05,
    /// Connect Request on secondary channel
    BLE_AUX_CONNECT_REQ        = 0x05,

    /// Discoverable advertising
    BLE_ADV_SCAN_IND           = 0x06,

    /// Extended Advertising indication on primary channel
    BLE_ADV_EXT_IND            = 0x07,

    /// ADV data indication on secondary channel
    BLE_AUX_ADV_IND            = 0x07,
    /// Scan response data on secondary channel
    BLE_AUX_SCAN_RSP           = 0x07,
    /// Periodic Advertising packet on secondary channel
    BLE_AUX_SYNC_IND           = 0x07,
    /// Continuation of advertising data on secondary channel
    BLE_AUX_CHAIN_IND          = 0x07,
    /// Response of the AUX_CONNECT_REQ on secondary channel to confirm that connection is accepted
    BLE_AUX_CONNECT_RSP        = 0x08,

    /// Reserved
    BLE_RESERVED_PDU_TYPES,
};


/// LE Advertising Report Event Type HCI:7.7.65.2
enum
{
    /// Connectable undirected advertising
    ADV_IND_EVT = 0x00,
    /// Connectable directed advertising
    ADV_DIRECT_IND_EVT,
    /// Scannable undirected advertising
    ADV_SCAN_IND_EVT,
    /// Non connectable undirected advertising
    ADV_NONCONN_IND_EVT,
    /// Scan Response
    SCAN_RSP_EVT,
    /// Reserved
    RESERVED_ADV_EVT_TYPES,
};

/// LE Extended Advertising Report Event Type Bit Mask HCI:7.765.13

/// Connectable advertising event
#define CON_ADV_EVT_MSK            0x01
/// Scannable advertising event
#define SCAN_ADV_EVT_MSK           0x02
/// Directed advertising event
#define DIR_ADV_EVT_MSK            0x04
/// Scan Response
#define SCAN_RSP_EVT_MSK           0x08
/// Legacy advertising PDUs
#define LGCY_ADV_EVT_MSK           0x10

/// Connectable undirected advertising
#define LGCY_ADV_IND_EVT                    0x13 //LGCY_ADV_EVT_MSK|SCAN_ADV_EVT_MSK|CON_ADV_EVT_MSK
/// Connectable directed advertising
#define LGCY_ADV_DIRECT_IND_EVT             0x15 //LGCY_ADV_EVT_MSK|DIR_ADV_EVT_MSK|CON_ADV_EVT_MSK
/// Scannable undirected advertising
#define LGCY_ADV_SCAN_IND_EVT               0x12 //LGCY_ADV_EVT_MSK|SCAN_ADV_EVT_MSK
/// Non connectable undirected advertising
#define LGCY_ADV_NONCONN_IND_EVT            0x10 //LGCY_ADV_EVT_MSK
/// Scan Response to ADV_IND
#define LGCY_SCAN_RSP_TO_ADV_IND_EVT        0x1B //LGCY_ADV_EVT_MSK|SCAN_RSP_EVT_MSK|SCAN_ADV_EVT_MSK|CON_ADV_EVT_MSK
/// Scan Response to ADV_SCAN_IND
#define LGCY_SCAN_RSP_TO_ADV_SCAN_IND_EVT   0x1A //LGCY_ADV_EVT_MSK|SCAN_RSP_EVT_MSK|SCAN_ADV_EVT_MSK

/// Offset of data status field in event type value
#define ADV_EVT_DATA_STATUS_OFFSET              5
/// Mask for data status field in event type value
#define ADV_EVT_DATA_STATUS_MASK                0x0003
/// Data status of extended advertising event - Complete
#define ADV_EVT_DATA_STATUS_COMPLETE            0
/// Data status of extended advertising event - Incomplete, more data to come
#define ADV_EVT_DATA_STATUS_INCOMPLETE          1
/// Data status of extended advertising event - Incomplete, data truncated, no more to come
#define ADV_EVT_DATA_STATUS_TRUNCATED           2
/// Data status of extended advertising event - Reserved for future use
#define ADV_EVT_DATA_STATUS_RESERVED            3

/// LLID packet
enum
{
    /// Reserved for future use
    LLID_RFU,
    /// Continue
    LLID_CONTINUE,
    /// Start
    LLID_START,
    /// Control
    LLID_CNTL
};

/// Remote OOB Data present parameter value HCI:7.1.29
enum
{
    REM_OOB_DATA_NO        = 0x00,
    REM_OOB_DATA_P192      = 0x01,
    REM_OOB_DATA_P256      = 0x02,
    REM_OOB_DATA_P192_P256 = 0x03,
};

/// Encryption enabled parameter in HCI_Enc_Chg_Evt HCI:7.7.8
enum
{
    ENC_OFF                  = 0x00,
    ENC_BRDER_E0_LE_AESCCM   = 0x01,
    ENC_BREDR_AESCC          = 0x02,
};

/// Combined duration of Preamble and Access Address depending on the PHY used (in us)
#define BLE_PREAMBLE_ACCESS_ADDR_DUR_1MBPS      (5*8)
#define BLE_PREAMBLE_ACCESS_ADDR_DUR_2MBPS      (6*4)
#define BLE_PREAMBLE_ACCESS_ADDR_DUR_125KBPS    (80+256)
#define BLE_PREAMBLE_ACCESS_ADDR_DUR_500KBPS    (80+256)

/// size of the LEN & MODE info preceeding the extended header
#define BLE_EXT_ADV_PRE_HEADER_LEN           (1)
/// size of the FLAGS info at start of the extended header
#define BLE_EXT_ADV_HEADER_FLAGS_LEN         (1)
/// size of the extended header in bytes (pre-header + flags)
#define BLE_EXT_ADV_HEADER_LEN               (2)
/// Size of supplemental info in extended header
#define BLE_EXT_SUP_INFO_LEN                 (1)
/// Size of ADV Data Info in extended header
#define BLE_EXT_ADI_LEN                      (2)
/// Size of Aux Pointer info in extended header
#define BLE_EXT_AUX_PTR_LEN                  (3)
/// Size of Sync PTR info in extended header
#define BLE_EXT_SYNC_LEN                    (18)
/// Size of TX Power info in extended header
#define BLE_EXT_TX_PWR_LEN                   (1)

/// Extended Header Flags
enum ble_ext_header_flags
{
    // AdvA
    ADVA_BIT                = 0x01,
    ADVA_POS                = 0,
    // TargetA
    TARGETA_BIT             = 0x02,
    TARGETA_POS             = 1,
    // SuppInfo
    SUPPINFO_BIT            = 0x04,
    SUPPINFO_POS            = 2,
    // AdvDataInfo (ADI)
    ADI_BIT                 = 0x08,
    ADI_POS                 = 3,
    // AuxPtr
    AUXPTR_BIT              = 0x10,
    AUXPTR_POS              = 4,
    // SyncInfo
    SYNCINFO_BIT            = 0x20,
    SYNCINFO_POS            = 5,
    // TxPower
    TXPOWER_BIT             = 0x40,
    TXPOWER_POS             = 6,
};

/// AUX pointer description
enum ble_aux_ptr
{
    // Aux LL Channel
    BLE_AUX_LL_CHANNEL_MASK = 0x0000003F,
    BLE_AUX_LL_CHANNEL_LSB  = 0,
    // Aux Clock Accuracy
    BLE_AUX_CA_BIT          = 0x00000040,
    BLE_AUX_CA_POS          = 6,
    // Aux Offset Unit
    BLE_AUX_OFFSET_UNIT_BIT = 0x00000080,
    BLE_AUX_OFFSET_UNIT_POS = 7,
    // Aux ADV offset
    BLE_AUX_OFFSET_MASK     = 0x001FFF00,
    BLE_AUX_OFFSET_LSB      = 8,
    // Aux PHY
    BLE_AUX_PHY_MASK        = 0x00E00000,
    BLE_AUX_PHY_LSB         = 21,
};

/// AdvDataInfo (ADI) field description
enum ble_adi
{
    // Advertising Data ID (DID)
    BLE_ADI_DID_MASK        = 0x0FFF,
    BLE_ADI_DID_LSB         = 0,
    // Advertising Set ID (SID)
    BLE_ADI_SID_MASK        = 0xF000,
    BLE_ADI_SID_LSB         = 12,
};

/// Advertising mode
enum ble_adv_mode
{
    /// Non connectable and non scannable mode
    BLE_MODE_NON_CON_SCAN   = 0,
    /// Connectable Mode (accept connection request)
    BLE_MODE_CONNECTABLE    = 1,
    /// Scannable Mode (accept scan request)
    BLE_MODE_SCANNABLE      = 2,
    /// Reserved Advertising mode
    BLE_MODE_RESERVED       = 3,
};

/// Advertising data operation
enum adv_data_op
{
    /// Intermediate fragment of fragmented extended advertising data
    ADV_DATA_OP_INTERMEDIATE_FRAG   = 0,
    /// First fragment of fragmented extended advertising data
    ADV_DATA_OP_FIRST_FRAG          = 1,
    /// Last fragment of fragmented extended advertising data
    ADV_DATA_OP_LAST_FRAG           = 2,
    /// Complete extended advertising data
    ADV_DATA_OP_COMPLETE            = 3,
    /// Unchanged data (just update the Advertising DID)
    ADV_DATA_OP_UNCHANGED_DATA      = 4,
};

/// Advertising data fragment preference
enum adv_data_frag_pref
{
    /// The Controller may fragment all Host advertising data
    ADV_DATA_MAY_FRAG         = 0,
    /// The Controller should not fragment nor minimize fragmentation of Host advertising data
    ADV_DATA_SHOULD_NOT_FRAG  = 1,
};

/*
 * STRUCTURE DEFINITONS
 ****************************************************************************************
 */

///BD name structure
struct bd_name
{
    ///length for name
    uint8_t namelen;
    ///array of bytes for name
    uint8_t name[BD_NAME_SIZE];
};

///Structure device name
struct device_name
{
    ///array of bytes for name
    uint8_t name[BD_NAME_SIZE];
};

///Structure name vector
struct name_vect
{
    uint8_t vect[NAME_VECT_SIZE];
};

/// lap structure
struct lap
{
    /// LAP
    uint8_t A[BD_ADDR_LAP_LEN];
};

/// class structure
struct devclass
{
    /// class
    uint8_t A[DEV_CLASS_LEN];
};

///Extended inquiry response structure
struct eir
{
    /// eir data
    uint8_t data[EIR_DATA_SIZE];
};

///Event mask structure
struct evt_mask
{
    ///8-byte array for mask value
    uint8_t mask[EVT_MASK_LEN];
};

///Host number of completed packets structure, for 1 connection handle
struct host_cmpl_pkts
{
    ///Connection handle
    uint16_t  con_hdl;
    ///Number of completed packets
    uint16_t  nb_cmpl_pkts;
};

///BD Address structure
struct bd_addr
{
    ///6-byte array address value
    uint8_t  addr[BD_ADDR_LEN];
};

///Access Address structure
struct access_addr
{
    ///4-byte array access address
    uint8_t  addr[ACCESS_ADDR_LEN];
};

///Advertising data structure
struct adv_data
{
    ///Maximum length data bytes array
    uint8_t        data[ADV_DATA_LEN];
};

///Scan response data structure
struct scan_rsp_data
{
    ///Maximum length data bytes array
    uint8_t        data[SCAN_RSP_DATA_LEN];
};

///Channel map structure
struct chnl_map
{
    ///10-bytes channel map array
    uint8_t map[CHNL_MAP_LEN];
};

///Channel map structure
struct le_chnl_map
{
    ///5-byte channel map array
    uint8_t map[LE_CHNL_MAP_LEN];
};

///Long Term Key structure
struct ltk
{
    ///16-byte array for LTK value
    uint8_t ltk[KEY_LEN];
};

///Identity Resolving Key structure
struct irk
{
    ///16-byte array for IRK value
    uint8_t key[KEY_LEN];
};

/// Initialization vector (for AES-CCM encryption)
struct initialization_vector
{
    ///8-byte array
    uint8_t vect[IV_LEN];
};

/// Bluetooth address with link key
struct bd_addr_plus_key
{
    /// BD Address
    struct bd_addr  bd_addr;
    /// Link Key
    struct ltk link_key;
};

///Random number structure
struct rand_nb
{
    ///8-byte array for random number
    uint8_t     nb[RAND_NB_LEN];
};

///Advertising report structure
struct adv_report
{
    ///Event type:
    /// - ADV_CONN_UNDIR: Connectable Undirected advertising
    /// - ADV_CONN_DIR: Connectable directed advertising
    /// - ADV_DISC_UNDIR: Discoverable undirected advertising
    /// - ADV_NONCONN_UNDIR: Non-connectable undirected advertising
    uint8_t        evt_type;
    ///Advertising address type: public/random
    uint8_t        adv_addr_type;
    ///Advertising address value
    struct bd_addr adv_addr;
    ///Data length in advertising packet
    uint8_t        data_len;
    ///Data of advertising packet
    uint8_t        data[ADV_DATA_LEN];
    ///RSSI value for advertising packet
    uint8_t        rssi;
};

///Direct Advertising report structure
struct dir_adv_report
{
    ///Event type:
    /// - ADV_CONN_DIR: Connectable directed advertising
    uint8_t        evt_type;
    ///Address type: public/random
    uint8_t        addr_type;
    ///Address value
    struct bd_addr addr;
    ///Direct address type: public/random
    uint8_t        dir_addr_type;
    ///Direct address value
    struct bd_addr dir_addr;
    ///RSSI value for advertising packet
    uint8_t        rssi;
};

///Exteneded Advertising report structure
struct ext_adv_report
{
    ///Event type
    uint16_t       evt_type;
    ///Advertising address type: public/random
    uint8_t        adv_addr_type;
    ///Advertising address value
    struct bd_addr adv_addr;
    ///Primary PHY
    uint8_t        phy;
    ///Secondary PHY
    uint8_t        phy2;
    ///Advertising SID
    uint8_t        adv_sid;
    ///Tx Power
    uint8_t        tx_power;
    ///RSSI value for advertising packet
    uint8_t        rssi;
    ///Periodic Advertising interval (Time=N*1.25ms)
    uint16_t       interval;
    ///Direct address type
    uint8_t        dir_addr_type;
    ///Direct address value
    struct bd_addr dir_addr;
    ///Data length in advertising packet
    uint8_t        data_len;
    ///Data of advertising packet
    uint8_t        data[EXT_ADV_DATA_MAX_LEN];
};

///Supported LE Features structure
struct le_features
{
    ///8-byte array for LE features
    uint8_t feats[LE_FEATS_LEN];
};

///Simple pairing hash structure
struct hash
{
    ///16-byte array for LTK value
    uint8_t C[KEY_LEN];
};

///Simple pairing randomizer structure
struct randomizer
{
    ///16-byte array for LTK value
    uint8_t R[KEY_LEN];
};

///Pin code structure
struct pin_code
{
    ///16-byte array for PIN value
    uint8_t pin[PIN_CODE_MAX_LEN];
};

///Sres structure
struct sres_nb
{
    ///8-byte array for random number
    uint8_t  nb[SRES_LEN];
};

///aco structure
struct aco
{
    ///8-byte array for random number
    uint8_t a[ACO_LEN];
};

///struct byte 16 to stay align with the sdl version
struct byte16
{
    uint8_t A[16];
};

///Controller number of completed packets structure
struct nb_cmpl_pk
{
    ///Connection handle
    uint16_t con_hdl;
    ///Controller number of data packets that have been completed since last time
    uint16_t nb_hc_cmpl_pkts;
};

///Supported Features  structure
struct features
{
    ///8-byte array for features
    uint8_t feats[FEATS_LEN];
};

///Supported commands structure
struct supp_cmds
{
    ///64-byte array for supported commands
    uint8_t cmds[SUPP_CMDS_LEN];
};

///Supported LMP features structure
struct lmp_features
{
    ///8-byte array for LMp features
    uint8_t feats[FEATS_LEN];
};

///Simple pairing IO capabilities
struct io_capability
{
    ///IO capability
    uint8_t  io_cap;
    /// Out Of Band Data present
    bool  oob_data_present;
    ///Authentication Requirement
    uint8_t  aut_req;
};

///Public key
struct pub_key_192
{
    uint8_t p_key[PUB_KEY_192_LEN/2];
};

///Public key
struct pub_key_256
{
    uint8_t p_key[PUB_KEY_256_LEN/2];
};

///Simple pairing public keys 192
struct sp_pub_key_192
{
    ///Public key X
    struct pub_key_192 X;
    ///Public key Y
    struct pub_key_192 Y;
};

///Simple pairing public keys 256
struct sp_pub_key_256
{
    ///Public key X
    struct pub_key_256 X;
    ///Public key Y
    struct pub_key_256 Y;
};

///Supported LE states structure
struct le_states
{
    ///8-byte array for LE states
    uint8_t  supp_states[LE_STATES_LEN];
};

///White List element structure
struct white_list
{
    ///BD address of device entry
    struct bd_addr wl_bdaddr;
    ///BD address type of device entry
    uint8_t wl_bdaddr_type;
};

///CRC initial value structure
struct crc_init
{
    ///3-byte array CRC initial value
    uint8_t crc[CRC_INIT_LEN];
};

///Session key diversifier master or slave structure
struct sess_k_div_x
{
    ///8-byte array for diversifier value
    uint8_t skdiv[SESS_KEY_DIV_LEN];
};

///Session key diversifier structure
struct sess_k_div
{
    ///16-byte array for session key diversifier.
    uint8_t skd[2*SESS_KEY_DIV_LEN];
};

///Initiator vector
struct init_vect
{
    ///4-byte array for vector
    uint8_t iv[INIT_VECT_LEN];
};

typedef struct t_public_key
{
    uint8_t x[PUBLIC_KEY_P256_LEN];
    uint8_t y[PUBLIC_KEY_P256_LEN];

} t_public_key;

/// structure connection request
struct pdu_con_req
{
    /// access address
    struct access_addr  aa;

    /// CRC init
    struct crc_init     crcinit;

    /// window size
    uint8_t             winsize;

    /// window offset
    uint16_t            winoffset;

    /// interval
    uint16_t            interval;

    /// latency
    uint16_t            latency;

    /// timeout
    uint16_t            timeout;

    /// channel mapping
    struct le_chnl_map  chm;

    /// hopping
    uint8_t             hop_sca;
};

/// Device specific link preferences
typedef struct
{
    // **** Data Length Management ****
    /// Suggested value for the Controller's maximum transmitted number of payload octets
    uint16_t   suggested_max_tx_octets;
    /// Suggested value for the Controller's maximum packet transmission time (in us)
    uint16_t   suggested_max_tx_time;

    // ****    PHY  Management     ****
    /// Default TX preferred PHY to use (@see enum le_phy_value)
    uint8_t    tx_phys;
    /// Default RX preferred PHY to use (@see enum le_phy_value)
    uint8_t    rx_phys;
} link_pref_t;


/// @} CO_BT_DEFINES
#endif // CO_BT_DEFINES_H_
